/* Read AIXcoff symbol tables and convert to internal format, for GDB.
   Copyright (C) 1986-1991 Free Software Foundation, Inc.
   Derived from coffread.c, dbxread.c, and a lot of hacking.
   Contributed by IBM Corporation.

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
#include "param.h"

#include <stdio.h>

/* AIX COFF names have a preceeding dot `.' */
#define NAMES_HAVE_DOT 1

#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>

#include "obstack.h"
#include <sys/param.h>
#include <sys/file.h>
#include <sys/stat.h>

#include "symtab.h"
#include "symfile.h"
#include "buildsym.h"

#include "internalcoff.h"	/* FIXME, internal data from BFD */
#include "libcoff.h"		/* FIXME, internal data from BFD */
#include "coff-rs6000.h"	/* FIXME, raw file-format guts of xcoff */

extern char *index();

static void enter_line_range ();
static struct symbol *process_xcoff_symbol ();
static int read_symbol_nvalue ();
static int read_symbol_lineno ();


/* Simplified internal version of coff symbol table information */

struct coff_symbol {
  char *c_name;
  int c_symnum;		/* symbol number of this entry */
  int c_nsyms;		/* 0 if syment only, 1 if syment + auxent */
  long c_value;
  int c_sclass;
  int c_secnum;
  unsigned int c_type;
};

/* The COFF line table, in raw form.  */
static char *linetab = NULL;		/* Its actual contents */
static long linetab_offset;		/* Its offset in the file */
static unsigned long linetab_size;	/* Its size */

/* last function's saved coff symbol `cs' */

static struct coff_symbol fcn_cs_saved;

static bfd *symfile_bfd;

/* Core address of start and end of text of current source file.
   This is calculated from the first function seen after a C_FILE
   symbol. */

static CORE_ADDR cur_src_start_addr;
static CORE_ADDR cur_src_end_addr;

/* Core address of the end of the first object file.  */

static CORE_ADDR first_object_file_end;

/* pointer to the string table */
static char *strtbl;

/* length of the string table */
static int  strtbl_len;

/* pointer to debug section */
static char *debugsec;

/* pointer to the a.out symbol table */
static char *symtbl;

/* initial symbol-table-debug-string vector length */

#define	INITIAL_STABVECTOR_LENGTH	40

struct pending_stabs *global_stabs;
struct pending_stabs *file_stabs;

/* Nonzero if within a function (so symbols should be local,
   if nothing says specifically).  */

int within_function;

/* Local variables that hold the shift and mask values for the
   COFF file that we are currently reading.  These come back to us
   from BFD, and are referenced by their macro names, as well as
   internally to the BTYPE, ISPTR, ISFCN, ISARY, ISTAG, and DECREF
   macros from ../internalcoff.h .  */

static unsigned	local_n_btshft;
static unsigned	local_n_tmask;

#undef	N_BTSHFT
#define	N_BTSHFT	local_n_btshft
#undef	N_TMASK
#define	N_TMASK		local_n_tmask
 
/* Local variables that hold the sizes in the file of various COFF structures.
   (We only need to know this to read them from the file -- BFD will then
   translate the data in them, into `internal_xxx' structs in the right
   byte order, alignment, etc.)  */

static unsigned	local_symesz;

/* coff_symfile_init()
   is the coff-specific initialization routine for reading symbols.
   It is passed a struct sym_fns which contains, among other things,
   the BFD for the file whose symbols are being read, and a slot for
   a pointer to "private data" which we fill with cookies and other
   treats for coff_symfile_read().
 
   We will only be called if this is a COFF or COFF-like file.
   BFD handles figuring out the format of the file, and code in symtab.c
   uses BFD's determination to vector to us.
 
   The ultimate result is a new symtab (or, FIXME, eventually a psymtab).  */

struct coff_symfile_info {
  file_ptr min_lineno_offset;		/* Where in file lowest line#s are */
  file_ptr max_lineno_offset;		/* 1+last byte of line#s in file */
};


/* Call sort_syms to sort alphabetically
   the symbols of each block of each symtab.  */

static int
compare_symbols (s1, s2)
struct symbol **s1, **s2;
{
  /* Names that are less should come first.  */
  register int namediff = strcmp (SYMBOL_NAME (*s1), SYMBOL_NAME (*s2));
  if (namediff != 0) 
    return namediff;

  /* For symbols of the same name, registers should come first.  */
  return ((SYMBOL_CLASS (*s2) == LOC_REGISTER)
      - (SYMBOL_CLASS (*s1) == LOC_REGISTER));
}


/* Sort a vector of symbols by their value. */

static void
sort_syms ()
{
  register struct symtab *s;
  register int i, nbl;
  register struct blockvector *bv;
  register struct block *b;

  for (s = symtab_list; s; s = s->next)
  {
    bv = BLOCKVECTOR (s);
    nbl = BLOCKVECTOR_NBLOCKS (bv);
    for (i = 0; i < nbl; i++)
    {
      b = BLOCKVECTOR_BLOCK (bv, i);
      if (BLOCK_SHOULD_SORT (b))
	qsort (&BLOCK_SYM (b, 0), BLOCK_NSYMS (b),
	    sizeof (struct symbol *), compare_symbols);
    }
  }
}


/* add a given stab string into given stab vector. */

static void
add_stab_to_list (stabname, stabvector)
char *stabname;
struct pending_stabs **stabvector;
{
  if ( *stabvector == NULL) {
    *stabvector = (struct pending_stabs *)
	xmalloc (sizeof (struct pending_stabs) + 
			INITIAL_STABVECTOR_LENGTH * sizeof (char*));
    (*stabvector)->count = 0;
    (*stabvector)->length = INITIAL_STABVECTOR_LENGTH;
  }
  else if ((*stabvector)->count >= (*stabvector)->length) {
    (*stabvector)->length += INITIAL_STABVECTOR_LENGTH;
    *stabvector = (struct pending_stabs *)
	xrealloc (*stabvector, sizeof (struct pending_stabs) + 
	(*stabvector)->length * sizeof (char*));
  }
  (*stabvector)->stab [(*stabvector)->count++] = stabname;
}


