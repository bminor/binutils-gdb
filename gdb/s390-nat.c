/* S390 native-dependent code for GDB, the GNU debugger.
   Copyright 2001, 2003 Free Software Foundation, Inc

   Contributed by D.J. Barrow (djbarrow@de.ibm.com,barrow_dj@yahoo.com)
   for IBM Deutschland Entwicklung GmbH, IBM Corporation.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include "defs.h"
#include "tm.h"
#include "regcache.h"
#include "inferior.h"

#include "s390-tdep.h"

#include <asm/ptrace.h>
#include <sys/ptrace.h>
#include <asm/types.h>
#include <sys/procfs.h>
#include <sys/user.h>
#include <sys/ucontext.h>


/* Map registers to gregset/ptrace offsets.
   These arrays are defined in s390-tdep.c.  */

#ifdef __s390x__
#define regmap_gregset s390x_regmap_gregset
#else
#define regmap_gregset s390_regmap_gregset
#endif

#define regmap_fpregset s390_regmap_fpregset


/* Fill GDB's register array with the general-purpose register values
   in *REGP.  */
void
supply_gregset (gregset_t *regp)
{
  int i;
  for (i = 0; i < S390_NUM_REGS; i++)
    if (regmap_gregset[i] != -1)
      regcache_raw_supply (current_regcache, i, 
			   (char *)regp + regmap_gregset[i]);
}

/* Fill register REGNO (if it is a general-purpose register) in
   *REGP with the value in GDB's register array.  If REGNO is -1,
   do this for all registers.  */
void
fill_gregset (gregset_t *regp, int regno)
{
  int i;
  for (i = 0; i < S390_NUM_REGS; i++)
    if (regmap_gregset[i] != -1)
      if (regno == -1 || regno == i)
	regcache_raw_collect (current_regcache, i, 
			      (char *)regp + regmap_gregset[i]);
}

/* Fill GDB's register array with the floating-point register values
   in *REGP.  */
void
supply_fpregset (fpregset_t *regp)
{
  int i;
  for (i = 0; i < S390_NUM_REGS; i++)
    if (regmap_fpregset[i] != -1)
      regcache_raw_supply (current_regcache, i,
			   ((char *)regp) + regmap_fpregset[i]);
}

/* Fill register REGNO (if it is a general-purpose register) in
   *REGP with the value in GDB's register array.  If REGNO is -1,
   do this for all registers.  */
void
fill_fpregset (fpregset_t *regp, int regno)
{
  int i;
  for (i = 0; i < S390_NUM_REGS; i++)
    if (regmap_fpregset[i] != -1)
      if (regno == -1 || regno == i)
        regcache_raw_collect (current_regcache, i, 
			      ((char *)regp) + regmap_fpregset[i]);
}

/* Find the TID for the current inferior thread to use with ptrace.  */
static int
s390_inferior_tid (void)
{
  /* GNU/Linux LWP ID's are process ID's.  */
  int tid = TIDGET (inferior_ptid);
  if (tid == 0)
    tid = PIDGET (inferior_ptid); /* Not a threaded program.  */

  return tid;
}

/* Fetch all general-purpose registers from process/thread TID and
   store their values in GDB's register cache.  */
static void
fetch_regs (int tid)
{
  gregset_t regs;
  ptrace_area parea;

  parea.len = sizeof (regs);
  parea.process_addr = (addr_t) &regs;
  parea.kernel_addr = offsetof (struct user_regs_struct, psw);
  if (ptrace (PTRACE_PEEKUSR_AREA, tid, (long) &parea) < 0)
    perror_with_name ("Couldn't get registers");

  supply_gregset (&regs);
}

/* Store all valid general-purpose registers in GDB's register cache
   into the process/thread specified by TID.  */
