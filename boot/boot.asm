;;;;boot.asm
;;;; �������򣬼���loadģ�鵽0x90000��
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

	;;����
	mov ax, 0x0600
	mov bx, 0x700
	mov cx, 0
	mov dx, 0x184f
	int 0x10

    call DispStr



load_system1:

    mov ax,LOADSEG
	mov es,ax           ;ES:BX - ���뻺�棨0x9000��0��
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
	mov es,ax           ;ES:BX - ���뻺�棨0x8000��0��
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


;;����:����ax= ��ʼ������
;;      es:bx �Ӵ��̶�ȡ���ݵ���ָ��ָ��Ļ���
;; ��ȡһ������
readsec:
        ;push bp
        ;mov bp, sp
        ;sub esp, 2  ;;[bp-2] ��Ҫ��ȡ��������

        ;mov byte [bp-2], cl
        push cx
        push ax
        push bx
        mov bl,18 ;;bl=������ax=������
        div bl      ;;����al�У�������ah��
        inc ah      ;;
        mov cl,ah    ;;��ʼ������
        mov dh, al
        shr al, 1    ;;
        mov ch, al   ;;�ŵ���
        and dh, 1    ;;��ͷ��
        pop bx

        mov dl, 0    ;;����
.goonread:
        mov ah, 2
        mov al, byte [bp-2]
        int 0x13
        jc  .goonread
        pop ax
        pop cx

        ret
;����idt��gdt��ַ�Ĵ���idtr��gdtr
 ;   mov ax, BOOTSEG
 ;   mov ds, ax
 ;   lidt [idt_48]
 ;   lgdt [gdt_48]

;a20��ַ��
;    in al, 0x92
;    or al, 00000010b
;    out 0x92, al
;���ÿ��ƼĴ���cr0�����뱣��ģʽ����ѡ���ֵ8��Ӧgdt���еĵڶ�����������
 ;   mov ax, 0x0001
;    lmsw ax             ;װ�����״̬��
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

;ȫ����������GDT�����ݡ�����3������������
;gdt:            Descriptor 0,           0,      0
;    code32_des: Descriptor LOADSEG*16,   0x7ff,  DA_32+DA_LIMIT_4K+DA_CR
 ;   data_des:   Descriptor LOADSEG*16,   0x7ff,  DA_DRW+DA_LIMIT_4K
;cli
;idt_48: dw 0  ;idt ����0
 ;       dw 0,0; ���Ի�ַҲ��0

;gdt_48: dw 0x7ff ;gdt������2kb��������256����������
 ;       dw 0x7c00+gdt,0 ;gdt�����Ի�ַ��0x7c0�ε�gdtƫ�ƴ�

BootMessage:	dd "Now Booting ..."
msgLen equ $-BootMessage

times 510-($-$$) db 0
dw 0xaa55

