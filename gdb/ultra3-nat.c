/* Host-dependent code for GDB, for NYU Ultra3 running Sym1 OS.
   Copyright (C) 1988, 1989, 1991, 1992 Free Software Foundation, Inc.
   Contributed by David Wood (wood@nyu.edu) at New York University.

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

#define DEBUG
#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "symtab.h"
#include "value.h"

#include <sys/types.h>
#include <sys/param.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>  

#include "gdbcore.h"

#include <sys/file.h>
#include <sys/stat.h>

/* Get all available registers from the inferior.  Registers that are
 * defined in REGISTER_NAMES, but not available to the user/gdb are
 * supplied as -1.  This may include gr64-gr95 and the protected special
 * purpose registers.
 */

void
fetch_inferior_registers (regno)
  int regno;
{
  register int i,j,ret_val=0;
  char buf[128];

  if (regno != -1) {
    fetch_register (regno);
    return;
  }

/* Global Registers */
#ifdef ULTRA3
  errno = 0;
  ptrace (PT_READ_STRUCT, inferior_pid,
	  (PTRACE_ARG3_TYPE) register_addr(GR96_REGNUM,0), 
	  (int)&pt_struct.pt_gr[0], 32*4);
  if (errno != 0) {
      perror_with_name ("reading global registers");
      ret_val = -1;
  } else for (regno=GR96_REGNUM, j=0 ; j<32 ; regno++, j++)  {
      supply_register (regno, &pt_struct.pt_gr[j]);
  }
#else
  for (regno=GR96_REGNUM ; !ret_val && regno < GR96_REGNUM+32 ; regno++)
    fetch_register(regno);
#endif

/* Local Registers */
#ifdef ULTRA3
  errno = 0;
  ptrace (PT_READ_STRUCT, inferior_pid,
	  (PTRACE_ARG3_TYPE) register_addr(LR0_REGNUM,0), 
	  (int)&pt_struct.pt_lr[0], 128*4);
  if (errno != 0) {
      perror_with_name ("reading local registers");
      ret_val = -1;
  } else for (regno=LR0_REGNUM, j=0 ; j<128 ; regno++, j++)  {
      supply_register (regno, &pt_struct.pt_lr[j]);
  }
#else
  for (regno=LR0_REGNUM ; !ret_val && regno < LR0_REGNUM+128 ; regno++)
    fetch_register(regno);
#endif

/* Special Registers */
  fetch_register(GR1_REGNUM);
  fetch_register(CPS_REGNUM);
  fetch_register(PC_REGNUM);
  fetch_register(NPC_REGNUM);
  fetch_register(PC2_REGNUM);
  fetch_register(IPC_REGNUM);
  fetch_register(IPA_REGNUM);
  fetch_register(IPB_REGNUM);
  fetch_register(Q_REGNUM);
  fetch_register(BP_REGNUM);
  fetch_register(FC_REGNUM);

/* Fake any registers that are in REGISTER_NAMES, but not available to gdb */ 
  registers_fetched();
}

/* Store our register values back into the inferior.
 * If REGNO is -1, do this for all registers.
 * Otherwise, REGNO specifies which register (so we can save time).  
 * NOTE: Assumes AMD's binary compatibility standard. 
 */

