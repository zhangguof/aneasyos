#include "MyOs.h"

void clock_handler(int irq)
{
    ticks++;
    p_proc_ready->ticks--;
    p_proc_ready->runtime++;
  //  disp_str("#");


    if(k_reenter != 0)
    {
       // disp_str("!");
        return;
    }
    if(p_proc_ready->ticks > 0)
    {
        return;
    }

    //seched();
    schedule();
}

void milli_delay(int milli_sec)
{
    int t=get_ticks();
    while( ((get_ticks()-t) * 1000 / HZ) < milli_sec){}
}
