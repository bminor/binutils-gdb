/* Base class for mapped indices

   Copyright (C) 2021 Free Software Foundation, Inc.

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

#ifndef GDB_DWARF2_MAPPED_INDEX_H
#define GDB_DWARF2_MAPPED_INDEX_H

#include "language.h"

/* An index into a (C++) symbol name component in a symbol name as
   recorded in the mapped_index's symbol table.  For each C++ symbol
   in the symbol table, we record one entry for the start of each
   component in the symbol in a table of name components, and then
   sort the table, in order to be able to binary search symbol names,
   ignoring leading namespaces, both completion and regular look up.
   For example, for symbol "A::B::C", we'll have an entry that points
   to "A::B::C", another that points to "B::C", and another for "C".
   Note that function symbols in GDB index have no parameter
   information, just the function/method names.  You can convert a
   name_component to a "const char *" using the
   'mapped_index::symbol_name_at(offset_type)' method.  */

struct name_component
{
  /* Offset in the symbol name where the component starts.  Stored as
     a (32-bit) offset instead of a pointer to save memory and improve
     locality on 64-bit architectures.  */
  offset_type name_offset;

  /* The symbol's index in the symbol and constant pool tables of a
     mapped_index.  */
  offset_type idx;
};

/* Base class containing bits shared by both .gdb_index and
   .debug_name indexes.  */

struct mapped_index_base
{
  mapped_index_base () = default;
  DISABLE_COPY_AND_ASSIGN (mapped_index_base);

  /* The name_component table (a sorted vector).  See name_component's
     description above.  */
  std::vector<name_component> name_components;

  /* How NAME_COMPONENTS is sorted.  */
  enum case_sensitivity name_components_casing;

  /* Return the number of names in the symbol table.  */
  virtual size_t symbol_name_count () const = 0;

  /* Get the name of the symbol at IDX in the symbol table.  */
  virtual const char *symbol_name_at
    (offset_type idx, dwarf2_per_objfile *per_objfile) const = 0;

  /* Return whether the name at IDX in the symbol table should be
     ignored.  */
  virtual bool symbol_name_slot_invalid (offset_type idx) const
  {
    return false;
  }

  /* Build the symbol name component sorted vector, if we haven't
     yet.  */
  void build_name_components (dwarf2_per_objfile *per_objfile);

  /* Returns the lower (inclusive) and upper (exclusive) bounds of the
     possible matches for LN_NO_PARAMS in the name component
     vector.  */
  std::pair<std::vector<name_component>::const_iterator,
	    std::vector<name_component>::const_iterator>
    find_name_components_bounds (const lookup_name_info &ln_no_params,
				 enum language lang,
				 dwarf2_per_objfile *per_objfile) const;

  /* Prevent deleting/destroying via a base class pointer.  */
protected:
  ~mapped_index_base() = default;
};

#endif /* GDB_DWARF2_MAPPED_INDEX_H */
