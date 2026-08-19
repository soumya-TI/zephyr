#!/usr/bin/env python3
"""
bridge_test.py — SHared_i2c protocol validator for max31790_fan driver

Connects to the board's console UART (XDS110 virtual COM port) while the
uart_i2c_bridge firmware is running.  For each packet it:
  - Validates the SHared_i2c byte encoding
  - For READ packets: sends back fake MAX31790 register data so the driver
    does not time out
  - Prints PASS / FAIL per packet and a final summary

Hardware setup:
  lp_mspm0g3519 → USB (XDS110) → PC

Prerequisites:
  pip install pyserial

Usage:
  python bridge_test.py --port /dev/ttyACM0      # Linux / macOS
  python bridge_test.py --port COM3               # Windows
  python bridge_test.py --port /dev/ttyACM0 --packets 5   # stop after 5

Expected test sequence from max31790_fan sample main.c:
  Pkt 1  READ  reg=GLOBAL_CONFIG    (1 byte)
  Pkt 2  WRITE reg=GLOBAL_CONFIG    val=0x20
  Pkt 3  WRITE reg=PWMOUT_TARGET    val=[0x80, 0x00]
  Pkt 4  READ  reg=TACH_COUNT_CH1  (2 bytes)
  Pkt 5  READ  reg=FAN_DYNAMICS     (1 byte)
  + any MFD/PWM/sensor init packets that arrive before these
"""

import argparse
import sys
import time
import serial

# ── Protocol constants (must match uart_to_i2c_bridge.c SHared_i2c branch) ──
BRIDGE_START  = 0xF8
BRIDGE_WRITE  = 0xFB
BRIDGE_READ   = 0xFA
MAX_PAYLOAD   = 16
MAX31790_ADDR = 0x50

# ── MAX31790 register names (for human-readable output) ──
REG_NAMES = {
    0x00: "GLOBAL_CONFIG",
    0x02: "PWM_FREQ",
    0x08: "FAN_CONFIG1_CH1",
    0x09: "FAN_CONFIG1_CH2",
    0x0A: "FAN_DYNAMICS_CH1",
    0x0C: "FAN_DYNAMICS_CH3",
    0x20: "TACH_COUNT_MSB_CH1",
    0x22: "TACH_COUNT_MSB_CH2",
    0x3F: "FANFAULTSTATUS1",
    0x40: "PWMOUT_TARGET_MSB_CH1",
    0x42: "PWMOUT_TARGET_MSB_CH2",
    0x48: "TACH_TARGET_MSB_CH1",
}

# ── Fake MAX31790 register map (plausible reset / operating values) ──
FAKE_REGS = {
    0x00: bytes([0x00]),        # GLOBAL_CONFIG — run, oscillator ready
    0x02: bytes([0x44]),        # PWM_FREQ      — 25 kHz
    0x08: bytes([0x04]),        # FAN_CONFIG1   — RPM mode
    0x0A: bytes([0xA0]),        # FAN_DYNAMICS  — speed range 2
    0x20: bytes([0x18, 0x00]),  # TACH_COUNT    — ~2000 RPM
    0x22: bytes([0x18, 0x00]),
    0x3F: bytes([0x00]),        # FANFAULTSTATUS — no faults
    0x40: bytes([0x80, 0x00]),  # PWMOUT_TARGET  — 50%
}
FAKE_DEFAULT = bytes([0x00])


