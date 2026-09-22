# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr_foc_motor_control_lvgl)

set(SDK_PATH ../../../modules/hal/ti/am13/mcu_sdk)
set(DEVICE am13e230x)

if(BOARD_REVISION STREQUAL "e1")
    set(syscfg_gen_path "e1_syscfg_gen_code")
elseif(BOARD_REVISION STREQUAL "e2")
    set(syscfg_gen_path "e2_syscfg_gen_code")
else()
    set(syscfg_gen_path "e1_syscfg_gen_code")
endif()

# ---------------------------------------------------------------------------
# LVGL UI — generated source list and user config (both append to
# LV_EDITOR_PROJECT_SOURCES using ${CMAKE_CURRENT_LIST_DIR}, which resolves
# to ui/ when included from here).
# ---------------------------------------------------------------------------
set(UI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ui-ti-motor-control-optimize-v2)
include(${UI_DIR}/file_list_gen.cmake)
include(${UI_DIR}/user_config.cmake)

add_subdirectory(realtimelib)

# ---------------------------------------------------------------------------
# Compile definitions
# ---------------------------------------------------------------------------
target_compile_definitions(app PRIVATE
    MOTOR_THREE_SHUNT=1
    BSXL8323RH_REVB=1
    DATALOGF2_EN=1
    LV_LVGL_H_INCLUDE_SIMPLE
)

target_compile_options(app PRIVATE
    -march=armv8.1-m.main+cdecp0
    -Wno-unused
    -Ofast
)

# ---------------------------------------------------------------------------
# Include paths
# ---------------------------------------------------------------------------
target_include_directories(app PUBLIC
    .
    src

    # LVGL UI
    ${UI_DIR}

    # FOC motor control
    motor_control/common/appLib/include
    motor_control/common/hal/controllerInterface/include
    motor_control/common/hal/focInterface/include
    motor_control/common/hal/gateDriverInterface/include

    # AM13 SDK
    ../../../modules/hal/ti/am13/mcu_sdk/source/compiler/m33_ti_arm_clang
    ../../../modules/hal/ti/am13/mcu_sdk/source/device/am13e2x/include
    ../../../modules/hal/ti/am13/mcu_sdk/source/device/am13e2x/include/hw
    ../../../modules/hal/ti/am13/mcu_sdk/ti_sdk_config/am13e2x/default/device_support/include
    ../../../modules/hal/ti/am13/mcu_sdk/source/arch/include
    ../../../modules/hal/ti/am13/mcu_sdk/source/arch/m33/include
    ../../../modules/hal/ti/am13/mcu_sdk/ti_sdk_config/am13e2x/default/arch_cfg
    ../../../modules/hal/ti/am13/mcu_sdk/source/cmsis/Core/Include
    ../../../modules/hal/ti/am13/mcu_sdk/source/cmsis/DSP/Include
    ../../../modules/lib/cmsis-dsp/Include
    ../../../modules/hal/ti/am13/mcu_sdk/source/driverlib/am13e2x
    ../../../modules/hal/ti/am13/mcu_sdk/source/utils/log/include
    ../../../modules/hal/ti/am13/mcu_sdk/ti_sdk_config/am13e2x/default/utils_cfg

    # realtimelib
    realtimelib/control/pi/include
    realtimelib/control/pr/include
    realtimelib/control/ipd/include
    realtimelib/control/dead_time_comp/include
    realtimelib/control/TiTune/include
    realtimelib/control/fwc/include
    realtimelib/control/mtpa/include
    realtimelib/control/vs_freq/include
    realtimelib/control/vsf/include
    realtimelib/control/dclink_ss/include
    realtimelib/filter/filter_fo/include
    realtimelib/observers/est_lib/include
    realtimelib/observers/esmo/include
    realtimelib/observers/speedfr/include
    realtimelib/transforms/clarke/include
    realtimelib/transforms/ipark/include
    realtimelib/transforms/park/include
    realtimelib/transforms/svgen/include
    realtimelib/transforms/volts/include
    realtimelib/utilities/angle_gen/include
    realtimelib/utilities/cpu_time/include
    realtimelib/utilities/datalog/include
    realtimelib/utilities/traj/include
    realtimelib/utilities/types/include
    realtimelib/math/include
    mathlib/fastrts

    motor_control/common/hal/controllerInterface/include
    motor_control/common/hal/focInterface/include
    motor_control/common/hal/gateDriverInterface/DRV8323RH
    motor_control/common/hal/gateDriverInterface/include
    motor_control/common/appLib/include
    motor_control/common/motorConfig/DRV8323_3SH_MTR1

    ${SDK_PATH}/ti_sdk_config/${DEVICE}/default/device_support/include/
    ${SDK_PATH}/ti_sdk_config/${DEVICE}/default/utils_cfg/
    ${syscfg_gen_path}
)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
target_sources(app PRIVATE
    src/main.c
    src/foc_display_bridge.c
    isr.c

    # Shell commands
    shell_commands/cmd_motor.c

    # LVGL UI (generated + custom, excluding motor_sim.c — replaced by bridge)
    ${LV_EDITOR_PROJECT_SOURCES}

    # Syscfg generated peripheral init
    ${syscfg_gen_path}/ti_sdk_dl_config.c

    # realtimelib
    realtimelib/control/pi/source/pi.c
    realtimelib/control/pr/source/pr.c
    realtimelib/control/ipd/source/ipd.c
    realtimelib/control/dead_time_comp/source/dead_time_comp.c
    realtimelib/control/fwc/source/fwc.c
    realtimelib/control/mtpa/source/mtpa.c
    realtimelib/control/vs_freq/source/vs_freq.c
    realtimelib/control/vsf/source/vsf.c
    realtimelib/control/dclink_ss/source/dclink_ss.c

    realtimelib/transforms/clarke/source/clarke.c
    realtimelib/transforms/ipark/source/ipark.c
    realtimelib/transforms/park/source/park.c
    realtimelib/transforms/svgen/source/svgen.c
    realtimelib/transforms/svgen/source/svgen_current.c
    realtimelib/transforms/volts/source/volt_recons.c

    realtimelib/filter/filter_fo/source/filter_fo.c

    realtimelib/observers/esmo/source/esmo.c
    realtimelib/observers/speedfr/source/speedfr.c

    realtimelib/control/TiTune/source/TiTune_Flux.c
    realtimelib/control/TiTune/source/TiTune_J.c
    realtimelib/control/TiTune/source/TiTune_LDQ.c
    realtimelib/control/TiTune/source/TiTune_Rs.c
    realtimelib/control/TiTune/source/TiTune_SatLDQ.c

    realtimelib/utilities/angle_gen/source/angle_gen.c
    realtimelib/utilities/traj/source/traj.c
    realtimelib/utilities/cpu_time/source/cpu_time.c
    realtimelib/utilities/cpu_time/include/cpu_time.h
    realtimelib/utilities/datalog/source/datalogIF.c

    # HAL
    motor_control/common/hal/focInterface/source/hal_3SH_FOCInterface.c
    motor_control/common/hal/focInterface/source/hal_1SH_FOCInterface.c
    motor_control/common/hal/controllerInterface/source/hal.c
    motor_control/common/hal/gateDriverInterface/DRV8323RH/DRV8323RH_Interface.c

    # App lib
    motor_control/common/appLib/source/motor_common.c
    motor_control/common/appLib/source/motor1_init.c
    motor_control/common/appLib/source/Ti_Tune.c
    motor_control/common/appLib/source/application.c
    motor_control/common/motorConfig/DRV8323_3SH_MTR1/drv8323_3SH_motorConfig_M1.c
)
