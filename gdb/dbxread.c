/* Read dbx symbol tables and convert to internal format, for GDB.
   Copyright (C) 1986-2024 Free Software Foundation, Inc.

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

/* This module provides three functions: dbx_symfile_init,
   which initializes to read a symbol file; dbx_new_init, which 
   discards existing cached information when all symbols are being
   discarded; and dbx_symfile_read, which reads a symbol table
   from a file.

   dbx_symfile_read only does the minimum work necessary for letting the
   user "name" things symbolically; it does not read the entire symtab.
   Instead, it reads the external and static symbols and puts them in partial
   symbol tables.  When more extensive information is requested of a
   file, the corresponding partial symbol table is mutated into a full
   fledged symbol table by going back and reading the symbols
   for real.  dbx_psymtab_to_symtab() is the function that does this */


#include "event-top.h"
#include "gdbsupport/gdb_obstack.h"
#include <sys/stat.h>
#include "symtab.h"
#include "breakpoint.h"
#include "target.h"
#include "gdbcore.h"
#include "libaout.h"
#include "filenames.h"
#include "objfiles.h"
#include "buildsym-legacy.h"
#include "stabsread.h"
#include "gdb-stabs.h"
#include "demangle.h"
#include "complaints.h"
#include "cp-abi.h"
#include "cp-support.h"
#include "c-lang.h"
#include "psymtab.h"
#include "block.h"
#include "aout/aout64.h"
#include "aout/stab_gnu.h"

/* Required for the following registry.  */
#include "gdb-stabs.h"



/* find_text_range --- find start and end of loadable code sections

   The find_text_range function finds the shortest address range that
   encloses all sections containing executable code, and stores it in
   objfile's text_addr and text_size members.

   dbx_symfile_read will use this to finish off the partial symbol
   table, in some cases.  */

static void
find_text_range (bfd * sym_bfd, struct objfile *objfile)
{
  asection *sec;
  int found_any = 0;
  CORE_ADDR start = 0;
  CORE_ADDR end = 0;

  for (sec = sym_bfd->sections; sec; sec = sec->next)
    if (bfd_section_flags (sec) & SEC_CODE)
      {
	CORE_ADDR sec_start = bfd_section_vma (sec);
	CORE_ADDR sec_end = sec_start + bfd_section_size (sec);

	if (found_any)
	  {
	    if (sec_start < start)
	      start = sec_start;
	    if (sec_end > end)
	      end = sec_end;
	  }
	else
	  {
	    start = sec_start;
	    end = sec_end;
	  }

	found_any = 1;
      }

  if (!found_any)
    error (_("Can't find any code sections in symbol file"));

  DBX_TEXT_ADDR (objfile) = start;
  DBX_TEXT_SIZE (objfile) = end - start;
}



/* Local function prototypes.  */

static void dbx_symfile_init (struct objfile *);

static void dbx_new_init (struct objfile *);

static void dbx_symfile_read (struct objfile *, symfile_add_flags);

static void dbx_symfile_finish (struct objfile *);


#if 0
static struct type **
explicit_lookup_type (int real_filenum, int index)
{
  struct header_file *f = &HEADER_FILES (dbxread_objfile)[real_filenum];

  if (index >= f->length)
    {
      f->length *= 2;
      f->vector = (struct type **)
	xrealloc (f->vector, f->length * sizeof (struct type *));
      memset (&f->vector[f->length / 2],
	      '\0', f->length * sizeof (struct type *) / 2);
    }
  return &f->vector[index];
}
#endif

/* Scan and build partial symbols for a symbol file.
   We have been initialized by a call to dbx_symfile_init, which 
   put all the relevant info into a "struct dbx_symfile_info",
   hung off the objfile structure.  */

