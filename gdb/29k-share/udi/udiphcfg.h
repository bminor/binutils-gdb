/* This file just picks the correct udiphxxx.h depending on the host */
/* The two hosts that are now defined are UNIX and MSDOS */

/*
 * For the moment, we will default to BSD_IPC; this might change if/when
 * another type of IPC (Mach? SysV?) is implemented.
 */

#ifdef DOS_IPC
#include "udiphdos.h"
#else
/*#ifdef BSD_IPC */
#include "udiphunix.h"
#endif

