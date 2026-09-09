/**
 * @file washing_machine_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "washing_machine_gen.h"

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

static uint32_t washing_machine_target = WASHING_MACHINE_TARGET_ALL;

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

lv_font_t * font_semibold_26;
extern lv_font_t font_semibold_26_data;
lv_font_t * font_semibold_22;
extern lv_font_t font_semibold_22_data;
lv_font_t * font_semibold_17;
extern lv_font_t font_semibold_17_data;
lv_font_t * font_medium_24;
extern lv_font_t font_medium_24_data;
lv_font_t * font_medium_14;
extern lv_font_t font_medium_14_data;
lv_font_t * font_medium_13;
extern lv_font_t font_medium_13_data;
lv_font_t * font_regular_16;
extern lv_font_t font_regular_16_data;
lv_font_t * font_regular_15;
extern lv_font_t font_regular_15_data;
lv_font_t * font_regular_14;
extern lv_font_t font_regular_14_data;
lv_font_t * font_hero_76;
extern lv_font_t font_hero_76_data;

/*----------------
 * Images
 *----------------*/

/* Targets: any */
const void * icon_prog_cotton = NULL;
extern const void * icon_prog_cotton_data;
const void * icon_prog_eco = NULL;
extern const void * icon_prog_eco_data;
const void * icon_prog_synth = NULL;
extern const void * icon_prog_synth_data;
const void * icon_prog_quick = NULL;
extern const void * icon_prog_quick_data;
const void * icon_water = NULL;
extern const void * icon_water_data;
const void * icon_rpm = NULL;
extern const void * icon_rpm_data;
const void * icon_ph_pre = NULL;
extern const void * icon_ph_pre_data;
const void * icon_ph_wash = NULL;
extern const void * icon_ph_wash_data;
const void * icon_ph_rinse = NULL;
extern const void * icon_ph_rinse_data;
const void * icon_ph_spin = NULL;
extern const void * icon_ph_spin_data;
const void * icon_unlock = NULL;
extern const void * icon_unlock_data;
const void * icon_back = NULL;
extern const void * icon_back_data;
const void * icon_options = NULL;
extern const void * icon_options_data;
const void * icon_service = NULL;
extern const void * icon_service_data;
const void * icon_board = NULL;
extern const void * icon_board_data;
const void * icon_opt_prewash = NULL;
extern const void * icon_opt_prewash_data;
const void * icon_opt_rinse = NULL;
extern const void * icon_opt_rinse_data;
const void * icon_opt_delay = NULL;
extern const void * icon_opt_delay_data;
const void * img_machine = NULL;
extern const void * img_machine_data;
const void * img_logostrip = NULL;
extern const void * img_logostrip_data;
const void * img_door = NULL;
extern const void * img_door_data;
const void * img_clothes = NULL;
extern const void * img_clothes_data;
const void * img_background = NULL;
extern const void * img_background_data;

/*----------------
 * Global styles
 *----------------*/