static void
dbx_symfile_read (struct objfile *objfile, symfile_add_flags symfile_flags)
{
  bfd *sym_bfd;
  int val;
  struct dbx_symfile_info *key = dbx_objfile_data_key.get (objfile);

  sym_bfd = objfile->obfd.get ();

  /* .o and .nlm files are relocatables with text, data and bss segs based at
     0.  This flag disables special (Solaris stabs-in-elf only) fixups for
     symbols with a value of 0.  */

  key->ctx.symfile_relocatable = bfd_get_file_flags (sym_bfd) & HAS_RELOC;

  val = bfd_seek (sym_bfd, DBX_SYMTAB_OFFSET (objfile), SEEK_SET);
  if (val < 0)
    perror_with_name (objfile_name (objfile));

  key->ctx.symbol_size = DBX_SYMBOL_SIZE (objfile);
  key->ctx.symbol_table_offset = DBX_SYMTAB_OFFSET (objfile);

  scoped_free_pendings free_pending;

  minimal_symbol_reader reader (objfile);

  /* Read stabs data from executable file and define symbols.  */

  psymbol_functions *psf = new psymbol_functions ();
  psymtab_storage *partial_symtabs = psf->get_partial_symtabs ().get ();
  objfile->qf.emplace_front (psf);
  read_stabs_symtab (reader, partial_symtabs, objfile);

  /* Install any minimal symbols that have been collected as the current
     minimal symbols for this objfile.  */

  reader.install ();
}

/* Initialize anything that needs initializing when a completely new
   symbol file is specified (not just adding some symbols from another
   file, e.g. a shared library).  */

static void
dbx_new_init (struct objfile *ignore)
{
  stabsread_new_init ();
  init_header_files ();
}


/* dbx_symfile_init ()
   is the dbx-specific initialization routine for reading symbols.
   It is passed a struct objfile which contains, among other things,
   the BFD for the file whose symbols are being read, and a slot for a pointer
   to "private data" which we fill with goodies.

   We read the string table into malloc'd space and stash a pointer to it.

   Since BFD doesn't know how to read debug symbols in a format-independent
   way (and may never do so...), we have to do it ourselves.  We will never
   be called unless this is an a.out (or very similar) file.
   FIXME, there should be a cleaner peephole into the BFD environment here.  */

#define DBX_STRINGTAB_SIZE_SIZE sizeof(long)	/* FIXME */

