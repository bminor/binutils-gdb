#include <ansidecl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#include <stdio.h>

#define USE_UTIME

#ifndef	DONTDECLARE_MALLOC
extern PTR	malloc	PARAMS ((unsigned));
extern PTR	realloc PARAMS ((PTR, unsigned));
#endif
extern int	abort	PARAMS ((void));
extern int	free	PARAMS ((PTR));
extern void	bcopy	PARAMS ((char*,char*,int));
extern void	exit	PARAMS ((int));
extern int	strtol	PARAMS ((char *, char **, int));
extern void	bzero	PARAMS ((char *, int));
#define NO_STDARG 1
extern int	abort	PARAMS ((void));
extern int	close	PARAMS ((int));
#if 0
/* Conflicts with system headers on SVR4.  */
extern int	fcntl	PARAMS ((int des, int cmd, int e));
#endif
extern int	qsort	PARAMS ((void *data,int els, int siz, int func()));
#if 0
/* Conflicts with system headers on SVR4.  */
extern int	fseek	PARAMS ((FILE*, int, int));
#endif
extern int	fclose	PARAMS ((FILE*));
extern void	bcopy	PARAMS ((char*,char*,int));
extern int	bcmp	PARAMS ((char *, char *, int));
extern void	bzero	PARAMS ((char *, int));
extern void	perror	PARAMS ((CONST char *));
extern PTR	memchr	PARAMS ((const void *, int, unsigned ));
#if 0
/* Conflicts with system headers on SVR4.  */
extern unsigned short	getuid	PARAMS ((void));
extern unsigned short	getgid	PARAMS ((void));
#endif

extern char * strchr();

extern char *getenv();
extern char *strrchr();
extern int chmod();
#if 0
/* Conflicts with system headers on SVR4.  */
extern int fread();
#endif
extern int fstat();
#if 0
/* Conflicts with system headers on SVR4.  */
extern int fwrite();
#endif
extern int sscanf();
extern int stat();
extern int strtol();
extern int fileno();
extern char *strrchr();
extern char *ctime();
extern int _flsbuf();
extern int fclose();
#if 0
/* Conflicts with system headers on SVR4.  */
extern int time();
#endif
extern int utime();
extern int vfprintf();
extern long atol();
extern char *getenv();
extern int fputc();
extern int unlink();


#include "fopen-same.h"
