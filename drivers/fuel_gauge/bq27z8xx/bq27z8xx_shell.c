/*

 * Copyright (c) 2026, Texas Instruments Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Shell commands for the BQ27Z8XX fuel gauge family.
 *
 * Provides the following sub-commands under "bq27z8xx":
 *   status       — full device status dump (safety, operation, gauging)
 *   security     — report current security mode
 *   lifetime     — lifetime data block dump
 *   serial       — device serial number
 *   ra_table     — resistance adjustment table dump
 *   reg_read     — raw SBS register read (1–32 bytes), optionally at arbitrary I2C address
 *   reg_write    — raw SBS register write (1–32 bytes), optionally at arbitrary I2C address
 *   mac_write    — MAC write command with optional data payload
 *   read         — raw MAC / data-flash hex dump
 *   seal         — send the SEAL command
 *   unseal       — send unseal key pair
 *   full_access  — send full-access key pair
 */

/* DT_DRV_COMPAT is set per-block below for the device list */

#include <zephyr/shell/shell.h>
#include <zephyr/device.h>
#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include "bq27z8xx_common.h"
#include "bq27z758.h"
#include "bq27z855.h"

#define MAC_DATA_LEN 32

/* MAC commands used only within the shell (not in common or variant enums). */
#define BQ27Z8XX_SHELL_MAC_CMD_SERIAL_NUMBER    0x004Eu
#define BQ27Z8XX_SHELL_MAC_CMD_MANUFACTURERINFO 0x0070u

/* Default security keys used when the caller omits explicit values. */
#define DEFAULT_UNSEAL_KEY1      0x0414u
#define DEFAULT_UNSEAL_KEY2      0x3672u
#define DEFAULT_FULL_ACCESS_KEY1 0xFFFFu
#define DEFAULT_FULL_ACCESS_KEY2 0xFFFFu

/* ---------------------------------------------------------------------------
 * Device list — populated at compile time from DT nodes with ti,bq27z758
 * and/or ti,bq27z855.
 * ---------------------------------------------------------------------------
 */
static const struct device *const bq27z8xx_devices[] = {
#if DT_HAS_COMPAT_STATUS_OKAY(ti_bq27z758)
#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT                   ti_bq27z758
#define BQ27Z8XX_DEVICE_ENTRY_758(inst) DEVICE_DT_INST_GET(inst),
	DT_INST_FOREACH_STATUS_OKAY(BQ27Z8XX_DEVICE_ENTRY_758)
#undef BQ27Z8XX_DEVICE_ENTRY_758
#endif
#if DT_HAS_COMPAT_STATUS_OKAY(ti_bq27z855)
#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT                   ti_bq27z855
#define BQ27Z8XX_DEVICE_ENTRY_855(inst) DEVICE_DT_INST_GET(inst),
		DT_INST_FOREACH_STATUS_OKAY(BQ27Z8XX_DEVICE_ENTRY_855)
#undef BQ27Z8XX_DEVICE_ENTRY_855
#endif
};

static void cmd_pos_bq27z8xx_device_name(size_t idx, struct shell_static_entry *entry)
{
	if (idx < ARRAY_SIZE(bq27z8xx_devices)) {
		entry->syntax = bq27z8xx_devices[idx]->name;
	} else {
		entry->syntax = NULL;
	}
	entry->handler = NULL;
	entry->help = "Device";
	entry->subcmd = NULL;
}

SHELL_DYNAMIC_CMD_CREATE(dsub_pos_bq27z8xx_device_name, cmd_pos_bq27z8xx_device_name);

/* ---------------------------------------------------------------------------
 * Transport helpers
 * ---------------------------------------------------------------------------
 */

/*
 * Detect device variant by comparing the variant_ops pointer in config.
 * No I2C traffic required.
 */
static bool bq27z8xx_shell_is_z855(const struct device *dev)
{
#if IS_ENABLED(CONFIG_BQ27Z855) && IS_ENABLED(CONFIG_BQ27Z758)
	/* Both variants compiled: determine from the variant_ops pointer. */
	const struct bq27z8xx_config *cfg = dev->config;

	return cfg->variant_ops == &bq27z855_variant_ops;
#elif IS_ENABLED(CONFIG_BQ27Z855)
	ARG_UNUSED(dev);
	return true;
#else
	ARG_UNUSED(dev);
	return false;
#endif
}

/*
 * Wrapper around bq27z8xx_read_mac() that:
 *   - prints an error on failure,
 *   - returns the payload in out[0..MAC_DATA_LEN-1], and
 *   - stores the device-reported payload length in *reported_len.
 *
 * bq27z8xx_read_mac() fills buf[0] with the payload length and
 * buf[1..MAC_DATA_LEN] with the payload bytes; this helper strips
 * that framing for callers that want just the raw data.
 */
static int shell_read_mac(const struct shell *sh, const struct device *dev, uint16_t cmd,
			  const char *label, uint8_t out[MAC_DATA_LEN], uint8_t *reported_len)
{
	uint8_t buf[1 + MAC_DATA_LEN] = {0};
	int ret = bq27z8xx_read_mac(dev, cmd, buf, MAC_DATA_LEN, K_NO_WAIT);

	if (ret != 0) {
		shell_error(sh, "%s read failed: %d", label, ret);
		return ret;
	}

	if (reported_len != NULL) {
		*reported_len = buf[0];
	}

	memcpy(out, &buf[1], MAC_DATA_LEN);
	return 0;
}

static int shell_read_df(const struct shell *sh, const struct device *dev, uint16_t addr,
			 uint8_t out[MAC_DATA_LEN], uint8_t *reported_len)
{
	uint8_t buf[1 + MAC_DATA_LEN] = {0};
	int ret = bq27z8xx_df_read(dev, addr, buf, MAC_DATA_LEN);

	if (ret != 0) {
		shell_error(sh, "DF read failed: %d", ret);
		return ret;
	}

	if (reported_len != NULL) {
		*reported_len = buf[0];
	}

	memcpy(out, &buf[1], MAC_DATA_LEN);
	return 0;
}

/* ---------------------------------------------------------------------------
 * Formatting utilities
 * ---------------------------------------------------------------------------
 */

static inline char printable_byte(uint8_t v)
{
	return (v >= 0x20 && v <= 0x7e) ? (char)v : '.';
}

static void print_raw_bytes(const struct shell *sh, const char *label, const uint8_t *data,
			    uint8_t len)
{
	char hex[(MAC_DATA_LEN * 3) + 1];
	size_t pos = 0;
	const uint8_t out_len = MIN(len, (uint8_t)MAC_DATA_LEN);

	for (uint8_t i = 0; i < out_len; i++) {
		pos += snprintk(&hex[pos], sizeof(hex) - pos, "%02x ", data[i]);
	}
	if (pos > 0) {
		hex[pos - 1] = '\0';
	} else {
		hex[0] = '\0';
	}

	shell_print(sh, "%s (%u): %s", label, out_len, hex);
}

/*
 * Copy up to min(src_len, dst_len-1) printable ASCII bytes from src into dst.
 * Non-printable bytes are replaced with '.'; the copy stops at NUL.
 * If no printable bytes are found, dst is set to "".
 * Returns the number of bytes written (excluding NUL terminator).
 */
static size_t copy_ascii(char *dst, size_t dst_len, const uint8_t *src, size_t src_len,
			 bool *replaced, bool *has_printable)
{
	size_t out_len = 0;
	bool rep = false;
	bool printable = false;

	if (dst_len == 0) {
		return 0;
	}

	const size_t max_len = MIN(src_len, dst_len - 1);

	for (size_t i = 0; i < max_len; i++) {
		const uint8_t v = src[i];

		if (v == '\0') {
			break;
		}
		if (v >= 0x20 && v <= 0x7e) {
			dst[out_len++] = (char)v;
			printable = true;
		} else {
			dst[out_len++] = '.';
			rep = true;
		}
	}

	if (!printable) {
		out_len = 0;
	}
	dst[out_len] = '\0';

	if (replaced != NULL) {
		*replaced = rep;
	}
	if (has_printable != NULL) {
		*has_printable = printable;
	}

	return out_len;
}

/* ---------------------------------------------------------------------------
 * Status printing helpers — per-register decoders
 * ---------------------------------------------------------------------------
 */

/*
 * Print BatteryStatus register bits (same layout on both devices).
 */
static void print_battery_status(const struct shell *sh, uint16_t v)
{
	shell_print(sh, "BatteryStatus: 0x%04" PRIx16, v);
#define PRINT_BIT(mask, active, inactive)                                                          \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (v & (mask)) ? (active) : (inactive))

	PRINT_BIT(BQ27Z8XX_BATTERY_STATUS_TCA, "Terminate Charge Alarm Active",
		  "Terminate Charge Alarm Inactive");
	PRINT_BIT(BQ27Z8XX_BATTERY_STATUS_TDA, "Terminate Discharge Alarm Active",
		  "Terminate Discharge Alarm Inactive");
	PRINT_BIT(BQ27Z8XX_BATTERY_STATUS_RCA, "Remaining Capacity Alarm Active",
		  "Remaining Capacity Alarm Inactive");
	PRINT_BIT(BQ27Z8XX_BATTERY_STATUS_INIT, "Initialization Active", "Initialization Complete");
	PRINT_BIT(BQ27Z8XX_BATTERY_STATUS_DSG, "Discharging", "Charging");
	PRINT_BIT(BQ27Z8XX_BATTERY_STATUS_FC, "Fully Charged", "Not fully charged");
	PRINT_BIT(BQ27Z8XX_BATTERY_STATUS_FD, "Fully Discharged", "Battery OK");
#undef PRINT_BIT
}

/*
 * Print InterruptStatus register bits (same layout on both devices).
 */
static void print_interrupt_status(const struct shell *sh, uint8_t v)
{
	shell_print(sh, "InterruptStatus: 0x%02x", v);
#define PRINT_BIT(mask, active, inactive)                                                          \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (v & (mask)) ? (active) : (inactive))

	PRINT_BIT(BQ27Z8XX_INTERRUPT_STATUS_SOC_DELTA, "SOC delta interrupt Set",
		  "SOC delta interrupt Clear");
	PRINT_BIT(BQ27Z8XX_INTERRUPT_STATUS_TEMP_LO, "Low temperature interrupt Set",
		  "Low temperature interrupt Clear");
	PRINT_BIT(BQ27Z8XX_INTERRUPT_STATUS_TEMP_HI, "High temperature interrupt Set",
		  "High temperature interrupt Clear");
	PRINT_BIT(BQ27Z8XX_INTERRUPT_STATUS_VOLT_LO, "Low voltage interrupt Set",
		  "Low voltage interrupt Clear");
	PRINT_BIT(BQ27Z8XX_INTERRUPT_STATUS_VOLT_HI, "High voltage interrupt Set",
		  "High voltage interrupt Clear");
#undef PRINT_BIT
}

/*
 * BQ27Z758: decode 32-bit SafetyAlert payload.
 */
static void print_safety_alert_z758(const struct shell *sh, uint32_t v)
{
	shell_print(sh, "SafetyAlert: 0x%08" PRIx32, v);
#define PB(mask, a, i)                                                                             \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (v & (mask)) ? (a) : (i))
	PB(BQ27Z758_SAFETY_ALERT_UTD, "Undertemperature in Discharge Active",
	   "Undertemperature in Discharge Inactive");
	PB(BQ27Z758_SAFETY_ALERT_UTC, "Undertemperature in Charge Active",
	   "Undertemperature in Charge Inactive");
	PB(BQ27Z758_SAFETY_ALERT_CTOS, "Charge timeout suspend Active",
	   "Charge timeout suspend Inactive");
	PB(BQ27Z758_SAFETY_ALERT_PTOS, "Precharge timeout suspend Active",
	   "Precharge timeout suspend Inactive");
	PB(BQ27Z758_SAFETY_ALERT_OTF, "Overtemperature fault Active",
	   "Overtemperature fault Inactive");
	PB(BQ27Z758_SAFETY_ALERT_OTD, "Overtemperature in Discharge Active",
	   "Overtemperature in Discharge Inactive");
	PB(BQ27Z758_SAFETY_ALERT_OTC, "Overtemperature in Charge Active",
	   "Overtemperature in Charge Inactive");
	PB(BQ27Z758_SAFETY_ALERT_OCD, "Overcurrent in Discharge Active",
	   "Overcurrent in Discharge Inactive");
	PB(BQ27Z758_SAFETY_ALERT_OCC, "Overcurrent in Charge Active",
	   "Overcurrent in Charge Inactive");
	PB(BQ27Z758_SAFETY_ALERT_COV, "Cell overvoltage Active", "Cell overvoltage Inactive");
	PB(BQ27Z758_SAFETY_ALERT_CUV, "Cell undervoltage Active", "Cell undervoltage Inactive");
#undef PB
}

/*
 * BQ27Z758: decode 32-bit SafetyStatus payload.
 */
