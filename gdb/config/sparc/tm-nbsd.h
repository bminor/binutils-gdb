/* Macro definitions for Sparc running under NetBSD.
   Copyright 1994 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef TM_NBSD_H
#define TM_NBSD_H

#include "sparc/tm-sparc.h"
#include "tm-nbsd.h"

/* make things match up with what is expected in sparc-nat.c */
#define regs		trapframe
#define fp_status	fpstate

#define r_g1		tf_global[1]
#define r_ps		tf_psr
#define r_pc		tf_pc
#define r_npc		tf_npc
#define r_y		tf_y

#define fpu		fpstate
#define fpu_regs	fs_regs
#define fpu_fsr		fs_fsr
#define fpu_fr		fs_regs
#define Fpu_fsr		fs_fsr
#define FPU_FSR_TYPE	int

#define PTRACE_GETREGS	 PT_GETREGS
#define PTRACE_GETFPREGS PT_GETFPREGS
#define PTRACE_SETREGS	 PT_SETREGS
#define PTRACE_SETFPREGS PT_SETFPREGS

#endif /* TM_NBSD_H */
