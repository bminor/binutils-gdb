#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>
#include <alloca.h>
#include <stdlib.h>

/* Make the basic types 64-bit quantities on the host.
   Also provide the support macros BFD needs.  */
#define	BFD_HOST_64_BIT	long
#define sprintf_vma(s,x) sprintf (s, "%016lx", x) /* BFD_HOST_64_BIT */
#define fprintf_vma(f,x) fprintf (f, "%016lx", x) /* BFD_HOST_64_BIT */

#define BYTES_IN_PRINTF_INT 4

/* These must have type unsigned long because they are used as
   arguments in printf functions.  */
#define uint64_typeLOW(x) ((unsigned long) (((x) & 0xffffffff))) /* BFD_HOST_64_BIT */
#define uint64_typeHIGH(x) ((unsigned long) (((x) >> 32) & 0xffffffff)) /* BFD_HOST_64_BIT */

#include "fopen-same.h"
