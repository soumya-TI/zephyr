# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Default to the Waveshare 3.5" RPi LCD (F) shield so that
# "west build -b lp_mspm0g3519 ." works without an explicit --shield.
if(NOT DEFINED SHIELD)
  set(SHIELD waveshare_3_5_rpi_lcd_f)
endif()

# pool_trim.conf overrides CONFIG_LV_Z_MEM_POOL_SIZE AFTER the shield's
# boards/lp_mspm0g3519.conf (which forces it back to 61440 = 93.75% of sram1).
# OVERLAY_CONFIG is consumed as EXTRA_CONF_FILE, processed last in the Kconfig
# merge order: CONF_FILE → shield_conf_files → EXTRA_CONF_FILE.
list(APPEND OVERLAY_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/pool_trim.conf")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ti_thermostat_full)

set(UI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ui)

# Generated UI source list (file_list_gen.cmake) + hand-written custom sources
# (user_config.cmake).  Both files append to LV_EDITOR_PROJECT_SOURCES using
# ${CMAKE_CURRENT_LIST_DIR}, which resolves to ui/ when included from here.
include(${UI_DIR}/file_list_gen.cmake)
include(${UI_DIR}/user_config.cmake)

target_sources(app PRIVATE
    src/main.c
    ${LV_EDITOR_PROJECT_SOURCES}
)

# The UI uses both same-directory ("bar_gen.h") and root-relative includes
# ("components/base/bar/bar_gen.h"), so the ui/ root alone is sufficient.
target_include_directories(app PRIVATE ${UI_DIR})

# Zephyr's LVGL module adds ${LVGL_DIR}/src/ to the include path (where both
# lvgl.h and lvgl_private.h live), but does NOT add ${LVGL_DIR}/ itself.
# The vendored UI headers include "lvgl.h" when this guard is set.
target_compile_definitions(app PRIVATE LV_LVGL_H_INCLUDE_SIMPLE)
