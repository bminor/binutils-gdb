/* Parameters for native support on HP 9000 model 320, for GDB, the GNU debugger.
   Copyright (C) 1986, 1987, 1989, 1992 Free Software Foundation, Inc.

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

/* Do implement the attach and detach commands.  */

#define ATTACH_DETACH

/* fetch_inferior_registers is in nat-hp300hpux.c.  */
#define FETCH_INFERIOR_REGISTERS

/* Get registers from a core file.  The floating point stuff is just
   guesses.  */
#define NEED_SYS_CORE_H
#define REGISTER_U_ADDR(addr, blockend, regno)				\
{									\
  if (regno < PS_REGNUM)						\
    addr = (int) (&((struct proc_regs *)(blockend))->d0 + regno);	\
  else if (regno == PS_REGNUM)						\
    addr = (int) ((char *) (&((struct proc_regs *)(blockend))->ps) - 2); \
  else if (regno == PC_REGNUM)						\
    addr = (int) &((struct proc_regs *)(blockend))->pc;			\
  else if (regno < FPC_REGNUM)						\
    addr = (int) (((struct proc_regs *)(blockend))->mc68881		\
		  + ((regno) - FP0_REGNUM) / 2);			\
  else									\
    addr = (int) (((struct proc_regs *)(blockend))->p_float		\
		  + (regno) - FPC_REGNUM);				\
}
