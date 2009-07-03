/* Darwin support for GDB, the GNU debugger.
   Copyright 1997, 1998, 1999, 2000, 2001, 2002, 2005, 2008, 2009
   Free Software Foundation, Inc.

   Contributed by Apple Computer, Inc.

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
#include "frame.h"
#include "inferior.h"
#include "gdbcore.h"
#include "target.h"
#include "floatformat.h"
#include "symtab.h"
#include "regcache.h"
#include "libbfd.h"
#include "objfiles.h"

#include "i387-tdep.h"
#include "i386-tdep.h"
#include "osabi.h"
#include "ui-out.h"
#include "symtab.h"
#include "frame.h"
#include "gdb_assert.h"
#include "i386-darwin-tdep.h"
#include "solib.h"
#include "solib-darwin.h"
#include "dwarf2-frame.h"

/* Offsets into the struct i386_thread_state where we'll find the saved regs.
   From <mach/i386/thread_status.h> and i386-tdep.h.  */
int i386_darwin_thread_state_reg_offset[] =
{
   0 * 4,   /* EAX */
   2 * 4,   /* ECX */
   3 * 4,   /* EDX */
   1 * 4,   /* EBX */
   7 * 4,   /* ESP */
   6 * 4,   /* EBP */
   5 * 4,   /* ESI */
   4 * 4,   /* EDI */
  10 * 4,   /* EIP */
   9 * 4,   /* EFLAGS */
  11 * 4,   /* CS */
   8,       /* SS */
  12 * 4,   /* DS */
  13 * 4,   /* ES */
  14 * 4,   /* FS */
  15 * 4    /* GS */
};

const int i386_darwin_thread_state_num_regs = 
  ARRAY_SIZE (i386_darwin_thread_state_reg_offset);

/* Assuming THIS_FRAME is a Darwin sigtramp routine, return the
   address of the associated sigcontext structure.  */

static CORE_ADDR
i386_darwin_sigcontext_addr (struct frame_info *this_frame)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  CORE_ADDR bp;
  CORE_ADDR si;
  gdb_byte buf[4];

  get_frame_register (this_frame, I386_EBP_REGNUM, buf);
  bp = extract_unsigned_integer (buf, 4, byte_order);

  /* A pointer to the ucontext is passed as the fourth argument
     to the signal handler.  */
  read_memory (bp + 24, buf, 4);
  si = extract_unsigned_integer (buf, 4, byte_order);

  /* The pointer to mcontext is at offset 28.  */
  read_memory (si + 28, buf, 4);

  /* First register (eax) is at offset 12.  */
  return extract_unsigned_integer (buf, 4, byte_order) + 12;
}

/* Return true if the PC of THIS_FRAME is in a signal trampoline which
   may have DWARF-2 CFI.

   On Darwin, signal trampolines have DWARF-2 CFI but it has only one FDE
   that covers only the indirect call to the user handler.
   Without this function, the frame is recognized as a normal frame which is
   not expected.  */

int
darwin_dwarf_signal_frame_p (struct gdbarch *gdbarch,
			     struct frame_info *this_frame)
{
  return i386_sigtramp_p (this_frame);
}

static void
i386_darwin_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* We support the SSE registers.  */
  tdep->num_xmm_regs = I386_NUM_XREGS - 1;
  set_gdbarch_num_regs (gdbarch, I386_SSE_NUM_REGS);

  dwarf2_frame_set_signal_frame_p (gdbarch, darwin_dwarf_signal_frame_p);

  tdep->struct_return = reg_struct_return;

  tdep->sigtramp_p = i386_sigtramp_p;
  tdep->sigcontext_addr = i386_darwin_sigcontext_addr;
  tdep->sc_reg_offset = i386_darwin_thread_state_reg_offset;
  tdep->sc_num_regs = i386_darwin_thread_state_num_regs;

  tdep->jb_pc_offset = 20;

  set_solib_ops (gdbarch, &darwin_so_ops);
}

static enum gdb_osabi
i386_mach_o_osabi_sniffer (bfd *abfd)
{
  if (!bfd_check_format (abfd, bfd_object))
    return GDB_OSABI_UNKNOWN;
  
  if (bfd_get_arch (abfd) == bfd_arch_i386)
    return GDB_OSABI_DARWIN;

  return GDB_OSABI_UNKNOWN;
}

void
_initialize_i386_darwin_tdep (void)
{
  gdbarch_register_osabi_sniffer (bfd_arch_unknown, bfd_target_mach_o_flavour,
                                  i386_mach_o_osabi_sniffer);

  gdbarch_register_osabi (bfd_arch_i386, bfd_mach_i386_i386,
			  GDB_OSABI_DARWIN, i386_darwin_init_abi);
}
