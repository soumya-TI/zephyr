/**
 * @file switch_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "switch_gen.h"
#include "../../../ti_thermostat.h"

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

lv_obj_t * switch_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_main;
    static lv_style_t style_indicator;
    static lv_style_t style_indicator_checked;
    static lv_style_t style_knob;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_main);
        lv_style_init(&style_indicator);
        lv_style_init(&style_indicator_checked);
        lv_style_init(&style_knob);

        lv_style_set_width(&style_main, 48);
        lv_style_set_height(&style_main, 28);
        lv_style_set_bg_color(&style_main, CONST_CONTROL_OFF);
        lv_style_set_bg_opa(&style_main, (255 * 100 / 100));
        lv_style_set_radius(&style_main, 32767);
        lv_style_set_anim_duration(&style_main, 140);
        lv_style_set_border_width(&style_main, 0);
        lv_style_set_radius(&style_indicator, 32767);
        lv_style_set_bg_color(&style_indicator_checked, CONST_ACCENT);
        lv_style_set_bg_opa(&style_indicator_checked, (255 * 100 / 100));
        lv_style_set_bg_color(&style_knob, CONST_ON_ACCENT);
        lv_style_set_bg_opa(&style_knob, (255 * 100 / 100));
        lv_style_set_radius(&style_knob, 32767);
        lv_style_set_pad_all(&style_knob, 0);
        lv_style_set_border_color(&style_knob, CONST_BORDER);
        lv_style_set_border_width(&style_knob, 1);
        lv_style_set_border_opa(&style_knob, (255 * 100 / 100));

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * lv_switch_0 = lv_switch_create(parent);
        lv_obj_set_name_static(lv_switch_0, "switch_#");

        lv_obj_remove_style_all(lv_switch_0);
        lv_obj_add_style(lv_switch_0, &style_main, 0);
        lv_obj_add_style(lv_switch_0, &style_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_switch_0, &style_knob, LV_PART_KNOB);
        lv_obj_add_style(lv_switch_0, &style_shadow_knob, LV_PART_KNOB);
        lv_obj_add_style(lv_switch_0, &style_indicator_checked, LV_PART_INDICATOR | LV_STATE_CHECKED);

        the_root = lv_switch_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

