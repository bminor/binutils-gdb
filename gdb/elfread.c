/* Read ELF (Executable and Linking Format) object files for GDB.
   Copyright 1991, 1992 Free Software Foundation, Inc.
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
 * to Fred Fish at Cygnus Support (fnf@cygnus.com)			*
 *									* 
 * FIXME	Still needs support for shared libraries.		*
 * FIXME	Still needs support for core files.			*
 * FIXME	The ".debug" and ".line" section names are hardwired.	*
 *									*
 ************************************************************************/

#include "defs.h"
#include "elf/common.h"
#include "elf/external.h"
#include "elf/internal.h"
#include "bfd.h"
#include "symtab.h"
#include "symfile.h"
#include "objfiles.h"
#include "buildsym.h"

#define STREQ(a,b) (strcmp((a),(b))==0)

struct elfinfo {
  unsigned int dboffset;	/* Offset to dwarf debug section */
  unsigned int dbsize;		/* Size of dwarf debug section */
  unsigned int lnoffset;	/* Offset to dwarf line number section */
  unsigned int lnsize;		/* Size of dwarf line number section */
};

static void
elf_symfile_init PARAMS ((struct objfile *));

static void
elf_new_init PARAMS ((struct objfile *));

static void
elf_symfile_read PARAMS ((struct objfile *, CORE_ADDR, int));

static void
elf_symfile_finish PARAMS ((struct objfile *));

static void
elf_symtab_read PARAMS ((bfd *,  CORE_ADDR, struct objfile *));

static void
record_minimal_symbol PARAMS ((char *, CORE_ADDR, enum minimal_symbol_type,
			       struct objfile *));

static void
elf_locate_sections PARAMS ((bfd *, asection *, PTR));

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
elf_locate_sections (ignore_abfd, sectp, eip)
     bfd *ignore_abfd;
     asection *sectp;
     PTR eip;
{
  register struct elfinfo *ei;

  ei = (struct elfinfo *) eip;
  if (STREQ (sectp -> name, ".debug"))
    {
      ei -> dboffset = sectp -> filepos;
      ei -> dbsize = bfd_get_section_size_before_reloc (sectp);
    }
  else if (STREQ (sectp -> name, ".line"))
    {
      ei -> lnoffset = sectp -> filepos;
      ei -> lnsize = bfd_get_section_size_before_reloc (sectp);
    }
}

#if 0	/* Currently unused */

char *
elf_interpreter (abfd)
     bfd *abfd;
{
  sec_ptr interp_sec;
  unsigned size;
  char *interp = NULL;

  interp_sec = bfd_get_section_by_name (abfd, ".interp");
  if (interp_sec)
    {
      size = bfd_section_size (abfd, interp_sec);
      interp = alloca (size);
      if (bfd_get_section_contents (abfd, interp_sec, interp, (file_ptr)0,
				    size))
	{
	  interp = savestring (interp, size - 1);
	}
      else
	{
	  interp = NULL;
	}
    }
  return (interp);
}

#endif

/*

LOCAL FUNCTION

	record_minimal_symbol -- add entry to minimal symbol table

SYNOPSIS

	static void record_minimal_symbol (char *name, CORE_ADDR address)

DESCRIPTION

	Given a pointer to the name of a symbol that should be added to the
	minimal symbol table and the address associated with that symbol, records
	this information for later use in building the minimal symbol table.

 */

static void
record_minimal_symbol (name, address, ms_type, objfile)
     char *name;
     CORE_ADDR address;
     enum minimal_symbol_type ms_type;
     struct objfile *objfile;
{
  name = obsavestring (name, strlen (name), &objfile -> symbol_obstack);
  prim_record_minimal_symbol (name, address, ms_type);
}

/*

LOCAL FUNCTION

	elf_symtab_read -- read the symbol table of an ELF file

SYNOPSIS

	void elf_symtab_read (bfd *abfd, CORE_ADDR addr,
			      struct objfile *objfile)

DESCRIPTION

	Given an open bfd, a base address to relocate symbols to, and a
	flag that specifies whether or not this bfd is for an executable
	or not (may be shared library for example), add all the global
	function and data symbols to the minimal symbol table.

*/

static void
elf_symtab_read (abfd, addr, objfile)
     bfd *abfd;
     CORE_ADDR addr;
     struct objfile *objfile;
{
  unsigned int storage_needed;
  asymbol *sym;
  asymbol **symbol_table;
  unsigned int number_of_symbols;
  unsigned int i;
  struct cleanup *back_to;
  CORE_ADDR symaddr;
  enum minimal_symbol_type ms_type;
  
  storage_needed = get_symtab_upper_bound (abfd);

