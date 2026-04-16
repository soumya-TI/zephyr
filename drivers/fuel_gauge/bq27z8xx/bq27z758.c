/*
 * Copyright (c) 2023, ithinx GmbH
 * Copyright (c) 2023, Tonies GmbH
 * Copyright (c) 2026, Texas Instruments Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bq27z758.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(BQ27Z8XX);

/* ---------------------------------------------------------------------------
 * Variant overrides — scalar properties
 *
 * The bq27z758 matches the common-layer defaults for all scalar properties:
 * FUEL_GAUGE_CYCLE_COUNT is returned as the raw register value (no ×100
 * factor), which is exactly what bq27z8xx_get_prop() does when no variant
 * override is present. No get_prop override is therefore needed.
 * ---------------------------------------------------------------------------
 */

/* ---------------------------------------------------------------------------
 * Variant overrides — buffer properties
 *
 * The bq27z758 handles three standard fuel_gauge buffer properties via MAC
 * commands (MANUFACTURER_NAME, DEVICE_NAME, DEVICE_CHEMISTRY) that the
 * bq27z855 does not support. It also exposes device-specific status register
 * buffer properties whose struct layouts differ from the bq27z855.
 * ---------------------------------------------------------------------------
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

static int bq27z758_variant_get_buffer_prop(const struct device *dev, fuel_gauge_prop_t prop,
					    void *dst, size_t dst_len)
{
	int rc = -ENOTSUP;

	switch (prop) {
	/* Standard fuel_gauge SBS buffer properties mapped to bq27z758 MAC commands */
	case FUEL_GAUGE_MANUFACTURER_NAME:
		READ_MAC_PROP(BQ27Z758_MAC_CMD_MANUFACTURER_NAME,
			      struct sbs_gauge_manufacturer_name);
		break;
	case FUEL_GAUGE_DEVICE_NAME:
		READ_MAC_PROP(BQ27Z758_MAC_CMD_DEVICE_NAME, struct sbs_gauge_device_name);
		break;
	case FUEL_GAUGE_DEVICE_CHEMISTRY:
		READ_MAC_PROP(BQ27Z758_MAC_CMD_DEVICE_CHEM, struct sbs_gauge_device_chemistry);
		break;

	/* bq27z758-specific status register buffer properties */
	case BQ27Z758_BUFFER_PROP_SAFETYALERT:
		READ_MAC_PROP(BQ27Z758_MAC_CMD_SAFETYALERT, struct bq27z758_safety_alert);
		break;
	case BQ27Z758_BUFFER_PROP_SAFETYSTATUS:
		READ_MAC_PROP(BQ27Z758_MAC_CMD_SAFETYSTATUS, struct bq27z758_safety_status);
		break;
	case BQ27Z758_BUFFER_PROP_PFALERT:
		READ_MAC_PROP(BQ27Z758_MAC_CMD_PFALERT, struct bq27z758_pf_alert);
		break;
	case BQ27Z758_BUFFER_PROP_PFSTATUS:
		READ_MAC_PROP(BQ27Z758_MAC_CMD_PFSTATUS, struct bq27z758_pf_status);
		break;
	case BQ27Z758_BUFFER_PROP_OPERATIONSTATUS:
		READ_MAC_PROP(BQ27Z758_MAC_CMD_OPERATIONSTATUS, struct bq27z758_operation_status);
		break;
	case BQ27Z758_BUFFER_PROP_CHARGINGSTATUS:
		READ_MAC_PROP(BQ27Z758_MAC_CMD_CHARGINGSTATUS, struct bq27z758_charging_status);
		break;
	case BQ27Z758_BUFFER_PROP_GAUGINGSTATUS:
		READ_MAC_PROP(BQ27Z758_MAC_CMD_GAUGINGSTATUS, struct bq27z758_gauging_status);
		break;
	case BQ27Z758_BUFFER_PROP_MANUFACTURINGSTATUS:
		READ_MAC_PROP(BQ27Z758_MAC_CMD_MANUFACTURINGSTATUS,
			      struct bq27z758_manufacturing_status);
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

const struct bq27z8xx_variant_ops bq27z758_variant_ops = {
	.get_prop = NULL, /* all scalar properties handled by common layer */
	.set_prop = NULL, /* all set properties handled by common layer */
	.get_buffer_prop = bq27z758_variant_get_buffer_prop,
};
