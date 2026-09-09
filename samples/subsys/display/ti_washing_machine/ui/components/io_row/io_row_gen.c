/**
 * @file io_row_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "io_row_gen.h"
#include "../../washing_machine.h"

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

lv_obj_t * io_row_create(lv_obj_t * parent, const char * title, const char * on_text, const char * off_text, lv_subject_t * state, int32_t w)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_dot;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_dot);

        lv_style_set_width(&style_dot, 9);
        lv_style_set_height(&style_dot, 9);
        lv_style_set_radius(&style_dot, 32767);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if WASHING_MACHINE_CHECK_COMPILE_TARGET(WASHING_MACHINE_TARGET_ALL)
    if (washing_machine_check_target(WASHING_MACHINE_TARGET_ALL)) {
        lv_obj_t * lv_button_0 = lv_button_create(parent);
        lv_obj_set_name_static(lv_button_0, "io_row_#");
        lv_obj_set_width(lv_button_0, w);
        lv_obj_set_height(lv_button_0, LV_SIZE_CONTENT);
        lv_obj_set_flag(lv_button_0, LV_OBJ_FLAG_STATE_TRICKLE, true);
        lv_obj_set_style_bg_opa(lv_button_0, 0, 0);
        lv_obj_set_style_border_width(lv_button_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_button_0, 0, 0);
        lv_obj_set_style_pad_top(lv_button_0, 7, 0);
        lv_obj_set_style_pad_bottom(lv_button_0, 7, 0);
        lv_obj_set_style_pad_left(lv_button_0, 0, 0);
        lv_obj_set_style_pad_right(lv_button_0, 0, 0);
        lv_obj_set_style_layout(lv_button_0, LV_LAYOUT_FLEX, 0);
        lv_obj_set_style_flex_flow(lv_button_0, LV_FLEX_FLOW_ROW, 0);
        lv_obj_set_style_flex_main_place(lv_button_0, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
        lv_obj_set_style_flex_cross_place(lv_button_0, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_track_place(lv_button_0, LV_FLEX_ALIGN_CENTER, 0);

        lv_obj_add_subject_toggle_event(lv_button_0, state, LV_EVENT_CLICKED);
        lv_obj_t * column_0 = column_create(lv_button_0, 0, 0, 0, 0, 1, 0, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_t * label_0 = label_create(column_0);
        lv_label_set_text(label_0, title);
        lv_obj_set_flag(label_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(label_0, &style_body_strong, 0);
        lv_obj_add_style(label_0, &style_ink, 0);

        lv_obj_t * label_1 = label_create(column_0);
        lv_label_set_text(label_1, on_text);
        lv_obj_set_flag(label_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(label_1, &style_micro, 0);
        lv_obj_add_style(label_1, &style_ink_accent, 0);
        lv_obj_bind_flag_if_not_eq(label_1, state, LV_OBJ_FLAG_HIDDEN, REF_ON);

        lv_obj_t * label_2 = label_create(column_0);
        lv_label_set_text(label_2, off_text);
        lv_obj_set_flag(label_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(label_2, &style_micro, 0);
        lv_obj_add_style(label_2, &style_ink_3, 0);
        lv_obj_bind_flag_if_not_eq(label_2, state, LV_OBJ_FLAG_HIDDEN, REF_OFF);

        lv_obj_t * lv_obj_0 = lv_obj_create(lv_button_0);
        lv_obj_set_width(lv_obj_0, 9);
        lv_obj_set_height(lv_obj_0, 9);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_radius(lv_obj_0, 32767, 0);
        lv_obj_set_style_border_width(lv_obj_0, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_0, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
        lv_obj_add_style(lv_obj_0, &style_chip_dot_accent, 0);
        lv_obj_bind_flag_if_not_eq(lv_obj_0, state, LV_OBJ_FLAG_HIDDEN, REF_ON);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_button_0);
        lv_obj_set_width(lv_obj_1, 9);
        lv_obj_set_height(lv_obj_1, 9);
        lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_set_style_radius(lv_obj_1, 32767, 0);
        lv_obj_set_style_border_width(lv_obj_1, 0, 0);
        lv_obj_set_style_shadow_width(lv_obj_1, 0, 0);
        lv_obj_set_style_pad_all(lv_obj_1, 0, 0);
        lv_obj_add_style(lv_obj_1, &style_chip_dot_neutral, 0);
        lv_obj_bind_flag_if_not_eq(lv_obj_1, state, LV_OBJ_FLAG_HIDDEN, REF_OFF);

        the_root = lv_button_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

