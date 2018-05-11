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

#include "common/common-regcache.h"

/* Where indicated, this file contains defines and macros lifted directly from
   the Linux kernel headers, with no modification.
   Refer to Linux kernel documentation for details.  */

#include <asm/sigcontext.h>
#include <sys/utsname.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>

/* Read VQ for the given tid using ptrace.  If SVE is not supported then zero
   is returned (on a system that supports SVE, then VQ cannot be zero).  */

extern unsigned long aarch64_sve_get_vq (int tid);

/* Read the current SVE register set using ptrace, allocating space as
   required.  */

extern gdb_byte *aarch64_sve_get_sveregs (int tid);

/* Put the registers from linux structure buf into regcache.  */

extern void aarch64_sve_regs_copy_to_regcache (reg_buffer_common *regcache,
					       const void *buf);

/* Put the registers from regcache into linux structure buf.  */

extern void aarch64_sve_regs_copy_from_regcache (reg_buffer_common *regcache,
						 void *buf);

/* Structures and defines taken from sigcontext.h.  */

#ifndef SVE_SIG_ZREGS_SIZE

struct sve_context {
   struct _aarch64_ctx head;
   __u16 vl;
   __u16 __reserved[3];
};

#define SVE_VQ_BYTES		16	/* number of bytes per quadword */

#define SVE_VQ_MIN		1
#define SVE_VQ_MAX		512

#define SVE_VL_MIN		(SVE_VQ_MIN * SVE_VQ_BYTES)
#define SVE_VL_MAX		(SVE_VQ_MAX * SVE_VQ_BYTES)

#define SVE_NUM_ZREGS		32
#define SVE_NUM_PREGS		16

#define sve_vl_valid(vl) \
	((vl) % SVE_VQ_BYTES == 0 && (vl) >= SVE_VL_MIN && (vl) <= SVE_VL_MAX)

#define SVE_SIG_ZREG_SIZE(vq) ((__u32)(vq) * SVE_VQ_BYTES)
#define SVE_SIG_PREG_SIZE(vq) ((__u32)(vq) * (SVE_VQ_BYTES / 8))
#define SVE_SIG_FFR_SIZE(vq)  SVE_SIG_PREG_SIZE(vq)

#define SVE_SIG_REGS_OFFSET               \
   ((sizeof(struct sve_context) + (SVE_VQ_BYTES - 1)) \
      / SVE_VQ_BYTES * SVE_VQ_BYTES)

#define SVE_SIG_ZREGS_OFFSET  SVE_SIG_REGS_OFFSET
#define SVE_SIG_ZREG_OFFSET(vq, n) \
   (SVE_SIG_ZREGS_OFFSET + SVE_SIG_ZREG_SIZE(vq) * (n))
#define SVE_SIG_ZREGS_SIZE(vq) \
   (SVE_SIG_ZREG_OFFSET(vq, SVE_NUM_ZREGS) - SVE_SIG_ZREGS_OFFSET)

#define SVE_SIG_PREGS_OFFSET(vq) \
   (SVE_SIG_ZREGS_OFFSET + SVE_SIG_ZREGS_SIZE(vq))
#define SVE_SIG_PREG_OFFSET(vq, n) \
   (SVE_SIG_PREGS_OFFSET(vq) + SVE_SIG_PREG_SIZE(vq) * (n))
#define SVE_SIG_PREGS_SIZE(vq) \
   (SVE_SIG_PREG_OFFSET(vq, SVE_NUM_PREGS) - SVE_SIG_PREGS_OFFSET(vq))

#define SVE_SIG_FFR_OFFSET(vq) \
   (SVE_SIG_PREGS_OFFSET(vq) + SVE_SIG_PREGS_SIZE(vq))

#define SVE_SIG_REGS_SIZE(vq) \
   (SVE_SIG_FFR_OFFSET(vq) + SVE_SIG_FFR_SIZE(vq) - SVE_SIG_REGS_OFFSET)

#define SVE_SIG_CONTEXT_SIZE(vq) (SVE_SIG_REGS_OFFSET + SVE_SIG_REGS_SIZE(vq))

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


#define SVE_PT_REGS_MASK      1

#define SVE_PT_REGS_FPSIMD    0
#define SVE_PT_REGS_SVE       SVE_PT_REGS_MASK

