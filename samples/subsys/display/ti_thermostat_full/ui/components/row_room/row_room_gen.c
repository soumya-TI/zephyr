/**
 * @file row_room_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "row_room_gen.h"
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

lv_obj_t * row_room_create(lv_obj_t * parent, const char * title, const char * meta, lv_subject_t * temp, lv_subject_t * demand, lv_style_t * bartone)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * row_0 = row_create(parent, 0, 0, 0, 0, 12, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_name_static(row_0, "row_room_#");
        lv_obj_set_width(row_0, 264);
        lv_obj_set_height(row_0, 60);

        lv_obj_t * column_0 = column_create(row_0, 0, 0, 0, 0, 3, 1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_width(column_0, 0);
        lv_obj_set_height(column_0, LV_SIZE_CONTENT);
        lv_obj_t * label_0 = label_create(column_0);
        lv_label_set_text(label_0, title);
        lv_obj_set_style_text_color(label_0, CONST_TEXT_PRIMARY, 0);
        lv_obj_add_style(label_0, &style_body_strong, 0);

        lv_obj_t * label_1 = label_create(column_0);
        lv_label_set_text(label_1, meta);
        lv_obj_set_style_text_color(label_1, CONST_TEXT_SECONDARY, 0);
        lv_obj_add_style(label_1, &style_caption, 0);

        lv_obj_t * bar_0 = bar_create(column_0);
        lv_obj_set_width(bar_0, 96);
        lv_obj_set_height(bar_0, 4);
        lv_bar_set_min_value(bar_0, 0);
        lv_bar_set_max_value(bar_0, 100);
        lv_bar_bind_value(bar_0, demand);
        lv_obj_set_style_pad_top(bar_0, 1, 0);
        lv_obj_add_style(bar_0, bartone, LV_PART_INDICATOR);

        lv_obj_t * label_2 = label_create(row_0);
        lv_label_bind_text(label_2, temp, NULL);
        lv_obj_set_style_text_color(label_2, CONST_TEXT_PRIMARY, 0);
        lv_obj_add_style(label_2, &style_stat_md, 0);

        the_root = row_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

