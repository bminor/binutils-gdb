/* Read dbx symbol tables and convert to internal format, for GDB.
   Copyright (C) 1986-1991 Free Software Foundation, Inc.

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

/* Symbol read-in occurs in two phases:
   1.  A scan (read_dbx_symtab()) of the entire executable, whose sole
       purpose is to make a list of symbols (partial symbol table)
       which will cause symbols
       to be read in if referenced.  This scan happens when the
       "symbol-file" command is given (symbol_file_command()).
   1a. The "add-file" command.  Similar to #1.
   2.  Full read-in of symbols.  (dbx_psymtab_to_symtab()).  This happens
       when a symbol in a file for which symbols have not yet been
       read in is referenced.  */

/* There used to be some PROFILE_TYPES code in this file which counted
   the number of occurances of various symbols.  I'd suggest instead:
     nm -ap foo | awk 'print $5' | sort | uniq -c
   to print how many of each n_type, or something like
     nm -ap foo | awk '$5 == "LSYM" {print $6 $7 $8 $9 $10 $11}' | \
     awk 'BEGIN {FS=":"}
     {print substr($2,1,1)}' | sort | uniq -c
   to print the number of each kind of symbol descriptor (i.e. the letter
   after ':').  */

#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "param.h"

#ifdef USG
#include <sys/types.h>
#include <fcntl.h>
#define L_SET 0
#define L_INCR 1
#endif

#include "a.out.gnu.h"		
#include "stab.gnu.h"		/* We always use GNU stabs, not native, now */
#include <ctype.h>

#ifndef NO_GNU_STABS
/*
 * Define specifically gnu symbols here.
 */

/* The following type indicates the definition of a symbol as being
   an indirect reference to another symbol.  The other symbol
   appears as an undefined reference, immediately following this symbol.

   Indirection is asymmetrical.  The other symbol's value will be used
   to satisfy requests for the indirect symbol, but not vice versa.
   If the other symbol does not have a definition, libraries will
   be searched to find a definition.  */
#ifndef N_INDR
#define N_INDR 0xa
#endif

/* The following symbols refer to set elements.
   All the N_SET[ATDB] symbols with the same name form one set.
   Space is allocated for the set in the text section, and each set
   element's value is stored into one word of the space.
   The first word of the space is the length of the set (number of elements).

   The address of the set is made into an N_SETV symbol
   whose name is the same as the name of the set.
   This symbol acts like a N_DATA global symbol
   in that it can satisfy undefined external references.  */

#ifndef N_SETA
#define	N_SETA	0x14		/* Absolute set element symbol */
#endif				/* This is input to LD, in a .o file.  */

#ifndef N_SETT
#define	N_SETT	0x16		/* Text set element symbol */
#endif				/* This is input to LD, in a .o file.  */

#ifndef N_SETD
#define	N_SETD	0x18		/* Data set element symbol */
#endif				/* This is input to LD, in a .o file.  */

#ifndef N_SETB
#define	N_SETB	0x1A		/* Bss set element symbol */
#endif				/* This is input to LD, in a .o file.  */

/* Macros dealing with the set element symbols defined in a.out.h */
#define	SET_ELEMENT_P(x)	((x)>=N_SETA&&(x)<=(N_SETB|N_EXT))
#define TYPE_OF_SET_ELEMENT(x)	((x)-N_SETA+N_ABS)

#ifndef N_SETV
#define N_SETV	0x1C		/* Pointer to set vector in data area.  */
#endif				/* This is output from LD.  */

#ifndef N_WARNING
#define N_WARNING 0x1E		/* Warning message to print if file included */
#endif				/* This is input to ld */

#endif /* NO_GNU_STABS */

#include <obstack.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "symtab.h"
#include "breakpoint.h"
#include "command.h"
#include "target.h"
#include "gdbcore.h"		/* for bfd stuff */
#include "liba.out.h"	 	/* FIXME Secret internal BFD stuff for a.out */
#include "symfile.h"

struct dbx_symfile_info {
  asection *text_sect;		/* Text section accessor */
  int symcount;			/* How many symbols are there in the file */
  char *stringtab;		/* The actual string table */
  int stringtab_size;		/* Its size */
  off_t symtab_offset;		/* Offset in file to symbol table */
  int desc;			/* File descriptor of symbol file */
};

extern void qsort ();
extern double atof ();
extern struct cmd_list_element *cmdlist;

extern void symbol_file_command ();

/* Forward declarations */

static void add_symbol_to_list ();
static void read_dbx_symtab ();
static void init_psymbol_list ();
static void process_one_symbol ();
static struct type *read_type ();
static struct type *read_range_type ();
static struct type *read_enum_type ();
static struct type *read_struct_type ();
static struct type *read_array_type ();
static long read_number ();
static void finish_block ();
static struct blockvector *make_blockvector ();
static struct symbol *define_symbol ();
static void start_subfile ();
static int hashname ();
static struct pending *copy_pending ();
static void fix_common_block ();
static void add_undefined_type ();
static void cleanup_undefined_types ();
static void scan_file_globals ();
static void read_ofile_symtab ();
static void dbx_psymtab_to_symtab ();

/* C++ */
static struct type **read_args ();

static const char vptr_name[] = { '_','v','p','t','r',CPLUS_MARKER };
static const char vb_name[] =   { '_','v','b',CPLUS_MARKER };

/* Macro to determine which symbols to ignore when reading the first symbol
   of a file.  Some machines override this definition. */
#ifndef IGNORE_SYMBOL
/* This code is used on Ultrix systems.  Ignore it */
#define IGNORE_SYMBOL(type)  (type == (int)N_NSYMS)
#endif

/* Macro for name of symbol to indicate a file compiled with gcc. */
#ifndef GCC_COMPILED_FLAG_SYMBOL
#define GCC_COMPILED_FLAG_SYMBOL "gcc_compiled."
#endif

/* Convert stab register number (from `r' declaration) to a gdb REGNUM.  */

#ifndef STAB_REG_TO_REGNUM
#define STAB_REG_TO_REGNUM(VALUE) (VALUE)
#endif

/* Define this as 1 if a pcc declaration of a char or short argument
   gives the correct address.  Otherwise assume pcc gives the
   address of the corresponding int, which is not the same on a
   big-endian machine.  */

#ifndef BELIEVE_PCC_PROMOTION
#define BELIEVE_PCC_PROMOTION 0
#endif

/* Nonzero means give verbose info on gdb action.  From main.c.  */
extern int info_verbose;

/* Name of source file whose symbol data we are now processing.
   This comes from a symbol of type N_SO.  */

static char *last_source_file;

/* Core address of start of text of current source file.
   This too comes from the N_SO symbol.  */

static CORE_ADDR last_source_start_addr;

/* The entry point of a file we are reading.  */
CORE_ADDR entry_point;

/* The list of sub-source-files within the current individual compilation.
   Each file gets its own symtab with its own linetable and associated info,
   but they all share one blockvector.  */

struct subfile
{
  struct subfile *next;
  char *name;
  char *dirname;
  struct linetable *line_vector;
  int line_vector_length;
  int line_vector_index;
  int prev_line_number;
};

static struct subfile *subfiles;

static struct subfile *current_subfile;

/* Count symbols as they are processed, for error messages.  */

static unsigned int symnum;

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

/* Hash table of global symbols whose values are not known yet.
   They are chained thru the SYMBOL_VALUE_CHAIN, since we don't
   have the correct data for that slot yet.  */
/* The use of the LOC_BLOCK code in this chain is nonstandard--
   it refers to a FORTRAN common block rather than the usual meaning.  */

#define HASHSIZE 127
static struct symbol *global_sym_chain[HASHSIZE];

/* Record the symbols defined for each context in a list.
   We don't create a struct block for the context until we
   know how long to make it.  */

#define PENDINGSIZE 100

struct pending
{
  struct pending *next;
  int nsyms;
  struct symbol *symbol[PENDINGSIZE];
};

/* List of free `struct pending' structures for reuse.  */
struct pending *free_pendings;

/* Here are the three lists that symbols are put on.  */

struct pending *file_symbols;	/* static at top level, and types */

struct pending *global_symbols;	/* global functions and variables */

struct pending *local_symbols;	/* everything local to lexical context */

/* List of symbols declared since the last BCOMM.  This list is a tail
   of local_symbols.  When ECOMM is seen, the symbols on the list
   are noted so their proper addresses can be filled in later,
   using the common block base address gotten from the assembler
   stabs.  */

struct pending *common_block;
int common_block_i;

/* Stack representing unclosed lexical contexts
   (that will become blocks, eventually).  */

struct context_stack
{
  struct pending *locals;
  struct pending_block *old_blocks;
  struct symbol *name;
  CORE_ADDR start_addr;
  CORE_ADDR end_addr;		/* Temp slot for exception handling. */
  int depth;
};

struct context_stack *context_stack;

/* Index of first unused entry in context stack.  */
int context_stack_depth;

/* Currently allocated size of context stack.  */

int context_stack_size;

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

/* Global variable which, when set, indicates that we are processing a
   .o file compiled with gcc */

static unsigned char processing_gcc_compilation;

/* Make a list of forward references which haven't been defined.  */
static struct type **undef_types;
static int undef_types_allocated, undef_types_length;

/* String table for the main symbol file.  It is kept in memory
   permanently, to speed up symbol reading.  Other files' symbol tables
   are read in on demand.  FIXME, this should be cleaner.  */

static char *symfile_string_table;
static int symfile_string_table_size;

  /* Setup a define to deal cleanly with the underscore problem */

#ifdef NAMES_HAVE_UNDERSCORE
#define HASH_OFFSET 1
#else
#define HASH_OFFSET 0
#endif

/* Complaints about the symbols we have encountered.  */

struct complaint innerblock_complaint =
  {"inner block not inside outer block in %s", 0, 0};

struct complaint blockvector_complaint = 
  {"block at %x out of order", 0, 0};

struct complaint lbrac_complaint = 
  {"bad block start address patched", 0, 0};

#if 0
struct complaint dbx_class_complaint =
  {"encountered DBX-style class variable debugging information.\n\
You seem to have compiled your program with \
\"g++ -g0\" instead of \"g++ -g\".\n\
Therefore GDB will not know about your class variables", 0, 0};
#endif

struct complaint string_table_offset_complaint =
  {"bad string table offset in symbol %d", 0, 0};

struct complaint unknown_symtype_complaint =
  {"unknown symbol type 0x%x", 0, 0};

struct complaint lbrac_rbrac_complaint =
  {"block start larger than block end", 0, 0};

struct complaint const_vol_complaint =
  {"const/volatile indicator missing, got '%c'", 0, 0};

struct complaint error_type_complaint =
  {"C++ type mismatch between compiler and debugger", 0, 0};

struct complaint invalid_member_complaint =
  {"invalid (minimal) member type data format at symtab pos %d.", 0, 0};

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

static struct header_file *header_files = 0;

static int n_header_files;

static int n_allocated_header_files;

/* During initial symbol readin, we need to have a structure to keep
   track of which psymtabs have which bincls in them.  This structure
   is used during readin to setup the list of dependencies within each
   partial symbol table. */

struct header_file_location
{
  char *name;			/* Name of header file */
  int instance;			/* See above */
  struct partial_symtab *pst;	/* Partial symtab that has the
				   BINCL/EINCL defs for this file */
};

/* The actual list and controling variables */
static struct header_file_location *bincl_list, *next_bincl;
static int bincls_allocated;

/* Within each object file, various header files are assigned numbers.
   A type is defined or referred to with a pair of numbers
   (FILENUM,TYPENUM) where FILENUM is the number of the header file
   and TYPENUM is the number within that header file.
   TYPENUM is the index within the vector of types for that header file.

   FILENUM == 1 is special; it refers to the main source of the object file,
   and not to any header file.  FILENUM != 1 is interpreted by looking it up
   in the following table, which contains indices in header_files.  */

static int *this_object_header_files = 0;

static int n_this_object_header_files;

static int n_allocated_this_object_header_files;

/* When a header file is getting special overriding definitions
   for one source file, record here the header_files index
   of its normal definition vector.
   At other times, this is -1.  */

static int header_file_prev_index;

/* Free up old header file tables, and allocate new ones.
   We're reading a new symbol file now.  */

void
free_and_init_header_files ()
{
  register int i;
  for (i = 0; i < n_header_files; i++)
    free (header_files[i].name);
  if (header_files)			/* First time null */
    free (header_files);
  if (this_object_header_files)		/* First time null */
    free (this_object_header_files);

  n_allocated_header_files = 10;
  header_files = (struct header_file *) xmalloc (10 * sizeof (struct header_file));
  n_header_files = 0;

  n_allocated_this_object_header_files = 10;
  this_object_header_files = (int *) xmalloc (10 * sizeof (int));
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
  header_file_prev_index = -1;

  /* Make sure there is room for one more header file.  */

  if (n_header_files == n_allocated_header_files)
    {
      n_allocated_header_files *= 2;
      header_files = (struct header_file *)
	xrealloc (header_files,
		  (n_allocated_header_files
		   * sizeof (struct header_file)));
    }

  /* Create an entry for this header file.  */

  i = n_header_files++;
  header_files[i].name = savestring (name, strlen(name));
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
	    xrealloc (type_vector,
		      (sizeof (struct typevector)
		       + type_vector_length * sizeof (struct type *)));
	  bzero (&type_vector->type[type_vector_length / 2],
		 type_vector_length * sizeof (struct type *) / 2);
	}
      return &type_vector->type[index];
    }
  else
    {
      register int real_filenum = this_object_header_files[filenum];
      register struct header_file *f;
      int f_orig_length;

      if (real_filenum >= n_header_files)
	abort ();

      f = &header_files[real_filenum];

      f_orig_length = f->length;
      if (index >= f_orig_length)
	{
	  while (index >= f->length)
	    f->length *= 2;
	  f->vector = (struct type **)
	    xrealloc (f->vector, f->length * sizeof (struct type *));
	  bzero (&f->vector[f_orig_length],
		 (f->length - f_orig_length) * sizeof (struct type *));
	}
      return &f->vector[index];
    }
}

/* Create a type object.  Occaisionally used when you need a type
   which isn't going to be given a type number.  */

static struct type *
dbx_create_type ()
{
  register struct type *type =
    (struct type *) obstack_alloc (symbol_obstack, sizeof (struct type));

  bzero (type, sizeof (struct type));
  TYPE_VPTR_FIELDNO (type) = -1;
  TYPE_VPTR_BASETYPE (type) = 0;
  return type;
}

/* Make sure there is a type allocated for type numbers TYPENUMS
   and return the type object.
   This can create an empty (zeroed) type object.
   TYPENUMS may be (-1, -1) to return a new type object that is not
   put into the type vector, and so may not be referred to by number. */

static struct type *
dbx_alloc_type (typenums)
     int typenums[2];
{
  register struct type **type_addr;
  register struct type *type;

  if (typenums[1] != -1)
    {
      type_addr = dbx_lookup_type (typenums);
      type = *type_addr;
    }
  else
    {
      type_addr = 0;
      type = 0;
    }

  /* If we are referring to a type not known at all yet,
     allocate an empty type for it.
     We will fill it in later if we find out how.  */
  if (type == 0)
    {
      type = dbx_create_type ();
      if (type_addr)
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
  /* We keep PENDINGSIZE symbols in each link of the list.
     If we don't have a link with room in it, add a new link.  */
  if (*listhead == 0 || (*listhead)->nsyms == PENDINGSIZE)
    {
      register struct pending *link;
      if (free_pendings)
	{
	  link = free_pendings;
	  free_pendings = link->next;
	}
      else
	link = (struct pending *) xmalloc (sizeof (struct pending));

      link->next = *listhead;
      *listhead = link;
      link->nsyms = 0;
    }

  (*listhead)->symbol[(*listhead)->nsyms++] = symbol;
}

/* At end of reading syms, or in case of quit,
   really free as many `struct pending's as we can easily find.  */

/* ARGSUSED */
static void
really_free_pendings (foo)
     int foo;
{
  struct pending *next, *next1;
#if 0
  struct pending_block *bnext, *bnext1;
#endif

  for (next = free_pendings; next; next = next1)
    {
      next1 = next->next;
      free (next);
    }
  free_pendings = 0;

#if 0 /* Now we make the links in the symbol_obstack, so don't free them.  */
  for (bnext = pending_blocks; bnext; bnext = bnext1)
    {
      bnext1 = bnext->next;
      free (bnext);
    }
#endif
  pending_blocks = 0;

  for (next = file_symbols; next; next = next1)
    {
      next1 = next->next;
      free (next);
    }
  file_symbols = 0;

  for (next = global_symbols; next; next = next1)
    {
      next1 = next->next;
      free (next);
    }
  global_symbols = 0;
}

/* Take one of the lists of symbols and make a block from it.
   Keep the order the symbols have in the list (reversed from the input file).
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

  for (next = *listhead, i = 0; next; i += next->nsyms, next = next->next)
    /*EMPTY*/;

  block = (struct block *) obstack_alloc (symbol_obstack,
					  (sizeof (struct block)
					   + ((i - 1)
					      * sizeof (struct symbol *))));

  /* Copy the symbols into the block.  */

  BLOCK_NSYMS (block) = i;
  for (next = *listhead; next; next = next->next)
    {
      register int j;
      for (j = next->nsyms - 1; j >= 0; j--)
	BLOCK_SYM (block, --i) = next->symbol[j];
    }

  BLOCK_START (block) = start;
  BLOCK_END (block) = end;
  BLOCK_SUPERBLOCK (block) = 0;	/* Filled in when containing block is made */
  BLOCK_GCC_COMPILED (block) = processing_gcc_compilation;

  /* Put the block in as the value of the symbol that names it.  */

  if (symbol)
    {
      SYMBOL_BLOCK_VALUE (symbol) = block;
      BLOCK_FUNCTION (block) = symbol;
    }
  else
    BLOCK_FUNCTION (block) = 0;

  /* Now "free" the links of the list, and empty the list.  */

  for (next = *listhead; next; next = next1)
    {
      next1 = next->next;
      next->next = free_pendings;
      free_pendings = next;
    }
  *listhead = 0;

  /* Install this block as the superblock
     of all blocks made since the start of this scope
     that don't have superblocks yet.  */

  opblock = 0;
  for (pblock = pending_blocks; pblock != old_blocks; pblock = pblock->next)
    {
      if (BLOCK_SUPERBLOCK (pblock->block) == 0) {
#if 1
	/* Check to be sure the blocks are nested as we receive them. 
	   If the compiler/assembler/linker work, this just burns a small
	   amount of time.  */
	if (BLOCK_START (pblock->block) < BLOCK_START (block)
	 || BLOCK_END   (pblock->block) > BLOCK_END   (block)) {
	  complain(&innerblock_complaint, symbol? SYMBOL_NAME (symbol):
						 "(don't know)");
	  BLOCK_START (pblock->block) = BLOCK_START (block);
	  BLOCK_END   (pblock->block) = BLOCK_END   (block);
	}
#endif
	BLOCK_SUPERBLOCK (pblock->block) = block;
      }
      opblock = pblock;
    }

  /* Record this block on the list of all blocks in the file.
     Put it after opblock, or at the beginning if opblock is 0.
     This puts the block in the list after all its subblocks.  */

  /* Allocate in the symbol_obstack to save time.
     It wastes a little space.  */
  pblock = (struct pending_block *)
    obstack_alloc (symbol_obstack,
		   sizeof (struct pending_block));
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
  register struct pending_block *next;
  register struct blockvector *blockvector;
  register int i;

  /* Count the length of the list of blocks.  */

  for (next = pending_blocks, i = 0; next; next = next->next, i++);

  blockvector = (struct blockvector *)
    obstack_alloc (symbol_obstack,
		   (sizeof (struct blockvector)
		    + (i - 1) * sizeof (struct block *)));

  /* Copy the blocks into the blockvector.
     This is done in reverse order, which happens to put
     the blocks into the proper order (ascending starting address).
     finish_block has hair to insert each block into the list
     after its subblocks in order to make sure this is true.  */

  BLOCKVECTOR_NBLOCKS (blockvector) = i;
  for (next = pending_blocks; next; next = next->next) {
    BLOCKVECTOR_BLOCK (blockvector, --i) = next->block;
  }

#if 0 /* Now we make the links in the obstack, so don't free them.  */
  /* Now free the links of the list, and empty the list.  */

  for (next = pending_blocks; next; next = next1)
    {
      next1 = next->next;
      free (next);
    }
#endif
  pending_blocks = 0;

#if 1  /* FIXME, shut this off after a while to speed up symbol reading.  */
  /* Some compilers output blocks in the wrong order, but we depend
     on their being in the right order so we can binary search. 
     Check the order and moan about it.  FIXME.  */
  if (BLOCKVECTOR_NBLOCKS (blockvector) > 1)
    for (i = 1; i < BLOCKVECTOR_NBLOCKS (blockvector); i++) {
      if (BLOCK_START(BLOCKVECTOR_BLOCK (blockvector, i-1))
	  > BLOCK_START(BLOCKVECTOR_BLOCK (blockvector, i))) {
	complain (&blockvector_complaint, 
	  BLOCK_START(BLOCKVECTOR_BLOCK (blockvector, i)));
      }
    }
#endif

  return blockvector;
}

