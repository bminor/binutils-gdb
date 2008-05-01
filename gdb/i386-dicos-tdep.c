/* Target-dependent code for DICOS running on i386's, for GDB.

   Copyright (C) 2008 Free Software Foundation, Inc.

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
#include "gdb_string.h"
#include "solib.h"
#include "solib-target.h"

static void
i386_dicos_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  set_solib_ops (gdbarch, &solib_target_so_ops);
}

/* Look in the elf symbol table of ABFD for a symbol named WANTED.
   Return true if found.  */
static int
i386_dicos_bfd_has_symbol_p (bfd *abfd, const char *wanted)
{
  long storage_needed;
  int ret = 0;
  asymbol **symbol_table = NULL;

  storage_needed = bfd_get_symtab_upper_bound (abfd);
  if (storage_needed < 0)
    {
      warning (_("Can't read elf symbols from %s: %s"), bfd_get_filename (abfd),
	       bfd_errmsg (bfd_get_error ()));
      return 0;
    }

  if (storage_needed > 0)
    {
      long i, symcount;

      symbol_table = xmalloc (storage_needed);
      symcount = bfd_canonicalize_symtab (abfd, symbol_table);

      if (symcount < 0)
	warning (_("Can't read elf symbols from %s: %s"),
		 bfd_get_filename (abfd),
		 bfd_errmsg (bfd_get_error ()));
      else
	{
	  for (i = 0; i < symcount; i++)
	    {
	      asymbol *sym = symbol_table[i];
	      if (sym->name != NULL
		  && wanted[0] == sym->name[0]
		  && strcmp (wanted + 1, sym->name + 1) == 0)
		{
		  ret = 1;
		  break;
		}
	    }
	}
    }

  xfree (symbol_table);
  return ret;
}

static enum gdb_osabi
i386_dicos_osabi_sniffer (bfd *abfd)
{
  char *target_name = bfd_get_target (abfd);

  /* DICOS debug info files don't have a .note.ABI-tag marker or
     something similar.  We do know there's always a "header" section
     of 36 bytes, and there's always a "Dicos_loadModuleInfo" symbol
     defined.  Look for the section first, as that should be
     cheaper.  */
  if (strcmp (target_name, "elf32-i386") == 0)
    {
      asection *section = bfd_get_section_by_name (abfd, "header");
      if (section
	  && bfd_section_size (abfd, section) == 36
	  && i386_dicos_bfd_has_symbol_p (abfd, "Dicos_loadModuleInfo"))
	return GDB_OSABI_DICOS;
    }

  return GDB_OSABI_UNKNOWN;
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
void _initialize_i386_dicos_tdep (void);

void
_initialize_i386_dicos_tdep (void)
{
  gdbarch_register_osabi_sniffer (bfd_arch_i386, bfd_target_elf_flavour,
                                  i386_dicos_osabi_sniffer);

  gdbarch_register_osabi (bfd_arch_i386, 0, GDB_OSABI_DICOS,
                          i386_dicos_init_abi);
}
