/* Native-dependent code for the i387.
   Copyright 2000 Free Software Foundation, Inc.

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
	  unsigned int val = *(unsigned short *) (FSAVE_ADDR (fsave, i));

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


/* At fxsave_offset[REGNO] you'll find the offset to the location in
   the data structure used by the "fxsave" instruction where GDB
   register REGNO is stored.  */

static int fxsave_offset[] =
{
  32,				/* FP0_REGNUM through ...  */
  48,
  64,
  80,
  96,
  112,
  128,
  144,				/* ... FP7_REGNUM (80 bits each).  */
  0,				/* FCTRL_REGNUM (16 bits).  */
  2,				/* FSTAT_REGNUM (16 bits).  */
  4,				/* FTAG_REGNUM (16 bits).  */
  12,				/* FCS_REGNUM (16 bits).  */
  8,				/* FCOFF_REGNUM.  */
  20,				/* FDS_REGNUM (16 bits).  */
  16,				/* FDOFF_REGNUM.  */
  6,				/* FOP_REGNUM (bottom 11 bits).  */
  160,				/* XMM0_REGNUM through ...  */
  176,
  192,
  208,
  224,
  240,
  256,
  272,				/* ... XMM7_REGNUM (128 bits each).  */
  24,				/* MXCSR_REGNUM.  */
};

#define FXSAVE_ADDR(fxsave, regnum) \
  (fxsave + fxsave_offset[regnum - FP0_REGNUM])

static int i387_tag (unsigned char *raw);


/* Fill GDB's register array with the floating-point and SSE register
   values in *FXSAVE.  This function masks off any of the reserved
   bits in *FXSAVE.  */

void
i387_supply_fxsave (char *fxsave)
{
  int i;

  for (i = FP0_REGNUM; i <= MXCSR_REGNUM; i++)
    {
      /* Most of the FPU control registers occupy only 16 bits in
	 the fxsave area.  Give those a special treatment.  */
      if (i >= FIRST_FPU_CTRL_REGNUM && i < XMM0_REGNUM
	  && i != FCOFF_REGNUM && i != FDOFF_REGNUM)
	{
	  unsigned long val = *(unsigned short *) (FXSAVE_ADDR (fxsave, i));

	  if (i == FOP_REGNUM)
	    {
	      val &= ((1 << 11) - 1);
	      supply_register (i, (char *) &val);
	    }
	  else if (i== FTAG_REGNUM)
	    {
	      /* The fxsave area contains a simplified version of the
                 tag word.  We have to look at the actual 80-bit FP
                 data to recreate the traditional i387 tag word.  */

	      unsigned long ftag = 0;
	      unsigned long fstat;
	      int fpreg;
	      int top;

	      fstat = *(unsigned short *) (FXSAVE_ADDR (fxsave, FSTAT_REGNUM));
	      top = ((fstat >> 11) & 0x111);

	      for (fpreg = 7; fpreg >= 0; fpreg--)
		{
		  int tag = 0x11;

		  if (val & (1 << fpreg))
		    {
		      int regnum = (fpreg + 8 - top) % 8 + FP0_REGNUM;
		      tag = i387_tag (FXSAVE_ADDR (fxsave, regnum));
		    }

		  ftag |= tag << (2 * fpreg);
		}
	      supply_register (i, (char *) &ftag);
	    }
	  else
	    supply_register (i, (char *) &val);
	}
      else
	supply_register (i, FXSAVE_ADDR (fxsave, i));
    }
}

/* Fill register REGNO (if it is a floating-point or SSE register) in
   *FXSAVE with the value in GDB's register array.  If REGNO is -1, do
   this for all registers.  This function doesn't touch any of the
   reserved bits in *FXSAVE.  */

void
i387_fill_fxsave (char *fxsave, int regno)
{
  int i;

  for (i = FP0_REGNUM; i <= MXCSR_REGNUM; i++)
    if (regno == -1 || regno == i)
      {
	/* Most of the FPU control registers occupy only 16 bits in
           the fxsave area.  Give those a special treatment.  */
	if (i >= FIRST_FPU_CTRL_REGNUM && i < XMM0_REGNUM
	    && i != FCOFF_REGNUM && i != FDOFF_REGNUM)
	  {
	    if (i == FOP_REGNUM)
	      {
		unsigned short oldval, newval;

		/* The opcode occupies only 11 bits.  */
		oldval = (*(unsigned short *) (FXSAVE_ADDR (fxsave, i)));
		newval = *(unsigned short *) &registers[REGISTER_BYTE (i)];
		newval &= ((1 << 11) - 1);
		newval |= oldval & ~((1 << 11) - 1);
		memcpy (FXSAVE_ADDR (fxsave, i), &newval, 2);
	      }
	    else if (i == FTAG_REGNUM)
	      {
		/* Converting back is much easier.  */

		unsigned char val = 0;
		unsigned short ftag;
		int fpreg;

		ftag = *(unsigned short *) &registers[REGISTER_BYTE (i)];

		for (fpreg = 7; fpreg >= 0; fpreg--)
		  {
		    int tag = (ftag >> (fpreg * 2)) & 0x11;

		    if (tag != 0x11)
		      val |= (1 << fpreg);
		  }

		memcpy (FXSAVE_ADDR (fxsave, i), &val, 2);
	      }
	    else
	      memcpy (FXSAVE_ADDR (fxsave, i),
		      &registers[REGISTER_BYTE (i)], 2);
	  }
	else
	  memcpy (FXSAVE_ADDR (fxsave, i), &registers[REGISTER_BYTE (i)],
		  REGISTER_RAW_SIZE (i));
      }
}

/* Recreate the FTW (tag word) valid bits from the 80-bit FP data in
   *RAW.  */

static int
i387_tag (unsigned char *raw)
{
  int integer;
  unsigned int exponent;
  unsigned long fraction[2];

  integer = raw[7] & 0x80;
  exponent = (((raw[9] & 0x7f) << 8) | raw[8]);
  fraction[0] = ((raw[3] << 24) | (raw[2] << 16) | (raw[1] << 8) | raw[0]);
  fraction[1] = (((raw[7] & 0x7f) << 24) | (raw[6] << 16)
		 | (raw[5] << 8) | raw[4]);

  if (exponent == 0x7fff)
    {
      /* Special.  */
      return (0x10);
    }
  else if (exponent == 0x0000)
    {
      if (integer)
	{
	  /* Valid.  */
	  return (0x00);
	}
      else
	{
	  /* Special.  */
	  return (0x10);
	}
    }
  else
    {
      if (fraction[0] == 0x0000 && fraction[1] == 0x0000 && !integer)
	{
	  /* Zero.  */
	  return (0x01);
	}
      else
	{
	  /* Special.  */
	  return (0x10);
	}
    }
}
