;;;;boot.asm
;;;; 引导程序，加载load模块到0x90000处
%include "pm.inc"

	BOOTSEG equ 0x7c0
	LOADSEG equ 0x9000;
	LOADLEN equ 17
	SYSSEG  equ 0x8000;
	SYSLEN  equ 100
	;org BOOTSEG
start: jmp BOOTSEG:go
go:
	mov ax,cs
	mov ds,ax
	mov es,ax
	mov ss,ax
	mov sp,0x7c00

	;;清屏
	mov ax, 0x0600
	mov bx, 0x700
	mov cx, 0
	mov dx, 0x184f
	int 0x10

    call DispStr



load_system1:

    mov ax,LOADSEG
	mov es,ax           ;ES:BX - 读入缓存（0x9000：0）
	xor bx,bx

    mov ax,0x1
    mov cx,LOADLEN
.1: call readsec
    dec cx
    inc ax
    add bx, 512
    cmp cx,0
    jnz .1


load_system2:
    mov ax,SYSSEG
	mov es,ax           ;ES:BX - 读入缓存（0x8000：0）
	xor bx,bx

    mov ax,LOADLEN+1
    mov cx,SYSLEN
.1: call readsec
    dec cx
    inc ax
    add bx, 512
    cmp cx,0
    jnz .1
    jmp LOADSEG:0


;;输入:参数ax= 起始扇区号
;;      es:bx 从磁盘读取数据到该指针指向的缓存
;; 读取一个扇区
readsec:
        ;push bp
        ;mov bp, sp
        ;sub esp, 2  ;;[bp-2] 需要读取的扇区数

        ;mov byte [bp-2], cl
        push cx
        push ax
        push bx
        mov bl,18 ;;bl=除数，ax=被除数
        div bl      ;;商在al中，余数早ah中
        inc ah      ;;
        mov cl,ah    ;;起始扇区号
        mov dh, al
        shr al, 1    ;;
        mov ch, al   ;;磁道号
        and dh, 1    ;;磁头号
        pop bx

        mov dl, 0    ;;磁盘
.goonread:
        mov ah, 2
        mov al, byte [bp-2]
        int 0x13
        jc  .goonread
        pop ax
        pop cx

        ret
;加载idt，gdt基址寄存器idtr和gdtr
 ;   mov ax, BOOTSEG
 ;   mov ds, ax
 ;   lidt [idt_48]
 ;   lgdt [gdt_48]

;a20地址线
;    in al, 0x92
;    or al, 00000010b
;    out 0x92, al
;设置控制寄存器cr0，进入保护模式。段选择符值8对应gdt表中的第二个段描述符
 ;   mov ax, 0x0001
;    lmsw ax             ;装入机器状态字
    ;mov eax,cr0
   ; or eax, 1
    ;mov cr0,eax

;    jmp dword 8:0

DispStr:
	mov ax,BootMessage
	mov bp,ax
	mov cx,msgLen
	mov ax,01301h
	mov bx,000ch
	mov dl,0
	mov dh,0
	int 10h
	ret

;全局描述符表GDT的内容。包含3个段描述符。
;gdt:            Descriptor 0,           0,      0
;    code32_des: Descriptor LOADSEG*16,   0x7ff,  DA_32+DA_LIMIT_4K+DA_CR
 ;   data_des:   Descriptor LOADSEG*16,   0x7ff,  DA_DRW+DA_LIMIT_4K
;cli
;idt_48: dw 0  ;idt 长度0
 ;       dw 0,0; 线性基址也是0

;gdt_48: dw 0x7ff ;gdt表长度是2kb，可容纳256个描述符项
 ;       dw 0x7c00+gdt,0 ;gdt的线性基址在0x7c0段的gdt偏移处

BootMessage:	dd "Now Booting ..."
msgLen equ $-BootMessage

times 510-($-$$) db 0
dw 0xaa55

