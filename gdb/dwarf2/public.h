/* Public API for gdb DWARF reader

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

#ifndef DWARF2_PUBLIC_H
#define DWARF2_PUBLIC_H

extern int dwarf2_has_info (struct objfile *,
                            const struct dwarf2_debug_sections *,
			    bool = false);

/* A DWARF names index variant.  */
enum class dw_index_kind
{
  /* GDB's own .gdb_index format.   */
  GDB_INDEX,

  /* DWARF5 .debug_names.  */
  DEBUG_NAMES,
};

/* Initialize for reading DWARF for OBJFILE.  Return false if this
   file will use psymtabs, or true if using an index, in which case
   *INDEX_KIND is set to the index variant in use.  */
extern bool dwarf2_initialize_objfile (struct objfile *objfile,
				       dw_index_kind *index_kind);

struct psymbol_functions;
extern void dwarf2_build_psymtabs (struct objfile *,
				   psymbol_functions *psf = nullptr);
extern void dwarf2_build_frame_info (struct objfile *);

extern quick_symbol_functions_up make_dwarf_gdb_index ();
extern quick_symbol_functions_up make_dwarf_debug_names ();

#endif /* DWARF2_PUBLIC_H */
