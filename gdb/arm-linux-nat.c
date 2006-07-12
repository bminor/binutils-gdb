/* GNU/Linux on ARM native support.
   Copyright (C) 1999, 2000, 2001, 2002, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "defs.h"
#include "inferior.h"
#include "gdbcore.h"
#include "gdb_string.h"
#include "regcache.h"
#include "target.h"
#include "linux-nat.h"

#include "arm-tdep.h"
#include "arm-linux-tdep.h"

#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/utsname.h>
#include <sys/procfs.h>

/* Prototypes for supply_gregset etc. */
#include "gregset.h"

/* Defines ps_err_e, struct ps_prochandle.  */
#include "gdb_proc_service.h"

#ifndef PTRACE_GET_THREAD_AREA
#define PTRACE_GET_THREAD_AREA 22
#endif

extern int arm_apcs_32;

/* The following variables are used to determine the version of the
   underlying GNU/Linux operating system.  Examples:

   GNU/Linux 2.0.35             GNU/Linux 2.2.12
   os_version = 0x00020023      os_version = 0x0002020c
   os_major = 2                 os_major = 2
   os_minor = 0                 os_minor = 2
   os_release = 35              os_release = 12

   Note: os_version = (os_major << 16) | (os_minor << 8) | os_release

   These are initialized using get_linux_version() from
   _initialize_arm_linux_nat().  */

static unsigned int os_version, os_major, os_minor, os_release;

/* On GNU/Linux, threads are implemented as pseudo-processes, in which
   case we may be tracing more than one process at a time.  In that
   case, inferior_ptid will contain the main process ID and the
   individual thread (process) ID.  get_thread_id () is used to get
   the thread id if it's available, and the process id otherwise.  */

int
get_thread_id (ptid_t ptid)
{
  int tid = TIDGET (ptid);
  if (0 == tid)
    tid = PIDGET (ptid);
  return tid;
}
#define GET_THREAD_ID(PTID)	get_thread_id ((PTID));

/* Get the value of a particular register from the floating point
   state of the process and store it into regcache.  */

static void
fetch_fpregister (int regno)
{
  int ret, tid;
  gdb_byte fp[ARM_LINUX_SIZEOF_NWFPE];
  
  /* Get the thread id for the ptrace call.  */
  tid = GET_THREAD_ID (inferior_ptid);

  /* Read the floating point state.  */
  ret = ptrace (PT_GETFPREGS, tid, 0, fp);
  if (ret < 0)
    {
      warning (_("Unable to fetch floating point register."));
      return;
    }

  /* Fetch fpsr.  */
  if (ARM_FPS_REGNUM == regno)
    regcache_raw_supply (current_regcache, ARM_FPS_REGNUM,
			 fp + NWFPE_FPSR_OFFSET);

  /* Fetch the floating point register.  */
  if (regno >= ARM_F0_REGNUM && regno <= ARM_F7_REGNUM)
    supply_nwfpe_register (current_regcache, regno, fp);
}

/* Get the whole floating point state of the process and store it
   into regcache.  */

static void
fetch_fpregs (void)
{
  int ret, regno, tid;
  gdb_byte fp[ARM_LINUX_SIZEOF_NWFPE];

  /* Get the thread id for the ptrace call.  */
  tid = GET_THREAD_ID (inferior_ptid);
  
  /* Read the floating point state.  */
  ret = ptrace (PT_GETFPREGS, tid, 0, fp);
  if (ret < 0)
    {
      warning (_("Unable to fetch the floating point registers."));
      return;
    }

  /* Fetch fpsr.  */
  regcache_raw_supply (current_regcache, ARM_FPS_REGNUM,
		       fp + NWFPE_FPSR_OFFSET);

  /* Fetch the floating point registers.  */
  for (regno = ARM_F0_REGNUM; regno <= ARM_F7_REGNUM; regno++)
    supply_nwfpe_register (current_regcache, regno, fp);
}

/* Save a particular register into the floating point state of the
   process using the contents from regcache.  */

static void
store_fpregister (int regno)
{
  int ret, tid;
  gdb_byte fp[ARM_LINUX_SIZEOF_NWFPE];

  /* Get the thread id for the ptrace call.  */
  tid = GET_THREAD_ID (inferior_ptid);
  
  /* Read the floating point state.  */
  ret = ptrace (PT_GETFPREGS, tid, 0, fp);
  if (ret < 0)
    {
      warning (_("Unable to fetch the floating point registers."));
      return;
    }

  /* Store fpsr.  */
  if (ARM_FPS_REGNUM == regno && register_cached (ARM_FPS_REGNUM))
    regcache_raw_collect (current_regcache, ARM_FPS_REGNUM,
			  fp + NWFPE_FPSR_OFFSET);

  /* Store the floating point register.  */
  if (regno >= ARM_F0_REGNUM && regno <= ARM_F7_REGNUM)
    collect_nwfpe_register (current_regcache, regno, fp);

  ret = ptrace (PTRACE_SETFPREGS, tid, 0, fp);
  if (ret < 0)
    {
      warning (_("Unable to store floating point register."));
      return;
    }
}

