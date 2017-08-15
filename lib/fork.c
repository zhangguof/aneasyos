#include "MyOs.h"

int fork()
{
    //milli_delay(0);
    //printf("my pid is %d\n",p_proc_ready-proc_table); //??为什么不加这句就错了啦。。。。-_-
    MESSAGE msg;
    msg.type = FORK;
    send_recv(BOTH, TASK_MM, &msg);
    //printf("the pid is:%d\n",msg.PID);
    //printf("msg type:%d\n",msg.type);
    assert(msg.type == SYSCALL_RET);
    assert(msg.RETVAL == 0);

    return msg.PID;
}
