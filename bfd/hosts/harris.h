#include <errno.h>
#include <sys/types.h>
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

/* EXACT TYPES */
typedef char int8e_type;
typedef unsigned char uint8e_type;
typedef short int16e_type;
typedef unsigned short uint16e_type;
typedef int int32e_type;
typedef unsigned int uint32e_type;

/* CORRECT SIZE OR GREATER */
typedef char int8_type;
typedef unsigned char uint8_type;
typedef short int16_type;
typedef unsigned short uint16_type;
typedef int int32_type;
typedef unsigned int uint32_type;
#include "fopen-same.h"
