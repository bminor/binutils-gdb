/* Architecture independant LynxOS host support for 2.2 */

#ifndef hosts_lynx_h
#define hosts_lynx_h

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <sys/conf.h>
#include <sys/kernel.h>
/* sys/kernel.h should define this, but doesn't always, sigh. */
#ifndef __LYNXOS
#define __LYNXOS
#endif
#include <sys/mem.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/itimer.h>
#include <sys/file.h>
#include <sys/proc.h>

#define	HOST_PAGE_SIZE		NBPG
#define	HOST_TEXT_START_ADDR	USRTEXT

#define HOST_LYNX

#include "fopen-same.h"

#define FPRINTF_ALREADY_DECLARED

#undef I386			/* Lynx defines this for some reason */

/* Lynx has calloc, but no cfree, and gprof uses it. */

#define cfree free

#endif /* hosts_lynx_h */
