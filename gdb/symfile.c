/* Generic symbol file reading for the GNU debugger, GDB.
   Copyright 1990, 1991 Free Software Foundation, Inc.
   Contributed by Cygnus Support, using pieces from other GDB modules.

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

#include <stdio.h>
#include "defs.h"
#include "symtab.h"
#include "gdbcore.h"
#include "frame.h"
#include "target.h"
#include "value.h"
#include "symfile.h"
#include "gdbcmd.h"
#include "breakpoint.h"

#include <obstack.h>
#include <assert.h>

#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

CORE_ADDR entry_point;			/* Where execution starts in symfile */

extern int info_verbose;

extern void qsort ();
extern char *getenv ();
extern char *rindex ();

extern CORE_ADDR startup_file_start;	/* From blockframe.c */
extern CORE_ADDR startup_file_end;	/* From blockframe.c */

/* Functions this file defines */
static struct objfile *symfile_open ();
static struct sym_fns *symfile_init ();
static void clear_symtab_users_once ();

static void free_all_psymtabs ();
static void free_all_symtabs ();

/* List of all available sym_fns.  */

struct sym_fns *symtab_fns = NULL;

/* Saves the sym_fns of the current symbol table, so we can call
   the right XXX_new_init function when we free it.  FIXME.  This
   should be extended to calling the new_init function for each
   existing symtab or psymtab, since the main symbol file and 
   subsequent added symbol files can have different types.  */

static struct sym_fns *symfile_fns;

/* Allocate an obstack to hold objects that should be freed
   when we load a new symbol table.
   This includes the symbols made by dbxread
   and the types that are not permanent.  */

struct obstack obstack1;

struct obstack *symbol_obstack = &obstack1;

/* This obstack will be used for partial_symbol objects.  It can
   probably actually be the same as the symbol_obstack above, but I'd
   like to keep them seperate for now.  If I want to later, I'll
   replace one with the other.  */

struct obstack obstack2;

struct obstack *psymbol_obstack = &obstack2;

/* The object file that the main symbol table was loaded from (e.g. the
   argument to the "symbol-file" or "file" command).  */

struct objfile *symfile_objfile = 0;

/* Structures with which to manage partial symbol allocation.  */

struct psymbol_allocation_list global_psymbols = {0}, static_psymbols = {0};

/* Flag for whether user will be reloading symbols multiple times.
   Defaults to ON for VxWorks, otherwise OFF.  */

#ifdef SYMBOL_RELOADING_DEFAULT
int symbol_reloading = SYMBOL_RELOADING_DEFAULT;
#else
int symbol_reloading = 0;
#endif

/* Structure to manage complaints about symbol file contents.  */

struct complaint complaint_root[1] = {
  {(char *)0, 0, complaint_root},
};

/* Some actual complaints.  */

struct complaint oldsyms_complaint = {
	"Replacing old symbols for `%s'", 0, 0 };

struct complaint empty_symtab_complaint = {
	"Empty symbol table found for `%s'", 0, 0 };


/* In the following sort, we always make sure that
   register debug symbol declarations always come before regular
   debug symbol declarations (as might happen when parameters are
   then put into registers by the compiler).  */

static int
compare_symbols (s1, s2)
     struct symbol **s1, **s2;
{
  register int namediff;

  /* Compare the initial characters.  */
  namediff = SYMBOL_NAME (*s1)[0] - SYMBOL_NAME (*s2)[0];
  if (namediff != 0) return namediff;

  /* If they match, compare the rest of the names.  */
  namediff = strcmp (SYMBOL_NAME (*s1), SYMBOL_NAME (*s2));
  if (namediff != 0) return namediff;

  /* For symbols of the same name, registers should come first.  */
  return ((SYMBOL_CLASS (*s2) == LOC_REGISTER)
	  - (SYMBOL_CLASS (*s1) == LOC_REGISTER));
}

/* Call sort_block_syms to sort alphabetically the symbols of one block.  */

void
sort_block_syms (b)
     register struct block *b;
{
  qsort (&BLOCK_SYM (b, 0), BLOCK_NSYMS (b),
	 sizeof (struct symbol *), compare_symbols);
}

/* Call sort_symtab_syms to sort alphabetically
   the symbols of each block of one symtab.  */

void
sort_symtab_syms (s)
     register struct symtab *s;
{
  register struct blockvector *bv;
  int nbl;
  int i;
  register struct block *b;

  if (s == 0)
    return;
  bv = BLOCKVECTOR (s);
  nbl = BLOCKVECTOR_NBLOCKS (bv);
  for (i = 0; i < nbl; i++)
    {
      b = BLOCKVECTOR_BLOCK (bv, i);
      if (BLOCK_SHOULD_SORT (b))
	sort_block_syms (b);
    }
}

