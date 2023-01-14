/* DWARF 2 debugging format support for GDB.

   Copyright (C) 1994-2020 Free Software Foundation, Inc.

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

#ifndef DWARF2READ_H
#define DWARF2READ_H

#include <queue>
#include <unordered_map>
#include "dwarf2/index-cache.h"
#include "dwarf2/section.h"
#include "filename-seen-cache.h"
#include "gdb_obstack.h"
#include "gdbsupport/hash_enum.h"
#include "psympriv.h"

/* Hold 'maintenance (set|show) dwarf' commands.  */
extern struct cmd_list_element *set_dwarf_cmdlist;
extern struct cmd_list_element *show_dwarf_cmdlist;

struct tu_stats
{
  int nr_uniq_abbrev_tables;
  int nr_symtabs;
  int nr_symtab_sharers;
  int nr_stmt_less_type_units;
  int nr_all_type_units_reallocs;
};

struct dwarf2_debug_sections;
struct dwarf2_per_cu_data;
struct mapped_index;
struct mapped_debug_names;
struct signatured_type;

/* One item on the queue of compilation units to read in full symbols
   for.  */
struct dwarf2_queue_item
{
  dwarf2_queue_item (dwarf2_per_cu_data *cu, enum language lang)
    : per_cu (cu),
      pretend_language (lang)
  {
  }

  ~dwarf2_queue_item ();

  DISABLE_COPY_AND_ASSIGN (dwarf2_queue_item);

  struct dwarf2_per_cu_data *per_cu;
  enum language pretend_language;
};

/* Collection of data recorded per objfile.
   This hangs off of dwarf2_objfile_data_key.  */

struct dwarf2_per_objfile
{
  /* Construct a dwarf2_per_objfile for OBJFILE.  NAMES points to the
     dwarf2 section names, or is NULL if the standard ELF names are
     used.  CAN_COPY is true for formats where symbol
     interposition is possible and so symbol values must follow copy
     relocation rules.  */
  dwarf2_per_objfile (struct objfile *objfile,
		      const dwarf2_debug_sections *names,
		      bool can_copy);

  ~dwarf2_per_objfile ();

  DISABLE_COPY_AND_ASSIGN (dwarf2_per_objfile);

  /* Return the CU/TU given its index.

     This is intended for loops like:

     for (i = 0; i < (dwarf2_per_objfile->n_comp_units
		      + dwarf2_per_objfile->n_type_units); ++i)
       {
         dwarf2_per_cu_data *per_cu = dwarf2_per_objfile->get_cutu (i);

         ...;
       }
  */
  dwarf2_per_cu_data *get_cutu (int index);

  /* Return the CU given its index.
     This differs from get_cutu in that it's for when you know INDEX refers to a
     CU.  */
  dwarf2_per_cu_data *get_cu (int index);

  /* Return the TU given its index.
     This differs from get_cutu in that it's for when you know INDEX refers to a
     TU.  */
  signatured_type *get_tu (int index);

  /* Free all cached compilation units.  */
  void free_cached_comp_units ();

  /* Return pointer to string at .debug_line_str offset as read from BUF.
     BUF is assumed to be in a compilation unit described by CU_HEADER.
     Return *BYTES_READ_PTR count of bytes read from BUF.  */
  const char *read_line_string (const gdb_byte *buf,
				const struct comp_unit_head *cu_header,
				unsigned int *bytes_read_ptr);

private:
  /* This function is mapped across the sections and remembers the
     offset and size of each of the debugging sections we are
     interested in.  */
  void locate_sections (bfd *abfd, asection *sectp,
			const dwarf2_debug_sections &names);

public:
  dwarf2_section_info info {};
  dwarf2_section_info abbrev {};
  dwarf2_section_info line {};
  dwarf2_section_info loc {};
  dwarf2_section_info loclists {};
  dwarf2_section_info macinfo {};
  dwarf2_section_info macro {};
  dwarf2_section_info str {};
  dwarf2_section_info str_offsets {};
  dwarf2_section_info line_str {};
  dwarf2_section_info ranges {};
  dwarf2_section_info rnglists {};
  dwarf2_section_info addr {};
  dwarf2_section_info frame {};
  dwarf2_section_info eh_frame {};
  dwarf2_section_info gdb_index {};
  dwarf2_section_info debug_names {};
  dwarf2_section_info debug_aranges {};

