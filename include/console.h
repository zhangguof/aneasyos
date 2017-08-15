/*
*
* console.h 控制台驱动
*
*/
#ifndef _console_h
#define _console_h

/* CONSOLE */
typedef struct s_console
{
	unsigned int	current_start_addr;	/* 当前显示到了什么位置	  */
	unsigned int	original_addr;		/* 当前控制台对应显存位置 */
	unsigned int	v_mem_limit;		/* 当前控制台占的显存大小 */
	unsigned int	cursor;			/* 当前光标位置 */
}CONSOLE;

#define SCR_UP	1	/* scroll forward */
#define SCR_DN	-1	/* scroll backward */

#define SCREEN_SIZE		(80 * 25)
#define SCREEN_WIDTH		80

#define GRAY_CHAR		(MAKE_COLOR(BLACK, BLACK) | BRIGHT)
#define RED_CHAR		(MAKE_COLOR(BLUE, RED) | BRIGHT)

#define DEFAULT_CHAR_COLOR	0x07	/* 0000 0111 黑底白字 */

int is_current_console(CONSOLE* p_con);
void out_char(CONSOLE* p_con, char ch);
void init_screen(TTY* p_tty);
void select_console(int nr_console);
void scroll_screen(CONSOLE* p_con, int direction);


#endif
