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

/* This module provides three functions: mipscoff_symfile_init,
   which initializes to read a symbol file; mipscoff_new_init, which
   discards existing cached information when all symbols are being
   discarded; and mipscoff_symfile_read, which reads a symbol table
   from a file.

   mipscoff_symfile_read only does the minimum work necessary for letting the
   user "name" things symbolically; it does not read the entire symtab.
   Instead, it reads the external and static symbols and puts them in partial
   symbol tables.  When more extensive information is requested of a
   file, the corresponding partial symbol table is mutated into a full
   fledged symbol table by going back and reading the symbols
   for real.  mipscoff_psymtab_to_symtab() is called indirectly through
   a pointer in the psymtab to do this.

   ECOFF symbol tables are mostly written in the byte order of the
   target machine.  However, one section of the table (the auxiliary
   symbol information) is written in the host byte order.  There is a
   bit in the other symbol info which describes which host byte order
   was used.  ECOFF thereby takes the trophy from Intel `b.out' for
   the most brain-dead adaptation of a file format to byte order.

   This module can read all four of the known byte-order combinations,
   on any type of host.  */

#include "defs.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcore.h"
#include "symfile.h"
#include "objfiles.h"
#include "obstack.h"
#include "buildsym.h"
#include "stabsread.h"
#include "complaints.h"

/* These are needed if the tm.h file does not contain the necessary
   mips specific definitions.  */

#ifndef MIPS_EFI_SYMBOL_NAME
#define MIPS_EFI_SYMBOL_NAME "__GDB_EFI_INFO__"
#include "coff/sym.h"
#include "coff/symconst.h"
typedef struct mips_extra_func_info {
        long    numargs;
        PDR     pdr;
} *mips_extra_func_info_t;
#ifndef RA_REGNUM
#define RA_REGNUM 0
#endif
#ifndef FP0_REGNUM
#define FP0_REGNUM 0
#endif
#endif

#ifdef USG
#include <sys/types.h>
#endif

#include <sys/param.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <string.h>

#include "gdb-stabs.h"

#include "bfd.h"

#include "coff/internal.h"
#include "coff/mips.h"		/* COFF-like aspects of ecoff files */
#include "coff/ecoff-ext.h"	/* External forms of ecoff sym structures */

/* FIXME: coff/internal.h and aout/aout64.h both define N_ABS.  We
   want the definition from aout/aout64.h.  */
#undef	N_ABS
/* FIXME: coff/mips.h and aout/aout64.h both define ZMAGIC.  We don't
   use it.  */
#undef	ZMAGIC

#include "libaout.h"		/* Private BFD a.out information.  */
#include "aout/aout64.h"
#include "aout/stab_gnu.h"	/* STABS information */

/* FIXME: libcoff.h and libaout.h both define a couple of macros.  We
   don't use them.  */
#undef exec_hdr
#undef obj_sym_filepos

#include "libcoff.h"		/* Private BFD COFF information.  */
#include "libecoff.h"		/* Private BFD ECOFF information.  */

#include "expression.h"
#include "language.h"		/* Needed inside partial-stab.h */

/* Each partial symbol table entry contains a pointer to private data
   for the read_symtab() function to use when expanding a partial
   symbol table entry to a full symbol table entry.

   For mipsread this structure contains the index of the FDR that this
   psymtab represents and a pointer to the BFD that the psymtab was
   created from.  */

#define PST_PRIVATE(p) ((struct symloc *)(p)->read_symtab_private)
#define FDR_IDX(p) (PST_PRIVATE(p)->fdr_idx)
#define CUR_BFD(p) (PST_PRIVATE(p)->cur_bfd)

struct symloc
{
  int fdr_idx;
  bfd *cur_bfd;
  EXTR *extern_tab;		/* Pointer to external symbols for this file. */
  int extern_count;		/* Size of extern_tab. */
  struct mips_pending **pending_list;
};

/* Things we import explicitly from other modules */

extern int info_verbose;

/* Various complaints about symbol reading that don't abort the process */

struct complaint bad_file_number_complaint =
{"bad file number %d", 0, 0};

struct complaint index_complaint =
{"bad aux index at symbol %s", 0, 0};

struct complaint aux_index_complaint =
{"bad proc end in aux found from symbol %s", 0, 0};

struct complaint block_index_complaint =
{"bad aux index at block symbol %s", 0, 0};

struct complaint unknown_ext_complaint =
{"unknown external symbol %s", 0, 0};

struct complaint unknown_sym_complaint =
{"unknown local symbol %s", 0, 0};

struct complaint unknown_st_complaint =
{"with type %d", 0, 0};

struct complaint block_overflow_complaint =
{"block containing %s overfilled", 0, 0};

struct complaint basic_type_complaint =
{"cannot map MIPS basic type 0x%x", 0, 0};

struct complaint unknown_type_qual_complaint =
{"unknown type qualifier 0x%x", 0, 0};

struct complaint array_bitsize_complaint =
{"size of array target type not known, assuming %d bits", 0, 0};

struct complaint bad_tag_guess_complaint =
{"guessed tag type of %s incorrectly", 0, 0};

struct complaint block_member_complaint =
{"declaration block contains unhandled symbol type %d", 0, 0};

struct complaint stEnd_complaint =
{"stEnd with storage class %d not handled", 0, 0};

struct complaint unknown_mips_symtype_complaint =
{"unknown symbol type 0x%x", 0, 0};

struct complaint stab_unknown_complaint =
{"unknown stabs symbol %s", 0, 0};

struct complaint pdr_for_nonsymbol_complaint =
{"PDR for %s, but no symbol", 0, 0};

struct complaint pdr_static_symbol_complaint =
{"can't handle PDR for static proc at 0x%x", 0, 0};

struct complaint bad_setjmp_pdr_complaint =
{"fixing bad setjmp PDR from libc", 0, 0};

/* Macros and extra defs */

/* Already-parsed symbols are marked specially */

#define stParsed stType

/* Puns: hard to find whether -g was used and how */

#define MIN_GLEVEL GLEVEL_0
#define compare_glevel(a,b)					\
	(((a) == GLEVEL_3) ? ((b) < GLEVEL_3) :			\
	 ((b) == GLEVEL_3) ? -1 : (int)((b) - (a)))

/* Things that really are local to this module */

/* Remember what we deduced to be the source language of this psymtab. */

static enum language psymtab_language = language_unknown;

/* Current BFD.  */

static bfd *cur_bfd;

/* Pointer to current file decriptor record, and its index */

static FDR *cur_fdr;
static int cur_fd;

/* Index of current symbol */

static int cur_sdx;

/* Note how much "debuggable" this image is.  We would like
   to see at least one FDR with full symbols */

static max_gdbinfo;
static max_glevel;

/* When examining .o files, report on undefined symbols */

static int n_undef_symbols, n_undef_labels, n_undef_vars, n_undef_procs;

/* Pseudo symbol to use when putting stabs into the symbol table.  */

static char stabs_symbol[] = STABS_SYMBOL;

/* Extra builtin types */

struct type *builtin_type_complex;
struct type *builtin_type_double_complex;
struct type *builtin_type_fixed_dec;
struct type *builtin_type_float_dec;
struct type *builtin_type_string;

/* Forward declarations */

static void
read_mips_symtab PARAMS ((struct objfile *, struct section_offsets *));

static void
read_the_mips_symtab PARAMS ((bfd *));

static int
upgrade_type PARAMS ((struct type **, int, union aux_ext *, int));

static void
parse_partial_symbols PARAMS ((struct objfile *,
			       struct section_offsets *));

static int
cross_ref PARAMS ((union aux_ext *, struct type **, enum type_code, char **,
		   int));

static void
fixup_sigtramp PARAMS ((void));

static struct symbol *
new_symbol PARAMS ((char *));

static struct type *
new_type PARAMS ((char *));

static struct block *
new_block PARAMS ((int));

static struct symtab *
new_symtab PARAMS ((char *, int, int, struct objfile *));

static struct linetable *
new_linetable PARAMS ((int));

static struct blockvector *
new_bvect PARAMS ((int));

static struct type *
parse_type PARAMS ((union aux_ext *, int *, int));

static struct symbol *
mylookup_symbol PARAMS ((char *, struct block *, enum namespace,
			 enum address_class));

static struct block *
shrink_block PARAMS ((struct block *, struct symtab *));

static PTR
xzalloc PARAMS ((unsigned int));

static void
sort_blocks PARAMS ((struct symtab *));

static int
compare_blocks PARAMS ((const void *, const void *));

static struct partial_symtab *
new_psymtab PARAMS ((char *, struct objfile *));

#if 0
static struct partial_symtab *
parse_fdr PARAMS ((int, int, struct objfile *));
#endif

static void
psymtab_to_symtab_1 PARAMS ((struct partial_symtab *, char *));

static void
add_block PARAMS ((struct block *, struct symtab *));

static void
add_symbol PARAMS ((struct symbol *, struct block *));

static int
add_line PARAMS ((struct linetable *, int, CORE_ADDR, int));

static struct linetable *
shrink_linetable PARAMS ((struct linetable *));

static char *
mips_next_symbol_text PARAMS ((void));

/* Things we export to other modules */

/* Address bounds for the signal trampoline in inferior, if any */
/* FIXME:  Nothing really seems to use this.  Why is it here? */

CORE_ADDR sigtramp_address, sigtramp_end;

static void
mipscoff_new_init (ignore)
     struct objfile *ignore;
{
  sigtramp_address = 0;
  stabsread_new_init ();
  buildsym_new_init ();
}

static void
mipscoff_symfile_init (objfile)
     struct objfile *objfile;
{
  if (objfile->sym_private != NULL)
    {
      mfree (objfile->md, objfile->sym_private);
    }
  objfile->sym_private = NULL;
}

static void
mipscoff_symfile_read (objfile, section_offsets, mainline)
     struct objfile *objfile;
     struct section_offsets *section_offsets;
     int mainline;
{
  init_minimal_symbol_collection ();
  make_cleanup (discard_minimal_symbols, 0);

  /* Now that the executable file is positioned at symbol table,
     process it and define symbols accordingly.  */

  read_mips_symtab (objfile, section_offsets);

  /* Install any minimal symbols that have been collected as the current
     minimal symbols for this objfile. */

  install_minimal_symbols (objfile);
}

/* Perform any local cleanups required when we are done with a particular
   objfile.  I.E, we are in the process of discarding all symbol information
   for an objfile, freeing up all memory held for it, and unlinking the
   objfile struct from the global list of known objfiles. */

static void
mipscoff_symfile_finish (objfile)
     struct objfile *objfile;
{
  if (objfile->sym_private != NULL)
    {
      mfree (objfile->md, objfile->sym_private);
    }

  cur_bfd = 0;
}

/* Allocate zeroed memory */

static PTR
xzalloc (size)
     unsigned int size;
{
  PTR p = xmalloc (size);

  memset (p, 0, size);
  return p;
}

/* Exported procedure: Builds a symtab from the PST partial one.
   Restores the environment in effect when PST was created, delegates
   most of the work to an ancillary procedure, and sorts
   and reorders the symtab list at the end */

static void
mipscoff_psymtab_to_symtab (pst)
     struct partial_symtab *pst;
{

  if (!pst)
    return;

  if (info_verbose)
    {
      printf_filtered ("Reading in symbols for %s...", pst->filename);
      fflush (stdout);
    }

  next_symbol_text_func = mips_next_symbol_text;

  psymtab_to_symtab_1 (pst, pst->filename);

  /* Match with global symbols.  This only needs to be done once,
     after all of the symtabs and dependencies have been read in.   */
  scan_file_globals (pst->objfile);

  if (info_verbose)
    printf_filtered ("done.\n");
}

/* Exported procedure: Is PC in the signal trampoline code */

int
in_sigtramp (pc, ignore)
     CORE_ADDR pc;
     char *ignore;		/* function name */
{
  if (sigtramp_address == 0)
    fixup_sigtramp ();
  return (pc >= sigtramp_address && pc < sigtramp_end);
}

/* File-level interface functions */

/* Read the symtab information from file ABFD into memory.  */

static void
read_the_mips_symtab (abfd)
     bfd *abfd;
{
  if (ecoff_slurp_symbolic_info (abfd) == false)
    error ("Error reading symbol table: %s", bfd_errmsg (bfd_error));
}

/* Find a file descriptor given its index RF relative to a file CF */

static FDR *
get_rfd (cf, rf)
     int cf, rf;
{
  FDR *fdrs;
  register FDR *f;
  RFDT rfd;

  fdrs = ecoff_data (cur_bfd)->fdr;
  f = fdrs + cf;
  /* Object files do not have the RFD table, all refs are absolute */
  if (f->rfdBase == 0)
    return fdrs + rf;
  ecoff_swap_rfd_in (cur_bfd,
		     ecoff_data (cur_bfd)->external_rfd + f->rfdBase + rf,
		     &rfd);
  return fdrs + rfd;
}

/* Return a safer print NAME for a file descriptor */

static char *
fdr_name (f)
     FDR *f;
{
  if (f->rss == -1)
    return "<stripped file>";
  if (f->rss == 0)
    return "<NFY>";
  return ecoff_data (cur_bfd)->ss + f->issBase + f->rss;
}


/* Read in and parse the symtab of the file OBJFILE.  Symbols from
   different sections are relocated via the SECTION_OFFSETS.  */

