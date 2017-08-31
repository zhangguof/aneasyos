#include "MyOs.h"


void init_sched()
{

        //初始化进程表
    TASK*    p_task = task_table;
    PROCESS* p_proc = proc_table;
    char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
    u16     sel_ldt = sel_ldt_first;

    u8 privilege;
    u8 rpl;
    int eflags;
    int i;
    int   prio;


    for(i=0; i<NR_TASKS + NR_PROCS;i++,p_proc++,p_task++)
    {
        p_proc->wake_up_ticks = 0;
        if(i >= NR_TASKS + NR_NATIVE_PROCS)
        {
            p_proc->p_flags = FREE_SLOT;
            continue;
        }

        if(i<NR_TASKS) //任务
        {
            p_task = task_table + i;
            privilege = PRIVILEGE_TASK;	/* Fill the LDT descriptors of each proc in GDT  */
            rpl       = 1;  //RPL_TASK=1
            eflags    = 0x1202; //if=1 iopl=1
            prio      = 15;
        }
        else
        {
            p_task = user_proc_table + (i-NR_TASKS);
            privilege = PRIVILEGE_USER;
            rpl       =  3;   //rpl_USE=3
            eflags   = 0x202; //if=1
            prio      = 5;
        }
        strcpy(p_proc->p_name, p_task->name); //task name
        p_proc->p_parent = NO_TASK;

        if(strcmp(p_task->name,"INIT") != 0)
        {
            p_proc->ldts[INDEX_LDT_C] = gdt[SEL_KERNEL_CS >>3];
            p_proc->ldts[INDEX_LDT_RW] = gdt[SEL_KERNEL_DS>>3];

            p_proc->ldts[INDEX_LDT_C].attr1 = DA_C | privilege<<5 ;//改变dpl
            p_proc->ldts[INDEX_LDT_RW].attr1 = DA_DRW | privilege<<5;
        }
        else
        {
        #ifndef MULTIBOOT
            unsigned int k_base = 0x30000;   //fix me when in mulitboot....
        #else
            unsigned int k_base = 0x100000;   //fix me when in mulitboot....
        #endif
            unsigned int k_limit = 0x7ffff;  //64kb NOTE! 0.5m

            init_descriptor(&p_proc->ldts[INDEX_LDT_C],
                      0,
                      (k_base + k_limit) >> LIMIT_4K_SHIFT,
                      DA_32 | DA_LIMIT_4K | DA_C | privilege<< 5);

            init_descriptor(&p_proc->ldts[INDEX_LDT_RW],
                      0,
                      (k_base + k_limit) >> LIMIT_4K_SHIFT,
                      DA_32 | DA_LIMIT_4K | DA_DRW | privilege << 5);

        }

        p_proc->pid = i;   //pid
        // p_proc->ldt_sel = sel_ldt;
        // memcpy(&p_proc->ldts[0], &gdt[SEL_KERNEL_CS>>3], sizeof(DESCRIPTOR));
        // p_proc->ldts[0].attr1 = DA_C | privilege<<5 ;//改变dpl

        // memcpy(&p_proc->ldts[1], &gdt[SEL_KERNEL_DS>>3], sizeof(DESCRIPTOR));
        // p_proc->ldts[1].attr1 = DA_DRW | privilege<<5;

        p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.gs = (SEL_KERNEL_GS & SA_RPL_MASK)  | rpl;


        p_proc->regs.eip= (u32)p_task->initial_eip;
        p_proc->regs.esp= (u32)p_task_stack;
        p_proc->regs.eflags= eflags;

        p_task_stack -= p_task->stacksize;

        //绑定tty0
        p_proc->nr_tty = 0;
        p_proc->runtime = 0;

        p_proc->p_flags = READY;
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;
        p_proc->ticks = p_proc->priority = prio;

        //p_proc++;
        //p_task++;
        sel_ldt += (1<<3);
    }
    
    //设置时钟中断
    put_irq_hangler(CLOCK_IRQ, clock_handler);
    enable_irq(CLOCK_IRQ);  //接受时钟中断


    //初始化8253
    out_byte(TIMER_MODE, RATE_GENERATOR);
    //out_byte(TIMER0, (u8)(((TIMER_FREQ/HZ))&0xff));
    //out_byte(TIMER0, (u8)(((TIMER_FREQ/HZ) >> 8)&0xff));

    out_byte(TIMER0, (u8)(TIMER_FREQ/HZ));
    out_byte(TIMER0, (u8)((TIMER_FREQ/HZ) >> 8));
    //printf("set timer0:%x,%x\n", (TIMER_FREQ/HZ),(TIMER_FREQ/HZ) >> 8);
    //printf("set timer02:%x,%x\n", (((TIMER_FREQ/HZ))&0xff),(((TIMER_FREQ/HZ) >> 8)&0xff));




    //初始化进程优先级
//    proc_table[0].ticks=proc_table[0].priority=20;
//    proc_table[1].ticks=proc_table[1].priority=20;
//
//    proc_table[2].ticks=proc_table[2].priority=10;
//    proc_table[3].ticks=proc_table[3].priority=10;
//    proc_table[4].ticks=proc_table[4].priority=10;

    //tty设置
    proc_table[NR_TASKS + 0].nr_tty = 0;
    proc_table[NR_TASKS + 1].nr_tty = 1;
    proc_table[NR_TASKS + 2].nr_tty = 1;
    proc_table[NR_TASKS + 3].nr_tty = 2;
    proc_table[3].nr_tty = 2;
}

