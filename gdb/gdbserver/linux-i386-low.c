/* GNU/Linux/i386 specific low level interface, for the remote server for GDB.
   Copyright 1995, 1996, 1998, 1999, 2000, 2001, 2002
   Free Software Foundation, Inc.

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

#include "server.h"

#ifdef HAVE_SYS_REG_H
#include <sys/reg.h>
#endif

/* This module only supports access to the general purpose registers.
   Adjust the relevant constants accordingly.

   FIXME: kettenis/2001-03-28: We should really use PTRACE_GETREGS to
   get at the registers.  Better yet, we should try to share code with
   i386-linux-nat.c.  */

int num_regs = 16;

/* This stuff comes from i386-linux-nat.c.  */

/* Mapping between the general-purpose registers in `struct user'
   format and GDB's register array layout.  */
int regmap[] = 
{
  EAX * 4, ECX * 4, EDX * 4, EBX * 4,
  UESP * 4, EBP * 4, ESI * 4, EDI * 4,
  EIP * 4, EFL * 4, CS * 4, SS * 4,
  DS * 4, ES * 4, FS * 4, GS * 4
};

int
cannot_store_register (int regno)
{
  return (regno >= num_regs);
}

int
cannot_fetch_register (int regno)
{
  return (regno >= num_regs);
}
