/* DWARF 2 debugging format support for GDB.

   Copyright (C) 1994-2025 Free Software Foundation, Inc.

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

#ifndef GDB_DWARF2_LINE_PROGRAM_H
#define GDB_DWARF2_LINE_PROGRAM_H

/* Decode the Line Number Program (LNP) for the given line_header
   structure and CU.  The actual information extracted and the type
   of structures created from the LNP depends on the value of PST.

   FND holds the CU file name and directory, if known.
   It is used for relative paths in the line table.

   NOTE: It is important that psymtabs have the same file name (via
   strcmp) as the corresponding symtab.  Since the directory is not
   used in the name of the symtab we don't use it in the name of the
   psymtabs we create.  E.g. expand_line_sal requires this when
   finding psymtabs to expand.  A good testcase for this is
   mb-inline.exp.

   LOWPC is the lowest address in CU (or 0 if not known).

   Boolean DECODE_MAPPING specifies we need to fully decode .debug_line
   for its PC<->lines mapping information.  Otherwise only the filename
   table is read in.  */

extern void dwarf_decode_lines (struct line_header *lh,
				struct dwarf2_cu *cu,
				unrelocated_addr lowpc, bool decode_mapping);

#endif /* GDB_DWARF2_LINE_PROGRAM_H */
