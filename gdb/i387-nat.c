/* Native-dependent code for the i387.
   Copyright (C) 2000 Free Software Foundation, Inc.

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
#include "value.h"

/* FIXME: kettenis/2000-05-21: Right now more than a few i386 targets
   define their own routines to manage the floating-point registers in
   GDB's register array.  Most (if not all) of these targets use the
   format used by the "fsave" instruction in their communication with
   the OS.  They should all be converted to use the routines below.  */

/* At fsave_offset[REGNO] you'll find the offset to the location in
   the data structure used by the "fsave" instruction where GDB
   register REGNO is stored.  */

static int fsave_offset[] =
{
  28 + 0 * FPU_REG_RAW_SIZE,	/* FP0_REGNUM through ...  */
  28 + 1 * FPU_REG_RAW_SIZE,  
  28 + 2 * FPU_REG_RAW_SIZE,  
  28 + 3 * FPU_REG_RAW_SIZE,  
  28 + 4 * FPU_REG_RAW_SIZE,  
  28 + 5 * FPU_REG_RAW_SIZE,  
  28 + 6 * FPU_REG_RAW_SIZE,  
  28 + 7 * FPU_REG_RAW_SIZE,	/* ... FP7_REGNUM.  */
  0,				/* FCTRL_REGNUM (16 bits).  */
  4,				/* FSTAT_REGNUM (16 bits).  */
  8,				/* FTAG_REGNUM (16 bits).  */
  16,				/* FCS_REGNUM (16 bits).  */
  12,				/* FCOFF_REGNUM.  */
  24,				/* FDS_REGNUM.  */
  20,				/* FDOFF_REGNUM.  */
  18				/* FOP_REGNUM (bottom 11 bits).  */
};

#define FSAVE_ADDR(fsave, regnum) (fsave + fsave_offset[regnum - FP0_REGNUM])


/* Fill GDB's register array with the floating-point register values
   in *FSAVE.  This function masks off any of the reserved
   bits in *FSAVE.  */

void
i387_supply_fsave (char *fsave)
{
  int i;

  for (i = FP0_REGNUM; i <= LAST_FPU_CTRL_REGNUM; i++)
    {
      /* Most of the FPU control registers occupy only 16 bits in
	 the fsave area.  Give those a special treatment.  */
      if (i >= FIRST_FPU_CTRL_REGNUM
	  && i != FCOFF_REGNUM && i != FDOFF_REGNUM)
	{
	  unsigned val = *(unsigned short *) (FSAVE_ADDR (fsave, i));

	  if (i == FOP_REGNUM)
	    {
	      val &= ((1 << 11) - 1);
	      supply_register (i, (char *) &val);
	    }
	  else
	    supply_register (i, (char *) &val);
	}
      else
	supply_register (i, FSAVE_ADDR (fsave, i));
    }
}

/* Fill register REGNO (if it is a floating-point register) in *FSAVE
   with the value in GDB's register array.  If REGNO is -1, do this
   for all registers.  This function doesn't touch any of the reserved
   bits in *FSAVE.  */

void
i387_fill_fsave (char *fsave, int regno)
{
  int i;

  for (i = FP0_REGNUM; i <= LAST_FPU_CTRL_REGNUM; i++)
    if (regno == -1 || regno == i)
      {
	/* Most of the FPU control registers occupy only 16 bits in
           the fsave area.  Give those a special treatment.  */
	if (i >= FIRST_FPU_CTRL_REGNUM
	    && i != FCOFF_REGNUM && i != FDOFF_REGNUM)
	  {
	    if (i == FOP_REGNUM)
	      {
		unsigned short oldval, newval;

		/* The opcode occupies only 11 bits.  */
		oldval = (*(unsigned short *) (FSAVE_ADDR (fsave, i)));
		newval = *(unsigned short *) &registers[REGISTER_BYTE (i)];
		newval &= ((1 << 11) - 1);
		newval |= oldval & ~((1 << 11) - 1);
		memcpy (FSAVE_ADDR (fsave, i), &newval, 2);
	      }
	    else
	      memcpy (FSAVE_ADDR (fsave, i), &registers[REGISTER_BYTE (i)], 2);
	  }
	else
	  memcpy (FSAVE_ADDR (fsave, i), &registers[REGISTER_BYTE (i)],
		  REGISTER_RAW_SIZE (i));
      }
}
