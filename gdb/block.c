/* Block-related functions for the GNU debugger, GDB.

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

#include "defs.h"
#include "block.h"
#include "symtab.h"
#include "symfile.h"

/* Return Nonzero if block a is lexically nested within block b,
   or if a and b have the same pc range.
   Return zero otherwise. */

int
contained_in (struct block *a, struct block *b)
{
  if (!a || !b)
    return 0;
  return BLOCK_START (a) >= BLOCK_START (b)
    && BLOCK_END (a) <= BLOCK_END (b);
}


/* Return the symbol for the function which contains a specified
   lexical block, described by a struct block BL.  */

struct symbol *
block_function (struct block *bl)
{
  while (BLOCK_FUNCTION (bl) == 0 && BLOCK_SUPERBLOCK (bl) != 0)
    bl = BLOCK_SUPERBLOCK (bl);

  return BLOCK_FUNCTION (bl);
}

/* Return the blockvector immediately containing the innermost lexical block
   containing the specified pc value and section, or 0 if there is none.
   PINDEX is a pointer to the index value of the block.  If PINDEX
   is NULL, we don't pass this information back to the caller.  */

struct blockvector *
blockvector_for_pc_sect (register CORE_ADDR pc, struct sec *section,
			 int *pindex, struct symtab *symtab)
{
  register struct block *b;
  register int bot, top, half;
  struct blockvector *bl;

  if (symtab == 0)		/* if no symtab specified by caller */
    {
      /* First search all symtabs for one whose file contains our pc */
      if ((symtab = find_pc_sect_symtab (pc, section)) == 0)
	return 0;
    }

  bl = BLOCKVECTOR (symtab);
  b = BLOCKVECTOR_BLOCK (bl, 0);

  /* Then search that symtab for the smallest block that wins.  */
  /* Use binary search to find the last block that starts before PC.  */

  bot = 0;
  top = BLOCKVECTOR_NBLOCKS (bl);

  while (top - bot > 1)
    {
      half = (top - bot + 1) >> 1;
      b = BLOCKVECTOR_BLOCK (bl, bot + half);
      if (BLOCK_START (b) <= pc)
	bot += half;
      else
	top = bot + half;
    }

  /* Now search backward for a block that ends after PC.  */

  while (bot >= 0)
    {
      b = BLOCKVECTOR_BLOCK (bl, bot);
      if (BLOCK_END (b) > pc)
	{
	  if (pindex)
	    *pindex = bot;
	  return bl;
	}
      bot--;
    }
  return 0;
}

/* Return the blockvector immediately containing the innermost lexical block
   containing the specified pc value, or 0 if there is none.
   Backward compatibility, no section.  */

struct blockvector *
blockvector_for_pc (register CORE_ADDR pc, int *pindex)
{
  return blockvector_for_pc_sect (pc, find_pc_mapped_section (pc),
				  pindex, NULL);
}

/* Return the innermost lexical block containing the specified pc value
   in the specified section, or 0 if there is none.  */

struct block *
block_for_pc_sect (register CORE_ADDR pc, struct sec *section)
{
  register struct blockvector *bl;
  int index;

  bl = blockvector_for_pc_sect (pc, section, &index, NULL);
  if (bl)
    return BLOCKVECTOR_BLOCK (bl, index);
  return 0;
}

/* Return the innermost lexical block containing the specified pc value,
   or 0 if there is none.  Backward compatibility, no section.  */

struct block *
block_for_pc (register CORE_ADDR pc)
{
  return block_for_pc_sect (pc, find_pc_mapped_section (pc));
}
