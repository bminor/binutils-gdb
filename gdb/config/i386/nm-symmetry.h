// OBSOLETE /* Definitions to make GDB run on a Sequent Symmetry under dynix 3.0,
// OBSOLETE    with Weitek 1167 and i387 support.
// OBSOLETE    Copyright 1986, 1987, 1989, 1992, 1994, 1996, 1998, 2000
// OBSOLETE    Free Software Foundation, Inc.
// OBSOLETE 
// OBSOLETE    This file is part of GDB.
// OBSOLETE 
// OBSOLETE    This program is free software; you can redistribute it and/or modify
// OBSOLETE    it under the terms of the GNU General Public License as published by
// OBSOLETE    the Free Software Foundation; either version 2 of the License, or
// OBSOLETE    (at your option) any later version.
// OBSOLETE 
// OBSOLETE    This program is distributed in the hope that it will be useful,
// OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of
// OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// OBSOLETE    GNU General Public License for more details.
// OBSOLETE 
// OBSOLETE    You should have received a copy of the GNU General Public License
// OBSOLETE    along with this program; if not, write to the Free Software
// OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330,
// OBSOLETE    Boston, MA 02111-1307, USA.  */
// OBSOLETE 
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE /* Override copies of {fetch,store}_inferior_registers in infptrace.c.  */
// OBSOLETE 
// OBSOLETE #define FETCH_INFERIOR_REGISTERS
// OBSOLETE 
// OBSOLETE /* We must fetch all the regs before storing, since we store all at once.  */
// OBSOLETE 
// OBSOLETE #define CHILD_PREPARE_TO_STORE() deprecated_read_register_bytes (0, NULL, REGISTER_BYTES)
// OBSOLETE 
// OBSOLETE #ifdef _SEQUENT_
// OBSOLETE #define CHILD_WAIT
// OBSOLETE extern ptid_t child_wait (ptid_t, struct target_waitstatus *);
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE /* This is the amount to subtract from u.u_ar0
// OBSOLETE    to get the offset in the core file of the register values.  */
// OBSOLETE 
// OBSOLETE #ifdef _SEQUENT_
// OBSOLETE #include <sys/param.h>
// OBSOLETE #include <sys/user.h>
// OBSOLETE #include <sys/mc_vmparam.h>
// OBSOLETE /* VA_UAREA is defined in <sys/mc_vmparam.h>, and is dependant upon 
// OBSOLETE    sizeof(struct user) */
// OBSOLETE #define KERNEL_U_ADDR (VA_UAREA)	/* ptx */
// OBSOLETE #else
// OBSOLETE #define KERNEL_U_ADDR (0x80000000 - (UPAGES * NBPG))	/* dynix */
// OBSOLETE #endif