static void
read_mips_symtab (objfile, section_offsets)
     struct objfile *objfile;
     struct section_offsets *section_offsets;
{
  cur_bfd = objfile->obfd;

  read_the_mips_symtab (objfile->obfd);

  parse_partial_symbols (objfile, section_offsets);

#if 0
  /* Check to make sure file was compiled with -g.  If not, warn the
     user of this limitation.  */
  if (compare_glevel (max_glevel, GLEVEL_2) < 0)
    {
      if (max_gdbinfo == 0)
	printf ("\n%s not compiled with -g, debugging support is limited.\n",
		 objfile->name);
      printf ("You should compile with -g2 or -g3 for best debugging support.\n");
      fflush (stdout);
    }
#endif
}

/* Local utilities */

/* Map of FDR indexes to partial symtabs */

struct pst_map
{
  struct partial_symtab *pst;	/* the psymtab proper */
  int n_globals;		/* exported globals (external symbols) */
  int globals_offset;		/* cumulative */
};


/* Utility stack, used to nest procedures and blocks properly.
   It is a doubly linked list, to avoid too many alloc/free.
   Since we might need it quite a few times it is NOT deallocated
   after use. */

static struct parse_stack
{
  struct parse_stack *next, *prev;
  struct symtab *cur_st;	/* Current symtab. */
  struct block *cur_block;	/* Block in it. */
  int blocktype;		/* What are we parsing. */
  int maxsyms;			/* Max symbols in this block. */
  struct type *cur_type;	/* Type we parse fields for. */
  int cur_field;		/* Field number in cur_type. */
  int procadr;			/* Start addres of this procedure */
  int numargs;			/* Its argument count */
}

 *top_stack;			/* Top stack ptr */


/* Enter a new lexical context */

static void
push_parse_stack ()
{
  struct parse_stack *new;

  /* Reuse frames if possible */
  if (top_stack && top_stack->prev)
    new = top_stack->prev;
  else
    new = (struct parse_stack *) xzalloc (sizeof (struct parse_stack));
  /* Initialize new frame with previous content */
  if (top_stack)
    {
      register struct parse_stack *prev = new->prev;

      *new = *top_stack;
      top_stack->prev = new;
      new->prev = prev;
      new->next = top_stack;
    }
  top_stack = new;
}

/* Exit a lexical context */

static void
pop_parse_stack ()
{
  if (!top_stack)
    return;
  if (top_stack->next)
    top_stack = top_stack->next;
}


/* Cross-references might be to things we haven't looked at
   yet, e.g. type references.  To avoid too many type
   duplications we keep a quick fixup table, an array
   of lists of references indexed by file descriptor */

static struct mips_pending
{
  struct mips_pending *next;	/* link */
  struct sym_ext *s;		/* the symbol */
  struct type *t;		/* its partial type descriptor */
} **pending_list;


/* Check whether we already saw symbol SH in file FH as undefined */

static struct mips_pending *
is_pending_symbol (fh, sh)
     FDR *fh;
     struct sym_ext *sh;
{
  int f_idx = fh - ecoff_data (cur_bfd)->fdr;
  register struct mips_pending *p;

  /* Linear search is ok, list is typically no more than 10 deep */
  for (p = pending_list[f_idx]; p; p = p->next)
    if (p->s == sh)
      break;
  return p;
}

/* Add a new undef symbol SH of type T */

static void
add_pending (fh, sh, t)
     FDR *fh;
     struct sym_ext *sh;
     struct type *t;
{
  int f_idx = fh - ecoff_data (cur_bfd)->fdr;
  struct mips_pending *p = is_pending_symbol (fh, sh);

  /* Make sure we do not make duplicates */
  if (!p)
    {
      p = (struct mips_pending *) xmalloc (sizeof (*p));
      p->s = sh;
      p->t = t;
      p->next = pending_list[f_idx];
      pending_list[f_idx] = p;
    }
}

/* Throw away undef entries when done with file index F_IDX */
/* FIXME -- storage leak.  This is never called!!!   --gnu */

#if 0

static void
free_pending (f_idx)
     int f_idx;
{
  register struct mips_pending *p, *q;

  for (p = pending_list[f_idx]; p; p = q)
    {
      q = p->next;
      free ((PTR) p);
    }
  pending_list[f_idx] = 0;
}

#endif



/* Parsing Routines proper. */

/* Parse a single symbol. Mostly just make up a GDB symbol for it.
   For blocks, procedures and types we open a new lexical context.
   This is basically just a big switch on the symbol's type.  Argument
   AX is the base pointer of aux symbols for this file (fh->iauxBase).
   EXT_SH points to the unswapped symbol, which is needed for struct,
   union, etc., types; it is NULL for an EXTR.  BIGEND says whether
   aux symbols are big-endian or little-endian.  Return count of
   SYMR's handled (normally one).

   FIXME: This modifies the symbol, but the only way we have to save
   the modified information is to stuff it back into the BFD data.  */

static int
parse_symbol (sh, ax, ext_sh, bigend)
     SYMR *sh;
     union aux_ext *ax;
     struct sym_ext *ext_sh;
     int bigend;
{
  char *name;
  struct symbol *s;
  struct block *b;
  struct mips_pending *pend;
  struct type *t;
  struct field *f;
  int count = 1;
  enum address_class class;
  TIR tir;

  if (ext_sh == (struct sym_ext *) NULL)
    name = ecoff_data (cur_bfd)->ssext + sh->iss;
  else
    name = ecoff_data (cur_bfd)->ss + cur_fdr->issBase + sh->iss;

  switch (sh->st)
    {
    case stNil:
      break;

    case stGlobal:		/* external symbol, goes into global block */
      class = LOC_STATIC;
      b = BLOCKVECTOR_BLOCK (BLOCKVECTOR (top_stack->cur_st),
			     GLOBAL_BLOCK);
      s = new_symbol (name);
      SYMBOL_VALUE_ADDRESS (s) = (CORE_ADDR) sh->value;
      goto data;

    case stStatic:		/* static data, goes into current block. */
      class = LOC_STATIC;
      b = top_stack->cur_block;
      s = new_symbol (name);
      if (sh->sc == scCommon)
	{
	  /* It is a FORTRAN common block.  At least for SGI Fortran the
	     address is not in the symbol; we need to fix it later in
	     scan_file_globals.  */
	  int bucket = hashname (SYMBOL_NAME (s));
	  SYMBOL_VALUE_CHAIN (s) = global_sym_chain[bucket];
	  global_sym_chain[bucket] = s;
	}
      else
	SYMBOL_VALUE_ADDRESS (s) = (CORE_ADDR) sh->value;
      goto data;

    case stLocal:		/* local variable, goes into current block */
      if (sh->sc == scRegister)
	{
	  class = LOC_REGISTER;
	  if (sh->value > 31)
	    sh->value += FP0_REGNUM - 32;
	}
      else
	class = LOC_LOCAL;
      b = top_stack->cur_block;
      s = new_symbol (name);
      SYMBOL_VALUE (s) = sh->value;

    data:			/* Common code for symbols describing data */
      SYMBOL_NAMESPACE (s) = VAR_NAMESPACE;
      SYMBOL_CLASS (s) = class;
      add_symbol (s, b);

      /* Type could be missing in a number of cases */
      if (sh->sc == scUndefined || sh->sc == scNil ||
	  sh->index == 0xfffff)
	SYMBOL_TYPE (s) = builtin_type_int;	/* undefined? */
      else
	SYMBOL_TYPE (s) = parse_type (ax + sh->index, 0, bigend);
      /* Value of a data symbol is its memory address */
      break;

    case stParam:		/* arg to procedure, goes into current block */
      max_gdbinfo++;
      top_stack->numargs++;

      /* Special GNU C++ name.  */
      if (name[0] == CPLUS_MARKER && name[1] == 't' && name[2] == 0)
	name = "this";		/* FIXME, not alloc'd in obstack */
      s = new_symbol (name);

      SYMBOL_NAMESPACE (s) = VAR_NAMESPACE;
      switch (sh->sc)
	{
	case scRegister:
	  /* Pass by value in register.  */
	  SYMBOL_CLASS(s) = LOC_REGPARM;
	  if (sh->value > 31)
	    sh->value += FP0_REGNUM-32;
	  break;
	case scVar:
	  /* Pass by reference on stack.  */
	  SYMBOL_CLASS(s) = LOC_REF_ARG;
	  break;
	case scVarRegister:
	  /* Pass by reference in register.  */
	  SYMBOL_CLASS(s) = LOC_REGPARM_ADDR;
	  break;
	default:
	  /* Pass by value on stack.  */
	  SYMBOL_CLASS(s) = LOC_ARG;
	  break;
	}
      SYMBOL_VALUE (s) = sh->value;
      SYMBOL_TYPE (s) = parse_type (ax + sh->index, 0, bigend);
      add_symbol (s, top_stack->cur_block);
#if 0
      /* FIXME:  This has not been tested.  See dbxread.c */
      /* Add the type of this parameter to the function/procedure
		   type of this block. */
      add_param_to_type (&top_stack->cur_block->function->type, s);
#endif
      break;

    case stLabel:		/* label, goes into current block */
      s = new_symbol (name);
      SYMBOL_NAMESPACE (s) = VAR_NAMESPACE;	/* so that it can be used */
      SYMBOL_CLASS (s) = LOC_LABEL;	/* but not misused */
      SYMBOL_VALUE_ADDRESS (s) = (CORE_ADDR) sh->value;
      SYMBOL_TYPE (s) = builtin_type_int;
      add_symbol (s, top_stack->cur_block);
      break;

    case stProc:		/* Procedure, usually goes into global block */
    case stStaticProc:		/* Static procedure, goes into current block */
      s = new_symbol (name);
      SYMBOL_NAMESPACE (s) = VAR_NAMESPACE;
      SYMBOL_CLASS (s) = LOC_BLOCK;
      /* Type of the return value */
      if (sh->sc == scUndefined || sh->sc == scNil)
	t = builtin_type_int;
      else
	t = parse_type (ax + sh->index + 1, 0, bigend);
      b = top_stack->cur_block;
      if (sh->st == stProc)
	{
	  struct blockvector *bv = BLOCKVECTOR (top_stack->cur_st);
	  /* The next test should normally be true,
		       but provides a hook for nested functions
		       (which we don't want to make global). */
	  if (b == BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK))
	    b = BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK);
	}
      add_symbol (s, b);

      /* Make a type for the procedure itself */
#if 0
      /* FIXME:  This has not been tested yet!  See dbxread.c */
      /* Generate a template for the type of this function.  The
	 types of the arguments will be added as we read the symbol
	 table. */
      bcopy (SYMBOL_TYPE (s), lookup_function_type (t), sizeof (struct type));
#else
      SYMBOL_TYPE (s) = lookup_function_type (t);