/* for all the stabs in a given stab vector, build appropriate types 
   and fix their symbols in given symbol vector. */

void
patch_block_stabs (symbols, stabs)
struct pending *symbols;
struct pending_stabs *stabs;
{
  int ii;

  if (!stabs)
    return;

  /* for all the stab entries, find their corresponding symbols and 
     patch their types! */

  for (ii=0; ii < stabs->count; ++ii) {
    char *name = stabs->stab[ii];
    char *pp = (char*) index (name, ':');
    struct symbol *sym = find_symbol_in_list (symbols, name, pp-name);
    if (!sym) {
      printf ("ERROR! stab symbol not found!\n");	/* FIXME */
    }
    else {
      pp += 2;

      if (*(pp-1) == 'F' || *(pp-1) == 'f')
	SYMBOL_TYPE (sym) = lookup_function_type (read_type (&pp));
      else
	SYMBOL_TYPE (sym) = read_type (&pp);
    }
  }
}

/* Enter a given range of lines into the line vector.
   can be called in the following two ways:
     enter_line_range (subfile, beginoffset, endoffset, 0, firstLine)  or
     enter_line_range (subfile, beginoffset, 0, endaddr, firstLine) */

static void
enter_line_range (subfile, beginoffset, endoffset, endaddr, firstLine)
  struct subfile *subfile;		/* which sub-file to put line#s in */
  unsigned   beginoffset, endoffset;	/* offsets to line table */
  CORE_ADDR  endaddr;
  unsigned   *firstLine;
{
  char		*pp, *limit;
  CORE_ADDR	addr;
  struct internal_lineno lptr;
  unsigned local_linesz = coff_data (symfile_bfd)->local_linesz;

  pp = &linetab [beginoffset - linetab_offset];
  limit = endoffset ? &linetab [endoffset - linetab_offset]
  		      : &linetab [linetab_size -1];

  while (pp <= limit) {

    /* Swap and align this lineno entry into lptr.  */
    bfd_coff_swap_lineno_in (symfile_bfd, pp, &lptr);

    /* find the address this line represents */
    addr = lptr.l_lnno ? 
      lptr.l_addr.l_paddr : read_symbol_nvalue (symtbl, lptr.l_addr.l_symndx);

    if (endaddr && addr >= endaddr)
      return;

    if (lptr.l_lnno == 0) {
      *firstLine = read_symbol_lineno (symtbl, lptr.l_addr.l_symndx);
      --(*firstLine);
    }
    else
      record_line (subfile, *firstLine + lptr.l_lnno, addr);

    pp += local_linesz;
  }
}


/* Save the vital information for use when closing off the current file.
   NAME is the file name the symbols came from, START_ADDR is the first
   text address for the file, and SIZE is the number of bytes of text.  */

#define complete_symtab(name, start_addr) {	\
  last_source_file = savestring (name, strlen (name));	\
  cur_src_start_addr = start_addr;			\
}


/* Refill the symbol table input buffer
   and set the variables that control fetching entries from it.
   Reports an error if no data available.
   This function can read past the end of the symbol table
   (into the string table) but this does no harm.  */

/* Reading symbol table has to be fast! Keep the followings as macros, rather
   than functions. */

#define	RECORD_MISC_FUNCTION(NAME, ADDR, TYPE, ALLOCED)	\
{						\
  char *namestr;				\
  if (ALLOCED) 					\
    namestr = (NAME) + 1;			\
  else {					\
    namestr = obstack_copy0 (symbol_obstack, (NAME) + 1, strlen ((NAME)+1)); \
    (ALLOCED) = 1;						\
  }								\
  prim_record_misc_function (namestr, (ADDR), (TYPE));		\
  last_recorded_fun = (ADDR);					\
}



/* aixcoff has static blocks marked in `.bs', `.es' pairs. They cannot be
   nested. At any given time, a symbol can only be in one static block.
   This is the base address of current static block, zero if non exists. */
   
static int static_block_base = 0;

/* true if space for symbol name has been allocated. */

static int symname_alloced = 0;

/* read the whole symbol table of a given bfd. */