static void
dbx_symfile_init (struct objfile *objfile)
{
  int val;
  bfd *sym_bfd = objfile->obfd.get ();
  const char *name = bfd_get_filename (sym_bfd);
  asection *text_sect;
  unsigned char size_temp[DBX_STRINGTAB_SIZE_SIZE];

  /* Allocate struct to keep track of the symfile.  */
  dbx_objfile_data_key.emplace (objfile);

  DBX_TEXT_SECTION (objfile) = bfd_get_section_by_name (sym_bfd, ".text");
  DBX_DATA_SECTION (objfile) = bfd_get_section_by_name (sym_bfd, ".data");
  DBX_BSS_SECTION (objfile) = bfd_get_section_by_name (sym_bfd, ".bss");

  /* FIXME POKING INSIDE BFD DATA STRUCTURES.  */
#define	STRING_TABLE_OFFSET	(sym_bfd->origin + obj_str_filepos (sym_bfd))
#define	SYMBOL_TABLE_OFFSET	(sym_bfd->origin + obj_sym_filepos (sym_bfd))

  /* FIXME POKING INSIDE BFD DATA STRUCTURES.  */

  text_sect = bfd_get_section_by_name (sym_bfd, ".text");
  if (!text_sect)
    error (_("Can't find .text section in symbol file"));
  DBX_TEXT_ADDR (objfile) = bfd_section_vma (text_sect);
  DBX_TEXT_SIZE (objfile) = bfd_section_size (text_sect);

  DBX_SYMBOL_SIZE (objfile) = obj_symbol_entry_size (sym_bfd);
  DBX_SYMCOUNT (objfile) = bfd_get_symcount (sym_bfd);
  DBX_SYMTAB_OFFSET (objfile) = SYMBOL_TABLE_OFFSET;

  /* Read the string table and stash it away in the objfile_obstack.
     When we blow away the objfile the string table goes away as well.
     Note that gdb used to use the results of attempting to malloc the
     string table, based on the size it read, as a form of sanity check
     for botched byte swapping, on the theory that a byte swapped string
     table size would be so totally bogus that the malloc would fail.  Now
     that we put in on the objfile_obstack, we can't do this since gdb gets
     a fatal error (out of virtual memory) if the size is bogus.  We can
     however at least check to see if the size is less than the size of
     the size field itself, or larger than the size of the entire file.
     Note that all valid string tables have a size greater than zero, since
     the bytes used to hold the size are included in the count.  */

  if (STRING_TABLE_OFFSET == 0)
    {
      /* It appears that with the existing bfd code, STRING_TABLE_OFFSET
	 will never be zero, even when there is no string table.  This
	 would appear to be a bug in bfd.  */
      DBX_STRINGTAB_SIZE (objfile) = 0;
      DBX_STRINGTAB (objfile) = NULL;
    }
  else
    {
      val = bfd_seek (sym_bfd, STRING_TABLE_OFFSET, SEEK_SET);
      if (val < 0)
	perror_with_name (name);

      memset (size_temp, 0, sizeof (size_temp));
      val = bfd_read (size_temp, sizeof (size_temp), sym_bfd);
      if (val < 0)
	{
	  perror_with_name (name);
	}
      else if (val == 0)
	{
	  /* With the existing bfd code, STRING_TABLE_OFFSET will be set to
	     EOF if there is no string table, and attempting to read the size
	     from EOF will read zero bytes.  */
	  DBX_STRINGTAB_SIZE (objfile) = 0;
	  DBX_STRINGTAB (objfile) = NULL;
	}
      else
	{
	  /* Read some data that would appear to be the string table size.
	     If there really is a string table, then it is probably the right
	     size.  Byteswap if necessary and validate the size.  Note that
	     the minimum is DBX_STRINGTAB_SIZE_SIZE.  If we just read some
	     random data that happened to be at STRING_TABLE_OFFSET, because
	     bfd can't tell us there is no string table, the sanity checks may
	     or may not catch this.  */
	  DBX_STRINGTAB_SIZE (objfile) = bfd_h_get_32 (sym_bfd, size_temp);

	  if (DBX_STRINGTAB_SIZE (objfile) < sizeof (size_temp)
	      || DBX_STRINGTAB_SIZE (objfile) > bfd_get_size (sym_bfd))
	    error (_("ridiculous string table size (%d bytes)."),
		   DBX_STRINGTAB_SIZE (objfile));

	  DBX_STRINGTAB (objfile) =
	    (char *) obstack_alloc (&objfile->objfile_obstack,
				    DBX_STRINGTAB_SIZE (objfile));
	  OBJSTAT (objfile, sz_strtab += DBX_STRINGTAB_SIZE (objfile));

	  /* Now read in the string table in one big gulp.  */

	  val = bfd_seek (sym_bfd, STRING_TABLE_OFFSET, SEEK_SET);
	  if (val < 0)
	    perror_with_name (name);
	  val = bfd_read (DBX_STRINGTAB (objfile),
			  DBX_STRINGTAB_SIZE (objfile),
			  sym_bfd);
	  if (val != DBX_STRINGTAB_SIZE (objfile))
	    perror_with_name (name);
	}
    }
}

/* Perform any local cleanups required when we are done with a particular
   objfile.  I.E, we are in the process of discarding all symbol information
   for an objfile, freeing up all memory held for it, and unlinking the
   objfile struct from the global list of known objfiles.  */

static void
dbx_symfile_finish (struct objfile *objfile)
{
  free_header_files ();
}





