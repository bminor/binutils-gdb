/* System V Release 4 Unix host system */

#include <stddef.h>
#include <ansidecl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#define POSIX_UTIME

/* Note that we also use this for ptx4 (sequent).  ptx4 does not
   really have procfs, but it has the relevant headers for BFD to read
   core files, so the proper thing to do for ptx4 is define
   HAVE_PROCFS in BFD but not GDB.  */

#define HAVE_PROCFS	/* This host has /proc support */

#include "fopen-same.h"