void
read_xcoff_symtab (objfile, nsyms)
     struct objfile *objfile;	/* Object file we're reading from */
     int nsyms;			/* # of symbols */
{
  bfd *abfd = objfile->obfd;
  /* char *symtbl; */		/* Raw symbol table base */
  char *raw_symbol;		/* Pointer into raw seething symbol table */
  char *raw_auxptr;		/* Pointer to first raw aux entry for sym */
  struct internal_syment symbol[1];
  union internal_auxent main_aux[1];
  struct coff_symbol cs[1];
  CORE_ADDR file_start_addr = 0;
  CORE_ADDR file_end_addr = 0;

  int next_file_symnum = -1;
  int just_started = 1;
  int depth = 0;
  int val;
  int fcn_first_line;
  int fcn_last_line;
  int fcn_start_addr;
  long fcn_line_offset;
  size_t size;

  /* fcn_cs_saved is global because process_xcoff_symbol needs it. */
  union internal_auxent fcn_aux_saved;
  struct context_stack *new;

  char *filestring = " _start_ ";	/* Name of the current file. */
  char *last_seen_csect;
  int last_recorded_fun = 0;		/* last recorded fun. value */

  /* Get the appropriate COFF "constants" related to the file we're handling. */
  N_TMASK = coff_data (abfd)->local_n_tmask;
  N_BTSHFT = coff_data (abfd)->local_n_btshft;
  local_symesz = coff_data (abfd)->local_symesz;

  last_source_file = 0;
  last_seen_csect = 0;
  last_recorded_fun = 0;

  start_symtab (filestring, (char *)NULL, file_start_addr);
  symnum = 0;
  first_object_file_end = 0;

  /* Allocate space for the entire symbol table at once, and read it
     all in.  The bfd is already positioned at the beginning of
     the symbol table.  */

  size = coff_data (abfd)->local_symesz * nsyms;
  symtbl = xmalloc (size);

  val = bfd_read (symtbl, size, 1, abfd);
  if (val != size)
    perror_with_name ("reading symbol table");

  raw_symbol = symtbl;

  while (symnum < nsyms) {

    QUIT;			/* make this command interruptable.  */

    /* READ_ONE_SYMBOL (symbol, cs, symname_alloced); */
    /* read one symbol into `cs' structure. After processing the whole symbol
       table, only string table will be kept in memory, symbol table and debug
       section of aixcoff will be freed. Thus we can mark symbols with names
       in string table as `alloced'. */
    {
      int ii;

      /* Swap and align the symbol into a reasonable C structure.  */
      bfd_coff_swap_sym_in (abfd, raw_symbol, symbol);

      cs->c_symnum = symnum;
      cs->c_nsyms = symbol->n_numaux;
      if (symbol->n_zeroes) {
	symname_alloced = 0;
	/* We must use the original, unswapped, name here so the name field
	   pointed to by cs->c_name will persist throughout xcoffread.  If
	   we use the new field, it gets overwritten for each symbol.  */
	cs->c_name = ((struct external_syment *)raw_symbol)->e.e_name;
      } else if (symbol->n_sclass & 0x80) {
	cs->c_name = debugsec + symbol->n_offset;
	symname_alloced = 0;
      } else {	/* in string table */
	cs->c_name = strtbl + (int)symbol->n_offset;
	symname_alloced = 1;
      }
      cs->c_value = symbol->n_value;
      cs->c_sclass = symbol->n_sclass & 0xff;
      cs->c_secnum = symbol->n_scnum;
      cs->c_type = (unsigned)symbol->n_type;

      raw_symbol += coff_data (abfd)->local_symesz;
      ++symnum;

      raw_auxptr = raw_symbol;		/* Save addr of first aux entry */

      /* Skip all the auxents associated with this symbol.  */
      for (ii = symbol->n_numaux; ii; --ii ) {
        raw_symbol += coff_data (abfd)->local_auxesz;
	++symnum;
      }
    }

    /* if symbol name starts with ".$" or "$", ignore it. */
    if (cs->c_name[0] == '$' || (cs->c_name[1] == '$' && cs->c_name[0] == '.'))
      continue;

    if (cs->c_symnum == next_file_symnum && cs->c_sclass != C_FILE) {
      if (last_source_file)
	end_symtab (cur_src_end_addr, 1, 1, objfile);

      start_symtab ("_globals_", (char *)NULL, (CORE_ADDR)0);
      cur_src_end_addr = first_object_file_end;
      /* done with all files, everything from here on is globals */
    }

    /* if explicitly specified as a function, treat is as one. */
    if (ISFCN(cs->c_type) && cs->c_sclass != C_TPDEF) {
      bfd_coff_swap_aux_in (abfd, raw_auxptr, cs->c_type, cs->c_sclass,
			    main_aux);
      goto function_entry_point;
    }

    if ((cs->c_sclass == C_EXT || cs->c_sclass == C_HIDEXT) && cs->c_nsyms == 1)
    {
	/* dealing with a symbol with a csect entry. */

#   define	CSECT(PP)	((PP)->x_csect)
#   define	CSECT_LEN(PP)	(CSECT(PP).x_scnlen)
#   define	CSECT_ALIGN(PP)	(SMTYP_ALIGN(CSECT(PP).x_smtyp))
#   define	CSECT_SMTYP(PP)	(SMTYP_SMTYP(CSECT(PP).x_smtyp))
#   define	CSECT_SCLAS(PP)	(CSECT(PP).x_smclas)

	/* Convert the auxent to something we can access.  */
        bfd_coff_swap_aux_in (abfd, raw_auxptr, cs->c_type, cs->c_sclass,
			      main_aux);

	switch (CSECT_SMTYP (main_aux)) {

	case XTY_ER :
	  continue;			/* ignore all external references. */

	case XTY_SD :			/* a section description. */
	  {
	    switch (CSECT_SCLAS (main_aux)) {

	    case XMC_PR :			/* a `.text' csect.	*/
	      {

		/* A program csect is seen.
		 
		   We have to allocate one symbol table for each program csect. Normally
		   gdb prefers one symtab for each compilation unit (CU). In case of AIX, one
		   CU might include more than one prog csect, and they don't have to be
		   adjacent in terms of the space they occupy in memory. Thus, one single
		   CU might get fragmented in the memory and gdb's file start and end address
		   approach does not work!  */

		if (last_seen_csect) {
		  complete_symtab (filestring, file_start_addr);
		  cur_src_end_addr = file_end_addr;
		  end_symtab (file_end_addr, 1, 1, objfile);
		  start_symtab ((char *)NULL, (char *)NULL, (CORE_ADDR)0);
		}

		/* If this is the very first csect seen, basically `__start'. */
		if (just_started) {
		  first_object_file_end = cs->c_value + CSECT_LEN (main_aux);
		  just_started = 0;
		}

		file_start_addr = cs->c_value;
		file_end_addr = cs->c_value + CSECT_LEN (main_aux);

		if (cs->c_name && cs->c_name[0] == '.') {
		  last_seen_csect = cs->c_name;
		  RECORD_MISC_FUNCTION (cs->c_name, cs->c_value, mf_text, symname_alloced);
		}
	      }
	      continue;

	    case XMC_RW :
	      break;

	      /* If the section is not a data description, ignore it. Note that
		 uninitialized data will show up as XTY_CM/XMC_RW pair. */

	    case XMC_TC0:
#ifdef XCOFF_ADD_TOC_TO_LOADINFO
	      XCOFF_ADD_TOC_TO_LOADINFO (cs->c_value);
#endif
	      /* fall down to default case. */

	    case XMC_TC	:		/* ignore toc entries	*/
	    default	:		/* any other XMC_XXX	*/
	      continue;
	    }
	  }
	  break;			/* switch CSECT_SCLAS() */

	case XTY_LD :
	  
	  /* a function entry point. */
	  if (CSECT_SCLAS (main_aux) == XMC_PR) {

function_entry_point:
	    if (cs->c_value != last_recorded_fun)
	      RECORD_MISC_FUNCTION (cs->c_name, cs->c_value, mf_text, 
	      						symname_alloced);

	    fcn_line_offset = main_aux->x_sym.x_fcnary.x_fcn.x_lnnoptr;
	    fcn_start_addr = cs->c_value;

	    /* save the function header info, which will be used
	       when `.bf' is seen. */
	    fcn_cs_saved = *cs;
	    fcn_aux_saved = *main_aux;
	    continue;
	  }

	  /* shared library function entry point. */
	  else if (CSECT_SCLAS (main_aux) == XMC_GL) {
	    if (last_recorded_fun != cs->c_value)
	      RECORD_MISC_FUNCTION (cs->c_name, cs->c_value, mf_text,
	      						symname_alloced);
	    continue;
	  }
	  break;

	default :		/* all other XTY_XXXs */
	  break;
	}			/* switch CSECT_SMTYP() */
    }

    switch (cs->c_sclass) {

    case C_FILE:

      /* c_value field contains symnum of next .file entry in table
	 or symnum of first global after last .file. */

      next_file_symnum = cs->c_value;

      /* complete symbol table for last object file containing
	 debugging information. */

      /* Whether or not there was a csect in the previous file, we have 
	 to call `end_symtab' and `start_symtab' to reset type_vector, 
	 line_vector, etc. structures. */
      complete_symtab (filestring, file_start_addr);
      cur_src_end_addr = file_end_addr;
      end_symtab (file_end_addr, 1, 1, objfile);
      start_symtab (cs->c_name, (char *)NULL, (CORE_ADDR)0);
      last_seen_csect = 0;

      /* reset file start and end addresses. A compilation unit with no text
         (only data) should have zero file boundaries. */
      file_start_addr = file_end_addr = 0;

      filestring = cs->c_name;
      break;


    case C_FCN:
      if (strcmp (cs->c_name, ".bf") == 0) {

        bfd_coff_swap_aux_in (abfd, raw_auxptr, cs->c_type, cs->c_sclass,
			      main_aux);

	within_function = 1;

	/* value contains address of first non-init type code */
	/* main_aux.x_sym.x_misc.x_lnsz.x_lnno
	   contains line number of '{' } */
	fcn_first_line = main_aux->x_sym.x_misc.x_lnsz.x_lnno;

	/* Linenos are now processed on a file-by-file, not fn-by-fn, basis.
	   Metin did it, I'm not sure why.  FIXME.  -- gnu@cygnus.com */
	/* mark_first_line (fcn_line_offset, cs->c_symnum); */

	new = push_context (0, fcn_start_addr);
	new->name = process_xcoff_symbol (&fcn_cs_saved);
      }
      else if (strcmp (cs->c_name, ".ef") == 0) {

        bfd_coff_swap_aux_in (abfd, raw_auxptr, cs->c_type, cs->c_sclass,
			      main_aux);

	/* the value of .ef is the address of epilogue code;
	   not useful for gdb */
	/* { main_aux.x_sym.x_misc.x_lnsz.x_lnno
	   contains number of lines to '}' */

	fcn_last_line = main_aux->x_sym.x_misc.x_lnsz.x_lnno;
#if 0
	enter_linenos (fcn_line_offset, fcn_first_line, 
					fcn_first_line + fcn_last_line);
#endif

	new = pop_context ();
	if (context_stack_depth != 0)
	  error ("invalid symbol data; .bf/.ef/.bb/.eb symbol mismatch, at symbol %d.",
	      symnum);

	finish_block (new->name, &local_symbols, new->old_blocks,
	    new->start_addr,
	    fcn_cs_saved.c_value +
	    fcn_aux_saved.x_sym.x_misc.x_fsize);
	within_function = 0;
      }
      break;

    case C_BSTAT	:		/* begin static block	*/
      static_block_base = read_symbol_nvalue (symtbl, cs->c_value);
      break;

    case C_ESTAT	:		/* end of static block	*/
      static_block_base = 0;
      break;

    case C_ARG		:		/* These are not implemented. */
    case C_REGPARM	:
    case C_TPDEF	:
    case C_STRTAG	:
    case C_UNTAG	:
    case C_ENTAG	:
      printf ("ERROR: Unimplemented storage class: %d.\n", cs->c_sclass);
      break;

    case C_HIDEXT	:		/* ignore these.. */
    case C_LABEL	:
    case C_NULL		:
      break;

    case C_BINCL	:		/* beginning of include file */
	push_subfile ();
  	start_subfile (cs->c_name, (char *)0);
  	fcn_first_line = cs->c_value;	/* Offset to first lineno of file */
	break;

    case C_EINCL	:		/* end of include file */
	fcn_last_line = cs->c_value;	/* Offset to last line number */
	{ long dummy = 0;
	enter_line_range (current_subfile, fcn_first_line, cs->c_value, 0,
			  &dummy);
	}
	start_subfile (pop_subfile (), (char *)0);
	break;

    case C_BLOCK	:
      if (strcmp (cs->c_name, ".bb") == 0) {
	depth++;
	new = push_context (depth, cs->c_value);
      }
      else if (strcmp (cs->c_name, ".eb") == 0) {
	new = pop_context ();
	if (depth != new->depth)
	  error ("Invalid symbol data: .bb/.eb symbol mismatch at symbol %d.",
			 symnum);

	depth--;
	if (local_symbols && context_stack_depth > 0) {
	  /* Make a block for the local symbols within.  */
	  finish_block (new->name, &local_symbols, new->old_blocks,
				  new->start_addr, cs->c_value);
	}
	local_symbols = new->locals;
      }
      break;

    default		:
      (void) process_xcoff_symbol (cs);
      break;
    }

  } /* while */

  if (last_source_file)
    end_symtab (cur_src_end_addr, 1, 1, objfile);

  free (symtbl);
}

