# ==============================================================================
# rtLibs - Real-Time Motor Control Library Collection
# ==============================================================================
# This library provides header-only motor control algorithms for real-time
# control applications including:
#   - Control algorithms (PI, PID, PR, FWC, MTPA, TiTune, V/F, VSF, etc.)
#   - Observers (EST, ESMO, SpeedFR)
#   - Transforms (Clarke, Park, SVGEN, Voltage reconstruction)
#   - Filters (First-order, Second-order, Notch, Offset)
#   - Utilities (Angle generation, Datalog, Trajectory, CPU time)
#
# IMPORTANT: IPD module is EXCLUDED from this library
# ------------------------------------------------------------------------------
# IPD (Initial Position Detection) requires HAL (hal_FOCInterface.h) which is
# device/config specific. Applications using IPD should:
#   1. Link against rtLibs for other algorithms
#   2. Link against HAL for hardware interface
#   3. Explicitly add ipd.c to application sources
#
# Example:
#   target_link_libraries(motor_app PRIVATE rtLibs hal)
#   target_sources(motor_app PRIVATE
#       ${CMAKE_SOURCE_DIR}/source/rtLibs/control/ipd/source/ipd.c)
# ==============================================================================

set(TARGET rtLibs)

# Create INTERFACE library (header-only, no compilation)
add_library(${TARGET} INTERFACE)

# Add all include directories (excluding IPD)
target_include_directories(${TARGET} INTERFACE
    # Control algorithms
    control/dclink_ss/include
    control/dead_time_comp/include
    control/fwc/include
    control/mtpa/include
    control/pi/include
    control/pid/include
    control/pr/include
    control/TiTune/include
    control/vib_comp/include
    control/vs_freq/include
    control/vsf/include
    # NOTE: control/ipd/include EXCLUDED - requires HAL, compile at app level

    # Filters
    filter/filter_fo/include
    filter/filter_so/include
    filter/notch/include
    filter/offset/include

    # Math utilities
    math/include

    # Observers
    observers/esmo/include
    observers/est_lib/include
    observers/speedfr/include

    # Transforms
    transforms/clarke/include
    transforms/ipark/include
    transforms/park/include
    transforms/svgen/include
    transforms/volts/include

    # Utilities
    utilities/angle_gen/include
    utilities/cpu_time/include
    utilities/datalog/include
    utilities/rampgen/include
    utilities/traj/include
    utilities/types/include
)

# Link dependencies
# rtLibs depends on fastrts for fast math functions (sincos, sqrt, etc.)
target_link_libraries(${TARGET} INTERFACE fastrts)