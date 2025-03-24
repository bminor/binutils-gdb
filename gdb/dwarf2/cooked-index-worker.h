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

#ifndef GDB_DWARF2_COOKED_INDEX_WORKER_H
#define GDB_DWARF2_COOKED_INDEX_WORKER_H

#include "dwarf2/abbrev-table-cache.h"
#include "dwarf2/cooked-index-entry.h"
#include "dwarf2/cooked-index-shard.h"
#include "dwarf2/types.h"
#include "dwarf2/read.h"

using cutu_reader_up = std::unique_ptr<cutu_reader>;

/* An instance of this is created when scanning DWARF to create a
   cooked index.  This class is the result of a single task to store
   results while working -- that is, it is an implementation detail of
   the threads managed by cooked_index_worker.  Once scanning is done,
   selected parts of the state here are stored into the shard, and
   then these temporary objects are destroyed.  */

class cooked_index_worker_result
{
public:

  cooked_index_worker_result ();
  DISABLE_COPY_AND_ASSIGN (cooked_index_worker_result);

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
  /* The abbrev table cache used by this indexer.  */
  abbrev_table_cache m_abbrev_table_cache;

  /* Hash function for a cutu_reader.  */
  struct cutu_reader_hash
  {
    using is_transparent = void;

    std::uint64_t operator() (const cutu_reader_up &reader) const noexcept;
    std::uint64_t operator() (const dwarf2_per_cu &per_cu) const noexcept;
  };

  /* Equality function for cutu_reader.  */
  struct cutu_reader_eq
  {
    using is_transparent = void;

    bool operator() (const cutu_reader_up &a,
		     const cutu_reader_up &b) const noexcept;

    bool operator() (const dwarf2_per_cu &per_cu,
		     const cutu_reader_up &reader) const noexcept;
  };

  /* A hash table of cutu_reader objects.  */
  gdb::unordered_set<cutu_reader_up, cutu_reader_hash, cutu_reader_eq>
    m_reader_hash;

  /* The index shard that is being constructed.  */
  cooked_index_shard_up m_shard;

  /* Parent map for each CU that is read.  */
  parent_map m_parent_map;

  /* A writeable addrmap being constructed by this scanner.  */
  addrmap_mutable m_addrmap;
};

#endif /* GDB_DWARF2_COOKED_INDEX_WORKER_H */
