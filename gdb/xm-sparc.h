/* Parameters for execution on a Sun 4, for GDB, the GNU debugger.
   Copyright (C) 1986, 1987, 1989 Free Software Foundation, Inc.
   Contributed by Michael Tiemann (tiemann@mcc.com)
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

#define HOST_BYTE_ORDER BIG_ENDIAN

/* Get rid of any system-imposed stack limit if possible.  */

#define SET_STACK_LIMIT_HUGE

/* Enable use of alternate code for Sun's format of core dump file.  */

#define NEW_SUN_CORE

/* Do implement the attach and detach commands.  */

#define ATTACH_DETACH

/* Override copies of {fetch,store}_inferior_registers in infptrace.c.  */

#define FETCH_INFERIOR_REGISTERS

/* Before storing, we need to read all the registers.  */

#define CHILD_PREPARE_TO_STORE() read_register_bytes (0, NULL, REGISTER_BYTES)

/* It does have a wait structure, and it might help things out . . . */

#define HAVE_WAIT_STRUCT

/* Optimization for storing registers to the inferior.  The hook
   DO_DEFERRED_STORES
   actually executes any deferred stores.  It is called any time
   we are going to proceed the child, or read its registers.
   The hook CLEAR_DEFERRED_STORES is called when we want to throw
   away the inferior process, e.g. when it dies or we kill it.
   FIXME, this does not handle remote debugging cleanly.  */

extern int deferred_stores;
extern int store_inferior_registers ();
#define	DO_DEFERRED_STORES	\
  if (deferred_stores)		\
    store_inferior_registers (-2);
#define	CLEAR_DEFERRED_STORES	\
  deferred_stores = 0;
