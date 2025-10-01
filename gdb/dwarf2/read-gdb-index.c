/* Reading code for .gdb_index

   Copyright (C) 2023-2025 Free Software Foundation, Inc.

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

#include "read-gdb-index.h"

#include "cli/cli-cmds.h"
#include "cli/cli-style.h"
#include "complaints.h"
#include "dwarf2/index-common.h"
#include "dwz.h"
#include "event-top.h"
#include "gdb/gdb-index.h"
#include "gdbsupport/gdb-checked-static-cast.h"
#include "cooked-index.h"
#include "read.h"
#include "extract-store-integer.h"
#include "cp-support.h"
#include "symtab.h"
#include "gdbsupport/selftest.h"
#include "tag.h"

/* When true, do not reject deprecated .gdb_index sections.  */
static bool use_deprecated_index_sections = false;

struct dwarf2_gdb_index : public cooked_index_functions
{
  /* This dumps minimal information about the index.
     It is called via "mt print objfiles".
     One use is to verify .gdb_index has been loaded by the
     gdb.dwarf2/gdb-index.exp testcase.  */
  void dump (struct objfile *objfile) override;
};

/* This is a cooked index as ingested from .gdb_index.  */

class cooked_gdb_index : public cooked_index
{
public:

  cooked_gdb_index (cooked_index_worker_up worker,
		    int version)
    : cooked_index (std::move (worker)),
      version (version)
  { }

  /* This can't be used to write an index.  */
  cooked_index *index_for_writing () override
  { return nullptr; }

  quick_symbol_functions_up make_quick_functions () const override
  { return quick_symbol_functions_up (new dwarf2_gdb_index); }

  bool version_check () const override
  {
    return version >= 8;
  }

  /* Index data format version.  */
  int version;
};

/* See above.  */

void
dwarf2_gdb_index::dump (struct objfile *objfile)
{
  dwarf2_per_objfile *per_objfile = get_dwarf2_per_objfile (objfile);

  cooked_gdb_index *index = (gdb::checked_static_cast<cooked_gdb_index *>
			     (per_objfile->per_bfd->index_table.get ()));
  gdb_printf (".gdb_index: version %d\n", index->version);
  cooked_index_functions::dump (objfile);
}

/* This is a view into the index that converts from bytes to an
   offset_type, and allows indexing.  Unaligned bytes are specifically
   allowed here, and handled via unpacking.  */

class offset_view
{
public:
  offset_view () = default;

  explicit offset_view (gdb::array_view<const gdb_byte> bytes)
    : m_bytes (bytes)
  {
  }

  /* Extract the INDEXth offset_type from the array.  */
  offset_type operator[] (size_t index) const
  {
    const gdb_byte *bytes = &m_bytes[index * sizeof (offset_type)];
    return (offset_type) extract_unsigned_integer (bytes,
						   sizeof (offset_type),
						   BFD_ENDIAN_LITTLE);
  }

  /* Return the number of offset_types in this array.  */
  size_t size () const
  {
    return m_bytes.size () / sizeof (offset_type);
  }

  /* Return true if this view is empty.  */
  bool empty () const
  {
    return m_bytes.empty ();
  }

private:
  /* The underlying bytes.  */
  gdb::array_view<const gdb_byte> m_bytes;
};

/* A worker for reading .gdb_index.  The file format is described in
   the manual.  */

struct mapped_gdb_index
{
  /* Index data format version.  */
  int version = 0;

  /* Compile units followed by type units, in the order as found in the
     index.  Indices found in index entries can index directly into this.  */
  std::vector<dwarf2_per_cu *> units;

  /* The address table data.  */
  gdb::array_view<const gdb_byte> address_table;

  /* The symbol table, implemented as a hash table.  */
  offset_view symbol_table;

  /* A pointer to the constant pool.  */
  gdb::array_view<const gdb_byte> constant_pool;

