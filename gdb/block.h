/* Code dealing with blocks for GDB.

   Copyright 2003 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef BLOCK_H
#define BLOCK_H

/* Opaque declarations.  */

struct symbol;
struct symtab;
struct block_namespace_info;
struct using_direct;
struct obstack;

/* All of the name-scope contours of the program
   are represented by `struct block' objects.
   All of these objects are pointed to by the blockvector.

   Each block represents one name scope.
   Each lexical context has its own block.

   The blockvector begins with some special blocks.
   The GLOBAL_BLOCK contains all the symbols defined in this compilation
   whose scope is the entire program linked together.
   The STATIC_BLOCK contains all the symbols whose scope is the
   entire compilation excluding other separate compilations.
   Blocks starting with the FIRST_LOCAL_BLOCK are not special.

   Each block records a range of core addresses for the code that
   is in the scope of the block.  The STATIC_BLOCK and GLOBAL_BLOCK
   give, for the range of code, the entire range of code produced
   by the compilation that the symbol segment belongs to.

   The blocks appear in the blockvector
   in order of increasing starting-address,
   and, within that, in order of decreasing ending-address.

   This implies that within the body of one function
   the blocks appear in the order of a depth-first tree walk.  */

struct block
{

  /* Addresses in the executable code that are in this block.  */

  CORE_ADDR startaddr;
  CORE_ADDR endaddr;

  /* The symbol that names this block, if the block is the body of a
     function; otherwise, zero.  */

  struct symbol *function;

  /* The `struct block' for the containing block, or 0 if none.

     The superblock of a top-level local block (i.e. a function in the
     case of C) is the STATIC_BLOCK.  The superblock of the
     STATIC_BLOCK is the GLOBAL_BLOCK.  */

  struct block *superblock;

  /* Used for language-specific info.  */

  union
  {
    struct
    {
      /* Contains information about namespace-related info relevant to
	 this block: using directives and the current namespace
	 scope.  */
      
      struct block_namespace_info *namespace;
    }
    cplus_specific;
  }
  language_specific;

  /* Version of GCC used to compile the function corresponding
     to this block, or 0 if not compiled with GCC.  When possible,
     GCC should be compatible with the native compiler, or if that
     is not feasible, the differences should be fixed during symbol
     reading.  As of 16 Apr 93, this flag is never used to distinguish
     between gcc2 and the native compiler.

     If there is no function corresponding to this block, this meaning
     of this flag is undefined.  */

  unsigned char gcc_compile_flag;

  /* The symbols for this block are either in a simple linear list or
     in a simple hashtable.  Blocks which correspond to a function
     (which have a list of symbols corresponding to arguments) use
     a linear list, as do some older symbol readers (currently only
     mdebugread and dstread).  Other blocks are hashed.

     The hashtable uses the same hash function as the minsym hashtables,
     found in minsyms.c:minsym_hash_iw.  Symbols are hashed based on
     their demangled name if appropriate, and on their name otherwise.
     The hash function ignores space, and stops at the beginning of the
     argument list if any.

     The table is laid out in NSYMS/5 buckets and symbols are chained via
     their hash_next field.  */

  /* If this is really a hashtable of the symbols, this flag is 1.  */

  unsigned char hashtable;

  /* Number of local symbols.  */

  int nsyms;

  /* The symbols.  If some of them are arguments, then they must be
     in the order in which we would like to print them.  */

  struct symbol *sym[1];
};

#define BLOCK_START(bl)		(bl)->startaddr
#define BLOCK_END(bl)		(bl)->endaddr
#define BLOCK_FUNCTION(bl)	(bl)->function
#define BLOCK_SUPERBLOCK(bl)	(bl)->superblock
#define BLOCK_GCC_COMPILED(bl)	(bl)->gcc_compile_flag
#define BLOCK_NAMESPACE(bl)   (bl)->language_specific.cplus_specific.namespace
#define BLOCK_HASHTABLE(bl)	(bl)->hashtable

/* For blocks without a hashtable (BLOCK_HASHTABLE (bl) == 0) only.  */
#define BLOCK_NSYMS(bl)		(bl)->nsyms
#define BLOCK_SYM(bl, n)	(bl)->sym[n]

/* For blocks with a hashtable, but these are valid for non-hashed blocks as
   well - each symbol will appear to be one bucket by itself.  */
#define BLOCK_BUCKETS(bl)	(bl)->nsyms
#define BLOCK_BUCKET(bl, n)	(bl)->sym[n]

/* Macro used to set the size of a hashtable for N symbols.  */
#define BLOCK_HASHTABLE_SIZE(n)	((n)/5 + 1)

/* Macro to loop through all symbols in a block BL, in no particular order.
   i counts which bucket we are in, and sym points to the current symbol.  */

#define ALL_BLOCK_SYMBOLS(bl, i, sym)				\
	for ((i) = 0; (i) < BLOCK_BUCKETS ((bl)); (i)++)	\
	  for ((sym) = BLOCK_BUCKET ((bl), (i)); (sym);		\
	       (sym) = (sym)->hash_next)

/* Nonzero if symbols of block BL should be sorted alphabetically.
   Don't sort a block which corresponds to a function.  If we did the
   sorting would have to preserve the order of the symbols for the
   arguments.  Also don't sort any block that we chose to hash.  */

#define BLOCK_SHOULD_SORT(bl) (! BLOCK_HASHTABLE (bl) \
			       && BLOCK_FUNCTION (bl) == NULL)

struct blockvector
{
  /* Number of blocks in the list.  */
  int nblocks;
  /* The blocks themselves.  */
  struct block *block[1];
};

#define BLOCKVECTOR_NBLOCKS(blocklist) (blocklist)->nblocks
#define BLOCKVECTOR_BLOCK(blocklist,n) (blocklist)->block[n]

/* Special block numbers */

#define GLOBAL_BLOCK		0
#define	STATIC_BLOCK		1
#define	FIRST_LOCAL_BLOCK	2

extern struct symbol *block_function (struct block *);

extern int contained_in (struct block *, struct block *);

extern struct blockvector *blockvector_for_pc (CORE_ADDR, int *);

extern struct blockvector *blockvector_for_pc_sect (CORE_ADDR, asection *,
						    int *, struct symtab *);

extern struct block *block_for_pc (CORE_ADDR);

extern struct block *block_for_pc_sect (CORE_ADDR, asection *);

extern void block_set_scope (struct block *block, const char *scope,
			     struct obstack *obstack);

extern void block_set_using (struct block *block,
			     struct using_direct *using,
			     struct obstack *obstack);

#endif /* BLOCK_H */