void
sort_all_symtab_syms ()
{
  register struct symtab *s;

  for (s = symtab_list; s; s = s->next)
    {
      sort_symtab_syms (s);
    }
}

/* Make a copy of the string at PTR with SIZE characters in the symbol obstack
   (and add a null character at the end in the copy).
   Returns the address of the copy.  */

char *
obsavestring (ptr, size)
     char *ptr;
     int size;
{
  register char *p = (char *) obstack_alloc (symbol_obstack, size + 1);
  /* Open-coded bcopy--saves function call time.
     These strings are usually short.  */
  {
    register char *p1 = ptr;
    register char *p2 = p;
    char *end = ptr + size;
    while (p1 != end)
      *p2++ = *p1++;
  }
  p[size] = 0;
  return p;
}

/* Concatenate strings S1, S2 and S3; return the new string.
   Space is found in the symbol_obstack.  */

char *
obconcat (s1, s2, s3)
     char *s1, *s2, *s3;
{
  register int len = strlen (s1) + strlen (s2) + strlen (s3) + 1;
  register char *val = (char *) obstack_alloc (symbol_obstack, len);
  strcpy (val, s1);
  strcat (val, s2);
  strcat (val, s3);
  return val;
}

/* Accumulate the misc functions in bunches of 127.
   At the end, copy them all into one newly allocated structure.  */

#define MISC_BUNCH_SIZE 127

struct misc_bunch
{
  struct misc_bunch *next;
  struct misc_function contents[MISC_BUNCH_SIZE];
};

/* Bunch currently being filled up.
   The next field points to chain of filled bunches.  */

static struct misc_bunch *misc_bunch;

/* Number of slots filled in current bunch.  */

static int misc_bunch_index;

/* Total number of misc functions recorded so far.  */

static int misc_count;

void
init_misc_bunches ()
{
  misc_count = 0;
  misc_bunch = 0;
  misc_bunch_index = MISC_BUNCH_SIZE;
}

void
prim_record_misc_function (name, address, misc_type)
     char *name;
     CORE_ADDR address;
     enum misc_function_type misc_type;
{
  register struct misc_bunch *new;

  if (misc_bunch_index == MISC_BUNCH_SIZE)
    {
      new = (struct misc_bunch *) xmalloc (sizeof (struct misc_bunch));
      misc_bunch_index = 0;
      new->next = misc_bunch;
      misc_bunch = new;
    }
  misc_bunch->contents[misc_bunch_index].name = name;
  misc_bunch->contents[misc_bunch_index].address = address;
  misc_bunch->contents[misc_bunch_index].type = misc_type;
  misc_bunch->contents[misc_bunch_index].misc_info = 0;
  misc_bunch_index++;
  misc_count++;
}

static int
compare_misc_functions (fn1, fn2)
     struct misc_function *fn1, *fn2;
{
  /* Return a signed result based on unsigned comparisons
     so that we sort into unsigned numeric order.  */
  if (fn1->address < fn2->address)
    return -1;
  if (fn1->address > fn2->address)
    return 1;
  return 0;
}

/* ARGSUSED */
void
discard_misc_bunches (foo)
     int foo;
{
  register struct misc_bunch *next;

  while (misc_bunch)
    {
      next = misc_bunch->next;
      free (misc_bunch);
      misc_bunch = next;
    }
}

/* After adding things to the vector, sort or re-sort it into address order.  */
void
sort_misc_function_vector ()
{
  qsort (misc_function_vector, misc_function_count,
	 sizeof (struct misc_function),
	 compare_misc_functions);
}

/* Compact duplicate entries out of the misc function vector by walking
   through the vector and compacting out entries with duplicate addresses
   and matching names.

   When files contain multiple sources of symbol information, it is
   possible for the misc function vector to contain many duplicate entries.
   As an example, SVR4 systems use ELF formatted object files, which
   usually contain at least two different types of symbol tables (a
   standard ELF one and a smaller dynamic linking table), as well as
   DWARF debugging information for files compiled with -g.

   Without compacting, the misc function vector for gdb itself contains
   over a 1000 duplicates, about a third of the total table size.  Aside
   from the potential trap of not noticing that two successive entries
   identify the same location, this duplication impacts the time required
   to linearly scan the table, which is done in a number of places.  So
   just do one linear scan here and toss out the duplicates.

   Note that the strings themselves are allocated on the symbol_obstack,
   so we can't easily reclaim their memory.  They will get automatically
   freed when the symbol table is freed.

   Also note we only go up to the next to last entry within the loop
   and then copy the last entry explicitly after the loop terminates.

   Since the different sources of information for each symbol may
   have different levels of "completeness", we may have duplicates
   that have one entry with type "mf_unknown" and the other with a
   known type.  So if the one we are leaving alone has type mf_unknown,
   overwrite its type with the type from the one we are compacting out.  */

