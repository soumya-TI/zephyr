.. zephyr:code-sample:: ti_thermostat
   :name: TI MSPM0 LVGL Thermostat
   :relevant-api: display_interface input_interface

   Run a four-screen LVGL 9.5 smart-thermostat UI on a 320x480 ST7796S + GT911
   panel.

Overview
********

This sample runs a smart-thermostat user interface authored with the LVGL
Editor (LVGL 9.5) on the Waveshare 3.5" RPi LCD (F) panel. The UI has three
screens -- ``home`` (drag-to-set temperature field), ``climate`` (mode
selection) and ``insights`` (trend chart and stats) -- and a built-in sensor
simulator that animates the readings so the panel shows live motion with no
external input.

The UI itself is *vendored* under ``ui/`` and built from its generated C
sources; ``src/main.c`` only waits for the display, calls
``thermostat_init()`` and pumps ``lv_timer_handler()``. Zephyr auto-initializes
LVGL and binds the display / GT911 pointer device through the shield's
``chosen`` and ``zephyr,lvgl-pointer-input`` nodes.

Vendored UI provenance
======================

``ui/`` is a snapshot of the compilable subset (generated C, custom logic,
compiled-in fonts and icons) of ``lvgl-services/ti-thermostat-alternate`` at
commit ``ac49c744c3286ea354fff558b8fbb6aa71fe8398``. The host simulator
(``sim/``), editor preview binaries, PNG/TTF asset sources and ``*.xml``
authoring files are omitted -- only what the firmware build needs is kept. Do
not hand-edit ``*_gen.*`` files; regenerate them from the upstream project.

Requirements
************

* An :ref:`lp_mspm0g3519` LaunchPad.
* The :ref:`waveshare_3_5_rpi_lcd_f` shield (ST7796S display + GT911 touch on
  the native MSPM0 I2C controller). See that shield's documentation for the
  full wiring table.

Building and Running
********************

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/display/ti_thermostat
   :board: lp_mspm0g3519
   :shield: waveshare_3_5_rpi_lcd_f
   :goals: build flash
   :compact:

After flashing, the ``home`` screen appears in portrait. Drag vertically on the
temperature field to change the setpoint; the nav buttons switch to the
``climate`` and ``insights`` screens.
