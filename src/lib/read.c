#include "MyOs.h"

int read(char *buf)
{
    MESSAGE msg;
    msg.type = READ;
    msg.BUF = buf;
    send_recv(BOTH, TASK_FS, &msg);
    assert(msg.type == SYSCALL_RET);
    //assert(msg.RETVAL == 0);

    return msg.RETVAL;
}
