list(
  APPEND
  LV_EDITOR_PROJECT_SOURCES
  # base/bar_gen.c was only used by row_room (rooms screen — dropped).
  ${CMAKE_CURRENT_LIST_DIR}/components/base/card/card_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/base/column/column_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/base/container/container_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/base/label/label_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/base/panel/panel_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/base/row/row_gen.c
  # base/slider_gen.c is never called by any remaining screen — dropped.
  # base/switch_gen.c was only used by row_toggle (rooms screen — dropped).
  ${CMAKE_CURRENT_LIST_DIR}/components/btn_step/btn_step_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/card_suggest/card_suggest_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/chart_week/chart_week_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/chip_value/chip_value_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/chip/chip_gen.c
  # day_chip_gen.c was only used by schedule screen — dropped.
  ${CMAKE_CURRENT_LIST_DIR}/components/dial/dial_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/mode_pill_auto/mode_pill_auto_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/mode_pill_cool/mode_pill_cool_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/mode_pill_heat/mode_pill_heat_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/overline/overline_gen.c
  # row_block_gen.c was only used by schedule screen — dropped.
  # row_room_gen.c was only used by rooms screen — dropped.
  # row_toggle_gen.c was only used by rooms screen — dropped.
  ${CMAKE_CURRENT_LIST_DIR}/components/stat_tile/stat_tile_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/surface/surface_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/tab_bar/tab_bar_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/tab_item/tab_item_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/components/top_bar/top_bar_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_hero_88_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_medium_11_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_medium_12_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_medium_14_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_medium_30_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_regular_11_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_regular_12_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_regular_14_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_semibold_14_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_semibold_15_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_semibold_17_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_semibold_20_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_semibold_22_data.c
  ${CMAKE_CURRENT_LIST_DIR}/fonts/font_semibold_28_data.c
  ${CMAKE_CURRENT_LIST_DIR}/images/icon_auto_data.c
  ${CMAKE_CURRENT_LIST_DIR}/images/icon_droplet_data.c
  ${CMAKE_CURRENT_LIST_DIR}/images/icon_fan_data.c
  ${CMAKE_CURRENT_LIST_DIR}/images/icon_flame_data.c
  ${CMAKE_CURRENT_LIST_DIR}/images/icon_leaf_data.c
  ${CMAKE_CURRENT_LIST_DIR}/images/icon_power_data.c
  ${CMAKE_CURRENT_LIST_DIR}/images/icon_snowflake_data.c
  ${CMAKE_CURRENT_LIST_DIR}/images/icon_thermo_data.c
  ${CMAKE_CURRENT_LIST_DIR}/images/icon_users_data.c
  # Background images (320x480 RGB565) from the upstream repo are ~300 KB each
  # and three of them (~900 KB total) would overflow the LP-MSPM0G3519's 512 KB
  # flash by nearly 2x.  Replaced by 1x1 placeholder descriptors defined in
  # image_bg_stub_data.c that satisfy the ti_thermostat_gen.c extern declarations
  # without the flash cost.  The screen gradient background fills the visible
  # area; with THERMOSTAT_USE_TRANSITIONS=0 the 1x1 objects are invisible.
  ${CMAKE_CURRENT_LIST_DIR}/images/image_bg_stub_data.c
  ${CMAKE_CURRENT_LIST_DIR}/images/image_logo_data.c
  ${CMAKE_CURRENT_LIST_DIR}/screens/home_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/screens/insights_gen.c
  # rooms_gen.c and schedule_gen.c dropped to free RAM (kept home + insights).
  ${CMAKE_CURRENT_LIST_DIR}/ti_thermostat_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/ti_thermostat.c
  # wd_schedule_chart files dropped along with schedule screen.
)
