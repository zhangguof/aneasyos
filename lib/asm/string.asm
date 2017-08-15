
; -------------------------------------------------------
;                              memcpy.asm
; �ڴ渴�ƺ���
;----------------------------------------------------------

[SECTION .text]

; ��������
global	memcpy
global  memset
global  strcpy
global  strlen


; ------------------------------------------------------------------------
; void* memcpy(void* es:p_dst, void* ds:p_src, int size);
; ------------------------------------------------------------------------
memcpy:
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
; memcpy ����-------------------------------------------------------------

; ------------------------------------------------------------------------
; void memset(void* p_dst, char ch, int size);
; ------------------------------------------------------------------------
memset:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination
	mov	edx, [ebp + 12]	; Char to be putted
	mov	ecx, [ebp + 16]	; Counter
.1:
	cmp	ecx, 0		; �жϼ�����
	jz	.2		; ������Ϊ��ʱ����

	mov	byte [edi], dl		; ��
	inc	edi			; ��

	dec	ecx		; ��������һ
	jmp	.1		; ѭ��
.2:

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; ��������������
; ------------------------------------------------------------------------


; ------------------------------------------------------------------------
; char* strcpy(char* p_dst, char* p_src);
; ------------------------------------------------------------------------
strcpy:
	push    ebp
	mov     ebp, esp

	mov     esi, [ebp + 12] ; Source
	mov     edi, [ebp + 8]  ; Destination

.1:
	mov     al, [esi]               ; ��
	inc     esi                     ; ��
					; �� ���ֽ��ƶ�
	mov     byte [edi], al          ; ��
	inc     edi                     ; ��

	cmp     al, 0           ; �Ƿ����� '\0'
	jnz     .1              ; û�����ͼ���ѭ���������ͽ���

	mov     eax, [ebp + 8]  ; ����ֵ

	pop     ebp
	ret                     ; ��������������
; strcpy ����-------------------------------------------------------------

; ------------------------------------------------------------------------
; int strlen(char* p_str);
; ------------------------------------------------------------------------
strlen:
        push    ebp
        mov     ebp, esp

        mov     eax, 0                  ; �ַ������ȿ�ʼ�� 0
        mov     esi, [ebp + 8]          ; esi ָ���׵�ַ

.1:
        cmp     byte [esi], 0           ; �� esi ָ����ַ��Ƿ��� '\0'
        jz      .2                      ; ����� '\0'���������
        inc     esi                     ; ������� '\0'��esi ָ����һ���ַ�
        inc     eax                     ;         ���ң�eax �Լ�һ
        jmp     .1                      ; ���ѭ��

.2:
        pop     ebp
        ret                             ; ��������������
; ------------------------------------------------------------------------
