/* Stratus FTX2  host system */

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>

#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#define SEEK_SET 0
#define SEEK_CUR 1

#define POSIX_UTIME
#define HAVE_PROCFS	/* This host has /proc support */

extern void EXFUN(abort,(void));
extern int  EXFUN(close,(int));
extern void EXFUN(exit,(int));
extern int  EXFUN(fclose,(FILE*));
extern void EXFUN(free,(PTR));
extern int  EXFUN(fseek,(FILE*, long, int));
extern int  EXFUN(getgid,());
extern int  EXFUN(getuid,());
extern PTR  EXFUN(malloc,(unsigned));
extern void EXFUN(perror,(CONST char *));
extern int  EXFUN(qsort,(void *data,int els, int siz, int func()));
extern PTR  EXFUN(realloc, (PTR, unsigned));

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
