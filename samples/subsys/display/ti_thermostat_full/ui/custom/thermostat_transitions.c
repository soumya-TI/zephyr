/**
 * @file thermostat_transitions.c
 *
 * See thermostat_transitions.h for why this is C and not XML.
 */

/*********************
 *      INCLUDES
 *********************/

#include "../ti_thermostat.h"

#include "thermostat_transitions.h"

/*********************
 *      DEFINES
 *********************/

/* Mirrors of the ref_hvac_* consts in globals.xml, and the index each one takes
 * in the background array. */
#define REF_HVAC_IDLE      0
#define REF_HVAC_HEATING   1
#define REF_HVAC_COOLING   2
#define BG_COUNT           3

/* Object names declared in the XML. If these drift the lookup returns NULL and
 * the effect silently does nothing, so keep them under review alongside the
 * files that declare them. */
#define NAME_BG_IDLE       "bg_idle"          /* screens/home.xml */
#define NAME_BG_HEAT       "bg_heat"
#define NAME_BG_COOL       "bg_cool"
#define NAME_DIAL_ARC      "arc"             /* components/dial */

/* Ring hues, mirroring const_text_tertiary / const_heat / const_cool in
 * globals.xml. Duplicated because C cannot read XML consts — the same hand-sync
 * cost as the ref_* enum mirrors.
 *
 * Straight RGB interpolation. An HSV hue rotation was tried, on the theory that
 * it would avoid the desaturated midpoint between orange and blue by travelling
 * round the wheel through green — it does, and it looked far worse: a ring that
 * sweeps yellow-green-cyan reads as a rainbow effect, not as a thermostat
 * changing what it is doing. The muted midpoint is the better artefact. */
#define ARC_RGB_IDLE       0xAEB4B9
#define ARC_RGB_HEAT       0xE8630A
#define ARC_RGB_COOL       0x2A7FE0

#if THERMOSTAT_USE_TRANSITIONS

/**********************
 *  STATIC VARIABLES
 **********************/

static bool transitions_enabled = true;

#if THERMOSTAT_USE_TRANSITIONS
/* The state the backgrounds are currently showing. Guards against animating a
 * change that did not happen: lv_subject_add_observer_obj invokes its callback
 * immediately on registration, so without this every arrival on Home replayed
 * the dissolve for no reason. -1 means "nothing applied yet". */
static int32_t bg_last_state = -1;

#endif

/*---------------------------------------------------------------------------
 * Background cross-dissolve
 *--------------------------------------------------------------------------*/

#if THERMOSTAT_USE_TRANSITIONS

static void opa_exec_cb(void * var, int32_t value);
static void translate_exec_cb(void * var, int32_t value);
static void bg_completed_cb(lv_anim_t * a);
static bool bg_collect(lv_obj_t * screen, lv_obj_t * out[BG_COUNT]);
static int32_t bg_active_index(int32_t state);
static void bg_apply(lv_obj_t * screen, int32_t state, bool animate);
/* Ring colour. The three bind_styles on the arc indicator stay in place as the
 * baseline — they are what colours it correctly when this file is compiled out
 * — and these locals animate over the top of them.
 *
 * Interpolated with lv_color_mix rather than swapped, because a hue jump on the
 * largest element on screen is the one place the change is most visible and
 * least excusable. The XML style layer has no transitions, so this cannot be
 * expressed declaratively. */
static uint32_t arc_rgb_for(int32_t state)
{
    switch(state) {
        case REF_HVAC_HEATING: return ARC_RGB_HEAT;
        case REF_HVAC_COOLING: return ARC_RGB_COOL;
        default:               return ARC_RGB_IDLE;
    }
}

static lv_color_t arc_from;
static lv_color_t arc_to;

static void arc_mix_exec_cb(void * var, int32_t value)
{
    /* lv_color_mix(a, b, ratio) = a*ratio + b*(255-ratio), so 0 is `from`
     * and 255 is `to`. */
    const lv_color_t mixed = lv_color_mix(arc_to, arc_from, (uint8_t)value);
    lv_obj_set_style_arc_color((lv_obj_t *)var, mixed, LV_PART_INDICATOR);
}

