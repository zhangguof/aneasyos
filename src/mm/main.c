#include "MyOs.h"
//#include "multiboot.h"

 void do_fork_test();

static void init_mm();

/*****************************************************************************
 *                                task_mm
 *****************************************************************************/
/**
 * <Ring 1> The main loop of TASK MM.
 *
 *****************************************************************************/
 void task_mm()
{
	printf("init the task_mm ticks:%d\n",get_ticks());

	init_mm();
    printf("The task_mm begin\n");
	while (1) {
		send_recv(RECEIVE, ANY, &mm_msg);
		int src = mm_msg.source;
		int reply = 1;

		int msgtype = mm_msg.type;

		printf("mm get message %d\n",msgtype);
		switch (msgtype) {
		case FORK:
			mm_msg.RETVAL = do_fork();
			break;
		case EXIT:
			do_exit(mm_msg.STATUS);
			reply = 0;
			break;
		/* case EXEC: */
		/* 	mm_msg.RETVAL = do_exec(); */
		/* 	break; */
		case WAIT:
			do_wait();
			reply = 0;
			break;
		default:
			dump_msg("MM::unknown msg", &mm_msg);
			assert(0);
		    break;
		}

		if (reply) {
			mm_msg.type = SYSCALL_RET;
			send_recv(SEND, src, &mm_msg);
		}
	}
}

/*****************************************************************************
 *                                init_mm
 *****************************************************************************/
/**
 * Do some initialization work.
 *
 *****************************************************************************/
static void init_mm()
{
#ifndef MULTIBOOT
	struct boot_params bp;
	get_boot_params(&bp);
	memory_size = bp.mem_size;
#else
    memory_size = mbi.mem_upper * 1024;
#endif
	printf("{MM} memsize:%dMB\n", memory_size / (1024 * 1024));
}

/*****************************************************************************
 *                                alloc_mem
 *****************************************************************************/
/**
 * Allocate a memory block for a proc.
 *
 * @param pid  Which proc the memory is for.
 * @param memsize  How many bytes is needed.
 *
 * @return  The base of the memory just allocated.
 *****************************************************************************/
 int alloc_mem(int pid, int memsize)
{
	assert(pid >= (NR_TASKS + NR_NATIVE_PROCS));
	if (memsize > PROC_IMAGE_SIZE_DEFAULT) {
		panic("unsupported memory request: %d. "
		      "(should be less than %d)",
		      memsize,
		      PROC_IMAGE_SIZE_DEFAULT);
	}

	int base = PROCS_BASE +
		(pid - (NR_TASKS + NR_NATIVE_PROCS)) * PROC_IMAGE_SIZE_DEFAULT;

	if (base + memsize >= memory_size)
		panic("memory allocation failed. pid:%d", pid);

	return base;
}

/*****************************************************************************
 *                                free_mem
 *****************************************************************************/
/**
 * Free a memory block. Because a memory block is corresponding with a PID, so
 * we don't need to really `free' anything. In another word, a memory block is
 * dedicated to one and only one PID, no matter what proc actually uses this
 * PID.
 *
 * @param pid  Whose memory is to be freed.
 *
 * @return  Zero if success.
 *****************************************************************************/
 int free_mem(int pid)
{
	return 0;
}
