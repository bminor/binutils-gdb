/* Native-dependent code for PA-RISC HP-UX.

   Copyright 2004 Free Software Foundation, Inc.

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
#include "inferior.h"
#include "regcache.h"
#include "target.h"

#include "gdb_assert.h"
#include <sys/ptrace.h>
#include <machine/save_state.h>

#ifdef HAVE_TTRACE
#include <sys/ttrace.h>
#endif

#include "hppa-tdep.h"
#include "inf-ptrace.h"
#include "inf-ttrace.h"

/* HP-UX 10.20 has a different name than HP-UX 11.00 and later.
   Apparently, the intended usage changed.  Unfortunately HP didn't
   care about backwards compatibility.  */
#ifdef ss_tlsp
#define ss_mpsfu_high ss_tlsp
#endif

int child_suppress_run = 0;     /* Non-zero if we should pretend not to be
				   a runnable target.  */

static int hppa_hpux_save_state_offset[] =
{
  ssoff(ss_flags),
  ssoff(ss_narrow.ss_gr1),
  ssoff(ss_narrow.ss_rp),
  ssoff(ss_narrow.ss_gr3),
  ssoff(ss_narrow.ss_gr4),
  ssoff(ss_narrow.ss_gr5),
  ssoff(ss_narrow.ss_gr6),
  ssoff(ss_narrow.ss_gr7),
  ssoff(ss_narrow.ss_gr8),
  ssoff(ss_narrow.ss_gr9),
  ssoff(ss_narrow.ss_gr10),
  ssoff(ss_narrow.ss_gr11),
  ssoff(ss_narrow.ss_gr12),
  ssoff(ss_narrow.ss_gr13),
  ssoff(ss_narrow.ss_gr14),
  ssoff(ss_narrow.ss_gr15),
  ssoff(ss_narrow.ss_gr16),
  ssoff(ss_narrow.ss_gr17),
  ssoff(ss_narrow.ss_gr18),
  ssoff(ss_narrow.ss_gr19),
  ssoff(ss_narrow.ss_gr20),
  ssoff(ss_narrow.ss_gr21),
  ssoff(ss_narrow.ss_gr22),
  ssoff(ss_narrow.ss_arg3),
  ssoff(ss_narrow.ss_arg2),
  ssoff(ss_narrow.ss_arg1),
  ssoff(ss_narrow.ss_arg0),
  ssoff(ss_narrow.ss_dp),
  ssoff(ss_narrow.ss_ret0),
  ssoff(ss_narrow.ss_ret1),
  ssoff(ss_narrow.ss_sp),
  ssoff(ss_narrow.ss_gr31),
  ssoff(ss_narrow.ss_cr11),
  ssoff(ss_narrow.ss_pcoq_head),
  ssoff(ss_narrow.ss_pcsq_head),
  ssoff(ss_narrow.ss_pcoq_tail),
  ssoff(ss_narrow.ss_pcsq_tail),
  ssoff(ss_narrow.ss_cr15),
  ssoff(ss_narrow.ss_cr19),
  ssoff(ss_narrow.ss_cr20),
  ssoff(ss_narrow.ss_cr21),
  ssoff(ss_narrow.ss_cr22),
  ssoff(ss_narrow.ss_cpustate),
  ssoff(ss_narrow.ss_sr4),
  ssoff(ss_narrow.ss_sr0),
  ssoff(ss_narrow.ss_sr1),
  ssoff(ss_narrow.ss_sr2),
  ssoff(ss_narrow.ss_sr3),
  ssoff(ss_narrow.ss_sr5),
  ssoff(ss_narrow.ss_sr6),
  ssoff(ss_narrow.ss_sr7),
  ssoff(ss_narrow.ss_cr0),
  ssoff(ss_narrow.ss_cr8),
  ssoff(ss_narrow.ss_cr9),
  ssoff(ss_narrow.ss_cr10),
  ssoff(ss_narrow.ss_cr12),
  ssoff(ss_narrow.ss_cr13),
  ssoff(ss_narrow.ss_cr24),
  ssoff(ss_narrow.ss_cr25),
  ssoff(ss_narrow.ss_cr26),
  ssoff(ss_narrow.ss_mpsfu_high),
  ssoff(ss_narrow.ss_mpsfu_low),
  ssoff(ss_narrow.ss_mpsfu_ovflo),
  ssoff(ss_pad),
  ssoff(ss_frstat),
  ssoff(ss_frexcp1),
  ssoff(ss_frexcp2),
  ssoff(ss_frexcp3),
  ssoff(ss_frexcp4),
  ssoff(ss_frexcp5),
  ssoff(ss_frexcp6),
  ssoff(ss_frexcp7),
  ssoff(ss_fr4_hi),
  ssoff(ss_fr4_lo),
  ssoff(ss_fr5_hi),
  ssoff(ss_fr5_lo),
  ssoff(ss_fr6_hi),
  ssoff(ss_fr6_lo),
  ssoff(ss_fr7_hi),
  ssoff(ss_fr7_lo),
  ssoff(ss_fr8_hi),
  ssoff(ss_fr8_lo),
  ssoff(ss_fr9_hi),
  ssoff(ss_fr9_lo),
  ssoff(ss_fr10_hi),
  ssoff(ss_fr10_lo),
  ssoff(ss_fr11_hi),
  ssoff(ss_fr11_lo),
  ssoff(ss_fr12_hi),
  ssoff(ss_fr12_lo),
  ssoff(ss_fr13_hi),
  ssoff(ss_fr13_lo),
  ssoff(ss_fr14_hi),
  ssoff(ss_fr14_lo),
  ssoff(ss_fr15_hi),
  ssoff(ss_fr15_lo),
  ssoff(ss_fr16_hi),
  ssoff(ss_fr16_lo),
  ssoff(ss_fr17_hi),
  ssoff(ss_fr17_lo),
  ssoff(ss_fr18_hi),
  ssoff(ss_fr18_lo),
  ssoff(ss_fr19_hi),
  ssoff(ss_fr19_lo),
  ssoff(ss_fr20_hi),
  ssoff(ss_fr20_lo),
  ssoff(ss_fr21_hi),
  ssoff(ss_fr21_lo),
  ssoff(ss_fr22_hi),
  ssoff(ss_fr22_lo),
  ssoff(ss_fr23_hi),
  ssoff(ss_fr23_lo),
  ssoff(ss_fr24_hi),
  ssoff(ss_fr24_lo),
  ssoff(ss_fr25_hi),
  ssoff(ss_fr25_lo),
  ssoff(ss_fr26_hi),
  ssoff(ss_fr26_lo),
  ssoff(ss_fr27_hi),
  ssoff(ss_fr27_lo),
  ssoff(ss_fr28_hi),
  ssoff(ss_fr28_lo),
  ssoff(ss_fr29_hi),
  ssoff(ss_fr29_lo),
  ssoff(ss_fr30_hi),
  ssoff(ss_fr30_lo),
  ssoff(ss_fr31_hi),
  ssoff(ss_fr31_lo)
};

