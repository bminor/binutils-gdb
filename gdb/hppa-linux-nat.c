/* Functions specific to running gdb native on HPPA running Linux.
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
#include "gdbcore.h"
#include "regcache.h"
#include "gdb_string.h"

#include <sys/procfs.h>
#include <string.h>
#include <asm/offsets.h>

/* Prototypes for supply_gregset etc. */
#include "gregset.h"

/* These must match the order of the register names.

   Some sort of lookup table is needed because the offsets associated
   with the registers are all over the board.  */

static const int u_offsets[] =
  {
    /* general registers */
    -1,
    PT_GR1,
    PT_GR2,
    PT_GR3,
    PT_GR4,
    PT_GR5,
    PT_GR6,
    PT_GR7,
    PT_GR8,
    PT_GR9,
    PT_GR10,
    PT_GR11,
    PT_GR12,
    PT_GR13,
    PT_GR14,
    PT_GR15,
    PT_GR16,
    PT_GR17,
    PT_GR18,
    PT_GR19,
    PT_GR20,
    PT_GR21,
    PT_GR22,
    PT_GR23,
    PT_GR24,
    PT_GR25,
    PT_GR26,
    PT_GR27,
    PT_GR28,
    PT_GR29,
    PT_GR30,
    PT_GR31,

    PT_SAR,
    PT_IAOQ0,
    PT_IASQ0,
    PT_IAOQ1,
    PT_IASQ1,
    -1, /* eiem */
    PT_IIR,
    PT_ISR,
    PT_IOR,
    PT_PSW,
    -1, /* goto */

    PT_SR4,
    PT_SR0,
    PT_SR1,
    PT_SR2,
    PT_SR3,
    PT_SR5,
    PT_SR6,
    PT_SR7,

    -1, /* cr0 */
    -1, /* pid0 */
    -1, /* pid1 */
    -1, /* ccr */
    -1, /* pid2 */
    -1, /* pid3 */
    -1, /* cr24 */
    -1, /* cr25 */
    -1, /* cr26 */
    PT_CR27,
    -1, /* cr28 */
    -1, /* cr29 */
    -1, /* cr30 */

    /* Floating point regs.  */
    PT_FR0,  PT_FR0 + 4,
    PT_FR1,  PT_FR1 + 4,
    PT_FR2,  PT_FR2 + 4,
    PT_FR3,  PT_FR3 + 4,
    PT_FR4,  PT_FR4 + 4,
    PT_FR5,  PT_FR5 + 4,
    PT_FR6,  PT_FR6 + 4,
    PT_FR7,  PT_FR7 + 4,
    PT_FR8,  PT_FR8 + 4,
    PT_FR9,  PT_FR9 + 4,
    PT_FR10, PT_FR10 + 4,
    PT_FR11, PT_FR11 + 4,
    PT_FR12, PT_FR12 + 4,
    PT_FR13, PT_FR13 + 4,
    PT_FR14, PT_FR14 + 4,
    PT_FR15, PT_FR15 + 4,
    PT_FR16, PT_FR16 + 4,
    PT_FR17, PT_FR17 + 4,
    PT_FR18, PT_FR18 + 4,
    PT_FR19, PT_FR19 + 4,
    PT_FR20, PT_FR20 + 4,
    PT_FR21, PT_FR21 + 4,
    PT_FR22, PT_FR22 + 4,
    PT_FR23, PT_FR23 + 4,
    PT_FR24, PT_FR24 + 4,
    PT_FR25, PT_FR25 + 4,
    PT_FR26, PT_FR26 + 4,
    PT_FR27, PT_FR27 + 4,
    PT_FR28, PT_FR28 + 4,
    PT_FR29, PT_FR29 + 4,
    PT_FR30, PT_FR30 + 4,
    PT_FR31, PT_FR31 + 4,
  };

CORE_ADDR
register_addr (int regno, CORE_ADDR blockend)
{
  CORE_ADDR addr;

  if ((unsigned) regno >= NUM_REGS)
    error ("Invalid register number %d.", regno);

  if (u_offsets[regno] == -1)
    addr = 0;
  else
    {
      addr = (CORE_ADDR) u_offsets[regno];
    }

  return addr;
}

/*
 * Registers saved in a coredump:
 * gr0..gr31
 * sr0..sr7
 * iaoq0..iaoq1
 * iasq0..iasq1
 * sar, iir, isr, ior, ipsw
 * cr0, cr24..cr31
 * cr8,9,12,13
 * cr10, cr15
 */