  /* The shortcut table data.  */
  gdb::array_view<const gdb_byte> shortcut_table;

  /* An address map that maps from PC to dwarf2_per_cu.  */
  addrmap_fixed *index_addrmap = nullptr;

  /* The name of 'main', or nullptr if not known.  */
  const char *main_name = nullptr;

  /* The language of 'main', if known.  */
  enum language main_lang = language_minimal;

  /* The result we're constructing.  */
  cooked_index_worker_result result;

  /* Return the index into the constant pool of the name of the IDXth
     symbol in the symbol table.  */
  offset_type symbol_name_index (offset_type idx) const
  {
    return symbol_table[2 * idx];
  }

  /* Return the index into the constant pool of the CU vector of the
     IDXth symbol in the symbol table.  */
  offset_type symbol_vec_index (offset_type idx) const
  {
    return symbol_table[2 * idx + 1];
  }

  /* Return whether the name at IDX in the symbol table should be
     ignored.  */
  bool symbol_name_slot_invalid (offset_type idx) const
  {
    return symbol_name_index (idx) == 0 && symbol_vec_index (idx) == 0;
  }

  /* Convenience method to get at the name of the symbol at IDX in the
     symbol table.  */
  const char *symbol_name_at (offset_type idx,
			      dwarf2_per_objfile *per_objfile) const
  {
    return (const char *) (this->constant_pool.data ()
			   + symbol_name_index (idx));
  }

  size_t symbol_name_count () const
  { return this->symbol_table.size () / 2; }

  /* Set the name and language of the main function from the shortcut
     table.  */
  void set_main_name (dwarf2_per_objfile *per_objfile);

  /* Build the symbol name component sorted vector, if we haven't
     yet.  */
  void build_name_components (dwarf2_per_objfile *per_objfile);
};

/* See declaration.  */