static void
compact_misc_function_vector ()
{
  struct misc_function *copyfrom;
  struct misc_function *copyto;

  if (misc_function_count == 0)
    return;

  copyfrom = copyto = misc_function_vector;
  while (copyfrom < misc_function_vector + misc_function_count - 1)
    {
      if (copyfrom -> address == (copyfrom + 1) -> address
	  && (strcmp (copyfrom -> name, (copyfrom + 1) -> name) == 0))
	{
	  if ((copyfrom + 1) -> type == mf_unknown)
	    {
	      (copyfrom + 1) -> type = copyfrom -> type;
	    }
	  copyfrom++;
	}
      else
	{
	  *copyto++ = *copyfrom++;
	}
    }
  *copyto++ = *copyfrom++;
  misc_function_count = copyto - misc_function_vector;
  misc_function_vector = (struct misc_function *)
    xrealloc (misc_function_vector,
	      misc_function_count * sizeof (struct misc_function));

}

/* INCLINK nonzero means bunches are from an incrementally-linked file.
   Add them to the existing bunches.
   Otherwise INCLINK is zero, and we start from scratch. */
void
condense_misc_bunches (inclink)
     int inclink;
{
  register int i, j;
  register struct misc_bunch *bunch;

  if (inclink)
    {
      misc_function_vector
	= (struct misc_function *)
	  xrealloc (misc_function_vector, (misc_count + misc_function_count)
		    * sizeof (struct misc_function));
      j = misc_function_count;
    }
  else
    {
      misc_function_vector
	= (struct misc_function *)
	  xmalloc (misc_count * sizeof (struct misc_function));
      j = 0;
    }

  bunch = misc_bunch;
  while (bunch)
    {
      for (i = 0; i < misc_bunch_index; i++, j++)
        {
	  misc_function_vector[j] = bunch->contents[i];
#ifdef NAMES_HAVE_UNDERSCORE
	  if (misc_function_vector[j].name[0] == '_')
	      misc_function_vector[j].name++;
#endif
#ifdef SOME_NAMES_HAVE_DOT
	  if (misc_function_vector[j].name[0] == '.')
	      misc_function_vector[j].name++;
#endif
	  
	}
      bunch = bunch->next;
      misc_bunch_index = MISC_BUNCH_SIZE;
    }

  if (misc_function_count + misc_count != j)		/* DEBUG */
    printf_filtered ("Function counts are off!  %d + %d != %d\n",
      misc_function_count, misc_count, j);

  misc_function_count = j;

  /* Sort the misc functions by address.  */

  sort_misc_function_vector ();

  /* Compact out any duplicates. */

  compact_misc_function_vector ();
}


/* Get the symbol table that corresponds to a partial_symtab.
   This is fast after the first time you do it.  In fact, there
   is an even faster macro PSYMTAB_TO_SYMTAB that does the fast
   case inline.  */

struct symtab *
psymtab_to_symtab (pst)
     register struct partial_symtab *pst;
{
  /* If it's been looked up before, return it. */
  if (pst->symtab)
    return pst->symtab;

  /* If it has not yet been read in, read it.  */
  if (!pst->readin)
    { 
      (*pst->read_symtab) (pst);
    }

  return pst->symtab;
}

/* Process a symbol file, as either the main file or as a dynamically
   loaded file.

   NAME is the file name (which will be tilde-expanded and made
   absolute herein) (but we don't free or modify NAME itself).
   FROM_TTY says how verbose to be.  MAINLINE specifies whether this
   is the main symbol file, or whether it's an extra symbol file such
   as dynamically loaded code.  If !mainline, ADDR is the address
   where the text segment was loaded.  If VERBO, the caller has printed
   a verbose message about the symbol reading (and complaints can be
   more terse about it).  */

