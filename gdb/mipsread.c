/* Read a symbol table in MIPS' format (Third-Eye).
   Copyright 1986, 1987, 1989, 1990, 1991, 1992, 1993 Free Software
   Foundation, Inc.
   Contributed by Alessandro Forin (af@cs.cmu.edu) at CMU.  Major work
   by Per Bothner, John Gilmore and Ian Lance Taylor at Cygnus Support.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Read symbols from an ECOFF file.  Most of the work is done in
   mdebugread.c.  */

#include "defs.h"
#include "symtab.h"
#include "symfile.h"
#include "objfiles.h"
#include "buildsym.h"
#include "stabsread.h"
#include "gdb-stabs.h"

#include "coff/internal.h"
#include "coff/ecoff.h"
#include "libcoff.h"		/* Private BFD COFF information.  */
#include "libecoff.h"		/* Private BFD ECOFF information.  */

static void
mipscoff_new_init PARAMS ((struct objfile *));

static void
mipscoff_symfile_init PARAMS ((struct objfile *));

static void
mipscoff_symfile_read PARAMS ((struct objfile *, struct section_offsets *,
			       int));

static void
mipscoff_symfile_finish PARAMS ((struct objfile *));

static struct section_offsets *
mipscoff_symfile_offsets PARAMS ((struct objfile *, CORE_ADDR));

/* Initialize anything that needs initializing when a completely new
   symbol file is specified (not just adding some symbols from another
   file, e.g. a shared library).  */

extern CORE_ADDR sigtramp_address;

static void
mipscoff_new_init (ignore)
     struct objfile *ignore;
{
  sigtramp_address = 0;
  stabsread_new_init ();
  buildsym_new_init ();
}

/* Initialize to read a symbol file (nothing to do).  */

static void
mipscoff_symfile_init (objfile)
     struct objfile *objfile;
{
}

/* Read a symbol file from a file.  */

static void
mipscoff_symfile_read (objfile, section_offsets, mainline)
     struct objfile *objfile;
     struct section_offsets *section_offsets;
     int mainline;
{
  bfd *abfd = objfile->obfd;
  struct cleanup * back_to;

  init_minimal_symbol_collection ();
  back_to = make_cleanup (discard_minimal_symbols, 0);

  /* Now that the executable file is positioned at symbol table,
     process it and define symbols accordingly.  */

  if (ecoff_slurp_symbolic_info (abfd) == false)
    error ("Error reading symbol table: %s", bfd_errmsg (bfd_error));

  mdebug_build_psymtabs (objfile, &ecoff_backend (abfd)->debug_swap,
			 &ecoff_data (abfd)->debug_info, section_offsets);

  /* Install any minimal symbols that have been collected as the current
     minimal symbols for this objfile. */

  install_minimal_symbols (objfile);

  do_cleanups (back_to);
}

/* Perform any local cleanups required when we are done with a
   particular objfile.  */

static void
mipscoff_symfile_finish (objfile)
     struct objfile *objfile;
{
}

/* Fake up identical offsets for all sections.  */

static struct section_offsets *
mipscoff_symfile_offsets (objfile, addr)
     struct objfile *objfile;
     CORE_ADDR addr;
{
  struct section_offsets *section_offsets;
  int i;

  objfile->num_sections = SECT_OFF_MAX;
  section_offsets = ((struct section_offsets *)
		     obstack_alloc (&objfile->psymbol_obstack,
				    (sizeof (struct section_offsets)
				     + (sizeof (section_offsets->offsets)
					* (SECT_OFF_MAX - 1)))));

  for (i = 0; i < SECT_OFF_MAX; i++)
    ANOFFSET (section_offsets, i) = addr;

  return section_offsets;
}

/* Initialization */

static struct sym_fns ecoff_sym_fns =
{
  bfd_target_ecoff_flavour,
  mipscoff_new_init,		/* sym_new_init: init anything gbl to entire symtab */
  mipscoff_symfile_init,	/* sym_init: read initial info, setup for sym_read() */
  mipscoff_symfile_read,	/* sym_read: read a symbol file into symtab */
  mipscoff_symfile_finish,	/* sym_finish: finished with file, cleanup */
  mipscoff_symfile_offsets,	/* sym_offsets: dummy FIXME til implem sym reloc */
  NULL				/* next: pointer to next struct sym_fns */
};

void
_initialize_mipsread ()
{
  add_symtab_fns (&ecoff_sym_fns);
}
