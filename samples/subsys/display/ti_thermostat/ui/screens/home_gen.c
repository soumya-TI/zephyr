/**
 * @file home_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "home_gen.h"
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

lv_obj_t * home_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_field;
    static lv_style_t style_full;
    static lv_style_t style_marker;
    static lv_style_t style_step_btn;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_field);
        lv_style_init(&style_full);
        lv_style_init(&style_marker);
        lv_style_init(&style_step_btn);

        lv_style_set_width(&style_field, CONST_SCREEN_W);
        lv_style_set_height(&style_field, CONST_SCREEN_H);
        lv_style_set_radius(&style_field, 0);
        lv_style_set_border_width(&style_field, 0);
        lv_style_set_pad_all(&style_field, 0);
        lv_style_set_shadow_width(&style_field, 0);
        lv_style_set_shadow_opa(&style_field, 0);
        lv_style_set_width(&style_full, CONST_SCREEN_W);
        lv_style_set_height(&style_full, CONST_SCREEN_H);
        lv_style_set_line_width(&style_marker, 1);
        lv_style_set_line_color(&style_marker, CONST_INK);
        lv_style_set_line_opa(&style_marker, 150);
        lv_style_set_line_dash_width(&style_marker, 4);
        lv_style_set_line_dash_gap(&style_marker, 5);
        lv_style_set_width(&style_step_btn, CONST_TOUCH);
        lv_style_set_height(&style_step_btn, CONST_TOUCH);
        lv_style_set_radius(&style_step_btn, CONST_RADIUS_PILL);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if THERMOSTAT_CHECK_COMPILE_TARGET(THERMOSTAT_TARGET_ALL)
    if (thermostat_check_target(THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "home_#");
        lv_obj_set_width(lv_obj_0, CONST_SCREEN_W);
        lv_obj_set_height(lv_obj_0, CONST_SCREEN_H);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

        lv_obj_add_style(lv_obj_0, &style_screen_base, 0);
        lv_obj_add_event_cb(lv_obj_0, thermostat_drag_attach_cb, LV_EVENT_SCREEN_LOADED, NULL);
        lv_obj_add_event_cb(lv_obj_0, thermostat_marker_attach_cb, LV_EVENT_SCREEN_LOADED, NULL);
        lv_obj_t * field = lv_bar_create(lv_obj_0);
        lv_obj_set_name(field, "field");
        lv_bar_set_orientation(field, LV_BAR_ORIENTATION_VERTICAL);
        lv_bar_set_min_value(field, CONST_LEVEL_MIN);
        lv_bar_set_max_value(field, CONST_LEVEL_MAX);
        lv_bar_bind_value(field, &subject_field_level);
        lv_obj_set_x(field, 0);
        lv_obj_set_y(field, 0);
        lv_obj_remove_style_all(field);
        lv_obj_add_style(field, &style_field, 0);
        lv_obj_add_style(field, &style_field_track_basalt, 0);
        lv_obj_add_style(field, &style_field_fill_basalt, LV_PART_INDICATOR);
        lv_obj_bind_style(field, &style_field_track_ember, LV_PART_MAIN, &subject_hvac_state, REF_STATE_HEATING);
        lv_obj_bind_style(field, &style_field_fill_ember, LV_PART_INDICATOR, &subject_hvac_state, REF_STATE_HEATING);
        lv_obj_bind_style(field, &style_field_track_glacier, LV_PART_MAIN, &subject_hvac_state, REF_STATE_COOLING);
        lv_obj_bind_style(field, &style_field_fill_glacier, LV_PART_INDICATOR, &subject_hvac_state, REF_STATE_COOLING);

        lv_obj_t * temp_marker = lv_line_create(lv_obj_0);
        lv_obj_set_name(temp_marker, "temp_marker");
        static const lv_point_precise_t temp_marker_points_0[] = {{0, 0}, {320, 0}};
        lv_line_set_points(temp_marker, temp_marker_points_0, 2);
        lv_obj_set_x(temp_marker, 0);
        lv_obj_set_y(temp_marker, 240);
        lv_obj_set_flag(temp_marker, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(temp_marker, &style_marker, 0);

        lv_obj_t * drag_layer = lv_obj_create(lv_obj_0);
        lv_obj_set_name(drag_layer, "drag_layer");
        lv_obj_set_x(drag_layer, 0);
        lv_obj_set_y(drag_layer, 0);
        lv_obj_set_flag(drag_layer, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(drag_layer, LV_OBJ_FLAG_CLICKABLE, true);
        lv_obj_remove_style_all(drag_layer);
        lv_obj_add_style(drag_layer, &style_full, 0);
        lv_obj_add_event_cb(drag_layer, thermostat_drag_cb, LV_EVENT_ALL, NULL);

        lv_obj_t * hero = row_create(lv_obj_0, 0, 0, 0, 0, 0, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_name(hero, "hero");
        lv_obj_set_align(hero, LV_ALIGN_CENTER);
        lv_obj_set_width(hero, LV_SIZE_CONTENT);
        lv_obj_set_height(hero, LV_SIZE_CONTENT);
        lv_obj_set_flag(hero, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_t * digit_tens = lv_roller_create(hero);
        lv_obj_set_name(digit_tens, "digit_tens");
        lv_obj_set_style_text_align(digit_tens, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_set_style_text_font(digit_tens, font_hero_72, 0);
        lv_obj_set_style_text_line_space(digit_tens, 30, 0);
        lv_roller_set_options(digit_tens, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
        lv_roller_set_visible_row_count(digit_tens, 1);
        lv_roller_bind_value(digit_tens, &subject_digit_tens);
        lv_obj_set_flag(digit_tens, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_width(digit_tens, 40);
        lv_obj_add_style(digit_tens, &style_roller, 0);
        lv_obj_add_style(digit_tens, &style_roller_sel, LV_PART_SELECTED);

        lv_obj_t * digit_units = lv_roller_create(hero);
        lv_obj_set_name(digit_units, "digit_units");
        lv_obj_set_style_text_align(digit_units, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_font(digit_units, font_hero_72, 0);
        lv_obj_set_style_text_line_space(digit_units, 30, 0);
        lv_roller_set_options(digit_units, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_INFINITE);
        lv_roller_set_visible_row_count(digit_units, 1);
        lv_roller_bind_value(digit_units, &subject_digit_units);
        lv_obj_set_flag(digit_units, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_width(digit_units, 40);
        lv_obj_add_style(digit_units, &style_roller, 0);
        lv_obj_add_style(digit_units, &style_roller_sel, LV_PART_SELECTED);

        lv_obj_t * label_0 = label_create(hero);
        lv_label_set_text(label_0, "°");
        lv_obj_set_style_translate_y(label_0, 6, 0);
        lv_obj_set_style_translate_x(label_0, 0, 0);
        lv_obj_set_flag(label_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(label_0, &style_stat_lg, 0);
        lv_obj_add_style(label_0, &style_ink_secondary, 0);

        lv_obj_t * content = column_create(lv_obj_0, 0, 0, 0, 0, 0, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_name(content, "content");
        lv_obj_set_width(content, CONST_SCREEN_W);
        lv_obj_set_height(content, CONST_SCREEN_H);
        lv_obj_set_flag(content, LV_OBJ_FLAG_CLICKABLE, false);
        top_bar_create(content, "Living Room", "Drag anywhere to set");

        lv_obj_t * hero_space = container_create(content);
        lv_obj_set_name(hero_space, "hero_space");
        lv_obj_set_width(hero_space, CONST_SCREEN_W);
        lv_obj_set_flex_grow(hero_space, 1);
        lv_obj_set_flag(hero_space, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * row_0 = row_create(content, 0, 0, 0, 40, CONST_SPACE_4, 0, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END);
        lv_obj_set_width(row_0, CONST_CARD_W);
        lv_obj_set_height(row_0, LV_SIZE_CONTENT);
        lv_obj_set_flag(row_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_t * label_1 = label_create(row_0);
        lv_label_set_text(label_1, "Room at");
        lv_obj_add_style(label_1, &style_body, 0);
        lv_obj_add_style(label_1, &style_ink_muted, 0);

        lv_obj_t * label_2 = label_create(row_0);
        lv_label_bind_text(label_2, &subject_indoor_str, NULL);
        lv_label_set_text(label_2, "20.2");
        lv_obj_add_style(label_2, &style_body_strong, 0);
        lv_obj_add_style(label_2, &style_ink, 0);

        lv_obj_t * label_3 = label_create(row_0);
        lv_label_set_text(label_3, "°C with");
        lv_obj_add_style(label_3, &style_body, 0);
        lv_obj_add_style(label_3, &style_ink_muted, 0);

        lv_obj_t * label_4 = label_create(row_0);
        lv_label_bind_text(label_4, &subject_humidity, "%d%%");
        lv_label_set_text(label_4, "44%");
        lv_obj_add_style(label_4, &style_body_strong, 0);
        lv_obj_add_style(label_4, &style_ink, 0);

        lv_obj_t * label_5 = label_create(row_0);
        lv_label_set_text(label_5, "humidity");
        lv_obj_add_style(label_5, &style_body, 0);
        lv_obj_add_style(label_5, &style_ink_muted, 0);

        lv_obj_t * row_1 = row_create(content, 0, 0, 0, CONST_SPACE_20, 0, 0, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_width(row_1, CONST_CARD_W);
        lv_obj_set_height(row_1, LV_SIZE_CONTENT);
        lv_obj_t * nav_button_0 = nav_button_create(row_1, icon_climate);
        lv_obj_add_screen_create_event(nav_button_0, LV_EVENT_CLICKED, climate_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * logostrip = lv_image_create(row_1);
        lv_obj_set_name(logostrip, "logostrip");
        lv_image_set_src(logostrip, image_logostrip);
        lv_obj_set_width(logostrip, 75);
        lv_obj_set_height(logostrip, 20);
        lv_obj_set_flag(logostrip, LV_OBJ_FLAG_CLICKABLE, false);

        lv_obj_t * nav_button_1 = nav_button_create(row_1, icon_insights);
        lv_obj_add_screen_create_event(nav_button_1, LV_EVENT_CLICKED, insights_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