def fake_response(reg_addr: int, length: int) -> bytes:
    """Return `length` bytes of fake register data."""
    data = FAKE_REGS.get(reg_addr, FAKE_DEFAULT)
    return (data * (length // len(data) + 1))[:length]


def reg_name(reg: int) -> str:
    return REG_NAMES.get(reg, f"0x{reg:02X}")


# ── Packet reader ──────────────────────────────────────────────────────────

def read_exact(ser: serial.Serial, n: int, timeout: float = 1.0) -> bytes | None:
    buf = b""
    deadline = time.monotonic() + timeout
    while len(buf) < n and time.monotonic() < deadline:
        chunk = ser.read(n - len(buf))
        if chunk:
            buf += chunk
    return buf if len(buf) == n else None


def wait_for_start(ser: serial.Serial, timeout: float) -> bool:
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        b = ser.read(1)
        if b and b[0] == BRIDGE_START:
            return True
    return False


# ── Packet handler ─────────────────────────────────────────────────────────

def handle_packet(ser: serial.Serial, pkt_num: int) -> tuple[bool, str]:
    """
    Read one complete packet (header already consumed up to start byte).
    Returns (pass, description).
    """
    header = read_exact(ser, 3, timeout=0.5)
    if header is None:
        return False, "timed out reading header"

    addr, cmd, length = header[0], header[1], header[2]
    prefix = f"addr=0x{addr:02X}"

    if addr != MAX31790_ADDR:
        return False, f"{prefix}  unexpected address (expected 0x{MAX31790_ADDR:02X})"

    if cmd == BRIDGE_WRITE:
        if length == 0 or length > MAX_PAYLOAD:
            return False, f"{prefix}  WRITE  invalid length={length}"
        data = read_exact(ser, length, timeout=0.5)
        if data is None:
            return False, f"{prefix}  WRITE  timed out reading {length} data bytes"
        reg     = data[0]
        payload = data[1:]
        desc = (f"{prefix}  WRITE  reg={reg_name(reg):<28}"
                f"data=[{payload.hex(' ') if payload else '—'}]")
        return True, desc

    elif cmd == BRIDGE_READ:
        if length == 0 or length > MAX_PAYLOAD:
            return False, f"{prefix}  READ   invalid length={length}"
        reg_byte = read_exact(ser, 1, timeout=0.5)
        if reg_byte is None:
            return False, f"{prefix}  READ   timed out reading register address"
        reg      = reg_byte[0]
        response = fake_response(reg, length)
        ser.write(response)
        desc = (f"{prefix}  READ   reg={reg_name(reg):<28}"
                f"len={length}  replied=[{response.hex(' ')}]")
        return True, desc

    else:
        return False, f"{prefix}  unknown cmd=0x{cmd:02X}"


# ── Main ───────────────────────────────────────────────────────────────────

def main() -> None:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("--port",    required=True,
                        help="Serial port (e.g. /dev/ttyACM0, COM3)")
    parser.add_argument("--baud",    type=int,   default=9600,
                        help="Baud rate (default: 9600)")
    parser.add_argument("--timeout", type=float, default=15.0,
                        help="Seconds to wait for first packet (default: 15)")
    parser.add_argument("--packets", type=int,   default=0,
                        help="Stop after N packets; 0 = run until Ctrl-C (default: 0)")
    args = parser.parse_args()

    print(f"Opening {args.port} at {args.baud} baud …")
    try:
        ser = serial.Serial(args.port, baudrate=args.baud, timeout=0.1)
    except serial.SerialException as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)

    print(f"Waiting up to {args.timeout:.0f}s for first packet "
          f"(reset the board now) …\n")

    if not wait_for_start(ser, args.timeout):
        print("ERROR: no packet received within timeout — "
              "check port, baud rate, and that firmware is running.",
              file=sys.stderr)
        ser.close()
        sys.exit(1)

    pass_count = 0
    fail_count = 0
    pkt_num    = 1

    def process(ok: bool, desc: str) -> None:
        nonlocal pass_count, fail_count
        tag = "\033[32mPASS\033[0m" if ok else "\033[31mFAIL\033[0m"
        print(f"  [{tag}]  #{pkt_num:>3}  {desc}")
        if ok:
            pass_count += 1
        else:
            fail_count += 1

    # First start byte already consumed by wait_for_start
    ok, desc = handle_packet(ser, pkt_num)
    process(ok, desc)

    try:
        while args.packets == 0 or pkt_num < args.packets:
            if not wait_for_start(ser, timeout=5.0):
                print("\nNo packet for 5 s — stopping.")
                break
            pkt_num += 1
            ok, desc = handle_packet(ser, pkt_num)
            process(ok, desc)
    except KeyboardInterrupt:
        print("\nInterrupted.")
    finally:
        ser.close()

    total = pass_count + fail_count
    print(f"\n{'─' * 56}")
    print(f"  Packets received : {total}")
    print(f"  PASS             : {pass_count}")
    print(f"  FAIL             : {fail_count}")
    result = "PASS" if fail_count == 0 and total > 0 else "FAIL"
    colour = "\033[32m" if result == "PASS" else "\033[31m"
    print(f"  Result           : {colour}{result}\033[0m")
    print(f"{'─' * 56}")
    sys.exit(0 if result == "PASS" else 1)


if __name__ == "__main__":
    main()
