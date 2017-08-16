;系统的内核模块
;;kernel.bin
;;nasm -f elf kernel.asm -o kernel.o
;;ld -s kernel.o -o kernel.bin

;;中断的
%include "sconst.inc"

;;导入函数
extern cstart
extern main
extern gdt_ptr
extern idt_ptr
extern p_proc_ready
extern tss
extern k_reenter
extern sys_call_table
;extern disp_int

extern get_mbi


;;导出
global _start   ;;导出_start ,用于ld识别入口
global restart
global restart_reenter
global TopOfStack
global save
global sys_call

[section .bss]
StackSpace resb 2 * 1024  ;;2kb的栈空间
TopOfStack:               ;栈顶 内核栈空间


[section .text]
[bits 32]
_start:

jmp mulit_entry
; ----- Multiboot Header Starts Here -----
MULTIBOOT_PAGE_ALIGN   equ 1<<0
MULTIBOOT_MEMORY_INFO  equ 1<<1
MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
; The Multiboot header
align 4
dd MULTIBOOT_HEADER_MAGIC
dd MULTIBOOT_HEADER_FLAGS
dd CHECKSUM
; ----- Multiboot Header Ends Here -----

;start:
mulit_entry:
       ; mov ah,0x0f
       ; mov al,'K'
        ;mov ebx, 0x0b8000
        ;mov [ebx + (80*1+39)*2],ax
        ;jmp $
        ; 将esp 从 setup 移动到kernel
        mov esp, TopOfStack

        push ebx
        call get_mbi
        add esp,4
       ; push eax
        ;call disp_int;
        ;add esp,4
       ; hlt
        ;;test
        ;sgdt [gdt_ptr]; 保存gdt信息到gdt_ptr

        call cstart   ;;重置gdt，idt

        lgdt[gdt_ptr]
        lidt[idt_ptr];

        jmp SEL_KERNEL_CS:csinit
csinit:
        xor eax, eax
        mov	ax, 0x18 + 3
	    mov	gs, ax
	    mov	ax, 0x10
	    mov	ds, ax
	    mov	es, ax
	    mov	fs, ax
	    mov	ss, ax
	    xor eax, eax
        mov ax, sel_tss
        ltr ax
        call main
    ;jmp 0x40:10
        sti
        hlt
;;=================restart====================
restart:
        mov esp, [p_proc_ready]
        lldt [esp + P_LDT_SEL]
        lea eax, [esp   + P_STACKTOP]
        mov dword [tss + TSS3_S_SP0] ,eax
restart_reenter:
        dec dword[k_reenter]
        pop gs
        pop fs
        pop es
        pop ds
        popad

        add esp ,4
        iretd
;;============save===========
save:

    pushad
    push ds
    push es
    push fs
    push gs

    mov esi, edx ;;保存edx
    mov dx, ss
    mov ds, dx
    mov es, dx

    mov edx, esi ;;恢复edx

    mov esi, esp ;esi = 进程表的起始地址，regs的开始处
    inc dword [k_reenter];
    cmp dword[k_reenter], 0
    jne .1   ;;中断重入
    mov esp, TopOfStack; ;内核栈
    push restart
    jmp [esi+RETADR-P_STACKBASE] ;return
.1:
    push restart_reenter
    jmp [esi+RETADR-P_STACKBASE] ;return

;;===========sys_call============================
;;----------------------------------------------

sys_call:
    call save
    push dword[p_proc_ready]
    sti

   ; push esi
    push edx
    push ecx
    push ebx
    call [sys_call_table+eax*4]  ;;
    add esp, 4*4    ;;四个参数

    ;pop esi

    mov [esi + EAXREG - P_STACKBASE], eax
    cli
    ret





