sys_call;;系统调用的汇编接口


%include "sconst.inc"

_NR_get_ticks   equ 0; 与global.c 中的sys_call_table对应
_NR_write       equ 1;
_NR_sendrec     equ 2;
_NR_printx      equ 3;
_NR_sleep       equ 4;
_NR_time       equ 5;

INT_VECTOR_SYS_CALL equ 0x90  ;系统中断

;;导出符合
global get_ticks
global write
global sendrec
global printx
global sleepx
global timex

%macro    before_int 0
          push ebp
          mov ebp, esp
          sub esp, 4; esp-4 = ret
          pusha
%endmacro

%macro    after_int 0
          mov [ebp-4],eax ;save ret value
          popa
          mov eax, [ebp-4]
          add esp, 4
          leave
%endmacro


[section .text]
[bits 32]
get_ticks:
          before_int
          mov eax, _NR_get_ticks
          int INT_VECTOR_SYS_CALL
          after_int
          ret

write:
          before_int
          mov eax, _NR_write
          mov ebx, [ebp+8] ;arg1
          mov ecx, [ebp+12];arg2
          int INT_VECTOR_SYS_CALL
          after_int
          ret
sendrec:
          before_int

          mov eax, _NR_sendrec
          mov ebx, [ebp+8]  ;function,arg1
          mov ecx, [ebp+12]  ;src_dest
          mov edx, [ebp+16] ;p_msg
          int INT_VECTOR_SYS_CALL;return by eax

          after_int

          ret
;void printx(char* s);
printx:
          before_int
          mov eax,_NR_printx
          mov edx,[ebp+8] ; arg1
          int INT_VECTOR_SYS_CALL
          after_int
          ret
sleepx:
          before_int
          mov eax, _NR_sleep
          mov ebx, [ebp+8];arg1=int time
          int INT_VECTOR_SYS_CALL
          after_int
          ret

timex:
          before_int
          mov eax, _NR_time
          int INT_VECTOR_SYS_CALL
          after_int
          ret 







