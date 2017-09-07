 /*
 *string.asm
 */
#ifndef _string_h
#define _string_h

void* memcpy(void* p_dst, void* p_src, unsigned int  size);
void  memset(void* p_dst, char ch, unsigned int  size);
char* strcpy(char* p_dst, char* p_src);
char* strncpy(char* p_dst, const char* p_src, unsigned int n);
int   strlen(char* p_str);
//klib.c
int	memcmp(const void * s1, const void *s2, unsigned int  n);
int	strcmp(const char * s1, const char *s2);
char*	strcat(char * s1, const char *s2);

void print_buf(const void* buf,unsigned int  size);

//用于ring0，物理地址， 当前线性地址物理地址是一样的
#define	phys_copy	memcpy
#define	phys_set	memset


#endif
