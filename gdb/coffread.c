/* Read coff symbol tables and convert to internal format, for GDB.
   Design and support routines derived from dbxread.c, and UMAX COFF
   specific routines written 9/1/87 by David D. Johnson, Brown University.
   Revised 11/27/87 ddj@cs.brown.edu
   Copyright (C) 1987-1991 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


#include <stdio.h>
#include "defs.h"
#include "param.h"
#include "symtab.h"
#include "breakpoint.h"
#include "bfd.h"
#include "symfile.h"

#ifndef NO_COFF

#if defined (TDESC)
/* Need to get C_VERSION and friends.  */
#include <a.out.h>
#else /* not TDESC */
#include <intel-coff.h>
#endif /* not TDESC */

#include <obstack.h>
#include <string.h>

#include "libcoff.h"		/* FIXME secret internal data from BFD */

static void add_symbol_to_list ();
static void read_coff_symtab ();
static void patch_opaque_types ();
static struct type *decode_function_type ();
static struct type *decode_type ();
static struct type *decode_base_type ();
static struct type *read_enum_type ();
static struct type *read_struct_type ();
static void finish_block ();
static struct blockvector *make_blockvector ();
static struct symbol *process_coff_symbol ();
static int init_stringtab ();
static void free_stringtab ();
static char *getfilename ();
static char *getsymname ();
static int init_lineno ();
static void enter_linenos ();
static void read_one_sym ();

extern int fclose ();
extern void free_all_symtabs ();
extern void free_all_psymtabs ();

/* To be an sdb debug type, type must have at least a basic or primary
   derived type.  Using this rather than checking against T_NULL is
   said to prevent core dumps if we try to operate on Michael Bloom
   dbx-in-coff file.  */

#define SDB_TYPE(type) (BTYPE(type) | (type & N_TMASK))

/* external routines from the BFD library -- undocumented interface used
   by GDB to read symbols.  Move to libcoff.h.  FIXME-SOMEDAY!  */
extern void bfd_coff_swap_sym (/* symfile_bfd, &sym */);
extern void bfd_coff_swap_aux (/* symfile_bfd, &aux, type, sclass */);
extern void bfd_coff_swap_lineno (/* symfile_bfd, &lineno */);


/* Name of source file whose symbol data we are now processing.
   This comes from a symbol named ".file".  */

static char *last_source_file;

/* Core address of start and end of text of current source file.
   This comes from a ".text" symbol where x_nlinno > 0.  */

static CORE_ADDR cur_src_start_addr;
static CORE_ADDR cur_src_end_addr;

/* Core address of the end of the first object file.  */
static CORE_ADDR first_object_file_end;

/* End of the text segment of the executable file,
   as found in the symbol _etext.  */

static CORE_ADDR end_of_text_addr;

/* The addresses of the symbol table stream and number of symbols
   of the object file we are reading (as copied into core).  */

static FILE *nlist_stream_global;
static int nlist_nsyms_global;

/* The entry point (starting address) of the file, if it is an executable.  */

static CORE_ADDR entry_point;

/* The index in the symbol table of the last coff symbol that was processed.  */

static int symnum;

/* Vector of types defined so far, indexed by their coff symnum.  */

static struct typevector *type_vector;

/* Number of elements allocated for type_vector currently.  */

static int type_vector_length;

/* Vector of line number information.  */

static struct linetable *line_vector;

/* Index of next entry to go in line_vector_index.  */

static int line_vector_index;

/* Last line number recorded in the line vector.  */

static int prev_line_number;

/* Number of elements allocated for line_vector currently.  */

static int line_vector_length;

#ifdef TDESC
#include "tdesc.h"
#define SEM
int int_sem_val = 's' << 24 | 'e' << 16 | 'm' << 8 | '.';
int temp_sem_val;
int last_coffsem = 2;
#if 0
  /* This isn't used currently.  */
int last_coffsyn = 0;
#endif
int debug_info = 0;	/*used by tdesc */
extern dc_dcontext_t tdesc_handle;
extern int safe_to_init_tdesc_context;
#endif

/* Chain of typedefs of pointers to empty struct/union types.
   They are chained thru the SYMBOL_VALUE_CHAIN.  */

#define HASHSIZE 127
static struct symbol *opaque_type_chain[HASHSIZE];

/* Record the symbols defined for each context in a list.
   We don't create a struct block for the context until we
   know how long to make it.  */

struct pending
{
  struct pending *next;
  struct symbol *symbol;
};

/* Here are the three lists that symbols are put on.  */

struct pending *file_symbols;	/* static at top level, and types */

struct pending *global_symbols;	/* global functions and variables */

struct pending *local_symbols;	/* everything local to lexical context */

/* List of unclosed lexical contexts
   (that will become blocks, eventually).  */

struct context_stack
{
  struct context_stack *next;
  struct pending *locals;
  struct pending_block *old_blocks;
  struct symbol *name;
  CORE_ADDR start_addr;
  int depth;
};

struct context_stack *context_stack;

/* Nonzero if within a function (so symbols should be local,
   if nothing says specifically).  */

int within_function;

/* List of blocks already made (lexical contexts already closed).
   This is used at the end to make the blockvector.  */

struct pending_block
{
  struct pending_block *next;
  struct block *block;
};

struct pending_block *pending_blocks;

extern CORE_ADDR startup_file_start;	/* From blockframe.c */
extern CORE_ADDR startup_file_end;	/* From blockframe.c */

/* Complaints about various problems in the file being read  */

struct complaint ef_complaint = 
  {"Unmatched .ef symbol(s) ignored starting at symnum %d", 0, 0};

struct complaint lineno_complaint =
  {"Line number pointer %d lower than start of line numbers", 0, 0};


/* Look up a coff type-number index.  Return the address of the slot
   where the type for that index is stored.
   The type-number is in INDEX. 

   This can be used for finding the type associated with that index
   or for associating a new type with the index.  */

static struct type **
coff_lookup_type (index)
     register int index;
{
  if (index >= type_vector_length)
    {
      int old_vector_length = type_vector_length;

      type_vector_length *= 2;
      if (type_vector_length < index) {
	type_vector_length = index * 2;
      }
      type_vector = (struct typevector *)
	xrealloc (type_vector, sizeof (struct typevector)
				+ type_vector_length * sizeof (struct type *));
      bzero (&type_vector->type[ old_vector_length ],
	     (type_vector_length - old_vector_length) * sizeof(struct type *));
    }
  return &type_vector->type[index];
}

/* Make sure there is a type allocated for type number index
   and return the type object.
   This can create an empty (zeroed) type object.  */

static struct type *
coff_alloc_type (index)
     int index;
{
  register struct type **type_addr = coff_lookup_type (index);
  register struct type *type = *type_addr;

  /* If we are referring to a type not known at all yet,
     allocate an empty type for it.
     We will fill it in later if we find out how.  */
  if (type == 0)
    {
      type = (struct type *) obstack_alloc (symbol_obstack,
					    sizeof (struct type));
      bzero (type, sizeof (struct type));
      *type_addr = type;
    }
  return type;
}

/* maintain the lists of symbols and blocks */

/* Add a symbol to one of the lists of symbols.  */
static void
add_symbol_to_list (symbol, listhead)
     struct symbol *symbol;
     struct pending **listhead;
{
  register struct pending *link
    = (struct pending *) xmalloc (sizeof (struct pending));

  link->next = *listhead;
  link->symbol = symbol;
  *listhead = link;
}

/* Take one of the lists of symbols and make a block from it.
   Put the block on the list of pending blocks.  */