void schedule()
{
    PROCESS* p;
    int MaxTicks=0;
    proc* tp = p_proc_ready;
    while(!MaxTicks)  //为0
    {
        for(p=proc_table;p<proc_table+NR_TASKS + NR_PROCS;p++)
        {
            if(p->p_flags & PROC_SLEEP)
            {
                if(p->wake_up_ticks <= ticks)
                {
                    //p->p_flags &= ~PROC_SLEEP;
                    RESET_FLAG(p->p_flags,PROC_SLEEP);
                    p->wake_up_ticks = 0;
                    printf("ticks:%d\n", ticks);
                }

            }

            if(p->p_flags == READY)
            {
                if(p->ticks > MaxTicks)
                {
                    MaxTicks = p->ticks;
                    p_proc_ready = p;
                    //printf("change to:%d,name:%s\n", p->pid,p->p_name);
                }
            }


        }
        if(!MaxTicks)
        {
            for(p=proc_table; p<proc_table + NR_TASKS + NR_PROCS; p++)
            {
                if(p->p_flags == READY)
                {
                    p->ticks = p->priority;
                }
            }
        }

    }
    if(tp!=p_proc_ready)
    {
        // printf("new process:%d,pre_pid:%d,cur_ticks:%d\n", p_proc_ready->pid,tp->pid,sys_get_ticks());
        // for(p=proc_table;p<proc_table+NR_TASKS + NR_PROCS;p++)
        // {
        //     if(p->p_flags != FREE_SLOT)
        //         printf("pid:%d,ticks:%d\n", p->pid,p->ticks);
        // }
    }
}



/*****************************************************************************
 *				  ldt_seg_linear
 *****************************************************************************/
/**
 * <Ring 0~1> Calculate the linear address of a certain segment of a given
 * proc.
 *
 * @param p   Whose (the proc ptr).
 * @param idx Which (one proc has more than one segments).
 *
 * @return  The required linear address.
 *****************************************************************************/
 int ldt_seg_linear( proc* p, int idx)
{
	 descriptor * d = &p->ldts[idx];

	return d->base_high << 24 | d->base_mid << 16 | d->base_low;
}

/*****************************************************************************
 *				  va2la
 *****************************************************************************/
/**
 * <Ring 0~1> Virtual addr --> Linear addr.
 *
 * @param pid  PID of the proc whose address is to be calculated.
 * @param va   Virtual address.
 *
 * @return The linear address for the given virtual address.
 *****************************************************************************/
