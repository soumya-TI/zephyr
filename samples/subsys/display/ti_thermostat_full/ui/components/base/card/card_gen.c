/**
 * @file card_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "card_gen.h"
#include "../../../ti_thermostat.h"

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

lv_obj_t * card_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_card;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_card);

        /* clip_corner makes LVGL render this card's children into an
         * intermediate ARGB8888 layer buffer (tens of KB) taken from the LVGL
         * object pool at DRAW time. The Insights screen is the only one that
         * builds cards (surface + stat_tile both wrap card_create); with the
         * home + insights trees permanently resident in the ~50 KB pool, that
         * layer allocation fails, and — with LVGL logging off and
         * LV_ASSERT_MALLOC on — the UI wedges silently the moment Insights first
         * draws (the "does not go to Insights" hang). The cards' content is
         * padded and never reaches the rounded corners, so disabling child
         * clipping is visually identical and removes the layer allocation. */
        lv_style_set_clip_corner(&style_card, false);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        lv_obj_t * panel_0 = panel_create(parent);
        lv_obj_set_name_static(panel_0, "card_#");

        lv_obj_add_style(panel_0, &style_card, 0);

        the_root = panel_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

