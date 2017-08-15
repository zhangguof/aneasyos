/*
*
* console.h ����̨����
*
*/
#ifndef _console_h
#define _console_h

/* CONSOLE */
typedef struct s_console
{
	unsigned int	current_start_addr;	/* ��ǰ��ʾ����ʲôλ��	  */
	unsigned int	original_addr;		/* ��ǰ����̨��Ӧ�Դ�λ�� */
	unsigned int	v_mem_limit;		/* ��ǰ����̨ռ���Դ��С */
	unsigned int	cursor;			/* ��ǰ���λ�� */
}CONSOLE;

#define SCR_UP	1	/* scroll forward */
#define SCR_DN	-1	/* scroll backward */

#define SCREEN_SIZE		(80 * 25)
#define SCREEN_WIDTH		80

#define GRAY_CHAR		(MAKE_COLOR(BLACK, BLACK) | BRIGHT)
#define RED_CHAR		(MAKE_COLOR(BLUE, RED) | BRIGHT)

#define DEFAULT_CHAR_COLOR	0x07	/* 0000 0111 �ڵװ��� */

int is_current_console(CONSOLE* p_con);
void out_char(CONSOLE* p_con, char ch);
void init_screen(TTY* p_tty);
void select_console(int nr_console);
void scroll_screen(CONSOLE* p_con, int direction);


#endif
