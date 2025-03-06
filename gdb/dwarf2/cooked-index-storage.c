/* DWARF index storage

   Copyright (C) 2022-2025 Free Software Foundation, Inc.

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

#include "dwarf2/cooked-index-storage.h"

/* See cooked-index-storage.h.  */

cooked_index_storage::cooked_index_storage ()
  : m_reader_hash (htab_create_alloc (10, hash_cutu_reader,
				      eq_cutu_reader,
				      htab_delete_entry<cutu_reader>,
				      xcalloc, xfree)),
    m_shard (new cooked_index_shard)
{
}

/* See cooked-index-storage.h.  */

cutu_reader *
cooked_index_storage::get_reader (dwarf2_per_cu *per_cu)
{
  int index = per_cu->index;
  return (cutu_reader *) htab_find_with_hash (m_reader_hash.get (),
					      &index, index);
}

/* See cooked-index-storage.h.  */

cutu_reader *
cooked_index_storage::preserve (cutu_reader_up reader)
{
  m_abbrev_table_cache.add (reader->release_abbrev_table ());

  int index = reader->cu ()->per_cu->index;
  void **slot = htab_find_slot_with_hash (m_reader_hash.get (), &index,
					  index, INSERT);
  gdb_assert (*slot == nullptr);
  cutu_reader *result = reader.get ();
  *slot = reader.release ();
  return result;
}

/* See cooked-index-storage.h.  */

hashval_t
cooked_index_storage::hash_cutu_reader (const void *a)
{
  const cutu_reader *reader = (const cutu_reader *) a;
  return reader->cu ()->per_cu->index;
}

/* See cooked-index-storage.h.  */

int
cooked_index_storage::eq_cutu_reader (const void *a, const void *b)
{
  const cutu_reader *ra = (const cutu_reader *) a;
  const int *rb = (const int *) b;
  return ra->cu ()->per_cu->index == *rb;
}