#endif

      /* Create and enter a new lexical context */
      b = new_block (top_stack->maxsyms);
      SYMBOL_BLOCK_VALUE (s) = b;
      BLOCK_FUNCTION (b) = s;
      BLOCK_START (b) = BLOCK_END (b) = sh->value;
      BLOCK_SUPERBLOCK (b) = top_stack->cur_block;
      add_block (b, top_stack->cur_st);

      /* Not if we only have partial info */
      if (sh->sc == scUndefined || sh->sc == scNil)
	break;

      push_parse_stack ();
      top_stack->cur_block = b;
      top_stack->blocktype = sh->st;
      top_stack->cur_type = SYMBOL_TYPE (s);
      top_stack->cur_field = -1;
      top_stack->procadr = sh->value;
      top_stack->numargs = 0;

      sh->value = (long) SYMBOL_TYPE (s);
      break;

      /* Beginning of code for structure, union, and enum definitions.
	       They all share a common set of local variables, defined here.  */
      {
	enum type_code type_code;
	struct sym_ext *ext_tsym;
	int nfields;
	long max_value;
	struct field *f;

    case stStruct:		/* Start a block defining a struct type */
	type_code = TYPE_CODE_STRUCT;
	goto structured_common;

    case stUnion:		/* Start a block defining a union type */
	type_code = TYPE_CODE_UNION;
	goto structured_common;

    case stEnum:		/* Start a block defining an enum type */
	type_code = TYPE_CODE_ENUM;
	goto structured_common;

    case stBlock:		/* Either a lexical block, or some type */
	if (sh->sc != scInfo && sh->sc != scCommon)
	  goto case_stBlock_code;	/* Lexical block */

	type_code = TYPE_CODE_UNDEF;	/* We have a type.  */

	/* Common code for handling struct, union, enum, and/or as-yet-
	   unknown-type blocks of info about structured data.  `type_code'
	   has been set to the proper TYPE_CODE, if we know it.  */
      structured_common:
	push_parse_stack ();
	top_stack->blocktype = stBlock;

	s = new_symbol (name);
	SYMBOL_NAMESPACE (s) = STRUCT_NAMESPACE;
	SYMBOL_CLASS (s) = LOC_TYPEDEF;
	SYMBOL_VALUE (s) = 0;
	add_symbol (s, top_stack->cur_block);

	/* First count the number of fields and the highest value. */
	nfields = 0;
	max_value = 0;
	for (ext_tsym = ext_sh + 1; ; ext_tsym++)
	  {
	    SYMR tsym;

	    ecoff_swap_sym_in (cur_bfd, ext_tsym, &tsym);

	    if (tsym.st == stEnd)
	      break;

	    if (tsym.st == stMember)
	      {
		if (nfields == 0 && type_code == TYPE_CODE_UNDEF)
		  /* If the type of the member is Nil (or Void),
		     without qualifiers, assume the tag is an
		     enumeration. */
		  if (tsym.index == indexNil)
		    type_code = TYPE_CODE_ENUM;
		  else
		    {
		      ecoff_swap_tir_in (bigend,
					 &ax[tsym.index].a_ti,
					 &tir);
		      if ((tir.bt == btNil || tir.bt == btVoid)
			  && tir.tq0 == tqNil)
			type_code = TYPE_CODE_ENUM;
		    }
		nfields++;
		if (tsym.value > max_value)
		  max_value = tsym.value;
	      }
	    else if (tsym.st == stBlock
		     || tsym.st == stUnion
		     || tsym.st == stEnum
		     || tsym.st == stStruct
		     || tsym.st == stParsed)
	      {
		if (tsym.sc == scVariant);	/*UNIMPLEMENTED*/
		if (tsym.index != 0)
		  {
		    /* This is something like a struct within a
		       struct.  Skip over the fields of the inner
		       struct.  The -1 is because the for loop will
		       increment ext_tsym.  */
		    ext_tsym = (ecoff_data (cur_bfd)->external_sym
				+ cur_fdr->isymBase
				+ tsym.index
				- 1);
		  }
	      }
	    else
	      complain (&block_member_complaint, tsym.st);
	  }

	/* In an stBlock, there is no way to distinguish structs,
	   unions, and enums at this point.  This is a bug in the
	   original design (that has been fixed with the recent
	   addition of the stStruct, stUnion, and stEnum symbol
	   types.)  The way you can tell is if/when you see a variable
	   or field of that type.  In that case the variable's type
	   (in the AUX table) says if the type is struct, union, or
	   enum, and points back to the stBlock here.  So you can
	   patch the tag kind up later - but only if there actually is
	   a variable or field of that type.

	   So until we know for sure, we will guess at this point.
	   The heuristic is:
	   If the first member has index==indexNil or a void type,
	   assume we have an enumeration.
	   Otherwise, if there is more than one member, and all
	   the members have offset 0, assume we have a union.
	   Otherwise, assume we have a struct.

	   The heuristic could guess wrong in the case of of an
	   enumeration with no members or a union with one (or zero)
	   members, or when all except the last field of a struct have
	   width zero.  These are uncommon and/or illegal situations,
	   and in any case guessing wrong probably doesn't matter
	   much.

	   But if we later do find out we were wrong, we fixup the tag
	   kind.  Members of an enumeration must be handled
	   differently from struct/union fields, and that is harder to
	   patch up, but luckily we shouldn't need to.  (If there are
	   any enumeration members, we can tell for sure it's an enum
	   here.) */

	if (type_code == TYPE_CODE_UNDEF)
	  if (nfields > 1 && max_value == 0)
	    type_code = TYPE_CODE_UNION;
	  else
	    type_code = TYPE_CODE_STRUCT;

	/* If this type was expected, use its partial definition */
	pend = is_pending_symbol (cur_fdr, ext_sh);
	if (pend != (struct mips_pending *) NULL)
	  t = pend->t;
	else
	  t = new_type (NULL);

	TYPE_TAG_NAME (t) = obconcat (&current_objfile->symbol_obstack,
				      "", "", name);
	TYPE_CODE (t) = type_code;
	TYPE_LENGTH (t) = sh->value;
	TYPE_NFIELDS (t) = nfields;
	TYPE_FIELDS (t) = f = ((struct field *)
			       TYPE_ALLOC (t,
					   nfields * sizeof (struct field)));

	if (type_code == TYPE_CODE_ENUM)
	  {
	    /* This is a non-empty enum. */
	    for (ext_tsym = ext_sh + 1; ; ext_tsym++)
	      {
		SYMR tsym;
		struct symbol *enum_sym;

		ecoff_swap_sym_in (cur_bfd, ext_tsym, &tsym);

		if (tsym.st != stMember)
		  break;

		f->bitpos = tsym.value;
		f->type = t;
		f->name = (ecoff_data (cur_bfd)->ss
			   + cur_fdr->issBase
			   + tsym.iss);
		f->bitsize = 0;

		enum_sym = ((struct symbol *)
			    obstack_alloc (&current_objfile->symbol_obstack,
					   sizeof (struct symbol)));
		memset ((PTR) enum_sym, 0, sizeof (struct symbol));
		SYMBOL_NAME (enum_sym) = f->name;
		SYMBOL_CLASS (enum_sym) = LOC_CONST;
		SYMBOL_TYPE (enum_sym) = t;
		SYMBOL_NAMESPACE (enum_sym) = VAR_NAMESPACE;
		SYMBOL_VALUE (enum_sym) = tsym.value;
		add_symbol (enum_sym, top_stack->cur_block);

		/* Skip the stMembers that we've handled. */
		count++;
		f++;
	      }
	  }
	SYMBOL_TYPE (s) = t;
	/* make this the current type */
	top_stack->cur_type = t;
	top_stack->cur_field = 0;
	/* Mark that symbol has a type, and say which one */
	sh->value = (long) t;
	break;

	/* End of local variables shared by struct, union, enum, and
	   block (as yet unknown struct/union/enum) processing.  */
      }

    case_stBlock_code:
      /* beginnning of (code) block. Value of symbol
	 is the displacement from procedure start */
      push_parse_stack ();
      top_stack->blocktype = stBlock;
      b = new_block (top_stack->maxsyms);
      BLOCK_START (b) = sh->value + top_stack->procadr;
      BLOCK_SUPERBLOCK (b) = top_stack->cur_block;
      top_stack->cur_block = b;
      add_block (b, top_stack->cur_st);
      break;

    case stEnd:		/* end (of anything) */
      if (sh->sc == scInfo || sh->sc == scCommon)
	{
	  /* Finished with type */
	  top_stack->cur_type = 0;
	}
      else if (sh->sc == scText &&
	       (top_stack->blocktype == stProc ||
		top_stack->blocktype == stStaticProc))
	{
	  /* Finished with procedure */
	  struct blockvector *bv = BLOCKVECTOR (top_stack->cur_st);
	  struct mips_extra_func_info *e;
	  struct block *b;
	  int i;

	  BLOCK_END (top_stack->cur_block) += sh->value;	/* size */

	  /* Make up special symbol to contain procedure specific info */
	  s = new_symbol (MIPS_EFI_SYMBOL_NAME);
	  SYMBOL_NAMESPACE (s) = LABEL_NAMESPACE;
	  SYMBOL_CLASS (s) = LOC_CONST;
	  SYMBOL_TYPE (s) = builtin_type_void;
	  e = ((struct mips_extra_func_info *)
	       obstack_alloc (&current_objfile->symbol_obstack,
			      sizeof (struct mips_extra_func_info)));
	  SYMBOL_VALUE (s) = (int) e;
	  e->numargs = top_stack->numargs;
	  add_symbol (s, top_stack->cur_block);

	  /* Reallocate symbols, saving memory */
	  b = shrink_block (top_stack->cur_block, top_stack->cur_st);

	  /* f77 emits proc-level with address bounds==[0,0],
	     So look for such child blocks, and patch them.  */
	  for (i = 0; i < BLOCKVECTOR_NBLOCKS (bv); i++)
	    {
	      struct block *b_bad = BLOCKVECTOR_BLOCK (bv, i);
	      if (BLOCK_SUPERBLOCK (b_bad) == b
		  && BLOCK_START (b_bad) == top_stack->procadr
		  && BLOCK_END (b_bad) == top_stack->procadr)
		{
		  BLOCK_START (b_bad) = BLOCK_START (b);
		  BLOCK_END (b_bad) = BLOCK_END (b);
		}
	    }
	}
      else if (sh->sc == scText && top_stack->blocktype == stBlock)
	{
	  /* End of (code) block. The value of the symbol is the
	     displacement from the procedure`s start address of the
	     end of this block. */
	  BLOCK_END (top_stack->cur_block) = sh->value + top_stack->procadr;
	  shrink_block (top_stack->cur_block, top_stack->cur_st);
	}
      else if (sh->sc == scText && top_stack->blocktype == stFile)
	{
	  /* End of file.  Pop parse stack and ignore.  Higher
	     level code deals with this.  */
	  ;
	}
      else
	complain (&stEnd_complaint, sh->sc);

      pop_parse_stack ();	/* restore previous lexical context */
      break;

    case stMember:		/* member of struct or union */
      f = &TYPE_FIELDS (top_stack->cur_type)[top_stack->cur_field++];
      f->name = name;
      f->bitpos = sh->value;
      f->bitsize = 0;
      f->type = parse_type (ax + sh->index, &f->bitsize, bigend);
      break;

    case stTypedef:		/* type definition */
      s = new_symbol (name);
      SYMBOL_NAMESPACE (s) = VAR_NAMESPACE;
      SYMBOL_CLASS (s) = LOC_TYPEDEF;
      SYMBOL_BLOCK_VALUE (s) = top_stack->cur_block;
      add_symbol (s, top_stack->cur_block);
      SYMBOL_TYPE (s) = parse_type (ax + sh->index, 0, bigend);
      sh->value = (long) SYMBOL_TYPE (s);
      break;

    case stFile:		/* file name */
      push_parse_stack ();
      top_stack->blocktype = sh->st;
      break;

      /* I`ve never seen these for C */
    case stRegReloc:
      break;			/* register relocation */
    case stForward:
      break;			/* forwarding address */
    case stConstant:
      break;			/* constant */
    default:
      complain (&unknown_mips_symtype_complaint, sh->st);
      break;
    }

  sh->st = stParsed;

  return count;
}

/* Parse the type information provided in the raw AX entries for
   the symbol SH. Return the bitfield size in BS, in case.
   We must byte-swap the AX entries before we use them; BIGEND says whether
   they are big-endian or little-endian (from fh->fBigendian).  */

static struct type *
parse_type (ax, bs, bigend)
     union aux_ext *ax;
     int *bs;
     int bigend;
{
  /* Null entries in this map are treated specially */
  static struct type **map_bt[] =
  {
    &builtin_type_void,		/* btNil */
    0,				/* btAdr */
    &builtin_type_char,		/* btChar */
    &builtin_type_unsigned_char,/* btUChar */
    &builtin_type_short,	/* btShort */
    &builtin_type_unsigned_short,	/* btUShort */
    &builtin_type_int,		/* btInt */
    &builtin_type_unsigned_int,	/* btUInt */
    &builtin_type_long,		/* btLong */
    &builtin_type_unsigned_long,/* btULong */
    &builtin_type_float,	/* btFloat */
    &builtin_type_double,	/* btDouble */
    0,				/* btStruct */
    0,				/* btUnion */
    0,				/* btEnum */
    0,				/* btTypedef */
    0,				/* btRange */
    0,				/* btSet */
    &builtin_type_complex,	/* btComplex */
    &builtin_type_double_complex,	/* btDComplex */
    0,				/* btIndirect */
    &builtin_type_fixed_dec,	/* btFixedDec */
    &builtin_type_float_dec,	/* btFloatDec */
    &builtin_type_string,	/* btString */
    0,				/* btBit */
    0,				/* btPicture */
    &builtin_type_void,		/* btVoid */
    &builtin_type_long_long,	/* btLongLong */
    &builtin_type_unsigned_long_long,	/* btULongLong */
  };

  TIR t[1];
  struct type *tp = 0;
  char *fmt;
  union aux_ext *tax;
  enum type_code type_code = TYPE_CODE_UNDEF;

  /* Use aux as a type information record, map its basic type.  */
  tax = ax;
  ecoff_swap_tir_in (bigend, &tax->a_ti, t);
  if (t->bt > (sizeof (map_bt) / sizeof (*map_bt)))
    {
      complain (&basic_type_complaint, t->bt);
      return builtin_type_int;
    }
  if (map_bt[t->bt])
    {
      tp = *map_bt[t->bt];
    }
  else
    {
      tp = NULL;
      /* Cannot use builtin types -- build our own */
      switch (t->bt)
	{
	case btAdr:
	  tp = lookup_pointer_type (builtin_type_void);
	  break;
	case btStruct:
	  type_code = TYPE_CODE_STRUCT;
	  break;
	case btUnion:
	  type_code = TYPE_CODE_UNION;
	  break;
	case btEnum:
	  type_code = TYPE_CODE_ENUM;
	  break;
	case btRange:
	  type_code = TYPE_CODE_RANGE;
	  break;
	case btSet:
	  type_code = TYPE_CODE_SET;
	  break;
	case btTypedef:
	default:
	  complain (&basic_type_complaint, t->bt);
	  return builtin_type_int;
	}
    }

  /* Skip over any further type qualifiers (FIXME).  */
  if (t->continued)
    {
      /* This is the way it would work if the compiler worked */
      TIR t1[1];
      do
	{
	  ax++;
	  ecoff_swap_tir_in (bigend, &ax->a_ti, t1);
	}
      while (t1->continued);
    }

  /* Move on to next aux */
  ax++;

  if (t->fBitfield)
    {
      *bs = AUX_GET_WIDTH (bigend, ax);
      ax++;
    }

  /* All these types really point to some (common) MIPS type
     definition, and only the type-qualifiers fully identify
     them.  We'll make the same effort at sharing. */
  if (t->bt == btStruct ||
      t->bt == btUnion ||
      t->bt == btEnum ||

      /* btSet (I think) implies that the name is a tag name, not a typedef
	 name.  This apparently is a MIPS extension for C sets.  */
      t->bt == btSet)
    {
      char *name;

      /* Try to cross reference this type */
      ax += cross_ref (ax, &tp, type_code, &name, bigend);
      /* reading .o file ? */
      if (tp == (struct type *) NULL)
	tp = init_type (type_code, 0, 0, (char *) NULL,
			(struct objfile *) NULL);

      /* Usually, TYPE_CODE(tp) is already type_code.  The main
	 exception is if we guessed wrong re struct/union/enum. */
      if (TYPE_CODE (tp) != type_code)
	{
	  complain (&bad_tag_guess_complaint, name);
	  TYPE_CODE (tp) = type_code;
	}
      if (TYPE_TAG_NAME (tp) == NULL || !STREQ (TYPE_TAG_NAME (tp), name))
	TYPE_TAG_NAME (tp) = obsavestring (name, strlen (name),
					   &current_objfile->type_obstack);
    }

  /* All these types really point to some (common) MIPS type
     definition, and only the type-qualifiers fully identify
     them.  We'll make the same effort at sharing. */
  if (t->bt == btIndirect ||
      t->bt == btRange)
    {
      char *name;

      /* Try to cross reference this type */
      ax += cross_ref (ax, &tp, type_code, &name, bigend);
      /* reading .o file ? */
      if (tp == (struct type *) NULL)
	tp = init_type (type_code, 0, 0, (char *) NULL,
			(struct objfile *) NULL);

      /* Usually, TYPE_CODE(tp) is already type_code.  The main
	 exception is if we guessed wrong re struct/union/enum. */
      if (TYPE_CODE (tp) != type_code)
	{
	  complain (&bad_tag_guess_complaint, name);
	  TYPE_CODE (tp) = type_code;
	}
      if (TYPE_NAME (tp) == NULL || !STREQ (TYPE_NAME (tp), name))
	TYPE_NAME (tp) = obsavestring (name, strlen (name),
				       &current_objfile->type_obstack);
    }

  /* Deal with range types */
  if (t->bt == btRange)
    {
      TYPE_NFIELDS (tp) = 2;
      TYPE_FIELDS (tp) = ((struct field *)
			  TYPE_ALLOC (tp, 2 * sizeof (struct field)));
      TYPE_FIELD_NAME (tp, 0) = obsavestring ("Low", strlen ("Low"),
					      &current_objfile->type_obstack);
      TYPE_FIELD_BITPOS (tp, 0) = AUX_GET_DNLOW (bigend, ax);
      ax++;
      TYPE_FIELD_NAME (tp, 1) = obsavestring ("High", strlen ("High"),
					      &current_objfile->type_obstack);
      TYPE_FIELD_BITPOS (tp, 1) = AUX_GET_DNHIGH (bigend, ax);
      ax++;
    }

  /* Parse all the type qualifiers now. If there are more
     than 6 the game will continue in the next aux */

#define PARSE_TQ(tq) \
	if (t->tq != tqNil) ax += upgrade_type(&tp, t->tq, ax, bigend);

again:PARSE_TQ (tq0);
  PARSE_TQ (tq1);
  PARSE_TQ (tq2);
  PARSE_TQ (tq3);
  PARSE_TQ (tq4);
  PARSE_TQ (tq5);
#undef	PARSE_TQ

  if (t->continued)
    {
      tax++;
      ecoff_swap_tir_in (bigend, &tax->a_ti, t);
      goto again;
    }
  return tp;
}

