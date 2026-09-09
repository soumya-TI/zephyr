/**
 * @file insights_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "insights_gen.h"
#include "../ti_thermostat.h"

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

lv_obj_t * insights_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        if (insights == NULL) insights = lv_obj_create(NULL);
        lv_obj_t * lv_obj_0 = insights;
        lv_obj_set_name_static(lv_obj_0, "insights_#");
        lv_obj_set_width(lv_obj_0, CONST_SCREEN_W);
        lv_obj_set_height(lv_obj_0, CONST_SCREEN_H);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_bg_color(lv_obj_0, CONST_SURFACE_CANVAS, 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_grad_color(lv_obj_0, lv_color_hex(0xE9EEF1), 0);
        lv_obj_set_style_bg_grad_dir(lv_obj_0, LV_GRAD_DIR_VER, 0);

        lv_obj_add_style(lv_obj_0, &style_screen_base, 0);
        lv_obj_add_subject_set_int_event(lv_obj_0, &subject_tab, LV_EVENT_SCREEN_LOADED, REF_TAB_INSIGHTS);
        lv_obj_add_subject_set_int_event(lv_obj_0, &subject_suggest_dismissed, LV_EVENT_SCREEN_LOADED, REF_OFF);
        top_bar_create(lv_obj_0, "Insights");

        lv_obj_t * column_0 = column_create(lv_obj_0, 0, 0, 0, 0, 12, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_width(column_0, CONST_SCREEN_W);
        lv_obj_set_height(column_0, 380);
        lv_obj_set_y(column_0, 44);
        lv_obj_set_flag(column_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_scrollbar_mode(column_0, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_style_pad_top(column_0, 8, 0);
        lv_obj_set_style_pad_bottom(column_0, 16, 0);
        lv_obj_t * surface_0 = surface_create(column_0);
        lv_obj_set_flex_flow(surface_0, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_all(surface_0, 12, 0);
        lv_obj_set_style_pad_row(surface_0, 8, 0);
        overline_create(surface_0, "THIS WEEK");

        lv_obj_t * row_0 = row_create(surface_0, 0, 0, 0, 0, 8, 0, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_END);
        lv_obj_set_width(row_0, 264);
        lv_obj_set_height(row_0, LV_SIZE_CONTENT);
        lv_obj_t * row_1 = row_create(row_0, 0, 0, 0, 0, 3, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END);
        lv_obj_set_width(row_1, LV_SIZE_CONTENT);
        lv_obj_set_height(row_1, LV_SIZE_CONTENT);
        lv_obj_t * label_0 = label_create(row_1);
        lv_label_bind_text(label_0, &subject_energy_kwh, "%d.4");
        lv_obj_set_style_text_color(label_0, CONST_TEXT_PRIMARY, 0);
        lv_obj_add_style(label_0, &style_stat_lg, 0);

        lv_obj_t * label_1 = label_create(row_1);
        lv_label_set_text(label_1, "kWh");
        lv_obj_set_style_text_color(label_1, CONST_TEXT_SECONDARY, 0);
        lv_obj_set_style_pad_bottom(label_1, 3, 0);
        lv_obj_add_style(label_1, &style_meta, 0);

        chip_create(row_0, &style_chip_fill_accent, &style_chip_dot_accent, &style_chip_text_accent, "12% less than last week", false);

        chart_week_create(surface_0);

        lv_obj_t * row_2 = row_create(column_0, 0, 0, 0, 0, 12, 0, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
        lv_obj_set_width(row_2, CONST_CONTENT_W);
        lv_obj_set_height(row_2, LV_SIZE_CONTENT);
        stat_tile_create(row_2, "41", "h", "Heat runtime");

        stat_tile_create(row_2, "6", "days", "Eco streak");

        card_suggest_create(column_0, "Lower the evening setpoint?", "You dropped heat at 22:30 on five of the last seven nights.", &subject_suggest_dismissed);

        lv_obj_t * tab_bar_0 = tab_bar_create(lv_obj_0);
        lv_obj_set_y(tab_bar_0, 424);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

