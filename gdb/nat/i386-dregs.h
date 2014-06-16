/* Debug register code for the i386.

   Copyright (C) 2009-2014 Free Software Foundation, Inc.

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

/* Support for hardware watchpoints and breakpoints using the i386
   debug registers.

   This provides several functions for inserting and removing
   hardware-assisted breakpoints and watchpoints, testing if one or
   more of the watchpoints triggered and at what address, checking
   whether a given region can be watched, etc.

   The functions below implement debug registers sharing by reference
   counts, and allow to watch regions up to 16 bytes long
   (32 bytes on 64 bit hosts).  */

#ifndef I386_DREGS_H
#define I386_DREGS_H 1

/* Forward declaration.  */
enum target_hw_bp_type;

/* Debug registers' indices.  */
#define DR_FIRSTADDR 0
#define DR_LASTADDR  3
#define DR_NADDR     4	/* The number of debug address registers.  */
#define DR_STATUS    6	/* Index of debug status register (DR6).  */
#define DR_CONTROL   7	/* Index of debug control register (DR7).  */

/* Global state needed to track h/w watchpoints.  */

struct i386_debug_reg_state
{
  /* Mirror the inferior's DRi registers.  We keep the status and
     control registers separated because they don't hold addresses.
     Note that since we can change these mirrors while threads are
     running, we never trust them to explain a cause of a trap.
     For that, we need to peek directly in the inferior registers.  */
  CORE_ADDR dr_mirror[DR_NADDR];
  unsigned dr_status_mirror, dr_control_mirror;

  /* Reference counts for each debug register.  */
  int dr_ref_count[DR_NADDR];
};

/* A macro to loop over all debug registers.  */
#define ALL_DEBUG_REGISTERS(i)	for (i = 0; i < DR_NADDR; i++)

/* High-level functions.  */

/* Insert a watchpoint to watch a memory region which starts at
   address ADDR and whose length is LEN bytes.  Watch memory accesses
   of the type TYPE.  Return 0 on success, -1 on failure.  */
extern int i386_dr_insert_watchpoint (struct i386_debug_reg_state *state,
				      enum target_hw_bp_type type,
				      CORE_ADDR addr,
				      int len);

/* Remove a watchpoint that watched the memory region which starts at
   address ADDR, whose length is LEN bytes, and for accesses of the
   type TYPE.  Return 0 on success, -1 on failure.  */
extern int i386_dr_remove_watchpoint (struct i386_debug_reg_state *state,
				      enum target_hw_bp_type type,
				      CORE_ADDR addr,
				      int len);

/* Return non-zero if we can watch a memory region that starts at
   address ADDR and whose length is LEN bytes.  */
extern int i386_dr_region_ok_for_watchpoint (struct i386_debug_reg_state *state,
					     CORE_ADDR addr, int len);

/* If the inferior has some break/watchpoint that triggered, set the
   address associated with that break/watchpoint and return true.
   Otherwise, return false.  */
extern int i386_dr_stopped_data_address (struct i386_debug_reg_state *state,
					 CORE_ADDR *addr_p);

/* Return true if the inferior has some watchpoint that triggered.
   Otherwise return false.  */
extern int i386_dr_stopped_by_watchpoint (struct i386_debug_reg_state *state);

/* Low-level functions.  */

/* Print the values of the mirrored debug registers.  */

extern void i386_dr_show (struct i386_debug_reg_state *state,
			  const char *func, CORE_ADDR addr,
			  int len, enum target_hw_bp_type type);

/* Return the value of a 4-bit field for DR7 suitable for watching a
   region of LEN bytes for accesses of type TYPE.  LEN is assumed to
   have the value of 1, 2, or 4.  */

extern unsigned i386_dr_length_and_rw_bits (int len,
					    enum target_hw_bp_type type);

/* Insert a watchpoint at address ADDR, which is assumed to be aligned
   according to the length of the region to watch.  LEN_RW_BITS is the
   value of the bits from DR7 which describes the length and access
   type of the region to be watched by this watchpoint.  Return 0 on
   success, -1 on failure.  */

extern int i386_dr_insert_aligned_watchpoint (struct i386_debug_reg_state *state,
					      CORE_ADDR addr,
					      unsigned len_rw_bits);

/* Remove a watchpoint at address ADDR, which is assumed to be aligned
   according to the length of the region to watch.  LEN_RW_BITS is the
   value of the bits from DR7 which describes the length and access
   type of the region watched by this watchpoint.  Return 0 on
   success, -1 on failure.  */

extern int i386_dr_remove_aligned_watchpoint (struct i386_debug_reg_state *state,
					      CORE_ADDR addr,
					      unsigned len_rw_bits);

/* Update the inferior debug registers state, in STATE, with the
   new debug registers state, in NEW_STATE.  */

extern void i386_dr_update_inferior_debug_regs (struct i386_debug_reg_state *state,
						struct i386_debug_reg_state *new_state);

#endif /* I386_DREGS_H */