static void print_safety_status_z758(const struct shell *sh, uint32_t v)
{
	shell_print(sh, "SafetyStatus: 0x%08" PRIx32, v);
#define PB(mask, a, i)                                                                             \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (v & (mask)) ? (a) : (i))
	PB(BQ27Z758_SAFETY_STATUS_HCUV, "High cell undervoltage Active",
	   "High cell undervoltage Inactive");
	PB(BQ27Z758_SAFETY_STATUS_HCOV, "High cell overvoltage Active",
	   "High cell overvoltage Inactive");
	PB(BQ27Z758_SAFETY_STATUS_UTD, "Undertemperature in Discharge Active",
	   "Undertemperature in Discharge Inactive");
	PB(BQ27Z758_SAFETY_STATUS_UTC, "Undertemperature in Charge Active",
	   "Undertemperature in Charge Inactive");
	PB(BQ27Z758_SAFETY_STATUS_CTO, "Charge timeout Active", "Charge timeout Inactive");
	PB(BQ27Z758_SAFETY_STATUS_PTO, "Precharge timeout Active", "Precharge timeout Inactive");
	PB(BQ27Z758_SAFETY_STATUS_OTF, "Overtemperature fault Active",
	   "Overtemperature fault Inactive");
	PB(BQ27Z758_SAFETY_STATUS_OTD, "Overtemperature in Discharge Active",
	   "Overtemperature in Discharge Inactive");
	PB(BQ27Z758_SAFETY_STATUS_OTC, "Overtemperature in Charge Active",
	   "Overtemperature in Charge Inactive");
	PB(BQ27Z758_SAFETY_STATUS_HSCD, "High-speed short-circuit in Discharge Active",
	   "High-speed short-circuit in Discharge Inactive");
	PB(BQ27Z758_SAFETY_STATUS_HOCC, "High overcurrent in Charge Active",
	   "High overcurrent in Charge Inactive");
	PB(BQ27Z758_SAFETY_STATUS_HOCD, "High overcurrent in Discharge Active",
	   "High overcurrent in Discharge Inactive");
	PB(BQ27Z758_SAFETY_STATUS_OCD, "Overcurrent in Discharge Active",
	   "Overcurrent in Discharge Inactive");
	PB(BQ27Z758_SAFETY_STATUS_OCC, "Overcurrent in Charge Active",
	   "Overcurrent in Charge Inactive");
	PB(BQ27Z758_SAFETY_STATUS_COV, "Cell overvoltage Active", "Cell overvoltage Inactive");
	PB(BQ27Z758_SAFETY_STATUS_CUV, "Cell undervoltage Active", "Cell undervoltage Inactive");
#undef PB
}

/*
 * BQ27Z855: decode 32-bit SafetyAlert payload.
 */
static void print_safety_alert_z855(const struct shell *sh, uint32_t v)
{
	shell_print(sh, "SafetyAlert: 0x%08" PRIx32, v);
#define PB(mask, a, i)                                                                             \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (v & (mask)) ? (a) : (i))
	PB(BQ27Z855_SAFETY_ALERT_ACOV, "AC cell overvoltage Active",
	   "AC cell overvoltage Inactive");
	PB(BQ27Z855_SAFETY_ALERT_ACUV, "AC cell undervoltage Active",
	   "AC cell undervoltage Inactive");
	PB(BQ27Z855_SAFETY_ALERT_UTD, "Undertemperature in Discharge Active",
	   "Undertemperature in Discharge Inactive");
	PB(BQ27Z855_SAFETY_ALERT_UTC, "Undertemperature in Charge Active",
	   "Undertemperature in Charge Inactive");
	PB(BQ27Z855_SAFETY_ALERT_CHGV, "Overcharging voltage Active",
	   "Overcharging voltage Inactive");
	PB(BQ27Z855_SAFETY_ALERT_CHGC, "Overcharging current Active",
	   "Overcharging current Inactive");
	PB(BQ27Z855_SAFETY_ALERT_OC, "Overcharge Active", "Overcharge Inactive");
	PB(BQ27Z855_SAFETY_ALERT_CTOS, "Charge timeout suspend Active",
	   "Charge timeout suspend Inactive");
	PB(BQ27Z855_SAFETY_ALERT_CTO, "Charge timeout Active", "Charge timeout Inactive");
	PB(BQ27Z855_SAFETY_ALERT_PTOS, "Precharge timeout suspend Active",
	   "Precharge timeout suspend Inactive");
	PB(BQ27Z855_SAFETY_ALERT_PTO, "Precharge timeout Active", "Precharge timeout Inactive");
	PB(BQ27Z855_SAFETY_ALERT_OTF, "Overtemperature FET Active", "Overtemperature FET Inactive");
	PB(BQ27Z855_SAFETY_ALERT_OTD, "Overtemperature in Discharge Active",
	   "Overtemperature in Discharge Inactive");
	PB(BQ27Z855_SAFETY_ALERT_OTC, "Overtemperature in Charge Active",
	   "Overtemperature in Charge Inactive");
	PB(BQ27Z855_SAFETY_ALERT_OCD2, "Overcurrent in Discharge 2 Active",
	   "Overcurrent in Discharge 2 Inactive");
	PB(BQ27Z855_SAFETY_ALERT_OCD1, "Overcurrent in Discharge 1 Active",
	   "Overcurrent in Discharge 1 Inactive");
	PB(BQ27Z855_SAFETY_ALERT_OCC2, "Overcurrent in Charge 2 Active",
	   "Overcurrent in Charge 2 Inactive");
	PB(BQ27Z855_SAFETY_ALERT_OCC1, "Overcurrent in Charge 1 Active",
	   "Overcurrent in Charge 1 Inactive");
	PB(BQ27Z855_SAFETY_ALERT_COV, "Cell overvoltage Active", "Cell overvoltage Inactive");
	PB(BQ27Z855_SAFETY_ALERT_CUV, "Cell undervoltage Active", "Cell undervoltage Inactive");
#undef PB
}

/*
 * BQ27Z855: decode 32-bit SafetyStatus payload.
 */
static void print_safety_status_z855(const struct shell *sh, uint32_t v)
{
	shell_print(sh, "SafetyStatus: 0x%08" PRIx32, v);
#define PB(mask, a, i)                                                                             \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (v & (mask)) ? (a) : (i))
	PB(BQ27Z855_SAFETY_STATUS_ACOV, "AC cell overvoltage Active",
	   "AC cell overvoltage Inactive");
	PB(BQ27Z855_SAFETY_STATUS_ACUV, "AC cell undervoltage Active",
	   "AC cell undervoltage Inactive");
	PB(BQ27Z855_SAFETY_STATUS_UTD, "Undertemperature in Discharge Active",
	   "Undertemperature in Discharge Inactive");
	PB(BQ27Z855_SAFETY_STATUS_UTC, "Undertemperature in Charge Active",
	   "Undertemperature in Charge Inactive");
	PB(BQ27Z855_SAFETY_STATUS_CHGV, "Overcharging voltage Active",
	   "Overcharging voltage Inactive");
	PB(BQ27Z855_SAFETY_STATUS_CHGC, "Overcharging current Active",
	   "Overcharging current Inactive");
	PB(BQ27Z855_SAFETY_STATUS_OC, "Overcharge Active", "Overcharge Inactive");
	PB(BQ27Z855_SAFETY_STATUS_CTO, "Charge timeout Active", "Charge timeout Inactive");
	PB(BQ27Z855_SAFETY_STATUS_PTO, "Precharge timeout Active", "Precharge timeout Inactive");
	PB(BQ27Z855_SAFETY_STATUS_HWDF, "SBS Host watchdog timeout Active",
	   "SBS Host watchdog timeout Inactive");
	PB(BQ27Z855_SAFETY_STATUS_OTF, "Overtemperature FET Active",
	   "Overtemperature FET Inactive");
	PB(BQ27Z855_SAFETY_STATUS_OTD, "Overtemperature in Discharge Active",
	   "Overtemperature in Discharge Inactive");
	PB(BQ27Z855_SAFETY_STATUS_OTC, "Overtemperature in Charge Active",
	   "Overtemperature in Charge Inactive");
	PB(BQ27Z855_SAFETY_STATUS_ASCD, "Short-circuit in Discharge Active",
	   "Short-circuit in Discharge Inactive");
	PB(BQ27Z855_SAFETY_STATUS_AOCC, "Short-circuit in Charge Active",
	   "Short-circuit in Charge Inactive");
	PB(BQ27Z855_SAFETY_STATUS_AOCD, "Overload in Discharge Active",
	   "Overload in Discharge Inactive");
	PB(BQ27Z855_SAFETY_STATUS_OCD2, "Overcurrent in Discharge 2 Active",
	   "Overcurrent in Discharge 2 Inactive");
	PB(BQ27Z855_SAFETY_STATUS_OCD1, "Overcurrent in Discharge 1 Active",
	   "Overcurrent in Discharge 1 Inactive");
	PB(BQ27Z855_SAFETY_STATUS_OCC2, "Overcurrent in Charge 2 Active",
	   "Overcurrent in Charge 2 Inactive");
	PB(BQ27Z855_SAFETY_STATUS_OCC1, "Overcurrent in Charge 1 Active",
	   "Overcurrent in Charge 1 Inactive");
	PB(BQ27Z855_SAFETY_STATUS_COV, "Cell overvoltage Active", "Cell overvoltage Inactive");
	PB(BQ27Z855_SAFETY_STATUS_CUV, "Cell undervoltage Active", "Cell undervoltage Inactive");
#undef PB
}

/*
 * BQ27Z758: decode 32-bit PFAlert payload.
 * The BQ27Z758 payload is two 16-bit word pairs (A+B and C+D).
 */
static void print_pf_alert_z758(const struct shell *sh, const uint8_t *data, uint8_t len)
{
	if (len < 4) {
		print_raw_bytes(sh, "PFAlert", data, len);
		return;
	}
	const uint16_t pf_ab = sys_get_le16(&data[0]);
	const uint16_t pf_cd = sys_get_le16(&data[2]);

	shell_print(sh, "PFAlertA+B: 0x%04" PRIx16, pf_ab);
	shell_print(sh, "  SOV (Bit %u): %s", __builtin_ctz((uint32_t)(BQ27Z758_PF_ALERT_SOV)),
		    (pf_ab & BQ27Z758_PF_ALERT_SOV) ? "Safety cell overvoltage Detected"
						    : "Safety cell overvoltage Not detected");
	shell_print(sh, "  SUV (Bit %u): %s", __builtin_ctz((uint32_t)(BQ27Z758_PF_ALERT_SUV)),
		    (pf_ab & BQ27Z758_PF_ALERT_SUV) ? "Safety cell undervoltage Detected"
						    : "Safety cell undervoltage Not detected");
	shell_print(sh, "PFAlertC+D: 0x%04" PRIx16, pf_cd);
	shell_print(sh, "  DFETF (Bit %u): %s", __builtin_ctz((uint32_t)(BQ27Z758_PF_ALERT_DFETF)),
		    (pf_cd & BQ27Z758_PF_ALERT_DFETF) ? "Discharge FET failure Detected"
						      : "Discharge FET failure Not detected");
	shell_print(sh, "  CFETF (Bit %u): %s", __builtin_ctz((uint32_t)(BQ27Z758_PF_ALERT_CFETF)),
		    (pf_cd & BQ27Z758_PF_ALERT_CFETF) ? "Charge FET failure Detected"
						      : "Charge FET failure Not detected");
}

/*
 * BQ27Z855: decode 32-bit PFAlert / PFStatus payload.
 */
static void print_pf_z855(const struct shell *sh, const char *label, uint32_t v, bool is_status)
{
	shell_print(sh, "%s: 0x%08" PRIx32, label, v);
#define PB(mask, a, i)                                                                             \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (v & (mask)) ? (a) : (i))
	if (is_status) {
		PB(BQ27Z855_PF_STATUS_DFW, "Data flash wearout failure Detected",
		   "Data flash wearout failure Not detected");
		PB(BQ27Z855_PF_STATUS_FORCE, "Manual permanent failure Detected",
		   "Manual permanent failure Not detected");
		PB(BQ27Z855_PF_STATUS_NTC, "NTC failure Detected", "NTC failure Not detected");
	}
	PB(BQ27Z855_PF_ALERT_2LVL, "Second level protector failure Detected",
	   "Second level protector failure Not detected");
	PB(BQ27Z855_PF_ALERT_AFEC, "AFE communication failure Detected",
	   "AFE communication failure Not detected");
	PB(BQ27Z855_PF_ALERT_AFER, "AFE register failure Detected",
	   "AFE register failure Not detected");
	PB(BQ27Z855_PF_ALERT_DFETF, "Discharge FET failure Detected",
	   "Discharge FET failure Not detected");
	PB(BQ27Z855_PF_ALERT_CFETF, "Charge FET failure Detected",
	   "Charge FET failure Not detected");
	PB(BQ27Z855_PF_ALERT_VIMA, "Voltage imbalance while pack active Detected",
	   "Voltage imbalance while pack active Not detected");
	PB(BQ27Z855_PF_ALERT_VIMR, "Voltage imbalance while pack at rest Detected",
	   "Voltage imbalance while pack at rest Not detected");
	PB(BQ27Z855_PF_ALERT_SOT, "Safety overtemperature cell failure Detected",
	   "Safety overtemperature cell failure Not detected");
	PB(BQ27Z855_PF_ALERT_SOCD, "Safety overcurrent in discharge Detected",
	   "Safety overcurrent in discharge Not detected");
	PB(BQ27Z855_PF_ALERT_SOCC, "Safety overcurrent in charge Detected",
	   "Safety overcurrent in charge Not detected");
	PB(BQ27Z855_PF_ALERT_SOV, "Safety cell overvoltage Detected",
	   "Safety cell overvoltage Not detected");
	PB(BQ27Z855_PF_ALERT_SUV, "Safety cell undervoltage Detected",
	   "Safety cell undervoltage Not detected");
#undef PB
}

/*
 * BQ27Z758: decode OperationStatus A and B 16-bit words.
 */
