#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>

#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#define SEEK_SET 0
#define SEEK_CUR 1

#ifndef DONTDECLARE_MALLOC
extern PTR  EXFUN(malloc,(unsigned));
extern PTR  EXFUN(realloc, (PTR, unsigned));
extern int  EXFUN(free,(PTR));
#endif
extern int  EXFUN(abort,(void));
extern void EXFUN(bcopy,(char*,char*,int));
extern void EXFUN(exit,(int));
extern void EXFUN(bzero,(char *, int));
extern int strtol();
 
#include <machine/param.h>
#include <machine/vmparam.h>
#define	HOST_PAGE_SIZE		(NBPG*CLSIZE)
#define	HOST_MACHINE_ARCH	bfd_arch_vax

#define	HOST_TEXT_START_ADDR	USRTEXT
#define	HOST_STACK_END_ADDR	USRSTACK
#undef	HOST_BIG_ENDIAN_P

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
