.. _max31790_fan:

MAX31790 Fan Controller via UART-to-I2C Bridge
###############################################

Overview
********

This sample demonstrates full fan control — PWM duty-cycle set, RPM read,
and fault detection — using the MAX31790 6-channel PWM fan controller from
a TI MSPM0G host MCU.

Because the MSPM0G does not have native I2C connectivity to the MAX31790 in
this topology, the host communicates over UART to a second **fixed-function
MSPM0G bridge MCU** that translates UART frames to I2C and drives the
MAX31790 directly.  The Zephyr ``uart-i2c-bridge`` virtual I2C controller
driver makes this transparent: the MAX31790 MFD / PWM / sensor drivers run
unchanged on the host, and the bridge UART link is invisible above the I2C
bus layer.

System Topology
***************

.. code-block:: none

   ┌──────────────────────────┐  UART1 9600 8N1  ┌──────────────────────┐  I2C 400 kHz
   │  Host MCU (g3519 / g3507)│ ────────────────► │ Fixed-function MSP   │ ──────────►  MAX31790
   │  Zephyr + this sample    │ ◄──────────────── │ (uart_i2c_bridge FW) │             Fan ctrl
   │                          │  (SHared_i2c prot)│ SHared_i2c branch    │
   └──────────────────────────┘                   └──────────────────────┘
            UART1 PB4 TX ──────────────────────────── UART RX (PA11)
            UART1 PB5 RX ──────────────────────────── UART TX (PA10)
            GND ──────────────────────────────────────── GND


SHared_i2c UART protocol (host → bridge)
=========================================

.. code-block:: none

   Write: [0xF8][i2c_addr][0xFB][len][data...]
   Read:  [0xF8][i2c_addr][0xFA][read_len][reg_addr]  →  bridge replies with read_len bytes


Requirements
************

Host MCU
   - ``lp_mspm0g3519`` (TI LP-MSPM0G3519 LaunchPad) or
   - ``lp_mspm0g3507`` (TI LP-MSPM0G3507 LaunchPad)

Bridge MCU
   - Second MSPM0G LaunchPad (G3507 or G3519) flashed with the
     ``uart_i2c_bridge`` firmware from the ``SHared_i2c`` branch of
     ``msp/uart_i2c_bridge``.

Fan controller
   - Maxim MAX31790 connected to the bridge MCU's I2C bus (I2C0):

     - I2C SDA → PA28
     - I2C SCL → PA31
     - ADDR pin wired for address **0x50** (all address pins to GND)
     - Pull-up resistors on SDA / SCL (4.7 kΩ to 3.3 V)


Hardware Connections
********************

.. list-table::
   :header-rows: 1

   * - Host MCU pin
     - Bridge MCU pin
     - Signal
   * - PB4 (UART1 TX, J2.11)
     - PA11 (UART0 RX, J2.14)
     - UART TX → RX
   * - PB5 (UART1 RX, J2.12)
     - PA10 (UART0 TX, J2.13)
     - UART RX ← TX
   * - GND
     - GND
     - Common ground

.. list-table::
   :header-rows: 1

   * - Bridge MCU pin
     - MAX31790 pin
     - Signal
   * - PA28 (I2C0 SDA)
     - SDA
     - I2C data
   * - PA31 (I2C0 SCL)
     - SCL
     - I2C clock
   * - 3V3
     - VCC
     - Power
   * - GND
     - GND / ADDR pins
     - Ground / address = 0x50


Environment Setup
*****************

1. Install the Zephyr SDK
=========================

.. code-block:: bash

   wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.17.0/zephyr-sdk-0.17.0_linux-x86_64.tar.xz
   tar xf zephyr-sdk-0.17.0_linux-x86_64.tar.xz
   cd zephyr-sdk-0.17.0 && ./setup.sh

2. Create a west workspace
==========================

.. code-block:: bash

   mkdir zephyrproject && cd zephyrproject
   python3 -m venv .venv && source .venv/bin/activate
   pip install west
   west init -m <downstream-zephyr-repo-url>
   west update

