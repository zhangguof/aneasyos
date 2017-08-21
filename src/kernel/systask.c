#include "MyOs.h"

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
        default:
            printf("msg_type:%d\n", msg.type);
            panic("[task_sys]unknow msg type");
            break;

        }
    }
}