#define	SYMBOL_DUP(SYMBOL1, SYMBOL2)	\
  (SYMBOL2) = (struct symbol *)		\
  	obstack_alloc (symbol_obstack, sizeof (struct symbol)); \
  *(SYMBOL2) = *(SYMBOL1);
  
 
#define	SYMNAME_ALLOC(NAME, ALLOCED)	\
  (ALLOCED) ? (NAME) : obstack_copy0 (symbol_obstack, (NAME), strlen (NAME));



/* process one xcoff symbol. */

static struct symbol *
process_xcoff_symbol (cs)
  register struct coff_symbol *cs;
{
  struct symbol onesymbol;
  register struct symbol *sym = &onesymbol;
  struct symbol *sym2 = NULL;
  struct type *ttype;
  char *name, *pp, *qq;

  name = cs->c_name;
  if (name[0] == '.')
    ++name;

  bzero (sym, sizeof (struct symbol));

  /* default assumptions */
  SYMBOL_VALUE (sym) = cs->c_value;
  SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;

  if (ISFCN (cs->c_type)) {

    /* At this point, we don't know the type of the function and assume it 
       is int. This will be patched with the type from its stab entry later 
       on in patch_block_stabs () */

    SYMBOL_NAME (sym) = SYMNAME_ALLOC (name, symname_alloced);
    SYMBOL_TYPE (sym) = lookup_function_type (builtin_type_int);

    SYMBOL_CLASS (sym) = LOC_BLOCK;
    SYMBOL_DUP (sym, sym2);

    if (cs->c_sclass == C_EXT)
      add_symbol_to_list (sym2, &global_symbols);
    else if (cs->c_sclass == C_HIDEXT || cs->c_sclass == C_STAT)
      add_symbol_to_list (sym2, &file_symbols);
  }

  else {

    /* in case we can't figure out the type, default is `int'. */
    SYMBOL_TYPE (sym) = builtin_type_int;

    switch (cs->c_sclass)
    {
    case C_FUN:
      if (fcn_cs_saved.c_sclass == C_EXT)
	add_stab_to_list (name, &global_stabs);
      else
	add_stab_to_list (name, &file_stabs);
      break;

    case C_DECL:      			/* a type decleration?? */
	qq =  (char*) index (name, ':');
	if (!qq)			/* skip if there is no ':' */
	  return NULL;

	pp = qq + 2;
	ttype = SYMBOL_TYPE (sym) = read_type (&pp);

	/* read_type() will return null if type (or tag) definition was
	   unnnecessarily duplicated. Also, if the symbol doesn't have a name,
	   there is no need to keep it in symbol table. */

	if (!ttype || name == qq)
	  return NULL;

	if (qq[1] == 'T')
	  SYMBOL_NAMESPACE (sym) = STRUCT_NAMESPACE;
	else if (qq[1] == 't')
	  SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
	else {
	  printf ("ERROR: Unrecognized stab string.\n");
	  return NULL;
	}

	SYMBOL_CLASS (sym) = LOC_TYPEDEF;
	SYMBOL_NAME (sym) = obsavestring (name, qq-name);

	if  (SYMBOL_NAMESPACE (sym) == STRUCT_NAMESPACE)
	    TYPE_NAME (ttype) = concat (
		TYPE_CODE (ttype) == TYPE_CODE_UNION ? "union " :
		TYPE_CODE (ttype) == TYPE_CODE_STRUCT? "struct " : "enum ",
		SYMBOL_NAME (sym), "");

	else if (!TYPE_NAME (ttype))      /* else, regular typedef. */
	    TYPE_NAME (ttype) = SYMBOL_NAME (sym);

	SYMBOL_DUP (sym, sym2);
	add_symbol_to_list 
	   (sym2, within_function ? &local_symbols : &file_symbols);
	break;

    case C_GSYM:
      add_stab_to_list (name, &global_stabs);
      break;

    case C_PSYM:
	if (*name == ':' || (pp = (char *) index (name, ':')) == NULL)
	  return NULL;
	SYMBOL_NAME (sym) = obsavestring (name, pp-name);
	SYMBOL_CLASS (sym) = LOC_ARG;
	pp += 2;
	SYMBOL_TYPE (sym) = read_type (&pp);
	SYMBOL_DUP (sym, sym2);
	add_symbol_to_list (sym2, &local_symbols);
	break;

    case C_STSYM:
	if (*name == ':' || (pp = (char *) index (name, ':')) == NULL)
	  return NULL;
	SYMBOL_NAME (sym) = obsavestring (name, pp-name);
	SYMBOL_CLASS (sym) = LOC_STATIC;
	SYMBOL_VALUE (sym) += static_block_base;
	pp += 2;
	SYMBOL_TYPE (sym) = read_type (&pp);
	SYMBOL_DUP (sym, sym2);
	add_symbol_to_list 
	   (sym2, within_function ? &local_symbols : &file_symbols);
	break;

    case C_LSYM:
	if (*name == ':' || (pp = (char *) index (name, ':')) == NULL)
	  return NULL;
	SYMBOL_NAME (sym) = obsavestring (name, pp-name);
	SYMBOL_CLASS (sym) = LOC_LOCAL;
	pp += 1;
	SYMBOL_TYPE (sym) = read_type (&pp);
	SYMBOL_DUP (sym, sym2);
	add_symbol_to_list (sym2, &local_symbols);
	break;

    case C_AUTO:
      SYMBOL_CLASS (sym) = LOC_LOCAL;
      SYMBOL_NAME (sym) = SYMNAME_ALLOC (name, symname_alloced);
      SYMBOL_DUP (sym, sym2);
      add_symbol_to_list (sym2, &local_symbols);
      break;

    case C_EXT:
      SYMBOL_CLASS (sym) = LOC_STATIC;
      SYMBOL_NAME (sym) = SYMNAME_ALLOC (name, symname_alloced);
      SYMBOL_DUP (sym, sym2);
      add_symbol_to_list (sym2, &global_symbols);
      break;

    case C_STAT:
      SYMBOL_CLASS (sym) = LOC_STATIC;
      SYMBOL_NAME (sym) = SYMNAME_ALLOC (name, symname_alloced);
      SYMBOL_DUP (sym, sym2);
      add_symbol_to_list 
	   (sym2, within_function ? &local_symbols : &file_symbols);
      break;

    case C_REG:
      printf ("ERROR! C_REG is not fully implemented!\n");
      SYMBOL_CLASS (sym) = LOC_REGISTER;
      SYMBOL_NAME (sym) = SYMNAME_ALLOC (name, symname_alloced);
      SYMBOL_DUP (sym, sym2);
      add_symbol_to_list (sym2, &local_symbols);
      break;

    case C_RSYM:
	pp = (char*) index (name, ':');
	SYMBOL_CLASS (sym) = LOC_REGISTER;
	SYMBOL_VALUE (sym) = STAB_REG_TO_REGNUM (cs->c_value);
	if (pp) {
	  SYMBOL_NAME (sym) = obsavestring (name, pp-name);
	  pp += 2;
	  if (*pp)
	    SYMBOL_TYPE (sym) = read_type (&pp);
	}
	else
	  /* else this is not a stab entry, suppose the type is either
	     `int' or `float', depending on the register class. */

	  SYMBOL_TYPE (sym) = (SYMBOL_VALUE (sym) < 32) ? 
	      builtin_type_int : builtin_type_float;

	SYMBOL_DUP (sym, sym2);
	add_symbol_to_list (sym2, &local_symbols);
	break;

    default	:
      printf ("ERROR: Unexpected storage class: %d.\n", cs->c_sclass);
      return NULL;
    }
  }
  return sym2;
}


