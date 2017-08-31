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


int sys_sleep(int ms,int _unused1,int _unused2,PROCESS* p_proc)
{
	int tick = ms * HZ/1000;
    SET_FLAG(p_proc->p_flags,PROC_SLEEP);
    p_proc->wake_up_ticks = ticks + tick;
    //printf("in sys_sleep:%x,%d,%d,%d\n", p_proc->p_flags,ticks,tick,p_proc->wake_up_ticks);
    schedule();
    return 0;
}

u32 sys_get_time()
{
    return unix_time;
}

