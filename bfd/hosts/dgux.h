#include <errno.h>
#include <sys/types.h>
#include <sys/dg_c_generics.h>
#include <unistd.h>

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#include <stdio.h>

#ifndef DONTDECLARE_MALLOC
extern PROTO(PTR,malloc,(unsigned));
extern PROTO(PTR ,realloc, (PTR, unsigned));
#endif
extern PROTO(int, abort,(void));
extern PROTO(int, free,(PTR));
extern PROTO(void, bcopy,(char*,char*,int));
extern PROTO(void, exit,(int));
extern int strtol();
extern PROTO(void, bzero,(char *, int));
#define NO_STDARG 1



