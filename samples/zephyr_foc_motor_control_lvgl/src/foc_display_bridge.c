/*
 * Copyright (c) 2026 Texas Instruments Incorporated
 * SPDX-License-Identifier: Apache-2.0
 */

#include "foc_display_bridge.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(foc_bridge, LOG_LEVEL_INF);

/* LVGL UI API */
#include "motor_control.h"
#include "custom/motor_sim.h"

/* FOC motor data */
#include "motor_common.h"
#include "motor.h"

/* ---------------------------------------------------------------------------
 * External motor state (defined in motor1_init.c / motor_common.c)
 * ---------------------------------------------------------------------------*/
extern MOTOR_Vars_t motorVars_M1;

/* ---------------------------------------------------------------------------
 * Configuration
 * ---------------------------------------------------------------------------*/

/* Motor has 4 pole-pairs: electrical_Hz * 60 / pole_pairs = shaft_RPM */
#define POLE_PAIRS        4

/*
 * Nominal torque used to scale torque_Nm to a 0-100% load percentage.
 * Adjust to match the rated torque of the connected motor.
 */
#define NOMINAL_TORQUE_NM 0.1f

/* Bridge fires every 100 ms — same cadence as the original simulation. */
#define BRIDGE_PERIOD_MS  100

/* Log once per second (every 10 bridge ticks). */
#define LOG_DIVIDER       10

/* ---------------------------------------------------------------------------
 * UI subject state mirrors
 * ---------------------------------------------------------------------------
 * REF_STATE_* must match the constants in motor_control_gen.h.
 */
#define UI_STATE_STOPPED  0
#define UI_STATE_RUNNING  1
#define UI_STATE_FAULT    2

/* ---------------------------------------------------------------------------
 * Static state
 * ---------------------------------------------------------------------------*/
static uint8_t log_div;

/* ---------------------------------------------------------------------------
 * Forward declarations
 * ---------------------------------------------------------------------------*/
static void bridge_tick(lv_timer_t *timer);

/* ---------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------------*/

void foc_display_bridge_init(void)
{
    /* No motor_digits_init() call here. The optimized UI binds subject_rpm_act
     * directly to a label with lv_label_bind_text(..., "%04d") — the old
     * roller-based odometer and its digit-decomposition helper are gone. */
    lv_timer_create(bridge_tick, BRIDGE_PERIOD_MS, NULL);

    /* motor_control_init_gen() initialises subject_state to 0 (STOPPED).
     * The bridge tick reads this every 100 ms and writes it to
     * flagEnableRunAndIdentify, so a STOPPED subject immediately disables
     * the motor regardless of what main.c set at startup.
     * Set the subject to RUNNING here so the first tick enables the drive. */
    // lv_subject_set_int(&subject_state, UI_STATE_RUNNING);

    printk("FOC display bridge initialised (%d ms period)\n", BRIDGE_PERIOD_MS);
}

/* ---------------------------------------------------------------------------
 * Bridge tick — fires every 100 ms inside lv_timer_handler()
 * ---------------------------------------------------------------------------*/
