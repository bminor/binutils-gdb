/* Machine-dependent hooks for the unix child process stratum.  This
   code is for the HP PA-RISC cpu.

   Copyright 1986, 1987, 1989, 1990, 1991, 1992, 1993 Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
#include "inferior.h"
#include "target.h"
#include <sys/ptrace.h>

/* Use an extra level of indirection for ptrace calls.
   This lets us breakpoint usefully on call_ptrace.   It also
   allows us to pass an extra argument to ptrace without
   using an ANSI-C specific macro.  */

#define ptrace call_ptrace

#if !defined (offsetof)
#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER)
#endif

/* U_REGS_OFFSET is the offset of the registers within the u area.  */
#if !defined (U_REGS_OFFSET)
#define U_REGS_OFFSET \
  ptrace (PT_READ_U, inferior_pid, \
          (PTRACE_ARG3_TYPE) (offsetof (struct user, u_ar0)), 0) \
    - KERNEL_U_ADDR
#endif

/* Fetch one register.  */

static void
fetch_register (regno)
     int regno;
{
  register unsigned int regaddr;
  char buf[MAX_REGISTER_RAW_SIZE];
  register int i;

  /* Offset of registers within the u area.  */
  unsigned int offset;

  offset = U_REGS_OFFSET;

  regaddr = register_addr (regno, offset);
  for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
    {
      errno = 0;
      *(int *) &buf[i] = ptrace (PT_RUREGS, inferior_pid,
				 (PTRACE_ARG3_TYPE) regaddr, 0);
      regaddr += sizeof (int);
      if (errno != 0)
	{
	  /* Warning, not error, in case we are attached; sometimes the
	     kernel doesn't let us at the registers.  */
	  char *err = safe_strerror (errno);
	  char *msg = alloca (strlen (err) + 128);
	  sprintf (msg, "reading register %s: %s", reg_names[regno], err);
	  warning (msg);
	  goto error_exit;
	}
    }
  supply_register (regno, buf);
 error_exit:;
}

/* Fetch all registers, or just one, from the child process.  */

void
fetch_inferior_registers (regno)
     int regno;
{
  if (regno == -1)
    for (regno = 0; regno < NUM_REGS; regno++)
      fetch_register (regno);
  else
    fetch_register (regno);
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

void
store_inferior_registers (regno)
     int regno;
{
  register unsigned int regaddr;
  extern char registers[];
  register int i;

  unsigned int offset = U_REGS_OFFSET;

  if (regno >= 0)
    {
      regaddr = register_addr (regno, offset);
      for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof(int))
	{
	  errno = 0;
	  ptrace (PT_WUREGS, inferior_pid, (PTRACE_ARG3_TYPE) regaddr,
		  *(int *) &registers[REGISTER_BYTE (regno) + i]);
	  if (errno != 0)
	    {
	      char *err = safe_strerror (errno);
	      char *msg = alloca (strlen (err) + 128);
	      sprintf (msg, "writing register %s: %s", reg_names[regno], err);
	      warning (msg);
	    }
	  regaddr += sizeof(int);
	}
    }
  else
    {
      for (regno = 0; regno < NUM_REGS; regno++)
	{
	  if (CANNOT_STORE_REGISTER (regno))
	    continue;
	  store_inferior_registers (regno);
	}
    }
  return;
}


/* PT_PROT is specific to the PA BSD kernel and isn't documented
   anywhere (except here).  

   PT_PROT allows one to enable/disable the data memory break bit
   for pages of memory in an inferior process.  This bit is used
   to cause "Data memory break traps" to occur when the appropriate
   page is written to.

   The arguments are as follows:

      PT_PROT -- The ptrace action to perform.

      INFERIOR_PID -- The pid of the process who's page table entries
      will be modified.

      PT_ARGS -- The *address* of a 3 word block of memory which has
      additional information:

        word 0 -- The start address to watch.  This should be a page-aligned
	address.

	word 1 -- The ending address to watch.  Again, this should be a 
	page aligned address.

	word 2 -- Nonzero to enable the data memory break bit on the
	given address range or zero to disable the data memory break
	bit on the given address range.

  This call may fail if the given addresses are not valid in the inferior
  process.  This most often happens when restarting a program which
  as watchpoints inserted on heap or stack memory.  */
	
#define PT_PROT 21

int
hppa_set_watchpoint (addr, len, flag)
     int addr, len, flag;
{
  int pt_args[3];
  pt_args[0] = addr;
  pt_args[1] = addr + len;
  pt_args[2] = flag;

  /* Mask off the lower 12 bits since we want to work on a page basis.  */
  pt_args[0] >>= 12; 
  pt_args[1] >>= 12; 

  /* Rounding adjustments.  */
  pt_args[1] -= pt_args[0]; 
  pt_args[1]++; 

  /* Put the lower 12 bits back as zero.  */
  pt_args[0] <<= 12; 
  pt_args[1] <<= 12; 

  /* Do it.  */
  return ptrace (PT_PROT, inferior_pid, (PTRACE_ARG3_TYPE) pt_args, 0);
}
