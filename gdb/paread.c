/* Read HP PA/Risc object files for GDB.
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

#include "defs.h"
#include "bfd.h"
#include <time.h> /* For time_t in libbfd.h.  */
#include <sys/types.h> /* For time_t, if not in time.h.  */
#include "libbfd.h"
#include "som.h"
#include <syms.h>
#include "symtab.h"
#include "symfile.h"
#include "objfiles.h"
#include "buildsym.h"
#include "stabsread.h"
#include "gdb-stabs.h"
#include "complaints.h"
#include <string.h>
#include "demangle.h"
#include <sys/file.h>

/* Size of n_value and n_strx fields in a stab symbol.  */
#define BYTES_IN_WORD 4

#include "aout/aout64.h"

/* Various things we might complain about... */

static void
pa_symfile_init PARAMS ((struct objfile *));

static void
pa_new_init PARAMS ((struct objfile *));

static void
read_unwind_info PARAMS ((struct objfile *));

static void
pa_symfile_read PARAMS ((struct objfile *, struct section_offsets *, int));

static void
pa_symfile_finish PARAMS ((struct objfile *));

static void
pa_symtab_read PARAMS ((bfd *,  CORE_ADDR, struct objfile *));

static void
free_painfo PARAMS ((PTR));

static struct section_offsets *
pa_symfile_offsets PARAMS ((struct objfile *, CORE_ADDR));

static void
record_minimal_symbol PARAMS ((char *, CORE_ADDR,
			       enum minimal_symbol_type,
			       struct objfile *));

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

	pa_symtab_read -- read the symbol table of a PA file

SYNOPSIS

	void pa_symtab_read (bfd *abfd, CORE_ADDR addr,
			      struct objfile *objfile)

DESCRIPTION

	Given an open bfd, a base address to relocate symbols to, and a
	flag that specifies whether or not this bfd is for an executable
	or not (may be shared library for example), add all the global
	function and data symbols to the minimal symbol table.
*/

static void
pa_symtab_read (abfd, addr, objfile)
     bfd *abfd;
     CORE_ADDR addr;
     struct objfile *objfile;
{
  unsigned int number_of_symbols;
  unsigned int i;
  int val;
  char *stringtab;
  struct symbol_dictionary_record *buf, *bufp, *endbufp;
  char *symname;
  CONST int symsize = sizeof (struct symbol_dictionary_record);

  number_of_symbols = bfd_get_symcount (abfd);

  buf = alloca (symsize * number_of_symbols);
  bfd_seek (abfd, obj_sym_filepos (abfd), L_SET);
  val = bfd_read (buf, symsize * number_of_symbols, 1, abfd);
  if (val != symsize * number_of_symbols)
    error ("Couldn't read symbol dictionary!");

  stringtab = alloca (obj_stringtab_size (abfd));
  bfd_seek (abfd, obj_str_filepos (abfd), L_SET);
  val = bfd_read (stringtab, obj_stringtab_size (abfd), 1, abfd);
  if (val != obj_stringtab_size (abfd))
    error ("Can't read in HP string table.");

