.. _waveshare_3_5_tft_touch:

Waveshare 3.5" Capacitive Touch LCD
####################################

Overview
********

The Waveshare 3.5" Capacitive Touch LCD has a resolution of 480x320
pixels and is based on the ST7796S controller. The capacitive touch
panel uses a FT6336U touch controller.

More information about the shield can be found at the
`Waveshare 3.5inch Capacitive Touch LCD website`_.

Pin Assignments
===============

LP-MSPM0G3519
-------------

+--------------------+------------------------------------------+
| Signal             | Pin                                      |
+====================+==========================================+
| SPI MOSI           | PB17                                     |
+--------------------+------------------------------------------+
| SPI CLK            | PB18                                     |
+--------------------+------------------------------------------+
| SPI CS             | PA8                                      |
+--------------------+------------------------------------------+
| ST7796S D/C        | PB7                                      |
+--------------------+------------------------------------------+
| ST7796S RST        | PA9                                      |
+--------------------+------------------------------------------+
| Backlight          | PA16                                     |
+--------------------+------------------------------------------+
| FT6336U INT        | PA13                                     |
+--------------------+------------------------------------------+
| FT6336U RST        | PB14                                     |
+--------------------+------------------------------------------+
| FT6336U SDA        | PA0                                      |
+--------------------+------------------------------------------+
| FT6336U SCL        | PA1                                      |
+--------------------+------------------------------------------+

Requirements
************

This shield can only be used with a board that provides a configuration
for SPI and GPIO interfaces (see :ref:`shields` for more details).

Programming
***********

Set ``--shield waveshare_3_5_tft_touch`` when you invoke ``west build``.
For example:

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/display/lvgl
   :board: lp_mspm0g3519
   :shield: waveshare_3_5_tft_touch
   :goals: build

References
**********

.. target-notes::

.. _Waveshare 3.5inch Capacitive Touch LCD website:
   https://www.waveshare.com/3.5inch-capacitive-touch-lcd.htm
