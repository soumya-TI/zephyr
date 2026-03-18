/*
 * Copyright (c) 2023, ithinx GmbH
 * Copyright (c) 2023, Tonies GmbH
 * Copyright (c) 2026, Texas Instruments Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Emulator for the TI BQ27Z758 and BQ27Z855 fuel gauges.
 *
 * Uses two DT_DRV_COMPAT passes so that the AltMAC response builder is
 * selected at compile time based on which compatible is used in the overlay,
 * with no runtime device_type_idx lookup.
 */

#include <string.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(EMUL_BQ27Z8XX);

#include <zephyr/device.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2c_emul.h>
#include <zephyr/sys/byteorder.h>

#include "bq27z758.h"
#include "bq27z855.h"

#define BQ27Z8XX_EMUL_MAC_DATA_LEN     32
#define BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN 4 /* 2 cmd bytes, 1 checksum byte, 1 length byte */
#define BQ27Z8XX_EMUL_MAC_COMPLETE_LEN (BQ27Z8XX_EMUL_MAC_DATA_LEN + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN)

/* ---------------------------------------------------------------------------
 * Shared data / config structs
 * ---------------------------------------------------------------------------
 */

typedef int (*read_altmac_fn_t)(const struct emul *target, uint8_t *buf, size_t len);

struct bq27z8xx_emul_data {
	uint16_t mac_cmd;

	/* Single-slot DF write-back cache for read-after-write verification */
	uint16_t df_written_addr;
	uint8_t df_written_data[BQ27Z8XX_EMUL_MAC_DATA_LEN];
	uint8_t df_written_len;
	bool df_has_write;

	/* Auto-increment address for df_read_next() — set after each DF read */
	uint16_t df_next_addr;
};

struct bq27z8xx_emul_cfg {
	uint16_t addr;
	read_altmac_fn_t read_altmac; /* set at compile time per compat */
};

/* ---------------------------------------------------------------------------
 * Common register read
 *
 * All BQ27Z8XX_* register addresses and their reset values are identical on
 * both devices.  Signed registers (ATRATE, CURRENT, AVERAGECURRENT) return -2
 * so that test code can verify sign extension is handled correctly.
 * ---------------------------------------------------------------------------
 */