static int
hppa_hpux_cannot_fetch_register (int regnum)
{
  gdb_assert (regnum >= 0 && regnum < NUM_REGS);
  return (regnum >= ARRAY_SIZE(hppa_hpux_save_state_offset));
}

static int
hppa_hpux_cannot_store_register (int regnum)
{
  return hppa_hpux_cannot_fetch_register (regnum);
}

/* Just in case a future version of PA-RISC HP-UX won't have ptrace(2)
   at all.  */
#ifndef PTRACE_TYPE_RET
#define PTRACE_TYPE_RET void
#endif

static void
hppa_hpux_fetch_register (int regnum)
{
  CORE_ADDR addr;
  size_t size;
  PTRACE_TYPE_RET *buf;
  pid_t pid;
  int i;

  if (hppa_hpux_cannot_fetch_register (regnum))
    {
      regcache_raw_supply (current_regcache, regnum, NULL);
      return;
    }

  pid = ptid_get_pid (inferior_ptid);

  /* This isn't really an address.  But ptrace thinks of it as one.  */
  addr = hppa_hpux_save_state_offset[regnum];
  size = register_size (current_gdbarch, regnum);

  gdb_assert (size == 4 || size == 8);
  buf = alloca (size);

#ifdef HAVE_TTRACE
  {
    lwpid_t lwp = ptid_get_lwp (inferior_ptid);

    if (ttrace (TT_LWP_RUREGS, pid, lwp, addr, size, (uintptr_t)buf) == -1)
      error ("Couldn't read register %s (#%d): %s",
	     REGISTER_NAME (regnum), regnum, safe_strerror (errno));
  }
#else
  {
    int i;

    /* Read the register contents from the inferior a chuck at the time.  */
    for (i = 0; i < size / sizeof (PTRACE_TYPE_RET); i++)
      {
	errno = 0;
	buf[i] = ptrace (PT_RUREGS, pid, (PTRACE_TYPE_ARG3) addr, 0, 0);
	if (errno != 0)
	  error ("Couldn't read register %s (#%d): %s",
		 REGISTER_NAME (regnum), regnum, safe_strerror (errno));

	addr += sizeof (PTRACE_TYPE_RET);
      }
  }
#endif

  regcache_raw_supply (current_regcache, regnum, buf);
}

