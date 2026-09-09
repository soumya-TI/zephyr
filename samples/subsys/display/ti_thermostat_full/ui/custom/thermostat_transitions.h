/**
 * @file thermostat_transitions.h
 *
 * Cross-dissolve for the Home background and the dial's ring colour when
 * subject_hvac_state changes.
 *
 * In C because the DSL cannot express it: <timeline> fires on OBJECT events
 * only (clicked, screen_loaded, gesture...) with no subject-changed trigger,
 * and the XML style layer has no transitions, so bind_style swaps are instant.
 *
 * Layered over a working baseline, never replacing it: the XML bindings hide,
 * show and colour everything correctly on their own. With the switches in
 * thermostat_config.h at 0 this compiles to nothing and the visuals simply
 * snap.
 */

#ifndef THERMOSTAT_TRANSITIONS_H
#define THERMOSTAT_TRANSITIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "thermostat_config.h"

#include <stdbool.h>

/**
 * Resolved by name from the XML: <event_cb callback="thermostat_screen_attach_cb"
 * trigger="screen_loaded"/>. Primes a screen's visuals to the current state
 * without animating; no-ops on screens that lack them.
 */
void thermostat_screen_attach_cb(lv_event_t * e);

/**
 * Register the single observer that drives the transitions. Call once, after the
 * generated init — not per screen, which would need a de-duplication guard that
 * goes stale when a screen is rebuilt.
 */
void thermostat_transitions_init(void);

/**
 * Runtime override for a board that cannot hold frame rate through a
 * full-screen blend, without a rebuild. Defaults to enabled.
 */
void thermostat_transitions_set_enabled(bool enabled);

/** Current state of the runtime override. Always false when compiled out. */
bool thermostat_transitions_get_enabled(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*THERMOSTAT_TRANSITIONS_H*/
