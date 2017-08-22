;;load模块
;;用于设置基本参数gdt，idt,ldt,分页等
;;将内核kernel镜像整理至0x30000处
;;跳转到内核

%include "pm.inc"

;导入
extern print_hello
extern DispInt
extern DispStr
extern DispReturn
extern memcpy

extern DispMemInfo
extern setup_paging
extern init_kernel

extern print_mem

;;导出
global SetCR_0_3

;org 0x90000
LOADSEG equ 0x9000;
LoadBaseAdr       equ  0x90000     ;load 模块基址
PageDirBase		equ	100000h	; 页目录开始地址:		1M
PageTblBase		equ	101000h	; 页表开始地址:			1M + 4K

BaseOfKernelFile	equ	 08000h	; KERNEL.BIN 被加载到的位置 ----  段地址
OffsetOfKernelFile	equ	     0h	; KERNEL.BIN 被加载到的位置 ---- 偏移地址

BaseOfKernelFilePhyAddr	equ	BaseOfKernelFile * 10h
KernelEntryPoint	equ	030400h	; 注意：1、必须与 MAKEFILE 中参数 -Ttext 的值相等!!
								;      2、这是个地址而非仅仅是个偏移，如果 -Ttext 的值为 0x400400，则它的值也应该是 0x400400。
								;===已废弃,自动识别elf头文件入口加载进入内核

;; where kernel file is loaded
KERNEL_FILE_SEG		equ	0x8000
KERNEL_FILE_OFF		equ	0
KERNEL_FILE_PHY_ADDR	equ	KERNEL_FILE_SEG * 0x10
;; ATTENTION:
;;     Macros below should corresponding with C source.
BOOT_PARAM_ADDR		equ	0x900
BOOT_PARAM_MAGIC	equ	0xB007

;; 符号地址从0开始
[SECTION .s16]
[BITS 16]
jmp starts16


starts16:
    mov ax,cs
    mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0x1000

    mov ax,LoadMessage
	mov bp,ax
	mov cx,LmsgLen
	mov ax,01301h
	mov bx,000ch
	mov dl,0
	mov dh,1
	int 10h

; 得到内存数
	mov	ebx, 0			; ebx = 后续值, 开始时需为 0
	mov	di, _MemChkBuf		; es:di 指向一个地址范围描述符结构（Address Range Descriptor Structure）
.MemChkLoop:
	mov	eax, 0E820h		; eax = 0000E820h
	mov	ecx, 20			; ecx = 地址范围描述符结构的大小
	mov	edx, 0534D4150h		; edx = 'SMAP'
	int	15h			; int 15h
	jc	.MemChkFail
	add	di, 20
	inc	dword [_dwMCRNumber]	; dwMCRNumber = ARDS 的个数
	cmp	ebx, 0
	jne	.MemChkLoop
	jmp	.MemChkOK
.MemChkFail:
	mov	dword [_dwMCRNumber], 0
.MemChkOK:

	;jmp $

	call SetupGdt

; 关中断
	cli

; 打开地址线A20
	in	al, 92h
	or	al, 00000010b
	out	92h, al

; 准备切换到保护模式
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

; 真正进入保护模式
	jmp	dword code32_sel:start32 ;; 调整链接脚本,32位符号地址从0x90000+sizeof(.16)开始


SetupGdt:
    lgdt [gdt_48]
    ret

SetupIdt:
    nop
    ret

;;gdt--------------------------------------------------------------------
gdt:        Descriptor        0,         0,      0
code32_des  Descriptor        0,   0xfffff,      DA_32 | DA_CR | DA_LIMIT_4K
data_des    Descriptor        0,   0xfffff,      DA_32 |DA_DRW | DA_LIMIT_4K
video_des   Descriptor  0x0b8000,    0xffff,      DA_DRW | DA_DPL3
;;gdt----------------------------------------------------------------------

GdtLen equ  $ - gdt
gdt_48 dw   GdtLen            ;界限
       dd   LoadBaseAdr + gdt; 基址
;gdt 选择子---------------------------------------------
code32_sel equ code32_des - gdt
data_sel   equ data_des - gdt
video_sel  equ video_des - gdt + SA_RPL3

;;-------------------------------------------------------
;;;data---------------------------
LoadMessage:	dd "Now Loading...."
LmsgLen equ $-LoadMessage
_dwMCRNumber:			dd	0	; Memory Check Result
_dwMemSize:				dd	0
_MemChkBuf:	times	256	db	0


;;32位模式----------------------
;; 符号地址从0x90000+sizof(.s16)开始
[SECTION .s32]
ALIGN 32
[BITS 32]

start32:
    mov	ax, video_sel
	mov	gs, ax
	mov	ax, data_sel
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	ss, ax
	mov	esp, TopOfStack


	pushad
	push dword [dwMCRNumber]
	push MemChkBuf
	call DispMemInfo
	mov [dwMemSize], eax
	popad

	;jmp $
    ;; fill in BootParam[]
	mov	dword [BOOT_PARAM_ADDR], BOOT_PARAM_MAGIC ; Magic Number
	mov	eax, [dwMemSize]
	mov	[BOOT_PARAM_ADDR + 4], eax ; memory size
	mov	eax, KERNEL_FILE_SEG
	shl	eax, 4
	add	eax, KERNEL_FILE_OFF
	mov	[BOOT_PARAM_ADDR + 8], eax ; phy-addr of kernel.bin

	
	pushad
	push dword [dwMemSize]
	push dword PageDirBase
	call setup_paging
	popad

	;========加载内核镜像,进入内核=========
	pushad
	push dword KERNEL_FILE_PHY_ADDR
	call init_kernel ;;c里进入kernel
	
	popad ;;never

;;;;;;进入内核--------------------------
	;jmp dword code32_sel:KernelEntryPoint
	;mov eax, [dwElfEnterPoint]
	;jmp eax
;;;;;;----------------------------------
	jmp $
;设置cr3 cr0 启用分页
SetCR_0_3:
	push ebp
	mov ebp, esp
	mov	eax, PageDirBase
	mov	cr3, eax
	mov	eax, cr0
	or	eax, 80000000h
	mov	cr0, eax
	pop ebp
	ret

;;--------------------------------------------------------------


; InitKernel ---------------------------------------------------------------------------------
; 将 KERNEL.BIN 的内容经过整理对齐后放到新的位置
; --------------------------------------------------------------------------------------------
; 在c中实现
; InitKernel ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

[section .data]
align 32
DATA:
;;引用16位段数据
dwMemSize		equ	LoadBaseAdr + _dwMemSize
dwMCRNumber		equ	LoadBaseAdr + _dwMCRNumber
MemChkBuf		equ	LoadBaseAdr + _MemChkBuf

[section .stack]
align 32
; 堆栈就在数据段的末尾
StackSpace:	times	1000h	db	0
TopOfStack	equ	$	; 栈顶
; SECTION .data1 S结束






