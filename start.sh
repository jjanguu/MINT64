make clean;
make;
qemu-system-x86_64 -L . -m 64 -fda ./Disk.img -hda ./HDD.img -boot a  -M pc -curses
