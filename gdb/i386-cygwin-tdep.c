/* Target-dependent code for Cygwin running on i386's, for GDB.
   Copyright 2003 Free Software Foundation, Inc.

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

#include "defs.h"

#include "gdb_string.h"
#include "gdbcore.h"
#include "i386-tdep.h"
#include "osabi.h"
#include "frame.h"
#include "dummy-frame.h"

static int
i386_cygwin_frame_chain_valid (CORE_ADDR chain, struct frame_info *thisframe)
{
  /* In the context where this is used, we get the saved PC before we've
     successfully unwound far enough to be sure what we've got (it may
     be a signal handler caller).  If we're dealing with a signal
     handler caller, this will return valid, which is fine.  If not,
     it'll make the correct test.  */
  return ((get_frame_type (thisframe) == SIGTRAMP_FRAME) || chain != 0);
}
/* Return the chain-pointer for FRAME.  In the case of the i386, the
   frame's nominal address is the address of a 4-byte word containing
   the calling frame's address.  */
static CORE_ADDR
i386_cygwin_frame_chain (struct frame_info *frame)
{
  if (pc_in_dummy_frame (get_frame_pc (frame)))
    return get_frame_base (frame);

  if (get_frame_type (frame) == SIGTRAMP_FRAME
      || i386_frameless_signal_p (frame))
    return get_frame_base (frame);

  return read_memory_unsigned_integer (get_frame_base (frame), 4);
}

static void
i386_cygwin_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  tdep->struct_return = reg_struct_return;
  set_gdbarch_deprecated_frame_chain (gdbarch, i386_cygwin_frame_chain);
  set_gdbarch_deprecated_frame_chain_valid (gdbarch, i386_cygwin_frame_chain_valid);
}

static enum gdb_osabi
i386_cygwin_osabi_sniffer (bfd * abfd)
{ 
  char *target_name = bfd_get_target (abfd);

  /* Interix also uses pei-i386. 
     We need a way to distinguish between the two. */
  if (strcmp (target_name, "pei-i386") == 0)
    return GDB_OSABI_CYGWIN;

  return GDB_OSABI_UNKNOWN;
}

void
_initialize_i386_cygwin_tdep (void)
{
  gdbarch_register_osabi_sniffer (bfd_arch_i386, bfd_target_coff_flavour,
                                  i386_cygwin_osabi_sniffer);

  gdbarch_register_osabi (bfd_arch_i386, 0, GDB_OSABI_CYGWIN,
                          i386_cygwin_init_abi);
}
