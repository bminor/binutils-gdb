/* Intel 386 running any BSD Unix */

#ifndef	hosts_i386bsd_H
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>

#ifndef	O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif

#define SEEK_SET 0
#define SEEK_CUR 1

#define	HOST_PAGE_SIZE		NBPG
#define	HOST_MACHINE_ARCH	bfd_arch_i386
#define	HOST_TEXT_START_ADDR		USRTEXT

#define u_comm u_kproc.kp_proc.p_comm

#include "fopen-same.h"
#define hosts_i386bsd_H
#undef I386			/* Lynx defines this for some reason */
#endif

/* From ANSI C <stddef.h>, which we can't depend upon the existence of */
#ifndef	offsetof
#define	offsetof(type,memb)	((size_t)&(((type *)0)->memb))
#endif
