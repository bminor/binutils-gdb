/* $Id$ */

#include <sys/types.h>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

#define USG

#ifdef __GNUC__
#define alloca __builtin_alloca
#endif


#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#define SEEK_SET 0
#define SEEK_CUR 1

#include <memory.h>
#define bcmp(b1,b2,len)         memcmp(b1,b2,len)
#define bcopy(src,dst,len)      memcpy(dst,src,len)
#define bzero(s,n)              memset(s,0,n)

#include <string.h>
#define index(s,c)              strchr(s,c)
#define rindex(s,c)             strrchr(s,c)


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