/* Manage the vector of line numbers.  */

static void
record_line (line, pc)
     int line;
     CORE_ADDR pc;
{
  struct linetable_entry *e;
  /* Ignore the dummy line number in libg.o */

  if (line == 0xffff)
    return;

  /* Make sure line vector is big enough.  */

  if (line_vector_index + 1 >= line_vector_length)
    {
      line_vector_length *= 2;
      line_vector = (struct linetable *)
	xrealloc (line_vector,
		  (sizeof (struct linetable)
		   + line_vector_length * sizeof (struct linetable_entry)));
      current_subfile->line_vector = line_vector;
    }

  e = line_vector->item + line_vector_index++;
  e->line = line; e->pc = pc;
}

/* Start a new symtab for a new source file.
   This is called when a dbx symbol of type N_SO is seen;
   it indicates the start of data for one original source file.  */

static void
start_symtab (name, dirname, start_addr)
     char *name;
     char *dirname;
     CORE_ADDR start_addr;
{

  last_source_file = name;
  last_source_start_addr = start_addr;
  file_symbols = 0;
  global_symbols = 0;
  within_function = 0;

  /* Context stack is initially empty, with room for 10 levels.  */
  context_stack
    = (struct context_stack *) xmalloc (10 * sizeof (struct context_stack));
  context_stack_size = 10;
  context_stack_depth = 0;

  new_object_header_files ();

  type_vector_length = 160;
  type_vector = (struct typevector *)
    xmalloc (sizeof (struct typevector)
	      + type_vector_length * sizeof (struct type *));
  bzero (type_vector->type, type_vector_length * sizeof (struct type *));

  /* Initialize the list of sub source files with one entry
     for this file (the top-level source file).  */

  subfiles = 0;
  current_subfile = 0;
  start_subfile (name, dirname);
}

/* Handle an N_SOL symbol, which indicates the start of
   code that came from an included (or otherwise merged-in)
   source file with a different name.  */

