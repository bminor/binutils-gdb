/* Read dbx symbol tables and convert to internal format, for GDB.
   Copyright (C) 1986, 1987, 1988 Free Software Foundation, Inc.
   Hacked by Michael Tiemann (tiemann@mcc.com)

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

#include "param.h"

#ifdef READ_DBX_FORMAT

#include <a.out.h>
#include <stab.h>
#include <stdio.h>
#include <obstack.h>
#include <sys/param.h>
#include <sys/file.h>
#include "defs.h"
#include "initialize.h"
#include "symtab.h"

static void add_symbol_to_list ();
static void read_dbx_symtab ();
static void process_one_symbol ();
static struct type *read_type ();
static struct type *read_range_type ();
static struct type *read_enum_type ();
static struct type *read_struct_type ();
static long read_number ();
static void finish_block ();
static struct blockvector *make_blockvector ();
static struct symbol *define_symbol ();
static void start_subfile ();
static int hashname ();
static void hash_symsegs ();

extern struct symtab *read_symsegs ();
extern void free_all_symtabs ();

/* C++ */
static struct type **read_args ();

START_FILE

/* Chain of symtabs made from reading the file's symsegs.
   These symtabs do not go into symtab_list themselves,
   but the information is copied from them when appropriate
   to make the symtabs that will exist permanently.  */

static struct symtab *symseg_chain;

/* Symseg symbol table for the file whose data we are now processing.
   It is one of those in symseg_chain.  Or 0, for a compilation that
   has no symseg.  */

static struct symtab *current_symseg;

/* Name of source file whose symbol data we are now processing.
   This comes from a symbol of type N_SO.  */

static char *last_source_file;

/* Core address of start of text of current source file.
   This too comes from the N_SO symbol.  */

static CORE_ADDR last_source_start_addr;

/* End of the text segment of the executable file,
   as found in the symbol _etext.  */

static CORE_ADDR end_of_text_addr;

/* The list of sub-source-files within the current individual compilation.
   Each file gets its own symtab with its own linetable and associated info,
   but they all share one blockvector.  */

struct subfile
{
  struct subfile *next;
  char *name;
  struct linetable *line_vector;
  int line_vector_length;
  int line_vector_index;
  int prev_line_number;
};

static struct subfile *subfiles;

static struct subfile *current_subfile;

/* The addresses of the symbol table stream and the string table
   of the object file we are reading (as copied into core).  */

static FILE *nlist_stream_global;
static int nlist_size_global;
static char *stringtab_global;

/* The index in nlist_global of the last dbx symbol to be processed.  */

static int symnum;

/* Vector of types defined so far, indexed by their dbx type numbers.
   (In newer sun systems, dbx uses a pair of numbers in parens,
    as in "(SUBFILENUM,NUMWITHINSUBFILE)".  Then these numbers must be
    translated through the type_translations hash table to get
    the index into the type vector.)  */

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

/* Chain of global symbols whose values are not known yet.
   They are chained thru the SYMBOL_VALUE, since we don't
   have the correct data for that slot yet.  */

#define HASHSIZE 127
static struct symbol *global_sym_chain[HASHSIZE];

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

/* Support for Sun changes to dbx symbol format */

/* For each identified header file, we have a table of types defined
   in that header file.

   header_files maps header file names to their type tables.
   It is a vector of n_header_files elements.
   Each element describes one header file.
   It contains a vector of types.

   Sometimes it can happen that the same header file produces
   different results when included in different places.
   This can result from conditionals or from different
   things done before including the file.
   When this happens, there are multiple entries for the file in this table,
   one entry for each distinct set of results.
   The entries are distinguished by the INSTANCE field.
   The INSTANCE field appears in the N_BINCL and N_EXCL symbol table and is
   used to match header-file references to their corresponding data.  */

struct header_file
{
  char *name;			/* Name of header file */
  int instance;			/* Numeric code distinguishing instances
				   of one header file that produced
				   different results when included.
				   It comes from the N_BINCL or N_EXCL.  */
  struct type **vector;		/* Pointer to vector of types */
  int length;			/* Allocated length (# elts) of that vector */
};

static struct header_file *header_files;

static int n_header_files;

static int n_allocated_header_files;

/* Within each object file, various header files are assigned numbers.
   A type is defined or referred to with a pair of numbers
   (FILENUM,TYPENUM) where FILENUM is the number of the header file
   and TYPENUM is the number within that header file.
   TYPENUM is the index within the vector of types for that header file.

   FILENUM == 1 is special; it refers to the main source of the object file,
   and not to any header file.  FILENUM != 1 is interpreted by looking it up
   in the following table, which contains indices in header_files.  */

static int *this_object_header_files;

static int n_this_object_header_files;

static int n_allocated_this_object_header_files;

/* When a header file is getting special overriding definitions
   for one source file, record here the header_files index
   of its normal definition vector.
   At other times, this is -1.  */

static int header_file_prev_index;

/* At the start of reading dbx symbols, allocate our tables.  */

static void
init_header_files ()
{
  n_allocated_header_files = 10;
  header_files = (struct header_file *) xmalloc (10 * sizeof (struct header_file));
  n_header_files = 0;

  n_allocated_this_object_header_files = 10;
  this_object_header_files = (int *) xmalloc (10 * sizeof (int));
}

/* At the end of reading dbx symbols, free our tables.  */

static void
free_header_files ()
{
  register int i;
  for (i = 0; i < n_header_files; i++)
    free (header_files[i].name);
  free (header_files);
  free (this_object_header_files);
}

/* Called at the start of each object file's symbols.
   Clear out the mapping of header file numbers to header files.  */

static void
new_object_header_files ()
{
  /* Leave FILENUM of 0 free for builtin types and this file's types.  */
  n_this_object_header_files = 1;
  header_file_prev_index = -1;
}

/* Add header file number I for this object file
   at the next successive FILENUM.  */

static void
add_this_object_header_file (i)
     int i;
{
  if (n_this_object_header_files == n_allocated_this_object_header_files)
    {
      n_allocated_this_object_header_files *= 2;
      this_object_header_files
	= (int *) xrealloc (this_object_header_files,
			    n_allocated_this_object_header_files * sizeof (int));
    }

  this_object_header_files[n_this_object_header_files++] = i;
}

/* Add to this file an "old" header file, one already seen in
   a previous object file.  NAME is the header file's name.
   INSTANCE is its instance code, to select among multiple
   symbol tables for the same header file.  */

static void
add_old_header_file (name, instance)
     char *name;
     int instance;
{
  register struct header_file *p = header_files;
  register int i;

  for (i = 0; i < n_header_files; i++)
    if (!strcmp (p[i].name, name) && instance == p[i].instance)
      {
	add_this_object_header_file (i);
	return;
      }
  error ("Invalid symbol data: \"repeated\" header file that hasn't been seen before, at symtab pos %d.",
	 symnum);
}

/* Add to this file a "new" header file: definitions for its types follow.
   NAME is the header file's name.
   Most often this happens only once for each distinct header file,
   but not necessarily.  If it happens more than once, INSTANCE has
   a different value each time, and references to the header file
   use INSTANCE values to select among them.

   dbx output contains "begin" and "end" markers for each new header file,
   but at this level we just need to know which files there have been;
   so we record the file when its "begin" is seen and ignore the "end".  */

static void
add_new_header_file (name, instance)
     char *name;
     int instance;
{
  register int i;
  register struct header_file *p = header_files;
  header_file_prev_index = -1;

#if 0
  /* This code was used before I knew about the instance codes.
     My first hypothesis is that it is not necessary now
     that instance codes are handled.  */

  /* Has this header file a previous definition?
     If so, make a new entry anyway so that this use in this source file
     gets a separate entry.  Later source files get the old entry.
     Record here the index of the old entry, so that any type indices
     not previously defined can get defined in the old entry as
     well as in the new one.  */

  for (i = 0; i < n_header_files; i++)
    if (!strcmp (p[i].name, name))
      {
	header_file_prev_index = i;
      }

#endif

  /* Make sure there is room for one more header file.  */

  if (n_header_files == n_allocated_header_files)
    {
      n_allocated_header_files *= 2;
      header_files
	= (struct header_file *) xrealloc (header_files, n_allocated_header_files * sizeof (struct header_file));
    }

  /* Create an entry for this header file.  */

  i = n_header_files++;
  header_files[i].name = name;
  header_files[i].instance = instance;
  header_files[i].length = 10;
  header_files[i].vector
    = (struct type **) xmalloc (10 * sizeof (struct type *));
  bzero (header_files[i].vector, 10 * sizeof (struct type *));

  add_this_object_header_file (i);
}

/* Look up a dbx type-number pair.  Return the address of the slot
   where the type for that number-pair is stored.
   The number-pair is in TYPENUMS.

   This can be used for finding the type associated with that pair
   or for associating a new type with the pair.  */

static struct type **
dbx_lookup_type (typenums)
     int typenums[2];
{
  register int filenum = typenums[0], index = typenums[1];

  if (filenum < 0 || filenum >= n_this_object_header_files)
    error ("Invalid symbol data: type number (%d,%d) out of range at symtab pos %d.",
	   filenum, index, symnum);

  if (filenum == 0)
    {
      /* Type is defined outside of header files.
	 Find it in this object file's type vector.  */
      if (index >= type_vector_length)
	{
	  type_vector_length *= 2;
	  type_vector = (struct typevector *)
	    xrealloc (type_vector, sizeof (struct typevector) + type_vector_length * sizeof (struct type *));
	  bzero (&type_vector->type[type_vector_length / 2],
		 type_vector_length * sizeof (struct type *) / 2);
	}
      return &type_vector->type[index];
    }
  else
    {
      register int real_filenum = this_object_header_files[filenum];
      register struct header_file *f;

      if (real_filenum >= n_header_files)
	abort ();

      f = &header_files[real_filenum];

      if (index >= f->length)
	{
	  f->length *= 2;
	  f->vector = (struct type **)
	    xrealloc (f->vector, f->length * sizeof (struct type *));
	  bzero (&f->vector[f->length / 2],
		 f->length * sizeof (struct type *) / 2);
	}
      return &f->vector[index];
    }
}

