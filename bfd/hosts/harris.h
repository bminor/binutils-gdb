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
/* Macros for the 'type' part of an fopen, freopen or fdopen. 
	<Read|Write>[Update]<Binary file><text file>
 */
#define FOPEN_RB	"r"
#define FOPEN_WB 	"w"
#define FOPEN_AB 	"a"
#define FOPEN_RUB 	"r+"
#define FOPEN_WUB 	"w+"
#define FOPEN_AUB 	"a+"
#define FOPEN_RT	"r"
#define FOPEN_WT 	"w"
#define FOPEN_AT 	"a"
#define FOPEN_RUT 	"r+"
#define FOPEN_WUT 	"w+"
#define FOPEN_AUT 	"a+"
