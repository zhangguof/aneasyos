
/**************
*klib.h
**************/
/*kliba.asm */
/*klib.c*/

#ifndef _klib_h
#define _klib_h

//klib.c kilb.asm
void out_byte(unsigned port,int value);
unsigned short in_byte(unsigned int port);
void	port_read(u16 port, void* buf, int n);
void	port_write(u16 port, void* buf, int n);

void disp_str(char * info);
void disp_color_str(char * info, int color);
void get_boot_params(struct boot_params * pbp);

int getpid();



void init_prot();
//8259中断使能
void disable_irq(int i);
void enable_irq(int i);
//cpu中断使能 sti cli
void disable_int();
void enable_int();

void delay(int time);

//main.c
//一些任务或进程原型
extern void Init();
extern void testA();
extern void testB();
extern void testC();

extern void task_tty();
extern void task_sys();
extern void task_hd();
extern void task_fs();
extern void task_mm();

u32 seg2phy(u16 seg);


//trapsa.asm
//中断句柄
void clock_handler(int irq);

//keyboard.c
void keyboard_handler(int irq);
void init_keyboard();

//hd.c
void hd_handler(int irq);

//clock.c
void milli_delay(int milli_sec);


//printf
int printf(const char* fmt,...);
int vsprintf(char *buf,const char* fmt, va_list args);
#define printl printf

/* lib/misc.c */
void spin(char * func_name);
void panic(const char* fmt,...);

//lib
int read(char *buf);

//mm
 int		do_fork();
 void		do_exit(int status);
 void		do_wait();

#endif
