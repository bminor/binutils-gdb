#include <ansidecl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <strings.h>
#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#ifndef	SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#endif

#ifndef	MAXPATHLEN
#define	MAXPATHLEN	1024
#endif	/* MAXPATHLEN */

#ifndef	DONTDECLARE_MALLOC
extern PTR  malloc	PARAMS ((unsigned));
extern PTR  realloc	PARAMS ((PTR, unsigned));
extern void free	PARAMS ((PTR));
#endif

#include "fopen-same.h"

/* From ANSI C <stddef.h>, which we can't depend upon the existence of */
#ifndef	offsetof
#define	offsetof(type,memb)	((size_t)&(((type *)0)->memb))
#endif
