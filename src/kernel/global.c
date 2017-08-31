/*
*global.c 各种全局数据
×
*/
#include "MyOs.h"


multiboot_info_t mbi;

int         disp_pos;
u8			gdt_ptr[6];	// 0~15:Limit  16~47:Base
DESCRIPTOR	gdt[GDT_SIZE];
u8			 idt_ptr[6];	// 0~15:Limit  16~47:Base
GATE		idt[IDT_SIZE];

PROCESS     proc_table[NR_TASKS + NR_PROCS];
u8          task_stack[STACK_SIZE_TOTAL];
TSS		    tss;
PROCESS*	p_proc_ready;
u32         k_reenter;


TASK task_table[NR_TASKS] = {{task_tty,STACK_SIZE_TTY,"tty"},
                             {task_sys,STACK_SIZE_SYS,"sys"},
                             {task_hd, STACK_SIZE_HD, "HD"},
                             {task_fs, STACK_SIZE_FS,  "FS"},
                             {task_mm, STACK_SIZE_MM,  "MM"},
                         	};
TASK user_proc_table[NR_PROCS]={{Init,   STACK_SIZE_INIT, "INIT"},
                                {testA, STACK_SIZE_TESTA,"testA"},
                                {testB, STACK_SIZE_TESTB,"testB"},
                                {testC, STACK_SIZE_TESTC,"testC"},
                            	};





//TASK task_table[NR_TASKS] = {testA, STACK_SIZE_TESTA,"testA"};

irq_handler irq_table[NR_IRQ];   //iqr处理句柄

 //系统调用句柄
system_call sys_call_table[]={sys_get_ticks,
                              sys_write,
                              sys_sendrec,
                              sys_printx,
                              sys_sleep,
                              sys_get_time,
                              };


//tty and console
TTY       tty_table[NR_CONSOLES];   //一个tty对应一个console
CONSOLE   console_table[NR_CONSOLES];
int nr_current_console;

int ticks;
int kernel_ticks;
u32 unix_time; //sec from 1970-01-01 00:00:00


u8 *		mmbuf		= (u8*)0x700000;
const int	MMBUF_SIZE	= 0x100000;
int			memory_size;
MESSAGE			mm_msg;