static void print_operation_status_z758(const struct shell *sh, uint16_t op_a, uint16_t op_b)
{
	shell_print(sh, "OperationStatusA: 0x%04" PRIx16, op_a);
#define PB(mask, a, i)                                                                             \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (op_a & (mask)) ? (a) : (i))
	PB(BQ27Z758_OP_STATUS_A_SLEEP, "Sleep mode Active", "Sleep mode Inactive");
	PB(BQ27Z758_OP_STATUS_A_XCHG, "Charging disabled Active", "Charging disabled Inactive");
	PB(BQ27Z758_OP_STATUS_A_XDSG, "Discharging disabled Active",
	   "Discharging disabled Inactive");
	PB(BQ27Z758_OP_STATUS_A_PF, "Permanent Failure Active", "Permanent Failure Inactive");
	PB(BQ27Z758_OP_STATUS_A_SS, "Safety Status OR Active", "Safety Status OR Inactive");
	PB(BQ27Z758_OP_STATUS_A_SDV, "Shutdown (low voltage) Active",
	   "Shutdown (low voltage) Inactive");
	{
		const uint8_t sec = (uint8_t)((op_a & BQ27Z758_OP_STATUS_A_SEC_MASK) >>
					      BQ27Z758_OP_STATUS_A_SEC_SHIFT);
		const char *sec_str = "Reserved";

		switch (sec) {
		case 0x1:
			sec_str = "Full Access";
			break;
		case 0x2:
			sec_str = "Unsealed";
			break;
		case 0x3:
			sec_str = "Sealed";
			break;
		default:
			break;
		}
		shell_print(sh, "  SEC1:SEC0 (Bits 9-8): %s (0x%x)", sec_str, sec);
	}
	PB(BQ27Z758_OP_STATUS_A_BTP_INT, "Battery trip point interrupt Active",
	   "Battery trip point interrupt Inactive");
	PB(BQ27Z758_OP_STATUS_A_SHELF, "Shelf mode Active", "Shelf mode Inactive");
	PB(BQ27Z758_OP_STATUS_A_SHIP, "Ship mode Active", "Ship mode Inactive");
	PB(BQ27Z758_OP_STATUS_A_ZVCHG, "Zero-volt charge Active", "Zero-volt charge Inactive");
	PB(BQ27Z758_OP_STATUS_A_CHG, "Charge FET Active", "Charge FET Inactive");
	PB(BQ27Z758_OP_STATUS_A_DSG, "Discharge FET Active", "Discharge FET Inactive");
	PB(BQ27Z758_OP_STATUS_A_SHIPV, "Ship mode (voltage) Active",
	   "Ship mode (voltage) Inactive");
#undef PB

	shell_print(sh, "OperationStatusB: 0x%04" PRIx16, op_b);
#define PB(mask, a, i)                                                                             \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (op_b & (mask)) ? (a) : (i))
	PB(BQ27Z758_OP_STATUS_B_SHELFV, "Shelf mode (voltage) Active",
	   "Shelf mode (voltage) Inactive");
	PB(BQ27Z758_OP_STATUS_B_SHIPM, "Ship mode (command) Active",
	   "Ship mode (command) Inactive");
	PB(BQ27Z758_OP_STATUS_B_SLPAD, "Sleep allowed Active", "Sleep allowed Inactive");
	PB(BQ27Z758_OP_STATUS_B_INIT, "Initialization Active", "Initialization Inactive");
	PB(BQ27Z758_OP_STATUS_B_SHELFM, "Shelf mode (command) Active",
	   "Shelf mode (command) Inactive");
	PB(BQ27Z758_OP_STATUS_B_XL, "XL mode Active", "XL mode Inactive");
	PB(BQ27Z758_OP_STATUS_B_CAL, "Calibration Active", "Calibration Inactive");
	PB(BQ27Z758_OP_STATUS_B_AUTH, "Authentication Active", "Authentication Inactive");
	PB(BQ27Z758_OP_STATUS_B_SDM, "Shutdown mode Active", "Shutdown mode Inactive");
#undef PB
}

/*
 * BQ27Z855: decode OperationStatus 32-bit word (bytes [0..3]).
 */
static void print_operation_status_z855(const struct shell *sh, uint32_t v)
{
	shell_print(sh, "OperationStatus: 0x%08" PRIx32, v);
#define PB(mask, a, i)                                                                             \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (v & (mask)) ? (a) : (i))
	PB(BQ27Z855_OP_STATUS_IOSHUT, "IO-based shutdown Active", "IO-based shutdown Inactive");
	PB(BQ27Z855_OP_STATUS_PSSHUT, "Power saving shutdown Active",
	   "Power saving shutdown Inactive");
	PB(BQ27Z855_OP_STATUS_DISCONN, "System disconnect Active", "System disconnect Inactive");
	PB(BQ27Z855_OP_STATUS_STORAGEM, "Storage mode Active", "Storage mode Inactive");
	PB(BQ27Z855_OP_STATUS_INIT, "Initialization after full reset Active",
	   "Initialization after full reset Inactive");
	PB(BQ27Z855_OP_STATUS_SLEEPM, "SLEEP mode (command) Active",
	   "SLEEP mode (command) Inactive");
	PB(BQ27Z855_OP_STATUS_XL, "400-kHz SMBus mode Active", "400-kHz SMBus mode Inactive");
	PB(BQ27Z855_OP_STATUS_CAL, "Calibration output Active", "Calibration output Inactive");
	PB(BQ27Z855_OP_STATUS_AUTH, "Authentication in progress Active",
	   "Authentication in progress Inactive");
	PB(BQ27Z855_OP_STATUS_SDM, "Shutdown triggered via command Active",
	   "Shutdown triggered via command Inactive");
	PB(BQ27Z855_OP_STATUS_SLEEP, "SLEEP mode conditions met Active",
	   "SLEEP mode conditions met Inactive");
	PB(BQ27Z855_OP_STATUS_XCHG, "Charging disabled Active", "Charging disabled Inactive");
	PB(BQ27Z855_OP_STATUS_XDSG, "Discharging disabled Active", "Discharging disabled Inactive");
	PB(BQ27Z855_OP_STATUS_PF, "Permanent failure mode Active",
	   "Permanent failure mode Inactive");
	PB(BQ27Z855_OP_STATUS_SS, "Safety status OR Active", "Safety status OR Inactive");
	PB(BQ27Z855_OP_STATUS_SDV, "Shutdown via low battery voltage Active",
	   "Shutdown via low battery voltage Inactive");
	{
		const uint8_t sec = (uint8_t)((v & BQ27Z855_OP_STATUS_SEC_MASK) >>
					      BQ27Z855_OP_STATUS_SEC_SHIFT);
		const char *sec_str = "Reserved";

		switch (sec) {
		case 0x1:
			sec_str = "Full Access";
			break;
		case 0x2:
			sec_str = "Unsealed";
			break;
		case 0x3:
			sec_str = "Sealed";
			break;
		default:
			break;
		}
		shell_print(sh, "  SEC1:SEC0 (Bits 9-8): %s (0x%x)", sec_str, sec);
	}
	PB(BQ27Z855_OP_STATUS_BTP_INT, "Battery trip point interrupt Active",
	   "Battery trip point interrupt Inactive");
	PB(BQ27Z855_OP_STATUS_EMSHUT, "Emergency FET shutdown Active",
	   "Emergency FET shutdown Inactive");
	PB(BQ27Z855_OP_STATUS_MINSYS, "MINSYS operation Active", "MINSYS operation Inactive");
	PB(BQ27Z855_OP_STATUS_CHG, "CHG FET on", "CHG FET off");
	PB(BQ27Z855_OP_STATUS_DSG, "DSG FET on", "DSG FET off");
	PB(BQ27Z855_OP_STATUS_PRES, "System present", "System not present");
#undef PB
}

/*
 * BQ27Z758: decode ChargingStatus 24-bit payload.
 * data[0]=temp_range, data[1..2]=status word (big-endian in the original; we
 * have already stripped the MAC command echo so this is raw device bytes).
 * Per the TRM the status word in the MAC data is packed as [data[2] data[1]]
 * with the temp_range in data[0], so we re-assemble it accordingly.
 */
static void print_charging_status_z758(const struct shell *sh, const uint8_t *data, uint8_t len)
{
	if (len < 3) {
		print_raw_bytes(sh, "ChargingStatus", data, len);
		return;
	}
	const uint8_t temp_range = data[0];
	/* Status is a 16-bit big-endian word in bytes [1..2] per TRM. */
	const uint16_t chg_status = (uint16_t)(((uint16_t)data[1] << 8) | data[2]);

	shell_print(sh, "ChargingStatus: TempRange=0x%02x Status=0x%04" PRIx16, temp_range,
		    chg_status);

#define PBTR(mask, a, i)                                                                           \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (temp_range & (mask)) ? (a) : (i))
	PBTR(BQ27Z758_CHG_TEMP_RANGE_OT, "Temperature > T4 Active", "Temperature > T4 Inactive");
	PBTR(BQ27Z758_CHG_TEMP_RANGE_HT, "T3 < Temp < T4 Active", "T3 < Temp < T4 Inactive");
	PBTR(BQ27Z758_CHG_TEMP_RANGE_STH, "T6 < Temp < T3 Active", "T6 < Temp < T3 Inactive");
	PBTR(BQ27Z758_CHG_TEMP_RANGE_RT, "T5 < Temp < T6 Active", "T5 < Temp < T6 Inactive");
	PBTR(BQ27Z758_CHG_TEMP_RANGE_STL, "T2 < Temp < T5 Active", "T2 < Temp < T5 Inactive");
	PBTR(BQ27Z758_CHG_TEMP_RANGE_LT, "T1 < Temp < T2 Active", "T1 < Temp < T2 Inactive");
	PBTR(BQ27Z758_CHG_TEMP_RANGE_UT, "Temp < T1 Active", "Temp < T1 Inactive");
#undef PBTR

	shell_print(sh, "  NCT (Bit %u): %s", __builtin_ctz((uint32_t)(BQ27Z758_CHG_STATUS_NCT)),
		    (chg_status & BQ27Z758_CHG_STATUS_NCT) ? "No charge termination Active"
							   : "No charge termination Inactive");
	{
		const uint8_t cv_dgrd = (uint8_t)((chg_status & BQ27Z758_CHG_STATUS_CV_DGRD_MASK) >>
						  BQ27Z758_CHG_STATUS_CV_DGRD_SHIFT);
		const char *dgrd_str = "No degradation";

		switch (cv_dgrd) {
		case 0x1:
			dgrd_str = "Mode 1";
			break;
		case 0x2:
			dgrd_str = "Mode 2";
			break;
		case 0x3:
			dgrd_str = "Mode 3";
			break;
		default:
			break;
		}
		shell_print(sh, "  CV_DGRD (Bits 9-8): %s (0x%x)", dgrd_str, cv_dgrd);
	}
#define PBCS(mask, a, i)                                                                           \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (chg_status & (mask)) ? (a) : (i))
	PBCS(BQ27Z758_CHG_STATUS_VCT, "Charge termination Active", "Charge termination Inactive");
	PBCS(BQ27Z758_CHG_STATUS_SU, "Charge suspend Active", "Charge suspend Inactive");
	PBCS(BQ27Z758_CHG_STATUS_IN, "Charge inhibit Active", "Charge inhibit Inactive");
	PBCS(BQ27Z758_CHG_STATUS_HV, "High voltage region Active", "High voltage region Inactive");
	PBCS(BQ27Z758_CHG_STATUS_MV, "Mid voltage region Active", "Mid voltage region Inactive");
	PBCS(BQ27Z758_CHG_STATUS_LV, "Low voltage region Active", "Low voltage region Inactive");
	PBCS(BQ27Z758_CHG_STATUS_PV, "Precharge voltage region Active",
	     "Precharge voltage region Inactive");
#undef PBCS
}

/*
 * BQ27Z855: decode ChargingStatus 40-bit payload (5 bytes).
 */
static void print_charging_status_z855(const struct shell *sh, const uint8_t *data, uint8_t len)
{
	if (len < 5) {
		print_raw_bytes(sh, "ChargingStatus", data, len);
		return;
	}
	shell_print(sh, "ChargingStatus: [%02x %02x %02x %02x %02x]", data[0], data[1], data[2],
		    data[3], data[4]);

#define PBTR(mask, idx, a, i)                                                                      \
	shell_print(sh, "  " #mask " (flags[%d] Bit %u): %s", (idx),                               \
		    __builtin_ctz((uint32_t)(mask)), (data[(idx)] & (mask)) ? (a) : (i))
	PBTR(BQ27Z855_CHG_STATUS_OT, 0, "Overtemperature region", "Normal temperature");
	PBTR(BQ27Z855_CHG_STATUS_HT, 0, "High temperature region", "Not high temperature region");
	PBTR(BQ27Z855_CHG_STATUS_STH, 0, "Standard-high region", "Not standard-high region");
	PBTR(BQ27Z855_CHG_STATUS_RT, 0, "Recommended temperature region",
	     "Not recommended temperature region");
	PBTR(BQ27Z855_CHG_STATUS_STL, 0, "Standard-low region", "Not standard-low region");
	PBTR(BQ27Z855_CHG_STATUS_LT, 0, "Low temperature region", "Not low temperature region");
	PBTR(BQ27Z855_CHG_STATUS_UT, 0, "Undertemperature region", "Not undertemperature region");

	PBTR(BQ27Z855_CHG_STATUS_VCT, 1, "Charge termination Active",
	     "Charge termination Inactive");
	PBTR(BQ27Z855_CHG_STATUS_MCHG, 1, "Maintenance charge Active",
	     "Maintenance charge Inactive");
	PBTR(BQ27Z855_CHG_STATUS_SU, 1, "Suspend charge Active", "Suspend charge Inactive");
	PBTR(BQ27Z855_CHG_STATUS_IN, 1, "Charge inhibit Active", "Charge inhibit Inactive");
	PBTR(BQ27Z855_CHG_STATUS_HV, 1, "High voltage region Active",
	     "High voltage region Inactive");
	PBTR(BQ27Z855_CHG_STATUS_MV, 1, "Mid voltage region Active", "Mid voltage region Inactive");
	PBTR(BQ27Z855_CHG_STATUS_LV, 1, "Low voltage region Active", "Low voltage region Inactive");
	PBTR(BQ27Z855_CHG_STATUS_PV, 1, "Precharge voltage region Active",
	     "Precharge voltage region Inactive");

	PBTR(BQ27Z855_CHG_STATUS_DEG1, 2, "Degradation level 1 Active",
	     "Degradation level 1 Inactive");
	PBTR(BQ27Z855_CHG_STATUS_DEG0, 2, "Degradation level 0 Active",
	     "Degradation level 0 Inactive");
	PBTR(BQ27Z855_CHG_STATUS_NCT, 2, "Near charge termination Active",
	     "Near charge termination Inactive");
#undef PBTR
}

/*
 * BQ27Z758: decode GaugingStatus 24-bit payload.
 * The 24-bit value is packed as flags[0]=bits 7:0, flags[1]=bits 15:8,
 * flags[2]=bits 23:16 in little-endian order.
 */
