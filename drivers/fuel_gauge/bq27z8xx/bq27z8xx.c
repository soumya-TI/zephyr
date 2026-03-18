/*
 * Copyright (c) 2026, Texas Instruments Incorporated
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Driver for the TI BQ27Z758 and BQ27Z855 fuel gauge family.
 *
 * Uses two separate DT compatibles (ti,bq27z758 / ti,bq27z855) so that
 * only the selected variant's code is compiled in.  The variant_ops pointer
 * is baked into the config struct at compile time — no runtime detection.
 */

#include "bq27z8xx_common.h"
#include "bq27z758.h"
#include "bq27z855.h"

#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/kernel.h>

static DEVICE_API(fuel_gauge, bq27z8xx_driver_api) = {
	.get_property = &bq27z8xx_get_prop,
	.set_property = &bq27z8xx_set_prop,
	.get_buffer_property = &bq27z8xx_get_buffer_prop,
	.battery_cutoff = &bq27z8xx_battery_cutoff,
};

#define BQ27Z8XX_DEFINE(n, _ops)                                                                   \
	static const struct bq27z8xx_config bq27z8xx_config_##n = {                                \
		.i2c = I2C_DT_SPEC_INST_GET(n),                                                    \
		.variant_ops = (_ops),                                                             \
	};                                                                                         \
	DEVICE_DT_INST_DEFINE(n, bq27z8xx_init, NULL, NULL, &bq27z8xx_config_##n, POST_KERNEL,     \
			      CONFIG_FUEL_GAUGE_INIT_PRIORITY, &bq27z8xx_driver_api)

/* ---- BQ27Z758 instances ---- */
#ifdef CONFIG_BQ27Z758
#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT        ti_bq27z758
#define BQ27Z8XX_INIT_758(n) BQ27Z8XX_DEFINE(n, &bq27z758_variant_ops)
DT_INST_FOREACH_STATUS_OKAY(BQ27Z8XX_INIT_758)
#endif /* CONFIG_BQ27Z758 */

/* ---- BQ27Z855 instances ---- */
#ifdef CONFIG_BQ27Z855
#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT        ti_bq27z855
#define BQ27Z8XX_INIT_855(n) BQ27Z8XX_DEFINE(n, &bq27z855_variant_ops)
DT_INST_FOREACH_STATUS_OKAY(BQ27Z8XX_INIT_855)
#endif /* CONFIG_BQ27Z855 */
