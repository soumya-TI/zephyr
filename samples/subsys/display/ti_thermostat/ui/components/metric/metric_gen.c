/**
 * @file metric_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "metric_gen.h"
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

lv_obj_t * metric_create(lv_obj_t * parent, const void * icon, const char * caption, lv_subject_t * value, const char * fmt, const char * placeholder, const char * unit, int32_t col_w)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if THERMOSTAT_CHECK_COMPILE_TARGET(THERMOSTAT_TARGET_ALL)
    if (thermostat_check_target(THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * column_0 = column_create(parent, 0, 0, 0, 0, CONST_SPACE_8, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_name_static(column_0, "metric_#");
        lv_obj_set_width(column_0, col_w);
        lv_obj_set_height(column_0, LV_SIZE_CONTENT);

        lv_obj_t * row_0 = row_create(column_0, 0, 0, 0, 0, CONST_SPACE_8, 0, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_t * lv_image_0 = lv_image_create(row_0);
        lv_image_set_src(lv_image_0, icon);
        lv_obj_set_width(lv_image_0, 16);
        lv_obj_set_height(lv_image_0, 16);
        lv_obj_add_style(lv_image_0, &style_icon_ink, 0);

        lv_obj_t * label_0 = label_create(row_0);
        lv_label_set_text(label_0, caption);
        lv_obj_add_style(label_0, &style_caption, 0);
        lv_obj_add_style(label_0, &style_ink_muted, 0);

        lv_obj_t * row_1 = row_create(column_0, 0, 0, 0, 0, CONST_SPACE_2, 0, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END);
        lv_obj_t * label_1 = label_create(row_1);
        lv_label_bind_text(label_1, value, fmt);
        lv_label_set_text(label_1, placeholder);
        lv_obj_add_style(label_1, &style_stat_md, 0);
        lv_obj_add_style(label_1, &style_ink, 0);

        lv_obj_t * label_2 = label_create(row_1);
        lv_label_set_text(label_2, unit);
        lv_obj_set_style_pad_bottom(label_2, 2, 0);
        lv_obj_add_style(label_2, &style_meta, 0);
        lv_obj_add_style(label_2, &style_ink_secondary, 0);

        the_root = column_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