/* Save the whole floating point state of the process using
   the contents from regcache.  */

static void
store_fpregs (void)
{
  int ret, regno, tid;
  gdb_byte fp[ARM_LINUX_SIZEOF_NWFPE];

  /* Get the thread id for the ptrace call.  */
  tid = GET_THREAD_ID (inferior_ptid);
  
  /* Read the floating point state.  */
  ret = ptrace (PT_GETFPREGS, tid, 0, fp);
  if (ret < 0)
    {
      warning (_("Unable to fetch the floating point registers."));
      return;
    }

  /* Store fpsr.  */
  if (register_cached (ARM_FPS_REGNUM))
    regcache_raw_collect (current_regcache, ARM_FPS_REGNUM,
			  fp + NWFPE_FPSR_OFFSET);

  /* Store the floating point registers.  */
  for (regno = ARM_F0_REGNUM; regno <= ARM_F7_REGNUM; regno++)
    if (register_cached (regno))
      collect_nwfpe_register (current_regcache, regno, fp);

  ret = ptrace (PTRACE_SETFPREGS, tid, 0, fp);
  if (ret < 0)
    {
      warning (_("Unable to store floating point registers."));
      return;
    }
}

/* Fetch a general register of the process and store into
   regcache.  */

static void
fetch_register (int regno)
{
  int ret, tid;
  elf_gregset_t regs;

  /* Get the thread id for the ptrace call.  */
  tid = GET_THREAD_ID (inferior_ptid);
  
  ret = ptrace (PTRACE_GETREGS, tid, 0, &regs);
  if (ret < 0)
    {
      warning (_("Unable to fetch general register."));
      return;
    }

  if (regno >= ARM_A1_REGNUM && regno < ARM_PC_REGNUM)
    regcache_raw_supply (current_regcache, regno, (char *) &regs[regno]);

  if (ARM_PS_REGNUM == regno)
    {
      if (arm_apcs_32)
        regcache_raw_supply (current_regcache, ARM_PS_REGNUM,
			     (char *) &regs[ARM_CPSR_REGNUM]);
      else
        regcache_raw_supply (current_regcache, ARM_PS_REGNUM,
			     (char *) &regs[ARM_PC_REGNUM]);
    }
    
  if (ARM_PC_REGNUM == regno)
    { 
      regs[ARM_PC_REGNUM] = ADDR_BITS_REMOVE (regs[ARM_PC_REGNUM]);
      regcache_raw_supply (current_regcache, ARM_PC_REGNUM,
			   (char *) &regs[ARM_PC_REGNUM]);
    }
}

/* Fetch all general registers of the process and store into
   regcache.  */

static void
fetch_regs (void)
{
  int ret, regno, tid;
  elf_gregset_t regs;

  /* Get the thread id for the ptrace call.  */
  tid = GET_THREAD_ID (inferior_ptid);
  
  ret = ptrace (PTRACE_GETREGS, tid, 0, &regs);
  if (ret < 0)
    {
      warning (_("Unable to fetch general registers."));
      return;
    }

  for (regno = ARM_A1_REGNUM; regno < ARM_PC_REGNUM; regno++)
    regcache_raw_supply (current_regcache, regno, (char *) &regs[regno]);

  if (arm_apcs_32)
    regcache_raw_supply (current_regcache, ARM_PS_REGNUM,
			 (char *) &regs[ARM_CPSR_REGNUM]);
  else
    regcache_raw_supply (current_regcache, ARM_PS_REGNUM,
			 (char *) &regs[ARM_PC_REGNUM]);

  regs[ARM_PC_REGNUM] = ADDR_BITS_REMOVE (regs[ARM_PC_REGNUM]);
  regcache_raw_supply (current_regcache, ARM_PC_REGNUM,
		       (char *) &regs[ARM_PC_REGNUM]);
}

/* Store all general registers of the process from the values in
   regcache.  */

static void
store_register (int regno)
{
  int ret, tid;
  elf_gregset_t regs;
  
  if (!register_cached (regno))
    return;

  /* Get the thread id for the ptrace call.  */
  tid = GET_THREAD_ID (inferior_ptid);
  
  /* Get the general registers from the process.  */
  ret = ptrace (PTRACE_GETREGS, tid, 0, &regs);
  if (ret < 0)
    {
      warning (_("Unable to fetch general registers."));
      return;
    }

  if (regno >= ARM_A1_REGNUM && regno <= ARM_PC_REGNUM)
    regcache_raw_collect (current_regcache, regno, (char *) &regs[regno]);
  else if (arm_apcs_32 && regno == ARM_PS_REGNUM)
    regcache_raw_collect (current_regcache, regno,
			 (char *) &regs[ARM_CPSR_REGNUM]);
  else if (!arm_apcs_32 && regno == ARM_PS_REGNUM)
    regcache_raw_collect (current_regcache, ARM_PC_REGNUM,
			 (char *) &regs[ARM_PC_REGNUM]);

  ret = ptrace (PTRACE_SETREGS, tid, 0, &regs);
  if (ret < 0)
    {
      warning (_("Unable to store general register."));
      return;
    }
}

