#ifndef	hosts_mpw_H

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

#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#ifndef	SEEK_SET
#define SEEK_SET 0
#endif
#ifndef	SEEK_CUR
#define SEEK_CUR 1
#endif
/*#include <string.h>*/

#ifndef	BYTES_IN_PRINTF_INT
#define BYTES_IN_PRINTF_INT 4
#endif

/* Binary files have different characteristics; for instance, no cr/nl
   translation. */

#include "fopen-bin.h"

#include <stddef.h>

#ifdef MPW_C

#undef  __PTR_TO_INT
#define __PTR_TO_INT(P) ((int)(P))
#undef __INT_TO_PTR
#define __INT_TO_PTR(P) ((char *)(P))

#endif

typedef short ino_t;
typedef short dev_t;

struct stat {
	ino_t st_ino;
	dev_t st_dev;
	int st_mode;
	int st_size;
	int st_uid;
	int st_gid;
	int st_mtime;
};

#define S_ISREG(m) (0)
#define S_ISDIR(m) (0)

#define NO_FCNTL

#ifdef __STDIO__
FILE *mpw_fopen ();
#endif
int mpw_fstat ();
int mpw_fseek ();
void mpw_abort ();

/* Map these standard functions to improved versions in libiberty. */

#define fopen mpw_fopen
#define fstat mpw_fstat
#define fseek mpw_fseek
#define abort mpw_abort

/* Define as macros so as to mask the previous enum. */

#ifndef BFD_TRUE_FALSE
#ifndef false
#define false ffalse
#endif
#ifndef true
#define true ttrue
#endif
#endif

#define TRUE_FALSE_ALREADY_DEFINED

#define LOSING_TOTALLY

#define hosts_mpw_H
#endif
