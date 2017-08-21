;;load模块
;;用于设置基本参数gdt，idt,ldt,分页等
;;将内核kernel镜像整理至0x30000处
;;跳转到内核

%include "pm.inc"

;extern disp_int

extern print_hello
extern DispInt
extern DispStr
extern DispReturn

;;导出
;;global DispStr   ;

;org 0x90000
LOADSEG equ 0x9000;
LoadBaseAdr       equ  0x90000     ;load 模块基址
PageDirBase		equ	100000h	; 页目录开始地址:		1M
PageTblBase		equ	101000h	; 页表开始地址:			1M + 4K

BaseOfKernelFile	equ	 08000h	; KERNEL.BIN 被加载到的位置 ----  段地址
OffsetOfKernelFile	equ	     0h	; KERNEL.BIN 被加载到的位置 ---- 偏移地址

BaseOfKernelFilePhyAddr	equ	BaseOfKernelFile * 10h
KernelEntryPoint	equ	030400h	; 注意：1、必须与 MAKEFILE 中参数 -Ttext 的值相等!!
					;       2、这是个地址而非仅仅是个偏移，如果 -Ttext 的值为 0x400400，则它的值也应该是 0x400400。


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
_dwDispPos:			dd	(80 * 6 + 0) * 2	; 屏幕第 6 行, 第 0 列。
_dwMemSize:			dd	0
_ARDStruct:			; Address Range Descriptor Structure
	_dwBaseAddrLow:		dd	0
	_dwBaseAddrHigh:	dd	0
	_dwLengthLow:		dd	0
	_dwLengthHigh:		dd	0
	_dwType:		dd	0
_MemChkBuf:	times	256	db	0


;;32位模式----------------------
;; 符号地址从0x90000+sizof(.16)开始
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

	;call print_hello

	pushad
	push szMemChkTitle
	call DispStr
	add esp, 4
	popad

	call DispMemInfo

	

	call SetupPaging
	call InitKernel

    ;; fill in BootParam[]
	mov	dword [BOOT_PARAM_ADDR], BOOT_PARAM_MAGIC ; Magic Number
	mov	eax, [dwMemSize]
	mov	[BOOT_PARAM_ADDR + 4], eax ; memory size
	mov	eax, KERNEL_FILE_SEG
	shl	eax, 4
	add	eax, KERNEL_FILE_OFF
	mov	[BOOT_PARAM_ADDR + 8], eax ; phy-addr of kernel.bin

	;;call print_hello
;;;;;;进入内核--------------------------
	;jmp dword code32_sel:KernelEntryPoint
	call print_hello
	mov eax, [dwElfEnterPoint]
	jmp eax
;;;;;;----------------------------------

	jmp $





; ------------------------------------------------------------------------
; 内存拷贝，仿 memcpy
; ------------------------------------------------------------------------
; void* MemCpy(void* es:pDest, void* ds:pSrc, int iSize);
; ------------------------------------------------------------------------
MemCpy:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination
	mov	esi, [ebp + 12]	; Source
	mov	ecx, [ebp + 16]	; Counter
.1:
	cmp	ecx, 0		; 判断计数器
	jz	.2		; 计数器为零时跳出

	mov	al, [ds:esi]		; ┓
	inc	esi			; ┃
					; ┣ 逐字节移动
	mov	byte [es:edi], al	; ┃
	inc	edi			; ┛

	dec	ecx		; 计数器减一
	jmp	.1		; 循环
.2:
	mov	eax, [ebp + 8]	; 返回值

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; 函数结束，返回
; MemCpy 结束-------------------------------------------------------------


; 显示内存信息 --------------------------------------------------------------
DispMemInfo:
	push	esi
	push	edi
	push	ecx

	mov	esi, MemChkBuf
	mov	ecx, [dwMCRNumber]	;for(int i=0;i<[MCRNumber];i++) // 每次得到一个ARDS(Address Range Descriptor Structure)结构
.loop:					;{
	mov	edx, 5			;	for(int j=0;j<5;j++)	// 每次得到一个ARDS中的成员，共5个成员
	mov	edi, ARDStruct		;	{			// 依次显示：BaseAddrLow，BaseAddrHigh，LengthLow，LengthHigh，Type
.1:					;
	pushad
	push	dword [esi]		;
	call	DispInt			;		DispInt(MemChkBuf[j*4]); // 显示一个成员
	pop	eax			;
	popad

	stosd				;		ARDStruct[j*4] = MemChkBuf[j*4];
	add	esi, 4			;
	dec	edx			;
	cmp	edx, 0		;
	jnz	.1			;	}

	pushad
	call	DispReturn		;	printf("\n");
	popad

	cmp	dword [dwType], 1	;	if(Type == AddressRangeMemory) // AddressRangeMemory : 1, AddressRangeReserved : 2
	jne	.2			;	{
	mov	eax, [dwBaseAddrLow]	;
	add	eax, [dwLengthLow]	;
	cmp	eax, [dwMemSize]	;		if(BaseAddrLow + LengthLow > MemSize)
	jb	.2			;
	mov	[dwMemSize], eax	;			MemSize = BaseAddrLow + LengthLow;
.2:					;	}
	loop	.loop			;}
					;
	pushad
	call	DispReturn		;printf("\n");
	popad

	pushad
	push	szRAMSize		;
	call	DispStr			;printf("RAM size:");
	add	esp, 4			;
	popad				;

	pushad
	push	dword [dwMemSize]	;
	call	DispInt			;DispInt(MemSize);
	add	esp, 4			;
	popad

	pop	ecx
	pop	edi
	pop	esi
	ret
