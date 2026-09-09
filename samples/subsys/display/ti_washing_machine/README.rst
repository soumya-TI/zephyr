.. _ti_washing_machine:

TI MSPM0 LVGL Washing Machine
##############################

Overview
********

A four-screen LVGL 9 washing-machine UI running on the
`Waveshare 3.5" RPi LCD (F) <https://www.waveshare.com/3.5inch-rpi-lcd-f.htm>`_
(320×480 ST7796S display + GT911 capacitive touch) attached to the LP-MSPM0G3519
LaunchPad.

The UI is vendored from the `LVGL Services ti-washing-machine project
<https://github.com/lvgl-services/ti-washing-machine>`_ and consists of four
tab-navigated screens:

* **Home** – program selector (Cotton, Synthetic, Quick, Eco), wash progress
  ring with animated drum, and start/pause control.
* **Cycle** – phase timeline (Pre-wash / Wash / Rinse / Spin) with live
  step indicators, temperature and RPM metrics.
* **Setup** – wash options (Prewash, Rinse+, Delay), temperature and spin-speed
  sliders, and water-level selector.
* **Service** – board info, unlock/lock child-safety toggle, and sensor readings.

A built-in simulation model (``ui/custom/wash_sim.c``) drives all subjects
on a periodic timer so the UI animates out of the box without real washer
hardware.  Replace the ``wash_sim_publish()`` call in your control loop to
feed live measurements from your motor and sensor targets.

Requirements
************

* :ref:`lp_mspm0g3519` board
* :ref:`waveshare_3_5_rpi_lcd_f` shield (ST7796S + GT911)

Building and Running
********************

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/display/ti_washing_machine
   :board: lp_mspm0g3519
   :goals: build flash
   :compact:

The shield is selected automatically by ``CMakeLists.txt``; an explicit
``--shield waveshare_3_5_rpi_lcd_f`` still overrides it.

Wiring the hardware seam
************************

Once the UI is running with the simulator, replace the simulation with your
real control loop:

.. code-block:: c

   #include "washing_machine.h"

   /* Called from your wash-controller task */
   wash_sim_publish(phase, temp_c, rpm, remaining_s);