void* va2la(int pid, void* va)
{
	 proc* p = &proc_table[pid];

	u32 seg_base = ldt_seg_linear(p, INDEX_LDT_RW);
	u32 la = seg_base + (u32)va;

	if (pid < NR_TASKS + NR_NATIVE_PROCS) {
		assert(la == (u32)va);
	}

	return (void*)la;
}

/************************************************************
*        reset_msg
*     清空消息
*************************************************************/
void reset_msg(MESSAGE* p)
{
    memset(p,0,sizeof(MESSAGE));
}

/************************************************************
*    block
*     在p_flags设置后调用 block将调用schedule()
*************************************************************/
void block(proc* p)
{
    assert(p->p_flags);

    schedule();
}
/**********************************************************************
*       unblock
*       this a dummy routine. it do noting
**********************************************************************/
void unblock(proc* p)
{
    assert(p->p_flags == 0);
}
/*****************************************************************
*   deadlock
*   检测死锁
*    检测是否存在一个环结构 a->b->c->a
******************************************************************/
static int deadlock(int src, int dest)
{
    proc* p = proc_table + dest;
    while(1)
    {
        if(p->p_flags & SENDING)
        {
            if(p->p_sendto == src)
            {
                //deadlock print the chain
                p = proc_table + dest;
                printf("=_=%s",p->p_name);
                do
                {
                    assert(p->p_msg);
                    p=proc_table + p->p_sendto;
                    printf("->%s",p->p_name);

                }while (p != proc_table + src);
                printf("=_=");
                return 1;
            }
            p = proc_table + p->p_sendto;
        }
        else
        {
            break;
        }
    }
    return 0;
}

