/* DIE indexing 

   Copyright (C) 2022 Free Software Foundation, Inc.

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

#ifndef GDB_DWARF2_COOKED_INDEX_H
#define GDB_DWARF2_COOKED_INDEX_H

#include "dwarf2.h"
#include "gdbtypes.h"
#include "symtab.h"
#include "hashtab.h"
#include "dwarf2/index-common.h"
#include "gdbsupport/gdb_string_view.h"
#include "quick-symbol.h"
#include "gdbsupport/gdb_obstack.h"
#include "addrmap.h"
#include "gdbsupport/iterator-range.h"

struct dwarf2_per_cu_data;

/* Flags that describe an entry in the index.  */
enum cooked_index_flag_enum : unsigned char
{
  /* True if this entry is the program's "main".  */
  IS_MAIN = 1,
  /* True if this entry represents a "static" object.  */
  IS_STATIC = 2,
  /* True if this entry is an "enum class".  */
  IS_ENUM_CLASS = 4,
  /* True if this entry uses the linkage name.  */
  IS_LINKAGE = 8,
};
DEF_ENUM_FLAGS_TYPE (enum cooked_index_flag_enum, cooked_index_flag);

/* A cooked_index_entry represents a single item in the index.  Note
   that two entries can be created for the same DIE -- one using the
   name, and another one using the linkage name, if any.

   This is an "open" class and the members are all directly
   accessible.  It is read-only after the index has been fully read
   and processed.  */
struct cooked_index_entry : public allocate_on_obstack
{
  cooked_index_entry (sect_offset die_offset_, enum dwarf_tag tag_,
		      cooked_index_flag flags_, const char *name_,
		      const cooked_index_entry *parent_entry_,
		      dwarf2_per_cu_data *per_cu_)
    : name (name_),
      tag (tag_),
      flags (flags_),
      die_offset (die_offset_),
      parent_entry (parent_entry_),
      per_cu (per_cu_)
  {
  }

  /* Return true if this entry matches SEARCH_FLAGS.  */
  bool matches (block_search_flags search_flags) const
  {
    if ((search_flags & SEARCH_STATIC_BLOCK) != 0
	&& (flags & IS_STATIC) != 0)
      return true;
    if ((search_flags & SEARCH_GLOBAL_BLOCK) != 0
	&& (flags & IS_STATIC) == 0)
      return true;
    return false;
  }

  /* Return true if this entry matches DOMAIN.  */
  bool matches (domain_enum domain) const
  {
    switch (domain)
      {
      case LABEL_DOMAIN:
	return false;

      case MODULE_DOMAIN:
	return tag == DW_TAG_module;

      case COMMON_BLOCK_DOMAIN:
	return tag == DW_TAG_common_block;
      }

    return true;
  }

  /* Return true if this entry matches KIND.  */
  bool matches (enum search_domain kind) const
  {
    switch (kind)
      {
      case VARIABLES_DOMAIN:
	return tag == DW_TAG_variable;
      case FUNCTIONS_DOMAIN:
	return tag == DW_TAG_subprogram;
      case TYPES_DOMAIN:
	return tag == DW_TAG_typedef || tag == DW_TAG_structure_type;
      case MODULES_DOMAIN:
	return tag == DW_TAG_module;
      }

    return true;
  }

  /* Construct the fully-qualified name of this entry and return a
     pointer to it.  If allocation is needed, it will be done on
     STORAGE.  */
  const char *full_name (struct obstack *storage) const;

  /* Entries must be sorted case-insensitively; this compares two
     entries.  */
  bool operator< (const cooked_index_entry &other) const
  {
    return strcasecmp (canonical, other.canonical) < 0;
  }

  /* The name as it appears in DWARF.  This always points into one of
     the mapped DWARF sections.  Note that this may be the name or the
     linkage name -- two entries are created for DIEs which have both
     attributes.  */
  const char *name;
  /* The canonical name.  For C++ names, this may differ from NAME.
     In all other cases, this is equal to NAME.  */
  const char *canonical = nullptr;
  /* The DWARF tag.  */
  enum dwarf_tag tag;
  /* Any flags attached to this entry.  */
  cooked_index_flag flags;
  /* The offset of this DIE.  */
  sect_offset die_offset;
  /* The parent entry.  This is NULL for top-level entries.
     Otherwise, it points to the parent entry, such as a namespace or
     class.  */
  const cooked_index_entry *parent_entry;
  /* The CU from which this entry originates.  */
  dwarf2_per_cu_data *per_cu;

private:

