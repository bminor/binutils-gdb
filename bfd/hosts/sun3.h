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

extern PROTO(int, abort,(void));
extern PROTO(int, close,(int));
extern PROTO(int, fcntl,(int des, int cmd));
extern PROTO(int, fprintf,(FILE *,char *,...));
extern PROTO(int, printf,(char *,...));
extern PROTO(int, qsort,(void *data,int els, int siz, int func()));
extern PROTO(int, exit,(int));
extern PROTO(int, fseek,(FILE*, int, int));
extern PROTO(int, fclose,(FILE*));
extern PROTO(void, bcopy,(char*,char*,int));
extern PROTO(int, bcmp,(char *, char *, int));
extern PROTO(void, bzero,(char *, int));
extern char * strchr();
extern PROTO(void, perror,(char *));
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
void free();
char *malloc();
char *realloc();
PROTO (void, perror, (char *s));
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