/* FIXME: The only difference between this and elfstab_build_psymtabs
   is the call to install_minimal_symbols for elf, and the support for
   split sections.  If the differences are really that small, the code
   should be shared.  */

/* Scan and build partial symbols for an coff symbol file.
   The coff file has already been processed to get its minimal symbols.

   This routine is the equivalent of dbx_symfile_init and dbx_symfile_read
   rolled into one.

   OBJFILE is the object file we are reading symbols from.
   ADDR is the address relative to which the symbols are (e.g.
   the base address of the text segment).
   TEXTADDR is the address of the text section.
   TEXTSIZE is the size of the text section.
   STABSECTS is the list of .stab sections in OBJFILE.
   STABSTROFFSET and STABSTRSIZE define the location in OBJFILE where the
   .stabstr section exists.

   This routine is mostly copied from dbx_symfile_init and dbx_symfile_read,
   adjusted for coff details.  */

void
coffstab_build_psymtabs (struct objfile *objfile,
			 CORE_ADDR textaddr, unsigned int textsize,
			 const std::vector<asection *> &stabsects,
			 file_ptr stabstroffset, unsigned int stabstrsize)
{
  int val;
  bfd *sym_bfd = objfile->obfd.get ();
  const char *name = bfd_get_filename (sym_bfd);
  unsigned int stabsize;

  /* Allocate struct to keep track of stab reading.  */
  dbx_objfile_data_key.emplace (objfile);
  dbx_symfile_info *key = dbx_objfile_data_key.get (objfile);

  DBX_TEXT_ADDR (objfile) = textaddr;
  DBX_TEXT_SIZE (objfile) = textsize;

#define	COFF_STABS_SYMBOL_SIZE	12	/* XXX FIXME XXX */
  DBX_SYMBOL_SIZE (objfile) = COFF_STABS_SYMBOL_SIZE;
  DBX_STRINGTAB_SIZE (objfile) = stabstrsize;

  if (stabstrsize > bfd_get_size (sym_bfd))
    error (_("ridiculous string table size: %d bytes"), stabstrsize);
  DBX_STRINGTAB (objfile) = (char *)
    obstack_alloc (&objfile->objfile_obstack, stabstrsize + 1);
  OBJSTAT (objfile, sz_strtab += stabstrsize + 1);

  /* Now read in the string table in one big gulp.  */

  val = bfd_seek (sym_bfd, stabstroffset, SEEK_SET);
  if (val < 0)
    perror_with_name (name);
  val = bfd_read (DBX_STRINGTAB (objfile), stabstrsize, sym_bfd);
  if (val != stabstrsize)
    perror_with_name (name);

  stabsread_new_init ();
  free_header_files ();
  init_header_files ();

  key->ctx.processing_acc_compilation = 1;

  /* In a coff file, we've already installed the minimal symbols that came
     from the coff (non-stab) symbol table, so always act like an
     incremental load here.  */
  scoped_restore save_symbuf_sections
    = make_scoped_restore (&key->ctx.symbuf_sections);
  if (stabsects.size () == 1)
    {
      stabsize = bfd_section_size (stabsects[0]);
      DBX_SYMCOUNT (objfile) = stabsize / DBX_SYMBOL_SIZE (objfile);
      DBX_SYMTAB_OFFSET (objfile) = stabsects[0]->filepos;
    }
  else
    {
      DBX_SYMCOUNT (objfile) = 0;
      for (asection *section : stabsects)
	{
	  stabsize = bfd_section_size (section);
	  DBX_SYMCOUNT (objfile) += stabsize / DBX_SYMBOL_SIZE (objfile);
	}

      DBX_SYMTAB_OFFSET (objfile) = stabsects[0]->filepos;

      key->ctx.sect_idx = 1;
      key->ctx.symbuf_sections = &stabsects;
      key->ctx.symbuf_left = bfd_section_size (stabsects[0]);
      key->ctx.symbuf_read = 0;
    }

  dbx_symfile_read (objfile, 0);
}

