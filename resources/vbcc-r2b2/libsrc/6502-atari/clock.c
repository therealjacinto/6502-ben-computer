#include <time.h>
#include "libsys.h"

clock_t __clock(void)="\tzpage\tbtmp0\n"
                      "cloop:\n"
                      "\tlda\t20\n"
                      "\tsta\tbtmp0\n"
                      "\tldx\t19\n"
                      "\tstx\tbtmp0+1\n"
                      "\tlda\t18\n"
                      "\tsta\tbtmp0+2\n"
                      "\tcpx\t19\n"
                      "\tbne\tcloop\n"
                      "\tlda\t#0\n"
                      "\tsta\tbtmp0+3\n";

clock_t clock(void)
{
  return __clock();
}