static void
finish_block (symbol, listhead, old_blocks, start, end)
     struct symbol *symbol;
     struct pending **listhead;
     struct pending_block *old_blocks;
     CORE_ADDR start, end;
{
  register struct pending *next, *next1;
  register struct block *block;
  register struct pending_block *pblock;
  struct pending_block *opblock;
  register int i;

  /* Count the length of the list of symbols.  */

  for (next = *listhead, i = 0; next; next = next->next, i++);

  block = (struct block *)
	    obstack_alloc (symbol_obstack, sizeof (struct block) + (i - 1) * sizeof (struct symbol *));

  /* Copy the symbols into the block.  */

  BLOCK_NSYMS (block) = i;
  for (next = *listhead; next; next = next->next)
    BLOCK_SYM (block, --i) = next->symbol;

  BLOCK_START (block) = start;
  BLOCK_END (block) = end;
  BLOCK_SUPERBLOCK (block) = 0;	/* Filled in when containing block is made */

  /* Put the block in as the value of the symbol that names it.  */

  if (symbol)
    {
      SYMBOL_BLOCK_VALUE (symbol) = block;
      BLOCK_FUNCTION (block) = symbol;
    }
  else
    BLOCK_FUNCTION (block) = 0;

  /* Now free the links of the list, and empty the list.  */

  for (next = *listhead; next; next = next1)
    {
      next1 = next->next;
      free (next);
    }
  *listhead = 0;

  /* Install this block as the superblock
     of all blocks made since the start of this scope
     that don't have superblocks yet.  */

  opblock = 0;
  for (pblock = pending_blocks; pblock != old_blocks; pblock = pblock->next)
    {
      if (BLOCK_SUPERBLOCK (pblock->block) == 0)
	BLOCK_SUPERBLOCK (pblock->block) = block;
      opblock = pblock;
    }

  /* Record this block on the list of all blocks in the file.
     Put it after opblock, or at the beginning if opblock is 0.
     This puts the block in the list after all its subblocks.  */

  pblock = (struct pending_block *) xmalloc (sizeof (struct pending_block));
  pblock->block = block;
  if (opblock)
    {
      pblock->next = opblock->next;
      opblock->next = pblock;
    }
  else
    {
      pblock->next = pending_blocks;
      pending_blocks = pblock;
    }
}

static struct blockvector *
make_blockvector ()
{
  register struct pending_block *next, *next1;
  register struct blockvector *blockvector;
  register int i;

  /* Count the length of the list of blocks.  */

  for (next = pending_blocks, i = 0; next; next = next->next, i++);

  blockvector = (struct blockvector *)
		  obstack_alloc (symbol_obstack, sizeof (struct blockvector) + (i - 1) * sizeof (struct block *));

  /* Copy the blocks into the blockvector.
     This is done in reverse order, which happens to put
     the blocks into the proper order (ascending starting address).
     finish_block has hair to insert each block into the list
     after its subblocks in order to make sure this is true.  */

  BLOCKVECTOR_NBLOCKS (blockvector) = i;
  for (next = pending_blocks; next; next = next->next)
    BLOCKVECTOR_BLOCK (blockvector, --i) = next->block;

  /* Now free the links of the list, and empty the list.  */

  for (next = pending_blocks; next; next = next1)
    {
      next1 = next->next;
      free (next);
    }
  pending_blocks = 0;

  return blockvector;
}

/* Manage the vector of line numbers.  */

static void
record_line (line, pc)
     int line;
     CORE_ADDR pc;
{
  struct linetable_entry *e;
  /* Make sure line vector is big enough.  */

  if (line_vector_index + 2 >= line_vector_length)
    {
      line_vector_length *= 2;
      line_vector = (struct linetable *)
	xrealloc (line_vector, sizeof (struct linetable)
		  + (line_vector_length
		     * sizeof (struct linetable_entry)));
    }

  e = line_vector->item + line_vector_index++;
  e->line = line; e->pc = pc;
}

/* Start a new symtab for a new source file.
   This is called when a COFF ".file" symbol is seen;
   it indicates the start of data for one original source file.  */

static void
start_symtab ()
{
  file_symbols = 0;
  global_symbols = 0;
  context_stack = 0;
  within_function = 0;
  last_source_file = 0;
#ifdef TDESC
  last_coffsem = 2;
#if 0
  /* This isn't used currently.  */
  last_coffsyn = 0;
#endif
#endif

  /* Initialize the source file information for this file.  */

  line_vector_index = 0;
  line_vector_length = 1000;
  prev_line_number = -2;	/* Force first line number to be explicit */
  line_vector = (struct linetable *)
    xmalloc (sizeof (struct linetable)
	     + line_vector_length * sizeof (struct linetable_entry));
}

/* Save the vital information from when starting to read a file,
   for use when closing off the current file.
   NAME is the file name the symbols came from, START_ADDR is the first
   text address for the file, and SIZE is the number of bytes of text.  */

static void
complete_symtab (name, start_addr, size)
    char *name;
    CORE_ADDR start_addr;
    unsigned int size;
{
  last_source_file = savestring (name, strlen (name));
  cur_src_start_addr = start_addr;
  cur_src_end_addr = start_addr + size;

  if (entry_point < cur_src_end_addr
      && entry_point >= cur_src_start_addr)
    {
      startup_file_start = cur_src_start_addr;
      startup_file_end = cur_src_end_addr;
    }
}

/* Finish the symbol definitions for one main source file,
   close off all the lexical contexts for that file
   (creating struct block's for them), then make the
   struct symtab for that file and put it in the list of all such. */

static void
end_symtab ()
{
  register struct symtab *symtab;
  register struct context_stack *cstk;
  register struct blockvector *blockvector;
  register struct linetable *lv;

  /* Finish the lexical context of the last function in the file.  */

  if (context_stack)
    {
      cstk = context_stack;
      context_stack = 0;
      /* Make a block for the local symbols within.  */
      finish_block (cstk->name, &local_symbols, cstk->old_blocks,
		    cstk->start_addr, cur_src_end_addr);
      free (cstk);
    }

  /* Ignore a file that has no functions with real debugging info.  */
  if (pending_blocks == 0 && file_symbols == 0 && global_symbols == 0)
    {
      free (line_vector);
      line_vector = 0;
      line_vector_length = -1;
      last_source_file = 0;
      return;
    }

  /* Create the two top-level blocks for this file (STATIC_BLOCK and
     GLOBAL_BLOCK).  */
  finish_block (0, &file_symbols, 0, cur_src_start_addr, cur_src_end_addr);
  finish_block (0, &global_symbols, 0, cur_src_start_addr, cur_src_end_addr);

  /* Create the blockvector that points to all the file's blocks.  */
  blockvector = make_blockvector ();

  /* Now create the symtab object for this source file.  */
  symtab = (struct symtab *) xmalloc (sizeof (struct symtab));
  symtab->free_ptr = 0;

  /* Fill in its components.  */
  symtab->blockvector = blockvector;
  symtab->free_code = free_linetable;
  symtab->filename = last_source_file;
  symtab->dirname = NULL;
  lv = line_vector;
  lv->nitems = line_vector_index;
  symtab->linetable = (struct linetable *)
    xrealloc (lv, (sizeof (struct linetable)
		   + lv->nitems * sizeof (struct linetable_entry)));
  symtab->nlines = 0;
  symtab->line_charpos = 0;

  symtab->language = language_unknown;
  symtab->fullname = NULL;

#ifdef TDESC
  symtab->coffsem = last_coffsem;
#if 0
  /* This isn't used currently.  Besides, if this is really about "syntax",
     it shouldn't need to stick around past symbol read-in time.  */
  symtab->coffsyn = last_coffsyn;
#endif
#endif

  free_named_symtabs (symtab->filename);

  /* Link the new symtab into the list of such.  */
  symtab->next = symtab_list;
  symtab_list = symtab;

  /* Reinitialize for beginning of new file. */
  line_vector = 0;
  line_vector_length = -1;
  last_source_file = 0;
}

