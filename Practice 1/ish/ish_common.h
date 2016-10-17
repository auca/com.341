#ifndef ISH_COMMON_H
#define ISH_COMMON_H

#ifdef ISH_USE_STDLIB
    #include <stdlib.h>
#else
    #include "ish_syscalls.h"
#endif

#define ish_check(value) do { if (!(value)) exit(-1); } while(0)

#endif
