/* Extract registers from a "standard" core file, for GDB.
   Copyright (C) 1988-1991  Free Software Foundation, Inc.

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

/* core.c is supposed to be the more machine-independent aspects of this;
   this file is more machine-specific.  */

#include "defs.h"
#include "param.h"
#include "gdbcore.h"

/* Some of these are needed on various systems, perhaps, to expand
   REGISTER_U_ADDR appropriately?  */
/* #include <sys/core.h> */
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/user.h>


/* Extract the register values out of the core file and store
   them where `read_register' will find them.  */

void
fetch_core_registers (core_reg_sect, core_reg_size)
     char *core_reg_sect;
     unsigned core_reg_size;
{
  register int regno;
  register unsigned int addr;
  int bad_reg = -1;

  for (regno = 0; regno < NUM_REGS; regno++)
    {
      addr = register_addr (regno, core_reg_size);
      if (addr >= core_reg_size) {
	if (bad_reg < 0)
	  bad_reg = regno;
      } else {
	supply_register (regno, core_reg_sect + addr);
      }
    }
  if (bad_reg > 0)
    {
      error ("Register %s not found in core file.", reg_names[bad_reg]);
    }
}


#ifdef REGISTER_U_ADDR

/* Return the address in the core dump or inferior of register REGNO.
   BLOCKEND is the address of the end of the user structure.  */

unsigned int
register_addr (regno, blockend)
     int regno;
     int blockend;
{
  int addr;

  if (regno < 0 || regno >= NUM_REGS)
    error ("Invalid register number %d.", regno);

  REGISTER_U_ADDR (addr, blockend, regno);

  return addr;
}

#endif /* REGISTER_U_ADDR */
