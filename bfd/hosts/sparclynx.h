/* Sparc running LynxOS */

#ifndef hosts_sparclynx_h
#define hosts_sparclynx_h

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <sys/conf.h>
#include <sys/kernel.h>
#include <sys/mem.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/itimer.h>
#include <sys/file.h>
#include <sys/proc.h>

#define	HOST_PAGE_SIZE		NBPG
#define	HOST_MACHINE_ARCH	bfd_arch_sparc
#define	HOST_TEXT_START_ADDR	USRTEXT

#define HOST_LYNX

#include "fopen-same.h"

/* Lynx has calloc, but no cfree, and gprof uses it. */

#define cfree free

#endif /* hosts_sparclynx_h */