static void
record_misc_function (name, address)
     char *name;
     CORE_ADDR address;
{
#ifdef TDESC
  /* We don't want TDESC entry points on the misc_function_vector */
  if (name[0] == '@') return;
#endif
  /* mf_text isn't true, but apparently COFF doesn't tell us what it really
     is, so this guess is more useful than mf_unknown.  */
  prim_record_misc_function (savestring (name, strlen (name)),
			     address,
			     (int)mf_text);
}

/* coff_symfile_init ()
   is the coff-specific initialization routine for reading symbols.
   It is passed a struct sym_fns which contains, among other things,
   the BFD for the file whose symbols are being read, and a slot for
   a pointer to "private data" which we fill with cookies and other
   treats for coff_symfile_read ().

   We will only be called if this is a COFF or COFF-like file.
   BFD handles figuring out the format of the file, and code in symtab.c
   uses BFD's determination to vector to us.

   The ultimate result is a new symtab (or, FIXME, eventually a psymtab).  */

struct coff_symfile_info {
  file_ptr min_lineno_offset;		/* Where in file lowest line#s are */
  file_ptr max_lineno_offset;		/* 1+last byte of line#s in file */
};

void
coff_symfile_init (sf)
     struct sym_fns *sf;
{
  bfd *abfd = sf->sym_bfd;

  /* Allocate struct to keep track of the symfile */
  /* FIXME memory leak */
  sf->sym_private = xmalloc (sizeof (struct coff_symfile_info));

#if defined (TDESC)
  safe_to_init_tdesc_context  = 0;
#endif

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

/* This function is called for every section; it finds the outer limits
   of the line table (minimum and maximum file offset) so that the
   mainline code can read the whole thing for efficiency.  */

/* ARGSUSED */
static void
find_linenos (abfd, asect, vpinfo)
     bfd *abfd;
     sec_ptr asect;
     void *vpinfo;
{
  struct coff_symfile_info *info;
  int size, count;
  file_ptr offset, maxoff;

/* WARNING WILL ROBINSON!  ACCESSING BFD-PRIVATE DATA HERE!  FIXME!  */
  count = asect->lineno_count;
/* End of warning */

  if (count == 0)
    return;
#if !defined (LINESZ)
/* Just in case, you never know what to expect from those
   COFF header files.  */
#define LINESZ (sizeof (struct lineno))
#endif /* No LINESZ.  */
  size = count * LINESZ;

  info = (struct coff_symfile_info *)vpinfo;
/* WARNING WILL ROBINSON!  ACCESSING BFD-PRIVATE DATA HERE!  FIXME!  */
  offset = asect->line_filepos;
/* End of warning */

  if (offset < info->min_lineno_offset || info->min_lineno_offset == 0)
    info->min_lineno_offset = offset;

  maxoff = offset + size;
  if (maxoff > info->max_lineno_offset)
    info->max_lineno_offset = maxoff;
#ifdef TDESC
  /* While we're at it, find the debug_info.  It's in the s_relptr
     (or, in BFD-speak, rel_filepos) of the text segment section header.  */
  if (strcmp (bfd_section_name (abfd, asect), ".text") == 0)
    {
      /* WARNING WILL ROBINSON!  ACCESSING BFD-PRIVATE DATA HERE!  FIXME!  */
      debug_info = asect->rel_filepos;
      /* End of warning */
      if (tdesc_handle)
	{
	  dc_terminate (tdesc_handle);
	  tdesc_handle = 0;
	}
    }
#endif /* TDESC */
}


/* The BFD for this file -- only good while we're actively reading
   symbols into a psymtab or a symtab.  */

static bfd *symfile_bfd;

/* Read a symbol file, after initialization by coff_symfile_init.  */
/* FIXME!  Addr and Mainline are not used yet -- this will not work for
   shared libraries or add_file!  */

void
coff_symfile_read (sf, addr, mainline)
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

  symfile_bfd = abfd;			/* Kludge for swap routines */

/* WARNING WILL ROBINSON!  ACCESSING BFD-PRIVATE DATA HERE!  FIXME!  */
   desc = fileno ((FILE *)(abfd->iostream));		/* File descriptor */
   num_symbols = bfd_get_symcount (abfd);	/* How many syms */
   symtab_offset = obj_sym_filepos (abfd);	/* Symbol table file offset */
   stringtab_offset = symtab_offset + num_symbols * SYMESZ;  /* String tab */
/* End of warning */

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

  /* Position to read the symbol table.  Do not read it all at once. */
  val = lseek (desc, (long)symtab_offset, 0);
  if (val < 0)
    perror_with_name (name);

  init_misc_bunches ();
  make_cleanup (discard_misc_bunches, 0);

  /* Now that the executable file is positioned at symbol table,
     process it and define symbols accordingly.  */

  read_coff_symtab (desc, num_symbols);

  patch_opaque_types ();

  /* Sort symbols alphabetically within each block.  */

  sort_all_symtab_syms ();

  /* Go over the misc symbol bunches and install them in vector.  */

  condense_misc_bunches (0);

  /* Make a default for file to list.  */

  select_source_symtab (0);	/* FIXME, this might be too slow, see dbxread */
}

void
coff_symfile_discard ()
{
  /* There seems to be nothing to do here.  */
}

void
coff_new_init ()
{
  /* There seems to be nothing to do except free_all_symtabs and set
     symfile to zero, which is done by our caller.  */
}

/* Simplified internal version of coff symbol table information */

struct coff_symbol {
  char *c_name;
  int c_symnum;		/* symbol number of this entry */
  int c_nsyms;		/* 1 if syment only, 2 if syment + auxent, etc */
  long c_value;
  int c_sclass;
  int c_secnum;
  unsigned int c_type;
};

/* Given pointers to a symbol table in coff style exec file,
   analyze them and create struct symtab's describing the symbols.
   NSYMS is the number of symbols in the symbol table.
   We read them one at a time using read_one_sym ().  */