  endbufp = buf + number_of_symbols;
  for (bufp = buf; bufp < endbufp; ++bufp)
    {
      enum minimal_symbol_type ms_type;

      QUIT;

      switch (bufp->symbol_scope)
	{
	case SS_UNIVERSAL:
	  switch (bufp->symbol_type)
	    {
	    case ST_SYM_EXT:
	    case ST_ARG_EXT:
	      continue;

	    case ST_CODE:
	    case ST_PRI_PROG:
	    case ST_SEC_PROG:
	    case ST_ENTRY:
	    case ST_MILLICODE:
	      symname = bufp->name.n_strx + stringtab;
	      ms_type = mst_text;
	      bufp->symbol_value &= ~0x3; /* clear out permission bits */
	      break;
	    case ST_DATA:
	      symname = bufp->name.n_strx + stringtab;
	      ms_type = mst_data;
	      break;
	    default:
	      continue;
	    }
	  break;

#if 0
	  /* SS_GLOBAL and SS_LOCAL are two names for the same thing (!).  */
	case SS_GLOBAL:
#endif
	case SS_LOCAL:
	  switch (bufp->symbol_type)
	    {
	    case ST_SYM_EXT:
	    case ST_ARG_EXT:
	      continue;

	    case ST_CODE:
	      symname = bufp->name.n_strx + stringtab;
	      ms_type = mst_file_text;
	      bufp->symbol_value &= ~0x3; /* clear out permission bits */

	    check_strange_names:
	      /* GAS leaves symbols with the prefixes "LS$", "LBB$",
		 and "LBE$" in .o files after assembling.  And thus
		 they appear in the final executable.  This can
		 cause problems if these special symbols have the
		 same value as real symbols.  So ignore them.  Also "LC$".  */
	      if (*symname == 'L'
		  && (symname[2] == '$' || symname[3] == '$'))
		continue;
	      break;

	    case ST_PRI_PROG:
	    case ST_SEC_PROG:
	    case ST_ENTRY:
	    case ST_MILLICODE:
	      symname = bufp->name.n_strx + stringtab;
	      ms_type = mst_file_text;
	      bufp->symbol_value &= ~0x3; /* clear out permission bits */
	      break;

	    case ST_DATA:
	      symname = bufp->name.n_strx + stringtab;
	      ms_type = mst_file_data;
	      goto check_strange_names;

	    default:
	      continue;
	    }
	  break;

	default:
	  continue;
	}

      if (bufp->name.n_strx > obj_stringtab_size (abfd))
	error ("Invalid symbol data; bad HP string table offset: %d",
	       bufp->name.n_strx);

      record_minimal_symbol (symname,
			     bufp->symbol_value, ms_type, 
			     objfile);
    }

  install_minimal_symbols (objfile);
}

/* Read in the backtrace information stored in the `$UNWIND_START$' section of
   the object file.  This info is used mainly by find_unwind_entry() to find
   out the stack frame size and frame pointer used by procedures.  We put
   everything on the psymbol obstack in the objfile so that it automatically
   gets freed when the objfile is destroyed.  */

static void
read_unwind_info (objfile)
     struct objfile *objfile;
{
  asection *unwind_sec;
  struct obj_unwind_info *ui;

  ui = obstack_alloc (&objfile->psymbol_obstack,
		      sizeof (struct obj_unwind_info));

  ui->table = NULL;
  ui->cache = NULL;
  ui->last = -1;

  unwind_sec = bfd_get_section_by_name (objfile->obfd,
					"$UNWIND_START$");
  if (unwind_sec)
    {
      int size;
      int i, *ip;

      size = bfd_section_size (objfile->obfd, unwind_sec);
      ui->table = obstack_alloc (&objfile->psymbol_obstack, size);
      ui->last = size / sizeof (struct unwind_table_entry) - 1;

      bfd_get_section_contents (objfile->obfd, unwind_sec, ui->table,
				0, size);

      OBJ_UNWIND_INFO (objfile) = ui;
    }
}

/* Scan and build partial symbols for a symbol file.
   We have been initialized by a call to pa_symfile_init, which 
   currently does nothing.

   SECTION_OFFSETS is a set of offsets to apply to relocate the symbols
   in each section.  This is ignored, as it isn't needed for the PA.

   MAINLINE is true if we are reading the main symbol
   table (as opposed to a shared lib or dynamically loaded file).

   This function only does the minimum work necessary for letting the
   user "name" things symbolically; it does not read the entire symtab.
   Instead, it reads the external and static symbols and puts them in partial
   symbol tables.  When more extensive information is requested of a
   file, the corresponding partial symbol table is mutated into a full
   fledged symbol table by going back and reading the symbols
   for real.

   We look for sections with specific names, to tell us what debug
   format to look for:  FIXME!!!

   pastab_build_psymtabs() handles STABS symbols.

   Note that PA files have a "minimal" symbol table, which is vaguely
   reminiscent of a COFF symbol table, but has only the minimal information
   necessary for linking.  We process this also, and use the information to
   build gdb's minimal symbol table.  This gives us some minimal debugging
   capability even for files compiled without -g.  */