  void write_scope (struct obstack *storage, const char *sep) const;
};

/* An index of interesting DIEs.  This is "cooked", in contrast to a
   mapped .debug_names or .gdb_index, which are "raw".  An entry in
   the index is of type cooked_index_entry.

   Operations on the index are described below.  They are chosen to
   make it relatively simple to implement the symtab "quick"
   methods.  */
class cooked_index
{
public:
  cooked_index () = default;
  explicit cooked_index (cooked_index &&other) = default;
  DISABLE_COPY_AND_ASSIGN (cooked_index);
  cooked_index &operator= (cooked_index &&other) = default;

  /* Create a new cooked_index_entry and register it with this object.
     Entries are owned by this object.  The new item is returned.  */
  const cooked_index_entry *add (sect_offset die_offset, enum dwarf_tag tag,
				 cooked_index_flag flags,
				 const char *name,
				 const cooked_index_entry *parent_entry,
				 dwarf2_per_cu_data *per_cu);

  /* Return the entry that is believed to represent the program's
     "main".  This will return NULL if no such entry is available.  */
  const cooked_index_entry *get_main () const
  {
    return m_main;
  }

  /* Install a new fixed addrmap from the given mutable addrmap.  */
  void install_addrmap (addrmap *map)
  {
    gdb_assert (m_addrmap == nullptr);
    m_addrmap = addrmap_create_fixed (map, &m_storage);
  }

  /* Look up ADDR in the address map, and return either the
     corresponding CU, or nullptr if the address could not be
     found.  */
  dwarf2_per_cu_data *lookup (CORE_ADDR addr)
  {
    return (dwarf2_per_cu_data *) addrmap_find (m_addrmap, addr);
  }

  /* Finalize the index.  This should be called a single time, when
     the index has been fully populated.  It enters all the entries
     into the internal hash table.  */
  void finalize ();

  /* A simple range over part of m_entries.  */
  typedef iterator_range<std::vector<cooked_index_entry *>::iterator> range;

  /* Look up an entry by name.  Returns a range of all matching
     results.  If COMPLETING is true, then a larger range, suitable
     for completion, will be returned.  */
  range find (gdb::string_view name, bool completing);

  /* Return a range of all the entries.  */
  range all_entries ()
  {
    return { m_entries.begin (), m_entries.end () };
  }

private:

  /* GNAT only emits mangled ("encoded") names in the DWARF, and does
     not emit the module structure.  However, we need this structure
     to do lookups.  This function recreates that structure for an
     existing entry.  It returns the base name (last element) of the
     full decoded name.  */
  gdb::unique_xmalloc_ptr<char> handle_gnat_encoded_entry
       (cooked_index_entry *entry, htab_t gnat_entries);

  /* Create a new cooked_index_entry and register it with this object.
     Entries are owned by this object.  The new item is returned.  */
  cooked_index_entry *create (sect_offset die_offset,
			      enum dwarf_tag tag,
			      cooked_index_flag flags,
			      const char *name,
			      const cooked_index_entry *parent_entry,
			      dwarf2_per_cu_data *per_cu)
  {
    return new (&m_storage) cooked_index_entry (die_offset, tag, flags,
						name, parent_entry,
						per_cu);
  }

  /* Storage for the entries.  */
  auto_obstack m_storage;
  /* List of all entries.  */
  std::vector<cooked_index_entry *> m_entries;
  /* If we found "main" or an entry with 'is_main' set, store it
     here.  */
  cooked_index_entry *m_main = nullptr;
  /* Storage for canonical names.  */
  std::vector<gdb::unique_xmalloc_ptr<char>> m_names;
  /* The addrmap.  This maps address ranges to dwarf2_per_cu_data
     objects.  */
  addrmap *m_addrmap = nullptr;
};

#endif /* GDB_DWARF2_COOKED_INDEX_H */
