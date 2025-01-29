/* Read AIX xcoff symbol tables and convert to internal format, for GDB.
   Copyright (C) 1986-2025 Free Software Foundation, Inc.
   Derived from coffread.c, dbxread.c, and a lot of hacking.
   Contributed by IBM Corporation.

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

#include "bfd.h"
#include "event-top.h"

#include <sys/types.h>
#include <fcntl.h>
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#include <sys/stat.h>
#include <algorithm>

#include "coff/internal.h"
#include "libcoff.h"
#include "coff/xcoff.h"
#include "libxcoff.h"
#include "coff/rs6000.h"
#include "xcoffread.h"

#include "symtab.h"
#include "gdbtypes.h"
/* FIXME: ezannoni/2004-02-13 Verify if the include below is really needed.  */
#include "symfile.h"
#include "objfiles.h"
#include "buildsym-legacy.h"
#include "expression.h"
#include "complaints.h"
#include "psymtab.h"
#include "dwarf2/sect-names.h"
#include "dwarf2/public.h"


/* We put a pointer to this structure in the read_symtab_private field
   of the psymtab.  */

struct xcoff_symloc
  {

    /* First symbol number for this file.  */

    int first_symnum;

    /* Number of symbols in the section of the symbol table devoted to
       this file's symbols (actually, the section bracketed may contain
       more than just this file's symbols).  If numsyms is 0, the only
       reason for this thing's existence is the dependency list.  Nothing
       else will happen when it is read in.  */

    int numsyms;

    /* Position of the start of the line number information for this
       psymtab.  */
    unsigned int lineno_off;
  };


/* Simplified internal version of coff symbol table information.  */

struct xcoff_symbol
  {
    char *c_name;
    int c_symnum;		/* Symbol number of this entry.  */
    int c_naux;			/* 0 if syment only, 1 if syment + auxent.  */
    CORE_ADDR c_value;
    unsigned char c_sclass;
    int c_secnum;
    unsigned int c_type;
  };

static bfd *symfile_bfd;

/* Core address of start and end of text of current source file.
   This is calculated from the first function seen after a C_FILE
   symbol.  */


static CORE_ADDR cur_src_end_addr;

/* Initial symbol-table-debug-string vector length.  */

#define	INITIAL_STABVECTOR_LENGTH	40

/* Size of a COFF symbol.  I think it is always 18, so I'm not sure
   there is any reason not to just use a #define, but might as well
   ask BFD for the size and store it here, I guess.  */

static unsigned local_symesz;

struct xcoff_symfile_info
  {
    file_ptr min_lineno_offset {};	/* Where in file lowest line#s are.  */
    file_ptr max_lineno_offset {};	/* 1+last byte of line#s in file.  */

    /* Pointer to the string table.  */
    char *strtbl = nullptr;

    /* Pointer to debug section.  */
    char *debugsec = nullptr;

    /* Pointer to the a.out symbol table.  */
    char *symtbl = nullptr;

    /* Number of symbols in symtbl.  */
    int symtbl_num_syms = 0;

    /* Offset in data section to TOC anchor.  */
    CORE_ADDR toc_offset = 0;
  };

/* Key for XCOFF-associated data.  */

static const registry<objfile>::key<xcoff_symfile_info> xcoff_objfile_data_key;

/* Convenience macro to access the per-objfile XCOFF data.  */

#define XCOFF_DATA(objfile)						\
  xcoff_objfile_data_key.get (objfile)

/* XCOFF names for dwarf sections.  There is no compressed sections.  */

