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

[section .text]
[bits 32]
get_ticks:
          mov eax, _NR_get_ticks
          int INT_VECTOR_SYS_CALL
          ret

write:
          mov eax, _NR_write
          mov ebx, [esp+4]
          mov ecx, [esp+8]
          int INT_VECTOR_SYS_CALL
          ret
sendrec:
          mov eax, _NR_sendrec
          mov ebx, [esp+4]  ;function
          mov ecx, [esp+8]  ;src_dest
          mov edx, [esp+12] ;p_msg
          int INT_VECTOR_SYS_CALL
          ret
;void printx(char* s);
printx:
          mov eax,_NR_printx
          mov edx,[esp+4]
          int INT_VECTOR_SYS_CALL
          ret
sleepx:
          mov eax, _NR_sleep
          mov ebx, [esp+4];arg=int time
          int INT_VECTOR_SYS_CALL
          ret

timex:
          mov eax, _NR_time
          int INT_VECTOR_SYS_CALL
          ret 







