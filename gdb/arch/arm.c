/* Common target dependent code for GDB on ARM systems.

   Copyright (C) 1988-2015 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "common-defs.h"
#include "arm.h"

/* See arm.h.  */

int
thumb_insn_size (unsigned short inst1)
{
  if ((inst1 & 0xe000) == 0xe000 && (inst1 & 0x1800) != 0)
    return 4;
  else
    return 2;
}

/* See arm.h.  */

int
bitcount (unsigned long val)
{
  int nbits;
  for (nbits = 0; val != 0; nbits++)
    val &= val - 1;		/* Delete rightmost 1-bit in val.  */
  return nbits;
}

/* See arm.h.  */

int
condition_true (unsigned long cond, unsigned long status_reg)
{
  if (cond == INST_AL || cond == INST_NV)
    return 1;

  switch (cond)
    {
    case INST_EQ:
      return ((status_reg & FLAG_Z) != 0);
    case INST_NE:
      return ((status_reg & FLAG_Z) == 0);
    case INST_CS:
      return ((status_reg & FLAG_C) != 0);
    case INST_CC:
      return ((status_reg & FLAG_C) == 0);
    case INST_MI:
      return ((status_reg & FLAG_N) != 0);
    case INST_PL:
      return ((status_reg & FLAG_N) == 0);
    case INST_VS:
      return ((status_reg & FLAG_V) != 0);
    case INST_VC:
      return ((status_reg & FLAG_V) == 0);
    case INST_HI:
      return ((status_reg & (FLAG_C | FLAG_Z)) == FLAG_C);
    case INST_LS:
      return ((status_reg & (FLAG_C | FLAG_Z)) != FLAG_C);
    case INST_GE:
      return (((status_reg & FLAG_N) == 0) == ((status_reg & FLAG_V) == 0));
    case INST_LT:
      return (((status_reg & FLAG_N) == 0) != ((status_reg & FLAG_V) == 0));
    case INST_GT:
      return (((status_reg & FLAG_Z) == 0)
	      && (((status_reg & FLAG_N) == 0)
		  == ((status_reg & FLAG_V) == 0)));
    case INST_LE:
      return (((status_reg & FLAG_Z) != 0)
	      || (((status_reg & FLAG_N) == 0)
		  != ((status_reg & FLAG_V) == 0)));
    }
  return 1;
}