void
store_inferior_registers (regno)
     int regno;
{
  register unsigned int regaddr;
  char buf[80];

  if (regno >= 0)
    {
      if (CANNOT_STORE_REGISTER(regno)) 
	return;
      regaddr = register_addr (regno, 0);
      errno = 0;
      ptrace (PT_WRITE_U, inferior_pid,
	      (PTRACE_ARG3_TYPE) regaddr, read_register(regno));
      if (errno != 0)
	{
	  sprintf (buf, "writing register %s (#%d)", reg_names[regno],regno);
	  perror_with_name (buf);
	}
    }
  else
    {
#ifdef ULTRA3
      pt_struct.pt_gr1 = read_register(GR1_REGNUM);
      for (regno = GR96_REGNUM; regno < GR96_REGNUM+32; regno++)
	pt_struct.pt_gr[regno] = read_register(regno);
      for (regno = LR0_REGNUM; regno < LR0_REGNUM+128; regno++)
	pt_struct.pt_gr[regno] = read_register(regno);
      errno = 0;
      ptrace (PT_WRITE_STRUCT, inferior_pid,
	      (PTRACE_ARG3_TYPE) register_addr(GR1_REGNUM,0), 
	      (int)&pt_struct.pt_gr1,(1*32*128)*4);
      if (errno != 0)
	{
	   sprintf (buf, "writing all local/global registers");
	   perror_with_name (buf);
	}
      pt_struct.pt_psr = read_register(CPS_REGNUM);
      pt_struct.pt_pc0 = read_register(NPC_REGNUM);
      pt_struct.pt_pc1 = read_register(PC_REGNUM);
      pt_struct.pt_pc2 = read_register(PC2_REGNUM);
      pt_struct.pt_ipc = read_register(IPC_REGNUM);
      pt_struct.pt_ipa = read_register(IPA_REGNUM);
      pt_struct.pt_ipb = read_register(IPB_REGNUM);
      pt_struct.pt_q   = read_register(Q_REGNUM);
      pt_struct.pt_bp  = read_register(BP_REGNUM);
      pt_struct.pt_fc  = read_register(FC_REGNUM);
      errno = 0;
      ptrace (PT_WRITE_STRUCT, inferior_pid,
	      (PTRACE_ARG3_TYPE) register_addr(CPS_REGNUM,0), 
	      (int)&pt_struct.pt_psr,(10)*4);
      if (errno != 0)
	{
	   sprintf (buf, "writing all special registers");
	   perror_with_name (buf);
	   return;
	}
#else
      store_inferior_registers(GR1_REGNUM);
      for (regno=GR96_REGNUM ; regno<GR96_REGNUM+32 ; regno++)
	store_inferior_registers(regno);
      for (regno=LR0_REGNUM ; regno<LR0_REGNUM+128 ; regno++)
	store_inferior_registers(regno);
      store_inferior_registers(CPS_REGNUM);
      store_inferior_registers(PC_REGNUM);
      store_inferior_registers(NPC_REGNUM);
      store_inferior_registers(PC2_REGNUM);
      store_inferior_registers(IPC_REGNUM);
      store_inferior_registers(IPA_REGNUM);
      store_inferior_registers(IPB_REGNUM);
      store_inferior_registers(Q_REGNUM);
      store_inferior_registers(BP_REGNUM);
      store_inferior_registers(FC_REGNUM);
#endif	/* ULTRA3 */
    }
}

/* 
 * Fetch an individual register (and supply it).
 * return 0 on success, -1 on failure.
 * NOTE: Assumes AMD's Binary Compatibility Standard for ptrace().
 */
static void
fetch_register (regno)
     int regno;
{
  char buf[128];
  int	val;

  if (CANNOT_FETCH_REGISTER(regno)) {
    val = -1;
    supply_register (regno, &val);
  } else {
    errno = 0;
    val = ptrace (PT_READ_U, inferior_pid,
		  (PTRACE_ARG3_TYPE) register_addr(regno,0), 0);
    if (errno != 0) {
      sprintf(buf,"reading register %s (#%d)",reg_names[regno],regno);
      perror_with_name (buf);
    } else {
      supply_register (regno, &val);
    }
  }
}


/* 
 * Read AMD's Binary Compatibilty Standard conforming core file.
 * struct ptrace_user is the first thing in the core file
 */

void
fetch_core_registers ()
{
  register int regno;
  int	val;
  char	buf[4];

  for (regno = 0 ; regno < NUM_REGS; regno++) {
    if (!CANNOT_FETCH_REGISTER(regno)) {
      val = bfd_seek (core_bfd, register_addr (regno, 0), 0);
      if (val < 0 || (val = bfd_read (buf, sizeof buf, 1, core_bfd)) < 0) {
        char * buffer = (char *) alloca (strlen (reg_names[regno]) + 35);
        strcpy (buffer, "Reading core register ");
        strcat (buffer, reg_names[regno]);
        perror_with_name (buffer);
      }
      supply_register (regno, buf);
    }
  }

  /* Fake any registers that are in REGISTER_NAMES, but not available to gdb */ 
  registers_fetched();
}