lv_style_t style_hero;
lv_style_t style_hero_unit;
lv_style_t style_heading;
lv_style_t style_stat_md;
lv_style_t style_body_strong;
lv_style_t style_key_legend;
lv_style_t style_body;
lv_style_t style_meta;
lv_style_t style_chip;
lv_style_t style_micro;
lv_style_t style_overline;
lv_style_t style_key_hint;
lv_style_t style_ink;
lv_style_t style_ink_2;
lv_style_t style_ink_3;
lv_style_t style_ink_overline;
lv_style_t style_ink_accent;
lv_style_t style_ink_inv;
lv_style_t style_icon_ink;
lv_style_t style_icon_2;
lv_style_t style_icon_accent;
lv_style_t style_icon_3;
lv_style_t style_icon_inv;
lv_style_t style_shadow_card;
lv_style_t style_shadow_soft;
lv_style_t style_shadow_knob;
lv_style_t style_screen_base;
lv_style_t style_card_surface;
lv_style_t style_pill;
lv_style_t style_tile_inset;
lv_style_t style_frame_door_outer;
lv_style_t style_frame_door_inner;
lv_style_t style_chip_dot_neutral;
lv_style_t style_chip_fill_accent;
lv_style_t style_chip_dot_accent;
lv_style_t style_chip_text_accent;
lv_style_t style_chip_fill_header;
lv_style_t style_arc_track;
lv_style_t style_arc_ind;
lv_style_t style_btn_primary;
lv_style_t style_btn_primary_pressed;
lv_style_t style_btn_danger;
lv_style_t style_btn_danger_pressed;
lv_style_t style_btn_accent;
lv_style_t style_btn_ghost;
lv_style_t style_btn_ghost_pressed;
lv_style_t style_btn_disabled;
lv_style_t style_text_disabled;
lv_style_t style_card_selected;
lv_style_t style_card_selected_pressed;
lv_style_t style_card_selected_edge;
lv_style_t style_key_accent;
lv_style_t style_key_accent_pressed;
lv_style_t style_key_danger;
lv_style_t style_key_danger_pressed;
lv_style_t style_seg_done;
lv_style_t style_seg_pending;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t subject_program;
lv_subject_t subject_temp_idx;
lv_subject_t subject_temp_c;
lv_subject_t subject_spin_rpm;
lv_subject_t subject_water_target;
lv_subject_t subject_opt_prewash;
lv_subject_t subject_opt_rinse;
lv_subject_t subject_opt_delay;
lv_subject_t subject_progress;
lv_subject_t subject_state;
lv_subject_t subject_door;
lv_subject_t subject_pwm_duty;
lv_subject_t subject_heater;
lv_subject_t subject_pump;
lv_subject_t subject_time_left_str;
lv_subject_t subject_time_unit_str;
lv_subject_t subject_phase;
lv_subject_t subject_ready;
lv_subject_t subject_hint_str;
lv_subject_t subject_duration_str;
lv_subject_t subject_program_name;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void washing_machine_init_gen(const char * asset_path)
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

    #if WASHING_MACHINE_CHECK_COMPILE_TARGET(WASHING_MACHINE_TARGET_ALL)
    if (washing_machine_check_target(WASHING_MACHINE_TARGET_ALL)) {
        if (!font_semibold_26) {
            /* font_semibold_26 */
            /* get font 'font_semibold_26' from a C array */
            font_semibold_26 = &font_semibold_26_data;

        }
        if (!font_semibold_22) {
            /* font_semibold_22 */
            /* get font 'font_semibold_22' from a C array */
            font_semibold_22 = &font_semibold_22_data;

        }
        if (!font_semibold_17) {
            /* font_semibold_17 */
            /* get font 'font_semibold_17' from a C array */
            font_semibold_17 = &font_semibold_17_data;

        }
        if (!font_medium_24) {
            /* font_medium_24 */
            /* get font 'font_medium_24' from a C array */
            font_medium_24 = &font_medium_24_data;

        }
        if (!font_medium_14) {
            /* font_medium_14 */
            /* get font 'font_medium_14' from a C array */
            font_medium_14 = &font_medium_14_data;

        }
        if (!font_medium_13) {
            /* font_medium_13 */
            /* get font 'font_medium_13' from a C array */
            font_medium_13 = &font_medium_13_data;

        }
        if (!font_regular_16) {
            /* font_regular_16 */
            /* get font 'font_regular_16' from a C array */
            font_regular_16 = &font_regular_16_data;

        }
        if (!font_regular_15) {
            /* font_regular_15 */
            /* get font 'font_regular_15' from a C array */
            font_regular_15 = &font_regular_15_data;

        }
        if (!font_regular_14) {
            /* font_regular_14 */
            /* get font 'font_regular_14' from a C array */
            font_regular_14 = &font_regular_14_data;

        }
        if (!font_hero_76) {
            /* font_hero_76 */
            /* get font 'font_hero_76' from a C array */
            font_hero_76 = &font_hero_76_data;

        }
    }
    #endif

    /*----------------
     * Images
     *----------------*/

    /* Targets: any */
    #if WASHING_MACHINE_CHECK_COMPILE_TARGET(WASHING_MACHINE_TARGET_ALL)
    if (washing_machine_check_target(WASHING_MACHINE_TARGET_ALL)) {
        /* icon_prog_cotton */
        if (!icon_prog_cotton) {
            icon_prog_cotton = &icon_prog_cotton_data;
        }
        /* icon_prog_eco */
        if (!icon_prog_eco) {
            icon_prog_eco = &icon_prog_eco_data;
        }
        /* icon_prog_synth */
        if (!icon_prog_synth) {
            icon_prog_synth = &icon_prog_synth_data;
        }
        /* icon_prog_quick */
        if (!icon_prog_quick) {
            icon_prog_quick = &icon_prog_quick_data;
        }
        /* icon_water */
        if (!icon_water) {
            icon_water = &icon_water_data;
        }
        /* icon_rpm */
        if (!icon_rpm) {
            icon_rpm = &icon_rpm_data;
        }
        /* icon_ph_pre */
        if (!icon_ph_pre) {
            icon_ph_pre = &icon_ph_pre_data;
        }
        /* icon_ph_wash */
        if (!icon_ph_wash) {
            icon_ph_wash = &icon_ph_wash_data;
        }
        /* icon_ph_rinse */
        if (!icon_ph_rinse) {
            icon_ph_rinse = &icon_ph_rinse_data;
        }
        /* icon_ph_spin */
        if (!icon_ph_spin) {
            icon_ph_spin = &icon_ph_spin_data;
        }
        /* icon_unlock */
        if (!icon_unlock) {
            icon_unlock = &icon_unlock_data;
        }
        /* icon_back */
        if (!icon_back) {
            icon_back = &icon_back_data;
        }
        /* icon_options */
        if (!icon_options) {
            icon_options = &icon_options_data;
        }
        /* icon_service */
        if (!icon_service) {
            icon_service = &icon_service_data;
        }
        /* icon_board */
        if (!icon_board) {
            icon_board = &icon_board_data;
        }
        /* icon_opt_prewash */
        if (!icon_opt_prewash) {
            icon_opt_prewash = &icon_opt_prewash_data;
        }
        /* icon_opt_rinse */
        if (!icon_opt_rinse) {
            icon_opt_rinse = &icon_opt_rinse_data;
        }
        /* icon_opt_delay */
        if (!icon_opt_delay) {
            icon_opt_delay = &icon_opt_delay_data;
        }
        /* img_machine */
        if (!img_machine) {
            img_machine = &img_machine_data;
        }
        /* img_logostrip */
        if (!img_logostrip) {
            img_logostrip = &img_logostrip_data;
        }
        /* img_door */
        if (!img_door) {
            img_door = &img_door_data;
        }
        /* img_clothes */
        if (!img_clothes) {
            img_clothes = &img_clothes_data;
        }
        /* img_background */
        if (!img_background) {
            img_background = &img_background_data;
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
        lv_style_init(&style_heading);
        lv_style_init(&style_stat_md);
        lv_style_init(&style_body_strong);
        lv_style_init(&style_key_legend);
        lv_style_init(&style_body);
        lv_style_init(&style_meta);
        lv_style_init(&style_chip);
        lv_style_init(&style_micro);
        lv_style_init(&style_overline);
        lv_style_init(&style_key_hint);
        lv_style_init(&style_ink);
        lv_style_init(&style_ink_2);
        lv_style_init(&style_ink_3);
        lv_style_init(&style_ink_overline);
        lv_style_init(&style_ink_accent);
        lv_style_init(&style_ink_inv);
        lv_style_init(&style_icon_ink);
        lv_style_init(&style_icon_2);
        lv_style_init(&style_icon_accent);
        lv_style_init(&style_icon_3);
        lv_style_init(&style_icon_inv);
        lv_style_init(&style_shadow_card);
        lv_style_init(&style_shadow_soft);
        lv_style_init(&style_shadow_knob);
        lv_style_init(&style_screen_base);
        lv_style_init(&style_card_surface);
        lv_style_init(&style_pill);
        lv_style_init(&style_tile_inset);
        lv_style_init(&style_frame_door_outer);
        lv_style_init(&style_frame_door_inner);
        lv_style_init(&style_chip_dot_neutral);
        lv_style_init(&style_chip_fill_accent);
        lv_style_init(&style_chip_dot_accent);
        lv_style_init(&style_chip_text_accent);
        lv_style_init(&style_chip_fill_header);
        lv_style_init(&style_arc_track);
        lv_style_init(&style_arc_ind);
        lv_style_init(&style_btn_primary);
        lv_style_init(&style_btn_primary_pressed);
        lv_style_init(&style_btn_danger);
        lv_style_init(&style_btn_danger_pressed);
        lv_style_init(&style_btn_accent);
        lv_style_init(&style_btn_ghost);
        lv_style_init(&style_btn_ghost_pressed);
        lv_style_init(&style_btn_disabled);
        lv_style_init(&style_text_disabled);
        lv_style_init(&style_card_selected);
        lv_style_init(&style_card_selected_pressed);
        lv_style_init(&style_card_selected_edge);
        lv_style_init(&style_key_accent);
        lv_style_init(&style_key_accent_pressed);
        lv_style_init(&style_key_danger);
        lv_style_init(&style_key_danger_pressed);
        lv_style_init(&style_seg_done);
        lv_style_init(&style_seg_pending);

        lv_style_set_text_font(&style_hero, font_hero_76);
        lv_style_set_text_letter_space(&style_hero, -3);
        lv_style_set_text_font(&style_hero_unit, font_medium_24);
        lv_style_set_text_letter_space(&style_hero_unit, -1);
        lv_style_set_text_font(&style_heading, font_semibold_26);
        lv_style_set_text_letter_space(&style_heading, -1);
        lv_style_set_text_font(&style_stat_md, font_semibold_22);
        lv_style_set_text_font(&style_body_strong, font_semibold_17);
        lv_style_set_text_font(&style_key_legend, font_semibold_17);
        lv_style_set_text_letter_space(&style_key_legend, 2);
        lv_style_set_text_font(&style_body, font_regular_16);
        lv_style_set_text_font(&style_meta, font_regular_15);
        lv_style_set_text_font(&style_chip, font_medium_14);
        lv_style_set_text_font(&style_micro, font_regular_14);
        lv_style_set_text_font(&style_overline, font_medium_13);
        lv_style_set_text_letter_space(&style_overline, 1);
        lv_style_set_text_font(&style_key_hint, font_medium_13);
        lv_style_set_text_color(&style_ink, CONST_TEXT_PRIMARY);
        lv_style_set_text_color(&style_ink_2, CONST_TEXT_SECONDARY);
        lv_style_set_text_color(&style_ink_3, CONST_TEXT_TERTIARY);
        lv_style_set_text_color(&style_ink_overline, CONST_TEXT_OVERLINE);
        lv_style_set_text_color(&style_ink_accent, CONST_ACCENT);
        lv_style_set_text_color(&style_ink_inv, CONST_ON_ACCENT);
        lv_style_set_image_recolor(&style_icon_ink, CONST_TEXT_PRIMARY);
        lv_style_set_image_recolor_opa(&style_icon_ink, 255);
        lv_style_set_image_recolor(&style_icon_2, CONST_TEXT_SECONDARY);
        lv_style_set_image_recolor_opa(&style_icon_2, 255);
        lv_style_set_image_recolor(&style_icon_accent, CONST_ACCENT);
        lv_style_set_image_recolor_opa(&style_icon_accent, 255);
        lv_style_set_image_recolor(&style_icon_3, CONST_TEXT_TERTIARY);
        lv_style_set_image_recolor_opa(&style_icon_3, 255);
        lv_style_set_image_recolor(&style_icon_inv, CONST_ON_ACCENT);
        lv_style_set_image_recolor_opa(&style_icon_inv, 255);
        lv_style_set_shadow_width(&style_shadow_card, 16);
        lv_style_set_shadow_offset_y(&style_shadow_card, 5);
        lv_style_set_shadow_spread(&style_shadow_card, -4);
        lv_style_set_shadow_opa(&style_shadow_card, 24);
        lv_style_set_shadow_color(&style_shadow_card, lv_color_hex(0x141C22));
        lv_style_set_shadow_width(&style_shadow_soft, 12);
        lv_style_set_shadow_offset_y(&style_shadow_soft, 4);
        lv_style_set_shadow_spread(&style_shadow_soft, -4);
        lv_style_set_shadow_opa(&style_shadow_soft, 20);
        lv_style_set_shadow_color(&style_shadow_soft, lv_color_hex(0x141C22));
        lv_style_set_shadow_width(&style_shadow_knob, 6);
        lv_style_set_shadow_offset_y(&style_shadow_knob, 2);
        lv_style_set_shadow_spread(&style_shadow_knob, -1);
        lv_style_set_shadow_opa(&style_shadow_knob, 46);
        lv_style_set_shadow_color(&style_shadow_knob, lv_color_hex(0x141C22));
        lv_style_set_bg_color(&style_screen_base, CONST_SURFACE_CANVAS);
        lv_style_set_bg_opa(&style_screen_base, (255 * 100 / 100));
        lv_style_set_bg_grad_color(&style_screen_base, lv_color_hex(0xE9EEF1));
        lv_style_set_bg_grad_dir(&style_screen_base, LV_GRAD_DIR_VER);
        lv_style_set_bg_image_src(&style_screen_base, img_background);
        lv_style_set_bg_color(&style_card_surface, CONST_SURFACE_CARD);
        lv_style_set_bg_opa(&style_card_surface, (255 * 70 / 100));
        lv_style_set_border_color(&style_card_surface, CONST_BORDER);
        lv_style_set_border_width(&style_card_surface, 1);
        lv_style_set_border_opa(&style_card_surface, (255 * 100 / 100));
        lv_style_set_radius(&style_card_surface, CONST_RADIUS_MD);
        lv_style_set_radius(&style_pill, 32767);
        lv_style_set_bg_color(&style_tile_inset, CONST_SURFACE_CANVAS);
        lv_style_set_bg_opa(&style_tile_inset, (255 * 100 / 100));
        lv_style_set_border_width(&style_tile_inset, 0);
        lv_style_set_radius(&style_tile_inset, CONST_RADIUS_SM);
        lv_style_set_bg_color(&style_frame_door_outer, CONST_ACCENT);
        lv_style_set_bg_opa(&style_frame_door_outer, (255 * 100 / 100));
        lv_style_set_radius(&style_frame_door_outer, 0);
        lv_style_set_border_width(&style_frame_door_outer, 0);
        lv_style_set_bg_color(&style_frame_door_inner, CONST_SURFACE_CANVAS);
        lv_style_set_bg_opa(&style_frame_door_inner, (255 * 100 / 100));
        lv_style_set_bg_grad_color(&style_frame_door_inner, lv_color_hex(0xE9EEF1));
        lv_style_set_bg_grad_dir(&style_frame_door_inner, LV_GRAD_DIR_VER);
        lv_style_set_radius(&style_frame_door_inner, 18);
        lv_style_set_border_width(&style_frame_door_inner, 0);
        lv_style_set_bg_color(&style_chip_dot_neutral, CONST_TEXT_SECONDARY);
        lv_style_set_bg_opa(&style_chip_dot_neutral, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_fill_accent, CONST_TINT_ACCENT);
        lv_style_set_bg_opa(&style_chip_fill_accent, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_dot_accent, CONST_ACCENT);
        lv_style_set_bg_opa(&style_chip_dot_accent, (255 * 100 / 100));
        lv_style_set_text_color(&style_chip_text_accent, CONST_ACCENT);
        lv_style_set_bg_color(&style_chip_fill_header, CONST_SURFACE_CARD);
        lv_style_set_bg_opa(&style_chip_fill_header, (255 * 100 / 100));
        lv_style_set_border_color(&style_chip_fill_header, CONST_BORDER);
        lv_style_set_border_width(&style_chip_fill_header, 1);
        lv_style_set_border_opa(&style_chip_fill_header, (255 * 100 / 100));
        lv_style_set_arc_color(&style_arc_track, lv_color_hex(0xD5DBDF));
        lv_style_set_arc_opa(&style_arc_track, (255 * 100 / 100));
        lv_style_set_arc_width(&style_arc_track, 12);
        lv_style_set_arc_rounded(&style_arc_track, true);
        lv_style_set_arc_color(&style_arc_ind, CONST_ACCENT_BRIGHT);
        lv_style_set_arc_opa(&style_arc_ind, (255 * 100 / 100));
        lv_style_set_arc_width(&style_arc_ind, 12);
        lv_style_set_arc_rounded(&style_arc_ind, true);
        lv_style_set_bg_color(&style_btn_primary, CONST_ACCENT);
        lv_style_set_bg_opa(&style_btn_primary, (255 * 100 / 100));
        lv_style_set_radius(&style_btn_primary, CONST_RADIUS_MD);
        lv_style_set_border_width(&style_btn_primary, 0);
        lv_style_set_bg_color(&style_btn_primary_pressed, CONST_ACCENT_PRESSED);
        lv_style_set_bg_color(&style_btn_danger, CONST_DANGER);
        lv_style_set_bg_opa(&style_btn_danger, (255 * 100 / 100));
        lv_style_set_radius(&style_btn_danger, CONST_RADIUS_MD);
        lv_style_set_border_width(&style_btn_danger, 0);
        lv_style_set_bg_color(&style_btn_danger_pressed, CONST_DANGER_PRESSED);
        lv_style_set_bg_color(&style_btn_accent, CONST_ACCENT);
        lv_style_set_bg_opa(&style_btn_accent, (255 * 100 / 100));
        lv_style_set_radius(&style_btn_accent, CONST_RADIUS_MD);
        lv_style_set_border_width(&style_btn_accent, 0);
        lv_style_set_bg_color(&style_btn_ghost, CONST_SURFACE_CARD);
        lv_style_set_bg_opa(&style_btn_ghost, (255 * 100 / 100));
        lv_style_set_border_color(&style_btn_ghost, CONST_BORDER);
        lv_style_set_border_width(&style_btn_ghost, 1);
        lv_style_set_border_opa(&style_btn_ghost, (255 * 100 / 100));
        lv_style_set_radius(&style_btn_ghost, CONST_RADIUS_MD);
        lv_style_set_bg_color(&style_btn_ghost_pressed, CONST_DIVIDER);
        lv_style_set_bg_color(&style_btn_disabled, CONST_BORDER);
        lv_style_set_bg_opa(&style_btn_disabled, (255 * 100 / 100));
        lv_style_set_border_width(&style_btn_disabled, 0);
        lv_style_set_text_color(&style_text_disabled, CONST_TEXT_TERTIARY);
        lv_style_set_bg_color(&style_card_selected, CONST_ACCENT);
        lv_style_set_bg_opa(&style_card_selected, (255 * 100 / 100));
        lv_style_set_border_width(&style_card_selected, 0);
        lv_style_set_bg_color(&style_card_selected_pressed, CONST_ACCENT_PRESSED);
        lv_style_set_bg_opa(&style_card_selected_pressed, (255 * 100 / 100));
        lv_style_set_border_width(&style_card_selected_pressed, 0);
        lv_style_set_border_color(&style_card_selected_edge, CONST_ACCENT);
        lv_style_set_border_opa(&style_card_selected_edge, (255 * 100 / 100));
        lv_style_set_bg_color(&style_key_accent, CONST_ACCENT);
        lv_style_set_bg_opa(&style_key_accent, (255 * 100 / 100));
        lv_style_set_bg_color(&style_key_accent_pressed, CONST_ACCENT_PRESSED);
        lv_style_set_bg_opa(&style_key_accent_pressed, (255 * 100 / 100));
        lv_style_set_bg_color(&style_key_danger, CONST_DANGER);
        lv_style_set_bg_opa(&style_key_danger, (255 * 100 / 100));
        lv_style_set_bg_color(&style_key_danger_pressed, CONST_DANGER_PRESSED);
        lv_style_set_bg_opa(&style_key_danger_pressed, (255 * 100 / 100));
        lv_style_set_bg_color(&style_seg_done, CONST_ACCENT);
        lv_style_set_bg_opa(&style_seg_done, (255 * 100 / 100));
        lv_style_set_bg_color(&style_seg_pending, CONST_TRACK);
        lv_style_set_bg_opa(&style_seg_pending, (255 * 100 / 100));

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&subject_program, -1);
    lv_subject_set_min_value_int(&subject_program, -1);
    lv_subject_set_max_value_int(&subject_program, 3);
    lv_subject_init_int(&subject_temp_idx, 0);
    lv_subject_set_min_value_int(&subject_temp_idx, 0);
    lv_subject_set_max_value_int(&subject_temp_idx, 4);
    lv_subject_init_int(&subject_temp_c, 0);
    lv_subject_set_min_value_int(&subject_temp_c, 0);
    lv_subject_set_max_value_int(&subject_temp_c, 90);
    lv_subject_init_int(&subject_spin_rpm, 1200);
    lv_subject_set_min_value_int(&subject_spin_rpm, 400);
    lv_subject_set_max_value_int(&subject_spin_rpm, 1400);
    lv_subject_init_int(&subject_water_target, 62);
    lv_subject_set_min_value_int(&subject_water_target, 0);
    lv_subject_set_max_value_int(&subject_water_target, 100);
    lv_subject_init_int(&subject_opt_prewash, 0);
    lv_subject_set_min_value_int(&subject_opt_prewash, 0);
    lv_subject_set_max_value_int(&subject_opt_prewash, 1);
    lv_subject_init_int(&subject_opt_rinse, 1);
    lv_subject_set_min_value_int(&subject_opt_rinse, 0);
    lv_subject_set_max_value_int(&subject_opt_rinse, 1);
    lv_subject_init_int(&subject_opt_delay, 0);
    lv_subject_set_min_value_int(&subject_opt_delay, 0);
    lv_subject_set_max_value_int(&subject_opt_delay, 1);
    lv_subject_init_int(&subject_progress, 0);
    lv_subject_set_min_value_int(&subject_progress, 0);
    lv_subject_set_max_value_int(&subject_progress, 100);
    lv_subject_init_int(&subject_state, 1);
    lv_subject_set_min_value_int(&subject_state, 0);
    lv_subject_set_max_value_int(&subject_state, 3);
    lv_subject_init_int(&subject_door, 1);
    lv_subject_set_min_value_int(&subject_door, 0);
    lv_subject_set_max_value_int(&subject_door, 1);
    lv_subject_init_int(&subject_pwm_duty, 74);
    lv_subject_set_min_value_int(&subject_pwm_duty, 0);
    lv_subject_set_max_value_int(&subject_pwm_duty, 100);
    lv_subject_init_int(&subject_heater, 0);
    lv_subject_set_min_value_int(&subject_heater, 0);
    lv_subject_set_max_value_int(&subject_heater, 1);
    lv_subject_init_int(&subject_pump, 0);
    lv_subject_set_min_value_int(&subject_pump, 0);
    lv_subject_set_max_value_int(&subject_pump, 1);
    static char subject_time_left_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_time_left_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_time_left_str,
                           subject_time_left_str_buf,
                           subject_time_left_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "2:15"
                          );
    static char subject_time_unit_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_time_unit_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_time_unit_str,
                           subject_time_unit_str_buf,
                           subject_time_unit_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "h"
                          );
    lv_subject_init_int(&subject_phase, 0);
    lv_subject_set_min_value_int(&subject_phase, 0);
    lv_subject_set_max_value_int(&subject_phase, 3);
    lv_subject_init_int(&subject_ready, 0);
    lv_subject_set_min_value_int(&subject_ready, 0);
    lv_subject_set_max_value_int(&subject_ready, 1);
    static char subject_hint_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_hint_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_hint_str,
                           subject_hint_str_buf,
                           subject_hint_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Select a fabric"
                          );
    static char subject_duration_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_duration_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_duration_str,
                           subject_duration_str_buf,
                           subject_duration_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "2h 15m"
                          );
    static char subject_program_name_buf[UI_SUBJECT_STRING_LENGTH];
    static char subject_program_name_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&subject_program_name,
                           subject_program_name_buf,
                           subject_program_name_prev_buf,
                           UI_SUBJECT_STRING_LENGTH,
                           "Cotton"
                          );

    /*----------------
     * Translations
     *----------------*/

