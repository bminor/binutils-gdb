/* Read coff symbol tables and convert to internal format, for GDB.
   Design and support routines derived from dbxread.c, and UMAX COFF
   specific routines written 9/1/87 by David D. Johnson, Brown University.
   Revised 11/27/87 ddj@cs.brown.edu
   Copyright (C) 1987, 1988 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

#include "defs.h"
#include "param.h"
#ifdef COFF_FORMAT
#include "initialize.h"
#include "symtab.h"

#include <a.out.h>
#include <stdio.h>
#include <obstack.h>
#include <sys/param.h>
#include <sys/file.h>

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

extern void free_all_symtabs ();

START_FILE

/* Name of source file whose symbol data we are now processing.
   This comes from a symbol named ".file".  */

static char *last_source_file;

/* Core address of start and end of text of current source file.
   This comes from a ".text" symbol where x_nlinno > 0.  */

static CORE_ADDR cur_src_start_addr;
static CORE_ADDR cur_src_end_addr;

/* End of the text segment of the executable file,
   as found in the symbol _etext.  */

static CORE_ADDR end_of_text_addr;

/* The addresses of the symbol table stream and number of symbols
   of the object file we are reading (as copied into core).  */

static FILE *nlist_stream_global;
static int nlist_nsyms_global;

/* The file and text section headers of the symbol file */

static FILHDR file_hdr;
static SCNHDR text_hdr;

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

/* Chain of typedefs of pointers to empty struct/union types.
   They are chained thru the SYMBOL_VALUE.  */

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

extern CORE_ADDR first_object_file_end;	/* From blockframe.c */

/* File name symbols were loaded from.  */

static char *symfile;

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
      type_vector_length *= 2;
      type_vector = (struct typevector *)
	xrealloc (type_vector, sizeof (struct typevector)
				+ type_vector_length * sizeof (struct type *));
      bzero (&type_vector->type[type_vector_length / 2],
	     type_vector_length * sizeof (struct type *) / 2);
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

static
record_line (line, pc)
     int line;
     CORE_ADDR pc;
{
  /* Make sure line vector is big enough.  */

  if (line_vector_index + 2 >= line_vector_length)
    {
      line_vector_length *= 2;
      line_vector = (struct linetable *)
	xrealloc (line_vector, sizeof (struct linetable)
				+ line_vector_length * sizeof (int));
    }

  /* If this line is not continguous with previous one recorded,
     all lines between subsequent line and current one are same pc.
     Add one item to line vector, and if more than one line skipped, 
     record a line-number entry for it.  */
  if (prev_line_number > 0 && line != prev_line_number + 1)
    line_vector->item[line_vector_index++] = pc;
  if (prev_line_number < 0 || line > prev_line_number + 2)
    line_vector->item[line_vector_index++] = - line;
  prev_line_number = line;

  /* Record the core address of the line.  */
  line_vector->item[line_vector_index++] = pc;
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

  /* Initialize the source file information for this file.  */

  line_vector_index = 0;
  line_vector_length = 1000;
  prev_line_number = -2;	/* Force first line number to be explicit */
  line_vector = (struct linetable *)
    xmalloc (sizeof (struct linetable) + line_vector_length * sizeof (int));
}

/* Save the vital information for use when closing off the current file.
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
}

/* Finish the symbol definitions for one main source file,
   close off all the lexical contexts for that file
   (creating struct block's for them), then make the
   struct sym