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

#include "defs.h"
#include "dwarf2/cooked-index.h"
#include "dwarf2/read.h"
#include "cp-support.h"
#include "ada-lang.h"
#include "split-name.h"
#include <algorithm>

/* Hash function for cooked_index_entry.  */

static hashval_t
hash_entry (const void *e)
{
  const cooked_index_entry *entry = (const cooked_index_entry *) e;
  return dwarf5_djb_hash (entry->canonical);
}

/* Equality function for cooked_index_entry.  */

static int
eq_entry (const void *a, const void *b)
{
  const cooked_index_entry *ae = (const cooked_index_entry *) a;
  const gdb::string_view *sv = (const gdb::string_view *) b;
  return (strlen (ae->canonical) == sv->length ()
	  && strncasecmp (ae->canonical, sv->data (), sv->length ()) == 0);
}

/* See cooked-index.h.  */

const char *
cooked_index_entry::full_name (struct obstack *storage) const
{
  if ((flags & IS_LINKAGE) != 0)
    return canonical;

  const char *sep = nullptr;
  switch (per_cu->lang)
    {
    case language_cplus:
    case language_rust:
      sep = "::";
      break;

    case language_go:
    case language_d:
    case language_ada:
      sep = ".";
      break;
    }

  if (sep == nullptr)
    return canonical;

  if (parent_entry != nullptr)
    parent_entry->write_scope (storage, sep);
  obstack_grow0 (storage, canonical, strlen (canonical));
  return (const char *) obstack_finish (storage);
}

/* See cooked-index.h.  */

void
cooked_index_entry::write_scope (struct obstack *storage,
				 const char *sep) const
{
  if (parent_entry != nullptr)
    parent_entry->write_scope (storage, sep);
  obstack_grow (storage, canonical, strlen (canonical));
  obstack_grow (storage, sep, strlen (sep));
}

/* See cooked-index.h.  */

const cooked_index_entry *
cooked_index::add (sect_offset die_offset, enum dwarf_tag tag,
		   cooked_index_flag flags, const char *name,
		   const cooked_index_entry *parent_entry,
		   dwarf2_per_cu_data *per_cu)
{
  cooked_index_entry *result = create (die_offset, tag, flags, name,
				       parent_entry, per_cu);
  m_entries.push_back (result);

  /* An explicitly-tagged main program should always override the
     implicit "main" discovery.  */
  if ((flags & IS_MAIN) != 0)
    m_main = result;
  else if (per_cu->lang != language_ada
	   && m_main == nullptr
	   && strcmp (name, "main") == 0)
    m_main = result;

  return result;
}

cooked_index_vector::cooked_index_vector (vec_type &&vec)
  : m_vector (std::move (vec))
{
  finalize ();
}

/* See cooked-index.h.  */

dwarf2_per_cu_data *
cooked_index_vector::lookup (CORE_ADDR addr)
{
  for (const auto &index : m_vector)
    {
      dwarf2_per_cu_data *result = index->lookup (addr);
      if (result != nullptr)
	return result;
    }
  return nullptr;
}

/* See cooked-index.h.  */

std::vector<addrmap *>
cooked_index_vector::get_addrmaps ()
{
  std::vector<addrmap *> result;
  for (const auto &index : m_vector)
    result.push_back (index->m_addrmap);
  return result;
}

/* See cooked-index.h.  */

cooked_index_vector::range
cooked_index_vector::find (gdb::string_view name, bool completing)
{
  auto lower = std::lower_bound (m_entries.begin (), m_entries.end (),
				 name,
				 [=] (const cooked_index_entry *entry,
				      const gdb::string_view &n)
  {
    int cmp = strncasecmp (entry->canonical, n.data (), n.length ());
    if (cmp != 0 || completing)
      return cmp < 0;
    return strlen (entry->canonical) < n.length ();
  });

  auto upper = std::upper_bound (m_entries.begin (), m_entries.end (),
				 name,
				 [=] (const gdb::string_view &n,
				      const cooked_index_entry *entry)
  {
    int cmp = strncasecmp (n.data (), entry->canonical, n.length ());
    if (cmp != 0 || completing)
      return cmp < 0;
    return n.length () < strlen (entry->canonical);
  });

  return range (lower, upper);
}

/* See cooked-index.h.  */

