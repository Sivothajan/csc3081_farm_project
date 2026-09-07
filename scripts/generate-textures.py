"""
Generate the project's original seamless 64x64, 24-bit BMP material textures.
No third-party images or Python packages are required. Rerunning is optional.
"""

from pathlib import Path
import math
import random
import struct

ROOT = Path(__file__).resolve().parents[1] / "assets" / "textures"


def make(relative, kind, seed):
    rng = random.Random(seed)
    size = 64
    pixels = bytearray()
    for y in range(size):
        for x in range(size):
            n = rng.uniform(-14, 14)
            if kind == "wood":
                value = (
                    215
                    + 17 * math.sin(x * math.tau / 8 + 0.7 * math.sin(y * math.tau / 64))
                    + n * 0.5
                )
                if x % 32 == 0:
                    value -= 38
            elif kind == "hay":
                value = 221 + 17 * math.sin(y * math.tau / 4) + n
            elif kind == "grass":
                value = 225 + n + 8 * math.sin(x * math.tau / 16) * math.sin(y * math.tau / 16)
            else:
                value = 230 + n * 1.3
            v = max(0, min(255, round(value)))
            pixels.extend((v, v, v))
    header = b"BM" + struct.pack("<IHHI", 54 + len(pixels), 0, 0, 54)
    header += struct.pack("<IiiHHIIiiII", 40, size, size, 1, 24, 0, len(pixels), 2835, 2835, 0, 0)
    path = ROOT / relative
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(header + pixels)


make("terrain/grass_ground.bmp", "grass", 3081)
make("terrain/dirt.bmp", "dirt", 3082)
make("structures/wood.bmp", "wood", 3083)
make("structures/hay.bmp", "hay", 3084)
