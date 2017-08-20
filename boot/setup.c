
#define VIDEO_ADDR 0x0b8000
#define LINE_CHAR_NUM 80


typedef unsigned short u16;
u16 *p_gs_base = (u16*) VIDEO_ADDR; //显存地址

int g_dwDispPos = (80 * 6 + 0);

void DispStr(char *s)
{
	u16 color = 0x0F;
	u16* p = p_gs_base + g_dwDispPos;
	while(*s)
	{
		char c = *s++;
		if(c=='\n')
		{
			int line_no = (p - p_gs_base)/LINE_CHAR_NUM;
			p = p_gs_base + (line_no+1)*LINE_CHAR_NUM;
		}
		else
		{
			*p++ = (u16)(c | (color<<8));
		}
	}
	g_dwDispPos = p - p_gs_base;
}

void DispReturn()
{
	DispStr("\n");
}

void DispInt(int a)
{
	char *p = "00000000h ";
	char *p_end = p+7;

	while(a)
	{
		int  c = a & 0x0000000F;
		if(c >= 10)
		{
			*p_end = (char)('A'+(c-10));
		}
		else
		{
			*p_end = (char)('0'+c);
		}
		p_end--;
		a = a/16;
	}
	DispStr(p);	
}

void print_hello()
{
	DispStr("Hello in C!!!\nTEST\nTset2\n");
	DispInt(0x123456EF);
	DispInt(0x123451FF);
	DispReturn();
	DispInt(0x123456FF);
	DispStr("Hello in C2!!!\nTEST\nTset2\n");
}