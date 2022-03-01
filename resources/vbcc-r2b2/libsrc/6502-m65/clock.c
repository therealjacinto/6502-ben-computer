#include <time.h>
#include "libsys.h"

clock_t clock(void)
{
    unsigned long  l=__rdtim();
    unsigned char tens,secs,mins;
    tens=l&255;
    secs=(l>>8)&255;
    secs=(secs&15)+(secs>>4)*10;
    mins=(l>>16)&255;
    mins=(mins&15)+(mins>>4)*10;
    return (tens*6+secs*60)+(long)mins*3600;
}

