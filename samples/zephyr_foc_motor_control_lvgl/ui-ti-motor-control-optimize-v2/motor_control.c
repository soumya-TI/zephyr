/**
 * @file motor_control.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "motor_control.h"
#include "custom/motor_sim.h"
#include "../src/foc_display_bridge.h"

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

        printk("DEBUG: Inside init\n");

//     printk("Initializing custom C code using LVGL v%d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

//     /* This is what creates and registers every subject, so nothing below may
//      * run before it. */
//     motor_control_init_gen(asset_path);

//     /* Replace the standalone simulation timer with the real FOC bridge.
//      * foc_display_bridge_init() creates the 100 ms lv_timer_t that reads
//      * motorVars_M1 and calls motor_sim_publish() with live data. */
//     foc_display_bridge_init();

//     /* Load the run screen so the application only has to call motor_control_init().
//      * Screens are not permanent - each is built by its create function and freed
//      * when another loads. The editor preview manages screens itself, hence the guard. */
// #if !defined(LV_EDITOR_PREVIEW)
//     lv_screen_load(run_create());
// #endif
        return;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/