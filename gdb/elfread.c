/* Read ELF (Executable and Linking Format) object files for GDB.
   Copyright (C) 1991 Free Software Foundation, Inc.
   Written by Fred Fish at Cygnus Support.

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

/************************************************************************
 *									*
 *				NOTICE					*
 *									*
 * This file is still under construction.  When it is complete, this	*
 * notice will be removed.  Until then, direct any questions or changes	*
 * to Fred Fish at Cygnus Support (fnf@cygint)				*
 *									* 
 * FIXME	Still needs support for shared libraries.		*
 * FIXME	Still needs support for core files.			*
 * FIXME	The ".debug" and ".line" section names are hardwired.	*
 * FIXME	Still needs support ELF symbol tables (as distinct	*
 *		from DWARF support).  Can use them to build the misc	*
 *		function vector at least.  This is fairly trivial once	*
 *		bfd is extended to handle ELF symbol tables.		*
 *									*
 ************************************************************************/

#include <stdio.h>

#include "defs.h"
#include "param.h"
#include "elf-common.h"
#include "elf-external.h"
#include "elf-internal.h"
#include "bfd.h"
#include "symfile.h"
#include "symtab.h"
#include "ansidecl.h"

extern int EXFUN (strcmp, (CONST char *a, CONST char *b));
extern int EXFUN (dwarf_build_psymtabs,
     (int desc, char *filename, CORE_ADDR addr, int mainline,
      unsigned int dbfoff, unsigned int dbsize, unsigned int lnoffset,
      unsigned int lnsize, struct objfile *objfile));

#define STREQ(a,b) (strcmp((a),(b))==0)

struct elfinfo {
  unsigned int dboffset;	/* Offset to dwarf debug section */
  unsigned int dbsize;		/* Size of dwarf debug section */
  unsigned int lnoffset;	/* Offset to dwarf line number section */
  unsigned int lnsize;		/* Size of dwarf line number section */
};

#if 0
/* FIXME - crude hack to resolve undefined global.  This function is
   part of support for corefiles, which is not yet implemented. */

unsigned int
DEFUN(register_addr, (regno, blockend),
      int regno AND
      int blockend)
{
	error ("Fetching registers from corefiles unimplemented.");
}
#endif

/* We are called once per section from elf_symfile_read.  We
   need to examine each section we are passed, check to see
   if it is something we are interested in processing, and
   if so, stash away some access information for the section.

   For now we recognize the dwarf debug information sections and
   line number sections from matching their section names.  The
   ELF definition is no real help here since it has no direct
   knowledge of DWARF (by design, so any debugging format can be
   used).

   FIXME:  The section names should not be hardwired strings. */

static void
DEFUN(elf_locate_sections, (abfd, sectp, ei),
      bfd *abfd AND
      asection *sectp AND
      struct elfinfo *ei)
{
  if (STREQ (sectp -> name, ".debug"))
    {
      ei -> dboffset = sectp -> filepos;
      ei -> dbsize = sectp -> size;
    }
  else if (STREQ (sectp -> name, ".line"))
    {
      ei -> lnoffset = sectp -> filepos;
      ei -> lnsize = sectp -> size;
    }
}

/* Scan and build partial symbols for a symbol file.
   We have been initialized by a call to elf_symfile_init, which 
   currently does nothing.

   ADDR is the address relative to which the symbols in it are (e.g.
   the base address of the text segment).

   MAINLINE is true if we are reading the main symbol
   table (as opposed to a shared lib or dynamically loaded file).

   This function only does the minimum work necessary for letting the
   user "name" things symbolically; it does not read the entire symtab.
   Instead, it reads the external and static symbols and puts them in partial
   symbol tables.  When more extensive information is requested of a
   file, the corresponding partial symbol table is mutated into a full
   fledged symbol table by going back and reading the symbols
   for real.  The function dwarf_psymtab_to_symtab() is the function that
   does this for DWARF symbols. */

static void
DEFUN(elf_symfile_read, (sf, addr, mainline),
      struct sym_fns *sf AND
      CORE_ADDR addr AND
      int mainline)
{
  bfd *abfd = sf->objfile->obfd;
  struct elfinfo ei;

  bfd_map_over_sections (abfd, elf_locate_sections, &ei);
  if (ei.dboffset && ei.lnoffset)
    {
      addr = 0;	/* FIXME: force address base to zero for now */
      dwarf_build_psymtabs (fileno ((FILE *)(abfd -> iostream)),
			    bfd_get_filename (abfd),
			    addr, mainline,
			    ei.dboffset, ei.dbsize,
			    ei.lnoffset, ei.lnsize, sf->objfile);
    }
  if (!partial_symtab_list)
    {
      wrap_here ("");
      printf_filtered ("(no debugging symbols found)...");
      wrap_here ("");
    }
}

/* Initialize anything that needs initializing when a completely new symbol
   file is specified (not just adding some symbols from another file, e.g. a
   shared library).

   For now at least, we have nothing in particular to do, so this function is
   just a stub. */

static void
DEFUN_VOID (elf_new_init)
{
}

/* ELF specific initialization routine for reading symbols.

   It is passed a pointer to a struct sym_fns which contains, among other
   things, the BFD for the file whose symbols are being read, and a slot for
   a pointer to "private data" which we can fill with goodies.

   For now at least, we have nothing in particular to do, so this function is
   just a stub. */

static void
DEFUN(elf_symfile_init, (sf),
      struct sym_fns *sf)
{
}


/*  Register that we are able to handle ELF object file formats and DWARF
    debugging formats.

    Unlike other object file formats, where the debugging information format
    is implied by the object file format, the ELF object file format and the
    DWARF debugging information format are two distinct, and potentially
    separate entities.  I.E. it is perfectly possible to have ELF objects
    with debugging formats other than DWARF.  And it is conceivable that the
    DWARF debugging format might be used with another object file format,
    like COFF, by simply using COFF's custom section feature.

    GDB, and to a lesser extent BFD, should support the notion of separate
    object file formats and debugging information formats.  For now, we just
    use "elf" in the same sense as "a.out" or "coff", to imply both the ELF
    object file format and the DWARF debugging format. */

static struct sym_fns elf_sym_fns = {
  "elf",		/* sym_name: name or name prefix of BFD target type */
  3,			/* sym_namelen: number of significant sym_name chars */
  elf_new_init,		/* sym_new_init: init anything gbl to entire symtab */
  elf_symfile_init,	/* sym_init: read initial info, setup for sym_read() */
  elf_symfile_read,	/* sym_read: read a symbol file into symtab */
  NULL,			/* sym_bfd: accessor for symbol file being read */
  NULL,			/* sym_private: sym_init & sym_read shared info */
  NULL			/* next: pointer to next struct sym_fns */
};

void
DEFUN_VOID (_initialize_elfread)
{
  add_symtab_fns (&elf_sym_fns);
}
