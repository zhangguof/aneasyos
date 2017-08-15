;;loadģ��
;;�������û�������gdt��idt,ldt,��ҳ��
;;���ں�kernel����������0x30000��
;;��ת���ں�

%include "pm.inc"
;org 0x90000
LoadBaseAdr       equ  0x90000     ;load ģ���ַ
PageDirBase		equ	100000h	; ҳĿ¼��ʼ��ַ:		1M
PageTblBase		equ	101000h	; ҳ��ʼ��ַ:			1M + 4K

BaseOfKernelFile	equ	 08000h	; KERNEL.BIN �����ص���λ�� ----  �ε�ַ
OffsetOfKernelFile	equ	     0h	; KERNEL.BIN �����ص���λ�� ---- ƫ�Ƶ�ַ

BaseOfKernelFilePhyAddr	equ	BaseOfKernelFile * 10h
KernelEntryPoint	equ	030400h	; ע�⣺1�������� MAKEFILE �в��� -Ttext ��ֵ���!!
					;       2�����Ǹ���ַ���ǽ����Ǹ�ƫ�ƣ���� -Ttext ��ֵΪ 0x400400��������ֵҲӦ���� 0x400400��


;; where kernel file is loaded
KERNEL_FILE_SEG		equ	0x8000
KERNEL_FILE_OFF		equ	0
KERNEL_FILE_PHY_ADDR	equ	KERNEL_FILE_SEG * 0x10
;; ATTENTION:
;;     Macros below should corresponding with C source.
BOOT_PARAM_ADDR		equ	0x900
BOOT_PARAM_MAGIC	equ	0xB007


jmp start

;;gdt--------------------------------------------------------------------
gdt:        Descriptor        0,         0,      0
code32_des  Descriptor        0,   0xfffff,      DA_32 | DA_CR | DA_LIMIT_4K
data_des    Descriptor        0,   0xfffff,      DA_32 |DA_DRW | DA_LIMIT_4K
video_des  Descriptor  0x0b8000,    0xffff,      DA_DRW | DA_DPL3
;;gdt----------------------------------------------------------------------

GdtLen equ  $ -gdt
gdt_48 dw   GdtLen            ;����
       dd   LoadBaseAdr + gdt; ��ַ
;gdt ѡ����---------------------------------------------
code32_sel equ code32_des - gdt
data_sel   equ data_des - gdt
video_sel  equ video_des - gdt + SA_RPL3

;;-------------------------------------------------------

start:
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

; �õ��ڴ���
	mov	ebx, 0			; ebx = ����ֵ, ��ʼʱ��Ϊ 0
	mov	di, _MemChkBuf		; es:di ָ��һ����ַ��Χ�������ṹ��Address Range Descriptor Structure��
.MemChkLoop:
	mov	eax, 0E820h		; eax = 0000E820h
	mov	ecx, 20			; ecx = ��ַ��Χ�������ṹ�Ĵ�С
	mov	edx, 0534D4150h		; edx = 'SMAP'
	int	15h			; int 15h
	jc	.MemChkFail
	add	di, 20
	inc	dword [_dwMCRNumber]	; dwMCRNumber = ARDS �ĸ���
	cmp	ebx, 0
	jne	.MemChkLoop
	jmp	.MemChkOK
.MemChkFail:
	mov	dword [_dwMCRNumber], 0
.MemChkOK:

	;jmp $

	call SetupGdt

; ���ж�
	cli

; �򿪵�ַ��A20
	in	al, 92h
	or	al, 00000010b
	out	92h, al

; ׼���л�������ģʽ
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

; �������뱣��ģʽ
	jmp	dword code32_sel:(LoadBaseAdr+start32)





SetupGdt:
    lgdt [gdt_48]
    ret

SetupIdt:
    nop
    ret





;;32λģʽ----------------------
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

	push szMemChkTitle
	call DispStr
	add esp, 4

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


;;;;;;�����ں�--------------------------
	jmp dword code32_sel:KernelEntryPoint
;;;;;;----------------------------------

	jmp $



; ------------------------------------------------------------------------
; ��ʾ AL �е�����
; ------------------------------------------------------------------------
DispAL:
	push	ecx
	push	edx
	push	edi

	mov	edi, [dwDispPos]

	mov	ah, 0Fh			; 0000b: �ڵ�    1111b: ����
	mov	dl, al
	shr	al, 4
	mov	ecx, 2