static const struct dwarf2_debug_sections dwarf2_xcoff_names = {
  { ".dwinfo", NULL },
  { ".dwabrev", NULL },
  { ".dwline", NULL },
  { ".dwloc", NULL },
  { NULL, NULL }, /* debug_loclists */
  /* AIX XCOFF defines one, named DWARF section for macro debug information.
     XLC does not generate debug_macinfo for DWARF4 and below.
     The section is assigned to debug_macro for DWARF5 and above. */
  { NULL, NULL },
  { ".dwmac", NULL },
  { ".dwstr", NULL },
  { NULL, NULL }, /* debug_str_offsets */
  { NULL, NULL }, /* debug_line_str */
  { ".dwrnges", NULL },
  { NULL, NULL }, /* debug_rnglists */
  { ".dwpbtyp", NULL },
  { NULL, NULL }, /* debug_addr */
  { ".dwframe", NULL },
  { NULL, NULL }, /* eh_frame */
  { NULL, NULL }, /* gdb_index */
  { NULL, NULL }, /* debug_names */
  { NULL, NULL }, /* debug_aranges */
  23
};

static void
bf_notfound_complaint (void)
{
  complaint (_("line numbers off, `.bf' symbol not found"));
}

static void xcoff_initial_scan (struct objfile *, symfile_add_flags);

static void
enter_line_range (struct subfile *, unsigned, unsigned,
		  CORE_ADDR, CORE_ADDR, unsigned *);

static void init_stringtab (bfd *, file_ptr, struct objfile *);

static void xcoff_symfile_init (struct objfile *);

static void xcoff_new_init (struct objfile *);

static void xcoff_symfile_finish (struct objfile *);

static void read_symbol (struct internal_syment *, int);

static int read_symbol_lineno (int);

static CORE_ADDR read_symbol_nvalue (int);

static void process_linenos (CORE_ADDR, CORE_ADDR);

struct xcoff_find_targ_sec_arg
  {
    int targ_index;
    int *resultp;
    asection **bfd_sect;
    struct objfile *objfile;
  };

/* Linenos are processed on a file-by-file basis.

   Two reasons:

   1) xlc (IBM's native c compiler) postpones static function code
   emission to the end of a compilation unit.  This way it can
   determine if those functions (statics) are needed or not, and
   can do some garbage collection (I think).  This makes line
   numbers and corresponding addresses unordered, and we end up
   with a line table like:


   lineno       addr
   foo()          10    0x100
   20   0x200
   30   0x300

   foo3()         70    0x400
   80   0x500
   90   0x600

   static foo2()
   40   0x700
   50   0x800
   60   0x900

   and that breaks gdb's binary search on line numbers, if the
   above table is not sorted on line numbers.  And that sort
   should be on function based, since gcc can emit line numbers
   like:

   10   0x100   - for the init/test part of a for stmt.
   20   0x200
   30   0x300
   10   0x400   - for the increment part of a for stmt.

   arrange_linetable() will do this sorting.

   2)   aix symbol table might look like:

   c_file               // beginning of a new file
   .bi          // beginning of include file
   .ei          // end of include file
   .bi
   .ei

   basically, .bi/.ei pairs do not necessarily encapsulate
   their scope.  They need to be recorded, and processed later
   on when we come the end of the compilation unit.
   Include table (inclTable) and process_linenos() handle
   that.  */


/* Given a line table with function entries are marked, arrange its
   functions in ascending order and strip off function entry markers
   and return it in a newly created table.  */

/* FIXME: I think all this stuff can be replaced by just passing
   sort_linevec = 1 to end_compunit_symtab.  */