/**************************************************************
* 消息发送  msg_send
* @param current 调用者，sender
* @param dest  目标进程
* @param m    消息

* @return zero if success
***************************************************************/

 int msg_send(proc* current, int dest, MESSAGE* m)
{
    proc* sender = current;
    proc* p_dest = proc_table + dest;

    assert(proc2pid(sender)!=dest);
    // printf("==[msg_send]cur_pid:%d,cur_pname:%s,dest:%d,msg_type:%d\n",
    //        current->pid,current->p_name,dest,m->type);

    //检测是否有死锁
    if(deadlock(proc2pid(sender),dest))
    {
        panic(">>DEADLOCK<< %s->%s",sender->p_name,p_dest->p_name);
    }

    if( (p_dest->p_flags & RECEIVING) &&
        (p_dest->p_recvfrom == proc2pid(sender) ||
         p_dest->p_recvfrom == ANY))
    {

        assert(p_dest->p_msg);
        assert(m);

        phys_copy(va2la(dest, p_dest->p_msg),
                  va2la(proc2pid(sender),m),
                  sizeof(MESSAGE));

        p_dest->p_msg = 0;
        p_dest->p_flags &= ~RECEIVING; //dest 收到了消息
        p_dest->p_recvfrom = NO_TASK;
        //unblock(p_dest);

        assert(p_dest->p_flags == 0);
        assert(p_dest->p_msg == 0);
        assert(p_dest->p_recvfrom == NO_TASK);
        assert(p_dest->p_sendto == NO_TASK);

        assert(sender->p_flags == 0);
        assert(sender->p_msg == 0);
        assert(sender->p_recvfrom == NO_TASK);
        assert(sender->p_sendto == NO_TASK);
    }
    else    //dest 没有在等待消息
    {
        sender->p_flags |= SENDING ; //
        assert(sender->p_flags == SENDING);
        sender->p_sendto = dest;
        sender->p_msg = m;

        //加入sending 对列
        proc* p;
        if(p_dest->q_sending)
        {
            p = p_dest->q_sending;
            while(p->next_sending)
                p = p->next_sending;
            p->next_sending = sender;
        }
        else
        {
            p_dest->q_sending = sender;
        }


        sender->next_sending = 0;  //队尾

        block(sender); //阻塞sender 调用scheduler()

        assert(sender->p_flags == SENDING);
        assert(sender->p_msg != 0);
        assert(sender->p_recvfrom == NO_TASK);
        assert(sender->p_sendto == dest);
    }
    return 0;
}
/**************************************************************
*    msg_receive
*    get a message from the src proc. if src is blocked sending
*    the message, copy the message from it and unblock src.
*    otherwise the caller will be blocked.
  @param current   caller proc
  @param src       from whom the message will be recevied
  @param m         the message ptr to accept the message

  @return          zero if is sucess
**************************************************************/

 int msg_recevie(proc* current, int src, MESSAGE* m)
{
    proc* p_who_wanna_recv = current; //^_^ a long name...

    proc* p_from = 0;
    proc* prev = 0;
    int copyok = 0;

    assert(proc2pid(p_who_wanna_recv) != src);
    // printf("===[msg_recevie]cur_pid:%d,cur_pname:%s,src:%d,msg_type:%d\n", 
    //         current->pid,current->p_name,src,m->type);

//中断消息
    if( (p_who_wanna_recv->has_int_msg) &&
       ((src == ANY) || (src == INTERRUPT)))
    {
        MESSAGE msg;
        reset_msg(&msg);
        msg.source = INTERRUPT;
        msg.type = HARD_INT;
        assert(m);
        phys_copy(va2la(proc2pid(p_who_wanna_recv),m),&msg,
                  sizeof(MESSAGE));

        p_who_wanna_recv->has_int_msg = 0;

        assert(p_who_wanna_recv->p_flags == 0);
        assert(p_who_wanna_recv->p_msg == 0);
        assert(p_who_wanna_recv->p_sendto == NO_TASK);
        assert(p_who_wanna_recv->has_int_msg == 0);

        return 0;
    }


    //if no interrupt
    if(src == ANY)
    {

        if(p_who_wanna_recv->q_sending)
        {
           // panic("is ok 2");

            p_from = p_who_wanna_recv->q_sending;
            copyok = 1;

            assert(p_who_wanna_recv->p_flags == 0);
            assert(p_who_wanna_recv->p_msg == 0);
            assert(p_who_wanna_recv->p_sendto == NO_TASK);
            assert(p_who_wanna_recv->p_recvfrom ==NO_TASK);
            assert(p_who_wanna_recv->q_sending != 0);

            assert(p_from->p_flags | SENDING);
            assert(p_from->p_msg != 0);
            assert(p_from->p_recvfrom == NO_TASK);
            assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));

        }
          //panic("is ok 2");
    }
    else if(src != INTERRUPT)
    {


        p_from = &proc_table[src];
        if( (p_from->p_flags & SENDING )&&
            (p_from->p_sendto == proc2pid(p_who_wanna_recv)))
        {
            copyok = 1;
            proc* p = p_who_wanna_recv->q_sending;
            assert(p);  //p_from must have been appended to the queue

            while(p)  //在queue中查找p_from
            {
                assert(p_from->p_flags & SENDING);
                if(proc2pid(p) == src)
                {
                    p_from = p;
                    break;
                }
                prev = p;
                p = p->next_sending;
            }

            assert(p_who_wanna_recv->p_flags == 0);
            assert(p_who_wanna_recv->p_msg == 0);
            assert(p_who_wanna_recv->p_sendto == NO_TASK);
            assert(p_who_wanna_recv->p_recvfrom ==NO_TASK);
            assert(p_who_wanna_recv->q_sending != 0);

            assert(p_from->p_flags == SENDING);
            assert(p_from->p_msg != 0);
            assert(p_from->p_recvfrom == NO_TASK);
            assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));


        }
    }



    if(copyok)
    {
        if(p_from == p_who_wanna_recv->q_sending) //the 1st one
        {
            assert(prev == 0);
            p_who_wanna_recv->q_sending = p_from->next_sending;
            p_from->next_sending = 0;

            //panic("is ok 3");
        }
        else
        {
            assert(prev);
            prev->next_sending = p_from->next_sending;
            p_from->next_sending = 0;
        }

        // panic("is ok 3");
        assert(m);
        assert(p_from->p_msg);


        phys_copy(va2la(proc2pid(p_who_wanna_recv),m),
                  va2la(proc2pid(p_from),p_from->p_msg),
                  sizeof(MESSAGE));

        //panic("is ok 4");
        p_from->p_msg = 0;
        p_from->p_sendto = NO_TASK;
        p_from->p_flags &= ~SENDING;
        //unblock(p_from);

    }
    else
    {
        //nobody sending any message
        p_who_wanna_recv->p_flags |= RECEIVING;
        p_who_wanna_recv->p_msg = m;

        if(src == ANY)
            p_who_wanna_recv->p_recvfrom = ANY;
        else
            p_who_wanna_recv->p_recvfrom = src;
        block(p_who_wanna_recv);

        assert(p_who_wanna_recv->p_flags == RECEIVING);
        assert(p_who_wanna_recv->p_msg !=0 );
        assert(p_who_wanna_recv->p_sendto == NO_TASK);
        assert(p_who_wanna_recv->p_recvfrom !=NO_TASK);
        assert(p_who_wanna_recv->has_int_msg == 0);
    }
    return 0;
}


