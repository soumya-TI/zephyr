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
#include <zephyr/drivers/sensor.h>
#include <stdio.h>
// #include <lv_demos.h>

#include "ti_washing_machine.h"
#include "wash_data.h"
#include "wash_sensor.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);



// #define WASH_SENSOR_POLL_MS   100

// /* Only lp_mspm33c321a's overlay instantiates the APDS9960 node; other boards
//  * supported by this sample (e.g. lp_mspm0g3519) have no such node, and
//  * DEVICE_DT_GET_ONE() requires the node to exist at compile time. */


// static const struct device * prox_dev;
// static lv_timer_t * sensor_timer;

// // Define your operational environment bounds (in Lux)
// #define LUX_DARK_THRESHOLD   10   // Below this is considered "Full Dark"
// #define LUX_BRIGHT_THRESHOLD 500  // Above this is considered "Bright Enough"

// // Define your target water fill levels (e.g., in centimeters or percentage 0-100)
// #define WATER_LEVEL_MIN      10   // Keep a minimum baseline level
// #define WATER_LEVEL_MAX      90   // Safety ceiling to prevent overflow

// static int calculate_water_target(int current_lux) 
// {
//     // 1. Safety Check / Boundary Clamping
//     if (current_lux <= LUX_DARK_THRESHOLD) {
//         return WATER_LEVEL_MAX; // Completely dark -> fill to max safety limit
//     }
//     if (current_lux >= LUX_BRIGHT_THRESHOLD) {
//         return WATER_LEVEL_MIN; // Bright enough -> drop to min baseline
//     }

//     // 2. Linear Inversion Mapping
//     // Formula scales linearly between your dark/bright thresholds
//     int light_fraction = (current_lux - LUX_DARK_THRESHOLD) / (LUX_BRIGHT_THRESHOLD - LUX_DARK_THRESHOLD);
    
//     // Invert it: as light_fraction goes 0.0 -> 1.0, water target goes MAX -> MIN
//     int target_level = WATER_LEVEL_MAX - (light_fraction * (WATER_LEVEL_MAX - WATER_LEVEL_MIN));

//     return target_level;
// }

// static void process_sensor_data(const struct device *opt3001_dev) 
// {
//     struct sensor_value lux_val;
    
//     // Fetch data from Zephyr sensor API
//     if (sensor_sample_fetch(opt3001_dev) == 0) {
//         sensor_channel_get(opt3001_dev, SENSOR_CHAN_LIGHT, &lux_val);
                
//         // Calculate the target
//         int water_target = calculate_water_target(lux_val.val1);
        
//         lv_subject_set_int(&subject_water_level, water_target);
        
//         printk("Current Light: %d lux -> Target Water Level: %d\n", 
//                 lux_val.val1, water_target);

            
//     }
//             printk("rimercallback\n");
// }

// static void wash_sensor_timer_cb(lv_timer_t * timer)
// {
//     LV_UNUSED(timer);

    
//     process_sensor_data(prox_dev);

// }


int main(void)
{
	const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	int ret;

	printk("Washing machine demo starting on %s", display_dev->name);

	if (!device_is_ready(display_dev)) {
		LOG_ERR("Display device not ready");
		return 0;
	}

	lv_display_set_rotation(lv_display_get_default(), LV_DISPLAY_ROTATION_90);
    
	/*
	prox_dev = DEVICE_DT_GET_ONE(ti_opt3001);
    if(!device_is_ready(prox_dev)) {
        printk("ti_opt3001 not ready, subject_water_level sensor input disabled");
        return 0;
    }

	printk("Sensor monitor initialized (HDC2080 + RTC)");    

    sensor_timer = lv_timer_create(wash_sensor_timer_cb, WASH_SENSOR_POLL_MS, NULL);
	*/

	/* Build the washing machine UI (subjects, screens, logic + simulator) and
	 * load the home screen. Assets are compiled in, so the asset-path
	 * argument is unused on this target. */
	ti_washing_machine_init("A:");

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
