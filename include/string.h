 /*
 *string.asm
 */
#ifndef _string_h
#define _string_h

void* memcpy(void* p_dst, void* p_src, int size);
void  memset(void* p_dst, char ch, int size);
char* strcpy(char* p_dst, char* p_src);
int   strlen(char* p_str);
//klib.c
int	memcmp(const void * s1, const void *s2, int n);
int	strcmp(const char * s1, const char *s2);
char*	strcat(char * s1, const char *s2);

//����ring0��������ַ�� ��ǰ���Ե�ַ������ַ��һ����
#define	phys_copy	memcpy
#define	phys_set	memset


#endif