static int emul_bq27z8xx_reg_read(const struct emul *target, int reg, int *val)
{
	switch (reg) {
	case BQ27Z8XX_MANUFACTURERACCESS:
		*val = 1;
		break;
	case BQ27Z8XX_ATRATE:
		*val = -2;
		break;
	case BQ27Z8XX_ATRATETIMETOEMPTY:
		*val = 1;
		break;
	case BQ27Z8XX_TEMPERATURE:
		*val = 1;
		break;
	case BQ27Z8XX_VOLTAGE:
		*val = 1;
		break;
	case BQ27Z8XX_BATTERYSTATUS:
		*val = 1;
		break;
	case BQ27Z8XX_CURRENT:
		*val = -2;
		break;
	case BQ27Z8XX_REMAININGCAPACITY:
		*val = 1;
		break;
	case BQ27Z8XX_FULLCHARGECAPACITY:
		*val = 1;
		break;
	case BQ27Z8XX_AVERAGECURRENT:
		*val = -2;
		break;
	case BQ27Z8XX_AVERAGETIMETOEMPTY:
		*val = 1;
		break;
	case BQ27Z8XX_AVERAGETIMETOFULL:
		*val = 1;
		break;
	case BQ27Z8XX_MAXLOADCURRENT:
		*val = 1;
		break;
	case BQ27Z8XX_MAXLOADTIMETOEMPTY:
		*val = 1;
		break;
	case BQ27Z8XX_AVERAGEPOWER:
		*val = 1;
		break;
	case BQ27Z8XX_BTPDISCHARGESET:
		*val = 1;
		break;
	case BQ27Z8XX_BTPCHARGESET:
		*val = 1;
		break;
	case BQ27Z8XX_INTERNALTEMPERATURE:
		*val = 1;
		break;
	case BQ27Z8XX_CYCLECOUNT:
		*val = 1;
		break;
	case BQ27Z8XX_RELATIVESTATEOFCHARGE:
		*val = 1;
		break;
	case BQ27Z8XX_STATEOFHEALTH:
		*val = 1;
		break;
	case BQ27Z8XX_CHARGINGVOLTAGE:
		*val = 1;
		break;
	case BQ27Z8XX_CHARGINGCURRENT:
		*val = 1;
		break;
	case BQ27Z8XX_TERMINATEVOLTAGE:
		*val = 1;
		break;
	case BQ27Z8XX_TIMESTAMPUPPER:
		*val = 1;
		break;
	case BQ27Z8XX_TIMESTAMPLOWER:
		*val = 1;
		break;
	case BQ27Z8XX_QMAXCYCLES:
		*val = 1;
		break;
	case BQ27Z8XX_DESIGNCAPACITY:
		*val = 1;
		break;
	case BQ27Z8XX_ALTMANUFACTURERACCESS:
		*val = 1;
		break;
	case BQ27Z8XX_MACDATA:
		*val = 1;
		break;
	case BQ27Z8XX_MACDATASUM:
		*val = 1;
		break;
	case BQ27Z8XX_MACDATALEN:
		*val = 1;
		break;
	case BQ27Z8XX_VOLTHISETTHRESHOLD:
		*val = 1;
		break;
	case BQ27Z8XX_VOLTHICLEARTHRESHOLD:
		*val = 1;
		break;
	case BQ27Z8XX_VOLTLOSETTHRESHOLD:
		*val = 1;
		break;
	case BQ27Z8XX_VOLTLOCLEARTHRESHOLD:
		*val = 1;
		break;
	case BQ27Z8XX_TEMPHISETTHRESHOLD:
		*val = 1;
		break;
	case BQ27Z8XX_TEMPHICLEARTHRESHOLD:
		*val = 1;
		break;
	case BQ27Z8XX_TEMPLOSETTHRESHOLD:
		*val = 1;
		break;
	case BQ27Z8XX_TEMPLOCLEARTHRESHOLD:
		*val = 1;
		break;
	case BQ27Z8XX_INTERRUPTSTATUS:
		*val = 1;
		break;
	case BQ27Z8XX_SOCDELTASETTHRESHOLD:
		*val = 1;
		break;
	default:
		LOG_ERR("Unknown register 0x%x read", reg);
		return -EIO;
	}
	LOG_INF("read 0x%x = 0x%x", reg, *val);

	return 0;
}

/* ---------------------------------------------------------------------------
 * Common write handler
 *
 * A 3-byte write to ALTMANUFACTURERACCESS (0x3E) latches the pending MAC
 * command for a subsequent 36-byte regular MAC read.
 *
 * A write of more than 3 bytes to 0x3E is a DF block write:
 *   [reg=0x3E, addr_lo, addr_hi, data_byte_0, ..., data_byte_N-1]
 * The data is committed to the single-slot write-back cache immediately.
 *
 * All other writes (threshold registers, etc.) are silently accepted.
 * ---------------------------------------------------------------------------
 */

static int emul_bq27z8xx_write(const struct emul *target, uint8_t *buf, size_t len)
{
	struct bq27z8xx_emul_data *data = target->data;
	const uint8_t reg = buf[0];

	if (reg == BQ27Z8XX_ALTMANUFACTURERACCESS && len >= 3) {
		data->mac_cmd = sys_get_le16(&buf[1]);

		/* DF block write: [0x3E, addr_lo, addr_hi, data...] */
		if (len > 3 && data->mac_cmd >= BQ27Z8XX_DF_ADDR_MIN &&
		    data->mac_cmd <= BQ27Z8XX_DF_ADDR_MAX) {
			uint8_t n =
				(uint8_t)MIN((size_t)(len - 3), (size_t)BQ27Z8XX_EMUL_MAC_DATA_LEN);

			data->df_written_addr = data->mac_cmd;
			memcpy(data->df_written_data, &buf[3], n);
			data->df_written_len = n;
			data->df_has_write = true;
		}
	}

	return 0;
}

