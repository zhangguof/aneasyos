/****************************
*mian.c kernel模块的起始函数
*各种初始化中断==
****************************/
#include "MyOs.h"
//#include "multiboot.h"

//void restart();

void get_mbi(multiboot_info_t* p_mbi)
{
    memcpy(&mbi,p_mbi,sizeof(multiboot_info_t));
}

//填充gdt描述符表
void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute)
{
    p_desc->limit_low   = limit & 0x0ffff;
    p_desc->base_low    = base & 0x0ffff;
    p_desc->base_mid     = (base>>16) & 0x0ff;
    p_desc->attr1       = attribute & 0xff;
    p_desc->limit_high_attr2 = ((limit>>16) & 0x0f) | (attribute>>8) & 0xf0;
    p_desc->base_high   = (base>>24) & 0x0ff;
}
//由段明求绝对地址
u32 seg2phy(u16 seg)
{
    DESCRIPTOR *p_desc = &gdt[seg>>3];
    return (p_desc->base_high<<24 | p_desc->base_mid<<16 | p_desc->base_low);
}


void cstart(void )
{
//  disp_pos=0;
    //  disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
    //           "-----\"cstart\" begins----\n");

    //while(1){}

    //复制gdt


    u16* p_gdt_limt=(u16 *)(&gdt_ptr[0]);
    u32* p_gdt_base=(u32 *)(&gdt_ptr[2]);

    init_descriptor(&gdt[0],0,0,0);
    init_descriptor(&gdt[1],
                    0,
                    0xfffff,
                    DA_32|DA_CR|DA_LIMIT_4K);
    init_descriptor(&gdt[2],
                    0,
                    0xfffff,
                    DA_32|DA_DRW|DA_LIMIT_4K);
    init_descriptor(&gdt[3],
                    0x0b8000,
                    0xffff,
                    DA_DRW|DA_DPL3);

//    memcpy(&gdt,                    //new gdt
//           (void *)(*p_gdt_base),  //old  gdt base
//            *(p_gdt_limt) + 1 );

    //gdt_ptr[6] 6b 0~15 limit 16~47:base
    //u16* p_gdt_limt=(u16 *)(&gdt_ptr[0]);
    //u32* p_gdt_base=(u32 *)(&gdt_ptr[2]);

    *p_gdt_limt = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *p_gdt_base = (u32)&gdt;

    //初始化idt
    u16* p_idt_limt = (u16*)(&idt_ptr[0]);
    u32* p_idt_base = (u32*)(&idt_ptr[2]);

    *p_idt_base = (u32) &idt;
    *p_idt_limt = IDT_SIZE * sizeof(GATE) - 1;

    //初始化gdt中的进程ldt
    int i;
    PROCESS* p_proc = proc_table;
    u16 sel_ldt = ind_ldt_first<<3;
    for(i=0; i<NR_TASKS + NR_PROCS; ++i)
    {
        memset(&proc_table[i],0,sizeof(proc));
        proc_table[i].ldt_sel = sel_ldt_first + (i<<3);
        assert(ind_ldt_first + i < GDT_SIZE);
        init_descriptor(&gdt[sel_ldt>>3],
                        vir2phys( seg2phy(SEL_KERNEL_DS), proc_table[i].ldts),
                        LDT_SIZE * sizeof(DESCRIPTOR) - 1,
                        DA_LDT);
        p_proc++;
        sel_ldt += 1<<3;

    }

    //tss 初始化
    memset(&tss, 0, sizeof(tss));
    tss.ss0 = SEL_KERNEL_DS;
    init_descriptor(&gdt[ind_tss],
                    vir2phys(seg2phy(SEL_KERNEL_DS),&tss),
                    sizeof(tss)-1,
                    DA_386TSS);
    tss.iobase = sizeof(tss);//没有io许可

    init_prot();  //初始化中断门，系统调用int0x90

    //  disp_str("---------\"init end\"--------\n");

}

int  get_ticks2()
{
    MESSAGE msg;
    reset_msg(&msg);
    msg.type = GET_TICKS;
    send_rec(BOTH, TASK_SYS, &msg);
    return msg.RETVAL;
}

