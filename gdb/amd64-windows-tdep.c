/* Copyright (C) 2009, 2010 Free Software Foundation, Inc.

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

#include "defs.h"
#include "osabi.h"
#include "amd64-tdep.h"
#include "solib.h"
#include "solib-target.h"
#include "gdbtypes.h"

/* The registers used to pass integer arguments during a function call.  */
static int amd64_windows_dummy_call_integer_regs[] =
{
  AMD64_RCX_REGNUM,          /* %rcx */
  AMD64_RDX_REGNUM,          /* %rdx */
  8,                         /* %r8 */
  9                          /* %r9 */
};

/* Implement the "classify" method in the gdbarch_tdep structure
   for amd64-windows.  */

static void
amd64_windows_classify (struct type *type, enum amd64_reg_class class[2])
{
  switch (TYPE_CODE (type))
    {
      case TYPE_CODE_ARRAY:
	/* Arrays are always passed by memory.	*/
	class[0] = class[1] = AMD64_MEMORY;
	break;

      case TYPE_CODE_STRUCT:
      case TYPE_CODE_UNION:
        /* Struct/Union types whose size is 1, 2, 4, or 8 bytes
	   are passed as if they were integers of the same size.
	   Types of different sizes are passed by memory.  */
	if (TYPE_LENGTH (type) == 1
	    || TYPE_LENGTH (type) == 2
	    || TYPE_LENGTH (type) == 4
	    || TYPE_LENGTH (type) == 8)
	  {
	    class[0] = AMD64_INTEGER;
	    class[1] = AMD64_NO_CLASS;
	  }
	else
	  class[0] = class[1] = AMD64_MEMORY;
	break;

      default:
	/* For all the other types, the conventions are the same as
	   with the System V ABI.  */
	amd64_classify (type, class);
    }
}

static void
amd64_windows_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  amd64_init_abi (info, gdbarch);

  /* On Windows, "long"s are only 32bit.  */
  set_gdbarch_long_bit (gdbarch, 32);

  /* Function calls.  */
  tdep->call_dummy_num_integer_regs =
    ARRAY_SIZE (amd64_windows_dummy_call_integer_regs);
  tdep->call_dummy_integer_regs = amd64_windows_dummy_call_integer_regs;
  tdep->classify = amd64_windows_classify;
  tdep->memory_args_by_pointer = 1;

  set_solib_ops (gdbarch, &solib_target_so_ops);
}

void
_initialize_amd64_windows_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_i386, bfd_mach_x86_64, GDB_OSABI_CYGWIN,
                          amd64_windows_init_abi);
}

