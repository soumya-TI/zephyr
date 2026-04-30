.. zephyr:code-sample:: blinky_idle_pm
   :name: Blinky with System-Level Idle Power Management
   :relevant-api: gpio_interface pm_policy_api

   Simple blinky example demonstrating low-power mode with clock output monitoring.

Overview
********

A simple LED blinker that prints status to UART0 and outputs ULPCLK on PA22.
Observe the clock frequency drop on PA22 during the 1000ms sleep to verify low-power mode operation.

PA22 Clock Output
*****************

- ULPCLK is output on PA22 with a 16x divider
- For example, 32MHz system clock ÷ 16 = 2MHz
- **Active mode**: ~2MHz (32MHz ÷ 16)
- **Low-power mode**: ~2kHz (32kHz ÷ 16)

Use a logic analyzer to observe the frequency change during the 1000ms sleep period.

Building and Running
********************

.. zephyr-app-commands::
   :zephyr-app: ti_samples/blinky_idle_pm
   :board: lp_mspm0g3507
   :goals: build flash
   :compact:

After flashing:

1. Open a serial terminal to view UART0 output (115200 baud)
2. Observe the LED blinking and console messages
3. Connect a logic analyzer to PA22 to see the clock frequency drop during sleep

Expected Output
***************

- **LED**: Blinks on/off
- **Console**: Prints LED state messages
- **PA22**: Frequency drops from ~2MHz to ~2kHz during 1000ms sleep