  std::vector<dwarf2_section_info> types;

  /* Back link.  */
  struct objfile *objfile = NULL;

  /* Table of all the compilation units.  This is used to locate
     the target compilation unit of a particular reference.  */
  std::vector<dwarf2_per_cu_data *> all_comp_units;

  /* The .debug_types-related CUs (TUs).  */
  std::vector<signatured_type *> all_type_units;

  /* Table of struct type_unit_group objects.
     The hash key is the DW_AT_stmt_list value.  */
  htab_up type_unit_groups;

  /* A table mapping .debug_types signatures to its signatured_type entry.
     This is NULL if the .debug_types section hasn't been read in yet.  */
  htab_up signatured_types;

  /* Type unit statistics, to see how well the scaling improvements
     are doing.  */
  struct tu_stats tu_stats {};

  /* A chain of compilation units that are currently read in, so that
     they can be freed later.  */
  dwarf2_per_cu_data *read_in_chain = NULL;

  /* A table mapping DW_AT_dwo_name values to struct dwo_file objects.
     This is NULL if the table hasn't been allocated yet.  */
  htab_up dwo_files;

  /* True if we've checked for whether there is a DWP file.  */
  bool dwp_checked = false;

  /* The DWP file if there is one, or NULL.  */
  std::unique_ptr<struct dwp_file> dwp_file;

  /* The shared '.dwz' file, if one exists.  This is used when the
     original data was compressed using 'dwz -m'.  */
  std::unique_ptr<struct dwz_file> dwz_file;

  /* Whether copy relocations are supported by this object format.  */
  bool can_copy;

  /* A flag indicating whether this objfile has a section loaded at a
     VMA of 0.  */
  bool has_section_at_zero = false;

  /* True if we are using the mapped index,
     or we are faking it for OBJF_READNOW's sake.  */
  bool using_index = false;

  /* The mapped index, or NULL if .gdb_index is missing or not being used.  */
  std::unique_ptr<mapped_index> index_table;

  /* The mapped index, or NULL if .debug_names is missing or not being used.  */
  std::unique_ptr<mapped_debug_names> debug_names_table;

  /* When using index_table, this keeps track of all quick_file_names entries.
     TUs typically share line table entries with a CU, so we maintain a
     separate table of all line table entries to support the sharing.
     Note that while there can be way more TUs than CUs, we've already
     sorted all the TUs into "type unit groups", grouped by their
     DW_AT_stmt_list value.  Therefore the only sharing done here is with a
     CU and its associated TU group if there is one.  */
  htab_up quick_file_names_table;

  /* Set during partial symbol reading, to prevent queueing of full
     symbols.  */
  bool reading_partial_symbols = false;

  /* Table mapping type DIEs to their struct type *.
     This is NULL if not allocated yet.
     The mapping is done via (CU/TU + DIE offset) -> type.  */
  htab_up die_type_hash;

  /* The CUs we recently read.  */
  std::vector<dwarf2_per_cu_data *> just_read_cus;

  /* Table containing line_header indexed by offset and offset_in_dwz.  */
  htab_up line_header_hash;

  /* Table containing all filenames.  This is an optional because the
     table is lazily constructed on first access.  */
  gdb::optional<filename_seen_cache> filenames_cache;

  /* If we loaded the index from an external file, this contains the
     resources associated to the open file, memory mapping, etc.  */
  std::unique_ptr<index_cache_resource> index_cache_res;

  /* Mapping from abstract origin DIE to concrete DIEs that reference it as
     DW_AT_abstract_origin.  */
  std::unordered_map<sect_offset, std::vector<sect_offset>,
		     gdb::hash_enum<sect_offset>>
    abstract_to_concrete;

