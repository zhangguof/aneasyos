#include "MyOs.h"

/*======================================================================*
                                i2a
 *======================================================================*/
static char* i2a(int val, int base, char ** ps)
{
	char *p = *ps;
	char *p_buf = p;
	int m;
	if(val==0)
	{
		*p++ = '0';
		*p = '\0';
		return *ps;
	}
	
	while(val)
	{
		m = val % base;
		val /= base;
		*p++ = (m < 10) ? (m + '0') : (m - 10 + 'A');
	}
	*p-- = '\0';
	for(char c;p_buf<p;--p,++p_buf)
	{
		c = *p_buf;
		*p_buf = *p;
		*p = c;
	}
	return *ps;
}

/*======================================================================*
                                u2a
 *======================================================================*/
static char* u2a(unsigned int val,char ** ps)
{
	char *p = *ps;
	//char *p_buf = p;
	*p++='0';
	*p++='x';

	for(int i=7;i>=0;--i)
	{
		int c = (val>>i*4)&(0x0000000F);
		*p++ = (char)((c<10)?(c+'0'):(c-10+'A'));
	}
	*p = '\0';
	return *ps;
}


/*======================================================================*
                                vsprintf
 *======================================================================*/
/*
 *  为更好地理解此函数的原理，可参考 printf 的注释部分。
 */
 int vsprintf(char *buf, const char *fmt, va_list args)
{
	char*	p;

	va_list	p_next_arg = args;
	int	m;

	char	inner_buf[STR_DEFAULT_LEN];
	char	cs;
	int	align_nr;

	for (p=buf;*fmt;fmt++) {
		if (*fmt != '%') {
			*p++ = *fmt;
			continue;
		}
		else {		/* a format string begins */
			align_nr = 0;
		}

		fmt++;

		if (*fmt == '%') {
			*p++ = *fmt;
			continue;
		}
		else if (*fmt == '0') {
			cs = '0';
			fmt++;
		}
		else {
			cs = ' ';
		}
		while (((unsigned char)(*fmt) >= '0') && ((unsigned char)(*fmt) <= '9')) {
			align_nr *= 10;
			align_nr += *fmt - '0';
			fmt++;
		}

		char * q = inner_buf;
		//memset(q, 0, sizeof(inner_buf));

		switch (*fmt) {
		case 'c':
			*q++ = *((char*)p_next_arg);
			p_next_arg += 4;
			*q='\0';
			break;
		case 'x':
			m = *((unsigned int*)p_next_arg);
			u2a(m, &q);
			p_next_arg += 4;
			break;
		case 'd':
			m = *((int*)p_next_arg);
			if (m < 0) {
				m = m * (-1);
				*q++ = '-';
			}
			i2a(m, 10, &q);
			p_next_arg += 4;
			break;
		case 's':
			strcpy(q, (*((char**)p_next_arg)));
			//q += strlen(*((char**)p_next_arg));
			p_next_arg += 4;
			break;
		default:
			break;
		}

		int buf_len = strlen(inner_buf);
		int cs_n = ((align_nr > buf_len) ? (align_nr - buf_len) : 0);
		for (int k = 0; k < cs_n; k++) {
			*p++ = cs;
		}
		q = inner_buf;
		while (*q) {
			*p++ = *q++;
		}
	}

	*p = 0;

	return (p - buf);
}


/*======================================================================*
                                 sprintf
 *======================================================================*/
int sprintf(char *buf, const char *fmt, ...)
{
	va_list arg = (va_list)((char*)(&fmt) + 4);        /* 4 是参数 fmt 所占堆栈中的大小 */
	return vsprintf(buf, fmt, arg);
}
