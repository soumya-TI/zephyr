/**
 * @file card_suggest_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "card_suggest_gen.h"
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

lv_obj_t * card_suggest_create(lv_obj_t * parent, const char * title, const char * body, lv_subject_t * dismissed)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_primary;
    static lv_style_t style_primary_pressed;
    static lv_style_t style_secondary;
    static lv_style_t style_secondary_pressed;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_primary);
        lv_style_init(&style_primary_pressed);
        lv_style_init(&style_secondary);
        lv_style_init(&style_secondary_pressed);

        lv_style_set_width(&style_primary, 96);
        lv_style_set_height(&style_primary, 36);
        lv_style_set_radius(&style_primary, 32767);
        lv_style_set_bg_color(&style_primary, CONST_TEXT_PRIMARY);
        lv_style_set_bg_opa(&style_primary, (255 * 100 / 100));
        lv_style_set_border_width(&style_primary, 0);
        lv_style_set_pad_all(&style_primary, 0);
        lv_style_set_layout(&style_primary, LV_LAYOUT_FLEX);
        lv_style_set_flex_main_place(&style_primary, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_primary, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_track_place(&style_primary, LV_FLEX_ALIGN_CENTER);
        lv_style_set_bg_color(&style_primary_pressed, CONST_INK_PRESSED);
        lv_style_set_width(&style_secondary, 80);
        lv_style_set_height(&style_secondary, 36);
        lv_style_set_radius(&style_secondary, 32767);
        lv_style_set_bg_opa(&style_secondary, 0);
        lv_style_set_border_width(&style_secondary, 0);
        lv_style_set_shadow_width(&style_secondary, 0);
        lv_style_set_pad_all(&style_secondary, 0);
        lv_style_set_layout(&style_secondary, LV_LAYOUT_FLEX);
        lv_style_set_flex_main_place(&style_secondary, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_cross_place(&style_secondary, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_track_place(&style_secondary, LV_FLEX_ALIGN_CENTER);
        lv_style_set_bg_color(&style_secondary_pressed, CONST_DIVIDER);
        lv_style_set_bg_opa(&style_secondary_pressed, (255 * 100 / 100));

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * surface_0 = surface_create(parent);
        lv_obj_set_name_static(surface_0, "card_suggest_#");
        lv_obj_set_flex_flow(surface_0, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_row(surface_0, 8, 0);
        lv_obj_set_style_pad_all(surface_0, 14, 0);

        lv_obj_bind_flag_if_eq(surface_0, dismissed, LV_OBJ_FLAG_HIDDEN, REF_ON);
        overline_create(surface_0, "SUGGESTED");

        lv_obj_t * label_0 = label_create(surface_0);
        lv_label_set_text(label_0, title);
        lv_obj_set_style_text_color(label_0, CONST_TEXT_PRIMARY, 0);
        lv_obj_set_style_pad_top(label_0, 2, 0);
        lv_obj_add_style(label_0, &style_subtitle, 0);

        lv_obj_t * label_1 = label_create(surface_0);
        lv_label_set_text(label_1, body);
        lv_obj_set_style_text_color(label_1, CONST_TEXT_SECONDARY, 0);
        lv_obj_set_width(label_1, 236);
        lv_label_set_long_mode(label_1, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_add_style(label_1, &style_caption, 0);

        lv_obj_t * row_0 = row_create(surface_0, 0, 0, 0, 0, 8, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_width(row_0, LV_SIZE_CONTENT);
        lv_obj_set_height(row_0, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_top(row_0, 4, 0);
        lv_obj_t * lv_button_0 = lv_button_create(row_0);
        lv_obj_remove_style_all(lv_button_0);
        lv_obj_add_style(lv_button_0, &style_primary, 0);
        lv_obj_add_style(lv_button_0, &style_primary_pressed, LV_STATE_PRESSED);
        lv_obj_add_subject_set_int_event(lv_button_0, dismissed, LV_EVENT_CLICKED, REF_ON);
        lv_obj_t * label_2 = label_create(lv_button_0);
        lv_label_set_text(label_2, "Add to schedule");
        lv_obj_set_style_text_color(label_2, CONST_ON_ACCENT, 0);
        lv_obj_set_flag(label_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(label_2, &style_badge, 0);

        lv_obj_t * lv_button_1 = lv_button_create(row_0);
        lv_obj_remove_style_all(lv_button_1);
        lv_obj_add_style(lv_button_1, &style_secondary, 0);
        lv_obj_add_style(lv_button_1, &style_secondary_pressed, LV_STATE_PRESSED);
        lv_obj_add_subject_set_int_event(lv_button_1, dismissed, LV_EVENT_CLICKED, REF_ON);
        lv_obj_t * label_3 = label_create(lv_button_1);
        lv_label_set_text(label_3, "Dismiss");
        lv_obj_set_style_text_color(label_3, CONST_TEXT_SECONDARY, 0);
        lv_obj_set_flag(label_3, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(label_3, &style_badge, 0);

        the_root = surface_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

