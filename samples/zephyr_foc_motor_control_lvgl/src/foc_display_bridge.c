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
#define NOMINAL_TORQUE_NM 0.4f

/* Bridge fires every 100 ms — same cadence as the original simulation. */
#define BRIDGE_PERIOD_MS  100

/* Log once per second (every 10 bridge ticks). */
#define LOG_DIVIDER       10

#define RAMP_PER_UNIT    25

#define COAST_PER_TICK   100

#define TEMP_AMBIENT     31
#define TEMP_WARN        80

/* Tenths of a degree of stage heating per tick at full load, and the passive
 * cooling that opposes it. Integer tenths keep the model exact. */
#define HEAT_PER_TICK    3
#define COOL_PER_TICK    1


/* ---------------------------------------------------------------------------
 * UI subject state mirrors
 * ---------------------------------------------------------------------------
 * REF_STATE_* must match the constants in motor_control_gen.h.
 */
#define UI_STATE_STOPPED  0
#define UI_STATE_RUNNING  1
#define UI_STATE_FAULT    2


/*======================= STATIC VARIABLES =======================*/

static uint8_t log_div;

/* Display strings are owned here for the lifetime of the app: lv_subject_copy_string
 * copies into the subject's own buffer, but the subject was declared in XML with
 * a literal, so we always write through the API rather than aliasing. */
static char current_buf[16];
static char bus_buf[16];
static char runtime_buf[16];

/* Stage temperature in TENTHS of a degree, so a slow ramp is representable. The
 * subject carries whole degrees; this is the model's own finer state. */
static int32_t temp_tenths = TEMP_AMBIENT * 10;

/* Session accumulators for the trend screen. */
static int32_t run_ticks;
static int64_t load_sum;
static int32_t load_samples;

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
     * 
     * 
     */

    int32_t ui_state   = lv_subject_get_int(&subject_state);
    int32_t ui_rpm_ref = lv_subject_get_int(&subject_rpm_ref);
    int32_t ui_rpm_act   = lv_subject_get_int(&subject_rpm_act);




    /* ── Speed ────────────────────────────────────────────────────────────
     * Running chases the reference; anything else coasts to rest. Soft start
     * is what the ramp models - with it off the drive steps straight to the
     * commanded speed, which is exactly what that interlock means. */
    if(ui_state == UI_STATE_RUNNING) {
        if(lv_subject_get_int(&subject_cfg_softstart)) {
            int32_t step = lv_subject_get_int(&subject_ramp) * RAMP_PER_UNIT;
            if(step < RAMP_PER_UNIT) step = RAMP_PER_UNIT;
            ui_rpm_act = approach(ui_rpm_act, ui_rpm_ref, step);
        }
        else {
            ui_rpm_act = ui_rpm_ref;
        }
        run_ticks++;
    }
    else {
        ui_rpm_act = approach(ui_rpm_act, 0, COAST_PER_TICK);
    }



    /* Convert RPM → electrical Hz: Hz = RPM / 60 * pole_pairs */
    int32_t ui_direction = lv_subject_get_int(&subject_dir);

    uint32_t reversal_allowed = lv_subject_get_int(&subject_cfg_reverse);

    if (reversal_allowed == 0){
        static int32_t direction_lock = 0;

        if(((ui_direction == 0) && (direction_lock == 0)) || (direction_lock == 1)){
            motorVars_M1.speedRef_Hz =
                (float)((ui_rpm_ref * POLE_PAIRS)/ 60.0f );
                direction_lock = 1;
        }

        else if(((ui_direction == 1) && (direction_lock == 0)) || (direction_lock == -1)){
            motorVars_M1.speedRef_Hz =
                -(float)((ui_rpm_ref * POLE_PAIRS)/ 60.0f );
            direction_lock = -1;
        }

        if (ui_state == UI_STATE_STOPPED){
            direction_lock = 0;
        }
    }
    else if(reversal_allowed == 1){
        if(ui_direction == 0){
            motorVars_M1.speedRef_Hz =
                (float)((ui_rpm_ref * POLE_PAIRS)/ 60.0f );
        }

        else if(ui_direction == 1){
            motorVars_M1.speedRef_Hz =
                -(float)((ui_rpm_ref * POLE_PAIRS)/ 60.0f );
        }
    }

    motorVars_M1.flagEnableRunAndIdentify =
        (ui_state == UI_STATE_RUNNING) ? 1 : 0;

    
    if(ui_state == UI_STATE_RUNNING)
    {
        if(motorVars_M1.speedRef_Hz != (float)((ui_rpm_ref * POLE_PAIRS)/ 60.0f))
        {
            
        }
    }
    
    lv_subject_set_int(&subject_rpm_act, ui_rpm_act);
    lv_subject_set_int(&subject_rpm_ref, ui_rpm_ref);

    if(ui_rpm_act > lv_subject_get_int(&subject_peak_rpm)) {
        lv_subject_set_int(&subject_peak_rpm, ui_rpm_act);
    }

