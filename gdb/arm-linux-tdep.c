/* GNU/Linux on ARM target support.
   Copyright 1999, 2000 Free Software Foundation, Inc.

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
#include "target.h"
#include "value.h"
#include "gdbtypes.h"

#ifdef GET_LONGJMP_TARGET

/* Figure out where the longjmp will land.  We expect that we have
   just entered longjmp and haven't yet altered r0, r1, so the
   arguments are still in the registers.  (A1_REGNUM) points at the
   jmp_buf structure from which we extract the pc (JB_PC) that we will
   land at.  The pc is copied into ADDR.  This routine returns true on
   success. */

#define LONGJMP_TARGET_SIZE 	sizeof(int)
#define JB_ELEMENT_SIZE		sizeof(int)
#define JB_SL			18
#define JB_FP			19
#define JB_SP			20
#define JB_PC			21

int
arm_get_longjmp_target (CORE_ADDR * pc)
{
  CORE_ADDR jb_addr;
  char buf[LONGJMP_TARGET_SIZE];

  jb_addr = read_register (A1_REGNUM);

  if (target_read_memory (jb_addr + JB_PC * JB_ELEMENT_SIZE, buf,
			  LONGJMP_TARGET_SIZE))
    return 0;

  *pc = extract_address (buf, LONGJMP_TARGET_SIZE);
  return 1;
}

#endif /* GET_LONGJMP_TARGET */

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

void
arm_linux_extract_return_value (struct type *type,
				char regbuf[REGISTER_BYTES],
				char *valbuf)
{
  /* ScottB: This needs to be looked at to handle the different
     floating point emulators on ARM Linux.  Right now the code
     assumes that fetch inferior registers does the right thing for
     GDB.  I suspect this won't handle NWFPE registers correctly, nor
     will the default ARM version (arm_extract_return_value()).  */

  int regnum = (TYPE_CODE_FLT == TYPE_CODE (type)) ? F0_REGNUM : A1_REGNUM;
  memcpy (valbuf, &regbuf[REGISTER_BYTE (regnum)], TYPE_LENGTH (type));
}

void
_initialize_arm_linux_tdep (void)
{
}
