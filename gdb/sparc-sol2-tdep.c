/* Target-dependent code for Solaris SPARC.

   Copyright 2003 Free Software Foundation, Inc.

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
#include "frame-base.h"
#include "frame-unwind.h"
#include "gdbcore.h"
#include "symtab.h"
#include "objfiles.h"
#include "osabi.h"
#include "regcache.h"
#include "target.h"
#include "trad-frame.h"

#include "gdb_assert.h"
#include "gdb_string.h"

#include "sparc-tdep.h"

/* From <sys/regset.h>.  */
const int sparc_sol2_R_PSR = 32;
const int sparc_sol2_R_PC  = 33;
const int sparc_sol2_R_nPC = 34;
const int sparc_sol2_R_Y   = 35;
const int sparc_sol2_R_WIM = 36;
const int sparc_sol2_R_TBR = 37;

void
sparc_sol2_supply_gregset (struct regcache *regcache,
			   int regnum, const void *gregs)
{
  const char *regs = gregs;
  int i;

  if (regnum == SPARC32_PSR_REGNUM || regnum == -1)
    regcache_raw_supply (regcache, SPARC32_PSR_REGNUM,
			 regs + sparc_sol2_R_PSR * 4);

  if (regnum == SPARC32_PC_REGNUM || regnum == -1)
    regcache_raw_supply (regcache, SPARC32_PC_REGNUM,
			 regs + sparc_sol2_R_PC * 4);

  if (regnum == SPARC32_NPC_REGNUM || regnum == -1)
    regcache_raw_supply (regcache, SPARC32_NPC_REGNUM,
			 regs + sparc_sol2_R_nPC * 4);

  if (regnum == SPARC32_Y_REGNUM || regnum == -1)
    regcache_raw_supply (regcache, SPARC32_Y_REGNUM,
			 regs + sparc_sol2_R_Y * 4);

  if ((regnum >= SPARC_G0_REGNUM && regnum <= SPARC_I7_REGNUM) || regnum == -1)
    {
      for (i = SPARC_G0_REGNUM; i <= SPARC_I7_REGNUM; i++)
	{
	  if (regnum == i || regnum == -1)
	    regcache_raw_supply (regcache, i,
				 (regs + (i - SPARC_G0_REGNUM) * 4));
	}
    }
}

void
sparc_sol2_collect_gregset (const struct regcache *regcache,
			    int regnum, void *gregs)
{
  char *regs = gregs;
  int i;

  if (regnum == SPARC32_PSR_REGNUM || regnum == -1)
    regcache_raw_collect (regcache, SPARC32_PSR_REGNUM,
			  regs + sparc_sol2_R_PSR * 4);

  if (regnum == SPARC32_PC_REGNUM || regnum == -1)
    regcache_raw_collect (regcache, SPARC32_PC_REGNUM,
			  regs + sparc_sol2_R_PC * 4);

  if (regnum == SPARC32_NPC_REGNUM || regnum == -1)
    regcache_raw_collect (regcache, SPARC32_NPC_REGNUM,
			  regs + sparc_sol2_R_nPC * 4);

  if (regnum == SPARC32_Y_REGNUM || regnum == -1)
    regcache_raw_collect (regcache, SPARC32_Y_REGNUM,
			  regs + sparc_sol2_R_Y * 4);

  if ((regnum >= SPARC_G0_REGNUM && regnum <= SPARC_I7_REGNUM) || regnum == -1)
    {
      /* %g0 is always zero.  */
      for (i = SPARC_G1_REGNUM; i <= SPARC_I7_REGNUM; i++)
	{
	  if (regnum == i || regnum == -1)
	    regcache_raw_collect (regcache, i,
				  (regs + (i - SPARC_G0_REGNUM) * 4));
	}
    }
}

void
sparc_sol2_supply_fpregset (struct regcache *regcache,
			    int regnum, const void *fpregs)
{
  const char *regs = fpregs;
  int i;

  for (i = 0; i < 32; i++)
    {
      if (regnum == (SPARC_F0_REGNUM + i) || regnum == -1)
	regcache_raw_supply (regcache, SPARC_F0_REGNUM + i, regs + (i * 4));
    }

  if (regnum == SPARC32_FSR_REGNUM || regnum == -1)
    regcache_raw_supply (regcache, SPARC32_FSR_REGNUM, regs + (32 * 4) + 4);
}

void
sparc_sol2_collect_fpregset (const struct regcache *regcache,
			     int regnum, void *fpregs)
{
  char *regs = fpregs;
  int i;

  for (i = 0; i < 32; i++)
    {
      if (regnum == (SPARC_F0_REGNUM + i) || regnum == -1)
	regcache_raw_collect (regcache, SPARC_F0_REGNUM + i, regs + (i * 4));
    }

  if (regnum == SPARC32_FSR_REGNUM || regnum == -1)
    regcache_raw_collect (regcache, SPARC32_FSR_REGNUM, regs + (32 * 4) + 4);
}

