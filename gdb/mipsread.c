/* Read a symbol table in MIPS' format (Third-Eye).
   Copyright (C) 1986, 1987, 1989-1991 Free Software Foundation, Inc.
   Contributed by Alessandro Forin (af@cs.cmu.edu) at CMU

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
#include "param.h"
#include "obstack.h"
#include <sys/param.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "defs.h"
#include "symtab.h"
#include "gdbcore.h"
#include "symfile.h"
#ifdef	CMUCS
#include <mips/syms.h>
#else /* not CMUCS */
#include <syms.h>
#endif /* not CMUCS */

/* Since these things are defined differently on various systems I'll
   (re)define here what I really need in this module.  I only assume the
   three standard COFF structure definitions: filehdr, aouthdr, scnhdr */
#define MIPS		/* Kludge to get MIPS version of coff */
#undef _ETEXT		/* Avoid duplicated define from <syms.h> */
#include "ecoff.h"

struct coff_exec {
	struct external_filehdr f;
	struct external_aouthdr a;
};
#undef a_magic
#undef a_text
#undef a_data
#undef a_bss
#undef a_syms
#undef a_entry
#define a_magic	a.magic		/* magic number */
#define a_text	a.tsize		/* size of text segment */
#define a_data	a.dsize		/* size of initialized data */
#define a_bss	a.bsize		/* size of uninitialized data */
#define a_syms	f.f_nsyms	/* size of symbol table */
#define a_entry	a.entry		/* entry point */

#undef	N_BADMAG
#define	N_BADMAG(x) \
    (((x).a_magic)!=OMAGIC && ((x).a_magic)!=NMAGIC && ((x).a_magic)!=ZMAGIC)

/* Things we import explicitly from other modules */

extern int	     info_verbose;
extern struct block *block_for_pc();
extern void	     sort_symtab_syms();

/* Forward declarations */

static void psymtab_to_symtab_1();

/* Macros and extra defs */

struct complaint unknown_ext_complaint = 
	{"unknown external symbol %s", 0, 0};

/* Already parsed symbols are marked specially */

#define stParsed stType

/* Puns: hard to find whether -g was used and how */

#define MIN_GLEVEL GLEVEL_0
#define compare_glevel(a,b)					\
	(((a) == GLEVEL_3) ? ((b) < GLEVEL_3) :			\
	 ((b) == GLEVEL_3) ? -1 : (int)((b) - (a)))

/* When looking at .o files avoid tripping over bad addresses */

#define SAFE_TEXT_ADDR 0x400000
#define SAFE_DATA_ADDR 0x10000000

#define UNSAFE_DATA_ADDR(p)	((unsigned)p < SAFE_DATA_ADDR || (unsigned)p > 2*SAFE_DATA_ADDR)

/* Things that really are local to this module */

/* GDB symtable for the current compilation unit */

static struct symtab *cur_stab;

/* Pointer to current file decriptor record, and its index */

static FDR	*cur_fdr;
static int	 cur_fd;

/* Index of current symbol */

static int	 cur_sdx;

/* Note how much "debuggable" this image is.  We would like
   to see at least one FDR with full symbols */

static max_gdbinfo;
static max_glevel;

/* When examining .o files, report on undefined symbols */

static int n_undef_symbols, n_undef_labels, n_undef_vars, n_undef_procs;

/* Extra builtin types */

struct type *builtin_type_complex;
struct type *builtin_type_double_complex;
struct type *builtin_type_fixed_dec;
struct type *builtin_type_float_dec;
struct type *builtin_type_string;

/* Template types */

static struct type *builtin_type_ptr;
static struct type *builtin_type_struct;
static struct type *builtin_type_union;
static struct type *builtin_type_enum;
static struct type *builtin_type_range;
static struct type *builtin_type_set;


/* Forward decls */

static struct symbol	*new_symbol();
static struct type	*new_type();
static struct field	*new_field();
static struct block	*new_block();
static struct symtab	*new_symtab();
static struct linetable	*new_linetable();
static struct blockvector *new_bvect();

static struct type	*parse_type();
static struct type	*make_type();
static struct symbol	*mylookup_symbol();
static struct block	*shrink_block();

static int compare_symtabs();
static int compare_psymtabs();
static int compare_blocks();

static struct partial_symtab *new_psymtab();
static struct partial_symbol *new_psymbol();
static struct partial_symtab *parse_fdr();
static int compare_psymbols();

static void reorder_symtabs();
static void reorder_psymtabs();

/* Things we export to other modules */


/* Lists of partial symbols */

struct psymbol_allocation_list global_psymbols, static_psymbols;

/* Address bounds for the signal trampoline in inferior, if any */

CORE_ADDR sigtramp_address, sigtramp_end;


/* Functions that we really export */

/* THIS DESCRIPTION IS OBSOLETE POST-BFD; FIXME! */
/* Basically, this module must provide two functions: symbol_file_command()
   which loads the symbol table from a file, and add_file_command() which
   adds more symbols to the symbol table (incrementally).

   These two functions only do the minimum work necessary for letting the
   user "name" things symbolically, they do not read the entire symtab.
   Instead, they read in the external symbols and put them in partial
   symbol tables.  When more extensive information is requested of a
   file the corresponding partial symbol table is mutated into a full
   fledged symbol table by going back and reading the relative symbols
   for real.  mipscoff_psymtab_to_symtab() is the function that does this */

/* The entry point (starting address) of the file, if it is an executable.  */

static CORE_ADDR entry_point;

extern CORE_ADDR startup_file_start;	/* From blockframe.c */
extern CORE_ADDR startup_file_end;	/* From blockframe.c */

void
mipscoff_new_init()
{
}

void
mipscoff_symfile_init (sf)
     struct sym_fns *sf;
{
  bfd *abfd = sf->sym_bfd;
  sf->sym_private = NULL;
  /* Save startup file's range of PC addresses to help blockframe.c
     decide where the bottom of the stack is.  */
  if (bfd_get_file_flags (abfd) & EXEC_P)
    {
      /* Executable file -- record its entry point so we'll recognize
	 the startup file because it contains the entry point.  */
      entry_point = bfd_get_start_address (abfd);
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
}

void
mipscoff_symfile_read(sf, addr, mainline)
     struct sym_fns *sf;
     CORE_ADDR addr;
     int mainline;
{
  struct coff_symfile_info *info = (struct coff_symfile_info *)sf->sym_private;
  bfd *abfd = sf->sym_bfd;
  char *name = bfd_get_filename (abfd);
  int desc;
  register int val;
  int num_symbols;
  int symtab_offset;
  int stringtab_offset;

/* WARNING WILL ROBINSON!  ACCESSING BFD-PRIVATE DATA HERE!  FIXME!  */
   desc = fileno ((FILE *)(abfd->iostream));	/* Raw file descriptor */
   num_symbols = bfd_get_symcount (abfd);	/* How many syms */
/*   symtab_offset = obj_sym_filepos (abfd);	* Symbol table file offset */
/*   stringtab_offset = symtab_offset + num_symbols * SYMESZ;  * String tab */
/* End of warning */

#ifdef TDESC
  debug_info = text_hdr.s_relptr;
  if (tdesc_handle)
    {
       dc_terminate (tdesc_handle);
       tdesc_handle = 0;
    }
#endif

#if 0
  /* Read the line number table, all at once.  */
  info->min_lineno_offset = 0;
  info->max_lineno_offset = 0;
  bfd_map_over_sections (abfd, find_linenos, info);

  val = init_lineno (desc, info->min_lineno_offset, 
		     info->max_lineno_offset - info->min_lineno_offset);
  if (val < 0)
    error ("\"%s\": error reading line numbers\n", name);

  /* Now read the string table, all at once.  */

  val = init_stringtab (desc, stringtab_offset);
  if (val < 0)
    {
      free_all_symtabs ();		/* FIXME blows whole symtab */
      printf ("\"%s\": can't get string table", name);
      fflush (stdout);
      return;
    }
  make_cleanup (free_stringtab, 0);
#endif

  /* Position to read the symbol table.  Do not read it all at once. */
  val = lseek (desc, (long)symtab_offset, 0);
  if (val < 0)
    perror_with_name (name);

  init_misc_bunches ();
  make_cleanup (discard_misc_bunches, 0);

  /* Now that the executable file is positioned at symbol table,
     process it and define symbols accordingly.  */

  read_mips_symtab(abfd, desc, 0);

/*  patch_opaque_types ();*/

  /* Sort symbols alphabetically within each block.  */

  sort_all_symtab_syms ();

  /* Go over the misc symbol bunches and install them in vector.  */

  condense_misc_bunches (0);

  /* Make a default for file to list.  */

  select_source_symtab (0);	/* FIXME, this might be too slow, see dbxread */
}  
  
void
mipscoff_symfile_discard()
{
}
  
/* Exported procedure: Allocate zeroed memory */

char *xzalloc(size)
{
	char           *p = xmalloc(size);

	bzero(p, size);
	return p;
}

/* Exported procedure: Builds a symtab from the PST partial one.
   Restores the environment in effect when PST was created, delegates
   most of the work to an ancillary procedure, and sorts
   and reorders the symtab list at the end */

/* Forward decls */
static HDRR	*cur_hdr;	/* MIPS symtab header for the current file */

void
mipscoff_psymtab_to_symtab(pst)
	struct partial_symtab *pst;
{
	struct symtab  *ret;
	int             i;

	if (!pst)
		return;

	if (info_verbose) {
		printf_filtered("Reading in symbols for %s...", pst->filename);
		fflush(stdout);
	}
	/* Restore the header and list of pending typedefs */
	cur_hdr = (HDRR *) pst->ldsymlen;

	psymtab_to_symtab_1(pst);

	reorder_symtabs();

