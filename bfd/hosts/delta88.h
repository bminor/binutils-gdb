#include <ansidecl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#include <stdio.h>

#define USE_UTIME

#ifndef DONTDECLARE_MALLOC
extern PTR  EXFUN(malloc,(unsigned));
extern PTR  EXFUN(realloc, (PTR, unsigned));
#endif
extern int  EXFUN(abort,(void));
extern int  EXFUN(free,(PTR));
extern void EXFUN(bcopy,(char*,char*,int));
extern void EXFUN(exit,(int));
extern int  EXFUN(strtol,(char *, char **, int));
extern void EXFUN(bzero,(char *, int));
#define NO_STDARG 1
extern int  EXFUN(abort,(void));
extern int  EXFUN(close,(int));
#if 0
/* Conflicts with system headers on SVR4.  */
extern int  EXFUN(fcntl,(int des, int cmd, int e));
#endif
extern int  EXFUN(qsort,(void *data,int els, int siz, int func()));
#if 0
/* Conflicts with system headers on SVR4.  */
extern int  EXFUN(fseek,(FILE*, int, int));
#endif
extern int  EXFUN(fclose,(FILE*));
extern void EXFUN(bcopy,(char*,char*,int));
extern int  EXFUN(bcmp,(char *, char *, int));
extern void EXFUN(bzero,(char *, int));
extern void EXFUN(perror,(CONST char *));
extern PTR  EXFUN(memchr,(const void *, int, unsigned ));
#if 0
/* Conflicts with system headers on SVR4.  */
extern unsigned short EXFUN(getuid,(void));
extern unsigned short EXFUN(getgid,(void));
#endif

extern char * strchr();

extern char *getenv();
extern char *strrchr();
extern int chmod();
#if 0
/* Conflicts with system headers on SVR4.  */
extern int fread();
#endif
extern int fstat();
#if 0
/* Conflicts with system headers on SVR4.  */
extern int fwrite();
#endif
extern int sscanf();
extern int stat();
extern int strtol();
extern int fileno();
extern char *strrchr();
extern char *ctime();
extern int _flsbuf();
extern int fclose();
#if 0
/* Conflicts with system headers on SVR4.  */
extern int time();
#endif
extern int utime();
extern int vfprintf();
extern long atol();
extern char *getenv();
extern int fputc();
extern int unlink();


#include "fopen-same.h"
