/* Motorola 680x0 running LynxOS */

#ifndef hosts_m68klynx_H
#define hosts_m68klynx_H

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <sys/file.h>

#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif

#define SEEK_SET 0
#define SEEK_CUR 1

#define	HOST_PAGE_SIZE		NBPG
#define	HOST_MACHINE_ARCH	bfd_arch_m68k
#define	HOST_TEXT_START_ADDR		USRTEXT

#define u_comm u_kproc.kp_proc.p_comm

#include "fopen-same.h"

/* Lynx has calloc, but no cfree, and gprof uses it. */

#define cfree free

#endif /* hosts_m68klynx_H */
