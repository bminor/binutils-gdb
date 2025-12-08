/* Self tests for blockvectors

   Copyright (C) 2025 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "gdbsupport/selftest.h"
#include "block.h"
#include "addrmap.h"


namespace selftests {

/* Create and add new block to given blockvector.  */
static struct block *
make_block (struct blockvector &bv, struct obstack &ob, CORE_ADDR start,
	    CORE_ADDR end, struct block *superblock = nullptr)
{
  auto b = new (&ob) struct block;
  b->set_start (start);
  b->set_end (end);
  b->set_superblock (superblock);
  b->set_multidict (mdict_create_linear_expandable (language_unknown));

  bv.append_block (b);

  return b;
}

/* A helper to create and set address map given a blockvector.  */
static void
make_map (struct blockvector &bv, struct obstack &ob)
{
  struct addrmap_mutable map;

  for (int i = bv.num_blocks () - 1; i > STATIC_BLOCK; i--)
    {
      auto b = bv.block (i);
      map.set_empty (b->start (), b->end () - 1, b);
    }

  bv.set_map (new (&ob) addrmap_fixed (&ob, &map));
}

/* Create and return blockvector with following blocks:

	B0    0x1000 - 0x4000   (global block)
	B1    0x1000 - 0x4000   (static block)
	 B2   0x1000 - 0x2000
				(hole)
	 B3   0x3000 - 0x4000

  If USE_MAP is true, then also set blockvector's address map.
*/
static blockvector_up
make_blockvector (struct obstack &ob, bool use_map)
{
  auto bv = std::make_unique<struct blockvector> (0);

  auto global_block = make_block (*bv.get (), ob, 0x1000, 0x4000);
  auto static_block = make_block (*bv.get (), ob, 0x1000, 0x4000,
				  global_block);
  make_block (*bv.get (), ob, 0x1000, 0x2000, static_block);
  make_block (*bv.get (), ob, 0x3000, 0x4000, static_block);

  if (use_map)
    make_map (*bv.get (), ob);

  return bv;
}

static void
test_blockvector_lookup_contains ()
{
  for (bool with_map : { false, true })
    {
      /* Test blockvector without an address map.  */
      auto_obstack ob;
      blockvector_up bv = make_blockvector (ob, with_map);

      /* Test address outside global block's range.  */
      SELF_CHECK (bv->lookup (0x0500) == nullptr);
      SELF_CHECK (bv->contains (0x0500) == false);

      /* Test address falling into a block.  */
      SELF_CHECK (bv->lookup (0x1500) == bv->block (2));
      SELF_CHECK (bv->contains (0x1500) == true);

      /* Test address falling into a "hole".  If BV has an address map,
	 lookup () returns nullptr and contains (). returns false.  If not,
	 lookup () return static block and contains() returns true.  */
      if (with_map)
	{
	  SELF_CHECK (bv->lookup (0x2500) == nullptr);
	  SELF_CHECK (bv->contains (0x2500) == false);
	}
      else
	{
	  SELF_CHECK (bv->lookup (0x2500) == bv->block (STATIC_BLOCK));
	  SELF_CHECK (bv->contains (0x2500) == true);
	}

      /* Test address falling into a block above the "hole".  */
      SELF_CHECK (bv->lookup (0x3500) == bv->block (3));
      SELF_CHECK (bv->contains (0x3500) == true);

      /* Test address outside global block's range.  */
      SELF_CHECK (bv->lookup (0x4000) == nullptr);
      SELF_CHECK (bv->contains (0x4000) == false);
    }
}

} /* namespace selftests */


INIT_GDB_FILE (block_selftest)
{
  selftests::register_test ("blockvector-lookup-contains",
			    selftests::test_blockvector_lookup_contains);
}
