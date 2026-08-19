#!/usr/bin/env python3
"""
bridge_test.py — uart_i2c_bridge Zephyr driver protocol validator

Validates scenarios ported from the SHared_i2c bridge firmware test suite
(TEST_SCENARIOS.md / TEST_RESULTS.md).  Connects to the board's console
UART (XDS110 virtual COM port) while the uart_i2c_bridge sample firmware
runs.

Scenarios covered
─────────────────
 TC1 / TC3  Write  lengths 1, 2, 3, 8 (≤FIFO), 9 (>FIFO), 16 (max)
 TC2        Read   lengths 1, 2, 8 (≤FIFO), 9 (>FIFO), 16 (max)
 TC4        Zero-length write  → -EINVAL firmware-side, NO packet emitted
 TC5        >16-byte write     → -EINVAL firmware-side, NO packet emitted
 (new)      write_len>1 in write-then-read → -ENOTSUP, NO packet emitted
 TC9        Sequential W→R→W, R→R, W→W
 TC11       Read timeout: script delays → firmware -EIO → driver recovers
 TC12       Rapid back-to-back writes (mutex serialization)
 TC13       Boundary I2C addresses 0x08 (min) and 0x7F (max)

Hardware setup
──────────────
 lp_mspm0g3519 → USB (XDS110) → PC  (console UART repurposed as bridge)
 Flash the uart_i2c_bridge sample, then run this script, then reset board.

Usage
─────
 pip install pyserial
 python bridge_test.py --port /dev/ttyACM0       # Linux / macOS
 python bridge_test.py --port COM3               # Windows
"""

import argparse
import sys
import time
import serial

# ── Protocol constants ────────────────────────────────────────────────────
BRIDGE_START  = 0xF8
BRIDGE_WRITE  = 0xFB
BRIDGE_READ   = 0xFA
MAX_PAYLOAD   = 16

# Register 0xFE is the agreed timeout-test marker
TIMEOUT_MARKER_REG = 0xFE
# How long to delay before responding to the timeout packet (ms)
TIMEOUT_DELAY_MS   = 350   # > driver's 200 ms timeout

# ── Register names (display only) ────────────────────────────────────────
REG_NAMES = {
    0x00: "GLOBAL_CONFIG",  0x02: "PWM_FREQ",
    0x08: "FAN_CONFIG_CH1", 0x0A: "FAN_DYNAMICS_CH1",
    0x10: "TACH_HIGH_CH1",  0x20: "TACH_COUNT_CH1",
    0x3F: "FANFAULTSTATUS", 0x40: "PWMOUT_TARGET_CH1",
    0xFE: "TIMEOUT_MARKER",
}

# Fake register responses — plausible MAX31790 reset values
FAKE_REGS: dict[int, bytes] = {
    0x00: bytes([0x00]), 0x02: bytes([0x44]),
    0x08: bytes([0x04]), 0x0A: bytes([0xA0]),
    0x10: bytes([0x00] * 16),
    0x20: bytes([0x18, 0x00]),
    0x3F: bytes([0x00] * 16),
    0x40: bytes([0x80, 0x00]),
}

