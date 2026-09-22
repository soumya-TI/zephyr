/**
 * @file nav.h
 *
 * Screen navigation that frees the outgoing screen BEFORE building the incoming
 * one, so only a single screen tree is ever resident. The stock
 * lv_obj_add_screen_create_event() builds the new screen while the old one is
 * still alive; on the MSPM0G3519's 60 KB LVGL pool that ~70 KB two-tree peak
 * overflows and the UI wedges. See nav.c.
 */
#ifndef NAV_H
#define NAV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../motor_control.h"

/**
 * LV_EVENT_CLICKED handler for tab / header nav buttons. Register with the
 * target screen's create function as user_data, e.g.:
 *   lv_obj_add_event_cb(btn, nav_switch_cb, LV_EVENT_CLICKED, (void *)run_create);
 * The actual switch is deferred with lv_async_call() so it runs after the click
 * event completes (the button being tapped lives on the screen we free).
 */
void nav_switch_cb(lv_event_t * e);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*NAV_H*/
