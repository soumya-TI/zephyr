/*
 * Copyright (c) 2026 Texas Instruments
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>

#include "ti_thermostat.h"
#include "sensor_monitor.h"

LOG_MODULE_REGISTER(sensor_monitor, CONFIG_LOG_DEFAULT_LEVEL);

static const struct device *hdc_dev;
static const struct device *rtc_dev;
static lv_timer_t *sensor_timer;

static void sensor_tick_cb(lv_timer_t *timer)
{
	struct sensor_value temp, humidity;
	struct rtc_time tm;
	char time_str[6];
	int rc;

	LV_UNUSED(timer);

	/* Read HDC2080 temperature/humidity sensor */
	rc = sensor_sample_fetch(hdc_dev);
	if (rc < 0) {
		LOG_WRN("Failed to fetch sensor sample: %d", rc);
		return;
	}

	rc = sensor_channel_get(hdc_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
	if (rc < 0) {
		LOG_WRN("Failed to get temperature: %d", rc);
		return;
	}

	rc = sensor_channel_get(hdc_dev, SENSOR_CHAN_HUMIDITY, &humidity);
	if (rc < 0) {
		LOG_WRN("Failed to get humidity: %d", rc);
		return;
	}

	// /* Log raw sensor values for debugging */
	// printk("Raw sensor: temp val1=%d val2=%d (%.2f C), humidity val1=%d val2=%d (%d%%)",
	//         temp.val1, temp.val2, sensor_value_to_float(&temp),
	//         humidity.val1, humidity.val2, (int)(sensor_value_to_double(&humidity) + 0.5));

	float temp_c = sensor_value_to_float(&temp);
	int hum_pct = (int)(sensor_value_to_double(&humidity) + 0.5);

	/* Read RTC time */
	rc = rtc_get_time(rtc_dev, &tm);
	if (rc < 0) {
		LOG_WRN("Failed to get RTC time: %d", rc);
		return;
	}

	snprintf(time_str, sizeof(time_str), "%02d:%02d", tm.tm_hour, tm.tm_min);

	// /* Update LVGL subjects - use int100 for temp to avoid float logging issues */
	// int temp_c_int100 = (int)(temp_c * 100);
	// LOG_INF("Setting subject_now_temp=%d.%02d, humidity=%d, time=%s",
	//         temp_c_int100 / 100, temp_c_int100 % 100, hum_pct, time_str);
	lv_subject_set_float(&subject_now_temp, temp_c);
	lv_subject_set_int(&subject_humidity, hum_pct);
	lv_subject_copy_string(&subject_time, time_str);
}

int sensor_monitor_init(void)
{
	/* Get HDC2080 device - uses ti,hdc2080 compatible */
	hdc_dev = DEVICE_DT_GET_ONE(ti_hdc2080);
	if (!device_is_ready(hdc_dev)) {
		LOG_ERR("HDC2080 device not ready");
		return -ENODEV;
	}

	/* Get RTC device */
	rtc_dev = DEVICE_DT_GET(DT_ALIAS(rtc));
	if (!device_is_ready(rtc_dev)) {
		LOG_ERR("RTC device not ready");
		return -ENODEV;
	}

	/* Create 1-second timer for sensor updates */
	sensor_timer = lv_timer_create(sensor_tick_cb, 1000, NULL);
	if (!sensor_timer) {
		LOG_ERR("Failed to create sensor timer");
		return -ENOMEM;
	}

	LOG_INF("Sensor monitor initialized (HDC2080 + RTC)");
	return 0;
}