/* Make sure there is a type allocated for type numbers TYPENUMS
   and return the type object.
   This can create an empty (zeroed) type object.  */

static struct type *
dbx_alloc_type (typenums)
     int typenums[2];
{
  register struct type **type_addr = dbx_lookup_type (typenums);
  register struct type *type = *type_addr;

  /* If we are referring to a type not known at all yet,
     allocate an empty type for it.
     We will fill it in later if we find out how.  */
  if (type == 0)
    {
      type = (struct type *) obstack_alloc (symbol_obstack,
					    sizeof (struct type));
      bzero (type, sizeof (struct type));
      TYPE_VPTR_FIELDNO (type) = -1;
      *type_addr = type;
    }
  return type;
}

#if 0
static struct type **
explicit_lookup_type (real_filenum, index)
     int real_filenum, index;
{
  register struct header_file *f = &header_files[real_filenum];

  if (index >= f->length)
    {
      f->length *= 2;
      f->vector = (struct type **)
	xrealloc (f->vector, f->length * sizeof (struct type *));
      bzero (&f->vector[f->length / 2],
	     f->length * sizeof (struct type *) / 2);
    }
  return &f->vector[index];
}
#endif

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

  block = (struct block *) obstack_alloc (symbol_obstack,
					  sizeof (struct block) + (i - 1) * sizeof (struct symbol *));

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

  blockvector = (struct blockvector *) obstack_alloc (symbol_obstack, sizeof (struct blockvector) + (i - 1) * sizeof (struct block *));

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
  /* Ignore the dummy line number in libg.o */

  if (line == 0xffff)
    return;

  /* Make sure line vector is big enough.  */

  if (line_vector_index + 1 >= line_vector_length)
    {
      line_vector_length *= 2;
      line_vector = (struct linetable *)
	xrealloc (line_vector,
		  sizeof (struct linetable) + line_vector_length * sizeof (int));
      current_subfile->line_vector = line_vector;
    }

  /* If this line is not continguous with previous one recorded,
     record a line-number entry for it.  */
  if (line != prev_line_number + 1)
    line_vector->item[line_vector_index++] = - line;
  prev_line_number = line;

  /* Record the core address of the line.  */
  line_vector->item[line_vector_index++] = pc;
}

/* Start a new symtab for a new source file.
   This is called when a dbx symbol of type N_SO is seen;
   it indicates the start of data for one original source file.  */

static void
start_symtab (name, start_addr)
     char *name;
     CORE_ADDR start_addr;
{
  register struct symtab *s;

  last_source_file = name;
  last_source_start_addr = start_addr;
  file_symbols = 0;
  global_symbols = 0;
  context_stack = 0;
  within_function = 0;

  new_object_header_files ();

  for (s = symseg_chain; s; s = s->next)
    if (s->ldsymoff == symnum * sizeof (struct nlist))
      break;
  current_symseg = s;
  if (s != 0)
    return;

  type_vector_length = 160;
  type_vector = (struct typevector *) xmalloc (sizeof (struct typevector) + type_vector_length * sizeof (struct type *));
  bzero (type_vector->type, type_vector_length * sizeof (struct type *));

  /* Initialize the list of sub source files with one entry
     for this file (the top-level source file).  */

  subfiles = 0;
  current_subfile = 0;
  start_subfile (name);
}

/* Handle an N_SOL symbol, which indicates the start of
   code that came from an included (or otherwise merged-in)
   source file with a different name.  */

static void
start_subfile (name)
     char *name;
{
  register struct subfile *subfile;

  /* Save the current subfile's line vector data.  */

  if (current_subfile)
    {
      current_subfile->line_vector_index = line_vector_index;
      current_subfile->line_vector_length = line_vector_length;
      current_subfile->prev_line_number = prev_line_number;
    }

  /* See if this subfile is already known as a subfile of the
     current main source file.  */

  for (subfile = subfiles; subfile; subfile = subfile->next)
    {
      if (!strcmp (subfile->name, name))
	{
	  line_vector = subfile->line_vector;
	  line_vector_index = subfile->line_vector_index;
	  line_vector_length = subfile->line_vector_length;
	  prev_line_number = subfile->prev_line_number;
	  current_subfile = subfile;
	  return;
	}
    }

  /* This subfile is not known.  Add an entry for it.  */

  line_vector_index = 0;
  line_vector_length = 1000;
  prev_line_number = -2;	/* Force first line number to be explicit */
  line_vector = (struct linetable *)
    xmalloc (sizeof (struct linetable) + line_vector_length * sizeof (int));

  /* Make an entry for this subfile in the list of all subfiles
     of the current main source file.  */

  subfile = (struct subfile *) xmalloc (sizeof (struct subfile));
  subfile->next = subfiles;
  subfile->name = savestring (name, strlen (name));
  subfile->line_vector = line_vector;
  subfiles = subfile;
  current_subfile = subfile;
}

/* Finish the symbol definitions for one main source file,
   close off all the lexical contexts for that file
   (creating struct block's for them), then make the struct symtab
   for that file and put it in the list of all such.

   END_ADDR is the address of the end of the file's text.  */

static void
end_symtab (end_addr)
     CORE_ADDR end_addr;
{
  register struct symtab *symtab;
  register struct context_stack *cstk;
  register struct blockvector *blockvector;
  register struct subfile *subfile;
  register struct linetable *lv;
  struct subfile *nextsub;

  if (current_symseg != 0)
    {
      last_source_file = 0;
      current_symseg = 0;
      return;
    }

  /* Finish the lexical context of the last function in the file.  */

  if (context_stack)
    {
      cstk = context_stack;
      /* Make a block for the local symbols within.  */
      finish_block (cstk->name, &local_symbols, cstk->old_blocks,
		    cstk->start_addr, end_addr);
      free (cstk);
    }

  /* Finish defining all the blocks of this symtab.  */
  finish_block (0, &file_symbols, 0, last_source_start_addr, end_addr);
  finish_block (0, &global_symbols, 0, last_source_start_addr, end_addr);
  blockvector = make_blockvector ();

  current_subfile->line_vector_index = line_vector_index;

  /* Now create the symtab objects proper, one for each subfile.  */
  /* (The main file is one of them.)  */

  for (subfile = subfiles; subfile; subfile = nextsub)
    {
      symtab = (struct symtab *) xmalloc (sizeof (struct symtab));
      symtab->free_ptr = 0;

      /* Fill in its components.  */
      symtab->blockvector = blockvector;
      type_vector->length = type_vector_length;
      symtab->typevector = type_vector;
      symtab->free_code = free_linetable;
      if (subfile->next == 0)
	symtab->free_ptr = (char *) type_vector;

      symtab->filename = subfile->name;
      lv = subfile->line_vector;
      lv->nitems = subfile->line_vector_index;
      symtab->linetable = (struct linetable *)
	xrealloc (lv, sizeof (struct linetable) + lv->nitems * sizeof (int));
      symtab->nlines = 0;
      symtab->line_charpos = 0;

      /* Link the new symtab into the list of such.  */
      symtab->next = symtab_list;
      symtab_list = symtab;

      nextsub = subfile->next;
      free (subfile);
    }

  type_vector = 0;
  type_vector_length = -1;
  line_vector = 0;
  line_vector_length = -1;
  last_source_file = 0;
}

#ifdef N_BINCL

/* Handle the N_BINCL and N_EINCL symbol types
   that act like N_SOL for switching source files
   (different subfiles, as we call them) within one object file,
   but using a stack rather than in an arbitrary order.  */

struct subfile_stack
{
  struct subfile_stack *next;
  char *name;
  int prev_index;
};

struct subfile_stack *subfile_stack;

static void
push_subfile ()
{
  register struct subfile_stack *tem
    = (struct subfile_stack *) xmalloc (sizeof (struct subfile_stack));

  tem->next = subfile_stack;
  subfile_stack = tem;
  if (current_subfile == 0 || current_subfile->name == 0)
    abort ();
  tem->name = current_subfile->name;
  tem->prev_index = header_file_prev_index;
}

static char *
pop_subfile ()
{
  register char *name;
  register struct subfile_stack *link = subfile_stack;

  if (link == 0)
    abort ();

  name = link->name;
  subfile_stack = link->next;
  header_file_prev_index = link->prev_index;
  free (link);

  return name;
}
#endif /* Have N_BINCL */

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

static void
init_misc_functions ()
{
  misc_count = 0;
  misc_bunch = 0;
  misc_bunch_index = MISC_BUNCH_SIZE;
}

static void
record_misc_function (name, address)
     char *name;
     CORE_ADDR address;
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

static void
discard_misc_bunches ()
{
  register struct misc_bunch *next;

  while (misc_bunch)
    {
      next = misc_bunch->next;
      free (misc_bunch);
      misc_bunch = next;
    }
}

static void
condense_misc_bunches ()
{
  register int i, j;
  register struct misc_bunch *bunch;
#ifdef NAMES_HAVE_UNDERSCORE
  int offset = 1;
#else
  int offset = 0;
#endif

  misc_function_vector
    = (struct misc_function *)
      xmalloc (misc_count * sizeof (struct misc_function));

  j = 0;
  bunch = misc_bunch;
  while (bunch)
    {
      for (i = 0; i < misc_bunch_index; i++)
	{
	  misc_function_vector[j] = bunch->contents[i];
	  misc_function_vector[j].name
	    = concat (misc_function_vector[j].name
		      + (misc_function_vector[j].name[0] == '_' ? offset : 0),
		      "", "");
	  j++;
	}
      bunch = bunch->next;
      misc_bunch_index = MISC_BUNCH_SIZE;
    }

  misc_function_count = j;

  /* Sort the misc functions by address.  */

  qsort (misc_function_vector, j, sizeof (struct misc_function),
	 compare_misc_functions);
}

