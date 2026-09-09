/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Stub background images for Zephyr/M0+ targets.
 *
 * The full ti-thermostat demo ships three full-screen (320×480 RGB565)
 * background bitmaps (~300 KB each, ~900 KB total) that overflow the
 * LP-MSPM0G3519's 512 KB flash.  The home_gen.c code still creates the
 * bg_idle / bg_heat / bg_cool image widgets and the transition logic looks
 * for them by name — so the symbol names must exist.
 *
 * These stubs are valid 1×1 RGB565 descriptors whose single pixel carries
 * the dominant tint of the original:
 *   bg_normal (idle) : light neutral  0xEEF1F3 → RGB565 0xF79E
 *   bg_warm  (heat)  : warm cream     0xFDEEE3 → RGB565 0xFEFC
 *   bg_cool  (cool)  : cool tint      0xE6F0FC → RGB565 0xE77F
 *
 * With THERMOSTAT_USE_TRANSITIONS=0 the only effect is a 1×1 pixel in the
 * corner of the home screen that appears or disappears as the HVAC state
 * changes; the screen's own gradient background fills the visible area.
 */

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

/* ---- bg_normal (idle, neutral tint) ---- */
static const LV_ATTRIBUTE_MEM_ALIGN uint8_t image_bg_normal_map[] = {
    0x9E, 0xF7,   /* 0xF79E in LE = RGB565 ~0xEEF1F3 neutral */
};
const lv_image_dsc_t image_bg_normal_data = {
    .header = {
        .magic  = LV_IMAGE_HEADER_MAGIC,
        .cf     = LV_COLOR_FORMAT_RGB565,
        .flags  = 0,
        .w      = 1,
        .h      = 1,
        .stride = 2,
    },
    .data_size = sizeof(image_bg_normal_map),
    .data      = image_bg_normal_map,
    .reserved  = NULL,
};

/* ---- bg_warm (heating, warm cream tint) ---- */
static const LV_ATTRIBUTE_MEM_ALIGN uint8_t image_bg_warm_map[] = {
    0xFC, 0xFE,   /* 0xFEFC LE ≈ 0xFDEEE3 warm cream */
};
const lv_image_dsc_t image_bg_warm_data = {
    .header = {
        .magic  = LV_IMAGE_HEADER_MAGIC,
        .cf     = LV_COLOR_FORMAT_RGB565,
        .flags  = 0,
        .w      = 1,
        .h      = 1,
        .stride = 2,
    },
    .data_size = sizeof(image_bg_warm_map),
    .data      = image_bg_warm_map,
    .reserved  = NULL,
};

/* ---- bg_cool (cooling, cool blue tint) ---- */
static const LV_ATTRIBUTE_MEM_ALIGN uint8_t image_bg_cool_map[] = {
    0x7F, 0xE7,   /* 0xE77F LE ≈ 0xE6F0FC cool tint */
};
const lv_image_dsc_t image_bg_cool_data = {
    .header = {
        .magic  = LV_IMAGE_HEADER_MAGIC,
        .cf     = LV_COLOR_FORMAT_RGB565,
        .flags  = 0,
        .w      = 1,
        .h      = 1,
        .stride = 2,
    },
    .data_size = sizeof(image_bg_cool_map),
    .data      = image_bg_cool_map,
    .reserved  = NULL,
};
