/* Definitions for reading symbol files into GDB.
   Copyright (C) 1990, 1991, 1992  Free Software Foundation, Inc.

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

#if !defined (SYMFILE_H)
#define SYMFILE_H

/* This file requires that you first include "bfd.h".  */

struct psymbol_allocation_list {
  struct partial_symbol *list;
  struct partial_symbol *next;
  int size;
};

/* Structure to keep track of symbol reading functions for various
   object file types.  */

struct sym_fns {

  /* is the name, or name prefix, of the BFD "target type" that this
     set of functions handles.  E.g. "a.out" or "sunOs" or "coff" or "elf".  */

  char *sym_name;

  /* counts how many bytes of sym_name should be checked against the
     BFD target type of the file being read.  If an exact match is
     desired, specify the number of characters in sym_name plus 1 for the
     NUL.  If a prefix match is desired, specify the number of characters in
     sym_name.  */

  int sym_namelen;

  /* Initializes anything that is global to the entire symbol table.  It is
     called during symbol_file_add, when we begin debugging an entirely new
     program. */

  void (*sym_new_init) PARAMS ((struct objfile *));

  /* Reads any initial information from a symbol file, and initializes the
     struct sym_fns SF in preparation for sym_read().  It is called every
     time we read a symbol file for any reason. */

  void (*sym_init) PARAMS ((struct objfile *));

  /* sym_read (objfile, addr, mainline)
     Reads a symbol file into a psymtab (or possibly a symtab).
     OBJFILE is the objfile struct for the file we are reading.  ADDR
     is the offset between the file's specified start address and
     its true address in memory.  MAINLINE is 1 if this is the
     main symbol table being read, and 0 if a secondary
     symbol file (e.g. shared library or dynamically loaded file)
     is being read.  */

  void (*sym_read) PARAMS ((struct objfile *, CORE_ADDR, int));

  /* Called when we are finished with an objfile.  Should do all cleanup
     that is specific to the object file format for the particular objfile. */
 
  void (*sym_finish) PARAMS ((struct objfile *));

  /* Finds the next struct sym_fns.  They are allocated and initialized
     in whatever module implements the functions pointed to; an 
     initializer calls add_symtab_fns to add them to the global chain.  */

  struct sym_fns *next;

};

/* Master structure for keeping track of each input file from which
   gdb reads symbols.  One of these is allocated for each such file we
   access, e.g. the exec_file, symbol_file, and any shared library object
   files. */

struct objfile
{

  /* All struct objfile's are chained together by their next pointers.
     The global variable "object_files" points to the first link in this
     chain. */

  struct objfile *next;

  /* The object file's name.  Malloc'd; free it if you free this struct.  */

  char *name;

  /* Some flag bits for this objfile. */

  unsigned short flags;

  /* Each objfile points to a linked list of symtabs derived from this file,
     one symtab structure for each compilation unit (source file).  Each link
     in the symtab list contains a backpointer to this objfile. */

  struct symtab *symtabs;

  /* Each objfile points to a linked list of partial symtabs derived from
     this file, one partial symtab structure for each compilation unit
     (source file). */

  struct partial_symtab *psymtabs;

  /* List of freed partial symtabs, available for re-use */

  struct partial_symtab *free_psymtabs;

  /* The object file's BFD.  Can be null, in which case bfd_open (name) and
     put the result here.  */

  bfd *obfd;

  /* The modification timestamp of the object file, as of the last time
     we read its symbols.  */

  long mtime;

  /* Obstacks to hold objects that should be freed when we load a new symbol
     table from this object file. */

  struct obstack psymbol_obstack;	/* Partial symbols */
  struct obstack symbol_obstack;	/* Full symbols */
  struct obstack type_obstack;		/* Types */

  /* Vectors of all partial symbols read in from file.  The actual data
     is stored in the psymbol_obstack. */

  struct psymbol_allocation_list global_psymbols;
  struct psymbol_allocation_list static_psymbols;

