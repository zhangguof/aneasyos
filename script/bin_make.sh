#!/bin/sh
export c_path=/opt/local/bin
export ASM=nasm
export LD=${c_path}/i386-elf-ld
export CC=${c_path}/i386-elf-gcc
export boot_src=../src/boot
export bin_path=../bin/
$CC -c -fno-builtin -fno-stack-protector -m32 -std=c99 ${boot_src}/setup_c.c -o ${boot_src}/setup_c.o
$ASM -I ${boot_src}/include/ -f elf ${boot_src}/setup.asm -o ${boot_src}/setup.o
$LD --script setup.ld ${boot_src}/setup.o ${boot_src}/setup_c.o -o ${bin_path}/setup.bin

dd if=${bin_path}/boot.bin of=${bin_path}/a.img bs=512 conv=notrunc
dd if=${bin_path}/setup.bin of=${bin_path}/a.img bs=512 seek=1 conv=notrunc
dd if=${bin_path}/kernel.bin of=${bin_path}/a.img bs=512 seek=18 conv=notrunc



