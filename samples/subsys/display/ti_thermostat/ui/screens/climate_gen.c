/**
 * @file climate_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "climate_gen.h"
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

lv_obj_t * climate_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_pos_back;
    static lv_style_t style_row;
    static lv_style_t style_mode_btn;
    static lv_style_t style_switch_pos;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_pos_back);
        lv_style_init(&style_row);
        lv_style_init(&style_mode_btn);
        lv_style_init(&style_switch_pos);

        lv_style_set_x(&style_pos_back, 0);
        lv_style_set_y(&style_pos_back, CONST_SPACE_8);
        lv_style_set_width(&style_row, CONST_CARD_W);
        lv_style_set_height(&style_row, 72);
        lv_style_set_radius(&style_row, CONST_RADIUS_LG);
        lv_style_set_pad_left(&style_row, 18);
        lv_style_set_pad_right(&style_row, 18);
        lv_style_set_pad_column(&style_row, 14);
        lv_style_set_layout(&style_row, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_row, LV_FLEX_FLOW_ROW);
        lv_style_set_flex_main_place(&style_row, LV_FLEX_ALIGN_START);
        lv_style_set_flex_cross_place(&style_row, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_track_place(&style_row, LV_FLEX_ALIGN_CENTER);
        lv_style_set_width(&style_mode_btn, 140);
        lv_style_set_height(&style_mode_btn, 104);
        lv_style_set_radius(&style_mode_btn, CONST_RADIUS_LG);
        lv_style_set_pad_all(&style_mode_btn, 16);
        lv_style_set_pad_row(&style_mode_btn, 10);
        lv_style_set_layout(&style_mode_btn, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_mode_btn, LV_FLEX_FLOW_COLUMN);
        lv_style_set_flex_main_place(&style_mode_btn, LV_FLEX_ALIGN_SPACE_BETWEEN);
        lv_style_set_flex_cross_place(&style_mode_btn, LV_FLEX_ALIGN_START);
        lv_style_set_flex_track_place(&style_mode_btn, LV_FLEX_ALIGN_START);
        lv_style_set_width(&style_switch_pos, 52);
        lv_style_set_height(&style_switch_pos, 30);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if THERMOSTAT_CHECK_COMPILE_TARGET(THERMOSTAT_TARGET_ALL)
    if (thermostat_check_target(THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "climate_#");
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
        lv_label_set_text(page_title, "Climate");
        lv_obj_set_x(page_title, 60);
        lv_obj_set_y(page_title, 22);
        lv_obj_add_style(page_title, &style_heading, 0);
        lv_obj_add_style(page_title, &style_ink, 0);

        lv_obj_t * column_0 = column_create(lv_obj_0, 0, 0, 0, 0, CONST_SPACE_16, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_x(column_0, CONST_GUTTER);
        lv_obj_set_y(column_0, 96);
        lv_obj_set_width(column_0, CONST_CARD_W);
        lv_obj_set_height(column_0, LV_SIZE_CONTENT);
        lv_obj_t * auto_row = container_create(column_0);
        lv_obj_set_name(auto_row, "auto_row");
        lv_obj_add_style(auto_row, &style_row, 0);
        lv_obj_add_style(auto_row, &style_glass, 0);
        lv_obj_bind_style(auto_row, &style_selected, 0, &subject_hvac_mode, REF_MODE_AUTO);
        lv_obj_t * lv_image_1 = lv_image_create(auto_row);
        lv_image_set_src(lv_image_1, icon_auto);
        lv_obj_set_width(lv_image_1, 20);
        lv_obj_set_height(lv_image_1, 20);
        lv_obj_add_style(lv_image_1, &style_icon_ink, 0);

        lv_obj_t * column_1 = column_create(auto_row, 0, 0, 0, 0, 2, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_width(column_1, 140);
        lv_obj_set_height(column_1, LV_SIZE_CONTENT);
        lv_obj_t * label_0 = label_create(column_1);
        lv_label_set_text(label_0, "Auto");
        lv_obj_add_style(label_0, &style_subtitle, 0);
        lv_obj_add_style(label_0, &style_ink, 0);

        lv_obj_t * label_1 = label_create(column_1);
        lv_label_set_text(label_1, "Follows the weather");
        lv_obj_add_style(label_1, &style_caption, 0);
        lv_obj_add_style(label_1, &style_ink_muted, 0);

        lv_obj_t * auto_on = lv_switch_create(auto_row);
        lv_obj_set_name(auto_on, "auto_on");
        lv_obj_set_state(auto_on, LV_STATE_CHECKED, true);
        lv_obj_add_style(auto_on, &style_switch_pos, 0);
        lv_obj_add_style(auto_on, &style_switch, 0);
        lv_obj_add_style(auto_on, &style_switch_ind, LV_PART_INDICATOR);
        lv_obj_add_style(auto_on, &style_switch_knob, LV_PART_KNOB);
        lv_obj_bind_flag_if_not_eq(auto_on, &subject_hvac_mode, LV_OBJ_FLAG_HIDDEN, REF_MODE_AUTO);
        lv_obj_add_subject_set_int_event(auto_on, &subject_hvac_mode, LV_EVENT_CLICKED, REF_MODE_HEAT);

        lv_obj_t * auto_off = lv_switch_create(auto_row);
        lv_obj_set_name(auto_off, "auto_off");
        lv_obj_add_style(auto_off, &style_switch_pos, 0);
        lv_obj_add_style(auto_off, &style_switch, 0);
        lv_obj_add_style(auto_off, &style_switch_ind, LV_PART_INDICATOR);
        lv_obj_add_style(auto_off, &style_switch_knob, LV_PART_KNOB);
        lv_obj_bind_flag_if_eq(auto_off, &subject_hvac_mode, LV_OBJ_FLAG_HIDDEN, REF_MODE_AUTO);
        lv_obj_add_subject_set_int_event(auto_off, &subject_hvac_mode, LV_EVENT_CLICKED, REF_MODE_AUTO);

        lv_obj_t * row_0 = row_create(column_0, 0, 0, 0, 0, CONST_SPACE_8, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_width(row_0, CONST_CARD_W);
        lv_obj_set_height(row_0, LV_SIZE_CONTENT);
        lv_obj_t * btn_heat = lv_button_create(row_0);
        lv_obj_set_name(btn_heat, "btn_heat");
        lv_obj_remove_style_all(btn_heat);
        lv_obj_add_style(btn_heat, &style_mode_btn, 0);
        lv_obj_add_style(btn_heat, &style_glass, 0);
        lv_obj_add_style(btn_heat, &style_glass_pressed, LV_STATE_PRESSED);
        lv_obj_bind_style(btn_heat, &style_selected, 0, &subject_hvac_mode, REF_MODE_HEAT);
        lv_obj_add_subject_set_int_event(btn_heat, &subject_hvac_mode, LV_EVENT_CLICKED, REF_MODE_HEAT);
        lv_obj_t * lv_image_2 = lv_image_create(btn_heat);
        lv_image_set_src(lv_image_2, icon_heat);
        lv_obj_set_width(lv_image_2, 22);
        lv_obj_set_height(lv_image_2, 22);
        lv_obj_set_flag(lv_image_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_image_2, &style_icon_ink, 0);

        lv_obj_t * label_2 = label_create(btn_heat);
        lv_label_set_text(label_2, "Heat");
        lv_obj_set_flag(label_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(label_2, &style_subtitle, 0);
        lv_obj_add_style(label_2, &style_ink, 0);

        lv_obj_t * btn_cool = lv_button_create(row_0);
        lv_obj_set_name(btn_cool, "btn_cool");
        lv_obj_remove_style_all(btn_cool);
        lv_obj_add_style(btn_cool, &style_mode_btn, 0);
        lv_obj_add_style(btn_cool, &style_glass, 0);
        lv_obj_add_style(btn_cool, &style_glass_pressed, LV_STATE_PRESSED);
        lv_obj_bind_style(btn_cool, &style_selected, 0, &subject_hvac_mode, REF_MODE_COOL);
        lv_obj_add_subject_set_int_event(btn_cool, &subject_hvac_mode, LV_EVENT_CLICKED, REF_MODE_COOL);
        lv_obj_t * lv_image_3 = lv_image_create(btn_cool);
        lv_image_set_src(lv_image_3, icon_cool);
        lv_obj_set_width(lv_image_3, 22);
        lv_obj_set_height(lv_image_3, 22);
        lv_obj_set_flag(lv_image_3, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_image_3, &style_icon_ink, 0);

        lv_obj_t * label_3 = label_create(btn_cool);
        lv_label_set_text(label_3, "Cool");
        lv_obj_set_flag(label_3, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(label_3, &style_subtitle, 0);
        lv_obj_add_style(label_3, &style_ink, 0);

        lv_obj_t * note_auto = container_create(column_0);
        lv_obj_set_name(note_auto, "note_auto");
        lv_obj_set_width(note_auto, CONST_CARD_W);
        lv_obj_set_height(note_auto, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_all(note_auto, CONST_SPACE_16, 0);
        lv_obj_add_style(note_auto, &style_glass, 0);
        lv_obj_bind_flag_if_not_eq(note_auto, &subject_hvac_mode, LV_OBJ_FLAG_HIDDEN, REF_MODE_AUTO);
        lv_obj_t * column_2 = column_create(note_auto, 0, 0, 0, 0, CONST_SPACE_8, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_width(column_2, lv_pct(100));
        lv_obj_set_height(column_2, LV_SIZE_CONTENT);
        lv_obj_t * label_4 = label_create(column_2);
        lv_label_set_text(label_4, "Heats when it is colder outside than your setpoint, cools when it is warmer.");
        lv_obj_set_width(label_4, lv_pct(100));
        lv_obj_set_style_text_line_space(label_4, 3, 0);
        lv_obj_add_style(label_4, &style_body, 0);
        lv_obj_add_style(label_4, &style_ink_secondary, 0);

        lv_obj_t * row_1 = row_create(column_2, 0, 0, 0, 0, CONST_SPACE_4, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END);
        lv_obj_t * label_5 = label_create(row_1);
        lv_label_set_text(label_5, "Outside now");
        lv_obj_add_style(label_5, &style_caption, 0);
        lv_obj_add_style(label_5, &style_ink_muted, 0);

        lv_obj_t * label_6 = label_create(row_1);
        lv_label_bind_text(label_6, &subject_outdoor_str, NULL);
        lv_label_set_text(label_6, "11.8");
        lv_obj_add_style(label_6, &style_body_strong, 0);
        lv_obj_add_style(label_6, &style_ink, 0);

        lv_obj_t * label_7 = label_create(row_1);
        lv_label_set_text(label_7, "°C");
        lv_obj_add_style(label_7, &style_caption, 0);
        lv_obj_add_style(label_7, &style_ink_muted, 0);

        lv_obj_t * note_manual = container_create(column_0);
        lv_obj_set_name(note_manual, "note_manual");
        lv_obj_set_width(note_manual, CONST_CARD_W);
        lv_obj_set_height(note_manual, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_all(note_manual, CONST_SPACE_16, 0);
        lv_obj_add_style(note_manual, &style_glass, 0);
        lv_obj_bind_flag_if_eq(note_manual, &subject_hvac_mode, LV_OBJ_FLAG_HIDDEN, REF_MODE_AUTO);
        lv_obj_t * column_3 = column_create(note_manual, 0, 0, 0, 0, CONST_SPACE_8, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_width(column_3, lv_pct(100));
        lv_obj_set_height(column_3, LV_SIZE_CONTENT);
        lv_obj_t * label_8 = label_create(column_3);
        lv_label_set_text(label_8, "Runs only in the direction you picked, and rests once the room is within a degree of target.");
        lv_obj_set_width(label_8, lv_pct(100));
        lv_obj_set_style_text_line_space(label_8, 3, 0);
        lv_obj_add_style(label_8, &style_body, 0);
        lv_obj_add_style(label_8, &style_ink_secondary, 0);

        lv_obj_t * row_2 = row_create(column_3, 0, 0, 0, 0, CONST_SPACE_4, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END);
        lv_obj_t * label_9 = label_create(row_2);
        lv_label_set_text(label_9, "Target");
        lv_obj_add_style(label_9, &style_caption, 0);
        lv_obj_add_style(label_9, &style_ink_muted, 0);

        lv_obj_t * label_10 = label_create(row_2);
        lv_label_bind_text(label_10, &subject_setpoint, "%d");
        lv_label_set_text(label_10, "21");
        lv_obj_add_style(label_10, &style_body_strong, 0);
        lv_obj_add_style(label_10, &style_ink, 0);

        lv_obj_t * label_11 = label_create(row_2);
        lv_label_set_text(label_11, "°C");
        lv_obj_add_style(label_11, &style_caption, 0);
        lv_obj_add_style(label_11, &style_ink_muted, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