void
syms_from_objfile (objfile, addr, mainline, verbo)
     struct objfile *objfile;
     CORE_ADDR addr;
     int mainline;
     int verbo;
{
  asection *text_sect;
  struct sym_fns *sf;
  bfd *sym_bfd = objfile->obfd;

  /* There is a distinction between having no symbol table
     (we refuse to read the file, leaving the old set of symbols around)
     and having no debugging symbols in your symbol table (we read
     the file and end up with a mostly empty symbol table).  */

  if (!(bfd_get_file_flags (sym_bfd) & HAS_SYMS))
    return;

  /* Save startup file's range of PC addresses to help blockframe.c
     decide where the bottom of the stack is.  */
  if (bfd_get_file_flags (sym_bfd) & EXEC_P)
    {
      /* Executable file -- record its entry point so we'll recognize
	 the startup file because it contains the entry point.  */
      entry_point = bfd_get_start_address (sym_bfd);
    }
  else
    {
      /* Examination of non-executable.o files.  Short-circuit this stuff.  */
      /* ~0 will not be in any file, we hope.  */
      entry_point = ~0;
      /* set the startup file to be an empty range.  */
      startup_file_start = 0;
      startup_file_end = 0;
    }

  sf = symfile_init (objfile);

  if (mainline) 
    {
      /* Since no error yet, throw away the old symbol table.  */

      if (symfile_objfile)
        free_objfile (symfile_objfile);
      symfile_objfile = 0;

      (*sf->sym_new_init) ();

      /* For mainline, caller didn't know the specified address of the
         text section.  We fix that here.  */
      text_sect = bfd_get_section_by_name (sym_bfd, ".text");
      addr = bfd_section_vma (sym_bfd, text_sect);
    }

  /* Allow complaints to appear for this new file, and record how
     verbose to be. */

  clear_complaints(1, verbo);

  (*sf->sym_read) (sf, addr, mainline);

  /* Don't allow char * to have a typename (else would get caddr_t.)  */
  /* Ditto void *.  FIXME should do this for all the builtin types.  */

  TYPE_NAME (lookup_pointer_type (builtin_type_char)) = 0;
  TYPE_NAME (lookup_pointer_type (builtin_type_void)) = 0;

  if (mainline)
    {
      /* OK, make it the "real" symbol file.  */
      symfile_objfile = objfile;
      symfile_fns = sf;
    }

  /* If we have wiped out any old symbol tables, clean up.  */
  clear_symtab_users_once ();

  /* We're done reading the symbol file; finish off complaints.  */
  clear_complaints(0, verbo);
}


/* Process a symbol file, as either the main file or as a dynamically
   loaded file.

   NAME is the file name (which will be tilde-expanded and made
   absolute herein) (but we don't free or modify NAME itself).
   FROM_TTY says how verbose to be.  MAINLINE specifies whether this
   is the main symbol file, or whether it's an extra symbol file such
   as dynamically loaded code.  If !mainline, ADDR is the address
   where the text segment was loaded.  */

void
symbol_file_add (name, from_tty, addr, mainline)
     char *name;
     int from_tty;
     CORE_ADDR addr;
     int mainline;
{
  struct objfile *objfile;
  bfd *sym_bfd;

  objfile = symfile_open (name);
  sym_bfd = objfile->obfd;

  /* There is a distinction between having no symbol table
     (we refuse to read the file, leaving the old set of symbols around)
     and having no debugging symbols in your symbol table (we read
     the file and end up with a mostly empty symbol table, but with lots
     of stuff in the misc function vector).  */

  if (!(bfd_get_file_flags (sym_bfd) & HAS_SYMS))
    {
      error ("%s has no symbol-table", name);
    }

  if ((symtab_list || partial_symtab_list)
      && mainline
      && from_tty
      && !query ("Load new symbol table from \"%s\"? ", name))
    error ("Not confirmed.");

  if (from_tty)
    {
      printf_filtered ("Reading symbols from %s...", name);
      wrap_here ("");
      fflush (stdout);
    }

  syms_from_objfile (objfile, addr, mainline, from_tty);

  if (from_tty)
    {
      printf_filtered ("done.\n");
      fflush (stdout);
    }
}

/* This is the symbol-file command.  Read the file, analyze its symbols,
   and add a struct symtab to symtab_list.  */

void
symbol_file_command (name, from_tty)
     char *name;
     int from_tty;
{

  dont_repeat ();

  if (name == 0)
    {
      if (symfile_objfile) {
        if ((symtab_list || partial_symtab_list)
	    && from_tty
	    && !query ("Discard symbol table from `%s'? ",
		       symfile_objfile->name))
	  error ("Not confirmed.");
	free_objfile (symfile_objfile);
      }
      symfile_objfile = 0;
      /* FIXME, this does not account for the main file and subsequent
         files (shared libs, dynloads, etc) having different formats. 
         It only calls the cleanup routine for the main file's format.  */
      if (symfile_fns) {
        (*symfile_fns->sym_new_init) ();
        free (symfile_fns);
        symfile_fns = 0;
      }
      return;
    }

  /* Getting new symbols may change our opinion about what is
     frameless.  */
  reinit_frame_cache ();

  symbol_file_add (name, from_tty, (CORE_ADDR)0, 1);
}

/* Open NAME and hand it off to BFD for preliminary analysis.  Result
   is newly malloc'd struct objfile *, which includes a newly malloc'd`
   copy of NAME (tilde-expanded and made absolute).
   In case of trouble, error() is called.  */

