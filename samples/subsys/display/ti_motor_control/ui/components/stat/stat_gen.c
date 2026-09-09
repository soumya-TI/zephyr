/**
 * @file stat_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "stat_gen.h"
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

lv_obj_t * stat_create(lv_obj_t * parent, const void * icon, lv_subject_t * value, const char * fmt, const char * placeholder, const char * unit, int32_t col_w, lv_style_t * value_style, lv_style_t * value_tone)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if MOTOR_CONTROL_CHECK_COMPILE_TARGET(MOTOR_CONTROL_TARGET_ALL)
    if (motor_control_check_target(MOTOR_CONTROL_TARGET_ALL)) {
        lv_obj_t * row_0 = row_create(parent, 0, 0, 0, 0, CONST_SPACE_4, 0, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_name_static(row_0, "stat_#");
        lv_obj_set_width(row_0, col_w);
        lv_obj_set_height(row_0, LV_SIZE_CONTENT);

        lv_obj_t * lv_image_0 = lv_image_create(row_0);
        lv_image_set_src(lv_image_0, icon);
        lv_obj_set_width(lv_image_0, 18);
        lv_obj_set_height(lv_image_0, 18);
        lv_obj_add_style(lv_image_0, &style_icon_secondary, 0);

        lv_obj_t * row_1 = row_create(row_0, 0, 0, 0, 0, CONST_SPACE_4, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END);
        lv_obj_set_height(row_1, LV_SIZE_CONTENT);
        lv_obj_t * label_0 = label_create(row_1);
        lv_label_bind_text(label_0, value, fmt);
        lv_label_set_text(label_0, placeholder);
        lv_obj_add_style(label_0, value_style, 0);
        lv_obj_add_style(label_0, &style_ink, 0);
        lv_obj_add_style(label_0, value_tone, 0);

        lv_obj_t * label_1 = label_create(row_1);
        lv_label_set_text(label_1, unit);
        lv_obj_set_style_pad_bottom(label_1, 3, 0);
        lv_obj_add_style(label_1, &style_meta, 0);
        lv_obj_add_style(label_1, &style_ink_tertiary, 0);

        the_root = row_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

