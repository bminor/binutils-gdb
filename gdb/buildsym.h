/* Build symbol tables in GDB's internal format.
   Copyright (C) 1986-1991 Free Software Foundation, Inc.

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

#if !defined (BUILDSYM_H)
#define BUILDSYM_H 1

/* This module provides definitions used for creating and adding to
   the symbol table.  These routines are called from various symbol-
   file-reading routines.  

   They originated in dbxread.c of gdb-4.2, and were split out to
   make xcoffread.c more maintainable by sharing code.

   Variables declared in this file can be defined by #define-ing
   the name EXTERN to null.  It is used to declare variables that
   are normally extern, but which get defined in a single module
   using this technique.  */

#ifndef EXTERN
#define	EXTERN	extern
#endif

/* Convert stab register number (from `r' declaration) to a gdb REGNUM.  */

#ifndef STAB_REG_TO_REGNUM
#define STAB_REG_TO_REGNUM(VALUE) (VALUE)
#endif

/* Name of source file whose symbol data we are now processing.
   This comes from a symbol of type N_SO.  */

EXTERN char *last_source_file;

/* Core address of start of text of current source file.
   This too comes from the N_SO symbol.  */

EXTERN CORE_ADDR last_source_start_addr;

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
};

EXTERN struct subfile *subfiles;

EXTERN struct subfile *current_subfile;

/* Global variable which, when set, indicates that we are processing a
   .o file compiled with gcc */

EXTERN unsigned char processing_gcc_compilation;

/* Count symbols as they are processed, for error messages.  */

EXTERN unsigned int symnum;

/* Vector of types defined so far, indexed by their dbx type numbers.
   (In newer sun systems, dbx uses a pair of numbers in parens,
    as in "(SUBFILENUM,NUMWITHINSUBFILE)".  Then these numbers must be
    translated through the type_translations hash table to get
    the index into the type vector.)  */

EXTERN struct type **type_vector;

/* Number of elements allocated for type_vector currently.  */

EXTERN int type_vector_length;

/* Hash table of global symbols whose values are not known yet.
   They are chained thru the SYMBOL_VALUE_CHAIN, since we don't
   have the correct data for that slot yet.  */
/* The use of the LOC_BLOCK code in this chain is nonstandard--
   it refers to a FORTRAN common block rather than the usual meaning.  */

#define HASHSIZE 127
EXTERN struct symbol *global_sym_chain[HASHSIZE];

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
EXTERN struct pending *free_pendings;

/* Here are the three lists that symbols are put on.  */

EXTERN struct pending *file_symbols;	/* static at top level, and types */

EXTERN struct pending *global_symbols;	/* global functions and variables */

EXTERN struct pending *local_symbols;	/* everything local to lexic context */

/* Kludge for xcoffread.c */
struct pending_stabs {
  int	count, length;
  char	*stab[1];
};

EXTERN struct pending_stabs *global_stabs;

/* List of symbols declared since the last BCOMM.  This list is a tail
   of local_symbols.  When ECOMM is seen, the symbols on the list
   are noted so their proper addresses can be filled in later,
   using the common block base address gotten from the assembler
   stabs.  */

EXTERN struct pending *common_block;
EXTERN int common_block_i;

/* Stack representing unclosed lexical contexts
   (that will become blocks, eventually).  */

struct context_stack
{
  struct pending *locals;	/* Outer locals at the time we entered */
  struct pending_block *old_blocks; /* Pointer into blocklist as of entry */
  struct symbol *name;		/* Name of function, if any, defining context*/
  CORE_ADDR start_addr;		/* PC where this context starts */
  CORE_ADDR end_addr;		/* Temp slot for exception handling. */
  int depth;			/* For error-checking matching push/pop */
};

EXTERN struct context_stack *context_stack;

/* Index of first unused entry in context stack.  */
EXTERN int context_stack_depth;

/* Currently allocated size of context stack.  */

EXTERN int context_stack_size;

/* Macro "function" for popping contexts from the stack.  Pushing is done
   by a real function, push_context.  This returns a pointer to a struct
   context_stack.  */

#define	pop_context()	\
      (&context_stack[--context_stack_depth]);