void
mapped_gdb_index::build_name_components (dwarf2_per_objfile *per_objfile)
{
  std::vector<std::pair<std::string_view, std::vector<cooked_index_entry *>>>
    need_parents;
  gdb::unordered_map<std::string_view, cooked_index_entry *> by_name;

  auto count = this->symbol_name_count ();
  for (offset_type idx = 0; idx < count; idx++)
    {
      if (this->symbol_name_slot_invalid (idx))
	continue;

      const char *name = this->symbol_name_at (idx, per_objfile);

      /* This code only knows how to break apart components of C++
	 symbol names (and other languages that use '::' as
	 namespace/module separator) and Ada symbol names.

	 It's unfortunate that we need the language, but since it is
	 really only used to rebuild full names, pairing it with the
	 split method is fine.  */
      enum language lang;
      std::vector<std::string_view> components;
      if (strstr (name, "::") != nullptr)
	{
	  components = split_name (name, split_style::CXX);
	  lang = language_cplus;
	}
      else if (strchr (name, '<') != nullptr)
	{
	  /* Guess that this is a template and so a C++ name.  */
	  components.emplace_back (name);
	  lang = language_cplus;
	}
      else if (strstr (name, "__") != nullptr)
	{
	  /* The Ada case is handled during finalization, because gdb
	     does not write the synthesized package names into the
	     index.  */
	  components.emplace_back (name);
	  lang = language_ada;
	}
      else
	{
	  components = split_name (name, split_style::DOT_STYLE);
	  /* Mark ordinary names as having an unknown language.  This
	     is a hack to avoid problems with some Ada names.  */
	  lang = (components.size () == 1) ? language_unknown : language_go;
	}

      std::vector<cooked_index_entry *> these_entries;
      offset_view vec (constant_pool.slice (symbol_vec_index (idx)));
      offset_type vec_len = vec[0];
      for (offset_type vec_idx = 0; vec_idx < vec_len; ++vec_idx)
	{
	  offset_type cu_index_and_attrs = vec[vec_idx + 1];
	  gdb_index_symbol_kind symbol_kind
	    = GDB_INDEX_SYMBOL_KIND_VALUE (cu_index_and_attrs);
	  /* Only use a symbol if the attributes are present.  Indices
	     prior to version 7 don't record them, and indices >= 7
	     may elide them for certain symbols (gold does this).  */
	  if (symbol_kind == GDB_INDEX_SYMBOL_KIND_NONE)
	    continue;

	  int is_static = GDB_INDEX_SYMBOL_STATIC_VALUE (cu_index_and_attrs);

	  int cu_index = GDB_INDEX_CU_VALUE (cu_index_and_attrs);
	  /* Don't crash on bad data.  */
	  if (cu_index >= units.size ())
	    {
	      complaint (_(".gdb_index entry has bad CU index"
			   " [in module %s]"),
			 objfile_name (per_objfile->objfile));
	      continue;
	    }
	  dwarf2_per_cu *per_cu = units[cu_index];

	  enum language this_lang = lang;
	  dwarf_tag tag;
	  switch (symbol_kind)
	    {
	    case GDB_INDEX_SYMBOL_KIND_VARIABLE:
	      tag = DW_TAG_variable;
	      break;
	    case GDB_INDEX_SYMBOL_KIND_FUNCTION:
	      tag = DW_TAG_subprogram;
	      break;
	    case GDB_INDEX_SYMBOL_KIND_TYPE:
	      if (is_static)
		tag = (dwarf_tag) DW_TAG_GDB_INDEX_TYPE;
	      else
		{
		  tag = DW_TAG_structure_type;
		  this_lang = language_cplus;
		}
	      break;
	      /* The "default" should not happen, but we mention it to
		 avoid an uninitialized warning.  */
	    default:
	    case GDB_INDEX_SYMBOL_KIND_OTHER:
	      tag = (dwarf_tag) DW_TAG_GDB_INDEX_OTHER;
	      break;
	    }

	  cooked_index_flag flags = 0;
	  if (is_static)
	    flags |= IS_STATIC;
	  if (main_name != nullptr
	      && tag == DW_TAG_subprogram
	      && strcmp (name, main_name) == 0)
	    {
	      flags |= IS_MAIN;
	      this_lang = main_lang;
	      /* Don't bother looking for another.  */
	      main_name = nullptr;
	    }

	  /* Note that this assumes the final component ends in \0.  */
	  cooked_index_entry *entry = result.add (per_cu->sect_off (), tag,
						  flags, this_lang,
						  components.back ().data (),
						  nullptr, per_cu);
	  /* Don't bother pushing if we do not need a parent.  */
	  if (components.size () > 1)
	    these_entries.push_back (entry);

	  /* We don't care exactly which entry ends up in this
	     map.  */
	  by_name[std::string_view (name)] = entry;
	}

      if (components.size () > 1)
	{
	  std::string_view penultimate = components[components.size () - 2];
	  std::string_view prefix (name, &penultimate.back () + 1 - name);

	  need_parents.emplace_back (prefix, std::move (these_entries));
	}
    }

  for (const auto &[prefix, entries] : need_parents)
    {
      auto iter = by_name.find (prefix);
      /* If we can't find the parent entry, just lose.  It should
	 always be there.  We could synthesize it from the components,
	 if we kept those, but that seems like overkill.  */
      if (iter != by_name.end ())
	{
	  for (cooked_index_entry *entry : entries)
	    entry->set_parent (iter->second);
	}
    }
}

/* The worker that reads a mapped index and fills in a
   cooked_index_worker_result.  */

class gdb_index_worker : public cooked_index_worker
{
public:

  gdb_index_worker (dwarf2_per_objfile *per_objfile,
		    std::unique_ptr<mapped_gdb_index> map)
    : cooked_index_worker (per_objfile),
      map (std::move (map))
  { }

  void do_reading () override;

  /* The map we're reading.  */
  std::unique_ptr<mapped_gdb_index> map;
};

