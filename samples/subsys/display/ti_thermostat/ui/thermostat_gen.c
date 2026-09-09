/**
 * @file thermostat_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "thermostat_gen.h"

#if defined(LV_USE_XML) && LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void check_font(lv_font_t ** font, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t thermostat_target = THERMOSTAT_TARGET_ALL;

/*----------------
 * Translations
 *----------------*/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/

lv_font_t * font_hero_72;
extern lv_font_t font_hero_72_data;
lv_font_t * font_semibold_28;
extern lv_font_t font_semibold_28_data;
lv_font_t * font_semibold_22;
extern lv_font_t font_semibold_22_data;
lv_font_t * font_semibold_20;
extern lv_font_t font_semibold_20_data;
lv_font_t * font_semibold_17;
extern lv_font_t font_semibold_17_data;
lv_font_t * font_semibold_15;
extern lv_font_t font_semibold_15_data;
lv_font_t * font_semibold_14;
extern lv_font_t font_semibold_14_data;
lv_font_t * font_medium_28;
extern lv_font_t font_medium_28_data;
lv_font_t * font_medium_12;
extern lv_font_t font_medium_12_data;
lv_font_t * font_medium_11;
extern lv_font_t font_medium_11_data;
lv_font_t * font_regular_14;
extern lv_font_t font_regular_14_data;
lv_font_t * font_regular_12;
extern lv_font_t font_regular_12_data;
lv_font_t * font_regular_11;
extern lv_font_t font_regular_11_data;

/*----------------
 * Images
 *----------------*/

/* Targets: any */
const void * image_logostrip = NULL;
extern const void * image_logostrip_data;
const void * icon_humidity = NULL;
extern const void * icon_humidity_data;
const void * icon_heat = NULL;
extern const void * icon_heat_data;
const void * icon_state_heat = NULL;
extern const void * icon_state_heat_data;
const void * icon_state_cool = NULL;
extern const void * icon_state_cool_data;
const void * icon_state_idle = NULL;
extern const void * icon_state_idle_data;
const void * icon_cool = NULL;
extern const void * icon_cool_data;
const void * icon_auto = NULL;
extern const void * icon_auto_data;
const void * icon_eco = NULL;
extern const void * icon_eco_data;
const void * icon_plus = NULL;
extern const void * icon_plus_data;
const void * icon_minus = NULL;
extern const void * icon_minus_data;
const void * icon_back = NULL;
extern const void * icon_back_data;
const void * icon_climate = NULL;
extern const void * icon_climate_data;
const void * icon_insights = NULL;
extern const void * icon_insights_data;
const void * icon_runtime = NULL;
extern const void * icon_runtime_data;

/*----------------
 * Global styles
 *----------------*/