static void
pa_symfile_read (objfile, section_offsets, mainline)
     struct objfile *objfile;
     struct section_offsets *section_offsets;
     int mainline;
{
  bfd *abfd = objfile->obfd;
  struct cleanup *back_to;
  CORE_ADDR offset;

  init_minimal_symbol_collection ();
  back_to = make_cleanup (discard_minimal_symbols, 0);

  make_cleanup (free_painfo, (PTR) objfile);

  /* Process the normal PA symbol table first. */

  /* FIXME, should take a section_offsets param, not just an offset.  */

  offset = ANOFFSET (section_offsets, 0);
  pa_symtab_read (abfd, offset, objfile);

  /* Now process debugging information, which is contained in
     special PA sections.  */

  pastab_build_psymtabs (objfile, section_offsets, mainline);

  read_unwind_info(objfile);

  do_cleanups (back_to);
}

/* This cleans up the objfile's sym_stab_info pointer, and the chain of
   stab_section_info's, that might be dangling from it.  */

static void
free_painfo (objp)
     PTR objp;
{
  struct objfile *objfile = (struct objfile *)objp;
  struct dbx_symfile_info *dbxinfo = (struct dbx_symfile_info *)
				     objfile->sym_stab_info;
  struct stab_section_info *ssi, *nssi;

  ssi = dbxinfo->stab_section_info;
  while (ssi)
    {
      nssi = ssi->next;
      mfree (objfile->md, ssi);
      ssi = nssi;
    }

  dbxinfo->stab_section_info = 0;	/* Just say No mo info about this.  */
}

/* Initialize anything that needs initializing when a completely new symbol
   file is specified (not just adding some symbols from another file, e.g. a
   shared library).

   We reinitialize buildsym, since we may be reading stabs from a PA file.  */

static void
pa_new_init (ignore)
     struct objfile *ignore;
{
  stabsread_new_init ();
  buildsym_new_init ();
}

/* Perform any local cleanups required when we are done with a particular
   objfile.  I.E, we are in the process of discarding all symbol information
   for an objfile, freeing up all memory held for it, and unlinking the
   objfile struct from the global list of known objfiles. */

static void
pa_symfile_finish (objfile)
     struct objfile *objfile;
{
  if (objfile -> sym_stab_info != NULL)
    {
      mfree (objfile -> md, objfile -> sym_stab_info);
    }
}

/* PA specific initialization routine for reading symbols.

   It is passed a pointer to a struct sym_fns which contains, among other
   things, the BFD for the file whose symbols are being read, and a slot for
   a pointer to "private data" which we can fill with goodies.

   This routine is almost a complete ripoff of dbx_symfile_init.  The
   common parts of these routines should be extracted and used instead of
   duplicating this code.  FIXME. */

static void
pa_symfile_init (objfile)
     struct objfile *objfile;
{
  int val;
  bfd *sym_bfd = objfile->obfd;
  char *name = bfd_get_filename (sym_bfd);
  asection *stabsect;		/* Section containing symbol table entries */
  asection *stringsect;		/* Section containing symbol name strings */

  stabsect = bfd_get_section_by_name (sym_bfd, "$GDB_SYMBOLS$");
  stringsect = bfd_get_section_by_name (sym_bfd, "$GDB_STRINGS$");

  /* Allocate struct to keep track of the symfile */
  objfile->sym_stab_info = (PTR)
    xmmalloc (objfile -> md, sizeof (struct dbx_symfile_info));

  memset ((PTR) objfile->sym_stab_info, 0, sizeof (struct dbx_symfile_info));

  if (!stabsect)
    return;

  if (!stringsect)
    error ("Found stabs, but not string section");

  /* FIXME POKING INSIDE BFD DATA STRUCTURES */
#define	STRING_TABLE_OFFSET	(stringsect->filepos)
#define	SYMBOL_TABLE_OFFSET	(stabsect->filepos)

  /* FIXME POKING INSIDE BFD DATA STRUCTURES */

  DBX_SYMFILE_INFO (objfile)->stab_section_info = NULL;
  DBX_TEXT_SECT (objfile) = bfd_get_section_by_name (sym_bfd, ".text");
  if (!DBX_TEXT_SECT (objfile))
    error ("Can't find .text section in symbol file");