/* Nonzero if within a function (so symbols should be local,
   if nothing says specifically).  */

EXTERN int within_function;

/* List of blocks already made (lexical contexts already closed).
   This is used at the end to make the blockvector.  */

struct pending_block
{
  struct pending_block *next;
  struct block *block;
};

EXTERN struct pending_block *pending_blocks;

/* Global variable which, when set, indicates that we are processing a
   .o file compiled with gcc */

EXTERN unsigned char processing_gcc_compilation;

/* The type code that process_one_symbol saw on its previous invocation.
   Used to detect pairs of N_SO symbols. */

EXTERN int previous_stab_code;

/* Setup a define to deal cleanly with the underscore problem */

#ifdef NAMES_HAVE_UNDERSCORE
#define HASH_OFFSET 1
#else
#define HASH_OFFSET 0
#endif

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

EXTERN struct header_file *header_files;

EXTERN int n_header_files;

EXTERN int n_allocated_header_files;

/* Within each object file, various header files are assigned numbers.
   A type is defined or referred to with a pair of numbers
   (FILENUM,TYPENUM) where FILENUM is the number of the header file
   and TYPENUM is the number within that header file.
   TYPENUM is the index within the vector of types for that header file.

   FILENUM == 1 is special; it refers to the main source of the object file,
   and not to any header file.  FILENUM != 1 is interpreted by looking it up
   in the following table, which contains indices in header_files.  */

EXTERN int *this_object_header_files;

EXTERN int n_this_object_header_files;

EXTERN int n_allocated_this_object_header_files;

/* When a header file is getting special overriding definitions
   for one source file, record here the header_files index
   of its normal definition vector.
   At other times, this is -1.  */

EXTERN int header_file_prev_index;

struct subfile_stack
{
  struct subfile_stack *next;
  char *name;
  int prev_index;
};

EXTERN struct subfile_stack *subfile_stack;

extern struct complaint unknown_symtype_complaint;

#define next_symbol_text() (*next_symbol_text_func)()

/* Function to invoke get the next symbol.  Return the symbol name. */

EXTERN char *(*next_symbol_text_func) PARAMS ((void));

extern void
add_symbol_to_list PARAMS ((struct symbol *, struct pending **));

extern struct symbol *
find_symbol_in_list PARAMS ((struct pending *, char *, int));

extern void
read_type_number PARAMS ((char **, int *));

extern struct type *
read_type PARAMS ((char **, struct objfile *));

extern struct type **
dbx_lookup_type PARAMS ((int [2]));

extern long
read_number PARAMS ((char **, int));

extern void
finish_block PARAMS ((struct symbol *, struct pending **,
		      struct pending_block *, CORE_ADDR, CORE_ADDR,
		      struct objfile *));

extern void
add_undefined_type PARAMS ((struct type *));

extern void
really_free_pendings PARAMS ((int foo));

extern void
start_subfile PARAMS ((char *, char *));

extern void
push_subfile PARAMS ((void));

extern char *
pop_subfile PARAMS ((void));

extern struct symtab *
end_symtab PARAMS ((CORE_ADDR, int, int,struct objfile *));

extern void
scan_file_globals PARAMS ((struct objfile *));

extern void
buildsym_new_init PARAMS ((void));

extern void
buildsym_init PARAMS ((void));

extern struct context_stack *
push_context PARAMS ((int, CORE_ADDR));

extern void
record_line PARAMS ((struct subfile *, int, CORE_ADDR));

extern void
start_symtab PARAMS ((char *, char *, CORE_ADDR));

extern struct symbol *
define_symbol PARAMS ((unsigned int, char *, int, int, struct objfile *));

extern struct partial_symtab *
start_psymtab PARAMS ((struct objfile *, CORE_ADDR, char *, CORE_ADDR, int,
		       struct partial_symbol *, struct partial_symbol *));

extern void
end_psymtab PARAMS ((struct partial_symtab *, char **, int, int, CORE_ADDR,
		     struct partial_symtab **, int));

extern void
process_one_symbol PARAMS ((int, int, CORE_ADDR, char *, int,
			    struct objfile *));

extern int
hashname PARAMS ((char *));

#endif	/* defined (BUILDSYM_H) */
