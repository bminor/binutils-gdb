/* GNU/Linux/ARM specific low level interface, for the remote server for GDB.
   Copyright 1995, 1996, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "server.h"
#include "linux-low.h"

#include <sys/ptrace.h>

/* Correct for all GNU/Linux targets (for quite some time).  */
#define GDB_GREGSET_T elf_gregset_t
#define GDB_FPREGSET_T elf_fpregset_t

#ifndef HAVE_ELF_FPREGSET_T
/* Make sure we have said types.  Not all platforms bring in <linux/elf.h>
   via <sys/procfs.h>.  */
#ifdef HAVE_LINUX_ELF_H   
#include <linux/elf.h>    
#endif
#endif
   
#include "../gdb_proc_service.h"

#ifndef PTRACE_GET_THREAD_AREA
#define PTRACE_GET_THREAD_AREA 22
#endif

#define PTRACE_GETWMMXREGS 18
#define PTRACE_SETWMMXREGS 19

#ifdef HAVE_SYS_REG_H
#include <sys/reg.h>
#endif

#define arm_num_regs 26

static int arm_regmap[] = {
  0, 4, 8, 12, 16, 20, 24, 28,
  32, 36, 40, 44, 48, 52, 56, 60,
  -1, -1, -1, -1, -1, -1, -1, -1, -1,
  64
};

static int
arm_cannot_store_register (int regno)
{
  return (regno >= arm_num_regs);
}

static int
arm_cannot_fetch_register (int regno)
{
  return (regno >= arm_num_regs);
}

extern int debug_threads;

static CORE_ADDR
arm_get_pc ()
{
  unsigned long pc;
  collect_register_by_name ("pc", &pc);
  if (debug_threads)
    fprintf (stderr, "stop pc is %08lx\n", pc);
  return pc;
}

static void
arm_set_pc (CORE_ADDR pc)
{
  unsigned long newpc = pc;
  supply_register_by_name ("pc", &newpc);
}

/* Correct in either endianness.  We do not support Thumb yet.  */
static const unsigned long arm_breakpoint = 0xef9f0001;
#define arm_breakpoint_len 4

static int
arm_breakpoint_at (CORE_ADDR where)
{
  unsigned long insn;

  (*the_target->read_memory) (where, (unsigned char *) &insn, 4);
  if (insn == arm_breakpoint)
    return 1;

  /* If necessary, recognize more trap instructions here.  GDB only uses the
     one.  */
  return 0;
}
  
static void
arm_fill_gregset (void *buf)
{
  int i;

  for (i = 0; i < arm_num_regs; i++)
    if (arm_regmap[i] != -1)
      collect_register (i, ((char *) buf) + arm_regmap[i]);
}
 
static void
arm_store_gregset (const void *buf)
{
  int i;
  char zerobuf[8];

  memset (zerobuf, 0, 8); 
  for (i = 0; i < arm_num_regs; i++)
    if (arm_regmap[i] != -1)
      supply_register (i, ((char *) buf) + arm_regmap[i]);
    else
      supply_register (i, zerobuf);
}
 
static void
arm_fill_wmmxregset (void *buf)
{
  int i;

  for (i = 0; i < 16; i++)
    collect_register (arm_num_regs + i, ((char *) buf) + i * 8);
 
  for (i = 0; i < 2; i++)
    collect_register (arm_num_regs + i + 16 + 2, ((char *) buf) + 16 * 8 + i * 4);
 
  for (i = 0; i < 4; i++)
    collect_register (arm_num_regs + i + 16 + 8, ((char *) buf) + 16 * 8 + 8 + i * 4);
}
 
static void
arm_store_wmmxregset (const void *buf)
{
  int i;

  for (i = 0; i < 16; i++)
    supply_register (arm_num_regs + i, ((char *) buf) + i * 8);
  
  for (i = 0; i < 2; i++)
    supply_register (arm_num_regs + i + 16 + 2, ((char *) buf) + 16 * 8 + i * 4);
 
  for (i = 0; i < 4; i++)
    supply_register (arm_num_regs + i + 16 + 8, ((char *) buf) + 16 * 8 + 8 + i * 4);
}

char *
arm_available_registers (void)
{
  char buf[64];

  if (use_regsets_p && target_regsets[1].size > 0)
    {
      int wr0 = find_regno ("wr0");
      sprintf (buf, "iwmmxt:%x", wr0);
      return strdup (buf);
    }

  return NULL;
}

struct regset_info target_regsets[] = {
  { PTRACE_GETREGS, PTRACE_SETREGS, sizeof (elf_gregset_t),
    GENERAL_REGS,
    arm_fill_gregset, arm_store_gregset },
  { PTRACE_GETWMMXREGS, PTRACE_SETWMMXREGS, 16 * 8 + 6 * 4,
    EXTENDED_REGS,
    arm_fill_wmmxregset, arm_store_wmmxregset },
  { 0, 0, -1, -1, NULL, NULL }
};

/* We only place breakpoints in empty marker functions, and thread locking
   is outside of the function.  So rather than importing software single-step,
   we can just run until exit.  */
static CORE_ADDR
arm_reinsert_addr ()
{
  unsigned long pc;
  collect_register_by_name ("lr", &pc);
  return pc;
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

struct linux_target_ops the_low_target = {
  arm_num_regs,
  arm_regmap,
  arm_cannot_fetch_register,
  arm_cannot_store_register,
  arm_get_pc,
  arm_set_pc,
  (const unsigned char *) &arm_breakpoint,
  arm_breakpoint_len,
  arm_reinsert_addr,
  0,
  arm_breakpoint_at,
  arm_available_registers
};
