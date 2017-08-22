
#define VIDEO_ADDR 0x0b8000
#define LINE_CHAR_NUM 80
#define BITWIDTH 8


typedef unsigned short u16;
typedef unsigned int u32;
u16 *p_gs_base = (u16*) VIDEO_ADDR; //显存地址

//Address Range Descriptor Structure
int dwMemSize = 0;
typedef struct
{
	u32 dwBaseAddrLow;
	u32 dwBaseAddrHigh;
	u32 dwLengthLow;
	u32 dwLengthHigh;
	u32 dwType;
}ARDStruct;

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

void DispInt(unsigned int a)
{
	char *p = "00000000h ";
	for(int i=0;i<BITWIDTH;++i)
	{
		int c = (a>>((BITWIDTH-1 - i)*4)) & (0xF);
		if(c >= 10)
		{
			p[i] = (char)('A'+(c-10));
		}
		else
		{
			p[i] = (char)('0'+c);
		}
	}
	DispStr(p);	
}

u32 DispMemInfo(ARDStruct *p_buf,int McrNum)
{
	u32 max_mem_size = 0;
	for(int i=0;i<McrNum;++i)
	{
		ARDStruct *p = p_buf+i;
		u32 mem_size = p->dwBaseAddrLow + p->dwLengthLow;
		DispInt(p->dwBaseAddrLow);
		DispInt(p->dwBaseAddrHigh);
		DispInt(p->dwLengthLow);
		DispInt(p->dwLengthHigh);
		DispInt(p->dwType);
		DispReturn();
		if(p->dwType == 1 && mem_size > max_mem_size)
		{
			max_mem_size = mem_size;
		}
	}
	DispStr("RAM size:");
	DispInt(max_mem_size);
	return max_mem_size;
}

void print_hello()
{
	// DispInt(0x1);
	// DispInt(0x0);
	// DispInt(0x2);
	// DispStr("Hello in C!!!\nTEST\nTset2\n");
	// DispInt(0x123456EF);
	// DispInt(0x123451FF);
	// DispReturn();
	// DispInt(0x123456FF);
	// DispStr("Hello in C2!!!\nTEST\nTset2\n");
}