/* Call sort_syms to sort alphabetically
   the symbols of each block of each symtab.  */

static int
compare_symbols (s1, s2)
     struct symbol **s1, **s2;
{
  /* Names that are less should come first.  */
  register int namediff = strcmp (SYMBOL_NAME (*s1), SYMBOL_NAME (*s2));
  if (namediff != 0) return namediff;
  /* For symbols of the same name, registers should come first.  */
  return ((SYMBOL_CLASS (*s2) == LOC_REGISTER)
	  - (SYMBOL_CLASS (*s1) == LOC_REGISTER));
}

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
	  qsort (&BLOCK_SYM (b, 0), BLOCK_NSYMS (b),
		 sizeof (struct symbol *), compare_symbols);
	}
    }
}

/* This is the symbol-file command.  Read the file, analyze its symbols,
   and add a struct symtab to symtab_list.  */

void
symbol_file_command (name)
     char *name;
{
  register int desc;
  struct exec hdr;
  struct nlist *nlist;
  char *stringtab;
  long buffer;
  register int val;
  extern void close ();
  struct cleanup *old_chain;
  struct symtab *symseg;

  dont_repeat ();

  if (name == 0)
    {
      if (symtab_list && !query ("Discard symbol table? ", 0))
	error ("Not confirmed.");
      free_all_symtabs ();
      return;
    }

  if (symtab_list && !query ("Load new symbol table from \"%s\"? ", name))
    error ("Not confirmed.");

  if (symfile)
    free (symfile);
  symfile = 0;

  {
    char *absolute_name;
    desc = openp (getenv ("PATH"), 1, name, O_RDONLY, 0, &absolute_name);
    if (desc < 0)
      perror_with_name (name);
    else
      name = absolute_name;
  }

  old_chain = make_cleanup (close, desc);
  make_cleanup (free_current_contents, &name);

  val = myread (desc, &hdr, sizeof hdr);
  if (val < 0)
    perror_with_name (name);

  if (N_BADMAG (hdr))
    error ("File \"%s\" not in executable format.", name);

  if (hdr.a_syms == 0)
    {
      free_all_symtabs ();
      printf ("%s does not have a symbol-table.\n", name);
      fflush (stdout);
      return;
    }

  printf ("Reading symbol data from %s...", name);
  fflush (stdout);

  /* Now read the string table, all at once.  */
  val = lseek (desc, N_SYMOFF (hdr) + hdr.a_syms, 0);
  if (val < 0)
    perror_with_name (name);
  val = myread (desc, &buffer, sizeof buffer);
  if (val < 0)
    perror_with_name (name);
  stringtab = (char *) alloca (buffer);
  if (stringtab == NULL)
    {
      free_all_symtabs ();
      printf("%s: symbol table too large for alloca: %d bytes.\n", name, 
	buffer);
      fflush (stdout);
      return;
    }
  bcopy (&buffer, stringtab, sizeof buffer);
  val = myread (desc, stringtab + sizeof buffer, buffer - sizeof buffer);
  if (val < 0)
    perror_with_name (name);

  /* Throw away the old symbol table.  */

  free_all_symtabs ();

#ifdef READ_GDB_SYMSEGS
  /* That puts us at the symsegs.  Read them.  */
  symseg_chain = read_symsegs (desc, name);
  hash_symsegs ();

  /* Free the symtabs made by read_symsegs, but not their contents,
     which have been copied into symtabs on symtab_list.  */
  for (symseg = symseg_chain; symseg; symseg = symseg->next)
    {
      int i;
      struct sourcevector *sv = (struct sourcevector *) symseg->linetable;

      for (i = 0; i < sv->length; i++)
	{
	  int j;
	  struct source *source = sv->source[i];
	  struct symtab *sp1
	    = (struct symtab *) xmalloc (sizeof (struct symtab));

	  bcopy (symseg, sp1, sizeof (struct symtab));
	  sp1->filename = savestring (source->name, strlen (source->name));
	  sp1->linetable = &source->contents;
	  sp1->free_code = free_nothing;
	  sp1->free_ptr = (i == 0) ? (char *) symseg : 0;

	  sp1->next = symtab_list;
	  symtab_list = sp1;
	}
    }
#else
  /* Where people are using the 4.2 ld program, must not check for
     symsegs, because that ld puts randonm garbage at the end of
     the output file and that would trigger an error message.  */
  symseg_chain = 0;
#endif

  /* Position to read the symbol table.  Do not read it all at once. */
  val = lseek (desc, N_SYMOFF (hdr), 0);
  if (val < 0)
    perror_with_name (name);

  init_misc_functions ();
  make_cleanup (discard_misc_bunches, 0);
  init_header_files ();
  make_cleanup (free_header_files, 0);

  /* Now that the symbol table data of the executable file are all in core,
     process them and define symbols accordingly.  Closes desc.  */

  read_dbx_symtab (desc, stringtab, hdr.a_syms / sizeof (struct nlist));

  /* Sort symbols alphabetically within each block.  */

  sort_syms ();

  /* Go over the misc functions and install them in vector.  */

  condense_misc_bunches ();

  /* Don't allow char * to have a typename (else would get caddr_t.)  */

  TYPE_NAME (lookup_pointer_type (builtin_type_char)) = 0;

  /* Make a default for file to list.  */

  select_source_symtab (symtab_list);

  symfile = savestring (name, strlen (name));

  do_cleanups (old_chain);

  /* Free the symtabs made by read_symsegs, but not their contents,
     which have been copied into symtabs on symtab_list.  */
  while (symseg_chain)
    {
      register struct symtab *s = symseg_chain->next;
      free (symseg_chain);
      symseg_chain = s;
    }

  printf ("done.\n");
  fflush (stdout);
}

/* Return name of file symbols were loaded from, or 0 if none..  */

char *
get_sym_file ()
{
  return symfile;
}

/* Given pointers to a a.out symbol table in core containing  dbx style data,
   analyze them and create struct symtab's describing the symbols.
   NLISTLEN is the number of symbols in the symbol table.
   We read them one at a time using stdio.
   All symbol names are given as offsets relative to STRINGTAB.  */

static void
read_dbx_symtab (desc, stringtab, nlistlen)
     int desc;
     register char *stringtab;
     register int nlistlen;
{
  FILE *stream = fdopen (desc, "r");
  struct nlist buf;
  register char *namestring;
  register struct symbol *sym, *prev;
  int hash;
  int num_object_files = 0;
  struct cleanup *old_chain;

#ifdef N_BINCL
  subfile_stack = 0;
#endif

  old_chain = make_cleanup (free_all_symtabs, 0);
  nlist_stream_global = stream;
  nlist_size_global = nlistlen;
  stringtab_global = stringtab;
  last_source_file = 0;
  bzero (global_sym_chain, sizeof global_sym_chain);

  for (symnum = 0; symnum < nlistlen; symnum++)
    {
      QUIT;	/* allow this to be interruptable */
      fread (&buf, sizeof buf, 1, stream);
      namestring = buf.n_un.n_strx ? buf.n_un.n_strx + stringtab : "";
      if (buf.n_type & N_STAB)
	process_one_symbol (buf.n_type, buf.n_desc,
			    buf.n_value, namestring);
      /* A static text symbol whose name ends in ".o"
	 can only mean the start of another object file.
	 So end the symtab of the source file we have been processing.
	 This is how we avoid counting the libraries as part
	 or the last source file.
	 Also this way we find end of first object file (crt0).  */
      else if (buf.n_type == N_TEXT
	       && !strcmp (namestring + strlen (namestring) - 2, ".o"))
	{
	  if (num_object_files++ == 1)
	    first_object_file_end = buf.n_value;
	  if (last_source_file)
	    end_symtab (buf.n_value);
	}
      else if (buf.n_type & N_EXT || buf.n_type == N_TEXT)
	{
	  int used_up = 0;

	  /* Record the location of _etext.  */
	  if (buf.n_type == (N_TEXT | N_EXT)
	      && !strcmp (namestring, "_etext"))
	    end_of_text_addr = buf.n_value;

	  /* Global symbol: see if we came across a dbx definition
	     for a corresponding symbol.  If so, store the value.
	     Remove syms from the chain when their values are stored,
	     but search the whole chain, as there may be several syms
	     from different files with the same name.  */
	  if (buf.n_type & N_EXT)
	    {
	      prev = 0;
#ifdef NAMES_HAVE_UNDERSCORE
	      hash = hashname (namestring + 1);
#else /* not NAMES_HAVE_UNDERSCORE */
	      hash = hashname (namestring);
#endif /* not NAMES_HAVE_UNDERSCORE */
	      for (sym = global_sym_chain[hash];
		   sym;)
		{
		  if (
#ifdef NAMES_HAVE_UNDERSCORE
		      *namestring == '_'
		      && namestring[1] == SYMBOL_NAME (sym)[0]
		      &&
		      !strcmp (namestring + 2, SYMBOL_NAME (sym) + 1)
#else /* NAMES_HAVE_UNDERSCORE */
		      namestring[0] == SYMBOL_NAME (sym)[0]
		      &&
		      !strcmp (namestring + 1, SYMBOL_NAME (sym) + 1)
#endif /* NAMES_HAVE_UNDERSCORE */
		      )
		    {
		      if (prev)
			SYMBOL_VALUE (prev) = SYMBOL_VALUE (sym);
		      else
			global_sym_chain[hash]
			  = (struct symbol *) SYMBOL_VALUE (sym);
		      SYMBOL_VALUE (sym) = buf.n_value;
		      if (prev)
			sym = (struct symbol *) SYMBOL_VALUE (prev);
		      else
			sym = global_sym_chain[hash];

		      used_up = 1;
		    }
		  else
		    {
		      prev = sym;
		      sym = (struct symbol *) SYMBOL_VALUE (sym);
		    }
		}
	    }

	  /* Defined global or text symbol: record as a misc function
	     if it didn't give its address to a debugger symbol above.  */
	  if (buf.n_type <= (N_TYPE | N_EXT)
	      && buf.n_type != N_EXT
	      && ! used_up)
	    record_misc_function (namestring, buf.n_value);
	}
    }

  if (last_source_file)
    end_symtab (end_of_text_addr);

  fclose (stream);
  discard_cleanups (old_chain);
}