static int
read_symbol_nvalue (symtable, symno)
     char *symtable;
     int symno;
{
  struct internal_syment symbol[1];

  bfd_coff_swap_sym_in (symfile_bfd, symtable + (symno*local_symesz), symbol);
  return symbol->n_value;  
}


static int
read_symbol_lineno (symtable, symno)
  char *symtable;
  int symno;
{
  struct internal_syment symbol[1];
  union internal_auxent main_aux[1];

  int ii;

  for (ii = 0; ii < 50; ii++) {
    bfd_coff_swap_sym_in (symfile_bfd,
			     symtable + (symno*local_symesz), symbol);
    if (symbol->n_sclass == C_FCN && 0 == strcmp (symbol->n_name, ".bf"))
      goto gotit;
    symno += 1 + symbol->n_numaux+1;
  }

  printf ("GDB Error: `.bf' not found.\n");
  return 0;

gotit:
  /* take aux entry and return its lineno */
  symno++;
  bfd_coff_swap_aux_in (symfile_bfd, symtable+(symno*local_symesz),
			symbol->n_type, symbol->n_sclass, main_aux);

  return main_aux->x_sym.x_misc.x_lnsz.x_lnno;
}

/* Support for line number handling */

/* This function is called for every section; it finds the outer limits
 * of the line table (minimum and maximum file offset) so that the
 * mainline code can read the whole thing for efficiency.
 */
