#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>

#define HOST_IRIX4

#if 0
#include <ctype.h>
#include <sys/file.h>
#endif

#if !defined (O_ACCMODE)
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif

#if !defined (SEEK_SET)
#define SEEK_SET 0
#define SEEK_CUR 1
#endif

#include "fopen-same.h"

/* From ANSI C <stddef.h>, which we can't depend upon the existence of */
#ifndef	offsetof
#define	offsetof(type,memb)	((size_t)&(((type *)0)->memb))
#endif
