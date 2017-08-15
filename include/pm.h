/*
*pm.h
*����ģʽ����Ҫ�õ���һЩ����
*/

#ifndef _pm_h
#define _pm_h

#include "type.h"

/* �洢��������/ϵͳ�������� */
typedef struct s_descriptor		/* �� 8 ���ֽ� */
{
	u16	limit_low;		/* Limit */
	u16	base_low;		/* Base */
	u8	base_mid;		/* Base */
	u8	attr1;			/* P(1) DPL(2) DT(1) TYPE(4) */
	u8	limit_high_attr2;	/* G(1) D(1) 0(1) AVL(1) LimitHigh(4) */
	u8	base_high;		/* Base */
}DESCRIPTOR;

typedef DESCRIPTOR descriptor;

/* �������� */
typedef struct s_gate
{
	u16	offset_low;	/* Offset Low */
	u16	selector;	/* Selector */
	u8	dcount;		/* ���ֶ�ֻ�ڵ���������������Ч��
				��������õ����ŵ����ӳ���ʱ������Ȩ����ת���Ͷ�ջ�ĸı䣬��Ҫ������ջ�еĲ������Ƶ��ڲ��ջ��
				��˫�ּ����ֶξ�������˵�������������ʱ��Ҫ���Ƶ�˫�ֲ����������� */
	u8	attr;		/* P(1) DPL(2) DT(1) TYPE(4) */
	u16	offset_high;	/* Offset High */
}GATE;
//tss
typedef struct s_tss {
	u32	backlink;
	u32	esp0;		/* stack pointer to use during interrupt */
	u32	ss0;		/*   "   segment  "  "    "        "     */
	u32	esp1;
	u32	ss1;
	u32	esp2;
	u32	ss2;
	u32	cr3;
	u32	eip;
	u32	flags;
	u32	eax;
	u32	ecx;
	u32	edx;
	u32	ebx;
	u32	esp;
	u32	ebp;
	u32	esi;
	u32	edi;
	u32	es;
	u32	cs;
	u32	ss;
	u32	ds;
	u32	fs;
	u32	gs;
	u32	ldt;
	u16	trap;
	u16	iobase;	/* I/Oλͼ��ַ���ڻ����TSS�ν��ޣ��ͱ�ʾû��I/O���λͼ */
	/*u8	iomap[2];*/
}TSS;


#define	reassembly(high, high_shift, mid, mid_shift, low)	\
	(((high) << (high_shift)) +				\
	 ((mid)  << (mid_shift)) +				\
	 (low))


/* GDT */
/* ���������� */
#define	ind_empty		0	// ��
#define	ind_code32		1	// �� LOADER �����Ѿ�ȷ���˵�.
#define	ind_data		2	// ��
#define	ind_video		3	// ��
#define ind_tss         4
#define ind_ldt_first   5

/* ѡ���� */
#define	sel_empty		   0		// ��
#define	sel_code32		0x08		// �� LOADER �����Ѿ�ȷ���˵�.
#define	sel_data	    0x10		// ��
#define	sel_video		(0x18+3)	// ��<-- RPL=3
#define sel_tss         0x20
#define sel_ldt_first   0x28

#define	SEL_KERNEL_CS	sel_code32
#define	SEL_KERNEL_DS	sel_data
#define	SEL_KERNEL_GS	sel_video


/* ÿ��������һ�������� LDT, ÿ�� LDT �е�����������: */
#define LDT_SIZE		2
/* descriptor indices in LDT */
#define INDEX_LDT_C             0
#define INDEX_LDT_RW            1


/* ����������ֵ˵�� */
#define	DA_32			0x4000	/* 32 λ��				*/
#define	DA_LIMIT_4K		0x8000	/* �ν�������Ϊ 4K �ֽ�			*/
#define	LIMIT_4K_SHIFT		  12
#define	DA_DPL0			0x00	/* DPL = 0				*/
#define	DA_DPL1			0x20	/* DPL = 1				*/
#define	DA_DPL2			0x40	/* DPL = 2				*/
#define	DA_DPL3			0x60	/* DPL = 3				*/
/* �洢������������ֵ˵�� */
#define	DA_DR			0x90	/* ���ڵ�ֻ�����ݶ�����ֵ		*/
#define	DA_DRW			0x92	/* ���ڵĿɶ�д���ݶ�����ֵ		*/
#define	DA_DRWA			0x93	/* ���ڵ��ѷ��ʿɶ�д���ݶ�����ֵ	*/
#define	DA_C			0x98	/* ���ڵ�ִֻ�д��������ֵ		*/
#define	DA_CR			0x9A	/* ���ڵĿ�ִ�пɶ����������ֵ		*/
#define	DA_CCO			0x9C	/* ���ڵ�ִֻ��һ�´��������ֵ		*/
#define	DA_CCOR			0x9E	/* ���ڵĿ�ִ�пɶ�һ�´��������ֵ	*/
/* ϵͳ������������ֵ˵�� */
#define	DA_LDT			0x82	/* �ֲ��������������ֵ			*/
#define	DA_TaskGate		0x85	/* ����������ֵ				*/
#define	DA_386TSS		0x89	/* ���� 386 ����״̬������ֵ		*/
#define	DA_386CGate		0x8C	/* 386 ����������ֵ			*/
#define	DA_386IGate		0x8E	/* 386 �ж�������ֵ			*/
#define	DA_386TGate		0x8F	/* 386 ����������ֵ			*/

/* ѡ��������ֵ˵�� */
/* ����, SA_ : Selector Attribute */
#define	SA_RPL_MASK	0xFFFC
#define	SA_RPL0		0
#define	SA_RPL1		1
#define	SA_RPL2		2
#define	SA_RPL3		3

#define	SA_TI_MASK	0xFFFB
#define	SA_TIG		0
#define	SA_TIL		4


//���Ե�ַ ��> �����ַ
#define vir2phys(seg_base, vir) (u32)(((u32)seg_base) + (u32)(vir))
/* seg:off -> linear addr */
#define seg2linear seg2phy
#define makelinear(seg,off) (u32)(((u32)(seg2linear(seg))) + (u32)(off))

/* �ж����� 0~16 intel ����*/
#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT		0x3
#define	INT_VECTOR_OVERFLOW		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT		0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT		0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
#define	INT_VECTOR_COPROC_ERR		0x10

/* �ж����� */
#define	INT_VECTOR_IRQ0			0x20 //ʱ���ж� ��Ƭ���
#define	INT_VECTOR_IRQ8			0x28 // ��Ƭ���

//ϵͳ�ж�
#define INT_VECTOR_SYS_CALL     0x90

#endif
