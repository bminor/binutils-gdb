/* Stratus FTX2  host system */
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

#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#define SEEK_SET 0
#define SEEK_CUR 1

#define POSIX_UTIME
#define HAVE_PROCFS	/* This host has /proc support */

extern void	abort	PARAMS ((void));
extern int	close	PARAMS ((int));
extern void	exit	PARAMS ((int));
extern int	fclose	PARAMS ((FILE*));
extern void	free	PARAMS ((PTR));
extern int	fseek	PARAMS ((FILE*, long, int));
extern int	getgid	PARAMS (());
extern int	getuid	PARAMS (());
extern PTR	malloc	PARAMS ((unsigned));
extern void	perror	PARAMS ((CONST char *));
extern int	qsort	PARAMS ((void *data, int els, int siz, int func()));
extern PTR	realloc	PARAMS ((PTR, unsigned));

extern char *getenv();
extern int chmod();
extern int fstat();
extern int stat();
extern int strtol();

extern char *ctime();
extern int _flsbuf();
extern int fclose();
extern int utimes();
extern int vfprintf();
extern long atol();
extern int fputc();
extern int unlink();

#include "fopen-same.h"
