/**
 * @file thermostat_sim.h
 *
 * Sensor input for the thermostat demo.
 *
 * ═══ FOR THE INTEGRATION TEAM ═══════════════════════════════════════════════
 * This is the ONLY file in the project that invents data. Everything else -
 * the UI, the control logic, the display strings - reads its inputs from
 * subjects and does not care where they came from.
 *
 * To attach real sensors:
 *
 *   1. Set THERMOSTAT_SIM_ENABLE to 0 in thermostat_config.h. The simulator
 *      then compiles to nothing.
 *   2. Call thermostat_inputs_publish() from wherever you read your sensors -
 *      an ADC conversion-complete callback, an I2C read in a timer, an RTOS
 *      task, whatever suits the board.
 *
 * Nothing else has to change. There is no other coupling between the UI and
 * the sensors.
 */

#ifndef THERMOSTAT_SIM_H
#define THERMOSTAT_SIM_H

#include <stdint.h>

/**
 * Publish one set of sensor readings to the UI.
 *
 * This is the seam between the hardware and everything above it. Call it as
 * often as you have fresh readings; it is cheap, and writing a subject with an
 * unchanged value does not trigger a redraw.
 *
 * @param indoor_tenths   room temperature, TENTHS of a degree C (214 = 21.4 C)
 * @param outdoor_tenths  outdoor temperature, tenths of a degree C, may be negative
 * @param humidity_pct    relative humidity, whole percent, 0-100
 *
 * THREADING: LVGL is not thread-safe. If your sensor read happens off the LVGL
 * task, take the LVGL lock around this call (lv_lock()/lv_unlock()), or post the
 * values to the LVGL task and call this from there.
 */
void thermostat_inputs_publish(int32_t indoor_tenths, int32_t outdoor_tenths, int32_t humidity_pct);

/**
 * Start the built-in simulator. Compiled out when THERMOSTAT_SIM_ENABLE is 0,
 * in which case this is an empty function and calling it is harmless.
 */
void thermostat_sim_init(void);

#endif /* THERMOSTAT_SIM_H */