static void print_gauging_status_z758(const struct shell *sh, const uint8_t *data, uint8_t len)
{
	if (len < 3) {
		print_raw_bytes(sh, "GaugingStatus", data, len);
		return;
	}
	/* Reconstruct 24-bit value from 3 little-endian bytes. */
	const uint32_t gauging =
		(uint32_t)data[0] | ((uint32_t)data[1] << 8) | ((uint32_t)data[2] << 16);

	shell_print(sh, "GaugingStatus: 0x%06" PRIx32, gauging);
#define PB(mask, a, i)                                                                             \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (gauging & (mask)) ? (a) : (i))
	PB(BQ27Z758_GAUGING_STATUS_QMAXDODOK, "Outside flat zone", "In flat zone");
	PB(BQ27Z758_GAUGING_STATUS_OCVFR, "OCV in flat region Detected",
	   "OCV in flat region Not detected");
	PB(BQ27Z758_GAUGING_STATUS_LDMD, "Constant power", "Constant current");
	PB(BQ27Z758_GAUGING_STATUS_RX, "Resistance update Toggled", "Resistance update No toggle");
	PB(BQ27Z758_GAUGING_STATUS_QMAX, "QMax update Toggled", "QMax update No toggle");
	PB(BQ27Z758_GAUGING_STATUS_VDQ, "Qualified for learning", "Not qualified for learning");
	PB(BQ27Z758_GAUGING_STATUS_NSFM, "Negative scale factor", "No negative scale factor");
	PB(BQ27Z758_GAUGING_STATUS_OCVPRED, "Fast OCV prediction Active",
	   "Fast OCV prediction Inactive");
	PB(BQ27Z758_GAUGING_STATUS_SLPQMAX, "QMax update during sleep Active",
	   "QMax update during sleep Inactive");
	PB(BQ27Z758_GAUGING_STATUS_QEN, "IT gauging Enabled", "IT gauging Disabled");
	PB(BQ27Z758_GAUGING_STATUS_VOK, "Voltage OK Detected", "Voltage OK Not detected");
	PB(BQ27Z758_GAUGING_STATUS_RDIS, "Resistance updates Disabled",
	   "Resistance updates Enabled");
	PB(BQ27Z758_GAUGING_STATUS_REST, "OCV reading taken", "Not taken");
	PB(BQ27Z758_GAUGING_STATUS_DSG, "Charging not detected", "Charging detected");
	PB(BQ27Z758_GAUGING_STATUS_EDV, "Termination voltage reached", "Not reached");
	PB(BQ27Z758_GAUGING_STATUS_TC, "Terminate charge Detected",
	   "Terminate charge Not detected");
	PB(BQ27Z758_GAUGING_STATUS_TD, "Terminate discharge Detected",
	   "Terminate discharge Not detected");
	PB(BQ27Z758_GAUGING_STATUS_FC, "Fully charged Detected", "Fully charged Not detected");
	PB(BQ27Z758_GAUGING_STATUS_FD, "Fully discharged Detected",
	   "Fully discharged Not detected");
#undef PB
}

/*
 * BQ27Z855: decode GaugingStatus 40-bit payload (5 bytes).
 */
static void print_gauging_status_z855(const struct shell *sh, const uint8_t *data, uint8_t len)
{
	if (len < 5) {
		print_raw_bytes(sh, "GaugingStatus", data, len);
		return;
	}
	shell_print(sh, "GaugingStatus: [%02x %02x %02x %02x %02x]", data[0], data[1], data[2],
		    data[3], data[4]);

#define PB(mask, idx, a, i)                                                                        \
	shell_print(sh, "  " #mask " (flags[%d] Bit %u): %s", (idx),                               \
		    __builtin_ctz((uint32_t)(mask)), (data[(idx)] & (mask)) ? (a) : (i))
	PB(BQ27Z855_GAUGING_STATUS_DSG, 0, "Discharge/relax mode", "Charge mode");
	PB(BQ27Z855_GAUGING_STATUS_EDV, 0, "Termination voltage reached", "Not reached");
	PB(BQ27Z855_GAUGING_STATUS_TC, 0, "Terminate charge Detected",
	   "Terminate charge Not detected");
	PB(BQ27Z855_GAUGING_STATUS_TD, 0, "Terminate discharge Detected",
	   "Terminate discharge Not detected");
	PB(BQ27Z855_GAUGING_STATUS_FC, 0, "Fully charged Detected", "Fully charged Not detected");
	PB(BQ27Z855_GAUGING_STATUS_FD, 0, "Fully discharged Detected",
	   "Fully discharged Not detected");

	PB(BQ27Z855_GAUGING_STATUS_NSFM, 1, "Negative scale factor mode", "Normal scale");
	PB(BQ27Z855_GAUGING_STATUS_OCVPRED, 1, "OCV predicted Active", "OCV predicted Inactive");
	PB(BQ27Z855_GAUGING_STATUS_SLPQMax, 1, "OCV update in SLEEP Active",
	   "OCV update in SLEEP Inactive");
	PB(BQ27Z855_GAUGING_STATUS_QEN, 1, "IT gauging Enabled", "IT gauging Disabled");
	PB(BQ27Z855_GAUGING_STATUS_VOK, 1, "Voltages OK for QMax update",
	   "Voltages not OK for QMax update");
	PB(BQ27Z855_GAUGING_STATUS_R_DIS, 1, "Resistance updates Disabled",
	   "Resistance updates Enabled");
	PB(BQ27Z855_GAUGING_STATUS_REST, 1, "Rest", "Not at rest");

	PB(BQ27Z855_GAUGING_STATUS_VLB, 2, "Very low battery warning Active",
	   "Very low battery warning Inactive");
	PB(BQ27Z855_GAUGING_STATUS_OCVFR, 2, "OCV in flat region (RELAX) Active",
	   "OCV in flat region Inactive");
	PB(BQ27Z855_GAUGING_STATUS_LDMD, 2, "LOAD mode Active", "LOAD mode Inactive");
	PB(BQ27Z855_GAUGING_STATUS_RX, 2, "Resistance update Toggled", "No toggle");
	PB(BQ27Z855_GAUGING_STATUS_QMax, 2, "QMax update Toggled", "No toggle");
	PB(BQ27Z855_GAUGING_STATUS_VDQ, 2, "Discharge qualified for learning",
	   "Not qualified for learning");
#undef PB
}

/*
 * BQ27Z758: decode ManufacturingStatus 16-bit value.
 */
static void print_manufacturing_status_z758(const struct shell *sh, uint16_t mfg)
{
	shell_print(sh, "ManufacturingStatus: 0x%04" PRIx16, mfg);
#define PB(mask, a, i)                                                                             \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (mfg & (mask)) ? (a) : (i))
	PB(BQ27Z758_MFG_STATUS_CAL_EN, "Calibration mode Enabled", "Calibration mode Disabled");
	PB(BQ27Z758_MFG_STATUS_LT_TEST, "Lifetime speed up Enabled", "Lifetime speed up Disabled");
	PB(BQ27Z758_MFG_STATUS_FET_OVRD, "FET override Active", "FET override Inactive");
	PB(BQ27Z758_MFG_STATUS_PF_EN, "Permanent Failure protection Enabled",
	   "Permanent Failure protection Disabled");
	PB(BQ27Z758_MFG_STATUS_LF_EN, "Lifetime data collection Enabled",
	   "Lifetime data collection Disabled");
	PB(BQ27Z758_MFG_STATUS_FET_EN, "FW FET control Enabled", "FW FET control Disabled");
	PB(BQ27Z758_MFG_STATUS_GAUGE_EN, "Gas gauging Enabled", "Gas gauging Disabled");
	PB(BQ27Z758_MFG_STATUS_DSG_TEST, "DSG FET test Active", "DSG FET test Inactive");
	PB(BQ27Z758_MFG_STATUS_CHG_TEST, "CHG FET test Active", "CHG FET test Inactive");
#undef PB
}

/*
 * BQ27Z855: decode ManufacturingStatus 16-bit value.
 */
static void print_manufacturing_status_z855(const struct shell *sh, uint16_t mfg)
{
	shell_print(sh, "ManufacturingStatus: 0x%04" PRIx16, mfg);
#define PB(mask, a, i)                                                                             \
	shell_print(sh, "  %s (Bit %u): %s", #mask, __builtin_ctz((uint32_t)(mask)),               \
		    (mfg & (mask)) ? (a) : (i))
	PB(BQ27Z855_MFG_STATUS_CAL_EN, "Calibration mode Enabled", "Calibration mode Disabled");
	PB(BQ27Z855_MFG_STATUS_LT_TEST, "Lifetime speed up Enabled", "Lifetime speed up Disabled");
	PB(BQ27Z855_MFG_STATUS_FUSE_EN, "Fuse action Enabled", "Fuse action Disabled");
	PB(BQ27Z855_MFG_STATUS_BBR_EN, "Black box recorder Enabled", "Black box recorder Disabled");
	PB(BQ27Z855_MFG_STATUS_PF_EN, "Permanent failure Enabled", "Permanent failure Disabled");
	PB(BQ27Z855_MFG_STATUS_LF_EN, "Lifetime data collection Enabled",
	   "Lifetime data collection Disabled");
	PB(BQ27Z855_MFG_STATUS_FET_EN, "All FET action Enabled", "All FET action Disabled");
	PB(BQ27Z855_MFG_STATUS_GAUGE_EN, "Gas gauging Enabled", "Gas gauging Disabled");
	PB(BQ27Z855_MFG_STATUS_DSG_EN, "Discharge FET test Active", "Discharge FET test Inactive");
	PB(BQ27Z855_MFG_STATUS_CHG_EN, "Charge FET test Active", "Charge FET test Inactive");
	PB(BQ27Z855_MFG_STATUS_PCHG_EN, "Precharge FET test Active", "Precharge FET test Inactive");
#undef PB
}

/*
 * Shared DAStatus1 printout (same MAC address and layout for both devices).
 */
static void print_dastatus1(const struct shell *sh, const uint8_t *data, uint8_t len)
{
	if (len < 32) {
		print_raw_bytes(sh, "DAStatus1", data, len);
		return;
	}
	shell_print(sh, "DAStatus1:");
	shell_print(sh, "  Cell Voltage: %u mV", sys_get_le16(&data[0]));
	shell_print(sh, "  Pack Pin Voltage: %u mV", sys_get_le16(&data[10]));
	shell_print(sh, "  Cell Current (simultaneous): %d mA", (int16_t)sys_get_le16(&data[12]));
	shell_print(sh, "  Cell Power: %d mW", (int16_t)sys_get_le16(&data[20]));
	shell_print(sh, "  Power (V*I): %d mW", (int16_t)sys_get_le16(&data[28]));
	shell_print(sh, "  Average Power (V*AvgI): %d mW", (int16_t)sys_get_le16(&data[30]));
}

/*
 * Shared DAStatus2 printout.
 */
static void print_dastatus2(const struct shell *sh, const uint8_t *data, uint8_t len)
{
	if (len < 20) {
		print_raw_bytes(sh, "DAStatus2", data, len);
		return;
	}
	shell_print(sh, "DAStatus2:");
	shell_print(sh, "  Int Temperature: %u (0.1K)", sys_get_le16(&data[0]));
	shell_print(sh, "  TS1 Temperature: %u (0.1K)", sys_get_le16(&data[2]));
	shell_print(sh, "  TS2 Temperature: %u (0.1K)", sys_get_le16(&data[4]));
	shell_print(sh, "  Cell Temperature: %u (0.1K)", sys_get_le16(&data[10]));
	shell_print(sh, "  FET Temperature: %u (0.1K)", sys_get_le16(&data[12]));
	shell_print(sh, "  Cell Raw Voltage: 0x%04" PRIx16, sys_get_le16(&data[14]));
	shell_print(sh, "  Cell Raw Current: 0x%04" PRIx16, sys_get_le16(&data[16]));
}

/*
 * ITStatus1 / GaugeStatus1 (MAC 0x0073) printout — common to both devices.
 */
static void print_itstatus1(const struct shell *sh, const uint8_t *data, uint8_t len, bool is_z855)
{
	const char *name = is_z855 ? "GaugeStatus1" : "ITStatus1";

	if (len < 20) {
		print_raw_bytes(sh, name, data, len);
		return;
	}
	shell_print(sh, "%s:", name);
	shell_print(sh, "  True Rem Q (mAh): %d", (int16_t)sys_get_le16(&data[0]));
	shell_print(sh, "  True Rem E (cWh): %d", (int16_t)sys_get_le16(&data[2]));
	shell_print(sh, "  Initial Q: %u", sys_get_le16(&data[4]));
	shell_print(sh, "  Initial E: %u", sys_get_le16(&data[6]));
	shell_print(sh, "  True Full Chg Q (mAh): %u", sys_get_le16(&data[8]));
	shell_print(sh, "  True Full Chg E (cWh): %u", sys_get_le16(&data[10]));
	shell_print(sh, "  T_sim: %u (0.1K)", sys_get_le16(&data[12]));
	shell_print(sh, "  T_ambient: %u (0.1K)", sys_get_le16(&data[14]));
	shell_print(sh, "  RaScale: 0x%04" PRIx16, sys_get_le16(&data[16]));
	shell_print(sh, "  CompRes: 0x%04" PRIx16, sys_get_le16(&data[18]));
}

/*
 * ITStatus2 / GaugeStatus2 (MAC 0x0074) printout.
 */
