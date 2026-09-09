/**
 * @file ti_thermostat_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "ti_thermostat_gen.h"

#if defined(LV_USE_XML) && LV_USE_XML
#include "widgets/wd_schedule_chart/wd_schedule_chart_private_gen.h"
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

static uint32_t ti_thermostat_target = TI_THERMOSTAT_TARGET_ALL;

/*----------------
 * Translations
 *----------------*/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

lv_obj_t * home = NULL;
lv_obj_t * insights = NULL;
lv_obj_t * rooms = NULL;
lv_obj_t * schedule = NULL;

/*----------------
 * Fonts
 *----------------*/

lv_font_t * font_hero_88;
extern lv_font_t font_hero_88_data;
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
lv_font_t * font_medium_30;
extern lv_font_t font_medium_30_data;
lv_font_t * font_medium_14;
extern lv_font_t font_medium_14_data;
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
const void * image_bg_normal = NULL;
extern const void * image_bg_normal_data;
const void * image_bg_warm = NULL;
extern const void * image_bg_warm_data;
const void * image_bg_cool = NULL;
extern const void * image_bg_cool_data;
const void * image_logo = NULL;
extern const void * image_logo_data;
const void * icon_flame = NULL;
extern const void * icon_flame_data;
const void * icon_snowflake = NULL;
extern const void * icon_snowflake_data;
const void * icon_auto = NULL;
extern const void * icon_auto_data;
const void * icon_power = NULL;
extern const void * icon_power_data;
const void * icon_droplet = NULL;
extern const void * icon_droplet_data;
const void * icon_leaf = NULL;
extern const void * icon_leaf_data;
const void * icon_fan = NULL;
extern const void * icon_fan_data;
const void * icon_users = NULL;
extern const void * icon_users_data;
const void * icon_thermo = NULL;
extern const void * icon_thermo_data;

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
lv_style_t style_shadow_card;
lv_style_t style_shadow_knob;
lv_style_t style_screen_base;
lv_style_t style_card_surface;
lv_style_t style_chip_fill_neutral;
lv_style_t style_chip_fill_heat;
lv_style_t style_chip_fill_cool;
lv_style_t style_chip_fill_accent;
lv_style_t style_chip_fill_warn;
lv_style_t style_chip_dot_neutral;
lv_style_t style_chip_dot_heat;
lv_style_t style_chip_dot_cool;
lv_style_t style_chip_dot_accent;
lv_style_t style_chip_dot_warn;
lv_style_t style_chip_text_neutral;
lv_style_t style_chip_text_heat;
lv_style_t style_chip_text_cool;
lv_style_t style_chip_text_accent;
lv_style_t style_chip_text_warn;
lv_style_t style_bar_heat;
lv_style_t style_bar_neutral;
lv_style_t style_arc_ind_heat;
lv_style_t style_arc_ind_cool;
lv_style_t style_arc_ind_idle;
lv_style_t style_block_idle;
lv_style_t style_block_active;
lv_style_t style_mode_on_heat;
lv_style_t style_mode_on_cool;
lv_style_t style_mode_on_auto;
lv_style_t style_mode_label_heat;
lv_style_t style_mode_label_cool;
lv_style_t style_mode_label_auto;
lv_style_t style_day_on;
lv_style_t style_day_label_on;
lv_style_t style_icon_heat;
lv_style_t style_icon_cool;
lv_style_t style_icon_danger;
lv_style_t style_icon_ink;
lv_style_t style_tab_active;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t subject_target_temp;
lv_subject_t subject_now_temp;
lv_subject_t subject_humidity;
lv_subject_t subject_outdoor_temp;
lv_subject_t subject_mode;
lv_subject_t subject_hvac_state;
lv_subject_t subject_hold_active;
lv_subject_t subject_hold_until;
lv_subject_t subject_time;
lv_subject_t subject_tab;
lv_subject_t subject_sched_day;
lv_subject_t subject_room_living_temp;
lv_subject_t subject_room_bed_temp;
lv_subject_t subject_room_study_temp;
lv_subject_t subject_room_hall_temp;
lv_subject_t subject_room_living_demand;
lv_subject_t subject_room_bed_demand;
lv_subject_t subject_room_study_demand;
lv_subject_t subject_room_hall_demand;
lv_subject_t subject_dev_fan;
lv_subject_t subject_dev_humidifier;
lv_subject_t subject_dev_vent;
lv_subject_t subject_dev_geofence;
lv_subject_t subject_humidifier_target;
lv_subject_t subject_energy_kwh;
lv_subject_t subject_energy_delta;
lv_subject_t subject_runtime_hrs;
lv_subject_t subject_eco_days;
lv_subject_t subject_suggest_dismissed;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ti_thermostat_init_gen(const char * asset_path)
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

    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        if (!font_hero_88) {
            /* font_hero_88 */
            /* get font 'font_hero_88' from a C array */
            font_hero_88 = &font_hero_88_data;

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
        if (!font_medium_30) {
            /* font_medium_30 */
            /* get font 'font_medium_30' from a C array */
            font_medium_30 = &font_medium_30_data;

        }
        if (!font_medium_14) {
            /* font_medium_14 */
            /* get font 'font_medium_14' from a C array */
            font_medium_14 = &font_medium_14_data;

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
    #if TI_THERMOSTAT_CHECK_COMPILE_TARGET(TI_THERMOSTAT_TARGET_ALL)
    if (ti_thermostat_check_target(TI_THERMOSTAT_TARGET_ALL)) {
        /* image_bg_normal */
        if (!image_bg_normal) {
            image_bg_normal = &image_bg_normal_data;
        }
        /* image_bg_warm */
        if (!image_bg_warm) {
            image_bg_warm = &image_bg_warm_data;
        }
        /* image_bg_cool */
        if (!image_bg_cool) {
            image_bg_cool = &image_bg_cool_data;
        }
        /* image_logo */
        if (!image_logo) {
            image_logo = &image_logo_data;
        }
        /* icon_flame */
        if (!icon_flame) {
            icon_flame = &icon_flame_data;
        }
        /* icon_snowflake */
        if (!icon_snowflake) {
            icon_snowflake = &icon_snowflake_data;
        }
        /* icon_auto */
        if (!icon_auto) {
            icon_auto = &icon_auto_data;
        }
        /* icon_power */
        if (!icon_power) {
            icon_power = &icon_power_data;
        }
        /* icon_droplet */
        if (!icon_droplet) {
            icon_droplet = &icon_droplet_data;
        }
        /* icon_leaf */
        if (!icon_leaf) {
            icon_leaf = &icon_leaf_data;
        }
        /* icon_fan */
        if (!icon_fan) {
            icon_fan = &icon_fan_data;
        }
        /* icon_users */
        if (!icon_users) {
            icon_users = &icon_users_data;
        }
        /* icon_thermo */
        if (!icon_thermo) {
            icon_thermo = &icon_thermo_data;
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
        lv_style_init(&style_shadow_card);
        lv_style_init(&style_shadow_knob);
        lv_style_init(&style_screen_base);
        lv_style_init(&style_card_surface);
        lv_style_init(&style_chip_fill_neutral);
        lv_style_init(&style_chip_fill_heat);
        lv_style_init(&style_chip_fill_cool);
        lv_style_init(&style_chip_fill_accent);
        lv_style_init(&style_chip_fill_warn);
        lv_style_init(&style_chip_dot_neutral);
        lv_style_init(&style_chip_dot_heat);
        lv_style_init(&style_chip_dot_cool);
        lv_style_init(&style_chip_dot_accent);
        lv_style_init(&style_chip_dot_warn);
        lv_style_init(&style_chip_text_neutral);
        lv_style_init(&style_chip_text_heat);
        lv_style_init(&style_chip_text_cool);
        lv_style_init(&style_chip_text_accent);
        lv_style_init(&style_chip_text_warn);
        lv_style_init(&style_bar_heat);
        lv_style_init(&style_bar_neutral);
        lv_style_init(&style_arc_ind_heat);
        lv_style_init(&style_arc_ind_cool);
        lv_style_init(&style_arc_ind_idle);
        lv_style_init(&style_block_idle);
        lv_style_init(&style_block_active);
        lv_style_init(&style_mode_on_heat);
        lv_style_init(&style_mode_on_cool);
        lv_style_init(&style_mode_on_auto);
        lv_style_init(&style_mode_label_heat);
        lv_style_init(&style_mode_label_cool);
        lv_style_init(&style_mode_label_auto);
        lv_style_init(&style_day_on);
        lv_style_init(&style_day_label_on);
        lv_style_init(&style_icon_heat);
        lv_style_init(&style_icon_cool);
        lv_style_init(&style_icon_danger);
        lv_style_init(&style_icon_ink);
        lv_style_init(&style_tab_active);

        lv_style_set_text_font(&style_hero, font_hero_88);
        lv_style_set_text_letter_space(&style_hero, -3);
        lv_style_set_text_font(&style_hero_unit, font_medium_30);
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
        lv_style_set_shadow_width(&style_shadow_card, 16);
        lv_style_set_shadow_offset_y(&style_shadow_card, 5);
        lv_style_set_shadow_spread(&style_shadow_card, -4);
        lv_style_set_shadow_color(&style_shadow_card, lv_color_hex(0x141C22));
        lv_style_set_shadow_opa(&style_shadow_card, 24);
        lv_style_set_shadow_width(&style_shadow_knob, 6);
        lv_style_set_shadow_offset_y(&style_shadow_knob, 2);
        lv_style_set_shadow_spread(&style_shadow_knob, -1);
        lv_style_set_shadow_color(&style_shadow_knob, lv_color_hex(0x141C22));
        lv_style_set_shadow_opa(&style_shadow_knob, 46);
        lv_style_set_border_width(&style_screen_base, 0);
        lv_style_set_radius(&style_screen_base, 0);
        lv_style_set_shadow_width(&style_screen_base, 0);
        lv_style_set_shadow_opa(&style_screen_base, 0);
        lv_style_set_bg_color(&style_card_surface, CONST_SURFACE_CARD);
        lv_style_set_bg_opa(&style_card_surface, (255 * 100 / 100));
        lv_style_set_border_color(&style_card_surface, CONST_BORDER);
        lv_style_set_border_width(&style_card_surface, 1);
        lv_style_set_radius(&style_card_surface, CONST_RADIUS_LG);
        lv_style_set_bg_color(&style_chip_fill_neutral, CONST_TINT_NEUTRAL);
        lv_style_set_bg_opa(&style_chip_fill_neutral, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_fill_heat, CONST_TINT_HEAT);
        lv_style_set_bg_opa(&style_chip_fill_heat, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_fill_cool, CONST_TINT_COOL);
        lv_style_set_bg_opa(&style_chip_fill_cool, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_fill_accent, CONST_TINT_ACCENT);
        lv_style_set_bg_opa(&style_chip_fill_accent, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_fill_warn, CONST_TINT_WARN);
        lv_style_set_bg_opa(&style_chip_fill_warn, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_dot_neutral, CONST_TEXT_SECONDARY);
        lv_style_set_bg_opa(&style_chip_dot_neutral, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_dot_heat, CONST_HEAT);
        lv_style_set_bg_opa(&style_chip_dot_heat, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_dot_cool, CONST_COOL);
        lv_style_set_bg_opa(&style_chip_dot_cool, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_dot_accent, CONST_ACCENT);
        lv_style_set_bg_opa(&style_chip_dot_accent, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_dot_warn, CONST_WARN);
        lv_style_set_bg_opa(&style_chip_dot_warn, (255 * 100 / 100));
        lv_style_set_text_color(&style_chip_text_neutral, CONST_TEXT_SECONDARY);
        lv_style_set_text_color(&style_chip_text_heat, CONST_HEAT);
        lv_style_set_text_color(&style_chip_text_cool, CONST_COOL);
        lv_style_set_text_color(&style_chip_text_accent, CONST_ACCENT);
        lv_style_set_text_color(&style_chip_text_warn, CONST_WARN);
        lv_style_set_bg_color(&style_bar_heat, CONST_HEAT);
        lv_style_set_bg_color(&style_bar_neutral, CONST_TEXT_TERTIARY);
        lv_style_set_arc_color(&style_arc_ind_heat, CONST_HEAT);
        lv_style_set_arc_color(&style_arc_ind_cool, CONST_COOL);
        lv_style_set_arc_color(&style_arc_ind_idle, CONST_TEXT_TERTIARY);
        lv_style_set_bg_color(&style_block_active, CONST_TINT_ACCENT);
        lv_style_set_border_color(&style_block_active, CONST_ACCENT);
        lv_style_set_border_width(&style_block_active, 2);
        lv_style_set_bg_color(&style_mode_on_heat, CONST_TINT_HEAT);
        lv_style_set_border_color(&style_mode_on_heat, CONST_HEAT);
        lv_style_set_border_width(&style_mode_on_heat, 2);
        lv_style_set_bg_color(&style_mode_on_cool, CONST_TINT_COOL);
        lv_style_set_border_color(&style_mode_on_cool, CONST_COOL);
        lv_style_set_border_width(&style_mode_on_cool, 2);
        lv_style_set_bg_color(&style_mode_on_auto, CONST_TINT_NEUTRAL);
        lv_style_set_border_color(&style_mode_on_auto, CONST_TEXT_PRIMARY);
        lv_style_set_border_width(&style_mode_on_auto, 2);
        lv_style_set_text_color(&style_mode_label_heat, CONST_HEAT);
        lv_style_set_text_color(&style_mode_label_cool, CONST_COOL);
        lv_style_set_text_color(&style_mode_label_auto, CONST_TEXT_PRIMARY);
        lv_style_set_bg_color(&style_day_on, CONST_TEXT_PRIMARY);
        lv_style_set_border_color(&style_day_on, CONST_TEXT_PRIMARY);
        lv_style_set_text_color(&style_day_label_on, CONST_ON_ACCENT);
        lv_style_set_image_recolor(&style_icon_heat, CONST_HEAT);
        lv_style_set_image_recolor_opa(&style_icon_heat, 255);
        lv_style_set_image_recolor(&style_icon_cool, CONST_COOL);
        lv_style_set_image_recolor_opa(&style_icon_cool, 255);
        lv_style_set_image_recolor(&style_icon_danger, CONST_DANGER);
        lv_style_set_image_recolor_opa(&style_icon_danger, 255);
        lv_style_set_image_recolor(&style_icon_ink, CONST_TEXT_PRIMARY);
        lv_style_set_image_recolor_opa(&style_icon_ink, 255);
        lv_style_set_text_color(&style_tab_active, CONST_TEXT_PRIMARY);

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&subject_target_temp, 22);
    lv_subject_set_min_value_int(&subject_target_temp, 5);
    lv_subject_set_max_value_int(&subject_target_temp, 30);
    lv_subject_init_float(&subject_now_temp, 21.5);
    lv_subject_init_int(&subject_humidity, 44);
    lv_subject_init_int(&subject_outdoor_temp, 8);
    lv_subject_init_int(&subject_mode, 0);
    lv_subject_init_int(&subject_hvac_state, 1);
    lv_subject_init_int(&subject_hold_active, 1);
    static char subject_hold_until_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_hold_until_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_hold_until,
                           subject_hold_until_buf,
                           subject_hold_until_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "18:00"
                          );
    static char subject_time_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_time_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_time,
                           subject_time_buf,
                           subject_time_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "07:12"
                          );
    lv_subject_init_int(&subject_tab, 0);
    lv_subject_init_int(&subject_sched_day, 0);
    static char subject_room_living_temp_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_room_living_temp_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_room_living_temp,
                           subject_room_living_temp_buf,
                           subject_room_living_temp_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "21.5°"
                          );
    static char subject_room_bed_temp_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_room_bed_temp_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_room_bed_temp,
                           subject_room_bed_temp_buf,
                           subject_room_bed_temp_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "19.7°"
                          );
    static char subject_room_study_temp_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_room_study_temp_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_room_study_temp,
                           subject_room_study_temp_buf,
                           subject_room_study_temp_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "22.3°"
                          );
    static char subject_room_hall_temp_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_room_hall_temp_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_room_hall_temp,
                           subject_room_hall_temp_buf,
                           subject_room_hall_temp_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "18.8°"
                          );
    lv_subject_init_int(&subject_room_living_demand, 72);
    lv_subject_init_int(&subject_room_bed_demand, 18);
    lv_subject_init_int(&subject_room_study_demand, 55);
    lv_subject_init_int(&subject_room_hall_demand, 0);
    lv_subject_init_int(&subject_dev_fan, 1);
    lv_subject_init_int(&subject_dev_humidifier, 0);
    lv_subject_init_int(&subject_dev_vent, 1);
    lv_subject_init_int(&subject_dev_geofence, 1);
    lv_subject_init_int(&subject_humidifier_target, 45);
    lv_subject_set_min_value_int(&subject_humidifier_target, 30);
    lv_subject_set_max_value_int(&subject_humidifier_target, 60);
    lv_subject_init_int(&subject_energy_kwh, 18);
    lv_subject_init_int(&subject_energy_delta, -12);
    lv_subject_init_int(&subject_runtime_hrs, 41);
    lv_subject_init_int(&subject_eco_days, 6);
    lv_subject_init_int(&subject_suggest_dismissed, 0);

    /*----------------
     * Translations
     *----------------*/

