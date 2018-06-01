/* Common target dependent for AArch64 systems.

   Copyright (C) 2018 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef AARCH64_SVE_LINUX_PTRACE_H
#define AARCH64_SVE_LINUX_PTRACE_H

/* Where indicated, this file contains defines and macros lifted directly from
   the Linux kernel headers, with no modification.
   Refer to Linux kernel documentation for details.  */

#include <asm/sigcontext.h>
#include <sys/utsname.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>

/* Read VQ for the given tid using ptrace.  If SVE is not supported then zero
   is returned (on a system that supports SVE, then VQ cannot be zero).  */

uint64_t aarch64_sve_get_vq (int tid);

/* Structures and defines taken from sigcontext.h.  */

#ifndef SVE_SIG_ZREGS_SIZE

#define SVE_VQ_BYTES		16	/* number of bytes per quadword */

#define SVE_VQ_MIN		1
#define SVE_VQ_MAX		512

#define SVE_VL_MIN		(SVE_VQ_MIN * SVE_VQ_BYTES)
#define SVE_VL_MAX		(SVE_VQ_MAX * SVE_VQ_BYTES)

#define SVE_NUM_ZREGS		32
#define SVE_NUM_PREGS		16

#define sve_vl_valid(vl) \
	((vl) % SVE_VQ_BYTES == 0 && (vl) >= SVE_VL_MIN && (vl) <= SVE_VL_MAX)

#endif /* SVE_SIG_ZREGS_SIZE.  */


/* Structures and defines taken from ptrace.h.  */

#ifndef SVE_PT_SVE_ZREG_SIZE

struct user_sve_header {
	__u32 size; /* total meaningful regset content in bytes */
	__u32 max_size; /* maxmium possible size for this thread */
	__u16 vl; /* current vector length */
	__u16 max_vl; /* maximum possible vector length */
	__u16 flags;
	__u16 __reserved;
};

#endif /* SVE_PT_SVE_ZREG_SIZE.  */

#endif /* aarch64-sve-linux-ptrace.h */