#if defined(LV_USE_XML) && LV_USE_XML
    /* Register widgets */

    /* Check all fonts / default if needed. This prevents fonts that are used in one target but
       defined in another from causing assertion failures during rendering of the Preview. */
    check_font(&font_semibold_26, "font_semibold_26");
    check_font(&font_semibold_22, "font_semibold_22");
    check_font(&font_semibold_17, "font_semibold_17");
    check_font(&font_medium_24, "font_medium_24");
    check_font(&font_medium_14, "font_medium_14");
    check_font(&font_medium_13, "font_medium_13");
    check_font(&font_regular_16, "font_regular_16");
    check_font(&font_regular_15, "font_regular_15");
    check_font(&font_regular_14, "font_regular_14");
    check_font(&font_hero_76, "font_hero_76");

    /* Register fonts */
    lv_xml_register_font(NULL, "font_semibold_26", font_semibold_26);
    lv_xml_register_font(NULL, "font_semibold_22", font_semibold_22);
    lv_xml_register_font(NULL, "font_semibold_17", font_semibold_17);
    lv_xml_register_font(NULL, "font_medium_24", font_medium_24);
    lv_xml_register_font(NULL, "font_medium_14", font_medium_14);
    lv_xml_register_font(NULL, "font_medium_13", font_medium_13);
    lv_xml_register_font(NULL, "font_regular_16", font_regular_16);
    lv_xml_register_font(NULL, "font_regular_15", font_regular_15);
    lv_xml_register_font(NULL, "font_regular_14", font_regular_14);
    lv_xml_register_font(NULL, "font_hero_76", font_hero_76);

    /* Register subjects */
    lv_xml_register_subject(NULL, "subject_program", &subject_program);
    lv_xml_register_subject(NULL, "subject_temp_idx", &subject_temp_idx);
    lv_xml_register_subject(NULL, "subject_temp_c", &subject_temp_c);
    lv_xml_register_subject(NULL, "subject_spin_rpm", &subject_spin_rpm);
    lv_xml_register_subject(NULL, "subject_water_target", &subject_water_target);
    lv_xml_register_subject(NULL, "subject_opt_prewash", &subject_opt_prewash);
    lv_xml_register_subject(NULL, "subject_opt_rinse", &subject_opt_rinse);
    lv_xml_register_subject(NULL, "subject_opt_delay", &subject_opt_delay);
    lv_xml_register_subject(NULL, "subject_progress", &subject_progress);
    lv_xml_register_subject(NULL, "subject_state", &subject_state);
    lv_xml_register_subject(NULL, "subject_door", &subject_door);
    lv_xml_register_subject(NULL, "subject_pwm_duty", &subject_pwm_duty);
    lv_xml_register_subject(NULL, "subject_heater", &subject_heater);
    lv_xml_register_subject(NULL, "subject_pump", &subject_pump);
    lv_xml_register_subject(NULL, "subject_time_left_str", &subject_time_left_str);
    lv_xml_register_subject(NULL, "subject_time_unit_str", &subject_time_unit_str);
    lv_xml_register_subject(NULL, "subject_phase", &subject_phase);
    lv_xml_register_subject(NULL, "subject_ready", &subject_ready);
    lv_xml_register_subject(NULL, "subject_hint_str", &subject_hint_str);
    lv_xml_register_subject(NULL, "subject_duration_str", &subject_duration_str);
    lv_xml_register_subject(NULL, "subject_program_name", &subject_program_name);

    /* Register callbacks */
    lv_xml_register_event_cb(NULL, "wash_sim_screen_loaded_cb", wash_sim_screen_loaded_cb);
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if defined(LV_USE_XML) && LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "icon_prog_cotton", icon_prog_cotton);
    lv_xml_register_image(NULL, "icon_prog_eco", icon_prog_eco);
    lv_xml_register_image(NULL, "icon_prog_synth", icon_prog_synth);
    lv_xml_register_image(NULL, "icon_prog_quick", icon_prog_quick);
    lv_xml_register_image(NULL, "icon_water", icon_water);
    lv_xml_register_image(NULL, "icon_rpm", icon_rpm);
    lv_xml_register_image(NULL, "icon_ph_pre", icon_ph_pre);
    lv_xml_register_image(NULL, "icon_ph_wash", icon_ph_wash);
    lv_xml_register_image(NULL, "icon_ph_rinse", icon_ph_rinse);
    lv_xml_register_image(NULL, "icon_ph_spin", icon_ph_spin);
    lv_xml_register_image(NULL, "icon_unlock", icon_unlock);
    lv_xml_register_image(NULL, "icon_back", icon_back);
    lv_xml_register_image(NULL, "icon_options", icon_options);
    lv_xml_register_image(NULL, "icon_service", icon_service);
    lv_xml_register_image(NULL, "icon_board", icon_board);
    lv_xml_register_image(NULL, "icon_opt_prewash", icon_opt_prewash);
    lv_xml_register_image(NULL, "icon_opt_rinse", icon_opt_rinse);
    lv_xml_register_image(NULL, "icon_opt_delay", icon_opt_delay);
    lv_xml_register_image(NULL, "img_machine", img_machine);
    lv_xml_register_image(NULL, "img_logostrip", img_logostrip);
    lv_xml_register_image(NULL, "img_door", img_door);
    lv_xml_register_image(NULL, "img_clothes", img_clothes);
    lv_xml_register_image(NULL, "img_background", img_background);
#endif

#if !defined(LV_USE_XML) || LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
#endif
}

void washing_machine_set_target(uint32_t target)
{
    washing_machine_target = target;
}

uint32_t washing_machine_get_target(void)
{
    return washing_machine_target;
}

bool washing_machine_check_target(uint32_t target)
{
    return (washing_machine_target & target) ? true : false;
}

/* Callbacks */
#if defined(LV_EDITOR_PREVIEW)
void __attribute__((weak)) wash_sim_screen_loaded_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("wash_sim_screen_loaded_cb was called\n");
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