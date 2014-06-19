/* Native-dependent code for the i386.

   Low level functions to implement Oeprating System specific
   code to manipulate I386 debug registers.

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

#ifndef I386_NAT_H
#define I386_NAT_H 1

#include "nat/i386-dregs.h"

/* Hardware-assisted breakpoints and watchpoints.  */

/* Whether or not to print the mirrored debug registers.  */
extern int debug_hw_points;

/* Add watchpoint methods to the provided target_ops.  
   Targets using i386 family debug registers for watchpoints should call
   this.  */
struct target_ops;
extern void i386_use_watchpoints (struct target_ops *);

/* Support for hardware watchpoints and breakpoints using the i386
   debug registers.

   Each target should provide several low-level functions
   regrouped into i386_dr_low_type struct below.  These functions
   that will be called to insert watchpoints and hardware breakpoints
   into the inferior, remove them, and check their status.  These
   functions are:

      set_control              -- set the debug control (DR7)
				  register to a given value for all LWPs

      set_addr                 -- put an address into one debug
				  register for all LWPs

      get_addr                 -- return the address in a given debug
				  register of the current LWP

      get_status               -- return the value of the debug
				  status (DR6) register for current LWP

      get_control               -- return the value of the debug
				  control (DR7) register for current LWP

   Additionally, the native file should set the debug_register_length
   field to 4 or 8 depending on the number of bytes used for
   deubg registers.  */

struct i386_dr_low_type
  {
    void (*set_control) (unsigned long);
    void (*set_addr) (int, CORE_ADDR);
    CORE_ADDR (*get_addr) (int);
    unsigned long (*get_status) (void);
    unsigned long (*get_control) (void);
    int debug_register_length;
  };

extern struct i386_dr_low_type i386_dr_low;

/* Can we update the inferior's debug registers?  */
#define i386_dr_low_can_set_addr() (i386_dr_low.set_addr != NULL)

/* Update the inferior's debug register REGNUM from STATE.  */
#define i386_dr_low_set_addr(new_state, i) \
  (i386_dr_low.set_addr ((i), (new_state)->dr_mirror[(i)]))

/* Return the inferior's debug register REGNUM.  */
#define i386_dr_low_get_addr(i) (i386_dr_low.get_addr ((i)))

/* Can we update the inferior's DR7 control register?  */
#define i386_dr_low_can_set_control() (i386_dr_low.set_control != NULL)

/* Update the inferior's DR7 debug control register from STATE.  */
#define i386_dr_low_set_control(new_state) \
  (i386_dr_low.set_control ((new_state)->dr_control_mirror))

/* Return the value of the inferior's DR7 debug control register.  */
#define i386_dr_low_get_control() (i386_dr_low.get_control ())

/* Return the value of the inferior's DR6 debug status register.  */
#define i386_dr_low_get_status() (i386_dr_low.get_status ())

/* Return the debug register size, in bytes.  */
#define i386_get_debug_register_length() \
  (i386_dr_low.debug_register_length)

/* Use this function to set i386_dr_low debug_register_length field
   rather than setting it directly to check that the length is only
   set once.  It also enables the 'maint set/show show-debug-regs' 
   command.  */

extern void i386_set_debug_register_length (int len);

/* Use this function to reset the i386-nat.c debug register state.  */

extern void i386_cleanup_dregs (void);

/* Return a pointer to the local mirror of the debug registers of
   process PID.  */

extern struct i386_debug_reg_state *i386_debug_reg_state (pid_t pid);

/* Called whenever GDB is no longer debugging process PID.  It deletes
   data structures that keep track of debug register state.  */

extern void i386_forget_process (pid_t pid);

#endif /* I386_NAT_H */
