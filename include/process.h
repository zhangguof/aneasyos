/*
* process.h
*���̱�����ݽṹ pcb
*
*/
#include "type.h"

typedef struct s_stackframe {	/* proc_ptr points here				�� Low			*/
	u32	gs;		/* ��						��			*/
	u32	fs;		/* ��						��			*/
	u32	es;		/* ��						��			*/
	u32	ds;		/* ��						��			*/
	u32	edi;		/* ��						��			*/
	u32	esi;		/* �� pushed by save()				��			*/
	u32	ebp;		/* ��						��			*/
	u32	kernel_esp;	/* <- 'popad' will ignore it			��			*/
	u32	ebx;		/* ��						��ջ�Ӹߵ�ַ���͵�ַ����*/
	u32	edx;		/* ��						��			*/
	u32	ecx;		/* ��						��			*/
	u32	eax;		/* ��						��			*/
	u32	retaddr;	/* return address for assembly code save()	��			*/
	u32	eip;		/*  ��						��			*/
	u32	cs;		/*  ��						��			*/
	u32	eflags;		/*  �� these are pushed by CPU during interrupt	��			*/
	u32	esp;		/*  ��						��			*/
	u32	ss;		/*  ��						��High			*/
}STACK_FRAME;


typedef struct s_proc {
	STACK_FRAME regs;          /* process registers saved in stack frame */

	u16 ldt_sel;               /* gdt selector giving ldt base and limit */

	//������������
	DESCRIPTOR ldts[LDT_SIZE]; /* local descriptors for code and data */

    int ticks;                 /* remained ticks */
    int priority;

	u32 pid;                   /* process id passed in from MM */
	char p_name[16];           /* name of the process */


	int  p_flags;   /**
				    * process flags.
				    * A proc is runnable iff p_flags==0
				    */
    MESSAGE * p_msg;  //ָ����Ϣ���ָ��
	int p_recvfrom;   //��Ҫ��˭���������Ϣ
	int p_sendto;     //��Ҫ������Ϣ�Ķ���

	int has_int_msg;/**
				    * nonzero if an INTERRUPT occurred when
				    * the task is not ready to deal with it.
				    */

	struct proc * q_sending;   /**
				    * queue of procs sending messages to
				    * this proc
				    */
	struct proc * next_sending;/**
				    * next proc in the sending
				    * queue (q_sending)
				    */
    int p_parent; /**< pid of parent process */

	int exit_status; /**< for parent */
	int runtime;   //��������ʱ�� ms

	int nr_tty;  //���̶�Ӧ��tty
}PROCESS;

typedef PROCESS proc;

typedef struct s_task {
	task_f	initial_eip;
	int	stacksize;
	char	name[32];
}TASK;

/**
 * All forked proc will use memory above PROCS_BASE.
 *
 * @attention make sure PROCS_BASE is higher than any buffers, such as
 *            fsbuf, mmbuf, etc
 * @see global.c
 * @see global.h
 */
#define	PROCS_BASE		0xA00000 /* 10 MB */
#define	PROC_IMAGE_SIZE_DEFAULT	0x200000 /*  2 MB */
#define	PROC_ORIGIN_STACK	0x400    /*  1 KB */


#define proc2pid(x) (x - proc_table)

/* Number of tasks & procs */
#define NR_TASKS	5
#define NR_PROCS	32
#define NR_NATIVE_PROCS 4

#define FIRST_PROC	proc_table[0]
#define LAST_PROC	proc_table[NR_TASKS + NR_PROCS - 1]


/* stacks of tasks */
#define	STACK_SIZE_DEFAULT	0x4000 /* 16 KB */
#define STACK_SIZE_TTY		STACK_SIZE_DEFAULT
#define STACK_SIZE_SYS		STACK_SIZE_DEFAULT
#define STACK_SIZE_HD		STACK_SIZE_DEFAULT
#define STACK_SIZE_FS		STACK_SIZE_DEFAULT
#define STACK_SIZE_MM		STACK_SIZE_DEFAULT
#define STACK_SIZE_INIT		STACK_SIZE_DEFAULT
#define STACK_SIZE_TESTA	STACK_SIZE_DEFAULT
#define STACK_SIZE_TESTB	STACK_SIZE_DEFAULT
#define STACK_SIZE_TESTC	STACK_SIZE_DEFAULT

#define STACK_SIZE_TOTAL	(STACK_SIZE_TTY + \
				STACK_SIZE_SYS + \
				STACK_SIZE_HD + \
				STACK_SIZE_FS + \
				STACK_SIZE_MM + \
				STACK_SIZE_INIT + \
				STACK_SIZE_TESTA + \
				STACK_SIZE_TESTB + \
				STACK_SIZE_TESTC)

