/**
 * @file tab_bar_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "tab_bar_gen.h"
#include "../../ti_thermostat.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

static void nav_to_home_cb(lv_event_t * e);
static void nav_to_insights_cb(lv_event_t * e);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * tab_bar_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * row_0 = row_create(parent, 0, 0, 0, 0, 0, 0, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_name_static(row_0, "tab_bar_#");
        lv_obj_set_width(row_0, CONST_SCREEN_W);
        lv_obj_set_height(row_0, CONST_TABBAR_H);
        lv_obj_set_style_bg_color(row_0, CONST_SURFACE_CARD, 0);
        lv_obj_set_style_bg_opa(row_0, (255 * 95 / 100), 0);
        lv_obj_set_style_border_color(row_0, CONST_BORDER, 0);
        lv_obj_set_style_border_width(row_0, 1, 0);
        lv_obj_set_style_border_side(row_0, LV_BORDER_SIDE_TOP, 0);

        lv_obj_t * tab_0 = tab_item_create(row_0, "Home", REF_TAB_HOME);
        lv_obj_set_name(tab_0, "tab_0");
        lv_obj_add_event_cb(tab_0, nav_to_home_cb, LV_EVENT_CLICKED, NULL);

        /* Schedule and Rooms screens are dropped to free RAM; their tabs are removed. */

        lv_obj_t * tab_3 = tab_item_create(row_0, "Insights", REF_TAB_INSIGHTS);
        lv_obj_set_name(tab_3, "tab_3");
        lv_obj_add_event_cb(tab_3, nav_to_insights_cb, LV_EVENT_CLICKED, NULL);

        the_root = row_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Use a plain lv_screen_load() rather than lv_screen_load_anim(ANIM_NONE) to
 * rule out a half-composited screen on this M0+ software renderer: the anim
 * path drives its own state machine / refresh scheduling, and we want a direct,
 * synchronous screen swap while diagnosing the Insights-tab hang. */
static void nav_to_home_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_screen_load(home);
}

static void nav_to_insights_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_screen_load(insights);
}

