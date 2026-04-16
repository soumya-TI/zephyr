/*
 * Copyright (c) 2023, ithinx GmbH
 * Copyright (c) 2023, Tonies GmbH
 * Copyright (c) 2026, Texas Instruments Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bq27z855.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(BQ27Z8XX);

/* ---------------------------------------------------------------------------
 * Public device-specific function — not part of the Zephyr fuel_gauge API.
 * See the note above bq27z855_mac_write_cmds in bq27z855.h for the reason.
 * ---------------------------------------------------------------------------
 */

int bq27z855_write_mac(const struct device *dev, enum bq27z855_mac_write_cmds cmd,
		       const uint8_t *data, size_t data_len)
{
	return bq27z8xx_write_mac(dev, (uint16_t)cmd, data, data_len);
}

/* ---------------------------------------------------------------------------
 * Variant overrides — scalar properties
 *
 * Only FUEL_GAUGE_CYCLE_COUNT differs between bq27z758 and bq27z855:
 * the bq27z855 reports the value in hundredths of a cycle (raw × 100).
 * All other scalar properties are handled by the common layer.
 * ---------------------------------------------------------------------------
 */

static int bq27z855_variant_get_prop(const struct device *dev, fuel_gauge_prop_t prop,
				     union fuel_gauge_prop_val *val)
{
	if (prop != FUEL_GAUGE_CYCLE_COUNT) {
		return -ENOTSUP;
	}

	uint16_t tmp_val = 0;
	int rc = bq27z8xx_read16(dev, BQ27Z8XX_CYCLECOUNT, &tmp_val);

	val->cycle_count = tmp_val * 100;

	return rc;
}

/* ---------------------------------------------------------------------------
 * Variant overrides — buffer properties
 *
 * All device-specific status register buffer properties are handled here.
 * Common buffer properties (DeviceType, FirmwareVersion, etc.) fall through
 * to bq27z8xx_get_buffer_prop() in the common layer.
 * ---------------------------------------------------------------------------
 */

/*
 * Read a MAC command response and verify the destination buffer size.
 * data[0] receives the device-reported payload length; data[1..dst_len-1]
 * receives the payload bytes.
 */
#define READ_MAC_PROP(cmd, struct_type)                                                            \
	do {                                                                                       \
		if (dst_len == sizeof(struct_type)) {                                              \
			rc = bq27z8xx_read_mac(dev, (cmd), (uint8_t *)dst, dst_len - 1,            \
					       K_NO_WAIT);                                         \
		} else {                                                                           \
			rc = -EINVAL;                                                              \
		}                                                                                  \
	} while (0)

static int bq27z855_variant_get_buffer_prop(const struct device *dev, fuel_gauge_prop_t prop,
					    void *dst, size_t dst_len)
{
	int rc = -ENOTSUP;

	switch (prop) {
	case BQ27Z855_BUFFER_PROP_SAFETYALERT:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_SAFETYALERT, struct bq27z855_safety_alert);
		break;
	case BQ27Z855_BUFFER_PROP_SAFETYSTATUS:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_SAFETYSTATUS, struct bq27z855_safety_status);
		break;
	case BQ27Z855_BUFFER_PROP_PFALERT:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_PFALERT, struct bq27z855_pf_alert);
		break;
	case BQ27Z855_BUFFER_PROP_PFSTATUS:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_PFSTATUS, struct bq27z855_pf_status);
		break;
	case BQ27Z855_BUFFER_PROP_OPERATIONSTATUS:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_OPERATIONSTATUS, struct bq27z855_operation_status);
		break;
	case BQ27Z855_BUFFER_PROP_CHARGINGSTATUS:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_CHARGINGSTATUS, struct bq27z855_charging_status);
		break;
	case BQ27Z855_BUFFER_PROP_GAUGINGSTATUS:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_GAUGINGSTATUS, struct bq27z855_gauging_status);
		break;
	case BQ27Z855_BUFFER_PROP_MANUFACTURINGSTATUS:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_MANUFACTURINGSTATUS,
			      struct bq27z855_manufacturing_status);
		break;
	case BQ27Z855_BUFFER_PROP_NOLOADREMCAP:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_NOLOADREMCAP, struct bq27z855_no_load_rem_cap);
		break;
	case BQ27Z855_BUFFER_PROP_CHARGINGSTATUSEXT:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_CHARGINGSTATUSEXT,
			      struct bq27z855_charging_status_ext);
		break;
	case BQ27Z855_BUFFER_PROP_ACCUMULATIONCHARGETHRESHOLD:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_ACCUMULATIONCHARGETHRESHOLD,
			      struct bq27z855_accumulation_charge_threshold);
		break;
	case BQ27Z855_BUFFER_PROP_ACCUMULATIONDISCHARGETHRESHOLD:
		READ_MAC_PROP(BQ27Z855_MAC_CMD_ACCUMULATIONDISCHARGETHRESHOLD,
			      struct bq27z855_accumulation_discharge_threshold);
		break;
	default:
		rc = -ENOTSUP;
	}

	return rc;
}

/* ---------------------------------------------------------------------------
 * Driver registration
 * ---------------------------------------------------------------------------
 */

const struct bq27z8xx_variant_ops bq27z855_variant_ops = {
	.get_prop = bq27z855_variant_get_prop,
	.set_prop = NULL, /* all set properties handled by common layer */
	.get_buffer_prop = bq27z855_variant_get_buffer_prop,
};
