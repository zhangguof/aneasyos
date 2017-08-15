#include "MyOs.h"

void task_sys()
{
    //printf("The task_sys begin\n");
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
            panic("unknow msg type");
            break;

        }
    }
}