/* dbx allows the text of a symbol name to be continued into the
   next symbol name!  When such a continuation is encountered
   (a \ at the end of the text of a name)
   call this function to get the continuation.  */

static char *
next_symbol_text ()
{
  struct nlist buf;
  fread (&buf, sizeof buf, 1, nlist_stream_global);
  symnum++;
  return buf.n_un.n_strx + stringtab_global;
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

/* Put all appropriate global symbols in the symseg data
   onto the hash chains so that their addresses will be stored
   when seen later in loader global symbols.  */

static void
hash_symsegs ()
{
  /* Look at each symbol in each block in each symseg symtab.  */
  struct symtab *s;
  for (s = symseg_chain; s; s = s->next)
    {
      register int n;
      for (n = BLOCKVECTOR_NBLOCKS (BLOCKVECTOR (s)) - 1; n >= 0; n--)
	{
	  register struct block *b = BLOCKVECTOR_BLOCK (BLOCKVECTOR (s), n);
	  register int i;
	  for (i = BLOCK_NSYMS (b) - 1; i >= 0; i--)
	    {
	      register struct symbol *sym = BLOCK_SYM (b, i);

	      /* Put the symbol on a chain if its value is an address
		 that is figured out by the loader.  */

	      if (SYMBOL_CLASS (sym) == LOC_EXTERNAL)
		{
		  register int hash = hashname (SYMBOL_NAME (sym));
		  SYMBOL_VALUE (sym) = (int) global_sym_chain[hash];
		  global_sym_chain[hash] = sym;
		  SYMBOL_CLASS (sym) = LOC_STATIC;
		}
	    }
	}
    }
}

static void
process_one_symbol (type, desc, value, name)
     int type, desc;
     CORE_ADDR value;
     char *name;
{
  register struct context_stack *new;
  
  /* Something is wrong if we see real data before
     seeing a source file name.  */
  
#ifdef N_NSYMS
  if (type == N_NSYMS) return;
#endif

  if (type != N_SO && last_source_file == 0)
    error ("Invalid symbol data: does not start by identifying a source file.");

  switch (type)
    {
    case N_FUN:
    case N_FNAME:
      /* Either of these types of symbols indicates the start of
	 a new function.  We must process its "name" normally for dbx,
	 but also record the start of a new lexical context, and possibly
	 also the end of the lexical context for the previous function.  */
      new = context_stack;
      within_function = 1;
      if (new)
	{
	  /* Make a block for the local symbols within.  */
	  finish_block (new->name, &local_symbols, new->old_blocks,
			new->start_addr, value);
	}
      else
	{
	  new = (struct context_stack *) xmalloc (sizeof (struct context_stack));
	  new->next = 0;
	  new->depth = -1;
	  context_stack = new;
	}
      new->locals = 0;
      new->old_blocks = pending_blocks;
      new->start_addr = value;
      new->name = define_symbol (value, name, desc);
      local_symbols = 0;
      break;

    case N_LBRAC:
      /* This "symbol" just indicates the start of an inner lexical
	 context within a function.  */
      new = (struct context_stack *) xmalloc (sizeof (struct context_stack));
      new->depth = desc;
      new->next = context_stack;
      context_stack = new;
      new->locals = local_symbols;
      new->old_blocks = pending_blocks;
      new->start_addr = value;
      new->name = 0;
      local_symbols = 0;
      break;

    case N_RBRAC:
      /* This "symbol" just indicates the end of an inner lexical
	 context that was started with N_RBRAC.  */
      new = context_stack;
      if (new == 0 || desc != new->depth)
	error ("Invalid symbol data: N_LBRAC/N_RBRAC symbol mismatch, symtab pos %d.", symnum);
      local_symbols = new->locals;
      context_stack = new->next;
      /* If this is not the outermost LBRAC...RBRAC pair in the
	 function, its local symbols preceded it, and are the ones
	 just recovered from the context stack.  Defined the block for them.

	 If this is the outermost LBRAC...RBRAC pair, there is no
	 need to do anything; leave the symbols that preceded it
	 to be attached to the function's own block.  */
      if (local_symbols && context_stack->next)
	{
	  /* Muzzle a compiler bug that makes end > start.  */
	  if (new->start_addr > value)
	    new->start_addr = value;
	  /* Make a block for the local symbols within.  */
	  finish_block (0, &local_symbols, new->old_blocks,
			new->start_addr + last_source_start_addr,
			value + last_source_start_addr);
	}
      free (new);
      break;

    case N_FN:
      /* This kind of symbol supposedly indicates the start
	 of an object file.  In fact this type does not appear.  */
      break;

    case N_SO:
      /* This type of symbol indicates the start of data
	 for one source file.
	 Finish the symbol table of the previous source file
	 (if any) and start accumulating a new symbol table.  */
      if (last_source_file)
	end_symtab (value);
      start_symtab (name, value);
      break;

    case N_SOL:
      /* This type of symbol indicates the start of data for
	 a sub-source-file, one whose contents were copied or
	 included in the compilation of the main source file
	 (whose name was given in the N_SO symbol.)  */
      start_subfile (name);
      break;

#ifdef N_BINCL
    case N_BINCL:
      push_subfile ();
      add_new_header_file (name, value);
      start_subfile (name);
      break;

    case N_EINCL:
      start_subfile (pop_subfile ());
      break;

    case N_EXCL:
      add_old_header_file (name, value);
      break;
#endif /* have N_BINCL */

    case N_SLINE:
      /* This type of "symbol" really just records
	 one line-number -- core-address correspondence.
	 Enter it in the line list for this symbol table.  */
      record_line (desc, value);
      break;

    default:
      if (name)
	define_symbol (value, name, desc);
    }
}

/************************ READ_ADDL_SYM() ***********************************/

static void
read_addl_syms (desc, stringtab, nlistlen, text_addr, text_size)
     int desc;
     register char *stringtab;
     register int nlistlen;
     unsigned text_addr;
     int text_size;
{
  FILE *stream = fdopen (desc, "r");
  struct nlist buf;
  register char *namestring;
  register struct symbol *sym, *prev;
  int hash;
  int num_object_files = 0;

#ifdef N_BINCL
  subfile_stack = 0;
#endif

  nlist_stream_global = stream;
  nlist_size_global = nlistlen;
  stringtab_global = stringtab;
  last_source_file = 0;
  bzero (global_sym_chain, sizeof global_sym_chain);

  for (symnum = 0; symnum < nlistlen; symnum++)
    {
      unsigned type;

      fread (&buf, sizeof buf, 1, stream);

      type = buf.n_type & N_TYPE;
      if( (type == N_TEXT) || (type == N_DATA) || (type == N_BSS) )
	{
	  buf.n_value += text_addr;        
	}			/* Right?? ###### */


      namestring = buf.n_un.n_strx ? buf.n_un.n_strx + stringtab : "";
      if (buf.n_type & N_STAB)
	process_one_symbol (buf.n_type, buf.n_desc,
			    buf.n_value, namestring);
      /* A static text symbol whose name ends in ".o"
	 can only mean the start of another object file.
	 So end the symtab of the source file we have been processing.
	 This is how we avoid counting the libraries as part
	 or the last source file.
	 Also this way we find end of first object file (crt0).  */
      else if (buf.n_type == N_TEXT
	       && !strcmp (namestring + strlen (namestring) - 2, ".o"))
	{
	  if (num_object_files++ == 1)
	    first_object_file_end = buf.n_value;
	  if (last_source_file)
	    {
	      end_symtab (buf.n_value);	/* All this not used##### */
	    } 
	}
      else if (buf.n_type & N_EXT || buf.n_type == N_TEXT)
	{
	  int used_up = 0;

	  /* Record the location of _etext.  */
	  if (buf.n_type == (N_TEXT | N_EXT)
	      && !strcmp (namestring, "_etext"))
	    {
	      end_of_text_addr = buf.n_value;
	    }

	  /* Global symbol: see if we came across a dbx definition
	     for a corresponding symbol.  If so, store the value.
	     Remove syms from the chain when their values are stored,
	     but search the whole chain, as there may be several syms
	     from different files with the same name.  */
	  if (buf.n_type & N_EXT)
	    {
	      prev = 0;
#ifdef NAMES_HAVE_UNDERSCORE
	      hash = hashname (namestring + 1);
#else				/* not NAMES_HAVE_UNDERSCORE */
	      hash = hashname (namestring);
#endif				/* not NAMES_HAVE_UNDERSCORE */
	      for (sym = global_sym_chain[hash];
		   sym;)
		{
		  if (
#ifdef NAMES_HAVE_UNDERSCORE
		      *namestring == '_'
		      && namestring[1] == SYMBOL_NAME (sym)[0]
		      &&
		      !strcmp (namestring + 2, SYMBOL_NAME (sym) + 1)
#else				/* NAMES_HAVE_UNDERSCORE */
		      namestring[0] == SYMBOL_NAME (sym)[0]
		      &&
		      !strcmp (namestring + 1, SYMBOL_NAME (sym) + 1)
#endif				/* NAMES_HAVE_UNDERSCORE */
		      )
		    {
		      if (prev)
			SYMBOL_VALUE (prev) = SYMBOL_VALUE (sym);
		      else
			global_sym_chain[hash]
			  = (struct symbol *) SYMBOL_VALUE (sym);
		      SYMBOL_VALUE (sym) = buf.n_value;
		      if (prev)
			sym = (struct symbol *) SYMBOL_VALUE (prev);
		      else
			sym = global_sym_chain[hash];

		      used_up = 1;
		    }
		  else
		    {
		      prev = sym;
		      sym = (struct symbol *) SYMBOL_VALUE (sym);
		    }
		}
	    }

	  /* Defined global or text symbol: record as a misc function
	     if it didn't give its address to a debugger symbol above.  */
	  if (buf.n_type <= (N_TYPE | N_EXT)
	      && buf.n_type != N_EXT
	      && ! used_up)
	    record_misc_function (namestring, buf.n_value);
	}
    }

  if (last_source_file)
    { 
      end_symtab (text_addr + text_size);
    }

  fclose (stream);
}

/***************************** CONDENSE_ADDL_MISC_BUNCHES *******************/

static void
condense_addl_misc_bunches ()
{ 
  register int i, j;
  register struct misc_bunch *bunch;
#ifdef NAMES_HAVE_UNDERSCORE
  int offset = 1;
#else
  int offset = 0;
#endif

  misc_function_vector
    = (struct misc_function *)  xrealloc (misc_function_vector,
					  (misc_count + misc_function_count) * sizeof (struct misc_function));

  j = misc_function_count;
  bunch = misc_bunch;
  while (bunch)
    {
      for (i = 0; i < misc_bunch_index; i++)
	{
	  misc_function_vector[j] = bunch->contents[i];
	  misc_function_vector[j].name
	    = concat (misc_function_vector[j].name
		      + (misc_function_vector[j].name[0] == '_' ? offset : 0),
		      "", "");
	  j++;
	}
      bunch = bunch->next;
      misc_bunch_index = MISC_BUNCH_SIZE;
    }

  misc_function_count += misc_count;

  /* Sort the misc functions by address.  */

  qsort (misc_function_vector, misc_function_count,
	 sizeof (struct misc_function),  compare_misc_functions);
}

/**************************** ADD_FILE_COMMAND() ****************************/
/* This function allows the addition of incrementally linked object files.
   Useful for debugging `sun_kick'. */

void
add_file_command (arg_string)
     char* arg_string;
{ 
  register int desc;
  struct exec hdr;
  struct nlist *nlist;
  char *stringtab;
  long buffer;
  register int val;
  extern void close ();
  struct cleanup *old_chain;
  char* name;
  unsigned text_addr;

  for( ; *arg_string == ' '; arg_string++ );
  name = arg_string;
  for( ; *arg_string != ' ' ; arg_string++ );
  *arg_string++ = (char) 0;
  for( ; *arg_string == ' '; arg_string++ );
  text_addr = (unsigned) atoi(arg_string);

  printf("filename \"%s\", and text_addr = 0x%x\n", name, text_addr );

  dont_repeat ();

  if (name == 0)
    {
      if (symtab_list && !query ("Discard symbol table? ", 0))
	error ("Not confirmed.");
      free_all_symtabs ();
      return;
    }

  /*    if (symtab_list && !query ("Add new symbols from \"%s\"? ", name))
	error ("Not confirmed.");
	*/
  {
    char *absolute_name;
    desc = openp (getenv ("PATH"), 1, name, O_RDONLY, 0, &absolute_name);
    if (desc < 0)
      perror_with_name (name);
    else
      name = absolute_name;
  }

  old_chain = make_cleanup (close, desc);
  make_cleanup (free_current_contents, &name);

  val = myread (desc, &hdr, sizeof hdr);
  if (val < 0)
    perror_with_name (name);

  if (N_BADMAG (hdr))
    error ("File \"%s\" has a bad header.", name);

  if (hdr.a_syms == 0)
    {
      printf ("%s does not have a symbol-table.\n", name);
      fflush (stdout);
      return;
    }

  /* Now read the string table, all at once.  */
  val = lseek (desc, N_SYMOFF (hdr) + hdr.a_syms, 0);
  if (val < 0)
    perror_with_name (name);
  val = myread (desc, &buffer, sizeof buffer);
  if (val < 0)
    perror_with_name (name);
  stringtab = (char *) alloca (buffer);
  bcopy (&buffer, stringtab, sizeof buffer);
  val = myread (desc, stringtab + sizeof buffer, buffer - sizeof buffer);
  if (val < 0)
    perror_with_name (name);

  /* That puts us at the symsegs.  Read them. ########## Also need other
     changes if they exist. */


  /* Position to read the symbol table.  Do not read it all at once. */
  val = lseek (desc, N_SYMOFF (hdr), 0);
  if (val < 0)
    perror_with_name (name);

  printf ("Reading symbol data from %s...", name);
  fflush (stdout);

  init_misc_functions ();
  make_cleanup (discard_misc_bunches, 0);
  init_header_files ();
  make_cleanup (free_header_files, 0);

  read_addl_syms (desc, stringtab, hdr.a_syms / sizeof(struct nlist)
		  ,text_addr, hdr.a_text) ;

  /* Sort symbols alphabetically within each block.  */

  sort_syms ();

  /* Go over the all misc functions and install them in vector.  */

  condense_addl_misc_bunches ();

  /* Don't allow char * to have a typename (else would get caddr_t.)  */

  TYPE_NAME (lookup_pointer_type (builtin_type_char)) = 0;

  /* Make a default for file to list.  */

  select_source_symtab (symtab_list);

  do_cleanups (old_chain);

  /* Free the symtabs made by read_symsegs, but not their contents,
     which have been copied into symtabs on symtab_list.  */
  while (symseg_chain)
    {
      register struct symtab *s = symseg_chain->next;
      free (symseg_chain);
      symseg_chain = s;
    }

  printf ("done.\n");
  fflush (stdout);
}

static struct symbol *
define_symbol (value, string, desc)
     int value;
     char *string;
     int desc;
{
  register struct symbol *sym
    = (struct symbol *) obstack_alloc (symbol_obstack, sizeof (struct symbol));
  char *p = (char *) index (string, ':');
  int deftype;
  register int i;

  bzero (sym, sizeof (struct symbol));
  SYMBOL_NAME (sym) = obstack_copy0 (symbol_obstack, string, p - string);
  p++;
  /* Determine the type of name being defined.  */
  if ((*p >= '0' && *p <= '9') || *p == '(')
    deftype = 'l';
  else
    deftype = *p++;

  /* c is a special case, not followed by a type-number.
     SYMBOL:c=iVALUE for an integer constant symbol.
     SYMBOL:c=rVALUE for a floating constant symbol.  */
  if (deftype == 'c')
    {
      if (*p++ != '=')
	error ("Invalid symbol data at symtab pos %d.", symnum);
      switch (*p++)
	{
	case 'r':
	  {
	    double d = atof (p);
	    char *value;

	    SYMBOL_TYPE (sym) = builtin_type_double;
	    value = (char *) obstack_alloc (symbol_obstack, sizeof (double));
	    bcopy (&d, value, sizeof (double));
	    SYMBOL_VALUE_BYTES (sym) = value;
	    SYMBOL_CLASS (sym) = LOC_CONST;
	  }
	  break;
	case 'i':
	  {
	    SYMBOL_TYPE (sym) = builtin_type_int;
	    SYMBOL_VALUE (sym) = atoi (p);
	    SYMBOL_CLASS (sym) = LOC_CONST_BYTES;
	  }
	  break;
	default:
	  error ("Invalid symbol data at symtab pos %d.", symnum);
	}
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &file_symbols);
      return sym;
    }

  /* Now usually comes a number that says which data type,
     and possibly more stuff to define the type
     (all of which is handled by read_type)  */

  if (deftype == 'p' && *p == 'F')
    /* pF is a two-letter code that means a function parameter in Fortran.
       The type-number specifies the type of the return value.
       Translate it into a pointer-to-function type.  */
    {
      p++;
      SYMBOL_TYPE (sym)
	= lookup_pointer_type (lookup_function_type (read_type (&p)));
    }
  else
    SYMBOL_TYPE (sym) = read_type (&p);

  switch (deftype)
    {
    case 'f':
      SYMBOL_CLASS (sym) = LOC_BLOCK;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &file_symbols);
      break;

    case 'F':
      SYMBOL_CLASS (sym) = LOC_BLOCK;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &global_symbols);
      break;

    case 'G':
      /* For a class G (global) symbol, it appears that the
	 value is not correct.  It is necessary to search for the
	 corresponding linker definition to find the value.
	 These definitions appear at the end of the namelist.  */
      i = hashname (SYMBOL_NAME (sym));
      SYMBOL_VALUE (sym) = (int) global_sym_chain[i];
      global_sym_chain[i] = sym;
      SYMBOL_CLASS (sym) = LOC_STATIC;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &global_symbols);
      break;

      /* This case is faked by a conditional above,
	 when there is no code letter in the dbx data.
	 Dbx data never actually contains 'l'.  */
    case 'l':
      SYMBOL_CLASS (sym) = LOC_LOCAL;
      SYMBOL_VALUE (sym) = value;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      break;

    case 'p':
      SYMBOL_CLASS (sym) = LOC_ARG;
      SYMBOL_VALUE (sym) = value;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      /* DESC == 0 implies compiled with GCC.
	 In this case, if it says `short', believe it.  */
      if (desc == 0)
	break;
      /* If PCC says a parameter is a short or a char,
	 it is really an int.  */
      if (SYMBOL_TYPE (sym) == builtin_type_char
	  || SYMBOL_TYPE (sym) == builtin_type_short)
	SYMBOL_TYPE (sym) = builtin_type_int;
      else if (SYMBOL_TYPE (sym) == builtin_type_unsigned_char
	       || SYMBOL_TYPE (sym) == builtin_type_unsigned_short)
	SYMBOL_TYPE (sym) = builtin_type_unsigned_int;
      break;

    case 'r':
      SYMBOL_CLASS (sym) = LOC_REGISTER;
      SYMBOL_VALUE (sym) = value;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      break;

    case 'S':
      /* Static symbol at top level of file */
      SYMBOL_CLASS (sym) = LOC_STATIC;
      SYMBOL_VALUE (sym) = value;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &file_symbols);
      break;

    case 't':
      SYMBOL_CLASS (sym) = LOC_TYPEDEF;
      SYMBOL_VALUE (sym) = value;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      if (TYPE_NAME (SYMBOL_TYPE (sym)) == 0
	  && (TYPE_FLAGS (SYMBOL_TYPE (sym)) & TYPE_FLAG_PERM) == 0)
	TYPE_NAME (SYMBOL_TYPE (sym)) = concat (SYMBOL_NAME (sym), "", "");
      /* C++ vagaries: we may have a type which is derived from
	 a base type which did not have its name defined when the
	 derived class was output.  We fill in the derived class's
	 base part member's name here in that case.  */
      else if ((TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_STRUCT
		|| TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_UNION)
	       && TYPE_BASECLASS (SYMBOL_TYPE (sym))
	       && TYPE_FIELD_NAME (SYMBOL_TYPE (sym), 0) == 0)
	TYPE_FIELD_NAME (SYMBOL_TYPE (sym), 0) = TYPE_NAME (TYPE_BASECLASS (SYMBOL_TYPE (sym)));

      add_symbol_to_list (sym, &file_symbols);
      break;

    case 'T':
      SYMBOL_CLASS (sym) = LOC_TYPEDEF;
      SYMBOL_VALUE (sym) = value;
      SYMBOL_NAMESPACE (sym) = STRUCT_NAMESPACE;
      if (TYPE_NAME (SYMBOL_TYPE (sym)) == 0
	  && (TYPE_FLAGS (SYMBOL_TYPE (sym)) & TYPE_FLAG_PERM) == 0)
	TYPE_NAME (SYMBOL_TYPE (sym))
	  = concat ("",
		    (TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_ENUM
		     ? "enum "
		     : (TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_STRUCT
			? "struct " : "union ")),
		    SYMBOL_NAME (sym));
      add_symbol_to_list (sym, &file_symbols);
      break;

    case 'V':
    case 'v':
      /* Static symbol of local scope */
      SYMBOL_CLASS (sym) = LOC_STATIC;
      SYMBOL_VALUE (sym) = value;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      break;

    default:
      error ("Invalid symbol data: unknown symbol-type code `%c' at symtab pos %d.", deftype, symnum);
    }
  return sym;
}

