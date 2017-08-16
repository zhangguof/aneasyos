#!/bin/sh

#/usr/i686-elf-tools-windows/bin/i686-elf-gcc.exe -v
export c_path=/usr/i686-elf-tools-windows/bin
export CC=${c_path}/i686-elf-gcc.exe
export CXX=${c_path}/i686-elf-g++.exe
export LD=${c_path}/i686-elf-ld.exe

$CC -c test.c -o test.o
$LD test.o -o test