static void
read_coff_symtab (desc, nsyms)
     int desc;
     int nsyms;
{
  int newfd;			/* Avoid multiple closes on same desc */
  FILE *stream; 
  register struct context_stack *new;
  struct coff_symbol coff_symbol;
  register struct coff_symbol *cs = &coff_symbol;
  static SYMENT main_sym;
  static AUXENT main_aux;
  struct coff_symbol fcn_cs_saved;
  static SYMENT fcn_sym_saved;
  static AUXENT fcn_aux_saved;

  /* A .file is open.  */
  int in_source_file = 0;
  int num_object_files = 0;
  int next_file_symnum = -1;

  /* Name of the current file.  */
  char *filestring = "";
  int depth;
  int fcn_first_line;
  int fcn_last_line;
  int fcn_start_addr;
  long fcn_line_ptr;
  struct cleanup *old_chain;


  newfd = dup (desc);
  if (newfd == -1)
    fatal ("Too many open files");
  stream = fdopen (newfd, "r");

  old_chain = make_cleanup (free_all_symtabs, 0);
  make_cleanup (fclose, stream);
  nlist_stream_global = stream;
  nlist_nsyms_global = nsyms;
  last_source_file = 0;
  bzero (opaque_type_chain, sizeof opaque_type_chain);

  type_vector_length = 160;
  type_vector = (struct typevector *)
		xmalloc (sizeof (struct typevector)
				+ type_vector_length * sizeof (struct type *));
  bzero (type_vector->type, type_vector_length * sizeof (struct type *));

  start_symtab ();

  symnum = 0;
  while (symnum < nsyms)
    {
      QUIT;			/* Make this command interruptable.  */
      read_one_sym (cs, &main_sym, &main_aux);

#ifdef SEM
      temp_sem_val = cs->c_name[0] << 24 | cs->c_name[1] << 16 |
                     cs->c_name[2] << 8 | cs->c_name[3];
      if (int_sem_val == temp_sem_val)
        last_coffsem = (int) strtol (cs->c_name+4, (char **) NULL, 10);
#endif

      if (cs->c_symnum == next_file_symnum && cs->c_sclass != C_FILE)
	{
	  if (last_source_file)
	    end_symtab ();

	  start_symtab ();
	  complete_symtab ("_globals_", 0, first_object_file_end);
	  /* done with all files, everything from here on out is globals */
	}

      /* Special case for file with type declarations only, no text.  */
      if (!last_source_file && SDB_TYPE (cs->c_type)
	  && cs->c_secnum == N_DEBUG)
	complete_symtab (filestring, 0, 0);

      /* Typedefs should not be treated as symbol definitions.  */
      if (ISFCN (cs->c_type) && cs->c_sclass != C_TPDEF)
	{
	  /* record as misc function.  if we get '.bf' next,
	   * then we undo this step
	   */
	  record_misc_function (cs->c_name, cs->c_value);

	  fcn_line_ptr = main_aux.x_sym.x_fcnary.x_fcn.x_lnnoptr;
	  fcn_start_addr = cs->c_value;
	  fcn_cs_saved = *cs;
	  fcn_sym_saved = main_sym;
	  fcn_aux_saved = main_aux;
	  continue;
	}

      switch (cs->c_sclass)
	{
	  case C_EFCN:
	  case C_EXTDEF:
	  case C_ULABEL:
	  case C_USTATIC:
	  case C_LINE:
	  case C_ALIAS:
	  case C_HIDDEN:
	    printf ("Bad n_sclass = %d\n", cs->c_sclass);
	    break;

	  case C_FILE:
	    /*
	     * c_value field contains symnum of next .file entry in table
	     * or symnum of first global after last .file.
	     */
	    next_file_symnum = cs->c_value;
	    filestring = getfilename (&main_aux);
	    /*
	     * Complete symbol table for last object file
	     * containing debugging information.
	     */
	    if (last_source_file)
	      {
		end_symtab ();
		start_symtab ();
	      }
	    in_source_file = 1;
	    break;

          case C_STAT:
	    if (cs->c_name[0] == '.') {
		    if (strcmp (cs->c_name, _TEXT) == 0) {
			    if (++num_object_files == 1) {
				    /* last address of startup file */
				    first_object_file_end = cs->c_value +
					    main_aux.x_scn.x_scnlen;
			    }
			    /* Check for in_source_file deals with case of
			       a file with debugging symbols
			       followed by a later file with no symbols.  */
			    if (in_source_file)
			      complete_symtab (filestring, cs->c_value,
					       main_aux.x_scn.x_scnlen);
			    in_source_file = 0;
		    }
		    /* flush rest of '.' symbols */
		    break;
	    }
	    else if (!SDB_TYPE (cs->c_type)
		     && cs->c_name[0] == 'L'
		     && (strncmp (cs->c_name, "LI%", 3) == 0
			 || strncmp (cs->c_name, "LF%", 3) == 0
			 || strncmp (cs->c_name,"LC%",3) == 0
			 || strncmp (cs->c_name,"LP%",3) == 0
			 || strncmp (cs->c_name,"LPB%",4) == 0
			 || strncmp (cs->c_name,"LBB%",4) == 0
			 || strncmp (cs->c_name,"LBE%",4) == 0
			 || strncmp (cs->c_name,"LPBX%",5) == 0))
	      /* At least on a 3b1, gcc generates swbeg and string labels
		 that look like this.  Ignore them.  */
	      break;
	    /* fall in for static symbols that don't start with '.' */
	  case C_EXT:
	    if (cs->c_sclass == C_EXT &&
		cs->c_secnum == N_ABS &&
		strcmp (cs->c_name, _ETEXT) == 0)
		    end_of_text_addr = cs->c_value;
	    if (!SDB_TYPE (cs->c_type)) {
		    if (cs->c_secnum <= 1) {	/* text or abs */
			    record_misc_function (cs->c_name, cs->c_value);
			    break;
		    } else {
			    cs->c_type = T_INT;
		    }
	    }
	    (void) process_coff_symbol (cs, &main_aux);
	    break;

	  case C_FCN:
	    if (strcmp (cs->c_name, ".bf") == 0)
	      {
		within_function = 1;

		/* value contains address of first non-init type code */
		/* main_aux.x_sym.x_misc.x_lnsz.x_lnno
			    contains line number of '{' } */
		fcn_first_line = main_aux.x_sym.x_misc.x_lnsz.x_lnno;

		new = (struct context_stack *)
		  xmalloc (sizeof (struct context_stack));
		new->depth = depth = 0;
		new->next = 0;
		context_stack = new;
		new->locals = 0;
		new->old_blocks = pending_blocks;
		new->start_addr = fcn_start_addr;
		fcn_cs_saved.c_name = getsymname (&fcn_sym_saved);
		new->name = process_coff_symbol (&fcn_cs_saved,
						 &fcn_aux_saved);
	      }
	    else if (strcmp (cs->c_name, ".ef") == 0)
	      {
		      /* the value of .ef is the address of epilogue code;
		       * not useful for gdb
		       */
		/* { main_aux.x_sym.x_misc.x_lnsz.x_lnno
			    contains number of lines to '}' */
		new = context_stack;
		if (new == 0)
		  {
		    complain (&ef_complaint, cs->c_symnum);
		    within_function = 0;
		    break;
		  }
		fcn_last_line = main_aux.x_sym.x_misc.x_lnsz.x_lnno;
		enter_linenos (fcn_line_ptr, fcn_first_line, fcn_last_line);

		finish_block (new->name, &local_symbols, new->old_blocks,
			      new->start_addr,
#if defined (FUNCTION_EPILOGUE_SIZE)
			      /* This macro should be defined only on
				 machines where the
				 fcn_aux_saved.x_sym.x_misc.x_fsize
				 field is always zero.
				 So use the .bf record information that
				 points to the epilogue and add the size
				 of the epilogue.  */
			      cs->c_value + FUNCTION_EPILOGUE_SIZE
#else
			      fcn_cs_saved.c_value +
			          fcn_aux_saved.x_sym.x_misc.x_fsize
#endif
			      );
		context_stack = 0;
		within_function = 0;
		free (new);
	      }
	    break;

	  case C_BLOCK:
	    if (strcmp (cs->c_name, ".bb") == 0)
	      {
		new = (struct context_stack *)
			    xmalloc (sizeof (struct context_stack));
		depth++;
		new->depth = depth;
		new->next = context_stack;
		context_stack = new;
		new->locals = local_symbols;
		new->old_blocks = pending_blocks;
		new->start_addr = cs->c_value;
		new->name = 0;
		local_symbols = 0;
	      }
	    else if (strcmp (cs->c_name, ".eb") == 0)
	      {
		new = context_stack;
		if (new == 0 || depth != new->depth)
		  error ("Invalid symbol data: .bb/.eb symbol mismatch at symbol %d.",
			 symnum);
		if (local_symbols && context_stack->next)
		  {
		    /* Make a block for the local symbols within.  */
		    finish_block (0, &local_symbols, new->old_blocks,
				  new->start_addr, cs->c_value);
		  }
		depth--;
		local_symbols = new->locals;
		context_stack = new->next;
		free (new);
	      }
	    break;
#ifdef TDESC
          case C_VERSION:
#if 0
	    /* This isn't used currently.  */
            if (strcmp (cs->c_name, ".coffsyn") == 0)
		last_coffsyn = cs->c_value;
	    else
#endif /* 0 */
	      if ((strcmp (cs->c_name, ".coffsem") == 0) &&
                     (cs->c_value != 0))
		last_coffsem = cs->c_value;
            break;
#endif /* TDESC */

	  default:
#ifdef TDESC
	    if ((strcmp (cs->c_name, ".coffsem") == 0) &&
		(cs->c_value != 0))
	      last_coffsem = cs->c_value;
            else
#endif
	    (void) process_coff_symbol (cs, &main_aux);
	    break;
	}
    }

  if (last_source_file)
    end_symtab ();
  fclose (stream);
  discard_cleanups (old_chain);
}

