#!/bin/sh
export c_path=/opt/local
export CC=${c_path}/bin/i386-elf-gcc
export CXX=${c_path}/bin/i386-elf-g++
export LD=${c_path}/bin/i386-elf-ld

$CC -c test.c -o test.o
$LD test.o -o test