/* ---------------------------------------------------------------------------
 * MAC checksum finalizer
 *
 * Called after buf[0..33] and buf[35] are fully populated.  Computes the
 * one's-complement checksum over bytes 0..33 and stores it in buf[34].
 * ---------------------------------------------------------------------------
 */

static void emul_mac_finalize(uint8_t *buf)
{
	uint8_t sum = 0; /* intentionally 8-bit and wrapping */

	for (int i = 0; i < BQ27Z8XX_EMUL_MAC_COMPLETE_LEN - 2; i++) {
		sum += buf[i];
	}
	buf[34] = 0xFF - sum;
}

/* 34-byte DF read response: 2-byte address echo + 32 bytes of DF data */
#define BQ27Z8XX_EMUL_DF_READ_LEN (2u + BQ27Z8XX_EMUL_MAC_DATA_LEN)

/* ---------------------------------------------------------------------------
 * bq27z758 AltMAC response builder
 *
 * Handles all MAC commands used by bq27z758_variant_get_buffer_prop() and
 * bq27z8xx_get_buffer_prop().  Status registers return all zeros (clean/idle
 * state).  String properties return their real values so that test code can
 * verify the MAC string path end-to-end.
 * ---------------------------------------------------------------------------
 */
#ifdef CONFIG_BQ27Z758
static int emul_bq27z758_read_altmac(const struct emul *target, uint8_t *buf, size_t len)
{
	static const uint8_t manufacturer_name[] = "Texas Instruments";
	static const uint8_t device_name[] = "BQ27Z758";
	static const uint8_t device_chemistry[] = "LION";
	const struct bq27z8xx_emul_data *data = target->data;

	if (len < BQ27Z8XX_EMUL_MAC_COMPLETE_LEN) {
		LOG_ERR("MAC read requires %u bytes, got %zu", BQ27Z8XX_EMUL_MAC_COMPLETE_LEN, len);
		return -EIO;
	}

	memset(buf, 0, len);
	sys_put_le16(data->mac_cmd, buf);

	switch (data->mac_cmd) {

	/* Common MAC read commands */
	case BQ27Z8XX_MAC_CMD_DEVICETYPE:
		sys_put_le16(BQ27Z8XX_DEVICE_TYPE_BQ27Z758, &buf[2]);
		buf[35] = sizeof(struct bq27z8xx_device_type) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_FIRMWAREVERSION:
		buf[35] = sizeof(struct bq27z8xx_firmware_version) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_HARDWAREVERSION:
		buf[35] = sizeof(struct bq27z8xx_hardware_version) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_IFSIGNATURE:
		buf[35] = sizeof(struct bq27z8xx_if_signature) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_STATICDFSIGNATURE:
		buf[35] = sizeof(struct bq27z8xx_static_df_signature) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_CHEMID:
		buf[35] = sizeof(struct bq27z8xx_chem_id) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_STATICCHEMDFSIGNATURE:
		buf[35] = sizeof(struct bq27z8xx_static_chem_df_signature) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_ALLDFSIGNATURE:
		buf[35] = sizeof(struct bq27z8xx_all_df_signature) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_SECURITYKEYS:
		buf[35] =
			sizeof(struct bq27z8xx_security_keys) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	/* bq27z758 SBS string properties via MAC */
	case BQ27Z758_MAC_CMD_MANUFACTURER_NAME:
		memcpy(&buf[2], manufacturer_name, sizeof(manufacturer_name) - 1);
		buf[35] = sizeof(manufacturer_name) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z758_MAC_CMD_DEVICE_NAME:
		memcpy(&buf[2], device_name, sizeof(device_name) - 1);
		buf[35] = sizeof(device_name) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z758_MAC_CMD_DEVICE_CHEM:
		memcpy(&buf[2], device_chemistry, sizeof(device_chemistry) - 1);
		buf[35] = sizeof(device_chemistry) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	/* bq27z758-specific status register MAC commands (all zeros = clean/idle state) */
	case BQ27Z758_MAC_CMD_SAFETYALERT:
		buf[35] = sizeof(struct bq27z758_safety_alert) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z758_MAC_CMD_SAFETYSTATUS:
		buf[35] =
			sizeof(struct bq27z758_safety_status) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z758_MAC_CMD_PFALERT:
		buf[35] = sizeof(struct bq27z758_pf_alert) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z758_MAC_CMD_PFSTATUS:
		buf[35] = sizeof(struct bq27z758_pf_status) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z758_MAC_CMD_OPERATIONSTATUS:
		buf[35] = sizeof(struct bq27z758_operation_status) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z758_MAC_CMD_CHARGINGSTATUS:
		buf[35] = sizeof(struct bq27z758_charging_status) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z758_MAC_CMD_GAUGINGSTATUS:
		buf[35] =
			sizeof(struct bq27z758_gauging_status) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z758_MAC_CMD_MANUFACTURINGSTATUS:
		buf[35] = sizeof(struct bq27z758_manufacturing_status) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	default:
		LOG_ERR("ALTMAC command 0x%04x is not supported (bq27z758)", data->mac_cmd);
		return -EIO;
	}

	emul_mac_finalize(buf);
	return 0;
}
#endif /* CONFIG_BQ27Z758 */

