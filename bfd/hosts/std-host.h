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
#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifdef STDC_HEADERS
#include <stdlib.h>
/*#include <string.h>*/
#else
extern char *EXFUN(mktemp,(CONST char*));
extern int   EXFUN(fflush,(FILE *));
extern int   EXFUN(write,(int, CONST char *, int));
extern void  EXFUN(abort,(void));
extern int   EXFUN(close,(int));
extern int   EXFUN(fcntl,(int des, int cmd, int e));
extern int   EXFUN(fprintf,(FILE *,char *,...));
extern int   EXFUN(printf,(char *,...));
extern int   EXFUN(qsort,(void *data,int els, int siz, int func()));
extern void  EXFUN(exit,(int));
extern int   EXFUN(fseek,(FILE*, int, int));
extern int   EXFUN(fclose,(FILE*));
extern void  EXFUN(bcopy,(char*,char*,int));
extern int   EXFUN(bcmp,(char *, char *, int));
extern void  EXFUN(bzero,(char *, int));
extern PTR   EXFUN(memset,(PTR, int, int));
extern void  EXFUN(puts,(CONST char*));
extern void  EXFUN(fputs,(CONST char*,FILE*));
extern int   EXFUN(rmdir,(CONST char *));
extern int   EXFUN(getuid,(void));
extern int   EXFUN(getgid,(void));
extern int   EXFUN(rename,(CONST char *, CONST char*));
extern int   EXFUN(getuid,());
extern int   EXFUN(getgid,());
extern void  EXFUN(perror,(CONST char *));
#ifndef DONTDECLARE_MALLOC
extern PTR   EXFUN(malloc,(unsigned));
extern PTR   EXFUN(realloc, (PTR, unsigned));
#endif

#ifndef __GNUC__
extern PTR   EXFUN(memcpy,(PTR,CONST PTR,unsigned int));
#else
/* char * EXFUN( memcpy,(char *,CONST char *,unsigned int)); */
#endif

#ifdef __STDC__
extern void EXFUN(free,(PTR));
#else
extern int free();
#endif

extern char * strchr();
extern char *getenv();
extern char *memchr();
extern char *strrchr();
extern int chmod();
extern int fread();
extern int fstat();
extern int fwrite();
extern int sscanf();
extern int stat();
extern int strtol();

extern char *strrchr();
extern char *ctime();
extern int _flsbuf();
extern int fclose();
extern int time();
extern int utimes();
extern int vfprintf();
extern long atol();
extern char *getenv();
extern int fputc();
extern int unlink();
#endif /* __STDC__ */

/* EXACT TYPES */
#ifndef int8e_type
#define int8e_type char
#endif
#ifndef uint8e_type
#define uint8e_type unsigned char
#endif
#ifndef int16e_type
#define int16e_type short
#endif
#ifndef uint16e_type
#define uint16e_type unsigned short
#endif
#ifndef int32e_type
#define int32e_type int
#endif
#ifndef uint32e_type
#define uint32e_type unsigned int
#endif
#ifndef uint64e_type
#ifdef __GNUC__
#define uint64e_type unsigned long long
#define int64e_type struct long long
#define uint64_typeLOW(x) (uint32_type)(((x) & 0xffffffff))
#define uint64_typeHIGH(x) (uint32_type)(((x) >> 32) & 0xffffffff)
#else
typedef struct  uint64e_struct {
  uint32e_type low, high;
};
#define uint64e_type struct uint64e_struct
#define int64e_type struct uint64e_struct
#define uint64_typeLOW(x) (uint32_type)(((x).low))
#define uint64_typeHIGH(x) (uint32_type)(((x).high))
#endif
#endif

/* CORRECT SIZE OR GREATER */
#ifndef int8_type
#define int8_type int8e_type
#endif
#ifndef uint8_type
#define uint8_type uint8e_type
#endif
#ifndef int16_type
#define int16_type int16e_type
#endif
#ifndef uint16_type
#define uint16_type uint16e_type
#endif
#ifndef int32_type
#define int32_type int32e_type
#endif
#ifndef uint32_type
#define uint32_type uint32e_type
#endif
#ifndef int64_type
#define int64_type int64e_type
#endif
#ifndef uint64_type
#define uint64_type uint64e_type
#endif

#ifndef BYTES_IN_PRINTF_INT
#define BYTES_IN_PRINTF_INT 4
#endif

#include "fopen-same.h"
