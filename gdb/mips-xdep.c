/* Low level MIPS interface to ptrace, for GDB when running under Unix.
   Copyright (C) 1988, 1989, 1991 Free Software Foundation, Inc.
   Contributed by Alessandro Forin(af@cs.cmu.edu) at CMU
   and by Per Bothner(bothner@cs.wisc.edu) at U.Wisconsin.

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

#include <stdio.h>
#include <mips/inst.h>
#include "defs.h"
#include "param.h"
#include "frame.h"
#include "inferior.h"
#include "symtab.h"
#include "value.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <sys/ioctl.h>
/* #include <fcntl.h>  Can we live without this?  */

#include "gdbcore.h"

#include <sys/user.h>		/* After a.out.h  */
#include <sys/file.h>
#include <sys/stat.h>

/* Get all registers from the inferior */

void
fetch_inferior_registers ()
{
  register int regno;
  register unsigned int regaddr;
  char buf[MAX_REGISTER_RAW_SIZE];
  register int i;

  registers_fetched ();

  for (regno = 1; regno < NUM_REGS; regno++)
    {
      regaddr = register_addr (regno, 1);
      for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
 	{
 	  *(int *) &buf[i] = ptrace (3, inferior_pid, regaddr, 0);
 	  regaddr += sizeof (int);
 	}
      supply_register (regno, buf);
    }
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

store_inferior_registers (regno)
     int regno;
{
  register unsigned int regaddr;
  char buf[80];

  if (regno == 0)
    return;

  if (regno > 0)
    {
      regaddr = register_addr (regno, 1);
      errno = 0;
      ptrace (6, inferior_pid, regaddr, read_register (regno));
      if (errno != 0)
	{
	  sprintf (buf, "writing register number %d", regno);
	  perror_with_name (buf);
	}
    }
  else
    {
      for (regno = 1; regno < NUM_REGS; regno++)
	{
	  if (regno == 32 || regno == 35 || regno == 36 || regno == 71)
	    continue;
	  regaddr = register_addr (regno, 1);
	  errno = 0;
	  ptrace (6, inferior_pid, regaddr, read_register (regno));
	  if (errno != 0)
	    {
	      sprintf (buf, "writing all regs, number %d", regno);
	      perror_with_name (buf);
	    }
	}
    }
}

#if 0
void
fetch_core_registers ()
{
  register int regno;
  int val;

  for (regno = 1; regno < NUM_REGS; regno++) {
    char buf[MAX_REGISTER_RAW_SIZE];

    val = bfd_seek (core_bfd, register_addr (regno, 0));
    if (val < 0 || (val = bfd_read (core_bfd, buf, sizeof buf)) < 0) {
      char buffer[50];
      strcpy (buffer, "Reading register ");
      strcat (buffer, reg_names[regno]);

      perror_with_name (buffer);
    }
    supply_register (regno, buf);
  }
}
#endif /* 0 */
