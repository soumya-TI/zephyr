/**
 * @file thermostat.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "thermostat.h"
#include "custom/thermostat_logic.h"
#include "custom/thermostat_sim.h"

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

void thermostat_init(const char * asset_path)
{
    LV_LOG("Initializing custom C code using LVGL v%d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

    /* This is what creates and registers every subject, so nothing below may
     * run before it. */
    thermostat_init_gen(asset_path);

    /* Order matters: the logic module must be observing before the simulator
     * starts publishing, or the first derived state is computed from stale
     * inputs. */
    thermostat_logic_init();
    thermostat_sim_init();

    /* The field and the temperature marker attach themselves from the home
     * screen's XML, through <event_cb trigger="screen_loaded">. Screen-owned
     * behaviour belongs in the screen, not in this hand-editable file. */

    /* Load the home screen here so the application only has to call
     * thermostat_init(). The screens are NOT permanent, so init_gen does not
     * pre-create them and there is no global to load - each screen is built by
     * its create function and freed when another is loaded. That is also what
     * makes home_create() the correct call here: on a permanent screen it would
     * build a second copy of every widget into the object init_gen already
     * populated. The editor preview manages screens itself. */
#if !defined(LV_EDITOR_PREVIEW)
    lv_screen_load(home_create());
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/