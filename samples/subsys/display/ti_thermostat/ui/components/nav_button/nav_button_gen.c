/**
 * @file nav_button_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "nav_button_gen.h"
#include "../../thermostat.h"

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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * nav_button_create(lv_obj_t * parent, const void * icon)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_circle;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_circle);

        lv_style_set_width(&style_circle, CONST_TOUCH_LG);
        lv_style_set_height(&style_circle, CONST_TOUCH_LG);
        lv_style_set_radius(&style_circle, CONST_RADIUS_PILL);
        lv_style_set_pad_all(&style_circle, 0);
        lv_style_set_layout(&style_circle, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_circle, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_circle, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_circle, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_track_place(&style_circle, LV_FLEX_ALIGN_CENTER);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if THERMOSTAT_CHECK_COMPILE_TARGET(THERMOSTAT_TARGET_ALL)
    if (thermostat_check_target(THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * lv_button_0 = lv_button_create(parent);
        lv_obj_set_name_static(lv_button_0, "nav_button_#");

        lv_obj_remove_style_all(lv_button_0);
        lv_obj_add_style(lv_button_0, &style_circle, 0);
        lv_obj_add_style(lv_button_0, &style_glass_dark, 0);
        lv_obj_add_style(lv_button_0, &style_glass_dark_pressed, LV_STATE_PRESSED);
        lv_obj_t * lv_image_0 = lv_image_create(lv_button_0);
        lv_image_set_src(lv_image_0, icon);
        lv_obj_set_width(lv_image_0, 26);
        lv_obj_set_height(lv_image_0, 26);
        lv_obj_set_flag(lv_image_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_image_0, &style_icon_ink, 0);

        the_root = lv_button_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

