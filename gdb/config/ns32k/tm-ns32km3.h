/* OBSOLETE /* Macro definitions for ns532, Mach 3.0 */
/* OBSOLETE    Copyright 1992, 1993, 1994 Free Software Foundation, Inc. */
/* OBSOLETE  */
/* OBSOLETE    This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE    This program is free software; you can redistribute it and/or modify */
/* OBSOLETE    it under the terms of the GNU General Public License as published by */
/* OBSOLETE    the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE    (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE    This program is distributed in the hope that it will be useful, */
/* OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE    GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE    You should have received a copy of the GNU General Public License */
/* OBSOLETE    along with this program; if not, write to the Free Software */
/* OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE    Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE  */
/* OBSOLETE /* Include common definitions for Mach3 systems */ */
/* OBSOLETE #include "nm-m3.h" */
/* OBSOLETE  */
/* OBSOLETE /* Define offsets to access CPROC stack when it does not have */
/* OBSOLETE  * a kernel thread. */
/* OBSOLETE  */ */
/* OBSOLETE #define MACHINE_CPROC_SP_OFFSET 20 */
/* OBSOLETE #define MACHINE_CPROC_PC_OFFSET 16 */
/* OBSOLETE #define MACHINE_CPROC_FP_OFFSET 12 */
/* OBSOLETE  */
/* OBSOLETE #include <ns532/psl.h> */
/* OBSOLETE #include <ns532/vmparam.h> */
/* OBSOLETE  */
/* OBSOLETE /* Thread flavors used in re-setting the T bit. */
/* OBSOLETE  * @@ this is also bad for cross debugging. */
/* OBSOLETE  */ */
/* OBSOLETE #define TRACE_FLAVOR		NS532_THREAD_STATE */
/* OBSOLETE #define TRACE_FLAVOR_SIZE	NS532_THREAD_STATE_COUNT */
/* OBSOLETE #define TRACE_SET(x,state) \ */
/* OBSOLETE   	((struct ns532_thread_state *)state)->psr |= PSR_T */
/* OBSOLETE #define TRACE_CLEAR(x,state) \ */
/* OBSOLETE   	((((struct ns532_thread_state *)state)->psr &= ~PSR_T), 1) */
/* OBSOLETE  */
/* OBSOLETE /* we can do it */ */
/* OBSOLETE #define ATTACH_DETACH 1 */
/* OBSOLETE  */
/* OBSOLETE /* Address of end of stack space. */
/* OBSOLETE  * for MACH, see <ns532/vmparam.h> */
/* OBSOLETE  */ */
/* OBSOLETE #define STACK_END_ADDR USRSTACK */
/* OBSOLETE  */
/* OBSOLETE #include "ns32k/tm-umax.h" */
/* OBSOLETE  */
/* OBSOLETE /* tm-umax.h assumes a 32082 fpu. We have a 32382 fpu. */ */
/* OBSOLETE #undef REGISTER_NAMES */
/* OBSOLETE #undef NUM_REGS */
/* OBSOLETE #undef REGISTER_BYTES */
/* OBSOLETE /* Initializer for an array of names of registers. */
/* OBSOLETE    There should be NUM_REGS strings in this initializer.  */ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_NAMES {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",	\ */
/* OBSOLETE  			"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",	\ */
/* OBSOLETE 			"sp", "fp", "pc", "ps",				\ */
/* OBSOLETE  			"fsr",						\ */
/* OBSOLETE 			"l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7", "xx",			\ */
/* OBSOLETE  			} */
/* OBSOLETE  */
/* OBSOLETE #define NUM_REGS		29 */
/* OBSOLETE  */
/* OBSOLETE /* Total amount of space needed to store our copies of the machine's */
/* OBSOLETE    register state, the array `registers'.  */ */
/* OBSOLETE #define REGISTER_BYTES \ */
/* OBSOLETE   ((NUM_REGS - 4) * REGISTER_RAW_SIZE(R0_REGNUM) \ */
/* OBSOLETE    + 8            * REGISTER_RAW_SIZE(LP0_REGNUM)) */
