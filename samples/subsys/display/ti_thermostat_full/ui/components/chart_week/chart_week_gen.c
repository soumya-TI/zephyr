/**
 * @file chart_week_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "chart_week_gen.h"
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

lv_obj_t * chart_week_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_chart;
    static lv_style_t style_bars;
    static lv_style_t style_daylabels;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_chart);
        lv_style_init(&style_bars);
        lv_style_init(&style_daylabels);

        lv_style_set_width(&style_chart, 264);
        lv_style_set_height(&style_chart, 112);
        lv_style_set_bg_opa(&style_chart, 0);
        lv_style_set_border_width(&style_chart, 0);
        lv_style_set_pad_all(&style_chart, 0);
        lv_style_set_pad_column(&style_chart, 12);
        lv_style_set_line_width(&style_chart, 0);
        lv_style_set_radius(&style_bars, 4);
        lv_style_set_bg_opa(&style_bars, (255 * 100 / 100));
        lv_style_set_layout(&style_daylabels, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_daylabels, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_daylabels, LV_FLEX_ALIGN_SPACE_BETWEEN);
        lv_style_set_pad_top(&style_daylabels, 6);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * column_0 = column_create(parent, 0, 0, 0, 0, 0, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_name_static(column_0, "chart_week_#");
        lv_obj_set_width(column_0, 264);
        lv_obj_set_height(column_0, LV_SIZE_CONTENT);

        lv_obj_t * lv_chart_0 = lv_chart_create(column_0);
        lv_chart_set_type(lv_chart_0, LV_CHART_TYPE_BAR);
        lv_chart_set_point_count(lv_chart_0, 7);
        lv_chart_set_hor_div_line_count(lv_chart_0, 0);
        lv_chart_set_ver_div_line_count(lv_chart_0, 0);
        lv_obj_remove_style_all(lv_chart_0);
        lv_obj_add_style(lv_chart_0, &style_chart, 0);
        lv_obj_add_style(lv_chart_0, &style_bars, LV_PART_ITEMS);
        lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(lv_chart_0, lv_color_hex(0xE8630A), LV_CHART_AXIS_PRIMARY_Y);
        static const int32_t lv_chart_0_values_0[] = {52, 61, 48, 70, 44, 38, 66};
        lv_chart_set_series_values(lv_chart_0, lv_chart_series_0, lv_chart_0_values_0, 7);
        lv_chart_set_axis_min_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 0);
        lv_chart_set_axis_max_value(lv_chart_0, LV_CHART_AXIS_PRIMARY_Y, 80);

        lv_obj_t * row_0 = row_create(column_0, 0, 0, 0, 0, 0, 0, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START);
        lv_obj_set_width(row_0, 264);
        lv_obj_set_style_pad_top(row_0, 5, 0);
        lv_obj_set_style_pad_hor(row_0, 8, 0);
        lv_obj_set_height(row_0, LV_SIZE_CONTENT);
        lv_obj_t * label_0 = label_create(row_0);
        lv_label_set_text(label_0, "M");
        lv_obj_set_style_text_color(label_0, CONST_TEXT_TERTIARY, 0);
        lv_obj_add_style(label_0, &style_micro, 0);

        lv_obj_t * label_1 = label_create(row_0);
        lv_label_set_text(label_1, "T");
        lv_obj_set_style_text_color(label_1, CONST_TEXT_TERTIARY, 0);
        lv_obj_add_style(label_1, &style_micro, 0);

        lv_obj_t * label_2 = label_create(row_0);
        lv_label_set_text(label_2, "W");
        lv_obj_set_style_text_color(label_2, CONST_TEXT_TERTIARY, 0);
        lv_obj_add_style(label_2, &style_micro, 0);

        lv_obj_t * label_3 = label_create(row_0);
        lv_label_set_text(label_3, "T");
        lv_obj_set_style_text_color(label_3, CONST_TEXT_TERTIARY, 0);
        lv_obj_add_style(label_3, &style_micro, 0);

        lv_obj_t * label_4 = label_create(row_0);
        lv_label_set_text(label_4, "F");
        lv_obj_set_style_text_color(label_4, CONST_TEXT_TERTIARY, 0);
        lv_obj_add_style(label_4, &style_micro, 0);

        lv_obj_t * label_5 = label_create(row_0);
        lv_label_set_text(label_5, "S");
        lv_obj_set_style_text_color(label_5, CONST_TEXT_TERTIARY, 0);
        lv_obj_add_style(label_5, &style_micro, 0);

        lv_obj_t * label_6 = label_create(row_0);
        lv_label_set_text(label_6, "S");
        lv_obj_set_style_text_color(label_6, CONST_TEXT_TERTIARY, 0);
        lv_obj_add_style(label_6, &style_micro, 0);

        the_root = column_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

