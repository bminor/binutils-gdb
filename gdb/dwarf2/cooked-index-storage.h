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

#ifndef GDB_DWARF2_COOKED_INDEX_STORAGE_H
#define GDB_DWARF2_COOKED_INDEX_STORAGE_H

#include "dwarf2/abbrev-table-cache.h"
#include "dwarf2/cooked-index.h"
#include "dwarf2/types.h"

struct cutu_reader;
struct dwarf2_per_cu;

using cutu_reader_up = std::unique_ptr<cutu_reader>;

/* An instance of this is created when scanning DWARF to create a
   cooked index.  */

class cooked_index_storage
{
public:

  cooked_index_storage ();
  DISABLE_COPY_AND_ASSIGN (cooked_index_storage);

  /* Return the current abbrev table_cache.  */
  const abbrev_table_cache &get_abbrev_table_cache () const
  { return m_abbrev_table_cache; }

  /* Return the DIE reader corresponding to PER_CU.  If no such reader
     has been registered, return NULL.  */
  cutu_reader *get_reader (dwarf2_per_cu *per_cu);

  /* Preserve READER by storing it in the local hash table.  */
  cutu_reader *preserve (cutu_reader_up reader);

  /* Add an entry to the index.  The arguments describe the entry; see
     cooked-index.h.  The new entry is returned.  */
  cooked_index_entry *add (sect_offset die_offset, enum dwarf_tag tag,
			   cooked_index_flag flags,
			   const char *name,
			   cooked_index_entry_ref parent_entry,
			   dwarf2_per_cu *per_cu)
  {
    return m_shard->add (die_offset, tag, flags, per_cu->lang (),
			 name, parent_entry, per_cu);
  }

  /* Install the current addrmap into the shard being constructed,
     then transfer ownership of the index to the caller.  */
  cooked_index_shard_up release ()
  {
    m_shard->install_addrmap (&m_addrmap);
    return std::move (m_shard);
  }

  /* Return the mutable addrmap that is currently being created.  */
  addrmap_mutable *get_addrmap ()
  {
    return &m_addrmap;
  }

  /* Return the parent_map that is currently being created.  */
  parent_map *get_parent_map ()
  {
    return &m_parent_map;
  }

  /* Return the parent_map that is currently being created.  Ownership
     is passed to the caller.  */
  parent_map release_parent_map ()
  {
    return std::move (m_parent_map);
  }

private:

  /* Hash function for a cutu_reader.  */
  static hashval_t hash_cutu_reader (const void *a);

  /* Equality function for cutu_reader.  */
  static int eq_cutu_reader (const void *a, const void *b);

  /* The abbrev table cache used by this indexer.  */
  abbrev_table_cache m_abbrev_table_cache;

  /* A hash table of cutu_reader objects.  */
  htab_up m_reader_hash;
  /* The index shard that is being constructed.  */
  cooked_index_shard_up m_shard;

  /* Parent map for each CU that is read.  */
  parent_map m_parent_map;

  /* A writeable addrmap being constructed by this scanner.  */
  addrmap_mutable m_addrmap;
};

#endif /* GDB_DWARF2_COOKED_INDEX_STORAGE_H */
