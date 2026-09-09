/**
 * @file thermostat_marker.h
 *
 * The dashed line marking the room's CURRENT temperature on the home field.
 *
 * Wired from the XML, not from an init function:
 *   <event_cb callback="thermostat_marker_attach_cb" trigger="screen_loaded" />
 *
 * The callback is resolved by name from generated code, so it cannot fall out of
 * sync with a hand-edited project .c file. See the note in thermostat_marker.c.
 */

#ifndef THERMOSTAT_MARKER_H
#define THERMOSTAT_MARKER_H

struct _lv_event_t;

/** Attach the marker to the screen that just loaded, and start tracking. */
void thermostat_marker_attach_cb(struct _lv_event_t * e);

#endif /* THERMOSTAT_MARKER_H */
