#include "MyOs.h"

int printf(const char* fmt,...)
{
    int i;
    char buf[256];
    va_list arg=(va_list)((char*)(&fmt) + 4); //fmt��ջ�еĴ�СΪ4
    i=vsprintf(buf,fmt,arg);  //arg fmt֮���һ�������ĵ�ַ
    buf[i]='\0';
    printx(buf);  //����printxϵͳ����
    //write(buf,i);
    return i;
}
