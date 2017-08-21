#ifndef _global_h
#define _global_h

extern  multiboot_info_t mbi;

extern int			disp_pos;
extern u8			gdt_ptr[6];	     // 0~15:Limit  16~47:Base
extern DESCRIPTOR	gdt[GDT_SIZE];
extern u8			 idt_ptr[6];	// 0~15:Limit  16~47:Base
extern GATE		idt[IDT_SIZE];

extern PROCESS     proc_table[];
extern u8          task_stack[];
extern TSS		tss;
extern PROCESS*	p_proc_ready;

extern u32         k_reenter;

extern TASK task_table[];
extern TASK user_proc_table[];
extern irq_handler irq_table[];
extern system_call sys_call_table[]; //系统调用句柄

//tty and console
extern TTY       tty_table[];   //一个tty对应一个console
extern CONSOLE   console_table[];
extern int nr_current_console;

extern int ticks;


/* MM */
extern	MESSAGE			mm_msg;
extern	u8 *			mmbuf;
extern	const int		MMBUF_SIZE;
extern	int			    memory_size;



#endif