	/* Finish up the debug error message.  */
	if (info_verbose)
		printf_filtered("done.\n");
}

/* Exported procedure: Is PC in the signal trampoline code */

int
in_sigtramp(pc, name)
	CORE_ADDR pc;
	char *name;
{
	if (sigtramp_address == 0)
		fixup_sigtramp();
	return (pc >= sigtramp_address && pc < sigtramp_end);
}

/* File-level interface functions */

/* Read the symtab information from file FSYM into memory.  Also,
   return address just past end of our text segment in *END_OF_TEXT_SEGP.  */

static
read_the_mips_symtab(abfd, fsym, end_of_text_segp)
	bfd 		*abfd;
	int		fsym;
	CORE_ADDR	*end_of_text_segp;
{
	int             stsize, st_hdrsize;
	unsigned        st_filptr;
	HDRR            st_hdr;
	/* Header for executable/object file we read symbols from */
	struct coff_exec filhdr;

	/* We get here with DESC pointing to the symtab header. But we need
	 * other info from the initial headers */
	lseek(fsym, 0L, 0);
	myread(fsym, &filhdr, sizeof filhdr);

	if (end_of_text_segp)
		*end_of_text_segp =
			bfd_h_get_32 (abfd, filhdr.a.text_start) +
			bfd_h_get_32 (abfd, filhdr.a.tsize);

	/* Find and read the symbol table header */
	st_hdrsize = bfd_h_get_32 (abfd, filhdr.f.f_nsyms);
	st_filptr  = bfd_h_get_32 (abfd, filhdr.f.f_symptr);
	if (st_filptr == 0)
		return 0;

	lseek(fsym, st_filptr, L_SET);
	if (st_hdrsize > sizeof (st_hdr))	/* Profanity check */
		abort();
	if (read(fsym, &st_hdr, st_hdrsize) != st_hdrsize)
		goto readerr;

	/* Find out how large the symbol table is */
	stsize = (st_hdr.cbExtOffset - (st_filptr + st_hdrsize))
		+ st_hdr.iextMax * cbEXTR;

	/* Allocate space for the symbol table.  Read it in.  */
	cur_hdr = (HDRR *) xmalloc(stsize + st_hdrsize);

	bcopy(&st_hdr, cur_hdr, st_hdrsize);
	if (read(fsym, (char *) cur_hdr + st_hdrsize, stsize) != stsize)
		goto readerr;

	/* Fixup file_pointers in it */
	fixup_symtab(cur_hdr, (char *) cur_hdr + st_hdrsize,
		     st_filptr + st_hdrsize);

	return;
readerr:
	error("Short read on %s", symfile);
}


/* Turn all file-relative pointers in the symtab described by HDR
   into memory pointers, given that the symtab itself is located
   at DATA in memory and F_PTR in the file. */

static
fixup_symtab( hdr, data, f_ptr)
	HDRR *hdr;
	char *data;
{
	int             f_idx, s_idx;
	FDR            *fh;
	SYMR	       *sh;
	OPTR	       *op;
	PDR	       *pr;
	EXTR	       *esh;

	/*
	 * These fields are useless (and empty) by now:
	 *	hdr->cbDnOffset, hdr->cbOptOffset
	 * We use them for other internal purposes.
	 */
	hdr->cbDnOffset = 0;
	hdr->cbOptOffset = 0;

#define FIX(off) \
        if (hdr->off) hdr->off = (unsigned int)data + (hdr->off - f_ptr);

        FIX(cbLineOffset);
        FIX(cbPdOffset);
        FIX(cbSymOffset);
        FIX(cbOptOffset);
        FIX(cbAuxOffset);
        FIX(cbSsOffset);
        FIX(cbSsExtOffset);
        FIX(cbFdOffset);
        FIX(cbRfdOffset);
        FIX(cbExtOffset);
#undef	FIX


	/*
	 * Fix all string pointers inside the symtab, and
	 * the FDR records.  Also fix other miscellany.
	 */
	for (f_idx = 0; f_idx < hdr->ifdMax; f_idx++) {
		register unsigned code_offset;

		/* Header itself, and strings */
		fh = (FDR *) (hdr->cbFdOffset) + f_idx;
		fh->issBase += hdr->cbSsOffset;
		if (fh->rss != -1)
			fh->rss = (long)fh->rss + fh->issBase;
		for (s_idx = 0; s_idx < fh->csym; s_idx++) {
			sh = (SYMR*)(hdr->cbSymOffset) + fh->isymBase + s_idx;
			sh->iss = (long) sh->iss + fh->issBase;
			sh->reserved = 0;
		}

		cur_fd = f_idx;

		/* Local symbols */
		fh->isymBase = (int)((SYMR*)(hdr->cbSymOffset)+fh->isymBase);

		/* cannot fix fh->ipdFirst because it is a short */
#define IPDFIRST(h,fh) \
		((long)h->cbPdOffset + fh->ipdFirst * sizeof(PDR))

		/* Optional symbols (actually used for partial_symtabs) */
		fh->ioptBase = 0;
		fh->copt = 0;

		/* Aux symbols */
		if (fh->caux)
			fh->iauxBase = hdr->cbAuxOffset + fh->iauxBase * sizeof(AUXU);
		/* Relative file descriptor table */
		fh->rfdBase = hdr->cbRfdOffset + fh->rfdBase * sizeof(RFDT);

		/* Line numbers */
		if (fh->cbLine)
			fh->cbLineOffset += hdr->cbLineOffset;

		/* Procedure symbols.  (XXX This should be done later) */
		code_offset = fh->adr;
		for (s_idx = 0; s_idx < fh->cpd; s_idx++) {
			unsigned name, only_ext;

			pr = (PDR*)(IPDFIRST(hdr,fh)) + s_idx;

			/* Simple rule to find files linked "-x" */
			only_ext = fh->rss == -1;
			if (only_ext) {
				if (pr->isym == -1) {
					/* static function */
					sh = (SYMR*)-1;
				} else {
					/* external */
					name = hdr->cbExtOffset + pr->isym * sizeof(EXTR);
					sh = &((EXTR*)name)->asym;
				}
			} else {
				/* Full symbols */
				sh = (SYMR*)fh->isymBase + pr->isym;
				/* Included code ? */
				if (s_idx == 0 && pr->adr != 0)
					code_offset -= pr->adr;
			}

			/* Turn index into a pointer */
			pr->isym = (long)sh;

			/* Fix line numbers */
			pr->cbLineOffset += fh->cbLineOffset;

			/* Relocate address */
			if (!only_ext)
				pr->adr += code_offset;
		}		
	}

	/* External symbols: fix string */
	for (s_idx = 0; s_idx < hdr->iextMax; s_idx++) {
		esh = (EXTR*)(hdr->cbExtOffset) + s_idx;
		esh->asym.iss = esh->asym.iss + hdr->cbSsExtOffset;
	}
}


/* Find a file descriptor given its index RF relative to a file CF */

static
FDR *get_rfd( cf, rf)
{
	register FDR   *f;

	f = (FDR *) (cur_hdr->cbFdOffset) + cf;
	/* Object files do not have the RFD table, all refs are absolute */
	if (f->rfdBase == 0)
		return (FDR *) (cur_hdr->cbFdOffset) + rf;
	cf = *((pRFDT) f->rfdBase + rf);
	return (FDR *) (cur_hdr->cbFdOffset) + cf;
}

/* Return a safer print NAME for a file descriptor */

static
char *fdr_name(name)
	char *name;
{
	if (name == (char *) -1)
		return "<stripped file>";
	if (UNSAFE_DATA_ADDR(name))
		return "<NFY>";
	return name;
}


/* Read in and parse the symtab of the file DESC. INCREMENTAL says
   whether we are adding to the general symtab or not. 
   FIXME:  INCREMENTAL is currently always zero, though it should not be. */

static
read_mips_symtab (abfd, desc, incremental)
	bfd *abfd;
	int desc;
	int incremental;
{
	CORE_ADDR end_of_text_seg;

	read_the_mips_symtab(abfd, desc, &end_of_text_seg);

	parse_partial_symbols(cur_hdr, incremental, end_of_text_seg);
	cur_hdr = 0;

	/*
	 * Check to make sure file was compiled with -g.
	 * If not, warn the user of this limitation.
	 */
	if (compare_glevel(max_glevel, GLEVEL_2) < 0) {
		if (max_gdbinfo == 0)
			printf("\n%s not compiled with -g, debugging support is limited.", symfile);
		printf("\nYou should compile with -g2 or -g3 for best debugging support.\n");
		fflush(stdout);
	}

	/*
	 * Dont allow char * to have a typename (else would get
	 * caddr_t.)  
	 */
	TYPE_NAME(lookup_pointer_type(builtin_type_char)) = 0;
}


/* Local utilities */


/* Map of FDR indexes to partial symtabs */

static struct pst_map {
	struct partial_symtab *pst;	/* the psymtab proper */
	int n_globals;			/* globals it exports */
} * fdr_to_pst;


/* Utility stack, used to nest procedures and blocks properly.
   It is a doubly linked list, to avoid too many alloc/free.
   Since we might need it quite a few times it is NOT deallocated
   after use. */

static struct parse_stack {
	struct parse_stack	*next, *prev;
	struct symtab		*cur_st;	/* Current symtab */
	struct block		*cur_block;	/* Block in it */
	int			 blocktype;	/* What are we parsing */
	int			 maxsyms;	/* Max symbols in this block */
	struct type		*cur_type;	/* Type we parse fields for */
	int			 procadr;	/* Start addres of this procedure */
	int			 numargs;	/* Its argument count */
} *top_stack;	/* Top stack ptr */


/* Enter a new lexical context */

static push_parse_stack()
{
	struct parse_stack *new;

	/* Reuse frames if possible */
	if (top_stack && top_stack->prev)
		new = top_stack->prev;
	else
		new = (struct parse_stack *) xzalloc(sizeof(struct parse_stack));
	/* Initialize new frame with previous content */
	if (top_stack) {
		register struct parse_stack *prev = new->prev;

		*new = *top_stack;
		top_stack->prev = new;
		new->prev = prev;
		new->next = top_stack;
	}
	top_stack = new;
}

/* Exit a lexical context */

static pop_parse_stack()
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

static struct pending {
	struct pending	*next;		/* link */
	SYMR		*s;		/* the symbol */
	struct type	*t;		/* its partial type descriptor */
} **pending_list;


/* Check whether we already saw symbol SH in file FH as undefined */

static
struct pending *is_pending_symbol(fh, sh)
	FDR *fh;
	SYMR *sh;
{
	int             f_idx = fh - (FDR *) cur_hdr->cbFdOffset;
	register struct pending *p;

	/* Linear search is ok, list is typically no more than 10 deep */
	for (p = pending_list[f_idx]; p; p = p->next)
		if (p->s == sh)
			break;
	return p;
}

/* Check whether we already saw type T in file FH as undefined */

static
struct pending *is_pending_type(fh, t)
	FDR *fh;
	struct type *t;
{
	int             f_idx = fh - (FDR *) cur_hdr->cbFdOffset;
	register struct pending *p;

	for (p = pending_list[f_idx]; p; p = p->next)
		if (p->t == t)
			break;
	return p;
}

/* Add a new undef symbol SH of type T */

static
add_pending(fh, sh, t)
	FDR *fh;
	SYMR *sh;
	struct type *t;
{
	int             f_idx = fh - (FDR *) cur_hdr->cbFdOffset;
	struct pending *p = is_pending_symbol(fh, sh);