/* Read a number by which a type is referred to in dbx data,
   or perhaps read a pair (FILENUM, TYPENUM) in parentheses.
   Just a single number N is equivalent to (0,N).
   Return the two numbers by storing them in the vector TYPENUMS.
   TYPENUMS will then be used as an argument to dbx_lookup_type.  */

static void
read_type_number (pp, typenums)
     register char **pp;
     register int *typenums;
{
  if (**pp == '(')
    {
      (*pp)++;
      typenums[0] = read_number (pp, ',');
      typenums[1] = read_number (pp, ')');
    }
  else
    {
      typenums[0] = 0;
      typenums[1] = read_number (pp, 0);
    }
}

/* Read a dbx type reference or definition;
   return the type that is meant.
   This can be just a number, in which case it references
   a type already defined and placed in type_vector.
   Or the number can be followed by an =, in which case
   it means to define a new type according to the text that
   follows the =.  */

static
struct type *
read_type (pp)
     register char **pp;
{
  register struct type *type = 0;
  register int n;
  struct type *type1;
  int typenums[2];
  int xtypenums[2];

  read_type_number (pp, typenums);

  /* Detect random reference to type not yet defined.
     Allocate a type object but leave it zeroed.  */
  if (**pp != '=')
    return dbx_alloc_type (typenums);

  *pp += 2;
  switch ((*pp)[-1])
    {
    case 'x':
      type = dbx_alloc_type (typenums);
      /* Set the type code according to the following letter.  */
      switch ((*pp)[0])
	{
	case 's':
	  TYPE_CODE (type) = TYPE_CODE_STRUCT;
	  break;
	case 'u':
	  TYPE_CODE (type) = TYPE_CODE_UNION;
	  break;
	case 'e':
	  TYPE_CODE (type) = TYPE_CODE_ENUM;
	  break;
	}
      /* Skip the name the cross-ref points to.  */
      /* Note: for C++, the cross reference may be to a base type which
	 has not yet been seen.  In this case, we skip to the comma,
	 which will mark the end of the base class name.  (The ':'
	 at the end of the base class name will be skipped as well.)  */
      *pp = (char *) index (*pp, ',');
      /* Just allocate the type and leave it zero if nothing known */
      return dbx_alloc_type (typenums);

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '(':
      (*pp)--;
      read_type_number (pp, xtypenums);
      type = *dbx_lookup_type (xtypenums);
      if (type == 0)
	type = builtin_type_void;
      *dbx_lookup_type (typenums) = type;
      break;
      
    case '*':
      type = dbx_alloc_type (typenums);
      smash_to_pointer_type (type, read_type (pp));
      break;

    case '@':
      {
	struct type *domain = read_type (pp);
	char c;
	struct type *ptrtype;

	if (*(*pp)++ != ',')
	  error ("invalid member pointer data format, at symtab pos %d.",
		 symnum);

	ptrtype = read_type (pp);
	type = dbx_alloc_type (typenums);
	smash_to_member_pointer_type (type, domain, ptrtype);
      }
      break;

    case '&':
      type = dbx_alloc_type (typenums);
      smash_to_reference_type (type, read_type (pp));
      break;

    case 'f':
      type = dbx_alloc_type (typenums);
      smash_to_function_type (type, read_type (pp));
      break;

    case 'r':
      type = read_range_type (pp, typenums);
      *dbx_lookup_type (typenums) = type;
      break;

    case 'e':
      type = dbx_alloc_type (typenums);
      type = read_enum_type (pp, type);
      *dbx_lookup_type (typenums) = type;
      break;

    case 's':
      type = dbx_alloc_type (typenums);
      type = read_struct_type (pp, type);
      break;

    case 'u':
      type = dbx_alloc_type (typenums);
      type = read_struct_type (pp, type);
      TYPE_CODE (type) = TYPE_CODE_UNION;
      break;

    case 'a':
      /* Define an array type.  */
      type = dbx_alloc_type (typenums);

      /* dbx expresses array types in terms of a range type for the index,
	 and that range type is specified right inside the array type spec
	 making ar1;MIN;MAX;VALTYPE  */
      if (!strncmp (*pp, "r1;0;", 5))
	(*pp) += 5;
      else if (!strncmp (*pp, "r(0,1);0;", 9))
	(*pp) += 9;
      else break;

      TYPE_CODE (type) = TYPE_CODE_ARRAY;
      /* In Fortran, an upper bound may be T... meaning a parameter specifies
	 the length of the data.  In this case, just pretend the bound is 1.
	 This happens only for array parameters, which are really passed
	 as pointers anyway, and we will translate them into such.  */
      if (**pp == 'T')
	{
	  n = 1;
	  while (**pp != ';')
	    (*pp)++;
	}
      else
	n = read_number (pp, ';') + 1;
      TYPE_TARGET_TYPE (type) = read_type (pp);
      TYPE_LENGTH (type) = TYPE_LENGTH (TYPE_TARGET_TYPE (type)) * n;
      break;

    default:
      error ("Invalid symbol data: unrecognized type-code `%c' at symtab pos %d.",
	     (*pp)[-1], symnum);
    }

  if (type == 0)
    abort ();

#if 0
  /* If this is an overriding temporary alteration for a header file's
     contents, and this type number is unknown in the global definition,
     put this type into the global definition at this type number.  */
  if (header_file_prev_index >= 0)
    {
      register struct type **tp
        = explicit_lookup_type (header_file_prev_index, typenums[1]);
      if (*tp == 0)
	*tp = type;
    }
#endif
  return type;
}

