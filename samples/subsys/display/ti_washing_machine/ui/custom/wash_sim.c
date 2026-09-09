#include "wash_sim.h"
#include "wash_logic.h"

#include "../washing_machine.h"

#define WM_TICK_MS  33

static lv_timer_t * wm_timer;
static uint32_t     wm_elapsed_ms;
static int32_t      wm_angle_tenths;   /* transform_rotation units: 0..3600 */

static int32_t wm_smooth(int32_t num, int32_t den, int32_t from, int32_t to)
{
    if(den <= 0) return to;
    if(num < 0)   num = 0;
    if(num > den) num = den;

    const int32_t t  = (num * 1024) / den;                 /* x      , 0..1024 */
    const int32_t t2 = (t * t) / 1024;                     /* x^2    , 0..1024 */
    const int32_t s  = (t2 * (3 * 1024 - 2 * t)) / 1024;   /* x^2(3-2x), 0..1024 */

    return from + ((to - from) * s) / 1024;
}

static int32_t wm_speed_for(int32_t progress)
{
    if(progress < 70)  return wm_smooth(progress,       70,  0,  45);   /* rest -> tumble */
    if(progress < 92)  return wm_smooth(progress - 70,  22, 45, 230);   /* tumble -> spin */
    return                    wm_smooth(progress - 92,   8, 230,  0);   /* spin -> rest */
}

static void wm_apply_rotation(void)
{
    lv_obj_t * screen = lv_screen_active();
    if(screen == NULL) return;

    lv_obj_t * load = lv_obj_find_by_name(screen, "drum_load");
    if(load == NULL) return;   /* not on the cycle screen - nothing to turn */

    lv_obj_set_style_transform_rotation(load, wm_angle_tenths, LV_PART_MAIN);
}

static void wm_tick(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    wm_elapsed_ms += WM_TICK_MS;

    if(wm_elapsed_ms < WASH_SIM_HOLD_MS) return;

    const uint32_t run_ms = wm_elapsed_ms - WASH_SIM_HOLD_MS;

    int32_t progress = (int32_t)((run_ms * 100) / WASH_SIM_DURATION_MS);
    if(progress > 100) progress = 100;
    lv_subject_set_int(&subject_progress, progress);

    wm_angle_tenths = (wm_angle_tenths + wm_speed_for(progress)) % 3600;
    wm_apply_rotation();

    if(progress >= 100) {
        wash_sim_stop();
        lv_subject_set_int(&subject_state, WASH_STATE_COMPLETE);
    }
}

void wash_sim_start(void)
{
    wm_elapsed_ms   = 0;
    wm_angle_tenths = 0;

    lv_subject_set_int(&subject_progress, 0);
    lv_subject_set_int(&subject_state, WASH_STATE_RUNNING);
    wm_apply_rotation();

    if(wm_timer == NULL) wm_timer = lv_timer_create(wm_tick, WM_TICK_MS, NULL);
    else                 lv_timer_resume(wm_timer);
}

void wash_sim_stop(void)
{
    if(wm_timer != NULL) lv_timer_pause(wm_timer);
}

static void wm_screen_loaded_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    wash_sim_start();
}

void wash_sim_init(void)
{
    wm_timer = NULL;
}

/* Named for the XML <event_cb callback="..."> on the cycle screen. */
void wash_sim_screen_loaded_cb(lv_event_t * e);
void wash_sim_screen_loaded_cb(lv_event_t * e) { wm_screen_loaded_cb(e); }
