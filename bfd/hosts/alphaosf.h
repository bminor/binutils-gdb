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

/* Make the basic types 64-bit quantities on the host */
#define	HOST_64_BIT	long 

typedef unsigned  long uint64e_type;
typedef unsigned  long uint64_type;
typedef  long int64_type;

#define BYTES_IN_PRINTF_INT 4

#define uint64_typeLOW(x) (((x) & 0xffffffff))
#define uint64_typeHIGH(x) (((x) >> 32) & 0xffffffff)

#include "fopen-same.h"