.begin:
	and	al, 01111b
	cmp	al, 9
	ja	.1
	add	al, '0'
	jmp	.2
.1:
	sub	al, 0Ah
	add	al, 'A'
.2:
	mov	[gs:edi], ax
	add	edi, 2

	mov	al, dl
	loop	.begin
	;add	edi, 2

	mov	[dwDispPos], edi

	pop	edi
	pop	edx
	pop	ecx

	ret
; DispAL ����-------------------------------------------------------------


; ------------------------------------------------------------------------
; ��ʾһ��������
; ------------------------------------------------------------------------
DispInt:
	mov	eax, [esp + 4]
	shr	eax, 24
	call	DispAL

	mov	eax, [esp + 4]
	shr	eax, 16
	call	DispAL

	mov	eax, [esp + 4]
	shr	eax, 8
	call	DispAL

	mov	eax, [esp + 4]
	call	DispAL

	mov	ah, 07h			; 0000b: �ڵ�    0111b: ����
	mov	al, 'h'
	push	edi
	mov	edi, [dwDispPos]
	mov	[gs:edi], ax
	add	edi, 4
	mov	[dwDispPos], edi
	pop	edi
	ret
; DispInt ����------------------------------------------------------------

; ------------------------------------------------------------------------
; ��ʾһ���ַ���
; ------------------------------------------------------------------------
DispStr:
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi

	mov	esi, [ebp + 8]	; pszInfo
	mov	edi, [dwDispPos]
	mov	ah, 0Fh
.1:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0Ah	; �ǻس���?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1

.2:
	mov	[dwDispPos], edi

	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret
; DispStr ����------------------------------------------------------------

; ------------------------------------------------------------------------
; ����
; ------------------------------------------------------------------------
DispReturn:
	push	szReturn
	call	DispStr			;printf("\n");
	add	esp, 4

	ret
; DispReturn ����---------------------------------------------------------


; ------------------------------------------------------------------------
; �ڴ濽������ memcpy
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
	cmp	ecx, 0		; �жϼ�����
	jz	.2		; ������Ϊ��ʱ����

	mov	al, [ds:esi]		; ��
	inc	esi			; ��
					; �� ���ֽ��ƶ�
	mov	byte [es:edi], al	; ��
	inc	edi			; ��

	dec	ecx		; ��������һ
	jmp	.1		; ѭ��
.2:
	mov	eax, [ebp + 8]	; ����ֵ

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; ��������������
; MemCpy ����-------------------------------------------------------------


; ��ʾ�ڴ���Ϣ --------------------------------------------------------------
DispMemInfo:
	push	esi
	push	edi
	push	ecx

	mov	esi, MemChkBuf
	mov	ecx, [dwMCRNumber]	;for(int i=0;i<[MCRNumber];i++) // ÿ�εõ�һ��ARDS(Address Range Descriptor Structure)�ṹ