static void print_itstatus2(const struct shell *sh, const uint8_t *data, uint8_t len, bool is_z855)
{
	const char *name = is_z855 ? "GaugeStatus2" : "ITStatus2";

	if (len < 20) {
		print_raw_bytes(sh, name, data, len);
		return;
	}
	const uint8_t lstatus = data[1];
	const uint8_t cell_grid = data[2];
	const uint32_t state_time = sys_get_le32(&data[6]);
	const uint8_t qmax_status = lstatus & 0x3;
	const char *qmax_status_str = "Reserved";

	switch (qmax_status) {
	case 0x0:
		qmax_status_str = "Battery OK";
		break;
	case 0x1:
		qmax_status_str = "QMax first updated in learning cycle";
		break;
	case 0x2:
		qmax_status_str = "QMax + resistance table updated in learning cycle";
		break;
	default:
		break;
	}

	shell_print(sh, "%s:", name);
	shell_print(sh, "  LStatus: 0x%02x", lstatus);
	shell_print(sh, "    QMax (Bit 3): %s",
		    (lstatus & BIT(3)) ? "Updated in field" : "Not updated");
	shell_print(sh, "    ITEN (Bit 2): %s", (lstatus & BIT(2)) ? "IT enabled" : "IT disabled");
	shell_print(sh, "    CF1:CF0 (Bits 1-0): %s (0x%x)", qmax_status_str, qmax_status);
	shell_print(sh, "  Cell Grid: %u", cell_grid);
	shell_print(sh, "  State Time: %u s", state_time);
	shell_print(sh, "  DOD0: 0x%04" PRIx16, sys_get_le16(&data[10]));
	shell_print(sh, "  DOD0 Passed Q: 0x%04" PRIx16, sys_get_le16(&data[12]));
	shell_print(sh, "  DOD0 Passed E: 0x%04" PRIx16, sys_get_le16(&data[14]));
	shell_print(sh, "  DOD0 Time: %u", sys_get_le16(&data[16]));
	shell_print(sh, "  DODEOC: 0x%04" PRIx16, sys_get_le16(&data[18]));
}

/*
 * ITStatus3 / GaugeStatus3 (MAC 0x0075) printout.
 */
static void print_itstatus3(const struct shell *sh, const uint8_t *data, uint8_t len, bool is_z855)
{
	const char *name = is_z855 ? "GaugeStatus3" : "ITStatus3";

	if (len < 14) {
		print_raw_bytes(sh, name, data, len);
		return;
	}
	shell_print(sh, "%s:", name);
	shell_print(sh, "  QMax 0: %u", sys_get_le16(&data[0]));
	shell_print(sh, "  QMax DOD0: 0x%04" PRIx16, sys_get_le16(&data[2]));
	shell_print(sh, "  QMax Passed Q: %u", sys_get_le16(&data[4]));
	shell_print(sh, "  QMax Time: %u", sys_get_le16(&data[6]));
	shell_print(sh, "  Temp k: 0x%04" PRIx16, sys_get_le16(&data[8]));
	shell_print(sh, "  Temp a: 0x%04" PRIx16, sys_get_le16(&data[10]));
	shell_print(sh, "  Raw DOD0: 0x%04" PRIx16, sys_get_le16(&data[12]));
}

/* ---------------------------------------------------------------------------
 * Security state extraction (common to both devices)
 * ---------------------------------------------------------------------------
 */

/*
 * Extract SEC bits from OperationStatus bytes[0..1] (first 16-bit word).
 * The encoding is identical for both BQ27Z758 and BQ27Z855:
 *   (SEC1,SEC0) at bits [9:8]:  0b00=Reserved, 0b01=Full Access,
 *                                0b10=Unsealed, 0b11=Sealed
 */
static const char *decode_security_mode(const uint8_t *op_status_bytes)
{
	const uint16_t op_a = sys_get_le16(op_status_bytes);
	const uint8_t sec = (uint8_t)((op_a & GENMASK(9, 8)) >> 8);

	switch (sec) {
	case 0x0:
		return "Reserved";
	case 0x1:
		return "Full Access";
	case 0x2:
		return "Unsealed";
	case 0x3:
		return "Sealed";
	default:
		return "Unknown";
	}
}

/* ---------------------------------------------------------------------------
 * Shell commands
 * ---------------------------------------------------------------------------
 */

static int cmd_bq27z8xx_status(const struct shell *sh, size_t argc, char **argv)
{
	(void)argc;

	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	const bool is_z855 = bq27z8xx_shell_is_z855(dev);

	shell_print(sh, "BQ27Z8XX status for %s (%s)", dev->name,
		    is_z855 ? "BQ27Z855" : "BQ27Z758");

	/* BatteryStatus register */
	uint16_t reg_val = 0;
	int ret = bq27z8xx_read16(dev, BQ27Z8XX_BATTERYSTATUS, &reg_val);

	if (ret == 0) {
		print_battery_status(sh, reg_val);
	} else {
		shell_error(sh, "BatteryStatus read failed: %d", ret);
	}

	/* InterruptStatus register */
	uint8_t reg_val8 = 0;

	ret = bq27z8xx_read8(dev, BQ27Z8XX_INTERRUPTSTATUS, &reg_val8);
	if (ret == 0) {
		print_interrupt_status(sh, reg_val8);
	} else {
		shell_error(sh, "InterruptStatus read failed: %d", ret);
	}

	uint8_t data[MAC_DATA_LEN] = {0};
	uint8_t reported_len = 0;

	/* SafetyAlert */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_SAFETYALERT
				   : (uint16_t)BQ27Z758_MAC_CMD_SAFETYALERT,
			   "SafetyAlert", data, &reported_len) == 0) {
		if (reported_len >= 4) {
			const uint32_t v = sys_get_le32(&data[0]);

			if (is_z855) {
				print_safety_alert_z855(sh, v);
			} else {
				print_safety_alert_z758(sh, v);
			}
		} else {
			print_raw_bytes(sh, "SafetyAlert", data, reported_len);
		}
	}

	/* SafetyStatus */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_SAFETYSTATUS
				   : (uint16_t)BQ27Z758_MAC_CMD_SAFETYSTATUS,
			   "SafetyStatus", data, &reported_len) == 0) {
		if (reported_len >= 4) {
			const uint32_t v = sys_get_le32(&data[0]);

			if (is_z855) {
				print_safety_status_z855(sh, v);
			} else {
				print_safety_status_z758(sh, v);
			}
		} else {
			print_raw_bytes(sh, "SafetyStatus", data, reported_len);
		}
	}

	/* PFAlert */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_PFALERT
				   : (uint16_t)BQ27Z758_MAC_CMD_PFALERT,
			   "PFAlert", data, &reported_len) == 0) {
		if (reported_len >= 4) {
			if (is_z855) {
				print_pf_z855(sh, "PFAlert", sys_get_le32(&data[0]), false);
			} else {
				print_pf_alert_z758(sh, data, reported_len);
			}
		} else {
			print_raw_bytes(sh, "PFAlert", data, reported_len);
		}
	}

	/* PFStatus */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_PFSTATUS
				   : (uint16_t)BQ27Z758_MAC_CMD_PFSTATUS,
			   "PFStatus", data, &reported_len) == 0) {
		if (reported_len >= 4) {
			if (is_z855) {
				print_pf_z855(sh, "PFStatus", sys_get_le32(&data[0]), true);
			} else {
				print_pf_alert_z758(sh, data, reported_len);
			}
		} else {
			print_raw_bytes(sh, "PFStatus", data, reported_len);
		}
	}

	/* OperationStatus */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_OPERATIONSTATUS
				   : (uint16_t)BQ27Z758_MAC_CMD_OPERATIONSTATUS,
			   "OperationStatus", data, &reported_len) == 0) {
		if (reported_len >= 4) {
			if (is_z855) {
				print_operation_status_z855(sh, sys_get_le32(&data[0]));
			} else {
				print_operation_status_z758(sh, sys_get_le16(&data[0]),
							    sys_get_le16(&data[2]));
			}
		} else if (reported_len >= 2) {
			shell_print(sh, "OperationStatus: 0x%04" PRIx16, sys_get_le16(&data[0]));
		} else {
			print_raw_bytes(sh, "OperationStatus", data, reported_len);
		}
	}

	/* ChargingStatus */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_CHARGINGSTATUS
				   : (uint16_t)BQ27Z758_MAC_CMD_CHARGINGSTATUS,
			   "ChargingStatus", data, &reported_len) == 0) {
		if (is_z855) {
			print_charging_status_z855(sh, data, reported_len);
		} else {
			print_charging_status_z758(sh, data, reported_len);
		}
	}

	/* GaugingStatus */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_GAUGINGSTATUS
				   : (uint16_t)BQ27Z758_MAC_CMD_GAUGINGSTATUS,
			   "GaugingStatus", data, &reported_len) == 0) {
		if (is_z855) {
			print_gauging_status_z855(sh, data, reported_len);
		} else {
			print_gauging_status_z758(sh, data, reported_len);
		}
	}

	/* ManufacturingStatus */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_MANUFACTURINGSTATUS
				   : (uint16_t)BQ27Z758_MAC_CMD_MANUFACTURINGSTATUS,
			   "ManufacturingStatus", data, &reported_len) == 0) {
		if (reported_len >= 2) {
			const uint16_t mfg = sys_get_le16(&data[0]);

			if (is_z855) {
				print_manufacturing_status_z855(sh, mfg);
			} else {
				print_manufacturing_status_z758(sh, mfg);
			}
		} else {
			print_raw_bytes(sh, "ManufacturingStatus", data, reported_len);
		}
	}

	/* DAStatus1 — same address (0x0071) on both devices */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_DASTATUS1
				   : (uint16_t)BQ27Z758_MAC_CMD_DASTATUS1,
			   "DAStatus1", data, &reported_len) == 0) {
		print_dastatus1(sh, data, reported_len);
	}

	/* DAStatus2 — same address (0x0072) on both devices */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_DASTATUS2
				   : (uint16_t)BQ27Z758_MAC_CMD_DASTATUS2,
			   "DAStatus2", data, &reported_len) == 0) {
		print_dastatus2(sh, data, reported_len);
	}

	/* ITStatus1 / GaugeStatus1 (MAC 0x0073) */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_GAUGESTATUS1
				   : (uint16_t)BQ27Z758_MAC_CMD_ITSTATUS1,
			   is_z855 ? "GaugeStatus1" : "ITStatus1", data, &reported_len) == 0) {
		print_itstatus1(sh, data, reported_len, is_z855);
	}

	/* ITStatus2 / GaugeStatus2 (MAC 0x0074) */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_GAUGESTATUS2
				   : (uint16_t)BQ27Z758_MAC_CMD_ITSTATUS2,
			   is_z855 ? "GaugeStatus2" : "ITStatus2", data, &reported_len) == 0) {
		print_itstatus2(sh, data, reported_len, is_z855);
	}

	/* ITStatus3 / GaugeStatus3 (MAC 0x0075) */
	if (shell_read_mac(sh, dev,
			   is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_GAUGESTATUS3
				   : (uint16_t)BQ27Z758_MAC_CMD_ITSTATUS3,
			   is_z855 ? "GaugeStatus3" : "ITStatus3", data, &reported_len) == 0) {
		print_itstatus3(sh, data, reported_len, is_z855);
	}

	return 0;
}

static int cmd_bq27z8xx_security(const struct shell *sh, size_t argc, char **argv)
{
	(void)argc;

	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	const bool is_z855 = bq27z8xx_shell_is_z855(dev);
	uint8_t data[MAC_DATA_LEN] = {0};
	uint8_t reported_len = 0;
	const uint16_t op_cmd = is_z855 ? (uint16_t)BQ27Z855_MAC_CMD_OPERATIONSTATUS
					: (uint16_t)BQ27Z758_MAC_CMD_OPERATIONSTATUS;
	int ret = shell_read_mac(sh, dev, op_cmd, "OperationStatus", data, &reported_len);

	if (ret == 0 && reported_len >= 2) {
		const char *sec_str = decode_security_mode(data);
		const uint16_t op_a = sys_get_le16(&data[0]);
		const uint8_t sec = (uint8_t)((op_a & GENMASK(9, 8)) >> 8);

		if (reported_len >= 4) {
			const uint16_t op_b = sys_get_le16(&data[2]);

			shell_print(sh, "OperationStatusA: 0x%04" PRIx16, op_a);
			shell_print(sh, "OperationStatusB: 0x%04" PRIx16, op_b);
		} else {
			shell_print(sh, "OperationStatus: 0x%04" PRIx16, op_a);
		}
		shell_print(sh, "Security: %s (SEC1,SEC0=0x%x)", sec_str, sec);
		return 0;
	}

	shell_error(sh, "Could not read OperationStatus: %d", ret);
	return ret < 0 ? ret : -EIO;
}

/* ---------------------------------------------------------------------------
 * Lifetime data dump
 * ---------------------------------------------------------------------------
 */

enum lifetime_type {
	LT_I1,
	LT_I2,
	LT_U1,
	LT_U2,
	LT_U4,
};

struct lifetime_field {
	uint8_t block;
	uint8_t offset;
	enum lifetime_type type;
	const char *label;
	const char *unit;
};

/* BQ27Z758: 13 sequential lifetime data blocks (MAC 0x0060–0x006C). */
#define BQ27Z758_LIFETIME_BLOCK_COUNT 13

