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

#ifndef GDB_DWARF2_READ_GDB_INDEX_H
#define GDB_DWARF2_READ_GDB_INDEX_H

#include "gdbsupport/function-view.h"

struct dwarf2_per_bfd;
struct dwarf2_per_objfile;
struct dwz_file;
struct objfile;

/* .gdb_index doesn't distinguish between the various "other" symbols
   -- but the symbol search machinery really wants to.  For example,
   an imported decl is "other" but is really a namespace and thus in
   TYPE_DOMAIN; whereas a Fortran module is also "other" but is in the
   MODULE_DOMAIN.  We use this value internally to represent the
   "other" case so that matching can work.  The exact value does not
   matter, all that matters here is that it won't overlap with any
   symbol that gdb might create.  */
#define DW_TAG_GDB_INDEX_OTHER 0xffff

/* Similarly, .gdb_index can't distinguish between the type and struct
   domains.  This is a special tag that inhabits both.  */
#define DW_TAG_GDB_INDEX_TYPE 0xfffe

/* Callback types for dwarf2_read_gdb_index.  */

typedef gdb::function_view
    <gdb::array_view<const gdb_byte>(objfile *, dwarf2_per_bfd *)>
    get_gdb_index_contents_ftype;
typedef gdb::function_view
    <gdb::array_view<const gdb_byte>(objfile *, dwz_file *)>
    get_gdb_index_contents_dwz_ftype;

/* Read .gdb_index.  If everything went ok, initialize the "quick"
   elements of all the CUs and return true.  Otherwise, return
   false.  */

bool dwarf2_read_gdb_index
  (dwarf2_per_objfile *per_objfile,
   get_gdb_index_contents_ftype get_gdb_index_contents,
   get_gdb_index_contents_dwz_ftype get_gdb_index_contents_dwz);

#endif /* GDB_DWARF2_READ_GDB_INDEX_H */
