#include <ansidecl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <ctype.h>
#include <string.h>
#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#define SEEK_SET 0
#define SEEK_CUR 1

#ifndef	MAXPATHLEN
#define	MAXPATHLEN	1024
#endif	/* MAXPATHLEN */

#define	POSIX_UTIME

/* Some things that need to be defined in order to make code written for
   BSD Unix compile under System V Unix.  */

#include <memory.h>
#define bcmp(b1,b2,len)		memcmp(b1,b2,len)
#define bcopy(src,dst,len)	memcpy(dst,src,len)
#define bzero(s,n)		memset(s,0,n)

#include <string.h>

#ifndef	DONTDECLARE_MALLOC
extern PTR  malloc	PARAMS ((unsigned));
extern PTR  realloc	PARAMS ((PTR, unsigned));
extern void free	PARAMS ((PTR));
#endif

#include "fopen-same.h"
