#!/usr/bin/env python3
import sys
import struct

def sector_align(data):
    if len(data) % 512 != 0:
        data += b"\x00"*(512 - (len(data) % 512))
    return data

if len(sys.argv) < 3:
    print("[ERROR] ImageMaker BootLoader.bin Kernel32.bin")
    sys.exit(-1)

bootLoader = sector_align(open(sys.argv[1], "rb").read())
kernel32 = sector_align(open(sys.argv[2], "rb").read())

imgData = bootLoader + kernel32

sectorCount = struct.pack("<H", int(len(kernel32) / 512))

with open("Disk.img", "wb") as diskFile:
    diskFile.write(imgData[:5] + sectorCount + imgData[7:])