static void
start_subfile (name, dirname)
     char *name;
     char *dirname;
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
    xmalloc (sizeof (struct linetable)
	      + line_vector_length * sizeof (struct linetable_entry));

  /* Make an entry for this subfile in the list of all subfiles
     of the current main source file.  */

  subfile = (struct subfile *) xmalloc (sizeof (struct subfile));
  subfile->next = subfiles;
  subfile->name = obsavestring (name, strlen (name));
  if (dirname == NULL)
    subfile->dirname = NULL;
  else
    subfile->dirname = obsavestring (dirname, strlen (dirname));
  
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
  register struct blockvector *blockvector;
  register struct subfile *subfile;
  register struct linetable *lv;
  struct subfile *nextsub;

  /* Finish the lexical context of the last function in the file;
     pop the context stack.  */

  if (context_stack_depth > 0)
    {
      register struct context_stack *cstk;
      context_stack_depth--;
      cstk = &context_stack[context_stack_depth];
      /* Make a block for the local symbols within.  */
      finish_block (cstk->name, &local_symbols, cstk->old_blocks,
		    cstk->start_addr, end_addr);
    }

  /* Cleanup any undefined types that have been left hanging around
     (this needs to be done before the finish_blocks so that
     file_symbols is still good).  */
  cleanup_undefined_types ();

  /* Define the STATIC_BLOCK and GLOBAL_BLOCK, and build the blockvector.  */
  finish_block (0, &file_symbols, 0, last_source_start_addr, end_addr);
  finish_block (0, &global_symbols, 0, last_source_start_addr, end_addr);
  blockvector = make_blockvector ();

  current_subfile->line_vector_index = line_vector_index;

  /* Now create the symtab objects proper, one for each subfile.  */
  /* (The main file is one of them.)  */

  for (subfile = subfiles; subfile; subfile = nextsub)
    {
      symtab = (struct symtab *) xmalloc (sizeof (struct symtab));

      /* Fill in its components.  */
      symtab->blockvector = blockvector;
      lv = subfile->line_vector;
      lv->nitems = subfile->line_vector_index;
      symtab->linetable = (struct linetable *)
	xrealloc (lv, (sizeof (struct linetable)
		       + lv->nitems * sizeof (struct linetable_entry)));
      type_vector->length = type_vector_length;
      symtab->typevector = type_vector;

      symtab->filename = subfile->name;
      symtab->dirname = subfile->dirname;

      symtab->free_code = free_linetable;
      symtab->free_ptr = 0;
      if (subfile->next == 0)
	symtab->free_ptr = (char *) type_vector;

      symtab->nlines = 0;
      symtab->line_charpos = 0;

      symtab->language = language_unknown;
      symtab->fullname = NULL;

      /* There should never already be a symtab for this name, since
	 any prev dups have been removed when the psymtab was read in.
  	 FIXME, there ought to be a way to check this here.  */
      /* FIXME blewit |= free_named_symtabs (symtab->filename);  */

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

void
record_misc_function (name, address, type)
     char *name;
     CORE_ADDR address;
     int type;
{
  enum misc_function_type misc_type =
    (type == (N_TEXT | N_EXT) ? mf_text :
     (type == (N_DATA | N_EXT)
      || type == (N_DATA)
      || type == (N_SETV | N_EXT)
      ) ? mf_data :
     type == (N_BSS | N_EXT) ? mf_bss :
     type == (N_ABS | N_EXT) ? mf_abs : mf_unknown);

  prim_record_misc_function (obsavestring (name, strlen (name)),
			     address, misc_type);
}

/* The BFD for this file -- only good while we're actively reading
   symbols into a psymtab or a symtab.  */

static bfd *symfile_bfd;

/* Scan and build partial symbols for a symbol file.
   We have been initialized by a call to dbx_symfile_init, which 
   put all the relevant info into a "struct dbx_symfile_info"
   hung off the struct sym_fns SF.

   ADDR is the address relative to which the symbols in it are (e.g.
   the base address of the text segment).
   MAINLINE is true if we are reading the main symbol
   table (as opposed to a shared lib or dynamically loaded file).  */

void
dbx_symfile_read (sf, addr, mainline)
     struct sym_fns *sf;
     CORE_ADDR addr;
     int mainline;	/* FIXME comments above */
{
  struct dbx_symfile_info *info = (struct dbx_symfile_info *) (sf->sym_private);
  bfd *sym_bfd = sf->sym_bfd;
  int val;
  char *filename = bfd_get_filename (sym_bfd);

  val = lseek (info->desc, info->symtab_offset, L_SET);
  if (val < 0)
    perror_with_name (filename);

  /* If mainline, set global string table pointers, and reinitialize global
     partial symbol list.  */
  if (mainline) {
    symfile_string_table = info->stringtab;
    symfile_string_table_size = info->stringtab_size;
  }

  /* If we are reinitializing, or if we have never loaded syms yet, init */
  if (mainline || global_psymbols.size == 0 || static_psymbols.size == 0)
    init_psymbol_list (info->symcount);

  symfile_bfd = sym_bfd;		/* Kludge for SWAP_SYMBOL */

  pending_blocks = 0;
  make_cleanup (really_free_pendings, 0);

  init_misc_bunches ();
  make_cleanup (discard_misc_bunches, 0);

  /* Now that the symbol table data of the executable file are all in core,
     process them and define symbols accordingly.  */

  read_dbx_symtab (filename, 
		   addr - bfd_section_vma (sym_bfd, info->text_sect), /*offset*/
		   info->desc, info->stringtab, info->stringtab_size,
		   info->symcount,
		   bfd_section_vma  (sym_bfd, info->text_sect),
		   bfd_section_size (sym_bfd, info->text_sect));

  /* Go over the misc symbol bunches and install them in vector.  */

  condense_misc_bunches (!mainline);

  /* Free up any memory we allocated for ourselves.  */

  if (!mainline) {
    free (info->stringtab);	/* Stringtab is only saved for mainline */
  }
  free (info);
  sf->sym_private = 0;		/* Zap pointer to our (now gone) info struct */

  /* Call to select_source_symtab used to be here; it was using too
     much time.  I'll make sure that list_sources can handle the lack
     of current_source_symtab */

  if (!partial_symtab_list)
    printf_filtered ("\n(no debugging symbols found)...");
}

/* Discard any information we have cached during the reading of a
   single symbol file.  This should not toss global information
   from previous symbol files that have been read.  E.g. we might
   be discarding info from reading a shared library, and should not
   throw away the info from the main file.  */

void
dbx_symfile_discard ()
{

  /* Empty the hash table of global syms looking for values.  */
  bzero (global_sym_chain, sizeof global_sym_chain);

  free_pendings = 0;
  file_symbols = 0;
  global_symbols = 0;
}

/* Initialize anything that needs initializing when a completely new
   symbol file is specified (not just adding some symbols from another
   file, e.g. a shared library).  */

void
dbx_new_init ()
{
  dbx_symfile_discard ();
  /* Don't put these on the cleanup chain; they need to stick around
     until the next call to symbol_file_command.  *Then* we'll free
     them. */
  if (symfile_string_table)
    {
      free (symfile_string_table);
      symfile_string_table = 0;
      symfile_string_table_size = 0;
    }
  free_and_init_header_files ();
}


/* dbx_symfile_init ()
   is the dbx-specific initialization routine for reading symbols.
   It is passed a struct sym_fns which contains, among other things,
   the BFD for the file whose symbols are being read, and a slot for a pointer
   to "private data" which we fill with goodies.

   We read the string table into malloc'd space and stash a pointer to it.

   Since BFD doesn't know how to read debug symbols in a format-independent
   way (and may never do so...), we have to do it ourselves.  We will never
   be called unless this is an a.out (or very similar) file. 
   FIXME, there should be a cleaner peephole into the BFD environment here.  */

void
dbx_symfile_init (sf)
  struct sym_fns *sf;
{
  int val;
  int desc;
  struct stat statbuf;
  bfd *sym_bfd = sf->sym_bfd;
  char *name = bfd_get_filename (sym_bfd);
  struct dbx_symfile_info *info;
  unsigned char size_temp[4];

  /* Allocate struct to keep track of the symfile */
  sf->sym_private = xmalloc (sizeof (*info));   /* FIXME storage leak */
  info = (struct dbx_symfile_info *)sf->sym_private;

  /* FIXME POKING INSIDE BFD DATA STRUCTURES */
  desc = fileno ((FILE *)(sym_bfd->iostream));	/* Raw file descriptor */
#define	STRING_TABLE_OFFSET	(sym_bfd->origin + obj_str_filepos (sym_bfd))
#define	SYMBOL_TABLE_OFFSET	(sym_bfd->origin + obj_sym_filepos (sym_bfd))
  /* FIXME POKING INSIDE BFD DATA STRUCTURES */

  info->desc = desc;
  info->text_sect = bfd_get_section_by_name (sym_bfd, ".text");
  if (!info->text_sect)
    abort();
  info->symcount = bfd_get_symcount (sym_bfd);

  /* Read the string table size and check it for bogosity.  */
  val = lseek (desc, STRING_TABLE_OFFSET, L_SET);
  if (val < 0)
      perror_with_name (name);
  if (fstat (desc, &statbuf) == -1)
      perror_with_name (name);

  val = myread (desc, size_temp, sizeof (long));
  if (val < 0)
      perror_with_name (name);
  info->stringtab_size = bfd_h_get_32 (sym_bfd, size_temp);
  
  if (info->stringtab_size >= 0 && info->stringtab_size < statbuf.st_size)
    {
      info->stringtab = (char *) xmalloc (info->stringtab_size);
      /* Caller is responsible for freeing the string table.  No cleanup. */
    }
  else
    info->stringtab = NULL;
  if (info->stringtab == NULL && info->stringtab_size != 0)
    error ("ridiculous string table size: %d bytes", info->stringtab_size);

  /* Now read in the string table in one big gulp.  */

  val = lseek (desc, STRING_TABLE_OFFSET, L_SET);
  if (val < 0)
    perror_with_name (name);
  val = myread (desc, info->stringtab, info->stringtab_size);
  if (val < 0)
    perror_with_name (name);

  /* Record the position of the symbol table for later use.  */

  info->symtab_offset = SYMBOL_TABLE_OFFSET;
}

/* Buffer for reading the symbol table entries.  */
static struct nlist symbuf[4096];
static int symbuf_idx;
static int symbuf_end;

/* I/O descriptor for reading the symbol table.  */
static int symtab_input_desc;

/* The address in memory of the string table of the object file we are
   reading (which might not be the "main" object file, but might be a
   shared library or some other dynamically loaded thing).  This is set
   by read_dbx_symtab when building psymtabs, and by read_ofile_symtab 
   when building symtabs, and is used only by next_symbol_text.  */
static char *stringtab_global;

/* Refill the symbol table input buffer
   and set the variables that control fetching entries from it.
   Reports an error if no data available.
   This function can read past the end of the symbol table
   (into the string table) but this does no harm.  */

static int
fill_symbuf ()
{
  int nbytes = myread (symtab_input_desc, symbuf, sizeof (symbuf));
  if (nbytes < 0)
    perror_with_name ("<symbol file>");
  else if (nbytes == 0)
    error ("Premature end of file reading symbol table");
  symbuf_end = nbytes / sizeof (struct nlist);
  symbuf_idx = 0;
  return 1;
}

#define SWAP_SYMBOL(symp) \
  { \
    (symp)->n_un.n_strx = bfd_h_get_32(symfile_bfd,			\
				(unsigned char *)&(symp)->n_un.n_strx);	\
    (symp)->n_desc = bfd_h_get_16 (symfile_bfd,			\
				(unsigned char *)&(symp)->n_desc);  	\
    (symp)->n_value = bfd_h_get_32 (symfile_bfd,			\
				(unsigned char *)&(symp)->n_value); 	\
  }

/* Invariant: The symbol pointed to by symbuf_idx is the first one
   that hasn't been swapped.  Swap the symbol at the same time
   that symbuf_idx is incremented.  */

/* dbx allows the text of a symbol name to be continued into the
   next symbol name!  When such a continuation is encountered
   (a \ at the end of the text of a name)
   call this function to get the continuation.  */

static char *
next_symbol_text ()
{
  if (symbuf_idx == symbuf_end)
    fill_symbuf ();
  symnum++;
  SWAP_SYMBOL(&symbuf[symbuf_idx]);
  return symbuf[symbuf_idx++].n_un.n_strx + stringtab_global;
}

/* Initializes storage for all of the partial symbols that will be
   created by read_dbx_symtab and subsidiaries.  */

static void
init_psymbol_list (total_symbols)
     int total_symbols;
{
  /* Free any previously allocated psymbol lists.  */
  if (global_psymbols.list)
    free (global_psymbols.list);
  if (static_psymbols.list)
    free (static_psymbols.list);

  /* Current best guess is that there are approximately a twentieth
     of the total symbols (in a debugging file) are global or static
     oriented symbols */
  global_psymbols.size = total_symbols / 10;
  static_psymbols.size = total_symbols / 10;
  global_psymbols.next = global_psymbols.list = (struct partial_symbol *)
    xmalloc (global_psymbols.size * sizeof (struct partial_symbol));
  static_psymbols.next = static_psymbols.list = (struct partial_symbol *)
    xmalloc (static_psymbols.size * sizeof (struct partial_symbol));
}

/* Initialize the list of bincls to contain none and have some
   allocated.  */

static void
init_bincl_list (number)
     int number;
{
  bincls_allocated = number;
  next_bincl = bincl_list = (struct header_file_location *)
      xmalloc (bincls_allocated * sizeof(struct header_file_location));
}

/* Add a bincl to the list.  */

static void
add_bincl_to_list (pst, name, instance)
     struct partial_symtab *pst;
     char *name;
     int instance;
{
  if (next_bincl >= bincl_list + bincls_allocated)
    {
      int offset = next_bincl - bincl_list;
      bincls_allocated *= 2;
      bincl_list = (struct header_file_location *)
	xrealloc ((char *)bincl_list,
		  bincls_allocated * sizeof (struct header_file_location));
      next_bincl = bincl_list + offset;
    }
  next_bincl->pst = pst;
  next_bincl->instance = instance;
  next_bincl++->name = name;
}

/* Given a name, value pair, find the corresponding
   bincl in the list.  Return the partial symtab associated
   with that header_file_location.  */

struct partial_symtab *
find_corresponding_bincl_psymtab (name, instance)
     char *name;
     int instance;
{
  struct header_file_location *bincl;

  for (bincl = bincl_list; bincl < next_bincl; bincl++)
    if (bincl->instance == instance
	&& !strcmp (name, bincl->name))
      return bincl->pst;

  return (struct partial_symtab *) 0;
}

/* Free the storage allocated for the bincl list.  */

static void
free_bincl_list ()
{
  free (bincl_list);
  bincls_allocated = 0;
}

static struct partial_symtab *start_psymtab ();
static void end_psymtab();

#ifdef DEBUG
/* This is normally a macro defined in read_dbx_symtab, but this
   is a lot easier to debug.  */

ADD_PSYMBOL_TO_PLIST(NAME, NAMELENGTH, NAMESPACE, CLASS, PLIST, VALUE)
     char *NAME;
     int NAMELENGTH;
     enum namespace NAMESPACE;
     enum address_class CLASS;
     struct psymbol_allocation_list *PLIST;
     unsigned long VALUE;
{
  register struct partial_symbol *psym;

#define LIST *PLIST
  do {		        						
    if ((LIST).next >=					
	(LIST).list + (LIST).size)			
      {									
	(LIST).list = (struct partial_symbol *)				
	  xrealloc ((LIST).list,					
		    ((LIST).size * 2					
		     * sizeof (struct partial_symbol)));		
	/* Next assumes we only went one over.  Should be good if	
	   program works correctly */					
	(LIST).next =							
	  (LIST).list + (LIST).size;				
	(LIST).size *= 2;				
      }									
    psym = (LIST).next++;						
#undef LIST

    SYMBOL_NAME (psym) = (char *) obstack_alloc (psymbol_obstack,	
						 (NAMELENGTH) + 1);	
    strncpy (SYMBOL_NAME (psym), (NAME), (NAMELENGTH));			
    SYMBOL_NAME (psym)[(NAMELENGTH)] = '\0';				
    SYMBOL_NAMESPACE (psym) = (NAMESPACE);				
    SYMBOL_CLASS (psym) = (CLASS);				
    SYMBOL_VALUE (psym) = (VALUE); 					
  } while (0);
}

/* Since one arg is a struct, we have to pass in a ptr and deref it (sigh) */
#define	ADD_PSYMBOL_TO_LIST(NAME, NAMELENGTH, NAMESPACE, CLASS,  LIST, VALUE) \
       ADD_PSYMBOL_TO_PLIST(NAME, NAMELENGTH, NAMESPACE, CLASS, &LIST, VALUE)

#endif /* DEBUG */

/* Given pointers to an a.out symbol table in core containing dbx
   style data, setup partial_symtab's describing each source file for
   which debugging information is available.  NLISTLEN is the number
   of symbols in the symbol table.  All symbol names are given as
   offsets relative to STRINGTAB.  STRINGTAB_SIZE is the size of
   STRINGTAB.  SYMFILE_NAME is the name of the file we are reading from
   and ADDR is its relocated address (if incremental) or 0 (if not).  */

static void
read_dbx_symtab (symfile_name, addr,
		 desc, stringtab, stringtab_size, nlistlen,
		 text_addr, text_size)
     char *symfile_name;
     CORE_ADDR addr;
     int desc;
     register char *stringtab;
     register long stringtab_size;
     register int nlistlen;
     CORE_ADDR text_addr;
     int text_size;
{
  register struct nlist *bufp;
  register char *namestring;
  register struct partial_symbol *psym;
  int nsl;
  int past_first_source_file = 0;
  CORE_ADDR last_o_file_start = 0;
  struct cleanup *old_chain;
  char *p;

  /* End of the text segment of the executable file.  */
  CORE_ADDR end_of_text_addr;

  /* Current partial symtab */
  struct partial_symtab *pst;

  /* List of current psymtab's include files */
  char **psymtab_include_list;
  int includes_allocated;
  int includes_used;

  /* Index within current psymtab dependency list */
  struct partial_symtab **dependency_list;
  int dependencies_used, dependencies_allocated;

  stringtab_global = stringtab;
  
  pst = (struct partial_symtab *) 0;

  includes_allocated = 30;
  includes_used = 0;
  psymtab_include_list = (char **) alloca (includes_allocated *
					   sizeof (char *));

  dependencies_allocated = 30;
  dependencies_used = 0;
  dependency_list =
    (struct partial_symtab **) alloca (dependencies_allocated *
				       sizeof (struct partial_symtab *));

  /* FIXME!!  If an error occurs, this blows away the whole symbol table! 
     It should only blow away the psymtabs created herein.  We could
     be reading a shared library or a dynloaded file!  */
  old_chain = make_cleanup (free_all_psymtabs, 0);

  /* Init bincl list */
  init_bincl_list (20);
  make_cleanup (free_bincl_list, 0);

  last_source_file = 0;

#ifdef END_OF_TEXT_DEFAULT
  end_of_text_addr = END_OF_TEXT_DEFAULT;
#else
  end_of_text_addr = text_addr + text_size;
#endif

  symtab_input_desc = desc;	/* This is needed for fill_symbuf below */
  symbuf_end = symbuf_idx = 0;

  for (symnum = 0; symnum < nlistlen; symnum++)
    {
      /* Get the symbol for this run and pull out some info */
      QUIT;	/* allow this to be interruptable */
      if (symbuf_idx == symbuf_end)
	fill_symbuf ();
      bufp = &symbuf[symbuf_idx++];

      /*
       * Special case to speed up readin.
       */
      if (bufp->n_type == (unsigned char)N_SLINE) continue;

      SWAP_SYMBOL (bufp);

      /* Ok.  There is a lot of code duplicated in the rest of this
         switch statement (for efficiency reasons).  Since I don't
         like duplicating code, I will do my penance here, and
         describe the code which is duplicated:

	 *) The assignment to namestring.
	 *) The call to strchr.
	 *) The addition of a partial symbol the the two partial
	    symbol lists.  This last is a large section of code, so
	    I've imbedded it in the following macro.
	 */
      
/* Set namestring based on bufp.  If the string table index is invalid, 
   give a fake name, and print a single error message per symbol file read,
   rather than abort the symbol reading or flood the user with messages.  */
#define SET_NAMESTRING()\
  if (bufp->n_un.n_strx < 0 || bufp->n_un.n_strx >= stringtab_size) {	\
    complain (&string_table_offset_complaint, symnum);			\
    namestring = "foo";							\
  } else								\
    namestring = bufp->n_un.n_strx + stringtab

/* Add a symbol with an integer value to a psymtab. */
/* This is a macro unless we're debugging.  See above this function. */
#ifndef DEBUG
#  define ADD_PSYMBOL_TO_LIST(NAME, NAMELENGTH, NAMESPACE, CLASS, LIST, VALUE) \
 ADD_PSYMBOL_VT_TO_LIST(NAME, NAMELENGTH, NAMESPACE, CLASS, LIST, VALUE, \
                        SYMBOL_VALUE)
#endif /* DEBUG */

/* Add a symbol with a CORE_ADDR value to a psymtab. */
#define	ADD_PSYMBOL_ADDR_TO_LIST(NAME, NAMELENGTH, NAMESPACE, CLASS, LIST, VALUE) \
 ADD_PSYMBOL_VT_TO_LIST(NAME, NAMELENGTH, NAMESPACE, CLASS, LIST, VALUE, \
                        SYMBOL_VALUE_ADDRESS)

/* Add any kind of symbol to a psymtab. */
#define	ADD_PSYMBOL_VT_TO_LIST(NAME, NAMELENGTH, NAMESPACE, CLASS, LIST, VALUE, VT)\
  do {		        						\
    if ((LIST).next >=							\
	(LIST).list + (LIST).size)					\
      {									\
	(LIST).list = (struct partial_symbol *)				\
	  xrealloc ((LIST).list,					\
		    ((LIST).size * 2					\
		     * sizeof (struct partial_symbol)));		\
	/* Next assumes we only went one over.  Should be good if	\
	   program works correctly */					\
	(LIST).next =							\
	  (LIST).list + (LIST).size;					\
	(LIST).size *= 2;						\
      }									\
    psym = (LIST).next++;						\
									\
    SYMBOL_NAME (psym) = (char *) obstack_alloc (psymbol_obstack,	\
						 (NAMELENGTH) + 1);	\
    strncpy (SYMBOL_NAME (psym), (NAME), (NAMELENGTH));			\
    SYMBOL_NAME (psym)[(NAMELENGTH)] = '\0';				\
    SYMBOL_NAMESPACE (psym) = (NAMESPACE);				\
    SYMBOL_CLASS (psym) = (CLASS);					\
    VT (psym) = (VALUE); 						\
  } while (0);

/* End of macro definitions, now let's handle them symbols!  */

      switch (bufp->n_type)
	{
	  /*
	   * Standard, external, non-debugger, symbols
	   */

	case N_TEXT | N_EXT:
	case N_NBTEXT | N_EXT:
	case N_NBDATA | N_EXT:
	case N_NBBSS | N_EXT:
	case N_SETV | N_EXT:
	case N_ABS | N_EXT:
	case N_DATA | N_EXT:
	case N_BSS | N_EXT:

	  bufp->n_value += addr;		/* Relocate */

	  SET_NAMESTRING();

	bss_ext_symbol:
	  record_misc_function (namestring, bufp->n_value,
				bufp->n_type); /* Always */

	  continue;

	  /* Standard, local, non-debugger, symbols */

	case N_NBTEXT:

	  /* We need to be able to deal with both N_FN or N_TEXT,
	     because we have no way of knowing whether the sys-supplied ld
	     or GNU ld was used to make the executable.  */
#if ! (N_FN & N_EXT)
	case N_FN:
#endif
	case N_FN | N_EXT:
	case N_TEXT:
	  bufp->n_value += addr;		/* Relocate */
	  SET_NAMESTRING();
	  if ((namestring[0] == '-' && namestring[1] == 'l')
	      || (namestring [(nsl = strlen (namestring)) - 1] == 'o'
		  && namestring [nsl - 2] == '.'))
	    {
	      if (entry_point < bufp->n_value
		  && entry_point >= last_o_file_start
		  && addr == 0)		/* FIXME nogood nomore */
		{
		  startup_file_start = last_o_file_start;
		  startup_file_end = bufp->n_value;
		}
	      if (past_first_source_file && pst
		  /* The gould NP1 uses low values for .o and -l symbols
		     which are not the address.  */
		  && bufp->n_value > pst->textlow)
		{
		  end_psymtab (pst, psymtab_include_list, includes_used,
			       symnum * sizeof (struct nlist), bufp->n_value,
			       dependency_list, dependencies_used,
			       global_psymbols.next, static_psymbols.next);
		  pst = (struct partial_symtab *) 0;
		  includes_used = 0;
		  dependencies_used = 0;
		}
	      else
		past_first_source_file = 1;
	      last_o_file_start = bufp->n_value;
	    }
	  continue;

	case N_DATA:
	  bufp->n_value += addr;		/* Relocate */
	  SET_NAMESTRING ();
	  /* Check for __DYNAMIC, which is used by Sun shared libraries. 
	     Record it even if it's local, not global, so we can find it.
	     Same with virtual function tables, both global and static.  */
	  if ((namestring[8] == 'C' && (strcmp ("__DYNAMIC", namestring) == 0))
	      || VTBL_PREFIX_P ((namestring+HASH_OFFSET)))
	    {
	      /* Not really a function here, but... */
	      record_misc_function (namestring, bufp->n_value,
				    bufp->n_type); /* Always */
	  }
	  continue;

	case N_UNDF | N_EXT:
	  if (bufp->n_value != 0) {
	    /* This is a "Fortran COMMON" symbol.  See if the target
	       environment knows where it has been relocated to.  */

	    CORE_ADDR reladdr;

	    SET_NAMESTRING();
	    if (target_lookup_symbol (namestring, &reladdr)) {
	      continue;		/* Error in lookup; ignore symbol for now.  */
	    }
	    bufp->n_type ^= (N_BSS^N_UNDF);	/* Define it as a bss-symbol */
	    bufp->n_value = reladdr;
	    goto bss_ext_symbol;
	  }
	  continue;	/* Just undefined, not COMMON */

	    /* Lots of symbol types we can just ignore.  */

	case N_UNDF:
	case N_ABS:
	case N_BSS:
	case N_NBDATA:
	case N_NBBSS:
	  continue;

	  /* Keep going . . .*/

	  /*
	   * Special symbol types for GNU
	   */
	case N_INDR:
	case N_INDR | N_EXT:
	case N_SETA:
	case N_SETA | N_EXT:
	case N_SETT:
	case N_SETT | N_EXT:
	case N_SETD:
	case N_SETD | N_EXT:
	case N_SETB:
	case N_SETB | N_EXT:
	case N_SETV:
	  continue;

	  /*
	   * Debugger symbols
	   */

	case N_SO: {
	  unsigned long valu = bufp->n_value;
	  /* Symbol number of the first symbol of this file (i.e. the N_SO
	     if there is just one, or the first if we have a pair).  */
	  int first_symnum = symnum;
	  
	  /* End the current partial symtab and start a new one */

	  SET_NAMESTRING();

	  /* Peek at the next symbol.  If it is also an N_SO, the
	     first one just indicates the directory.  */
	  if (symbuf_idx == symbuf_end)
	    fill_symbuf ();
	  bufp = &symbuf[symbuf_idx];
	  /* n_type is only a char, so swapping swapping is irrelevant.  */
	  if (bufp->n_type == (unsigned char)N_SO)
	    {
	      SWAP_SYMBOL (bufp);
	      SET_NAMESTRING ();
	      valu = bufp->n_value;
	      symbuf_idx++;
	      symnum++;
	    }
	  valu += addr;		/* Relocate */

	  if (pst && past_first_source_file)
	    {
	      end_psymtab (pst, psymtab_include_list, includes_used,
			   first_symnum * sizeof (struct nlist), valu,
			   dependency_list, dependencies_used,
			   global_psymbols.next, static_psymbols.next);
	      pst = (struct partial_symtab *) 0;
	      includes_used = 0;
	      dependencies_used = 0;
	    }
	  else
	    past_first_source_file = 1;

	  pst = start_psymtab (symfile_name, addr,
			       namestring, valu,
			       first_symnum * sizeof (struct nlist),
			       global_psymbols.next, static_psymbols.next);

	  continue;
	}

	case N_BINCL:
	  /* Add this bincl to the bincl_list for future EXCLs.  No
	     need to save the string; it'll be around until
	     read_dbx_symtab function returns */

	  SET_NAMESTRING();

	  add_bincl_to_list (pst, namestring, bufp->n_value);

	  /* Mark down an include file in the current psymtab */

	  psymtab_include_list[includes_used++] = namestring;
	  if (includes_used >= includes_allocated)
	    {
	      char **orig = psymtab_include_list;

	      psymtab_include_list = (char **)
		alloca ((includes_allocated *= 2) *
			sizeof (char *));
	      bcopy (orig, psymtab_include_list,
		     includes_used * sizeof (char *));
	    }

	  continue;

	case N_SOL:
	  /* Mark down an include file in the current psymtab */

	  SET_NAMESTRING();

	  /* In C++, one may expect the same filename to come round many
	     times, when code is coming alternately from the main file
	     and from inline functions in other files. So I check to see
	     if this is a file we've seen before -- either the main
	     source file, or a previously included file.

	     This seems to be a lot of time to be spending on N_SOL, but
	     things like "break expread.y:435" need to work (I
	     suppose the psymtab_include_list could be hashed or put
	     in a binary tree, if profiling shows this is a major hog).  */
	  if (!strcmp (namestring, pst->filename))
	    continue;
	  {
	    register int i;
	    for (i = 0; i < includes_used; i++)
	      if (!strcmp (namestring, psymtab_include_list[i]))
		{
		  i = -1; 
		  break;
		}
	    if (i == -1)
	      continue;
	  }

	  psymtab_include_list[includes_used++] = namestring;
	  if (includes_used >= includes_allocated)
	    {
	      char **orig = psymtab_include_list;

	      psymtab_include_list = (char **)
		alloca ((includes_allocated *= 2) *
			sizeof (char *));
	      bcopy (orig, psymtab_include_list,
		     includes_used * sizeof (char *));
	    }
	  continue;

	case N_LSYM:		/* Typedef or automatic variable. */
	  SET_NAMESTRING();

	  p = (char *) strchr (namestring, ':');

	  /* Skip if there is no :.  */
	  if (!p) continue;

	  switch (p[1])
	    {
	    case 'T':
	      ADD_PSYMBOL_TO_LIST (namestring, p - namestring,
				   STRUCT_NAMESPACE, LOC_TYPEDEF,
				   static_psymbols, bufp->n_value);
	      if (p[2] == 't')
		{
		  /* Also a typedef with the same name.  */
		  ADD_PSYMBOL_TO_LIST (namestring, p - namestring,
				       VAR_NAMESPACE, LOC_TYPEDEF,
				       static_psymbols, bufp->n_value);
		  p += 1;
		}
	      goto check_enum;
	    case 't':
	      ADD_PSYMBOL_TO_LIST (namestring, p - namestring,
				   VAR_NAMESPACE, LOC_TYPEDEF,
				   static_psymbols, bufp->n_value);
	    check_enum:
	      /* If this is an enumerated type, we need to
		 add all the enum constants to the partial symbol
		 table.  This does not cover enums without names, e.g.
		 "enum {a, b} c;" in C, but fortunately those are
		 rare.  There is no way for GDB to find those from the
		 enum type without spending too much time on it.  Thus
		 to solve this problem, the compiler needs to put out separate
		 constant symbols ('c' N_LSYMS) for enum constants in
		 enums without names, or put out a dummy type.  */

	      /* We are looking for something of the form
		 <name> ":" ("t" | "T") [<number> "="] "e"
		 {<constant> ":" <value> ","} ";".  */

	      /* Skip over the colon and the 't' or 'T'.  */
	      p += 2;
	      /* This type may be given a number.  Skip over it.  */
	      while ((*p >= '0' && *p <= '9')
		     || *p == '=')
		p++;

	      if (*p++ == 'e')
		{
		  /* We have found an enumerated type.  */
		  /* According to comments in read_enum_type
		     a comma could end it instead of a semicolon.
		     I don't know where that happens.
		     Accept either.  */
		  while (*p && *p != ';' && *p != ',')
		    {
		      char *q;

		      /* Check for and handle cretinous dbx symbol name
			 continuation!  */
		      if (*p == '\\')
			p = next_symbol_text ();

		      /* Point to the character after the name
			 of the enum constant.  */
		      for (q = p; *q && *q != ':'; q++)
			;
		      /* Note that the value doesn't matter for
			 enum constants in psymtabs, just in symtabs.  */
		      ADD_PSYMBOL_TO_LIST (p, q - p,
					   VAR_NAMESPACE, LOC_CONST,
					   static_psymbols, 0);
		      /* Point past the name.  */
		      p = q;
		      /* Skip over the value.  */
		      while (*p && *p != ',')
			p++;
		      /* Advance past the comma.  */
		      if (*p)
			p++;
		    }
		}

	      continue;
	    case 'c':
	      /* Constant, e.g. from "const" in Pascal.  */
	      ADD_PSYMBOL_TO_LIST (namestring, p - namestring,
				   VAR_NAMESPACE, LOC_CONST,
				   static_psymbols, bufp->n_value);
	      continue;
	    default:
	      /* Skip if the thing following the : is
	         not a letter (which indicates declaration of a local
	         variable, which we aren't interested in).  */
	      continue;
	    }

	case N_FUN:
	case N_GSYM:		/* Global (extern) variable; can be
				   data or bss (sigh).  */
	case N_STSYM:		/* Data seg var -- static  */
	case N_LCSYM:		/* BSS      "  */

	case N_NBSTS:           /* Gould nobase.  */
	case N_NBLCS:           /* symbols.  */

	/* Following may probably be ignored; I'll leave them here
	   for now (until I do Pascal and Modula 2 extensions).  */

	case N_PC:		/* I may or may not need this; I
				   suspect not.  */
	case N_M2C:		/* I suspect that I can ignore this here. */
	case N_SCOPE:		/* Same.   */

	  SET_NAMESTRING();

	  p = (char *) strchr (namestring, ':');
	  if (!p)
	    continue;		/* Not a debugging symbol.   */



	  /* Main processing section for debugging symbols which
	     the initial read through the symbol tables needs to worry
	     about.  If we reach this point, the symbol which we are
	     considering is definitely one we are interested in.
	     p must also contain the (valid) index into the namestring
	     which indicates the debugging type symbol.  */

	  switch (p[1])
	    {
	    case 'c':
	      ADD_PSYMBOL_TO_LIST (namestring, p - namestring,
				   VAR_NAMESPACE, LOC_CONST,
				   static_psymbols, bufp->n_value);
	      continue;
	    case 'S':
	      bufp->n_value += addr;		/* Relocate */
	      ADD_PSYMBOL_ADDR_TO_LIST (namestring, p - namestring,
				   VAR_NAMESPACE, LOC_STATIC,
				   static_psymbols, bufp->n_value);
	      continue;
	    case 'G':
	      bufp->n_value += addr;		/* Relocate */
	      ADD_PSYMBOL_ADDR_TO_LIST (namestring, p - namestring,
				   VAR_NAMESPACE, LOC_EXTERNAL,
				   global_psymbols, bufp->n_value);
	      continue;

	    case 't':
	      ADD_PSYMBOL_TO_LIST (namestring, p - namestring,
				   VAR_NAMESPACE, LOC_TYPEDEF,
				   global_psymbols, bufp->n_value);
	      continue;

	    case 'f':
	      ADD_PSYMBOL_TO_LIST (namestring, p - namestring,
				   VAR_NAMESPACE, LOC_BLOCK,
				   static_psymbols, bufp->n_value);
	      continue;

	      /* Global functions were ignored here, but now they
	         are put into the global psymtab like one would expect.
		 They're also in the misc fn vector... 
		 FIXME, why did it used to ignore these?  That broke
		 "i fun" on these functions.  */
	    case 'F':
	      ADD_PSYMBOL_TO_LIST (namestring, p - namestring,
				   VAR_NAMESPACE, LOC_BLOCK,
				   global_psymbols, bufp->n_value);
	      continue;

	      /* Two things show up here (hopefully); static symbols of
		 local scope (static used inside braces) or extensions
		 of structure symbols.  We can ignore both.  */
	    case 'V':
	    case '(':
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
	      continue;

	    default:
	      /* Unexpected symbol.  Ignore it; perhaps it is an extension
		 that we don't know about.

		 Someone says sun cc puts out symbols like
		 /foo/baz/maclib::/usr/local/bin/maclib,
		 which would get here with a symbol type of ':'.  */
	      continue;
	    }

	case N_EXCL:

	  SET_NAMESTRING();

	  /* Find the corresponding bincl and mark that psymtab on the
	     psymtab dependency list */
	  {
	    struct partial_symtab *needed_pst =
	      find_corresponding_bincl_psymtab (namestring, bufp->n_value);

	    /* If this include file was defined earlier in this file,
	       leave it alone.  */
	    if (needed_pst == pst) continue;

	    if (needed_pst)
	      {
		int i;
		int found = 0;

		for (i = 0; i < dependencies_used; i++)
		  if (dependency_list[i] == needed_pst)
		    {
		      found = 1;
		      break;
		    }

		/* If it's already in the list, skip the rest.  */
		if (found) continue;

		dependency_list[dependencies_used++] = needed_pst;
		if (dependencies_used >= dependencies_allocated)
		  {
		    struct partial_symtab **orig = dependency_list;
		    dependency_list =
		      (struct partial_symtab **)
			alloca ((dependencies_allocated *= 2)
				* sizeof (struct partial_symtab *));
		    bcopy (orig, dependency_list,
			   (dependencies_used
			    * sizeof (struct partial_symtab *)));
#ifdef DEBUG_INFO
		    fprintf (stderr, "Had to reallocate dependency list.\n");
		    fprintf (stderr, "New dependencies allocated: %d\n",
			     dependencies_allocated);
#endif
		  }
	      }
	    else
	      error ("Invalid symbol data: \"repeated\" header file not previously seen, at symtab pos %d.",
		     symnum);
	  }
	  continue;

	case N_EINCL:
	case N_DSLINE:
	case N_BSLINE:
	case N_SSYM:		/* Claim: Structure or union element.
				   Hopefully, I can ignore this.  */
	case N_ENTRY:		/* Alternate entry point; can ignore. */
	case N_MAIN:		/* Can definitely ignore this.   */
	case N_CATCH:		/* These are GNU C++ extensions */
	case N_EHDECL:		/* that can safely be ignored here. */
	case N_LENG:
	case N_BCOMM:
	case N_ECOMM:
	case N_ECOML:
	case N_FNAME:
	case N_SLINE:
	case N_RSYM:
	case N_PSYM:
	case N_LBRAC:
	case N_RBRAC:
	case N_NSYMS:		/* Ultrix 4.0: symbol count */
	  /* These symbols aren't interesting; don't worry about them */

	  continue;

	default:
	  /* If we haven't found it yet, ignore it.  It's probably some
	     new type we don't know about yet.  */
	  complain (&unknown_symtype_complaint, bufp->n_type);
	  continue;
	}
    }

  /* If there's stuff to be cleaned up, clean it up.  */
  if (nlistlen > 0				/* We have some syms */
      && entry_point < bufp->n_value
      && entry_point >= last_o_file_start)
    {
      startup_file_start = last_o_file_start;
      startup_file_end = bufp->n_value;
    }

  if (pst)
    {
      end_psymtab (pst, psymtab_include_list, includes_used,
		   symnum * sizeof (struct nlist), end_of_text_addr,
		   dependency_list, dependencies_used,
		   global_psymbols.next, static_psymbols.next);
      includes_used = 0;
      dependencies_used = 0;
      pst = (struct partial_symtab *) 0;
    }

  free_bincl_list ();
  discard_cleanups (old_chain);
}

/*
 * Allocate and partially fill a partial symtab.  It will be
 * completely filled at the end of the symbol list.

 SYMFILE_NAME is the name of the symbol-file we are reading from, and ADDR
 is the address relative to which its symbols are (incremental) or 0
 (normal).  */
static struct partial_symtab *
start_psymtab (symfile_name, addr,
	       filename, textlow, ldsymoff, global_syms, static_syms)
     char *symfile_name;
     CORE_ADDR addr;
     char *filename;
     CORE_ADDR textlow;
     int ldsymoff;
     struct partial_symbol *global_syms;
     struct partial_symbol *static_syms;
{
  struct partial_symtab *result =
    (struct partial_symtab *) obstack_alloc (psymbol_obstack,
					     sizeof (struct partial_symtab));

  result->addr = addr;

  result->symfile_name =
    (char *) obstack_alloc (psymbol_obstack,
			    strlen (symfile_name) + 1);
  strcpy (result->symfile_name, symfile_name);
  
  result->filename =
    (char *) obstack_alloc (psymbol_obstack,
			    strlen (filename) + 1);
  strcpy (result->filename, filename);

  result->textlow = textlow;
  result->ldsymoff = ldsymoff;

  result->readin = 0;
  result->symtab = 0;
  result->read_symtab = dbx_psymtab_to_symtab;

  result->globals_offset = global_syms - global_psymbols.list;
  result->statics_offset = static_syms - static_psymbols.list;

  result->n_global_syms = 0;
  result->n_static_syms = 0;


  return result;
}

static int
compare_psymbols (s1, s2)
     register struct partial_symbol *s1, *s2;
{
  register char
    *st1 = SYMBOL_NAME (s1),
    *st2 = SYMBOL_NAME (s2);

  return (st1[0] - st2[0] ? st1[0] - st2[0] :
	  strcmp (st1 + 1, st2 + 1));
}


/* Close off the current usage of a partial_symbol table entry.  This
   involves setting the correct number of includes (with a realloc),
   setting the high text mark, setting the symbol length in the
   executable, and setting the length of the global and static lists
   of psymbols.

   The global symbols and static symbols are then seperately sorted.

   Then the partial symtab is put on the global list.
   *** List variables and peculiarities of same. ***
   */
static void
end_psymtab (pst, include_list, num_includes, capping_symbol_offset,
	     capping_text, dependency_list, number_dependencies,
	     capping_global, capping_static)
     struct partial_symtab *pst;
     char **include_list;
     int num_includes;
     int capping_symbol_offset;
     CORE_ADDR capping_text;
     struct partial_symtab **dependency_list;
     int number_dependencies;
     struct partial_symbol *capping_global, *capping_static;
{
  int i;

  pst->ldsymlen = capping_symbol_offset - pst->ldsymoff;
  pst->texthigh = capping_text;

  pst->n_global_syms =
    capping_global - (global_psymbols.list + pst->globals_offset);
  pst->n_static_syms =
    capping_static - (static_psymbols.list + pst->statics_offset);

  pst->number_of_dependencies = number_dependencies;
  if (number_dependencies)
    {
      pst->dependencies = (struct partial_symtab **)
	obstack_alloc (psymbol_obstack,
		       number_dependencies * sizeof (struct partial_symtab *));
      bcopy (dependency_list, pst->dependencies,
	     number_dependencies * sizeof (struct partial_symtab *));
    }
  else
    pst->dependencies = 0;

  for (i = 0; i < num_includes; i++)
    {
      /* Eventually, put this on obstack */
      struct partial_symtab *subpst =
	(struct partial_symtab *)
	  obstack_alloc (psymbol_obstack,
			 sizeof (struct partial_symtab));

      subpst->filename =
	(char *) obstack_alloc (psymbol_obstack,
				strlen (include_list[i]) + 1);
      strcpy (subpst->filename, include_list[i]);

      subpst->symfile_name = pst->symfile_name;
      subpst->addr = pst->addr;
      subpst->ldsymoff =
	subpst->ldsymlen =
	  subpst->textlow =
	    subpst->texthigh = 0;

      /* We could save slight bits of space by only making one of these,
	 shared by the entire set of include files.  FIXME-someday.  */
      subpst->dependencies = (struct partial_symtab **)
	obstack_alloc (psymbol_obstack,
		       sizeof (struct partial_symtab *));
      subpst->dependencies[0] = pst;
      subpst->number_of_dependencies = 1;

      subpst->globals_offset =
	subpst->n_global_syms =
	  subpst->statics_offset =
	    subpst->n_static_syms = 0;

      subpst->readin = 0;
      subpst->symtab = 0;
      subpst->read_symtab = dbx_psymtab_to_symtab;

      subpst->next = partial_symtab_list;
      partial_symtab_list = subpst;
    }

  /* Sort the global list; don't sort the static list */
  qsort (global_psymbols.list + pst->globals_offset, pst->n_global_syms,
	 sizeof (struct partial_symbol), compare_psymbols);

  /* If there is already a psymtab or symtab for a file of this name, remove it.
     (If there is a symtab, more drastic things also happen.)
     This happens in VxWorks.  */
  free_named_symtabs (pst->filename);

  /* Put the psymtab on the psymtab list */
  pst->next = partial_symtab_list;
  partial_symtab_list = pst;
}

static void
psymtab_to_symtab_1 (pst, desc, stringtab, stringtab_size, sym_offset)
     struct partial_symtab *pst;
     int desc;
     char *stringtab;
     int stringtab_size;
     int sym_offset;
{
  struct cleanup *old_chain;
  int i;
  
  if (!pst)
    return;

  if (pst->readin)
    {
      fprintf (stderr, "Psymtab for %s already read in.  Shouldn't happen.\n",
	       pst->filename);
      return;
    }

  /* Read in all partial symbtabs on which this one is dependent */
  for (i = 0; i < pst->number_of_dependencies; i++)
    if (!pst->dependencies[i]->readin)
      {
	/* Inform about additional files that need to be read in.  */
	if (info_verbose)
	  {
	    fputs_filtered (" ", stdout);
	    wrap_here ("");
	    fputs_filtered ("and ", stdout);
	    wrap_here ("");
	    printf_filtered ("%s...", pst->dependencies[i]->filename);
	    wrap_here ("");		/* Flush output */
	    fflush (stdout);
	  }
	psymtab_to_symtab_1 (pst->dependencies[i], desc,
			     stringtab, stringtab_size, sym_offset);
      }

  if (pst->ldsymlen)		/* Otherwise it's a dummy */
    {
      /* Init stuff necessary for reading in symbols */
      free_pendings = 0;
      pending_blocks = 0;
      file_symbols = 0;
      global_symbols = 0;
      old_chain = make_cleanup (really_free_pendings, 0);

      /* Read in this files symbols */
      lseek (desc, sym_offset, L_SET);
      read_ofile_symtab (desc, stringtab, stringtab_size,
			 pst->ldsymoff,
			 pst->ldsymlen, pst->textlow,
			 pst->texthigh - pst->textlow, pst->addr);
      sort_symtab_syms (symtab_list); /* At beginning since just added */

      do_cleanups (old_chain);
    }

  pst->readin = 1;
}

/*
 * Read in all of the symbols for a given psymtab for real.
 * Be verbose about it if the user wants that.
 */
static void
dbx_psymtab_to_symtab (pst)
     struct partial_symtab *pst;
{
  int desc;
  char *stringtab;
  int stsize, val;
  struct stat statbuf;
  struct cleanup *old_chain;
  bfd *sym_bfd;
  long st_temp;

  if (!pst)
    return;

  if (pst->readin)
    {
      fprintf (stderr, "Psymtab for %s already read in.  Shouldn't happen.\n",
	       pst->filename);
      return;
    }

  if (pst->ldsymlen || pst->number_of_dependencies)
    {
      /* Print the message now, before reading the string table,
	 to avoid disconcerting pauses.  */
      if (info_verbose)
	{
	  printf_filtered ("Reading in symbols for %s...", pst->filename);
	  fflush (stdout);
	}

      /* Open symbol file and read in string table.  Symbol_file_command
	 guarantees that the symbol file name will be absolute, so there is
	 no need for openp.  */
      desc = open(pst->symfile_name, O_RDONLY, 0);

      if (desc < 0)
	perror_with_name (pst->symfile_name);

      sym_bfd = bfd_fdopenr (pst->symfile_name, NULL, desc);
      if (!sym_bfd)
	{
	  (void)close (desc);
	  error ("Could not open `%s' to read symbols: %s",
		 pst->symfile_name, bfd_errmsg (bfd_error));
	}
      old_chain = make_cleanup (bfd_close, sym_bfd);
      if (!bfd_check_format (sym_bfd, bfd_object))
	  error ("\"%s\": can't read symbols: %s.",
		 pst->symfile_name, bfd_errmsg (bfd_error));

      /* We keep the string table for symfile resident in memory, but
	 not the string table for any other symbol files.  */
      if ((symfile == 0) || 0 != strcmp(pst->symfile_name, symfile))
	{
	  /* Read in the string table */

	  /* FIXME, this uses internal BFD variables.  See above in
	     dbx_symbol_file_open where the macro is defined!  */
	  lseek (desc, STRING_TABLE_OFFSET, L_SET);

	  val = myread (desc, &st_temp, sizeof st_temp);
	  if (val < 0)
	      perror_with_name (pst->symfile_name);
	  stsize = bfd_h_get_32 (sym_bfd, (unsigned char *)&st_temp);
	  if (fstat (desc, &statbuf) < 0)
	    perror_with_name (pst->symfile_name);
	  
	  if (stsize >= 0 && stsize < statbuf.st_size)
	    {
#ifdef BROKEN_LARGE_ALLOCA
	      stringtab = (char *) xmalloc (stsize);
	      make_cleanup (free, stringtab);
#else
	      stringtab = (char *) alloca (stsize);
#endif
	    }
	  else
	    stringtab = NULL;
	  if (stringtab == NULL && stsize != 0)
	    error ("ridiculous string table size: %d bytes", stsize);

	  /* FIXME, this uses internal BFD variables.  See above in
	     dbx_symbol_file_open where the macro is defined!  */
	  val = lseek (desc, STRING_TABLE_OFFSET, L_SET);
	  if (val < 0)
	    perror_with_name (pst->symfile_name);
	  val = myread (desc, stringtab, stsize);
	  if (val < 0)
	    perror_with_name (pst->symfile_name);
	}
      else
	{
	  stringtab = symfile_string_table;
	  stsize = symfile_string_table_size;
	}

      symfile_bfd = sym_bfd;		/* Kludge for SWAP_SYMBOL */

      /* FIXME, this uses internal BFD variables.  See above in
	 dbx_symbol_file_open where the macro is defined!  */
      psymtab_to_symtab_1 (pst, desc, stringtab, stsize,
			   SYMBOL_TABLE_OFFSET);

      /* Match with global symbols.  This only needs to be done once,
         after all of the symtabs and dependencies have been read in.   */
      scan_file_globals ();

      do_cleanups (old_chain);

      /* Finish up the debug error message.  */
      if (info_verbose)
	printf_filtered ("done.\n");
    }
}

/*
 * Scan through all of the global symbols defined in the object file,
 * assigning values to the debugging symbols that need to be assigned
 * to.  Get these symbols from the misc function list.
 */
static void
scan_file_globals ()
{
  int hash;
  int mf;

  for (mf = 0; mf < misc_function_count; mf++)
    {
      char *namestring = misc_function_vector[mf].name;
      struct symbol *sym, *prev;

      QUIT;

      prev = (struct symbol *) 0;

      /* Get the hash index and check all the symbols
	 under that hash index. */

      hash = hashname (namestring);

      for (sym = global_sym_chain[hash]; sym;)
	{
	  if (*namestring == SYMBOL_NAME (sym)[0]
	      && !strcmp(namestring + 1, SYMBOL_NAME (sym) + 1))
	    {
	      /* Splice this symbol out of the hash chain and
		 assign the value we have to it. */
	      if (prev)
		SYMBOL_VALUE_CHAIN (prev) = SYMBOL_VALUE_CHAIN (sym);
	      else
		global_sym_chain[hash] = SYMBOL_VALUE_CHAIN (sym);
	      
	      /* Check to see whether we need to fix up a common block.  */
	      /* Note: this code might be executed several times for
		 the same symbol if there are multiple references.  */
	      if (SYMBOL_CLASS (sym) == LOC_BLOCK)
		fix_common_block (sym, misc_function_vector[mf].address);
	      else
		SYMBOL_VALUE_ADDRESS (sym) = misc_function_vector[mf].address;
	      
	      if (prev)
		sym = SYMBOL_VALUE_CHAIN (prev);
	      else
		sym = global_sym_chain[hash];
	    }
	  else
	    {
	      prev = sym;
	      sym = SYMBOL_VALUE_CHAIN (sym);
	    }
	}
    }
}

/* Process a pair of symbols.  Currently they must both be N_SO's.  */
static void
process_symbol_pair (type1, desc1, value1, name1,
		     type2, desc2, value2, name2)
     int type1;
     int desc1;
     CORE_ADDR value1;
     char *name1;
     int type2;
     int desc2;
     CORE_ADDR value2;
     char *name2;
{
  /* No need to check PCC_SOL_BROKEN, on the assumption that such
     broken PCC's don't put out N_SO pairs.  */
  if (last_source_file)
    end_symtab (value2);
  start_symtab (name2, name1, value2);
}

/*
 * Read in a defined section of a specific object file's symbols.
 *
 * DESC is the file descriptor for the file, positioned at the
 * beginning of the symtab
 * STRINGTAB is a pointer to the files string
 * table, already read in
 * SYM_OFFSET is the offset within the file of
 * the beginning of the symbols we want to read, NUM_SUMBOLS is the
 * number of symbols to read
 * TEXT_OFFSET is the beginning of the text segment we are reading symbols for
 * TEXT_SIZE is the size of the text segment read in.
 * OFFSET is a relocation offset which gets added to each symbol
 */

static void
read_ofile_symtab (desc, stringtab, stringtab_size, sym_offset,
		   sym_size, text_offset, text_size, offset)
     int desc;
     register char *stringtab;
     unsigned int stringtab_size;
     int sym_offset;
     int sym_size;
     CORE_ADDR text_offset;
     int text_size;
     int offset;
{
  register char *namestring;
  struct nlist *bufp;
  unsigned char type;
  subfile_stack = 0;

  stringtab_global = stringtab;
  last_source_file = 0;

  symtab_input_desc = desc;
  symbuf_end = symbuf_idx = 0;

  /* It is necessary to actually read one symbol *before* the start
     of this symtab's symbols, because the GCC_COMPILED_FLAG_SYMBOL
     occurs before the N_SO symbol.

     Detecting this in read_dbx_symtab
     would slow down initial readin, so we look for it here instead.  */
  if (sym_offset >= (int)sizeof (struct nlist))
    {
      lseek (desc, sym_offset - sizeof (struct nlist), L_INCR);
      fill_symbuf ();
      bufp = &symbuf[symbuf_idx++];
      SWAP_SYMBOL (bufp);

      if (bufp->n_un.n_strx < 0 || bufp->n_un.n_strx >= stringtab_size)
	error ("Invalid symbol data: bad string table offset: %d",
	       bufp->n_un.n_strx);
      namestring = bufp->n_un.n_strx + stringtab;

      processing_gcc_compilation =
	(bufp->n_type == N_TEXT
	 && !strcmp (namestring, GCC_COMPILED_FLAG_SYMBOL));
    }
  else
    {
      /* The N_SO starting this symtab is the first symbol, so we
	 better not check the symbol before it.  I'm not this can
	 happen, but it doesn't hurt to check for it.  */
      lseek(desc, sym_offset, L_INCR);
      processing_gcc_compilation = 0;
    }

  if (symbuf_idx == symbuf_end)
    fill_symbuf();
  bufp = &symbuf[symbuf_idx];
  if (bufp->n_type != (unsigned char)N_SO)
    error("First symbol in segment of executable not a source symbol");

  for (symnum = 0;
       symnum < sym_size / sizeof(struct nlist);
       symnum++)
    {
      QUIT;			/* Allow this to be interruptable */
      if (symbuf_idx == symbuf_end)
	fill_symbuf();
      bufp = &symbuf[symbuf_idx++];
      SWAP_SYMBOL (bufp);

      type = bufp->n_type & N_TYPE;
      if (type == (unsigned char)N_CATCH)
	{
	  /* N_CATCH is not fixed up by the linker, and unfortunately,
	     there's no other place to put it in the .stab map.  */
	  bufp->n_value += text_offset + offset;
	}
      else if (type == N_TEXT || type == N_DATA || type == N_BSS)
	bufp->n_value += offset;

      type = bufp->n_type;
      if (bufp->n_un.n_strx < 0 || bufp->n_un.n_strx >= stringtab_size)
	error ("Invalid symbol data: bad string table offset: %d",
	       bufp->n_un.n_strx);
      namestring = bufp->n_un.n_strx + stringtab;

      if (type & N_STAB)
	{
	  short bufp_n_desc = bufp->n_desc;
	  unsigned long valu = bufp->n_value;

	  /* Check for a pair of N_SO symbols.  */
	  if (type == (unsigned char)N_SO)
	    {
	      if (symbuf_idx == symbuf_end)
		fill_symbuf ();
	      bufp = &symbuf[symbuf_idx];
	      if (bufp->n_type == (unsigned char)N_SO)
		{
		  char *namestring2;

		  SWAP_SYMBOL (bufp);
		  bufp->n_value += offset;		/* Relocate */
		  symbuf_idx++;
		  symnum++;

		  if (bufp->n_un.n_strx < 0
		      || bufp->n_un.n_strx >= stringtab_size)
		    error ("Invalid symbol data: bad string table offset: %d",
			   bufp->n_un.n_strx);
		  namestring2 = bufp->n_un.n_strx + stringtab;

		  process_symbol_pair (N_SO, bufp_n_desc, valu, namestring,
				       N_SO, bufp->n_desc, bufp->n_value,
				       namestring2);
		}
	      else
		process_one_symbol(type, bufp_n_desc, valu, namestring);
	    }
	  else
	    process_one_symbol (type, bufp_n_desc, valu, namestring);
	}
      /* We skip checking for a new .o or -l file; that should never
         happen in this routine. */
      else if (type == N_TEXT
	       && !strcmp (namestring, GCC_COMPILED_FLAG_SYMBOL))
	/* I don't think this code will ever be executed, because
	   the GCC_COMPILED_FLAG_SYMBOL usually is right before
	   the N_SO symbol which starts this source file.
	   However, there is no reason not to accept
	   the GCC_COMPILED_FLAG_SYMBOL anywhere.  */
	processing_gcc_compilation = 1;
      else if (type & N_EXT || type == (unsigned char)N_TEXT
	       || type == (unsigned char)N_NBTEXT
	       )
	  /* Global symbol: see if we came across a dbx defintion for
	     a corresponding symbol.  If so, store the value.  Remove
	     syms from the chain when their values are stored, but
	     search the whole chain, as there may be several syms from
	     different files with the same name. */
	  /* This is probably not true.  Since the files will be read
	     in one at a time, each reference to a global symbol will
	     be satisfied in each file as it appears. So we skip this
	     section. */
	  ;
    }
  end_symtab (text_offset + text_size);
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

  /* Ensure result is positive.  */
  if (total < 0) total += (1000 << 6);
  return total % HASHSIZE;
}


static void
process_one_symbol (type, desc, valu, name)
     int type, desc;
     CORE_ADDR valu;
     char *name;
{
#ifndef SUN_FIXED_LBRAC_BUG
  /* This records the last pc address we've seen.  We depend on their being
     an SLINE or FUN or SO before the first LBRAC, since the variable does
     not get reset in between reads of different symbol files.  */
  static CORE_ADDR last_pc_address;
#endif
  register struct context_stack *new;
  char *colon_pos;

  /* Something is wrong if we see real data before
     seeing a source file name.  */

  if (last_source_file == 0 && type != (unsigned char)N_SO)
    {
      /* Currently this ignores N_ENTRY on Gould machines, N_NSYM on machines
	 where that code is defined.  */
      if (IGNORE_SYMBOL (type))
	return;

      /* FIXME, this should not be an error, since it precludes extending
         the symbol table information in this way...  */
      error ("Invalid symbol data: does not start by identifying a source file.");
    }

  switch (type)
    {
    case N_FUN:
    case N_FNAME:
      /* Either of these types of symbols indicates the start of
	 a new function.  We must process its "name" normally for dbx,
	 but also record the start of a new lexical context, and possibly
	 also the end of the lexical context for the previous function.  */
      /* This is not always true.  This type of symbol may indicate a
         text segment variable.  */

#ifndef SUN_FIXED_LBRAC_BUG
      last_pc_address = valu;	/* Save for SunOS bug circumcision */
#endif

      colon_pos = strchr (name, ':');
      if (!colon_pos++
	  || (*colon_pos != 'f' && *colon_pos != 'F'))
	{
	  define_symbol (valu, name, desc, type);
	  break;
	}

      within_function = 1;
      if (context_stack_depth > 0)
	{
	  new = &context_stack[--context_stack_depth];
	  /* Make a block for the local symbols within.  */
	  finish_block (new->name, &local_symbols, new->old_blocks,
			new->start_addr, valu);
	}
      /* Stack must be empty now.  */
      if (context_stack_depth != 0)
	error ("Invalid symbol data: unmatched N_LBRAC before symtab pos %d.",
	       symnum);

      new = &context_stack[context_stack_depth++];
      new->old_blocks = pending_blocks;
      new->start_addr = valu;
      new->name = define_symbol (valu, name, desc, type);
      local_symbols = 0;
      break;

    case N_CATCH:
      /* Record the address at which this catch takes place.  */
      define_symbol (valu, name, desc, type);
      break;

    case N_EHDECL:
      /* Don't know what to do with these yet.  */
      error ("action uncertain for eh extensions");
      break;

    case N_LBRAC:
      /* This "symbol" just indicates the start of an inner lexical
	 context within a function.  */

#if !defined (BLOCK_ADDRESS_ABSOLUTE)
      /* On most machines, the block addresses are relative to the
	 N_SO, the linker did not relocate them (sigh).  */
      valu += last_source_start_addr;
#endif

#ifndef SUN_FIXED_LBRAC_BUG
      if (valu < last_pc_address) {
	/* Patch current LBRAC pc value to match last handy pc value */
 	complain (&lbrac_complaint, 0);
	valu = last_pc_address;
      }
#endif
      if (context_stack_depth == context_stack_size)
	{
	  context_stack_size *= 2;
	  context_stack = (struct context_stack *)
	    xrealloc (context_stack,
		      (context_stack_size
		       * sizeof (struct context_stack)));
	}

      new = &context_stack[context_stack_depth++];
      new->depth = desc;
      new->locals = local_symbols;
      new->old_blocks = pending_blocks;
      new->start_addr = valu;
      new->name = 0;
      local_symbols = 0;
      break;

    case N_RBRAC:
      /* This "symbol" just indicates the end of an inner lexical
	 context that was started with N_LBRAC.  */

#if !defined (BLOCK_ADDRESS_ABSOLUTE)
      /* On most machines, the block addresses are relative to the
	 N_SO, the linker did not relocate them (sigh).  */
      valu += last_source_start_addr;
#endif

      new = &context_stack[--context_stack_depth];
      if (desc != new->depth)
	error ("Invalid symbol data: N_LBRAC/N_RBRAC symbol mismatch, symtab pos %d.", symnum);

      /* Some compilers put the variable decls inside of an
         LBRAC/RBRAC block.  This macro should be nonzero if this
	 is true.  DESC is N_DESC from the N_RBRAC symbol.
	 GCC_P is true if we've detected the GCC_COMPILED_SYMBOL.  */
#if !defined (VARIABLES_INSIDE_BLOCK)
#define VARIABLES_INSIDE_BLOCK(desc, gcc_p) 0
#endif

      /* Can only use new->locals as local symbols here if we're in
         gcc or on a machine that puts them before the lbrack.  */
      if (!VARIABLES_INSIDE_BLOCK(desc, processing_gcc_compilation))
	local_symbols = new->locals;

      /* If this is not the outermost LBRAC...RBRAC pair in the
	 function, its local symbols preceded it, and are the ones
	 just recovered from the context stack.  Defined the block for them.

	 If this is the outermost LBRAC...RBRAC pair, there is no
	 need to do anything; leave the symbols that preceded it
	 to be attached to the function's own block.  However, if
	 it is so, we need to indicate that we just moved outside
	 of the function.  */
      if (local_symbols
	  && (context_stack_depth
	      > !VARIABLES_INSIDE_BLOCK(desc, processing_gcc_compilation)))
	{
	  /* FIXME Muzzle a compiler bug that makes end < start.  */
	  if (new->start_addr > valu)
	    {
	      complain(&lbrac_rbrac_complaint, 0);
	      new->start_addr = valu;
	    }
	  /* Make a block for the local symbols within.  */
	  finish_block (0, &local_symbols, new->old_blocks,
			new->start_addr, valu);
	}
      else
	{
	  within_function = 0;
	}
      if (VARIABLES_INSIDE_BLOCK(desc, processing_gcc_compilation))
	/* Now pop locals of block just finished.  */
	local_symbols = new->locals;
      break;

    case N_FN | N_EXT:
      /* This kind of symbol supposedly indicates the start
	 of an object file.  In fact this type does not appear.  */
      break;

    case N_SO:
      /* This type of symbol indicates the start of data
	 for one source file.
	 Finish the symbol table of the previous source file
	 (if any) and start accumulating a new symbol table.  */
#ifndef SUN_FIXED_LBRAC_BUG
      last_pc_address = valu;	/* Save for SunOS bug circumcision */
#endif
  
#ifdef PCC_SOL_BROKEN
      /* pcc bug, occasionally puts out SO for SOL.  */
      if (context_stack_depth > 0)
	{
	  start_subfile (name, NULL);
	  break;
	}
#endif
      if (last_source_file)
	end_symtab (valu);
      start_symtab (name, NULL, valu);
      break;

    case N_SOL:
      /* This type of symbol indicates the start of data for
	 a sub-source-file, one whose contents were copied or
	 included in the compilation of the main source file
	 (whose name was given in the N_SO symbol.)  */
      start_subfile (name, NULL);
      break;

    case N_BINCL:
      push_subfile ();
      add_new_header_file (name, valu);
      start_subfile (name, NULL);
      break;

    case N_EINCL:
      start_subfile (pop_subfile (), NULL);
      break;

    case N_EXCL:
      add_old_header_file (name, valu);
      break;

    case N_SLINE:
      /* This type of "symbol" really just records
	 one line-number -- core-address correspondence.
	 Enter it in the line list for this symbol table.  */
#ifndef SUN_FIXED_LBRAC_BUG
      last_pc_address = valu;	/* Save for SunOS bug circumcision */
#endif
      record_line (desc, valu);
      break;

    case N_BCOMM:
      if (common_block)
	error ("Invalid symbol data: common within common at symtab pos %d",
	       symnum);
      common_block = local_symbols;
      common_block_i = local_symbols ? local_symbols->nsyms : 0;
      break;

    case N_ECOMM:
      /* Symbols declared since the BCOMM are to have the common block
	 start address added in when we know it.  common_block points to
	 the first symbol after the BCOMM in the local_symbols list;
	 copy the list and hang it off the symbol for the common block name
	 for later fixup.  */
      {
	int i;
	struct symbol *sym =
	  (struct symbol *) xmalloc (sizeof (struct symbol));
	bzero (sym, sizeof *sym);
	SYMBOL_NAME (sym) = savestring (name, strlen (name));
	SYMBOL_CLASS (sym) = LOC_BLOCK;
	SYMBOL_NAMESPACE (sym) = (enum namespace)((long)
	  copy_pending (local_symbols, common_block_i, common_block));
	i = hashname (SYMBOL_NAME (sym));
	SYMBOL_VALUE_CHAIN (sym) = global_sym_chain[i];
	global_sym_chain[i] = sym;
	common_block = 0;
	break;
      }

    case N_ECOML:
    case N_LENG:
      break;

    default:
      if (name)
	define_symbol (valu, name, desc, type);
    }
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

/* To handle GNU C++ typename abbreviation, we need to be able to
   fill in a type's name as soon as space for that type is allocated.
   `type_synonym_name' is the name of the type being allocated.
   It is cleared as soon as it is used (lest all allocated types
   get this name).  */
static char *type_synonym_name;

static struct symbol *
define_symbol (valu, string, desc, type)
     unsigned int valu;
     char *string;
     int desc;
     int type;
{
  register struct symbol *sym;
  char *p = (char *) strchr (string, ':');
  int deftype;
  int synonym = 0;
  register int i;

  /* Ignore syms with empty names.  */
  if (string[0] == 0)
    return 0;

  /* Ignore old-style symbols from cc -go  */
  if (p == 0)
    return 0;

  sym = (struct symbol *)obstack_alloc (symbol_obstack, sizeof (struct symbol));

  if (processing_gcc_compilation) {
    /* GCC 2.x puts the line number in desc.  SunOS apparently puts in the
       number of bytes occupied by a type or object, which we ignore.  */
    SYMBOL_LINE(sym) = desc;
  } else {
    SYMBOL_LINE(sym) = 0;			/* unknown */
  }

  if (string[0] == CPLUS_MARKER)
    {
      /* Special GNU C++ names.  */
      switch (string[1])
	{
	case 't':
	  SYMBOL_NAME (sym) = "this";
	  break;
	case 'v': /* $vtbl_ptr_type */
	  /* Was: SYMBOL_NAME (sym) = "vptr"; */
	  goto normal;
	case 'e':
	  SYMBOL_NAME (sym) = "eh_throw";
	  break;

	case '_':
	  /* This was an anonymous type that was never fixed up.  */
	  goto normal;

	default:
	  abort ();
	}
    }
  else
    {
    normal:
      SYMBOL_NAME (sym)
	= (char *) obstack_alloc (symbol_obstack, ((p - string) + 1));
      /* Open-coded bcopy--saves function call time.  */
      {
	register char *p1 = string;
	register char *p2 = SYMBOL_NAME (sym);
	while (p1 != p)
	  *p2++ = *p1++;
	*p2++ = '\0';
      }
    }
  p++;
  /* Determine the type of name being defined.  */
  /* The Acorn RISC machine's compiler can put out locals that don't
     start with "234=" or "(3,4)=", so assume anything other than the
     deftypes we know how to handle is a local.  */
  /* (Peter Watkins @ Computervision)
     Handle Sun-style local fortran array types 'ar...' . 
     (gnu@cygnus.com) -- this strchr() handles them properly?
     (tiemann@cygnus.com) -- 'C' is for catch.  */
  if (!strchr ("cfFGpPrStTvVXC", *p))
    deftype = 'l';
  else
    deftype = *p++;

  /* c is a special case, not followed by a type-number.
     SYMBOL:c=iVALUE for an integer constant symbol.
     SYMBOL:c=rVALUE for a floating constant symbol.
     SYMBOL:c=eTYPE,INTVALUE for an enum constant symbol.
        e.g. "b:c=e6,0" for "const b = blob1"
	(where type 6 is defined by "blobs:t6=eblob1:0,blob2:1,;").  */
  if (deftype == 'c')
    {
      if (*p++ != '=')
	error ("Invalid symbol data at symtab pos %d.", symnum);
      switch (*p++)
	{
	case 'r':
	  {
	    double d = atof (p);
	    char *dbl_valu;

	    SYMBOL_TYPE (sym) = builtin_type_double;
	    dbl_valu =
	      (char *) obstack_alloc (symbol_obstack, sizeof (double));
	    bcopy (&d, dbl_valu, sizeof (double));
	    SWAP_TARGET_AND_HOST (dbl_valu, sizeof (double));
	    SYMBOL_VALUE_BYTES (sym) = dbl_valu;
	    SYMBOL_CLASS (sym) = LOC_CONST_BYTES;
	  }
	  break;
	case 'i':
	  {
	    SYMBOL_TYPE (sym) = builtin_type_int;
	    SYMBOL_VALUE (sym) = atoi (p);
	    SYMBOL_CLASS (sym) = LOC_CONST;
	  }
	  break;
	case 'e':
	  /* SYMBOL:c=eTYPE,INTVALUE for an enum constant symbol.
	     e.g. "b:c=e6,0" for "const b = blob1"
	     (where type 6 is defined by "blobs:t6=eblob1:0,blob2:1,;").  */
	  {
	    int typenums[2];
	    
	    read_type_number (&p, typenums);
	    if (*p++ != ',')
	      error ("Invalid symbol data: no comma in enum const symbol");
	    
	    SYMBOL_TYPE (sym) = *dbx_lookup_type (typenums);
	    SYMBOL_VALUE (sym) = atoi (p);
	    SYMBOL_CLASS (sym) = LOC_CONST;
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
    {
      struct type *type_read;
      synonym = *p == 't';

      if (synonym)
	{
	  p += 1;
	  type_synonym_name = obsavestring (SYMBOL_NAME (sym),
					    strlen (SYMBOL_NAME (sym)));
	}

      type_read = read_type (&p);

      if ((deftype == 'F' || deftype == 'f')
	  && TYPE_CODE (type_read) != TYPE_CODE_FUNC)
	SYMBOL_TYPE (sym) = lookup_function_type (type_read);
      else
	SYMBOL_TYPE (sym) = type_read;
    }

  switch (deftype)
    {
    case 'C':
      /* The name of a caught exception.  */
      SYMBOL_CLASS (sym) = LOC_LABEL;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      SYMBOL_VALUE_ADDRESS (sym) = valu;
      add_symbol_to_list (sym, &local_symbols);
      break;

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
      SYMBOL_VALUE_CHAIN (sym) = global_sym_chain[i];
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
      SYMBOL_VALUE (sym) = valu;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      break;

    case 'p':
      /* Normally this is a parameter, a LOC_ARG.  On the i960, it
	 can also be a LOC_LOCAL_ARG depending on symbol type.  */
#ifndef DBX_PARM_SYMBOL_CLASS
#define	DBX_PARM_SYMBOL_CLASS(type)	LOC_ARG
#endif
      SYMBOL_CLASS (sym) = DBX_PARM_SYMBOL_CLASS (type);
      SYMBOL_VALUE (sym) = valu;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);

      /* If it's gcc-compiled, if it says `short', believe it.  */
      if (processing_gcc_compilation || BELIEVE_PCC_PROMOTION)
	break;

#if defined(BELIEVE_PCC_PROMOTION_TYPE)
      /* This macro is defined on machines (e.g. sparc) where
	 we should believe the type of a PCC 'short' argument,
	 but shouldn't believe the address (the address is
	 the address of the corresponding int).  Note that
	 this is only different from the BELIEVE_PCC_PROMOTION
	 case on big-endian machines.

	 My guess is that this correction, as opposed to changing
	 the parameter to an 'int' (as done below, for PCC
	 on most machines), is the right thing to do
	 on all machines, but I don't want to risk breaking
	 something that already works.  On most PCC machines,
	 the sparc problem doesn't come up because the calling
	 function has to zero the top bytes (not knowing whether
	 the called function wants an int or a short), so there
	 is no practical difference between an int and a short
	 (except perhaps what happens when the GDB user types
	 "print short_arg = 0x10000;"). 

	 Hacked for SunOS 4.1 by gnu@cygnus.com.  In 4.1, the compiler
	 actually produces the correct address (we don't need to fix it
	 up).  I made this code adapt so that it will offset the symbol
	 if it was pointing at an int-aligned location and not
	 otherwise.  This way you can use the same gdb for 4.0.x and
	 4.1 systems.  */

      if (0 == SYMBOL_VALUE (sym) % sizeof (int))
	{
	  if (SYMBOL_TYPE (sym) == builtin_type_char
	      || SYMBOL_TYPE (sym) == builtin_type_unsigned_char)
	    SYMBOL_VALUE (sym) += 3;
	  else if (SYMBOL_TYPE (sym) == builtin_type_short
	      || SYMBOL_TYPE (sym) == builtin_type_unsigned_short)
	    SYMBOL_VALUE (sym) += 2;
	}
      break;

#else /* no BELIEVE_PCC_PROMOTION_TYPE.  */

      /* If PCC says a parameter is a short or a char,
	 it is really an int.  */
      if (SYMBOL_TYPE (sym) == builtin_type_char
	  || SYMBOL_TYPE (sym) == builtin_type_short)
	SYMBOL_TYPE (sym) = builtin_type_int;
      else if (SYMBOL_TYPE (sym) == builtin_type_unsigned_char
	       || SYMBOL_TYPE (sym) == builtin_type_unsigned_short)
	SYMBOL_TYPE (sym) = builtin_type_unsigned_int;
      break;

#endif /* no BELIEVE_PCC_PROMOTION_TYPE.  */

    case 'P':
      SYMBOL_CLASS (sym) = LOC_REGPARM;
      SYMBOL_VALUE (sym) = STAB_REG_TO_REGNUM (valu);
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      break;

    case 'r':
      SYMBOL_CLASS (sym) = LOC_REGISTER;
      SYMBOL_VALUE (sym) = STAB_REG_TO_REGNUM (valu);
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      break;

    case 'S':
      /* Static symbol at top level of file */
      SYMBOL_CLASS (sym) = LOC_STATIC;
      SYMBOL_VALUE_ADDRESS (sym) = valu;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &file_symbols);
      break;

    case 't':
      SYMBOL_CLASS (sym) = LOC_TYPEDEF;
      SYMBOL_VALUE (sym) = valu;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      if (TYPE_NAME (SYMBOL_TYPE (sym)) == 0
	  && (TYPE_FLAGS (SYMBOL_TYPE (sym)) & TYPE_FLAG_PERM) == 0)
	TYPE_NAME (SYMBOL_TYPE (sym)) =
	  obsavestring (SYMBOL_NAME (sym),
			strlen (SYMBOL_NAME (sym)));
       /* C++ vagaries: we may have a type which is derived from
 	 a base type which did not have its name defined when the
 	 derived class was output.  We fill in the derived class's
 	 base part member's name here in that case.  */
       else if ((TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_STRUCT
		 || TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_UNION)
		&& TYPE_N_BASECLASSES (SYMBOL_TYPE (sym)))
	 {
	   int i;
	   for (i = TYPE_N_BASECLASSES (SYMBOL_TYPE (sym)) - 1; i >= 0; i--)
	     if (TYPE_BASECLASS_NAME (SYMBOL_TYPE (sym), i) == 0)
	       TYPE_BASECLASS_NAME (SYMBOL_TYPE (sym), i) =
		 type_name_no_tag (TYPE_BASECLASS (SYMBOL_TYPE (sym), i));
	 }

      add_symbol_to_list (sym, &file_symbols);
      break;

    case 'T':
      SYMBOL_CLASS (sym) = LOC_TYPEDEF;
      SYMBOL_VALUE (sym) = valu;
      SYMBOL_NAMESPACE (sym) = STRUCT_NAMESPACE;
      if (TYPE_NAME (SYMBOL_TYPE (sym)) == 0
	  && (TYPE_FLAGS (SYMBOL_TYPE (sym)) & TYPE_FLAG_PERM) == 0)
	TYPE_NAME (SYMBOL_TYPE (sym))
	  = obconcat ("",
		      (TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_ENUM
		       ? "enum "
		       : (TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_STRUCT
			  ? "struct " : "union ")),
		      SYMBOL_NAME (sym));
      add_symbol_to_list (sym, &file_symbols);

      if (synonym)
	{
	  register struct symbol *typedef_sym
	    = (struct symbol *) obstack_alloc (symbol_obstack, sizeof (struct symbol));
	  SYMBOL_NAME (typedef_sym) = SYMBOL_NAME (sym);
	  SYMBOL_TYPE (typedef_sym) = SYMBOL_TYPE (sym);

	  SYMBOL_CLASS (typedef_sym) = LOC_TYPEDEF;
	  SYMBOL_VALUE (typedef_sym) = valu;
	  SYMBOL_NAMESPACE (typedef_sym) = VAR_NAMESPACE;
	  add_symbol_to_list (typedef_sym, &file_symbols);
	}
      break;

    case 'V':
      /* Static symbol of local scope */
      SYMBOL_CLASS (sym) = LOC_STATIC;
      SYMBOL_VALUE_ADDRESS (sym) = valu;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      break;

    case 'v':
      /* Reference parameter */
      SYMBOL_CLASS (sym) = LOC_REF_ARG;
      SYMBOL_VALUE (sym) = valu;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      break;

    case 'X':
      /* This is used by Sun FORTRAN for "function result value".
	 Sun claims ("dbx and dbxtool interfaces", 2nd ed)
	 that Pascal uses it too, but when I tried it Pascal used
	 "x:3" (local symbol) instead.  */
      SYMBOL_CLASS (sym) = LOC_LOCAL;
      SYMBOL_VALUE (sym) = valu;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      break;

    default:
      error ("Invalid symbol data: unknown symbol-type code `%c' at symtab pos %d.", deftype, symnum);
    }
  return sym;
}

/* What about types defined as forward references inside of a small lexical
   scope?  */
/* Add a type to the list of undefined types to be checked through
   once this file has been read in.  */
static void
add_undefined_type (type)
     struct type *type;
{
  if (undef_types_length == undef_types_allocated)
    {
      undef_types_allocated *= 2;
      undef_types = (struct type **)
	xrealloc (undef_types,
		  undef_types_allocated * sizeof (struct type *));
    }
  undef_types[undef_types_length++] = type;
}

/* Add here something to go through each undefined type, see if it's
   still undefined, and do a full lookup if so.  */
static void
cleanup_undefined_types ()
{
  struct type **type;

  for (type = undef_types; type < undef_types + undef_types_length; type++)
    {
      /* Reasonable test to see if it's been defined since.  */
      if (TYPE_NFIELDS (*type) == 0)
	{
	  struct pending *ppt;
	  int i;
	  /* Name of the type, without "struct" or "union" */
	  char *typename = TYPE_NAME (*type);

	  if (!strncmp (typename, "struct ", 7))
	    typename += 7;
	  if (!strncmp (typename, "union ", 6))
	    typename += 6;

	  for (ppt = file_symbols; ppt; ppt = ppt->next)
	    for (i = 0; i < ppt->nsyms; i++)
	      {
		struct symbol *sym = ppt->symbol[i];

		if (SYMBOL_CLASS (sym) == LOC_TYPEDEF
		    && SYMBOL_NAMESPACE (sym) == STRUCT_NAMESPACE
		    && (TYPE_CODE (SYMBOL_TYPE (sym)) ==
			TYPE_CODE (*type))
		    && !strcmp (SYMBOL_NAME (sym), typename))
		  bcopy (SYMBOL_TYPE (sym), *type, sizeof (struct type));
	      }
	}
      else
	/* It has been defined; don't mark it as a stub.  */
	TYPE_FLAGS (*type) &= ~TYPE_FLAG_STUB;
    }
  undef_types_length = 0;
}

/* Skip rest of this symbol and return an error type.

   General notes on error recovery:  error_type always skips to the
   end of the symbol (modulo cretinous dbx symbol name continuation).
   Thus code like this:

   if (*(*pp)++ != ';')
     return error_type (pp);

   is wrong because if *pp starts out pointing at '\0' (typically as the
   result of an earlier error), it will be incremented to point to the
   start of the next symbol, which might produce strange results, at least
   if you run off the end of the string table.  Instead use

   if (**pp != ';')
     return error_type (pp);
   ++*pp;

   or

   if (**pp != ';')
     foo = error_type (pp);
   else
     ++*pp;

   And in case it isn't obvious, the point of all this hair is so the compiler
   can define new types and new syntaxes, and old versions of the
   debugger will be able to read the new symbol tables.  */

static struct type *
error_type (pp)
     char **pp;
{
  complain (&error_type_complaint, 0);
  while (1)
    {
      /* Skip to end of symbol.  */
      while (**pp != '\0')
	(*pp)++;

      /* Check for and handle cretinous dbx symbol name continuation!  */
      if ((*pp)[-1] == '\\')
	*pp = next_symbol_text ();
      else
	break;
    }
  return builtin_type_error;
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
  struct type *type1;
  int typenums[2];
  int xtypenums[2];

  /* Read type number if present.  The type number may be omitted.
     for instance in a two-dimensional array declared with type
     "ar1;1;10;ar1;1;10;4".  */
  if ((**pp >= '0' && **pp <= '9')
      || **pp == '(')
    {
      read_type_number (pp, typenums);
      
      /* Detect random reference to type not yet defined.
	 Allocate a type object but leave it zeroed.  */
      if (**pp != '=')
	return dbx_alloc_type (typenums);

      *pp += 2;
    }
  else
    {
      /* 'typenums=' not present, type is anonymous.  Read and return
	 the definition, but don't put it in the type vector.  */
      typenums[0] = typenums[1] = -1;
      *pp += 1;
    }
      
  switch ((*pp)[-1])
    {
    case 'x':
      {
	enum type_code code;

	/* Used to index through file_symbols.  */
	struct pending *ppt;
	int i;
	
	/* Name including "struct", etc.  */
	char *type_name;
	
	/* Name without "struct", etc.  */
	char *type_name_only;

	{
	  char *prefix;
	  char *from, *to;
	  
	  /* Set the type code according to the following letter.  */
	  switch ((*pp)[0])
	    {
	    case 's':
	      code = TYPE_CODE_STRUCT;
	      prefix = "struct ";
	      break;
	    case 'u':
	      code = TYPE_CODE_UNION;
	      prefix = "union ";
	      break;
	    case 'e':
	      code = TYPE_CODE_ENUM;
	      prefix = "enum ";
	      break;
	    default:
	      return error_type (pp);
	    }
	  
	  to = type_name = (char *)
	    obstack_alloc (symbol_obstack,
			   (strlen (prefix) +
			    ((char *) strchr (*pp, ':') - (*pp)) + 1));
	
	  /* Copy the prefix.  */
	  from = prefix;
	  while (*to++ = *from++)
	    ;
	  to--; 
	
	  type_name_only = to;

	  /* Copy the name.  */
	  from = *pp + 1;
	  while ((*to++ = *from++) != ':')
	    ;
	  *--to = '\0';
	  
	  /* Set the pointer ahead of the name which we just read.  */
	  *pp = from;
	
#if 0
	  /* The following hack is clearly wrong, because it doesn't
	     check whether we are in a baseclass.  I tried to reproduce
	     the case that it is trying to fix, but I couldn't get
	     g++ to put out a cross reference to a basetype.  Perhaps
	     it doesn't do it anymore.  */
	  /* Note: for C++, the cross reference may be to a base type which
	     has not yet been seen.  In this case, we skip to the comma,
	     which will mark the end of the base class name.  (The ':'
	     at the end of the base class name will be skipped as well.)
	     But sometimes (ie. when the cross ref is the last thing on
	     the line) there will be no ','.  */
	  from = (char *) strchr (*pp, ',');
	  if (from)
	    *pp = from;
#endif /* 0 */
	}

	/* Now check to see whether the type has already been declared.  */
	/* This is necessary at least in the case where the
	   program says something like
	     struct foo bar[5];
	   The compiler puts out a cross-reference; we better find
	   set the length of the structure correctly so we can
	   set the length of the array.  */
	for (ppt = file_symbols; ppt; ppt = ppt->next)
	  for (i = 0; i < ppt->nsyms; i++)
	    {
	      struct symbol *sym = ppt->symbol[i];

	      if (SYMBOL_CLASS (sym) == LOC_TYPEDEF
		  && SYMBOL_NAMESPACE (sym) == STRUCT_NAMESPACE
		  && (TYPE_CODE (SYMBOL_TYPE (sym)) == code)
		  && !strcmp (SYMBOL_NAME (sym), type_name_only))
		{
		  obstack_free (symbol_obstack, type_name);
		  type = SYMBOL_TYPE (sym);
		  return type;
		}
	    }
	
	/* Didn't find the type to which this refers, so we must
	   be dealing with a forward reference.  Allocate a type
	   structure for it, and keep track of it so we can
	   fill in the rest of the fields when we get the full
	   type.  */
	type = dbx_alloc_type (typenums);
	TYPE_CODE (type) = code;
	TYPE_NAME (type) = type_name;

	TYPE_FLAGS (type) |= TYPE_FLAG_STUB;

	add_undefined_type (type);
	return type;
      }

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
      if (typenums[0] != -1)
	*dbx_lookup_type (typenums) = type;
      break;

    case '*':
      type1 = read_type (pp);
      type = lookup_pointer_type (type1);
      if (typenums[0] != -1)
	*dbx_lookup_type (typenums) = type;
      break;

    case '@':
      {
	struct type *domain = read_type (pp);
	struct type *memtype;

	if (**pp != ',')
	  /* Invalid member type data format.  */
	  return error_type (pp);
	++*pp;

	memtype = read_type (pp);
	type = dbx_alloc_type (typenums);
	smash_to_member_type (type, domain, memtype);
      }
      break;

    case '#':
      if ((*pp)[0] == '#')
	{
	  /* We'll get the parameter types from the name.  */
	  struct type *return_type;

	  *pp += 1;
	  return_type = read_type (pp);
	  if (*(*pp)++ != ';')
	    complain (&invalid_member_complaint, symnum);
	  type = allocate_stub_method (return_type);
	  if (typenums[0] != -1)
	    *dbx_lookup_type (typenums) = type;
	}
      else
	{
	  struct type *domain = read_type (pp);
	  struct type *return_type;
	  struct type **args;

	  if (*(*pp)++ != ',')
	    error ("invalid member type data format, at symtab pos %d.",
		   symnum);

	  return_type = read_type (pp);
	  args = read_args (pp, ';');
	  type = dbx_alloc_type (typenums);
	  smash_to_method_type (type, domain, return_type, args);
	}
      break;

    case '&':
      type1 = read_type (pp);
      type = lookup_reference_type (type1);
      if (typenums[0] != -1)
	*dbx_lookup_type (typenums) = type;
      break;

    case 'f':
      type1 = read_type (pp);
      type = lookup_function_type (type1);
      if (typenums[0] != -1)
	*dbx_lookup_type (typenums) = type;
      break;

    case 'r':
      type = read_range_type (pp, typenums);
      if (typenums[0] != -1)
	*dbx_lookup_type (typenums) = type;
      break;

    case 'e':
      type = dbx_alloc_type (typenums);
      type = read_enum_type (pp, type);
      *dbx_lookup_type (typenums) = type;
      break;

    case 's':
      type = dbx_alloc_type (typenums);
      TYPE_NAME (type) = type_synonym_name;
      type_synonym_name = 0;
      type = read_struct_type (pp, type);
      break;

    case 'u':
      type = dbx_alloc_type (typenums);
      TYPE_NAME (type) = type_synonym_name;
      type_synonym_name = 0;
      type = read_struct_type (pp, type);
      TYPE_CODE (type) = TYPE_CODE_UNION;
      break;

    case 'a':
      if (**pp != 'r')
	return error_type (pp);
      ++*pp;
      
      type = dbx_alloc_type (typenums);
      type = read_array_type (pp, type);
      break;

    default:
      return error_type (pp);
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

#if 0
/* This would be a good idea, but it doesn't really work.  The problem
   is that in order to get the virtual context for a particular type,
   you need to know the virtual info from all of its basetypes,
   and you need to have processed its methods.  Since GDB reads
   symbols on a file-by-file basis, this means processing the symbols
   of all the files that are needed for each baseclass, which
   means potentially reading in all the debugging info just to fill
   in information we may never need.  */

/* This page contains subroutines of read_type.  */

/* FOR_TYPE is a struct type defining a virtual function NAME with type
   FN_TYPE.  The `virtual context' for this virtual function is the
   first base class of FOR_TYPE in which NAME is defined with signature
   matching FN_TYPE.  OFFSET serves as a hash on matches here.

   TYPE is the current type in which we are searching.  */

static struct type *
virtual_context (for_type, type, name, fn_type, offset)
     struct type *for_type, *type;
     char *name;
     struct type *fn_type;
     int offset;
{
  struct type *basetype = 0;
  int i;

  if (for_type != type)
    {
      /* Check the methods of TYPE.  */
      /* Need to do a check_stub_type here, but that breaks
	 things because we can get infinite regress.  */
      for (i = TYPE_NFN_FIELDS (type) - 1; i >= 0; --i)
	if (!strcmp (TYPE_FN_FIELDLIST_NAME (type, i), name))
	  break;
      if (i >= 0)
	{
	  int j = TYPE_FN_FIELDLIST_LENGTH (type, i);
	  struct fn_field *f = TYPE_FN_FIELDLIST1 (type, i);

	  while (--j >= 0)
	    if (TYPE_FN_FIELD_VOFFSET (f, j) == offset-1)
	      return TYPE_FN_FIELD_FCONTEXT (f, j);
	}
    }
  for (i = TYPE_N_BASECLASSES (type) - 1; i >= 0; i--)
    {
      basetype = virtual_context (for_type, TYPE_BASECLASS (type, i), name,
				  fn_type, offset);
      if (basetype != for_type)
	return basetype;
    }
  return for_type;
}
#endif

/* Read the description of a structure (or union type)
   and return an object describing the type.  */

static struct type *
read_struct_type (pp, type)
     char **pp;
     register struct type *type;
{
  /* Total number of methods defined in this class.
     If the class defines two `f' methods, and one `g' method,
     then this will have the value 3.  */
  int total_length = 0;

  struct nextfield
    {
      struct nextfield *next;
      int visibility;			/* 0=public, 1=protected, 2=public */
      struct field field;
    };

  struct next_fnfield
    {
      struct next_fnfield *next;
      int visibility;			/* 0=public, 1=protected, 2=public */
      struct fn_field fn_field;
    };

  struct next_fnfieldlist
    {
      struct next_fnfieldlist *next;
      struct fn_fieldlist fn_fieldlist;
    };

  register struct nextfield *list = 0;
  struct nextfield *new;
  register char *p;
  int nfields = 0;
  register int n;

  register struct next_fnfieldlist *mainlist = 0;
  int nfn_fields = 0;

  if (TYPE_MAIN_VARIANT (type) == 0)
    {
      TYPE_MAIN_VARIANT (type) = type;
    }

  TYPE_CODE (type) = TYPE_CODE_STRUCT;

  /* First comes the total size in bytes.  */

  TYPE_LENGTH (type) = read_number (pp, 0);

  /* C++: Now, if the class is a derived class, then the next character
     will be a '!', followed by the number of base classes derived from.
     Each element in the list contains visibility information,
     the offset of this base class in the derived structure,
     and then the base type. */
  if (**pp == '!')
    {
      int i, n_baseclasses, offset;
      struct type *baseclass;
      int via_public;

      /* Nonzero if it is a virtual baseclass, i.e.,

	 struct A{};
	 struct B{};
	 struct C : public B, public virtual A {};

	 B is a baseclass of C; A is a virtual baseclass for C.  This is a C++
	 2.0 language feature.  */
      int via_virtual;

      *pp += 1;

      n_baseclasses = read_number (pp, ',');
      TYPE_FIELD_VIRTUAL_BITS (type) =
	  (B_TYPE *) obstack_alloc (symbol_obstack, B_BYTES (n_baseclasses));
      B_CLRALL (TYPE_FIELD_VIRTUAL_BITS (type), n_baseclasses);

      for (i = 0; i < n_baseclasses; i++)
	{
	  if (**pp == '\\')
	    *pp = next_symbol_text ();

	  switch (**pp)
	    {
	    case '0':
	      via_virtual = 0;
	      break;
	    case '1':
	      via_virtual = 1;
	      break;
	    default:
	      /* Bad visibility format.  */
	      return error_type (pp);
	    }
	  ++*pp;

	  switch (**pp)
	    {
	    case '0':
	      via_public = 0;
	      break;
	    case '2':
	      via_public = 2;
	      break;
	    default:
	      /* Bad visibility format.  */
	      return error_type (pp);
	    }
	  if (via_virtual) 
	    SET_TYPE_FIELD_VIRTUAL (type, i);
	  ++*pp;

	  /* Offset of the portion of the object corresponding to
	     this baseclass.  Always zero in the absence of
	     multiple inheritance.  */
	  offset = read_number (pp, ',');
	  baseclass = read_type (pp);
	  *pp += 1;		/* skip trailing ';' */

#if 0
/* One's understanding improves, grasshopper... */
	  if (offset != 0)
	    {
	      static int error_printed = 0;

	      if (!error_printed)
		{
		  fprintf (stderr, 
"\nWarning:  GDB has limited understanding of multiple inheritance...");
		  if (!info_verbose)
		    fprintf(stderr, "\n");
		  error_printed = 1;
		}
	    }
#endif

	  /* Make this baseclass visible for structure-printing purposes.  */
	  new = (struct nextfield *) alloca (sizeof (struct nextfield));
	  new->next = list;
	  list = new;
	  list->visibility = via_public;
	  list->field.type = baseclass;
	  list->field.name = type_name_no_tag (baseclass);
	  list->field.bitpos = offset;
	  list->field.bitsize = 0;	/* this should be an unpacked field! */
	  nfields++;
	}
      TYPE_N_BASECLASSES (type) = n_baseclasses;
    }

  /* Now come the fields, as NAME:?TYPENUM,BITPOS,BITSIZE; for each one.
     At the end, we see a semicolon instead of a field.

     In C++, this may wind up being NAME:?TYPENUM:PHYSNAME; for
     a static field.

     The `?' is a placeholder for one of '/2' (public visibility),
     '/1' (protected visibility), '/0' (private visibility), or nothing
     (C style symbol table, public visibility).  */

  /* We better set p right now, in case there are no fields at all...    */
  p = *pp;

  while (**pp != ';')
    {
      /* Check for and handle cretinous dbx symbol name continuation!  */
      if (**pp == '\\') *pp = next_symbol_text ();

      /* Get space to record the next field's data.  */
      new = (struct nextfield *) alloca (sizeof (struct nextfield));
      new->next = list;
      list = new;

      /* Get the field name.  */
      p = *pp;
      if (*p == CPLUS_MARKER)
	{
	  /* Special GNU C++ name.  */
	  if (*++p == 'v')
	    {
	      const char *prefix;
	      char *name = 0;
	      struct type *context;

	      switch (*++p)
		{
		case 'f':
		  prefix = vptr_name;
		  break;
		case 'b':
		  prefix = vb_name;
		  break;
		default:
		  error ("invalid abbreviation at symtab pos %d.", symnum);
		}
	      *pp = p + 1;
	      context = read_type (pp);
	      if (type_name_no_tag (context) == 0)
		{
		  if (name == 0)
		    error ("type name unknown at symtab pos %d.", symnum);
		  /* FIXME-tiemann: when is `name' ever non-0?  */
		  TYPE_NAME (context) = obsavestring (name, p - name - 1);
		}
	      list->field.name = obconcat (prefix, type_name_no_tag (context), "");
	      p = ++(*pp);
	      if (p[-1] != ':')
		error ("invalid abbreviation at symtab pos %d.", symnum);
	      list->field.type = read_type (pp);
	      (*pp)++;			/* Skip the comma.  */
	      list->field.bitpos = read_number (pp, ';');
	      /* This field is unpacked.  */
	      list->field.bitsize = 0;
	    }
	  else
	    error ("invalid abbreviation at symtab pos %d.", symnum);

	  nfields++;
	  continue;
	}

      while (*p != ':') p++;
      list->field.name = obsavestring (*pp, p - *pp);

      /* C++: Check to see if we have hit the methods yet.  */
      if (p[1] == ':')
	break;

      *pp = p + 1;

      /* This means we have a visibility for a field coming. */
      if (**pp == '/')
	{
	  switch (*++*pp)
	    {
	    case '0':
	      list->visibility = 0;	/* private */
	      *pp += 1;
	      break;

 	    case '1':
 	      list->visibility = 1;	/* protected */
 	      *pp += 1;
 	      break;

 	    case '2':
 	      list->visibility = 2;	/* public */
 	      *pp += 1;
 	      break;
 	    }
 	}
       else /* normal dbx-style format.  */
	list->visibility = 2;		/* public */

      list->field.type = read_type (pp);
      if (**pp == ':')
 	{
	  /* Static class member.  */
 	  list->field.bitpos = (long)-1;
 	  p = ++(*pp);
 	  while (*p != ';') p++;
 	  list->field.bitsize = (long) savestring (*pp, p - *pp);
 	  *pp = p + 1;
 	  nfields++;
 	  continue;
 	}
       else if (**pp != ',')
	 /* Bad structure-type format.  */
	 return error_type (pp);

      (*pp)++;			/* Skip the comma.  */
      list->field.bitpos = read_number (pp, ',');
      list->field.bitsize = read_number (pp, ';');

#if 0
      /* FIXME-tiemann: Can't the compiler put out something which
	 lets us distinguish these? (or maybe just not put out anything
	 for the field).  What is the story here?  What does the compiler
	really do?  Also, patch gdb.texinfo for this case; I document
	it as a possible problem there.  Search for "DBX-style".  */

      /* This is wrong because this is identical to the symbols
	 produced for GCC 0-size arrays.  For example:
         typedef union {
	   int num;
	   char str[0];
	 } foo;
	 The code which dumped core in such circumstances should be
	 fixed not to dump core.  */

      /* g++ -g0 can put out bitpos & bitsize zero for a static
	 field.  This does not give us any way of getting its
	 class, so we can't know its name.  But we can just
	 ignore the field so we don't dump core and other nasty
	 stuff.  */
      if (list->field.bitpos == 0
	  && list->field.bitsize == 0)
	{
	  complain (&dbx_class_complaint, 0);
	  /* Ignore this field.  */
	  list = list->next;
	}
      else
#endif /* 0 */
	{
	  /* Detect an unpacked field and mark it as such.
	     dbx gives a bit size for all fields.
	     Note that forward refs cannot be packed,
	     and treat enums as if they had the width of ints.  */
	  if (TYPE_CODE (list->field.type) != TYPE_CODE_INT
	      && TYPE_CODE (list->field.type) != TYPE_CODE_ENUM)
	    list->field.bitsize = 0;
	  if ((list->field.bitsize == 8 * TYPE_LENGTH (list->field.type)
	       || (TYPE_CODE (list->field.type) == TYPE_CODE_ENUM
		   && (list->field.bitsize
		       == 8 * TYPE_LENGTH (builtin_type_int))
		   )
	       )
	      &&
	      list->field.bitpos % 8 == 0)
	    list->field.bitsize = 0;
	  nfields++;
	}
    }

  if (p[1] == ':')
    /* chill the list of fields: the last entry (at the head)
       is a partially constructed entry which we now scrub.  */
    list = list->next;

  /* Now create the vector of fields, and record how big it is.
     We need this info to record proper virtual function table information
     for this class's virtual functions.  */

  TYPE_NFIELDS (type) = nfields;
  TYPE_FIELDS (type) = (struct field *) obstack_alloc (symbol_obstack,
					       sizeof (struct field) * nfields);

  TYPE_FIELD_PRIVATE_BITS (type) =
    (B_TYPE *) obstack_alloc (symbol_obstack, B_BYTES (nfields));
  B_CLRALL (TYPE_FIELD_PRIVATE_BITS (type), nfields);

  TYPE_FIELD_PROTECTED_BITS (type) =
    (B_TYPE *) obstack_alloc (symbol_obstack, B_BYTES (nfields));
  B_CLRALL (TYPE_FIELD_PROTECTED_BITS (type), nfields);

  /* Copy the saved-up fields into the field vector.  */

  for (n = nfields; list; list = list->next)
    {
      n -= 1;
      TYPE_FIELD (type, n) = list->field;
      if (list->visibility == 0)
	SET_TYPE_FIELD_PRIVATE (type, n);
      else if (list->visibility == 1)
	SET_TYPE_FIELD_PROTECTED (type, n);
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

      /* For each list of method lists... */
      do
	{
	  int i;
	  struct next_fnfield *sublist = 0;
	  struct type *look_ahead_type = NULL;
	  int length = 0;
	  struct next_fnfieldlist *new_mainlist =
	    (struct next_fnfieldlist *)alloca (sizeof (struct next_fnfieldlist));
	  char *main_fn_name;

	  p = *pp;

	  /* read in the name.  */
	  while (*p != ':') p++;
	  if ((*pp)[0] == 'o' && (*pp)[1] == 'p' && (*pp)[2] == CPLUS_MARKER)
	    {
	      /* This lets the user type "break operator+".
	         We could just put in "+" as the name, but that wouldn't
		 work for "*".  */
	      static char opname[32] = {'o', 'p', CPLUS_MARKER};
	      char *o = opname + 3;

	      /* Skip past '::'.  */
	      p += 2;
	      while (*p != '.')
		*o++ = *p++;
	     main_fn_name = savestring (opname, o - opname);
	      /* Skip past '.'  */
	      *pp = p + 1;
	    }
	  else
	    {
	      i = 0;
	      main_fn_name = savestring (*pp, p - *pp);
	      /* Skip past '::'.  */
	      *pp = p + 2;
	    }
	  new_mainlist->fn_fieldlist.name = main_fn_name;

	  do
	    {
	      struct next_fnfield *new_sublist =
		(struct next_fnfield *)alloca (sizeof (struct next_fnfield));

	      /* Check for and handle cretinous dbx symbol name continuation!  */
	      if (look_ahead_type == NULL) /* Normal case. */
		{
		  if (**pp == '\\') *pp = next_symbol_text ();

		  new_sublist->fn_field.type = read_type (pp);
		  if (**pp != ':')
		    /* Invalid symtab info for method.  */
		    return error_type (pp);
	        }
	      else
		{ /* g++ version 1 kludge */
		  new_sublist->fn_field.type = look_ahead_type;
		  look_ahead_type = NULL;
	        }

	      *pp += 1;
	      p = *pp;
	      while (*p != ';') p++;
	      /* If this is just a stub, then we don't have the
		 real name here.  */
	      new_sublist->fn_field.physname = savestring (*pp, p - *pp);
	      *pp = p + 1;
	      new_sublist->visibility = *(*pp)++ - '0';
	      if (**pp == '\\') *pp = next_symbol_text ();
	      /* FIXME-tiemann: need to add const/volatile info
		 to the methods.  For now, just skip the char.
		 In future, here's what we need to implement:

		 A for normal functions.
		 B for `const' member functions.
		 C for `volatile' member functions.
		 D for `const volatile' member functions.  */
	      if (**pp == 'A' || **pp == 'B' || **pp == 'C' || **pp == 'D')
	        (*pp)++;
#if 0
	      /* This probably just means we're processing a file compiled
		 with g++ version 1.  */
	      else
	        complain(&const_vol_complaint, **pp);
#endif /* 0 */

	      switch (*(*pp)++)
		{
		case '*':
		  /* virtual member function, followed by index.  */
		  /* The sign bit is set to distinguish pointers-to-methods
		     from virtual function indicies.  Since the array is
		     in words, the quantity must be shifted left by 1
		     on 16 bit machine, and by 2 on 32 bit machine, forcing
		     the sign bit out, and usable as a valid index into
		     the array.  Remove the sign bit here.  */
		  new_sublist->fn_field.voffset =
		      (0x7fffffff & read_number (pp, ';')) + 1;

		  if (**pp == '\\') *pp = next_symbol_text ();

		  if (**pp == ';' || **pp == '\0')
		    /* Must be g++ version 1.  */
		    new_sublist->fn_field.fcontext = 0;
		  else
		    {
		      /* Figure out from whence this virtual function came.
			 It may belong to virtual function table of
			 one of its baseclasses.  */
		      look_ahead_type = read_type (pp);
		      if (**pp == ':')
			{ /* g++ version 1 overloaded methods. */ }
		      else
			{
			  new_sublist->fn_field.fcontext = look_ahead_type;
			  if (**pp != ';')
			    return error_type (pp);
			  else
			    ++*pp;
			  look_ahead_type = NULL;
		        }
		    }
		  break;

		case '?':
		  /* static member function.  */
		  new_sublist->fn_field.voffset = VOFFSET_STATIC;
		  break;
		default:
		  /* **pp == '.'.  */
		  /* normal member function.  */
		  new_sublist->fn_field.voffset = 0;
		  new_sublist->fn_field.fcontext = 0;
		  break;
		}

	      new_sublist->next = sublist;
	      sublist = new_sublist;
	      length++;
	    }
	  while (**pp != ';' && **pp != '\0');

	  *pp += 1;

	  new_mainlist->fn_fieldlist.fn_fields =
	    (struct fn_field *) obstack_alloc (symbol_obstack,
					       sizeof (struct fn_field) * length);
	  TYPE_FN_PRIVATE_BITS (new_mainlist->fn_fieldlist) =
	    (B_TYPE *) obstack_alloc (symbol_obstack, B_BYTES (length));
	  B_CLRALL (TYPE_FN_PRIVATE_BITS (new_mainlist->fn_fieldlist), length);

	  TYPE_FN_PROTECTED_BITS (new_mainlist->fn_fieldlist) =
	    (B_TYPE *) obstack_alloc (symbol_obstack, B_BYTES (length));
	  B_CLRALL (TYPE_FN_PROTECTED_BITS (new_mainlist->fn_fieldlist), length);

	  for (i = length; (i--, sublist); sublist = sublist->next)
	    {
	      new_mainlist->fn_fieldlist.fn_fields[i] = sublist->fn_field;
	      if (sublist->visibility == 0)
		B_SET (new_mainlist->fn_fieldlist.private_fn_field_bits, i);
	      else if (sublist->visibility == 1)
		B_SET (new_mainlist->fn_fieldlist.protected_fn_field_bits, i);
	    }

	  new_mainlist->fn_fieldlist.length = length;
	  new_mainlist->next = mainlist;
	  mainlist = new_mainlist;
	  nfn_fields++;
	  total_length += length;
	}
      while (**pp != ';');
    }

  *pp += 1;

  TYPE_FN_FIELDLISTS (type) =
    (struct fn_fieldlist *) obstack_alloc (symbol_obstack,
				   sizeof (struct fn_fieldlist) * nfn_fields);

  TYPE_NFN_FIELDS (type) = nfn_fields;
  TYPE_NFN_FIELDS_TOTAL (type) = total_length;

  {
    int i;
    for (i = 0; i < TYPE_N_BASECLASSES (type); ++i)
      TYPE_NFN_FIELDS_TOTAL (type) +=
	TYPE_NFN_FIELDS_TOTAL (TYPE_BASECLASS (type, i));
  }

  for (n = nfn_fields; mainlist; mainlist = mainlist->next)
    TYPE_FN_FIELDLISTS (type)[--n] = mainlist->fn_fieldlist;

  if (**pp == '~')
    {
      *pp += 1;

      if (**pp == '=')
	{
	  TYPE_FLAGS (type)
	    |= TYPE_FLAG_HAS_CONSTRUCTOR | TYPE_FLAG_HAS_DESTRUCTOR;
	  *pp += 1;
	}
      else if (**pp == '+')
	{
	  TYPE_FLAGS (type) |= TYPE_FLAG_HAS_CONSTRUCTOR;
	  *pp += 1;
	}
      else if (**pp == '-')
	{
	  TYPE_FLAGS (type) |= TYPE_FLAG_HAS_DESTRUCTOR;
	  *pp += 1;
	}

      /* Read either a '%' or the final ';'.  */
      if (*(*pp)++ == '%')
	{
	  /* Now we must record the virtual function table pointer's
	     field information.  */

	  struct type *t;
	  int i;

	  t = read_type (pp);
	  p = (*pp)++;
	  while (*p != '\0' && *p != ';')
	    p++;
	  if (*p == '\0')
	    /* Premature end of symbol.  */
	    return error_type (pp);
	  
	  TYPE_VPTR_BASETYPE (type) = t;
	  if (type == t)
	    {
	      if (TYPE_FIELD_NAME (t, TYPE_N_BASECLASSES (t)) == 0)
		{
		  /* FIXME-tiemann: what's this?  */
#if 0
		  TYPE_VPTR_FIELDNO (type) = i = TYPE_N_BASECLASSES (t);
#else
		  error_type (pp);
#endif
		}
	      else for (i = TYPE_NFIELDS (t) - 1; i >= TYPE_N_BASECLASSES (t); --i)
		if (! strncmp (TYPE_FIELD_NAME (t, i), vptr_name, 
			sizeof (vptr_name) -1))
		  {
		    TYPE_VPTR_FIELDNO (type) = i;
		    break;
		  }
	      if (i < 0)
		/* Virtual function table field not found.  */
		return error_type (pp);
	    }
	  else
	    TYPE_VPTR_FIELDNO (type) = TYPE_VPTR_FIELDNO (t);
	  *pp = p + 1;
	}
    }

  return type;
}

/* Read a definition of an array type,
   and create and return a suitable type object.
   Also creates a range type which represents the bounds of that
   array.  */
static struct type *
read_array_type (pp, type)
     register char **pp;
     register struct type *type;
{
  struct type *index_type, *element_type, *range_type;
  int lower, upper;
  int adjustable = 0;

  /* Format of an array type:
     "ar<index type>;lower;upper;<array_contents_type>".  Put code in
     to handle this.

     Fortran adjustable arrays use Adigits or Tdigits for lower or upper;
     for these, produce a type like float[][].  */

  index_type = read_type (pp);
  if (**pp != ';')
    /* Improper format of array type decl.  */
    return error_type (pp);
  ++*pp;

  if (!(**pp >= '0' && **pp <= '9'))
    {
      *pp += 1;
      adjustable = 1;
    }
  lower = read_number (pp, ';');

  if (!(**pp >= '0' && **pp <= '9'))
    {
      *pp += 1;
      adjustable = 1;
    }
  upper = read_number (pp, ';');
  
  element_type = read_type (pp);

  if (adjustable)
    {
      lower = 0;
      upper = -1;
    }

  {
    /* Create range type.  */
    range_type = (struct type *) obstack_alloc (symbol_obstack,
						sizeof (struct type));
    TYPE_CODE (range_type) = TYPE_CODE_RANGE;
    TYPE_TARGET_TYPE (range_type) = index_type;

    /* This should never be needed.  */
    TYPE_LENGTH (range_type) = sizeof (int);

    TYPE_NFIELDS (range_type) = 2;
    TYPE_FIELDS (range_type) =
      (struct field *) obstack_alloc (symbol_obstack,
				      2 * sizeof (struct field));
    TYPE_FIELD_BITPOS (range_type, 0) = lower;
    TYPE_FIELD_BITPOS (range_type, 1) = upper;
  }

  TYPE_CODE (type) = TYPE_CODE_ARRAY;
  TYPE_TARGET_TYPE (type) = element_type;
  TYPE_LENGTH (type) = (upper - lower + 1) * TYPE_LENGTH (element_type);
  TYPE_NFIELDS (type) = 1;
  TYPE_FIELDS (type) =
    (struct field *) obstack_alloc (symbol_obstack,
				    sizeof (struct field));
  TYPE_FIELD_TYPE (type, 0) = range_type;

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
  int o_nsyms;

  if (within_function)
    symlist = &local_symbols;
  else
    symlist = &file_symbols;
  osyms = *symlist;
  o_nsyms = osyms ? osyms->nsyms : 0;

  /* Read the value-names and their values.
     The input syntax is NAME:VALUE,NAME:VALUE, and so on.
     A semicolon or comman instead of a NAME means the end.  */
  while (**pp && **pp != ';' && **pp != ',')
    {
      /* Check for and handle cretinous dbx symbol name continuation!  */
      if (**pp == '\\')	*pp = next_symbol_text ();

      p = *pp;
      while (*p != ':') p++;
      name = obsavestring (*pp, p - *pp);
      *pp = p + 1;
      n = read_number (pp, ',');

      sym = (struct symbol *) obstack_alloc (symbol_obstack, sizeof (struct symbol));
      bzero (sym, sizeof (struct symbol));
      SYMBOL_NAME (sym) = name;
      SYMBOL_CLASS (sym) = LOC_CONST;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      SYMBOL_VALUE (sym) = n;
      add_symbol_to_list (sym, symlist);
      nsyms++;
    }

  if (**pp == ';')
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
  /* Note that we preserve the order of the enum constants, so
     that in something like "enum {FOO, LAST_THING=FOO}" we print
     FOO, not LAST_THING.  */

  for (syms = *symlist, n = 0; syms; syms = syms->next)
    {
      int j = 0;
      if (syms == osyms)
	j = o_nsyms;
      for (; j < syms->nsyms; j++,n++)
	{
	  struct symbol *sym = syms->symbol[j];
	  SYMBOL_TYPE (sym) = type;
	  TYPE_FIELD_NAME (type, n) = SYMBOL_NAME (sym);
	  TYPE_FIELD_VALUE (type, n) = 0;
	  TYPE_FIELD_BITPOS (type, n) = SYMBOL_VALUE (sym);
	  TYPE_FIELD_BITSIZE (type, n) = 0;
	}
      if (syms == osyms)
	break;
    }

  return type;
}

/* Read a number from the string pointed to by *PP.
   The value of *PP is advanced over the number.
   If END is nonzero, the character that ends the
   number must match END, or an error happens;
   and that character is skipped if it does match.
   If END is zero, *PP is left pointing to that character.

   If the number fits in a long, set *VALUE and set *BITS to 0.
   If not, set *BITS to be the number of bits in the number.

   If encounter garbage, set *BITS to -1.  */

static void
read_huge_number (pp, end, valu, bits)
     char **pp;
     int end;
     long *valu;
     int *bits;
{
  char *p = *pp;
  int sign = 1;
  long n = 0;
  int radix = 10;
  char overflow = 0;
  int nbits = 0;
  int c;
  
  if (*p == '-')
    {
      sign = -1;
      p++;
    }

  /* Leading zero means octal.  GCC uses this to output values larger
     than an int (because that would be hard in decimal).  */
  if (*p == '0')
    {
      radix = 8;
      p++;
    }

  while ((c = *p++) >= '0' && c <= ('0' + radix))
    {
      if (n <= LONG_MAX / radix)
	{
	  n *= radix;
	  n += c - '0';		/* FIXME this overflows anyway */
	}
      else
	overflow = 1;
      
      /* This depends on large values being output in octal, which is
	 what GCC does. */
      if (radix == 8)
	{
	  if (nbits == 0)
	    {
	      if (c == '0')
		/* Ignore leading zeroes.  */
		;
	      else if (c == '1')
		nbits = 1;
	      else if (c == '2' || c == '3')
		nbits = 2;
	      else
		nbits = 3;
	    }
	  else
	    nbits += 3;
	}
    }
  if (end)
    {
      if (c && c != end)
	{
	  if (bits != NULL)
	    *bits = -1;
	  return;
	}
    }
  else
    --p;

  *pp = p;
  if (overflow)
    {
      if (nbits == 0)
	{
	  /* Large decimal constants are an error (because it is hard to
	     count how many bits are in them).  */
	  if (bits != NULL)
	    *bits = -1;
	  return;
	}
      
      /* -0x7f is the same as 0x80.  So deal with it by adding one to
	 the number of bits.  */
      if (sign == -1)
	++nbits;
      if (bits)
	*bits = nbits;
    }
  else
    {
      if (valu)
	*valu = n * sign;
      if (bits)
	*bits = 0;
    }
}

#define	MAX_OF_TYPE(t)	((1 << (sizeof (t)*8 - 1)) - 1)
#define MIN_OF_TYPE(t)	(-(1 << (sizeof (t)*8 - 1)))

static struct type *
read_range_type (pp, typenums)
     char **pp;
     int typenums[2];
{
  int rangenums[2];
  long n2, n3;
  int n2bits, n3bits;
  int self_subrange;
  struct type *result_type;

  /* First comes a type we are a subrange of.
     In C it is usually 0, 1 or the type being defined.  */
  read_type_number (pp, rangenums);
  self_subrange = (rangenums[0] == typenums[0] &&
		   rangenums[1] == typenums[1]);

  /* A semicolon should now follow; skip it.  */
  if (**pp == ';')
    (*pp)++;

  /* The remaining two operands are usually lower and upper bounds
     of the range.  But in some special cases they mean something else.  */
  read_huge_number (pp, ';', &n2, &n2bits);
  read_huge_number (pp, ';', &n3, &n3bits);

  if (n2bits == -1 || n3bits == -1)
    return error_type (pp);
  
  /* If limits are huge, must be large integral type.  */
  if (n2bits != 0 || n3bits != 0)
    {
      char got_signed = 0;
      char got_unsigned = 0;
      /* Number of bits in the type.  */
      int nbits;

      /* Range from 0 to <large number> is an unsigned large integral type.  */
      if ((n2bits == 0 && n2 == 0) && n3bits != 0)
	{
	  got_unsigned = 1;
	  nbits = n3bits;
	}
      /* Range from <large number> to <large number>-1 is a large signed
	 integral type.  */
      else if (n2bits != 0 && n3bits != 0 && n2bits == n3bits + 1)
	{
	  got_signed = 1;
	  nbits = n2bits;
	}

      /* Check for "long long".  */
      if (got_signed && nbits == TARGET_LONG_LONG_BIT)
	return builtin_type_long_long;
      if (got_unsigned && nbits == TARGET_LONG_LONG_BIT)
	return builtin_type_unsigned_long_long;

      if (got_signed || got_unsigned)
	{
	  result_type = (struct type *) obstack_alloc (symbol_obstack,
						       sizeof (struct type));
	  bzero (result_type, sizeof (struct type));
	  TYPE_LENGTH (result_type) = nbits / TARGET_CHAR_BIT;
	  TYPE_MAIN_VARIANT (result_type) = result_type;
	  TYPE_CODE (result_type) = TYPE_CODE_INT;
	  if (got_unsigned)
	    TYPE_FLAGS (result_type) |= TYPE_FLAG_UNSIGNED;
	  return result_type;
	}
      else
	return error_type (pp);
    }

  /* A type defined as a subrange of itself, with bounds both 0, is void.  */
  if (self_subrange && n2 == 0 && n3 == 0)
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

  /* Special case: char is defined (Who knows why) as a subrange of
     itself with range 0-127.  */
  else if (self_subrange && n2 == 0 && n3 == 127)
    return builtin_type_char;

  /* Assumptions made here: Subrange of self is equivalent to subrange
     of int.  */
  else if (n2 == 0
	   && (self_subrange ||
	       *dbx_lookup_type (rangenums) == builtin_type_int))
    {
      /* an unsigned type */
#ifdef LONG_LONG
      if (n3 == - sizeof (long long))
	return builtin_type_unsigned_long_long;
#endif
      if (n3 == (unsigned int)~0L)
	return builtin_type_unsigned_int;
      if (n3 == (unsigned long)~0L)
	return builtin_type_unsigned_long;
      if (n3 == (unsigned short)~0L)
	return builtin_type_unsigned_short;
      if (n3 == (unsigned char)~0L)
	return builtin_type_unsigned_char;
    }
#ifdef LONG_LONG
  else if (n3 == 0 && n2 == -sizeof (long long))
    return builtin_type_long_long;
#endif  
  else if (n2 == -n3 -1)
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

  /* We have a real range type on our hands.  Allocate space and
     return a real pointer.  */

  /* At this point I don't have the faintest idea how to deal with
     a self_subrange type; I'm going to assume that this is used
     as an idiom, and that all of them are special cases.  So . . .  */
  if (self_subrange)
    return error_type (pp);

  result_type = (struct type *) obstack_alloc (symbol_obstack,
					       sizeof (struct type));
  bzero (result_type, sizeof (struct type));

  TYPE_TARGET_TYPE (result_type) = (self_subrange ?
				    builtin_type_int :
				    *dbx_lookup_type(rangenums));

  /* We have to figure out how many bytes it takes to hold this
     range type.  I'm going to assume that anything that is pushing
     the bounds of a long was taken care of above.  */
  if (n2 >= MIN_OF_TYPE(char) && n3 <= MAX_OF_TYPE(char))
    TYPE_LENGTH (result_type) = 1;
  else if (n2 >= MIN_OF_TYPE(short) && n3 <= MAX_OF_TYPE(short))
    TYPE_LENGTH (result_type) = sizeof (short);
  else if (n2 >= MIN_OF_TYPE(int) && n3 <= MAX_OF_TYPE(int))
    TYPE_LENGTH (result_type) = sizeof (int);
  else if (n2 >= MIN_OF_TYPE(long) && n3 <= MAX_OF_TYPE(long))
    TYPE_LENGTH (result_type) = sizeof (long);
  else
    /* Ranged type doesn't fit within known sizes.  */
    return error_type (pp);

  TYPE_LENGTH (result_type) = TYPE_LENGTH (TYPE_TARGET_TYPE (result_type));
  TYPE_CODE (result_type) = TYPE_CODE_RANGE;
  TYPE_NFIELDS (result_type) = 2;
  TYPE_FIELDS (result_type) =
    (struct field *) obstack_alloc (symbol_obstack,
				    2 * sizeof (struct field));
  bzero (TYPE_FIELDS (result_type), 2 * sizeof (struct field));
  TYPE_FIELD_BITPOS (result_type, 0) = n2;
  TYPE_FIELD_BITPOS (result_type, 1) = n3;

  return result_type;
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
      if (c && c != end)
	error ("Invalid symbol data: invalid character \\%03o at symbol pos %d.", c, symnum);
    }
  else
    --p;

  *pp = p;
  return n * sign;
}

/* Read in an argument list.  This is a list of types, separated by commas
   and terminated with END.  Return the list of types read in, or (struct type
   **)-1 if there is an error.  */
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
	/* Invalid argument list: no ','.  */
	return (struct type **)-1;
      *pp += 1;

      /* Check for and handle cretinous dbx symbol name continuation! */
      if (**pp == '\\')
	*pp = next_symbol_text ();

      types[n++] = read_type (pp);
    }
  *pp += 1;			/* get past `end' (the ':' character) */

  if (n == 1)
    {
      rval = (struct type **) xmalloc (2 * sizeof (struct type *));
    }
  else if (TYPE_CODE (types[n-1]) != TYPE_CODE_VOID)
    {
      rval = (struct type **) xmalloc ((n + 1) * sizeof (struct type *));
      bzero (rval + n, sizeof (struct type *));
    }
  else
    {
      rval = (struct type **) xmalloc (n * sizeof (struct type *));
    }
  bcopy (types, rval, n * sizeof (struct type *));
  return rval;
}

/* Copy a pending list, used to record the contents of a common
   block for later fixup.  */
static struct pending *
copy_pending (beg, begi, end)
    struct pending *beg, *end;
    int begi;
{
  struct pending *new = 0;
  struct pending *next;

  for (next = beg; next != 0 && (next != end || begi < end->nsyms);
       next = next->next, begi = 0)
    {
      register int j;
      for (j = begi; j < next->nsyms; j++)
	add_symbol_to_list (next->symbol[j], &new);
    }
  return new;
}

/* Add a common block's start address to the offset of each symbol
   declared to be in it (by being between a BCOMM/ECOMM pair that uses
   the common block name).  */

static void
fix_common_block (sym, valu)
    struct symbol *sym;
    int valu;
{
  struct pending *next = (struct pending *) SYMBOL_NAMESPACE (sym);
  for ( ; next; next = next->next)
    {
      register int j;
      for (j = next->nsyms - 1; j >= 0; j--)
	SYMBOL_VALUE_ADDRESS (next->symbol[j]) += valu;
    }
}

/* Register our willingness to decode symbols for SunOS and a.out and
   b.out files handled by BFD... */
static struct sym_fns sunos_sym_fns = {"sunOs", 6,
              dbx_new_init, dbx_symfile_init,
              dbx_symfile_read, dbx_symfile_discard};

static struct sym_fns aout_sym_fns = {"a.out", 5,
              dbx_new_init, dbx_symfile_init,
              dbx_symfile_read, dbx_symfile_discard};

static struct sym_fns bout_sym_fns = {"b.out", 5,
              dbx_new_init, dbx_symfile_init,
              dbx_symfile_read, dbx_symfile_discard};

void
_initialize_dbxread ()
{
  add_symtab_fns(&sunos_sym_fns);
  add_symtab_fns(&aout_sym_fns);
  add_symtab_fns(&bout_sym_fns);

  undef_types_allocated = 20;
  undef_types_length = 0;
  undef_types = (struct type **) xmalloc (undef_types_allocated *
					  sizeof (struct type *));

  dbx_new_init ();
}