#define SVE_PT_VL_INHERIT     (PR_SVE_VL_INHERIT >> 16)
#define SVE_PT_VL_ONEXEC      (PR_SVE_SET_VL_ONEXEC >> 16)

#define SVE_PT_REGS_OFFSET             \
   ((sizeof(struct sve_context) + (SVE_VQ_BYTES - 1)) \
      / SVE_VQ_BYTES * SVE_VQ_BYTES)

#define SVE_PT_FPSIMD_OFFSET     SVE_PT_REGS_OFFSET

#define SVE_PT_FPSIMD_SIZE(vq, flags)  (sizeof(struct user_fpsimd_state))

#define SVE_PT_SVE_ZREG_SIZE(vq) SVE_SIG_ZREG_SIZE(vq)
#define SVE_PT_SVE_PREG_SIZE(vq) SVE_SIG_PREG_SIZE(vq)
#define SVE_PT_SVE_FFR_SIZE(vq)     SVE_SIG_FFR_SIZE(vq)
#define SVE_PT_SVE_FPSR_SIZE     sizeof(__u32)
#define SVE_PT_SVE_FPCR_SIZE     sizeof(__u32)

#define __SVE_SIG_TO_PT(offset) \
   ((offset) - SVE_SIG_REGS_OFFSET + SVE_PT_REGS_OFFSET)

#define SVE_PT_SVE_OFFSET     SVE_PT_REGS_OFFSET

#define SVE_PT_SVE_ZREGS_OFFSET \
   __SVE_SIG_TO_PT(SVE_SIG_ZREGS_OFFSET)
#define SVE_PT_SVE_ZREG_OFFSET(vq, n) \
   __SVE_SIG_TO_PT(SVE_SIG_ZREG_OFFSET(vq, n))
#define SVE_PT_SVE_ZREGS_SIZE(vq) \
   (SVE_PT_SVE_ZREG_OFFSET(vq, SVE_NUM_ZREGS) - SVE_PT_SVE_ZREGS_OFFSET)

#define SVE_PT_SVE_PREGS_OFFSET(vq) \
   __SVE_SIG_TO_PT(SVE_SIG_PREGS_OFFSET(vq))
#define SVE_PT_SVE_PREG_OFFSET(vq, n) \
   __SVE_SIG_TO_PT(SVE_SIG_PREG_OFFSET(vq, n))
#define SVE_PT_SVE_PREGS_SIZE(vq) \
   (SVE_PT_SVE_PREG_OFFSET(vq, SVE_NUM_PREGS) - \
      SVE_PT_SVE_PREGS_OFFSET(vq))

#define SVE_PT_SVE_FFR_OFFSET(vq) \
   __SVE_SIG_TO_PT(SVE_SIG_FFR_OFFSET(vq))

#define SVE_PT_SVE_FPSR_OFFSET(vq)           \
   ((SVE_PT_SVE_FFR_OFFSET(vq) + SVE_PT_SVE_FFR_SIZE(vq) +  \
         (SVE_VQ_BYTES - 1))        \
      / SVE_VQ_BYTES * SVE_VQ_BYTES)
#define SVE_PT_SVE_FPCR_OFFSET(vq) \
   (SVE_PT_SVE_FPSR_OFFSET(vq) + SVE_PT_SVE_FPSR_SIZE)

#define SVE_PT_SVE_SIZE(vq, flags)              \
   ((SVE_PT_SVE_FPCR_OFFSET(vq) + SVE_PT_SVE_FPCR_SIZE      \
         - SVE_PT_SVE_OFFSET + (SVE_VQ_BYTES - 1)) \
      / SVE_VQ_BYTES * SVE_VQ_BYTES)

#define SVE_PT_SIZE(vq, flags)                  \
    (((flags) & SVE_PT_REGS_MASK) == SVE_PT_REGS_SVE ?      \
        SVE_PT_SVE_OFFSET + SVE_PT_SVE_SIZE(vq, flags)   \
      : SVE_PT_FPSIMD_OFFSET + SVE_PT_FPSIMD_SIZE(vq, flags))

#endif /* SVE_PT_SVE_ZREG_SIZE.  */


/* Indicates whether a SVE ptrace header is followed by SVE registers or a
   fpsimd structure.  */

#define HAS_SVE_STATE(header) ((header).flags && SVE_PT_REGS_SVE)


#endif /* aarch64-sve-linux-ptrace.h */
