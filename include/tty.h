#ifndef _tty_h
#define _tty_h
struct s_console;

#define TTY_IN_BYTES	256  //tty����Ĵ�С

/* TTY */
typedef struct s_tty
{
	u32	in_buf[TTY_IN_BYTES];	/* TTY ���뻺���� */
	u32*	p_inbuf_head;		/* ָ�򻺳�������һ������λ�� */
	u32*	p_inbuf_tail;		/* ָ���������Ӧ����ļ�ֵ */
	int	inbuf_count;		/* ���������Ѿ�����˶��� */
	//char* p_buf;            //���buf

	struct s_console *	p_console;
}TTY;



void in_process(TTY* p_tty, u32 key);
void tty_write(TTY* p_tty, char* buf, int len);  //������sys_write����

#endif
