#ifndef _sched_h
#define _sched_h

void init_sched();//初始化程序，包括进程表 8253等

//void seched(); //进出调度函数
void schedule();
void* va2la(int pid, void* va);
//int msg_send(proc* current, int dest, MESSAGE* m);
//int msg_recevie(proc* current, int src, MESSAGE* m);

int	 send_recv(int function, int src_dest, MESSAGE* msg);
#define send_rec send_recv
void inform_int(int task_nr);

#endif