static void bridge_tick(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    /* ── Direction 1: UI → FOC ──────────────────────────────────────────────
     *
     * Read the subjects that the user modifies through the touchscreen and
     * write the corresponding fields in motorVars_M1 so the FOC loop picks
     * them up on its next 1 ms state-machine tick.
     *
     * subject_state: 0 = stopped, 1 = running, 2 = fault
     * subject_rpm_ref: speed setpoint in RPM (0–3600), set by +/- buttons
     */
    const int32_t ui_state   = lv_subject_get_int(&subject_state);
    const int32_t ui_rpm_ref = lv_subject_get_int(&subject_rpm_ref);

    /* Convert RPM → electrical Hz: Hz = RPM / 60 * pole_pairs */
    motorVars_M1.speedRef_Hz =
        (float)((ui_rpm_ref * POLE_PAIRS)/ 60.0f );
    printk("Motor Speed %f\n", motorVars_M1.speedRef_Hz);

    /* Clear-faults request: if the UI transitions from FAULT to STOPPED
     * (user tapped "Clear faults to start"), mirror that into the FOC. */
    static int32_t prev_ui_state;
    if (prev_ui_state == UI_STATE_FAULT && ui_state == UI_STATE_STOPPED) {
        motorVars_M1.flagClearFaults = true;
    }

    motorVars_M1.flagEnableRunAndIdentify =
        (ui_state == UI_STATE_RUNNING) ? 1 : 0;
    
    printk("motorVars_M1.flagEnableRunAndIdentify %d\n", motorVars_M1.flagEnableRunAndIdentify);

    prev_ui_state = ui_state;

    // /* ── Direction 2: FOC → UI ──────────────────────────────────────────────
    //  *
    //  * Read the measured quantities from motorVars_M1 and publish them as
    //  * LVGL subjects. motor_sim_publish() marks them dirty; the widget
    //  * re-render happens at the end of this lv_timer_handler() call.
    //  */

    // /* speed_Hz is electrical frequency. Shaft RPM = Hz / pole_pairs * 60. */
    // const int32_t rpm = (int32_t)(motorVars_M1.speed_Hz
    //                               / (float)POLE_PAIRS * 60.0f);

    // /* Is_A is RMS phase current in amps. UI expects milliamps. */
    // const int32_t milliamps = (int32_t)(motorVars_M1.Is_A * 1000.0f);

    // /* torque_Nm scaled to 0–100% load. Clamp to valid range. */
    // int32_t load_pct = (int32_t)(motorVars_M1.torque_Nm
    //                              / NOMINAL_TORQUE_NM * 100.0f);
    // if (load_pct < 0)   load_pct = 0;
    // if (load_pct > 100) load_pct = 100;

    // /* Temperature: placeholder until real ADC thermal channel is wired. */
    // const int32_t temp_c = 25;

    // motor_sim_publish(rpm, milliamps, temp_c, load_pct);

    // /* ── FOC state → UI state ───────────────────────────────────────────────
    //  *
    //  * Map the FOC's detailed state machine onto the UI's three-value subject.
    //  * Only write when the mapped value differs to avoid unnecessary redraws.
    //  */
    // int32_t new_ui_state = ui_state; /* default: keep current */

    // if (motorVars_M1.faultMtrNow.all != 0u) {
    //     new_ui_state = UI_STATE_FAULT;
    // } else if (motorVars_M1.motorState == MOTOR_CL_RUNNING ||
    //            motorVars_M1.motorState == MOTOR_CTRL_RUN   ||
    //            motorVars_M1.motorState == MOTOR_FWC_RUN) {
    //     new_ui_state = UI_STATE_RUNNING;
    // } else if (motorVars_M1.motorState == MOTOR_STOP_IDLE  ||
    //            motorVars_M1.motorState == MOTOR_NORM_STOP  ||
    //            motorVars_M1.motorState == MOTOR_BRAKE_STOP ||
    //            motorVars_M1.motorState == MOTOR_FAULT_STOP) {
    //     new_ui_state = UI_STATE_STOPPED;
    // }

    // if (new_ui_state != ui_state) {
    //     lv_subject_set_int(&subject_state, new_ui_state);
    //     if (new_ui_state == UI_STATE_FAULT) {
    //         motor_sim_trip();
    //     }
    }

    /* ── 1 Hz verification log ──────────────────────────────────────────────
     *
     * Prints one line per second so UART overhead is negligible.
     * Deferred log backend (CONFIG_LOG_MODE_DEFERRED) means this never
     * blocks the LVGL thread — it enqueues the message and the log thread
     * (priority 10) drains it when nothing else is runnable.
     */
    // if (++log_div >= LOG_DIVIDER) {
    //     log_div = 0;
    //     printk("rpm=%4d  mA=%5d  load=%3d%%  foc_state=%2d  fault=0x%04x\n",
    //             rpm, milliamps, load_pct,
    //             (int)motorVars_M1.motorState,
    //             (unsigned)motorVars_M1.faultMtrNow.all);
    // }