/* This page contains subroutines of read_type.  */

/* Read the description of a structure (or union type)
   and return an object describing the type.  */

static struct type *
read_struct_type (pp, type)
     char **pp;
     register struct type *type;
{
  struct nextfield
    {
      struct nextfield *next;
      int visibility;
      struct field field;
    };

  struct next_fnfield
    {
      struct next_fnfield *next;
      int visibility;
      struct fn_field fn_field;
    };

  struct next_fnfieldlist
    {
      struct next_fnfieldlist *next;
      struct fn_fieldlist fn_fieldlist;
    };

  register struct nextfield *list = 0;
  struct nextfield *new;
  int totalsize;
  char *name;
  register char *p;
  int nfields = 0;
  register int n;

  register struct next_fnfieldlist *mainlist = 0;
  int nfn_fields = 0;
  struct type *baseclass = NULL;
  int read_possible_virtual_info = 0;

  TYPE_CODE (type) = TYPE_CODE_STRUCT;

  /* First comes the total size in bytes.  */

  TYPE_LENGTH (type) = read_number (pp, 0);

  /* C++: Now, if the class is a derived class, then the next character
     will be a '!', followed by the type of the base class. Allocate
     pretend that that base class is a sub-structure of this one,
     with its field name being the type name of the derived class. This
     cannot cause a naming conflict, since field names cannot be
     type names. This will magically recurse itself to gound terms
     when all is read and done. */
  if (**pp == '!')
    {
      *pp += 1;

      switch (*(*pp)++)
	{
	case '0':
	  break;
	case '1':
	  TYPE_VIA_PROTECTED (type) = 1;
	  break;
	case '2':
	  TYPE_VIA_PUBLIC (type) = 1;
	  break;
	default:
	  error ("Invalid symbol data: bad visibility format at symtab pos %d.",
		 symnum);
	}

      if (**pp == '\\') *pp = next_symbol_text ();
      new = (struct nextfield *) alloca (sizeof (struct nextfield));
      new->next = list;
      list = new;

      baseclass = read_type (pp);
      list->field.type = baseclass;
      list->field.name = TYPE_NAME (baseclass);
      *pp += 1;			/* skip ',' */
      list->field.bitpos = 0;
      list->field.bitsize = 0;	/* this should be an unpacked field! */
      nfields++;
    }

  /* Now come the fields, as NAME:?TYPENUM,BITPOS,BITSIZE; for each one.
     At the end, we see a semicolon instead of a field.

     In C++, this may wind up being NAME:?TYPENUM:PHYSNAME; for
     a static field.

     The `?' is a placeholder for one of '+' (public visibility),
     '0' (protected visibility), and '-' (private visibility).  */

  while (**pp != ';')
    {
      int visibility;

      /* Check for and handle cretinous dbx symbol name continuation!  */
      if (**pp == '\\') *pp = next_symbol_text ();

      /* Get space to record the next field's data.  */
      new = (struct nextfield *) alloca (sizeof (struct nextfield));
      new->next = list;
      list = new;

      /* Read the data.  */
      p = *pp;
      while (*p != ':') p++;
      list->field.name = savestring (*pp, p - *pp);

      /* Check to see if we have hit the methods yet.  */
      if (p[1] == ':')
	break;

      *pp = p + 1;

      /* This means we have a visibility for a field coming.  */
      if (**pp == '/')
	{
	  switch (*++*pp)
	    {
	    case '0':
	      visibility = 0;
	      *pp += 1;
	      break;

	    case '1':
	      visibility = 1;
	      *pp += 1;
	      break;

	    case '2':
	      visibility = 2;
	      *pp += 1;
	      break;
	    }
	}
      /* else normal dbx-style format.  */

      list->field.type = read_type (pp);
      if (**pp == ':')
	{
	  list->field.bitpos = (long)-1;
	  p = ++(*pp);
	  while (*p != ';') p++;
	  list->field.bitsize = (long) savestring (*pp, p - *pp);
	  *pp = p + 1;
	  nfields++;
	  continue;
	}
      else if (**pp != ',')
	error ("Invalid symbol data: bad structure-type format at symtab pos %d.",
	       symnum);
      (*pp)++;			/* Skip the comma.  */
      list->field.bitpos = read_number (pp, ',');
      list->field.bitsize = read_number (pp, ';');
      /* Detect an unpacked field and mark it as such.
	 dbx gives a bit size for all fields.
	 Also detect forward refs to structures and unions,
	 and treat enums as if they had the width of ints.  */
      if ((list->field.bitsize == 8 * TYPE_LENGTH (list->field.type)
	   || TYPE_CODE (list->field.type) == TYPE_CODE_STRUCT
	   || TYPE_CODE (list->field.type) == TYPE_CODE_UNION
	   || (TYPE_CODE (list->field.type) == TYPE_CODE_ENUM
	       && list->field.bitsize == 8 * TYPE_LENGTH (builtin_type_int)))
	  &&
	  list->field.bitpos % 8 == 0)
	list->field.bitsize = 0;
      nfields++;
    }

  /* Now come the method fields, as NAME::methods
     where each method is of the form TYPENUM,ARGS,...:PHYSNAME;
     At the end, we see a semicolon instead of a field.

     For the case of overloaded operators, the format is
     OPERATOR::*.methods, where OPERATOR is the string "operator",
     `*' holds the place for an operator name (such as `+=')
     and `.' marks the end of the operator name.  */
  if (p[1] == ':')
    {
      /* Now, read in the methods.  To simplify matters, we
	 "unread" the name that has been read, so that we can
	 start from the top.  */

      p = *pp;

      /* chill the list of fields: the last entry (at the head)
         is a partially constructed entry which we now scrub.  */
      list = list->next;

      /* For each list of method lists... */
      do
	{
	  int i;
	  struct next_fnfield *sublist = 0;
	  struct fn_field *fn_fields = 0;
	  int length = 0;
	  struct next_fnfieldlist *new_mainlist =
	    (struct next_fnfieldlist *)alloca (sizeof (struct next_fnfieldlist));

	  /* read in the name.  */
	  while (*p != ':') p++;
	  if ((*pp)[0] == 'o' && (*pp)[1] == 'p' && (*pp)[2] == '$')
	    {
	      static char opname[32] = "operator ";
	      char *o = opname + 9;

	      /* Skip past '::'.  */
	      p += 2;
	      while (*p != '.')
		*o++ = *p++;
	      new_mainlist->fn_fieldlist.name = savestring (opname, o - opname);
	      /* Skip past '.'  */
	      *pp = p + 1;
	    }
	  else
	    {
	      i = 0;
	      new_mainlist->fn_fieldlist.name = savestring (*pp, p - *pp);
	      /* Skip past '::'.  */
	      *pp = p + 2;
	    }

	  do
	    {
	      struct next_fnfield *new_sublist =
		(struct next_fnfield *)alloca (sizeof (struct next_fnfield));

	      /* Check for and handle cretinous dbx symbol name continuation!  */
	      if (**pp == '\\') *pp = next_symbol_text ();

	      new_sublist->fn_field.type = read_type (pp);
	      new_sublist->fn_field.args = read_args (pp, ':');
	      p = *pp;
	      while (*p != ';') p++;
	      new_sublist->fn_field.physname = savestring (*pp, p - *pp);
	      *pp = p + 1;
	      new_sublist->visibility = *(*pp)++ - '0';
	      if (**pp == '\\') *pp = next_symbol_text ();

	      if (*(*pp)++ == '*')
		new_sublist->fn_field.voffset = read_number (pp, ';') + 1;
	      else
		new_sublist->fn_field.voffset = 0;

	      new_sublist->next = sublist;
	      sublist = new_sublist;
	      length++;
	    }
	  while (**pp != ';');

	  *pp += 1;

	  new_mainlist->fn_fieldlist.fn_fields =
	    (struct fn_field *) obstack_alloc (symbol_obstack,
					       sizeof (struct fn_field) * length);
	  TYPE_FN_PRIVATE_BITS (new_mainlist->fn_fieldlist) =
	    (int *) obstack_alloc (symbol_obstack,
				   sizeof (int) * (1 + (length >> 5)));

	  TYPE_FN_PROTECTED_BITS (new_mainlist->fn_fieldlist) =
	    (int *) obstack_alloc (symbol_obstack,
				   sizeof (int) * (1 + (length >> 5)));

	  for (i = length; sublist; sublist = sublist->next)
	    {
	      new_mainlist->fn_fieldlist.fn_fields[--i] = sublist->fn_field;
	      if (sublist->visibility == 0)
		B_SET (new_mainlist->fn_fieldlist.private_fn_field_bits, i);
	      else if (sublist->visibility == 1)
		B_SET (new_mainlist->fn_fieldlist.protected_fn_field_bits, i);
	    }

	  new_mainlist->fn_fieldlist.length = length;
	  new_mainlist->next = mainlist;
	  mainlist = new_mainlist;
	  nfn_fields++;
	}
      while (**pp != ';');

      *pp += 2;

      if (**pp == '~')
	{
	  read_possible_virtual_info = 1;
	  *pp += 1;
	}
      if (**pp == '-')
	{
	  TYPE_HAS_DESTRUCTOR (type) = 1;
	  TYPE_HAS_CONSTRUCTOR (type) = 1;
	  *pp += 1;
	}
      else if (**pp == '+')
	{
	  TYPE_HAS_CONSTRUCTOR (type) = 1;
	  *pp += 1;
	}
    }
  else *pp += 1;

  /* Now create the vector of fields, and record how big it is.  */

  TYPE_NFIELDS (type) = nfields;
  TYPE_FIELDS (type) = (struct field *) obstack_alloc (symbol_obstack,
						       sizeof (struct field) * nfields);
  TYPE_FIELD_PRIVATE_BITS (type) =
    (int *) obstack_alloc (symbol_obstack,
			   sizeof (int) * (1 + (nfields >> 5)));
  TYPE_FIELD_PROTECTED_BITS (type) =
    (int *) obstack_alloc (symbol_obstack,
			   sizeof (int) * (1 + (nfields >> 5)));

  TYPE_NFN_FIELDS (type) = nfn_fields;
  TYPE_NFN_FIELDS_TOTAL (type) = nfn_fields;
  if (baseclass)
    TYPE_NFN_FIELDS_TOTAL (type) += TYPE_NFN_FIELDS_TOTAL (baseclass);

  TYPE_FN_FIELDLISTS (type) =
    (struct fn_fieldlist *) obstack_alloc (symbol_obstack,
					   sizeof (struct fn_fieldlist) * nfn_fields);

  /* Copy the saved-up fields into the field vector.  */

  for (n = nfields; list; list = list->next)
    {
      TYPE_FIELD (type, --n) = list->field;
      if (list->visibility == 0)
	SET_TYPE_FIELD_PRIVATE (type, n);
      else if (list->visibility == 1)
	SET_TYPE_FIELD_PROTECTED (type, n);
    }

  for (n = nfn_fields; mainlist; mainlist = mainlist->next)
    TYPE_FN_FIELDLISTS (type)[--n] = mainlist->fn_fieldlist;

  TYPE_BASECLASS (type) = baseclass;

  if (read_possible_virtual_info)
    {
      /* Read either a '%' or the final ';'.  */
      if (*(*pp)++ == '%')
	{
	  /* Now we must record the virtual function table pointer's
	     field information.  */

	  struct type *t;
	  int i;

	  t = read_type (pp);
	  p = (*pp)++;
	  while (*p != ';') p++;
	  TYPE_VPTR_BASETYPE (type) = t;
	  if (type == t)
	    {
	      if (TYPE_FIELD_NAME (t, 0) == 0)
		TYPE_VPTR_FIELDNO (type) = i = 0;
	      else for (i = TYPE_NFIELDS (t) - 1; i >= 0; --i)
		if (! strncmp (TYPE_FIELD_NAME (t, i), *pp,
			       strlen (TYPE_FIELD_NAME (t, i))))
		  {
		    TYPE_VPTR_FIELDNO (type) = i;
		    break;
		  }
	      if (i < 0)
		error ("virtual function table field not found");
	    }
	  else
	    TYPE_VPTR_FIELDNO (type) = TYPE_VPTR_FIELDNO (TYPE_BASECLASS (type));
	  *pp = p;
	}
      else
	{
	  TYPE_VPTR_BASETYPE (type) = 0;
	  TYPE_VPTR_FIELDNO (type) = -1;
	}
    }
  else
    {
      TYPE_VPTR_BASETYPE (type) = 0;
      TYPE_VPTR_FIELDNO (type) = -1;
    }

  return type;
}