static void
find_linenos(abfd, asect, vpinfo)
bfd *abfd;
sec_ptr asect;
void *vpinfo; 
{
  struct coff_symfile_info *info;
  int size, count;
  file_ptr offset, maxoff;

  count = asect->lineno_count;

  if (count == 0)
    return;

  size   = count * coff_data (symfile_bfd)->local_linesz;
  info   = (struct coff_symfile_info *)vpinfo;
  offset = asect->line_filepos;
  maxoff = offset + size;

  if (offset < info->min_lineno_offset || info->min_lineno_offset == 0)
    info->min_lineno_offset = offset;

  if (maxoff > info->max_lineno_offset)
    info->max_lineno_offset = maxoff;
}


/* Read in all the line numbers for fast lookups later.  Leave them in
   external (unswapped) format in memory; we'll swap them as we enter
   them into GDB's data structures.  */

static int
init_lineno (abfd, offset, size)
     bfd *abfd;
     long offset;
     int size;
{
  int val;

  if (bfd_seek(abfd, offset, 0) < 0)
    return -1;

  linetab = (char *) xmalloc(size);

  val = bfd_read(linetab, 1, size, abfd);
  if (val != size)
    return -1;

  linetab_offset = offset;
  linetab_size = size;
  make_cleanup(free, linetab);	/* Be sure it gets de-allocated. */
  return 0;
}


void
dump_strtbl ()
{
  int ii;
  printf ("===STRING TABLE DUMP...\n\n");
  for ( ii=0; ii < strtbl_len; ++ii )
    printf ("%c", isprint (*(strtbl+ii)) ? *(strtbl+ii) : ' ');
  printf ("\n");
}

void
dump_linetable (ltb)
     struct linetable *ltb;
{
  int ii;
  for (ii=0; ii < ltb->nitems; ++ii)
    printf ("line: %d, addr: 0x%x\n", ltb->item[ii].line, ltb->item[ii].pc);
}

void
dump_type (typeP)
     struct type *typeP;
{
  printf ("0x%x: name: %s\n", typeP, typeP->name ? typeP->name : "(nil)");
}

char *dump_namespace ();
char *dump_addrclass ();

void
dump_symbol (pp)
     struct symbol *pp;
{
  printf (" sym: %s\t%s,\t%s\ttype: 0x%x, val: 0x%x end: 0x%x\n", 
      pp->name, dump_namespace (pp->namespace),
      dump_addrclass (pp->class), pp->type,
      SYMBOL_CLASS(pp) == LOC_BLOCK ? BLOCK_START(SYMBOL_BLOCK_VALUE(pp))
      : pp->value.value,
      SYMBOL_CLASS(pp) == LOC_BLOCK ? BLOCK_END(SYMBOL_BLOCK_VALUE(pp)) : 0);
}


char *
dump_namespace (ns)
int ns;
{
  static char *ns_name [] = { 
    "UNDEF_NS", "VAR_NS", "STRUCT_NS", "LABEL_NS"};

  switch (ns) {
  case UNDEF_NAMESPACE:
  case VAR_NAMESPACE:
  case STRUCT_NAMESPACE:
  case LABEL_NAMESPACE:
    return ns_name[ns];
  }
 
  return "***ERROR***";
}


char *
dump_addrclass (ac)
int ac;						/* address class */
{
  static char *ac_name [] = {
    "LOC_UNDEF",
    "LOC_CONST",
    "LOC_STATIC",
    "LOC_REGISTER",
    "LOC_ARG",
    "LOC_REF_ARG",
    "LOC_REGPARM",
    "LOC_LOCAL",
    "LOC_TYPEDEF",
    "LOC_LABEL",
    "LOC_BLOCK",
    "LOC_CONST_BYTES",
    "LOC_LOCAL_ARG",
  };
  switch (ac) {
  case LOC_UNDEF:
  case LOC_CONST:
  case LOC_STATIC:
  case LOC_REGISTER:
  case LOC_ARG:
  case LOC_REF_ARG:
  case LOC_REGPARM:
  case LOC_LOCAL:
  case LOC_TYPEDEF:
  case LOC_LABEL:
  case LOC_BLOCK:
  case LOC_CONST_BYTES:
  case LOC_LOCAL_ARG:
    return ac_name [ac];
  }
  return "***ERROR***";
}

void
dump_block (pp)
     struct block *pp;
{
  int ii;
  printf ("BLOCK..: start: 0x%x, end: 0x%x\n", pp->startaddr, pp->endaddr);
  for (ii=0; ii < pp->nsyms; ++ii)
    dump_symbol (pp->sym[ii]);
}

void
dump_blockvector (pp)
     struct blockvector *pp;
{
  int ii;
  for (ii=0; ii < pp->nblocks; ++ii)
    dump_block (pp->block [ii]);
}


void
dump_last_symtab (pp)
     struct symtab *pp;
{
  for ( ; pp; pp = pp->next) {
    if ( pp->next == 0 ) {
      printf ("SYMTAB NAME: %s\n", pp->filename);
      dump_blockvector (pp->blockvector);
    }
  }
}

void
dump_symtabs (pp)
     struct symtab *pp;
{
  for ( ; pp; pp = pp->next) {
    printf ("SYMTAB NAME: %s\n", pp->filename ? pp->filename : "(nil)");
/*    if (pp->linetable)
      dump_linetable (pp->linetable); */
    dump_blockvector (pp->blockvector);
  }
}

void
dump_symtab_lines (pp)
     struct symtab *pp;
{
  for ( ; pp; pp = pp->next) {
    printf ("SYMTAB NAME: %s\n", pp->filename ? pp->filename : "(nil)");
    if (pp->linetable)
      dump_linetable (pp->linetable);
    /* dump_blockvector (pp->blockvector); */
  }
}

void
dump_misc_funcs ()
{
  int ii;
  for (ii=0; ii < misc_function_count; ++ii)
    printf ("name: %s, addr: 0x%x\n", 
	misc_function_vector[ii].name, 
	misc_function_vector[ii].address);
}


/* dbx allows the text of a symbol name to be continued into the
   next symbol name!  When such a continuation is encountered
   (a \ at the end of the text of a name)
   call this function to get the continuation.  */
/* So far, I haven't seen this happenning xlc output. I doubt we'll need this
   for aixcoff. */

#define	next_symbol_text() \
  printf ("Gdb Error: symbol names on multiple lines not implemented.\n")


/* xlc/dbx combination uses a set of builtin types, starting from -1. return
   the proper type node fora given builtin type #. */

struct type *
builtin_type (pp)
char **pp;
{
  int typenums[2];

  if (**pp != '-') {
    printf ("ERROR!, unknown built-in type!\n");
    return NULL;
  }
  *pp += 1;
  read_type_number (pp, typenums);

  /* default types are defined in dbxstclass.h. */
  switch ( typenums[1] ) {
  case 1: 
    return builtin_type_int;
  case 2: 
    return builtin_type_char;
  case 3: 
    return builtin_type_short;
  case 4: 
    return builtin_type_long;
  case 5: 
    return builtin_type_unsigned_char;
  case 6: 
    return builtin_type_char;	   /* requires a builtin `signed char' */
  case 7: 
    return builtin_type_unsigned_short;
  case 8: 
    return builtin_type_unsigned_int;
  case 9: 
    return builtin_type_unsigned_int;
  case 10: 
    return builtin_type_unsigned_long;
  case 11: 
    return builtin_type_void;
  case 12: 
    return builtin_type_float;
  case 13: 
    return builtin_type_double;
  case 14: 
    return builtin_type_double;   /* requires a builtin `long double' */
  case 15: 
    return builtin_type_int;	   /* requires a builtin `integer' */
  case 16: 
    return builtin_type_int;	   /* requires builtin `boolean' */
  case 17: 
    return builtin_type_float;	   /* requires builtin `short real' */
  case 18: 
    return builtin_type_float;	   /* requires builtin `real' */
  default :
    printf ("ERROR! Unknown builtin type -%d\n", typenums[1]);
    return NULL;
  }
}

/* if we now nothing about a function but its address, make a function symbol
   out of it with the limited knowladge you have. This will be used when
   somebody refers to a function, which doesn't exist in the symbol table,
   but in misc_function_vector. */

struct symbol *
build_function_symbol (ind)
     int ind;
{
  struct symbol *sym =
  (struct symbol *) obstack_alloc (symbol_obstack, sizeof (struct symbol));
  SYMBOL_NAME (sym) = misc_function_vector[ind].name;
  /*   SYMBOL_VALUE (sym) = misc_function_vector[ind].address; */
  SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
  SYMBOL_CLASS (sym) = LOC_BLOCK;
  SYMBOL_TYPE (sym) = lookup_function_type (builtin_type_int);
  SYMBOL_BLOCK_VALUE (sym) = (struct block *)
      obstack_alloc (symbol_obstack, sizeof (struct block));
  BLOCK_START (SYMBOL_BLOCK_VALUE (sym)) = misc_function_vector[ind].address;
  return sym;
}


void
aixcoff_new_init ()
{
  /* Nothin' to do.  */
}

void
aixcoff_symfile_init (sf)
struct sym_fns *sf;
{
  bfd *abfd = sf->sym_bfd;

  /* Allocate struct to keep track of the symfile */
  /* FIXME memory leak */
  sf->sym_private = xmalloc(sizeof (struct coff_symfile_info));

  /*
   * Save startup file's range of PC addresses to help
   * blockframe.c decide where the bottom of the stack is.
   */
  if (bfd_get_file_flags(abfd) & EXEC_P) {
    entry_point = bfd_get_start_address(abfd);
  } else {
    entry_point = ~0;
    /* set the startup file to be an empty range.  */
    startup_file_start = 0;
    startup_file_end = 0;
  }
}


static int
init_stringtab(abfd, offset)
     bfd *abfd;
     long offset;
{
  long length;
  int val;
  unsigned char lengthbuf[4];

  if (bfd_seek(abfd, offset, 0) < 0)
    return -1;

  val    = bfd_read((char *)lengthbuf, 1, sizeof lengthbuf, abfd);
  length = bfd_h_get_32(abfd, lengthbuf);

  /* If no string table is needed, then the file may end immediately
     after the symbols.  Just return with `strtbl' set to null. */

  if (val != sizeof length || length < sizeof length)
    return 0;

  /* Allocate string table from symbol_obstack. We will need this table
     as long as we have its symbol table around. */

  strtbl = (char*) obstack_alloc (symbol_obstack, length);
  if (strtbl == NULL)
    return -1;

  bcopy(&length, strtbl, sizeof length);
  if (length == sizeof length)
    return 0;

  val = bfd_read(strtbl + sizeof length, 1, length - sizeof length, abfd);

  if (val != length - sizeof length || strtbl[length - 1] != '\0')
    return -1;

  return 0;
}

static int
init_debugsection(abfd)
     bfd *abfd;
{
  register sec_ptr secp;
  bfd_size_type length;

  if (debugsec) {
    free(debugsec);
    debugsec = NULL;
  }

  secp = bfd_get_section_by_name(abfd, ".debug");
  if (!secp)
    return 0;

  if (!(length = bfd_section_size(abfd, secp)))
    return 0;

  debugsec = (void *) xmalloc ((unsigned)length);
  if (debugsec == NULL)
    return -1;

  if (!bfd_get_section_contents(abfd, secp, debugsec, (file_ptr) 0, length)) {
    printf ("Can't read .debug section from symbol file\n");
    return -1;
  }
  return 0;
}

static void
free_debugsection()
{
  if (debugsec)
    free(debugsec);
  debugsec = NULL;
}


/* aixcoff version of symbol file read. */

void
aixcoff_symfile_read (sf, addr, mainline)
  struct sym_fns *sf;
  CORE_ADDR addr;
  int mainline;
{
  int num_symbols;				/* # of symbols */
  int symtab_offset;				/* symbol table and */
  int stringtab_offset;				/* string table file offsets */
  int val;
  bfd *abfd;
  struct coff_symfile_info *info = (void*) sf->sym_private;
  char *name;

  symfile_bfd = abfd = sf->objfile->obfd;
  name = sf->objfile->name;

  num_symbols = bfd_get_symcount (abfd);	/* # of symbols */
  symtab_offset = obj_sym_filepos (abfd);	/* symbol table file offset */
  stringtab_offset = symtab_offset +
    num_symbols * coff_data(abfd)->local_symesz;

  info->min_lineno_offset = 0;
  info->max_lineno_offset = 0;
  bfd_map_over_sections (abfd, find_linenos, info);

  /* FIXME!  This stuff should move into symfile_init */
  if (info->min_lineno_offset != 0
      && info->max_lineno_offset > info->min_lineno_offset) {

    /* only read in the line # table if one exists */
    val = init_lineno(abfd, info->min_lineno_offset,
	info->max_lineno_offset - info->min_lineno_offset);

    if (val < 0)
      error("\"%s\": error reading line numbers\n", name);
  }

  val = init_stringtab(abfd, stringtab_offset);
  if (val < 0) {
    error ("\"%s\": can't get string table", name);
  }

  if (init_debugsection(abfd) < 0) {
    error ("Error reading .debug section of `%s'\n", name);
  }

  /* Position to read the symbol table.  Do not read it all at once. */
  val = bfd_seek(abfd, (long)symtab_offset, 0);
  if (val < 0)
    perror_with_name(name);

  if (bfd_tell(abfd) != symtab_offset)
    fatal("bfd? BFD!");

  init_misc_bunches ();
  make_cleanup(discard_misc_bunches, 0);

#ifdef XCOFF_INIT_LOADINFO
  if (mainline)
    XCOFF_INIT_LOADINFO ();
#endif

  /* Now that the executable file is positioned at symbol table,
     process it and define symbols accordingly. */

  read_xcoff_symtab(sf->objfile, num_symbols);

  make_cleanup(free_debugsection, 0);

  /* Sort symbols alphabetically within each block.  */
  sort_syms ();

  /* Go over the misc functions and install them in vector.  */
  condense_misc_bunches (!mainline);

  /* Make a default for file to list.  */
  select_source_symtab (0);
}

/* Register our ability to parse symbols for aixcoff BFD files. */

static struct sym_fns aixcoff_sym_fns =
{
  "aixcoff-rs6000", 15,
  aixcoff_new_init, aixcoff_symfile_init,
  aixcoff_symfile_read, 
};

void
_initialize_xcoffread ()
{
  add_symtab_fns(&aixcoff_sym_fns);
}