static void
arrange_linetable (std::vector<linetable_entry> &old_linetable)
{
  std::vector<linetable_entry> fentries;

  for (int ii = 0; ii < old_linetable.size (); ++ii)
    {
      if (!old_linetable[ii].is_stmt)
	continue;

      if (old_linetable[ii].line == 0)
	{
	  /* Function entry found.  */
	  linetable_entry &e = fentries.emplace_back ();
	  e.line = ii;
	  e.is_stmt = true;
	  e.set_unrelocated_pc (old_linetable[ii].unrelocated_pc ());
	}
    }

  if (fentries.empty ())
    return;

  std::sort (fentries.begin (), fentries.end ());

  /* Allocate a new line table.  */
  std::vector<linetable_entry> new_linetable;
  new_linetable.reserve (old_linetable.size ());

  /* If line table does not start with a function beginning, copy up until
     a function begin.  */
  for (int i = 0; i < old_linetable.size () && old_linetable[i].line != 0; ++i)
    new_linetable.push_back (old_linetable[i]);

  /* Now copy function lines one by one.  */
  for (const linetable_entry &entry : fentries)
    {
      /* If the function was compiled with XLC, we may have to add an
	 extra line to cover the function prologue.  */
      int jj = entry.line;
      if (jj + 1 < old_linetable.size ()
	  && (old_linetable[jj].unrelocated_pc ()
	      != old_linetable[jj + 1].unrelocated_pc ()))
	{
	  new_linetable.push_back (old_linetable[jj]);
	  new_linetable.back ().line = old_linetable[jj + 1].line;
	}

      for (jj = entry.line + 1;
	   jj < old_linetable.size () && old_linetable[jj].line != 0;
	   ++jj)
	new_linetable.push_back (old_linetable[jj]);
    }

  new_linetable.shrink_to_fit ();
  old_linetable = std::move (new_linetable);
}

/* include file support: C_BINCL/C_EINCL pairs will be kept in the
   following `IncludeChain'.  At the end of each symtab (end_compunit_symtab),
   we will determine if we should create additional symtab's to
   represent if (the include files.  */


typedef struct _inclTable
{
  char *name;			/* include filename */

  /* Offsets to the line table.  end points to the last entry which is
     part of this include file.  */
  int begin, end;

  struct subfile *subfile;
  unsigned funStartLine;	/* Start line # of its function.  */
}
InclTable;

#define	INITIAL_INCLUDE_TABLE_LENGTH	20
static InclTable *inclTable;	/* global include table */
static int inclIndx;		/* last entry to table */
static int inclLength;		/* table length */
static int inclDepth;		/* nested include depth */

/* subfile structure for the main compilation unit.  */
static subfile *main_subfile;

/* Global variable to pass the psymtab down to all the routines involved
   in psymtab to symtab processing.  */
static legacy_psymtab *this_symtab_psymtab;

/* Objfile related to this_symtab_psymtab; set at the same time.  */
static struct objfile *this_symtab_objfile;

/* given the start and end addresses of a compilation unit (or a csect,
   at times) process its lines and create appropriate line vectors.  */