void
gdb_index_worker::do_reading ()
{
  complaint_interceptor complaint_handler;
  map->build_name_components (m_per_objfile);

  m_results.push_back (std::move (map->result));
  m_results[0].done_reading (complaint_handler.release ());

  /* No longer needed.  */
  map.reset ();

  done_reading ();

  bfd_thread_cleanup ();
}

/* A helper function that reads the .gdb_index from BUFFER and fills
   in MAP.  FILENAME is the name of the file containing the data;
   it is used for error reporting.  DEPRECATED_OK is true if it is
   ok to use deprecated sections.

   CU_LIST, CU_LIST_ELEMENTS, TYPES_LIST, and TYPES_LIST_ELEMENTS are
   out parameters that are filled in with information about the CU and
   TU lists in the section.

   Returns true if all went well, false otherwise.  */

static bool
read_gdb_index_from_buffer (const char *filename,
			    bool deprecated_ok,
			    gdb::array_view<const gdb_byte> buffer,
			    mapped_gdb_index *map,
			    const gdb_byte **cu_list,
			    offset_type *cu_list_elements,
			    const gdb_byte **types_list,
			    offset_type *types_list_elements)
{
  const gdb_byte *addr = &buffer[0];
  offset_view metadata (buffer);

  /* Version check.  */
  offset_type version = metadata[0];
  /* GDB now requires the symbol attributes, which were added in
     version 7.  */
  if (version < 7)
    {
      static int warning_printed = 0;
      if (!warning_printed)
	{
	  warning (_("Skipping obsolete .gdb_index section in %s."),
		   filename);
	  warning_printed = 1;
	}
      return 0;
    }
  /* Version 7 indices generated by gold refer to the CU for a symbol instead
     of the TU (for symbols coming from TUs),
     http://sourceware.org/bugzilla/show_bug.cgi?id=15021.
     Plus gold-generated indices can have duplicate entries for global symbols,
     http://sourceware.org/bugzilla/show_bug.cgi?id=15646.
     These are just performance bugs, and we can't distinguish gdb-generated
     indices from gold-generated ones, so issue no warning here.  */

  /* Indexes with higher version than the one supported by GDB may be no
     longer backward compatible.  */
  if (version > 9)
    return 0;

  map->version = version;

  int i = 1;
  *cu_list = addr + metadata[i];
  *cu_list_elements = (metadata[i + 1] - metadata[i]) / 8;
  ++i;

  *types_list = addr + metadata[i];
  *types_list_elements = (metadata[i + 1] - metadata[i]) / 8;
  ++i;

  const gdb_byte *address_table = addr + metadata[i];
  const gdb_byte *address_table_end = addr + metadata[i + 1];
  map->address_table
    = gdb::array_view<const gdb_byte> (address_table, address_table_end);
  ++i;

  const gdb_byte *symbol_table = addr + metadata[i];
  const gdb_byte *symbol_table_end = addr + metadata[i + 1];
  map->symbol_table
    = offset_view (gdb::array_view<const gdb_byte> (symbol_table,
						    symbol_table_end));

  ++i;

  if (version >= 9)
    {
      const gdb_byte *shortcut_table = addr + metadata[i];
      const gdb_byte *shortcut_table_end = addr + metadata[i + 1];
      map->shortcut_table
	= gdb::array_view<const gdb_byte> (shortcut_table, shortcut_table_end);
      ++i;
    }

  map->constant_pool = buffer.slice (metadata[i]);

  if (map->constant_pool.empty () && !map->symbol_table.empty ())
    {
      /* An empty constant pool implies that all symbol table entries are
	 empty.  Make map->symbol_table.empty () == true.  */
      map->symbol_table
	= offset_view (gdb::array_view<const gdb_byte> (symbol_table,
							symbol_table));
    }

  return 1;
}

/* A helper for create_cus_from_gdb_index that handles a given list of
   CUs.  */