lv_style_t style_hero;
lv_style_t style_hero_unit;
lv_style_t style_stat_lg;
lv_style_t style_heading;
lv_style_t style_stat_md;
lv_style_t style_title;
lv_style_t style_subtitle;
lv_style_t style_body_strong;
lv_style_t style_body;
lv_style_t style_meta;
lv_style_t style_chip;
lv_style_t style_caption;
lv_style_t style_badge;
lv_style_t style_overline;
lv_style_t style_micro;
lv_style_t style_screen_base;
lv_style_t style_glass;
lv_style_t style_glass_pressed;
lv_style_t style_glass_dark;
lv_style_t style_glass_dark_pressed;
lv_style_t style_glass_circle_sm;
lv_style_t style_glass_circle_lg;
lv_style_t style_selected;
lv_style_t style_ink;
lv_style_t style_ink_secondary;
lv_style_t style_ink_muted;
lv_style_t style_field_track_ember;
lv_style_t style_field_fill_ember;
lv_style_t style_field_track_glacier;
lv_style_t style_field_fill_glacier;
lv_style_t style_field_track_basalt;
lv_style_t style_field_fill_basalt;
lv_style_t style_ground_ember;
lv_style_t style_ground_glacier;
lv_style_t style_ground_basalt;
lv_style_t style_switch;
lv_style_t style_switch_ind;
lv_style_t style_switch_knob;
lv_style_t style_roller;
lv_style_t style_roller_sel;
lv_style_t style_icon_ink;
lv_style_t style_icon_dim;
lv_style_t style_icon_inverted;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t subject_setpoint;
lv_subject_t subject_digit_tens;
lv_subject_t subject_digit_units;
lv_subject_t subject_field_level;
lv_subject_t subject_indoor;
lv_subject_t subject_indoor_str;
lv_subject_t subject_outdoor;
lv_subject_t subject_outdoor_str;
lv_subject_t subject_humidity;
lv_subject_t subject_hvac_mode;
lv_subject_t subject_hvac_state;
lv_subject_t subject_fan;
lv_subject_t subject_eco;
lv_subject_t subject_schedule;
lv_subject_t subject_deadband;
lv_subject_t subject_runtime_str;
lv_subject_t subject_cycles;
lv_subject_t subject_saving;
lv_subject_t subject_time;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void thermostat_init_gen(const char * asset_path)
{
    /* When running from the editor the theme set from the XML should overwrite this */
#if !defined(LV_EDITOR_PREVIEW)
#if LV_USE_THEME_SIMPLE
    lv_display_t * disp = lv_display_get_default();
    lv_theme_t * th = lv_theme_simple_init(disp);
    lv_display_set_theme(disp, th);
#else
    LV_LOG_WARN("Simple theme is selected in project.xml but LV_USE_THEME_SIMPLE is disabled");
#endif
#endif /*LV_EDITOR_PREVIEW*/


    /*----------------
     * Fonts
     *----------------*/

    /* Targets: any */

    #if THERMOSTAT_CHECK_COMPILE_TARGET(THERMOSTAT_TARGET_ALL)
    if (thermostat_check_target(THERMOSTAT_TARGET_ALL)) {
        if (!font_hero_72) {
            /* font_hero_72 */
            /* get font 'font_hero_72' from a C array */
            font_hero_72 = &font_hero_72_data;

        }
        if (!font_semibold_28) {
            /* font_semibold_28 */
            /* get font 'font_semibold_28' from a C array */
            font_semibold_28 = &font_semibold_28_data;

        }
        if (!font_semibold_22) {
            /* font_semibold_22 */
            /* get font 'font_semibold_22' from a C array */
            font_semibold_22 = &font_semibold_22_data;

        }
        if (!font_semibold_20) {
            /* font_semibold_20 */
            /* get font 'font_semibold_20' from a C array */
            font_semibold_20 = &font_semibold_20_data;

        }
        if (!font_semibold_17) {
            /* font_semibold_17 */
            /* get font 'font_semibold_17' from a C array */
            font_semibold_17 = &font_semibold_17_data;

        }
        if (!font_semibold_15) {
            /* font_semibold_15 */
            /* get font 'font_semibold_15' from a C array */
            font_semibold_15 = &font_semibold_15_data;

        }
        if (!font_semibold_14) {
            /* font_semibold_14 */
            /* get font 'font_semibold_14' from a C array */
            font_semibold_14 = &font_semibold_14_data;

        }
        if (!font_medium_28) {
            /* font_medium_28 */
            /* get font 'font_medium_28' from a C array */
            font_medium_28 = &font_medium_28_data;

        }
        if (!font_medium_12) {
            /* font_medium_12 */
            /* get font 'font_medium_12' from a C array */
            font_medium_12 = &font_medium_12_data;

        }
        if (!font_medium_11) {
            /* font_medium_11 */
            /* get font 'font_medium_11' from a C array */
            font_medium_11 = &font_medium_11_data;

        }
        if (!font_regular_14) {
            /* font_regular_14 */
            /* get font 'font_regular_14' from a C array */
            font_regular_14 = &font_regular_14_data;

        }
        if (!font_regular_12) {
            /* font_regular_12 */
            /* get font 'font_regular_12' from a C array */
            font_regular_12 = &font_regular_12_data;

        }
        if (!font_regular_11) {
            /* font_regular_11 */
            /* get font 'font_regular_11' from a C array */
            font_regular_11 = &font_regular_11_data;

        }
    }
    #endif

    /*----------------
     * Images
     *----------------*/

    /* Targets: any */
    #if THERMOSTAT_CHECK_COMPILE_TARGET(THERMOSTAT_TARGET_ALL)
    if (thermostat_check_target(THERMOSTAT_TARGET_ALL)) {
        /* image_logostrip */
        if (!image_logostrip) {
            image_logostrip = &image_logostrip_data;
        }
        /* icon_humidity */
        if (!icon_humidity) {
            icon_humidity = &icon_humidity_data;
        }
        /* icon_heat */
        if (!icon_heat) {
            icon_heat = &icon_heat_data;
        }
        /* icon_state_heat */
        if (!icon_state_heat) {
            icon_state_heat = &icon_state_heat_data;
        }
        /* icon_state_cool */
        if (!icon_state_cool) {
            icon_state_cool = &icon_state_cool_data;
        }
        /* icon_state_idle */
        if (!icon_state_idle) {
            icon_state_idle = &icon_state_idle_data;
        }
        /* icon_cool */
        if (!icon_cool) {
            icon_cool = &icon_cool_data;
        }
        /* icon_auto */
        if (!icon_auto) {
            icon_auto = &icon_auto_data;
        }
        /* icon_eco */
        if (!icon_eco) {
            icon_eco = &icon_eco_data;
        }
        /* icon_plus */
        if (!icon_plus) {
            icon_plus = &icon_plus_data;
        }
        /* icon_minus */
        if (!icon_minus) {
            icon_minus = &icon_minus_data;
        }
        /* icon_back */
        if (!icon_back) {
            icon_back = &icon_back_data;
        }
        /* icon_climate */
        if (!icon_climate) {
            icon_climate = &icon_climate_data;
        }
        /* icon_insights */
        if (!icon_insights) {
            icon_insights = &icon_insights_data;
        }
        /* icon_runtime */
        if (!icon_runtime) {
            icon_runtime = &icon_runtime_data;
        }
    }
    #endif

    /*----------------
     * Global styles
     *----------------*/

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_hero);
        lv_style_init(&style_hero_unit);
        lv_style_init(&style_stat_lg);
        lv_style_init(&style_heading);
        lv_style_init(&style_stat_md);
        lv_style_init(&style_title);
        lv_style_init(&style_subtitle);
        lv_style_init(&style_body_strong);
        lv_style_init(&style_body);
        lv_style_init(&style_meta);
        lv_style_init(&style_chip);
        lv_style_init(&style_caption);
        lv_style_init(&style_badge);
        lv_style_init(&style_overline);
        lv_style_init(&style_micro);
        lv_style_init(&style_screen_base);
        lv_style_init(&style_glass);
        lv_style_init(&style_glass_pressed);
        lv_style_init(&style_glass_dark);
        lv_style_init(&style_glass_dark_pressed);
        lv_style_init(&style_glass_circle_sm);
        lv_style_init(&style_glass_circle_lg);
        lv_style_init(&style_selected);
        lv_style_init(&style_ink);
        lv_style_init(&style_ink_secondary);
        lv_style_init(&style_ink_muted);
        lv_style_init(&style_field_track_ember);
        lv_style_init(&style_field_fill_ember);
        lv_style_init(&style_field_track_glacier);
        lv_style_init(&style_field_fill_glacier);
        lv_style_init(&style_field_track_basalt);
        lv_style_init(&style_field_fill_basalt);
        lv_style_init(&style_ground_ember);
        lv_style_init(&style_ground_glacier);
        lv_style_init(&style_ground_basalt);
        lv_style_init(&style_switch);
        lv_style_init(&style_switch_ind);
        lv_style_init(&style_switch_knob);
        lv_style_init(&style_roller);
        lv_style_init(&style_roller_sel);
        lv_style_init(&style_icon_ink);
        lv_style_init(&style_icon_dim);
        lv_style_init(&style_icon_inverted);

        lv_style_set_text_font(&style_hero, font_hero_72);
        lv_style_set_text_letter_space(&style_hero, -3);
        lv_style_set_text_font(&style_hero_unit, font_medium_28);
        lv_style_set_text_letter_space(&style_hero_unit, -1);
        lv_style_set_text_font(&style_stat_lg, font_semibold_28);
        lv_style_set_text_letter_space(&style_stat_lg, -1);
        lv_style_set_text_font(&style_heading, font_semibold_22);
        lv_style_set_text_font(&style_stat_md, font_semibold_20);
        lv_style_set_text_font(&style_title, font_semibold_17);
        lv_style_set_text_font(&style_subtitle, font_semibold_15);
        lv_style_set_text_font(&style_body_strong, font_semibold_14);
        lv_style_set_text_font(&style_body, font_regular_14);
        lv_style_set_text_font(&style_meta, font_regular_12);
        lv_style_set_text_font(&style_chip, font_medium_12);
        lv_style_set_text_font(&style_caption, font_regular_12);
        lv_style_set_text_font(&style_badge, font_medium_11);
        lv_style_set_text_font(&style_overline, font_medium_11);
        lv_style_set_text_letter_space(&style_overline, 1);
        lv_style_set_text_font(&style_micro, font_regular_11);
        lv_style_set_border_width(&style_screen_base, 0);
        lv_style_set_radius(&style_screen_base, 0);
        lv_style_set_shadow_width(&style_screen_base, 0);
        lv_style_set_shadow_opa(&style_screen_base, 0);
        lv_style_set_bg_color(&style_glass, CONST_INK);
        lv_style_set_bg_opa(&style_glass, CONST_GLASS_OPA);
        lv_style_set_border_color(&style_glass, CONST_INK);
        lv_style_set_border_opa(&style_glass, CONST_GLASS_BORDER_OPA);
        lv_style_set_border_width(&style_glass, 1);
        lv_style_set_radius(&style_glass, CONST_RADIUS_LG);
        lv_style_set_shadow_width(&style_glass, 0);
        lv_style_set_shadow_opa(&style_glass, 0);
        lv_style_set_bg_opa(&style_glass_pressed, CONST_GLASS_OPA_PRESSED);
        lv_style_set_bg_color(&style_glass_dark, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_glass_dark, 42);
        lv_style_set_border_width(&style_glass_dark, 0);
        lv_style_set_radius(&style_glass_dark, CONST_RADIUS_LG);
        lv_style_set_shadow_width(&style_glass_dark, 0);
        lv_style_set_shadow_opa(&style_glass_dark, 0);
        lv_style_set_bg_opa(&style_glass_dark_pressed, 76);
        lv_style_set_width(&style_glass_circle_sm, CONST_TOUCH);
        lv_style_set_height(&style_glass_circle_sm, CONST_TOUCH);
        lv_style_set_radius(&style_glass_circle_sm, CONST_RADIUS_PILL);
        lv_style_set_width(&style_glass_circle_lg, CONST_TOUCH_LG);
        lv_style_set_height(&style_glass_circle_lg, CONST_TOUCH_LG);
        lv_style_set_radius(&style_glass_circle_lg, CONST_RADIUS_PILL);
        lv_style_set_bg_color(&style_selected, CONST_INK);
        lv_style_set_bg_opa(&style_selected, 38);
        lv_style_set_border_color(&style_selected, CONST_INK);
        lv_style_set_border_opa(&style_selected, 255);
        lv_style_set_border_width(&style_selected, 2);
        lv_style_set_radius(&style_selected, CONST_RADIUS_LG);
        lv_style_set_text_color(&style_ink, CONST_INK);
        lv_style_set_text_opa(&style_ink, CONST_INK_OPA);
        lv_style_set_text_color(&style_ink_secondary, CONST_INK);
        lv_style_set_text_opa(&style_ink_secondary, CONST_INK_OPA_SECONDARY);
        lv_style_set_text_color(&style_ink_muted, CONST_INK);
        lv_style_set_text_opa(&style_ink_muted, CONST_INK_OPA_MUTED);
        lv_style_set_bg_color(&style_field_track_ember, CONST_EMBER_DEEP);
        lv_style_set_bg_opa(&style_field_track_ember, (255 * 100 / 100));
        lv_style_set_bg_color(&style_field_fill_ember, CONST_EMBER);
        lv_style_set_bg_opa(&style_field_fill_ember, (255 * 100 / 100));
        lv_style_set_bg_color(&style_field_track_glacier, CONST_GLACIER_DEEP);
        lv_style_set_bg_opa(&style_field_track_glacier, (255 * 100 / 100));
        lv_style_set_bg_color(&style_field_fill_glacier, CONST_GLACIER);
        lv_style_set_bg_opa(&style_field_fill_glacier, (255 * 100 / 100));
        lv_style_set_bg_color(&style_field_track_basalt, CONST_BASALT_DEEP);
        lv_style_set_bg_opa(&style_field_track_basalt, (255 * 100 / 100));
        lv_style_set_bg_color(&style_field_fill_basalt, CONST_BASALT);
        lv_style_set_bg_opa(&style_field_fill_basalt, (255 * 100 / 100));
        lv_style_set_bg_color(&style_ground_ember, CONST_EMBER);
        lv_style_set_bg_opa(&style_ground_ember, (255 * 100 / 100));
        lv_style_set_bg_grad_color(&style_ground_ember, CONST_EMBER_DEEP);
        lv_style_set_bg_grad_dir(&style_ground_ember, LV_GRAD_DIR_VER);
        lv_style_set_bg_color(&style_ground_glacier, CONST_GLACIER);
        lv_style_set_bg_opa(&style_ground_glacier, (255 * 100 / 100));
        lv_style_set_bg_grad_color(&style_ground_glacier, CONST_GLACIER_DEEP);
        lv_style_set_bg_grad_dir(&style_ground_glacier, LV_GRAD_DIR_VER);
        lv_style_set_bg_color(&style_ground_basalt, CONST_BASALT);
        lv_style_set_bg_opa(&style_ground_basalt, (255 * 100 / 100));
        lv_style_set_bg_grad_color(&style_ground_basalt, CONST_BASALT_DEEP);
        lv_style_set_bg_grad_dir(&style_ground_basalt, LV_GRAD_DIR_VER);
        lv_style_set_width(&style_switch, 52);
        lv_style_set_height(&style_switch, 30);
        lv_style_set_radius(&style_switch, CONST_RADIUS_PILL);
        lv_style_set_bg_color(&style_switch, CONST_INK);
        lv_style_set_bg_opa(&style_switch, 40);
        lv_style_set_border_color(&style_switch, CONST_INK);
        lv_style_set_border_opa(&style_switch, 60);
        lv_style_set_border_width(&style_switch, 1);
        lv_style_set_bg_color(&style_switch_ind, CONST_INK);
        lv_style_set_bg_opa(&style_switch_ind, 100);
        lv_style_set_radius(&style_switch_ind, CONST_RADIUS_PILL);
        lv_style_set_border_width(&style_switch_ind, 0);
        lv_style_set_bg_color(&style_switch_knob, CONST_INK);
        lv_style_set_bg_opa(&style_switch_knob, 255);
        lv_style_set_radius(&style_switch_knob, CONST_RADIUS_PILL);
        lv_style_set_pad_all(&style_switch_knob, 0);
        lv_style_set_border_width(&style_switch_knob, 0);
        lv_style_set_shadow_width(&style_switch_knob, 6);
        lv_style_set_shadow_offset_y(&style_switch_knob, 1);
        lv_style_set_shadow_spread(&style_switch_knob, -1);
        lv_style_set_shadow_color(&style_switch_knob, lv_color_hex(0x141C22));
        lv_style_set_shadow_opa(&style_switch_knob, 70);
        lv_style_set_bg_opa(&style_roller, 0);
        lv_style_set_border_width(&style_roller, 0);
        lv_style_set_pad_all(&style_roller, 0);
        lv_style_set_radius(&style_roller, 0);
        lv_style_set_shadow_width(&style_roller, 0);
        lv_style_set_shadow_opa(&style_roller, 0);
        lv_style_set_text_align(&style_roller, LV_TEXT_ALIGN_CENTER);
        lv_style_set_anim_duration(&style_roller, 330);
        lv_style_set_text_line_space(&style_roller, 0);
        lv_style_set_text_font(&style_roller, font_hero_72);
        lv_style_set_text_letter_space(&style_roller, -3);
        lv_style_set_text_color(&style_roller, CONST_INK);
        lv_style_set_bg_opa(&style_roller_sel, 0);
        lv_style_set_border_width(&style_roller_sel, 0);
        lv_style_set_text_font(&style_roller_sel, font_hero_72);
        lv_style_set_text_letter_space(&style_roller_sel, -3);
        lv_style_set_text_color(&style_roller_sel, CONST_INK);
        lv_style_set_image_recolor(&style_icon_ink, CONST_INK);
        lv_style_set_image_recolor_opa(&style_icon_ink, 255);
        lv_style_set_image_recolor(&style_icon_dim, CONST_INK);
        lv_style_set_image_recolor_opa(&style_icon_dim, 255);
        lv_style_set_image_opa(&style_icon_dim, 80);
        lv_style_set_image_recolor(&style_icon_inverted, CONST_INK_INVERTED);
        lv_style_set_image_recolor_opa(&style_icon_inverted, 255);

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&subject_setpoint, 21);
    lv_subject_set_min_value_int(&subject_setpoint, 15);
    lv_subject_set_max_value_int(&subject_setpoint, 30);
    lv_subject_init_int(&subject_digit_tens, 2);
    lv_subject_set_min_value_int(&subject_digit_tens, 0);
    lv_subject_set_max_value_int(&subject_digit_tens, 9);
    lv_subject_init_int(&subject_digit_units, 1);
    lv_subject_set_min_value_int(&subject_digit_units, 0);
    lv_subject_set_max_value_int(&subject_digit_units, 9);
    lv_subject_init_int(&subject_field_level, 2100);
    lv_subject_set_min_value_int(&subject_field_level, 1500);
    lv_subject_set_max_value_int(&subject_field_level, 3000);
    lv_subject_init_int(&subject_indoor, 202);
    static char subject_indoor_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_indoor_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_indoor_str,
                           subject_indoor_str_buf,
                           subject_indoor_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "20.2"
                          );
    lv_subject_init_int(&subject_outdoor, 118);
    static char subject_outdoor_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_outdoor_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_outdoor_str,
                           subject_outdoor_str_buf,
                           subject_outdoor_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "11.8"
                          );
    lv_subject_init_int(&subject_humidity, 44);
    lv_subject_set_min_value_int(&subject_humidity, 0);
    lv_subject_set_max_value_int(&subject_humidity, 100);
    lv_subject_init_int(&subject_hvac_mode, 3);
    lv_subject_set_min_value_int(&subject_hvac_mode, 0);
    lv_subject_set_max_value_int(&subject_hvac_mode, 3);
    lv_subject_init_int(&subject_hvac_state, 1);
    lv_subject_set_min_value_int(&subject_hvac_state, 0);
    lv_subject_set_max_value_int(&subject_hvac_state, 2);
    lv_subject_init_int(&subject_fan, 0);
    lv_subject_init_int(&subject_eco, 1);
    lv_subject_init_int(&subject_schedule, 1);
    lv_subject_init_int(&subject_deadband, 10);
    lv_subject_set_min_value_int(&subject_deadband, 5);
    lv_subject_set_max_value_int(&subject_deadband, 30);
    static char subject_runtime_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_runtime_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_runtime_str,
                           subject_runtime_str_buf,
                           subject_runtime_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "3h 12m"
                          );
    lv_subject_init_int(&subject_cycles, 7);
    lv_subject_init_int(&subject_saving, 12);
    static char subject_time_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_time_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_time,
                           subject_time_buf,
                           subject_time_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "09:41"
                          );

    /*----------------
     * Translations
     *----------------*/