static void
process_linenos (CORE_ADDR start, CORE_ADDR end)
{
  int offset;
  file_ptr max_offset
    = XCOFF_DATA (this_symtab_objfile)->max_lineno_offset;

  /* In the main source file, any time we see a function entry, we
     reset this variable to function's absolute starting line number.
     All the following line numbers in the function are relative to
     this, and we record absolute line numbers in record_line().  */

  unsigned int main_source_baseline = 0;

  unsigned *firstLine;

  offset =
    ((struct xcoff_symloc *) this_symtab_psymtab->read_symtab_private)->lineno_off;
  if (offset == 0)
    goto return_after_cleanup;

  if (inclIndx == 0)
    /* All source lines were in the main source file.  None in include
       files.  */

    enter_line_range (main_subfile, offset, 0, start, end,
		      &main_source_baseline);

  else
    {
      /* There was source with line numbers in include files.  */

      int linesz =
	coff_data (this_symtab_objfile->obfd)->local_linesz;
      main_source_baseline = 0;

      for (int ii = 0; ii < inclIndx; ++ii)
	{
	  /* If there is main file source before include file, enter it.  */
	  if (offset < inclTable[ii].begin)
	    {
	      enter_line_range
		(main_subfile, offset, inclTable[ii].begin - linesz,
		 start, 0, &main_source_baseline);
	    }

	  if (strcmp (inclTable[ii].name, get_last_source_file ()) == 0)
	    {
	      /* The entry in the include table refers to the main source
		 file.  Add the lines to the main subfile.  */

	      main_source_baseline = inclTable[ii].funStartLine;
	      enter_line_range
		(main_subfile, inclTable[ii].begin, inclTable[ii].end,
		 start, 0, &main_source_baseline);
	      inclTable[ii].subfile = main_subfile;
	    }
	  else
	    {
	      /* Have a new subfile for the include file.  */
	      inclTable[ii].subfile = new subfile;

	      firstLine = &(inclTable[ii].funStartLine);

	      /* Enter include file's lines now.  */
	      enter_line_range (inclTable[ii].subfile, inclTable[ii].begin,
				inclTable[ii].end, start, 0, firstLine);
	    }

	  if (offset <= inclTable[ii].end)
	    offset = inclTable[ii].end + linesz;
	}

      /* All the include files' line have been processed at this point.  Now,
	 enter remaining lines of the main file, if any left.  */
      if (offset < max_offset + 1 - linesz)
	{
	  enter_line_range (main_subfile, offset, 0, start, end,
			    &main_source_baseline);
	}
    }

  /* Process main file's line numbers.  */
  if (!main_subfile->line_vector_entries.empty ())
    {
      /* Line numbers are not necessarily ordered.  xlc compilation will
	 put static function to the end.  */
      arrange_linetable (main_subfile->line_vector_entries);
    }

  /* Now, process included files' line numbers.  */

  for (int ii = 0; ii < inclIndx; ++ii)
    {
      if (inclTable[ii].subfile != main_subfile
	  && !inclTable[ii].subfile->line_vector_entries.empty ())
	{
	  /* Line numbers are not necessarily ordered.  xlc compilation will
	     put static function to the end.  */
	  arrange_linetable (inclTable[ii].subfile->line_vector_entries);

	  push_subfile ();

	  /* For the same include file, we might want to have more than one
	     subfile.  This happens if we have something like:

	     ......
	     #include "foo.h"
	     ......
	     #include "foo.h"
	     ......

	     while foo.h including code in it.  (stupid but possible)
	     Since start_subfile() looks at the name and uses an
	     existing one if finds, we need to provide a fake name and
	     fool it.  */

#if 0
	  start_subfile (inclTable[ii].name);
#else
	  {
	    /* Pick a fake name that will produce the same results as this
	       one when passed to deduce_language_from_filename.  Kludge on
	       top of kludge.  */
	    const char *fakename = strrchr (inclTable[ii].name, '.');

	    if (fakename == NULL)
	      fakename = " ?";
	    start_subfile (fakename);
	  }
	  struct subfile *current_subfile = get_current_subfile ();
	  current_subfile->name = inclTable[ii].name;
	  current_subfile->name_for_id = inclTable[ii].name;
#endif

	  start_subfile (pop_subfile ());
	}
    }

return_after_cleanup:

  /* We don't want to keep alloc/free'ing the global include file table.  */
  inclIndx = 0;
}

static void
aix_process_linenos (struct objfile *objfile)
{
  /* There is no linenos to read if there are only dwarf info.  */
  if (this_symtab_psymtab == NULL)
    return;

  /* Process line numbers and enter them into line vector.  */
  process_linenos (get_last_source_start_addr (), cur_src_end_addr);
}


/* Enter a given range of lines into the line vector.
   can be called in the following two ways:
   enter_line_range (subfile, beginoffset, endoffset,
		     startaddr, 0, firstLine)  or
   enter_line_range (subfile, beginoffset, 0,
		     startaddr, endaddr, firstLine)

   endoffset points to the last line table entry that we should pay
   attention to.  */