/* Scan and build partial symbols for an ELF symbol file.
   This ELF file has already been processed to get its minimal symbols.

   This routine is the equivalent of dbx_symfile_init and dbx_symfile_read
   rolled into one.

   OBJFILE is the object file we are reading symbols from.
   ADDR is the address relative to which the symbols are (e.g.
   the base address of the text segment).
   STABSECT is the BFD section information for the .stab section.
   STABSTROFFSET and STABSTRSIZE define the location in OBJFILE where the
   .stabstr section exists.

   This routine is mostly copied from dbx_symfile_init and dbx_symfile_read,
   adjusted for elf details.  */

void
elfstab_build_psymtabs (struct objfile *objfile, asection *stabsect,
			file_ptr stabstroffset, unsigned int stabstrsize)
{
  int val;
  bfd *sym_bfd = objfile->obfd.get ();
  const char *name = bfd_get_filename (sym_bfd);

  stabsread_new_init ();

  /* Allocate struct to keep track of stab reading.  */
  dbx_objfile_data_key.emplace (objfile);
  dbx_symfile_info *key = dbx_objfile_data_key.get (objfile);

  /* Find the first and last text address.  dbx_symfile_read seems to
     want this.  */
  find_text_range (sym_bfd, objfile);

#define	ELF_STABS_SYMBOL_SIZE	12	/* XXX FIXME XXX */
  DBX_SYMBOL_SIZE (objfile) = ELF_STABS_SYMBOL_SIZE;
  DBX_SYMCOUNT (objfile)
    = bfd_section_size (stabsect) / DBX_SYMBOL_SIZE (objfile);
  DBX_STRINGTAB_SIZE (objfile) = stabstrsize;
  DBX_SYMTAB_OFFSET (objfile) = stabsect->filepos;
  DBX_STAB_SECTION (objfile) = stabsect;

  if (stabstrsize > bfd_get_size (sym_bfd))
    error (_("ridiculous string table size: %d bytes"), stabstrsize);
  DBX_STRINGTAB (objfile) = (char *)
    obstack_alloc (&objfile->objfile_obstack, stabstrsize + 1);
  OBJSTAT (objfile, sz_strtab += stabstrsize + 1);

  /* Now read in the string table in one big gulp.  */

  val = bfd_seek (sym_bfd, stabstroffset, SEEK_SET);
  if (val < 0)
    perror_with_name (name);
  val = bfd_read (DBX_STRINGTAB (objfile), stabstrsize, sym_bfd);
  if (val != stabstrsize)
    perror_with_name (name);

  stabsread_new_init ();
  free_header_files ();
  init_header_files ();

  key->ctx.processing_acc_compilation = 1;

  key->ctx.symbuf_read = 0;
  key->ctx.symbuf_left = bfd_section_size (stabsect);

  scoped_restore restore_stabs_data = make_scoped_restore (&key->ctx.stabs_data);
  gdb::unique_xmalloc_ptr<gdb_byte> data_holder;

  key->ctx.stabs_data = symfile_relocate_debug_section (objfile, stabsect, NULL);
  if (key->ctx.stabs_data)
    data_holder.reset (key->ctx.stabs_data);

  /* In an elf file, we've already installed the minimal symbols that came
     from the elf (non-stab) symbol table, so always act like an
     incremental load here.  dbx_symfile_read should not generate any new
     minimal symbols, since we will have already read the ELF dynamic symbol
     table and normal symbol entries won't be in the ".stab" section; but in
     case it does, it will install them itself.  */
  dbx_symfile_read (objfile, 0);
}

