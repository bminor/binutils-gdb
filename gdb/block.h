/* Block definitions for GDB.
   Copyright 1986, 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996,
   1997, 1998, 1999, 2000, 2001, 2002
   Free Software Foundation, Inc.

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

/* Opaque declarations.  */

struct symbol;
struct dictionary;
struct namespace_info;
struct using_direct_node;
struct obstack;

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

  /* This is used to store the symbols in the block.  */

  struct dictionary *dict;

  /* Used for language-specific info.  */

  union
  {
    struct
    {
      /* Contains information about namespace-related info relevant to
	 this block: using directives and the current namespace
	 scope.  */
      
      struct namespace_info *namespace;
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
};

#define BLOCK_START(bl)		(bl)->startaddr
#define BLOCK_END(bl)		(bl)->endaddr
#define BLOCK_FUNCTION(bl)	(bl)->function
#define BLOCK_SUPERBLOCK(bl)	(bl)->superblock
#define BLOCK_DICT(bl)		(bl)->dict
#define BLOCK_NAMESPACE(bl)	(bl)->language_specific.cplus_specific.namespace
#define BLOCK_GCC_COMPILED(bl)	(bl)->gcc_compile_flag

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

extern struct using_direct_node *block_using (const struct block *);

extern struct using_direct_node *block_all_usings (const struct block *block);

extern void block_set_using (struct block *block,
			     struct using_direct_node *using,
			     struct obstack *obstack);

extern const char *block_scope (const struct block *block);

extern void block_set_scope (struct block *block, const char *scope,
			     struct obstack *obstack);

extern const struct block *block_static_block (const struct block *block);
