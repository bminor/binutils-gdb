/* MPW host-specific definitions. */

#ifndef	hosts_mpw_H
#define hosts_mpw_H

#ifndef MPW
#define MPW
#endif

/* MPW C is basically ANSI, but doesn't actually enable __STDC__,
   nor does it allow __STDC__ to be #defined. */

#ifndef ALMOST_STDC
#define ALMOST_STDC
#endif

#ifdef BFD_TRUE_FALSE
#ifndef false
#define false mpw_false
#endif
#ifndef true
#define true mpw_true
#endif
#endif

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>

#include <sys/stat.h>

#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#ifndef	SEEK_SET
#define SEEK_SET 0
#endif
#ifndef	SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef	BYTES_IN_PRINTF_INT
#define BYTES_IN_PRINTF_INT 4
#endif

/* Binary files have different characteristics; for instance, no cr/nl
   translation. */

#include "fopen-bin.h"

#include <stddef.h>

#include <spin.h>

#ifdef MPW_C

#undef  __PTR_TO_INT
#define __PTR_TO_INT(P) ((int)(P))
#undef __INT_TO_PTR
#define __INT_TO_PTR(P) ((char *)(P))

#endif

#define NO_FCNTL

int fstat ();

FILE *mpw_fopen ();
int mpw_fseek ();
void mpw_abort ();

/* Map these standard functions to improved versions in libiberty. */

#define fopen mpw_fopen
#define fseek mpw_fseek
#define abort mpw_abort

#ifndef TRUE_FALSE_ALREADY_DEFINED
#define TRUE_FALSE_ALREADY_DEFINED
#endif

#define POSIX_UTIME

#define LOSING_TOTALLY

/* Define this so that files will be closed before being unlinked. */

#define CLOSE_BEFORE_UNLINK

#endif /* hosts_mpw_H */