void main()
{
    disp_pos=0;
    disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    disp_str("--------\"main\" begin------\n");

    //进程调度初始化
    disp_str("--------\"init_sched\" begin------\n");
    init_sched();
    disp_str("--------\"init_sched\" end------\n");
    //init_keyboard();

    /* Print out the flags.  */


    ticks=0;
    k_reenter = 0;
    p_proc_ready = proc_table;


    restart(); //载入任务0 ring0--->ring1


    while(1) {}
}

//taskA代码
void testA()
{

    while(1)
    {
        printf("A.");
        milli_delay(5000);
    }
}

//taskB
void testB()
{

    while(1)
    {
        printf("B.");
        milli_delay(5000);
    }
}

void testC()
{
#ifdef MULTIBOOT
//       Print out the flags.
    multiboot_info_t *p_mbi = &mbi;
    printf("flags = 0x%x\n", (unsigned)p_mbi->flags);

//   Are mem_* valid?
    if (CHECK_FLAG (p_mbi->flags, 0))
        printf ("mem_lower = %dKB, mem_upper = %dKB\n",
                (unsigned) p_mbi->mem_lower, (unsigned) p_mbi->mem_upper);
#endif
//   int pid=getpid();
//   printf("my pid is:%d\n",pid);
    printf("shell is begining...\n");
    char buf[256];
    char *p_buf;
    char cmd[10];
    int cn;

    while(1)
    {
        printf("$:");
        int rn = read(buf);
        if(rn)
        {
            //get cmd
            cn=0;
            p_buf = buf;
            while(*p_buf != '\0' && *p_buf != ' ')
            {
                cmd[cn++] = *p_buf++;
            }
            cmd[cn]='\0';
            if(*p_buf == ' ') p_buf++;   //skip the space

            if(strcmp(cmd,"echo")==0)   //echo
            {
                printf("%s\n",p_buf);
            }
            else if(strcmp(cmd,"ps") == 0)  //ps
            {
                printf("PID  name   tty_nr  state  runtime\n");
                int i;
                for(i=0;i<NR_TASKS + NR_PROCS;++i)
                {
                    proc* p=&proc_table[i];
                    if(p->p_flags != FREE_SLOT)
                    {
                        int nlen=strlen(p->p_name);
                        printf("%d    ",i);
                        printf("%s",p->p_name);
                        nlen = 7 - nlen;
                        while(nlen--) printf(" ");

                        printf("%d       %d      %d\n",p->nr_tty,p->p_flags,p->runtime);

                    }
                }
            }
            else
            {
                printf("%s command not found\n",cmd);
            }

            //printf("you input %s\n",buf);
        }

    }
    spin("C.");
}



/*****************************************************************************
 *                                task_fs
 *****************************************************************************/
/**
 * <Ring 1> The main loop of TASK FS.
 *
 *****************************************************************************/
void task_fs()
{
    printf("Task FS begins.\n");

    /* open the device: hard disk */
    MESSAGE driver_msg;
    driver_msg.type = DEV_OPEN;
    send_rec(BOTH, TASK_HD, &driver_msg);

    MESSAGE msg;
   // milli_delay(20000);
    while(1)
    {
        send_rec(RECEIVE, ANY, &msg);
        int src = msg.source;
        int ret;
        switch(msg.type)
        {
        case READ:
            //printf("{FS}get a read msg form %d\n",src);
            ret = do_read(&tty_table[proc_table[src].nr_tty],va2la(src,msg.BUF));
            msg.type = SYSCALL_RET;
            msg.RETVAL = ret;
            send_rec(SEND, src, &msg);
            break;
        default:
            panic("unknow msg type");
            break;

        }
    }

    spin("FS");
    //while(1){}
}

void Init()
{
    printf("Init is begin");
    //spin("test end");
    int pid = fork();

    if (pid != 0)
    {
        // parent process
        printf("parent is running, child pid:%d\n", pid);
        int s;
        int child = wait(&s);
        printf("child (%d) exited with status: %d.\n", child, s);
    }
    else
    {
        //child process

        printf("child is running, pid:%d\n", getpid());  //why not p_proc_ready - proc_table?
        //because child is a copy of kernel space
        exit(123);

    }
    while (1)
    {
        int s;
        int child = wait(&s);
        printf("child (%d) exited with status: %d.\n", child, s);
    }
}


