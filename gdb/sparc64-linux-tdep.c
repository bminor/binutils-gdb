/* Target-dependent code for GNU/Linux UltraSPARC.

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
#include "frame.h"
#include "frame-unwind.h"
#include "gdbarch.h"
#include "osabi.h"
#include "solib-svr4.h"
#include "symtab.h"
#include "trad-frame.h"

#include "gdb_assert.h"
#include "gdb_string.h"

#include "sparc64-tdep.h"

/* The instruction sequence for RT signals is
       mov __NR_rt_sigreturn, %g1	! hex: 0x82102065
       ta  0x6d				! hex: 0x91d0206d

   The effect is to call the system call rt_sigreturn.
   Note that 64-bit binaries only use this RT signal return method.  */

#define LINUX64_RT_SIGTRAMP_INSN0	0x82102065
#define LINUX64_RT_SIGTRAMP_INSN1	0x91d0206d

/* If PC is in a sigtramp routine consisting of the instructions
   LINUX64_RT_SIGTRAMP_INSN0 and LINUX64_RT_SIGTRAMP_INSN1, return
   the address of the start of the routine.  Otherwise, return 0.  */

static CORE_ADDR
sparc64_linux_sigtramp_start (struct frame_info *next_frame)
{
  CORE_ADDR pc = frame_pc_unwind (next_frame);
  ULONGEST word0, word1;
  unsigned char buf[8];		/* Two instructions.  */

  /* We only recognize a signal trampoline if PC is at the start of
     one of the instructions.  We optimize for finding the PC at the
     start of the instruction sequence, as will be the case when the
     trampoline is not the first frame on the stack.  We assume that
     in the case where the PC is not at the start of the instruction
     sequence, there will be a few trailing readable bytes on the
     stack.  */

  if (!safe_frame_unwind_memory (next_frame, pc, buf, sizeof buf))
    return 0;

  word0 = extract_unsigned_integer (buf, 4);
  if (word0 != LINUX64_RT_SIGTRAMP_INSN0)
    {
      if (word0 != LINUX64_RT_SIGTRAMP_INSN1)
	return 0;

      pc -= 4;
      if (!safe_frame_unwind_memory (next_frame, pc, buf, sizeof buf))
	return 0;

      word0 = extract_unsigned_integer (buf, 4);
    }

  word1 = extract_unsigned_integer (buf + 4, 4);
  if (word0 != LINUX64_RT_SIGTRAMP_INSN0
      || word1 != LINUX64_RT_SIGTRAMP_INSN1)
    return 0;

  return pc;
}

static int
sparc64_linux_sigtramp_p (struct frame_info *next_frame)
{
  CORE_ADDR pc = frame_pc_unwind (next_frame);
  char *name;

  find_pc_partial_function (pc, &name, NULL, NULL);

  /* If we have NAME, we can optimize the search.  The trampolines is
     named __rt_sigreturn_stub.  However, is isn't dynamically exported
     from the shared C library, so the trampoline may appear to be part
     of the preceding function.  This should always be sigaction,
     __sigaction, or __libc_sigaction (all aliases to the same function).  */
  if (name == NULL || strstr (name, "sigaction") != NULL)
    return (sparc64_linux_sigtramp_start (next_frame) != 0);

  return (strcmp ("__rt_sigreturn_stub", name) == 0);
}

static struct sparc_frame_cache *
sparc64_linux_sigtramp_frame_cache (struct frame_info *next_frame,
				    void **this_cache)
{
  struct sparc_frame_cache *cache;
  CORE_ADDR sigcontext_addr, addr;
  int regnum;

  if (*this_cache)
    return *this_cache;

  cache = sparc_frame_cache (next_frame, this_cache);
  gdb_assert (cache == *this_cache);

  regnum = SPARC_SP_REGNUM;
  cache->base = frame_unwind_register_unsigned (next_frame, regnum);
  if (cache->base & 1)
    cache->base += BIAS;

  regnum = SPARC_O1_REGNUM;
  sigcontext_addr = frame_unwind_register_unsigned (next_frame, regnum);

  /* If this is a RT signal trampoline, adjust SIGCONTEXT_ADDR
     accordingly.  */
  addr = sparc64_linux_sigtramp_start (next_frame);
  if (addr)
    sigcontext_addr += 128;
  else
    addr = frame_func_unwind (next_frame);

  cache->pc = addr;

  cache->saved_regs = trad_frame_alloc_saved_regs (next_frame);

  /* Offsets from <bits/sigcontext.h> */

  /* Since %g0 is always zero, keep the identity encoding.  */
  for (addr = sigcontext_addr + 8, regnum = SPARC_G1_REGNUM;
       regnum <= SPARC_O7_REGNUM; regnum++, addr += 8)
    cache->saved_regs[regnum].addr = addr;

  cache->saved_regs[SPARC64_STATE_REGNUM].addr = addr + 0;
  cache->saved_regs[SPARC64_PC_REGNUM].addr = addr + 8;
  cache->saved_regs[SPARC64_NPC_REGNUM].addr = addr + 16;
  cache->saved_regs[SPARC64_Y_REGNUM].addr = addr + 24;
  cache->saved_regs[SPARC64_FPRS_REGNUM].addr = addr + 28;

  for (regnum = SPARC_L0_REGNUM, addr = cache->base;
       regnum <= SPARC_I7_REGNUM; regnum++, addr += 8)
    cache->saved_regs[regnum].addr = addr;

  return cache;
}

static void
sparc64_linux_sigtramp_frame_this_id (struct frame_info *next_frame,
				      void **this_cache,
				      struct frame_id *this_id)
{
  struct sparc_frame_cache *cache =
    sparc64_linux_sigtramp_frame_cache (next_frame, this_cache);

  (*this_id) = frame_id_build (cache->base, cache->pc);
}

static void
sparc64_linux_sigtramp_frame_prev_register (struct frame_info *next_frame,
					    void **this_cache,
					    int regnum, int *optimizedp,
					    enum lval_type *lvalp,
					    CORE_ADDR *addrp,
					    int *realnump, void *valuep)
{
  struct sparc_frame_cache *cache =
    sparc64_linux_sigtramp_frame_cache (next_frame, this_cache);

  trad_frame_get_prev_register (next_frame, cache->saved_regs, regnum,
				optimizedp, lvalp, addrp, realnump, valuep);
}

static const struct frame_unwind sparc64_linux_sigtramp_frame_unwind =
{
  SIGTRAMP_FRAME,
  sparc64_linux_sigtramp_frame_this_id,
  sparc64_linux_sigtramp_frame_prev_register
};

static const struct frame_unwind *
sparc64_linux_sigtramp_frame_sniffer (struct frame_info *next_frame)
{
  if (sparc64_linux_sigtramp_p (next_frame))
    return &sparc64_linux_sigtramp_frame_unwind;

  return NULL;
}

static void
sparc64_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  frame_unwind_append_sniffer (gdbarch, sparc64_linux_sigtramp_frame_sniffer);

  /* GNU/Linux is very similar to Solaris ...  */
  sparc64_sol2_init_abi (info, gdbarch);

  /* ... but doesn't have kernel-assisted single-stepping support.  */
  set_gdbarch_software_single_step (gdbarch, sparc_software_single_step);

  /* Enable TLS support.  */
  set_gdbarch_fetch_tls_load_module_address (gdbarch,
                                             svr4_fetch_objfile_link_map);
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern void _initialize_sparc64_linux_tdep (void);

void
_initialize_sparc64_linux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_sparc, bfd_mach_sparc_v9,
			  GDB_OSABI_LINUX, sparc64_linux_init_abi);
}
