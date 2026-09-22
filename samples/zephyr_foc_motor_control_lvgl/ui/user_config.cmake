# =============================================================================
# user_config.cmake - Add your custom source files here
# =============================================================================
#
# This file is included by the generated CMakeLists.txt and allows you to
# add extra source files to the project without modifying generated files
# (which may be overwritten).
#
# To add your own sources, append them to LV_EDITOR_PROJECT_SOURCES:
#
#   list(APPEND LV_EDITOR_PROJECT_SOURCES
#       ${CMAKE_CURRENT_LIST_DIR}/src/my_widget.c
#       ${CMAKE_CURRENT_LIST_DIR}/src/my_screen.c
#   )
#
# Tip:
#   - Use ${CMAKE_CURRENT_LIST_DIR} to get paths relative to this file
#
# =============================================================================

list(APPEND LV_EDITOR_PROJECT_SOURCES
    # motor_sim.c is intentionally excluded here. The FOC bridge
    # (src/foc_display_bridge.c) replaces the simulation timer and calls
    # motor_sim_publish() with real motorVars_M1 data instead.
    # motor_sim_publish() itself is still compiled (it's in motor_sim.c),
    # but its 100 ms simulation tick_cb is not registered.
    ${CMAKE_CURRENT_LIST_DIR}/custom/motor_sim.c

    # Projects subject_rpm_act onto the four odometer digit subjects.
    ${CMAKE_CURRENT_LIST_DIR}/custom/motor_digits.c

    # Screen navigation (frees outgoing screen before building next).
    ${CMAKE_CURRENT_LIST_DIR}/custom/nav.c
)