/* Routines for reading headers and symbols from executable.  */

#ifdef FIXME
/* Move these XXXMAGIC symbol defns into BFD!  */

/* Read COFF file header, check magic number,
   and return number of symbols. */
read_file_hdr (chan, file_hdr)
    int chan;
    FILHDR *file_hdr;
{
  lseek (chan, 0L, 0);
  if (myread (chan, (char *)file_hdr, FILHSZ) < 0)
    return -1;

  switch (file_hdr->f_magic)
    {
#ifdef MC68MAGIC
    case MC68MAGIC:
#endif
#ifdef NS32GMAGIC
      case NS32GMAGIC:
      case NS32SMAGIC:
#endif
#ifdef I386MAGIC
    case I386MAGIC:
#endif
#ifdef CLIPPERMAGIC
    case CLIPPERMAGIC:
#endif
#if defined (MC68KWRMAGIC) \
  && (!defined (MC68MAGIC) || MC68KWRMAGIC != MC68MAGIC)
    case MC68KWRMAGIC:
#endif
#ifdef MC68KROMAGIC
    case MC68KROMAGIC:
    case MC68KPGMAGIC:
#endif
#ifdef MC88DGMAGIC
    case MC88DGMAGIC:
#endif      
#ifdef MC88MAGIC
    case MC88MAGIC:
#endif      
#ifdef I960ROMAGIC
    case I960ROMAGIC:		/* Intel 960 */
#endif
#ifdef I960RWMAGIC
    case I960RWMAGIC:		/* Intel 960 */
#endif
	return file_hdr->f_nsyms;

      default:
#ifdef BADMAG
	if (BADMAG(file_hdr))
	  return -1;
	else
	  return file_hdr->f_nsyms;
#else
	return -1;
#endif
    }
}
#endif


static void
read_one_sym (cs, sym, aux)
    register struct coff_symbol *cs;
    register SYMENT *sym;
    register AUXENT *aux;
{
  AUXENT temp_aux;
  int i;

  cs->c_symnum = symnum;
  fread ((char *)sym, SYMESZ, 1, nlist_stream_global);
  bfd_coff_swap_sym (symfile_bfd, sym);
  cs->c_nsyms = (sym->n_numaux & 0xff) + 1;
  if (cs->c_nsyms >= 2)
    {
    fread ((char *)aux, AUXESZ, 1, nlist_stream_global);
    bfd_coff_swap_aux (symfile_bfd, aux, sym->n_type, sym->n_sclass);
    /* If more than one aux entry, read past it (only the first aux
       is important). */
    for (i = 2; i < cs->c_nsyms; i++)
      fread ((char *)&temp_aux, AUXESZ, 1, nlist_stream_global);
    }
  cs->c_name = getsymname (sym);
  cs->c_value = sym->n_value;
  cs->c_sclass = (sym->n_sclass & 0xff);
  cs->c_secnum = sym->n_scnum;
  cs->c_type = (unsigned) sym->n_type;
  if (!SDB_TYPE (cs->c_type))
    cs->c_type = 0;

  symnum += cs->c_nsyms;
}

/* Support for string table handling */

static char *stringtab = NULL;

static int
init_stringtab (chan, offset)
    int chan;
    long offset;
{
  long length;
  int val;
  unsigned char lengthbuf[4];

  if (stringtab)
    {
      free (stringtab);
      stringtab = NULL;
    }

  if (lseek (chan, offset, 0) < 0)
    return -1;

  val = myread (chan, (char *)lengthbuf, sizeof lengthbuf);
  length = bfd_h_getlong (symfile_bfd, lengthbuf);

  /* If no string table is needed, then the file may end immediately
     after the symbols.  Just return with `stringtab' set to null. */
  if (val != sizeof length || length < sizeof length)
    return 0;

  stringtab = (char *) xmalloc (length);
  if (stringtab == NULL)
    return -1;

  bcopy (&length, stringtab, sizeof length);
  if (length == sizeof length)		/* Empty table -- just the count */
    return 0;

  val = myread (chan, stringtab + sizeof length, length - sizeof length);
  if (val != length - sizeof length || stringtab[length - 1] != '\0')
    return -1;

  return 0;
}

static void
free_stringtab ()
{
  if (stringtab)
    free (stringtab);
  stringtab = NULL;
}

static char *
getsymname (symbol_entry)
    SYMENT *symbol_entry;
{
  static char buffer[SYMNMLEN+1];
  char *result;

  if (symbol_entry->n_zeroes == 0)
    {
      result = stringtab + symbol_entry->n_offset;
    }
  else
    {
      strncpy (buffer, symbol_entry->n_name, SYMNMLEN);
      buffer[SYMNMLEN] = '\0';
      result = buffer;
    }
  return result;
}

static char *
getfilename (aux_entry)
    AUXENT *aux_entry;
{
  static char buffer[BUFSIZ];
  register char *temp;
  char *result;
  extern char *rindex ();

#ifndef COFF_NO_LONG_FILE_NAMES
#if defined (x_zeroes)
  /* Data General.  */
  if (aux_entry->x_zeroes == 0)
    strcpy (buffer, stringtab + aux_entry->x_offset);
#else /* no x_zeroes */
  if (aux_entry->x_file.x_n.x_zeroes == 0)
    strcpy (buffer, stringtab + aux_entry->x_file.x_n.x_offset);
#endif /* no x_zeroes */
  else
#endif /* COFF_NO_LONG_FILE_NAMES */
    {
#if defined (x_name)
      /* Data General.  */
      strncpy (buffer, aux_entry->x_name, FILNMLEN);
#else
      strncpy (buffer, aux_entry->x_file.x_fname, FILNMLEN);
#endif
      buffer[FILNMLEN] = '\0';
    }
  result = buffer;
  if ((temp = rindex (result, '/')) != NULL)
    result = temp + 1;
  return (result);
}

/* Support for line number handling */
static char *linetab = NULL;
static long linetab_offset;
static unsigned long linetab_size;

/* Read in all the line numbers for fast lookups later.  */
 
static int
init_lineno (chan, offset, size)
    int chan;
    long offset;
    int size;
{
  int val;
  register char *p, *q;

  if (lseek (chan, offset, 0) < 0)
    return -1;
  
  linetab = (char *) xmalloc (size);

  val = myread (chan, linetab, size);
  if (val != size)
    return -1;

  /* Swap all entries */
  q = linetab + size;
  for (p = linetab; p < q; p += LINESZ)
    bfd_coff_swap_lineno (symfile_bfd, (LINENO *)p);

  linetab_offset = offset;
  linetab_size = size;
  make_cleanup (free, linetab);		/* Be sure it gets de-allocated. */
  return 0;
}

