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

LP-AM13E230 (Rev E2)
--------------------

SPI uses UNICOMM3 (BoosterPack standard SPI position). I2C uses UNICOMM2
whose PA22/PA23 carry the board's on-board 2.2 kΩ pull-up resistors
(jumpers J17/J18, installed by default). PA6, PA8, and PA13 are not
available on BoosterPack headers and are not used.

+--------------------+----------------+------------------------------------------+
| Signal             | Panel hdr pin  | Pin                                      |
+====================+================+==========================================+
| VCC (5V)           | 4              | 5V (J3-21)                               |
+--------------------+----------------+------------------------------------------+
| GND                | 6              | GND (J3-22)                              |
+--------------------+----------------+------------------------------------------+
| SPI MOSI           | 19             | PB10 (J2-15, UC3)                        |
+--------------------+----------------+------------------------------------------+
| SPI CLK            | 23             | PB5  (J1-7,  UC3)                        |
+--------------------+----------------+------------------------------------------+
| SPI CS             | 24             | PA21 (J2-19, UC3)                        |
+--------------------+----------------+------------------------------------------+
| SPI MISO (opt.)    | 21             | PA20 (J2-14, UC3)                        |
+--------------------+----------------+------------------------------------------+
| ST7796S D/C        | 15             | PA25 (J4-35)                             |
+--------------------+----------------+------------------------------------------+
| ST7796S RST        | 13             | PA17 (J3-26)                             |
+--------------------+----------------+------------------------------------------+
| Backlight          | 12             | PA18 (J3-24)                             |
+--------------------+----------------+------------------------------------------+
| GT911 SDA (UC2)    | 3              | PA22 (J1-10, 2.2 kΩ pull-up via J18)    |
+--------------------+----------------+------------------------------------------+
| GT911 SCL (UC2)    | 5              | PA23 (J1-9,  2.2 kΩ pull-up via J17)    |
+--------------------+----------------+------------------------------------------+
| GT911 INT          | 7              | PB11 (J2-11)                             |
+--------------------+----------------+------------------------------------------+
| GT911 RST          | 11             | PA16 (J3-23)                             |
+--------------------+----------------+------------------------------------------+

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
