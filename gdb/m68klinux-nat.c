/* Motorola m68k native support for Linux
   Copyright 1996, 1998, 2000, 2001 Free Software Foundation, Inc.

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
#include "inferior.h"
#include "language.h"
#include "gdbcore.h"
#include "regcache.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/procfs.h>

#include <sys/file.h>
#include "gdb_stat.h"

#include "floatformat.h"

#include "target.h"


/* This table must line up with REGISTER_NAMES in tm-m68k.h */
static const int regmap[] =
{
  PT_D0, PT_D1, PT_D2, PT_D3, PT_D4, PT_D5, PT_D6, PT_D7,
  PT_A0, PT_A1, PT_A2, PT_A3, PT_A4, PT_A5, PT_A6, PT_USP,
  PT_SR, PT_PC,
  /* PT_FP0, ..., PT_FP7 */
  21, 24, 27, 30, 33, 36, 39, 42,
  /* PT_FPCR, PT_FPSR, PT_FPIAR */
  45, 46, 47
};

/* BLOCKEND is the value of u.u_ar0, and points to the place where GS
   is stored.  */

int
m68k_linux_register_u_addr (int blockend, int regnum)
{
  return (blockend + 4 * regmap[regnum]);
}

/*  Given a pointer to a general register set in /proc format
   (elf_gregset_t *), unpack the register contents and supply
   them as gdb's idea of the current register values. */


/* Note both m68k-tdep.c and m68klinux-nat.c contain definitions
   for supply_gregset and supply_fpregset. The definitions
   in m68k-tdep.c are valid if USE_PROC_FS is defined. Otherwise,
   the definitions in m68klinux-nat.c will be used. This is a 
   bit of a hack. The supply_* routines do not belong in 
   *_tdep.c files. But, there are several lynx ports that currently 
   depend on these definitions. */

#ifndef USE_PROC_FS

/* Prototypes for supply_gregset etc. */
#include "gregset.h"

void
supply_gregset (elf_gregset_t *gregsetp)
{
  int regi;

  for (regi = D0_REGNUM; regi <= SP_REGNUM; regi++)
    supply_register (regi, (char *) (*gregsetp + regmap[regi]));
  supply_register (PS_REGNUM, (char *) (*gregsetp + PT_SR));
  supply_register (PC_REGNUM, (char *) (*gregsetp + PT_PC));
}

/*  Given a pointer to a floating point register set in /proc format
   (fpregset_t *), unpack the register contents and supply them as gdb's
   idea of the current floating point register values. */

void
supply_fpregset (elf_fpregset_t *fpregsetp)
{
  int regi;

  for (regi = FP0_REGNUM; regi < FPC_REGNUM; regi++)
    supply_register (regi, (char *) &fpregsetp->fpregs[(regi - FP0_REGNUM) * 3]);
  supply_register (FPC_REGNUM, (char *) &fpregsetp->fpcntl[0]);
  supply_register (FPS_REGNUM, (char *) &fpregsetp->fpcntl[1]);
  supply_register (FPI_REGNUM, (char *) &fpregsetp->fpcntl[2]);
}

#endif


/* Interpreting register set info found in core files.  */

/* Provide registers to GDB from a core file.

   (We can't use the generic version of this function in
   core-regset.c, because we need to use elf_gregset_t instead of
   gregset_t.)

   CORE_REG_SECT points to an array of bytes, which are the contents
   of a `note' from a core file which BFD thinks might contain
   register contents.  CORE_REG_SIZE is its size.

   WHICH says which register set corelow suspects this is:
     0 --- the general-purpose register set, in elf_gregset_t format
     2 --- the floating-point register set, in elf_fpregset_t format

   REG_ADDR isn't used on Linux.  */

static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size,
		      int which, CORE_ADDR reg_addr)
{
  elf_gregset_t gregset;
  elf_fpregset_t fpregset;

  switch (which)
    {
    case 0:
      if (core_reg_size != sizeof (gregset))
	warning ("Wrong size gregset in core file.");
      else
	{
	  memcpy (&gregset, core_reg_sect, sizeof (gregset));
	  supply_gregset (&gregset);
	}
      break;

    case 2:
      if (core_reg_size != sizeof (fpregset))
	warning ("Wrong size fpregset in core file.");
      else
	{
	  memcpy (&fpregset, core_reg_sect, sizeof (fpregset));
	  supply_fpregset (&fpregset);
	}
      break;

    default:
      /* We've covered all the kinds of registers we know about here,
         so this must be something we wouldn't know what to do with
         anyway.  Just ignore it.  */
      break;
    }
}


int
kernel_u_size (void)
{
  return (sizeof (struct user));
}

/* Return non-zero if PC points into the signal trampoline.  */

int
in_sigtramp (CORE_ADDR pc)
{
  CORE_ADDR sp;
  char buf[TARGET_SHORT_BIT / TARGET_CHAR_BIT];
  int insn;

  sp = read_register (SP_REGNUM);
  if (pc - 2 < sp)
    return 0;

  if (read_memory_nobpt (pc, buf, sizeof (buf)))
    return 0;
  insn = extract_unsigned_integer (buf, sizeof (buf));
  if (insn == 0xdefc		/* addaw #,sp */
      || insn == 0x7077		/* moveq #119,d0 */
      || insn == 0x4e40		/* trap #0 */
      || insn == 0x203c /* movel #,d0 */ )
    return 1;

  if (read_memory_nobpt (pc - 2, buf, sizeof (buf)))
    return 0;
  insn = extract_unsigned_integer (buf, sizeof (buf));
  if (insn == 0xdefc		/* addaw #,sp */
      || insn == 0x7077		/* moveq #119,d0 */
      || insn == 0x4e40		/* trap #0 */
      || insn == 0x203c /* movel #,d0 */ )
    return 1;

  return 0;
}


/* Register that we are able to handle Linux ELF core file formats.  */

static struct core_fns linux_elf_core_fns =
{
  bfd_target_elf_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL					/* next */
};

void
_initialize_m68k_linux_nat ()
{
  add_core_fns (&linux_elf_core_fns);
}