static void
enter_line_range (struct subfile *subfile, unsigned beginoffset,
		  unsigned endoffset,	/* offsets to line table */
		  CORE_ADDR startaddr,	/* offsets to line table */
		  CORE_ADDR endaddr, unsigned *firstLine)
{
  struct objfile *objfile = this_symtab_objfile;
  struct gdbarch *gdbarch = objfile->arch ();
  unsigned int curoffset;
  CORE_ADDR addr;
  void *ext_lnno;
  struct internal_lineno int_lnno;
  unsigned int limit_offset;
  bfd *abfd;
  int linesz;

  if (endoffset == 0 && startaddr == 0 && endaddr == 0)
    return;
  curoffset = beginoffset;
  limit_offset = XCOFF_DATA (objfile)->max_lineno_offset;

  if (endoffset != 0)
    {
      if (endoffset >= limit_offset)
	{
	  complaint (_("Bad line table offset in C_EINCL directive"));
	  return;
	}
      limit_offset = endoffset;
    }
  else
    limit_offset -= 1;

  abfd = objfile->obfd.get ();
  linesz = coff_data (abfd)->local_linesz;
  ext_lnno = alloca (linesz);

  while (curoffset <= limit_offset)
    {
      if (bfd_seek (abfd, curoffset, SEEK_SET) != 0
	  || bfd_read (ext_lnno, linesz, abfd) != linesz)
	return;
      bfd_coff_swap_lineno_in (abfd, ext_lnno, &int_lnno);

      /* Find the address this line represents.  */
      addr = (int_lnno.l_lnno
	      ? int_lnno.l_addr.l_paddr
	      : read_symbol_nvalue (int_lnno.l_addr.l_symndx));
      addr += objfile->text_section_offset ();

      if (addr < startaddr || (endaddr && addr >= endaddr))
	return;

      CORE_ADDR record_addr = (gdbarch_addr_bits_remove (gdbarch, addr)
			       - objfile->text_section_offset ());
      if (int_lnno.l_lnno == 0)
	{
	  *firstLine = read_symbol_lineno (int_lnno.l_addr.l_symndx);
	  record_line (subfile, 0, unrelocated_addr (record_addr));
	  --(*firstLine);
	}
      else
	record_line (subfile, *firstLine + int_lnno.l_lnno,
		     unrelocated_addr (record_addr));
      curoffset += linesz;
    }
}


/* Save the vital information for use when closing off the current file.
   NAME is the file name the symbols came from, START_ADDR is the first
   text address for the file, and SIZE is the number of bytes of text.  */

#define complete_symtab(name, start_addr) {	\
  set_last_source_file (name);			\
  set_last_source_start_addr (start_addr);	\
}


#define	SYMNAME_ALLOC(NAME, ALLOCED)	\
  ((ALLOCED) ? (NAME) : obstack_strdup (&objfile->objfile_obstack, \
					(NAME)))

/* Set *SYMBOL to symbol number symno in symtbl.  */
static void
read_symbol (struct internal_syment *symbol, int symno)
{
  struct xcoff_symfile_info *xcoff = XCOFF_DATA (this_symtab_objfile);
  int nsyms = xcoff->symtbl_num_syms;
  char *stbl = xcoff->symtbl;

  if (symno < 0 || symno >= nsyms)
    {
      complaint (_("Invalid symbol offset"));
      symbol->n_value = 0;
      symbol->n_scnum = -1;
      return;
    }
  bfd_coff_swap_sym_in (this_symtab_objfile->obfd.get (),
			stbl + (symno * local_symesz),
			symbol);
}

/* Get value corresponding to symbol number symno in symtbl.  */

static CORE_ADDR
read_symbol_nvalue (int symno)
{
  struct internal_syment symbol[1];

  read_symbol (symbol, symno);
  return symbol->n_value;
}


/* Find the address of the function corresponding to symno, where
   symno is the symbol pointed to by the linetable.  */

