/**
 * @file nav.c
 *
 * See nav.h. Frees the current screen before building the next so the two
 * screen trees never coexist in the LVGL object pool.
 */

#include "nav.h"

typedef lv_obj_t * (*screen_create_fn)(void);

/* Runs from lv_async_call, i.e. AFTER the originating click event has fully
 * returned — so it is safe to delete the screen that owned the tapped button. */
static void nav_do_switch(void * param)
{
    /* LVGL stores the create function in the event user_data (void *); the
     * round-trip cast mirrors what lv_obj_add_screen_create_event does. */
    screen_create_fn create = (screen_create_fn)param;
    lv_obj_t * old = lv_screen_active();

    /* A tiny placeholder lets us drop the outgoing screen before allocating the
     * incoming one. It is never rendered: no lv_timer_handler runs between these
     * calls, so only the final screen is ever drawn. */
    lv_obj_t * placeholder = lv_obj_create(NULL);
    lv_screen_load(placeholder);
    if (old != NULL) {
        lv_obj_delete(old);
    }

    lv_screen_load(create());
    lv_obj_delete(placeholder);
}

void nav_switch_cb(lv_event_t * e)
{
    lv_async_call(nav_do_switch, lv_event_get_user_data(e));
}
