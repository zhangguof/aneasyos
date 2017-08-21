
#AnEasyOS 's makefile

#some vary
ENTRYPOINT	     = 0x30400
ENTRYPOFFSET	= 0X400
#Mulit boot use
MBENTRYPOINT     =0x100400
#ENTRYPOINT		= $(MBENTRYPOINT)
TOOL_PATH=/opt/local/bin
SRC_PATH = src
BIN_PATH = bin
#OBJS_PATH = bin/objs
OBJS_PATH = src

LD_SCRIPT = script/setup.ld 

OUT_IMG = $(BIN_PATH)/a.img
#flag,compiler
ASM				 = nasm
LD				 = $(TOOL_PATH)/i386-elf-ld
CC				 = $(TOOL_PATH)/i386-elf-gcc
ASMBFLAGS		 = -I $(SRC_PATH)/boot/include/
ASMKFLAGS		 = -g -I $(SRC_PATH)/include/ -f elf
LDFLAGS			 = -Ttext $(ENTRYPOINT) -m elf_i386
#LDFLAGS			 = -elf -T link.ld --oformat elf32-i386
CFLAGS			 = -g -I $(SRC_PATH)/include/ -c -fno-builtin -fno-stack-protector -m32 -std=c99

#program
BOOT			 = $(BIN_PATH)/boot.bin $(BIN_PATH)/setup.bin
KERNEL			 = $(BIN_PATH)/kernel.bin

CSRC= $(wildcard $(SRC_PATH)/kernel/*.c $(SRC_PATH)/lib/*.c $(SRC_PATH)/mm/*.c)
ASRC= $(wildcard $(SRC_PATH)/kernel/asm/*.asm $(SRC_PATH)/lib/asm/*.asm)
OBJS1= $(addsuffix .o,$(basename $(ASRC) $(CSRC)))
OBJS= $(subst $(SRC_PATH)/, $(OBJS_PATH)/, $(OBJS1))

bootlen			 = 1
setuplen	     = 17
kernel_off       = $(shell expr $(setuplen) + $(bootlen))
kernellen		 = 20

#ALL PHONE Targets
.PHONY :  clean all image everything

#start
image: a.img
all: clean image

a.img: $(BOOT) $(KERNEL)
	dd if=$(BIN_PATH)/boot.bin of=$(OUT_IMG) bs=512 conv=notrunc
	dd if=$(BIN_PATH)/setup.bin of=$(OUT_IMG) bs=512 seek=$(bootlen) conv=notrunc
	dd if=$(BIN_PATH)/kernel.bin of=$(OUT_IMG) bs=512 seek=$(kernel_off) conv=notrunc

clean:
		rm -f $(BOOT) $(KERNEL) $(OBJS)
#part of boot
$(BIN_PATH)/boot.bin: $(SRC_PATH)/boot/boot.asm
		$(ASM) $(ASMBFLAGS) -o $@ $<

$(BIN_PATH)/setup.bin: $(SRC_PATH)/boot/setup.asm $(SRC_PATH)/boot/setup_c.c
		$(CC) -c -fno-builtin -fno-stack-protector -m32 -std=c99 $(SRC_PATH)/boot/setup_c.c -o $(OBJS_PATH)/boot/setup_c.o
		$(ASM) -I $(SRC_PATH)/boot/include/ -f elf $(SRC_PATH)/boot/setup.asm -o $(OBJS_PATH)/boot/setup.o
		$(LD) --script $(LD_SCRIPT) $(OBJS_PATH)/boot/setup.o $(OBJS_PATH)/boot/setup_c.o -o $(BIN_PATH)/setup.bin


#part of kernel bin
$(BIN_PATH)/kernel.bin: $(OBJS)
		$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJS)

#for all .asm
$(OBJS_PATH)/kernel/asm/%.o: $(SRC_PATH)/kernel/asm/%.asm
		$(ASM) $(ASMKFLAGS) -o $@ $<
$(OBJS_PATH)/lib/asm/%.o: $(SRC_PATH)/lib/asm/%.asm
		$(ASM) $(ASMKFLAGS) -o $@ $<

#for all .c
$(OBJS_PATH)/kernel/%.o: $(SRC_PATH)/kernel/%.c
		$(CC) $(CFLAGS) -o $@ $<

$(OBJS_PATH)/lib/%.o: l$(SRC_PATH)/ib/%.c
		$(CC) $(CFLAGS) -o $@ $<







