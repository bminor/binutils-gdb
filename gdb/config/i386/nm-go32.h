/* Native definitions for Intel x86 running DJGPP.
   Copyright (C) 1997, 1998, 1999 Free Software Foundation, Inc.

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

#define NO_PTRACE_H

#include "i386/nm-i386v.h"

#define TARGET_HAS_HARDWARE_WATCHPOINTS

#define TARGET_CAN_USE_HARDWARE_WATCHPOINT(type, cnt, ot) 1

/* After a watchpoint trap, the PC points to the instruction after the
   one that caused the trap.  Therefore we don't need to step over it.
   But we do need to reset the status register to avoid another trap.  */

#define HAVE_CONTINUABLE_WATCHPOINT

#define STOPPED_BY_WATCHPOINT(W)  \
  go32_stopped_by_watchpoint (inferior_pid)

/* Use these macros for watchpoint insertion/removal.  */

#define target_insert_watchpoint(addr, len, type)  \
  go32_insert_watchpoint (inferior_pid, addr, len, 2)

#define target_remove_watchpoint(addr, len, type)  \
  go32_remove_watchpoint (inferior_pid, addr, len)

#define target_insert_hw_breakpoint(addr, shadow)  \
  go32_insert_hw_breakpoint(addr, shadow)
  
#define target_remove_hw_breakpoint(addr, shadow)  \
  go32_remove_hw_breakpoint(addr, shadow)

#define DECR_PC_AFTER_HW_BREAK 0

#undef FLOAT_INFO
#define FLOAT_INFO { i386_go32_float_info (); }

extern void i386_go32_float_info (void);

