#include "MyOs.h"

int printf(const char* fmt,...)
{
    int i;
    char buf[256];
    va_list arg=(va_list)((char*)(&fmt) + 4); //fmt在栈中的大小为4
    i=vsprintf(buf,fmt,arg);  //arg fmt之后第一个参数的地址
    buf[i]='\0';
    printx(buf);  //改用printx系统调用
    //write(buf,i);
    return i;
}