/* ---------------------------------------------------------------------------
 * bq27z855 AltMAC response builder
 *
 * Handles all MAC commands used by bq27z855_variant_get_buffer_prop() and
 * bq27z8xx_get_buffer_prop().  All status registers return zeros.
 * ---------------------------------------------------------------------------
 */
#ifdef CONFIG_BQ27Z855
static int emul_bq27z855_read_altmac(const struct emul *target, uint8_t *buf, size_t len)
{
	const struct bq27z8xx_emul_data *data = target->data;

	if (len < BQ27Z8XX_EMUL_MAC_COMPLETE_LEN) {
		LOG_ERR("MAC read requires %u bytes, got %zu", BQ27Z8XX_EMUL_MAC_COMPLETE_LEN, len);
		return -EIO;
	}

	memset(buf, 0, len);
	sys_put_le16(data->mac_cmd, buf);

	switch (data->mac_cmd) {

	/* Common MAC read commands */
	case BQ27Z8XX_MAC_CMD_DEVICETYPE:
		sys_put_le16(BQ27Z8XX_DEVICE_TYPE_BQ27Z855, &buf[2]);
		buf[35] = sizeof(struct bq27z8xx_device_type) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_FIRMWAREVERSION:
		buf[35] = sizeof(struct bq27z8xx_firmware_version) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_HARDWAREVERSION:
		buf[35] = sizeof(struct bq27z8xx_hardware_version) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_IFSIGNATURE:
		buf[35] = sizeof(struct bq27z8xx_if_signature) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_STATICDFSIGNATURE:
		buf[35] = sizeof(struct bq27z8xx_static_df_signature) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_CHEMID:
		buf[35] = sizeof(struct bq27z8xx_chem_id) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_STATICCHEMDFSIGNATURE:
		buf[35] = sizeof(struct bq27z8xx_static_chem_df_signature) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_ALLDFSIGNATURE:
		buf[35] = sizeof(struct bq27z8xx_all_df_signature) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z8XX_MAC_CMD_SECURITYKEYS:
		buf[35] =
			sizeof(struct bq27z8xx_security_keys) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	/* bq27z855-specific status register MAC commands (all zeros = clean/idle state) */
	case BQ27Z855_MAC_CMD_SAFETYALERT:
		buf[35] = sizeof(struct bq27z855_safety_alert) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z855_MAC_CMD_SAFETYSTATUS:
		buf[35] =
			sizeof(struct bq27z855_safety_status) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z855_MAC_CMD_PFALERT:
		buf[35] = sizeof(struct bq27z855_pf_alert) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z855_MAC_CMD_PFSTATUS:
		buf[35] = sizeof(struct bq27z855_pf_status) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z855_MAC_CMD_OPERATIONSTATUS:
		buf[35] = sizeof(struct bq27z855_operation_status) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z855_MAC_CMD_CHARGINGSTATUS:
		buf[35] = sizeof(struct bq27z855_charging_status) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z855_MAC_CMD_GAUGINGSTATUS:
		buf[35] =
			sizeof(struct bq27z855_gauging_status) - 1 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z855_MAC_CMD_MANUFACTURINGSTATUS:
		buf[35] = sizeof(struct bq27z855_manufacturing_status) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z855_MAC_CMD_NOLOADREMCAP:
		buf[35] = sizeof(struct bq27z855_no_load_rem_cap) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z855_MAC_CMD_CHARGINGSTATUSEXT:
		buf[35] = sizeof(struct bq27z855_charging_status_ext) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z855_MAC_CMD_ACCUMULATIONCHARGETHRESHOLD:
		buf[35] = sizeof(struct bq27z855_accumulation_charge_threshold) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;
	case BQ27Z855_MAC_CMD_ACCUMULATIONDISCHARGETHRESHOLD:
		buf[35] = sizeof(struct bq27z855_accumulation_discharge_threshold) - 1 +
			  BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	/* Lifetime data blocks — 32-byte payloads each.
	 * Blocks 1–15 are at sequential addresses 0x0060–0x006E.
	 * Block 16 is at the non-sequential address 0x007E.
	 */
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK1:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK2:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK3:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK4:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK5:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK6:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK7:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK8:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK9:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK10:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK11:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK12:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK13:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK14:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK15:
	case BQ27Z855_MAC_CMD_LIFETIMEDATABLOCK16:
		buf[35] = BQ27Z8XX_EMUL_MAC_DATA_LEN + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	/* ManufacturerInfo (MAC 0x0070) — 32-byte string field */
	case BQ27Z855_MAC_CMD_MANUFACTURERINFO:
		buf[35] = BQ27Z8XX_EMUL_MAC_DATA_LEN + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	/* DAStatus1 (MAC 0x0071) — 32 bytes of voltage/current/power measurements */
	case BQ27Z855_MAC_CMD_DASTATUS1:
		buf[35] = BQ27Z8XX_EMUL_MAC_DATA_LEN + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	/* DAStatus2 (MAC 0x0072) — 20 bytes of temperature/raw ADC data */
	case BQ27Z855_MAC_CMD_DASTATUS2:
		buf[35] = 20 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	/* GaugeStatus1 (MAC 0x0073) — 20 bytes of IT gauging data */
	case BQ27Z855_MAC_CMD_GAUGESTATUS1:
		buf[35] = 20 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	/* GaugeStatus2 (MAC 0x0074) — 20 bytes of DOD/learning data */
	case BQ27Z855_MAC_CMD_GAUGESTATUS2:
		buf[35] = 20 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	/* GaugeStatus3 (MAC 0x0075) — 14 bytes of QMax data */
	case BQ27Z855_MAC_CMD_GAUGESTATUS3:
		buf[35] = 14 + BQ27Z8XX_EMUL_MAC_OVERHEAD_LEN;
		break;

	default:
		LOG_ERR("ALTMAC command 0x%04x is not supported (bq27z855)", data->mac_cmd);
		return -EIO;
	}

	emul_mac_finalize(buf);
	return 0;
}
#endif /* CONFIG_BQ27Z855 */

