/*
*klib.c
*各种库函数
*/
#include "MyOs.h"

#define BCD2BIN(a) ((a)/16*10+(a&0xF))
#define CURRENT_YEAR (2017)


int getpid()
{
    MESSAGE msg;
	msg.type	= GET_PID;

	send_recv(BOTH, TASK_SYS, &msg);
	assert(msg.type == SYSCALL_RET);

	return msg.PID;
    //return p_proc_ready-proc_table;
}


/*****************************************************************************
 *                                get_boot_params
 *****************************************************************************/
/**
 * <Ring 0~1> The boot parameters have been saved by LOADER.
 *            We just read them out.
 *
 * @param pbp  Ptr to the boot params structure
 *****************************************************************************/
 void get_boot_params(struct boot_params * pbp)
{
	/**
	 * Boot params should have been saved at BOOT_PARAM_ADDR.
	 * @see include/load.inc boot/loader.asm boot/hdloader.asm
	 */
	int * p = (int*)BOOT_PARAM_ADDR;
	assert(p[BI_MAG] == BOOT_PARAM_MAGIC);

	pbp->mem_size = p[BI_MEM_SIZE];
	pbp->kernel_file = (unsigned char *)(p[BI_KERNEL_FILE]);

	/**
	 * the kernel file should be a ELF executable,
	 * check it's magic number
	 */
	//assert(memcmp(pbp->kernel_file, ELFMAG, SELFMAG) == 0);
}



/*======================================================================*
                               itoa
 *======================================================================*/
char * itoa(char * str, int num)/* 数字前面的 0 不被显示出来, 比如 0000B800 被显示成 B800 */
{
	char *	p = str;
	char	ch;
	int	i;
	t_bool	flag = FALSE;

	*p++ = '0';
	*p++ = 'x';

	if(num == 0){
		*p++ = '0';
	}
	else{
		for(i=28;i>=0;i-=4){
			ch = (num >> i) & 0xF;
			if(flag || (ch > 0)){
				flag = TRUE;
				ch += '0';
				if(ch > '9'){
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}

	*p = 0;

	return str;
}

 void disp_int(int input)
{
	char output[16];
	itoa(output, input);
	disp_str(output);
}

void delay(int time)
{
    int i,j,k;
    for(k=0;k<time;k++)
    {
        for(i=0;i<10;++i)
        for(j=0;j<10000;j++)
        {

        }
    }
}

/*****************************************************************************
 *                                memcmp
 *****************************************************************************/
/**
 * Compare memory areas.
 *
 * @param s1  The 1st area.
 * @param s2  The 2nd area.
 * @param n   The first n bytes will be compared.
 *
 * @return  an integer less than, equal to, or greater than zero if the first
 *          n bytes of s1 is found, respectively, to be less than, to match,
 *          or  be greater than the first n bytes of s2.
 *****************************************************************************/
 int memcmp(const void * s1, const void *s2, u32 n)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return (s1 - s2);
	}

	const char * p1 = (const char *)s1;
	const char * p2 = (const char *)s2;
	int i;
	for (i = 0; i < n; i++,p1++,p2++) {
		if (*p1 != *p2) {
			return (*p1 - *p2);
		}
	}
	return 0;
}

/*****************************************************************************
 *                                strcmp
 *****************************************************************************/
/**
 * Compare two strings.
 *
 * @param s1  The 1st string.
 * @param s2  The 2nd string.
 *
 * @return  an integer less than, equal to, or greater than zero if s1 (or the
 *          first n bytes thereof) is  found,  respectively,  to  be less than,
 *          to match, or be greater than s2.
 *****************************************************************************/
 int strcmp(const char * s1, const char *s2)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return (s1 - s2);
	}

	const char * p1 = s1;
	const char * p2 = s2;

	for (; *p1 && *p2; p1++,p2++) {
		if (*p1 != *p2) {
			break;
		}
	}

	return (*p1 - *p2);
}

 int strncmp(const char * s1, const char *s2, u32 size)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return (s1 - s2);
	}

	const char * p1 = s1;
	const char * p2 = s2;
	int i=0;
	for (; *p1 && *p2 && i < size;i++, p1++,p2++) {
		if (*p1 != *p2) {
			break;
		}
	}

	return (*p1 - *p2);
}

/*****************************************************************************
 *                                strcat
 *****************************************************************************/
/**
 * Concatenate two strings.
 *
 * @param s1  The 1st string.
 * @param s2  The 2nd string.
 *
 * @return  Ptr to the 1st string.
 *****************************************************************************/
 char * strcat(char * s1, const char *s2)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return 0;
	}

	char * p1 = s1;
	for (; *p1; p1++) {}

	const char * p2 = s2;
	for (; *p2; p1++,p2++) {
		*p1 = *p2;
	}
	*p1 = 0;

	return s1;
}

//get cpu info string...
//sizeof(buf)>=3*4*4=48b
void cpuid_info_str(char *buf)
{
	unsigned int op;
	op = 0x80000002;
	cpuid_info(op,buf);
	op = 0x80000003;
	buf = buf + 4*4;
	cpuid_info(op,buf);
	buf = buf + 4*4;
	op = 0x80000004;
	cpuid_info(op,buf);
}

u8 get_RTC_reg(int reg)
{
	out_byte(COMOS_RTC_ADD, reg);
	u8 r = in_byte(COMOS_RTC_DATA);
	return r;
}

int get_update_in_progress_flag() {
    out_byte(COMOS_RTC_ADD, 0x0A);
    return (in_byte(COMOS_RTC_DATA) & 0x80);
}
 

void read_rtc(DATE* date)
{
	int format = get_RTC_reg(0x0B);
	u8 second,minute,hour,day,month;
	u32 year;
	while (get_update_in_progress_flag());

	second =  get_RTC_reg(0x00);
	minute = get_RTC_reg(0x02);
	hour = get_RTC_reg(0x04);
	day = get_RTC_reg(0x07);
	month = get_RTC_reg(0x08);
	year = get_RTC_reg(0x09);

	if(!(format & 0x40))
	{
		//bcd mode
		second = BCD2BIN(second);
		minute = BCD2BIN(minute);
		hour = BCD2BIN(hour);
		day = BCD2BIN(day);
		month = BCD2BIN(month);
		year = BCD2BIN(year);
	}

 	//12hour format to 24 hour format
    if (!(format & 0x02) && (hour & 0x80)) {
            hour = ((hour & 0x7F) + 12) % 24;
      }
 
	year = CURRENT_YEAR/100*100 + year;

	date->second =  second;
	date->minute = minute;
	date->hour = hour;
	date->day = day;
	date->month = month;
	date->year = year;
}

u32 get_rtc_unxi_time()
{
	DATE date;
	read_rtc(&date);
	return mktime(&date);
}