gdb::unique_xmalloc_ptr<char>
cooked_index_vector::handle_gnat_encoded_entry (cooked_index_entry *entry,
						htab_t gnat_entries)
{
  std::string canonical = ada_decode (entry->name, false, false);
  if (canonical.empty ())
    return {};
  std::vector<gdb::string_view> names = split_name (canonical.c_str (),
						    split_style::DOT);
  gdb::string_view tail = names.back ();
  names.pop_back ();

  const cooked_index_entry *parent = nullptr;
  for (const auto &name : names)
    {
      uint32_t hashval = dwarf5_djb_hash (name);
      void **slot = htab_find_slot_with_hash (gnat_entries, &name,
					      hashval, INSERT);
      /* CUs are processed in order, so we only need to check the most
	 recent entry.  */
      cooked_index_entry *last = (cooked_index_entry *) *slot;
      if (last == nullptr || last->per_cu != entry->per_cu)
	{
	  gdb::unique_xmalloc_ptr<char> new_name
	    = make_unique_xstrndup (name.data (), name.length ());
	  /* It doesn't matter which obstack we allocate this on, so
	     we pick the most convenient one.  */
	  last = m_vector[0]->create (entry->die_offset, DW_TAG_namespace,
				      0, new_name.get (), parent,
				      entry->per_cu);
	  last->canonical = last->name;
	  m_names.push_back (std::move (new_name));
	  *slot = last;
	}

      parent = last;
    }

  entry->parent_entry = parent;
  return make_unique_xstrndup (tail.data (), tail.length ());
}

/* See cooked-index.h.  */

const cooked_index_entry *
cooked_index_vector::get_main () const
{
  const cooked_index_entry *result = nullptr;

  for (const auto &index : m_vector)
    {
      const cooked_index_entry *entry = index->get_main ();
      if (result == nullptr
	  || ((result->flags & IS_MAIN) == 0
	      && entry != nullptr
	      && (entry->flags & IS_MAIN) != 0))
	result = entry;
    }

  return result;
}

/* See cooked-index.h.  */

void
cooked_index_vector::finalize ()
{
  auto hash_name_ptr = [] (const void *p)
    {
      const cooked_index_entry *entry = (const cooked_index_entry *) p;
      return htab_hash_pointer (entry->name);
    };

  auto eq_name_ptr = [] (const void *a, const void *b) -> int
    {
      const cooked_index_entry *ea = (const cooked_index_entry *) a;
      const cooked_index_entry *eb = (const cooked_index_entry *) b;
      return ea->name == eb->name;
    };

  /* We can use pointer equality here because names come from
     .debug_str, which will normally be unique-ified by the linker.
     Also, duplicates are relatively harmless -- they just mean a bit
     of extra memory is used.  */
  htab_up seen_names (htab_create_alloc (10, hash_name_ptr, eq_name_ptr,
					 nullptr, xcalloc, xfree));

  for (auto &index : m_vector)
    {
      htab_up gnat_entries (htab_create_alloc (10, hash_entry, eq_entry,
					       nullptr, xcalloc, xfree));

      std::vector<cooked_index_entry *> entries
	= std::move (index->m_entries);
      for (cooked_index_entry *entry : entries)
	{
	  gdb_assert (entry->canonical == nullptr);
	  if ((entry->per_cu->lang != language_cplus
	       && entry->per_cu->lang != language_ada)
	      || (entry->flags & IS_LINKAGE) != 0)
	    entry->canonical = entry->name;
	  else
	    {
	      if (entry->per_cu->lang == language_ada)
		{
		  gdb::unique_xmalloc_ptr<char> canon_name
		    = handle_gnat_encoded_entry (entry, gnat_entries.get ());
		  if (canon_name == nullptr)
		    entry->canonical = entry->name;
		  else
		    {
		      entry->canonical = canon_name.get ();
		      m_names.push_back (std::move (canon_name));
		    }
		}
	      else
		{
		  void **slot = htab_find_slot (seen_names.get (), entry,
						INSERT);
		  if (*slot == nullptr)
		    {
		      gdb::unique_xmalloc_ptr<char> canon_name
			= cp_canonicalize_string (entry->name);
		      if (canon_name == nullptr)
			entry->canonical = entry->name;
		      else
			{
			  entry->canonical = canon_name.get ();
			  m_names.push_back (std::move (canon_name));
			}
		    }
		  else
		    {
		      const cooked_index_entry *other
			= (const cooked_index_entry *) *slot;
		      entry->canonical = other->canonical;
		    }
		}
	    }
	}

      if (m_entries.empty ())
	m_entries = std::move (entries);
      else
	{
	  size_t old_size = m_entries.size ();
	  m_entries.resize (m_entries.size () + entries.size ());
	  memcpy (m_entries.data () + old_size,
		  entries.data (), entries.size () * sizeof (entries[0]));
	}
    }

  m_names.shrink_to_fit ();
  m_entries.shrink_to_fit ();
  std::sort (m_entries.begin (), m_entries.end (),
	     [] (const cooked_index_entry *a, const cooked_index_entry *b)
	     {
	       return *a < *b;
	     });
}