static int
sparc_sol2_pc_in_sigtramp (CORE_ADDR pc, char *name)
{
  return (name && strcmp (name, "sigacthandler") == 0);
}

static struct sparc32_frame_cache *
sparc_sol2_sigtramp_frame_cache (struct frame_info *next_frame,
				 void **this_cache)
{
  struct sparc32_frame_cache *cache;
  CORE_ADDR mcontext_addr, addr;
  int regnum;

  if (*this_cache)
    return *this_cache;

  cache = sparc32_frame_cache (next_frame, this_cache);
  gdb_assert (cache == *this_cache);

  cache->saved_regs = trad_frame_alloc_saved_regs (next_frame);

  regnum = (cache->frameless_p ? SPARC_O2_REGNUM : SPARC_I2_REGNUM);
  mcontext_addr = frame_unwind_register_unsigned (next_frame, regnum) + 40;

  cache->saved_regs[SPARC32_PSR_REGNUM].addr = mcontext_addr + 0;
  cache->saved_regs[SPARC32_PC_REGNUM].addr = mcontext_addr + 4;
  cache->saved_regs[SPARC32_NPC_REGNUM].addr = mcontext_addr + 8;
  cache->saved_regs[SPARC32_Y_REGNUM].addr = mcontext_addr + 12;

  /* Since %g0 is always zero, keep the identity encoding.  */
  for (regnum = SPARC_G1_REGNUM, addr = mcontext_addr + 16;
       regnum <= SPARC_O7_REGNUM; regnum++, addr += 4)
    cache->saved_regs[regnum].addr = addr;

  if (get_frame_memory_unsigned (next_frame, mcontext_addr + 19 * 4, 4))
    {
      /* The register windows haven't been flushed.  */
      for (regnum = SPARC_L0_REGNUM; regnum <= SPARC_I7_REGNUM; regnum++)
	trad_frame_set_unknown (cache->saved_regs, regnum);
    }
  else
    {
      for (regnum = SPARC_L0_REGNUM, addr = cache->base;
	   regnum <= SPARC_I7_REGNUM; regnum++, addr += 4)
	cache->saved_regs[regnum].addr = addr;
    }

  return cache;
}

static void
sparc_sol2_sigtramp_frame_this_id (struct frame_info *next_frame,
				   void **this_cache, struct frame_id *this_id)
{
  struct sparc32_frame_cache *cache =
    sparc_sol2_sigtramp_frame_cache (next_frame, this_cache);

  (*this_id) = frame_id_build (cache->base, cache->pc);
}

static void
sparc_sol2_sigtramp_frame_prev_register (struct frame_info *next_frame,
					 void **this_cache,
					 int regnum, int *optimizedp,
					 enum lval_type *lvalp,
					 CORE_ADDR *addrp,
					 int *realnump, void *valuep)
{
  struct sparc32_frame_cache *cache =
    sparc_sol2_sigtramp_frame_cache (next_frame, this_cache);

  trad_frame_prev_register (next_frame, cache->saved_regs, regnum,
			    optimizedp, lvalp, addrp, realnump, valuep);
}

static const struct frame_unwind sparc_sol2_sigtramp_frame_unwind =
{
  SIGTRAMP_FRAME,
  sparc_sol2_sigtramp_frame_this_id,
  sparc_sol2_sigtramp_frame_prev_register
};

static const struct frame_unwind *
sparc_sol2_sigtramp_frame_sniffer (struct frame_info *next_frame)
{
  CORE_ADDR pc = frame_pc_unwind (next_frame);
  char *name;

  find_pc_partial_function (pc, &name, NULL, NULL);
  if (sparc_sol2_pc_in_sigtramp (pc, name))
    return &sparc_sol2_sigtramp_frame_unwind;

  return NULL;
}


void
sparc_sol2_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* Solaris has SVR4-style shared libraries...  */
  set_gdbarch_in_solib_call_trampoline (gdbarch, in_plt_section);
  set_gdbarch_skip_trampoline_code (gdbarch, find_solib_trampoline_target);

  /* ...which means that we need some special handling when doing
     prologue analysis.  */
  tdep->plt_entry_size = 12;

  /* Solaris has kernel-assisted single-stepping support.  */
  set_gdbarch_software_single_step (gdbarch, NULL);

  frame_unwind_append_sniffer (gdbarch, sparc_sol2_sigtramp_frame_sniffer);
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_sparc_sol2_tdep (void);

void
_initialize_sparc_sol2_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_sparc, 0,
			  GDB_OSABI_SOLARIS, sparc_sol2_init_abi);
}