static struct objfile *
symfile_open (name)
     char *name;
{
  bfd *sym_bfd;
  int desc;
  char *absolute_name;
  struct objfile *objfile;

  name = tilde_expand (name);	/* Returns 1st new malloc'd copy */

  /* Look down path for it, allocate 2nd new malloc'd copy.  */
  desc = openp (getenv ("PATH"), 1, name, O_RDONLY, 0, &absolute_name);
  if (desc < 0) {
    make_cleanup (free, name);
    perror_with_name (name);
  }
  free (name);			/* Free 1st new malloc'd copy */
  name = absolute_name;		/* Keep 2nd malloc'd copy in objfile and bfd */

  sym_bfd = bfd_fdopenr (name, NULL, desc);
  if (!sym_bfd)
    {
      close (desc);
      make_cleanup (free, name);
      error ("Could not open `%s' to read symbols: %s",
	     name, bfd_errmsg (bfd_error));
    }

  if (!bfd_check_format (sym_bfd, bfd_object)) {
    bfd_close (sym_bfd);	/* This also closes desc */
    make_cleanup (free, name);
    error ("\"%s\": can't read symbols: %s.",
	   name, bfd_errmsg (bfd_error));
  }

  objfile = allocate_objfile (sym_bfd, name);
  return objfile;
}


/* Allocate a new objfile struct, fill it in as best we can, and return it.
   FIXME-soon!  Eventually, the objfile will contain the obstack in which
   the symtabs and psymtabs are contained, so they can all be blown away
   cheaply and easily.  */

struct objfile *
allocate_objfile (abfd, filename)
     bfd *abfd;
     char *filename;
{
  struct objfile *objfile;

  objfile = (struct objfile *) xmalloc (sizeof (struct objfile));
  bzero (objfile, sizeof (*objfile));

  objfile->obfd = abfd;
  objfile->name = filename;

  objfile->symtabs = 0;		/* Don't have any yet */
  objfile->psymtabs = 0;	/* Don't have any yet */

  objfile->mtime = bfd_get_mtime (abfd);

  /* Chain it to the list.  */
  objfile->next = object_files;
  object_files = objfile;

  return objfile;
}


/* Destroy an objfile and all the symtabs and psymtabs under it.  */

void
free_objfile (objfile)
     struct objfile *objfile;
{
  struct objfile *ofp;

  if (objfile->name)
    free (objfile->name);
  if (objfile->obfd)
    bfd_close (objfile->obfd);

  /* Remove it from the chain of all objfiles.  */
  if (object_files == objfile)
    object_files = objfile->next;
  else for (ofp = object_files; ofp; ofp = ofp->next) {
    if (ofp->next == objfile)
      ofp->next = objfile->next;
  }

  /* FIXME!  This should only free those associated with the objfile
     being passed to us.  THIS IS A KLUDGE TO BOOTSTRAP US.  */
  free_all_psymtabs ();
  free_all_symtabs ();

  free (objfile);
}


/* Link a new symtab_fns into the global symtab_fns list.
   Called by various _initialize routines.  */

void
add_symtab_fns (sf)
     struct sym_fns *sf;
{
  sf->next = symtab_fns;
  symtab_fns = sf;
}


/* Initialize to read symbols from the symbol file sym_bfd.  It either
   returns or calls error().  The result is a malloc'd struct sym_fns
   that contains cached information about the symbol file.  */

static struct sym_fns *
symfile_init (objfile)
     struct objfile *objfile;
{
  struct sym_fns *sf, *sf2;
  bfd *sym_bfd = objfile->obfd;

  for (sf = symtab_fns; sf != NULL; sf = sf->next)
    {
      if (!strncmp (bfd_get_target (sym_bfd), sf->sym_name, sf->sym_namelen))
	{
	  sf2 = (struct sym_fns *)xmalloc (sizeof (*sf2));	
	  /* FIXME, who frees this? */
	  *sf2 = *sf;
  	  sf2->objfile = objfile;
	  sf2->sym_bfd = sym_bfd;
	  sf2->sym_private = 0;			/* Not alloc'd yet */
	  (*sf2->sym_init) (sf2);
	  return sf2;
	}
    }
  error ("I'm sorry, Dave, I can't do that.  Symbol format `%s' unknown.",
	 bfd_get_target (sym_bfd));
  return 0; /* Appease lint.  */
}

/* This function runs the load command of our current target.  */

void
load_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  target_load (arg, from_tty);
}

/* This function allows the addition of incrementally linked object files.
   It does not modify any state in the target, only in the debugger.  */

/* ARGSUSED */
void
add_symbol_file_command (arg_string, from_tty)
     char *arg_string;
     int from_tty;
{
  char *name;
  CORE_ADDR text_addr;
  
  /* Getting new symbols may change our opinion about what is
     frameless.  */
  reinit_frame_cache ();

  if (arg_string == 0)
    error ("add-symbol-file takes a file name and an address");

  arg_string = tilde_expand (arg_string);
  make_cleanup (free, arg_string);

  for( ; *arg_string == ' '; arg_string++ );
  name = arg_string;
  for( ; *arg_string && *arg_string != ' ' ; arg_string++ );
  *arg_string++ = (char) 0;

  if (name[0] == 0)
    error ("add-symbol-file takes a file name and an address");

  text_addr = parse_and_eval_address (arg_string);

  dont_repeat ();

  if (!query ("add symbol table from file \"%s\" at text_addr = %s?\n",
	      name, local_hex_string (text_addr)))
    error ("Not confirmed.");

  symbol_file_add (name, 0, text_addr, 0);
}

