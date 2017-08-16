//一次性包含需要的都文件

#include "const.h"  //常数说明
#include "type.h"   //基本类型定义
#include "multiboot.h"
#include "pm.h"   //intel保护模式下的一些数据结构gdt idt tss 等
#include "process.h"  //进程结构pcb声明 任务数量 栈等
#include "tty.h"      //tty 结构定义
#include "console.h"  //控制台数据结构定义 console.c用到的一些常量
#include "hd.h"       //硬盘驱动 用到的一些数据结构
#include "string.h"   //几个字符操作函数的原型
#include "klib.h"     //各种汇编 不知名函数的原型
#include "syscall.h"   //系统调用接口函数原型
#include "global.h"   //全局变量声明 大多在global.c中定义
#include "sched.h"    //进程调度的几个函数原型
#include "keyboard.h"  //键盘读取缓冲队列结构原型，一些特殊键位的常量定义

#include "assert.h"

//#define MULTIBOOT 1
