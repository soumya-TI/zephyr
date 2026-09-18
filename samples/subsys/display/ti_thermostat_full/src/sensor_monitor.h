/*
 * Copyright (c) 2026 Texas Instruments
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SENSOR_MONITOR_H
#define SENSOR_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the sensor monitor
 *
 * Initializes the HDC2080 temperature/humidity sensor and RTC,
 * then creates a periodic LVGL timer to update UI subjects with real data.
 *
 * @return 0 on success, negative error code on failure
 */
int sensor_monitor_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_MONITOR_H */