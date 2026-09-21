/*
 * Copyright (c) 2026 Texas Instruments
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Runs the vendored LVGL "washing machine" UI (see ui/, provenance in README.rst)
 * on a Zephyr display + touch. Zephyr auto-initializes LVGL and registers the
 * display/pointer devices via SYS_INIT (CONFIG_LV_Z_AUTO_INIT), so by the time
 * main() runs lv_init() has already happened and we only have to build the UI
 * and pump lv_timer_handler().
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <lvgl.h>
#include <stdio.h>
// #include <lv_demos.h>

#include "ti_washing_machine.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

/* Runs right after serial (prio 50) but before display/LVGL POST_KERNEL
 * init, so we can see what caused the *previous* boot's reset even if
 * this boot never reaches main() (e.g. it crashes/resets again).
 */
static int print_reset_cause(void)
{
	uint32_t cause = 0;

	hwinfo_get_reset_cause(&cause);
	hwinfo_clear_reset_cause();
	printk("reset cause: 0x%08x\n", cause);

	return 0;
}
SYS_INIT(print_reset_cause, PRE_KERNEL_1, 55);

int main(void)
{
	const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	int ret;

	printf("Washing machine demo starting on %s", display_dev->name);

	if (!device_is_ready(display_dev)) {
		LOG_ERR("Display device not ready");
		return 0;
	}

	lv_display_set_rotation(lv_display_get_default(), LV_DISPLAY_ROTATION_90);

	/* Build the washing machine UI (subjects, screens, logic + simulator) and
	 * load the home screen. Assets are compiled in, so the asset-path
	 * argument is unused on this target. */
	ti_washing_machine_init("A:");
	// lv_demo_music();

	/* Render one frame, then enable the panel. */
	lv_timer_handler();
	ret = display_blanking_off(display_dev);
	if (ret < 0 && ret != -ENOSYS) {
		LOG_ERR("Failed to turn display blanking off (%d)", ret);
		return 0;
	}

	while (1) {
		uint32_t sleep_ms = lv_timer_handler();

		if (sleep_ms == LV_NO_TIMER_READY) {
			sleep_ms = LV_DEF_REFR_PERIOD;
		}
		k_sleep(K_MSEC(sleep_ms));
	}

	return 0;
}
