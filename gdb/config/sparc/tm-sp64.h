/* Target machine sub-parameters for SPARC64, for GDB, the GNU debugger.
   This is included by other tm-*.h files to define SPARC64 cpu-related info.
   Copyright 1994, 1995, 1996, 1998, 1999, 2000
   Free Software Foundation, Inc.
   This is (obviously) based on the SPARC Vn (n<9) port.
   Contributed by Doug Evans (dje@cygnus.com).
   Further modified by Bob Manson (manson@cygnus.com).

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

struct type;

#ifndef GDB_TARGET_IS_SPARC64
#define GDB_TARGET_IS_SPARC64 1
#endif

#include "sparc/tm-sparc.h"

/* Eeeew. Ok, we have to assume (for now) that the processor really is
   in sparc64 mode. While this is the same instruction sequence as
   on the Sparc, the stack frames are offset by +2047 (and the arguments
   are 8 bytes instead of 4). */
/* Instructions are:
   std  %f10, [ %fp + 0x7a7 ]
   std  %f8, [ %fp + 0x79f ]
   std  %f6, [ %fp + 0x797 ]
   std  %f4, [ %fp + 0x78f ]
   std  %f2, [ %fp + 0x787 ]
   std  %f0, [ %fp + 0x77f ]
   std  %g6, [ %fp + 0x777 ]
   std  %g4, [ %fp + 0x76f ]
   std  %g2, [ %fp + 0x767 ]
   std  %g0, [ %fp + 0x75f ]
   std  %fp, [ %fp + 0x757 ]
   std  %i4, [ %fp + 0x74f ]
   std  %i2, [ %fp + 0x747 ]
   std  %i0, [ %fp + 0x73f ]
   nop
   nop
   nop
   nop
   rd  %tbr, %o0
   st  %o0, [ %fp + 0x72b ]
   rd  %tpc, %o0
   st  %o0, [ %fp + 0x727 ]
   rd  %psr, %o0
   st  %o0, [ %fp + 0x723 ]
   rd  %y, %o0
   st  %o0, [ %fp + 0x71f ]
   ldx  [ %sp + 0x8a7 ], %o5
   ldx  [ %sp + 0x89f ], %o4
   ldx  [ %sp + 0x897 ], %o3
   ldx  [ %sp + 0x88f ], %o2
   ldx  [ %sp + 0x887 ], %o1
   call  %g0
   ldx  [ %sp + 0x87f ], %o0
   nop
   ta  1
   nop
   nop
 */

/* Offsets into jmp_buf.
   FIXME: This was borrowed from the v8 stuff and will probably have to change
   for v9.  */

#define JB_ELEMENT_SIZE 8	/* Size of each element in jmp_buf */

#define JB_ONSSTACK 0
#define JB_SIGMASK 1
#define JB_SP 2
#define JB_PC 3
#define JB_NPC 4
#define JB_PSR 5
#define JB_G1 6
#define JB_O0 7
#define JB_WBCNT 8

/* Figure out where the longjmp will land.  We expect that we have
   just entered longjmp and haven't yet setup the stack frame, so the
   args are still in the output regs.  %o0 (O0_REGNUM) points at the
   jmp_buf structure from which we extract the pc (JB_PC) that we will
   land at.  The pc is copied into ADDR.  This routine returns true on
   success */

extern int get_longjmp_target (CORE_ADDR *);

#define GET_LONGJMP_TARGET(ADDR) get_longjmp_target(ADDR)

#undef TM_PRINT_INSN_MACH
#define TM_PRINT_INSN_MACH bfd_mach_sparc_v9a

