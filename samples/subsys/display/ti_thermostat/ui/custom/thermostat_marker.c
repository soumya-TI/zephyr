/**
 * @file thermostat_marker.c
 *
 * Positions the dashed line marking the room's CURRENT temperature against the
 * field, which shows the target. The gap between them is what the system has
 * left to do.
 *
 * C because the DSL can bind a subject to text, a flag, a style or a widget's
 * value - but not to a position, and this is temperature-to-pixel arithmetic.
 *
 * Attached from the screen's XML via <event_cb trigger="screen_loaded">, not
 * from an init in thermostat.c: an ESP build carrying an older copy of that file
 * skipped the call, and the line sat still on hardware while working in preview.
 */

#include "thermostat_marker.h"
#include "thermostat_config.h"
#include "../thermostat.h"

/*======================= GEOMETRY =======================*/
/* Field scale is shared with the drag gesture - both map onto the same pixels. */

#define FIELD_H     THERMOSTAT_FIELD_H
#define LEVEL_MIN   THERMOSTAT_LEVEL_MIN
#define LEVEL_MAX   THERMOSTAT_LEVEL_MAX

/** Glide time. The room moves in ~3px steps; easing makes that a descent. */
#define MARKER_GLIDE_MS 500

/*======================= STATIC PROTOTYPES =======================*/

static void indoor_changed_cb(lv_observer_t * observer, lv_subject_t * subject);
static void position_marker(lv_obj_t * screen, bool animate);
static void marker_y_anim_cb(void * var, int32_t y);

/*======================= STATIC VARIABLES =======================*/

/** Registered once, however many times the screen loads. */
static bool observing;

/*======================= GLOBAL FUNCTIONS =======================*/

void thermostat_marker_attach_cb(lv_event_t * e)
{
    lv_obj_t * screen = lv_event_get_current_target_obj(e);
    if(screen == NULL) return;

    if(!observing) {
        lv_subject_add_observer(&subject_indoor, indoor_changed_cb, NULL);
        observing = true;
    }

    /* Snap, do not animate: arriving on a screen is not a state change. */
    position_marker(screen, false);
}

/*======================= STATIC FUNCTIONS =======================*/

static void indoor_changed_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    LV_UNUSED(subject);

    /* The marker only exists on home; elsewhere this finds nothing. */
    position_marker(lv_screen_active(), true);
}

static void position_marker(lv_obj_t * screen, bool animate)
{
    if(screen == NULL) return;

    lv_obj_t * marker = lv_obj_find_by_name(screen, "temp_marker");
    if(marker == NULL) return;

    /* subject_indoor is in TENTHS; the field's scale is hundredths. */
    int32_t level = lv_subject_get_int(&subject_indoor) * 10;
    if(level < LEVEL_MIN) level = LEVEL_MIN;
    if(level > LEVEL_MAX) level = LEVEL_MAX;

    /* Bottom-up: the minimum sits at the bottom edge of the screen. */
    const int32_t y = FIELD_H - ((level - LEVEL_MIN) * FIELD_H) / (LEVEL_MAX - LEVEL_MIN);

    lv_anim_delete(marker, marker_y_anim_cb);

    if(!animate) {
        lv_obj_set_y(marker, y);
        return;
    }

    const int32_t current = lv_obj_get_y(marker);
    if(current == y) return;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, marker);
    lv_anim_set_exec_cb(&a, marker_y_anim_cb);
    lv_anim_set_values(&a, current, y);
    lv_anim_set_duration(&a, MARKER_GLIDE_MS);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

static void marker_y_anim_cb(void * var, int32_t y)
{
    lv_obj_set_y((lv_obj_t *)var, y);
}
