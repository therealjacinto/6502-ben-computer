#include <time.h>
#include "libsys.h"

clock_t clock(void)
{
    return __rdtim();
}

