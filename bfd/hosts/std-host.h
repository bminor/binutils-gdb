#ifndef	hosts_std_host_H
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>

#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#ifndef	SEEK_SET
#define SEEK_SET 0
#endif
#ifndef	SEEK_CUR
#define SEEK_CUR 1
#endif
#ifdef STDC_HEADERS
#include <stdlib.h>
/*#include <string.h>*/
#else
extern char *mktemp ();
extern PTR   memset ();

#ifndef	DONTDECLARE_MALLOC
extern PTR   malloc ();
extern PTR   realloc ();
#endif

#ifndef	__GNUC__
extern PTR   memcpy ();
#else
/* char * memcpy (); */
#endif

#ifdef __STDC__
extern void free ();
#else
extern int free();
#endif

extern char * strchr();
extern char *getenv();
extern PTR memchr();
extern char *strrchr();

extern char *strrchr();
extern char *ctime();
extern long atol();
extern char *getenv();
#endif /* STDC_HEADERS */

#ifndef	BYTES_IN_PRINTF_INT
#define BYTES_IN_PRINTF_INT 4
#endif

#include "fopen-same.h"
#define hosts_std_host_H
#endif

#ifdef	STDC_HEADERS
#include <stddef.h>
#endif /* STDC_HEADERS */