  /* CUs that are queued to be read.  */
  std::queue<dwarf2_queue_item> queue;
};

/* Get the dwarf2_per_objfile associated to OBJFILE.  */

dwarf2_per_objfile *get_dwarf2_per_objfile (struct objfile *objfile);

/* A partial symtab specialized for DWARF.  */
struct dwarf2_psymtab : public standard_psymtab
{
  dwarf2_psymtab (const char *filename, struct objfile *objfile,
		  dwarf2_per_cu_data *per_cu)
    : standard_psymtab (filename, objfile, 0),
      per_cu_data (per_cu)
  {
  }

  void read_symtab (struct objfile *) override;
  void expand_psymtab (struct objfile *) override;

  struct dwarf2_per_cu_data *per_cu_data;
};

/* Persistent data held for a compilation unit, even when not
   processing it.  We put a pointer to this structure in the
   psymtab.  */

struct dwarf2_per_cu_data
{
  /* The start offset and length of this compilation unit.
     NOTE: Unlike comp_unit_head.length, this length includes
     initial_length_size.
     If the DIE refers to a DWO file, this is always of the original die,
     not the DWO file.  */
  sect_offset sect_off;
  unsigned int length;

  /* DWARF standard version this data has been read from (such as 4 or 5).  */
  short dwarf_version;

  /* Flag indicating this compilation unit will be read in before
     any of the current compilation units are processed.  */
  unsigned int queued : 1;

  /* This flag will be set when reading partial DIEs if we need to load
     absolutely all DIEs for this compilation unit, instead of just the ones
     we think are interesting.  It gets set if we look for a DIE in the
     hash table and don't find it.  */
  unsigned int load_all_dies : 1;

  /* Non-zero if this CU is from .debug_types.
     Struct dwarf2_per_cu_data is contained in struct signatured_type iff
     this is non-zero.  */
  unsigned int is_debug_types : 1;

  /* Non-zero if this CU is from the .dwz file.  */
  unsigned int is_dwz : 1;

  /* Non-zero if reading a TU directly from a DWO file, bypassing the stub.
     This flag is only valid if is_debug_types is true.
     We can't read a CU directly from a DWO file: There are required
     attributes in the stub.  */
  unsigned int reading_dwo_directly : 1;

  /* Non-zero if the TU has been read.
     This is used to assist the "Stay in DWO Optimization" for Fission:
     When reading a DWO, it's faster to read TUs from the DWO instead of
     fetching them from random other DWOs (due to comdat folding).
     If the TU has already been read, the optimization is unnecessary
     (and unwise - we don't want to change where gdb thinks the TU lives
     "midflight").
     This flag is only valid if is_debug_types is true.  */
  unsigned int tu_read : 1;

  /* The section this CU/TU lives in.
     If the DIE refers to a DWO file, this is always the original die,
     not the DWO file.  */
  struct dwarf2_section_info *section;

  /* Set to non-NULL iff this CU is currently loaded.  When it gets freed out
     of the CU cache it gets reset to NULL again.  This is left as NULL for
     dummy CUs (a CU header, but nothing else).  */
  struct dwarf2_cu *cu;

  /* The unit type of this CU.  */
  enum dwarf_unit_type unit_type;

  /* The language of this CU.  */
  enum language lang;

  /* The corresponding dwarf2_per_objfile.  */
  struct dwarf2_per_objfile *dwarf2_per_objfile;

  /* When dwarf2_per_objfile->using_index is true, the 'quick' field
     is active.  Otherwise, the 'psymtab' field is active.  */
  union
  {
    /* The partial symbol table associated with this compilation unit,
       or NULL for unread partial units.  */
    dwarf2_psymtab *psymtab;

    /* Data needed by the "quick" functions.  */
    struct dwarf2_per_cu_quick_data *quick;
  } v;