#define GR_REGNUM(_n)	(R0_REGNUM+_n)
#define TR_REGNUM(_n)	(TR0_REGNUM+_n)
static const int greg_map[] =
  {
    GR_REGNUM(0), GR_REGNUM(1), GR_REGNUM(2), GR_REGNUM(3),
    GR_REGNUM(4), GR_REGNUM(5), GR_REGNUM(6), GR_REGNUM(7),
    GR_REGNUM(8), GR_REGNUM(9), GR_REGNUM(10), GR_REGNUM(11),
    GR_REGNUM(12), GR_REGNUM(13), GR_REGNUM(14), GR_REGNUM(15),
    GR_REGNUM(16), GR_REGNUM(17), GR_REGNUM(18), GR_REGNUM(19),
    GR_REGNUM(20), GR_REGNUM(21), GR_REGNUM(22), GR_REGNUM(23),
    GR_REGNUM(24), GR_REGNUM(25), GR_REGNUM(26), GR_REGNUM(27),
    GR_REGNUM(28), GR_REGNUM(29), GR_REGNUM(30), GR_REGNUM(31),

    SR4_REGNUM+1, SR4_REGNUM+2, SR4_REGNUM+3, SR4_REGNUM+4,
    SR4_REGNUM, SR4_REGNUM+5, SR4_REGNUM+6, SR4_REGNUM+7,

    PCOQ_HEAD_REGNUM, PCOQ_TAIL_REGNUM,
    PCSQ_HEAD_REGNUM, PCSQ_TAIL_REGNUM,

    SAR_REGNUM, IIR_REGNUM, ISR_REGNUM, IOR_REGNUM,
    IPSW_REGNUM, RCR_REGNUM,

    TR_REGNUM(0), TR_REGNUM(1), TR_REGNUM(2), TR_REGNUM(3),
    TR_REGNUM(4), TR_REGNUM(5), TR_REGNUM(6), TR_REGNUM(7),

    PID0_REGNUM, PID1_REGNUM, PID2_REGNUM, PID3_REGNUM,
    CCR_REGNUM, EIEM_REGNUM,
  };

void
supply_gregset (gdb_gregset_t *gregsetp)
{
  int i;
  greg_t *regp = (elf_greg_t *) gregsetp;

  for (i = 0; i < sizeof (greg_map) / sizeof (greg_map[0]); i++, regp++)
    {
      int regno = greg_map[i];
      int size = register_size (current_gdbarch, regno);
      /* When running a 64 bit kernel, a greg_t may be larger than the
	 actual register, so just pick off the LS bits of big-endian word.  */
      supply_register (regno, ((char *) (regp + 1)) - size);
    }
}

void
fill_gregset (gdb_gregset_t *gregsetp, int regno)
{
  int i;
  greg_t *regp = (greg_t *) gregsetp;

  memset (gregsetp, 0, sizeof (*gregsetp));
  for (i = 0; i < sizeof (greg_map) / sizeof (greg_map[0]); i++, regp++)
    {
      int regi = greg_map[i];

      if (regno == -1 || regi == regno)
	{
	  int rawsize = register_size (current_gdbarch, regi);
	  regcache_collect (regi, ((char *) (regp + 1)) - rawsize);
	}
    }
}

/*  Given a pointer to a floating point register set in /proc format
   (fpregset_t *), unpack the register contents and supply them as gdb's
   idea of the current floating point register values. */

void
supply_fpregset (gdb_fpregset_t *fpregsetp)
{
  register int regi;
  char *from;

  for (regi = 0; regi <= 31; regi++)
    {
      from = (char *) &((*fpregsetp)[regi]);
      supply_register (2*regi + HPPA_FP0_REGNUM, from);
      supply_register (2*regi + HPPA_FP0_REGNUM + 1, from + 4);
    }
}

/*  Given a pointer to a floating point register set in /proc format
   (fpregset_t *), update the register specified by REGNO from gdb's idea
   of the current floating point register set.  If REGNO is -1, update
   them all. */

void
fill_fpregset (gdb_fpregset_t *fpregsetp, int regno)
{
  int i;

  for (i = 0; i < NUM_REGS; i++)
   {
    if (regno == -1 || regno == i)
      {
        /* Gross.  fpregset_t is double, registers[x] has single
	   precision reg.  */
        char *to = (char *) &((*fpregsetp)[(i - HPPA_FP0_REGNUM) / 2]);
        if ((i - HPPA_FP0_REGNUM) & 1)
	  to += 4;
        regcache_collect (i, to);
      }
   }
}
