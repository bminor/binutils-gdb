/* OBSOLETE /* Definitions to make GDB run on an encore under umax 4.2 */
/* OBSOLETE    Copyright 1987, 1989, 1992, 2001 Free Software Foundation, Inc. */
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
/* OBSOLETE /* Do implement the attach and detach commands...  */ */
/* OBSOLETE #define ATTACH_DETACH */
/* OBSOLETE  */
/* OBSOLETE /* Offset of registers within u area.  */ */
/* OBSOLETE #define U_REGS_OFFSET 0 */
/* OBSOLETE  */
/* OBSOLETE /* called from register_addr() -- blockend not used for now */ */
/* OBSOLETE #define REGISTER_U_ADDR(addr, blockend, regno) \ */
/* OBSOLETE { 									\ */
/* OBSOLETE   switch (regno) {							\ */
/* OBSOLETE   case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:	\ */
/* OBSOLETE 	addr = PU_R0 - (regno * sizeof (int)); break;			\ */
/* OBSOLETE   case SP_REGNUM:							\ */
/* OBSOLETE   	addr = PU_SP; break;						\ */
/* OBSOLETE   case PC_REGNUM:							\ */
/* OBSOLETE   	addr = PU_PC; break;						\ */
/* OBSOLETE   case FP_REGNUM:							\ */
/* OBSOLETE   	addr = PU_FP; break;						\ */
/* OBSOLETE   case PS_REGNUM:							\ */
/* OBSOLETE   	addr = PU_PSL; break;						\ */
/* OBSOLETE   case FPS_REGNUM:							\ */
/* OBSOLETE   	addr = PU_FSR; break;						\ */
/* OBSOLETE   case FP0_REGNUM + 0: case FP0_REGNUM + 1:				\ */
/* OBSOLETE   case FP0_REGNUM + 2: case FP0_REGNUM + 3: 				\ */
/* OBSOLETE   case FP0_REGNUM + 4: case FP0_REGNUM + 5: 				\ */
/* OBSOLETE   case FP0_REGNUM + 6: case FP0_REGNUM + 7: 				\ */
/* OBSOLETE   	addr = PU_F0 + (regno - FP0_REGNUM) * sizeof (float); break;	\ */
/* OBSOLETE   case LP0_REGNUM + 0: case LP0_REGNUM + 1:				\ */
/* OBSOLETE   case LP0_REGNUM + 2: case LP0_REGNUM + 3:				\ */
/* OBSOLETE   	addr = PU_F0 + (regno - LP0_REGNUM) * sizeof (double); break;	\ */
/* OBSOLETE   default:								\ */
/* OBSOLETE   	printf ("bad argument to REGISTER_U_ADDR %d\n", regno);		\ */
/* OBSOLETE 	internal_error (__FILE__, __LINE__, "failed internal consistency check");							\ */
/* OBSOLETE   }									\ */
/* OBSOLETE } */
