#include <ansidecl.h>
#include <fcntl.h>
#include <errno.h>
extern int errno;		/* <errno.h> forgets to do this... */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>
#include <machine/machparam.h>

#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif

#define SEEK_SET 0
#define SEEK_CUR 1

extern PTR  EXFUN(malloc, (unsigned));
extern PTR  EXFUN(realloc, (PTR, unsigned));
extern void EXFUN(free, (PTR));

#define	HAVE_STRERROR

#define	HOST_PAGE_SIZE		NBPG
#define	HOST_SEGMENT_SIZE	NBPG
#define	HOST_MACHINE_ARCH	bfd_arch_i386
#define	HOST_TEXT_START_ADDR	0x10000		/* By inspection */
#define	HOST_STACK_END_ADDR	KERNBASE

#include "fopen-same.h"
