/* Definitions to make GDB run on a mips box under 4.3bsd.
   Copyright (C) 1986, 1987, 1989 Free Software Foundation, Inc.
   Contributed by Per Bothner(bothner@cs.wisc.edu) at U.Wisconsin
   and by Alessandro Forin(af@cs.cmu.edu) at CMU

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

#if !defined (HOST_BYTE_ORDER)
#define HOST_BYTE_ORDER LITTLE_ENDIAN
#endif

/* wait.h */
#define HAVE_WAIT_STRUCT

/* Get rid of any system-imposed stack limit if possible */

#define	SET_STACK_LIMIT_HUGE

/* This WOULD BE the amount to subtract from u.u_ar0
   to get the offset in the core file of the register values.
   But Mips' ptrace works on regnums, not displacements */

#define KERNEL_U_ADDR (int)u.u_ar0

/* Override copies of {fetch,store}_inferior_registers in infptrace.c.  */
#define FETCH_INFERIOR_REGISTERS

/* Interface definitions for kernel debugger KDB */

/* I am not going to pretend I've done anything about this */
