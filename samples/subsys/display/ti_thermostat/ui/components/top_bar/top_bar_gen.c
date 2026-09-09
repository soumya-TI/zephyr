/**
 * @file top_bar_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "top_bar_gen.h"
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

lv_obj_t * top_bar_create(lv_obj_t * parent, const char * title, const char * hint)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if THERMOSTAT_CHECK_COMPILE_TARGET(THERMOSTAT_TARGET_ALL)
    if (thermostat_check_target(THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * column_0 = column_create(parent, CONST_GUTTER, CONST_GUTTER, CONST_SPACE_16, 0, CONST_SPACE_2, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_name_static(column_0, "top_bar_#");
        lv_obj_set_width(column_0, CONST_SCREEN_W);
        lv_obj_set_height(column_0, LV_SIZE_CONTENT);

        lv_obj_t * row_0 = row_create(column_0, 0, 0, 0, 0, CONST_SPACE_8, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_width(row_0, lv_pct(100));
        lv_obj_set_height(row_0, LV_SIZE_CONTENT);
        lv_obj_t * title_label = label_create(row_0);
        lv_obj_set_name(title_label, "title_label");
        lv_label_set_text(title_label, title);
        lv_obj_add_style(title_label, &style_title, 0);
        lv_obj_add_style(title_label, &style_ink, 0);

        state_icons_create(row_0);

        lv_obj_t * spacer = container_create(row_0);
        lv_obj_set_name(spacer, "spacer");
        lv_obj_set_flex_grow(spacer, 1);
        lv_obj_set_height(spacer, 1);

        lv_obj_t * clock = label_create(row_0);
        lv_obj_set_name(clock, "clock");
        lv_label_bind_text(clock, &subject_time, NULL);
        lv_label_set_text(clock, "09:41");
        lv_obj_add_style(clock, &style_title, 0);
        lv_obj_add_style(clock, &style_ink, 0);

        lv_obj_t * hint_label = label_create(column_0);
        lv_obj_set_name(hint_label, "hint_label");
        lv_label_set_text(hint_label, hint);
        lv_obj_add_style(hint_label, &style_caption, 0);
        lv_obj_add_style(hint_label, &style_ink_muted, 0);

        the_root = column_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

