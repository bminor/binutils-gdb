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

#ifndef	DONTDECLARE_MALLOC
extern PTR	malloc	PARAMS ((unsigned));
extern PTR	realloc	PARAMS ((PTR, unsigned));
#endif
extern int	abort	PARAMS ((void));
extern int	free	PARAMS ((PTR));
extern void	bcopy	PARAMS ((char*,char*,int));
extern void	exit	PARAMS ((int));
extern void	bzero	PARAMS ((char *, int));

extern int strtol();

#include "fopen-same.h"

#define POSIX_UTIME
#ifndef _ES_MP
#define HAVE_PROCFS    /* Used for core file stuff only */
#endif