/* ---------------------------------------------------------------------------
 * Shared read dispatcher
 *
 * Handles 1-byte reads, 2-byte reads (all standard 16-bit registers), and
 * multi-byte reads from 0x3E / 0x40 / 0x44:
 *   - 36-byte read from 0x3E: regular MAC read — dispatched to read_altmac
 *   - 32-byte read from 0x40: DF block read — raw payload, no address echo
 *   - 32-byte read from 0x44: DF read-next (auto-increment) — [data[0..31]]
 * ---------------------------------------------------------------------------
 */

static int emul_bq27z8xx_read(const struct emul *target, int reg, uint8_t *buf, size_t len,
			      read_altmac_fn_t read_altmac)
{
	if (len == 1) {
		int val;
		int rc = emul_bq27z8xx_reg_read(target, reg, &val);

		if (rc) {
			return rc;
		}
		buf[0] = (uint8_t)val;
	} else if (len == 2) {
		int val;
		int rc = emul_bq27z8xx_reg_read(target, reg, &val);

		if (rc) {
			return rc;
		}
		sys_put_le16((uint16_t)val, buf);
	} else if (reg == BQ27Z8XX_ALTMANUFACTURERACCESS) {
		struct bq27z8xx_emul_data *data = target->data;

		if (len == BQ27Z8XX_EMUL_DF_READ_LEN) {
			/* 34-byte DF block read: [addr_lo, addr_hi, data[0..31]] */
			if (data->mac_cmd < BQ27Z8XX_DF_ADDR_MIN ||
			    data->mac_cmd > BQ27Z8XX_DF_ADDR_MAX) {
				LOG_ERR("34-byte read for non-DF addr 0x%04x", data->mac_cmd);
				return -EIO;
			}
			memset(buf, 0, len);
			sys_put_le16(data->mac_cmd, buf);
			if (data->df_has_write && data->df_written_addr == data->mac_cmd) {
				memcpy(&buf[2], data->df_written_data, data->df_written_len);
			}
			/* Prime auto-increment for the next df_read_next() call */
			data->df_next_addr = data->mac_cmd + BQ27Z8XX_EMUL_MAC_DATA_LEN;
			return 0;
		}

		LOG_DBG("Reading %zu bytes from ALTMAC", len);
		return read_altmac(target, buf, len);
	} else if (reg == BQ27Z8XX_MACDATA) {
		/*
		 * 32-byte DF block read: no address echo, raw payload only.
		 * Mirrors bq27z8xx_df_read() reading from 0x40 after writing a
		 * DF address to 0x3E.  Prime df_next_addr for auto-increment.
		 */
		struct bq27z8xx_emul_data *data = target->data;

		if (data->mac_cmd < BQ27Z8XX_DF_ADDR_MIN || data->mac_cmd > BQ27Z8XX_DF_ADDR_MAX) {
			LOG_ERR("MACDATA read for non-DF addr 0x%04x", data->mac_cmd);
			return -EIO;
		}
		memset(buf, 0, len);
		if (data->df_has_write && data->df_written_addr == data->mac_cmd) {
			memcpy(buf, data->df_written_data, data->df_written_len);
		}
		data->df_next_addr = data->mac_cmd + BQ27Z8XX_EMUL_MAC_DATA_LEN;
		return 0;
	} else if (reg == BQ27Z8XX_MACDATANEXT) {
		/* 32-byte DF read-next: auto-incremented address, no address echo */
		struct bq27z8xx_emul_data *data = target->data;

		memset(buf, 0, len);
		if (data->df_has_write && data->df_written_addr == data->df_next_addr) {
			memcpy(buf, data->df_written_data, data->df_written_len);
		}
		data->df_next_addr += BQ27Z8XX_EMUL_MAC_DATA_LEN;
		return 0;
	} else {
		LOG_ERR("Unexpected read: reg=0x%x len=%zu", reg, len);
		return -EIO;
	}

	return 0;
}

