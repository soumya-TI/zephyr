/**
 * @file stat_tile_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "stat_tile_gen.h"
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

lv_obj_t * stat_tile_create(lv_obj_t * parent, const char * value, const char * unit, const char * label)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * card_0 = card_create(parent);
        lv_obj_set_name_static(card_0, "stat_tile_#");
        lv_obj_set_flex_flow(card_0, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_flex_main_place(card_0, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
        lv_obj_set_style_pad_all(card_0, 12, 0);
        lv_obj_set_style_pad_row(card_0, 2, 0);
        lv_obj_set_width(card_0, 134);
        lv_obj_set_height(card_0, 72);
        lv_obj_set_style_bg_color(card_0, CONST_SURFACE_CANVAS, 0);
        lv_obj_set_style_bg_opa(card_0, (255 * 100 / 100), 0);
        lv_obj_set_style_border_color(card_0, CONST_BORDER, 0);
        lv_obj_set_style_border_width(card_0, 1, 0);
        lv_obj_set_style_radius(card_0, CONST_RADIUS_MD, 0);

        lv_obj_t * row_0 = row_create(card_0, 0, 0, 0, 0, 2, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END);
        lv_obj_set_width(row_0, LV_SIZE_CONTENT);
        lv_obj_set_height(row_0, LV_SIZE_CONTENT);
        lv_obj_t * label_0 = label_create(row_0);
        lv_label_set_text(label_0, value);
        lv_obj_set_style_text_color(label_0, CONST_TEXT_PRIMARY, 0);
        lv_obj_add_style(label_0, &style_stat_md, 0);

        lv_obj_t * label_1 = label_create(row_0);
        lv_label_set_text(label_1, unit);
        lv_obj_set_style_text_color(label_1, CONST_TEXT_SECONDARY, 0);
        lv_obj_set_style_pad_bottom(label_1, 2, 0);
        lv_obj_add_style(label_1, &style_meta, 0);

        lv_obj_t * label_2 = label_create(card_0);
        lv_label_set_text(label_2, label);
        lv_obj_set_style_text_color(label_2, CONST_TEXT_SECONDARY, 0);
        lv_obj_add_style(label_2, &style_caption, 0);

        the_root = card_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

