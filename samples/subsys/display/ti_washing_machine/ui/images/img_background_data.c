/*
 * Background image stub for embedded targets.
 *
 * The original ARGB8888 480x320 background (~600 KB) exceeds the 512 KB flash
 * of the LP-MSPM0G3519. This empty descriptor causes LVGL to skip rendering
 * the background image and show the CSS background color instead.
 */

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_LVGL_H_INCLUDE_SYSTEM)
#include <lvgl.h>
#else
#include "lvgl/lvgl.h"
#endif

const lv_image_dsc_t img_background_data = {
    .header = {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = LV_COLOR_FORMAT_ARGB8888,
        .flags = 0,
        .w = 0,
        .h = 0,
        .stride = 0,
        .reserved_2 = 0,
    },
    .data_size = 0,
    .data = NULL,
    .reserved = NULL,
};