static void
hppa_hpux_fetch_inferior_registers (int regnum)
{
  if (regnum == -1)
    for (regnum = 0; regnum < NUM_REGS; regnum++)
      hppa_hpux_fetch_register (regnum);
  else
    hppa_hpux_fetch_register (regnum);
}

/* Store register REGNUM into the inferior.  */

static void
hppa_hpux_store_register (int regnum)
{
  CORE_ADDR addr;
  size_t size;
  PTRACE_TYPE_RET *buf;
  pid_t pid;

  if (hppa_hpux_cannot_store_register (regnum))
    return;

  pid = ptid_get_pid (inferior_ptid);

  /* This isn't really an address.  But ptrace thinks of it as one.  */
  addr = hppa_hpux_save_state_offset[regnum];
  size = register_size (current_gdbarch, regnum);

  gdb_assert (size == 4 || size == 8);
  buf = alloca (size);

  regcache_raw_collect (current_regcache, regnum, buf);

#ifdef HAVE_TTRACE
  {
    lwpid_t lwp = ptid_get_lwp (inferior_ptid);

    if (ttrace (TT_LWP_WUREGS, pid, lwp, addr, size, (uintptr_t)buf) == -1)
      error ("Couldn't write register %s (#%d): %s",
	     REGISTER_NAME (regnum), regnum, safe_strerror (errno));
  }
#else
  {
    int i;

    /* Write the register contents into the inferior a chunk at the time.  */
    for (i = 0; i < size / sizeof (PTRACE_TYPE_RET); i++)
      {
	errno = 0;
	ptrace (PT_WUREGS, pid, (PTRACE_TYPE_ARG3) addr, buf[i], 0);
	if (errno != 0)
	  error ("Couldn't write register %s (#%d): %s",
		 REGISTER_NAME (regnum), regnum, safe_strerror (errno));

	addr += sizeof (PTRACE_TYPE_RET);
      }
  }
#endif
}

/* Store register REGNUM back into the inferior.  If REGNUM is -1, do
   this for all registers (including the floating point registers).  */

static void
hppa_hpux_store_inferior_registers (int regnum)
{
  if (regnum == -1)
    for (regnum = 0; regnum < NUM_REGS; regnum++)
      hppa_hpux_store_register (regnum);
  else
    hppa_hpux_store_register (regnum);
}

static int
hppa_hpux_child_can_run (void)
{
  /* This variable is controlled by modules that layer their own process
     structure atop that provided here.  hpux-thread.c does this because
     of the HP-UX user-mode level thread model.  */

  return !child_suppress_run;
}


/* Prevent warning from -Wmissing-prototypes.  */
void _initialize_hppa_hpux_nat (void);

void
_initialize_hppa_hpux_nat (void)
{
  struct target_ops *t;

#ifdef HAVE_TTRACE
  t = inf_ttrace_target ();
#else
  t = inf_ptrace_target ();
#endif

  t->to_fetch_registers = hppa_hpux_fetch_inferior_registers;
  t->to_store_registers = hppa_hpux_store_inferior_registers;
  t->to_can_run = hppa_hpux_child_can_run;

  add_target (t);
}
