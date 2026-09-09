.. _waveshare_3_5_rpi_lcd_f:

Waveshare 3.5" RPi LCD (F)
##########################

Overview
********

The Waveshare 3.5" RPi LCD (F) is a 320x480 capacitive touch display based on
the Sitronix ST7796S display controller (SPI) and a Goodix GT911 capacitive
touch controller (I2C).

Unlike the :ref:`waveshare_3_5_tft_touch` shield (which uses an FT6336U touch
controller on a software bit-bang I2C bus), this shield drives the GT911 over
the native MSPM0 hardware I2C controller.

More information about the panel can be found at the
`Waveshare 3.5inch RPi LCD (F) website`_.

Pin Assignments
===============

LP-MSPM0G3519
-------------

The panel is powered from **5V** (RPi header pin 4); logic is 3.3V. Panel
header pin numbers are the Raspberry Pi 40-pin positions on the panel.

+--------------------+----------------+--------------------------------+
| Signal             | Panel hdr pin  | Pin                            |
+====================+================+================================+
| VCC (5V)           | 4              | 5V                             |
+--------------------+----------------+--------------------------------+
| GND                | 6              | GND                            |
+--------------------+----------------+--------------------------------+
| SPI MOSI           | 19             | PB17                           |
+--------------------+----------------+--------------------------------+
| SPI CLK            | 23             | PB18                           |
+--------------------+----------------+--------------------------------+
| SPI CS             | 24             | PA8                            |
+--------------------+----------------+--------------------------------+
| ST7796S D/C        | 15             | PB7                            |
+--------------------+----------------+--------------------------------+
| ST7796S RST        | 13             | PA9                            |
+--------------------+----------------+--------------------------------+
| Backlight          | 12             | PA16                           |
+--------------------+----------------+--------------------------------+
| GT911 SDA (I2C0)   | 3              | PA0 (board pull-up jumper J16) |
+--------------------+----------------+--------------------------------+
| GT911 SCL (I2C0)   | 5              | PA1 (board pull-up jumper J17) |
+--------------------+----------------+--------------------------------+
| GT911 INT          | 7              | PA13                           |
+--------------------+----------------+--------------------------------+
| GT911 RST          | 11             | PB14                           |
+--------------------+----------------+--------------------------------+

Requirements
************

This shield can only be used with a board that provides a configuration for SPI
and native I2C interfaces, and that has the MSPM0 hardware I2C driver enabled
(see :ref:`shields` for more details). The GT911 touch driver
(``CONFIG_INPUT_GT911``) is enabled by the shield.

Programming
***********

Set ``--shield waveshare_3_5_rpi_lcd_f`` when you invoke ``west build``. For
example:

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/display/lvgl
   :board: lp_mspm0g3519
   :shield: waveshare_3_5_rpi_lcd_f
   :goals: build

References
**********

.. target-notes::

.. _Waveshare 3.5inch RPi LCD (F) website:
   https://www.waveshare.com/3.5inch-rpi-lcd-f.htm