/* Scan and build partial symbols for a file with special sections for stabs
   and stabstrings.  The file has already been processed to get its minimal
   symbols, and any other symbols that might be necessary to resolve GSYMs.

   This routine is the equivalent of dbx_symfile_init and dbx_symfile_read
   rolled into one.

   OBJFILE is the object file we are reading symbols from.
   ADDR is the address relative to which the symbols are (e.g. the base address
   of the text segment).
   STAB_NAME is the name of the section that contains the stabs.
   STABSTR_NAME is the name of the section that contains the stab strings.

   This routine is mostly copied from dbx_symfile_init and
   dbx_symfile_read.  */

void
stabsect_build_psymtabs (struct objfile *objfile, char *stab_name,
			 char *stabstr_name, char *text_name)
{
  int val;
  bfd *sym_bfd = objfile->obfd.get ();
  const char *name = bfd_get_filename (sym_bfd);
  asection *stabsect;
  asection *stabstrsect;
  asection *text_sect;

  stabsect = bfd_get_section_by_name (sym_bfd, stab_name);
  stabstrsect = bfd_get_section_by_name (sym_bfd, stabstr_name);

  if (!stabsect)
    return;

  if (!stabstrsect)
    error (_("stabsect_build_psymtabs:  Found stabs (%s), "
	     "but not string section (%s)"),
	   stab_name, stabstr_name);

  dbx_objfile_data_key.emplace (objfile);

  text_sect = bfd_get_section_by_name (sym_bfd, text_name);
  if (!text_sect)
    error (_("Can't find %s section in symbol file"), text_name);
  DBX_TEXT_ADDR (objfile) = bfd_section_vma (text_sect);
  DBX_TEXT_SIZE (objfile) = bfd_section_size (text_sect);

  DBX_SYMBOL_SIZE (objfile) = sizeof (struct external_nlist);
  DBX_SYMCOUNT (objfile) = bfd_section_size (stabsect)
    / DBX_SYMBOL_SIZE (objfile);
  DBX_STRINGTAB_SIZE (objfile) = bfd_section_size (stabstrsect);
  DBX_SYMTAB_OFFSET (objfile) = stabsect->filepos;	/* XXX - FIXME: POKING
							   INSIDE BFD DATA
							   STRUCTURES */

  if (DBX_STRINGTAB_SIZE (objfile) > bfd_get_size (sym_bfd))
    error (_("ridiculous string table size: %d bytes"),
	   DBX_STRINGTAB_SIZE (objfile));
  DBX_STRINGTAB (objfile) = (char *)
    obstack_alloc (&objfile->objfile_obstack,
		   DBX_STRINGTAB_SIZE (objfile) + 1);
  OBJSTAT (objfile, sz_strtab += DBX_STRINGTAB_SIZE (objfile) + 1);

  /* Now read in the string table in one big gulp.  */

  val = bfd_get_section_contents (sym_bfd,	/* bfd */
				  stabstrsect,	/* bfd section */
				  DBX_STRINGTAB (objfile), /* input buffer */
				  0,		/* offset into section */
				  DBX_STRINGTAB_SIZE (objfile)); /* amount to
								    read */

  if (!val)
    perror_with_name (name);

  stabsread_new_init ();
  free_header_files ();
  init_header_files ();

  /* Now, do an incremental load.  */

  dbx_objfile_data_key.get (objfile)->ctx.processing_acc_compilation = 1;
  dbx_symfile_read (objfile, 0);
}

static const struct sym_fns aout_sym_fns =
{
  dbx_new_init,			/* init anything gbl to entire symtab */
  dbx_symfile_init,		/* read initial info, setup for sym_read() */
  dbx_symfile_read,		/* read a symbol file into symtab */
  dbx_symfile_finish,		/* finished with file, cleanup */
  default_symfile_offsets, 	/* parse user's offsets to internal form */
  default_symfile_segments,	/* Get segment information from a file.  */
  NULL,
  default_symfile_relocate,	/* Relocate a debug section.  */
  NULL,				/* sym_probe_fns */
};

void _initialize_dbxread ();
void
_initialize_dbxread ()
{
  add_symtab_fns (bfd_target_aout_flavour, &aout_sym_fns);
}
