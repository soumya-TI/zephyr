.. _bq27z8xx_shell_sample:

TI BQ27Z8XX Fuel Gauge Shell
#############################

Overview
********

This sample enables the interactive shell interface for the BQ27Z8XX fuel
gauge driver.  After flashing, connect a terminal to the board's UART at
115200 baud and use ``bq27z8xx`` subcommands to inspect and control the device.

Requirements
************

* LP-MSPM0G3507 LaunchPad (or compatible board with I²C and UART)
* BQ27Z855 connected to I²C0 at address ``0x55``
* Terminal program (minicom, screen, PuTTY, etc.)

Building and Flashing
*********************

.. code-block:: console

   west build -b lp_mspm0g3507 ti_samples/bq27z8xx_shell
   west flash

Connecting
**********

Open your terminal on the board's USB-UART port at **115200 baud, 8N1**.
After reset you should see the Zephyr boot banner and shell prompt:

.. code-block:: console

   *** Booting Zephyr OS build v4.x.x ***
   uart:~$

Send ``bq27z8xx help`` to list available subcommands:

.. code-block:: console

   uart:~$ bq27z8xx
     full_access  lifetime  ra_table  read  seal  security  serial  status  unseal

Available Commands
******************

All commands take the device name as the first argument.  With the default
overlay the device name is ``bq27z855@55``.

.. list-table::
   :header-rows: 1
   :widths: 20 35 45

   * - Subcommand
     - Syntax
     - Description
   * - ``status``
     - ``bq27z8xx status <device>``
     - Full status dump: safety alerts, operation status, charging status,
       gauging status, DA status, and gauge/IT status registers.
   * - ``security``
     - ``bq27z8xx security <device>``
     - Report current security mode: Sealed, Unsealed, or Full Access.
   * - ``lifetime``
     - ``bq27z8xx lifetime <device>``
     - Dump all lifetime data blocks (cumulative energy, time, and power
       metrics recorded by the gauge).
   * - ``serial``
     - ``bq27z8xx serial <device>``
     - Read and display the device serial number.
   * - ``ra_table``
     - ``bq27z8xx ra_table <device> [tsv]``
     - Decode and display the Cell0 and xCell0 resistance profile (Ra) tables
       from Data Flash (addresses 0x4100 and 0x4140).  Pass ``tsv`` for
       tab-separated output suitable for import into a spreadsheet.
   * - ``read``
     - ``bq27z8xx read <device> <addr_hex> [blocks]``
     - Hex dump of one or more 32-byte blocks starting at ``addr_hex``.
       Addresses below ``0x4000`` are treated as MAC commands; ``0x4000``
       and above are Data Flash addresses.
   * - ``seal``
     - ``bq27z8xx seal <device>``
     - Transition the device from Unsealed to Sealed security mode.
   * - ``unseal``
     - ``bq27z8xx unseal <device> [key1_hex key2_hex]``
     - Unseal the device using the provided key pair (defaults:
       ``0x0414 0x3672``).
   * - ``full_access``
     - ``bq27z8xx full_access <device> [key1_hex key2_hex]``
     - Enter Full Access security mode using the provided key pair (defaults:
       ``0xFFFF 0xFFFF``).

Example Session
***************

.. code-block:: console

   uart:~$ bq27z8xx status bq27z855@55
   BQ27Z8XX status for bq27z855@55 (BQ27Z855)
   BatteryStatus: 0x0001
   ...
   GaugingStatus: 0x000000
   ...

   uart:~$ bq27z8xx security bq27z855@55
   Security: Sealed (SEC1,SEC0=0x3)

   uart:~$ bq27z8xx unseal bq27z855@55
   Using default unseal keys: 0x0414 0x3672
   Unseal keys sent.

   uart:~$ bq27z8xx security bq27z855@55
   Security: Unsealed (SEC1,SEC0=0x2)

   uart:~$ bq27z8xx lifetime bq27z855@55
     Cell 1 Max Voltage: 4150 mV
     Cell 1 Min Voltage: 2800 mV
     ...

   uart:~$ bq27z8xx ra_table bq27z855@55
   Ra Table (Data Flash)
     Cell0 R_a flag: 0x0001
     Cell0 R_a 0: 512 (500 mOhm)
     ...

   uart:~$ bq27z8xx read bq27z855@55 0001
   MAC 0x0001 (len=32):
     0001: 55 18 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |U...............|
     0011: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................|

Notes
*****

* ``seal`` requires the device to be in Unsealed mode.
* ``unseal`` requires the device to be in Sealed mode.
* ``full_access`` requires the device to be in Unsealed mode.
* Default security keys match the TI factory defaults.  Supply them explicitly
  if they have been changed.
* The ``read`` command is read-only.