static int
read_symbol_lineno (int symno)
{
  struct objfile *objfile = this_symtab_objfile;
  int xcoff64 = bfd_xcoff_is_xcoff64 (objfile->obfd);

  struct xcoff_symfile_info *info = XCOFF_DATA (objfile);
  int nsyms = info->symtbl_num_syms;
  char *stbl = info->symtbl;
  char *strtbl = info->strtbl;

  struct internal_syment symbol[1];
  union internal_auxent main_aux[1];

  if (symno < 0)
    {
      bf_notfound_complaint ();
      return 0;
    }

  /* Note that just searching for a short distance (e.g. 50 symbols)
     is not enough, at least in the following case.

     .extern foo
     [many .stabx entries]
     [a few functions, referring to foo]
     .globl foo
     .bf

     What happens here is that the assembler moves the .stabx entries
     to right before the ".bf" for foo, but the symbol for "foo" is before
     all the stabx entries.  See PR gdb/2222.  */

  /* Maintaining a table of .bf entries might be preferable to this search.
     If I understand things correctly it would need to be done only for
     the duration of a single psymtab to symtab conversion.  */
  while (symno < nsyms)
    {
      bfd_coff_swap_sym_in (symfile_bfd,
			    stbl + (symno * local_symesz), symbol);
      if (symbol->n_sclass == C_FCN)
	{
	  char *name = xcoff64 ? strtbl + symbol->n_offset : symbol->n_name;

	  if (strcmp (name, ".bf") == 0)
	    goto gotit;
	}
      symno += symbol->n_numaux + 1;
    }

  bf_notfound_complaint ();
  return 0;

gotit:
  /* Take aux entry and return its lineno.  */
  symno++;
  bfd_coff_swap_aux_in (objfile->obfd.get (), stbl + symno * local_symesz,
			symbol->n_type, symbol->n_sclass,
			0, symbol->n_numaux, main_aux);

  return main_aux->x_sym.x_misc.x_lnsz.x_lnno;
}

/* Support for line number handling.  */

/* This function is called for every section; it finds the outer limits
 * of the line table (minimum and maximum file offset) so that the
 * mainline code can read the whole thing for efficiency.
 */
static void
find_linenos (struct bfd *abfd, struct bfd_section *asect, void *vpinfo)
{
  struct xcoff_symfile_info *info;
  int size, count;
  file_ptr offset, maxoff;

  count = asect->lineno_count;

  if (strcmp (asect->name, ".text") != 0 || count == 0)
    return;

  size = count * coff_data (abfd)->local_linesz;
  info = (struct xcoff_symfile_info *) vpinfo;
  offset = asect->line_filepos;
  maxoff = offset + size;

  if (offset < info->min_lineno_offset || info->min_lineno_offset == 0)
    info->min_lineno_offset = offset;

  if (maxoff > info->max_lineno_offset)
    info->max_lineno_offset = maxoff;
}

static void
xcoff_new_init (struct objfile *objfile)
{
}

/* Do initialization in preparation for reading symbols from OBJFILE.

   We will only be called if this is an XCOFF or XCOFF-like file.
   BFD handles figuring out the format of the file, and code in symfile.c
   uses BFD's determination to vector to us.  */

static void
xcoff_symfile_init (struct objfile *objfile)
{
  /* Allocate struct to keep track of the symfile.  */
  xcoff_objfile_data_key.emplace (objfile);
}

/* Perform any local cleanups required when we are done with a particular
   objfile.  I.E, we are in the process of discarding all symbol information
   for an objfile, freeing up all memory held for it, and unlinking the
   objfile struct from the global list of known objfiles.  */

static void
xcoff_symfile_finish (struct objfile *objfile)
{
  /* Start with a fresh include table for the next objfile.  */
  if (inclTable)
    {
      xfree (inclTable);
      inclTable = NULL;
      delete main_subfile;
    }
  inclIndx = inclLength = inclDepth = 0;
}


