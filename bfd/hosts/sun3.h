#include <ansidecl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>
#include <stdlib.h>
#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#define SEEK_SET 0
#define SEEK_CUR 1

extern int	close	PARAMS ((int));
extern int	fprintf	PARAMS ((FILE *,CONST char *,...));
extern int	fclose	PARAMS ((FILE*));
extern void	bcopy	PARAMS ((char*,char*,int));
extern int	bcmp	PARAMS ((char *, char *, int));
extern void	bzero	PARAMS ((char *, int));
extern void	perror	PARAMS ((CONST char *));
extern char * strchr();
extern char *memchr();
extern char *strrchr();
extern int chmod();
extern int fstat();
extern int stat();
extern int strtol();

extern char *strrchr();
extern char *ctime();
extern int _flsbuf();
extern int fclose();
extern int utimes();
extern int vfprintf();
extern long atol();
extern char *getenv();
extern int fputc();
extern int unlink();


#include "fopen-same.h"
