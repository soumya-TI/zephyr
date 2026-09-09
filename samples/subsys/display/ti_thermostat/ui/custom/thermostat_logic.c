/**
 * @file thermostat_logic.c
 *
 * The two things the XML genuinely cannot do:
 *
 *   1. Derived state. subject_hvac_state is a function of four other subjects
 *      AND of its own previous value (see the switching band in
 *      thermostat_config.h). The DSL compares a subject against a literal, never
 *      against another subject - and Auto is exactly that comparison.
 *   2. Display strings. bind_text-fmt knows only %d and %s, so measured
 *      temperatures, which carry a decimal, are formatted here.
 *
 * It must not read sensors: every input arrives as a subject. See
 * thermostat_sim.c for that boundary.
 */

#include "thermostat_logic.h"
#include "thermostat_config.h"
#include "../thermostat.h"

#include <stdio.h>

/*======================= ENUM MIRRORS =======================*/
/* Keep in sync with the ref_* consts in globals.xml. */

#define MODE_OFF   0
#define MODE_HEAT  1
#define MODE_COOL  2
#define MODE_AUTO  3

#define STATE_IDLE     0
#define STATE_HEATING  1
#define STATE_COOLING  2

/*======================= STATIC PROTOTYPES =======================*/

static void recompute_cb(lv_observer_t * observer, lv_subject_t * subject);
static void format_tenths(char * buf, size_t len, int32_t tenths);
static int32_t heat_demand(int32_t state, int32_t indoor, int32_t on_below, int32_t off_above);
static int32_t cool_demand(int32_t state, int32_t indoor, int32_t on_above, int32_t off_below);

/*======================= STATIC VARIABLES =======================*/

/** One group observer over every input: a single recompute cannot drift. */
static lv_subject_t * inputs[5];
static lv_subject_t input_group;

static int32_t prev_state = STATE_IDLE;

/*======================= GLOBAL FUNCTIONS =======================*/

void thermostat_logic_init(void)
{
    inputs[0] = &subject_hvac_mode;
    inputs[1] = &subject_setpoint;
    inputs[2] = &subject_indoor;
    inputs[3] = &subject_outdoor;
    inputs[4] = &subject_deadband;

    lv_subject_init_group(&input_group, inputs, 5);
    lv_subject_add_observer(&input_group, recompute_cb, NULL);

    /* Group observers do not fire on registration; prime the outputs once. */
    recompute_cb(NULL, NULL);
}

/*======================= STATIC FUNCTIONS =======================*/

/* Total: reads every input and writes every output, so there is no partial path. */
static void recompute_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    LV_UNUSED(subject);

    const int32_t mode          = lv_subject_get_int(&subject_hvac_mode);
    const int32_t setpoint_deg  = lv_subject_get_int(&subject_setpoint);
    const int32_t indoor        = lv_subject_get_int(&subject_indoor);
    const int32_t outdoor       = lv_subject_get_int(&subject_outdoor);
    const int32_t deadband      = lv_subject_get_int(&subject_deadband);

    const int32_t setpoint = THERMOSTAT_DEGREES_TO_TENTHS(setpoint_deg);

    const int32_t on_below  = setpoint - THERMOSTAT_ON_DELTA_TENTHS;
    const int32_t off_above = setpoint + THERMOSTAT_OFF_DELTA_TENTHS;
    const int32_t on_above  = setpoint + THERMOSTAT_ON_DELTA_TENTHS;
    const int32_t off_below = setpoint - THERMOSTAT_OFF_DELTA_TENTHS;

    /* Stateful by necessity: whether heating should run depends on whether it is
     * already running. Auto picks a side from the outdoor reading, then the same
     * switching band governs the actual on/off. */
    int32_t state = prev_state;

    switch(mode) {
        case MODE_HEAT:
            state = heat_demand(state, indoor, on_below, off_above);
            break;

        case MODE_COOL:
            state = cool_demand(state, indoor, on_above, off_below);
            break;

        case MODE_AUTO:
            if(outdoor < setpoint - deadband) {
                /* Heating side. If it was cooling, that side is no longer
                 * selected, so drop the demand before evaluating this one. */
                if(state == STATE_COOLING) state = STATE_IDLE;
                state = heat_demand(state, indoor, on_below, off_above);
            }
            else if(outdoor > setpoint + deadband) {
                if(state == STATE_HEATING) state = STATE_IDLE;
                state = cool_demand(state, indoor, on_above, off_below);
            }
            else {
                if(state == STATE_HEATING)      state = heat_demand(state, indoor, on_below, off_above);
                else if(state == STATE_COOLING) state = cool_demand(state, indoor, on_above, off_below);
            }
            break;

        case MODE_OFF:
        default:
            state = STATE_IDLE;
            break;
    }

#if THERMOSTAT_LOG_TRANSITIONS
    if(state != prev_state) {
        LV_LOG_USER("hvac %d -> %d  (indoor %d, outdoor %d, set %d)",
                    (int)prev_state, (int)state, (int)indoor, (int)outdoor, (int)setpoint);
    }
#endif

    prev_state = state;

    lv_subject_set_int(&subject_hvac_state, state);

    char buf[16];

    format_tenths(buf, sizeof(buf), indoor);
    lv_subject_copy_string(&subject_indoor_str, buf);

    format_tenths(buf, sizeof(buf), outdoor);
    lv_subject_copy_string(&subject_outdoor_str, buf);

    /* Whole degrees print with "%d"; only the measured values carry a decimal. */
    lv_subject_set_int(&subject_digit_tens,  (setpoint_deg / 10) % 10);
    lv_subject_set_int(&subject_digit_units, setpoint_deg % 10);
}

/* On below on_below, off above off_above. Between the two the answer is
 * whatever it was already doing - that gap is the whole point. */
static int32_t heat_demand(int32_t state, int32_t indoor, int32_t on_below, int32_t off_above)
{
    if(state == STATE_HEATING) return (indoor >= off_above) ? STATE_IDLE : STATE_HEATING;
    return (indoor <= on_below) ? STATE_HEATING : STATE_IDLE;
}

/** The mirror of heat_demand for cooling. */
static int32_t cool_demand(int32_t state, int32_t indoor, int32_t on_above, int32_t off_below)
{
    if(state == STATE_COOLING) return (indoor <= off_below) ? STATE_IDLE : STATE_COOLING;
    return (indoor >= on_above) ? STATE_COOLING : STATE_IDLE;
}

static void format_tenths(char * buf, size_t len, int32_t tenths)
{
    const char * sign = (tenths < 0) ? "-" : "";
    const int32_t mag = (tenths < 0) ? -tenths : tenths;

    lv_snprintf(buf, len, "%s%d.%d", sign, (int)(mag / 10), (int)(mag % 10));
}
