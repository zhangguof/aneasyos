/*
*syscall.c
*系统调用的c实现
*
*/

#include "MyOs.h"

int sys_get_ticks()
{
    //disp_str("+");
    return ticks;
}

int sys_write(char* buf, int len, int _unused,PROCESS* p_proc)
{
    tty_write(&tty_table[p_proc->nr_tty],buf,len);
    return 0;
}

