/**
 * @file thermostat_config.h
 *
 * Tuning constants and build switches for the thermostat demo.
 *
 * INTEGRATION: this is the first file to look at when moving from the demo to
 * real hardware. Set THERMOSTAT_SIM_ENABLE to 0 and the simulator is compiled
 * out entirely; the UI, the control logic and the subjects are untouched.
 */

#ifndef THERMOSTAT_CONFIG_H
#define THERMOSTAT_CONFIG_H

/*======================= BUILD SWITCHES =======================*/

/** 1 = drive the sensor subjects from the built-in simulator (demo).
 *  0 = the application is expected to call thermostat_inputs_publish()
 *      itself from a real sensor read. See thermostat_sim.c. */
#define THERMOSTAT_SIM_ENABLE 0

/** How often the simulator advances, in milliseconds. Also a sensible poll
 *  period for a real sensor: room temperature does not move faster than this. */
#define THERMOSTAT_SIM_PERIOD_MS 250

/*======================= UNITS =======================*/
/* Everything the control logic compares is in TENTHS of a degree - measured
 * temperatures, the deadband, the switching band. 214 = 21.4 C.
 *
 * The one exception is subject_setpoint, which is in WHOLE degrees, because it
 * is the only quantity a person sets and "%d" prints it with no help. Convert
 * with the macro below rather than by hand.
 *
 * (A third unit, hundredths, exists purely for the field's fill position - see
 * const_level_* in globals.xml. It never reaches the control logic.) */

#define THERMOSTAT_DEGREES_TO_TENTHS(deg) ((deg) * 10)

/*======================= FIELD GEOMETRY =======================*/
/* The temperature field's scale, shared by the drag gesture and the current-
 * temperature marker. Both map a temperature onto the same pixels, so the
 * numbers have to agree; they were duplicated in the two modules until a
 * double-tap needed the mapping in a third place.
 *
 * Mirrors the lv_bar in screens/home.xml and const_level_* / const_setpoint_* in
 * globals.xml. The bar is the full screen height with no padding and fills
 * bottom-up, so the mapping is a straight linear one:
 *
 *     y = FIELD_H - (level - LEVEL_MIN) * FIELD_H / (LEVEL_MAX - LEVEL_MIN)
 *
 * Keep in sync with the XML. */

#define THERMOSTAT_FIELD_H        480

#define THERMOSTAT_LEVEL_MIN      1500   /* hundredths of a degree: 15.00 C */
#define THERMOSTAT_LEVEL_MAX      3000   /* 30.00 C */
#define THERMOSTAT_LEVEL_PER_STEP 100    /* hundredths per whole-degree step */

#define THERMOSTAT_SETPOINT_MIN   15     /* whole degrees */
#define THERMOSTAT_SETPOINT_MAX   30

/*======================= CONTROL =======================*/

/* ── THE SWITCHING BAND ──────────────────────────────────────────────────────
 * A thermostat needs two DIFFERENT thresholds, not one. Turning on and off at
 * the same temperature is not hysteresis: the moment the room crosses it the
 * demand disappears, the room drifts back, and the state flips again - which is
 * short-cycling, and on real hardware it is what destroys compressors and
 * chatters relays.
 *
 * So heating starts only once the room has fallen ON_DELTA below target, and
 * then runs until it is OFF_DELTA ABOVE target. Cooling mirrors it. The full
 * switching band is ON_DELTA + OFF_DELTA - 0.7 C by default - and the system
 * sits in idle anywhere inside it.
 *
 * The consequence, which is correct and worth stating: the room is normally NOT
 * at exactly the setpoint. It cycles slowly around it, and the number you set is
 * the centre of that cycle rather than a value that is continuously held.
 *
 * Both in TENTHS of a degree. Raise ON_DELTA to make the system more patient;
 * raise OFF_DELTA to make each burst of heating or cooling run longer. */

/** How far BELOW target the room must fall before heating starts (and how far
 *  above before cooling starts). */
#define THERMOSTAT_ON_DELTA_TENTHS 8

/** How far PAST target the room must go before heating stops (and how far below
 *  before cooling stops). Overshoot is deliberate: stopping exactly at target
 *  guarantees an immediate restart. */
#define THERMOSTAT_OFF_DELTA_TENTHS 2

/** Log every state transition through LV_LOG_USER. Off by default; turn it on
 *  when tuning the band, and count the transitions over a few minutes - a
 *  healthy system cycles slowly, a short-cycling one floods the log. */
#define THERMOSTAT_LOG_TRANSITIONS 0

#endif /* THERMOSTAT_CONFIG_H */
