/* SVR4 Unix host system */
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

extern PROTO(int,  abort,(void));
extern PROTO(int,  close,(int));
extern PROTO(void, exit,(int));
extern PROTO(int,  fclose,(FILE*));
extern PROTO(void, free,(PTR));
extern PROTO(int,  fseek,(FILE*, long, int));
extern PROTO(int,  getgid,());
extern PROTO(int,  getuid,());
extern PROTO(PTR,  malloc,(unsigned));
extern PROTO(void, perror,(CONST char *));
extern PROTO(int,  qsort,(void *data,int els, int siz, int func()));
extern PROTO(PTR,  realloc, (PTR, unsigned));

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
