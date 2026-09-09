# =============================================================================
# user_config.cmake - Custom source files for ti_thermostat_full
# =============================================================================
#
# Included by CMakeLists.txt after file_list_gen.cmake.  Appends hand-written
# logic modules that the XML DSL cannot express.

list(APPEND LV_EDITOR_PROJECT_SOURCES
    # Auto-mode changeover: compares subject_target_temp against subject_now_temp
    # (two subjects, not a subject vs a literal), which the XML comparators
    # cannot express.  Also drives subject_hvac_state as the sole writer.
    ${CMAKE_CURRENT_LIST_DIR}/custom/thermostat_logic.c

    # Background cross-dissolve stubs.  THERMOSTAT_USE_TRANSITIONS=0 in
    # thermostat_config.h compiles the whole feature out; the symbols still
    # need to exist because the generated init calls thermostat_transitions_init()
    # and home.xml declares thermostat_screen_attach_cb as an event callback.
    ${CMAKE_CURRENT_LIST_DIR}/custom/thermostat_transitions.c
)