3. Install Python dependencies
===============================

.. code-block:: bash

   pip install -r zephyr/scripts/requirements.txt
   pip install pyserial          # only needed for PC bridge simulation


Building
********

lp_mspm0g3519
=============

.. code-block:: bash

   cd zephyrproject
   west build -p -b lp_mspm0g3519 zephyr/samples/drivers/max31790_fan

lp_mspm0g3507
=============

.. code-block:: bash

   west build -p -b lp_mspm0g3507 zephyr/samples/drivers/max31790_fan


Flashing
********

Using the on-board XDS110 debugger with a custom OpenOCD build:

.. code-block:: bash

   west flash \
     --openocd /path/to/openocd/src/openocd \
     --openocd-search /path/to/openocd/tcl

Flash the bridge firmware first (onto the bridge MCU), then flash this
sample onto the host MCU.


Running
*******

After flashing both MCUs and wiring them together:

1. Connect a USB cable to the **host MCU** LaunchPad (XDS110 virtual COM
   port provides the Zephyr console at 115200 baud).

2. Open a serial terminal::

      minicom -D /dev/ttyACM0 -b 115200
      # or: screen /dev/ttyACM0 115200

3. Reset the host MCU.  Expected console output:

   .. code-block:: none

      MAX31790 ready via UART-I2C bridge
      Fan ch1: 50% duty
      Fan 1: 2000 RPM
      No fan faults
      Fan 1: 2010 RPM
      ...

   The blue LED (LED0) blinks at 1 Hz while the sample is running.
   If the MAX31790 or bridge is unreachable, the red LED (LED1) lights solid.


PC Simulation (no hardware bridge required)
*******************************************

The ``uart_i2c_bridge`` sample and ``bridge_test.py`` script let you
validate the protocol driver on a PC without any bridge or fan hardware.
The PC simulates the bridge UART interface.

See ``samples/boards/ti/mspm0/uart_i2c_bridge/README.rst`` for full
instructions.

Quick start:

1. Build and flash the bridge validation firmware::

      west build -p -b lp_mspm0g3519 zephyr/samples/boards/ti/mspm0/uart_i2c_bridge
      west flash ...

2. Run the PC validator::

      python samples/boards/ti/mspm0/uart_i2c_bridge/bridge_test.py \
        --port /dev/ttyACM0


Configuration
*************

.. list-table::
   :header-rows: 1

   * - Kconfig option
     - Default
     - Description
   * - ``CONFIG_MFD_MAX31790``
     - y (auto)
     - MAX31790 MFD parent driver
   * - ``CONFIG_PWM_MAX31790``
     - y (auto)
     - MAX31790 PWM child driver
   * - ``CONFIG_MAX31790_FAN_SPEED``
     - y (auto)
     - MAX31790 tachometer sensor driver
   * - ``CONFIG_MAX31790_FAN_FAULT``
     - y (auto)
     - MAX31790 fan fault sensor driver
   * - ``CONFIG_I2C_UART_BRIDGE``
     - y (auto)
     - UART-to-I2C virtual I2C controller driver


Source files
************

.. list-table::
   :header-rows: 1

   * - File
     - Description
   * - :zephyr_file:`samples/drivers/max31790_fan/src/main.c`
     - Sample application (PWM set, RPM read, fault monitor loop)
   * - :zephyr_file:`samples/drivers/max31790_fan/boards/lp_mspm0g3519.overlay`
     - DTS overlay: uart-i2c-bridge on UART1 for lp_mspm0g3519
   * - :zephyr_file:`samples/drivers/max31790_fan/boards/lp_mspm0g3507.overlay`
     - DTS overlay: uart-i2c-bridge on UART1 for lp_mspm0g3507
   * - :zephyr_file:`drivers/i2c/i2c_uart_bridge.c`
     - UART-to-I2C virtual I2C controller driver
   * - :zephyr_file:`dts/bindings/i2c/uart-i2c-bridge.yaml`
     - DTS binding for ``uart-i2c-bridge`` compatible
