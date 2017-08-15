/*======================================================================*
                            init_8259A
                            8259aоƬ�ĳ�ʼ��
 *======================================================================*/

#include "MyOs.h"

void put_irq_hangler(int irq, irq_handler handler)
{
    disable_irq(irq);
    irq_table[irq]=handler;
}

/*======================================================================*
                           spurious_irq
 *======================================================================*/
void spurious_irq(int irq)
{
	disp_str("spurious_irq: ");
	disp_int(irq);
	disp_str("\n");
}

void init_8259A()
{
    int i;
    for(i=0;i<NR_IRQ;++i)
    {
        irq_table[i] = spurious_irq;  //8259aĬ���жϴ���
    }

	out_byte(INT_M_CTL,	0x11);			// Master 8259, ICW1.
	out_byte(INT_S_CTL,	0x11);			// Slave  8259, ICW1.
	out_byte(INT_M_CTLMASK,	INT_VECTOR_IRQ0);	// Master 8259, ICW2. ���� '��8259' ���ж���ڵ�ַΪ 0x20.
	out_byte(INT_S_CTLMASK,	INT_VECTOR_IRQ8);	// Slave  8259, ICW2. ���� '��8259' ���ж���ڵ�ַΪ 0x28
	out_byte(INT_M_CTLMASK,	0x4);			// Master 8259, ICW3. IR2 ��Ӧ '��8259'.
	out_byte(INT_S_CTLMASK,	0x2);			// Slave  8259, ICW3. ��Ӧ '��8259' �� IR2.
	out_byte(INT_M_CTLMASK,	0x1);			// Master 8259, ICW4.
	out_byte(INT_S_CTLMASK,	0x1);			// Slave  8259, ICW4.

    //���������ж�
	out_byte(INT_M_CTLMASK,	0xFF);	// Master 8259, OCW1.
	out_byte(INT_S_CTLMASK,	0xFF);	// Slave  8259, OCW1.


}