static void
create_cus_from_gdb_index_list (dwarf2_per_bfd *per_bfd,
				const gdb_byte *cu_list, offset_type n_elements,
				struct dwarf2_section_info *section,
				int is_dwz, std::vector<dwarf2_per_cu *> &units)
{
  for (offset_type i = 0; i < n_elements; i += 2)
    {
      static_assert (sizeof (ULONGEST) >= 8);

      sect_offset sect_off
	= (sect_offset) extract_unsigned_integer (cu_list, 8, BFD_ENDIAN_LITTLE);
      ULONGEST length = extract_unsigned_integer (cu_list + 8, 8, BFD_ENDIAN_LITTLE);
      cu_list += 2 * 8;

      dwarf2_per_cu_up per_cu = per_bfd->allocate_per_cu (section, sect_off,
							  length, is_dwz);
      units.emplace_back (per_cu.get ());
      per_bfd->all_units.emplace_back (std::move (per_cu));
    }
}

/* Read the CU list from the mapped index, and use it to create all
   the CU objects for PER_BFD.  */

static void
create_cus_from_gdb_index (dwarf2_per_bfd *per_bfd,
			   const gdb_byte *cu_list, offset_type cu_list_elements,
			   std::vector<dwarf2_per_cu *> &units,
			   const gdb_byte *dwz_list, offset_type dwz_elements)
{
  gdb_assert (per_bfd->all_units.empty ());
  per_bfd->all_units.reserve ((cu_list_elements + dwz_elements) / 2);

  create_cus_from_gdb_index_list (per_bfd, cu_list, cu_list_elements,
				  &per_bfd->infos[0], 0, units);

  if (dwz_elements == 0)
    return;

  dwz_file *dwz = per_bfd->get_dwz_file ();
  create_cus_from_gdb_index_list (per_bfd, dwz_list, dwz_elements,
				  &dwz->info, 1, units);
}

/* Create the signatured type hash table from the index.  */

static void
create_signatured_type_table_from_gdb_index
  (dwarf2_per_bfd *per_bfd, struct dwarf2_section_info *section,
   const gdb_byte *bytes, offset_type elements,
   std::vector<dwarf2_per_cu *> &units)
{
  signatured_type_set sig_types_hash;

  for (offset_type i = 0; i < elements; i += 3)
    {
      static_assert (sizeof (ULONGEST) >= 8);
      sect_offset sect_off
	= (sect_offset) extract_unsigned_integer (bytes, 8, BFD_ENDIAN_LITTLE);
	cu_offset type_offset_in_tu
	= (cu_offset) extract_unsigned_integer (bytes + 8, 8,
						BFD_ENDIAN_LITTLE);
      ULONGEST signature
	= extract_unsigned_integer (bytes + 16, 8, BFD_ENDIAN_LITTLE);
      bytes += 3 * 8;

      /* The length of the type unit is unknown at this time.  It gets
	 (presumably) set by a cutu_reader when it gets expanded later.  */
      signatured_type_up sig_type
	= per_bfd->allocate_signatured_type (section, sect_off, 0 /* length */,
					     false /* is_dwz */, signature);
      sig_type->type_offset_in_tu = type_offset_in_tu;

      sig_types_hash.emplace (sig_type.get ());
      units.emplace_back (sig_type.get ());
      per_bfd->all_units.emplace_back (sig_type.release ());
    }

  per_bfd->signatured_types = std::move (sig_types_hash);
}

/* Read the address map data from the mapped GDB index.  Return true if no
   errors were found, otherwise return false.  */