#if !defined (L_LNNO32)
#define L_LNNO32(lp) ((lp)->l_lnno)
#endif

static void
enter_linenos (file_offset, first_line, last_line)
    long file_offset;
    register int first_line;
    register int last_line;
{
  register char *rawptr;
  struct lineno lptr;

  if (file_offset < linetab_offset)
    {
      complain (&lineno_complaint, file_offset);
      if (file_offset > linetab_size)	/* Too big to be an offset? */
	return;
      file_offset += linetab_offset;  /* Try reading at that linetab offset */
    }
  
  rawptr = &linetab[file_offset - linetab_offset];

  /* skip first line entry for each function */
  rawptr += LINESZ;
  /* line numbers start at one for the first line of the function */
  first_line--;

  /* Bcopy since occaisionally rawptr isn't pointing at long
     boundaries.  */  
  for (bcopy (rawptr, &lptr, LINESZ);
       L_LNNO32 (&lptr) && L_LNNO32 (&lptr) <= last_line;
       rawptr += LINESZ, bcopy (rawptr, &lptr, LINESZ))
    {
      record_line (first_line + L_LNNO32 (&lptr), lptr.l_addr.l_paddr);
    }
}

static int
hashname (name)
     char *name;
{
  register char *p = name;
  register int total = p[0];
  register int c;

  c = p[1];
  total += c << 2;
  if (c)
    {
      c = p[2];
      total += c << 4;
      if (c)
	total += p[3] << 6;
    }
  
  return total % HASHSIZE;
}

static void
patch_type (type, real_type)
    struct type *type;
    struct type *real_type;
{
  register struct type *target = TYPE_TARGET_TYPE (type);
  register struct type *real_target = TYPE_TARGET_TYPE (real_type);
  int field_size = TYPE_NFIELDS (real_target) * sizeof (struct field);

  TYPE_LENGTH (target) = TYPE_LENGTH (real_target);
  TYPE_NFIELDS (target) = TYPE_NFIELDS (real_target);
  TYPE_FIELDS (target) = (struct field *)
				obstack_alloc (symbol_obstack, field_size);

  bcopy (TYPE_FIELDS (real_target), TYPE_FIELDS (target), field_size);

  if (TYPE_NAME (real_target))
    {
      if (TYPE_NAME (target))
	free (TYPE_NAME (target));
      TYPE_NAME (target) = concat (TYPE_NAME (real_target), "", "");
    }
}

/* Patch up all appropriate typdef symbols in the opaque_type_chains
   so that they can be used to print out opaque data structures properly */

static void
patch_opaque_types ()
{
  struct symtab *s;

  /* Look at each symbol in the per-file block of each symtab.  */
  for (s = symtab_list; s; s = s->next)
    {
      register struct block *b;
      register int i;

      /* Go through the per-file symbols only */
      b = BLOCKVECTOR_BLOCK (BLOCKVECTOR (s), STATIC_BLOCK);
      for (i = BLOCK_NSYMS (b) - 1; i >= 0; i--)
	{
	  register struct symbol *real_sym;

	  /* Find completed typedefs to use to fix opaque ones.
	     Remove syms from the chain when their types are stored,
	     but search the whole chain, as there may be several syms
	     from different files with the same name.  */
	  real_sym = BLOCK_SYM (b, i);
	  if (SYMBOL_CLASS (real_sym) == LOC_TYPEDEF &&
	      SYMBOL_NAMESPACE (real_sym) == VAR_NAMESPACE &&
	      TYPE_CODE (SYMBOL_TYPE (real_sym)) == TYPE_CODE_PTR &&
	      TYPE_LENGTH (TYPE_TARGET_TYPE (SYMBOL_TYPE (real_sym))) != 0)
	    {
	      register char *name = SYMBOL_NAME (real_sym);
	      register int hash = hashname (name);
	      register struct symbol *sym, *prev;

	      prev = 0;
	      for (sym = opaque_type_chain[hash]; sym;)
		{
		  if (name[0] == SYMBOL_NAME (sym)[0] &&
		      !strcmp (name + 1, SYMBOL_NAME (sym) + 1))
		    {
		      if (prev)
			SYMBOL_VALUE_CHAIN (prev) = SYMBOL_VALUE_CHAIN (sym);
		      else
			opaque_type_chain[hash] = SYMBOL_VALUE_CHAIN (sym);

		      patch_type (SYMBOL_TYPE (sym), SYMBOL_TYPE (real_sym));

		      if (prev)
			sym = SYMBOL_VALUE_CHAIN (prev);
		      else
			sym = opaque_type_chain[hash];
		    }
		  else
		    {
		      prev = sym;
		      sym = SYMBOL_VALUE_CHAIN (sym);
		    }
		}
	    }
	}
    }
}

#if defined (clipper)
#define BELIEVE_PCC_PROMOTION 1
#endif

