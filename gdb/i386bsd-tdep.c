/* Target-dependent code for i386 BSD's.
   Copyright 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include "arch-utils.h"
#include "frame.h"
#include "gdbcore.h"
#include "regcache.h"
#include "osabi.h"

#include "gdb_string.h"

#include "i386-tdep.h"

/* Support for signal handlers.  */

/* Return whether PC is in a BSD sigtramp routine.  */

static int
i386bsd_pc_in_sigtramp (CORE_ADDR pc, char *name)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);

  return (pc >= tdep->sigtramp_start && pc < tdep->sigtramp_end);
}

/* Assuming NEXT_FRAME is for a frame following a BSD sigtramp
   routine, return the address of the associated sigcontext structure.  */

static CORE_ADDR
i386bsd_sigcontext_addr (struct frame_info *next_frame)
{
  char buf[4];
  CORE_ADDR sp;

  frame_unwind_register (next_frame, I386_ESP_REGNUM, buf);
  sp = extract_unsigned_integer (buf, 4);

  return read_memory_unsigned_integer (sp + 8, 4);
}

/* Return the start address of the sigtramp routine.  */

CORE_ADDR
i386bsd_sigtramp_start (CORE_ADDR pc)
{
  return gdbarch_tdep (current_gdbarch)->sigtramp_start;
}

/* Return the end address of the sigtramp routine.  */

CORE_ADDR
i386bsd_sigtramp_end (CORE_ADDR pc)
{
  return gdbarch_tdep (current_gdbarch)->sigtramp_end;
}


/* Support for shared libraries.  */

/* Return non-zero if we are in a shared library trampoline code stub.  */

int
i386bsd_aout_in_solib_call_trampoline (CORE_ADDR pc, char *name)
{
  return (name && !strcmp (name, "_DYNAMIC"));
}

/* Traditional BSD (4.3 BSD, still used for BSDI and 386BSD).  */

/* From <machine/signal.h>.  */
int i386bsd_sc_reg_offset[I386_NUM_GREGS] =
{
  -1,				/* %eax */
  -1,				/* %ecx */
  -1,				/* %edx */
  -1,				/* %ebx */
  8 + 0 * 4,			/* %esp */
  8 + 1 * 4,			/* %ebp */
  -1,				/* %esi */
  -1,				/* %edi */
  8 + 3 * 4,			/* %eip */
  8 + 4 * 4,			/* %eflags */
  -1,				/* %cs */
  -1,				/* %ss */
  -1,				/* %ds */
  -1,				/* %es */
  -1,				/* %fs */
  -1				/* %gs */
};

void
i386bsd_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  set_gdbarch_pc_in_sigtramp (gdbarch, i386bsd_pc_in_sigtramp);

  /* Allow the recognition of sigtramps as a function named <sigtramp>.  */
  set_gdbarch_sigtramp_start (gdbarch, i386bsd_sigtramp_start);
  set_gdbarch_sigtramp_end (gdbarch, i386bsd_sigtramp_end);

  /* Assume SunOS-style shared libraries.  */
  set_gdbarch_in_solib_call_trampoline (gdbarch,
					i386bsd_aout_in_solib_call_trampoline);

  tdep->jb_pc_offset = 0;

  tdep->sigtramp_start = 0xfdbfdfc0;
  tdep->sigtramp_end = 0xfdbfe000;
  tdep->sigcontext_addr = i386bsd_sigcontext_addr;
  tdep->sc_reg_offset = i386bsd_sc_reg_offset;
  tdep->sc_num_regs = I386_NUM_GREGS;
}

/* FreeBSD 3.0-RELEASE or later.  */

CORE_ADDR i386fbsd_sigtramp_start = 0xbfbfdf20;
CORE_ADDR i386fbsd_sigtramp_end = 0xbfbfdff0;