/* Make up a complex type from a basic one.  Type is passed by
   reference in TPP and side-effected as necessary. The type
   qualifier TQ says how to handle the aux symbols at AX for
   the symbol SX we are currently analyzing.  BIGEND says whether
   aux symbols are big-endian or little-endian.
   Returns the number of aux symbols we parsed. */

static int
upgrade_type (tpp, tq, ax, bigend)
     struct type **tpp;
     int tq;
     union aux_ext *ax;
     int bigend;
{
  int off;
  struct type *t;

  /* Used in array processing */
  int rf, id;
  FDR *fh;
  struct type *range;
  struct type *indx;
  int lower, upper;
  RNDXR rndx;

  switch (tq)
    {
    case tqPtr:
      t = lookup_pointer_type (*tpp);
      *tpp = t;
      return 0;

    case tqProc:
      t = lookup_function_type (*tpp);
      *tpp = t;
      return 0;

    case tqArray:
      off = 0;

      /* Determine and record the domain type (type of index) */
      ecoff_swap_rndx_in (bigend, &ax->a_rndx, &rndx);
      id = rndx.index;
      rf = rndx.rfd;
      if (rf == 0xfff)
	{
	  ax++;
	  rf = AUX_GET_ISYM (bigend, ax);
	  off++;
	}
      fh = get_rfd (cur_fd, rf);

      indx = parse_type ((ecoff_data (cur_bfd)->external_aux
			  + fh->iauxBase
			  + id),
			 (int *) NULL, bigend);

      /* Get the bounds, and create the array type.  */
      ax++;
      lower = AUX_GET_DNLOW (bigend, ax);
      ax++;
      upper = AUX_GET_DNHIGH (bigend, ax);
      ax++;
      rf = AUX_GET_WIDTH (bigend, ax);	/* bit size of array element */

      range = create_range_type ((struct type *) NULL, indx,
				 lower, upper);

      t = create_array_type ((struct type *) NULL, *tpp, range);

      /* Check whether supplied array element bit size matches
	 the known size of the element type.  If this complaint
	 ends up not happening, we can remove this code.  It's
	 here because we aren't sure we understand this *&%&$
	 symbol format.  */
      id = TYPE_LENGTH (TYPE_TARGET_TYPE (t)) << 3;	/* bitsize */
      if (id == 0)
	{
	  /* Most likely an undefined type */
	  id = rf;
	  TYPE_LENGTH (TYPE_TARGET_TYPE (t)) = id >> 3;
	}
      if (id != rf)
	complain (&array_bitsize_complaint, rf);

      *tpp = t;
      return 4 + off;

    case tqVol:
      /* Volatile -- currently ignored */
      return 0;

    case tqConst:
      /* Const -- currently ignored */
      return 0;

    default:
      complain (&unknown_type_qual_complaint, tq);
      return 0;
    }
}


/* Parse a procedure descriptor record PR.  Note that the procedure is
   parsed _after_ the local symbols, now we just insert the extra
   information we need into a MIPS_EFI_SYMBOL_NAME symbol that has
   already been placed in the procedure's main block.  Note also that
   images that have been partially stripped (ld -x) have been deprived
   of local symbols, and we have to cope with them here.  FIRST_OFF is
   the offset of the first procedure for this FDR; we adjust the
   address by this amount, but I don't know why.  SEARCH_SYMTAB is the symtab
   to look for the function which contains the MIPS_EFI_SYMBOL_NAME symbol
   in question, or NULL to use top_stack->cur_block.  */

static void parse_procedure PARAMS ((PDR *, struct symtab *, unsigned long));

static void
parse_procedure (pr, search_symtab, first_off)
     PDR *pr;
     struct symtab *search_symtab;
     unsigned long first_off;
{
  struct symbol *s, *i;
  struct block *b;
  struct mips_extra_func_info *e;
  char *sh_name;

  /* Simple rule to find files linked "-x" */
  if (cur_fdr->rss == -1)
    {
      if (pr->isym == -1)
	{
	  /* Static procedure at address pr->adr.  Sigh. */
	  complain (&pdr_static_symbol_complaint, pr->adr);
	  return;
	}
      else
	{
	  /* external */
	  EXTR she;
	  
	  ecoff_swap_ext_in (cur_bfd,
			     ecoff_data (cur_bfd)->external_ext + pr->isym,
			     &she);
	  sh_name = ecoff_data (cur_bfd)->ssext + she.asym.iss;
	}
    }
  else
    {
      /* Full symbols */
      SYMR sh;

      ecoff_swap_sym_in (cur_bfd,
			 (ecoff_data (cur_bfd)->external_sym
			  + cur_fdr->isymBase
			  + pr->isym),
			 &sh);
      sh_name = ecoff_data (cur_bfd)->ss + cur_fdr->issBase + sh.iss;
    }

  if (search_symtab != NULL)
    {
#if 0
      /* This loses both in the case mentioned (want a static, find a global),
	 but also if we are looking up a non-mangled name which happens to
	 match the name of a mangled function.  */
      /* We have to save the cur_fdr across the call to lookup_symbol.
	 If the pdr is for a static function and if a global function with
	 the same name exists, lookup_symbol will eventually read in the symtab
	 for the global function and clobber cur_fdr.  */
      FDR *save_cur_fdr = cur_fdr;
      s = lookup_symbol (sh_name, NULL, VAR_NAMESPACE, 0, NULL);
      cur_fdr = save_cur_fdr;
#else
      s = mylookup_symbol
	(sh_name,
	 BLOCKVECTOR_BLOCK (BLOCKVECTOR (search_symtab), STATIC_BLOCK),
	 VAR_NAMESPACE,
	 LOC_BLOCK);
#endif
    }
  else
    s = mylookup_symbol (sh_name, top_stack->cur_block,
			 VAR_NAMESPACE, LOC_BLOCK);

  if (s != 0)
    {
      b = SYMBOL_BLOCK_VALUE (s);
    }
  else
    {
      complain (&pdr_for_nonsymbol_complaint, sh_name);
#if 1
      return;
#else
/* FIXME -- delete.  We can't do symbol allocation now; it's all done.  */
      s = new_symbol (sh_name);
      SYMBOL_NAMESPACE (s) = VAR_NAMESPACE;
      SYMBOL_CLASS (s) = LOC_BLOCK;
      /* Donno its type, hope int is ok */
      SYMBOL_TYPE (s) = lookup_function_type (builtin_type_int);
      add_symbol (s, top_stack->cur_block);
      /* Wont have symbols for this one */
      b = new_block (2);
      SYMBOL_BLOCK_VALUE (s) = b;
      BLOCK_FUNCTION (b) = s;
      BLOCK_START (b) = pr->adr;
      /* BOUND used to be the end of procedure's text, but the
	 argument is no longer passed in.  */
      BLOCK_END (b) = bound;
      BLOCK_SUPERBLOCK (b) = top_stack->cur_block;
      add_block (b, top_stack->cur_st);
#endif
    }

  i = mylookup_symbol (MIPS_EFI_SYMBOL_NAME, b, LABEL_NAMESPACE, LOC_CONST);

  if (i)
    {
      e = (struct mips_extra_func_info *) SYMBOL_VALUE (i);
      e->pdr = *pr;
      e->pdr.isym = (long) s;
      e->pdr.adr += cur_fdr->adr - first_off;

      /* Correct incorrect setjmp procedure descriptor from the library
	 to make backtrace through setjmp work.  */
      if (e->pdr.pcreg == 0 && strcmp (sh_name, "setjmp") == 0)
	{
	  complain (&bad_setjmp_pdr_complaint, 0);
	  e->pdr.pcreg = RA_REGNUM;
	  e->pdr.regmask = 0x80000000;
	  e->pdr.regoffset = -4;
	}
    }
}

/* Parse the external symbol ES. Just call parse_symbol() after
   making sure we know where the aux are for it. For procedures,
   parsing of the PDRs has already provided all the needed
   information, we only parse them if SKIP_PROCEDURES is false,
   and only if this causes no symbol duplication.
   BIGEND says whether aux entries are big-endian or little-endian.

   This routine clobbers top_stack->cur_block and ->cur_st. */

static void
parse_external (es, skip_procedures, bigend)
     EXTR *es;
     int skip_procedures;
     int bigend;
{
  union aux_ext *ax;

  if (es->ifd != ifdNil)
    {
      cur_fd = es->ifd;
      cur_fdr = ecoff_data (cur_bfd)->fdr + cur_fd;
      ax = ecoff_data (cur_bfd)->external_aux + cur_fdr->iauxBase;
    }
  else
    {
      cur_fdr = ecoff_data (cur_bfd)->fdr;
      ax = 0;
    }

  /* Reading .o files */
  if (es->asym.sc == scUndefined || es->asym.sc == scNil)
    {
      char *what;
      switch (es->asym.st)
	{
	case stStaticProc:
	case stProc:
	  what = "procedure";
	  n_undef_procs++;
	  break;
	case stGlobal:
	  what = "variable";
	  n_undef_vars++;
	  break;
	case stLabel:
	  what = "label";
	  n_undef_labels++;
	  break;
	default:
	  what = "symbol";
	  break;
	}
      n_undef_symbols++;
      /* FIXME:  Turn this into a complaint? */
      if (info_verbose)
	printf_filtered ("Warning: %s `%s' is undefined (in %s)\n",
			 what,
			 ecoff_data (cur_bfd)->ssext + es->asym.iss,
			 fdr_name (cur_fdr));
      return;
    }

  switch (es->asym.st)
    {
    case stProc:
      /* If we have full symbols we do not need more */
      if (skip_procedures)
	return;
      if (mylookup_symbol (ecoff_data (cur_bfd)->ssext + es->asym.iss,
			   top_stack->cur_block,
			   VAR_NAMESPACE, LOC_BLOCK))
	break;
      /* fall through */
    case stGlobal:
    case stLabel:
      /* Note that the case of a symbol with indexNil must be handled
	 anyways by parse_symbol().  */
      parse_symbol (&es->asym, ax, (struct sym_ext *) NULL, bigend);
      /* Note that parse_symbol changed es->asym.  */
      break;
    default:
      break;
    }
}

/* Parse the line number info for file descriptor FH into
   GDB's linetable LT.  MIPS' encoding requires a little bit
   of magic to get things out.  Note also that MIPS' line
   numbers can go back and forth, apparently we can live
   with that and do not need to reorder our linetables */

static void
parse_lines (fh, pr, lt)
     FDR *fh;
     PDR *pr;
     struct linetable *lt;
{
  unsigned char *base;
  int j, k;
  int delta, count, lineno = 0;
  unsigned long first_off = pr->adr;

  if (fh->cbLineOffset == 0)
    return;

  base = ecoff_data (cur_bfd)->line + fh->cbLineOffset;

  /* Scan by procedure descriptors */
  k = 0;
  for (j = 0; j < fh->cpd; j++, pr++)
    {
      int l, halt;
      unsigned long adr;

      /* No code for this one */
      if (pr->iline == ilineNil ||
	  pr->lnLow == -1 || pr->lnHigh == -1)
	continue;

      /* Aurgh! To know where to stop expanding we must look-ahead.  */
      for (l = 1; l < (fh->cpd - j); l++)
	if (pr[l].iline != -1)
	  break;
      if (l == (fh->cpd - j))
	halt = fh->cline;
      else
	halt = pr[l].iline;

      /* When procedures are moved around the linenumbers are
	 attributed to the next procedure up.  */
      if (pr->iline >= halt)
	continue;

      base = ecoff_data (cur_bfd)->line + fh->cbLineOffset + pr->cbLineOffset;
      adr = fh->adr + pr->adr - first_off;
      l = adr >> 2;		/* in words */
      halt += (adr >> 2) - pr->iline;
      for (lineno = pr->lnLow; l < halt;)
	{
	  count = *base & 0x0f;
	  delta = *base++ >> 4;
	  if (delta >= 8)
	    delta -= 16;
	  if (delta == -8)
	    {
	      delta = (base[0] << 8) | base[1];
	      if (delta >= 0x8000)
		delta -= 0x10000;
	      base += 2;
	    }
	  lineno += delta;	/* first delta is 0 */
	  k = add_line (lt, lineno, l, k);
	  l += count + 1;
	}
    }
}

/* Master parsing procedure for first-pass reading of file symbols
   into a partial_symtab.  */

static void
parse_partial_symbols (objfile, section_offsets)
     struct objfile *objfile;
     struct section_offsets *section_offsets;
{
  int f_idx, s_idx;
  HDRR *hdr = &ecoff_data (cur_bfd)->symbolic_header;
  /* Running pointers */
  FDR *fh;
  struct ext_ext *ext_out;
  struct ext_ext *ext_out_end;
  EXTR *ext_block;
  register EXTR *ext_in;
  EXTR *ext_in_end;
  SYMR sh;
  struct partial_symtab *pst;

  int past_first_source_file = 0;

  /* List of current psymtab's include files */
  char **psymtab_include_list;
  int includes_allocated;
  int includes_used;
  EXTR *extern_tab;
  struct pst_map *fdr_to_pst;
  /* Index within current psymtab dependency list */
  struct partial_symtab **dependency_list;
  int dependencies_used, dependencies_allocated;
  struct cleanup *old_chain;
  char *name;

  extern_tab = (EXTR *) obstack_alloc (&objfile->psymbol_obstack,
				       sizeof (EXTR) * hdr->iextMax);

  includes_allocated = 30;
  includes_used = 0;
  psymtab_include_list = (char **) alloca (includes_allocated *
					   sizeof (char *));
  next_symbol_text_func = mips_next_symbol_text;

  dependencies_allocated = 30;
  dependencies_used = 0;
  dependency_list =
    (struct partial_symtab **) alloca (dependencies_allocated *
				       sizeof (struct partial_symtab *));

  last_source_file = NULL;

  /*
   * Big plan:
   *
   * Only parse the Local and External symbols, and the Relative FDR.
   * Fixup enough of the loader symtab to be able to use it.
   * Allocate space only for the file's portions we need to
   * look at. (XXX)
   */

  max_gdbinfo = 0;
  max_glevel = MIN_GLEVEL;

  /* Allocate the map FDR -> PST.
     Minor hack: -O3 images might claim some global data belongs
     to FDR -1. We`ll go along with that */
  fdr_to_pst = (struct pst_map *) xzalloc ((hdr->ifdMax + 1) * sizeof *fdr_to_pst);
  old_chain = make_cleanup (free, fdr_to_pst);
  fdr_to_pst++;
  {
    struct partial_symtab *pst = new_psymtab ("", objfile);
    fdr_to_pst[-1].pst = pst;
    FDR_IDX (pst) = -1;
  }

  /* Pass 0 over external syms: swap them in.  */
  ext_block = (EXTR *) xmalloc (hdr->iextMax * sizeof (EXTR));
  make_cleanup (free, ext_block);

  ext_out = ecoff_data (cur_bfd)->external_ext;
  ext_out_end = ext_out + hdr->iextMax;
  ext_in = ext_block;
  for (; ext_out < ext_out_end; ext_out++, ext_in++)
    ecoff_swap_ext_in (cur_bfd, ext_out, ext_in);

  /* Pass 1 over external syms: Presize and partition the list */
  ext_in = ext_block;
  ext_in_end = ext_in + hdr->iextMax;
  for (; ext_in < ext_in_end; ext_in++)
    fdr_to_pst[ext_in->ifd].n_globals++;

  /* Pass 1.5 over files:  partition out global symbol space */
  s_idx = 0;
  for (f_idx = -1; f_idx < hdr->ifdMax; f_idx++)
    {
      fdr_to_pst[f_idx].globals_offset = s_idx;
      s_idx += fdr_to_pst[f_idx].n_globals;
      fdr_to_pst[f_idx].n_globals = 0;
    }

  /* Pass 2 over external syms: fill in external symbols */
  ext_in = ext_block;
  ext_in_end = ext_in + hdr->iextMax;
  for (; ext_in < ext_in_end; ext_in++)
    {
      enum minimal_symbol_type ms_type = mst_text;

      extern_tab[fdr_to_pst[ext_in->ifd].globals_offset
		 + fdr_to_pst[ext_in->ifd].n_globals++] = *ext_in;

      if (ext_in->asym.sc == scUndefined || ext_in->asym.sc == scNil)
	continue;

      name = ecoff_data (cur_bfd)->ssext + ext_in->asym.iss;
      switch (ext_in->asym.st)
	{
	case stProc:
	  break;
	case stGlobal:
	  ms_type = mst_data;
	  break;
	case stLabel:
	  break;
	default:
	  ms_type = mst_unknown;
	  complain (&unknown_ext_complaint, name);
	}
      prim_record_minimal_symbol (name, ext_in->asym.value, ms_type);
    }

  /* Pass 3 over files, over local syms: fill in static symbols */
  for (f_idx = 0; f_idx < hdr->ifdMax; f_idx++)
    {
      struct partial_symtab *save_pst;
      EXTR *ext_ptr;

      cur_fdr = fh = ecoff_data (cur_bfd)->fdr + f_idx;

      if (fh->csym == 0)
	{
	  fdr_to_pst[f_idx].pst = NULL;
	  continue;
	}
      pst = start_psymtab_common (objfile, section_offsets,
				  fdr_name (fh),
				  fh->cpd ? fh->adr : 0,
				  objfile->global_psymbols.next,
				  objfile->static_psymbols.next);
      pst->read_symtab_private = ((char *)
				  obstack_alloc (&objfile->psymbol_obstack,
						 sizeof (struct symloc)));
      memset ((PTR) pst->read_symtab_private, 0, sizeof (struct symloc));

      save_pst = pst;
      /* Make everything point to everything. */
      FDR_IDX (pst) = f_idx;
      fdr_to_pst[f_idx].pst = pst;

      /* FIXME: This tampers with data from BFD.  */
      fh->ioptBase = (int) pst;

      CUR_BFD (pst) = cur_bfd;

      /* The way to turn this into a symtab is to call... */
      pst->read_symtab = mipscoff_psymtab_to_symtab;

      pst->texthigh = pst->textlow;

      /* For stabs-in-ecoff files, the second symbol must be @stab.
	 This symbol is emitted by mips-tfile to signal that the
	 current object file uses encapsulated stabs instead of mips
	 ecoff for local symbols.  (It is the second symbol because
	 the first symbol is the stFile used to signal the start of a
	 file). */
      processing_gcc_compilation = 0;
      if (fh->csym >= 2)
	{
	  ecoff_swap_sym_in (cur_bfd,
			     (ecoff_data (cur_bfd)->external_sym
			      + fh->isymBase
			      + 1),
			     &sh);
	  if (STREQ (ecoff_data (cur_bfd)->ss + fh->issBase + sh.iss,
		     stabs_symbol))
	    processing_gcc_compilation = 2;
	}

      if (processing_gcc_compilation != 0)
	{
	  for (cur_sdx = 2; cur_sdx < fh->csym; cur_sdx++)
	    {
	      int type_code;
	      char *namestring;

	      ecoff_swap_sym_in (cur_bfd,
				 (ecoff_data (cur_bfd)->external_sym
				  + fh->isymBase
				  + cur_sdx),
				 &sh);
	      type_code = MIPS_UNMARK_STAB (sh.index);
	      if (!MIPS_IS_STAB (&sh))
		{
		  if (sh.st == stProc || sh.st == stStaticProc)
		    {
		      long procaddr = sh.value;
		      long isym;


		      isym = AUX_GET_ISYM (fh->fBigendian,
					   (ecoff_data (cur_bfd)->external_aux
					    + fh->iauxBase
					    + sh.index));
		      ecoff_swap_sym_in (cur_bfd,
					 (ecoff_data (cur_bfd)->external_sym
					  + fh->isymBase
					  + isym
					  - 1),
					 &sh);
		      if (sh.st == stEnd)
			{
			  long high = procaddr + sh.value;
			  if (high > pst->texthigh)
			    pst->texthigh = high;
			}
		    }
		  continue;
		}
#define SET_NAMESTRING() \
  namestring = ecoff_data (cur_bfd)->ss + fh->issBase + sh.iss
#define CUR_SYMBOL_TYPE type_code
#define CUR_SYMBOL_VALUE sh.value
#define START_PSYMTAB(ofile,secoff,fname,low,symoff,global_syms,static_syms)\
  pst = save_pst
#define END_PSYMTAB(pst,ilist,ninc,c_off,c_text,dep_list,n_deps) (void)0
#define HANDLE_RBRAC(val) \
  if ((val) > save_pst->texthigh) save_pst->texthigh = (val);
#include "partial-stab.h"
	    }
	}
      else
	{
	  for (cur_sdx = 0; cur_sdx < fh->csym;)
	    {
	      char *name;
	      enum address_class class;

	      ecoff_swap_sym_in (cur_bfd,
				 (ecoff_data (cur_bfd)->external_sym
				  + fh->isymBase
				  + cur_sdx),
				 &sh);

	      if (MIPS_IS_STAB (&sh))
		{
		  cur_sdx++;
		  continue;
		}

	      if (sh.sc == scUndefined || sh.sc == scNil ||
		  sh.index == 0xfffff)
		{
		  /* FIXME, premature? */
		  cur_sdx++;
		  continue;
		}

	      name = ecoff_data (cur_bfd)->ss + fh->issBase + sh.iss;

	      switch (sh.st)
		{
		  long high;
		  long procaddr;
		  int new_sdx;

		case stProc:	/* Asm labels apparently */
		case stStaticProc:	/* Function */
		  ADD_PSYMBOL_TO_LIST (name, strlen (name),
				       VAR_NAMESPACE, LOC_BLOCK,
				       objfile->static_psymbols, sh.value,
				       psymtab_language, objfile);
		  /* Skip over procedure to next one. */
		  if (sh.index >= hdr->iauxMax)
		    {
		      /* Should not happen, but does when cross-compiling
			   with the MIPS compiler.  FIXME -- pull later.  */
		      complain (&index_complaint, name);
		      new_sdx = cur_sdx + 1;	/* Don't skip at all */
		    }
		  else
		    new_sdx = AUX_GET_ISYM (fh->fBigendian,
					    (ecoff_data (cur_bfd)->external_aux
					     + fh->iauxBase
					     + sh.index));
		  procaddr = sh.value;

		  if (new_sdx <= cur_sdx)
		    {
		      /* This should not happen either... FIXME.  */
		      complain (&aux_index_complaint, name);
		      new_sdx = cur_sdx + 1;	/* Don't skip backward */
		    }

		  cur_sdx = new_sdx;
		  ecoff_swap_sym_in (cur_bfd,
				     (ecoff_data (cur_bfd)->external_sym
				      + fh->isymBase
				      + cur_sdx
				      - 1),
				     &sh);
		  if (sh.st != stEnd)
		    continue;
		  high = procaddr + sh.value;
		  if (high > pst->texthigh)
		    pst->texthigh = high;
		  continue;

		case stStatic:	/* Variable */
		  class = LOC_STATIC;
		  break;

		case stTypedef:/* Typedef */
		  class = LOC_TYPEDEF;
		  break;

		case stConstant:	/* Constant decl */
		  class = LOC_CONST;
		  break;

		case stUnion:
		case stStruct:
		case stEnum:
		case stBlock:	/* { }, str, un, enum*/
		  if (sh.sc == scInfo || sh.sc == scCommon)
		    {
		      ADD_PSYMBOL_TO_LIST (name, strlen (name),
					   STRUCT_NAMESPACE, LOC_TYPEDEF,
					   objfile->static_psymbols,
					   sh.value,
					   psymtab_language, objfile);
		    }
		  /* Skip over the block */
		  new_sdx = sh.index;
		  if (new_sdx <= cur_sdx)
		    {
		      /* This happens with the Ultrix kernel. */
		      complain (&block_index_complaint, name);
		      new_sdx = cur_sdx + 1;	/* Don't skip backward */
		    }
		  cur_sdx = new_sdx;
		  continue;

		case stFile:	/* File headers */
		case stLabel:	/* Labels */
		case stEnd:	/* Ends of files */
		  goto skip;

		case stLocal:	/* Local variables */
		  /* Normally these are skipped because we skip over
		     all blocks we see.  However, these can occur
		     as visible symbols in a .h file that contains code. */
		  goto skip;

		default:
		  /* Both complaints are valid:  one gives symbol name,
		     the other the offending symbol type.  */
		  complain (&unknown_sym_complaint, name);
		  complain (&unknown_st_complaint, sh.st);
		  cur_sdx++;
		  continue;
		}
	      /* Use this gdb symbol */
	      ADD_PSYMBOL_TO_LIST (name, strlen (name),
				   VAR_NAMESPACE, class,
				   objfile->static_psymbols, sh.value,
				   psymtab_language, objfile);
	    skip:
	      cur_sdx++;	/* Go to next file symbol */
	    }

	  /* Now do enter the external symbols. */
	  ext_ptr = &extern_tab[fdr_to_pst[f_idx].globals_offset];
	  cur_sdx = fdr_to_pst[f_idx].n_globals;
	  PST_PRIVATE (save_pst)->extern_count = cur_sdx;
	  PST_PRIVATE (save_pst)->extern_tab = ext_ptr;
	  for (; --cur_sdx >= 0; ext_ptr++)
	    {
	      register struct partial_symbol *psym;
	      enum address_class class;
	      SYMR *psh;
	      char *name;

	      if (ext_ptr->ifd != f_idx)
		abort ();
	      psh = &ext_ptr->asym;
	      switch (psh->st)
		{
		case stProc:
		  class = LOC_BLOCK;
		  break;
		case stLabel:
		  class = LOC_LABEL;
		  break;
		default:
		  complain (&unknown_ext_complaint,
			    ecoff_data (cur_bfd)->ssext + psh->iss);
		  /* Fall through, pretend it's global.  */
		case stGlobal:
		  class = LOC_STATIC;
		  break;
		}
	      name = ecoff_data (cur_bfd)->ssext + psh->iss;
	      ADD_PSYMBOL_ADDR_TO_LIST (name, strlen (name),
				        VAR_NAMESPACE, class,
				        objfile->global_psymbols, (CORE_ADDR) psh->value,
				        psymtab_language, objfile);
	    }
	}

      end_psymtab (save_pst, psymtab_include_list, includes_used,
		   -1, save_pst->texthigh,
		   dependency_list, dependencies_used);
      if (objfile->ei.entry_point >= save_pst->textlow &&
	  objfile->ei.entry_point < save_pst->texthigh)
	{
	  objfile->ei.entry_file_lowpc = save_pst->textlow;
	  objfile->ei.entry_file_highpc = save_pst->texthigh;
	}
    }

  /* Now scan the FDRs for dependencies */
  for (f_idx = 0; f_idx < hdr->ifdMax; f_idx++)
    {
      int s_id0 = 0;
      fh = f_idx + ecoff_data (cur_bfd)->fdr;
      pst = fdr_to_pst[f_idx].pst;

      /* This should catch stabs-in-ecoff. */
      if (fh->crfd <= 1)
	continue;

      if (fh->cpd == 0)
	{		/* If there are no functions defined here ... */
	  /* ...then presumably a .h file: drop reverse depends .h->.c */
	  for (; s_id0 < fh->crfd; s_id0++)
	    {
	      RFDT rh;

	      ecoff_swap_rfd_in (cur_bfd,
				 (ecoff_data (cur_bfd)->external_rfd
				  + fh->rfdBase
				  + s_id0),
				 &rh);
	      if (rh == f_idx)
		{
		  s_id0++;	/* Skip self-dependency */
		  break;
		}
	    }
	}
      pst->number_of_dependencies = fh->crfd - s_id0;
      pst->dependencies =
	((struct partial_symtab **)
	 obstack_alloc (&objfile->psymbol_obstack,
			(pst->number_of_dependencies
			 * sizeof (struct partial_symtab *))));
      for (s_idx = s_id0; s_idx < fh->crfd; s_idx++)
	{
	  RFDT rh;

	  ecoff_swap_rfd_in (cur_bfd,
			     (ecoff_data (cur_bfd)->external_rfd
			      + fh->rfdBase
			      + s_idx),
			     &rh);
	  if (rh < 0 || rh >= hdr->ifdMax)
	    complain (&bad_file_number_complaint, rh);
	  else
	    pst->dependencies[s_idx - s_id0] = fdr_to_pst[rh].pst;
	}
    }
  do_cleanups (old_chain);
}


#if 0
/* Do the initial analisys of the F_IDX-th file descriptor.
   Allocates a partial symtab for it, and builds the list
   of dependent files by recursion. LEV says at which level
   of recursion we are called (to pretty up debug traces) */

static struct partial_symtab *
parse_fdr (f_idx, lev, objfile)
     int f_idx;
     int lev;
     struct objfile *objfile;
{
  register FDR *fh;
  register struct partial_symtab *pst;
  int s_idx, s_id0;

  fh = ecoff_data (cur_bfd)->fdr + f_idx;

  /* Use this to indicate into which symtab this file was parsed */
  if (fh->ioptBase)
    return (struct partial_symtab *) fh->ioptBase;

  /* Debuggability level */
  if (compare_glevel (max_glevel, fh->glevel) < 0)
    max_glevel = fh->glevel;

  /* Make a new partial_symtab */
  pst = new_psymtab (fdr_name (fh), objfile);
  if (fh->cpd == 0)
    {
      pst->textlow = 0;
      pst->texthigh = 0;
    }
  else
    {
      pst->textlow = fh->adr;
      pst->texthigh = fh->cpd;	/* To be fixed later */
    }

  /* Make everything point to everything. */
  FDR_IDX (pst) = f_idx;
  fdr_to_pst[f_idx].pst = pst;
  fh->ioptBase = (int) pst;

  /* Analyze its dependencies */
  if (fh->crfd <= 1)
    return pst;

  s_id0 = 0;
  if (fh->cpd == 0)
    {				/* If there are no functions defined here ... */
      /* ...then presumably a .h file: drop reverse depends .h->.c */
      for (; s_id0 < fh->crfd; s_id0++)
	{
	  RFDT rh;

	  ecoff_swap_rfd_in (cur_bfd,
			     (ecoff_data (cur_bfd)->external_rfd
			      + fh->rfdBase
			      + s_id0),
			     &rh);
	  if (rh == f_idx)
	    {
	      s_id0++;		/* Skip self-dependency */
	      break;
	    }
	}
    }
  pst->number_of_dependencies = fh->crfd - s_id0;
  pst->dependencies = ((struct partial_symtab **)
		       obstack_alloc (&objfile->psymbol_obstack,
				      (pst->number_of_dependencies
				       * sizeof (struct partial_symtab *))));
  for (s_idx = s_id0; s_idx < fh->crfd; s_idx++)
    {
      RFDT rh;

      ecoff_swap_rfd_in (cur_bfd,
			 (ecoff_data (cur_bfd)->external_rfd
			  + fh->rfdBase
			  + s_idx),
			 &rh);
      pst->dependencies[s_idx - s_id0] = parse_fdr (rh, lev + 1, objfile);
    }

  return pst;
}

#endif

static char *
mips_next_symbol_text ()
{
  SYMR sh;

  cur_sdx++;
  ecoff_swap_sym_in (cur_bfd,
		     (ecoff_data (cur_bfd)->external_sym
		      + cur_fdr->isymBase
		      + cur_sdx),
		     &sh);
  return ecoff_data (cur_bfd)->ss + cur_fdr->issBase + sh.iss;
}

/* Ancillary function to psymtab_to_symtab().  Does all the work
   for turning the partial symtab PST into a symtab, recurring
   first on all dependent psymtabs.  The argument FILENAME is
   only passed so we can see in debug stack traces what file
   is being read.

   This function has a split personality, based on whether the
   symbol table contains ordinary ecoff symbols, or stabs-in-ecoff.
   The flow of control and even the memory allocation differs.  FIXME.  */