  /* Each file contains a pointer to an array of minimal symbols for all
     global symbols that are defined within the file.  The array is terminated
     by a "null symbol", one that has a NULL pointer for the name and a zero
     value for the address.  This makes it easy to walk through the array
     when passed a pointer to somewhere in the middle of it.  There is also
     a count of the number of symbols, which does include the terminating
     null symbol.  The array itself, as well as all the data that it points
     to, should be allocated on the symbol_obstack for this file. */

  struct minimal_symbol *msymbols;
  int minimal_symbol_count;

  /* For object file formats which don't specify fundamental types, gdb
     can create such types.  For now, it maintains a vector of pointers
     to these internally created fundamental types on a per objfile basis,
     however it really should ultimately keep them on a per-compilation-unit
     basis, to account for linkage-units that consist of a number of
     compilation units that may have different fundamental types, such as
     linking C modules with ADA modules, or linking C modules that are
     compiled with 32-bit ints with C modules that are compiled with 64-bit
     ints (not inherently evil with a smarter linker). */

  struct type **fundamental_types;

  /* The mmalloc() malloc-descriptor for this objfile if we are using
     the memory mapped malloc() package to manage storage for this objfile's
     data.  NULL if we are not. */

  PTR md;

  /* Structure which keeps track of functions that manipulate objfile's
     of the same type as this objfile.  I.E. the function to read partial
     symbols for example.  Note that this structure is in statically
     allocated memory, and is shared by all objfiles that use the
     object module reader of this type. */

  struct sym_fns *sf;

  /* Hook for information which is shared by sym_init and sym_read for
     this objfile.  It is typically a pointer to malloc'd memory.  */

  PTR sym_private;

};

/* Defines for the objfile flag word. */

/* Gdb can arrange to allocate storage for all objects related to a
   particular objfile in a designated section of it's address space,
   managed at a low level by mmap() and using a special version of
   malloc that handles malloc/free/realloc on top of the mmap() interface.
   This allows the "internal gdb state" for a particular objfile to be
   dumped to a gdb state file and subsequently reloaded at a later time. */

#define OBJF_MAPPED	(1 << 0)	/* Objfile data is mmap'd */


extern void
extend_psymbol_list PARAMS ((struct psymbol_allocation_list *,
			     struct objfile *));

/* Add any kind of symbol to a psymbol_allocation_list. */

#define	ADD_PSYMBOL_VT_TO_LIST(NAME,NAMELENGTH,NAMESPACE,CLASS,LIST,VALUE,VT) \
  do {		        						\
    register struct partial_symbol *psym;				\
    if ((LIST).next >= (LIST).list + (LIST).size)			\
	   extend_psymbol_list (&(LIST),objfile);				\
    psym = (LIST).next++;						\
									\
    SYMBOL_NAME (psym) = (char *) obstack_alloc (&objfile->psymbol_obstack,	\
						 (NAMELENGTH) + 1);	\
    memcpy (SYMBOL_NAME (psym), (NAME), (NAMELENGTH));			\
    SYMBOL_NAME (psym)[(NAMELENGTH)] = '\0';				\
    SYMBOL_NAMESPACE (psym) = (NAMESPACE);				\
    SYMBOL_CLASS (psym) = (CLASS);					\
    VT (psym) = (VALUE); 						\
  } while (0);

#ifdef DEBUG

/* Since one arg is a struct, we have to pass in a ptr and deref it (sigh) */

#define	ADD_PSYMBOL_TO_LIST(name, namelength, namespace, class, list, value) \
  add_psymbol_to_list (name, namelength, namespace, class, &list, value)

#define	ADD_PSYMBOL_ADDR_TO_LIST(name, namelength, namespace, class, list, value) \
  add_psymbol_addr_to_list (name, namelength, namespace, class, &list, value)

#else	/* !DEBUG */

/* Add a symbol with an integer value to a psymtab. */

#define ADD_PSYMBOL_TO_LIST(name, namelength, namespace, class, list, value) \
  ADD_PSYMBOL_VT_TO_LIST (name, namelength, namespace, class, list, value, SYMBOL_VALUE)

/* Add a symbol with a CORE_ADDR value to a psymtab. */

