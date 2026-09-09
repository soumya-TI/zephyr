/**
 * @file insights_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "insights_gen.h"
#include "../thermostat.h"

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

    static lv_style_t style_pos_back;
    static lv_style_t style_chart;
    static lv_style_t style_chart_line;
    static lv_style_t style_chart_div;
    static lv_style_t style_chart_pts;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_pos_back);
        lv_style_init(&style_chart);
        lv_style_init(&style_chart_line);
        lv_style_init(&style_chart_div);
        lv_style_init(&style_chart_pts);

        lv_style_set_x(&style_pos_back, 0);
        lv_style_set_y(&style_pos_back, CONST_SPACE_8);
        lv_style_set_width(&style_chart, CONST_SCREEN_W);
        lv_style_set_height(&style_chart, 196);
        lv_style_set_x(&style_chart, 0);
        lv_style_set_y(&style_chart, 132);
        lv_style_set_bg_opa(&style_chart, 0);
        lv_style_set_border_width(&style_chart, 0);
        lv_style_set_pad_all(&style_chart, 0);
        lv_style_set_radius(&style_chart, 0);
        lv_style_set_shadow_width(&style_chart, 0);
        lv_style_set_shadow_opa(&style_chart, 0);
        lv_style_set_line_width(&style_chart_line, 3);
        lv_style_set_line_rounded(&style_chart_line, true);
        lv_style_set_line_color(&style_chart_div, CONST_INK);
        lv_style_set_line_opa(&style_chart_div, 34);
        lv_style_set_line_width(&style_chart_div, 1);
        lv_style_set_line_dash_width(&style_chart_div, 2);
        lv_style_set_line_dash_gap(&style_chart_div, 4);
        lv_style_set_width(&style_chart_pts, 0);
        lv_style_set_height(&style_chart_pts, 0);
        lv_style_set_bg_opa(&style_chart_pts, 0);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if THERMOSTAT_CHECK_COMPILE_TARGET(THERMOSTAT_TARGET_ALL)
    if (thermostat_check_target(THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "insights_#");
        lv_obj_set_width(lv_obj_0, CONST_SCREEN_W);
        lv_obj_set_height(lv_obj_0, CONST_SCREEN_H);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

        lv_obj_add_style(lv_obj_0, &style_screen_base, 0);
        lv_obj_add_style(lv_obj_0, &style_ground_basalt, 0);
        lv_obj_bind_style(lv_obj_0, &style_ground_ember, 0, &subject_hvac_state, REF_STATE_HEATING);
        lv_obj_bind_style(lv_obj_0, &style_ground_glacier, 0, &subject_hvac_state, REF_STATE_COOLING);
        lv_obj_t * back = lv_button_create(lv_obj_0);
        lv_obj_set_name(back, "back");
        lv_obj_remove_style_all(back);
        lv_obj_add_style(back, &style_pos_back, 0);
        lv_obj_add_style(back, &style_glass_circle_sm, 0);
        lv_obj_add_style(back, &style_glass_pressed, LV_STATE_PRESSED);
        lv_obj_add_screen_create_event(back, LV_EVENT_CLICKED, home_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);
        lv_obj_t * lv_image_0 = lv_image_create(back);
        lv_image_set_src(lv_image_0, icon_back);
        lv_obj_set_align(lv_image_0, LV_ALIGN_CENTER);
        lv_obj_set_flag(lv_image_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_image_0, &style_icon_ink, 0);

        lv_obj_t * page_title = label_create(lv_obj_0);
        lv_obj_set_name(page_title, "page_title");
        lv_label_set_text(page_title, "Insights");
        lv_obj_set_x(page_title, 60);
        lv_obj_set_y(page_title, 22);
        lv_obj_add_style(page_title, &style_heading, 0);
        lv_obj_add_style(page_title, &style_ink, 0);

        lv_obj_t * trend = lv_chart_create(lv_obj_0);
        lv_obj_set_name(trend, "trend");
        lv_chart_set_type(trend, LV_CHART_TYPE_LINE);
        lv_chart_set_point_count(trend, 67);
        lv_chart_set_hor_div_line_count(trend, 3);
        lv_chart_set_ver_div_line_count(trend, 0);
        lv_obj_remove_style_all(trend);
        lv_obj_add_style(trend, &style_chart, 0);
        lv_obj_add_style(trend, &style_chart_line, LV_PART_ITEMS);
        lv_obj_add_style(trend, &style_chart_div, LV_PART_MAIN);
        lv_obj_add_style(trend, &style_chart_pts, LV_PART_INDICATOR);
        lv_chart_set_axis_min_value(trend, LV_CHART_AXIS_PRIMARY_Y, 1900);
        lv_chart_set_axis_max_value(trend, LV_CHART_AXIS_PRIMARY_Y, 2260);
        lv_chart_series_t * lv_chart_series_0 = lv_chart_add_series(trend, lv_color_hex(0xFFFFFF), LV_CHART_AXIS_PRIMARY_Y);
        static const int32_t trend_values_0[] = {2050, 2048, 2045, 2042, 2038, 2034, 2030, 2025, 2020, 2015, 2010, 2005, 2000, 1996, 1991, 1986, 1983, 1980, 1980, 1982, 1985, 1989, 1995, 2002, 2010, 2019, 2031, 2043, 2056, 2068, 2080, 2091, 2101, 2112, 2122, 2131, 2140, 2149, 2157, 2165, 2172, 2177, 2180, 2180, 2178, 2174, 2170, 2165, 2160, 2156, 2151, 2146, 2140, 2135, 2130, 2125, 2120, 2115, 2110, 2105, 2100, 2095, 2089, 2083, 2078, 2073, 2070};
        lv_chart_set_series_values(trend, lv_chart_series_0, trend_values_0, 67);

        lv_obj_t * axis_max = label_create(lv_obj_0);
        lv_obj_set_name(axis_max, "axis_max");
        lv_label_set_text(axis_max, "22.6 °C");
        lv_obj_set_x(axis_max, CONST_GUTTER);
        lv_obj_set_y(axis_max, 126);
        lv_obj_set_style_text_align(axis_max, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_add_style(axis_max, &style_micro, 0);
        lv_obj_add_style(axis_max, &style_ink_muted, 0);

        lv_obj_t * axis_min = label_create(lv_obj_0);
        lv_obj_set_name(axis_min, "axis_min");
        lv_label_set_text(axis_min, "19.0 °C");
        lv_obj_set_x(axis_min, CONST_GUTTER);
        lv_obj_set_y(axis_min, 314);
        lv_obj_set_style_text_align(axis_min, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_add_style(axis_min, &style_micro, 0);
        lv_obj_add_style(axis_min, &style_ink_muted, 0);

        lv_obj_t * row_0 = row_create(lv_obj_0, 0, 0, 0, 0, 0, 0, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START);
        lv_obj_set_x(row_0, CONST_GUTTER);
        lv_obj_set_y(row_0, 382);
        lv_obj_set_width(row_0, CONST_CARD_W);
        lv_obj_set_height(row_0, LV_SIZE_CONTENT);
        metric_create(row_0, icon_runtime, "Runtime", &subject_runtime_str, "%s", "3h 12m", "", 96);

        metric_create(row_0, icon_auto, "Cycles", &subject_cycles, "%d", "7", "", 96);

        metric_create(row_0, icon_eco, "Saved", &subject_saving, "%d", "12", "%", 96);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