/****************************************************************
*       sys_sendrec      senderc的c实现
******************************************************************/

int sys_sendrec(int function, int src_dest, MESSAGE* m, proc* p)
{
    assert(k_reenter==0);  //被ring1～3的task调用
    assert((src_dest >=0 && src_dest < NR_TASKS + NR_PROCS) ||
            src_dest == ANY ||
            src_dest == INTERRUPT);

    int ret = 0;
    int caller=proc2pid(p);

    MESSAGE* m1a = (MESSAGE*)va2la(caller,m); //通过led_sel 进行地址转换

    m1a->source = caller;

    assert(m1a->source != src_dest);

    if(function == SEND)
    {
        ret = msg_send(p,src_dest,m);
        if(ret!=0)
            return ret;
    }
    else if(function == RECEIVE)
    {
        ret = msg_recevie(p,src_dest,m);
        if(ret!=0)
            return ret;
    }
    else
    {
        panic("{sys_sendrec} invalid function: "
              "%d (send:%d,RECEVIE:%d).",function,SEND,RECEIVE);
    }

    return 0;
}

/*****************************************************************
*   send_recv ring1~3
*   ipc syscall
*   sendrec 的一个包装
******************************************************************/
int send_recv(int function, int src_dest, MESSAGE* msg)
{

    int ret=0;
    //printf("in send_recv:func:%x,src_dest:%d,msg_type:%d,msg_src:%d\n", function,src_dest,msg->type,msg->source);
    if(function == RECEIVE)
    {
        memset(msg,0,sizeof(MESSAGE));
    }
    switch(function)
    {
    case BOTH:
              //printf("func:%x,src_dest:%d,msg_type:%d,msg_src:%d\n", function,src_dest,msg->type,msg->source);

              ret = sendrec(SEND, src_dest, msg);
              if(ret == 0)  //fix a bug here =_= why not 0==ret
              {
                  //printf("func:%x,src_dest:%d,msg_type:%d,msg_src:%d\n", function,src_dest,msg->type,msg->source);
                  ret=sendrec(RECEIVE, src_dest, msg);
                  //printf("func:%x,src_dest:%d,msg_type:%d,msg_src:%d\n", function,src_dest,msg->type,msg->source);
              }
              break;
    case SEND:
    case RECEIVE:
               ret = sendrec(function, src_dest, msg);
               break;
    default:
            assert(function == BOTH ||
                   function == SEND || function == RECEIVE);
            break;
    }
    return ret;
}

/*****************************************************************************
 *                                inform_int
 *****************************************************************************/
