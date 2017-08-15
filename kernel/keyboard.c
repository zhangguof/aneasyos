/*
name:keybord.c
By Tony
������������
tty��ʹ��
*/
#include "MyOs.h"
#include "keymap.h"

static KB_INPUT kb_in;  //һ�����뻺�� ʹ��ѭ������ʵ��
static	int	code_with_E0;
static	int	shift_l;	/* l shift state */
static	int	shift_r;	/* r shift state */
static	int	alt_l;		/* l alt state	 */
static	int	alt_r;		/* r left state	 */
static	int	ctrl_l;		/* l ctrl state	 */
static	int	ctrl_r;		/* l ctrl state	 */
static	int	caps_lock;	/* Caps Lock	 */
static	int	num_lock;	/* Num Lock	 */
static	int	scroll_lock;	/* Scroll Lock	 */
static	int	column;

static int	caps_lock;	/* Caps Lock	 */
static int	num_lock;	/* Num Lock	 */
static int	scroll_lock;	/* Scroll Lock	 */

static u8 get_byte_from_kbuf()
{

    u8 scan_code;
    while(kb_in.count <= 0) {}

    disable_int();
    scan_code = *(kb_in.p_tail);
    kb_in.p_tail++;
    if(kb_in.p_tail == kb_in.buf + KB_IN_BYTES)
    {
        kb_in.p_tail=kb_in.buf;
    }
    kb_in.count--;
    enable_int();

    return scan_code;

}


//����ʶ��󲿷ְ��� ����С���̺�num lock��û�д���
void keyboard_read(TTY* p_tty)
{
    u8 scan_code;
    char output[2];
    int make;  //true:make false : break

    u32 key=0;  //��һ����������ʾһ����
    u32* keyrow; //ָ��keymap[]��ĳ��
    // memset(output,0,2);

    if(kb_in.count > 0)
    {
        code_with_E0=0;
        scan_code = get_byte_from_kbuf();

        //���濪ʼ����ɨ����

        if(scan_code == 0xe1)
        {
            int i;
            u8 pausebrk_scode[] = {0xe1,0x1d,0x45,
                                   0xe1,0x9d,0xc5
                                  };
            int is_pausebrak =1 ;
            for(i=1; i<6; i++)
            {
                if(get_byte_from_kbuf() != pausebrk_scode[i])
                {
                    is_pausebrak=0;
                    break;
                }
            }
            if(is_pausebrak)
            {
                key=PAUSEBREAK;
            }
        }
        else if(scan_code == 0xe0)
        {
            scan_code = get_byte_from_kbuf();
            //disp_int(scan_code);

            /* PrintScreen ������ */
            if (scan_code == 0x2A)
            {
                if (get_byte_from_kbuf() == 0xE0)
                {
                    if (get_byte_from_kbuf() == 0x37)
                    {
                        key = PRINTSCREEN;
                        make = 1;
                    }
                }
            }
            /* PrintScreen ���ͷ� */
            if (scan_code == 0xB7)
            {
                if (get_byte_from_kbuf() == 0xE0)
                {
                    if (get_byte_from_kbuf() == 0xAA)
                    {
                        key = PRINTSCREEN;
                        make = 0;
                    }
                }
            }
            /* ����PrintScreen, ��ʱscan_codeΪ0xE0�������Ǹ�ֵ. */
            if (key == 0)
            {
                code_with_E0 = 1;
            }
            //disp_int(scan_code);
        }
        if(key!=PAUSEBREAK && key!=PRINTSCREEN)
        {
            make = (scan_code & FLAG_BREAK? FALSE : TRUE);

            //��λ����
            keyrow = &keymap[(scan_code & 0x7f) * MAP_COLS];
            column = 0;
            if(shift_l || shift_r)
            {
                column=1;
            }
            if(code_with_E0)
            {
                column=2;
                code_with_E0=0;
            }

            key = keyrow[column];
            switch(key)
            {
            case SHIFT_L:
                shift_l = make;
                //key = 0;
                break;
            case SHIFT_R:
                shift_r = make;
                //key = 0;
                break;
            case CTRL_L:
                ctrl_l =make;
                //key = 0;
                break;
            case CTRL_R:
                ctrl_r = make;
               // key = 0;
                break;
            case ALT_R:
                alt_r = make;
               // key = 0;
                break;
            case ALT_L:
                alt_l = make;
               // key = 0;
                break;
            default:
                break;

            } //end of case
            if(make)
            {
                key |= shift_l? FLAG_SHIFT_L : 0;
                key |= shift_r? FLAG_SHIFT_R : 0;
                key |= ctrl_l?  FLAG_CTRL_L : 0;
                key |= ctrl_r?  FLAG_CTRL_R : 0;
                key |= alt_l? FLAG_ALT_L : 0;
                key |= alt_r? FLAG_ALT_R : 0;

                //disp_str("\nget a char:");
               // disp_int(key);
                in_process(p_tty,key);
            }

        }//end of if

    }
}

void keyboard_handler(int irq)
{
    //disp_str("now in keybord handler\n");
    u8 scan_code = in_byte(KB_DATA);
    //disp_int(scan_code);

    if(kb_in.count < KB_IN_BYTES)
    {
        *(kb_in.p_head) = scan_code;
        kb_in.p_head++;
        if(kb_in.p_head == kb_in.buf + KB_IN_BYTES)
        {
            kb_in.p_head=kb_in.buf;
        }
        kb_in.count++;
    }

}

void init_keyboard()
{
    kb_in.count=0;
    kb_in.p_head = kb_in.p_tail = kb_in.buf;
    alt_l=alt_r=ctrl_l=ctrl_r=shift_l=shift_r=0;

    put_irq_hangler(KEYBOARD_IRQ, keyboard_handler); //���ü����ж�
    enable_irq(KEYBOARD_IRQ);  //���������ж�
}


