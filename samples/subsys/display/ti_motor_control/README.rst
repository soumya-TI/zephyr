.. _ti_motor_control:

TI MSPM0 LVGL Motor Control
############################

Overview
********

A four-screen LVGL 9 motor-control dashboard running on the
`Waveshare 3.5" RPi LCD (F) <https://www.waveshare.com/3.5inch-rpi-lcd-f.htm>`_
(320×480 ST7796S display + GT911 capacitive touch) attached to the LP-MSPM0G3519
LaunchPad.

The UI is vendored from the `LVGL Services ti-motor-control project
<https://github.com/lvgl-services/ti-motor-control>`_ and consists of four
tab-navigated screens:

* **Run** – speed reference roller, live odometer (four digit-roller digits),
  direction indicator, and start/stop/e-stop buttons.
* **Trend** – dual-axis line chart showing speed (RPM) and load (%) history
  over the last 30 samples.
* **Faults** – fault log table (code / description / severity) with a
  one-tap clear action.
* **Config** – drive-setup options: direction selector, soft-start, reverse
  and derate toggles, and ramp-rate picker.

A built-in simulation model (``ui/custom/motor_sim.c``) drives all subjects
on a 100 ms timer so the UI animates out of the box without real motor
hardware.  Replace the ``motor_sim_publish()`` call in your control loop to
feed live measurements from an LP-AM13E230 or any other drive target.

Requirements
************

* :ref:`lp_mspm0g3519` board
* :ref:`waveshare_3_5_rpi_lcd_f` shield (ST7796S + GT911)

Building and Running
********************

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/display/ti_motor_control
   :board: lp_mspm0g3519
   :goals: build flash
   :compact:

The shield is selected automatically by ``CMakeLists.txt``; an explicit
``--shield waveshare_3_5_rpi_lcd_f`` still overrides it.

Wiring the hardware seam
************************

Once the UI is running with the simulator, wire your real control loop:

.. code-block:: c

   #include "motor_control.h"

   /* Called from your 100 Hz ISR / task */
   motor_sim_publish(rpm_measured, phase_ma, temp_c, load_pct);

``motor_sim_publish()`` atomically updates all LVGL subjects; the UI
re-renders on the next ``lv_timer_handler()`` tick.  No widget handles
needed, no direct LVGL calls from the control context.
