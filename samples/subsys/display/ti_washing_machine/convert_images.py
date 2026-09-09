#!/usr/bin/env python3
"""
Convert LVGL ARGB8888 image C files to RGB565A8 for embedded targets.

Also downscales by SCALE_FACTOR using a box filter (default 0.5 = halve each
dimension) to fit in the LP-MSPM0G3519's 512 KB flash.

LVGL ARGB8888 memory layout: B, G, R, A per pixel (lv_color32_t byte order).
LVGL RGB565A8 memory layout: [w*h RGB565 LE words][w*h alpha bytes], stride = w*2.
"""

import re
import sys
import os

SCALE_FACTOR = 0.5   # 50% — halves each dimension, quarter area


def parse_array(content):
    """Return list of byte values from the static C uint8_t array."""
    # Declaration may span multiple lines with attribute macros.
    m = re.search(r'uint8_t \w+\[\]\s*=\s*\{(.*?)\};', content, re.DOTALL)
    if not m:
        raise ValueError("Could not find uint8_t array in file")
    return [int(v, 16) for v in re.findall(r'0x([0-9a-fA-F]{1,2})', m.group(1))]


def box_scale(bgra, src_w, src_h, factor):
    """Downscale BGRA flat bytes by factor using a box filter."""
    dst_w = max(1, int(src_w * factor))
    dst_h = max(1, int(src_h * factor))
    fw = src_w / dst_w
    fh = src_h / dst_h
    out = []
    for dy in range(dst_h):
        sy0 = int(dy * fh)
        sy1 = min(src_h, int((dy + 1) * fh))
        for dx in range(dst_w):
            sx0 = int(dx * fw)
            sx1 = min(src_w, int((dx + 1) * fw))
            acc = [0, 0, 0, 0]
            count = 0
            for sy in range(sy0, sy1):
                for sx in range(sx0, sx1):
                    base = (sy * src_w + sx) * 4
                    for c in range(4):
                        acc[c] += bgra[base + c]
                    count += 1
            out.extend(round(v / count) for v in acc)
    return bytes(out), dst_w, dst_h


def bgra_to_rgb565a8(bgra, w, h):
    """Convert flat BGRA bytes to non-interleaved RGB565 + alpha bytes."""
    n = w * h
    rgb = []
    alpha = []
    for i in range(n):
        b, g, r, a = bgra[i*4], bgra[i*4+1], bgra[i*4+2], bgra[i*4+3]
        v = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
        rgb.append(v & 0xFF)
        rgb.append((v >> 8) & 0xFF)
        alpha.append(a)
    return bytes(rgb) + bytes(alpha)


def format_hex_array(name, data, cols=16):
    lines = []
    for i in range(0, len(data), cols):
        chunk = data[i:i+cols]
        lines.append("    " + ",".join(f"0x{b:02x}" for b in chunk) + ",")
    return f"static const uint8_t {name}[] = {{\n" + "\n".join(lines) + "\n};\n"


HEADER_GUARD = """\
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_LVGL_H_INCLUDE_SYSTEM)
#include <lvgl.h>
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif
"""


def convert_file(path, scale=SCALE_FACTOR):
    with open(path) as f:
        content = f.read()

    m = re.search(r'const lv_image_dsc_t (\w+)\s*=', content)
    if not m:
        print(f"  skip {path}: no lv_image_dsc_t found")
        return

    img_name = m.group(1)
    map_name = img_name + "_map"

    w = int(re.search(r'\.w\s*=\s*(\d+)', content).group(1))
    h = int(re.search(r'\.h\s*=\s*(\d+)', content).group(1))
    cf = re.search(r'\.cf\s*=\s*(\S+),', content).group(1)

    if cf == "LV_COLOR_FORMAT_RGB565A8":
        # Already converted — re-run if a different scale is needed.
        # Detect original dimensions from current dimensions and redo.
        # For idempotency, skip if scale == 1.0.
        if scale == 1.0:
            print(f"  skip {path}: already RGB565A8")
            return
        # Parse existing RGB565A8 data and decode back to BGRA for rescaling
        raw = bytearray(parse_array(content))
        n = w * h
        rgb_plane = raw[:n*2]
        a_plane = raw[n*2:]
        bgra_bytes = bytearray(n * 4)
        for i in range(n):
            lo = rgb_plane[i*2]
            hi = rgb_plane[i*2+1]
            v = lo | (hi << 8)
            r8 = ((v >> 11) & 0x1F) << 3
            g8 = ((v >> 5) & 0x3F) << 2
            b8 = (v & 0x1F) << 3
            bgra_bytes[i*4]   = b8
            bgra_bytes[i*4+1] = g8
            bgra_bytes[i*4+2] = r8
            bgra_bytes[i*4+3] = a_plane[i]
        raw = bytes(bgra_bytes)
        orig_cf = "LV_COLOR_FORMAT_ARGB8888"
    else:
        if cf != "LV_COLOR_FORMAT_ARGB8888":
            print(f"  skip {path}: unsupported format {cf}")
            return
        raw = bytes(parse_array(content))
        orig_cf = cf

    expected = w * h * 4
    if len(raw) != expected:
        print(f"    WARNING: expected {expected} bytes, got {len(raw)} — skipping")
        return

    print(f"  converting {img_name} {w}x{h} {orig_cf} -> RGB565A8 (scale={scale}) ...")

    scaled_bgra, dst_w, dst_h = box_scale(raw, w, h, scale)
    converted = bgra_to_rgb565a8(scaled_bgra, dst_w, dst_h)
    stride = dst_w * 2

    array_macro = f"LV_ATTRIBUTE_IMG_{img_name.upper()}"
    out = HEADER_GUARD
    out += f"\n#ifndef {array_macro}\n#define {array_macro}\n#endif\n\n"
    out += f"LV_ATTRIBUTE_MEM_ALIGN {array_macro} "
    out += format_hex_array(map_name, converted)
    out += f"""
const lv_image_dsc_t {img_name} = {{
  .header = {{
    .magic = LV_IMAGE_HEADER_MAGIC,
    .cf = LV_COLOR_FORMAT_RGB565A8,
    .flags = 0,
    .w = {dst_w},
    .h = {dst_h},
    .stride = {stride},
    .reserved_2 = 0,
  }},
  .data_size = sizeof({map_name}),
  .data = {map_name},
  .reserved = NULL,
}};
"""

    with open(path, "w") as f:
        f.write(out)

    old_kb = (w * h * 4) / 1024
    new_kb = len(converted) / 1024
    print(f"    {w}x{h} -> {dst_w}x{dst_h}, {old_kb:.0f} KB -> {new_kb:.0f} KB (saves {old_kb-new_kb:.0f} KB)")


if __name__ == "__main__":
    scale = float(sys.argv[1]) if len(sys.argv) > 1 else SCALE_FACTOR
    targets = [
        "ui/images/img_machine_data.c",
        "ui/images/img_door_data.c",
        "ui/images/img_clothes_data.c",
        "ui/images/img_logostrip_data.c",
    ]
    base = os.path.dirname(os.path.abspath(__file__))
    for t in targets:
        p = os.path.join(base, t)
        convert_file(p, scale=scale)
    print("Done.")
