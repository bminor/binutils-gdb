#include <ansidecl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>
#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
 
#ifndef	DONTDECLARE_MALLOC
extern PTR	malloc	PARAMS ((unsigned));
extern PTR	realloc	PARAMS ((PTR, unsigned));
#endif
extern int	abort	PARAMS ((void));
extern int	free	PARAMS ((PTR));
extern void	bcopy	PARAMS ((char*, char*, int));
extern void	exit	PARAMS ((int));
extern void	bzero	PARAMS ((char *, int));
extern int strtol();
#define NO_STDARG 1
 
#include "fopen-same.h"
