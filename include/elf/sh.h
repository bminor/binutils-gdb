/* SH ELF support for BFD.
   Copyright (C) 1998 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _ELF_SH_H
#define _ELF_SH_H

/* Unless otherwise told we define an enum with the relocation entries.  */
#ifndef START_RELOC_NUMBERS
# define START_RELOC_NUMBERS(name)   enum name {
# define RELOC_NUMBER(name, number)  name = number ,
# define END_RELOC_NUMBERS           };
#endif

/* Relocations.  */
START_RELOC_NUMBERS (elf_sh_reloc_type)
  RELOC_NUMBER (R_SH_NONE, 0)
  RELOC_NUMBER (R_SH_DIR32, 1)
  RELOC_NUMBER (R_SH_REL32, 2)
  RELOC_NUMBER (R_SH_DIR8WPN, 3)
  RELOC_NUMBER (R_SH_IND12W, 4)
  RELOC_NUMBER (R_SH_DIR8WPL, 5)
  RELOC_NUMBER (R_SH_DIR8WPZ, 6)
  RELOC_NUMBER (R_SH_DIR8BP, 7)
  RELOC_NUMBER (R_SH_DIR8W, 8)
  RELOC_NUMBER (R_SH_DIR8L, 9)
  RELOC_NUMBER (R_SH_SWITCH16, 25)
  RELOC_NUMBER (R_SH_SWITCH32, 26)
  RELOC_NUMBER (R_SH_USES, 27)
  RELOC_NUMBER (R_SH_COUNT, 28)
  RELOC_NUMBER (R_SH_ALIGN, 29)
  RELOC_NUMBER (R_SH_CODE, 30)
  RELOC_NUMBER (R_SH_DATA, 31)
  RELOC_NUMBER (R_SH_LABEL, 32)
END_RELOC_NUMBERS

#endif
