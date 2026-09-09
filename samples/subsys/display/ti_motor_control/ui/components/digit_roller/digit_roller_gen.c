/**
 * @file digit_roller_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "digit_roller_gen.h"
#include "../../motor_control.h"

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

lv_obj_t * digit_roller_create(lv_obj_t * parent, lv_subject_t * value, int32_t w)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if MOTOR_CONTROL_CHECK_COMPILE_TARGET(MOTOR_CONTROL_TARGET_ALL)
    if (motor_control_check_target(MOTOR_CONTROL_TARGET_ALL)) {
        lv_obj_t * lv_roller_0 = lv_roller_create(parent);
        lv_obj_set_name_static(lv_roller_0, "digit_roller_#");
        lv_obj_set_style_text_font(lv_roller_0, font_hero_64, 0);
        lv_obj_set_style_text_line_space(lv_roller_0, 20, 0);
        lv_roller_set_options(lv_roller_0, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_INFINITE);
        lv_roller_set_visible_row_count(lv_roller_0, 1);
        lv_roller_bind_value(lv_roller_0, value);
        lv_obj_set_flag(lv_roller_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_width(lv_roller_0, w);

        lv_obj_add_style(lv_roller_0, &style_roller, 0);
        lv_obj_add_style(lv_roller_0, &style_roller_sel, LV_PART_SELECTED);
        lv_obj_bind_style(lv_roller_0, &style_roller_dim, 0, &subject_state, REF_STATE_STOPPED);
        lv_obj_bind_style(lv_roller_0, &style_roller_dim, LV_PART_SELECTED, &subject_state, REF_STATE_STOPPED);
        lv_obj_bind_style(lv_roller_0, &style_roller_fault, 0, &subject_state, REF_STATE_FAULT);
        lv_obj_bind_style(lv_roller_0, &style_roller_fault, LV_PART_SELECTED, &subject_state, REF_STATE_FAULT);

        the_root = lv_roller_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