#if defined(LV_USE_XML) && LV_USE_XML
    /* Register widgets */

    /* Check all fonts / default if needed. This prevents fonts that are used in one target but
       defined in another from causing assertion failures during rendering of the Preview. */
    check_font(&font_hero_72, "font_hero_72");
    check_font(&font_semibold_28, "font_semibold_28");
    check_font(&font_semibold_22, "font_semibold_22");
    check_font(&font_semibold_20, "font_semibold_20");
    check_font(&font_semibold_17, "font_semibold_17");
    check_font(&font_semibold_15, "font_semibold_15");
    check_font(&font_semibold_14, "font_semibold_14");
    check_font(&font_medium_28, "font_medium_28");
    check_font(&font_medium_12, "font_medium_12");
    check_font(&font_medium_11, "font_medium_11");
    check_font(&font_regular_14, "font_regular_14");
    check_font(&font_regular_12, "font_regular_12");
    check_font(&font_regular_11, "font_regular_11");

    /* Register fonts */
    lv_xml_register_font(NULL, "font_hero_72", font_hero_72);
    lv_xml_register_font(NULL, "font_semibold_28", font_semibold_28);
    lv_xml_register_font(NULL, "font_semibold_22", font_semibold_22);
    lv_xml_register_font(NULL, "font_semibold_20", font_semibold_20);
    lv_xml_register_font(NULL, "font_semibold_17", font_semibold_17);
    lv_xml_register_font(NULL, "font_semibold_15", font_semibold_15);
    lv_xml_register_font(NULL, "font_semibold_14", font_semibold_14);
    lv_xml_register_font(NULL, "font_medium_28", font_medium_28);
    lv_xml_register_font(NULL, "font_medium_12", font_medium_12);
    lv_xml_register_font(NULL, "font_medium_11", font_medium_11);
    lv_xml_register_font(NULL, "font_regular_14", font_regular_14);
    lv_xml_register_font(NULL, "font_regular_12", font_regular_12);
    lv_xml_register_font(NULL, "font_regular_11", font_regular_11);

    /* Register subjects */
    lv_xml_register_subject(NULL, "subject_setpoint", &subject_setpoint);
    lv_xml_register_subject(NULL, "subject_digit_tens", &subject_digit_tens);
    lv_xml_register_subject(NULL, "subject_digit_units", &subject_digit_units);
    lv_xml_register_subject(NULL, "subject_field_level", &subject_field_level);
    lv_xml_register_subject(NULL, "subject_indoor", &subject_indoor);
    lv_xml_register_subject(NULL, "subject_indoor_str", &subject_indoor_str);
    lv_xml_register_subject(NULL, "subject_outdoor", &subject_outdoor);
    lv_xml_register_subject(NULL, "subject_outdoor_str", &subject_outdoor_str);
    lv_xml_register_subject(NULL, "subject_humidity", &subject_humidity);
    lv_xml_register_subject(NULL, "subject_hvac_mode", &subject_hvac_mode);
    lv_xml_register_subject(NULL, "subject_hvac_state", &subject_hvac_state);
    lv_xml_register_subject(NULL, "subject_fan", &subject_fan);
    lv_xml_register_subject(NULL, "subject_eco", &subject_eco);
    lv_xml_register_subject(NULL, "subject_schedule", &subject_schedule);
    lv_xml_register_subject(NULL, "subject_deadband", &subject_deadband);
    lv_xml_register_subject(NULL, "subject_runtime_str", &subject_runtime_str);
    lv_xml_register_subject(NULL, "subject_cycles", &subject_cycles);
    lv_xml_register_subject(NULL, "subject_saving", &subject_saving);
    lv_xml_register_subject(NULL, "subject_time", &subject_time);

    /* Register callbacks */
    lv_xml_register_event_cb(NULL, "thermostat_drag_attach_cb", thermostat_drag_attach_cb);
    lv_xml_register_event_cb(NULL, "thermostat_marker_attach_cb", thermostat_marker_attach_cb);
    lv_xml_register_event_cb(NULL, "thermostat_drag_cb", thermostat_drag_cb);
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if defined(LV_USE_XML) && LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "image_logostrip", image_logostrip);
    lv_xml_register_image(NULL, "icon_humidity", icon_humidity);
    lv_xml_register_image(NULL, "icon_heat", icon_heat);
    lv_xml_register_image(NULL, "icon_state_heat", icon_state_heat);
    lv_xml_register_image(NULL, "icon_state_cool", icon_state_cool);
    lv_xml_register_image(NULL, "icon_state_idle", icon_state_idle);
    lv_xml_register_image(NULL, "icon_cool", icon_cool);
    lv_xml_register_image(NULL, "icon_auto", icon_auto);
    lv_xml_register_image(NULL, "icon_eco", icon_eco);
    lv_xml_register_image(NULL, "icon_plus", icon_plus);
    lv_xml_register_image(NULL, "icon_minus", icon_minus);
    lv_xml_register_image(NULL, "icon_back", icon_back);
    lv_xml_register_image(NULL, "icon_climate", icon_climate);
    lv_xml_register_image(NULL, "icon_insights", icon_insights);
    lv_xml_register_image(NULL, "icon_runtime", icon_runtime);
#endif

#if !defined(LV_USE_XML) || LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
#endif
}

void thermostat_set_target(uint32_t target)
{
    thermostat_target = target;
}

uint32_t thermostat_get_target(void)
{
    return thermostat_target;
}

bool thermostat_check_target(uint32_t target)
{
    return (thermostat_target & target) ? true : false;
}

/* Callbacks */
#if defined(LV_EDITOR_PREVIEW)
void __attribute__((weak)) thermostat_drag_attach_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("thermostat_drag_attach_cb was called\n");
}
void __attribute__((weak)) thermostat_marker_attach_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("thermostat_marker_attach_cb was called\n");
}
void __attribute__((weak)) thermostat_drag_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("thermostat_drag_cb was called\n");
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void check_font(lv_font_t ** font, const char * name)
{
    if (!(*font)) {
        *font = (lv_font_t *)LV_FONT_DEFAULT;
        LV_LOG_WARN("font `%s` was not set. Using `LV_FONT_DEFAULT` instead", name);
    }
}