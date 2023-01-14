/* DWARF CU data structure

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

#include "defs.h"
#include "dwarf2/cu.h"
#include "dwarf2/read.h"

/* Initialize dwarf2_cu to read PER_CU, in the context of PER_OBJFILE.  */

dwarf2_cu::dwarf2_cu (dwarf2_per_cu_data *per_cu,
		      dwarf2_per_objfile *per_objfile)
  : per_cu (per_cu),
    per_objfile (per_objfile),
    m_mark (false),
    has_loclist (false),
    checked_producer (false),
    producer_is_gxx_lt_4_6 (false),
    producer_is_gcc_lt_4_3 (false),
    producer_is_icc (false),
    producer_is_icc_lt_14 (false),
    producer_is_codewarrior (false),
    processing_has_namespace_info (false),
    load_all_dies (false)
{
}

/* See cu.h.  */

struct type *
dwarf2_cu::addr_sized_int_type (bool unsigned_p) const
{
  int addr_size = this->per_cu->addr_size ();
  return this->per_objfile->int_type (addr_size, unsigned_p);
}

/* Start a symtab for DWARF.  NAME, COMP_DIR, LOW_PC are passed to the
   buildsym_compunit constructor.  */

struct compunit_symtab *
dwarf2_cu::start_symtab (const char *name, const char *comp_dir,
			 CORE_ADDR low_pc)
{
  gdb_assert (m_builder == nullptr);

  m_builder.reset (new struct buildsym_compunit
		   (this->per_objfile->objfile,
		    name, comp_dir, per_cu->lang, low_pc));

  list_in_scope = get_builder ()->get_file_symbols ();

  get_builder ()->record_debugformat ("DWARF 2");
  get_builder ()->record_producer (producer);

  processing_has_namespace_info = false;

  return get_builder ()->get_compunit_symtab ();
}

/* See read.h.  */

struct type *
dwarf2_cu::addr_type () const
{
  struct objfile *objfile = this->per_objfile->objfile;
  struct type *void_type = objfile_type (objfile)->builtin_void;
  struct type *addr_type = lookup_pointer_type (void_type);
  int addr_size = this->per_cu->addr_size ();

  if (TYPE_LENGTH (addr_type) == addr_size)
    return addr_type;

  addr_type = addr_sized_int_type (addr_type->is_unsigned ());
  return addr_type;
}

/* A hashtab traversal function that marks the dependent CUs.  */

static int
dwarf2_mark_helper (void **slot, void *data)
{
  dwarf2_per_cu_data *per_cu = (dwarf2_per_cu_data *) *slot;
  dwarf2_per_objfile *per_objfile = (dwarf2_per_objfile *) data;
  dwarf2_cu *cu = per_objfile->get_cu (per_cu);

  /* cu->m_dependencies references may not yet have been ever read if
     QUIT aborts reading of the chain.  As such dependencies remain
     valid it is not much useful to track and undo them during QUIT
     cleanups.  */
  if (cu != nullptr)
    cu->mark ();
  return 1;
}

/* See dwarf2/cu.h.  */

void
dwarf2_cu::mark ()
{
  if (!m_mark)
    {
      m_mark = true;
      if (m_dependencies != nullptr)
	htab_traverse (m_dependencies, dwarf2_mark_helper, per_objfile);
    }
}

/* See dwarf2/cu.h.  */

void
dwarf2_cu::add_dependence (struct dwarf2_per_cu_data *ref_per_cu)
{
  void **slot;

  if (m_dependencies == nullptr)
    m_dependencies
      = htab_create_alloc_ex (5, htab_hash_pointer, htab_eq_pointer,
			      NULL, &comp_unit_obstack,
			      hashtab_obstack_allocate,
			      dummy_obstack_deallocate);

  slot = htab_find_slot (m_dependencies, ref_per_cu, INSERT);
  if (*slot == nullptr)
    *slot = ref_per_cu;
}

/* See dwarf2/cu.h.  */

buildsym_compunit *
dwarf2_cu::get_builder ()
{
  /* If this CU has a builder associated with it, use that.  */
  if (m_builder != nullptr)
    return m_builder.get ();

  if (per_objfile->sym_cu != nullptr)
    return per_objfile->sym_cu->m_builder.get ();

  gdb_assert_not_reached ("");
}