/* fault */

    if (ui_state != UI_STATE_RUNNING){
        if (motorVars_M1.faultMtrNow.all != 0){
            if(lv_subject_get_int(&subject_fault_active)!=0)
            {
                motorVars_M1.flagClearFaults = 1;
            }
        }
    } 
    else
    {
        if (motorVars_M1.faultMtrNow.all != 0)
        {
            lv_subject_set_int(&subject_fault_active, 1);
            lv_subject_set_int(&subject_fault_count, 1);
            lv_subject_set_int(&subject_state, UI_STATE_FAULT);
        }
    }




    /* ── Load and current ─────────────────────────────────────────────────
     * Torque demand rises with speed and with the gap still being closed, so
     * accelerating draws more than holding the same speed - which is what makes
     * the two trend series diverge during a ramp instead of tracking. */
    const int32_t gap  = (ui_rpm_ref > ui_rpm_act) ? (ui_rpm_ref - ui_rpm_act) : 0;
    int32_t load = (ui_rpm_act * 34) / 3600 + (gap * 60) / 3600;
    if(ui_rpm_act == 0) load = 0;
    if(load > 100) load = 100;

    lv_subject_set_int(&subject_load, load);

    /* ~0.9 A at rated load, plus a small no-load draw whenever it is turning. */
    int32_t milliamps = (load * 900) / 100 + (ui_rpm_act > 0 ? 180 : 0);

    const int32_t temp_c = temp_tenths / 10;


    /* ── Thermals ─────────────────────────────────────────────────────────
     * Heating is proportional to load; cooling is constant and always applies,
     * so the stage settles at an equilibrium rather than climbing forever. */
    temp_tenths += (load * HEAT_PER_TICK) / 100 - COOL_PER_TICK;
    if(temp_tenths < TEMP_AMBIENT * 10) temp_tenths = TEMP_AMBIENT * 10;

    /* Thermal derate is an interlock the operator can switch off, so the model
     * has to honour it - an interlock the simulation ignores is a lie in the UI. */
    if(lv_subject_get_int(&subject_cfg_derate) && temp_tenths > TEMP_WARN * 10) {
        milliamps = (milliamps * 70) / 100;
        temp_tenths -= 2;
    }

    lv_subject_set_int(&subject_temp, temp_c);


    /* ── Bus voltage ──────────────────────────────────────────────────────
     * Sags slightly under load; this is what the F311 fault in the log means. */
    const int32_t bus_mv = 24000 - load * 18;
    format_milliamps(bus_buf, sizeof(bus_buf), bus_mv);
    lv_subject_copy_string(&subject_bus_str, bus_buf);


    /* ── Session summary for the trend screen ─────────────────────────────── */
    if(ui_state == UI_STATE_RUNNING) {
        load_sum += load;
        load_samples++;
        if(load_samples > 0) {
            lv_subject_set_int(&subject_avg_load, (int32_t)(load_sum / load_samples));
        }

        const int32_t secs = run_ticks / (1000 / BRIDGE_PERIOD_MS);
        lv_snprintf(runtime_buf, sizeof(runtime_buf), "%02d:%02d",
                    (int)(secs / 60), (int)(secs % 60));
        lv_subject_copy_string(&subject_runtime_str, runtime_buf);
    }

}

