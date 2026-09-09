.. zephyr:code-sample:: ti_thermostat_full
   :name: TI MSPM0 LVGL Thermostat Full
   :relevant-api: display_interface input_interface

   Run a four-screen LVGL 9.5 smart-thermostat UI on a 320x480 ST7796S + GT911
   panel. Vendored from lvgl-services/ti-thermostat, trimmed for 512 KB flash.

Overview
********

This sample runs the smart-thermostat user interface from the
``lvgl-services/ti-thermostat`` LVGL Editor project on the Waveshare 3.5" RPi
LCD (F) panel (ST7796S display + GT911 touch).  The UI has four screens:

* **Home** — large dial arc for temperature, heating/cooling mode pills, HVAC
  status chip.
* **Schedule** — weekly setpoint curve chart (custom ``wd_schedule_chart``
  widget) with day-selector chips.
* **Rooms** — per-zone temperature and demand bars for four rooms.
* **Insights** — weekly energy chart and runtime/eco stats.

The bottom tab bar switches between screens.  The UI drives a built-in
simulator that animates all subjects so the panel shows live motion without
external sensors.

``src/main.c`` only waits for the display, calls ``ti_thermostat_init()`` and
pumps ``lv_timer_handler()``.  Zephyr auto-initialises LVGL and binds the
display / GT911 pointer device through the shield's ``chosen`` and
``zephyr,lvgl-pointer-input`` nodes.

Vendored UI provenance
======================

``ui/`` is a snapshot of the compilable subset of ``lvgl-services/ti-thermostat``
at commit ``2ff753931ea0cc256bfea3513ff4d27892120fb9`` (shallow clone, HEAD of
``main`` at time of vendoring, 2026-09-08).

Files omitted from the upstream repo and why:

* **Host simulator** (``sim/``): Linux/Windows SDL build; not needed for
  firmware.
* **Editor preview binaries** (``preview-bin/``): Host-only JavaScript/WASM
  runtime.
* **PNG / TTF sources** (``images/*.png``, ``fonts/*.ttf``): Binary source
  files not compiled into firmware; the generated ``*_data.c`` files are kept.
* **XML authoring files** (``*.xml``): LVGL Editor source; regenerate
  ``*_gen.[ch]`` from these upstream.  The firmware build uses only the
  generated C.
* **Tools** (``tools/``): Node.js icon-generation helper, not a build input.

Files removed or stubbed for the LP-MSPM0G3519 flash budget:

* ``images/image_bg_normal_data.c``, ``image_bg_warm_data.c``,
  ``image_bg_cool_data.c`` — Three full-screen 320×480 RGB565 backgrounds,
  ~300 KB each (~900 KB total), which overflow the 512 KB flash by nearly 2×.
  Replaced by ``images/image_bg_stub_data.c``, which defines each as a 1×1
  single-pixel descriptor carrying the dominant tint of the original.  The
  screen's own gradient background (``lv_style_bg_grad``) fills the visible
  area.  ``THERMOSTAT_USE_TRANSITIONS=0`` in ``custom/thermostat_config.h``
  disables the per-frame cross-dissolve animation so the 1×1 stubs are
  invisible.

* ``widgets/wd_schedule_chart/wd_schedule_chart_xml_parser.c`` — Includes
  ``lv_xml_private/lv_xml_private.h``, which is only present when
  ``CONFIG_LV_USE_XML=y``.  Zephyr's LVGL 9.5 module does not include the XML
  module.  The parser file's entire implementation is already guarded by
  ``#if defined(LV_USE_XML) && LV_USE_XML``, and the call to
  ``wd_schedule_chart_register()`` in ``ti_thermostat_gen.c`` is similarly
  guarded.  The widget's runtime behaviour (``wd_schedule_chart.c`` and
  ``wd_schedule_chart_gen.c``) compiles and works without the XML parser.

Requirements
************

* An :ref:`lp_mspm0g3519` LaunchPad.
* The :ref:`waveshare_3_5_rpi_lcd_f` shield (ST7796S display + GT911 touch on
  the native MSPM0 I2C controller).

Building and Running
********************

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/display/ti_thermostat_full
   :board: lp_mspm0g3519
   :shield: waveshare_3_5_rpi_lcd_f
   :goals: build flash
   :compact:

After flashing, the ``home`` screen appears in portrait.  The tab bar at the
bottom navigates to the schedule, rooms, and insights screens.

Build size (LP-MSPM0G3519, optimised)
======================================

Measured at the vendored commit listed above:

.. list-table::
   :header-rows: 1
   :widths: 25 30 15

   * - Region
     - Used
     - % of budget
   * - FLASH (512 KB)
     - ~492 KB
     - ~94 %
   * - RAM / sram0 (64 KB)
     - ~62 KB
     - ~95 %
   * - LVGL_BUF / sram1 (64 KB)
     - ~60 KB
     - ~94 %
