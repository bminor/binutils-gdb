/* DWARF abbrev table cache

   Copyright (C) 2022-2024 Free Software Foundation, Inc.

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

#ifndef GDB_DWARF2_ABBREV_CACHE_H
#define GDB_DWARF2_ABBREV_CACHE_H

#include "dwarf2/abbrev.h"
#include "gdbsupport/unordered_map.h"

/* An abbrev cache holds abbrev tables for easier reuse.  */
class abbrev_cache
{
public:
  abbrev_cache () = default;
  DISABLE_COPY_AND_ASSIGN (abbrev_cache);

  /* Find an abbrev table coming from the abbrev section SECTION at
     offset OFFSET.  Return the table, or nullptr if it has not yet
     been registered.  */
  abbrev_table *find (struct dwarf2_section_info *section, sect_offset offset)
  {
    if (auto iter = m_tables.find ({ section, offset });
	iter != m_tables.end ())
      return iter->second.get ();

    return nullptr;
  }

  /* Add TABLE to this cache.  Ownership of TABLE is transferred to
     the cache.  Note that a table at a given section+offset may only
     be registered once -- a violation of this will cause an assert.
     To avoid this, call the 'find' method first, to see if the table
     has already been read.  */
  void add (abbrev_table_up table);

private:
  struct key
  {
    dwarf2_section_info *section;
    sect_offset offset;
  };

  struct key_hash
  {
    std::size_t operator() (const key &k) const noexcept
    {
      return (std::hash<dwarf2_section_info *> () (k.section)
	      + std::hash<std::uint64_t> () (to_underlying (k.offset)));
    }
  };

  struct key_eq
  {
    bool operator() (const key &lhs, const key &rhs) const noexcept
    { return lhs.section == rhs.section && lhs.offset == rhs.offset; }
  };

  /* Hash table of abbrev tables.  */
  gdb::unordered_map<key, abbrev_table_up, key_hash, key_eq> m_tables;
};

#endif /* GDB_DWARF2_ABBREV_CACHE_H */
