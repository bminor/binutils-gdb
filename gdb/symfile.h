/* Definitions for reading symbol files into GDB.
   Copyright (C) 1990  Free Software Foundation, Inc.

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

/* This file requires that you first include "bfd.h".  */

/* Data structures and function definitions for dealing with
   symbol table reading from files.  */

/* Structure to keep track of symbol reading functions for various
   object file types.  */

struct sym_fns {

  /* sym_name
     is the name, or name prefix, of the BFD "target type" that this
     set of functions handles.  E.g. "a.out" or "sunOs" or "coff".  */

  char *sym_name;

  /* sym_namelen
     counts how many bytes of sym_name should be checked against the
     BFD target type of the file being read.  If an exact match is
     desired, specify the number of characters in sym_name plus 1 for the
     NUL.  If a prefix match is desired, specify the number of characters in
     sym_name.  */

  int sym_namelen;

  /* sym_new_init
     initializes anything that is global to the entire
     symbol table.  It is called e.g. during symbol_file_command, when
     we begin debugging an entirely new program.  */

  void (*sym_new_init) ();

  /* sym_init (sf)
     reads any initial information from a symbol file, and
     initializes the struct sym_fns SF in preparation for sym_read().
     It is called every time we read a symbol file for any reason.  */

  void (*sym_init) ();

  /* sym_read (sf, addr, mainline)
     reads a symbol file into a psymtab (or possibly a symtab).
     SF is the struct sym_fns that sym_init initialized.  ADDR
     is the offset between the file's specified start address and
     its true address in memory.  MAINLINE is 1 if this is the
     main symbol table being read, and 0 if a secondary
     symbol file (e.g. shared library or dynamically loaded file)
     is being read.  */

  void (*sym_read) ();

  /* sym_discard (sf)
     discards any cached information from SF or elsewhere, that
     was saved as part of reading a single symbol file.  */

  void (*sym_discard) ();

  /* sym_bfd
     is the accessor for the symbol file being read.  */

  bfd  *sym_bfd;

  /* sym_private
     is where information can be shared among sym_init, sym_read and
     sym_discard.  It is typically a pointer to malloc'd memory.  */

  char *sym_private;			/* Should be void * */

  /* next
     finds the next struct sym_fns.  They are allocated and initialized
     in whatever module implements the functions pointed to; an 
     initializer calls add_symtab_fns to add them to the global chain.  */
  struct sym_fns *next;
};

			/*   Functions   */

extern int  free_named_symtabs ();
extern int  fill_in_vptr_fieldno ();
extern void add_symtab_fns ();

/* Functions for dealing with the misc "function" vector, really a misc
   address<->symbol mapping vector for things we don't have debug symbols
   for.  */

extern void init_misc_bunches ();
extern void prim_record_misc_function ();
extern void discard_misc_bunches ();
extern void condense_misc_bunches ();

/* Sorting your symbols for fast lookup or alphabetical printing.  */

extern void sort_block_syms ();
extern void sort_symtab_syms ();
extern void sort_all_symtab_syms ();
extern void sort_block_syms ();

/* Make a copy of the string at PTR with SIZE characters in the symbol obstack
   (and add a null character at the end in the copy).
   Returns the address of the copy.  */

extern char *obsavestring ();

/* Concatenate strings S1, S2 and S3; return the new string.
   Space is found in the symbol_obstack.  */

extern char *obconcat ();

			/*   Variables   */

/* File name symbols were loaded from.  */

extern char *symfile;

/* The modification date of the file when they were loaded.  */

extern int symfile_mtime;

/* The BFD for this file -- only good while we're actively reading
   symbols into a psymtab or a symtab.  */

static bfd *symfile_bfd;

/* Vectors of all partial symbols read in from file.  */

extern struct psymbol_allocation_list {
  struct partial_symbol *list, *next;
  int size;
} global_psymbols, static_psymbols;

/* Support for complaining about things in the symbol file that aren't
   catastrophic.

   Each such thing gets a counter.  The first time we have the problem,
   during a symbol read, we report it.  At the end of symbol reading,
   if verbose, we report how many of each problem we had.  */

struct complaint {
  char *message;
  unsigned counter;
  struct complaint *next;
};

/* Root of the chain of complaints that have at some point been issued. 
   This is used to reset the counters, and/or report the total counts.  */

extern struct complaint complaint_root[1];

/* Functions that handle complaints.  (in symfile.c)  */

int complain();
void clear_complaints();
