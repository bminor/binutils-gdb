/* gdb_block.c - Deal with GDB blocks
   Copyright (C) 1987 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler.

GAS is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
 * Implements .gdbblk, .gdbbeg, .gdbend concepts.
 * No other modules need to know the details of these concepts.
 *
 * During assembly, note the addresses of block beginnings and ends.
 * Each block has a begin-address, an end-address, a number, and
 * a place in the GDB symbol file to place the 2 addresses.
 * Block numbers are 0, 1, ... with no gaps.
 *
 * During assembly, we don't actually know the addresses, so they are
 * expressed as {frag-address + offset-in-frag}.
 *
 * gdb_block_begin ()
 *		Call once before using this package.
 *
 * gdb_block_beg  (number, frag, offset)
 *		Note a block beginning.
 *
 * gdb_block_end    (number, frag, offset)
 *		Note a block end.
 *
 * gdb_block_position (block_number, pos)
 *		Remember, after assembly, to copy a structure containing
 *		the beginning and ending addresses of block number
 *		block_number into the gdb file, starting at position pos.
 *
 * gdb_block_emit  (block_number, where_in_gdb_symbol_file)
 *		Emit a block begin/end locations to a place in the GDB symbol
 *		file.
 *
 * uses:
 *	xmalloc()
 *	gdb_alter()
 */


#include "as.h"

/*
 * malloc() calls are considered expensive. So ...
 *
 * We remember blocks by making a tree, and each block number has a leaf.
 * The tree is 3 levels, and we don't allocate interior nodes until they
 * are needed. Both leaves and interior nodes are allocated in lumps,
 * which should save on malloc() calls. Due to the way we break up a
 * block number to navigate through the tree, we insist that lumps of
 * memory contain a power of 2 items each. Powers of 2 may differ
 * for different levels of tree.
 */

/*
 *	A block number:
 *
 *	+---------------+---------------+---------------+
 *	|		|		|		|
 *	|  Z2-part bits	|  Z1-part bits	|  Z0-part bits	|
 *	|		|		|		|
 *	+---------------+---------------+---------------+
 *
 *	High order				Low order
 *
 * "Z" is short for "siZe".
 */

#define LOG_2_Z0 (8)		/* How many bits are in Z0 part? */
#define LOG_2_Z1 (8)		/* How many bits are in Z1 part? */
#define LOG_2_Z2 (8)		/* How many bits are in Z2 part? */

#define BLOCK_NUMBER_LIMIT (1 << (LOG_2_Z0 + LOG_2_Z1 + LOG_2_Z2))
				/* What is the first block number that is */
				/* "too big"? */

struct gdb_block
{
  fragS *	begin_frag;
  fragS *	  end_frag;
  long int	begin_where_in_frag;
  long int	  end_where_in_frag;
  long int	position;	/* In GDB symbols file. */
};

typedef struct gdb_block	node_0_T	[1 << LOG_2_Z0];

typedef node_0_T *		node_1_T	[1 << LOG_2_Z1];

typedef node_1_T *		node_2_T	[1 << LOG_2_Z2];


static long int		highest_block_number_seen;
static node_2_T *	root;	/* 3 level tree of block locations. */

static node_2_T * new_2 ();


char * xmalloc();
void gdb_alter();

void
gdb_block_begin ()
{
  root = new_2 ();
  highest_block_number_seen = -1;
}

static node_0_T *
new_0 ()
{
  register node_0_T *	place;

  place = (node_0_T *) xmalloc ((long)sizeof(node_0_T));
  bzero ((char *)place, sizeof(node_0_T));
  return (place);
}

static node_1_T *
new_1 ()
{
  register node_1_T *	place;

  place = (node_1_T *) xmalloc ((long)sizeof(node_1_T));
  bzero ((char *)place, sizeof(node_1_T));
  return (place);
}

static node_2_T *
new_2 ()
{
  register node_2_T *	place;

  place = (node_2_T *) xmalloc ((long)sizeof(node_2_T));
  bzero ((char *)place, sizeof(node_2_T));
  return (place);
}

static struct gdb_block *
find (block_number)
     register long int	block_number;
{
  register node_1_T **		pp_1;
  register node_0_T **		pp_0;
  register struct gdb_block *	b;
  register int			index0;
  register int			index1;
  register int			index2;

#ifdef SUSPECT
  if (block_number >= BLOCK_NUMBER_LIMIT)
    {
      as_fatal ("gdb_block: Block number = %ld.", block_number);
    }
#endif

  index2 = block_number >> (LOG_2_Z0 + LOG_2_Z1);
  index1 = block_number >> (LOG_2_Z0) & ((1 << LOG_2_Z1) - 1);
  index0 = block_number & ((1 << LOG_2_Z0) - 1);
  pp_1 = * root + index2;
  if (* pp_1 == 0)
    {
      * pp_1 = new_1 ();
    }
  pp_0 = ** pp_1 + index1;
  if (* pp_0 == 0)
    {
      * pp_0 = new_0 ();
    }
  b = ** pp_0 + index0;
  return (b);
}


static struct gdb_block *
find_create (block_number)
     long int	block_number;
{
  if (highest_block_number_seen < block_number)
    {
      highest_block_number_seen = block_number;
    }
  return (find (block_number));
}

void
gdb_block_beg (block_number, frag, offset)
     long int	block_number;
     fragS *	frag;
     long int	offset;
{
  struct gdb_block *	pointer;
      
  pointer = find_create (block_number);
#ifdef SUSPECT
  if (pointer -> begin_frag != 0)
    {
      as_warn( "Overwriting begin_frag for block # %ld.", block_number );
    }
  if (pointer -> begin_where_in_frag != 0)
    {
      as_warn( "Overwriting begin_where_in_frag for block # %ld.", block_number );
    }
#endif
  pointer -> begin_frag 	 = frag;
  pointer -> begin_where_in_frag = offset;
}

void
gdb_block_end (block_number, frag, offset)
     long int	block_number;
     fragS *	frag;
     long int	offset;
{
  struct gdb_block *	pointer;
      
  pointer = find_create (block_number);
#ifdef SUSPECT
  if (pointer -> end_frag != 0)
    {
      as_warn( "Overwriting end_frag for block # %ld.", block_number );
    }
  if (pointer -> end_where_in_frag != 0)
    {
      as_warn( "Overwriting end_where_in_frag for block # %ld.", block_number );
    }
#endif
  pointer -> end_frag	       = frag;
  pointer -> end_where_in_frag = offset;
}

void
gdb_block_position (block_number, pos)
     long int	block_number;
     long int	pos;
{
  struct gdb_block *	pointer;

  pointer = find_create (block_number);
  if (pointer -> position != 0)
    {
      as_warn( "Overwriting old position %ld. in block #%ld.",
	      pointer -> position, block_number);
    }
  pointer -> position = pos;
}

void
gdb_block_emit ()
{
  long int		block_number;
  struct gdb_block *	b;

  for (block_number = 0;
       block_number <= highest_block_number_seen;
       block_number ++)
    {
      b = find (block_number);
      if (b -> begin_frag)
	{
	  gdb_alter (b -> position,
		     (long int)
		     (b -> begin_frag -> fr_address + b -> begin_where_in_frag));
	}
      if (b -> end_frag)
	{
	  gdb_alter (b -> position + sizeof( long int ),
		     (long int)
		     (b -> end_frag -> fr_address + b -> end_where_in_frag));
	}
    }
}

/* end: gdb_block.c */
