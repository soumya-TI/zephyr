/**
 * @file motor_control.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "motor_control.h"
#include "custom/motor_sim.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void motor_control_init(const char * asset_path)
{
    LV_LOG("Initializing custom C code using LVGL v%d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

    /* This is what creates and registers every subject, so nothing below may
     * run before it. */
    motor_control_init_gen(asset_path);

    /* The drive model. It also starts motor_digits, which must be observing
     * before the first speed is published or the odometer shows 0000 for a
     * frame after a real value already exists. */
    motor_sim_init();

    /* Load the run screen so the application only has to call motor_control_init().
     * Screens are not permanent - each is built by its create function and freed
     * when another loads. The editor preview manages screens itself, hence the guard. */
#if !defined(LV_EDITOR_PREVIEW)
    lv_screen_load(run_create());
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/