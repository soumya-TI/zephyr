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
    # The drive model - the application logic this UI is a view of. Publishes
    # measured speed, current, temperature and load as subjects, and offers
    # motor_sim_publish() as the seam a real control loop calls instead.
    ${CMAKE_CURRENT_LIST_DIR}/custom/motor_sim.c

    # Projects subject_rpm_act onto the four odometer digit subjects. Needed
    # because the XML DSL cannot divide - see the file header.
    ${CMAKE_CURRENT_LIST_DIR}/custom/motor_digits.c

    # Screen navigation that frees the outgoing screen before building the next
    # (avoids the two-tree peak that overflows the 60 KB LVGL pool).
    ${CMAKE_CURRENT_LIST_DIR}/custom/nav.c
)
