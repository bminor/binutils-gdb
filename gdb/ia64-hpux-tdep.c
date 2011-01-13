/* Target-dependent code for the IA-64 for GDB, the GNU debugger.

   Copyright (C) 2010 Free Software Foundation, Inc.

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
#include "ia64-tdep.h"
#include "ia64-hpux-tdep.h"
#include "osabi.h"
#include "gdbtypes.h"
#include "solib.h"
#include "target.h"
#include "frame.h"

/* The offset to be used in order to get the __reason pseudo-register
   when using one of the *UREGS ttrace requests (see system header file
   /usr/include/ia64/sys/uregs.h for more details).

   The documentation for this pseudo-register says that a nonzero value
   indicates that the thread stopped due to a fault, trap, or interrupt.
   A null value indicates a stop inside a syscall.  */
#define IA64_HPUX_UREG_REASON 0x00070000

/* Return nonzero if the value of the register identified by REGNUM
   can be modified.  */

static int
ia64_hpux_can_store_ar_register (int regnum)
{
  switch (regnum)
    {
      case IA64_RSC_REGNUM:
      case IA64_RNAT_REGNUM:
      case IA64_CSD_REGNUM:
      case IA64_SSD_REGNUM:
      case IA64_CCV_REGNUM:
      case IA64_UNAT_REGNUM:
      case IA64_FPSR_REGNUM:
      case IA64_PFS_REGNUM:
      case IA64_LC_REGNUM:
      case IA64_EC_REGNUM:
         return 1;
         break;

      default:
         return 0;
         break;
    }
}

/* The "cannot_store_register" target_ops method.  */

static int
ia64_hpux_cannot_store_register (struct gdbarch *gdbarch, int regnum)
{
  /* General registers.  */

  if (regnum == IA64_GR0_REGNUM)
    return 1;

  /* FP register.  */

  if (regnum == IA64_FR0_REGNUM || regnum == IA64_FR1_REGNUM)
    return 1;

  /* Application registers.  */
  if (regnum >= IA64_AR0_REGNUM && regnum <= IA64_AR0_REGNUM + 127)
    return (!ia64_hpux_can_store_ar_register (regnum));

  /* We can store all other registers.  */
  return 0;
}

/* Return nonzero if the inferior is stopped inside a system call.  */

static int
ia64_hpux_stopped_in_syscall (struct gdbarch *gdbarch)
{
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  struct target_ops *ops = &current_target;
  gdb_byte buf[8];
  int len;

  len = target_read (ops, TARGET_OBJECT_HPUX_UREGS, NULL,
		     buf, IA64_HPUX_UREG_REASON, sizeof (buf));
  if (len == -1)
    /* The target wasn't able to tell us.  Assume we are not stopped
       in a system call, which is the normal situation.  */
    return 0;
  gdb_assert (len == 8);

  return (extract_unsigned_integer (buf, len, byte_order) == 0);
}

/* The "size_of_register_frame" gdbarch_tdep routine for ia64-hpux.  */

static int
ia64_hpux_size_of_register_frame (struct frame_info *this_frame,
				  ULONGEST cfm)
{
  int sof;

  if (frame_relative_level (this_frame) == 0
      && ia64_hpux_stopped_in_syscall (get_frame_arch (this_frame)))
    /* If the inferior stopped in a system call, the base address
       of the register frame is at BSP - SOL instead of BSP - SOF.
       This is an HP-UX exception.  */
    sof = (cfm & 0x3f80) >> 7;
  else
    sof = (cfm & 0x7f);

  return sof;
}

/* Should be set to non-NULL if the ia64-hpux solib module is linked in.
   This may not be the case because the shared library support code can
   only be compiled on ia64-hpux.  */

struct target_so_ops *ia64_hpux_so_ops = NULL;

static void
ia64_hpux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  tdep->size_of_register_frame = ia64_hpux_size_of_register_frame;

  set_gdbarch_long_double_format (gdbarch, floatformats_ia64_quad);
  set_gdbarch_cannot_store_register (gdbarch, ia64_hpux_cannot_store_register);

  if (ia64_hpux_so_ops)
    set_solib_ops (gdbarch, ia64_hpux_so_ops);
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern initialize_file_ftype _initialize_ia64_hpux_tdep;

void
_initialize_ia64_hpux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_ia64, 0, GDB_OSABI_HPUX_ELF,
			  ia64_hpux_init_abi);
}
