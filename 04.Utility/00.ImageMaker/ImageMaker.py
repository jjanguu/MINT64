#!/usr/bin/env python3
import sys
import struct

def sector_align(data):
    if len(data) % 512 != 0:
        data += b"\x00"*(512 - (len(data) % 512))
    return data

if len(sys.argv) < 4:
    print("[ERROR] ImageMaker BootLoader.bin Kernel32.bin Kernel64.bin")
    sys.exit(-1)

bootLoader = sector_align(open(sys.argv[1], "rb").read())
kernel32 = sector_align(open(sys.argv[2], "rb").read())
kernel64 = sector_align(open(sys.argv[3], "rb").read())

imgData = bootLoader + kernel32 + kernel64

kernel32SectorCount = int(len(kernel32) / 512)
kernel64SectorCount = int(len(kernel64) / 512)


with open("Disk.img", "wb") as diskFile:
    diskFile.write(imgData[:5] + struct.pack("<H", kernel32SectorCount + kernel64SectorCount) + struct.pack("<H", kernel32SectorCount) + imgData[9:])