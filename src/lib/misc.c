#include "MyOs.h"

/*****************************************************************************
 *                                spin
 *****************************************************************************/
 void spin(char * func_name)
{
	printf("\nspinning in %s ...\n", func_name);
	while (1) {}
}


/*****************************************************************************
 *                           assertion_failure
 *************************************************************************//**
 * Invoked by assert().
 *
 * @param exp       The failure expression itself.
 * @param file      __FILE__
 * @param base_file __BASE_FILE__
 * @param line      __LINE__
 *****************************************************************************/
 void assertion_failure(char *exp, char *file, char *base_file, int line)
{
	printf("%c  assert(%s) failed: file: %s, base_file: %s, ln%d",
	       MAG_CH_ASSERT,
	       exp, file, base_file, line);

	/**
	 * If assertion fails in a TASK, the system will halt before
	 * printl() returns. If it happens in a USER PROC, printl() will
	 * return like a common routine and arrive here.
	 * @see sys_printx()
	 *
	 * We use a forever loop to prevent the proc from going on:
	 */
	spin("assertion_failure()");

	/* should never arrive here */
        __asm__ __volatile__("ud2");
}





void panic(const char* fmt,...)
{
    int i;
    char buf[256];
    va_list arg=(va_list)((char* )&fmt + 4);
    i = vsprintf(buf,fmt,arg);
    printf("%c !!panic!! %s",MAG_CH_PANIC,buf);

    __asm__ __volatile__("ud2");
}
