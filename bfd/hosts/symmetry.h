/* Symmetry running either dynix 3.1 (bsd) or ptx (sysv).  */
#include "hosts/std-host.h"

#include <string.h>

#define NBPG 4096
#define UPAGES 1

#ifdef _SEQUENT_
/* ptx */
#define HOST_TEXT_START_ADDR 0
#define HOST_STACK_END_ADDR 0x3fffe000
#else
/* dynix */
#define HOST_TEXT_START_ADDR 0x1000
#define HOST_STACK_END_ADDR 0x3ffff000
#endif

#define HOST_STACK_FILEPOS (NBPG * UPAGES) + (NBPG * (u.u_dsize - u.u_tsize))

/* This would be more clean as a separate bfd, but I doubt it's used for
   corss-debugging anyway.  */
#include "aout/dynix3.h"
