/* Native-dependent code for BSD Unix running on i386's, for GDB.
   Copyright 1988, 1989, 1991, 1992, 1994, 1995, 1996, 1998, 1999, 2000,
   2001 Free Software Foundation, Inc.

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

#ifdef FETCH_INFERIOR_REGISTERS
#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>
#include <machine/frame.h>
#include "inferior.h"
#include "gdbcore.h" /* for registers_fetched() */
#include "regcache.h"

void
fetch_inferior_registers (int regno)
{
  struct reg inferior_registers;

  ptrace (PT_GETREGS, PIDGET (inferior_ptid),
          (PTRACE_ARG3_TYPE) & inferior_registers, 0);
  memcpy (&registers[REGISTER_BYTE (0)], &inferior_registers, 4 * NUM_REGS);
  registers_fetched ();
}

void
store_inferior_registers (int regno)
{
  struct reg inferior_registers;

  memcpy (&inferior_registers, &registers[REGISTER_BYTE (0)], 4 * NUM_REGS);
  ptrace (PT_SETREGS, PIDGET (inferior_ptid),
          (PTRACE_ARG3_TYPE) & inferior_registers, 0);
}

struct md_core
{
  struct reg intreg;
  struct fpreg freg;
};

void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size, int which,
		      CORE_ADDR ignore)
{
  struct md_core *core_reg = (struct md_core *) core_reg_sect;

  /* integer registers */
  memcpy (&registers[REGISTER_BYTE (0)], &core_reg->intreg,
	  sizeof (struct reg));
  /* floating point registers */
  /* XXX */
}

#else

#include <machine/reg.h>

/* this table must line up with REGISTER_NAMES in tm-i386.h */
/* symbols like 'tEAX' come from <machine/reg.h> */
static int tregmap[] =
{
  tEAX, tECX, tEDX, tEBX,
  tESP, tEBP, tESI, tEDI,
  tEIP, tEFLAGS, tCS, tSS
};

#ifdef sEAX
static int sregmap[] =
{
  sEAX, sECX, sEDX, sEBX,
  sESP, sEBP, sESI, sEDI,
  sEIP, sEFLAGS, sCS, sSS
};
#else /* No sEAX */

/* FreeBSD has decided to collapse the s* and t* symbols.  So if the s*
   ones aren't around, use the t* ones for sregmap too.  */

static int sregmap[] =
{
  tEAX, tECX, tEDX, tEBX,
  tESP, tEBP, tESI, tEDI,
  tEIP, tEFLAGS, tCS, tSS
};
#endif /* No sEAX */

/* blockend is the value of u.u_ar0, and points to the
   place where ES is stored.  */

int
i386_register_u_addr (int blockend, int regnum)
{
  /* The following condition is a kludge to get at the proper register map
     depending upon the state of pcb_flag.
     The proper condition would be
     if (u.u_pcb.pcb_flag & FM_TRAP)
     but that would require a ptrace call here and wouldn't work
     for corefiles.  */

  if (blockend < 0x1fcc)
    return (blockend + 4 * tregmap[regnum]);
  else
    return (blockend + 4 * sregmap[regnum]);
}

#endif /* !FETCH_INFERIOR_REGISTERS */
