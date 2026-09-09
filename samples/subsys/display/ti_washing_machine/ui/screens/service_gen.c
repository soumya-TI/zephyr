/**
 * @file service_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "service_gen.h"
#include "../washing_machine.h"

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

lv_obj_t * service_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if WASHING_MACHINE_CHECK_COMPILE_TARGET(WASHING_MACHINE_TARGET_ALL)
    if (washing_machine_check_target(WASHING_MACHINE_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "service_#");
        lv_obj_set_width(lv_obj_0, CONST_SCREEN_W);
        lv_obj_set_height(lv_obj_0, CONST_SCREEN_H);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

        lv_obj_add_style(lv_obj_0, &style_screen_base, 0);
        nav_header_create(lv_obj_0, "Service");

        lv_obj_t * btn_back_0 = btn_back_create(lv_obj_0);
        lv_obj_set_x(btn_back_0, 0);
        lv_obj_set_y(btn_back_0, 0);
        lv_obj_add_screen_create_event(btn_back_0, LV_EVENT_CLICKED, home_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

        lv_obj_t * surface_0 = surface_create(lv_obj_0, 252, 178, 14, 12);
        lv_obj_set_x(surface_0, CONST_GUTTER);
        lv_obj_set_y(surface_0, 64);
        overline_create(surface_0, "COMPONENT TEST");

        row_metric_create(surface_0, "Drum motor", icon_rpm, &subject_pwm_duty, "%d", "74", "% power", 224);

        lv_obj_t * slider_0 = slider_create(surface_0);
        lv_obj_set_width(slider_0, 224);
        lv_slider_set_min_value(slider_0, 0);
        lv_slider_set_max_value(slider_0, 100);
        lv_slider_bind_value(slider_0, &subject_pwm_duty);
        lv_obj_set_ext_click_area(slider_0, 18);

        row_metric_create(surface_0, "Water fill", icon_water, &subject_water_target, "%d", "62", "% level", 224);

        lv_obj_t * slider_1 = slider_create(surface_0);
        lv_obj_set_width(slider_1, 224);
        lv_slider_set_min_value(slider_1, 0);
        lv_slider_set_max_value(slider_1, 100);
        lv_slider_bind_value(slider_1, &subject_water_target);
        lv_obj_set_ext_click_area(slider_1, 18);

        lv_obj_t * column_0 = column_create(lv_obj_0, 0, 0, 0, 0, 2, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_x(column_0, CONST_GUTTER);
        lv_obj_set_y(column_0, 252);
        lv_obj_t * row_0 = row_create(column_0, 0, 0, 0, 0, 7, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
        lv_obj_t * lv_image_0 = lv_image_create(row_0);
        lv_image_set_src(lv_image_0, icon_board);
        lv_obj_set_flag(lv_image_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_image_0, &style_icon_3, 0);

        lv_obj_t * label_0 = label_create(row_0);
        lv_label_set_text(label_0, "Model WM-480 . firmware 1.4.2");
        lv_obj_add_style(label_0, &style_micro, 0);
        lv_obj_add_style(label_0, &style_ink_3, 0);

        lv_obj_t * label_1 = label_create(column_0);
        lv_label_set_text(label_1, "1284 cycles . last service 96 days ago");
        lv_obj_add_style(label_1, &style_micro, 0);
        lv_obj_add_style(label_1, &style_ink_3, 0);

        lv_obj_t * surface_1 = surface_create(lv_obj_0, 176, 206, 12, 4);
        lv_obj_set_x(surface_1, 284);
        lv_obj_set_y(surface_1, 64);
        overline_create(surface_1, "STATUS");

        io_row_create(surface_1, "Door interlock", "Locked", "Unlocked", &subject_door, 152);

        io_row_create(surface_1, "Heater", "Heating", "Off", &subject_heater, 152);

        io_row_create(surface_1, "Drain pump", "Running", "Idle", &subject_pump, 152);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

