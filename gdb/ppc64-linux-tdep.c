/* Target-dependent code for GDB, the GNU debugger.

   Copyright 1986, 1987, 1989, 1991, 1992, 1993, 1994, 1995, 1996, 1997,
   2000, 2001 Free Software Foundation, Inc.

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


/* These elfcore fuctions are defined in libbfd.a but only when host
   == target, once we can convince the bfd to supply it these can go */
#define _SYSCALL32
#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"
#include "libbfd.h"
#define ARCH_SIZE 0
#include "elf-bfd.h"
#include "libiberty.h"
#include <sys/procfs.h>


/*
 * Initialization
 */
void
_initialize_ppc64_linux_tdep (void)
{
  /* hardware/kernel supports single stepping */
  set_gdbarch_software_single_step (current_gdbarch, NULL);
}

/* the start_address stored in the bfd is a function descriptor */
#include "gdb/target.h"
CORE_ADDR
ppc64_bfd_get_start_address (bfd *abfd)
{
  extern struct target_ops exec_ops;
  CORE_ADDR myaddr;
  xfer_memory(abfd->start_address, (char *)&myaddr, 8, 0, 0, &exec_ops);
  return myaddr;
}

/* Fetch (and possibly build) an appropriate link_map_offsets
   structure for GNU/Linux PPC targets using the struct offsets
   defined in link.h (but without actual reference to that file).

   This makes it possible to access GNU/Linux PPC shared libraries
   from a GDB that was not built on an GNU/Linux PPC host (for cross
   debugging).

*/
#include "gdbcore.h"
#include "symfile.h"
#include "objfiles.h"

#include "ppc-tdep.h"
#define TDEP	gdbarch_tdep (current_gdbarch)

static CORE_ADDR
read_memory_addr (CORE_ADDR memaddr, int len)
{
  return read_memory_unsigned_integer (memaddr, len);
}
