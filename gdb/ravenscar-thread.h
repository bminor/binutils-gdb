/* Ada Ravenscar thread support.

   Copyright (C) 2004-2022 Free Software Foundation, Inc.

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

#ifndef RAVENSCAR_THREAD_H
#define RAVENSCAR_THREAD_H

/* Architecture-specific hooks.  */

struct ravenscar_arch_ops
{
  ravenscar_arch_ops (gdb::array_view<const int> offsets_,
		      int first_stack = -1,
		      int last_stack = -1)
    : offsets (offsets_),
      first_stack_register (first_stack),
      last_stack_register (last_stack)
  {
    /* These must either both be -1 or both be valid.  */
    gdb_assert ((first_stack_register == -1) == (last_stack_register == -1));
    /* They must also be ordered.  */
    gdb_assert (last_stack_register >= first_stack_register);
  }

  void fetch_registers (struct regcache *, int) const;
  void store_registers (struct regcache *, int) const;

private:

  /* An array where the indices are register numbers and the contents
     are offsets.  The offsets are either in the thread descriptor or
     the stack, depending on the other fields.  An offset of -1 means
     that the corresponding register is not stored.  */
  const gdb::array_view<const int> offsets;

  /* If these are -1, then all registers for this architecture are
     stored in the thread descriptor.  Otherwise, these mark a range
     of registers that are stored on the stack.  */
  const int first_stack_register;
  const int last_stack_register;

  /* Helper function to supply one register.  */
  void supply_one_register (struct regcache *regcache, int regnum,
			    CORE_ADDR descriptor,
			    CORE_ADDR stack_base) const;
  /* Helper function to store one register.  */
  void store_one_register (struct regcache *regcache, int regnum,
			   CORE_ADDR descriptor,
			   CORE_ADDR stack_base) const;
  /* Helper function to find stack address where registers are stored.
     This must be called with the stack pointer already supplied in
     the register cache.  */
  CORE_ADDR get_stack_base (struct regcache *) const;
};

#endif /* !defined (RAVENSCAR_THREAD_H) */