static bool
create_addrmap_from_gdb_index (dwarf2_per_objfile *per_objfile,
			       mapped_gdb_index *index)
{
  objfile *objfile = per_objfile->objfile;

  addrmap_mutable mutable_map;

  /* Build an unrelocated address map of the sections in this objfile.  */
  addrmap_mutable sect_map;
  for (obj_section &s : objfile->sections ())
    {
      if (s.addr_unrel () >= s.endaddr_unrel ())
	continue;

      CORE_ADDR start = CORE_ADDR (s.addr_unrel ());
      CORE_ADDR end_inclusive = CORE_ADDR (s.endaddr_unrel ()) - 1;
      sect_map.set_empty (start, end_inclusive, &s);
    }

  auto find_section
    = [&] (ULONGEST addr, struct obj_section *&cached_section)
    {
      if (cached_section != nullptr
	  && cached_section->contains (unrelocated_addr (addr)))
	return cached_section;

      cached_section = (struct obj_section *) sect_map.find (addr);
      return cached_section;
    };

  auto invalid_range_warning = [&] (ULONGEST lo, ULONGEST hi)
    {
      warning (_(".gdb_index address table has invalid range (%s - %s),"
		 " ignoring .gdb_index"),
	       hex_string (lo), hex_string (hi));
      return false;
    };

  /* Cache the section for possible re-use on the next entry.  */
  struct obj_section *prev_sect = nullptr;

  const gdb_byte *iter = index->address_table.data ();
  const gdb_byte *end = iter + index->address_table.size ();
  while (iter < end)
    {
      ULONGEST hi, lo, cu_index;
      lo = extract_unsigned_integer (iter, 8, BFD_ENDIAN_LITTLE);
      iter += 8;
      hi = extract_unsigned_integer (iter, 8, BFD_ENDIAN_LITTLE);
      iter += 8;
      cu_index = extract_unsigned_integer (iter, 4, BFD_ENDIAN_LITTLE);
      iter += 4;

      if (lo >= hi)
	return invalid_range_warning (lo, hi);

      if (cu_index >= index->units.size ())
	{
	  warning (_(".gdb_index address table has invalid CU number %u,"
		     " ignoring .gdb_index"),
		   (unsigned) cu_index);
	  return false;
	}

      /* Variable hi is the exclusive upper bound, get the inclusive one.  */
      CORE_ADDR hi_incl = hi - 1;

      struct obj_section *lo_sect = find_section (lo, prev_sect);
      struct obj_section *hi_sect = find_section (hi_incl, prev_sect);
      if (lo_sect == nullptr || hi_sect == nullptr)
	return invalid_range_warning (lo, hi);

      bool full_range_p
	= mutable_map.set_empty (lo, hi_incl, index->units[cu_index]);
      if (!full_range_p)
	{
	  warning (_(".gdb_index address table has a range (%s - %s) that"
		     " overlaps with an earlier range, ignoring .gdb_index"),
		     hex_string (lo), hex_string (hi));
	  return false;
	}
    }

  index->result.set_addrmap (std::move (mutable_map));

  return true;
}

void
mapped_gdb_index::set_main_name (dwarf2_per_objfile *per_objfile)
{
  const auto expected_size = 2 * sizeof (offset_type);
  if (this->shortcut_table.size () < expected_size)
    /* The data in the section is not present, is corrupted or is in a version
       we don't know about.  Regardless, we can't make use of it.  */
    return;

  auto ptr = this->shortcut_table.data ();
  const auto dw_lang = extract_unsigned_integer (ptr, 4, BFD_ENDIAN_LITTLE);
  if (dw_lang >= DW_LANG_hi_user)
    {
      complaint (_(".gdb_index shortcut table has invalid main language %u"),
		   (unsigned) dw_lang);
      return;
    }
  if (dw_lang == 0)
    {
      /* Don't bother if the language for the main symbol was not known or if
	 there was no main symbol at all when the index was built.  */
      return;
    }
  ptr += 4;

  main_lang = dwarf_lang_to_enum_language (dw_lang);
  const auto name_offset = extract_unsigned_integer (ptr,
						     sizeof (offset_type),
						     BFD_ENDIAN_LITTLE);
  main_name = (const char *) (this->constant_pool.data () + name_offset);
}

