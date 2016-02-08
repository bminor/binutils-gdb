/* Common target dependent code for GNU/Linux on ARM systems.

   Copyright (C) 1999-2016 Free Software Foundation, Inc.

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

#include "common-defs.h"
#include "common-regcache.h"
#include "arch/arm.h"
#include "arm-linux.h"
#include "arch/arm-get-next-pcs.h"

/* Calculate the offset from stack pointer of the pc register on the stack
   in the case of a sigreturn or sigreturn_rt syscall.  */
int
arm_linux_sigreturn_next_pc_offset (unsigned long sp,
				    unsigned long sp_data,
				    unsigned long svc_number,
				    int is_sigreturn)
{
  /* Offset of R0 register.  */
  int r0_offset = 0;
  /* Offset of PC register.  */
  int pc_offset = 0;

  if (is_sigreturn)
    {
      if (sp_data == ARM_NEW_SIGFRAME_MAGIC)
	r0_offset = ARM_UCONTEXT_SIGCONTEXT + ARM_SIGCONTEXT_R0;
      else
	r0_offset = ARM_SIGCONTEXT_R0;
    }
  else
    {
      if (sp_data == sp + ARM_OLD_RT_SIGFRAME_SIGINFO)
	r0_offset = ARM_OLD_RT_SIGFRAME_UCONTEXT;
      else
	r0_offset = ARM_NEW_RT_SIGFRAME_UCONTEXT;

      r0_offset += ARM_UCONTEXT_SIGCONTEXT + ARM_SIGCONTEXT_R0;
    }

  pc_offset = r0_offset + INT_REGISTER_SIZE * ARM_PC_REGNUM;

  return pc_offset;
}

/* Implementation of "fixup" method of struct arm_get_next_pcs_ops
   for arm-linux.  */

CORE_ADDR
arm_linux_get_next_pcs_fixup (struct arm_get_next_pcs *self,
			      CORE_ADDR nextpc)
{
  /* The Linux kernel offers some user-mode helpers in a high page.  We can
     not read this page (as of 2.6.23), and even if we could then we
     couldn't set breakpoints in it, and even if we could then the atomic
     operations would fail when interrupted.  They are all called as
     functions and return to the address in LR, so step to there
     instead.  */
  if (nextpc > 0xffff0000)
    nextpc = regcache_raw_get_unsigned (self->regcache, ARM_LR_REGNUM);
  return nextpc;
}
