#ifndef	_HPPAHPUX_H
#define _HPPAHPUX_H

#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>
#include <memory.h>
#include <stdlib.h>

#define HOST_HPPAHPUX

void free();

#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#define SEEK_SET 0
#define SEEK_CUR 1

#ifndef	__STDC__
#define NATIVE_HPPAHPUX_COMPILER
#endif

#define USE_UTIME

#if 0
static int
rename(from, to)
{
  unlink(to);
  return(link(from, to));
}
#endif

#include "fopen-same.h"

#endif /* _HPPAHPUX_H */