/* ---------------------------------------------------------------------------
 * Shared I2C transfer core
 *
 * Handles 1-message (write) and 2-message (register-select + read) transfers.
 * The read_altmac function pointer is forwarded to emul_bq27z8xx_read.
 * ---------------------------------------------------------------------------
 */

static int emul_bq27z8xx_transfer_i2c_core(const struct emul *target, struct i2c_msg *msgs,
					   int num_msgs, int addr, read_altmac_fn_t read_altmac)
{
	int reg;
	int rc;

	__ASSERT_NO_MSG(msgs && num_msgs);

	i2c_dump_msgs_rw(target->dev, msgs, num_msgs, addr, false);
	switch (num_msgs) {
	case 1:
		if (msgs->flags & I2C_MSG_READ) {
			LOG_ERR("Unexpected read");
			return -EIO;
		}
		return emul_bq27z8xx_write(target, msgs->buf, msgs->len);
	case 2:
		if (msgs->flags & I2C_MSG_READ) {
			LOG_ERR("Unexpected read");
			return -EIO;
		}
		if (msgs->len != 1) {
			LOG_ERR("Unexpected msg0 length %d", msgs->len);
			return -EIO;
		}
		reg = msgs->buf[0];

		msgs++;
		if (msgs->flags & I2C_MSG_READ) {
			rc = emul_bq27z8xx_read(target, reg, msgs->buf, msgs->len, read_altmac);
			if (rc) {
				return rc;
			}
		} else {
			LOG_ERR("Second message must be an I2C read");
			return -EIO;
		}
		return rc;
	default:
		LOG_ERR("Invalid number of messages: %d", num_msgs);
		return -EIO;
	}
}

