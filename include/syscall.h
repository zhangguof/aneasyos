#ifndef _syscall_h
#define _syscall_h

//ϵͳ���õ�һЩ����ԭ��

//syscall.c
int sys_get_ticks();  //�ڲ�ʵ��
int sys_write(char *buf,int len,  int _unused,PROCESS* p_proc);
int sys_sendrec(int function, int src_dest, MESSAGE* m, proc* p_proc);
int sys_printx(int _unused1, int _unused2, char* s,  proc* p_proc);


//syscall.asm
void sys_call();  //�ж����

int get_ticks();  //�ⲿ�ӿ�
void write(char *buf, int len);
int sendrec(int function, int src_dest, MESSAGE* m);
void printx(char* s);





#endif
