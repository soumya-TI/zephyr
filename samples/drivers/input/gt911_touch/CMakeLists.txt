# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Default to the RPi LCD (F) shield (provides the GT911 on i2c0). An explicit
# --shield still overrides.
if(NOT DEFINED SHIELD)
  set(SHIELD waveshare_3_5_rpi_lcd_f)
endif()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gt911_touch)

target_sources(app PRIVATE src/main.c)