static void
psymtab_to_symtab_1 (pst, filename)
     struct partial_symtab *pst;
     char *filename;
{
  int i;
  struct symtab *st;
  FDR *fh;
  struct linetable *lines;

  if (pst->readin)
    return;
  pst->readin = 1;

  /* Read in all partial symbtabs on which this one is dependent.
     NOTE that we do have circular dependencies, sigh.  We solved
     that by setting pst->readin before this point.  */

  for (i = 0; i < pst->number_of_dependencies; i++)
    if (!pst->dependencies[i]->readin)
      {
	/* Inform about additional files to be read in.  */
	if (info_verbose)
	  {
	    fputs_filtered (" ", stdout);
	    wrap_here ("");
	    fputs_filtered ("and ", stdout);
	    wrap_here ("");
	    printf_filtered ("%s...",
			     pst->dependencies[i]->filename);
	    wrap_here ("");	/* Flush output */
	    fflush (stdout);
	  }
	/* We only pass the filename for debug purposes */
	psymtab_to_symtab_1 (pst->dependencies[i],
			     pst->dependencies[i]->filename);
      }

  /* Do nothing if this is a dummy psymtab.  */

  if (pst->n_global_syms == 0 && pst->n_static_syms == 0
      && pst->textlow == 0 && pst->texthigh == 0)
    return;

  /* Now read the symbols for this symtab */

  cur_bfd = CUR_BFD (pst);
  current_objfile = pst->objfile;
  cur_fd = FDR_IDX (pst);
  fh = (cur_fd == -1) ? (FDR *) NULL : ecoff_data (cur_bfd)->fdr + cur_fd;
  cur_fdr = fh;

  /* See comment in parse_partial_symbols about the @stabs sentinel. */
  processing_gcc_compilation = 0;
  if (fh != (FDR *) NULL && fh->csym >= 2)
    {
      SYMR sh;

      ecoff_swap_sym_in (cur_bfd,
			 (ecoff_data (cur_bfd)->external_sym
			  + fh->isymBase
			  + 1),
			 &sh);
      if (STREQ (ecoff_data (cur_bfd)->ss + fh->issBase + sh.iss,
		 stabs_symbol))
	{
	  /* We indicate that this is a GCC compilation so that certain
	     features will be enabled in stabsread/dbxread.  */
	  processing_gcc_compilation = 2;
	}
    }

  if (processing_gcc_compilation != 0)
    {
      struct pdr_ext *pdr_ptr;
      struct pdr_ext *pdr_end;
      int first_pdr;
      unsigned long first_off;

      /* This symbol table contains stabs-in-ecoff entries.  */

      /* Parse local symbols first */

      if (fh->csym <= 2)	/* FIXME, this blows psymtab->symtab ptr */
	{
	  current_objfile = NULL;
	  return;
	}
      for (cur_sdx = 2; cur_sdx < fh->csym; cur_sdx++)
	{
	  SYMR sh;
	  char *name;
	  CORE_ADDR valu;

	  ecoff_swap_sym_in (cur_bfd,
			     (ecoff_data (cur_bfd)->external_sym
			      + fh->isymBase
			      + cur_sdx),
			     &sh);
	  name = ecoff_data (cur_bfd)->ss + fh->issBase + sh.iss;
	  valu = sh.value;
	  if (MIPS_IS_STAB (&sh))
	    {
	      int type_code = MIPS_UNMARK_STAB (sh.index);
	      process_one_symbol (type_code, 0, valu, name,
				  pst->section_offsets, pst->objfile);
	      if (type_code == N_FUN)
		{
		  /* Make up special symbol to contain
		     procedure specific info */
		  struct mips_extra_func_info *e =
		    ((struct mips_extra_func_info *)
		     obstack_alloc (&current_objfile->symbol_obstack,
				    sizeof (struct mips_extra_func_info)));
		  struct symbol *s = new_symbol (MIPS_EFI_SYMBOL_NAME);
		  SYMBOL_NAMESPACE (s) = LABEL_NAMESPACE;
		  SYMBOL_CLASS (s) = LOC_CONST;
		  SYMBOL_TYPE (s) = builtin_type_void;
		  SYMBOL_VALUE (s) = (int) e;
		  add_symbol_to_list (s, &local_symbols);
		}
	    }
	  else if (sh.st == stLabel && sh.index != indexNil)
	    {
	      /* Handle encoded stab line number. */
	      record_line (current_subfile, sh.index, valu);
	    }
	  else if (sh.st == stProc || sh.st == stStaticProc || sh.st == stEnd)
	    /* These are generated by gcc-2.x, do not complain */
	    ;
	  else
	    complain (&stab_unknown_complaint, name);
	}
      st = end_symtab (pst->texthigh, 0, 0, pst->objfile, SECT_OFF_TEXT);
      end_stabs ();

      /* Sort the symbol table now, we are done adding symbols to it.
	 We must do this before parse_procedure calls lookup_symbol.  */
      sort_symtab_syms (st);

      /* This may not be necessary for stabs symtabs.  FIXME.  */
      sort_blocks (st);

      /* Fill in procedure info next.  */
      first_pdr = 1;
      pdr_ptr = ecoff_data (cur_bfd)->external_pdr + fh->ipdFirst;
      pdr_end = pdr_ptr + fh->cpd;
      for (; pdr_ptr < pdr_end; pdr_ptr++)
	{
	  PDR pr;

	  ecoff_swap_pdr_in (cur_bfd, pdr_ptr, &pr);
	  if (first_pdr)
	    {
	      first_off = pr.adr;
	      first_pdr = 0;
	    }
	  parse_procedure (&pr, st, first_off);
	}
    }
  else
    {
      /* This symbol table contains ordinary ecoff entries.  */

      /* FIXME:  doesn't use pst->section_offsets.  */

      int f_max;
      int maxlines;
      EXTR *ext_ptr;

      /* How many symbols will we need */
      /* FIXME, this does not count enum values. */
      f_max = pst->n_global_syms + pst->n_static_syms;
      if (fh == 0)
	{
	  maxlines = 0;
	  st = new_symtab ("unknown", f_max, 0, pst->objfile);
	}
      else
	{
	  f_max += fh->csym + fh->cpd;
	  maxlines = 2 * fh->cline;
	  st = new_symtab (pst->filename, 2 * f_max, maxlines, pst->objfile);
	}

      lines = LINETABLE (st);
      pending_list = PST_PRIVATE (pst)->pending_list;
      if (pending_list == 0)
	{
	  pending_list = ((struct mips_pending **)
			  xzalloc (ecoff_data (cur_bfd)->symbolic_header.ifdMax
				   * sizeof (struct mips_pending *)));
	  PST_PRIVATE (pst)->pending_list = pending_list;
	}

      /* Get a new lexical context */

      push_parse_stack ();
      top_stack->cur_st = st;
      top_stack->cur_block = BLOCKVECTOR_BLOCK (BLOCKVECTOR (st),
						STATIC_BLOCK);
      BLOCK_START (top_stack->cur_block) = fh ? fh->adr : 0;
      BLOCK_END (top_stack->cur_block) = 0;
      top_stack->blocktype = stFile;
      top_stack->maxsyms = 2 * f_max;
      top_stack->cur_type = 0;
      top_stack->procadr = 0;
      top_stack->numargs = 0;

      if (fh)
	{
	  struct sym_ext *sym_ptr;
	  struct sym_ext *sym_end;

	  /* Parse local symbols first */
	  sym_ptr = ecoff_data (cur_bfd)->external_sym + fh->isymBase;
	  sym_end = sym_ptr + fh->csym;
	  while (sym_ptr < sym_end)
	    {
	      SYMR sh;
	      int c;

	      ecoff_swap_sym_in (cur_bfd, sym_ptr, &sh);
	      c = parse_symbol (&sh,
				(ecoff_data (cur_bfd)->external_aux
				 + fh->iauxBase),
				sym_ptr, fh->fBigendian);
	      /* FIXME: We must swap the modified symbol back out,
		 although we would rather not.  See parse_symbol.  */
	      ecoff_swap_sym_out (cur_bfd, &sh, sym_ptr);
	      sym_ptr += c;
	    }

	  /* Linenumbers.  At the end, check if we can save memory.
	     parse_lines has to look ahead an arbitrary number of PDR
	     structures, so we swap them all first.  */
	  if (fh->cpd > 0)
	    {
	      PDR *pr_block;
	      struct cleanup *old_chain;
	      struct pdr_ext *pdr_ptr;
	      struct pdr_ext *pdr_end;
	      PDR *pdr_in;
	      PDR *pdr_in_end;

	      pr_block = (PDR *) xmalloc (fh->cpd * sizeof (PDR));

	      old_chain = make_cleanup (free, pr_block);

	      pdr_ptr = ecoff_data (cur_bfd)->external_pdr + fh->ipdFirst;
	      pdr_end = pdr_ptr + fh->cpd;
	      pdr_in = pr_block;
	      for (; pdr_ptr < pdr_end; pdr_ptr++, pdr_in++)
		ecoff_swap_pdr_in (cur_bfd, pdr_ptr, pdr_in);

	      parse_lines (fh, pr_block, lines);
	      if (lines->nitems < fh->cline)
		lines = shrink_linetable (lines);

	      /* Fill in procedure info next.  */
	      pdr_in = pr_block;
	      pdr_in_end = pdr_in + fh->cpd;
	      for (; pdr_in < pdr_in_end; pdr_in++)
		parse_procedure (pdr_in, 0, pr_block->adr);

	      do_cleanups (old_chain);
	    }
	}

      LINETABLE (st) = lines;

      /* .. and our share of externals.
	 XXX use the global list to speed up things here. how?
	 FIXME, Maybe quit once we have found the right number of ext's? */
      top_stack->cur_st = st;
      top_stack->cur_block = BLOCKVECTOR_BLOCK (BLOCKVECTOR (top_stack->cur_st),
						GLOBAL_BLOCK);
      top_stack->blocktype = stFile;
      top_stack->maxsyms = (ecoff_data (cur_bfd)->symbolic_header.isymMax
			    + ecoff_data (cur_bfd)->symbolic_header.ipdMax
			    + ecoff_data (cur_bfd)->symbolic_header.iextMax);

      ext_ptr = PST_PRIVATE (pst)->extern_tab;
      for (i = PST_PRIVATE (pst)->extern_count; --i >= 0; ext_ptr++)
	parse_external (ext_ptr, 1, fh->fBigendian);

      /* If there are undefined, tell the user */
      if (n_undef_symbols)
	{
	  printf_filtered ("File %s contains %d unresolved references:",
			   st->filename, n_undef_symbols);
	  printf_filtered ("\n\t%4d variables\n\t%4d procedures\n\t%4d labels\n",
			   n_undef_vars, n_undef_procs, n_undef_labels);
	  n_undef_symbols = n_undef_labels = n_undef_vars = n_undef_procs = 0;

	}
      pop_parse_stack ();

      /* Sort the symbol table now, we are done adding symbols to it.*/
      sort_symtab_syms (st);

      sort_blocks (st);
    }

  /* Now link the psymtab and the symtab.  */
  pst->symtab = st;

  current_objfile = NULL;
}

/* Ancillary parsing procedures. */

/* Lookup the type at relative index RN.  Return it in TPP
   if found and in any event come up with its name PNAME.
   BIGEND says whether aux symbols are big-endian or not (from fh->fBigendian).
   Return value says how many aux symbols we ate. */

static int
cross_ref (ax, tpp, type_code, pname, bigend)
     union aux_ext *ax;
     struct type **tpp;
     enum type_code type_code;	/* Use to alloc new type if none is found. */
     char **pname;
     int bigend;
{
  RNDXR rn[1];
  unsigned rf;
  int result = 1;

  ecoff_swap_rndx_in (bigend, &ax->a_rndx, rn);

  /* Escape index means 'the next one' */
  if (rn->rfd == 0xfff)
    {
      result++;
      rf = AUX_GET_ISYM (bigend, ax + 1);
    }
  else
    {
      rf = rn->rfd;
    }

  if (rf == -1)
    {
      /* Ooops */
      *pname = "<undefined>";
    }
  else
    {
      /*
       * Find the relative file descriptor and the symbol in it
       */
      FDR *fh = get_rfd (cur_fd, rf);
      struct sym_ext *esh;
      SYMR sh;
      struct type *t;

      /* If we have processed this symbol then we left a forwarding
	 pointer to the corresponding GDB symbol.  If not, we`ll put
	 it in a list of pending symbols, to be processed later when
	 the file will be.  In any event, we collect the name for the
	 type here.  Which is why we made a first pass at strings.  */

      esh = ecoff_data (cur_bfd)->external_sym + fh->isymBase + rn->index;
      ecoff_swap_sym_in (cur_bfd, esh, &sh);

      /* Careful, we might be looking at .o files */
      if (sh.iss == 0)
	*pname = "<undefined>";
      else if (rn->rfd == 0xfff && rn->index == 0)
	/* For structs, unions and enums, rn->rfd is 0xfff and the index
	   is a relative symbol number for the type, but an index of 0
	   seems to mean that we don't know.  This is said to fix a problem
	   with "info func opendir" on an SGI showing
	   "struct BSDopendir.c *BSDopendir();".  */
	*pname = "<unknown>";
      else
	*pname = ecoff_data (cur_bfd)->ss + fh->issBase + sh.iss;

      /* Have we parsed it ? */
      if (sh.value != 0 && sh.st == stParsed)
	{
	  t = (struct type *) sh.value;
	  *tpp = t;
	}
      else
	{
	  /* Avoid duplicates */
	  struct mips_pending *p = is_pending_symbol (fh, esh);
	  if (p)
	    *tpp = p->t;
	  else
	    {
	      *tpp = init_type (type_code, 0, 0, (char *) NULL,
				(struct objfile *) NULL);
	      add_pending (fh, esh, *tpp);
	    }
	}
    }

  /* We used one auxent normally, two if we got a "next one" rf. */
  return result;
}


/* Quick&dirty lookup procedure, to avoid the MI ones that require
   keeping the symtab sorted */

static struct symbol *
mylookup_symbol (name, block, namespace, class)
     char *name;
     register struct block *block;
     enum namespace namespace;
     enum address_class class;
{
  register int bot, top, inc;
  register struct symbol *sym;

  bot = 0;
  top = BLOCK_NSYMS (block);
  inc = name[0];
  while (bot < top)
    {
      sym = BLOCK_SYM (block, bot);
      if (SYMBOL_NAME (sym)[0] == inc
	  && SYMBOL_NAMESPACE (sym) == namespace
	  && SYMBOL_CLASS (sym) == class
	  && STREQ (SYMBOL_NAME (sym), name))
	return sym;
      bot++;
    }
  block = BLOCK_SUPERBLOCK (block);
  if (block)
    return mylookup_symbol (name, block, namespace, class);
  return 0;
}


/* Add a new symbol S to a block B.
   Infrequently, we will need to reallocate the block to make it bigger.
   We only detect this case when adding to top_stack->cur_block, since
   that's the only time we know how big the block is.  FIXME.  */

static void
add_symbol (s, b)
     struct symbol *s;
     struct block *b;
{
  int nsyms = BLOCK_NSYMS (b)++;
  struct block *origb;
  struct parse_stack *stackp;

  if (b == top_stack->cur_block &&
      nsyms >= top_stack->maxsyms)
    {
      complain (&block_overflow_complaint, SYMBOL_NAME (s));
      /* In this case shrink_block is actually grow_block, since
		   BLOCK_NSYMS(b) is larger than its current size.  */
      origb = b;
      b = shrink_block (top_stack->cur_block, top_stack->cur_st);

      /* Now run through the stack replacing pointers to the
	 original block.  shrink_block has already done this
	 for the blockvector and BLOCK_FUNCTION.  */
      for (stackp = top_stack; stackp; stackp = stackp->next)
	{
	  if (stackp->cur_block == origb)
	    {
	      stackp->cur_block = b;
	      stackp->maxsyms = BLOCK_NSYMS (b);
	    }
	}
    }
  BLOCK_SYM (b, nsyms) = s;
}

/* Add a new block B to a symtab S */

static void
add_block (b, s)
     struct block *b;
     struct symtab *s;
{
  struct blockvector *bv = BLOCKVECTOR (s);

  bv = (struct blockvector *) xrealloc ((PTR) bv,
					(sizeof (struct blockvector)
					 + BLOCKVECTOR_NBLOCKS (bv)
					 * sizeof (bv->block)));
  if (bv != BLOCKVECTOR (s))
    BLOCKVECTOR (s) = bv;

  BLOCKVECTOR_BLOCK (bv, BLOCKVECTOR_NBLOCKS (bv)++) = b;
}

/* Add a new linenumber entry (LINENO,ADR) to a linevector LT.
   MIPS' linenumber encoding might need more than one byte
   to describe it, LAST is used to detect these continuation lines.

   Combining lines with the same line number seems like a bad idea.
   E.g: There could be a line number entry with the same line number after the
   prologue and GDB should not ignore it (this is a better way to find
   a prologue than mips_skip_prologue).
   But due to the compressed line table format there are line number entries
   for the same line which are needed to bridge the gap to the next
   line number entry. These entries have a bogus address info with them
   and we are unable to tell them from intended duplicate line number
   entries.
   This is another reason why -ggdb debugging format is preferable.  */

static int
add_line (lt, lineno, adr, last)
     struct linetable *lt;
     int lineno;
     CORE_ADDR adr;
     int last;
{
  if (last == 0)
    last = -2;			/* make sure we record first line */

  if (last == lineno)		/* skip continuation lines */
    return lineno;

  lt->item[lt->nitems].line = lineno;
  lt->item[lt->nitems++].pc = adr << 2;
  return lineno;
}

/* Sorting and reordering procedures */

/* Blocks with a smaller low bound should come first */

