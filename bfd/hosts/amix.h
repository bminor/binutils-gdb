/* Not sure why this is needed -- Fred Fish put it in */
#define USG

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

extern PROTO(int,  abort,(void));
extern PROTO(int,  close,(int));
extern PROTO(void, exit,(int));
extern PROTO(int,  fclose,(FILE*));
extern PROTO(int,  free,(PTR));
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