static void
store_regs (void)
{
  int ret, regno, tid;
  elf_gregset_t regs;

  /* Get the thread id for the ptrace call.  */
  tid = GET_THREAD_ID (inferior_ptid);
  
  /* Fetch the general registers.  */
  ret = ptrace (PTRACE_GETREGS, tid, 0, &regs);
  if (ret < 0)
    {
      warning (_("Unable to fetch general registers."));
      return;
    }

  for (regno = ARM_A1_REGNUM; regno <= ARM_PC_REGNUM; regno++)
    {
      if (register_cached (regno))
	regcache_raw_collect (current_regcache, regno, (char *) &regs[regno]);
    }

  if (arm_apcs_32 && register_cached (ARM_PS_REGNUM))
    regcache_raw_collect (current_regcache, ARM_PS_REGNUM,
			 (char *) &regs[ARM_CPSR_REGNUM]);

  ret = ptrace (PTRACE_SETREGS, tid, 0, &regs);

  if (ret < 0)
    {
      warning (_("Unable to store general registers."));
      return;
    }
}

/* Fetch registers from the child process.  Fetch all registers if
   regno == -1, otherwise fetch all general registers or all floating
   point registers depending upon the value of regno.  */

static void
arm_linux_fetch_inferior_registers (int regno)
{
  if (-1 == regno)
    {
      fetch_regs ();
      fetch_fpregs ();
    }
  else 
    {
      if (regno < ARM_F0_REGNUM || regno > ARM_FPS_REGNUM)
        fetch_register (regno);

      if (regno >= ARM_F0_REGNUM && regno <= ARM_FPS_REGNUM)
        fetch_fpregister (regno);
    }
}

/* Store registers back into the inferior.  Store all registers if
   regno == -1, otherwise store all general registers or all floating
   point registers depending upon the value of regno.  */

static void
arm_linux_store_inferior_registers (int regno)
{
  if (-1 == regno)
    {
      store_regs ();
      store_fpregs ();
    }
  else
    {
      if ((regno < ARM_F0_REGNUM) || (regno > ARM_FPS_REGNUM))
        store_register (regno);

      if ((regno >= ARM_F0_REGNUM) && (regno <= ARM_FPS_REGNUM))
        store_fpregister (regno);
    }
}

/* Wrapper functions for the standard regset handling, used by
   thread debugging.  */

void
fill_gregset (gdb_gregset_t *gregsetp, int regno)
{
  arm_linux_collect_gregset (NULL, current_regcache, regno, gregsetp, 0);
}

void
supply_gregset (gdb_gregset_t *gregsetp)
{
  arm_linux_supply_gregset (NULL, current_regcache, -1, gregsetp, 0);
}

void
fill_fpregset (gdb_fpregset_t *fpregsetp, int regno)
{
  arm_linux_collect_nwfpe (NULL, current_regcache, regno, fpregsetp, 0);
}

/* Fill GDB's register array with the floating-point register values
   in *fpregsetp.  */

void
supply_fpregset (gdb_fpregset_t *fpregsetp)
{
  arm_linux_supply_nwfpe (NULL, current_regcache, -1, fpregsetp, 0);
}

int
arm_linux_kernel_u_size (void)
{
  return (sizeof (struct user));
}

/* Fetch the thread-local storage pointer for libthread_db.  */

ps_err_e
ps_get_thread_area (const struct ps_prochandle *ph,
                    lwpid_t lwpid, int idx, void **base)
{
  if (ptrace (PTRACE_GET_THREAD_AREA, lwpid, NULL, base) != 0)
    return PS_ERR;

  /* IDX is the bias from the thread pointer to the beginning of the
     thread descriptor.  It has to be subtracted due to implementation
     quirks in libthread_db.  */
  *base = (void *) ((char *)*base - idx);

  return PS_OK;
}

static unsigned int
get_linux_version (unsigned int *vmajor,
		   unsigned int *vminor,
		   unsigned int *vrelease)
{
  struct utsname info;
  char *pmajor, *pminor, *prelease, *tail;

  if (-1 == uname (&info))
    {
      warning (_("Unable to determine GNU/Linux version."));
      return -1;
    }

  pmajor = strtok (info.release, ".");
  pminor = strtok (NULL, ".");
  prelease = strtok (NULL, ".");

  *vmajor = (unsigned int) strtoul (pmajor, &tail, 0);
  *vminor = (unsigned int) strtoul (pminor, &tail, 0);
  *vrelease = (unsigned int) strtoul (prelease, &tail, 0);

  return ((*vmajor << 16) | (*vminor << 8) | *vrelease);
}

void _initialize_arm_linux_nat (void);

void
_initialize_arm_linux_nat (void)
{
  struct target_ops *t;

  os_version = get_linux_version (&os_major, &os_minor, &os_release);

  /* Fill in the generic GNU/Linux methods.  */
  t = linux_target ();

  /* Add our register access methods.  */
  t->to_fetch_registers = arm_linux_fetch_inferior_registers;
  t->to_store_registers = arm_linux_store_inferior_registers;

  /* Register the target.  */
  linux_nat_add_target (t);
}
