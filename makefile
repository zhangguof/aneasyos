
#AnEasyOS 's makefile

#some vary
ENTRYPOINT	     = 0x30400
ENTRYPOFFSET	= 0X400
#Mulit boot use
MBENTRYPOINT     =0x100400
#ENTRYPOINT		= $(MBENTRYPOINT)
c_path=/opt/local/bin
#flag,compiler
ASM				 =nasm
LD				 =$(c_path)/i386-elf-ld
CC				 =$(c_path)/i386-elf-gcc
ASMBFLAGS		 = -I boot/include/
ASMKFLAGS		 = -I include/ -f elf
LDFLAGS			 = -s -Ttext $(ENTRYPOINT)
#LDFLAGS			 = -elf -T link.ld --oformat elf32-i386
CFLAGS			 = -I include/ -c -fno-builtin -fno-stack-protector

#program
BOOT			 = boot/boot.bin boot/setup.bin
KERNEL			 = kernel/kernel.bin

CSRC= $(wildcard kernel/*.c lib/*.c mm/*.c)
ASRC= $(wildcard kernel/asm/*.asm lib/asm/*.asm)
OBJS= $(addsuffix .o,$(basename $(ASRC) $(CSRC)))

bootlen			 = 1
setuplen	     = 17
kernellen		 = 20

#ALL PHONE Targets
.PHONY :  clean all image everything

#start
image: a.img
all: clean image

a.img: $(BOOT) $(KERNEL)
	dd if=boot/boot.bin of=a.img bs=512 conv=notrunc
	dd if=boot/setup.bin of=a.img bs=512 seek=1 conv=notrunc
	dd if=kernel/kernel.bin of=a.img bs=512 seek=18 conv=notrunc

clean:
		rm -f $(BOOT) $(KERNEL) $(OBJS)
#part of boot
boot/boot.bin: boot/boot.asm
		$(ASM) $(ASMBFLAGS) -o $@ $<
boot/setup.bin: boot/setup.asm
		$(ASM) $(ASMBFLAGS) -o $@ $<


#part of kernel bin
kernel/kernel.bin: $(OBJS)
		$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJS)

#for all .asm
kernel/asm/%.o: kernel/asm/%.asm
		$(ASM) $(ASMKFLAGS) -o $@ $<
lib/asm/%.o: lib/asm/%.asm
		$(ASM) $(ASMKFLAGS) -o $@ $<

#for all .c
kernel/%.o: kernel/%.c
		$(CC) $(CFLAGS) -o $@ $<
lib/%.o: lib/%.c
		$(CC) $(CFLAGS) -o $@ $<







