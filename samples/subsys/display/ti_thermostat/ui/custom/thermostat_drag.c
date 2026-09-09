/**
 * @file thermostat_drag.c
 *
 * Relative drag-to-set for the temperature field.
 *
 * Not an lv_slider: a slider is absolute and jumps the value to wherever you
 * press, which on a wall thermostat is a destructive mis-tap. This is relative -
 * the level moves with the finger by the distance travelled.
 *
 * Two quantities, deliberately:
 *   subject_setpoint      whole degrees  the value, snapped
 *   subject_field_level   hundredths     the picture, continuous
 *
 * Binding the bar straight to the setpoint makes it lurch between detents while
 * the finger moves smoothly; this module keeps the two in step and eases the
 * level onto the snapped value when the finger lifts.
 *
 * Attached from the screen's XML via <event_cb trigger="screen_loaded">, not
 * from an init in thermostat.c - that file is hand-editable and a stale copy on
 * a target build silently drops calls it does not know about.
 */

#include "thermostat_drag.h"
#include "thermostat_config.h"
#include "../thermostat.h"

/*======================= TUNING =======================*/

/** Finger travel per degree. 28px puts the 15-30 range across ~420px. */
#define DRAG_PX_PER_STEP 28

/** Movement below this is not a drag. Without it, the press and release of an
 *  ordinary tap can register a step from a pixel or two of finger roll. */
#define DRAG_DEADZONE_PX 6

/** How long the field takes to settle onto the snapped value after release. */
#define DRAG_SETTLE_MS 260

/** Jump duration. Long enough for the slow-in/slow-out curve to read. */
#define DRAG_JUMP_MS 620

#define DRAG_STEP_MS 320

/** Max the fill may advance per frame, in hundredths of a degree. The lv_bar
 *  invalidates the strip between its old and new value, so an unbounded jump (a
 *  fast full-range drag) repaints the whole screen in one ~500ms frame on the
 *  M0+. Clamping the per-frame travel bounds each repaint to a thin strip: the
 *  fill slews toward the finger over a few frames (PRESSING fires every frame,
 *  so it converges while the finger holds) and snaps exactly on release. 150 =
 *  1.5 C of bar per frame, ~10% of the field height. */
#define DRAG_MAX_SLEW 150

/* The field's scale lives in thermostat_config.h, shared with the marker - both
 * map a temperature onto the same pixels and must agree exactly. */
#define SETPOINT_MIN   THERMOSTAT_SETPOINT_MIN
#define SETPOINT_MAX   THERMOSTAT_SETPOINT_MAX
#define LEVEL_MIN      THERMOSTAT_LEVEL_MIN
#define LEVEL_MAX      THERMOSTAT_LEVEL_MAX
#define LEVEL_PER_STEP THERMOSTAT_LEVEL_PER_STEP
#define FIELD_H        THERMOSTAT_FIELD_H

/*======================= STATIC PROTOTYPES =======================*/

static void level_anim_cb(void * var, int32_t value);
static void animate_level_to(int32_t target, uint32_t duration);
static void animate_level_jump(int32_t target);
static void setpoint_changed_cb(lv_observer_t * observer, lv_subject_t * subject);
static int32_t clampi(int32_t v, int32_t lo, int32_t hi);

/*======================= STATIC VARIABLES =======================*/

/** Registered once, however many times the screen loads. */
static bool observing;

static int32_t press_y;       /**< where the finger landed */
static int32_t press_level;   /**< the field level at that moment, hundredths */
static int32_t shown_level;   /**< the fill value actually written last frame, for slew-limiting */
static bool    dragging;      /**< has the deadzone been cleared? */
/** Suppresses the observer's plain settle while a jump starts its own. */
static bool    jumping;

/*======================= GLOBAL FUNCTIONS =======================*/

void thermostat_drag_attach_cb(lv_event_t * e)
{
    LV_UNUSED(e);

    /* Registers the observer that follows setpoint changes from anything that is
     * not the drag, then snaps the fill to the current value - arriving on a
     * screen is not a value change, so it must not animate. */
    if(!observing) {
        lv_subject_add_observer(&subject_setpoint, setpoint_changed_cb, NULL);
        observing = true;
    }

    lv_anim_delete(&subject_field_level, level_anim_cb);
    lv_subject_set_int(&subject_field_level,
                       lv_subject_get_int(&subject_setpoint) * LEVEL_PER_STEP);
}

