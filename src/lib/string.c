//string.asm改用c实现

// ; 导出函数
// global	memcpy
// global  memset
// global  strcpy
// global  strlen
#include "type.h"
#include "klib.h"

void memcpy(void *dst,void *src,size_t size)
{
	u8 *p_dst = (u8*) dst;
	u8 *p_src = (u8*) src;
	while(size--)
		*p_dst++ = *p_src++;
}

void memset(void* dst, u8 ch, size_t size)
{
	u8 *p_dst = (u8*) dst;
	while(size--)
	{
		*p_dst = ch;
	}
}

char* strcpy(char* p_dst, char* p_src)
{
	char *p = p_dst;
	while(*p_src)
	{
		*p_dst++ = *p_src++;
	}
	*p_dst = '\0';
	return p;

}

size_t strlen(char* p_str)
{
	char *p = p_str;
	while(*p++);
	return (size_t)(p - p_str);
}

void print_buf(const void* buf,int size)
{
	u32* p = (u32*) buf;
	for(int i=0;i<size/4;++i)
	{
		printf("%x ", p[i]);
		if(i && i%4==0)
		{
			printf("\n");
		}
	}
}



