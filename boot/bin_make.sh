#!/bin/sh
export c_path=/opt/local/bin
export ASM=nasm
export LD=${c_path}/i386-elf-ld
export CC=${c_path}/i386-elf-gcc
$CC -c -fno-builtin -fno-stack-protector -m32 -std=c99 setup.c -o setup1.o
$ASM -I include/ -f elf setup.asm -o setup.o
$LD --script setup.ld setup.o setup1.o -o setup.bin

dd if=boot.bin of=../a.img bs=512 conv=notrunc
dd if=setup.bin of=../a.img bs=512 seek=1 conv=notrunc
dd if=../kernel/kernel.bin of=../a.img bs=512 seek=18 conv=notrunc