void thermostat_drag_cb(lv_event_t * e)
{
    const lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t * indev = lv_indev_active();
    if(indev == NULL) return;
    if(lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER) return;

    lv_point_t p;

    switch(code) {
        case LV_EVENT_PRESSED:
            /* Take over from any settle still in flight, or it will keep writing
             * the level underneath the finger. */
            lv_anim_delete(&subject_field_level, level_anim_cb);

            lv_indev_get_point(indev, &p);
            press_y     = p.y;
            press_level = lv_subject_get_int(&subject_field_level);
            shown_level = press_level;
            dragging    = false;
            break;

        case LV_EVENT_PRESSING: {
            lv_indev_get_point(indev, &p);

            /* Up is warmer: screen y grows downward, so the sign flips here and
             * nowhere else. */
            const int32_t travel = press_y - p.y;

            if(!dragging) {
                if(travel > -DRAG_DEADZONE_PX && travel < DRAG_DEADZONE_PX) break;
                dragging = true;
            }

            /* Target derived from the press anchor, never accumulated: an
             * accumulator loses a remainder per frame and would not return to
             * where it started. */
            const int32_t target = clampi(press_level + (travel * LEVEL_PER_STEP) / DRAG_PX_PER_STEP,
                                          LEVEL_MIN, LEVEL_MAX);

            /* Slew-limit toward the target so one frame never repaints more than
             * a thin strip of the full-screen bar (see DRAG_MAX_SLEW). PRESSING
             * fires every frame, so the fill converges to the finger within a few
             * frames even if it is held still. */
            int32_t level = target;
            if(level > shown_level + DRAG_MAX_SLEW)      level = shown_level + DRAG_MAX_SLEW;
            else if(level < shown_level - DRAG_MAX_SLEW) level = shown_level - DRAG_MAX_SLEW;
            shown_level = level;

            lv_subject_set_int(&subject_field_level, level);

            const int32_t setpoint = clampi((level + LEVEL_PER_STEP / 2) / LEVEL_PER_STEP,
                                            SETPOINT_MIN, SETPOINT_MAX);
            lv_subject_set_int(&subject_setpoint, setpoint);
            break;
        }

        case LV_EVENT_DOUBLE_CLICKED: {
            /* Double tap jumps the target to the tapped position. Absolute, and safe only
             * because it takes two deliberate taps. LVGL raises this event itself. */
            lv_indev_get_point(indev, &p);

            int32_t level = LEVEL_MIN
                            + ((FIELD_H - p.y) * (LEVEL_MAX - LEVEL_MIN)) / FIELD_H;
            level = clampi(level, LEVEL_MIN, LEVEL_MAX);

            const int32_t setpoint = clampi((level + LEVEL_PER_STEP / 2) / LEVEL_PER_STEP,
                                            SETPOINT_MIN, SETPOINT_MAX);
            jumping = true;
            lv_subject_set_int(&subject_setpoint, setpoint);
            jumping = false;
            animate_level_jump(setpoint * LEVEL_PER_STEP);

            press_level = setpoint * LEVEL_PER_STEP;
            press_y     = p.y;
            dragging    = false;
            break;
        }

        case LV_EVENT_RELEASED:
        case LV_EVENT_PRESS_LOST:
            if(dragging) {
                animate_level_to(lv_subject_get_int(&subject_setpoint) * LEVEL_PER_STEP,
                                 DRAG_SETTLE_MS);
            }
            dragging = false;
            break;

        default:
            break;
    }
}

/*======================= STATIC FUNCTIONS =======================*/

static void level_anim_cb(void * var, int32_t value)
{
    LV_UNUSED(var);
    lv_subject_set_int(&subject_field_level, value);
}

static void animate_level_to(int32_t target, uint32_t duration)
{
    LV_UNUSED(duration);

    /* Instant, not animated. On the Cortex-M0+ with software rendering, a
     * smooth settle sweeps the full-screen bar through many large repaints
     * (~300ms/frame during a big swing); at 100Hz touch polling that backlogs
     * the 16-deep input queue faster than it drains and the UI wedges. Snapping
     * to the target is one repaint, after which the screen goes idle and input
     * catches up. See src/main.c fps/idle_ms probe for the evidence. */
    lv_anim_delete(&subject_field_level, level_anim_cb);
    lv_subject_set_int(&subject_field_level, target);
}

/**
 * The double-tap jump: cubic-bezier(0.85, 0, 0.16, 1.06) over DRAG_JUMP_MS.
 * Slow at both ends, quick through the middle, and 6% past the target before it
 * settles - proportional, so short jumps barely nudge and long ones rebound.
 * A drag settle uses plain ease-out instead: the finger already led it there.
 */
static void animate_level_jump(int32_t target)
{
    /* Instant for the same reason as animate_level_to(): the double-tap jump's
     * eased overshoot was the most expensive full-screen sweep of all. */
    lv_anim_delete(&subject_field_level, level_anim_cb);
    lv_subject_set_int(&subject_field_level, target);
}

static void setpoint_changed_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    LV_UNUSED(subject);

    if(dragging || jumping) return;

    animate_level_to(lv_subject_get_int(&subject_setpoint) * LEVEL_PER_STEP, DRAG_STEP_MS);
}

static int32_t clampi(int32_t v, int32_t lo, int32_t hi)
{
    if(v < lo) return lo;
    if(v > hi) return hi;
    return v;
}