static void
store_regs (int tid, int regnum)
{
  gregset_t regs;
  ptrace_area parea;

  parea.len = sizeof (regs);
  parea.process_addr = (addr_t) &regs;
  parea.kernel_addr = offsetof (struct user_regs_struct, psw);
  if (ptrace (PTRACE_PEEKUSR_AREA, tid, (long) &parea) < 0)
    perror_with_name ("Couldn't get registers");

  fill_gregset (&regs, regnum);

  if (ptrace (PTRACE_POKEUSR_AREA, tid, (long) &parea) < 0)
    perror_with_name ("Couldn't write registers");
}

/* Fetch all floating-point registers from process/thread TID and store
   their values in GDB's register cache.  */
static void
fetch_fpregs (int tid)
{
  fpregset_t fpregs;
  ptrace_area parea;

  parea.len = sizeof (fpregs);
  parea.process_addr = (addr_t) &fpregs;
  parea.kernel_addr = offsetof (struct user_regs_struct, fp_regs);
  if (ptrace (PTRACE_PEEKUSR_AREA, tid, (long) &parea) < 0)
    perror_with_name ("Couldn't get floating point status");

  supply_fpregset (&fpregs);
}

/* Store all valid floating-point registers in GDB's register cache
   into the process/thread specified by TID.  */
static void
store_fpregs (int tid, int regnum)
{
  fpregset_t fpregs;
  ptrace_area parea;

  parea.len = sizeof (fpregs);
  parea.process_addr = (addr_t) &fpregs;
  parea.kernel_addr = offsetof (struct user_regs_struct, fp_regs);
  if (ptrace (PTRACE_PEEKUSR_AREA, tid, (long) &parea) < 0)
    perror_with_name ("Couldn't get floating point status");

  fill_fpregset (&fpregs, regnum);

  if (ptrace (PTRACE_POKEUSR_AREA, tid, (long) &parea) < 0)
    perror_with_name ("Couldn't write floating point status");
}

/* Fetch register REGNUM from the child process.  If REGNUM is -1, do
   this for all registers.  */
void
fetch_inferior_registers (int regnum)
{
  int tid = s390_inferior_tid ();

  if (regnum == -1 
      || (regnum < S390_NUM_REGS && regmap_gregset[regnum] != -1))
    fetch_regs (tid);

  if (regnum == -1 
      || (regnum < S390_NUM_REGS && regmap_fpregset[regnum] != -1))
    fetch_fpregs (tid);
}

/* Store register REGNUM back into the child process.  If REGNUM is
   -1, do this for all registers.  */
void
store_inferior_registers (int regnum)
{
  int tid = s390_inferior_tid ();

  if (regnum == -1 
      || (regnum < S390_NUM_REGS && regmap_gregset[regnum] != -1))
    store_regs (tid, regnum);

  if (regnum == -1 
      || (regnum < S390_NUM_REGS && regmap_fpregset[regnum] != -1))
    store_fpregs (tid, regnum);
}


/* watch_areas are required if you put 2 or more watchpoints on the same 
   address or overlapping areas gdb will call us to delete the watchpoint 
   more than once when we try to delete them.
   attempted reference counting to reduce the number of areas unfortunately
   they didn't shrink when areas had to be split overlapping occurs. */
struct watch_area;
typedef struct watch_area watch_area;
struct watch_area
{
  watch_area *next;
  CORE_ADDR lo_addr;
  CORE_ADDR hi_addr;
};

static watch_area *watch_base = NULL;
int watch_area_cnt = 0;
static CORE_ADDR watch_lo_addr = 0, watch_hi_addr = 0;



CORE_ADDR
s390_stopped_by_watchpoint (int pid)
{
  per_lowcore_bits per_lowcore;
  ptrace_area parea;

  parea.len = sizeof (per_lowcore);
  parea.process_addr = (addr_t) & per_lowcore;
  parea.kernel_addr = offsetof (struct user_regs_struct, per_info.lowcore);
  ptrace (PTRACE_PEEKUSR_AREA, pid, &parea);
  return ((per_lowcore.perc_storage_alteration == 1) &&
	  (per_lowcore.perc_store_real_address == 0));
}


