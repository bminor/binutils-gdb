/* Common target dependent definitions for AArch64 Scalable Extensions
   (SVE/SME).

   Copyright (C) 2018-2023 Free Software Foundation, Inc.

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

#ifndef NAT_AARCH64_SCALABLE_LINUX_PTRACE_H
#define NAT_AARCH64_SCALABLE_LINUX_PTRACE_H

#include <signal.h>
#include <sys/utsname.h>

/* The order in which <sys/ptrace.h> and <asm/ptrace.h> are included
   can be important.  <sys/ptrace.h> often declares various PTRACE_*
   enums.  <asm/ptrace.h> often defines preprocessor constants for
   these very same symbols.  When that's the case, build errors will
   result when <asm/ptrace.h> is included before <sys/ptrace.h>.  */
#include <sys/ptrace.h>
#include <asm/ptrace.h>

#ifndef SVE_SIG_ZREGS_SIZE
#include "aarch64-scalable-linux-sigcontext.h"
#endif

/* Indicates whether a SVE ptrace header is followed by SVE registers or a
   fpsimd structure.  */

#define HAS_SVE_STATE(header) ((header).flags & SVE_PT_REGS_SVE)

/* Read VQ for the given tid using ptrace.  If SVE is not supported then zero
   is returned (on a system that supports SVE, then VQ cannot be zero).  */

uint64_t aarch64_sve_get_vq (int tid);

/* Set VQ in the kernel for the given tid, using either the value VQ or
   reading from the register VG in the register buffer.  */

bool aarch64_sve_set_vq (int tid, uint64_t vq);
bool aarch64_sve_set_vq (int tid, struct reg_buffer_common *reg_buf);

/* Read the current SVE register set from thread TID and return its data
   through a byte vector.  */

extern gdb::byte_vector aarch64_fetch_sve_regset (int tid);

/* Write the SVE contents from SVE_STATE to thread TID.  */

extern void
aarch64_store_sve_regset (int tid, const gdb::byte_vector &sve_state);

/* Given a thread id TID and a register buffer REG_BUF, update the register
   buffer with the SVE state from thread TID.  */

extern void
aarch64_sve_regs_copy_to_reg_buf (int tid, struct reg_buffer_common *reg_buf);

/* Given a thread id TID and a register buffer REG_BUF containing SVE
   register data, write the SVE data to thread TID.  */

extern void
aarch64_sve_regs_copy_from_reg_buf (int tid,
				    struct reg_buffer_common *reg_buf);

#endif /* NAT_AARCH64_SCALABLE_LINUX_PTRACE_H */
