/**
 * @file thermostat_logic.h
 *
 * Derived state and display projections for the thermostat UI.
 * See thermostat_logic.c for what belongs here and what does not.
 */

#ifndef THERMOSTAT_LOGIC_H
#define THERMOSTAT_LOGIC_H

/**
 * Start observing the input subjects and computing the derived ones.
 * Call once, AFTER thermostat_init_gen(), which is what creates the subjects.
 */
void thermostat_logic_init(void);

#endif /* THERMOSTAT_LOGIC_H */
