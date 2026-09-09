/**
 * @file row_metric_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "row_metric_gen.h"
#include "../../washing_machine.h"

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

lv_obj_t * row_metric_create(lv_obj_t * parent, const char * title, const void * icon, lv_subject_t * value, const char * fmt, const char * placeholder, const char * unit, int32_t w)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if WASHING_MACHINE_CHECK_COMPILE_TARGET(WASHING_MACHINE_TARGET_ALL)
    if (washing_machine_check_target(WASHING_MACHINE_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(parent);
        lv_obj_set_name_static(lv_obj_0, "row_metric_#");
        lv_obj_set_width(lv_obj_0, w);
        lv_obj_set_height(lv_obj_0, LV_SIZE_CONTENT);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_bg_opa(lv_obj_0, 0, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_set_style_layout(lv_obj_0, LV_LAYOUT_FLEX, 0);
        lv_obj_set_style_flex_flow(lv_obj_0, LV_FLEX_FLOW_COLUMN, 0);

        lv_obj_t * row_0 = row_create(lv_obj_0, 0, 0, 0, 0, 0, 0, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_width(row_0, w);
        lv_obj_t * row_1 = row_create(row_0, 0, 0, 0, 0, 6, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
        lv_obj_t * lv_image_0 = lv_image_create(row_1);
        lv_image_set_src(lv_image_0, icon);
        lv_obj_set_flag(lv_image_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_image_0, &style_icon_2, 0);

        lv_obj_t * label_0 = label_create(row_1);
        lv_label_set_text(label_0, title);
        lv_obj_add_style(label_0, &style_body, 0);
        lv_obj_add_style(label_0, &style_ink_2, 0);

        lv_obj_t * row_2 = row_create(row_0, 0, 0, 0, 0, 2, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END);
        lv_obj_t * label_1 = label_create(row_2);
        lv_label_set_text(label_1, placeholder);
        lv_label_bind_text(label_1, value, fmt);
        lv_obj_add_style(label_1, &style_body_strong, 0);
        lv_obj_add_style(label_1, &style_ink, 0);

        lv_obj_t * label_2 = label_create(row_2);
        lv_label_set_text(label_2, unit);
        lv_obj_set_style_translate_y(label_2, -1, 0);
        lv_obj_add_style(label_2, &style_micro, 0);
        lv_obj_add_style(label_2, &style_ink_3, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