.loop:					;{
	mov	edx, 5			;	for(int j=0;j<5;j++)	// ÿ�εõ�һ��ARDS�еĳ�Ա����5����Ա
	mov	edi, ARDStruct		;	{			// ������ʾ��BaseAddrLow��BaseAddrHigh��LengthLow��LengthHigh��Type
.1:					;
	push	dword [esi]		;
	call	DispInt			;		DispInt(MemChkBuf[j*4]); // ��ʾһ����Ա
	pop	eax			;
	stosd				;		ARDStruct[j*4] = MemChkBuf[j*4];
	add	esi, 4			;
	dec	edx			;
	cmp	edx, 0			;
	jnz	.1			;	}
	call	DispReturn		;	printf("\n");
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
	call	DispReturn		;printf("\n");
	push	szRAMSize		;
	call	DispStr			;printf("RAM size:");
	add	esp, 4			;
					;
	push	dword [dwMemSize]	;
	call	DispInt			;DispInt(MemSize);
	add	esp, 4			;

	pop	ecx
	pop	edi
	pop	esi
	ret
; ---------------------------------------------------------------------------

; ������ҳ���� --------------------------------------------------------------
SetupPaging:
	; �����ڴ��С����Ӧ��ʼ������PDE�Լ�����ҳ��
	xor	edx, edx
	mov	eax, [dwMemSize]
	mov	ebx, 400000h	; 400000h = 4M = 4096 * 1024, һ��ҳ���Ӧ���ڴ��С
	div	ebx
	mov	ecx, eax	; ��ʱ ecx Ϊҳ��ĸ�����Ҳ�� PDE Ӧ�õĸ���
	test	edx, edx
	jz	.no_remainder
	inc	ecx		; ���������Ϊ 0 ��������һ��ҳ��
.no_remainder:
	push	ecx		; �ݴ�ҳ�����

	; Ϊ�򻯴���, �������Ե�ַ��Ӧ��ȵ������ַ. ���Ҳ������ڴ�ն�.

	; ���ȳ�ʼ��ҳĿ¼
	mov	ax, data_sel
	mov	es, ax
	mov	edi, PageDirBase	; �˶��׵�ַΪ PageDirBase
	xor	eax, eax
	mov	eax, PageTblBase | PG_P  | PG_USU | PG_RWW
.1:
	stosd
	add	eax, 4096		; Ϊ�˼�, ����ҳ�����ڴ�����������.
	loop	.1

	; �ٳ�ʼ������ҳ��
	pop	eax			; ҳ�����
	mov	ebx, 1024		; ÿ��ҳ�� 1024 �� PTE
	mul	ebx
	mov	ecx, eax		; PTE���� = ҳ����� * 1024
	mov	edi, PageTblBase	; �˶��׵�ַΪ PageTblBase
	xor	eax, eax
	mov	eax, PG_P  | PG_USU | PG_RWW
.2:
	stosd
	add	eax, 4096		; ÿһҳָ�� 4K �Ŀռ�
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
; �� KERNEL.BIN �����ݾ�����������ŵ��µ�λ��
; --------------------------------------------------------------------------------------------
InitKernel:	; ����ÿһ�� Program Header������ Program Header �е���Ϣ��ȷ����ʲô�Ž��ڴ棬�ŵ�ʲôλ�ã��Լ��Ŷ��١�
	xor	esi, esi
	mov	cx, word [BaseOfKernelFilePhyAddr + 2Ch]; �� ecx <- pELFHdr->e_phnum
	movzx	ecx, cx					; ��
	mov	esi, [BaseOfKernelFilePhyAddr + 1Ch]	; esi <- pELFHdr->e_phoff
	add	esi, BaseOfKernelFilePhyAddr		; esi <- OffsetOfKernel + pELFHdr->e_phoff
.Begin:
	mov	eax, [esi + 0]
	cmp	eax, 0				; PT_NULL
	jz	.NoAction
	push	dword [esi + 010h]		; size	��
	mov	eax, [esi + 04h]		;	��
	add	eax, BaseOfKernelFilePhyAddr	;	�� ::memcpy(	(void*)(pPHdr->p_vaddr),
	push	eax				; src	��		uchCode + pPHdr->p_offset,
	push	dword [esi + 08h]		; dst	��		pPHdr->p_filesz;
	call	MemCpy				;	��
	add	esp, 12				;	��
.NoAction:
	add	esi, 020h			; esi += pELFHdr->e_phentsize
	dec	ecx
	jnz	.Begin

	ret
; InitKernel ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

[section .data1]
align 32
DATA:

;;;data---------------------------
LoadMessage:	dd "Now Loading...."
LmsgLen equ $-LoadMessage
;;--------------------------------

;;�ַ���
_szMemChkTitle:			db	"BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0
_szRAMSize:			db	"RAM size:", 0
_szReturn:			db	0Ah, 0
;;����
_dwMCRNumber:			dd	0	; Memory Check Result
_dwDispPos:			dd	(80 * 6 + 0) * 2	; ��Ļ�� 6 ��, �� 0 �С�
_dwMemSize:			dd	0
_ARDStruct:			; Address Range Descriptor Structure
	_dwBaseAddrLow:		dd	0
	_dwBaseAddrHigh:	dd	0
	_dwLengthLow:		dd	0
	_dwLengthHigh:		dd	0
	_dwType:		dd	0
_MemChkBuf:	times	256	db	0
;

;; ����ģʽ��ʹ����Щ����
szMemChkTitle		equ	LoadBaseAdr + _szMemChkTitle
szRAMSize		equ	LoadBaseAdr + _szRAMSize
szReturn		equ	LoadBaseAdr + _szReturn
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

; ��ջ�������ݶε�ĩβ
StackSpace:	times	1000h	db	0
TopOfStack	equ	LoadBaseAdr + $	; ջ��
; SECTION .data1 S����






