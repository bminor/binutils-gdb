/* Generic support for 64-bit ELF
   Copyright 1993 Free Software Foundation, Inc.

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
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libelf.h"

/* This does not include any relocations, but should be good enough
   for GDB to read the file.  */

#define TARGET_LITTLE_SYM		bfd_elf64_little_generic_vec
#define TARGET_LITTLE_NAME		"elf64-little"
#define TARGET_BIG_SYM			bfd_elf64_big_generic_vec
#define TARGET_BIG_NAME			"elf64-big"
#define ELF_ARCH			bfd_arch_unknown
#define bfd_elf64_bfd_reloc_type_lookup bfd_default_reloc_type_lookup
#define elf_info_to_howto		bfd_elf64_no_info_to_howto

#include "elf64-target.h"
