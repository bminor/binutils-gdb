/* Architecture independant NetBSD host support */

#ifndef hosts_nbsd_H_
#define hosts_nbsd_H__

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>
#include <machine/param.h>
#include <machine/vmparam.h>
#include <machine/reg.h>

#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif

#define SEEK_SET 0
#define SEEK_CUR 1

#define POSIX_UTIME

#include "fopen-same.h"

#define	HOST_PAGE_SIZE			NBPG
#define	HOST_TEXT_START_ADDR		USRTEXT
#define	HOST_STACK_END_ADDR		USRSTACK

#endif
