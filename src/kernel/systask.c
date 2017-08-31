#include "MyOs.h"

void do_sleep_tick(int pid,int tick)
{
    PROCESS* p_proc = proc_table + pid;
    //p_proc->p_flags |= PROC_SLEEP;
    SET_FLAG(p_proc->p_flags,PROC_SLEEP);
    p_proc->wake_up_ticks = ticks + tick;
}

int do_clock()
{
    return ticks * 1000 / HZ;
}

u32 time()
{
    return timex();
    //TODO: 使用同步消息
    // MESSAGE msg;
    // msg.type = GETTIME;
    // send_recv(BOTH,TASK_SYS,&msg);
    // return msg.RETVAL;
}

void sleep(int ms)
{

    sleepx(ms);
}


void task_sys()
{
    printf("The task_sys begin ticks:%d\n",get_ticks());
    MESSAGE msg;
    while(1)
    {
        send_rec(RECEIVE, ANY, &msg);
        int src = msg.source;
        switch(msg.type)
        {
        case GET_TICKS:
            msg.RETVAL = ticks;
            send_recv(SEND, src, &msg);
            break;
		case GET_PID:
			msg.type = SYSCALL_RET;
			msg.PID = src;
			send_recv(SEND, src, &msg);
			break;
        case MSGSLEEP:
            //msg.type = SYSCALL_RET;
            //double sec = 1.5;
            //msg.m3.m3l1 = *(u64*)(&sec);
            //double *sec = *((double*)&msg.m3.m3l1);
            //printf("sleep:%d\n", msg.u.m3.m3i1*HZ/1000);
            do_sleep_tick(src, msg.u.m3.m3i1*HZ/1000);
            break;
        case GETTIME:
            msg.type = SYSCALL_RET;
            msg.RETVAL = unix_time;
            send_recv(SEND,src,&msg);
            break;
        default:
            printf("msg_type:%d\n", msg.type);
            panic("[task_sys]unknow msg type");
            break;

        }
    }
}
