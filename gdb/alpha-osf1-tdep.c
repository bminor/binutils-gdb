/* Target-dependent code for OSF/1 on Alpha.
   Copyright 2002, 2003 Free Software Foundation, Inc.

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

#include "defs.h"
#include "frame.h"
#include "gdbcore.h"
#include "value.h"
#include "osabi.h"
#include "gdb_string.h"
#include "objfiles.h"

#include "alpha-tdep.h"

static int
alpha_osf1_pc_in_sigtramp (CORE_ADDR pc, char *func_name)
{
  return (func_name != NULL && strcmp ("__sigtramp", func_name) == 0);
}

static CORE_ADDR
alpha_osf1_sigcontext_addr (struct frame_info *frame)
{
  struct frame_info *next_frame = get_next_frame (frame);

  if (next_frame != NULL)
    return (read_memory_integer (get_frame_base (next_frame), 8));
  else
    return (read_memory_integer (get_frame_base (frame), 8));
}

/* This is the definition of CALL_DUMMY_ADDRESS.  It's a heuristic that is used
   to find a convenient place in the text segment to stick a breakpoint to
   detect the completion of a target function call (ala call_function_by_hand).
 */

static CORE_ADDR
alpha_call_dummy_address (void)
{
  CORE_ADDR entry;
  struct minimal_symbol *sym;

  entry = entry_point_address ();

  if (entry != 0)
    return entry;

  sym = lookup_minimal_symbol ("_Prelude", NULL, symfile_objfile);

  if (!sym || MSYMBOL_TYPE (sym) != mst_text)
    return 0;
  else
    return SYMBOL_VALUE_ADDRESS (sym) + 4;
}

static void
alpha_osf1_init_abi (struct gdbarch_info info,
                     struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* Hook into the MDEBUG frame unwinder.  */
  alpha_mdebug_init_abi (info, gdbarch);

  set_gdbarch_pc_in_sigtramp (gdbarch, alpha_osf1_pc_in_sigtramp);
  /* The next/step support via procfs on OSF1 is broken when running
     on multi-processor machines. We need to use software single stepping
     instead.  */
  set_gdbarch_software_single_step (gdbarch, alpha_software_single_step);

  /* Alpha OSF/1 inhibits execution of code on the stack.  But there is
     no need for a dummy on the Alpha.  PUSH_ARGUMENTS takes care of all
     argument handling and bp_call_dummy takes care of stopping the dummy.  */
  set_gdbarch_call_dummy_address (gdbarch, alpha_call_dummy_address);

  tdep->sigcontext_addr = alpha_osf1_sigcontext_addr;

  tdep->jb_pc = 2;
  tdep->jb_elt_size = 8;
}

void
_initialize_alpha_osf1_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_alpha, 0, GDB_OSABI_OSF1,
			  alpha_osf1_init_abi);
}
