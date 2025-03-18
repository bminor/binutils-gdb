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
  : m_shard (new cooked_index_shard)
{
}

/* See cooked-index-storage.h.  */

cutu_reader *
cooked_index_storage::get_reader (dwarf2_per_cu *per_cu)
{
  auto it = m_reader_hash.find (*per_cu);
  return it != m_reader_hash.end () ? it->get () : nullptr;
}

/* See cooked-index-storage.h.  */

cutu_reader *
cooked_index_storage::preserve (cutu_reader_up reader)
{
  m_abbrev_table_cache.add (reader->release_abbrev_table ());

  auto [it, inserted] = m_reader_hash.insert (std::move (reader));
  gdb_assert (inserted);

  return it->get();
}

/* See cooked-index-storage.h.  */

std::uint64_t
cooked_index_storage::cutu_reader_hash::operator()
  (const cutu_reader_up &reader) const noexcept
{
  return (*this) (*reader->cu ()->per_cu);
}

/* See cooked-index-storage.h.  */

std::uint64_t
cooked_index_storage::cutu_reader_hash::operator() (const dwarf2_per_cu &per_cu)
  const noexcept
{
  return per_cu.index;
}

/* See cooked-index-storage.h.  */

bool
cooked_index_storage::cutu_reader_eq::operator() (const cutu_reader_up &a,
						  const cutu_reader_up &b) const noexcept
{
  return (*this) (*a->cu ()->per_cu, b);
}

/* See cooked-index-storage.h.  */

bool cooked_index_storage::cutu_reader_eq::operator()
  (const dwarf2_per_cu &per_cu, const cutu_reader_up &reader) const noexcept
{
  return per_cu.index == reader->cu ()->per_cu->index;
}
