/* OBSOLETE /* Target definitions for m88k running Harris CX/UX. */
/* OBSOLETE    Copyright 1993, 1994 Free Software Foundation, Inc. */
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
/* OBSOLETE #define HARRIS_TARGET 1 */
/* OBSOLETE  */
/* OBSOLETE #define CXUX_TARGET 1 */
/* OBSOLETE  */
/* OBSOLETE /* Type of X registers, as supplied by the OS */ */
/* OBSOLETE  */
/* OBSOLETE typedef struct */
/* OBSOLETE   { */
/* OBSOLETE     long w1, w2, w3, w4; */
/* OBSOLETE   } */
/* OBSOLETE X_REGISTER_RAW_TYPE; */
/* OBSOLETE  */
/* OBSOLETE #define X_REGISTER_VIRTUAL_TYPE double */
/* OBSOLETE  */
/* OBSOLETE #include "m88k/tm-m88k.h" */
/* OBSOLETE  */
/* OBSOLETE #define ADD_SHARED_SYMBOL_FILES(args,have_tty)  add_shared_symbol_files () */
/* OBSOLETE  */
/* OBSOLETE #define CONVERT_REGISTER_ADDRESS */
/* OBSOLETE  */
/* OBSOLETE /* Always allocate space for both, but recognize that the m88100 has no */
/* OBSOLETE    FP_REGS.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef ARCH_NUM_REGS */
/* OBSOLETE #define ARCH_NUM_REGS (target_is_m88110 ? (GP_REGS + FP_REGS) : (GP_REGS)) */
/* OBSOLETE  */
/* OBSOLETE /* Don't need this grotesquerie.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef SHIFT_INST_REGS */
/* OBSOLETE  */
/* OBSOLETE /* Extended registers are treated as 16 bytes by Harris' OS's.  */
/* OBSOLETE    We treat them as 16 bytes here for consistency's sake.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef REGISTER_RAW_SIZE */
/* OBSOLETE #define REGISTER_RAW_SIZE(N) ((N) < XFP_REGNUM ? 4 : 16) */
/* OBSOLETE  */
/* OBSOLETE #undef REGISTER_BYTE */
/* OBSOLETE #define REGISTER_BYTE(N) \ */
/* OBSOLETE   ((N) >= XFP_REGNUM \ */
/* OBSOLETE    ? (((GP_REGS) * REGISTER_RAW_SIZE(0)) + \ */
/* OBSOLETE       (((N) - XFP_REGNUM) * REGISTER_RAW_SIZE(XFP_REGNUM))) \ */
/* OBSOLETE    : ((N) * REGISTER_RAW_SIZE(0))) */
