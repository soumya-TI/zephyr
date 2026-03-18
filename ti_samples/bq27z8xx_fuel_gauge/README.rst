.. zephyr:code-sample:: ti_bq27z8xx
   :name: TI BQ27Z8XX Fuel Gauge

   Continuously read current, voltage, and status from a BQ27Z855 or
   BQ27Z758 fuel gauge using three concurrent threads.

Overview
********

This sample reads ``FUEL_GAUGE_AVG_CURRENT``, ``FUEL_GAUGE_VOLTAGE``, and
``FUEL_GAUGE_STATUS`` from a TI BQ27Z855 or BQ27Z758 fuel gauge.

Three threads poll their respective properties every 1 second, serialised by
a shared I²C mutex and a serial-output mutex.  Before starting the threads,
``main()`` performs an I²C bus scan (0x08–0x77), prints each detected address,
and — if the gauge is found at an address other than 0x55 (ROM mode) — sends
SMBus command 0x08 to exit ROM mode.  Device identity (DeviceType,
FirmwareVersion, HardwareVersion, ChemID) is then read via the
``BQ27Z8XX_BUFFER_PROP_*`` fuel-gauge buffer properties.

The driver variant (BQ27Z758 or BQ27Z855) is selected at compile time by the
DT compatible string.  The ``bq27z8xx_init()`` function only verifies that the
I²C bus is ready; no runtime device-type detection is performed.

Building and Running
********************

On ``lp_mspm0g3507`` (BQ27Z855 on I²C0 at address 0x55):

.. code-block:: console

   west build -b lp_mspm0g3507 ti_samples/bq27z8xx_fuel_gauge
   west flash

Build check on ``native_sim`` (uses the built-in bq27z8xx emulator):

.. code-block:: console

   west build -b native_sim ti_samples/bq27z8xx_fuel_gauge

For other boards, provide a DeviceTree overlay placing the gauge on an
available I²C bus with the ``fuel-gauge0`` alias.  Use the variant-specific
compatible for hardware (``ti,bq27z855`` or ``ti,bq27z758``):

.. code-block:: devicetree

   / {
       aliases { fuel-gauge0 = &bq27z855; };
   };

   &i2c0 {
       bq27z855: bq27z855@55 {
           compatible = "ti,bq27z855";
           reg = <0x55>;
       };
   };

For the ``native_sim`` emulator, the ``ti,bq27z8xx`` compatible is used with a
``device-type`` property to select the emulated variant:

.. code-block:: devicetree

   bq27z8xx: bq27z8xx@55 {
       compatible = "ti,bq27z8xx";
       reg = <0x55>;
       device-type = "bq27z758";  /* or "bq27z855" */
   };

Sample Output
*************

.. code-block:: console

   *** Booting Zephyr OS build ... ***
   Scanning I2C bus...
     Device found at address 0x55
   Scan complete. Starting fuel gauge reads...
   Device type: BQ27Z855 (0x1855)
   Firmware: device=0 version=0.0 build=0 type=0 dzt=0
   Hardware version: 0x0000
   Chem ID: 0x0000
   Current reading: -2000 uA
   Voltage reading: 1000 uV
   Status: 0x0000