/* Re-read symbols if a symbol-file has changed.  */
void
reread_symbols ()
{
  struct objfile *objfile;
  long new_modtime;
  int reread_one = 0;

  /* With the addition of shared libraries, this should be modified,
     the load time should be saved in the partial symbol tables, since
     different tables may come from different source files.  FIXME.
     This routine should then walk down each partial symbol table
     and see if the symbol table that it originates from has been changed
  */

  for (objfile = object_files; objfile; objfile = objfile->next) {
    if (objfile->obfd) {
      objfile->obfd->mtime_set = false;		/* Force it to reread. */
      new_modtime = bfd_get_mtime (objfile->obfd);
      if (new_modtime != objfile->mtime) {
	printf_filtered ("`%s' has changed; re-reading symbols.\n",
			 objfile->name);
	/* FIXME, this should use a different command...that would only
 	   affect this objfile's symbols.  */
	symbol_file_command (objfile->name, 0);
        objfile->mtime = new_modtime;
	reread_one = 1;
      }
    }
  }

  if (reread_one)
    breakpoint_re_set ();
}

/* This function is really horrible, but to avoid it, there would need
   to be more filling in of forward references.  */
void
fill_in_vptr_fieldno (type)
     struct type *type;
{
  if (TYPE_VPTR_FIELDNO (type) < 0)
    {
      int i;
      for (i = 1; i < TYPE_N_BASECLASSES (type); i++)
	{
	  fill_in_vptr_fieldno (TYPE_BASECLASS (type, i));
	  if (TYPE_VPTR_FIELDNO (TYPE_BASECLASS (type, i)) >= 0)
	    {
	      TYPE_VPTR_FIELDNO (type)
		= TYPE_VPTR_FIELDNO (TYPE_BASECLASS (type, i));
	      TYPE_VPTR_BASETYPE (type)
		= TYPE_VPTR_BASETYPE (TYPE_BASECLASS (type, i));
	      break;
	    }
	}
    }
}

/* Functions to handle complaints during symbol reading.  */

/* How many complaints about a particular thing should be printed before
   we stop whining about it?  Default is no whining at all, since so many
   systems have ill-constructed symbol files.  */

static unsigned stop_whining = 0;

/* Should each complaint be self explanatory, or should we assume that
   a series of complaints is being produced? 
   case 0:  self explanatory message.
   case 1:  First message of a series that must start off with explanation.
   case 2:  Subsequent message, when user already knows we are reading
            symbols and we can just state our piece.  */

static int complaint_series = 0;

/* Print a complaint about the input symbols, and link the complaint block
   into a chain for later handling.  */

void
complain (complaint, val)
     struct complaint *complaint;
     char *val;
{
  complaint->counter++;
  if (complaint->next == 0) {
    complaint->next = complaint_root->next;
    complaint_root->next = complaint;
  }
  if (complaint->counter > stop_whining)
    return;
  wrap_here ("");

  switch (complaint_series + (info_verbose << 1)) {

  /* Isolated messages, must be self-explanatory.  */
  case 0:
    puts_filtered ("During symbol reading, ");
    wrap_here("");
    printf_filtered (complaint->message, val);
    puts_filtered (".\n");
    break;

  /* First of a series, without `set verbose'.  */
  case 1:
    puts_filtered ("During symbol reading...");
    printf_filtered (complaint->message, val);
    puts_filtered ("...");
    wrap_here("");
    complaint_series++;
    break;

  /* Subsequent messages of a series, or messages under `set verbose'.
     (We'll already have produced a "Reading in symbols for XXX..." message
      and will clean up at the end with a newline.)  */
  default:
    printf_filtered (complaint->message, val);
    puts_filtered ("...");
    wrap_here("");
  }
}

/* Clear out all complaint counters that have ever been incremented.
   If sym_reading is 1, be less verbose about successive complaints,
   since the messages are appearing all together during a command that
   reads symbols (rather than scattered around as psymtabs get fleshed
   out into symtabs at random times).  If noisy is 1, we are in a
   noisy symbol reading command, and our caller will print enough
   context for the user to figure it out.  */

void
clear_complaints (sym_reading, noisy)
     int sym_reading;
     int noisy;
{
  struct complaint *p;

  for (p = complaint_root->next; p != complaint_root; p = p->next)
    p->counter = 0;

  if (!sym_reading && !noisy && complaint_series > 1) {
    /* Terminate previous series, since caller won't.  */
    puts_filtered ("\n");
  }

  complaint_series = sym_reading? 1 + noisy: 0;
}