/* See read-gdb-index.h.  */

bool
dwarf2_read_gdb_index
  (dwarf2_per_objfile *per_objfile,
   get_gdb_index_contents_ftype get_gdb_index_contents,
   get_gdb_index_contents_dwz_ftype get_gdb_index_contents_dwz)
{
  const gdb_byte *cu_list, *types_list, *dwz_list = NULL;
  offset_type cu_list_elements, types_list_elements, dwz_list_elements = 0;
  struct objfile *objfile = per_objfile->objfile;
  dwarf2_per_bfd *per_bfd = per_objfile->per_bfd;
  scoped_remove_all_units remove_all_units (*per_bfd);

  gdb::array_view<const gdb_byte> main_index_contents
    = get_gdb_index_contents (objfile, per_bfd);

  if (main_index_contents.empty ())
    return false;

  auto map = std::make_unique<mapped_gdb_index> ();
  if (!read_gdb_index_from_buffer (objfile_name (objfile),
				   use_deprecated_index_sections,
				   main_index_contents, map.get (), &cu_list,
				   &cu_list_elements, &types_list,
				   &types_list_elements))
    return false;

  /* Don't use the index if it's empty.  */
  if (map->symbol_table.empty ())
    return false;

  /* If there is a .dwz file, read it so we can get its CU list as
     well.  */
  dwz_file *dwz = per_bfd->get_dwz_file ();
  if (dwz != NULL)
    {
      mapped_gdb_index dwz_map;
      const gdb_byte *dwz_types_ignore;
      offset_type dwz_types_elements_ignore;

      gdb::array_view<const gdb_byte> dwz_index_content
	= get_gdb_index_contents_dwz (objfile, dwz);

      if (dwz_index_content.empty ())
	return false;

      if (!read_gdb_index_from_buffer (dwz->filename (),
				       1, dwz_index_content, &dwz_map,
				       &dwz_list, &dwz_list_elements,
				       &dwz_types_ignore,
				       &dwz_types_elements_ignore))
	{
	  warning (_("could not read '.gdb_index' section from %s; skipping"),
		   dwz->filename ());
	  return false;
	}
    }

  create_cus_from_gdb_index (per_bfd, cu_list, cu_list_elements, map->units,
			     dwz_list, dwz_list_elements);

  if (types_list_elements)
    {
      /* We can only handle a single .debug_info and .debug_types when we have
	 an index.  */
      if (per_bfd->infos.size () > 1
	  || per_bfd->types.size () > 1)
	return false;

      dwarf2_section_info *section
	= (per_bfd->types.size () == 1
	   ? &per_bfd->types[0]
	   : &per_bfd->infos[0]);

      create_signatured_type_table_from_gdb_index (per_bfd, section, types_list,
						   types_list_elements,
						   map->units);
    }

  finalize_all_units (per_bfd);

  if (!create_addrmap_from_gdb_index (per_objfile, map.get ()))
    return false;

  map->set_main_name (per_objfile);

  int version = map->version;
  auto worker = std::make_unique<gdb_index_worker> (per_objfile,
						    std::move (map));
  auto idx = std::make_unique<cooked_gdb_index> (std::move (worker),
						 version);

  per_bfd->start_reading (std::move (idx));
  remove_all_units.disable ();

  return true;
}

INIT_GDB_FILE (read_gdb_index)
{
  add_setshow_boolean_cmd ("use-deprecated-index-sections",
			   no_class, &use_deprecated_index_sections, _("\
Set whether to use deprecated gdb_index sections."), _("\
Show whether to use deprecated gdb_index sections."), _("\
When enabled, deprecated .gdb_index sections are used anyway.\n\
Normally they are ignored either because of a missing feature or\n\
performance issue.\n\
Warning: This option must be enabled before gdb reads the file."),
			   NULL,
			   NULL,
			   &setlist, &showlist);
}