static void
init_stringtab (bfd *abfd, file_ptr offset, struct objfile *objfile)
{
  long length;
  int val;
  unsigned char lengthbuf[4];
  char *strtbl;
  struct xcoff_symfile_info *xcoff = XCOFF_DATA (objfile);

  xcoff->strtbl = NULL;

  if (bfd_seek (abfd, offset, SEEK_SET) < 0)
    error (_("cannot seek to string table in %s: %s"),
	   bfd_get_filename (abfd), bfd_errmsg (bfd_get_error ()));

  val = bfd_read ((char *) lengthbuf, sizeof lengthbuf, abfd);
  length = bfd_h_get_32 (abfd, lengthbuf);

  /* If no string table is needed, then the file may end immediately
     after the symbols.  Just return with `strtbl' set to NULL.  */

  if (val != sizeof lengthbuf || length < sizeof lengthbuf)
    return;

  /* Allocate string table from objfile_obstack.  We will need this table
     as long as we have its symbol table around.  */

  strtbl = (char *) obstack_alloc (&objfile->objfile_obstack, length);
  xcoff->strtbl = strtbl;

  /* Copy length buffer, the first byte is usually zero and is
     used for stabs with a name length of zero.  */
  memcpy (strtbl, lengthbuf, sizeof lengthbuf);
  if (length == sizeof lengthbuf)
    return;

  val = bfd_read (strtbl + sizeof lengthbuf, length - sizeof lengthbuf, abfd);

  if (val != length - sizeof lengthbuf)
    error (_("cannot read string table from %s: %s"),
	   bfd_get_filename (abfd), bfd_errmsg (bfd_get_error ()));
  if (strtbl[length - 1] != '\0')
    error (_("bad symbol file: string table "
	     "does not end with null character"));

  return;
}

/* Return the toc offset value for a given objfile.  */

CORE_ADDR
xcoff_get_toc_offset (struct objfile *objfile)
{
  if (objfile)
    return XCOFF_DATA (objfile)->toc_offset;
  return 0;
}

/* Scan and build partial symbols for a symbol file.
   We have been initialized by a call to dbx_symfile_init, which
   put all the relevant info into a "struct dbx_symfile_info",
   hung off the objfile structure.

   SECTION_OFFSETS contains offsets relative to which the symbols in the
   various sections are (depending where the sections were actually
   loaded).  */

static void
xcoff_initial_scan (struct objfile *objfile, symfile_add_flags symfile_flags)
{
  bfd *abfd;
  int num_symbols;		/* # of symbols */
  file_ptr symtab_offset;	/* symbol table and */
  file_ptr stringtab_offset;	/* string table file offsets */
  struct xcoff_symfile_info *info;

  info = XCOFF_DATA (objfile);
  symfile_bfd = abfd = objfile->obfd.get ();

  num_symbols = bfd_get_symcount (abfd);	/* # of symbols */
  symtab_offset = obj_sym_filepos (abfd);	/* symbol table file offset */
  stringtab_offset = symtab_offset +
    num_symbols * coff_data (abfd)->local_symesz;

  info->min_lineno_offset = 0;
  info->max_lineno_offset = 0;
  info->debugsec = nullptr;
  bfd_map_over_sections (abfd, find_linenos, info);

  if (num_symbols > 0)
    {
      /* Read the string table.  */
      init_stringtab (abfd, stringtab_offset, objfile);
    }

  /* DWARF2 sections.  */

  if (!(objfile->flags & OBJF_READNEVER))
    {
      /* If we can't read dwarf from an inferior, it will only have
	 stabs debuginfo, which we don't support anymore.  */
      if (!dwarf2_initialize_objfile (objfile, &dwarf2_xcoff_names))
	  warning (_("No usable debug information found."));
    }
}