static void arc_apply(lv_obj_t * screen, int32_t state, bool animate)
{
    lv_obj_t * arc = lv_obj_find_by_name(screen, NAME_DIAL_ARC);
    if(arc == NULL) return;                       /* screen has no dial */

    const lv_color_t target = lv_color_hex(arc_rgb_for(state));

    if(!animate) {
        lv_obj_set_style_arc_color(arc, target, LV_PART_INDICATOR);
        arc_from = target;
        arc_to = target;
        return;
    }

    arc_from = arc_to;
    arc_to = target;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc);
    lv_anim_set_exec_cb(&a, arc_mix_exec_cb);
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_duration(&a, THERMOSTAT_TRANSITION_MS);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_start(&a);
}

static bool bg_collect(lv_obj_t * screen, lv_obj_t * out[BG_COUNT])
{
    out[REF_HVAC_IDLE]    = lv_obj_find_by_name(screen, NAME_BG_IDLE);
    out[REF_HVAC_HEATING] = lv_obj_find_by_name(screen, NAME_BG_HEAT);
    out[REF_HVAC_COOLING] = lv_obj_find_by_name(screen, NAME_BG_COOL);

    return out[0] != NULL && out[1] != NULL && out[2] != NULL;
}

static int32_t bg_active_index(int32_t state)
{
    if(state < 0 || state >= BG_COUNT) return REF_HVAC_IDLE;
    return state;
}

static void opa_exec_cb(void * var, int32_t value)
{
    lv_obj_set_style_opa((lv_obj_t *)var, (lv_opa_t)value, LV_PART_MAIN);
}

static void translate_exec_cb(void * var, int32_t value)
{
    lv_obj_set_style_translate_y((lv_obj_t *)var, value, LV_PART_MAIN);
}

static void bg_completed_cb(lv_anim_t * a)
{
    lv_obj_t * incoming = (lv_obj_t *)a->var;
    lv_obj_t * screen = lv_obj_get_screen(incoming);
    if(screen == NULL) return;

    lv_obj_t * bg[BG_COUNT];
    if(!bg_collect(screen, bg)) return;

    /* Re-derive the winner rather than capturing it: the state may have changed
     * again while this animation was running. */
    const int32_t active = bg_active_index(lv_subject_get_int(&subject_hvac_state));

    for(int32_t i = 0; i < BG_COUNT; i++) {
        if(i == active) continue;

        /* Park the losers: hidden, un-translated and opaque, so whichever one
         * wins next starts from a clean slate. A fully transparent full-screen
         * image is still a composite pass every frame; hiding it is free. */
        lv_obj_add_flag(bg[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_translate_y(bg[i], 0, LV_PART_MAIN);
        lv_obj_set_style_opa(bg[i], LV_OPA_COVER, LV_PART_MAIN);
    }
}

static void bg_apply(lv_obj_t * screen, int32_t state, bool animate)
{
    lv_obj_t * bg[BG_COUNT];
    if(!bg_collect(screen, bg)) return;

    const int32_t active = bg_active_index(state);

    /* Nothing changed: do not animate. */
    if(animate && active == bg_last_state) return;

    bg_last_state = active;

    if(!animate) {
        for(int32_t i = 0; i < BG_COUNT; i++) {
            lv_obj_set_style_opa(bg[i], i == active ? LV_OPA_COVER : LV_OPA_TRANSP,
                                 LV_PART_MAIN);
            lv_obj_set_style_translate_y(bg[i], 0, LV_PART_MAIN);
            if(i == active) lv_obj_remove_flag(bg[i], LV_OBJ_FLAG_HIDDEN);
            else            lv_obj_add_flag(bg[i], LV_OBJ_FLAG_HIDDEN);
        }
        return;
    }

    /* Everything visible for the duration: the outgoing image has to stay on
     * screen to be dissolved away from. The bind_flag observers have already
     * hidden it by the time this runs — they were registered when the screen
     * was built, this one on screen_loaded — so un-hiding here undoes that, and
     * that ordering is what makes the effect possible at all. */
    for(int32_t i = 0; i < BG_COUNT; i++) {
        lv_obj_remove_flag(bg[i], LV_OBJ_FLAG_HIDDEN);
    }

    /* Index 2, not move_foreground: the backgrounds are the first three children
     * and everything else paints above them. Sending one to the true foreground
     * would put it over the dial and the tab bar. This keeps the incoming image
     * topmost among the backgrounds only. */
    lv_obj_move_to_index(bg[active], BG_COUNT - 1);

    for(int32_t i = 0; i < BG_COUNT; i++) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, bg[i]);
        lv_anim_set_duration(&a, THERMOSTAT_TRANSITION_MS);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);

        if(i == active) {
            /* Rises into place while fading up, on top of the outgoing image.
             * Only the incoming one is blended — the layer beneath stays fully
             * opaque — so there is no mid-transition dip where both are half
             * transparent and the canvas shows through. */
            lv_obj_set_style_opa(bg[i], LV_OPA_TRANSP, LV_PART_MAIN);
            lv_obj_set_style_translate_y(bg[i], THERMOSTAT_TRANSITION_SLIDE_IN,
                                         LV_PART_MAIN);

            lv_anim_set_exec_cb(&a, opa_exec_cb);
            lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
            lv_anim_set_completed_cb(&a, bg_completed_cb);
            lv_anim_start(&a);

            /* Second animation on the same object: lv_anim keys on
             * (var, exec_cb), so a different exec_cb runs alongside the first
             * rather than replacing it. */
            lv_anim_set_exec_cb(&a, translate_exec_cb);
            lv_anim_set_values(&a, THERMOSTAT_TRANSITION_SLIDE_IN, 0);
            lv_anim_set_completed_cb(&a, NULL);
            lv_anim_start(&a);
        }
        else {
            /* Pushed down and out from underneath. */
            lv_obj_set_style_opa(bg[i], LV_OPA_COVER, LV_PART_MAIN);

            lv_anim_set_exec_cb(&a, translate_exec_cb);
            lv_anim_set_values(&a, 0, THERMOSTAT_TRANSITION_SLIDE_OUT);
            lv_anim_start(&a);
        }
    }
}

