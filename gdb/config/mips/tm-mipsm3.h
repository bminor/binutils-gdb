// OBSOLETE /* Definitions to make GDB run on a mips box under Mach 3.0
// OBSOLETE    Copyright 1992, 1993, 1998 Free Software Foundation, Inc.
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
// OBSOLETE /* Mach specific definitions for little endian mips (e.g. pmax)
// OBSOLETE  * running Mach 3.0
// OBSOLETE  *
// OBSOLETE  * Author: Jukka Virtanen <jtv@hut.fi>
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE /* Include common definitions for Mach3 systems */
// OBSOLETE #include "config/nm-m3.h"
// OBSOLETE 
// OBSOLETE /* Define offsets to access CPROC stack when it does not have
// OBSOLETE  * a kernel thread.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE /* From mk/user/threads/mips/csw.s */
// OBSOLETE #define SAVED_FP	(12*4)
// OBSOLETE #define SAVED_PC	(13*4)
// OBSOLETE #define SAVED_BYTES	(14*4)
// OBSOLETE 
// OBSOLETE /* Using these, define our offsets to items strored in
// OBSOLETE  * cproc_switch in csw.s
// OBSOLETE  */
// OBSOLETE #define MACHINE_CPROC_SP_OFFSET SAVED_BYTES
// OBSOLETE #define MACHINE_CPROC_PC_OFFSET SAVED_PC
// OBSOLETE #define MACHINE_CPROC_FP_OFFSET SAVED_FP
// OBSOLETE 
// OBSOLETE /* Thread flavors used in setting the Trace state.
// OBSOLETE 
// OBSOLETE  * In <mach/machine/thread_status.h>
// OBSOLETE  */
// OBSOLETE #define TRACE_FLAVOR		MIPS_EXC_STATE
// OBSOLETE #define TRACE_FLAVOR_SIZE	MIPS_EXC_STATE_COUNT
// OBSOLETE #define TRACE_SET(x,state)	((struct mips_exc_state *)state)->cause = EXC_SST;
// OBSOLETE #define TRACE_CLEAR(x,state)	0
// OBSOLETE 
// OBSOLETE /* Mach supports attach/detach */
// OBSOLETE #define ATTACH_DETACH 1
// OBSOLETE 
// OBSOLETE #include "mips/tm-mips.h"
// OBSOLETE 
// OBSOLETE /* Address of end of user stack space.
// OBSOLETE  * for MACH, see <machine/vmparam.h>
// OBSOLETE  */
// OBSOLETE #undef  STACK_END_ADDR
// OBSOLETE #define STACK_END_ADDR USRSTACK
// OBSOLETE 
// OBSOLETE /* Output registers in tabular format */
// OBSOLETE #define TABULAR_REGISTER_OUTPUT
