/**
 * @file card_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "card_gen.h"
#include "../../../motor_control.h"

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

lv_obj_t * card_create(lv_obj_t * parent, int32_t pad, int32_t gap)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_clip;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_clip);

        /* clip_corner forces LVGL to render the card's children into an
         * intermediate ARGB8888 layer buffer (up to tens of KB) allocated from
         * the LVGL object pool at draw time. On the MSPM0G3519's 60 KB pool that
         * allocation fails and — with LVGL logging off and LV_ASSERT_MALLOC on —
         * the UI wedges silently, half-drawn. The cards' content is padded and
         * never reaches the rounded corners, so disabling child clipping is
         * visually identical here and removes the layer allocation entirely. */
        lv_style_set_clip_corner(&style_clip, false);
        lv_style_set_layout(&style_clip, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_clip, LV_FLEX_FLOW_COLUMN);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if MOTOR_CONTROL_CHECK_COMPILE_TARGET(MOTOR_CONTROL_TARGET_ALL)
    if (motor_control_check_target(MOTOR_CONTROL_TARGET_ALL)) {
        lv_obj_t * panel_0 = panel_create(parent);
        lv_obj_set_name_static(panel_0, "card_#");
        lv_obj_set_width(panel_0, CONST_CONTENT_W);
        lv_obj_set_height(panel_0, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_all(panel_0, pad, 0);
        lv_obj_set_style_pad_row(panel_0, gap, 0);

        lv_obj_add_style(panel_0, &style_card, 0);
        lv_obj_add_style(panel_0, &style_clip, 0);

        the_root = panel_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