enum language
deduce_language_from_filename (filename)
     char *filename;
{
  char *c = rindex (filename, '.');
  
  if (!c) ; /* Get default. */
  else if(!strcmp(c,".mod"))
     return language_m2;
  else if(!strcmp(c,".c"))
     return language_c;
  else if(!strcmp(c,".cc") || !strcmp(c,".C"))
     return language_cplus;

  return language_unknown;		/* default */
}

/* allocate_symtab:

   Allocate and partly initialize a new symbol table.  Return a pointer
   to it.  error() if no space.

   Caller must set these fields:
	LINETABLE(symtab)
	symtab->blockvector
	symtab->dirname
	symtab->free_code
	symtab->free_ptr
	initialize any EXTRA_SYMTAB_INFO
	possibly free_named_symtabs (symtab->filename);
	symtab->next = symtab_list;
	symtab_list = symtab;
 */

struct symtab *
allocate_symtab(name, objfile)
	char *name;
	struct objfile *objfile;
{
  register struct symtab *symtab;

  symtab = (struct symtab *) xmalloc (sizeof (struct symtab));
  bzero (symtab, sizeof (*symtab));
  symtab->filename = name;
  symtab->fullname = NULL;
  symtab->nlines = 0;
  symtab->line_charpos = 0;
  symtab->version = 0;
  symtab->language = deduce_language_from_filename (name);

  /* Hook it to the objfile it comes from */
  symtab->objfile = objfile;
  symtab->objfile_chain = objfile->symtabs;
  objfile->symtabs = symtab;

#ifdef INIT_EXTRA_SYMTAB_INFO
  INIT_EXTRA_SYMTAB_INFO(symtab);
#endif

  return symtab;
}

/* clear_symtab_users_once:

   This function is run after symbol reading, or from a cleanup.
   If an old symbol table was obsoleted, the old symbol table
   has been blown away, but the other GDB data structures that may 
   reference it have not yet been cleared or re-directed.  (The old
   symtab was zapped, and the cleanup queued, in free_named_symtab()
   below.)

   This function can be queued N times as a cleanup, or called
   directly; it will do all the work the first time, and then will be a
   no-op until the next time it is queued.  This works by bumping a
   counter at queueing time.  Much later when the cleanup is run, or at
   the end of symbol processing (in case the cleanup is discarded), if
   the queued count is greater than the "done-count", we do the work
   and set the done-count to the queued count.  If the queued count is
   less than or equal to the done-count, we just ignore the call.  This
   is needed because reading a single .o file will often replace many
   symtabs (one per .h file, for example), and we don't want to reset
   the breakpoints N times in the user's face.

   The reason we both queue a cleanup, and call it directly after symbol
   reading, is because the cleanup protects us in case of errors, but is
   discarded if symbol reading is successful.  */

static int clear_symtab_users_queued;
static int clear_symtab_users_done;

static void
clear_symtab_users_once ()
{
  /* Enforce once-per-`do_cleanups'-semantics */
  if (clear_symtab_users_queued <= clear_symtab_users_done)
    return;
  clear_symtab_users_done = clear_symtab_users_queued;

  printf ("Resetting debugger state after updating old symbol tables\n");

  /* Someday, we should do better than this, by only blowing away
     the things that really need to be blown.  */
  clear_value_history ();
  clear_displays ();
  clear_internalvars ();
  breakpoint_re_set ();
  set_default_breakpoint (0, 0, 0, 0);
  current_source_symtab = 0;
}

/* Delete the specified psymtab, and any others that reference it.  */

static void
cashier_psymtab (pst)
     struct partial_symtab *pst;
{
  struct partial_symtab *ps, *pprev;
  int i;

  /* Find its previous psymtab in the chain */
  for (ps = partial_symtab_list; ps; ps = ps->next) {
    if (ps == pst)
      break;
    pprev = ps;
  }

  if (ps) {
    /* Unhook it from the chain.  */
    if (ps == partial_symtab_list)
      partial_symtab_list = ps->next;
    else
      pprev->next = ps->next;

    /* FIXME, we can't conveniently deallocate the entries in the
       partial_symbol lists (global_psymbols/static_psymbols) that
       this psymtab points to.  These just take up space until all
       the psymtabs are reclaimed.  Ditto the dependencies list and
       filename, which are all in the psymbol_obstack.  */

    /* We need to cashier any psymtab that has this one as a dependency... */
again:
    for (ps = partial_symtab_list; ps; ps = ps->next) {
      for (i = 0; i < ps->number_of_dependencies; i++) {
	if (ps->dependencies[i] == pst) {
	  cashier_psymtab (ps);
	  goto again;		/* Must restart, chain has been munged. */
	}
      }
    }
  }
}

/* If a symtab or psymtab for filename NAME is found, free it along
   with any dependent breakpoints, displays, etc.
   Used when loading new versions of object modules with the "add-file"
   command.  This is only called on the top-level symtab or psymtab's name;
   it is not called for subsidiary files such as .h files.

   Return value is 1 if we blew away the environment, 0 if not.

   FIXME.  I think this is not the best way to do this.  We should
   work on being gentler to the environment while still cleaning up
   all stray pointers into the freed symtab.  */

