/**
 * @file ti_thermostat.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "ti_thermostat.h"

#include "custom/thermostat_logic.h"
#include "custom/thermostat_transitions.h"

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

void ti_thermostat_init(const char * asset_path)
{
    LV_LOG("Initializing custom C code using LVGL v%d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

    ti_thermostat_init_gen(asset_path);

    /* Must run after the generated init — that is what creates and registers
     * the subjects this observes. */
    thermostat_logic_init();

    /* One observer for the whole app; the screens only prime themselves.
     * See custom/thermostat_transitions.h. */
    thermostat_transitions_init();

    /* Load the home screen so the application only has to call
     * ti_thermostat_init(). Screens are permanent, so init_gen already built
     * every one of them; "home" is the global init_gen populated, not a
     * _create() call, which would build a second copy. The editor preview
     * manages screens itself, hence the guard. */
#if !defined(LV_EDITOR_PREVIEW)
    lv_screen_load(home);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/