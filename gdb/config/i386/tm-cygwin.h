/* Macro definitions for i386 running under the win32 API Unix.
   Copyright 1995 - 1999 Free Software Foundation, Inc.

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


#undef HAVE_SSE_REGS	/* FIXME! win32-nat.c needs to support XMMi registers */
#define HAVE_I387_REGS

#include "i386/tm-i386v.h"

#define IN_SOLIB_CALL_TRAMPOLINE(pc, name) skip_trampoline_code (pc, name)
#define SKIP_TRAMPOLINE_CODE(pc)           skip_trampoline_code (pc, 0)
extern CORE_ADDR skip_trampoline_code PARAMS ((CORE_ADDR pc, char *name));

extern char *cygwin_pid_to_str PARAMS ((int pid));

struct frame_info;
void child_init_frame(int x, struct frame_info *);
CORE_ADDR child_frame_saved_pc(struct frame_info *);
CORE_ADDR child_frame_chain(struct frame_info *);

#undef FRAME_CHAIN_VALID_ALTERNATE
#define FRAME_CHAIN_VALID_ALTERNATE 1

#undef INIT_EXTRA_FRAME_INFO
#define INIT_EXTRA_FRAME_INFO(x, f) child_init_frame(x, f) 

#undef FRAME_CHAIN
#define FRAME_CHAIN child_frame_chain

#undef FRAME_SAVED_PC
#define FRAME_SAVED_PC child_frame_saved_pc