static const struct lifetime_field lifetime_fields_z758[] = {
	/* Block 1 (0x0060) */
	{0, 0, LT_I2, "Cell 1 Max Voltage", "mV"},
	{0, 2, LT_I2, "Cell 1 Min Voltage", "mV"},
	{0, 4, LT_I2, "Max Charge Current", "mA"},
	{0, 6, LT_I2, "Max Discharge Current", "mA"},
	{0, 8, LT_I2, "Max Avg Dsg Current", "mA"},
	{0, 10, LT_I2, "Max Avg Dsg Power", "cW"},
	{0, 12, LT_I1, "Max Temp Cell", "C"},
	{0, 13, LT_I1, "Min Temp Cell", "C"},
	{0, 14, LT_I1, "Max Temp Int Sensor", "C"},
	{0, 15, LT_I1, "Min Temp Int Sensor", "C"},

	/* Block 2 (0x0061) */
	{1, 0, LT_U1, "No Of Shutdowns", "events"},

	/* Block 3 (0x0062) */
	{2, 0, LT_U4, "Total Fw Runtime", "s"},
	{2, 4, LT_U4, "Total Charge Time", "s"},
	{2, 8, LT_U4, "Total Discharge Time", "s"},

	/* Block 4 (0x0063) */
	{3, 0, LT_U2, "No Of COV Events", "events"},
	{3, 2, LT_U2, "Last COV Event", "cycles"},
	{3, 4, LT_U2, "No Of CUV Events", "events"},
	{3, 6, LT_U2, "Last CUV Event", "cycles"},
	{3, 8, LT_U2, "No Of OCD Events", "events"},
	{3, 10, LT_U2, "Last OCD Event", "cycles"},
	{3, 12, LT_U2, "No Of OCC Events", "events"},
	{3, 14, LT_U2, "Last OCC Event", "cycles"},
	{3, 16, LT_U2, "No Of UTC Events", "events"},
	{3, 18, LT_U2, "Last UTC Event", "cycles"},
	{3, 20, LT_U2, "No Of HSCD Events", "events"},
	{3, 22, LT_U2, "Last HSCD Event", "cycles"},
	{3, 24, LT_U2, "No Of UTD Events", "events"},
	{3, 26, LT_U2, "Last UTD Event", "cycles"},
	{3, 28, LT_U2, "No Of OTC Events", "events"},
	{3, 30, LT_U2, "Last OTC Event", "cycles"},

	/* Block 5 (0x0064) */
	{4, 0, LT_U2, "No Of OTD Events", "events"},
	{4, 2, LT_U2, "Last OTD Event", "cycles"},
	{4, 4, LT_U2, "No Of OTF Events", "events"},
	{4, 6, LT_U2, "Last OTF Event", "cycles"},
	{4, 8, LT_U2, "No Valid Charge Term", "events"},
	{4, 10, LT_U2, "Last Valid Charge Term", "cycles"},
	{4, 12, LT_U2, "No Of Qmax Updates", "events"},
	{4, 14, LT_U2, "Last Qmax Update", "cycles"},
	{4, 16, LT_U2, "No Of Ra Updates", "events"},
	{4, 18, LT_U2, "Last Ra Update", "cycles"},
	{4, 20, LT_U2, "No Of Ra Disable", "events"},
	{4, 22, LT_U2, "Last Ra Disable", "cycles"},
	{4, 24, LT_U2, "No Of PTO Events", "events"},
	{4, 26, LT_U2, "Last PTO Event", "cycles"},
	{4, 28, LT_U2, "No Of CTO Events", "events"},
	{4, 30, LT_U2, "Last CTO Event", "cycles"},

	/* Blocks 6–11 (0x0065–0x006A): time-in-temperature-band RSOC histograms */
	{5, 0, LT_U4, "Time In UT RSOC A", "s"},
	{5, 4, LT_U4, "Time In UT RSOC B", "s"},
	{5, 8, LT_U4, "Time In UT RSOC C", "s"},
	{5, 12, LT_U4, "Time In UT RSOC D", "s"},
	{5, 16, LT_U4, "Time In UT RSOC E", "s"},
	{5, 20, LT_U4, "Time In UT RSOC F", "s"},
	{5, 24, LT_U4, "Time In UT RSOC G", "s"},
	{5, 28, LT_U4, "Time In UT RSOC H", "s"},

	{6, 0, LT_U4, "Time In LT RSOC A", "s"},
	{6, 4, LT_U4, "Time In LT RSOC B", "s"},
	{6, 8, LT_U4, "Time In LT RSOC C", "s"},
	{6, 12, LT_U4, "Time In LT RSOC D", "s"},
	{6, 16, LT_U4, "Time In LT RSOC E", "s"},
	{6, 20, LT_U4, "Time In LT RSOC F", "s"},
	{6, 24, LT_U4, "Time In LT RSOC G", "s"},
	{6, 28, LT_U4, "Time In LT RSOC H", "s"},

	{7, 0, LT_U4, "Time In STL RSOC A", "s"},
	{7, 4, LT_U4, "Time In STL RSOC B", "s"},
	{7, 8, LT_U4, "Time In STL RSOC C", "s"},
	{7, 12, LT_U4, "Time In STL RSOC D", "s"},
	{7, 16, LT_U4, "Time In STL RSOC E", "s"},
	{7, 20, LT_U4, "Time In STL RSOC F", "s"},
	{7, 24, LT_U4, "Time In STL RSOC G", "s"},
	{7, 28, LT_U4, "Time In STL RSOC H", "s"},

	{8, 0, LT_U4, "Time In RT RSOC A", "s"},
	{8, 4, LT_U4, "Time In RT RSOC B", "s"},
	{8, 8, LT_U4, "Time In RT RSOC C", "s"},
	{8, 12, LT_U4, "Time In RT RSOC D", "s"},
	{8, 16, LT_U4, "Time In RT RSOC E", "s"},
	{8, 20, LT_U4, "Time In RT RSOC F", "s"},
	{8, 24, LT_U4, "Time In RT RSOC G", "s"},
	{8, 28, LT_U4, "Time In RT RSOC H", "s"},

	{9, 0, LT_U4, "Time In STH RSOC A", "s"},
	{9, 4, LT_U4, "Time In STH RSOC B", "s"},
	{9, 8, LT_U4, "Time In STH RSOC C", "s"},
	{9, 12, LT_U4, "Time In STH RSOC D", "s"},
	{9, 16, LT_U4, "Time In STH RSOC E", "s"},
	{9, 20, LT_U4, "Time In STH RSOC F", "s"},
	{9, 24, LT_U4, "Time In STH RSOC G", "s"},
	{9, 28, LT_U4, "Time In STH RSOC H", "s"},

	{10, 0, LT_U4, "Time In HT RSOC A", "s"},
	{10, 4, LT_U4, "Time In HT RSOC B", "s"},
	{10, 8, LT_U4, "Time In HT RSOC C", "s"},
	{10, 12, LT_U4, "Time In HT RSOC D", "s"},
	{10, 16, LT_U4, "Time In HT RSOC E", "s"},
	{10, 20, LT_U4, "Time In HT RSOC F", "s"},
	{10, 24, LT_U4, "Time In HT RSOC G", "s"},
	{10, 28, LT_U4, "Time In HT RSOC H", "s"},

	{11, 0, LT_U4, "Time In OT RSOC A", "s"},
	{11, 4, LT_U4, "Time In OT RSOC B", "s"},
	{11, 8, LT_U4, "Time In OT RSOC C", "s"},
	{11, 12, LT_U4, "Time In OT RSOC D", "s"},
	{11, 16, LT_U4, "Time In OT RSOC E", "s"},
	{11, 20, LT_U4, "Time In OT RSOC F", "s"},
	{11, 24, LT_U4, "Time In OT RSOC G", "s"},
	{11, 28, LT_U4, "Time In OT RSOC H", "s"},

	/* Block 13 (0x006C) */
	{12, 4, LT_U2, "No Full Discharge", "events"},
	{12, 6, LT_U2, "Last Full Discharge", "cycles"},
	{12, 8, LT_U2, "No Of HCOV Events", "events"},
	{12, 10, LT_U2, "Last HCOV Event", "cycles"},
	{12, 12, LT_U2, "No Of HCUV Events", "events"},
	{12, 14, LT_U2, "Last HCUV Event", "cycles"},
	{12, 16, LT_U2, "No Of HOCC Events", "events"},
	{12, 18, LT_U2, "Last HOCC Event", "cycles"},
	{12, 20, LT_U2, "No Of HOCD Events", "events"},
	{12, 22, LT_U2, "Last HOCD Event", "cycles"},
};

/*
 * BQ27Z855: 16 lifetime data blocks.
 * Blocks 1–15 are at sequential MAC addresses 0x0060–0x006E (indices 0–14).
 * Block 16 is at the non-sequential address 0x007E (index 15).
 */
#define BQ27Z855_LIFETIME_BLOCK_COUNT 16

static const uint16_t z855_lifetime_macs[BQ27Z855_LIFETIME_BLOCK_COUNT] = {
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK1,  /* 0x0060 */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK2,  /* 0x0061 */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK3,  /* 0x0062 */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK4,  /* 0x0063 */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK5,  /* 0x0064 */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK6,  /* 0x0065 */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK7,  /* 0x0066 */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK8,  /* 0x0067 */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK9,  /* 0x0068 */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK10, /* 0x0069 */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK11, /* 0x006A */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK12, /* 0x006B */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK13, /* 0x006C */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK14, /* 0x006D */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK15, /* 0x006E */
	BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK16, /* 0x007E — non-sequential */
};

static const struct lifetime_field lifetime_fields_z855[] = {
	/*
	 * Block 1 (0x0060): voltage/current extremes.
	 * Offsets 2–7 and 10–17 are RSVD; only the named fields are decoded.
	 */
	{0, 0, LT_I2, "Cell 1 Max Voltage", "mV"},
	{0, 8, LT_I2, "Cell 1 Min Voltage", "mV"},
	{0, 18, LT_I2, "Max Charge Current", "mA"},
	{0, 20, LT_I2, "Max Discharge Current", "mA"},
	{0, 22, LT_I2, "Max Avg Dsg Current", "mA"},
	{0, 24, LT_I2, "Max Avg Dsg Power", "cW"},

	/* Block 2 (0x0061): SOH capacity minima + cell balance time */
	{1, 0, LT_I2, "Min FCC-SOH", "mAh"},
	{1, 2, LT_I2, "Min FCC-SOH", "cWh"},
	{1, 4, LT_U4, "CB Time Cell 1", "s"},

	/* Block 3 (0x0062): firmware runtime */
	{2, 0, LT_U4, "Total FW Runtime", "s"},

	/* Block 4 (0x0063): protection event counts and last-event cycle indices */
	{3, 0, LT_U2, "No Of COV Events", "events"},
	{3, 2, LT_U2, "Last COV Event", "cycles"},
	{3, 4, LT_U2, "No Of CUV Events", "events"},
	{3, 6, LT_U2, "Last CUV Event", "cycles"},
	{3, 8, LT_U2, "No Of OCD1 Events", "events"},
	{3, 10, LT_U2, "Last OCD1 Event", "cycles"},
	{3, 12, LT_U2, "No Of OCD2 Events", "events"},
	{3, 14, LT_U2, "Last OCD2 Event", "cycles"},
	{3, 16, LT_U2, "No Of OCC1 Events", "events"},
	{3, 18, LT_U2, "Last OCC1 Event", "cycles"},
	{3, 20, LT_U2, "No Of OCC2 Events", "events"},
	{3, 22, LT_U2, "Last OCC2 Event", "cycles"},
	{3, 24, LT_U2, "No Of AOCD Events", "events"},
	{3, 26, LT_U2, "Last AOCD Event", "cycles"},
	{3, 28, LT_U2, "No Of ASCD Events", "events"},
	{3, 30, LT_U2, "Last ASCD Event", "cycles"},

	/* Block 5 (0x0064): more protection and gauging event counters */
	{4, 0, LT_U2, "No Of AOCC Events", "events"},
	{4, 2, LT_U2, "Last AOCC Event", "cycles"},
	{4, 4, LT_U2, "No Of OTC Events", "events"},
	{4, 6, LT_U2, "Last OTC Event", "cycles"},
	{4, 8, LT_U2, "No Of OTD Events", "events"},
	{4, 10, LT_U2, "Last OTD Event", "cycles"},
	{4, 12, LT_U2, "No Of OTF Events", "events"},
	{4, 14, LT_U2, "Last OTF Event", "cycles"},
	{4, 16, LT_U2, "No Valid Charge Term", "events"},
	{4, 18, LT_U2, "Last Valid Charge Term", "cycles"},
	{4, 20, LT_U2, "No Of Qmax Updates", "events"},
	{4, 22, LT_U2, "Last Qmax Update", "cycles"},
	{4, 24, LT_U2, "No Of Ra Updates", "events"},
	{4, 26, LT_U2, "Last Ra Update", "cycles"},
	{4, 28, LT_U2, "No Of Ra Disable", "events"},
	{4, 30, LT_U2, "Last Ra Disable", "cycles"},

	/* Blocks 6–12 (0x0065–0x006B): RSOC histograms per temperature band */
	{5, 0, LT_U4, "Time In LFT_UT RSOC A", "s"},
	{5, 4, LT_U4, "Time In LFT_UT RSOC B", "s"},
	{5, 8, LT_U4, "Time In LFT_UT RSOC C", "s"},
	{5, 12, LT_U4, "Time In LFT_UT RSOC D", "s"},
	{5, 16, LT_U4, "Time In LFT_UT RSOC E", "s"},
	{5, 20, LT_U4, "Time In LFT_UT RSOC F", "s"},
	{5, 24, LT_U4, "Time In LFT_UT RSOC G", "s"},
	{5, 28, LT_U4, "Time In LFT_UT RSOC H", "s"},

	{6, 0, LT_U4, "Time In LFT_LT RSOC A", "s"},
	{6, 4, LT_U4, "Time In LFT_LT RSOC B", "s"},
	{6, 8, LT_U4, "Time In LFT_LT RSOC C", "s"},
	{6, 12, LT_U4, "Time In LFT_LT RSOC D", "s"},
	{6, 16, LT_U4, "Time In LFT_LT RSOC E", "s"},
	{6, 20, LT_U4, "Time In LFT_LT RSOC F", "s"},
	{6, 24, LT_U4, "Time In LFT_LT RSOC G", "s"},
	{6, 28, LT_U4, "Time In LFT_LT RSOC H", "s"},

	{7, 0, LT_U4, "Time In LFT_STL RSOC A", "s"},
	{7, 4, LT_U4, "Time In LFT_STL RSOC B", "s"},
	{7, 8, LT_U4, "Time In LFT_STL RSOC C", "s"},
	{7, 12, LT_U4, "Time In LFT_STL RSOC D", "s"},
	{7, 16, LT_U4, "Time In LFT_STL RSOC E", "s"},
	{7, 20, LT_U4, "Time In LFT_STL RSOC F", "s"},
	{7, 24, LT_U4, "Time In LFT_STL RSOC G", "s"},
	{7, 28, LT_U4, "Time In LFT_STL RSOC H", "s"},

	{8, 0, LT_U4, "Time In LFT_RT RSOC A", "s"},
	{8, 4, LT_U4, "Time In LFT_RT RSOC B", "s"},
	{8, 8, LT_U4, "Time In LFT_RT RSOC C", "s"},
	{8, 12, LT_U4, "Time In LFT_RT RSOC D", "s"},
	{8, 16, LT_U4, "Time In LFT_RT RSOC E", "s"},
	{8, 20, LT_U4, "Time In LFT_RT RSOC F", "s"},
	{8, 24, LT_U4, "Time In LFT_RT RSOC G", "s"},
	{8, 28, LT_U4, "Time In LFT_RT RSOC H", "s"},

	{9, 0, LT_U4, "Time In LFT_STH RSOC A", "s"},
	{9, 4, LT_U4, "Time In LFT_STH RSOC B", "s"},
	{9, 8, LT_U4, "Time In LFT_STH RSOC C", "s"},
	{9, 12, LT_U4, "Time In LFT_STH RSOC D", "s"},
	{9, 16, LT_U4, "Time In LFT_STH RSOC E", "s"},
	{9, 20, LT_U4, "Time In LFT_STH RSOC F", "s"},
	{9, 24, LT_U4, "Time In LFT_STH RSOC G", "s"},
	{9, 28, LT_U4, "Time In LFT_STH RSOC H", "s"},

	{10, 0, LT_U4, "Time In LFT_HT RSOC A", "s"},
	{10, 4, LT_U4, "Time In LFT_HT RSOC B", "s"},
	{10, 8, LT_U4, "Time In LFT_HT RSOC C", "s"},
	{10, 12, LT_U4, "Time In LFT_HT RSOC D", "s"},
	{10, 16, LT_U4, "Time In LFT_HT RSOC E", "s"},
	{10, 20, LT_U4, "Time In LFT_HT RSOC F", "s"},
	{10, 24, LT_U4, "Time In LFT_HT RSOC G", "s"},
	{10, 28, LT_U4, "Time In LFT_HT RSOC H", "s"},

	{11, 0, LT_U4, "Time In LFT_OT RSOC A", "s"},
	{11, 4, LT_U4, "Time In LFT_OT RSOC B", "s"},
	{11, 8, LT_U4, "Time In LFT_OT RSOC C", "s"},
	{11, 12, LT_U4, "Time In LFT_OT RSOC D", "s"},
	{11, 16, LT_U4, "Time In LFT_OT RSOC E", "s"},
	{11, 20, LT_U4, "Time In LFT_OT RSOC F", "s"},
	{11, 24, LT_U4, "Time In LFT_OT RSOC G", "s"},
	{11, 28, LT_U4, "Time In LFT_OT RSOC H", "s"},

	/*
	 * Blocks 13–15 (0x006C–0x006E): peak temperatures per operating mode.
	 * Each field is a signed 8-bit value in °C (0.1 K × 10 − 273 ≈ °C in the
	 * gauge's raw output, but stored directly as integer °C here).
	 * Offsets 7–9, 11–13 are RSVD and skipped.
	 */
	{12, 0, LT_I1, "Max Temp Cell RELAX", "C"},
	{12, 1, LT_I1, "Min Temp Cell RELAX", "C"},
	{12, 2, LT_I1, "Max Delta Cell Temp RELAX", "C"},
	{12, 3, LT_I1, "Max Temp Int Sensor RELAX", "C"},
	{12, 4, LT_I1, "Min Temp Int Sensor RELAX", "C"},
	{12, 5, LT_I1, "Max Temp FET RELAX", "C"},
	{12, 6, LT_I1, "Max Temp TS1 RELAX", "C"},
	{12, 10, LT_I1, "Min Temp TS1 RELAX", "C"},

	{13, 0, LT_I1, "Max Temp Cell CHG", "C"},
	{13, 1, LT_I1, "Min Temp Cell CHG", "C"},
	{13, 2, LT_I1, "Max Delta Cell Temp CHG", "C"},
	{13, 3, LT_I1, "Max Temp Int Sensor CHG", "C"},
	{13, 4, LT_I1, "Min Temp Int Sensor CHG", "C"},
	{13, 5, LT_I1, "Max Temp FET CHG", "C"},
	{13, 6, LT_I1, "Max Temp TS1 CHG", "C"},
	{13, 10, LT_I1, "Min Temp TS1 CHG", "C"},

	{14, 0, LT_I1, "Max Temp Cell DSG", "C"},
	{14, 1, LT_I1, "Min Temp Cell DSG", "C"},
	{14, 2, LT_I1, "Max Delta Cell Temp DSG", "C"},
	{14, 3, LT_I1, "Max Temp Int Sensor DSG", "C"},
	{14, 4, LT_I1, "Min Temp Int Sensor DSG", "C"},
	{14, 5, LT_I1, "Max Temp FET DSG", "C"},
	{14, 6, LT_I1, "Max Temp TS1 DSG", "C"},
	{14, 10, LT_I1, "Min Temp TS1 DSG", "C"},

	/* Block 16 (0x007E — non-sequential): ultra-under-temperature RSOC histogram */
	{15, 0, LT_U4, "Time In LFT_UUT RSOC A", "s"},
	{15, 4, LT_U4, "Time In LFT_UUT RSOC B", "s"},
	{15, 8, LT_U4, "Time In LFT_UUT RSOC C", "s"},
	{15, 12, LT_U4, "Time In LFT_UUT RSOC D", "s"},
	{15, 16, LT_U4, "Time In LFT_UUT RSOC E", "s"},
	{15, 20, LT_U4, "Time In LFT_UUT RSOC F", "s"},
	{15, 24, LT_U4, "Time In LFT_UUT RSOC G", "s"},
	{15, 28, LT_U4, "Time In LFT_UUT RSOC H", "s"},
};

