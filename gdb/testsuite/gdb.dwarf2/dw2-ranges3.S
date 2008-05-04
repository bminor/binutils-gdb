/*
   Copyright 2007, 2008 Free Software Foundation, Inc.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Despite the sections below will be adjacent the assembler has to produce
   DW_AT_ranges as the linker could place both sections at arbitrary locations.
   */

	/* Such directive is required by GAS for builds without `-g'.  */
	.file	1 "dw2-ranges3.S"

	/* Without this directive GAS will not emit DWARF2 unless we provide an
	   instruction to assemble.  We want to avoid any instructions to
	   remain architecture independent.  */
	.loc_mark_labels	1

	.text

	.globl	main3
	.func	main3
main3:	.int	0
	.endfunc
	.size	main3, . - main3