static struct symbol *
process_coff_symbol (cs, aux)
     register struct coff_symbol *cs;
     register AUXENT *aux;
{
  register struct symbol *sym
    = (struct symbol *) obstack_alloc (symbol_obstack, sizeof (struct symbol));
  char *name;
#ifdef NAMES_HAVE_UNDERSCORE
  int offset = 1;
#else
  int offset = 0;
#endif

  bzero (sym, sizeof (struct symbol));
  name = cs->c_name;
  name = (name[0] == '_' ? name + offset : name);
  SYMBOL_NAME (sym) = obstack_copy0 (symbol_obstack, name, strlen (name));

  /* default assumptions */
  SYMBOL_VALUE (sym) = cs->c_value;
  SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;

  if (ISFCN (cs->c_type))
    {
      SYMBOL_TYPE (sym) = 
	lookup_function_type (decode_function_type (cs, cs->c_type, aux));
      SYMBOL_CLASS (sym) = LOC_BLOCK;
      if (cs->c_sclass == C_STAT)
	add_symbol_to_list (sym, &file_symbols);
      else if (cs->c_sclass == C_EXT)
	add_symbol_to_list (sym, &global_symbols);
    }
  else
    {
      SYMBOL_TYPE (sym) = decode_type (cs, cs->c_type, aux);
      switch (cs->c_sclass)
	{
	  case C_NULL:
	    break;

	  case C_AUTO:
	    SYMBOL_CLASS (sym) = LOC_LOCAL;
	    add_symbol_to_list (sym, &local_symbols);
	    break;

	  case C_EXT:
	    SYMBOL_CLASS (sym) = LOC_STATIC;
	    SYMBOL_VALUE_ADDRESS (sym) = (CORE_ADDR) cs->c_value;
	    add_symbol_to_list (sym, &global_symbols);
	    break;

	  case C_STAT:
	    SYMBOL_CLASS (sym) = LOC_STATIC;
	    SYMBOL_VALUE_ADDRESS (sym) = (CORE_ADDR) cs->c_value;
	    if (within_function) {
	      /* Static symbol of local scope */
	      add_symbol_to_list (sym, &local_symbols);
	    }
	    else {
	      /* Static symbol at top level of file */
	      add_symbol_to_list (sym, &file_symbols);
	    }
	    break;

	  case C_REG:
	    SYMBOL_CLASS (sym) = LOC_REGISTER;
	    add_symbol_to_list (sym, &local_symbols);
	    break;

	  case C_LABEL:
	    break;

	  case C_ARG:
	    SYMBOL_CLASS (sym) = LOC_ARG;
	    add_symbol_to_list (sym, &local_symbols);
#if !defined (BELIEVE_PCC_PROMOTION)
	    /* If PCC says a parameter is a short or a char,
	       it is really an int.  */
	    if (SYMBOL_TYPE (sym) == builtin_type_char
		|| SYMBOL_TYPE (sym) == builtin_type_short)
	      SYMBOL_TYPE (sym) = builtin_type_int;
	    else if (SYMBOL_TYPE (sym) == builtin_type_unsigned_char
		     || SYMBOL_TYPE (sym) == builtin_type_unsigned_short)
	      SYMBOL_TYPE (sym) = builtin_type_unsigned_int;
#endif
	    break;

	  case C_REGPARM:
	    SYMBOL_CLASS (sym) = LOC_REGPARM;
	    add_symbol_to_list (sym, &local_symbols);
#if !defined (BELIEVE_PCC_PROMOTION)
	    /* If PCC says a parameter is a short or a char,
	       it is really an int.  */
	    if (SYMBOL_TYPE (sym) == builtin_type_char
		|| SYMBOL_TYPE (sym) == builtin_type_short)
	      SYMBOL_TYPE (sym) = builtin_type_int;
	    else if (SYMBOL_TYPE (sym) == builtin_type_unsigned_char
		     || SYMBOL_TYPE (sym) == builtin_type_unsigned_short)
	      SYMBOL_TYPE (sym) = builtin_type_unsigned_int;
#endif
	    break;
	    
	  case C_TPDEF:
	    SYMBOL_CLASS (sym) = LOC_TYPEDEF;
	    SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;

	    /* If type has no name, give it one */
	    if (TYPE_NAME (SYMBOL_TYPE (sym)) == 0 
		&& (TYPE_FLAGS (SYMBOL_TYPE (sym)) & TYPE_FLAG_PERM) == 0)
	      TYPE_NAME (SYMBOL_TYPE (sym))
					  = concat (SYMBOL_NAME (sym), "", "");

	    /* Keep track of any type which points to empty structured type,
		so it can be filled from a definition from another file */
	    if (TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_PTR &&
		TYPE_LENGTH (TYPE_TARGET_TYPE (SYMBOL_TYPE (sym))) == 0)
	      {
		register int i = hashname (SYMBOL_NAME (sym));

		SYMBOL_VALUE_CHAIN (sym) = opaque_type_chain[i];
		opaque_type_chain[i] = sym;
	      }
	    add_symbol_to_list (sym, &file_symbols);
	    break;

	  case C_STRTAG:
	  case C_UNTAG:
	  case C_ENTAG:
	    SYMBOL_CLASS (sym) = LOC_TYPEDEF;
	    SYMBOL_NAMESPACE (sym) = STRUCT_NAMESPACE;
	    if (TYPE_NAME (SYMBOL_TYPE (sym)) == 0
		&& (TYPE_FLAGS (SYMBOL_TYPE (sym)) & TYPE_FLAG_PERM) == 0)
	      TYPE_NAME (SYMBOL_TYPE (sym))
		= concat ("",
			  (cs->c_sclass == C_ENTAG
			   ? "enum "
			   : (cs->c_sclass == C_STRTAG
			      ? "struct " : "union ")),
			  SYMBOL_NAME (sym));
	    add_symbol_to_list (sym, &file_symbols);
	    break;

	  default:
	    break;
	}
    }
  return sym;
}

/* Decode a coff type specifier;
   return the type that is meant.  */

static
struct type *
decode_type (cs, c_type, aux)
     register struct coff_symbol *cs;
     unsigned int c_type;
     register AUXENT *aux;
{
  register struct type *type = 0;
  unsigned int new_c_type;

  if (c_type & ~N_BTMASK)
    {
      new_c_type = DECREF (c_type);
      if (ISPTR (c_type))
	{
	  type = decode_type (cs, new_c_type, aux);
	  type = lookup_pointer_type (type);
	}
      else if (ISFCN (c_type))
	{
	  type = decode_type (cs, new_c_type, aux);
	  type = lookup_function_type (type);
	}
      else if (ISARY (c_type))
	{
	  int i, n;
	  register unsigned short *dim;
	  struct type *base_type;

	  /* Define an array type.  */
	  /* auxent refers to array, not base type */
	  if (aux->x_sym.x_tagndx == 0)
	    cs->c_nsyms = 1;

	  /* shift the indices down */
	  dim = &aux->x_sym.x_fcnary.x_ary.x_dimen[0];
	  i = 1;
	  n = dim[0];
	  for (i = 0; *dim && i < DIMNUM - 1; i++, dim++)
	    *dim = *(dim + 1);
	  *dim = 0;

	  type = (struct type *)
		    obstack_alloc (symbol_obstack, sizeof (struct type));
	  bzero (type, sizeof (struct type));

	  base_type = decode_type (cs, new_c_type, aux);

	  TYPE_CODE (type) = TYPE_CODE_ARRAY;
	  TYPE_TARGET_TYPE (type) = base_type;
	  TYPE_LENGTH (type) = n * TYPE_LENGTH (base_type);
	}
      return type;
    }

  /* Reference to existing type */
  if (cs->c_nsyms > 1 && aux->x_sym.x_tagndx != 0)
    {
      type = coff_alloc_type (aux->x_sym.x_tagndx);
      return type;
    }

  return decode_base_type (cs, BTYPE (c_type), aux);
}

/* Decode a coff type specifier for function definition;
   return the type that the function returns.  */

static
struct type *
decode_function_type (cs, c_type, aux)
     register struct coff_symbol *cs;
     unsigned int c_type;
     register AUXENT *aux;
{
  if (aux->x_sym.x_tagndx == 0)
    cs->c_nsyms = 1;	/* auxent refers to function, not base type */

  return decode_type (cs, DECREF (c_type), aux);
}

/* basic C types */

static
struct type *
decode_base_type (cs, c_type, aux)
     register struct coff_symbol *cs;
     unsigned int c_type;
     register AUXENT *aux;
{
  struct type *type;

  switch (c_type)
    {
      case T_NULL:
        /* shows up with "void (*foo)();" structure members */
	return builtin_type_void;

#if 0
/* DGUX actually defines both T_ARG and T_VOID to the same value.  */
#ifdef T_ARG
      case T_ARG:
	/* Shows up in DGUX, I think.  Not sure where.  */
	return builtin_type_void;	/* shouldn't show up here */
#endif
#endif /* 0 */

#ifdef T_VOID
      case T_VOID:
	/* Intel 960 COFF has this symbol and meaning.  */
	return builtin_type_void;
#endif

      case T_CHAR:
	return builtin_type_char;

      case T_SHORT:
	return builtin_type_short;

      case T_INT:
	return builtin_type_int;

      case T_LONG:
	return builtin_type_long;

      case T_FLOAT:
	return builtin_type_float;

      case T_DOUBLE:
	return builtin_type_double;

      case T_STRUCT:
	if (cs->c_nsyms != 2)
	  {
	    /* anonymous structure type */
	    type = coff_alloc_type (cs->c_symnum);
	    TYPE_CODE (type) = TYPE_CODE_STRUCT;
	    TYPE_NAME (type) = concat ("struct ", "<opaque>", "");
	    TYPE_LENGTH (type) = 0;
	    TYPE_FIELDS (type) = 0;
	    TYPE_NFIELDS (type) = 0;
	  }
	else
	  {
	    type = read_struct_type (cs->c_symnum,
				    aux->x_sym.x_misc.x_lnsz.x_size,
				    aux->x_sym.x_fcnary.x_fcn.x_endndx);
	  }
	return type;

      case T_UNION:
	if (cs->c_nsyms != 2)
	  {
	    /* anonymous union type */
	    type = coff_alloc_type (cs->c_symnum);
	    TYPE_NAME (type) = concat ("union ", "<opaque>", "");
	    TYPE_LENGTH (type) = 0;
	    TYPE_FIELDS (type) = 0;
	    TYPE_NFIELDS (type) = 0;
	  }
	else
	  {
	    type = read_struct_type (cs->c_symnum,
				    aux->x_sym.x_misc.x_lnsz.x_size,
				    aux->x_sym.x_fcnary.x_fcn.x_endndx);
	  }
	TYPE_CODE (type) = TYPE_CODE_UNION;
	return type;

      case T_ENUM:
	return read_enum_type (cs->c_symnum,
				    aux->x_sym.x_misc.x_lnsz.x_size,
				    aux->x_sym.x_fcnary.x_fcn.x_endndx);

      case T_MOE:
	/* shouldn't show up here */
	break;

      case T_UCHAR:
	return builtin_type_unsigned_char;

      case T_USHORT:
	return builtin_type_unsigned_short;

      case T_UINT:
	return builtin_type_unsigned_int;

      case T_ULONG:
	return builtin_type_unsigned_long;
    }
  printf ("unexpected type %d at symnum %d\n", c_type, cs->c_symnum);
  return builtin_type_void;
}