static int cmd_bq27z8xx_lifetime(const struct shell *sh, size_t argc, char **argv)
{
	(void)argc;

	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	const bool is_z855 = bq27z8xx_shell_is_z855(dev);
	const int block_count =
		is_z855 ? BQ27Z855_LIFETIME_BLOCK_COUNT : BQ27Z758_LIFETIME_BLOCK_COUNT;
	const struct lifetime_field *fields = is_z855 ? lifetime_fields_z855 : lifetime_fields_z758;
	const size_t field_count =
		is_z855 ? ARRAY_SIZE(lifetime_fields_z855) : ARRAY_SIZE(lifetime_fields_z758);

	/*
	 * Read and decode one block at a time.  Keeping only one 32-byte block
	 * on the stack avoids the 512-byte lifetime_raw[16][32] array that
	 * previously overflowed the 1024-byte default ztest thread stack on ARM.
	 */
	for (int block = 0; block < block_count; block++) {
		uint16_t cmd;

		if (is_z855) {
			cmd = z855_lifetime_macs[block];
		} else {
			cmd = (uint16_t)(BQ27Z758_MAC_CMD_LIFETIMEDATABLOCK1 + block);
		}

		uint8_t tmp[MAC_DATA_LEN] = {0};
		uint8_t reported_len = 0;
		int ret = shell_read_mac(sh, dev, cmd, "LifetimeData", tmp, &reported_len);

		if (ret != 0) {
			shell_error(sh, "Error reading lifetime block %d (cmd 0x%04x): %d",
				    block + 1, cmd, ret);
			return ret;
		}

		for (size_t i = 0; i < field_count; i++) {
			const struct lifetime_field *f = &fields[i];

			if (f->block != (uint8_t)block) {
				continue;
			}

			size_t need = 0;

			switch (f->type) {
			case LT_I1:
			case LT_U1:
				need = 1;
				break;
			case LT_I2:
			case LT_U2:
				need = 2;
				break;
			case LT_U4:
				need = 4;
				break;
			}

			if ((size_t)f->offset + need > MAC_DATA_LEN) {
				shell_error(sh,
					    "Lifetime field out of range: block %u offset 0x%02x",
					    f->block, f->offset);
				continue;
			}

			const uint8_t *raw = &tmp[f->offset];

			switch (f->type) {
			case LT_I1:
				shell_print(sh, "  %s: %" PRIi8 " %s", f->label, (int8_t)raw[0],
					    f->unit);
				break;
			case LT_U1:
				shell_print(sh, "  %s: %" PRIu8 " %s", f->label, raw[0], f->unit);
				break;
			case LT_I2:
				shell_print(sh, "  %s: %" PRIi16 " %s", f->label,
					    (int16_t)sys_get_le16(raw), f->unit);
				break;
			case LT_U2:
				shell_print(sh, "  %s: %" PRIu16 " %s", f->label, sys_get_le16(raw),
					    f->unit);
				break;
			case LT_U4: {
				const uint32_t v = sys_get_le32(raw);

				shell_print(sh, "  %s: %" PRIu32 " %s", f->label, v, f->unit);
				break;
			}
			}
		}
	}

	return 0;
}

static int cmd_bq27z8xx_serial(const struct shell *sh, size_t argc, char **argv)
{
	(void)argc;

	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	/* Try MAC 0x004E (Serial Number) — BQ27Z758 only; BQ27Z855 does not
	 * implement this command and would echo back a stale response.
	 */
	uint8_t data[MAC_DATA_LEN] = {0};
	uint8_t reported_len = 0;
	int ret = -ENOTSUP;

	if (!bq27z8xx_shell_is_z855(dev)) {
		ret = shell_read_mac(sh, dev, BQ27Z8XX_SHELL_MAC_CMD_SERIAL_NUMBER, "SerialNumber",
				     data, &reported_len);
	}

	if (ret == 0 && reported_len > 0) {
		char serial[MAC_DATA_LEN + 1] = {0};
		bool replaced = false;
		bool has_printable = false;
		const size_t out_len = copy_ascii(serial, sizeof(serial), data, reported_len,
						  &replaced, &has_printable);

		if (out_len > 0) {
			shell_print(sh, "Serial (MAC 0x004E): %s", serial);
			if (replaced) {
				shell_warn(sh,
					   "Serial contains non-printable bytes (shown as '.')");
			}
			return 0;
		}
	}

	/* Fallback: ManufacturerInfo (MAC 0x0070), first 16 bytes. */
	memset(data, 0, sizeof(data));
	reported_len = 0;
	ret = shell_read_mac(sh, dev, BQ27Z8XX_SHELL_MAC_CMD_MANUFACTURERINFO, "ManufacturerInfo",
			     data, &reported_len);
	if (ret != 0) {
		shell_error(sh, "Error reading ManufacturerInfo: %d", ret);
		return ret;
	}

	char serial[17] = {0};
	bool replaced = false;
	bool has_printable = false;
	const size_t out_len =
		copy_ascii(serial, sizeof(serial), data, MIN((size_t)reported_len, (size_t)16),
			   &replaced, &has_printable);

	if (out_len == 0) {
		shell_print(sh, "Serial: <empty>");
		return 0;
	}

	shell_print(sh, "Serial (ManufacturerInfo): %s", serial);
	if (replaced) {
		shell_warn(sh, "Serial contains non-printable bytes (shown as '.')");
	}

	return 0;
}

/* ---------------------------------------------------------------------------
 * Ra table
 * ---------------------------------------------------------------------------
 */

#define BQ27Z8XX_DF_RA_CELL0  0x4100
#define BQ27Z8XX_DF_RA_XCELL0 0x4140
#define BQ27Z8XX_RA_VALUES    15

static void ra_table_print_block(const struct shell *sh, const char *prefix,
				 const uint8_t data[MAC_DATA_LEN], bool tsv)
{
	const uint16_t flag = sys_get_le16(&data[0]);

	if (tsv) {
		shell_print(sh, "%s R_a flag\t0x%04x\t", prefix, flag);
	} else {
		shell_print(sh, "  %s R_a flag: 0x%04x", prefix, flag);
	}

	for (int i = 0; i < BQ27Z8XX_RA_VALUES; i++) {
		const uint16_t v = sys_get_le16(&data[2 + (i * 2)]);
		const uint32_t m_ohm = (uint32_t)(v * 1000U + 512U) / 1024U;

		if (tsv) {
			shell_print(sh, "%s R_a %d\t%u\t%u", prefix, i, v, m_ohm);
		} else {
			shell_print(sh, "  %s R_a %d: %u (%u mOhm)", prefix, i, v, m_ohm);
		}
	}
}

static int cmd_bq27z8xx_ra_table(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	const bool tsv = (argc > 2) && (strcmp(argv[2], "tsv") == 0);

	uint8_t cell0[MAC_DATA_LEN] = {0};
	uint8_t xcell0[MAC_DATA_LEN] = {0};
	uint8_t reported_len = 0;

	int ret = shell_read_df(sh, dev, BQ27Z8XX_DF_RA_CELL0, cell0, &reported_len);

	if (ret != 0) {
		shell_error(sh, "Error reading Ra table 0x%04x: %d", BQ27Z8XX_DF_RA_CELL0, ret);
		return ret;
	}

	ret = shell_read_df(sh, dev, BQ27Z8XX_DF_RA_XCELL0, xcell0, &reported_len);
	if (ret != 0) {
		shell_error(sh, "Error reading Ra table 0x%04x: %d", BQ27Z8XX_DF_RA_XCELL0, ret);
		return ret;
	}

	shell_print(sh, "Ra Table (Data Flash)");
	if (tsv) {
		shell_print(sh, "Label\tRaw\tmOhm");
	}

	ra_table_print_block(sh, "Cell0", cell0, tsv);
	ra_table_print_block(sh, "xCell0", xcell0, tsv);

	return 0;
}

