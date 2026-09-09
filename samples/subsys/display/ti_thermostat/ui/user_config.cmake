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
    # Derived state the XML cannot express - the Auto-mode comparison of the
    # setpoint against the outdoor temperature - plus the one-decimal display
    # strings that bind_text-fmt cannot render.
    ${CMAKE_CURRENT_LIST_DIR}/custom/thermostat_logic.c

    # The relative drag gesture on the temperature field. LVGL's slider is
    # absolute (it jumps to the finger) and the XML DSL has no gesture
    # vocabulary, so this is a real capability boundary.
    ${CMAKE_CURRENT_LIST_DIR}/custom/thermostat_drag.c

    # Positions the dashed current-temperature marker on the home field. The DSL
    # can bind a subject to text, a flag, a style or a value - but not to a
    # position, and this is temperature-to-pixel arithmetic.
    ${CMAKE_CURRENT_LIST_DIR}/custom/thermostat_marker.c

    # Simulated sensors, and the thermostat_inputs_publish() seam that real
    # sensor code calls. Behind THERMOSTAT_SIM_ENABLE in custom/thermostat_config.h.
    ${CMAKE_CURRENT_LIST_DIR}/custom/thermostat_sim.c
)