/* From <machine/signal.h>.  */
int i386fbsd_sc_reg_offset[I386_NUM_GREGS] =
{
  8 + 14 * 4,			/* %eax */
  8 + 13 * 4,			/* %ecx */
  8 + 12 * 4,			/* %edx */
  8 + 11 * 4,			/* %ebx */
  8 + 0 * 4,                    /* %esp */
  8 + 1 * 4,                    /* %ebp */
  8 + 10 * 4,                   /* %esi */
  8 + 9 * 4,                    /* %edi */
  8 + 3 * 4,                    /* %eip */
  8 + 4 * 4,                    /* %eflags */
  8 + 7 * 4,                    /* %cs */
  8 + 8 * 4,                    /* %ss */
  8 + 6 * 4,                    /* %ds */
  8 + 5 * 4,                    /* %es */
  8 + 15 * 4,			/* %fs */
  8 + 16 * 4			/* %gs */
};

static void
i386fbsdaout_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* Obviously FreeBSD is BSD-based.  */
  i386bsd_init_abi (info, gdbarch);

  /* FreeBSD uses -freg-struct-return by default.  */
  tdep->struct_return = reg_struct_return;

  /* FreeBSD uses a different memory layout.  */
  tdep->sigtramp_start = i386fbsd_sigtramp_start;
  tdep->sigtramp_end = i386fbsd_sigtramp_end;

  /* FreeBSD has a more complete `struct sigcontext'.  */
  tdep->sc_reg_offset = i386fbsd_sc_reg_offset;
  tdep->sc_num_regs = I386_NUM_GREGS;
}

static void
i386fbsd_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  /* It's almost identical to FreeBSD a.out.  */
  i386fbsdaout_init_abi (info, gdbarch);

  /* Except that it uses ELF.  */
  i386_elf_init_abi (info, gdbarch);

  /* FreeBSD ELF uses SVR4-style shared libraries.  */
  set_gdbarch_in_solib_call_trampoline (gdbarch,
					generic_in_solib_call_trampoline);
}

/* FreeBSD 4.0-RELEASE or later.  */

/* From <machine/signal.h>.  */
int i386fbsd4_sc_reg_offset[I386_NUM_GREGS] =
{
  20 + 11 * 4,			/* %eax */
  20 + 10 * 4,			/* %ecx */
  20 + 9 * 4,			/* %edx */
  20 + 8 * 4,			/* %ebx */
  20 + 17 * 4,			/* %esp */
  20 + 6 * 4,			/* %ebp */
  20 + 5 * 4,			/* %esi */
  20 + 4 * 4,			/* %edi */
  20 + 14 * 4,			/* %eip */
  20 + 16 * 4,			/* %eflags */
  20 + 15 * 4,			/* %cs */
  20 + 18 * 4,			/* %ss */
  20 + 3 * 4,			/* %ds */
  20 + 2 * 4,			/* %es */
  20 + 1 * 4,			/* %fs */
  20 + 0 * 4			/* %gs */
};

static void
i386fbsd4_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* Inherit stuff from older releases.  We assume that FreeBSD
     4.0-RELEASE always uses ELF.  */
  i386fbsd_init_abi (info, gdbarch);

  /* FreeBSD 4.0 introduced a new `struct sigcontext'.  */
  tdep->sc_reg_offset = i386fbsd4_sc_reg_offset;
  tdep->sc_num_regs = I386_NUM_GREGS;
}


static enum gdb_osabi
i386bsd_aout_osabi_sniffer (bfd *abfd)
{
  if (strcmp (bfd_get_target (abfd), "a.out-i386-netbsd") == 0)
    return GDB_OSABI_NETBSD_AOUT;

  if (strcmp (bfd_get_target (abfd), "a.out-i386-freebsd") == 0)
    return GDB_OSABI_FREEBSD_AOUT;

  return GDB_OSABI_UNKNOWN;
}


/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_i386bsd_tdep (void);

void
_initialize_i386bsd_tdep (void)
{
  gdbarch_register_osabi_sniffer (bfd_arch_i386, bfd_target_aout_flavour,
				  i386bsd_aout_osabi_sniffer);

  gdbarch_register_osabi (bfd_arch_i386, 0, GDB_OSABI_FREEBSD_AOUT,
			  i386fbsdaout_init_abi);
  gdbarch_register_osabi (bfd_arch_i386, 0, GDB_OSABI_FREEBSD_ELF,
			  i386fbsd4_init_abi);
}
