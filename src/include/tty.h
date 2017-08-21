#ifndef _tty_h
#define _tty_h
struct s_console;

#define TTY_IN_BYTES	256  //tty缓冲的大小

/* TTY */
typedef struct s_tty
{
	u32	in_buf[TTY_IN_BYTES];	/* TTY 输入缓冲区 */
	u32*	p_inbuf_head;		/* 指向缓冲区中下一个空闲位置 */
	u32*	p_inbuf_tail;		/* 指向键盘任务应处理的键值 */
	int	inbuf_count;		/* 缓冲区中已经填充了多少 */
	//char* p_buf;            //输出buf

	struct s_console *	p_console;
}TTY;



void in_process(TTY* p_tty, u32 key);
void tty_write(TTY* p_tty, char* buf, int len);  //导出给sys_write调用

#endif
