#ifndef	_HPPAOSF_H
#define _HPPAOSF_H

#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>
#include <stdlib.h>
#include <unistd.h>

#define HOST_HPPAOSF

#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif

#define SEEK_SET 0
#define SEEK_CUR 1

#include <machine/machparam.h>

#define	HOST_PAGE_SIZE		NBPG
#define	HOST_SEGMENT_SIZE	NBPG	/* Data seg start addr rounds to NBPG */
#define	HOST_MACHINE_ARCH	bfd_arch_hppa
/* #define	HOST_MACHINE_MACHINE	 */

#define	HOST_TEXT_START_ADDR		UTEXTSEG
#define HOST_DATA_START_ADDR		UDATASEG
#define	HOST_STACK_END_ADDR		KSTAKSEG

#include "fopen-same.h"

#endif /* _HPPAOSF_H */
