/* Native macro definitions for GDB on an Intel i[3456]86.
   Copyright 2001, 2004, 2007, 2008, 2009 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef NM_I386_H
#define NM_I386_H 1

/* Hardware-assisted breakpoints and watchpoints.  */

/* Targets should define this to use the generic x86 watchpoint support.  */
#ifdef I386_USE_GENERIC_WATCHPOINTS

/* Add watchpoint methods to the provided target_ops.  
   Targets which define I386_USE_GENERIC_WATCHPOINTS must
   call this function.  */
struct target_ops;
void i386_use_watchpoints (struct target_ops *);

/* Clear the reference counts and forget everything we knew about DRi.  */
extern void i386_cleanup_dregs (void);

/* Insert a watchpoint to watch a memory region which starts at
   address ADDR and whose length is LEN bytes.  Watch memory accesses
   of the type TYPE.  Return 0 on success, -1 on failure.  */
extern int i386_insert_watchpoint (CORE_ADDR addr, int len, int type);

/* Remove a watchpoint that watched the memory region which starts at
   address ADDR, whose length is LEN bytes, and for accesses of the
   type TYPE.  Return 0 on success, -1 on failure.  */
extern int i386_remove_watchpoint (CORE_ADDR addr, int len, int type);

/* Return non-zero if we can watch a memory region that starts at
   address ADDR and whose length is LEN bytes.  */
extern int i386_region_ok_for_watchpoint (CORE_ADDR addr, int len);

/* Return non-zero if the inferior has some break/watchpoint that
   triggered.  */
extern int i386_stopped_by_hwbp (void);

/* If the inferior has some break/watchpoint that triggered, set
   the address associated with that break/watchpoint and return
   true.  Otherwise, return false.  */
extern int i386_stopped_data_address (struct target_ops *, CORE_ADDR *);

/* Insert a hardware-assisted breakpoint at BP_TGT->placed_address.
   Return 0 on success, EBUSY on failure.  */
struct bp_target_info;
extern int i386_insert_hw_breakpoint (struct bp_target_info *bp_tgt);

/* Remove a hardware-assisted breakpoint at BP_TGT->placed_address.
   Return 0 on success, -1 on failure.  */
extern int  i386_remove_hw_breakpoint (struct bp_target_info *bp_tgt);

extern int i386_stopped_by_watchpoint (void);

#endif /* I386_USE_GENERIC_WATCHPOINTS */

#endif /* NM_I386_H */
