/* OBSOLETE /* Definitions to make GDB run on a merlin under utek 2.1 */
/* OBSOLETE    Copyright 1986, 1987, 1989, 1991, 1992, 1993, 2001 */
/* OBSOLETE    Free Software Foundation, Inc. */
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
/* OBSOLETE #include <machine/reg.h> */
/* OBSOLETE  */
/* OBSOLETE /* This machine doesn't have the siginterrupt call.  */ */
/* OBSOLETE #define NO_SIGINTERRUPT */
/* OBSOLETE  */
/* OBSOLETE /* Under Utek, a ptrace'd process can be the only active process for */
/* OBSOLETE    an executable.  Therefore instead of /bin/sh use gdb-sh (which should */
/* OBSOLETE    just be a copy of /bin/sh which is world readable and writeable).  */ */
/* OBSOLETE /* FIXME: name should be passed in from Makefile so it can use prefix.  */ */
/* OBSOLETE #define SHELL_FILE "/usr/local/lib/gdb-sh" */
/* OBSOLETE  */
/* OBSOLETE #define HOST_BYTE_ORDER LITTLE_ENDIAN */
/* OBSOLETE  */
/* OBSOLETE /* This is the amount to subtract from u.u_ar0 */
/* OBSOLETE    to get the offset in the core file of the register values.  */ */
/* OBSOLETE  */
/* OBSOLETE #define KERNEL_U_ADDR (0xfef000) */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_U_ADDR(addr, blockend, regno) \ */
/* OBSOLETE { 									\ */
/* OBSOLETE   switch (regno) {							\ */
/* OBSOLETE   case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:	\ */
/* OBSOLETE 	addr = blockend + (R0 - regno) * sizeof (int); break;		\ */
/* OBSOLETE   case PC_REGNUM:							\ */
/* OBSOLETE   	addr = blockend + PC * sizeof (int); break;			\ */
/* OBSOLETE   case SP_REGNUM:							\ */
/* OBSOLETE   	addr = blockend + SP * sizeof (int); break;			\ */
/* OBSOLETE   case FP_REGNUM:							\ */
/* OBSOLETE   	addr = blockend + FP * sizeof (int); break;			\ */
/* OBSOLETE   case PS_REGNUM:							\ */
/* OBSOLETE   	addr = blockend + 12 * sizeof (int); break;			\ */
/* OBSOLETE   case FPS_REGNUM:							\ */
/* OBSOLETE   	addr = 108; break;						\ */
/* OBSOLETE   case FP0_REGNUM + 0: case FP0_REGNUM + 1:				\ */
/* OBSOLETE   case FP0_REGNUM + 2: case FP0_REGNUM + 3: 				\ */
/* OBSOLETE   case FP0_REGNUM + 4: case FP0_REGNUM + 5: 				\ */
/* OBSOLETE   case FP0_REGNUM + 6: case FP0_REGNUM + 7: 				\ */
/* OBSOLETE   	addr = 76 + (regno - FP0_REGNUM) * sizeof (float); break;	\ */
/* OBSOLETE   case LP0_REGNUM + 0: case LP0_REGNUM + 1:				\ */
/* OBSOLETE   case LP0_REGNUM + 2: case LP0_REGNUM + 3:				\ */
/* OBSOLETE   	addr = 76 + (regno - LP0_REGNUM) * sizeof (double); break;	\ */
/* OBSOLETE   default:								\ */
/* OBSOLETE   	printf ("bad argument to REGISTER_U_ADDR %d\n", regno);		\ */
/* OBSOLETE 	internal_error (__FILE__, __LINE__, "failed internal consistency check");							\ */
/* OBSOLETE   }									\ */
/* OBSOLETE } */