#if defined(LV_USE_XML) && LV_USE_XML
    /* Register widgets */
    wd_schedule_chart_register();

    /* Check all fonts / default if needed. This prevents fonts that are used in one target but
       defined in another from causing assertion failures during rendering of the Preview. */
    check_font(&font_hero_88, "font_hero_88");
    check_font(&font_semibold_28, "font_semibold_28");
    check_font(&font_semibold_22, "font_semibold_22");
    check_font(&font_semibold_20, "font_semibold_20");
    check_font(&font_semibold_17, "font_semibold_17");
    check_font(&font_semibold_15, "font_semibold_15");
    check_font(&font_semibold_14, "font_semibold_14");
    check_font(&font_medium_30, "font_medium_30");
    check_font(&font_medium_14, "font_medium_14");
    check_font(&font_medium_12, "font_medium_12");
    check_font(&font_medium_11, "font_medium_11");
    check_font(&font_regular_14, "font_regular_14");
    check_font(&font_regular_12, "font_regular_12");
    check_font(&font_regular_11, "font_regular_11");

    /* Register fonts */
    lv_xml_register_font(NULL, "font_hero_88", font_hero_88);
    lv_xml_register_font(NULL, "font_semibold_28", font_semibold_28);
    lv_xml_register_font(NULL, "font_semibold_22", font_semibold_22);
    lv_xml_register_font(NULL, "font_semibold_20", font_semibold_20);
    lv_xml_register_font(NULL, "font_semibold_17", font_semibold_17);
    lv_xml_register_font(NULL, "font_semibold_15", font_semibold_15);
    lv_xml_register_font(NULL, "font_semibold_14", font_semibold_14);
    lv_xml_register_font(NULL, "font_medium_30", font_medium_30);
    lv_xml_register_font(NULL, "font_medium_14", font_medium_14);
    lv_xml_register_font(NULL, "font_medium_12", font_medium_12);
    lv_xml_register_font(NULL, "font_medium_11", font_medium_11);
    lv_xml_register_font(NULL, "font_regular_14", font_regular_14);
    lv_xml_register_font(NULL, "font_regular_12", font_regular_12);
    lv_xml_register_font(NULL, "font_regular_11", font_regular_11);

    /* Register subjects */
    lv_xml_register_subject(NULL, "subject_target_temp", &subject_target_temp);
    lv_xml_register_subject(NULL, "subject_now_temp", &subject_now_temp);
    lv_xml_register_subject(NULL, "subject_humidity", &subject_humidity);
    lv_xml_register_subject(NULL, "subject_outdoor_temp", &subject_outdoor_temp);
    lv_xml_register_subject(NULL, "subject_mode", &subject_mode);
    lv_xml_register_subject(NULL, "subject_hvac_state", &subject_hvac_state);
    lv_xml_register_subject(NULL, "subject_hold_active", &subject_hold_active);
    lv_xml_register_subject(NULL, "subject_hold_until", &subject_hold_until);
    lv_xml_register_subject(NULL, "subject_time", &subject_time);
    lv_xml_register_subject(NULL, "subject_tab", &subject_tab);
    lv_xml_register_subject(NULL, "subject_sched_day", &subject_sched_day);
    lv_xml_register_subject(NULL, "subject_room_living_temp", &subject_room_living_temp);
    lv_xml_register_subject(NULL, "subject_room_bed_temp", &subject_room_bed_temp);
    lv_xml_register_subject(NULL, "subject_room_study_temp", &subject_room_study_temp);
    lv_xml_register_subject(NULL, "subject_room_hall_temp", &subject_room_hall_temp);
    lv_xml_register_subject(NULL, "subject_room_living_demand", &subject_room_living_demand);
    lv_xml_register_subject(NULL, "subject_room_bed_demand", &subject_room_bed_demand);
    lv_xml_register_subject(NULL, "subject_room_study_demand", &subject_room_study_demand);
    lv_xml_register_subject(NULL, "subject_room_hall_demand", &subject_room_hall_demand);
    lv_xml_register_subject(NULL, "subject_dev_fan", &subject_dev_fan);
    lv_xml_register_subject(NULL, "subject_dev_humidifier", &subject_dev_humidifier);
    lv_xml_register_subject(NULL, "subject_dev_vent", &subject_dev_vent);
    lv_xml_register_subject(NULL, "subject_dev_geofence", &subject_dev_geofence);
    lv_xml_register_subject(NULL, "subject_humidifier_target", &subject_humidifier_target);
    lv_xml_register_subject(NULL, "subject_energy_kwh", &subject_energy_kwh);
    lv_xml_register_subject(NULL, "subject_energy_delta", &subject_energy_delta);
    lv_xml_register_subject(NULL, "subject_runtime_hrs", &subject_runtime_hrs);
    lv_xml_register_subject(NULL, "subject_eco_days", &subject_eco_days);
    lv_xml_register_subject(NULL, "subject_suggest_dismissed", &subject_suggest_dismissed);

    /* Register callbacks */
    lv_xml_register_event_cb(NULL, "thermostat_screen_attach_cb", thermostat_screen_attach_cb);
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if defined(LV_USE_XML) && LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "image_bg_normal", image_bg_normal);
    lv_xml_register_image(NULL, "image_bg_warm", image_bg_warm);
    lv_xml_register_image(NULL, "image_bg_cool", image_bg_cool);
    lv_xml_register_image(NULL, "image_logo", image_logo);
    lv_xml_register_image(NULL, "icon_flame", icon_flame);
    lv_xml_register_image(NULL, "icon_snowflake", icon_snowflake);
    lv_xml_register_image(NULL, "icon_auto", icon_auto);
    lv_xml_register_image(NULL, "icon_power", icon_power);
    lv_xml_register_image(NULL, "icon_droplet", icon_droplet);
    lv_xml_register_image(NULL, "icon_leaf", icon_leaf);
    lv_xml_register_image(NULL, "icon_fan", icon_fan);
    lv_xml_register_image(NULL, "icon_users", icon_users);
    lv_xml_register_image(NULL, "icon_thermo", icon_thermo);
#endif

#if !defined(LV_USE_XML) || LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
    /* rooms and schedule are dropped to stay within RAM. Only home and insights
     * are built; their tab-bar items reference the globals (which remain NULL
     * for the removed screens) but those tab items are also removed. */
    home = lv_obj_create(NULL);
    insights = lv_obj_create(NULL);

    home_create();
    insights_create();
#endif
}

void ti_thermostat_set_target(uint32_t target)
{
    ti_thermostat_target = target;
}

uint32_t ti_thermostat_get_target(void)
{
    return ti_thermostat_target;
}

bool ti_thermostat_check_target(uint32_t target)
{
    return (ti_thermostat_target & target) ? true : false;
}

/* Callbacks */
#if defined(LV_EDITOR_PREVIEW)
void __attribute__((weak)) thermostat_screen_attach_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("thermostat_screen_attach_cb was called\n");
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