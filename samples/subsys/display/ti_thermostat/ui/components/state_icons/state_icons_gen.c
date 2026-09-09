/**
 * @file state_icons_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "state_icons_gen.h"
#include "../../thermostat.h"

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

lv_obj_t * state_icons_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if THERMOSTAT_CHECK_COMPILE_TARGET(THERMOSTAT_TARGET_ALL)
    if (thermostat_check_target(THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * container_0 = container_create(parent);
        lv_obj_set_name_static(container_0, "state_icons_#");
        lv_obj_set_width(container_0, 15);
        lv_obj_set_height(container_0, 15);

        lv_obj_t * lv_image_0 = lv_image_create(container_0);
        lv_image_set_src(lv_image_0, icon_state_heat);
        lv_obj_set_width(lv_image_0, 15);
        lv_obj_set_height(lv_image_0, 15);
        lv_obj_add_style(lv_image_0, &style_icon_ink, 0);
        lv_obj_bind_flag_if_not_eq(lv_image_0, &subject_hvac_state, LV_OBJ_FLAG_HIDDEN, REF_STATE_HEATING);

        lv_obj_t * lv_image_1 = lv_image_create(container_0);
        lv_image_set_src(lv_image_1, icon_state_cool);
        lv_obj_set_width(lv_image_1, 15);
        lv_obj_set_height(lv_image_1, 15);
        lv_obj_add_style(lv_image_1, &style_icon_ink, 0);
        lv_obj_bind_flag_if_not_eq(lv_image_1, &subject_hvac_state, LV_OBJ_FLAG_HIDDEN, REF_STATE_COOLING);

        lv_obj_t * lv_image_2 = lv_image_create(container_0);
        lv_image_set_src(lv_image_2, icon_state_idle);
        lv_obj_set_width(lv_image_2, 15);
        lv_obj_set_height(lv_image_2, 15);
        lv_obj_add_style(lv_image_2, &style_icon_ink, 0);
        lv_obj_bind_flag_if_not_eq(lv_image_2, &subject_hvac_state, LV_OBJ_FLAG_HIDDEN, REF_STATE_IDLE);

        the_root = container_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

