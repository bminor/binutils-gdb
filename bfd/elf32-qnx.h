/* QNX specific support for 32-bit ELF
   Copyright 2002  Free Software Foundation, Inc.

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

boolean elf_qnx_copy_private_bfd_data_p
  PARAMS ((bfd *, asection *, bfd *, asection *));
boolean elf_qnx_is_contained_by_filepos
  PARAMS ((asection *, Elf_Internal_Phdr *));
void elf_qnx_set_nonloadable_filepos
  PARAMS ((bfd *, Elf_Internal_Phdr *));

#undef	elf_backend_set_nonloadable_filepos
#define	elf_backend_set_nonloadable_filepos elf_qnx_set_nonloadable_filepos
#undef	elf_backend_is_contained_by_filepos
#define	elf_backend_is_contained_by_filepos elf_qnx_is_contained_by_filepos
#undef	elf_backend_copy_private_bfd_data_p
#define	elf_backend_copy_private_bfd_data_p elf_qnx_copy_private_bfd_data_p