  if (storage_needed > 0)
    {
      symbol_table = (asymbol **) xmalloc (storage_needed);
      back_to = make_cleanup (free, symbol_table);
      number_of_symbols = bfd_canonicalize_symtab (abfd, symbol_table); 
  
      for (i = 0; i < number_of_symbols; i++)
	{
	  sym = *symbol_table++;
	  /* Select global/weak symbols that are defined in a specific section.
	     Note that bfd now puts abs symbols in their own section, so
	     all symbols we are interested in will have a section. */
	  if ((sym -> flags & (BSF_GLOBAL | BSF_WEAK))
	      && (sym -> section != NULL))
	    {
	      symaddr = sym -> value;
	      /* Relocate all non-absolute symbols by base address.  */
	      if (sym -> section != &bfd_abs_section)
		{
		  symaddr += addr;
		}
	      /* For non-absolute symbols, use the type of the section
		 they are relative to, to intuit text/data.  Bfd provides
		 no way of figuring this out for absolute symbols. */
	      if (sym -> section -> flags & SEC_CODE)
		{
		  ms_type = mst_text;
		}
	      else if (sym -> section -> flags & SEC_DATA)
		{
		  ms_type = mst_data;
		}
	      else
		{
		  ms_type = mst_unknown;
		}
	      record_minimal_symbol ((char *) sym -> name, symaddr, ms_type, objfile);
	    }
	}
      do_cleanups (back_to);
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
   does this for DWARF symbols.

   Note that ELF files have a "minimal" symbol table, which looks a lot
   like a COFF symbol table, but has only the minimal information necessary
   for linking.  We process this also, and just use the information to
   add to gdb's minimal symbol table.  This gives us some minimal debugging
   capability even for files compiled without -g.
 */

static void
elf_symfile_read (objfile, addr, mainline)
     struct objfile *objfile;
     CORE_ADDR addr;
     int mainline;
{
  bfd *abfd = objfile->obfd;
  struct elfinfo ei;
  struct cleanup *back_to;
  asection *text_sect;

  init_minimal_symbol_collection ();
  back_to = make_cleanup (discard_minimal_symbols, 0);

  /* Compute the amount to relocate all symbols by.  The value passed in
     as ADDR is typically either the actual address of the text section,
     or a user specified address.  By subtracting off the actual address
     of the text section, we can compute the relocation amount. */

  text_sect = bfd_get_section_by_name (objfile -> obfd, ".text");
  addr -= bfd_section_vma (objfile -> obfd, text_sect);

  /* Process the normal ELF symbol table first. */

  elf_symtab_read (abfd, addr, objfile);

  /* Now process the DWARF debugging information, which is contained in
     special ELF sections.  We first have to find them... */

  (void) memset ((char *) &ei, 0, sizeof (ei));
  bfd_map_over_sections (abfd, elf_locate_sections, (PTR) &ei);
  if (ei.dboffset && ei.lnoffset)
    {
      dwarf_build_psymtabs (fileno ((FILE *)(abfd -> iostream)),
			    bfd_get_filename (abfd),
			    addr, mainline,
			    ei.dboffset, ei.dbsize,
			    ei.lnoffset, ei.lnsize, objfile);
    }

  if (!have_partial_symbols ())
    {
      wrap_here ("");
      printf_filtered ("(no debugging symbols found)...");
      wrap_here ("");
    }

  /* Install any minimal symbols that have been collected as the current
     minimal symbols for this objfile. */

  install_minimal_symbols (objfile);

  do_cleanups (back_to);
}

/* Initialize anything that needs initializing when a completely new symbol
   file is specified (not just adding some symbols from another file, e.g. a
   shared library).

   For now at least, we have nothing in particular to do, so this function is
   just a stub. */

static void
elf_new_init (ignore)
     struct objfile *ignore;
{
  buildsym_new_init ();
}

/* Perform any local cleanups required when we are done with a particular
   objfile.  I.E, we are in the process of discarding all symbol information
   for an objfile, freeing up all memory held for it, and unlinking the
   objfile struct from the global list of known objfiles. */

static void
elf_symfile_finish (objfile)
     struct objfile *objfile;
{
  if (objfile -> sym_private != NULL)
    {
      mfree (objfile -> md, objfile -> sym_private);
    }
}

/* ELF specific initialization routine for reading symbols.

   It is passed a pointer to a struct sym_fns which contains, among other
   things, the BFD for the file whose symbols are being read, and a slot for
   a pointer to "private data" which we can fill with goodies.

   For now at least, we have nothing in particular to do, so this function is
   just a stub. */

static void
elf_symfile_init (ignore)
     struct objfile *ignore;
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

static struct sym_fns elf_sym_fns =
{
  "elf",		/* sym_name: name or name prefix of BFD target type */
  3,			/* sym_namelen: number of significant sym_name chars */
  elf_new_init,		/* sym_new_init: init anything gbl to entire symtab */
  elf_symfile_init,	/* sym_init: read initial info, setup for sym_read() */
  elf_symfile_read,	/* sym_read: read a symbol file into symtab */
  elf_symfile_finish,	/* sym_finish: finished with file, cleanup */
  NULL			/* next: pointer to next struct sym_fns */
};

void
_initialize_elfread ()
{
  add_symtab_fns (&elf_sym_fns);
}