static int
compare_blocks (arg1, arg2)
     const void *arg1, *arg2;
{
  register int addr_diff;
  struct block **b1 = (struct block **) arg1;
  struct block **b2 = (struct block **) arg2;

  addr_diff = (BLOCK_START ((*b1))) - (BLOCK_START ((*b2)));
  if (addr_diff == 0)
    return (BLOCK_END ((*b2))) - (BLOCK_END ((*b1)));
  return addr_diff;
}

/* Sort the blocks of a symtab S.
   Reorder the blocks in the blockvector by code-address,
   as required by some MI search routines */

static void
sort_blocks (s)
     struct symtab *s;
{
  struct blockvector *bv = BLOCKVECTOR (s);

  if (BLOCKVECTOR_NBLOCKS (bv) <= 2)
    {
      /* Cosmetic */
      if (BLOCK_END (BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK)) == 0)
	BLOCK_START (BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK)) = 0;
      if (BLOCK_END (BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK)) == 0)
	BLOCK_START (BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK)) = 0;
      return;
    }
  /*
   * This is very unfortunate: normally all functions are compiled in
   * the order they are found, but if the file is compiled -O3 things
   * are very different.  It would be nice to find a reliable test
   * to detect -O3 images in advance.
   */
  if (BLOCKVECTOR_NBLOCKS (bv) > 3)
    qsort (&BLOCKVECTOR_BLOCK (bv, FIRST_LOCAL_BLOCK),
	   BLOCKVECTOR_NBLOCKS (bv) - FIRST_LOCAL_BLOCK,
	   sizeof (struct block *),
	   compare_blocks);

  {
    register CORE_ADDR high = 0;
    register int i, j = BLOCKVECTOR_NBLOCKS (bv);

    for (i = FIRST_LOCAL_BLOCK; i < j; i++)
      if (high < BLOCK_END (BLOCKVECTOR_BLOCK (bv, i)))
	high = BLOCK_END (BLOCKVECTOR_BLOCK (bv, i));
    BLOCK_END (BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK)) = high;
  }

  BLOCK_START (BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK)) =
    BLOCK_START (BLOCKVECTOR_BLOCK (bv, FIRST_LOCAL_BLOCK));

  BLOCK_START (BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK)) =
    BLOCK_START (BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK));
  BLOCK_END (BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK)) =
    BLOCK_END (BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK));
}


/* Constructor/restructor/destructor procedures */

/* Allocate a new symtab for NAME.  Needs an estimate of how many symbols
   MAXSYMS and linenumbers MAXLINES we'll put in it */

static struct symtab *
new_symtab (name, maxsyms, maxlines, objfile)
     char *name;
     int maxsyms;
     int maxlines;
     struct objfile *objfile;
{
  struct symtab *s = allocate_symtab (name, objfile);

  LINETABLE (s) = new_linetable (maxlines);

  /* All symtabs must have at least two blocks */
  BLOCKVECTOR (s) = new_bvect (2);
  BLOCKVECTOR_BLOCK (BLOCKVECTOR (s), GLOBAL_BLOCK) = new_block (maxsyms);
  BLOCKVECTOR_BLOCK (BLOCKVECTOR (s), STATIC_BLOCK) = new_block (maxsyms);
  BLOCK_SUPERBLOCK (BLOCKVECTOR_BLOCK (BLOCKVECTOR (s), STATIC_BLOCK)) =
    BLOCKVECTOR_BLOCK (BLOCKVECTOR (s), GLOBAL_BLOCK);

  s->free_code = free_linetable;

  return (s);
}

/* Allocate a new partial_symtab NAME */

static struct partial_symtab *
new_psymtab (name, objfile)
     char *name;
     struct objfile *objfile;
{
  struct partial_symtab *psymtab;

  psymtab = allocate_psymtab (name, objfile);

  /* Keep a backpointer to the file's symbols */

  psymtab->read_symtab_private = ((char *)
				  obstack_alloc (&objfile->psymbol_obstack,
						 sizeof (struct symloc)));
  memset ((PTR) psymtab->read_symtab_private, 0, sizeof (struct symloc));
  CUR_BFD (psymtab) = cur_bfd;

  /* The way to turn this into a symtab is to call... */
  psymtab->read_symtab = mipscoff_psymtab_to_symtab;
  return (psymtab);
}


/* Allocate a linetable array of the given SIZE.  Since the struct
   already includes one item, we subtract one when calculating the
   proper size to allocate.  */

static struct linetable *
new_linetable (size)
     int size;
{
  struct linetable *l;

  size = (size - 1) * sizeof (l->item) + sizeof (struct linetable);
  l = (struct linetable *) xmalloc (size);
  l->nitems = 0;
  return l;
}

/* Oops, too big. Shrink it.  This was important with the 2.4 linetables,
   I am not so sure about the 3.4 ones.

   Since the struct linetable already includes one item, we subtract one when
   calculating the proper size to allocate.  */

static struct linetable *
shrink_linetable (lt)
     struct linetable *lt;
{

  return (struct linetable *) xrealloc ((PTR) lt,
					(sizeof (struct linetable)
					 + ((lt->nitems - 1)
					    * sizeof (lt->item))));
}

/* Allocate and zero a new blockvector of NBLOCKS blocks. */

static struct blockvector *
new_bvect (nblocks)
     int nblocks;
{
  struct blockvector *bv;
  int size;

  size = sizeof (struct blockvector) + nblocks * sizeof (struct block *);
  bv = (struct blockvector *) xzalloc (size);

  BLOCKVECTOR_NBLOCKS (bv) = nblocks;

  return bv;
}

/* Allocate and zero a new block of MAXSYMS symbols */

static struct block *
new_block (maxsyms)
     int maxsyms;
{
  int size = sizeof (struct block) + (maxsyms - 1) * sizeof (struct symbol *);

  return (struct block *) xzalloc (size);
}

/* Ooops, too big. Shrink block B in symtab S to its minimal size.
   Shrink_block can also be used by add_symbol to grow a block.  */

static struct block *
shrink_block (b, s)
     struct block *b;
     struct symtab *s;
{
  struct block *new;
  struct blockvector *bv = BLOCKVECTOR (s);
  int i;

  /* Just reallocate it and fix references to the old one */

  new = (struct block *) xrealloc ((PTR) b,
				   (sizeof (struct block)
				    + ((BLOCK_NSYMS (b) - 1)
				       * sizeof (struct symbol *))));

  /* Should chase pointers to old one.  Fortunately, that`s just
	   the block`s function and inferior blocks */
  if (BLOCK_FUNCTION (new) && SYMBOL_BLOCK_VALUE (BLOCK_FUNCTION (new)) == b)
    SYMBOL_BLOCK_VALUE (BLOCK_FUNCTION (new)) = new;
  for (i = 0; i < BLOCKVECTOR_NBLOCKS (bv); i++)
    if (BLOCKVECTOR_BLOCK (bv, i) == b)
      BLOCKVECTOR_BLOCK (bv, i) = new;
    else if (BLOCK_SUPERBLOCK (BLOCKVECTOR_BLOCK (bv, i)) == b)
      BLOCK_SUPERBLOCK (BLOCKVECTOR_BLOCK (bv, i)) = new;
  return new;
}

/* Create a new symbol with printname NAME */

static struct symbol *
new_symbol (name)
     char *name;
{
  struct symbol *s = ((struct symbol *)
		      obstack_alloc (&current_objfile->symbol_obstack,
				     sizeof (struct symbol)));

  memset ((PTR) s, 0, sizeof (*s));
  SYMBOL_NAME (s) = name;
  return s;
}

/* Create a new type with printname NAME */

static struct type *
new_type (name)
     char *name;
{
  struct type *t;

  t = alloc_type (current_objfile);
  TYPE_NAME (t) = name;
  TYPE_CPLUS_SPECIFIC (t) = (struct cplus_struct_type *) &cplus_struct_default;
  return t;
}


/* Things used for calling functions in the inferior.
   These functions are exported to our companion
   mips-tdep.c file and are here because they play
   with the symbol-table explicitly. */

/* Sigtramp: make sure we have all the necessary information
   about the signal trampoline code. Since the official code
   from MIPS does not do so, we make up that information ourselves.
   If they fix the library (unlikely) this code will neutralize itself. */

static void
fixup_sigtramp ()
{
  struct symbol *s;
  struct symtab *st;
  struct block *b, *b0;

  sigtramp_address = -1;

  /* We have to handle the following cases here:
     a) The Mips library has a sigtramp label within sigvec.
     b) Irix has a _sigtramp which we want to use, but it also has sigvec.  */
  s = lookup_symbol ("sigvec", 0, VAR_NAMESPACE, 0, NULL);
  if (s != 0)
    {
      b0 = SYMBOL_BLOCK_VALUE (s);
      s = lookup_symbol ("sigtramp", b0, VAR_NAMESPACE, 0, NULL);
    }
  if (s == 0)
    {
      /* No sigvec or no sigtramp inside sigvec, try _sigtramp.  */
      s = lookup_symbol ("_sigtramp", 0, VAR_NAMESPACE, 0, NULL);
    }

  /* But maybe this program uses its own version of sigvec */
  if (s == 0)
    return;

  /* Did we or MIPSco fix the library ? */
  if (SYMBOL_CLASS (s) == LOC_BLOCK)
    {
      sigtramp_address = BLOCK_START (SYMBOL_BLOCK_VALUE (s));
      sigtramp_end = BLOCK_END (SYMBOL_BLOCK_VALUE (s));
      return;
    }

  sigtramp_address = SYMBOL_VALUE (s);
  sigtramp_end = sigtramp_address + 0x88;	/* black magic */

  /* But what symtab does it live in ? */
  st = find_pc_symtab (SYMBOL_VALUE (s));

  /*
   * Ok, there goes the fix: turn it into a procedure, with all the
   * needed info.  Note we make it a nested procedure of sigvec,
   * which is the way the (assembly) code is actually written.
   */
  SYMBOL_NAMESPACE (s) = VAR_NAMESPACE;
  SYMBOL_CLASS (s) = LOC_BLOCK;
  SYMBOL_TYPE (s) = init_type (TYPE_CODE_FUNC, 4, 0, (char *) NULL,
			       (struct objfile *) NULL);
  TYPE_TARGET_TYPE (SYMBOL_TYPE (s)) = builtin_type_void;

  /* Need a block to allocate MIPS_EFI_SYMBOL_NAME in */
  b = new_block (1);
  SYMBOL_BLOCK_VALUE (s) = b;
  BLOCK_START (b) = sigtramp_address;
  BLOCK_END (b) = sigtramp_end;
  BLOCK_FUNCTION (b) = s;
  BLOCK_SUPERBLOCK (b) = BLOCK_SUPERBLOCK (b0);
  add_block (b, st);
  sort_blocks (st);

  /* Make a MIPS_EFI_SYMBOL_NAME entry for it */
  {
    struct mips_extra_func_info *e =
      ((struct mips_extra_func_info *)
       xzalloc (sizeof (struct mips_extra_func_info)));

    e->numargs = 0;		/* the kernel thinks otherwise */
    /* align_longword(sigcontext + SIGFRAME) */
    e->pdr.frameoffset = 0x150;
    e->pdr.framereg = SP_REGNUM;
    /* read_next_frame_reg provides the true pc at the time of signal */
    e->pdr.pcreg = PC_REGNUM;
    e->pdr.regmask = -2;
    e->pdr.regoffset = -(41 * sizeof (int));
    e->pdr.fregmask = -1;
    e->pdr.fregoffset = -(7 * sizeof (int));
    e->pdr.isym = (long) s;

    current_objfile = st->objfile;	/* Keep new_symbol happy */
    s = new_symbol (MIPS_EFI_SYMBOL_NAME);
    SYMBOL_VALUE (s) = (int) e;
    SYMBOL_NAMESPACE (s) = LABEL_NAMESPACE;
    SYMBOL_CLASS (s) = LOC_CONST;
    SYMBOL_TYPE (s) = builtin_type_void;
    current_objfile = NULL;
  }

  BLOCK_SYM (b, BLOCK_NSYMS (b)++) = s;
}


/* Fake up identical offsets for all sections.  */

struct section_offsets *
mipscoff_symfile_offsets (objfile, addr)
     struct objfile *objfile;
     CORE_ADDR addr;
{
  struct section_offsets *section_offsets;
  int i;

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
  "ecoff",			/* sym_name: name or name prefix of BFD target type */
  5,				/* sym_namelen: number of significant sym_name chars */
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

  /* Missing basic types */

  builtin_type_string =
    init_type (TYPE_CODE_STRING,
	       TARGET_CHAR_BIT / TARGET_CHAR_BIT,
	       0, "string",
	       (struct objfile *) NULL);
  builtin_type_complex =
    init_type (TYPE_CODE_FLT,
	       TARGET_COMPLEX_BIT / TARGET_CHAR_BIT,
	       0, "complex",
	       (struct objfile *) NULL);
  builtin_type_double_complex =
    init_type (TYPE_CODE_FLT,
	       TARGET_DOUBLE_COMPLEX_BIT / TARGET_CHAR_BIT,
	       0, "double complex",
	       (struct objfile *) NULL);
  builtin_type_fixed_dec =
    init_type (TYPE_CODE_INT,
	       TARGET_INT_BIT / TARGET_CHAR_BIT,
	       0, "fixed decimal",
	       (struct objfile *) NULL);
  builtin_type_float_dec =
    init_type (TYPE_CODE_FLT,
	       TARGET_DOUBLE_BIT / TARGET_CHAR_BIT,
	       0, "floating decimal",
	       (struct objfile *) NULL);
}