  /* FIXME: I suspect this should be external_nlist.  The size of host
     types like long and bfd_vma should not affect how we read the
     file.  */
  DBX_SYMBOL_SIZE (objfile) = sizeof (struct internal_nlist);
  DBX_SYMCOUNT (objfile) = bfd_section_size (sym_bfd, stabsect)
    / DBX_SYMBOL_SIZE (objfile);
  DBX_SYMTAB_OFFSET (objfile) = SYMBOL_TABLE_OFFSET;

  /* Read the string table and stash it away in the psymbol_obstack.  It is
     only needed as long as we need to expand psymbols into full symbols,
     so when we blow away the psymbol the string table goes away as well.
     Note that gdb used to use the results of attempting to malloc the
     string table, based on the size it read, as a form of sanity check
     for botched byte swapping, on the theory that a byte swapped string
     table size would be so totally bogus that the malloc would fail.  Now
     that we put in on the psymbol_obstack, we can't do this since gdb gets
     a fatal error (out of virtual memory) if the size is bogus.  We can
     however at least check to see if the size is zero or some negative
     value. */

  DBX_STRINGTAB_SIZE (objfile) = bfd_section_size (sym_bfd, stringsect);

  if (DBX_SYMCOUNT (objfile) == 0
      || DBX_STRINGTAB_SIZE (objfile) == 0)
    return;

  if (DBX_STRINGTAB_SIZE (objfile) <= 0
      || DBX_STRINGTAB_SIZE (objfile) > bfd_get_size (sym_bfd))
    error ("ridiculous string table size (%d bytes).",
	   DBX_STRINGTAB_SIZE (objfile));

  DBX_STRINGTAB (objfile) =
    (char *) obstack_alloc (&objfile -> psymbol_obstack,
			    DBX_STRINGTAB_SIZE (objfile));

  /* Now read in the string table in one big gulp.  */

  val = bfd_seek (sym_bfd, STRING_TABLE_OFFSET, L_SET);
  if (val < 0)
    perror_with_name (name);
  val = bfd_read (DBX_STRINGTAB (objfile), DBX_STRINGTAB_SIZE (objfile), 1,
		  sym_bfd);
  if (val == 0)
    error ("End of file reading string table");
  else if (val < 0)
    /* It's possible bfd_read should be setting bfd_error, and we should be
       checking that.  But currently it doesn't set bfd_error.  */
    perror_with_name (name);
  else if (val != DBX_STRINGTAB_SIZE (objfile))
    error ("Short read reading string table");
}

/* PA specific parsing routine for section offsets.

   Plain and simple for now.  */

static struct section_offsets *
pa_symfile_offsets (objfile, addr)
     struct objfile *objfile;
     CORE_ADDR addr;
{
  struct section_offsets *section_offsets;
  int i;
 
  section_offsets = (struct section_offsets *)
    obstack_alloc (&objfile -> psymbol_obstack,
		   sizeof (struct section_offsets) +
		          sizeof (section_offsets->offsets) * (SECT_OFF_MAX-1));

  for (i = 0; i < SECT_OFF_MAX; i++)
    ANOFFSET (section_offsets, i) = addr;
  
  return section_offsets;
}

/*  Register that we are able to handle PA object file formats. */

/* This is probably a mistake.  FIXME.  Why can't the HP's use an ordinary
   file format name with an -hppa suffix?  */
static struct sym_fns pa_sym_fns =
{
  "hppa",		/* sym_name: name or name prefix of BFD target type */
  4,			/* sym_namelen: number of significant sym_name chars */
  pa_new_init,		/* sym_new_init: init anything gbl to entire symtab */
  pa_symfile_init,	/* sym_init: read initial info, setup for sym_read() */
  pa_symfile_read,	/* sym_read: read a symbol file into symtab */
  pa_symfile_finish,	/* sym_finish: finished with file, cleanup */
  pa_symfile_offsets,	/* sym_offsets:  Translate ext. to int. relocation */
  NULL			/* next: pointer to next struct sym_fns */
};

void
_initialize_paread ()
{
  add_symtab_fns (&pa_sym_fns);
}
