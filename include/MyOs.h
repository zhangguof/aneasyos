//һ���԰�����Ҫ�Ķ��ļ�

#include "const.h"  //����˵��
#include "type.h"   //�������Ͷ���
#include "multiboot.h"
#include "pm.h"   //intel����ģʽ�µ�һЩ���ݽṹgdt idt tss ��
#include "process.h"  //���̽ṹpcb���� �������� ջ��
#include "tty.h"      //tty �ṹ����
#include "console.h"  //����̨���ݽṹ���� console.c�õ���һЩ����
#include "hd.h"       //Ӳ������ �õ���һЩ���ݽṹ
#include "string.h"   //�����ַ�����������ԭ��
#include "klib.h"     //���ֻ�� ��֪��������ԭ��
#include "syscall.h"   //ϵͳ���ýӿں���ԭ��
#include "global.h"   //ȫ�ֱ������� �����global.c�ж���
#include "sched.h"    //���̵��ȵļ�������ԭ��
#include "keyboard.h"  //���̶�ȡ������нṹԭ�ͣ�һЩ�����λ�ĳ�������

#include "assert.h"

//#define MULTIBOOT 1