static void
xcoff_symfile_offsets (struct objfile *objfile,
		       const section_addr_info &addrs)
{
  const char *first_section_name;

  default_symfile_offsets (objfile, addrs);

  /* Oneof the weird side-effects of default_symfile_offsets is that
     it sometimes sets some section indices to zero for sections that,
     in fact do not exist. See the body of default_symfile_offsets
     for more info on when that happens. Undo that, as this then allows
     us to test whether the associated section exists or not, and then
     access it quickly (without searching it again).  */

  if (objfile->section_offsets.empty ())
    return; /* Is that even possible?  Better safe than sorry.  */

  first_section_name
    = bfd_section_name (objfile->sections_start[0].the_bfd_section);

  if (objfile->sect_index_text == 0
      && strcmp (first_section_name, ".text") != 0)
    objfile->sect_index_text = -1;

  if (objfile->sect_index_data == 0
      && strcmp (first_section_name, ".data") != 0)
    objfile->sect_index_data = -1;

  if (objfile->sect_index_bss == 0
      && strcmp (first_section_name, ".bss") != 0)
    objfile->sect_index_bss = -1;

  if (objfile->sect_index_rodata == 0
      && strcmp (first_section_name, ".rodata") != 0)
    objfile->sect_index_rodata = -1;
}

/* Register our ability to parse symbols for xcoff BFD files.  */

static const struct sym_fns xcoff_sym_fns =
{

  /* It is possible that coff and xcoff should be merged as
     they do have fundamental similarities (for example, the extra storage
     classes used for stabs could presumably be recognized in any COFF file).
     However, in addition to obvious things like all the csect hair, there are
     some subtler differences between xcoffread.c and coffread.c, notably
     the fact that coffread.c has no need to read in all the symbols, but
     xcoffread.c reads all the symbols and does in fact randomly access them
     (in C_BSTAT and line number processing).  */

  xcoff_new_init,		/* init anything gbl to entire symtab */
  xcoff_symfile_init,		/* read initial info, setup for sym_read() */
  xcoff_initial_scan,		/* read a symbol file into symtab */
  xcoff_symfile_finish,		/* finished with file, cleanup */
  xcoff_symfile_offsets,	/* xlate offsets ext->int form */
  default_symfile_segments,	/* Get segment information from a file.  */
  aix_process_linenos,
  default_symfile_relocate,	/* Relocate a debug section.  */
  NULL,				/* sym_probe_fns */
};

/* Same as xcoff_get_n_import_files, but for core files.  */

static int
xcoff_get_core_n_import_files (bfd *abfd)
{
  asection *sect = bfd_get_section_by_name (abfd, ".ldinfo");
  gdb_byte buf[4];
  file_ptr offset = 0;
  int n_entries = 0;

  if (sect == NULL)
    return -1;  /* Not a core file.  */

  for (offset = 0; offset < bfd_section_size (sect);)
    {
      int next;

      n_entries++;

      if (!bfd_get_section_contents (abfd, sect, buf, offset, 4))
	return -1;
      next = bfd_get_32 (abfd, buf);
      if (next == 0)
	break;  /* This is the last entry.  */
      offset += next;
    }

  /* Return the number of entries, excluding the first one, which is
     the path to the executable that produced this core file.  */
  return n_entries - 1;
}

/* Return the number of import files (shared libraries) that the given
   BFD depends on.  Return -1 if this number could not be computed.  */

int
xcoff_get_n_import_files (bfd *abfd)
{
  asection *sect = bfd_get_section_by_name (abfd, ".loader");
  gdb_byte buf[4];
  int l_nimpid;

  /* If the ".loader" section does not exist, the objfile is probably
     not an executable.  Might be a core file...  */
  if (sect == NULL)
    return xcoff_get_core_n_import_files (abfd);

  /* The number of entries in the Import Files Table is stored in
     field l_nimpid.  This field is always at offset 16, and is
     always 4 bytes long.  Read those 4 bytes.  */

  if (!bfd_get_section_contents (abfd, sect, buf, 16, 4))
    return -1;
  l_nimpid = bfd_get_32 (abfd, buf);

  /* By convention, the first entry is the default LIBPATH value
     to be used by the system loader, so it does not count towards
     the number of import files.  */
  return l_nimpid - 1;
}

INIT_GDB_FILE (xcoffread)
{
  add_symtab_fns (bfd_target_xcoff_flavour, &xcoff_sym_fns);
}
