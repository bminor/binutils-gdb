/* i386 AIX 1.2.x host system */
/* From Minh Tran-Le <TRANLE@INTELLICORP.COM>.  */

#include <ansidecl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>
#include <stdlib.h>

#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#define SEEK_SET 0
#define SEEK_CUR 1

#define POSIX_UTIME

extern void	abort	PARAMS ((void));
extern int	close	PARAMS ((int));
extern void	exit	PARAMS ((int));
extern int	fclose	PARAMS ((FILE*));
extern void	free	PARAMS ((PTR));
extern int	fseek	PARAMS ((FILE*, long, int));
extern void	perror	PARAMS ((CONST char *));

extern char *getenv();
extern int chmod();
extern int fstat();
extern int stat();

extern char *ctime();
extern int _flsbuf();
extern int fclose();
extern int utimes();
extern int vfprintf();
extern long atol();
extern int fputc();
extern int unlink();

#include "fopen-same.h"
