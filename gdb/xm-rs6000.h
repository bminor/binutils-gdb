/* Parameters for hosting on an RS6000, for GDB, the GNU debugger.
   Copyright (C) 1986, 1987, 1989, 1991 Free Software Foundation, Inc.
   Contributed by IBM Corporation.

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

/* Big end is at the low address */

#define	HOST_BYTE_ORDER	BIG_ENDIAN

#define	HAVE_TERMIO 1
#define	USG 1
#define	HAVE_SIGSETMASK	1

/* This system requires that we open a terminal with O_NOCTTY for it to
   not become our controlling terminal.  */

#define	USE_O_NOCTTY

/* Get rid of any system-imposed stack limit if possible.  */

#define SET_STACK_LIMIT_HUGE

/* Brain death inherited from PC's pervades.  */
#undef NULL
#define NULL 0

/* The IBM compiler requires this in order to properly compile alloca().  */
#pragma alloca

#define	vfork	fork

/* Do implement the attach and detach commands.  */

#define ATTACH_DETACH

/* Override copies of {fetch,store}_inferior_registers in infptrace.c.  */

#define FETCH_INFERIOR_REGISTERS

/* Setpgrp() takes arguments, unlike ordinary Sys V's.  */

#define	SETPGRP_ARGS 1

/* RS6000/AIXCOFF does not support PT_STEP. Has to be simulated. */

#define NO_SINGLE_STEP

/* AIX's assembler doesn't grok dollar signs in identifiers.
   So we use dots instead.  This item must be coordinated with G++. */
#undef CPLUS_MARKER
#define CPLUS_MARKER '.'

/* Flag for machine-specific stuff in shared files.  FIXME */
#define IBM6000_HOST

/* /usr/include/stdlib.h always uses void* and void,
   even when __STDC__ isn't defined. */
#define MALLOC_INCOMPATIBLE
extern void* malloc PARAMS (());
extern void* realloc PARAMS (());
extern void free PARAMS (());

