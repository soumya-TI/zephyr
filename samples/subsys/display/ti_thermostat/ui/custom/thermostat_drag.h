/**
 * @file thermostat_drag.h
 *
 * Relative drag-to-set for the temperature field.
 *
 * Both callbacks are resolved BY NAME from the XML (`<event_cb callback="..."/>`),
 * so they only need external linkage - there is no registration table and
 * nothing to call from an init function.
 */

#ifndef THERMOSTAT_DRAG_H
#define THERMOSTAT_DRAG_H

struct _lv_event_t;

/**
 * Drag handler for the set-mode gesture layer.
 * Wired from XML as: <event_cb callback="thermostat_drag_cb" trigger="all" />
 */
void thermostat_drag_cb(struct _lv_event_t * e);

/**
 * Attach the field to the screen that just loaded: registers the observer that
 * moves it when the setpoint changes by anything other than a drag, and brings
 * the fill into step with the current value.
 * Wired from XML as:
 *   <event_cb callback="thermostat_drag_attach_cb" trigger="screen_loaded" />
 */
void thermostat_drag_attach_cb(struct _lv_event_t * e);

#endif /* THERMOSTAT_DRAG_H */