  /* The CUs we import using DW_TAG_imported_unit.  This is filled in
     while reading psymtabs, used to compute the psymtab dependencies,
     and then cleared.  Then it is filled in again while reading full
     symbols, and only deleted when the objfile is destroyed.

     This is also used to work around a difference between the way gold
     generates .gdb_index version <=7 and the way gdb does.  Arguably this
     is a gold bug.  For symbols coming from TUs, gold records in the index
     the CU that includes the TU instead of the TU itself.  This breaks
     dw2_lookup_symbol: It assumes that if the index says symbol X lives
     in CU/TU Y, then one need only expand Y and a subsequent lookup in Y
     will find X.  Alas TUs live in their own symtab, so after expanding CU Y
     we need to look in TU Z to find X.  Fortunately, this is akin to
     DW_TAG_imported_unit, so we just use the same mechanism: For
     .gdb_index version <=7 this also records the TUs that the CU referred
     to.  Concurrently with this change gdb was modified to emit version 8
     indices so we only pay a price for gold generated indices.
     http://sourceware.org/bugzilla/show_bug.cgi?id=15021.

     This currently needs to be a public member due to how
     dwarf2_per_cu_data is allocated and used.  Ideally in future things
     could be refactored to make this private.  Until then please try to
     avoid direct access to this member, and instead use the helper
     functions above.  */
  std::vector <dwarf2_per_cu_data *> *imported_symtabs;

  /* Return true of IMPORTED_SYMTABS is empty or not yet allocated.  */
  bool imported_symtabs_empty () const
  {
    return (imported_symtabs == nullptr || imported_symtabs->empty ());
  }

  /* Push P to the back of IMPORTED_SYMTABS, allocated IMPORTED_SYMTABS
     first if required.  */
  void imported_symtabs_push (dwarf2_per_cu_data *p)
  {
    if (imported_symtabs == nullptr)
      imported_symtabs = new std::vector <dwarf2_per_cu_data *>;
    imported_symtabs->push_back (p);
  }

  /* Return the size of IMPORTED_SYMTABS if it is allocated, otherwise
     return 0.  */
  size_t imported_symtabs_size () const
  {
    if (imported_symtabs == nullptr)
      return 0;
    return imported_symtabs->size ();
  }

  /* Delete IMPORTED_SYMTABS and set the pointer back to nullptr.  */
  void imported_symtabs_free ()
  {
    delete imported_symtabs;
    imported_symtabs = nullptr;
  }

  /* Return the OBJFILE associated with this compilation unit.  If
     this compilation unit came from a separate debuginfo file, then
     the master objfile is returned.  */
  struct objfile *objfile () const;

  /* Return the address size given in the compilation unit header for
     this CU.  */
  int addr_size () const;

  /* Return the offset size given in the compilation unit header for
     this CU.  */
  int offset_size () const;

  /* Return the DW_FORM_ref_addr size given in the compilation unit
     header for this CU.  */
  int ref_addr_size () const;

  /* Return the text offset of the CU.  The returned offset comes from
     this CU's objfile.  If this objfile came from a separate
     debuginfo file, then the offset may be different from the
     corresponding offset in the parent objfile.  */
  CORE_ADDR text_offset () const;

  /* Return a type that is a generic pointer type, the size of which
     matches the address size given in the compilation unit header for
     this CU.  */
  struct type *addr_type () const;

  /* Find an integer type SIZE_IN_BYTES bytes in size and return it.
     UNSIGNED_P controls if the integer is unsigned or not.  */
  struct type *int_type (int size_in_bytes, bool unsigned_p) const;

  /* Find an integer type the same size as the address size given in
     the compilation unit header for this CU.  UNSIGNED_P controls if
     the integer is unsigned or not.  */
  struct type *addr_sized_int_type (bool unsigned_p) const;

  /* Return DWARF version number of this CU.  */
  short version () const
  {
    return dwarf_version;
  }

  /* A type unit group has a per_cu object that is recognized by
     having no section.  */
  bool type_unit_group_p () const
  {
    return section == nullptr;
  }
};