#define	ADD_PSYMBOL_ADDR_TO_LIST(name, namelength, namespace, class, list, value)\
  ADD_PSYMBOL_VT_TO_LIST (name, namelength, namespace, class, list, value, SYMBOL_VALUE_ADDRESS)

#endif	/* DEBUG */

			/*   Functions   */

extern void
sort_pst_symbols PARAMS ((struct partial_symtab *));

extern struct symtab *
allocate_symtab PARAMS ((char *, struct objfile *));

extern struct objfile *
allocate_objfile PARAMS ((bfd *, int));

extern void
free_objfile PARAMS ((struct objfile *));

extern void
free_all_objfiles PARAMS ((void));

extern int
free_named_symtabs PARAMS ((char *));

extern void
fill_in_vptr_fieldno PARAMS ((struct type *));

extern void
add_symtab_fns PARAMS ((struct sym_fns *));

extern void
syms_from_objfile PARAMS ((struct objfile *, CORE_ADDR, int, int));

extern struct partial_symtab *
start_psymtab_common PARAMS ((struct objfile *, CORE_ADDR, char *, CORE_ADDR,
			      struct partial_symbol *,
			      struct partial_symbol *));

/* Sorting your symbols for fast lookup or alphabetical printing.  */

extern void
sort_block_syms PARAMS ((struct block *));

extern void
sort_symtab_syms PARAMS ((struct symtab *));

extern void
sort_all_symtab_syms PARAMS ((void));

/* Make a copy of the string at PTR with SIZE characters in the symbol obstack
   (and add a null character at the end in the copy).
   Returns the address of the copy.  */

extern char *
obsavestring PARAMS ((char *, int, struct obstack *));

/* Concatenate strings S1, S2 and S3; return the new string.
   Space is found in the symbol_obstack.  */

extern char *
obconcat PARAMS ((struct obstack *obstackp, const char *, const char *,
		  const char *));

			/*   Variables   */

/* The object file that the main symbol table was loaded from (e.g. the
   argument to the "symbol-file" or "file" command).  */

extern struct objfile *symfile_objfile;

/* Where execution starts in symfile */

extern CORE_ADDR entry_point;

/* Root of object file struct chain.  */

extern struct objfile *object_files;

/* Traverse all object files.  ALL_OBJFILES_SAFE works even if you delete
   the objfile during the traversal.  */

#define	ALL_OBJFILES(obj) \
   for (obj = object_files; 0 != obj; obj = obj->next)

#define	ALL_OBJFILES_SAFE(obj,nxt) \
   for (obj = object_files; obj? (nxt=obj->next, 1): 0; obj = nxt)

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

extern void
complain PARAMS ((struct complaint *, char *));

extern void
clear_complaints PARAMS ((int sym_reading, int noisy));

/* From symfile.c */

extern struct partial_symtab *
allocate_psymtab PARAMS ((char *, struct objfile *));

/* From minsyms.c */

extern PTR
iterate_over_msymbols PARAMS ((PTR (*func) (struct objfile *,
					    struct minimal_symbol *,
					    PTR arg1, PTR arg2, PTR arg3),
			       PTR arg1, PTR arg2, PTR arg3));

/* From objfiles.c */

extern PTR
iterate_over_objfiles PARAMS ((PTR (*func) (struct objfile *,
					    PTR arg1, PTR arg2, PTR arg3),
			       PTR arg1, PTR arg2, PTR arg3));

extern PTR
iterate_over_symtabs PARAMS ((PTR (*func) (struct objfile *, struct symtab *,
					   PTR arg1, PTR arg2, PTR arg3),
			      PTR arg1, PTR arg2, PTR arg3));

extern PTR 
iterate_over_psymtabs PARAMS ((PTR (*func) (struct objfile *,
					    struct partial_symtab *,
					    PTR arg1, PTR arg2, PTR arg3),
			       PTR arg1, PTR arg2, PTR arg3));

/* From dwarfread.c */

extern void
dwarf_build_psymtabs PARAMS ((int, char *, CORE_ADDR, int, unsigned int,
			      unsigned int, unsigned int, unsigned int,
			      struct objfile *));

#endif	/* !defined(SYMFILE_H) */
