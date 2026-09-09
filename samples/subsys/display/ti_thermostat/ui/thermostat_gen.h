/**
 * @file thermostat_gen.h
 */

#ifndef LVGL_PRO_THERMOSTAT_GEN_H
#define LVGL_PRO_THERMOSTAT_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#if defined(LV_USE_XML) && LV_USE_XML
    #include "lv_xml/lv_xml.h"
#endif



/* Prototypes for target functions, needed by responsive const definitions */

void thermostat_set_target(uint32_t target);
uint32_t thermostat_get_target(void);
bool thermostat_check_target(uint32_t target);

/*********************
 *      DEFINES
 *********************/

#define THERMOSTAT_TARGET_UNDEFINED  (0 << 1)
#define THERMOSTAT_TARGET_TARGET1    (1 << 1)
#define THERMOSTAT_TARGET_ALL        0x0FFFFFFF

/* By default compile for all targets, allowing to switch to any targets at runtime */
#ifndef THERMOSTAT_COMPILE_TARGET
#define THERMOSTAT_COMPILE_TARGET THERMOSTAT_TARGET_ALL
#endif

#define THERMOSTAT_CHECK_COMPILE_TARGET(target) (THERMOSTAT_COMPILE_TARGET & (target) ? 1 : 0)

#define CONST_EMBER_DEEP lv_color_hex(0x9B2C11)
#define CONST_EMBER lv_color_hex(0xE44420)
#define CONST_GLACIER_DEEP lv_color_hex(0x0E4C74)
#define CONST_GLACIER lv_color_hex(0x27A3CE)
#define CONST_BASALT_DEEP lv_color_hex(0x2E2A28)
#define CONST_BASALT lv_color_hex(0x4A4441)
#define CONST_INK lv_color_hex(0xFFFFFF)
#define CONST_INK_OPA 255
#define CONST_INK_OPA_SECONDARY 175
#define CONST_INK_OPA_MUTED 130
#define CONST_GLASS_OPA 20
#define CONST_GLASS_OPA_PRESSED 44
#define CONST_GLASS_BORDER_OPA 42
#define CONST_INK_INVERTED lv_color_hex(0x211D1B)
#define CONST_SPACE_2 2
#define CONST_SPACE_4 4
#define CONST_SPACE_8 8
#define CONST_SPACE_12 12
#define CONST_SPACE_16 16
#define CONST_SPACE_20 20
#define CONST_SPACE_24 24
#define CONST_RADIUS_SM 8
#define CONST_RADIUS_MD 12
#define CONST_RADIUS_LG 16
#define CONST_RADIUS_PILL 999
#define CONST_SCREEN_W 320
#define CONST_SCREEN_H 480
#define CONST_CONTENT_W 320
#define CONST_CARD_W 288
#define CONST_ROW_W 264
#define CONST_GUTTER 16
#define CONST_TOPBAR_H 56
#define CONST_TOUCH 56
#define CONST_TOUCH_LG 64
#define CONST_TILE_W 140
#define CONST_TILE_H 76
#define CONST_SETPOINT_MIN 15
#define CONST_SETPOINT_MAX 30
#define CONST_LEVEL_MIN 1500
#define CONST_LEVEL_MAX 3000
#define REF_MODE_OFF 0
#define REF_MODE_HEAT 1
#define REF_MODE_COOL 2
#define REF_MODE_AUTO 3
#define REF_STATE_IDLE 0
#define REF_STATE_HEATING 1
#define REF_STATE_COOLING 2
#define REF_OFF 0
#define REF_ON 1


#ifndef LV_XML_EVAL_STRING_BUF_SIZE
    #define LV_XML_EVAL_STRING_BUF_SIZE 256
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