; ---------------------------------------------------------------------------

; 启动分页机制 --------------------------------------------------------------
SetupPaging:
	; 根据内存大小计算应初始化多少PDE以及多少页表
	xor	edx, edx
	mov	eax, [dwMemSize]
	mov	ebx, 400000h	; 400000h = 4M = 4096 * 1024, 一个页表对应的内存大小
	div	ebx
	mov	ecx, eax	; 此时 ecx 为页表的个数，也即 PDE 应该的个数
	test	edx, edx
	jz	.no_remainder
	inc	ecx		; 如果余数不为 0 就需增加一个页表
.no_remainder:
	push	ecx		; 暂存页表个数

	; 为简化处理, 所有线性地址对应相等的物理地址. 并且不考虑内存空洞.

	; 首先初始化页目录
	mov	ax, data_sel
	mov	es, ax
	mov	edi, PageDirBase	; 此段首地址为 PageDirBase
	xor	eax, eax
	mov	eax, PageTblBase | PG_P  | PG_USU | PG_RWW
.1:
	stosd
	add	eax, 4096		; 为了简化, 所有页表在内存中是连续的.
	loop	.1

	; 再初始化所有页表
	pop	eax			; 页表个数
	mov	ebx, 1024		; 每个页表 1024 个 PTE
	mul	ebx
	mov	ecx, eax		; PTE个数 = 页表个数 * 1024
	mov	edi, PageTblBase	; 此段首地址为 PageTblBase
	xor	eax, eax
	mov	eax, PG_P  | PG_USU | PG_RWW
.2:
	stosd
	add	eax, 4096		; 每一页指向 4K 的空间
	loop	.2

	mov	eax, PageDirBase
	mov	cr3, eax
	mov	eax, cr0
	or	eax, 80000000h
	mov	cr0, eax
	jmp	short .3
.3:
	nop

	ret

;;--------------------------------------------------------------


; InitKernel ---------------------------------------------------------------------------------
; 将 KERNEL.BIN 的内容经过整理对齐后放到新的位置
; --------------------------------------------------------------------------------------------
InitKernel:	; 遍历每一个 Program Header，根据 Program Header 中的信息来确定把什么放进内存，放到什么位置，以及放多少。
	mov eax, [BaseOfKernelFilePhyAddr + 18h]; eax <-pELFHdr->e_entry
	mov [dwElfEnterPoint], eax; elf 入口地址
	xor	esi, esi
	mov	cx, word [BaseOfKernelFilePhyAddr + 2Ch]; ┓ ecx <- pELFHdr->e_phnum
	movzx	ecx, cx					; ┛
	mov	esi, [BaseOfKernelFilePhyAddr + 1Ch]	; esi <- pELFHdr->e_phoff
	add	esi, BaseOfKernelFilePhyAddr		; esi <- OffsetOfKernel + pELFHdr->e_phoff
.Begin:
	mov	eax, [esi + 0]
	cmp	eax, 0				; PT_NULL
	jz	.NoAction
	push	dword [esi + 010h]		; size	┓
	mov	eax, [esi + 04h]		;	┃
	add	eax, BaseOfKernelFilePhyAddr	;	┣ ::memcpy(	(void*)(pPHdr->p_vaddr),
	push	eax				; src	┃		uchCode + pPHdr->p_offset,
	push	dword [esi + 08h]		; dst	┃		pPHdr->p_filesz;
	call	MemCpy				;	┃
	add	esp, 12				;	┛
.NoAction:
	add	esi, 020h			; esi += pELFHdr->e_phentsize
	dec	ecx
	jnz	.Begin

	ret
; InitKernel ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

[section .data]
align 32
DATA:


;;--------------------------------

;;字符串
_szMemChkTitle:			db	"BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0
_szRAMSize:			db	"RAM size:", 0
_szReturn:			db	0Ah, 0
;;变量

;
_dwElfEnterPoint: dd 0


;; 保护模式下使用这些符号
szMemChkTitle		equ	_szMemChkTitle
szRAMSize		equ	_szRAMSize
szReturn		equ	_szReturn

;;引用16位段数据
dwDispPos		equ	LoadBaseAdr + _dwDispPos
dwMemSize		equ	LoadBaseAdr + _dwMemSize
dwMCRNumber		equ	LoadBaseAdr + _dwMCRNumber
ARDStruct		equ	LoadBaseAdr + _ARDStruct
	dwBaseAddrLow	equ	LoadBaseAdr + _dwBaseAddrLow
	dwBaseAddrHigh	equ	LoadBaseAdr + _dwBaseAddrHigh
	dwLengthLow	equ	LoadBaseAdr + _dwLengthLow
	dwLengthHigh	equ	LoadBaseAdr + _dwLengthHigh
	dwType		equ	LoadBaseAdr + _dwType
MemChkBuf		equ	LoadBaseAdr + _MemChkBuf

dwElfEnterPoint equ _dwElfEnterPoint
[section .stack]
align 32
; 堆栈就在数据段的末尾
StackSpace:	times	1000h	db	0
TopOfStack	equ	$	; 栈顶
; SECTION .data1 S结束