static void bg_prime(lv_obj_t * screen)
{
    lv_obj_t * probe[BG_COUNT];
    if(!bg_collect(screen, probe)) return;   /* not the Home screen */

    /* Set a known state without animating. A hidden object still carries
       opa 255, so un-hiding one to animate it would flash it to full opacity
       first; priming defuses that. Arriving on a screen is not a state change,
       so nothing here animates and no observer is registered. */
    const int32_t state = lv_subject_get_int(&subject_hvac_state);
    bg_apply(screen, state, false);
    arc_apply(screen, state, false);
}

/* One observer for the lifetime of the app, resolving its target from
 * lv_screen_active() when it fires. Registering per screen instead needs a
 * de-duplication guard, and any such guard goes stale when a screen is
 * rebuilt — see learnings_of_writing_xml.md. */
static void hvac_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);

    lv_obj_t * screen = lv_screen_active();
    if(screen == NULL) return;

    const int32_t state = lv_subject_get_int(subject);
    bg_apply(screen, state, transitions_enabled);
    arc_apply(screen, state, transitions_enabled);
}

#endif /* THERMOSTAT_USE_TRANSITIONS */

/*---------------------------------------------------------------------------
 * Entry point
 *--------------------------------------------------------------------------*/

void thermostat_screen_attach_cb(lv_event_t * e)
{
    lv_obj_t * screen = lv_event_get_current_target_obj(e);
    if(screen == NULL) return;

    /* No-ops on any screen without the named background images, so it is safe
     * to hook from anywhere. */
    bg_prime(screen);
}

void thermostat_transitions_init(void)
{
    lv_subject_add_observer(&subject_hvac_state, hvac_observer_cb, NULL);
}

void thermostat_transitions_set_enabled(bool enabled)
{
    transitions_enabled = enabled;
}

bool thermostat_transitions_get_enabled(void)
{
    return transitions_enabled;
}

#else /* THERMOSTAT_USE_TRANSITIONS */

/* Compiled out. The symbols still have to exist — the XML names the callback in
 * an <event_cb>, and the project init calls the initialiser — but they do
 * nothing. The bind_style bindings in the XML then swap the backgrounds and the
 * ring colour instantly, which is the baseline and is correct on its own. */
void thermostat_screen_attach_cb(lv_event_t * e)
{
    LV_UNUSED(e);
}

void thermostat_transitions_init(void)
{
}

void thermostat_transitions_set_enabled(bool enabled)
{
    LV_UNUSED(enabled);
}

bool thermostat_transitions_get_enabled(void)
{
    return false;
}

#endif /* THERMOSTAT_USE_TRANSITIONS */
