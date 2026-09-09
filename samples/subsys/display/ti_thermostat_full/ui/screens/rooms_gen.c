/**
 * @file rooms_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "rooms_gen.h"
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

lv_obj_t * rooms_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        if (rooms == NULL) rooms = lv_obj_create(NULL);
        lv_obj_t * lv_obj_0 = rooms;
        lv_obj_set_name_static(lv_obj_0, "rooms_#");
        lv_obj_set_width(lv_obj_0, CONST_SCREEN_W);
        lv_obj_set_height(lv_obj_0, CONST_SCREEN_H);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_style_bg_color(lv_obj_0, CONST_SURFACE_CANVAS, 0);
        lv_obj_set_style_bg_opa(lv_obj_0, (255 * 100 / 100), 0);
        lv_obj_set_style_bg_grad_color(lv_obj_0, lv_color_hex(0xE9EEF1), 0);
        lv_obj_set_style_bg_grad_dir(lv_obj_0, LV_GRAD_DIR_VER, 0);

        lv_obj_add_style(lv_obj_0, &style_screen_base, 0);
        lv_obj_add_subject_set_int_event(lv_obj_0, &subject_tab, LV_EVENT_SCREEN_LOADED, REF_TAB_ROOMS);
        top_bar_create(lv_obj_0, "Rooms");

        lv_obj_t * column_0 = column_create(lv_obj_0, 0, 0, 0, 0, 12, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_width(column_0, CONST_SCREEN_W);
        lv_obj_set_height(column_0, 380);
        lv_obj_set_y(column_0, 44);
        lv_obj_set_flag(column_0, LV_OBJ_FLAG_SCROLLABLE, true);
        lv_obj_set_scrollbar_mode(column_0, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_style_pad_top(column_0, 8, 0);
        lv_obj_set_style_pad_bottom(column_0, 16, 0);
        lv_obj_t * surface_0 = surface_create(column_0);
        lv_obj_set_flex_flow(surface_0, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_all(surface_0, 12, 0);
        lv_obj_set_style_pad_row(surface_0, 4, 0);
        lv_obj_t * row_0 = row_create(surface_0, 0, 0, 0, 0, 0, 0, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_width(row_0, 264);
        lv_obj_set_height(row_0, LV_SIZE_CONTENT);
        overline_create(row_0, "ZONES");

        lv_obj_t * label_0 = label_create(row_0);
        lv_label_set_text(label_0, "4 sensors");
        lv_obj_set_style_text_color(label_0, CONST_TEXT_SECONDARY, 0);
        lv_obj_add_style(label_0, &style_caption, 0);

        row_room_create(surface_0, "Living room", "Occupied · leading", &subject_room_living_temp, &subject_room_living_demand, &style_bar_heat);

        row_room_create(surface_0, "Study", "Occupied", &subject_room_study_temp, &subject_room_study_demand, &style_bar_heat);

        row_room_create(surface_0, "Bedroom", "Empty since 07:40", &subject_room_bed_temp, &subject_room_bed_demand, &style_bar_neutral);

        row_room_create(surface_0, "Hallway", "No call for heat", &subject_room_hall_temp, &subject_room_hall_demand, &style_bar_neutral);

        lv_obj_t * surface_1 = surface_create(column_0);
        lv_obj_set_flex_flow(surface_1, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_all(surface_1, 12, 0);
        lv_obj_set_style_pad_row(surface_1, 2, 0);
        overline_create(surface_1, "CONNECTED DEVICES");

        row_toggle_create(surface_1, "Circulation fan", "Auto · runs with heat", &subject_dev_fan);

        row_toggle_create(surface_1, "Humidifier", "Target 45% RH", &subject_dev_humidifier);

        row_toggle_create(surface_1, "Bedroom vent", "Open", &subject_dev_vent);

        row_toggle_create(surface_1, "Geofence", "Drop to 17° when nobody is home", &subject_dev_geofence);

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