/**
 * <Ring 0> Inform a proc that an interrupt has occured.
 *
 * @param task_nr  The task which will be informed.
 *****************************************************************************/
 void inform_int(int task_nr)
{
	 proc* p = proc_table + task_nr;

	if ((p->p_flags & RECEIVING) && /* dest is waiting for the msg */
	    ((p->p_recvfrom == INTERRUPT) || (p->p_recvfrom == ANY))) {
		p->p_msg->source = INTERRUPT;
		p->p_msg->type = HARD_INT;
		p->p_msg = 0;
		p->has_int_msg = 0;
		p->p_flags &= ~RECEIVING; /* dest has received the msg */
		p->p_recvfrom = NO_TASK;
		assert(p->p_flags == 0);
		//unblock(p);

		assert(p->p_flags == 0);
		assert(p->p_msg == 0);
		assert(p->p_recvfrom == NO_TASK);
		assert(p->p_sendto == NO_TASK);
	}
	else {
		p->has_int_msg = 1;
	}
}

/*****************************************************************************
 *                                dump_proc
 *****************************************************************************/
 void dump_proc( proc* p)
{
	char info[STR_DEFAULT_LEN];
	int i;
	int text_color = MAKE_COLOR(GREEN, RED);

	int dump_len = sizeof( proc);

	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, 0);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, 0);

	sprintf(info, "byte dump of proc_table[%d]:\n", p - proc_table); disp_color_str(info, text_color);
	for (i = 0; i < dump_len; i++) {
		sprintf(info, "%x.", ((unsigned char *)p)[i]);
		disp_color_str(info, text_color);
	}

	/* printl("^^"); */

	disp_color_str("\n\n", text_color);
	sprintf(info, "ANY: 0x%x.\n", ANY); disp_color_str(info, text_color);
	sprintf(info, "NO_TASK: 0x%x.\n", NO_TASK); disp_color_str(info, text_color);
	disp_color_str("\n", text_color);

	sprintf(info, "ldt_sel: 0x%x.  ", p->ldt_sel); disp_color_str(info, text_color);
	sprintf(info, "ticks: 0x%x.  ", p->ticks); disp_color_str(info, text_color);
	sprintf(info, "priority: 0x%x.  ", p->priority); disp_color_str(info, text_color);
	sprintf(info, "pid: 0x%x.  ", p->pid); disp_color_str(info, text_color);
	sprintf(info, "name: %s.  ", p->p_name); disp_color_str(info, text_color);
	disp_color_str("\n", text_color);
	sprintf(info, "p_flags: 0x%x.  ", p->p_flags); disp_color_str(info, text_color);
	sprintf(info, "p_recvfrom: 0x%x.  ", p->p_recvfrom); disp_color_str(info, text_color);
	sprintf(info, "p_sendto: 0x%x.  ", p->p_sendto); disp_color_str(info, text_color);
	sprintf(info, "nr_tty: 0x%x.  ", p->nr_tty); disp_color_str(info, text_color);
	disp_color_str("\n", text_color);
	sprintf(info, "has_int_msg: 0x%x.  ", p->has_int_msg); disp_color_str(info, text_color);
}


/*****************************************************************************
 *                                dump_msg
 *****************************************************************************/
 void dump_msg(const char * title, MESSAGE* m)
{
	int packed = 0;
	printf("{%s}<0x%x>{%ssrc:%s(%d),%stype:%d,%s(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)%s}%s",  //, (0x%x, 0x%x, 0x%x)}",
	       title,
	       (int)m,
	       packed ? "" : "\n        ",
	       proc_table[m->source].p_name,
	       m->source,
	       packed ? " " : "\n        ",
	       m->type,
	       packed ? " " : "\n        ",
	       m->u.m3.m3i1,
	       m->u.m3.m3i2,
	       m->u.m3.m3i3,
	       m->u.m3.m3i4,
	       (int)m->u.m3.m3p1,
	       (int)m->u.m3.m3p2,
	       packed ? "" : "\n",
	       packed ? "" : "\n"/* , */
		);
}



