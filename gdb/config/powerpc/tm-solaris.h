/* OBSOLETE /* Macro definitions for GDB for a PowerPC running Solaris 2 */
/* OBSOLETE    Copyright 1996, 2000 Free Software Foundation, Inc. */
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
/* OBSOLETE #include "powerpc/tm-ppc-eabi.h" */
/* OBSOLETE  */
/* OBSOLETE /* There are two different signal handler trampolines in Solaris2.  */ */
/* OBSOLETE #define IN_SIGTRAMP(pc, name) \ */
/* OBSOLETE   ((name) \ */
/* OBSOLETE    && (STREQ ("sigacthandler", name) || STREQ ("ucbsigvechandler", name))) */
/* OBSOLETE  */
/* OBSOLETE /* The signal handler gets a pointer to an ucontext as third argument */
/* OBSOLETE    if it is called from sigacthandler.  This is the offset to the saved */
/* OBSOLETE    PC within it.  sparc_frame_saved_pc knows how to deal with */
/* OBSOLETE    ucbsigvechandler.  */ */
/* OBSOLETE #define SIGCONTEXT_PC_OFFSET 44 */
/* OBSOLETE  */
/* OBSOLETE #if 0				/* FIXME Setjmp/longjmp are not as well doc'd in SunOS 5.x yet */ */
/* OBSOLETE  */
/* OBSOLETE /* Offsets into jmp_buf.  Not defined by Sun, but at least documented in a */
/* OBSOLETE    comment in <machine/setjmp.h>! */ */
/* OBSOLETE  */
/* OBSOLETE #define JB_ELEMENT_SIZE 4	/* Size of each element in jmp_buf */ */
/* OBSOLETE  */
/* OBSOLETE #define JB_ONSSTACK 0 */
/* OBSOLETE #define JB_SIGMASK 1 */
/* OBSOLETE #define JB_SP 2 */
/* OBSOLETE #define JB_PC 3 */
/* OBSOLETE #define JB_NPC 4 */
/* OBSOLETE #define JB_PSR 5 */
/* OBSOLETE #define JB_G1 6 */
/* OBSOLETE #define JB_O0 7 */
/* OBSOLETE #define JB_WBCNT 8 */
/* OBSOLETE  */
/* OBSOLETE /* Figure out where the longjmp will land.  We expect that we have just entered */
/* OBSOLETE    longjmp and haven't yet setup the stack frame, so the args are still in the */
/* OBSOLETE    output regs.  %o0 (O0_REGNUM) points at the jmp_buf structure from which we */
/* OBSOLETE    extract the pc (JB_PC) that we will land at.  The pc is copied into ADDR. */
/* OBSOLETE    This routine returns true on success */ */
/* OBSOLETE  */
/* OBSOLETE extern int get_longjmp_target (CORE_ADDR *); */
/* OBSOLETE  */
/* OBSOLETE #define GET_LONGJMP_TARGET(ADDR) get_longjmp_target(ADDR) */
/* OBSOLETE #endif /* 0 */ */
/* OBSOLETE  */
/* OBSOLETE /* The SunPRO compiler puts out 0 instead of the address in N_SO symbols, */
/* OBSOLETE    and for SunPRO 3.0, N_FUN symbols too.  */ */
/* OBSOLETE #define SOFUN_ADDRESS_MAYBE_MISSING */
/* OBSOLETE  */
/* OBSOLETE #if 0 */
/* OBSOLETE extern char *sunpro_static_transform_name (char *); */
/* OBSOLETE #define STATIC_TRANSFORM_NAME(x) sunpro_static_transform_name (x) */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE #define FAULTED_USE_SIGINFO */
/* OBSOLETE  */
/* OBSOLETE /* Enable handling of shared libraries for a.out executables.  */ */
/* OBSOLETE #define HANDLE_SVR4_EXEC_EMULATORS */