	/* Make sure we do not make duplicates */
	if (!p) {
		p = (struct pending *) xmalloc(sizeof(*p));
		p->s = sh;
		p->t = t;
		p->next = pending_list[f_idx];
		pending_list[f_idx] = p;
	}
	sh->reserved = 1;	/* for quick check */
}

/* Throw away undef entries when done with file index F_IDX */

static
free_pending(f_idx)
{
	register struct pending *p, *q;

	for (p = pending_list[f_idx]; p; p = q) {
		q = p->next;
		free(p);
	}
	pending_list[f_idx] = 0;
}

/* The number of args to a procedure is not explicit in the symtab,
   this is the list of all those we know of.
   This makes parsing more reasonable and avoids extra passes */

static struct numarg {
	struct numarg	*next;		/* link */
	unsigned	 adr;		/* procedure's start address */
	unsigned	 num;		/* arg count */
} *numargs_list;

/* Record that the procedure at ADR takes NUM arguments. */

static
got_numargs(adr,num)
{
	struct numarg  *n = (struct numarg *) xmalloc(sizeof(struct numarg));

	n->adr = adr;
	n->num = num;
	n->next = numargs_list;
	numargs_list = n;
}

/* See if we know how many arguments the procedure at ADR takes */

static
lookup_numargs(adr)
{
	struct numarg  *n = numargs_list;

	while (n && n->adr != adr)
		n = n->next;
	return (n) ? n->num : -1;
}

/* Release storage when done with this file */

static
free_numargs()
{
	struct numarg  *n = numargs_list, *m;

	while (n) {
		m = n->next;
		free(n);
		n = m;
	}
	numargs_list = 0;
}


/* Parsing Routines proper. */

/* Parse a single symbol. Mostly just make up a GDB symbol for it.
   For blocks, procedures and types we open a new lexical context.
   This is basically just a big switch on the symbol's type */

static
parse_symbol(sh, ax)
	SYMR *sh;
	AUXU *ax;
{
	struct symbol  *s;
	struct block   *b;
	struct type    *t;
	struct field   *f;
	/* When a symbol is cross-referenced from other files/symbols
	   we mark it explicitly */
	int             pend = (sh->reserved == 1);
	enum address_class class;

	switch (sh->st) {

	    case stNil:
		break;

	    case stGlobal:	/* external symbol, goes into the primary block */
		class = LOC_STATIC;
		b = BLOCKVECTOR_BLOCK(BLOCKVECTOR(top_stack->cur_st),
				      GLOBAL_BLOCK);
		goto data;

	    case stStatic:	/* static data, goes into the current block. */
		class = LOC_STATIC;
		b = top_stack->cur_block;
		goto data;

	    case stLocal:	/* local variable, goes into the current block */
		if (sh->sc == scRegister) {
			class = LOC_REGISTER;
			if (sh->value > 31)
				sh->value += 6;
		} else
			class = LOC_LOCAL;
		b = top_stack->cur_block;

data:		/* Common code for symbols describing data */
		s = new_symbol(sh->iss);
		SYMBOL_NAMESPACE(s) = VAR_NAMESPACE;
		SYMBOL_CLASS(s) = class;
		add_symbol(s, b);

		/* Type could be missing in a number of cases */
		if (sh->sc == scUndefined || sh->sc == scNil ||
		    sh->index == 0xfffff)
			SYMBOL_TYPE(s) = builtin_type_int;	/* undefined? */
		else
			SYMBOL_TYPE(s) = parse_type(ax + sh->index, sh, 0);
		/* Value of a data symbol is its memory address */
		SYMBOL_VALUE(s) = sh->value;
		break;

	    case stParam:	/* argument to procedure, goes into current block */
		max_gdbinfo++;
		top_stack->numargs++;
		s = new_symbol(sh->iss);
		SYMBOL_NAMESPACE(s) = VAR_NAMESPACE;
		if (sh->sc == scRegister) {
			SYMBOL_CLASS(s) = LOC_REGPARM;
			if (sh->value > 31)
				sh->value += 6;
		} else
			SYMBOL_CLASS(s) = LOC_ARG;
		SYMBOL_VALUE(s) = sh->value;
		SYMBOL_TYPE(s) = parse_type(ax + sh->index, sh, 0);
		add_symbol(s, top_stack->cur_block);
		break;

	    case stLabel:	/* label, we do make a symbol for it */
		s = new_symbol(sh->iss);
		SYMBOL_NAMESPACE(s) = VAR_NAMESPACE;	/* so that it can be used */
		SYMBOL_CLASS(s) = LOC_LABEL;		/* but not misused */
		SYMBOL_VALUE(s) = sh->value;
		SYMBOL_TYPE(s) = builtin_type_int;
		add_symbol(s, top_stack->cur_block);
		break;

	    case stProc:	/* Procedure */
	    case stStaticProc:	/* Static procedure */
		s = new_symbol(sh->iss);
		SYMBOL_NAMESPACE(s) = VAR_NAMESPACE;
		SYMBOL_CLASS(s) = LOC_BLOCK;
		/* Type of the return value */
		if (sh->sc == scUndefined || sh->sc == scNil)
			t = builtin_type_int;
		else
			t = parse_type(ax + sh->index, sh, 0);
		add_symbol(s, top_stack->cur_block);

		/* Make a type for the procedure itself */
		SYMBOL_TYPE(s) = lookup_function_type (t);

		/* Create and enter a new lexical context */
		b = new_block(top_stack->maxsyms);
		SYMBOL_BLOCK_VALUE(s) = b;
		BLOCK_FUNCTION(b) = s;
		BLOCK_START(b) = BLOCK_END(b) = sh->value;
		BLOCK_SUPERBLOCK(b) = top_stack->cur_block;
		add_block(b, top_stack->cur_st);

		/* Not if we only have partial info */
		if (sh->sc == scUndefined || sh->sc == scNil)
			break;

		push_parse_stack();
		top_stack->cur_block = b;
		top_stack->blocktype = sh->st;
		top_stack->cur_type = SYMBOL_TYPE(s);
		top_stack->procadr = sh->value;
		top_stack->numargs = 0;

		sh->value = (long) SYMBOL_TYPE(s);
		break;

	    case stBlock:	/* Either a lexical block, or some type */
		push_parse_stack();
		top_stack->blocktype = stBlock;
		if (sh->sc == scInfo) {	/* structure/union/enum def */
			s = new_symbol(sh->iss);
			SYMBOL_NAMESPACE(s) = STRUCT_NAMESPACE;
			SYMBOL_CLASS(s) = LOC_TYPEDEF;
			SYMBOL_VALUE(s) = 0;
			add_symbol(s, top_stack->cur_block);
			/* If this type was expected, use its partial definition */
			if (pend) {
				t = is_pending_symbol(cur_fdr, sh)->t;
			} else {
				/* Uhmm, can`t decide yet. Smash later */
				t = new_type(sh->iss);
				TYPE_CODE(t) = TYPE_CODE_UNDEF;
				add_pending(cur_fdr, sh, t);
			}
			SYMBOL_TYPE(s) = t;
			/* make this the current type */
			top_stack->cur_type = t;
			TYPE_LENGTH(t) = sh->value;
			/* Mark that symbol has a type, and say which one */
			sh->value = (long) t;
		} else {
			/* beginnning of (code) block. Value of symbol
			   is the displacement from procedure start */
			b = new_block(top_stack->maxsyms);
			BLOCK_START(b) = sh->value + top_stack->procadr;
			BLOCK_SUPERBLOCK(b) = top_stack->cur_block;
			top_stack->cur_block = b;
			add_block(b, top_stack->cur_st);
		}
		break;

	    case stEnd:		/* end (of anything) */
		if (sh->sc == scInfo) {
			/* Finished with type */
			top_stack->cur_type = 0;
		} else if (sh->sc == scText &&
			   (top_stack->blocktype == stProc ||
			    top_stack->blocktype == stStaticProc)) {
		    /* Finished with procedure */
		    struct blockvector *bv = BLOCKVECTOR(top_stack->cur_st);
		    struct block *b;
		    int i;

		    BLOCK_END(top_stack->cur_block) += sh->value; /* size */
		    got_numargs(top_stack->procadr, top_stack->numargs);
		    /* Reallocate symbols, saving memory */
		    b = shrink_block(top_stack->cur_block, top_stack->cur_st);

		    /* f77 emits proc-level with address bounds==[0,0],
		       So look for such child blocks, and patch them.  */
		    for (i = 0; i < BLOCKVECTOR_NBLOCKS(bv); i++) {
			struct block *b_bad = BLOCKVECTOR_BLOCK(bv,i);
			if (BLOCK_SUPERBLOCK(b_bad) == b
			 && BLOCK_START(b_bad) == top_stack->procadr
			 && BLOCK_END(b_bad) == top_stack->procadr) {
			    BLOCK_START(b_bad) = BLOCK_START(b);
			    BLOCK_END(b_bad) = BLOCK_END(b);
			}
		    }
		    if (entry_point < BLOCK_END(b)
		      && entry_point >= BLOCK_START(b)) {
			startup_file_start = BLOCK_START(b);
			startup_file_end = BLOCK_END(b);
		    }
		} else if (sh->sc == scText && top_stack->blocktype == stBlock) {
			/* End of (code) block. The value of the symbol
			   is the displacement from the procedure`s start
			   address of the end of this block. */
			BLOCK_END(top_stack->cur_block) = sh->value + top_stack->procadr;
			(void) shrink_block(top_stack->cur_block, top_stack->cur_st);
		}
		pop_parse_stack();	/* restore previous lexical context */
		break;

	    case stMember:	/* member of struct/union/enum.. */
		f = new_field(top_stack->cur_type, sh->iss);
		f->bitpos = sh->value;
		f->type = parse_type(ax + sh->index, sh, &f->bitsize);
		break;

	    case stTypedef:	/* type definition */
		s = new_symbol(sh->iss);
		SYMBOL_NAMESPACE(s) = VAR_NAMESPACE;
		SYMBOL_CLASS(s) = LOC_TYPEDEF;
		SYMBOL_BLOCK_VALUE(s) = top_stack->cur_block;
		add_symbol(s, top_stack->cur_block);			
		SYMBOL_TYPE(s) = parse_type(ax + sh->index, sh, 0);
		sh->value = (long) SYMBOL_TYPE(s);
		break;

	    case stFile:	/* file name */
		push_parse_stack();
		top_stack->blocktype = sh->st;
		break;

		/* I`ve never seen these for C */
	    case stRegReloc:
		break;		/* register relocation */
	    case stForward:
		break;		/* forwarding address */
	    case stConstant:
		break;		/* constant */
	    default:
		error("Unknown symbol type %x.", sh->st);
	}
	sh->st = stParsed;
}

/* Parse the type information provided in the AX entries for
   the symbol SH. Return the bitfield size in BS, in case. */

static struct type *parse_type(ax, sh, bs)
	AUXU	*ax;
	SYMR	*sh;
	int	*bs;
{
	/* Null entries in this map are treated specially */
	static struct type **map_bt[] =
	{
		 &builtin_type_void,		/* btNil */
		 0,				/* btAdr */
		 &builtin_type_char,		/* btChar */
		 &builtin_type_unsigned_char,	/* btUChar */
		 &builtin_type_short,		/* btShort */
		 &builtin_type_unsigned_short,	/* btUShort */
		 &builtin_type_int,		/* btInt */
		 &builtin_type_unsigned_int,	/* btUInt */
		 &builtin_type_long,		/* btLong */
		 &builtin_type_unsigned_long,	/* btULong */
		 &builtin_type_float,		/* btFloat */
		 &builtin_type_double,		/* btDouble */
		 0,				/* btStruct */
		 0,				/* btUnion */
		 0,				/* btEnum */
		 0,				/* btTypedef */
		 0,				/* btRange */
		 0,				/* btSet */
		 &builtin_type_complex,		/* btComplex */
		 &builtin_type_double_complex,	/* btDComplex */
		 0,				/* btIndirect */
		 &builtin_type_fixed_dec,	/* btFixedDec */
		 &builtin_type_float_dec,	/* btFloatDec */
		 &builtin_type_string,		/* btString */
		 0,				/* btBit */
		 0,				/* btPicture */
		 &builtin_type_void,		/* btVoid */
	};

	TIR            *t;
	struct type    *tp = 0, *tp1;
	char           *fmt = "%s";

	/* Procedures start off by one */
	if (sh->st == stProc || sh->st == stStaticProc)
		ax++;

	/* Undefined ? Should not happen */
	if (ax->rndx.rfd == 0xfff) {
		return builtin_type_void;
	}

	/* Use aux as a type information record, map its basic type */
	t = &ax->ti;
	if (t->bt > 26 || t->bt == btPicture) {
		printf_filtered("Internal: cannot map MIPS basic type x%x\n", t->bt);
		return builtin_type_int;
	}
	if (map_bt[t->bt])
		tp = *map_bt[t->bt];
	else {
		/* Cannot use builtin types, use templates */
		tp = make_type(TYPE_CODE_VOID, 0, 0, 0);
		switch (t->bt) {
		    case btAdr:
			*tp = *builtin_type_ptr;
			break;
		    case btStruct:
			*tp = *builtin_type_struct;
			fmt = "struct %s";
			break;
		    case btUnion:
			*tp = *builtin_type_union;
			fmt = "union %s";
			break;
		    case btEnum:
			*tp = *builtin_type_enum;
			fmt = "enum %s";
			break;
		    case btRange:
			*tp = *builtin_type_range;
			break;
		    case btSet:
			*tp = *builtin_type_set;
			fmt = "set %s";
			break;
		}
	}

	/* Move on to next aux */
	ax++;
	if (t->continued) {
		/* This is the way it would work if the compiler worked */
		register TIR *t1 = t;
		while (t1->continued)
			ax++;
	}

	/* For bitfields all we need is the width */
	if (t->fBitfield) {
		*bs = ax->width;
		return tp;
	}

	/* All these types really point to some (common) MIPS type
	   definition, and only the type-qualifiers fully identify
	   them.  We`ll make the same effort at sharing */
	if (t->bt == btIndirect ||
	    t->bt == btStruct ||
	    t->bt == btUnion ||
	    t->bt == btEnum ||
	    t->bt == btTypedef ||
	    t->bt == btRange ||
	    t->bt == btSet) {
		char            name[256], *pn;

		/* Try to cross reference this type */
		tp1 = tp;
		ax += cross_ref(ax, &tp1, &pn);
		/* SOMEONE OUGHT TO FIX DBXREAD TO DROP "STRUCT" */
		sprintf(name, fmt, pn);

		/* reading .o file ? */
		if (UNSAFE_DATA_ADDR(tp1))
			tp1 = tp;
		if (TYPE_CODE(tp1) == TYPE_CODE_UNDEF) {
			/*
			 * Type was incompletely defined, now we know.
			 */
			TYPE_CODE(tp1) = TYPE_CODE(tp);
			TYPE_NAME(tp1) = obsavestring(name, strlen(name));
			if (TYPE_CODE(tp1) == TYPE_CODE_ENUM) {
				int             i;

				for (i = 0; i < TYPE_NFIELDS(tp1); i++)
					make_enum_constant(&TYPE_FIELD(tp1,i), tp1);
			}
		}
		if (tp1 != tp) {
			/* found as cross ref, rid of our template */
			if ((TYPE_FLAGS(tp) & TYPE_FLAG_PERM) == 0)
				free(tp);
			tp = tp1;
			/* stupid idea of prepending "struct" to type names */
			if (t->bt == btStruct && !index(TYPE_NAME(tp), ' ')) {
				sprintf(name, fmt, TYPE_NAME(tp));
				TYPE_NAME(tp) = obsavestring(name, strlen(name));
			}
		} else
			TYPE_NAME(tp) = savestring(name, strlen(name));
	}

	/* Deal with range types */
	if (t->bt == btRange) {
		struct field   *f;

		f = new_field(tp, "Low");
		f->bitpos = ax->dnLow;
		ax++;
		f = new_field(tp, "High");
		f->bitpos = ax->dnHigh;
		ax++;
	}

	/* Parse all the type qualifiers now. If there are more
	   than 6 the game will continue in the next aux */

#define PARSE_TQ(tq) \
	if (t->tq != tqNil) ax += upgrade_type(&tp, t->tq, ax, sh);

again:	PARSE_TQ(tq0);
	PARSE_TQ(tq1);
	PARSE_TQ(tq2);
	PARSE_TQ(tq3);
	PARSE_TQ(tq4);
	PARSE_TQ(tq5);
#undef	PARSE_TQ

	if (t->continued) {
		t++;
		goto again;
	}
	return tp;
}

/* Make up a complex type from a basic one.  Type is passed by
   reference in TPP and side-effected as necessary. The type
   qualifier TQ says how to handle the aux symbols at AX for
   the symbol SX we are currently analyzing.
   Returns the number of aux symbols we parsed. */

static
upgrade_type(tpp, tq, ax, sh)
	struct type   **tpp;
	AUXU	       *ax;
	SYMR	       *sh;
{
	int             off = 0;
	int             ret = 0;
	struct type    *t;

	if (tq == tqPtr) {
		t = lookup_pointer_type (*tpp);
	} else if (tq == tqProc) {
		t = lookup_function_type (*tpp);
	} else if (tq == tqArray) {
		int             rf, id;
		FDR            *fh;
		struct field   *f;
		SYMR            ss;

		t = make_type(TYPE_CODE_ARRAY, 0, 0, 0);
		TYPE_TARGET_TYPE(t) = *tpp;

		/* Pointer to domain type (type of index) */
		id = ax->rndx.index;
		if ((rf = ax->rndx.rfd) == 0xfff)
			rf = (++ax)->isym, off++;

		fh = get_rfd(cur_fd, rf);
		f = new_field(t, 0);
		bzero(&ss, sizeof ss);
/* XXX */	f->type = parse_type(fh->iauxBase + id * sizeof(AUXU),
				     &ss, &f->bitsize);

		/*
		 * This seems to be a pointer to the end of the Block defining
		 * the type.  Why it is here is magic for me, and I have no
		 * good use for it anyways.
		 */
		if (off == 0) {
			off++;
			id = (++ax)->rndx.index;
			if ((rf = ax->rndx.rfd) == 0xfff)
				rf = (++ax)->isym, off++;
		}
		f->bitpos = (++ax)->dnLow;	/* ?? */
		f->bitsize = (++ax)->dnHigh;	/* ?? */
		rf = (++ax)->width - 1;	/* bit alignment */
		id = TYPE_LENGTH(TYPE_TARGET_TYPE(t)) << 3; /* bitsize */

		if (id == 0) {
			/* Most likely an undefined type */
			id = rf + 1;
			TYPE_LENGTH(TYPE_TARGET_TYPE(t)) = id >> 3;
		}
		TYPE_LENGTH(t) = (f->bitsize < 0) ? 0 :
			(f->bitsize - f->bitpos + 1) * (id >> 3);
		ret = 4 + off;
	} else {
		if (tq != tqVol)
			printf_filtered("Internal: unknown type qualifier %x\n", tq);
		return ret;
	}

	*tpp = t;
	return ret;
}


/* Parse a procedure descriptor record PR.  Note that the procedure
   is parsed _after_ the local symbols, now we just make up the
   extra information we need into a special symbol that we insert
   in the procedure's main block.  Note also that images that
   have been partially stripped (ld -x) have been deprived
   of local symbols, and we have to cope with them here.
   The procedure's code ends at BOUND */

static
parse_procedure(pr, bound)
	PDR *pr;
{
	struct symbol *s, *i;
	SYMR *sh = (SYMR*)pr->isym;
	struct block *b;
	struct mips_extra_func_info *e;
	char name[100];
	char *sh_name;

	/* Reuse the MIPS record */
	e = (struct mips_extra_func_info *) pr;
	e->numargs = lookup_numargs(pr->adr);

	/* Make up our special symbol */
	i = new_symbol(".gdbinfo.");
	SYMBOL_VALUE(i) = (int)e;
	SYMBOL_NAMESPACE(i) = LABEL_NAMESPACE;
	SYMBOL_CLASS(i) = LOC_CONST;
	SYMBOL_TYPE(i) = builtin_type_void;

	/* Make up a name for static procedures. Sigh. */
	if (sh == (SYMR*)-1) {
	    sprintf(name,".static_procedure@%x",pr->adr);
	    sh_name = savestring(name, strlen(name));
	    s = NULL;
	}
	else {
	    sh_name = (char*)sh->iss;
	    s = mylookup_symbol(sh_name, top_stack->cur_block,
				VAR_NAMESPACE, LOC_BLOCK);
	}
	if (s != 0) {
		b = SYMBOL_BLOCK_VALUE(s);
	} else {
		s = new_symbol(sh_name);
		SYMBOL_NAMESPACE(s) = VAR_NAMESPACE;
		SYMBOL_CLASS(s) = LOC_BLOCK;
		/* Donno its type, hope int is ok */
		SYMBOL_TYPE(s) = lookup_function_type (builtin_type_int);
		add_symbol(s, top_stack->cur_block);
		/* Wont have symbols for this one */
		b = new_block(2);
		SYMBOL_BLOCK_VALUE(s) = b;
		BLOCK_FUNCTION(b) = s;
		BLOCK_START(b) = pr->adr;
		BLOCK_END(b) = bound;
		BLOCK_SUPERBLOCK(b) = top_stack->cur_block;
		add_block(b, top_stack->cur_st);
	}
	e->isym = (long)s;
	add_symbol(i,b);
}

/* Parse the external symbol ES. Just call parse_symbol() after
   making sure we know where the aux are for it. For procedures,
   parsing of the PDRs has already provided all the needed
   information, we only parse them if SKIP_PROCEDURES is false,
   and only if this causes no symbol duplication */

static
parse_external(es, skip_procedures)
	EXTR *es;
{
	AUXU *ax;

	if (es->ifd != ifdNil) {
		cur_fd = es->ifd;
		cur_fdr = (FDR*)(cur_hdr->cbFdOffset) + cur_fd;
		ax = (AUXU*)cur_fdr->iauxBase;
	} else {
		cur_fdr = (FDR*)(cur_hdr->cbFdOffset);
		ax = 0;
	}
	top_stack->cur_st = cur_stab;
	top_stack->cur_block = BLOCKVECTOR_BLOCK(BLOCKVECTOR(top_stack->cur_st),
						 GLOBAL_BLOCK);

	/* Reading .o files */
	if (es->asym.sc == scUndefined || es->asym.sc == scNil) {
		char *what;
		switch (es->asym.st) {
		case stStaticProc:
		case stProc:	what = "Procedure"; n_undef_procs++;  break;
		case stGlobal:	what = "Variable";  n_undef_vars++;   break;
		case stLabel:	what = "Label";     n_undef_labels++; break;
		default :	what = "Symbol";                      break;
		}
		n_undef_symbols++;
		if (info_verbose)
			printf_filtered("Warning: %s %s is undefined (in %s)\n", what,
				es->asym.iss, fdr_name(cur_fdr->rss));
		return;
	}

	switch (es->asym.st) {
	case stProc:
		/* If we have full symbols we do not need more */
		if (skip_procedures)
			return;
		if (mylookup_symbol (es->asym.iss, top_stack->cur_block,
					VAR_NAMESPACE, LOC_BLOCK))
			break;
		/* fall through */
	case stGlobal:
	case stLabel:
		/*
		 * Note that the case of a symbol with indexNil
		 * must be handled anyways by parse_symbol().
		 */
		parse_symbol(&es->asym, ax);
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

static
parse_lines(fh, lt)
	FDR *fh;
	struct linetable *lt;
{
	char *base = (char*)fh->cbLineOffset;
	int i, j, k;
	int delta, count, lineno = 0;
	PDR *pr;

	if (base == 0)
		return;

	/* Scan by procedure descriptors */
	i = 0; j = 0, k = 0;
	for (pr = (PDR*)IPDFIRST(cur_hdr,fh); j < fh->cpd; j++, pr++) {
		int l, halt;

		/* No code for this one */
		if (pr->iline == ilineNil ||
		    pr->lnLow == -1 || pr->lnHigh == -1)
			continue;
		/*
		 *	Aurgh! To know where to stop expanding we
		 *	must look-ahead.
		 */
		for (l = 1; l < (fh->cpd - j); l++)
			if (pr[l].iline != -1)
				break;
		if (l == (fh->cpd - j))
			halt = fh->cline;
		else
			halt = pr[l].iline;
		/*
		 * When procedures are moved around the linenumbers
		 * are attributed to the next procedure up
		 */
		if (pr->iline >= halt) continue;

		base = (char*)pr->cbLineOffset;
		l = pr->adr >> 2;	/* in words */
		halt += (pr->adr >> 2) - pr->iline;
		for (lineno = pr->lnLow; l < halt;) {
			count = *base & 0x0f;
			delta = *base++ >> 4;
			if (delta == -8) {
				delta = (base[0] << 8) | (base[1] & 0xff);
				base += 2;
			}
			lineno += delta;/* first delta is 0 */
			k = add_line(lt, lineno, l, k);
			l += count + 1;
		}
	}
}


/* Parse the symbols of the file described by FH, whose index is F_IDX.
   BOUND is the highest core address of this file's procedures */

static
parse_one_file(fh, f_idx, bound)
	FDR *fh;
{
	register int s_idx;
	SYMR *sh;
	PDR *pr;

	/* Parse local symbols first */

	for (s_idx = 0; s_idx < fh->csym; s_idx++) {
		sh = (SYMR *) (fh->isymBase) + s_idx;
		cur_sdx = s_idx;
		parse_symbol(sh, fh->iauxBase);
	}

	/* Procedures next, note we need to look-ahead to
	   find out where the procedure's code ends */

	for (s_idx = 0; s_idx < fh->cpd-1; s_idx++) {
		pr = (PDR *) (IPDFIRST(cur_hdr, fh)) + s_idx;
		parse_procedure(pr, pr[1].adr);	/* next proc up */
	}
	if (fh->cpd) {
		pr = (PDR *) (IPDFIRST(cur_hdr, fh)) + s_idx;
		parse_procedure(pr, bound);	/* next file up */
	}

	/* Linenumbers. At the end, check if we can save memory */
	parse_lines(fh, LINETABLE(cur_stab));
	if (LINETABLE(cur_stab)->nitems < fh->cline)
		shrink_linetable(cur_stab);
}


/* Master parsing procedure. Parses the symtab described by the
   symbolic header HDR.  If INCREMENTAL is true we are called
   by add-file and must preserve the old symtabs.  END_OF_TEXT_SEG
   gives the address just after the text segment for the symtab
   we are reading.  */
static
parse_partial_symbols(hdr, incremental, end_of_text_seg)
	HDRR *hdr;
{
	int             f_idx, s_idx, h_max;
	CORE_ADDR	dummy, *prevhigh;
	/* Running pointers */
	FDR            *fh;
	RFDT	       *rh;
	register EXTR           *esh;

	/*
	 * Big plan: 
	 *
	 * Only parse the External symbols, and the Relative FDR.
	 * Fixup enough of the loader symtab to be able to use it.
	 * Allocate space only for the file`s portions we need to
	 * look at. (XXX)
	 */

	cur_hdr = hdr;
	max_gdbinfo = 0;
	max_glevel = MIN_GLEVEL;

	/* Allocate the map FDR -> PST.
	   Minor hack: -O3 images might claim some global data belongs
	   to FDR -1. We`ll go along with that */
	fdr_to_pst = (struct pst_map *)xzalloc((hdr->ifdMax+1) * sizeof *fdr_to_pst);
	fdr_to_pst++;
	{
		struct partial_symtab * pst = new_psymtab("");
		fdr_to_pst[-1].pst = pst;
		pst->ldsymoff = -1;
	}

	/* Now scan the FDRs, mostly for dependencies */
	for (f_idx = 0; f_idx < hdr->ifdMax; f_idx++)
		(void) parse_fdr(f_idx, 1);

	/* Take a good guess at how many symbols we might ever need */
	h_max = hdr->iextMax;

	/* Parse externals: two passes because they can be ordered
	   in any way */

	/* Pass 1: Presize and partition the list */
	for (s_idx = 0; s_idx < hdr->iextMax; s_idx++) {
		esh = (EXTR *) (hdr->cbExtOffset) + s_idx;
		fdr_to_pst[esh->ifd].n_globals++;
	}

	if (global_psymbols.list) {
		global_psymbols.list = (struct partial_symbol *)
			xrealloc( global_psymbols.list, (h_max +
				global_psymbols.size) * sizeof(struct partial_symbol));
		global_psymbols.next = global_psymbols.list + global_psymbols.size;
		global_psymbols.size += h_max;
	} else {
		global_psymbols.list = (struct partial_symbol *)
				xmalloc( h_max * sizeof(struct partial_symbol));
		global_psymbols.size = h_max;
		global_psymbols.next = global_psymbols.list;
	}

	s_idx = global_psymbols.next - global_psymbols.list;
	for (f_idx = -1; f_idx < hdr->ifdMax; f_idx++) {
		fdr_to_pst[f_idx].pst->globals_offset = s_idx;
		s_idx += fdr_to_pst[f_idx].n_globals;
	}

	/* Pass 2: fill in symbols */
	for (s_idx = 0; s_idx < hdr->iextMax; s_idx++) {
		register struct partial_symbol *p;
		enum misc_function_type misc_type = mf_text;
		esh = (EXTR *) (hdr->cbExtOffset) + s_idx;

		if (esh->asym.sc == scUndefined || esh->asym.sc == scNil)
			continue;
		p = new_psymbol(&global_psymbols, esh->asym.iss, esh->ifd);
		SYMBOL_VALUE(p) = esh->asym.value;
		SYMBOL_NAMESPACE(p) = VAR_NAMESPACE;

		switch (esh->asym.st) {
		case stProc:
			SYMBOL_CLASS(p) = LOC_BLOCK;
			break;
		case stGlobal:
			SYMBOL_CLASS(p) = LOC_STATIC;
			misc_type = mf_data;
			break;
		case stLabel:
			SYMBOL_CLASS(p) = LOC_LABEL;
			break;
		default:
			misc_type = mf_unknown;
			complain (&unknown_ext_complaint, SYMBOL_NAME(p));
		}
		prim_record_misc_function (SYMBOL_NAME(p),
				           SYMBOL_VALUE(p),
				           misc_type);
	}


	/* The array (of lists) of globals must be sorted.
	   Take care, since we are at it, of pst->texthigh.

	   NOTE: The way we handle textlow/high is incorrect, but good
	   enough for a first approximation. The case we fail is on a
	   file "foo.c" that looks like
	   	proc1() {...}
		#include "bar.c"	-- this contains proc2()
		proc3() {...}
	   where proc3() is attributed to bar.c.  But since this is a
	   dependent file it will cause loading of foo.c as well, so
	   everything will be fine at the end.  */

	prevhigh = &dummy;
	for (f_idx = 0; f_idx < hdr->ifdMax; f_idx++) {
		struct partial_symtab *pst = fdr_to_pst[f_idx].pst;
		if (pst->n_global_syms > 1)
			qsort (global_psymbols.list + pst->globals_offset,
				pst->n_global_syms, sizeof (struct partial_symbol),
				compare_psymbols);
		if (pst->textlow) {
			*prevhigh = pst->textlow;
			prevhigh = &pst->texthigh;
		}
	}

	/* Mark the last code address, and remember it for later */
	*prevhigh = end_of_text_seg;
	hdr->cbDnOffset = end_of_text_seg;

	reorder_psymtabs();
	free(&fdr_to_pst[-1]);
	fdr_to_pst = 0;
}


/* Do the initial analisys of the F_IDX-th file descriptor.
   Allocates a partial symtab for it, and builds the list
   of dependent files by recursion. LEV says at which level
   of recursion we are called (to pretty up debug traces) */

static struct partial_symtab *
parse_fdr(f_idx, lev)
	int f_idx;
{
	register FDR *fh;
	register struct partial_symtab *pst;
	int s_idx, s_id0;

	fh = (FDR *) (cur_hdr->cbFdOffset) + f_idx;

	/* Use this to indicate into which symtab this file was parsed */
	if (fh->ioptBase)
		return (struct partial_symtab *) fh->ioptBase;

	/* Debuggability level */
	if (compare_glevel(max_glevel, fh->glevel) < 0)
		max_glevel = fh->glevel;

	/* Make a new partial_symtab */
	pst = new_psymtab(fh->rss);
	if (fh->cpd == 0){
		pst->textlow = 0;
		pst->texthigh = 0;
	} else {
		pst->textlow = fh->adr;
		pst->texthigh = fh->cpd;	/* To be fixed later */
	}
	/* Reverse mapping PST -> FDR */
	pst->ldsymoff = f_idx;

	fdr_to_pst[f_idx].pst = pst;
	fh->ioptBase = (int)pst;

	/* Analyze its dependencies */
	if (fh->crfd <= 1)
		return pst;

	s_id0 = 0;
	if (fh->cpd == 0) {  /* If there are no functions defined here ... */
		/* ...then presumably a .h file: drop reverse depends .h->.c */
		for (; s_id0 < fh->crfd; s_id0++) {
			RFDT *rh = (RFDT *) (fh->rfdBase) + s_id0;
			if (*rh == f_idx) {
				s_id0++;	/* Skip self-dependency */
				break;
			}
		}
	}
	pst->number_of_dependencies = fh->crfd - s_id0;
	pst->dependencies = (struct partial_symtab **)
		obstack_alloc (psymbol_obstack,
		               pst->number_of_dependencies * sizeof(char*));
	for (s_idx = s_id0; s_idx < fh->crfd; s_idx++) {
		RFDT *rh = (RFDT *) (fh->rfdBase) + s_idx;

		pst->dependencies[s_idx-s_id0] = parse_fdr(*rh, lev+1);

	}

	return pst;
}


/* Ancillary function to psymtab_to_symtab().  Does all the work
   for turning the partial symtab PST into a symtab, recurring
   first on all dependent psymtabs */

static void
psymtab_to_symtab_1(pst)
	struct partial_symtab *pst;
{
	int             i, f_max;
	struct symtab  *st;
	FDR *fh;

	if (pst->readin)
		return;
	pst->readin = 1;

	pending_list = (struct pending **) cur_hdr->cbOptOffset;
	if (pending_list == 0) {
		pending_list = (struct pending **)
			xzalloc(cur_hdr->ifdMax * sizeof(struct pending *));
		cur_hdr->cbOptOffset = (int)pending_list;
	}

	/* How many symbols will we need */
	f_max = pst->n_global_syms + pst->n_static_syms;
	if (pst->ldsymoff == -1) {
		fh = 0;
		st = new_symtab( "unknown", f_max, 0);
	} else {
		fh = (FDR *) (cur_hdr->cbFdOffset) + pst->ldsymoff;
		f_max += fh->csym + fh->cpd;
		st = new_symtab(pst->filename, 2 * f_max, 2 * fh->cline);
	}

	/*
	 * Read in all partial symbtabs on which this one is dependent.
	 * NOTE that we do have circular dependencies, sigh. 
	 */
	for (i = 0; i < pst->number_of_dependencies; i++)
		if (!pst->dependencies[i]->readin) {
			/*
			 * DO NOT inform about additional files that need to
			 * be read in, it would only annoy the user.
			 */
			psymtab_to_symtab_1(pst->dependencies[i]);
		}

	/* Now read the symbols for this symtab */

	cur_fd = pst->ldsymoff;
	cur_fdr = fh;
	cur_stab = st;

	/* Get a new lexical context */

	push_parse_stack();
	top_stack->cur_st = cur_stab;
	top_stack->cur_block = BLOCKVECTOR_BLOCK(BLOCKVECTOR(cur_stab),
						 GLOBAL_BLOCK);
	BLOCK_START(top_stack->cur_block) = fh ? fh->adr : 0;
	BLOCK_END(top_stack->cur_block) = 0;
	top_stack->blocktype = stFile;
	top_stack->maxsyms = f_max;
	top_stack->cur_type = 0;
	top_stack->procadr = 0;
	top_stack->numargs = 0;

	/* Parse locals and procedures */
	if (fh)
		parse_one_file(fh, cur_fd, (cur_fd == (cur_hdr->ifdMax - 1)) ?
				cur_hdr->cbDnOffset : fh[1].adr);

	/* .. and our share of externals.
	   XXX use the global list to speed up things here. how ? */
	top_stack->blocktype = stFile;
	top_stack->maxsyms = cur_hdr->isymMax + cur_hdr->ipdMax + cur_hdr->iextMax;
	for (i = 0; i < cur_hdr->iextMax; i++) {
		register EXTR *esh = (EXTR *) (cur_hdr->cbExtOffset) + i;
		if (esh->ifd == cur_fd)
			parse_external(esh, 1);
	}

	/* If there are undefined, tell the user */
	if (n_undef_symbols) {
		printf_filtered("File %s contains %d unresolved references:",
				st->filename, n_undef_symbols);
		printf_filtered("\n\t%4d variables\n\t%4d procedures\n\t%4d labels\n",
				n_undef_vars, n_undef_procs, n_undef_labels);
		n_undef_symbols = n_undef_labels = n_undef_vars = n_undef_procs = 0;
	}

	pop_parse_stack();

	/*
	 * Sort the symbol table now, we are done adding symbols to it.
	 */
	sort_symtab_syms(st);
}




/* Ancillary parsing procedures. */

/* Lookup the type at relative index RN.  Return it in TPP
   if found and in any event come up with its name PNAME.
   Return value says how many aux symbols we ate */

static
cross_ref(rn, tpp, pname)
	RNDXR *rn;
	struct type **tpp;
	char **pname;
{
	unsigned        rf;

	/* Escape index means 'the next one' */
	if (rn->rfd == 0xfff)
		rf = *(unsigned *) (rn + 1);
	else
		rf = rn->rfd;

	if (rf == -1) {
		/* Ooops */
		*pname = "<undefined>";
	} else {
		/*
		 * Find the relative file descriptor and the symbol in it 
		 */
		FDR            *fh = get_rfd(cur_fd, rf);
		SYMR           *sh;
		struct type    *t;

		/*
		 * If we have processed this symbol then we left a forwarding
		 * pointer to the corresponding GDB symbol.  If not, we`ll put
		 * it in a list of pending symbols, to be processed later when
		 * the file f will be.  In any event, we collect the name for
		 * the type here. Which is why we made a first pass at
		 * strings. 
		 */
		sh = (SYMR *) (fh->isymBase) + rn->index;

		/* Careful, we might be looking at .o files */
		*pname = (UNSAFE_DATA_ADDR(sh->iss)) ? "<undefined>" :
			(char *) sh->iss;

		/* Have we parsed it ? */
		if ((!UNSAFE_DATA_ADDR(sh->value)) && (sh->st == stParsed)) {
			t = (struct type *) sh->value;
			*tpp = t;
		} else {
			struct pending *p;

			/* Avoid duplicates */
			p = is_pending_symbol(fh, sh);

			if (p)
				*tpp = p->t;
			else
				add_pending(fh, sh, *tpp);
		}
	}
	return (rn->rfd == 0xfff);
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
	register int    bot, top, inc;
	register struct symbol *sym;

	bot = 0;
	top = BLOCK_NSYMS(block);
	inc = name[0];
	while (bot < top) {
		sym = BLOCK_SYM(block, bot);
		if (SYMBOL_NAME(sym)[0] == inc
		    && SYMBOL_NAMESPACE(sym) == namespace
		    && SYMBOL_CLASS(sym) == class
		    && !strcmp(SYMBOL_NAME(sym), name))
			return sym;
		bot++;
	}
	if (block = BLOCK_SUPERBLOCK (block))
		return mylookup_symbol (name, block, namespace, class);
	return 0;
}


/* Add a new symbol S to a block B */

static
add_symbol(s,b)
	struct symbol *s;
	struct block *b;
{
	BLOCK_SYM(b,BLOCK_NSYMS(b)++) = s;
	if (b == top_stack->cur_block &&
	    BLOCK_NSYMS(b) > top_stack->maxsyms)
		printf_filtered("Internal: block at @%x overfilled (by %d)\n",
			b, BLOCK_NSYMS(b) - top_stack->maxsyms);
}

/* Add a new block B to a symtab S */

static
add_block(b,s)
	struct block *b;
	struct symtab *s;
{
	struct blockvector *bv = BLOCKVECTOR(s);

	bv = (struct blockvector *)xrealloc(bv, sizeof(struct blockvector) +
					    BLOCKVECTOR_NBLOCKS(bv) * sizeof(bv->block));
	if (bv != BLOCKVECTOR(s))
		BLOCKVECTOR(s) = bv;

	BLOCKVECTOR_BLOCK(bv, BLOCKVECTOR_NBLOCKS(bv)++) = b;
}

/* Add a new linenumber entry (LINENO,ADR) to a linevector LT.
   MIPS' linenumber encoding might need more than one byte
   to describe it, LAST is used to detect these continuation lines */

static 
add_line(lt, lineno, adr, last)
	struct linetable *lt;
	CORE_ADDR adr;
{
	if (last == 0)
		last = -2;	/* make sure we record first line */

	if (last == lineno)	/* skip continuation lines */
		return lineno;

	lt->item[lt->nitems].line = lineno;
	lt->item[lt->nitems++].pc = adr << 2;
	return lineno;
}



/* Comparison functions, used when sorting things */

/*  Symtabs must be ordered viz the code segments they cover */

static int
compare_symtabs( s1, s2)
	struct symtab **s1, **s2;
{
	/* "most specific" first */

	register struct block *b1, *b2;
	b1 = BLOCKVECTOR_BLOCK(BLOCKVECTOR(*s1),GLOBAL_BLOCK);
	b2 = BLOCKVECTOR_BLOCK(BLOCKVECTOR(*s2),GLOBAL_BLOCK);
	if (BLOCK_END(b1) == BLOCK_END(b2))
		return BLOCK_START(b1) - BLOCK_START(b2);
	return BLOCK_END(b1) - BLOCK_END(b2);
}


/*  Partial Symtabs, same */

static int
compare_psymtabs( s1, s2)
	struct partial_symtab **s1, **s2;
{
	/* Perf twist: put the ones with no code at the end */

	register int a = (*s1)->textlow;
	register int b = (*s2)->textlow;
	if (a == 0)
		return b;
	if (b == 0)
		return -a;
	return a - b;
}


/* Partial symbols are compared lexicog by their print names */

static int
compare_psymbols (s1, s2)
	register struct partial_symbol *s1, *s2;
{
	register char
	               *st1 = SYMBOL_NAME(s1),
	               *st2 = SYMBOL_NAME(s2);

	return (st1[0] - st2[0] ? st1[0] - st2[0] :
		strcmp(st1 + 1, st2 + 1));
}

/* Blocks with a smaller low bound should come first */

static int compare_blocks(b1,b2)
	struct block **b1, **b2;
{
	register int addr_diff;

	addr_diff = (BLOCK_START((*b1))) - (BLOCK_START((*b2)));
	if (addr_diff == 0)
		return (BLOCK_END((*b1))) - (BLOCK_END((*b2)));
	return addr_diff;
}


/* Sorting and reordering procedures */

/* Sort the blocks of a symtab S.
   Reorder the blocks in the blockvector by code-address,
   as required by some MI search routines */

static void
sort_blocks(s)
	struct symtab *s;
{
	struct blockvector *bv = BLOCKVECTOR(s);

	if (BLOCKVECTOR_NBLOCKS(bv) <= 2) {
		/* Cosmetic */
		if (BLOCK_END(BLOCKVECTOR_BLOCK(bv,GLOBAL_BLOCK)) == 0)
			BLOCK_START(BLOCKVECTOR_BLOCK(bv,GLOBAL_BLOCK)) = 0;
		return;
	}
	/*
	 * This is very unfortunate: normally all functions are compiled in
	 * the order they are found, but if the file is compiled -O3 things
	 * are very different.  It would be nice to find a reliable test
	 * to detect -O3 images in advance.
	 */
	if (BLOCKVECTOR_NBLOCKS(bv) > 3)
		qsort(&BLOCKVECTOR_BLOCK(bv,FIRST_LOCAL_BLOCK),
		      BLOCKVECTOR_NBLOCKS(bv) - FIRST_LOCAL_BLOCK,
		      sizeof(struct block *),
		      compare_blocks);

	{
		register CORE_ADDR high = 0;
		register int    i, j = BLOCKVECTOR_NBLOCKS(bv);

		for (i = FIRST_LOCAL_BLOCK; i < j; i++)
			if (high < BLOCK_END(BLOCKVECTOR_BLOCK(bv,i)))
				high = BLOCK_END(BLOCKVECTOR_BLOCK(bv,i));
		BLOCK_END(BLOCKVECTOR_BLOCK(bv,GLOBAL_BLOCK)) = high;
	}

	BLOCK_START(BLOCKVECTOR_BLOCK(bv,GLOBAL_BLOCK)) =
		BLOCK_START(BLOCKVECTOR_BLOCK(bv,FIRST_LOCAL_BLOCK));

	BLOCK_START(BLOCKVECTOR_BLOCK(bv,STATIC_BLOCK)) = 
		BLOCK_START(BLOCKVECTOR_BLOCK(bv,GLOBAL_BLOCK));
	BLOCK_END  (BLOCKVECTOR_BLOCK(bv,STATIC_BLOCK)) =
		BLOCK_END  (BLOCKVECTOR_BLOCK(bv,GLOBAL_BLOCK));
}

/* Sort the symtab list, as required by some search procedures.
   We want files ordered to make them look right to users, and for
   searching (see block_for_pc).  */

static void
reorder_symtabs()
{
	register int i;
	struct symtab *stab;
	register struct symtab **all_symtabs;
	register int symtab_count;

	if (!symtab_list)
		return;

	/* Create an array of pointers to all the symtabs.  */
	for (symtab_count = 0, stab = symtab_list;
	     stab;
	     symtab_count++, stab = stab->next) {
		obstack_grow (psymbol_obstack, &stab, sizeof (stab));
		/* FIXME: Only sort blocks for new symtabs ??? */
		sort_blocks(stab);
	}

	all_symtabs = (struct symtab **)
		obstack_base (psymbol_obstack);
	qsort((char *)all_symtabs, symtab_count,
		sizeof(struct symtab *), compare_symtabs);

	/* Re-construct the symtab list, but now it is sorted.  */
	for (i = 0; i < symtab_count-1; i++)
		all_symtabs[i]->next = all_symtabs[i+1];
	all_symtabs[i]->next = 0;
	symtab_list = all_symtabs[0];

	obstack_free (psymbol_obstack, all_symtabs);
}

/* Sort the partial symtab list, as required by some search procedures.
   PC lookups stop at the first psymtab such that textlow <= PC < texthigh */

static void
reorder_psymtabs()
{
	register int i;
	register int all_psymtabs_count;
	struct partial_symtab *pstab;
	struct partial_symtab **all_psymtabs;

	if (!partial_symtab_list)
		return;

	/* Create an array of pointers to all the partial_symtabs.  */

	for (all_psymtabs_count = 0, pstab = partial_symtab_list;
	     pstab;
	     all_psymtabs_count++, pstab = pstab->next)
	  obstack_grow (psymbol_obstack, &pstab, sizeof (pstab));

	all_psymtabs = (struct partial_symtab **)
		 obstack_base (psymbol_obstack);

	qsort((char *)all_psymtabs, all_psymtabs_count,
		sizeof(struct partial_symtab *), compare_psymtabs);

	/* Re-construct the partial_symtab_list, but now it is sorted.  */

	for (i = 0; i < all_psymtabs_count-1; i++)
		all_psymtabs[i]->next = all_psymtabs[i+1];
	all_psymtabs[i]->next = 0;
	partial_symtab_list = all_psymtabs[0];

	obstack_free (psymbol_obstack, all_psymtabs);
}

/* Constructor/restructor/destructor procedures */

/* Allocate a new symtab for NAME.  Needs an estimate of how many symbols
   MAXSYMS and linenumbers MAXLINES we'll put in it */

static
struct symtab *
new_symtab(name, maxsyms, maxlines)
	char *name;
{
	struct symtab *s = (struct symtab *) xzalloc(sizeof(struct symtab));
	int i;

	LINETABLE(s) = new_linetable(maxlines);

	s->filename = name;

	/* All symtabs must have at least two blocks */
	BLOCKVECTOR(s) = new_bvect(2);
	BLOCKVECTOR_BLOCK(BLOCKVECTOR(s), GLOBAL_BLOCK) = new_block(maxsyms);
	BLOCKVECTOR_BLOCK(BLOCKVECTOR(s), STATIC_BLOCK) = new_block(maxsyms);
	BLOCK_SUPERBLOCK( BLOCKVECTOR_BLOCK(BLOCKVECTOR(s),STATIC_BLOCK)) =
		BLOCKVECTOR_BLOCK(BLOCKVECTOR(s), GLOBAL_BLOCK);

	s->free_code = free_linetable;

	/* Link the new symtab into the list of such.  */
	s->next = symtab_list;
	symtab_list = s;

	return s;
}

/* Cleanup before loading a fresh image */

static destroy_all_symtabs()
{
  if (symfile)
    free(symfile);
  symfile = 0;

  free_all_symtabs();
  current_source_symtab = 0;
  /* psymtabs! */
}

/* Allocate a new partial_symtab NAME */

static struct partial_symtab *
new_psymtab(name)
	char *name;
{
	struct partial_symtab *pst;

	pst = (struct partial_symtab *)
	      obstack_alloc (psymbol_obstack, sizeof (*pst));
	bzero (pst, sizeof (*pst));

	if (name == (char*)-1)		/* FIXME -- why not null here? */
		pst->filename = "<no name>";
	else
		pst->filename = name;

	pst->next = partial_symtab_list;
	partial_symtab_list = pst;

	/* Keep a backpointer to the file`s symbols */
	pst->ldsymlen = (int)cur_hdr;

	/* The way to turn this into a symtab is to call... */
	pst->read_symtab = mipscoff_psymtab_to_symtab;

	return pst;
}


/* Allocate a new NAME psymbol from LIST, extending it if necessary.
   The psymbol belongs to the psymtab at index PST_IDX */

static struct partial_symbol *
new_psymbol(list, name, pst_idx)
	struct psymbol_allocation_list *list;
	char *name;
{
	struct partial_symbol *p;
	struct partial_symtab *pst = fdr_to_pst[pst_idx].pst;

	/* Lists are pre-sized, we won`t overflow */

	p = list->list + pst->globals_offset + pst->n_global_syms++;
	SYMBOL_NAME(p) = name;
	return p;
}


/* Allocate a linetable array of the given SIZE */

static
struct linetable *new_linetable(size)
{
	struct linetable *l;

	size = size * sizeof(l->item) + sizeof(struct linetable);
	l = (struct linetable *)xmalloc(size);
	l->nitems = 0;
	return l;
}

/* Oops, too big. Shrink it.  This was important with the 2.4 linetables,
   I am not so sure about the 3.4 ones */

static shrink_linetable(s)
	struct symtab *s;
{
	struct linetable *l = new_linetable(LINETABLE(s)->nitems);

	bcopy(LINETABLE(s), l,
	      LINETABLE(s)->nitems * sizeof(l->item) + sizeof(struct linetable));
	free (LINETABLE(s));
	LINETABLE(s) = l;
}

/* Allocate and zero a new blockvector of NBLOCKS blocks. */

static
struct blockvector *new_bvect(nblocks)
{
	struct blockvector *bv;
	int size;

	size = sizeof(struct blockvector) + nblocks * sizeof(struct block*);
	bv = (struct blockvector *) xzalloc(size);

	BLOCKVECTOR_NBLOCKS(bv) = nblocks;

	return bv;
}

/* Allocate and zero a new block of MAXSYMS symbols */

static
struct block *new_block(maxsyms)
{
	int size = sizeof(struct block) + (maxsyms-1) * sizeof(struct symbol *);
	struct block *b = (struct block *)xzalloc(size);

	return b;
}

/* Ooops, too big. Shrink block B in symtab S to its minimal size */

static struct block *
shrink_block(b, s)
	struct block *b;
	struct symtab *s;
{
	struct block *new;
	struct blockvector *bv = BLOCKVECTOR(s);
	int i;

	/* Just get a new one, copy, and fix references to the old one */

	new = (struct block *)xmalloc(sizeof(struct block) +
	  	(BLOCK_NSYMS(b)-1) * sizeof(struct symbol *));

	bcopy(b, new, sizeof(*new) + (BLOCK_NSYMS(b) - 1) * sizeof(struct symbol*));

	/* Should chase pointers to old one.  Fortunately, that`s just
	   the block`s function and inferior blocks */
	if (BLOCK_FUNCTION(b) && SYMBOL_BLOCK_VALUE(BLOCK_FUNCTION(b)) == b)
		SYMBOL_BLOCK_VALUE(BLOCK_FUNCTION(b)) = new;
	for (i = 0; i < BLOCKVECTOR_NBLOCKS(bv); i++)
		if (BLOCKVECTOR_BLOCK(bv,i) == b)
			BLOCKVECTOR_BLOCK(bv,i) = new;
		else if (BLOCK_SUPERBLOCK(BLOCKVECTOR_BLOCK(bv,i)) == b)
			BLOCK_SUPERBLOCK(BLOCKVECTOR_BLOCK(bv,i)) = new;
	free(b);
	return new;
}

/* Create a new symbol with printname NAME */

static
struct symbol *
new_symbol(name)
	char *name;
{
	struct symbol *s = (struct symbol *) 
		obstack_alloc (symbol_obstack, sizeof (struct symbol));

	bzero (s, sizeof (*s));
	SYMBOL_NAME(s) = name;
	return s;
}

/* Create a new type with printname NAME */

static
struct type *
new_type(name)
	char *name;
{
	struct type *t = (struct type *)
		obstack_alloc (symbol_obstack, sizeof (struct type));

	bzero (t, sizeof (*t));
	TYPE_NAME(t) = name;
	return t;
}

/* Create and initialize a new type with printname NAME.
   CODE and LENGTH are the initial info we put in,
   UNS says whether the type is unsigned or not.  */

static
struct type *
make_type(code, length, uns, name)
     enum type_code code;
     int length, uns;
     char *name;
{
	register struct type *type;

	type = (struct type *) xzalloc(sizeof(struct type));
	TYPE_CODE(type) = code;
	TYPE_LENGTH(type) = length;
	TYPE_FLAGS(type) = uns ? TYPE_FLAG_UNSIGNED : 0;
	TYPE_NAME(type) = name;

	return type;
}

/* Allocate a new field named NAME to the type TYPE */

static
struct field *new_field(type,name)
	struct type *type;
	char *name;
{
	struct field *f;

	/* Fields are kept in an array */
	if (TYPE_NFIELDS(type))
		TYPE_FIELDS(type) = (struct field*)xrealloc(TYPE_FIELDS(type),
			(TYPE_NFIELDS(type)+1) * sizeof(struct field));
	else
		TYPE_FIELDS(type) = (struct field*)xzalloc(2*sizeof(struct field));
	f = &(TYPE_FIELD(type,TYPE_NFIELDS(type)++));
	bzero(f, sizeof(struct field));
	if (name)
		f->name = name;
	return f;
}

/* Make an enum constant for a member F of an enumerated type T */

static
make_enum_constant(f,t)
	struct field *f;
	struct type *t;
{
	struct symbol *s;
	/*
	 * This is awful, but that`s the way it is supposed to be
	 * (BTW, no need to free the real 'type', it's a builtin)
	 */
	f->type = (struct type *) f->bitpos;

	s = new_symbol(f->name);
	SYMBOL_NAMESPACE(s) = VAR_NAMESPACE;
	SYMBOL_CLASS(s) = LOC_CONST;
	SYMBOL_TYPE(s) = t;
	SYMBOL_VALUE(s) = f->bitpos;
	add_symbol(s, top_stack->cur_block);
}



/* Things used for calling functions in the inferior.
   These functions are exported to our companion
   mips-dep.c file and are here because they play
   with the symbol-table explicitly. */

#if 0
/* Need to make a new symbol on the fly for the dummy
   frame we put on the stack.  Which goes in the.. */

static struct symtab *dummy_symtab;

/* Make up a dummy symbol for the code we put at END_PC,
   of size SIZE, invoking a function with NARGS arguments
   and using a frame of FRAMESIZE bytes */

mips_create_dummy_symbol(end_pc, size, nargs, framesize)
{
	struct block   *bl;
	struct symbol  *g;
	struct mips_extra_func_info *gdbinfo;

	/* Allocate symtab if not done already */
	if (dummy_symtab == 0)
		dummy_symtab = new_symtab(".dummy_symtab.", 100, 0);

	/* Make a new block. Only needs one symbol */
	bl = new_block(1);
	BLOCK_START(bl) = end_pc - size;
	BLOCK_END(bl) = end_pc;

	BLOCK_SUPERBLOCK(bl) =
		BLOCKVECTOR_BLOCK(BLOCKVECTOR(dummy_symtab),GLOBAL_BLOCK);
	add_block(bl, dummy_symtab);
	sort_blocks(dummy_symtab);

	BLOCK_FUNCTION(bl) = new_symbol("??");
	SYMBOL_BLOCK_VALUE(BLOCK_FUNCTION(bl)) = bl;
	g = new_symbol(".gdbinfo.");
	BLOCK_SYM(bl,BLOCK_NSYMS(bl)++) = g;

	SYMBOL_NAMESPACE(g) = LABEL_NAMESPACE;
	SYMBOL_CLASS(g) = LOC_CONST;
	SYMBOL_TYPE(g) = builtin_type_void;
	gdbinfo = (struct mips_extra_func_info *)
		xzalloc(sizeof(struct mips_extra_func_info));

	SYMBOL_VALUE(g) = (long) gdbinfo;

	gdbinfo->numargs = nargs;
	gdbinfo->framesize = framesize;
	gdbinfo->framereg = 29;
	gdbinfo->pcreg = 31;
	gdbinfo->regmask = -2;
	gdbinfo->regoffset = -4;
	gdbinfo->fregmask = 0;	/* XXX */
	gdbinfo->fregoffset = 0;	/* XXX */
}

/* We just returned from the dummy code at END_PC, drop its symbol */

mips_destroy_dummy_symbol(end_pc)
{
	struct block *bl;
	struct blockvector *bv = BLOCKVECTOR(dummy_symtab);
	int i;

	bl = block_for_pc(end_pc);
	free(BLOCK_FUNCTION(bl));
	free(SYMBOL_VALUE(BLOCK_SYM(bl,0)));
	free(BLOCK_SYM(bl,0));

	for (i = FIRST_LOCAL_BLOCK; i < BLOCKVECTOR_NBLOCKS(bv); i++)
		if (BLOCKVECTOR_BLOCK(bv,i) == bl)
			break;
	for (; i < BLOCKVECTOR_NBLOCKS(bv) - 1; i++)
		BLOCKVECTOR_BLOCK(bv,i) = BLOCKVECTOR_BLOCK(bv,i+1);
	BLOCKVECTOR_NBLOCKS(bv)--;
	sort_blocks(dummy_symtab);
	free(bl);
}
#endif

/* Sigtramp: make sure we have all the necessary information
   about the signal trampoline code. Since the official code
   from MIPS does not do so, we make up that information ourselves.
   If they fix the library (unlikely) this code will neutralize itself. */

static
fixup_sigtramp()
{
	struct symbol  *s;
	struct symtab  *st;
	struct block   *b, *b0;

	sigtramp_address = -1;

	/* We know it is sold as sigvec */
	s = lookup_symbol("sigvec", 0, VAR_NAMESPACE, 0, NULL);

	/* Most programs do not play with signals */
	if (s == 0)
		return;

	b0 = SYMBOL_BLOCK_VALUE(s);

	/* A label of sigvec, to be more precise */
	s = lookup_symbol("sigtramp", b0, VAR_NAMESPACE, 0, NULL);

	/* But maybe this program uses its own version of sigvec */
	if (s == 0)
		return;

	sigtramp_address = SYMBOL_VALUE(s);
	sigtramp_end = sigtramp_address + 0x88;	/* black magic */

	/* Did we or MIPSco fix the library ? */
	if (SYMBOL_CLASS(s) == LOC_BLOCK)
		return;

	/* But what symtab does it live in ? */
	st = find_pc_symtab(SYMBOL_VALUE(s));

	/*
	 * Ok, there goes the fix: turn it into a procedure, with all the
	 * needed info.  Note we make it a nested procedure of sigvec,
	 * which is the way the (assembly) code is actually written.
	 */
	SYMBOL_NAMESPACE(s) = VAR_NAMESPACE;
	SYMBOL_CLASS(s) = LOC_BLOCK;
	SYMBOL_TYPE(s) = make_type(TYPE_CODE_FUNC, 4, 0, 0);
	TYPE_TARGET_TYPE(SYMBOL_TYPE(s)) = builtin_type_void;

	/* Need a block to allocate .gdbinfo. in */
	b = new_block(1);
	SYMBOL_BLOCK_VALUE(s) = b;
	BLOCK_START(b) = sigtramp_address;
	BLOCK_END(b) = sigtramp_end;
	BLOCK_FUNCTION(b) = s;
	BLOCK_SUPERBLOCK(b) = BLOCK_SUPERBLOCK(b0);
	add_block(b, st);
	sort_blocks(st);

	/* Make a .gdbinfo. for it */
	{
		struct mips_extra_func_info *e =
			(struct mips_extra_func_info *)
			xzalloc(sizeof(struct mips_extra_func_info));

		e->numargs = 0;	/* the kernel thinks otherwise */
		/* align_longword(sigcontext + SIGFRAME) */
		e->framesize = 0x150;
		e->framereg = SP_REGNUM;
		e->pcreg = 31;
		e->regmask = -2;
		e->regoffset = -(41 * sizeof(int));
		e->fregmask = -1;
		e->fregoffset = -(37 * sizeof(int));
		e->isym = (long)s;

		s = new_symbol(".gdbinfo.");
		SYMBOL_VALUE(s) = (int) e;
		SYMBOL_NAMESPACE(s) = LABEL_NAMESPACE;
		SYMBOL_CLASS(s) = LOC_CONST;
		SYMBOL_TYPE(s) = builtin_type_void;
	}

	BLOCK_SYM(b,BLOCK_NSYMS(b)++) = s;
}


/* Initialization */

static struct sym_fns ecoff_sym_fns = {"ecoff", 5,
		mipscoff_new_init, mipscoff_symfile_init,
		mipscoff_symfile_read, mipscoff_symfile_discard};

_initialize_mipsread ()
{
	add_symtab_fns (&ecoff_sym_fns);

	bzero (&global_psymbols, sizeof (global_psymbols));
	bzero (&static_psymbols, sizeof (static_psymbols));

	/* Missing basic types */
	builtin_type_string = make_type(TYPE_CODE_PASCAL_ARRAY,
					1, 0, "string");
	builtin_type_complex = make_type(TYPE_CODE_FLT,
					 2 * sizeof(float), 0, "complex");
	builtin_type_double_complex = make_type(TYPE_CODE_FLT,
				    2 * sizeof(double), 0, "double_complex");
	builtin_type_fixed_dec = make_type(TYPE_CODE_INT, sizeof(int),
					   0, "fixed_decimal");
	builtin_type_float_dec = make_type(TYPE_CODE_FLT, sizeof(double),
					   0, "floating_decimal");

	/* Templates types */
	builtin_type_ptr = lookup_pointer_type (builtin_type_void);
	builtin_type_struct = make_type(TYPE_CODE_STRUCT, 0, 0, 0);
	builtin_type_union = make_type(TYPE_CODE_UNION, 0, 0, 0);
	builtin_type_enum = make_type(TYPE_CODE_ENUM, 0, 0, 0);
	builtin_type_range = make_type(TYPE_CODE_RANGE, 0, 0, 0);
	builtin_type_set = make_type(TYPE_CODE_SET, 0, 0, 0);
}