/* Read a definition of an enumeration type,
   and create and return a suitable type object.
   Also defines the symbols that represent the values of the type.  */

static struct type *
read_enum_type (pp, type)
     register char **pp;
     register struct type *type;
{
  register char *p;
  char *name;
  register long n;
  register struct symbol *sym;
  int nsyms = 0;
  struct pending **symlist;
  struct pending *osyms, *syms;

  if (within_function)
    symlist = &local_symbols;
  else
    symlist = &file_symbols;
  osyms = *symlist;

  /* Read the value-names and their values.
     The input syntax is NAME:VALUE,NAME:VALUE, and so on.
     A semicolon instead of a NAME means the end.  */
  while (**pp && **pp != ';')
    {
      /* Check for and handle cretinous dbx symbol name continuation!  */
      if (**pp == '\\')	*pp = next_symbol_text ();

      p = *pp;
      while (*p != ':') p++;
      name = savestring (*pp, p - *pp);
      *pp = p + 1;
      n = read_number (pp, ',');
      
      sym = (struct symbol *) xmalloc (sizeof (struct symbol));
      bzero (sym, sizeof (struct symbol));
      SYMBOL_NAME (sym) = name;
      SYMBOL_CLASS (sym) = LOC_CONST;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      SYMBOL_VALUE (sym) = n;
      add_symbol_to_list (sym, symlist);
      nsyms++;
    }

  (*pp)++;			/* Skip the semicolon.  */

  /* Now fill in the fields of the type-structure.  */

  TYPE_LENGTH (type) = sizeof (int);
  TYPE_CODE (type) = TYPE_CODE_ENUM;
  TYPE_NFIELDS (type) = nsyms;
  TYPE_FIELDS (type) = (struct field *) obstack_alloc (symbol_obstack, sizeof (struct field) * nsyms);

  /* Find the symbols for the values and put them into the type.
     The symbols can be found in the symlist that we put them on
     to cause them to be defined.  osyms contains the old value
     of that symlist; everything up to there was defined by us.  */

  for (syms = *symlist, n = nsyms; syms != osyms; syms = syms->next)
    {
      SYMBOL_TYPE (syms->symbol) = type;
      TYPE_FIELD_NAME (type, --n) = SYMBOL_NAME (syms->symbol);
      TYPE_FIELD_VALUE (type, n) = SYMBOL_VALUE (syms->symbol);
      TYPE_FIELD_BITPOS (type, n) = 0;
      TYPE_FIELD_BITSIZE (type, n) = 0;
    }

  return type;
}
  