def fake_response(reg: int, n: int) -> bytes:
    data = FAKE_REGS.get(reg, bytes([0x00]))
    return (data * (n // len(data) + 1))[:n]

def rname(reg: int) -> str:
    return REG_NAMES.get(reg, f"0x{reg:02X}")


# ── Expected packet sequence ──────────────────────────────────────────────
# Each entry describes one UART packet the firmware MUST emit.
# 'timeout' entries deliberately delay the response to trigger -EIO.
#
# 'gap_before' entries represent scenarios where the firmware performs
# an operation that must NOT produce a packet (error cases).  The validator
# checks that the NEXT packet is the one immediately following the gap.

EXPECTED = [
    # ── Write length variants ──────────────────────────────────────────
    # S1  Basic write len=1
    {"id": "S01", "tc": "TC1", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Basic write len=1 (reg only)",
     "check": lambda pkt: pkt["len"] == 1 and pkt["data"][0] == 0x00},

    # S2  Basic write len=2
    {"id": "S02", "tc": "TC1", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Basic write len=2 (reg + 1 byte)",
     "check": lambda pkt: pkt["len"] == 2 and pkt["data"][0] == 0x00
                          and pkt["data"][1] == 0x20},

    # S3  Write len=3
    {"id": "S03", "tc": "TC1", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Write len=3 (reg + MSB + LSB)",
     "check": lambda pkt: pkt["len"] == 3 and pkt["data"][0] == 0x40
                          and pkt["data"][1] == 0x80 and pkt["data"][2] == 0x00},

    # S4  Write len=8 (≤FIFO)
    {"id": "S04", "tc": "TC3-FIFO≤8", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Multi-byte write len=8 (≤ FIFO single load)",
     "check": lambda pkt: pkt["len"] == 8
                          and list(pkt["data"][1:]) == list(range(1, 8))},

    # S5  Write len=9 (>FIFO)
    {"id": "S05", "tc": "TC3-FIFO>8", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Multi-byte write len=9 (> FIFO, crosses boundary)",
     "check": lambda pkt: pkt["len"] == 9
                          and list(pkt["data"][1:]) == list(range(1, 9))},

    # S6  Write len=16 (max)
    {"id": "S06", "tc": "TC3-MAX", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Multi-byte write len=16 (maximum payload)",
     "check": lambda pkt: pkt["len"] == 16
                          and list(pkt["data"][1:]) == list(range(1, 16))},

    # S7-S9 are error cases (NO packet expected).  If the driver sends a
    # packet here bridge_test will see an unexpected packet and fail.
    # Documented as a comment so the sequence numbering is clear.
    #   S07: len=0 write    → firmware -EINVAL, no packet
    #   S08: len=17 write   → firmware -EINVAL, no packet
    #   S09: write_len>1 in write-then-read → firmware -ENOTSUP, no packet

    # ── Read length variants ───────────────────────────────────────────
    # S10 Read len=1
    {"id": "S10", "tc": "TC2", "cmd": BRIDGE_READ, "addr": 0x50,
     "desc": "Basic read len=1",
     "check": lambda pkt: pkt["read_len"] == 1 and pkt["reg"] == 0x00},

    # S11 Read len=2
    {"id": "S11", "tc": "TC2", "cmd": BRIDGE_READ, "addr": 0x50,
     "desc": "Basic read len=2",
     "check": lambda pkt: pkt["read_len"] == 2 and pkt["reg"] == 0x20},

    # S12 Read len=8 (≤FIFO)
    {"id": "S12", "tc": "TC2-FIFO≤8", "cmd": BRIDGE_READ, "addr": 0x50,
     "desc": "Multi-byte read len=8 (≤ FIFO single drain)",
     "check": lambda pkt: pkt["read_len"] == 8 and pkt["reg"] == 0x10},

    # S13 Read len=9 (>FIFO)
    {"id": "S13", "tc": "TC2-FIFO>8", "cmd": BRIDGE_READ, "addr": 0x50,
     "desc": "Multi-byte read len=9 (> FIFO, crosses boundary)",
     "check": lambda pkt: pkt["read_len"] == 9 and pkt["reg"] == 0x10},

    # S14 Read len=16 (max)
    {"id": "S14", "tc": "TC2-MAX", "cmd": BRIDGE_READ, "addr": 0x50,
     "desc": "Multi-byte read len=16 (maximum)",
     "check": lambda pkt: pkt["read_len"] == 16 and pkt["reg"] == 0x3F},

    # ── Sequential patterns (TC9) ──────────────────────────────────────
    # S15-S17  W→R→W
    {"id": "S15", "tc": "TC9-W→R→W", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Sequential W→R→W: first Write",
     "check": lambda pkt: pkt["len"] == 2 and pkt["data"][1] == 0x42},

    {"id": "S16", "tc": "TC9-W→R→W", "cmd": BRIDGE_READ, "addr": 0x50,
     "desc": "Sequential W→R→W: Read",
     "check": lambda pkt: pkt["read_len"] == 1},

    {"id": "S17", "tc": "TC9-W→R→W", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Sequential W→R→W: second Write",
     "check": lambda pkt: pkt["len"] == 2 and pkt["data"][1] == 0x43},

    # S18-S19  R→R
    {"id": "S18", "tc": "TC9-R→R", "cmd": BRIDGE_READ, "addr": 0x50,
     "desc": "Sequential R→R: first Read",
     "check": lambda pkt: pkt["reg"] == 0x02},

    {"id": "S19", "tc": "TC9-R→R", "cmd": BRIDGE_READ, "addr": 0x50,
     "desc": "Sequential R→R: second Read",
     "check": lambda pkt: pkt["reg"] == 0x08},

    # S20-S21  W→W
    {"id": "S20", "tc": "TC9-W→W", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Sequential W→W: first Write",
     "check": lambda pkt: pkt["data"][1] == 0x01},

    {"id": "S21", "tc": "TC9-W→W", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Sequential W→W: second Write",
     "check": lambda pkt: pkt["data"][1] == 0x02},

    # ── Boundary addresses (TC13) ──────────────────────────────────────
    # S22 addr=0x08 write
    {"id": "S22", "tc": "TC13-0x08", "cmd": BRIDGE_WRITE, "addr": 0x08,
     "desc": "Boundary addr 0x08 (minimum) — write",
     "check": lambda pkt: pkt["addr"] == 0x08 and pkt["data"][-1] == 0xAA},

    # S23 addr=0x08 read
    {"id": "S23", "tc": "TC13-0x08", "cmd": BRIDGE_READ, "addr": 0x08,
     "desc": "Boundary addr 0x08 — read",
     "check": lambda pkt: pkt["addr"] == 0x08},

    # S24 addr=0x7F write
    {"id": "S24", "tc": "TC13-0x7F", "cmd": BRIDGE_WRITE, "addr": 0x7F,
     "desc": "Boundary addr 0x7F (maximum) — write",
     "check": lambda pkt: pkt["addr"] == 0x7F and pkt["data"][-1] == 0xBB},

    # S25 addr=0x7F read
    {"id": "S25", "tc": "TC13-0x7F", "cmd": BRIDGE_READ, "addr": 0x7F,
     "desc": "Boundary addr 0x7F — read",
     "check": lambda pkt: pkt["addr"] == 0x7F},

    # ── Timeout + recovery (TC11) ──────────────────────────────────────
    # S26 timeout: Python delays TIMEOUT_DELAY_MS ms → firmware -EIO
    {"id": "S26", "tc": "TC11-TIMEOUT", "cmd": BRIDGE_READ, "addr": 0x50,
     "desc": f"Read timeout: script delays {TIMEOUT_DELAY_MS}ms → -EIO",
     "check": lambda pkt: pkt["reg"] == TIMEOUT_MARKER_REG,
     "timeout_test": True},

    # S27 recovery: must succeed after timeout
    {"id": "S27", "tc": "TC11-RECOVERY", "cmd": BRIDGE_READ, "addr": 0x50,
     "desc": "Recovery read after timeout — driver must succeed",
     "check": lambda pkt: pkt["read_len"] == 1 and pkt["reg"] == 0x00},

    # ── Rapid back-to-back (TC12) ──────────────────────────────────────
    {"id": "S28", "tc": "TC12", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Rapid write 1 of 3 (mutex serialization)",
     "check": lambda pkt: pkt["data"][1] == 0x11},

    {"id": "S29", "tc": "TC12", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Rapid write 2 of 3",
     "check": lambda pkt: pkt["data"][1] == 0x22},

    {"id": "S30", "tc": "TC12", "cmd": BRIDGE_WRITE, "addr": 0x50,
     "desc": "Rapid write 3 of 3",
     "check": lambda pkt: pkt["data"][1] == 0x33},
]


# ── UART helpers ──────────────────────────────────────────────────────────

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


def recv_packet(ser: serial.Serial) -> dict | None:
    """Read one complete packet after the start byte has been consumed."""
    header = read_exact(ser, 3, timeout=0.5)
    if header is None:
        return None

    addr, cmd, field3 = header[0], header[1], header[2]

    if cmd == BRIDGE_WRITE:
        length = field3
        if length == 0 or length > MAX_PAYLOAD:
            return {"error": f"write: invalid length {length}"}
        data = read_exact(ser, length, timeout=0.5)
        if data is None:
            return {"error": "write: timed out reading data bytes"}
        return {"cmd": BRIDGE_WRITE, "addr": addr, "len": length, "data": data}

    elif cmd == BRIDGE_READ:
        read_len = field3
        if read_len == 0 or read_len > MAX_PAYLOAD:
            return {"error": f"read: invalid length {read_len}"}
        reg_byte = read_exact(ser, 1, timeout=0.5)
        if reg_byte is None:
            return {"error": "read: timed out reading register byte"}
        return {"cmd": BRIDGE_READ, "addr": addr,
                "read_len": read_len, "reg": reg_byte[0]}

    else:
        return {"error": f"unknown cmd byte 0x{cmd:02X}"}


# ── Validator ─────────────────────────────────────────────────────────────

PASS = "\033[32mPASS\033[0m"
FAIL = "\033[31mFAIL\033[0m"
SKIP = "\033[33mSKIP\033[0m"


def fmt_pkt(pkt: dict) -> str:
    if "error" in pkt:
        return f"ERROR({pkt['error']})"
    if pkt["cmd"] == BRIDGE_WRITE:
        data_hex = pkt["data"].hex(" ")
        return (f"WRITE addr=0x{pkt['addr']:02X} len={pkt['len']}"
                f" reg=0x{pkt['data'][0]:02X}"
                f"({rname(pkt['data'][0])})"
                f" data=[{data_hex}]")
    else:
        return (f"READ  addr=0x{pkt['addr']:02X} len={pkt['read_len']}"
                f" reg=0x{pkt['reg']:02X}({rname(pkt['reg'])})")


def run_scenario(ser: serial.Serial, exp: dict) -> bool:
    """
    Receive one packet, validate it against `exp`, send response if READ.
    Returns True on pass.
    """
    sid   = exp["id"]
    tc    = exp["tc"]
    desc  = exp["desc"]
    is_to = exp.get("timeout_test", False)

    pkt = recv_packet(ser)
    if pkt is None:
        print(f"  [{FAIL}]  {sid} ({tc})  timed out waiting for packet\n"
              f"          {desc}")
        return False

    if "error" in pkt:
        print(f"  [{FAIL}]  {sid} ({tc})  protocol error: {pkt['error']}\n"
              f"          {desc}")
        return False

    # Address check
    if pkt["addr"] != exp["addr"]:
        print(f"  [{FAIL}]  {sid} ({tc})  addr mismatch:"
              f" got 0x{pkt['addr']:02X} expected 0x{exp['addr']:02X}\n"
              f"          {desc}\n"
              f"          got: {fmt_pkt(pkt)}")
        return False

    # Command type check
    if pkt["cmd"] != exp["cmd"]:
        got  = "WRITE" if pkt["cmd"] == BRIDGE_WRITE else "READ"
        want = "WRITE" if exp["cmd"]  == BRIDGE_WRITE else "READ"
        print(f"  [{FAIL}]  {sid} ({tc})  cmd mismatch:"
              f" got {got} expected {want}\n"
              f"          {desc}\n"
              f"          got: {fmt_pkt(pkt)}")
        return False

    # Payload check via lambda
    try:
        ok = exp["check"](pkt)
    except Exception as e:
        ok = False
        print(f"  [{FAIL}]  {sid} ({tc})  check raised {e}")

    if not ok:
        print(f"  [{FAIL}]  {sid} ({tc})  payload check failed\n"
              f"          {desc}\n"
              f"          got: {fmt_pkt(pkt)}")
        return False

    # ── Respond if READ ──────────────────────────────────────────────
    if pkt["cmd"] == BRIDGE_READ:
        if is_to:
            # Timeout test: delay past driver's timeout, then don't respond
            print(f"  [    ]  {sid} ({tc})  deliberately delaying"
                  f" {TIMEOUT_DELAY_MS} ms ...")
            time.sleep(TIMEOUT_DELAY_MS / 1000.0)
            # No bytes sent → driver times out
        else:
            response = fake_response(pkt["reg"], pkt["read_len"])
            ser.write(response)

    tag = f"[{PASS}]"
    note = "  (timeout triggered)" if is_to else ""
    print(f"  {tag}  {sid} ({tc})  {desc}{note}\n"
          f"          {fmt_pkt(pkt)}")
    return True


# ── Main ──────────────────────────────────────────────────────────────────

def main() -> None:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("--port",    required=True,
                        help="Serial port (e.g. /dev/ttyACM0, COM3)")
    parser.add_argument("--baud",    type=int,   default=9600)
    parser.add_argument("--timeout", type=float, default=15.0,
                        help="Seconds to wait for first packet (default: 15)")
    args = parser.parse_args()

    print(f"Opening {args.port} at {args.baud} baud …")
    try:
        ser = serial.Serial(args.port, baudrate=args.baud, timeout=0.1)
    except serial.SerialException as exc:
        sys.exit(f"ERROR: {exc}")

    print(f"\nWaiting up to {args.timeout:.0f}s for first packet"
          f" — reset the board now …\n")
    if not wait_for_start(ser, args.timeout):
        ser.close()
        sys.exit("ERROR: no packet received — check port and that firmware is running")

    pass_count = 0
    fail_count = 0
    idx = 0  # index into EXPECTED

    # First start byte already consumed; process its packet
    ok = run_scenario(ser, EXPECTED[idx])
    pass_count += ok; fail_count += not ok
    idx += 1

    # Print the implicit "no packet" scenarios that happen between S06 and S10
    error_cases = [
        ("S07", "TC4",   "Zero-length write → -EINVAL, NO packet emitted"),
        ("S08", "TC5",   ">16-byte write    → -EINVAL, NO packet emitted"),
        ("S09", "(new)", "write_len>1 in write-then-read → -ENOTSUP, NO packet emitted"),
    ]

    try:
        while idx < len(EXPECTED):
            exp = EXPECTED[idx]

            # After S06 (idx=5 → last WRITE scenario), insert no-packet notes
            if idx == 6:
                for sid, tc, desc in error_cases:
                    # Give the board time to finish the error ops
                    time.sleep(0.05)
                print()
                print("  ── Error cases (no packet expected) ──")
                for sid, tc, desc in error_cases:
                    print(f"  [{SKIP}]  {sid} ({tc})  {desc}")
                print()

            if not wait_for_start(ser, timeout=5.0):
                remaining = len(EXPECTED) - idx
                print(f"\n  [{FAIL}]  No packet for 5s —"
                      f" {remaining} scenario(s) not reached.")
                fail_count += remaining
                break

            ok = run_scenario(ser, exp)
            pass_count += ok; fail_count += not ok
            idx += 1

    except KeyboardInterrupt:
        print("\nInterrupted.")
    finally:
        ser.close()

    total = pass_count + fail_count
    note  = (f"  (+ {len(error_cases)} implicit no-packet error cases"
             f" above — validated by absence of unexpected packets)")
    result = "PASS" if fail_count == 0 and total > 0 else "FAIL"
    colour = "\033[32m" if result == "PASS" else "\033[31m"

    print(f"\n{'─' * 62}")
    print(f"  Scenarios run  : {total}  (of {len(EXPECTED)} in sequence)")
    print(f"  PASS           : {pass_count}")
    print(f"  FAIL           : {fail_count}")
    print(note)
    print(f"  Result         : {colour}{result}\033[0m")
    print(f"{'─' * 62}")
    sys.exit(0 if result == "PASS" else 1)


if __name__ == "__main__":
    main()
