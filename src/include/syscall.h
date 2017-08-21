#ifndef _syscall_h
#define _syscall_h

//系统调用的一些函数原型

//syscall.c
int sys_get_ticks();  //内部实现
int sys_write(char *buf,int len,  int _unused,PROCESS* p_proc);
int sys_sendrec(int function, int src_dest, MESSAGE* m, proc* p_proc);
int sys_printx(int _unused1, int _unused2, char* s,  proc* p_proc);


//syscall.asm
void sys_call();  //中断入口

int get_ticks();  //外部接口
void write(char *buf, int len);
int sendrec(int function, int src_dest, MESSAGE* m);
void printx(char* s);





#endif