static struct type *
read_range_type (pp, typenums)
     char **pp;
     int typenums[2];
{
  char *errp = *pp;
  int rangenums[2];
  int n1, n2, n3;

  /* First comes a type we are a subrange of.
     In practice it is usually 0, 1 or the type being defined.  */
  read_type_number (pp, rangenums);
  n1 = rangenums[1];

  /* A semicolon should now follow; skip it.  */
  if (**pp == ';')
    (*pp)++;

  /* The remaining two operands are usually lower and upper bounds
     of the range.  But in some special cases they mean something else.  */
  n2 = read_number (pp, ';');
  n3 = read_number (pp, ';');

  /* A type defined as a subrange of itself, with bounds both 0, is void.  */
  if (rangenums[0] == typenums[0] && rangenums[1] == typenums[1]
      && n2 == 0 && n3 == 0)
    return builtin_type_void;

  /* If n3 is zero and n2 is not, we want a floating type,
     and n2 is the width in bytes.

     Fortran programs appear to use this for complex types also,
     and they give no way to distinguish between double and single-complex!
     We don't have complex types, so we would lose on all fortran files!
     So return type `double' for all of those.  It won't work right
     for the complex values, but at least it makes the file loadable.  */

  if (n3 == 0 && n2 > 0)
    {
      if (n2 == sizeof (float))
	return builtin_type_float;
      return builtin_type_double;
    }

  /* If the upper bound is -1, it must really be an unsigned int.  */

  else if (n2 == 0 && n3 == -1)
    {
      if (sizeof (int) == sizeof (long))
	return builtin_type_unsigned_int;
      else
	return builtin_type_unsigned_long;
    }

  /* Detect unsigned subranges of int.  Int is normally 1.
     Note that `char' is usually given bounds of 0 to 127,
     and would therefore appear unsigned; but it is described
     as a subrange of itself, so we reject it here.  */

  else if (n2 == 0 && n1 == 1)
    {
      /* an unsigned type */
      if (n3 == (1 << (8 * sizeof (int))) - 1)
	return builtin_type_unsigned_int;
      if (n3 == (1 << (8 * sizeof (short))) - 1)
	return builtin_type_unsigned_short;
      if (n3 == (1 << (8 * sizeof (char))) - 1)
	return builtin_type_unsigned_char;
    }
  else
    {
      /* a signed type */
      if (n3 == (1 << (8 * sizeof (int) - 1)) - 1)
	return builtin_type_int;
      if (n3 == (1 << (8 * sizeof (long) - 1)) - 1)
	 return builtin_type_long;
      if (n3 == (1 << (8 * sizeof (short) - 1)) - 1)
	return builtin_type_short;
      if (n3 == (1 << (8 * sizeof (char) - 1)) - 1)
	return builtin_type_char;
    }
  error ("Invalid symbol data: range type spec %s at symtab pos %d.",
	 errp - 1, symnum);
}

/* Read a number from the string pointed to by *PP.
   The value of *PP is advanced over the number.
   If END is nonzero, the character that ends the
   number must match END, or an error happens;
   and that character is skipped if it does match.
   If END is zero, *PP is left pointing to that character.  */

static long
read_number (pp, end)
     char **pp;
     int end;
{
  register char *p = *pp;
  register long n = 0;
  register int c;
  int sign = 1;

  /* Handle an optional leading minus sign.  */

  if (*p == '-')
    {
      sign = -1;
      p++;
    }

  /* Read the digits, as far as they go.  */

  while ((c = *p++) >= '0' && c <= '9')
    {
      n *= 10;
      n += c - '0';
    }
  if (end)
    {
      if (c != end)
	error ("Invalid symbol data: invalid character \\%03o at symbol pos %d.", c, symnum);
    }
  else
    --p;

  *pp = p;
  return n * sign;
}

/* Read in an argument list. This is a list of types. It is terminated with
   a ':', FYI. Return the list of types read in. */
static struct type **
read_args (pp, end)
     char **pp;
     int end;
{
  struct type *types[1024], **rval; /* allow for fns of 1023 parameters */
  int n = 0;

  while (**pp != end)
    {
      if (**pp != ',')
	error ("Invalid argument list: no ',', at symtab pos %d", symnum);
      *pp += 1;
      types[n++] = read_type (pp);
    }
  *pp += 1;			/* get past `end' (the ':' character) */

  if (n == 1)
    {
      rval = (struct type **) xmalloc (2 * sizeof (struct type *));
    }
  else
    {
      rval = (struct type **) xmalloc (n * sizeof (struct type *));
    }
  bcopy (types, rval, n * sizeof (struct type *));
  return rval;
}

/* This function is really horrible, but to avoid it, there would need
   to be more filling in of forward references.  */
int
fill_in_vptr_fieldno (type)
     struct type *type;
{
  if (TYPE_VPTR_FIELDNO (type) < 0)
    TYPE_VPTR_FIELDNO (type) = fill_in_vptr_fieldno (TYPE_BASECLASS (type));
  return TYPE_VPTR_FIELDNO (type);
}

static
initialize ()
{
  symfile = 0;

  add_com ("symbol-file", class_files, symbol_file_command,
	   "Load symbol table (in dbx format) from executable file FILE.");

  add_com ("add-file", class_files, add_file_command,
           "Load the symbols from FILE, assuming its codes is at TEXT_START.") ;
}

END_FILE

#endif /* READ_DBX_FORMAT */
