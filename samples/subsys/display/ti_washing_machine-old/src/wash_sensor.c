#include "wash_sensor.h"

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include "ti_washing_machine.h"

LOG_MODULE_REGISTER(wash_sensor, CONFIG_LOG_DEFAULT_LEVEL);

#define WASH_SENSOR_POLL_MS   100

/* Only lp_mspm33c321a's overlay instantiates the APDS9960 node; other boards
 * supported by this sample (e.g. lp_mspm0g3519) have no such node, and
 * DEVICE_DT_GET_ONE() requires the node to exist at compile time. */


static const struct device * prox_dev;
static lv_timer_t * sensor_timer;

// Define your operational environment bounds (in Lux)
#define LUX_DARK_THRESHOLD   10   // Below this is considered "Full Dark"
#define LUX_BRIGHT_THRESHOLD 500  // Above this is considered "Bright Enough"

// Define your target water fill levels (e.g., in centimeters or percentage 0-100)
#define WATER_LEVEL_MIN      10   // Keep a minimum baseline level
#define WATER_LEVEL_MAX      90   // Safety ceiling to prevent overflow

static int calculate_water_target(int current_lux) 
{
    // 1. Safety Check / Boundary Clamping
    if (current_lux <= LUX_DARK_THRESHOLD) {
        return WATER_LEVEL_MAX; // Completely dark -> fill to max safety limit
    }
    if (current_lux >= LUX_BRIGHT_THRESHOLD) {
        return WATER_LEVEL_MIN; // Bright enough -> drop to min baseline
    }

    // 2. Linear Inversion Mapping
    // Formula scales linearly between your dark/bright thresholds
    int light_fraction = (current_lux - LUX_DARK_THRESHOLD) / (LUX_BRIGHT_THRESHOLD - LUX_DARK_THRESHOLD);
    
    // Invert it: as light_fraction goes 0.0 -> 1.0, water target goes MAX -> MIN
    int target_level = WATER_LEVEL_MAX - (light_fraction * (WATER_LEVEL_MAX - WATER_LEVEL_MIN));

    return target_level;
}

static void process_sensor_data(const struct device *opt3001_dev)
{
    struct sensor_value lux_val;

    // Fetch data from Zephyr sensor API
    if (sensor_sample_fetch(opt3001_dev) == 0) {
        sensor_channel_get(opt3001_dev, SENSOR_CHAN_LIGHT, &lux_val);

        // Calculate the target
        int water_target = calculate_water_target(lux_val.val1);

        lv_subject_set_int(&subject_water_level, water_target);

        LOG_INF("Current Light: %d lux -> Target Water Level: %d",
                lux_val.val1, water_target);
    }
}

static void wash_sensor_timer_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    
    process_sensor_data(prox_dev);

    // if(sensor_sample_fetch(prox_dev) < 0) return;
    // if(sensor_channel_get(prox_dev, SENSOR_CHAN_LIGHT, &prox) < 0) return;

    
    

    // int32_t level = (prox.val1 * 100) / APDS9960_PROX_MAX;
    // if(level > 100) level = 100;
    // if(level < 0) level = 0;

    // lv_subject_set_int(&subject_water_level, level);
}

void wash_sensor_init(void)
{
    prox_dev = DEVICE_DT_GET_ONE(ti_opt3001);
    if(!device_is_ready(prox_dev)) {
        LV_LOG_WARN("ti_opt3001 not ready, subject_water_level sensor input disabled");
        return;
    }

	    printk("Sensor monitor initialized (HDC2080 + RTC)");    

    sensor_timer = lv_timer_create(wash_sensor_timer_cb, WASH_SENSOR_POLL_MS, NULL);
}