/* Entry in the signatured_types hash table.  */

struct signatured_type
{
  /* The "per_cu" object of this type.
     This struct is used iff per_cu.is_debug_types.
     N.B.: This is the first member so that it's easy to convert pointers
     between them.  */
  struct dwarf2_per_cu_data per_cu;

  /* The type's signature.  */
  ULONGEST signature;

  /* Offset in the TU of the type's DIE, as read from the TU header.
     If this TU is a DWO stub and the definition lives in a DWO file
     (specified by DW_AT_GNU_dwo_name), this value is unusable.  */
  cu_offset type_offset_in_tu;

  /* Offset in the section of the type's DIE.
     If the definition lives in a DWO file, this is the offset in the
     .debug_types.dwo section.
     The value is zero until the actual value is known.
     Zero is otherwise not a valid section offset.  */
  sect_offset type_offset_in_section;

  /* Type units are grouped by their DW_AT_stmt_list entry so that they
     can share them.  This points to the containing symtab.  */
  struct type_unit_group *type_unit_group;

  /* The type.
     The first time we encounter this type we fully read it in and install it
     in the symbol tables.  Subsequent times we only need the type.  */
  struct type *type;

  /* Containing DWO unit.
     This field is valid iff per_cu.reading_dwo_directly.  */
  struct dwo_unit *dwo_unit;
};

/* Open the separate '.dwz' debug file, if needed.  Return NULL if
   there is no .gnu_debugaltlink section in the file.  Error if there
   is such a section but the file cannot be found.  */

extern struct dwz_file *dwarf2_get_dwz_file
    (struct dwarf2_per_objfile *dwarf2_per_objfile);

/* Return the type of the DIE at DIE_OFFSET in the CU named by
   PER_CU.  */

struct type *dwarf2_get_die_type (cu_offset die_offset,
				  struct dwarf2_per_cu_data *per_cu);

/* Given an index in .debug_addr, fetch the value.
   NOTE: This can be called during dwarf expression evaluation,
   long after the debug information has been read, and thus per_cu->cu
   may no longer exist.  */

CORE_ADDR dwarf2_read_addr_index (dwarf2_per_cu_data *per_cu,
				  unsigned int addr_index);

/* Return DWARF block referenced by DW_AT_location of DIE at SECT_OFF at PER_CU.
   Returned value is intended for DW_OP_call*.  Returned
   dwarf2_locexpr_baton->data has lifetime of
   PER_CU->DWARF2_PER_OBJFILE->OBJFILE.  */

struct dwarf2_locexpr_baton dwarf2_fetch_die_loc_sect_off
  (sect_offset sect_off, dwarf2_per_cu_data *per_cu,
   CORE_ADDR (*get_frame_pc) (void *baton),
   void *baton, bool resolve_abstract_p = false);

/* Like dwarf2_fetch_die_loc_sect_off, but take a CU
   offset.  */

struct dwarf2_locexpr_baton dwarf2_fetch_die_loc_cu_off
  (cu_offset offset_in_cu, dwarf2_per_cu_data *per_cu,
   CORE_ADDR (*get_frame_pc) (void *baton),
   void *baton);

/* If the DIE at SECT_OFF in PER_CU has a DW_AT_const_value, return a
   pointer to the constant bytes and set LEN to the length of the
   data.  If memory is needed, allocate it on OBSTACK.  If the DIE
   does not have a DW_AT_const_value, return NULL.  */

extern const gdb_byte *dwarf2_fetch_constant_bytes
  (sect_offset sect_off, dwarf2_per_cu_data *per_cu, obstack *obstack,
   LONGEST *len);

/* Return the type of the die at SECT_OFF in PER_CU.  Return NULL if no
   valid type for this die is found.  */

struct type *dwarf2_fetch_die_type_sect_off
  (sect_offset sect_off, dwarf2_per_cu_data *per_cu);

/* When non-zero, dump line number entries as they are read in.  */
extern unsigned int dwarf_line_debug;

#endif /* DWARF2READ_H */