/* This page contains subroutines of read_type.  */

/* Read the description of a structure (or union type)
   and return an object describing the type.  */

static struct type *
read_struct_type (index, length, lastsym)
     int index;
     int length;
     int lastsym;
{
  struct nextfield
    {
      struct nextfield *next;
      struct field field;
    };

  register struct type *type;
  register struct nextfield *list = 0;
  struct nextfield *new;
  int nfields = 0;
  register int n;
  char *name;
#ifdef NAMES_HAVE_UNDERSCORE
  int offset = 1;
#else
  int offset = 0;
#endif
  struct coff_symbol member_sym;
  register struct coff_symbol *ms = &member_sym;
  SYMENT sub_sym;
  AUXENT sub_aux;
  int done = 0;

  type = coff_alloc_type (index);
  TYPE_CODE (type) = TYPE_CODE_STRUCT;
  TYPE_LENGTH (type) = length;

  while (!done && symnum < lastsym && symnum < nlist_nsyms_global)
    {
      read_one_sym (ms, &sub_sym, &sub_aux);
      name = ms->c_name;
      name = (name[0] == '_' ? name + offset : name);

      switch (ms->c_sclass)
	{
	  case C_MOS:
	  case C_MOU:

	    /* Get space to record the next field's data.  */
	    new = (struct nextfield *) alloca (sizeof (struct nextfield));
	    new->next = list;
	    list = new;

	    /* Save the data.  */
	    list->field.name = savestring (name, strlen (name));
	    list->field.type = decode_type (ms, ms->c_type, &sub_aux);
	    list->field.bitpos = 8 * ms->c_value;
	    list->field.bitsize = 0;
	    nfields++;
	    break;

	  case C_FIELD:

	    /* Get space to record the next field's data.  */
	    new = (struct nextfield *) alloca (sizeof (struct nextfield));
	    new->next = list;
	    list = new;

	    /* Save the data.  */
	    list->field.name = savestring (name, strlen (name));
	    list->field.type = decode_type (ms, ms->c_type, &sub_aux);
	    list->field.bitpos = ms->c_value;
	    list->field.bitsize = sub_aux.x_sym.x_misc.x_lnsz.x_size;
	    nfields++;
	    break;

	  case C_EOS:
	    done = 1;
	    break;
	}
    }
  /* Now create the vector of fields, and record how big it is.  */

  TYPE_NFIELDS (type) = nfields;
  TYPE_FIELDS (type) = (struct field *)
		obstack_alloc (symbol_obstack, sizeof (struct field) * nfields);

  /* Copy the saved-up fields into the field vector.  */

  for (n = nfields; list; list = list->next)
    TYPE_FIELD (type, --n) = list->field;

  return type;
}

/* Read a definition of an enumeration type,
   and create and return a suitable type object.
   Also defines the symbols that represent the values of the type.  */
/* Currently assumes it's sizeof (int) and doesn't use length.  */

static struct type *
read_enum_type (index, length, lastsym)
     int index;
     int length;
     int lastsym;
{
  register struct symbol *sym;
  register struct type *type;
  int nsyms = 0;
  int done = 0;
  struct pending **symlist;
  struct coff_symbol member_sym;
  register struct coff_symbol *ms = &member_sym;
  SYMENT sub_sym;
  AUXENT sub_aux;
  struct pending *osyms, *syms;
  register int n;
  char *name;
#ifdef NAMES_HAVE_UNDERSCORE
  int offset = 1;
#else
  int offset = 0;
#endif

  type = coff_alloc_type (index);
  if (within_function)
    symlist = &local_symbols;
  else
    symlist = &file_symbols;
  osyms = *symlist;

  while (!done && symnum < lastsym && symnum < nlist_nsyms_global)
    {
      read_one_sym (ms, &sub_sym, &sub_aux);
      name = ms->c_name;
      name = (name[0] == '_' ? name + offset : name);

      switch (ms->c_sclass)
	{
	  case C_MOE:
	    sym = (struct symbol *) xmalloc (sizeof (struct symbol));
	    bzero (sym, sizeof (struct symbol));

	    SYMBOL_NAME (sym) = savestring (name, strlen (name));
	    SYMBOL_CLASS (sym) = LOC_CONST;
	    SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
	    SYMBOL_VALUE (sym) = ms->c_value;
	    add_symbol_to_list (sym, symlist);
	    nsyms++;
	    break;

	  case C_EOS:
	    /* Sometimes the linker (on 386/ix 2.0.2 at least) screws
	       up the count of how many symbols to read.  So stop
	       on .eos.  */
	    done = 1;
	    break;
	}
    }

  /* Now fill in the fields of the type-structure.  */

  /* FIXME: Should be sizeof (int) on target, not host.  */
  TYPE_LENGTH (type) = sizeof (int);
  TYPE_CODE (type) = TYPE_CODE_ENUM;
  TYPE_NFIELDS (type) = nsyms;
  TYPE_FIELDS (type) = (struct field *)
		obstack_alloc (symbol_obstack, sizeof (struct field) * nsyms);

  /* Find the symbols for the values and put them into the type.
     The symbols can be found in the symlist that we put them on
     to cause them to be defined.  osyms contains the old value
     of that symlist; everything up to there was defined by us.  */

  for (syms = *symlist, n = nsyms; syms != osyms; syms = syms->next)
    {
      SYMBOL_TYPE (syms->symbol) = type;
      TYPE_FIELD_NAME (type, --n) = SYMBOL_NAME (syms->symbol);
      TYPE_FIELD_VALUE (type, n) = 0;
      TYPE_FIELD_BITPOS (type, n) = SYMBOL_VALUE (syms->symbol);
      TYPE_FIELD_BITSIZE (type, n) = 0;
    }
  return type;
}

/* Register our ability to parse symbols for coff BFD files */

static struct sym_fns coff_sym_fns =
{
    /* This assumes that 88kbcs implies TDESC and TDESC implies 88kbcs.
       If that's not true, this can be relaxed, but if it is true,
       it will just cause users grief if we try to read the wrong kind
       of symbol file.  */
#if defined (TDESC)
    "m88kbcs", 8,
#else /* not TDESC */
    "coff", 4,
#endif /* not TDESC */
    coff_new_init, coff_symfile_init,
    coff_symfile_read, coff_symfile_discard
};

void
_initialize_coffread ()
{
  add_symtab_fns(&coff_sym_fns);
}

#endif /* NO_COFF */
