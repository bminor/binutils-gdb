/* Host definition file for Sun-4 running with gcc, using "long long"
   for addresses, to handle 64-bit target systems. */
#include <ansidecl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>
#include <alloca.h>
#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#define SEEK_SET 0
#define SEEK_CUR 1

/* Make the basic types 64-bit quantities on the host */
#define	HOST_64_BIT	long long

extern int  EXFUN(abort,(void));
extern int  EXFUN(close,(int));
extern int  EXFUN(fcntl,(int des, int cmd, int e));
extern int  EXFUN(fprintf,(FILE *,char *,...));
extern int  EXFUN(printf,(char *,...));
extern int  EXFUN(qsort,(void *data,int els, int siz, int func()));
extern void EXFUN(exit,(int));
extern int  EXFUN(fseek,(FILE*, int, int));
extern int  EXFUN(fclose,(FILE*));
extern void EXFUN(bcopy,(char*,char*,int));
extern int  EXFUN(bcmp,(char *, char *, int));
extern void EXFUN(bzero,(char *, int));
extern PTR  EXFUN(memset,(PTR, int,unsigned int));
#ifndef DONTDECLARE_MALLOC
extern PTR  EXFUN(malloc,(unsigned));
extern PTR  EXFUN(realloc, (PTR, unsigned));
#endif

#ifndef __GNUC__
 PTR EXFUN(memcpy,(PTR,CONST PTR,unsigned int));
#else
/* char * EXFUN(memcpy,(char *,CONST char *,unsigned int)); */
#endif

extern int EXFUN(getuid,());
extern int EXFUN(getgid,());
extern char * strchr();
extern void EXFUN(perror,(CONST char *));
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

extern int EXFUN(free,(PTR));


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


#ifdef __GNUC__
typedef unsigned long long uint64e_type;

#else
typedef struct {
  unsigned long low, high;
} uint64e_type;

#endif

#ifdef __GNUC__
typedef unsigned long long uint64_type;
typedef long long int64_type;
#else
typedef struct {
  unsigned long low, high;
} uint64_type;

typedef struct {
  unsigned long low, high;
} int64_type;

#endif

#define BYTES_IN_PRINTF_INT 4
#ifndef __GNUC__
#define uint64_typeLOW(x) (unsigned long)(((x).low))
#define uint64_typeHIGH(x) (unsigned long)(((x).high))
#else
#define uint64_typeLOW(x) (unsigned long)(((x) & 0xffffffff))
#define uint64_typeHIGH(x) (unsigned long)(((x) >> 32) & 0xffffffff)
#endif

#include "fopen-same.h"
