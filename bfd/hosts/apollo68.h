#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <strings.h>
#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#ifndef	SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#endif

#ifndef	MAXPATHLEN
#define	MAXPATHLEN	1024
#endif	/* MAXPATHLEN */

#ifndef DONTDECLARE_MALLOC
extern PTR  EXFUN(malloc,(unsigned));
extern PTR  EXFUN(realloc, (PTR, unsigned));
extern void EXFUN( free,(PTR));
#endif

#include "fopen-same.h"
