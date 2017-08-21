/*
* tty.c
*实现tty任务
*一个tty任务是一个进程，处理所有的终端输入输出
*/

#include "MyOs.h"

#define TTY_FIRST (tty_table)
#define TTY_END (tty_table+NR_CONSOLES)

static void tty_do_read(TTY* p_tty);
static void tty_do_write(TTY* p_tty);
static void init_tty(TTY* p_tty);
static void put_key(TTY* p_tty, u32 key);



void init_tty(TTY* p_tty)
{
    p_tty->inbuf_count = 0;
    p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;
    init_screen(p_tty);
}

void task_tty()
{
    disp_str("--\"tty task\" begin--ticks:");
    disp_int(get_ticks());
    disp_str("\n");
    //assert(0);
    //panic("in tty!");

    TTY* p_tty;
    init_keyboard();

    for(p_tty=TTY_FIRST; p_tty<TTY_END; p_tty++)
    {
        init_tty(p_tty);
    }
    //nr_current_console = 0;  //初始控制台
    select_console(0);
    //disp_str("--begin tty loop----\n")
    while(1)
    {
        for(p_tty = TTY_FIRST; p_tty<TTY_END; p_tty++)
        {
            tty_do_read(p_tty);
            //tty_do_write(p_tty);
        }
    }

}

void tty_do_read(TTY* p_tty)
{
    if(is_current_console(p_tty->p_console))
    {
        keyboard_read(p_tty);
    }
}

void tty_do_write(TTY* p_tty)
{
    if(p_tty->inbuf_count)  //缓存中有数据
    {
        char ch = *(p_tty->p_inbuf_tail);
        p_tty->p_inbuf_tail++;

        //inbuf_count 保证了一定会有数据
        if(p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES)
        {
            p_tty->p_inbuf_tail= p_tty->in_buf;
        }
        p_tty->inbuf_count--;
        //out_char(p_tty->p_console, ch);
        //*buf++ = ch;
    }
}
char get_char_form_tty(TTY* p_tty)
{
   char ch;
   //printf("the tty is : %d\n",p_tty-tty_table);
   while(p_tty->inbuf_count <= 0){};
   //disable_int();

   ch = *(p_tty->p_inbuf_tail);
   p_tty->p_inbuf_tail++;

    //inbuf_count 保证了一定会有数据
   if(p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES)
   {
       p_tty->p_inbuf_tail= p_tty->in_buf;
   }
    p_tty->inbuf_count--;
   // enable_int();
    return ch;
}
//put the data to buf
int do_read(TTY* p_tty, char* buf)
{
    int cnt = 0;  //已读字符
    //p_tty->inbuf_count = 0;
    char ch = get_char_form_tty(p_tty);

    while(ch != '\n')
    {
        //printf("Now get a char %c:\n",ch);
        if(ch == '\b' && cnt>0)
        {
            cnt--;
            ch=get_char_form_tty(p_tty);
            continue;
        }
        buf[cnt++]=ch;
        ch=get_char_form_tty(p_tty);
    }
    buf[cnt]='\0';
    return cnt;

}


void in_process(TTY* p_tty,u32 key)
{
    char output[2]= {'\0','\0'};
    //disp_int(key);
    if(!(key & FLAG_EXT))
    {
        put_key(p_tty,key);
        //printf("Has put a key %c in ppt_%d,the count is %d\n",\
        //       (char)key,p_tty-tty_table,p_tty->inbuf_count);
        out_char(p_tty->p_console,key);
    }
    else
    {
        int raw_code = key & MASK_RAW;
        switch(raw_code)
        {
        case ENTER:
            put_key(p_tty, '\n');
            out_char(p_tty->p_console,'\n');
            break;
        case BACKSPACE:
            put_key(p_tty, '\b');
            out_char(p_tty->p_console,'\b');
            //printf("Has put a key %d\n",key);
            break;
        case UP:
            if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
            {
                 //disp_str("up chang\n");
                scroll_screen(p_tty->p_console, SCR_UP);
            }
            break;
        case DOWN:
            if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
            {
                //disp_str("down chang\n");
                scroll_screen(p_tty->p_console, SCR_DN);
            }
            break;
        case F1:
        case F2:
        case F3:
        case F4:
        case F5:
        case F6:
        case F7:
        case F8:
        case F9:
        case F10:
        case F11:
        case F12:
            /* Alt + F1~F12 */
            if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R))
            {

                select_console(raw_code - F1);
            }
            break;
        default:
            break;
        }
    }
}


void put_key(TTY* p_tty, u32 key)
{
	if (p_tty->inbuf_count < TTY_IN_BYTES) {
		*(p_tty->p_inbuf_head) = key;
		p_tty->p_inbuf_head++;
		if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_head = p_tty->in_buf;
		}
		p_tty->inbuf_count++;
	}
}

//tty_write
void tty_write(TTY* p_tty, char* buf, int len)
{
    //disp_str("tty nr:");
    //disp_int(p_tty-TTY_FIRST);

    char *p =buf;
    int i=len;
    while(i)
    {
        out_char(p_tty->p_console, *p++);
        i--;
    }
}

int sys_printx(int _unused1, int _unused2, char* s,  proc* p_proc)
{
    //disp_int("in sys_printx\n");
	const char * p;
	char ch;

	char reenter_err[] = "? k_reenter is incorrect for unknown reason";
	reenter_err[0] = MAG_CH_PANIC;

	/**
	 * @note Code in both Ring 0 and Ring 1~3 may invoke printx().
	 * If this happens in Ring 0, no linear-physical address mapping
	 * is needed.
	 *
	 * @attention The value of `k_reenter' is tricky here. When
	 *   -# printx() is called in Ring 0
	 *      - k_reenter > 0. When code in Ring 0 calls printx(),
	 *        an `interrupt re-enter' will occur (printx() generates
	 *        a software interrupt). Thus `k_reenter' will be increased
	 *        by `kernel.asm::save' and be greater than 0.
	 *   -# printx() is called in Ring 1~3
	 *      - k_reenter == 0.
	 */
	if (k_reenter == 0)  /* printx() called in Ring<1~3> */
		p = va2la(proc2pid(p_proc), s);
	else if (k_reenter > 0) /* printx() called in Ring<0> */
		p = s;
	else	/* this should NOT happen */
		p = reenter_err;

	/**
	 * @note if assertion fails in any TASK, the system will be halted;
	 * if it fails in a USER PROC, it'll return like any normal syscall
	 * does.
	 */
	if ((*p == MAG_CH_PANIC) ||
	    (*p == MAG_CH_ASSERT && p_proc_ready < &proc_table[NR_TASKS])) {
		disable_int();
		char * v = (char*)V_MEM_BASE;
		const char * q = p + 1; /* +1: skip the magic char */

		while (v < (char*)(V_MEM_BASE + V_MEM_SIZE)) {
			*v++ = *q++;
			*v++ = RED_CHAR;
			if (!*q) {
				while (((int)v - V_MEM_BASE) % (SCREEN_WIDTH * 16)) {
					/* *v++ = ' '; */
					v++;
					*v++ = GRAY_CHAR;
				}
				q = p + 1;
			}
		}

		__asm__ __volatile__("hlt");
	}

	while ((ch = *p++) != 0) {
		if (ch == MAG_CH_PANIC || ch == MAG_CH_ASSERT)
			continue; /* skip the magic char */

		out_char(tty_table[p_proc->nr_tty].p_console, ch);
	}

	return 0;
}