static int cmd_bq27z8xx_read(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	int err = 0;
	const uint16_t start = (uint16_t)shell_strtoul(argv[2], 16, &err);

	if (err < 0) {
		shell_error(sh, "Invalid start address: %s", argv[2]);
		return err;
	}

	uint32_t blocks = 1;

	if (argc > 3) {
		blocks = shell_strtoul(argv[3], 0, &err);
		if (err < 0 || blocks == 0) {
			shell_error(sh, "Invalid block count: %s", argv[3]);
			return err < 0 ? err : -EINVAL;
		}
		if (blocks > 16) {
			shell_warn(sh, "Clamping block count to 16");
			blocks = 16;
		}
	}

	for (uint32_t b = 0; b < blocks; b++) {
		const uint16_t addr = (uint16_t)(start + (b * MAC_DATA_LEN));
		const bool is_df = (addr >= BQ27Z8XX_DF_ADDR_MIN && addr <= BQ27Z8XX_DF_ADDR_MAX);
		const char *label = is_df ? "DF" : "MAC";
		uint8_t data[MAC_DATA_LEN] = {0};
		uint8_t reported_len = 0;
		int ret = is_df ? shell_read_df(sh, dev, addr, data, &reported_len)
				: shell_read_mac(sh, dev, addr, label, data, &reported_len);

		if (ret != 0) {
			shell_error(sh, "Error reading %s 0x%04x: %d", label, addr, ret);
			return ret;
		}

		if (reported_len == 0) {
			shell_print(sh, "%s 0x%04x: <empty>", label, addr);
			continue;
		}

		size_t display_len = MIN((size_t)reported_len, (size_t)MAC_DATA_LEN);

		shell_print(sh, "%s 0x%04x (len=%u):", label, addr, reported_len);
		for (size_t off = 0; off < display_len; off += 16) {
			const size_t chunk_len = MIN((size_t)16, display_len - off);
			char hex[3 * 16 + 1];
			char ascii[16 + 1];

			for (size_t i = 0; i < 16; i++) {
				if (i < chunk_len) {
					const uint8_t v = data[off + i];

					snprintk(&hex[i * 3], sizeof(hex) - (i * 3), "%02x ", v);
					ascii[i] = printable_byte(v);
				} else {
					hex[i * 3] = ' ';
					hex[i * 3 + 1] = ' ';
					hex[i * 3 + 2] = ' ';
					ascii[i] = ' ';
				}
			}
			hex[3 * 16] = '\0';
			ascii[16] = '\0';
			shell_print(sh, "  %04x: %s|%s|", (unsigned int)(addr + off), hex, ascii);
		}
	}

	return 0;
}

/* ---------------------------------------------------------------------------
 * Raw register access
 * ---------------------------------------------------------------------------
 */

/*
 * Parse an argv array of hex-string tokens into a uint8_t array.
 * Returns the byte count on success or negative errno on failure.
 */
static int parse_hex_bytes(const struct shell *sh, char **argv, int argc, uint8_t *out, int max_len)
{
	for (int i = 0; i < argc; i++) {
		int err = 0;
		unsigned long v = shell_strtoul(argv[i], 16, &err);

		if (err || v > 0xFF) {
			shell_error(sh, "Invalid hex byte: %s", argv[i]);
			return -EINVAL;
		}
		if (i >= max_len) {
			shell_error(sh, "Too many bytes (max %d)", max_len);
			return -EINVAL;
		}
		out[i] = (uint8_t)v;
	}
	return argc;
}

static int cmd_bq27z8xx_reg_read(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	int err = 0;
	unsigned long reg_ul = shell_strtoul(argv[2], 16, &err);

	if (err || reg_ul > 0xFF) {
		shell_error(sh, "Invalid register: %s", argv[2]);
		return -EINVAL;
	}

	uint8_t reg = (uint8_t)reg_ul;
	int len = 2;
	uint16_t addr = 0;

	/* Check for optional address and length arguments */
	int arg_idx = 3;
	if (argc > arg_idx) {
		unsigned long addr_ul = shell_strtoul(argv[arg_idx], 16, &err);
		if (!err && addr_ul <= 0x7F) {
			/* Looks like an address, consume it */
			addr = (uint16_t)addr_ul;
			arg_idx++;
		}
	}
	if (argc > arg_idx) {
		len = (int)shell_strtoul(argv[arg_idx], 0, &err);
		if (err || len < 1 || len > MAC_DATA_LEN) {
			shell_error(sh, "Invalid byte count: %s (must be 1-%d)", argv[arg_idx],
				    MAC_DATA_LEN);
			return -EINVAL;
		}
	}

	const struct bq27z8xx_config *cfg = dev->config;
	struct i2c_dt_spec i2c_spec;

	if (addr != 0) {
		/* Use custom address */
		i2c_spec = cfg->i2c;
		i2c_spec.addr = addr;
	} else {
		/* Use device's address */
		i2c_spec = cfg->i2c;
	}

	uint8_t buf[MAC_DATA_LEN];
	int ret = i2c_burst_read_dt(&i2c_spec, reg, buf, len);

	if (ret != 0) {
		shell_error(sh, "REG read failed: %d", ret);
		return ret;
	}

	char hex[MAC_DATA_LEN * 3 + 1];
	size_t pos = 0;

	for (int i = 0; i < len; i++) {
		pos += snprintk(&hex[pos], sizeof(hex) - pos, "%02x ", buf[i]);
	}
	if (pos > 0) {
		hex[pos - 1] = '\0';
	} else {
		hex[0] = '\0';
	}

	shell_print(sh, "REG 0x%02x: %s", reg, hex);
	return 0;
}

static int cmd_bq27z8xx_reg_write(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	int err = 0;
	unsigned long reg_ul = shell_strtoul(argv[2], 16, &err);

	if (err || reg_ul > 0xFF) {
		shell_error(sh, "Invalid register: %s", argv[2]);
		return -EINVAL;
	}

	uint8_t reg = (uint8_t)reg_ul;
	uint16_t addr = 0;

	/* Check for optional address argument */
	int data_start_idx = 3;
	if (argc > data_start_idx) {
		unsigned long addr_ul = shell_strtoul(argv[data_start_idx], 16, &err);
		if (!err && addr_ul <= 0x7F) {
			/* Looks like an address, consume it */
			addr = (uint16_t)addr_ul;
			data_start_idx++;
		}
	}

	int data_len = (int)argc - data_start_idx;

	if (data_len < 1 || data_len > MAC_DATA_LEN) {
		shell_error(sh, "Need 1-%d data bytes", MAC_DATA_LEN);
		return -EINVAL;
	}

	uint8_t data[MAC_DATA_LEN];
	int n = parse_hex_bytes(sh, &argv[data_start_idx], data_len, data, MAC_DATA_LEN);

	if (n < 0) {
		return n;
	}

	uint8_t buf[1 + MAC_DATA_LEN];

	buf[0] = reg;
	memcpy(&buf[1], data, data_len);

	const struct bq27z8xx_config *cfg = dev->config;
	struct i2c_dt_spec i2c_spec;

	if (addr != 0) {
		/* Use custom address */
		i2c_spec = cfg->i2c;
		i2c_spec.addr = addr;
	} else {
		/* Use device's address */
		i2c_spec = cfg->i2c;
	}

	int ret = i2c_write_dt(&i2c_spec, buf, 1 + data_len);

	if (ret != 0) {
		shell_error(sh, "REG write failed: %d", ret);
		return ret;
	}

	shell_print(sh, "Wrote %d byte(s) to REG 0x%02x.", data_len, reg);
	return 0;
}

static int cmd_bq27z8xx_mac_write(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	int err = 0;
	unsigned long cmd_ul = shell_strtoul(argv[2], 16, &err);

	if (err || cmd_ul > 0xFFFF) {
		shell_error(sh, "Invalid MAC command: %s", argv[2]);
		return -EINVAL;
	}

	uint16_t cmd = (uint16_t)cmd_ul;
	int data_len = (int)argc - 3;

	if (data_len > MAC_DATA_LEN) {
		shell_error(sh, "Too many data bytes (max %d)", MAC_DATA_LEN);
		return -EINVAL;
	}

	uint8_t data[MAC_DATA_LEN];
	int n = 0;

	if (data_len > 0) {
		n = parse_hex_bytes(sh, &argv[3], data_len, data, MAC_DATA_LEN);
		if (n < 0) {
			return n;
		}
	}

	int ret = bq27z8xx_write_mac(dev, cmd, data_len > 0 ? data : NULL, (size_t)data_len);

	if (ret != 0) {
		shell_error(sh, "MAC write failed: %d", ret);
		return ret;
	}

	shell_print(sh, "MAC write 0x%04x sent (%u data byte(s)).", cmd, (unsigned int)data_len);
	return 0;
}

/* ---------------------------------------------------------------------------
 * Security key operations
 * ---------------------------------------------------------------------------
 */

static int cmd_bq27z8xx_seal(const struct shell *sh, size_t argc, char **argv)
{
	(void)argc;

	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	int ret = bq27z8xx_write16(dev, BQ27Z8XX_ALTMANUFACTURERACCESS,
				   BQ27Z8XX_MAC_CTRL_CMD_SEALDEVICE);

	if (ret != 0) {
		shell_error(sh, "Failed to send seal command: %d", ret);
		return ret;
	}

	shell_print(sh, "Seal command sent.");
	return 0;
}

static int cmd_bq27z8xx_unseal(const struct shell *sh, size_t argc, char **argv)
{
	(void)argc;

	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	uint16_t key1 = DEFAULT_UNSEAL_KEY1;
	uint16_t key2 = DEFAULT_UNSEAL_KEY2;

	if (argc == 2) {
		shell_print(sh, "Using default unseal keys: 0x%04x 0x%04x", key1, key2);
	} else if (argc == 4) {
		int err = 0;

		key1 = (uint16_t)shell_strtoul(argv[2], 16, &err);
		if (err < 0) {
			shell_error(sh, "Invalid key1: %s", argv[2]);
			return err;
		}
		key2 = (uint16_t)shell_strtoul(argv[3], 16, &err);
		if (err < 0) {
			shell_error(sh, "Invalid key2: %s", argv[3]);
			return err;
		}
	} else {
		shell_error(sh, "Expected no keys or two keys.");
		return -EINVAL;
	}

	/*
	 * Per TRM: write both key words consecutively to AltManufacturerAccess
	 * with no other writes in between.
	 */
	int ret = bq27z8xx_write16(dev, BQ27Z8XX_ALTMANUFACTURERACCESS, key1);

	if (ret != 0) {
		shell_error(sh, "Failed to write unseal key1: %d", ret);
		return ret;
	}

	ret = bq27z8xx_write16(dev, BQ27Z8XX_ALTMANUFACTURERACCESS, key2);
	if (ret != 0) {
		shell_error(sh, "Failed to write unseal key2: %d", ret);
		return ret;
	}

	shell_print(sh, "Unseal keys sent.");
	return 0;
}

static int cmd_bq27z8xx_full_access(const struct shell *sh, size_t argc, char **argv)
{
	(void)argc;

	const struct device *dev = shell_device_get_binding(argv[1]);

	if (!dev) {
		shell_error(sh, "Device not found: %s", argv[1]);
		return -ENODEV;
	}

	uint16_t key1 = DEFAULT_FULL_ACCESS_KEY1;
	uint16_t key2 = DEFAULT_FULL_ACCESS_KEY2;

	if (argc == 2) {
		shell_print(sh, "Using default full-access keys: 0x%04x 0x%04x", key1, key2);
	} else if (argc == 4) {
		int err = 0;

		key1 = (uint16_t)shell_strtoul(argv[2], 16, &err);
		if (err < 0) {
			shell_error(sh, "Invalid key1: %s", argv[2]);
			return err;
		}
		key2 = (uint16_t)shell_strtoul(argv[3], 16, &err);
		if (err < 0) {
			shell_error(sh, "Invalid key2: %s", argv[3]);
			return err;
		}
	} else {
		shell_error(sh, "Expected no keys or two keys.");
		return -EINVAL;
	}

	int ret = bq27z8xx_write16(dev, BQ27Z8XX_ALTMANUFACTURERACCESS, key1);

	if (ret != 0) {
		shell_error(sh, "Failed to write full-access key1: %d", ret);
		return ret;
	}

	ret = bq27z8xx_write16(dev, BQ27Z8XX_ALTMANUFACTURERACCESS, key2);
	if (ret != 0) {
		shell_error(sh, "Failed to write full-access key2: %d", ret);
		return ret;
	}

	shell_print(sh, "Full-access keys sent.");
	return 0;
}

/* ---------------------------------------------------------------------------
 * Shell command registration
 * ---------------------------------------------------------------------------
 */

/* clang-format off */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_bq27z8xx,
	SHELL_CMD_ARG(status, &dsub_pos_bq27z8xx_device_name,
		      "<device_name>", cmd_bq27z8xx_status, 2, 0),
	SHELL_CMD_ARG(security, &dsub_pos_bq27z8xx_device_name,
		      "<device_name>", cmd_bq27z8xx_security, 2, 0),
	SHELL_CMD_ARG(lifetime, &dsub_pos_bq27z8xx_device_name,
		      "<device_name>", cmd_bq27z8xx_lifetime, 2, 0),
	SHELL_CMD_ARG(serial, &dsub_pos_bq27z8xx_device_name,
		      "<device_name>", cmd_bq27z8xx_serial, 2, 0),
	SHELL_CMD_ARG(ra_table, &dsub_pos_bq27z8xx_device_name,
		      "<device_name> [tsv]", cmd_bq27z8xx_ra_table, 2, 1),
	SHELL_CMD_ARG(reg_read, &dsub_pos_bq27z8xx_device_name,
		      "<device_name> <reg_hex> [addr_7bit] [bytes]", cmd_bq27z8xx_reg_read, 3, MAC_DATA_LEN + 2),
	SHELL_CMD_ARG(reg_write, &dsub_pos_bq27z8xx_device_name,
		      "<device_name> <reg_hex> [addr_7bit] <byte_hex> [byte_hex ...]",
		      cmd_bq27z8xx_reg_write, 4, MAC_DATA_LEN + 2),
	SHELL_CMD_ARG(mac_write, &dsub_pos_bq27z8xx_device_name,
		      "<device_name> <cmd_hex> [byte_hex ...]",
		      cmd_bq27z8xx_mac_write, 3, MAC_DATA_LEN),
	SHELL_CMD_ARG(read, &dsub_pos_bq27z8xx_device_name,
		      "<device_name> <start_hex> [blocks]", cmd_bq27z8xx_read, 3, 1),
	SHELL_CMD_ARG(unseal, &dsub_pos_bq27z8xx_device_name,
		      "<device_name> [key1_hex] [key2_hex]", cmd_bq27z8xx_unseal, 2, 2),
	SHELL_CMD_ARG(full_access, &dsub_pos_bq27z8xx_device_name,
		      "<device_name> [key1_hex] [key2_hex]", cmd_bq27z8xx_full_access, 2, 2),
	SHELL_CMD_ARG(seal, &dsub_pos_bq27z8xx_device_name,
		      "<device_name>", cmd_bq27z8xx_seal, 2, 0),
	SHELL_SUBCMD_SET_END
);
/* clang-format on */

SHELL_CMD_REGISTER(bq27z8xx, &sub_bq27z8xx, "BQ27Z8XX fuel gauge commands", NULL);