int
free_named_symtabs (name)
     char *name;
{
  register struct symtab *s;
  register struct symtab *prev;
  register struct partial_symtab *ps;
  struct blockvector *bv;
  int blewit = 0;

  /* We only wack things if the symbol-reload switch is set.  */
  if (!symbol_reloading)
    return 0;

  /* Some symbol formats have trouble providing file names... */
  if (name == 0 || *name == '\0')
    return 0;

  /* Look for a psymtab with the specified name.  */

again2:
  for (ps = partial_symtab_list; ps; ps = ps->next) {
    if (!strcmp (name, ps->filename)) {
      cashier_psymtab (ps);	/* Blow it away...and its little dog, too.  */
      goto again2;		/* Must restart, chain has been munged */
    }
  }

  /* Look for a symtab with the specified name.  */

  for (s = symtab_list; s; s = s->next)
    {
      if (!strcmp (name, s->filename))
	break;
      prev = s;
    }

  if (s)
    {
      if (s == symtab_list)
	symtab_list = s->next;
      else
	prev->next = s->next;

      /* For now, queue a delete for all breakpoints, displays, etc., whether
	 or not they depend on the symtab being freed.  This should be
	 changed so that only those data structures affected are deleted.  */

      /* But don't delete anything if the symtab is empty.
	 This test is necessary due to a bug in "dbxread.c" that
	 causes empty symtabs to be created for N_SO symbols that
	 contain the pathname of the object file.  (This problem
	 has been fixed in GDB 3.9x).  */

      bv = BLOCKVECTOR (s);
      if (BLOCKVECTOR_NBLOCKS (bv) > 2
	  || BLOCK_NSYMS (BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK))
	  || BLOCK_NSYMS (BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK)))
	{
	  complain (&oldsyms_complaint, name);

	  clear_symtab_users_queued++;
	  make_cleanup (clear_symtab_users_once, 0);
	  blewit = 1;
	} else {
	  complain (&empty_symtab_complaint, name);
	}

      free_symtab (s);
    }
  else
    {
      /* It is still possible that some breakpoints will be affected
	 even though no symtab was found, since the file might have
	 been compiled without debugging, and hence not be associated
	 with a symtab.  In order to handle this correctly, we would need
	 to keep a list of text address ranges for undebuggable files.
	 For now, we do nothing, since this is a fairly obscure case.  */
      ;
    }

  /* FIXME, what about the misc function vector? */
  return blewit;
}

/*
 * Free all partial_symtab storage.
 */
static void
free_all_psymtabs()
{
  obstack_free (psymbol_obstack, 0);
  obstack_init (psymbol_obstack);
  partial_symtab_list = (struct partial_symtab *) 0;
}

/* Free all the symtabs that are currently installed,
   and all storage associated with them.
   Leaves us in a consistent state with no symtabs installed.  */

static void
free_all_symtabs ()
{
  register struct symtab *s, *snext;

  /* All values will be invalid because their types will be!  */

  clear_value_history ();
  clear_displays ();
  clear_internalvars ();
#if defined (CLEAR_SOLIB)
  CLEAR_SOLIB ();
#endif
  set_default_breakpoint (0, 0, 0, 0);

  current_source_symtab = 0;

  for (s = symtab_list; s; s = snext)
    {
      snext = s->next;
      free_symtab (s);
    }
  symtab_list = 0;
  obstack_free (symbol_obstack, 0);
  obstack_init (symbol_obstack);

  if (misc_function_vector)
    free (misc_function_vector);
  misc_function_count = 0;
  misc_function_vector = 0;
  clear_pc_function_cache();
}

void
_initialize_symfile ()
{

  add_com ("symbol-file", class_files, symbol_file_command,
	   "Load symbol table from executable file FILE.\n\
The `file' command can also load symbol tables, as well as setting the file\n\
to execute.");

  add_com ("add-symbol-file", class_files, add_symbol_file_command,
   "Load the symbols from FILE, assuming FILE has been dynamically loaded.\n\
The second argument provides the starting address of the file's text.");

  add_com ("load", class_files, load_command,
   "Dynamically load FILE into the running program, and record its symbols\n\
for access from GDB.");

  add_show_from_set
    (add_set_cmd ("complaints", class_support, var_zinteger,
		  (char *)&stop_whining,
	  "Set max number of complaints about incorrect symbols.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("symbol-reloading", class_support, var_boolean,
		  (char *)&symbol_reloading,
	  "Set dynamic symbol table reloading multiple times in one run.",
		  &setlist),
     &showlist);

  obstack_init (symbol_obstack);
  obstack_init (psymbol_obstack);
}
