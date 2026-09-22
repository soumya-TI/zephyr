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
    # motor_sim.c is compiled so that motor_sim_publish() and motor_sim_trip()
    # are available as the subject-write API. However motor_sim_init() (which
    # would start the fake simulation timer) is NOT called — foc_display_bridge_init()
    # in src/foc_display_bridge.c registers the real 100 ms bridge timer instead.
    ${CMAKE_CURRENT_LIST_DIR}/custom/motor_sim.c
)
