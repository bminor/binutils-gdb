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
extern int   fflush ();
extern int   write ();
extern void  abort ();
extern int   close ();
extern void  exit ();
extern int   fseek ();
extern int   fclose ();
extern void  bcopy ();
extern int   bcmp ();
extern void  bzero ();
extern PTR   memset ();

/* These used to be declared to return void.  As far as I know that is just
   wrong; are there any systems for which they actually don't return a
   value (EOF for error, something else for success).  */
extern int  puts ();
extern int  fputs ();

extern int   rmdir ();
extern int   getuid ();
extern int   getgid ();
extern int   rename ();
extern void  perror ();
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
extern int chmod();
extern int fstat();
extern int stat();

extern char *strrchr();
extern char *ctime();
extern int _flsbuf();
extern int fclose();
/*extern int time(); this causes error with time.h on mach3 */
extern int utimes();
extern int vfprintf();
extern long atol();
extern char *getenv();
extern int fputc();
extern int unlink();
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