extern lv_style_t style_hero;
extern lv_style_t style_hero_unit;
extern lv_style_t style_stat_lg;
extern lv_style_t style_heading;
extern lv_style_t style_stat_md;
extern lv_style_t style_title;
extern lv_style_t style_subtitle;
extern lv_style_t style_body_strong;
extern lv_style_t style_body;
extern lv_style_t style_meta;
extern lv_style_t style_chip;
extern lv_style_t style_caption;
extern lv_style_t style_badge;
extern lv_style_t style_overline;
extern lv_style_t style_micro;
extern lv_style_t style_screen_base;
extern lv_style_t style_glass;
extern lv_style_t style_glass_pressed;
extern lv_style_t style_glass_dark;
extern lv_style_t style_glass_dark_pressed;
extern lv_style_t style_glass_circle_sm;
extern lv_style_t style_glass_circle_lg;
extern lv_style_t style_selected;
extern lv_style_t style_ink;
extern lv_style_t style_ink_secondary;
extern lv_style_t style_ink_muted;
extern lv_style_t style_field_track_ember;
extern lv_style_t style_field_fill_ember;
extern lv_style_t style_field_track_glacier;
extern lv_style_t style_field_fill_glacier;
extern lv_style_t style_field_track_basalt;
extern lv_style_t style_field_fill_basalt;
extern lv_style_t style_ground_ember;
extern lv_style_t style_ground_glacier;
extern lv_style_t style_ground_basalt;
extern lv_style_t style_switch;
extern lv_style_t style_switch_ind;
extern lv_style_t style_switch_knob;
extern lv_style_t style_roller;
extern lv_style_t style_roller_sel;
extern lv_style_t style_icon_ink;
extern lv_style_t style_icon_dim;
extern lv_style_t style_icon_inverted;

/*----------------
 * Fonts
 *----------------*/

/* Targets: any */
extern lv_font_t * font_hero_72;
extern lv_font_t * font_semibold_28;
extern lv_font_t * font_semibold_22;
extern lv_font_t * font_semibold_20;
extern lv_font_t * font_semibold_17;
extern lv_font_t * font_semibold_15;
extern lv_font_t * font_semibold_14;
extern lv_font_t * font_medium_28;
extern lv_font_t * font_medium_12;
extern lv_font_t * font_medium_11;
extern lv_font_t * font_regular_14;
extern lv_font_t * font_regular_12;
extern lv_font_t * font_regular_11;


/*----------------
 * Images
 *----------------*/

/* Targets: any */
extern const void * image_logostrip;
extern const void * icon_humidity;
extern const void * icon_heat;
extern const void * icon_state_heat;
extern const void * icon_state_cool;
extern const void * icon_state_idle;
extern const void * icon_cool;
extern const void * icon_auto;
extern const void * icon_eco;
extern const void * icon_plus;
extern const void * icon_minus;
extern const void * icon_back;
extern const void * icon_climate;
extern const void * icon_insights;
extern const void * icon_runtime;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t subject_setpoint;
extern lv_subject_t subject_digit_tens;
extern lv_subject_t subject_digit_units;
extern lv_subject_t subject_field_level;
extern lv_subject_t subject_indoor;
extern lv_subject_t subject_indoor_str;
extern lv_subject_t subject_outdoor;
extern lv_subject_t subject_outdoor_str;
extern lv_subject_t subject_humidity;
extern lv_subject_t subject_hvac_mode;
extern lv_subject_t subject_hvac_state;
extern lv_subject_t subject_fan;
extern lv_subject_t subject_eco;
extern lv_subject_t subject_schedule;
extern lv_subject_t subject_deadband;
extern lv_subject_t subject_runtime_str;
extern lv_subject_t subject_cycles;
extern lv_subject_t subject_saving;
extern lv_subject_t subject_time;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

void thermostat_drag_attach_cb(lv_event_t * e);
void thermostat_marker_attach_cb(lv_event_t * e);
void thermostat_drag_cb(lv_event_t * e);

/**
 * Initialize the component library
 */

void thermostat_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "components/base/card/card_gen.h"
#include "components/base/column/column_gen.h"
#include "components/base/container/container_gen.h"
#include "components/base/label/label_gen.h"
#include "components/base/panel/panel_gen.h"
#include "components/base/row/row_gen.h"
#include "components/glass/glass_gen.h"
#include "components/metric/metric_gen.h"
#include "components/nav_button/nav_button_gen.h"
#include "components/state_icons/state_icons_gen.h"
#include "components/top_bar/top_bar_gen.h"
#include "screens/climate_gen.h"
#include "screens/home_gen.h"
#include "screens/insights_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_THERMOSTAT_GEN_H*/