void
s390_fix_watch_points (int pid)
{
  per_struct per_info;
  ptrace_area parea;

  parea.len = sizeof (per_info);
  parea.process_addr = (addr_t) & per_info;
  parea.kernel_addr = PT_CR_9;
  ptrace (PTRACE_PEEKUSR_AREA, pid, &parea);
  /* The kernel automatically sets the psw for per depending */
  /* on whether the per control registers are set for event recording */
  /* & sets cr9 & cr10 appropriately also */
  if (watch_area_cnt)
    {
      per_info.control_regs.bits.em_storage_alteration = 1;
      per_info.control_regs.bits.storage_alt_space_ctl = 1;
    }
  else
    {
      per_info.control_regs.bits.em_storage_alteration = 0;
      per_info.control_regs.bits.storage_alt_space_ctl = 0;
    }
  per_info.starting_addr = watch_lo_addr;
  per_info.ending_addr = watch_hi_addr;
  ptrace (PTRACE_POKEUSR_AREA, pid, &parea);
}

int
s390_insert_watchpoint (int pid, CORE_ADDR addr, int len, int rw)
{
  CORE_ADDR hi_addr = addr + len - 1;
  watch_area *newarea = (watch_area *) xmalloc (sizeof (watch_area));


  if (newarea)
    {
      newarea->next = watch_base;
      watch_base = newarea;
      watch_lo_addr = min (watch_lo_addr, addr);
      watch_hi_addr = max (watch_hi_addr, hi_addr);
      newarea->lo_addr = addr;
      newarea->hi_addr = hi_addr;
      if (watch_area_cnt == 0)
	{
	  watch_lo_addr = newarea->lo_addr;
	  watch_hi_addr = newarea->hi_addr;
	}
      watch_area_cnt++;
      s390_fix_watch_points (pid);
    }
  return newarea ? 0 : -1;
}


int
s390_remove_watchpoint (int pid, CORE_ADDR addr, int len)
{
  watch_area *curr = watch_base, *prev, *matchCurr;
  CORE_ADDR hi_addr = addr + len - 1;
  CORE_ADDR watch_second_lo_addr = 0xffffffffUL, watch_second_hi_addr = 0;
  int lo_addr_ref_cnt, hi_addr_ref_cnt;
  prev = matchCurr = NULL;
  lo_addr_ref_cnt = (addr == watch_lo_addr);
  hi_addr_ref_cnt = (addr == watch_hi_addr);
  while (curr)
    {
      if (matchCurr == NULL)
	{
	  if (curr->lo_addr == addr && curr->hi_addr == hi_addr)
	    {
	      matchCurr = curr;
	      if (prev)
		prev->next = curr->next;
	      else
		watch_base = curr->next;
	    }
	  prev = curr;
	}
      if (lo_addr_ref_cnt)
	{
	  if (watch_lo_addr == curr->lo_addr)
	    lo_addr_ref_cnt++;
	  if (curr->lo_addr > watch_lo_addr &&
	      curr->lo_addr < watch_second_lo_addr)
	    watch_second_lo_addr = curr->lo_addr;
	}
      if (hi_addr_ref_cnt)
	{
	  if (watch_hi_addr == curr->hi_addr)
	    hi_addr_ref_cnt++;
	  if (curr->hi_addr < watch_hi_addr &&
	      curr->hi_addr > watch_second_hi_addr)
	    watch_second_hi_addr = curr->hi_addr;
	}
      curr = curr->next;
    }
  if (matchCurr)
    {
      xfree (matchCurr);
      watch_area_cnt--;
      if (watch_area_cnt)
	{
	  if (lo_addr_ref_cnt == 2)
	    watch_lo_addr = watch_second_lo_addr;
	  if (hi_addr_ref_cnt == 2)
	    watch_hi_addr = watch_second_hi_addr;
	}
      else
	{
	  watch_lo_addr = watch_hi_addr = 0;
	}
      s390_fix_watch_points (pid);
      return 0;
    }
  else
    {
      fprintf_unfiltered (gdb_stderr,
			  "Attempt to remove nonexistent watchpoint in s390_remove_watchpoint\n");
      return -1;
    }
}

int
kernel_u_size (void)
{
  return sizeof (struct user);
}

