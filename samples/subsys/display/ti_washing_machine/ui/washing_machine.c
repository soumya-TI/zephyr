/**
 * @file washing_machine.c - application entry point for the generated UI.
 */
#include "washing_machine.h"
#include "custom/wash_logic.h"
#include "custom/wash_sim.h"

void washing_machine_init(const char * asset_path)
{
    LV_LOG("Initializing custom C code using LVGL v%d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

    /* init_gen first: it creates every subject the modules below observe. */
    washing_machine_init_gen(asset_path);

    wash_logic_init();
    wash_sim_init();

/* Screens are non-permanent and built on demand, so the generated screen globals
 * are NULL under LV_USE_XML; only a real build loads a screen here. */
#if !defined(LV_EDITOR_PREVIEW)
    lv_screen_load(home_create());
#endif
}
