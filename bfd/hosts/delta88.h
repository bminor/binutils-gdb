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
#define NO_STDARG 1



extern PROTO(int, abort,(void));
extern PROTO(int, close,(int));
extern PROTO(int, fcntl,(int des, int cmd, int e));


extern PROTO(int, qsort,(void *data,int els, int siz, int func()));
extern PROTO(unsigned short,getuid,(void));
extern PROTO(unsigned short,getgid,(void));
extern PROTO(int, fseek,(FILE*, int, int));
extern PROTO(int, fclose,(FILE*));
extern PROTO(void, bcopy,(char*,char*,int));
extern PROTO(int, bcmp,(char *, char *, int));
extern PROTO(void, bzero,(char *, int));
extern char * strchr();
extern PROTO(void, perror,(CONST char *));
extern char *getenv();
extern PROTO(PTR, memchr,(const void *, int, unsigned ));
extern char *strrchr();
extern int chmod();
extern int fread();
extern int fstat();
extern int fwrite();
extern int sscanf();
extern int stat();
extern int strtol();
extern int fileno();
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
