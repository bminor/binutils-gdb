#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>
#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
 
#ifndef DONTDECLARE_MALLOC
extern PTR  EXFUN(malloc,(unsigned));
extern PTR  EXFUN(realloc, (PTR, unsigned));
#endif
extern int  EXFUN(abort,(void));
extern int  EXFUN(free,(PTR));
extern void EXFUN(bcopy,(char*,char*,int));
extern void EXFUN(exit,(int));
extern void EXFUN(bzero,(char *, int));
extern int strtol();
#define NO_STDARG 1
 
#include "fopen-same.h"
