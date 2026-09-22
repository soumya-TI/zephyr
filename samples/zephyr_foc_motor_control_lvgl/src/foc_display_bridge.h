/*
 * Copyright (c) 2026 Texas Instruments Incorporated
 * SPDX-License-Identifier: Apache-2.0
 *
 * FOC ↔ LVGL bridge.
 *
 * Replaces motor_sim_init()'s fake simulation timer with a real 100 ms
 * lv_timer_t that:
 *   - reads motorVars_M1 (measured speed, current, torque) and pushes the
 *     values into LVGL subjects via motor_sim_publish(), and
 *   - reads LVGL subjects written by the UI (speed reference, start/stop) and
 *     writes them back into motorVars_M1 so the FOC loop picks them up.
 *
 * Call foc_display_bridge_init() once, after motor_control_init_gen() has
 * registered all subjects but before the first lv_timer_handler() call.
 */

#ifndef FOC_DISPLAY_BRIDGE_H
#define FOC_DISPLAY_BRIDGE_H

void foc_display_bridge_init(void);

#endif /* FOC_DISPLAY_BRIDGE_H */
