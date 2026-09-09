/**
 * @file row_toggle_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "row_toggle_gen.h"
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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * row_toggle_create(lv_obj_t * parent, const char * title, const char * subtitle, lv_subject_t * state)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * row_0 = row_create(parent, 0, 4, 0, 0, 12, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_name_static(row_0, "row_toggle_#");
        lv_obj_set_width(row_0, 264);
        lv_obj_set_height(row_0, 52);

        lv_obj_t * column_0 = column_create(row_0, 0, 0, 0, 0, 2, 1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_width(column_0, 0);
        lv_obj_set_height(column_0, LV_SIZE_CONTENT);
        lv_obj_t * label_0 = label_create(column_0);
        lv_label_set_text(label_0, title);
        lv_obj_set_style_text_color(label_0, CONST_TEXT_PRIMARY, 0);
        lv_obj_add_style(label_0, &style_body_strong, 0);

        lv_obj_t * label_1 = label_create(column_0);
        lv_label_set_text(label_1, subtitle);
        lv_obj_set_style_text_color(label_1, CONST_TEXT_SECONDARY, 0);
        lv_obj_add_style(label_1, &style_caption, 0);

        lv_obj_t * switch_0 = switch_create(row_0);
        lv_obj_bind_state_if_eq(switch_0, state, LV_STATE_CHECKED, REF_ON);
        lv_obj_add_subject_toggle_event(switch_0, state, LV_EVENT_CLICKED);

        the_root = row_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

