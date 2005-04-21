/* Target-dependent code for GNU/Linux SPARC.

   Copyright 2003, 2004 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "floatformat.h"
#include "frame.h"
#include "frame-unwind.h"
#include "tramp-frame.h"
#include "gdbarch.h"
#include "gdbcore.h"
#include "osabi.h"
#include "regcache.h"
#include "solib-svr4.h"
#include "symtab.h"
#include "trad-frame.h"

#include "sparc-tdep.h"

/* Signal trampoline support.  */

/* GNU/Linux has two flavors of signals.  Normal signal handlers, and
   "realtime" (RT) signals.  The RT signals can provide additional
   information to the signal handler if the SA_SIGINFO flag is set
   when establishing a signal handler using `sigaction'.  It is not
   unlikely that future versions of GNU/Linux will support SA_SIGINFO
   for normal signals too.  */

/* When the sparc Linux kernel calls a signal handler and the
   SA_RESTORER flag isn't set, the return address points to a bit of
   code on the stack.  This function returns whether the PC appears to
   be within this bit of code.

   The instruction sequence for normal signals is
	mov __NR_sigreturn, %g1		! hex: 0x821020d8
	ta  0x10			! hex: 0x91d02010

   Checking for the code sequence should be somewhat reliable, because
   the effect is to call the system call sigreturn.  This is unlikely
   to occur anywhere other than a signal trampoline.

   It kind of sucks that we have to read memory from the process in
   order to identify a signal trampoline, but there doesn't seem to be
   any other way.  However, sparc32_linux_pc_in_sigtramp arranges to
   only call us if no function name could be identified, which should
   be the case since the code is on the stack.  */

#define LINUX32_SIGTRAMP_INSN0	0x821020d8	/* mov __NR_sigreturn, %g1 */
#define LINUX32_SIGTRAMP_INSN1	0x91d02010	/* ta  0x10 */

/* The instruction sequence for RT signals is
       mov __NR_rt_sigreturn, %g1	! hex: 0x82102065
       ta  {0x10,0x6d}			! hex: 0x91d02010

   The effect is to call the system call rt_sigreturn.
   Note that 64-bit binaries only use this RT signal return method.  */

#define LINUX32_RT_SIGTRAMP_INSN0	0x82102065
#define LINUX32_RT_SIGTRAMP_INSN1	0x91d02010

static void sparc32_linux_sigframe_init (const struct tramp_frame *self,
					 struct frame_info *next_frame,
					 struct trad_frame_cache *this_cache,
					 CORE_ADDR func);

static const struct tramp_frame sparc32_linux_sigframe = {
  SIGTRAMP_FRAME,
  4,
  {
    { LINUX32_SIGTRAMP_INSN0, -1 },
    { LINUX32_SIGTRAMP_INSN1, -1 },
    { TRAMP_SENTINEL_INSN, -1 }
  },
  sparc32_linux_sigframe_init
};

static const struct tramp_frame sparc32_linux_rt_sigframe = {
  SIGTRAMP_FRAME,
  4,
  {
    { LINUX32_RT_SIGTRAMP_INSN0, -1 },
    { LINUX32_RT_SIGTRAMP_INSN1, -1 },
    { TRAMP_SENTINEL_INSN, -1 }
  },
  sparc32_linux_sigframe_init
};

static void
sparc32_linux_sigframe_init (const struct tramp_frame *self,
			     struct frame_info *next_frame,
			     struct trad_frame_cache *this_cache,
			     CORE_ADDR func)
{
  CORE_ADDR base, addr;
  int regnum;

  base = frame_unwind_register_unsigned (next_frame, SPARC_O1_REGNUM);
  if (self == &sparc32_linux_rt_sigframe)
    base += 128;

  /* Offsets from <bits/sigcontext.h> */

  trad_frame_set_reg_addr (this_cache, SPARC32_PSR_REGNUM, base + 0x00);
  trad_frame_set_reg_addr (this_cache, SPARC32_PC_REGNUM,  base + 0x04);
  trad_frame_set_reg_addr (this_cache, SPARC32_NPC_REGNUM, base + 0x08);
  trad_frame_set_reg_addr (this_cache, SPARC32_Y_REGNUM,   base + 0x0c);

  /* Since %g0 is always zero, keep the identity encoding.  */
  addr = base + 0x14;
  for (regnum = SPARC_G1_REGNUM; regnum <= SPARC_O7_REGNUM; regnum++)
    {
      trad_frame_set_reg_addr (this_cache, regnum, addr);
      addr += 4;
    }

  base = addr = frame_unwind_register_unsigned (next_frame, SPARC_SP_REGNUM);
  for (regnum = SPARC_L0_REGNUM; regnum <= SPARC_I7_REGNUM; regnum++)
    {
      trad_frame_set_reg_addr (this_cache, regnum, addr);
      addr += 4;
    }
  trad_frame_set_id (this_cache, frame_id_build (base, func));
}



static void
sparc32_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  tramp_frame_prepend_unwinder (gdbarch, &sparc32_linux_sigframe);
  tramp_frame_prepend_unwinder (gdbarch, &sparc32_linux_rt_sigframe);

  /* GNU/Linux is very similar to Solaris ...  */
  sparc32_sol2_init_abi (info, gdbarch);

  /* ... but doesn't have kernel-assisted single-stepping support.  */
  set_gdbarch_software_single_step (gdbarch, sparc_software_single_step);

  /* GNU/Linux doesn't support the 128-bit `long double' from the psABI.  */
  set_gdbarch_long_double_bit (gdbarch, 64);
  set_gdbarch_long_double_format (gdbarch, &floatformat_ieee_double_big);

  /* Enable TLS support.  */
  set_gdbarch_fetch_tls_load_module_address (gdbarch,
                                             svr4_fetch_objfile_link_map);
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern void _initialize_sparc_linux_tdep (void);

void
_initialize_sparc_linux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_sparc, 0, GDB_OSABI_LINUX,
			  sparc32_linux_init_abi);
}
