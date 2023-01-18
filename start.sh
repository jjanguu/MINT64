clear;make clean;make;qemu-system-x86_64 -L . -m 64 -fda ./DiskWithPackage.img -boot a -M pc -smp 4 -rtc base=localtime,clock=host