/* ---------------------------------------------------------------------------
 * Generic transfer entry point (reads read_altmac from emul cfg)
 * ---------------------------------------------------------------------------
 */

static int bq27z8xx_emul_transfer_i2c(const struct emul *target, struct i2c_msg *msgs, int num_msgs,
				      int addr)
{
	const struct bq27z8xx_emul_cfg *cfg = target->cfg;

	return emul_bq27z8xx_transfer_i2c_core(target, msgs, num_msgs, addr, cfg->read_altmac);
}

static const struct i2c_emul_api bq27z8xx_emul_api_i2c = {
	.transfer = bq27z8xx_emul_transfer_i2c,
};

/* ---------------------------------------------------------------------------
 * Emulator init
 * ---------------------------------------------------------------------------
 */

static int emul_bq27z8xx_init(const struct emul *target, const struct device *parent)
{
	ARG_UNUSED(target);
	ARG_UNUSED(parent);

	return 0;
}

/* ---------------------------------------------------------------------------
 * Device instantiation — two passes, one per DT compatible
 * ---------------------------------------------------------------------------
 */

#define BQ27Z8XX_EMUL_DEFINE(n, _fn)                                                               \
	static struct bq27z8xx_emul_data bq27z8xx_emul_data_##n;                                   \
	static const struct bq27z8xx_emul_cfg bq27z8xx_emul_cfg_##n = {                            \
		.addr = DT_INST_REG_ADDR(n),                                                       \
		.read_altmac = (_fn),                                                              \
	};                                                                                         \
	EMUL_DT_INST_DEFINE(n, emul_bq27z8xx_init, &bq27z8xx_emul_data_##n,                        \
			    &bq27z8xx_emul_cfg_##n, &bq27z8xx_emul_api_i2c, NULL)

/* --- ti,bq27z758 instances --- */
#ifdef CONFIG_BQ27Z758
#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT               ti_bq27z758
#define BQ27Z8XX_EMUL_DEFINE_758(n) BQ27Z8XX_EMUL_DEFINE(n, emul_bq27z758_read_altmac)
DT_INST_FOREACH_STATUS_OKAY(BQ27Z8XX_EMUL_DEFINE_758)
#endif /* CONFIG_BQ27Z758 */

/* --- ti,bq27z855 instances --- */
#ifdef CONFIG_BQ27Z855
#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT               ti_bq27z855
#define BQ27Z8XX_EMUL_DEFINE_855(n) BQ27Z8XX_EMUL_DEFINE(n, emul_bq27z855_read_altmac)
DT_INST_FOREACH_STATUS_OKAY(BQ27Z8XX_EMUL_DEFINE_855)
#endif /* CONFIG_BQ27Z855 */
