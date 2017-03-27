/* 32-bit ELF for the WebAssembly target
   Copyright (C) 2017 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "bfd_stdint.h"
#include "elf/wasm32.h"

#define ELF_ARCH		bfd_arch_wasm32
#define ELF_TARGET_ID		EM_WEBASSEMBLY
#define ELF_MACHINE_CODE	EM_WEBASSEMBLY
/* FIXME we don't have paged executables, see:
   https://github.com/pipcet/binutils-gdb/issues/4  */
#define ELF_MAXPAGESIZE		4096

#define TARGET_LITTLE_SYM       wasm32_elf32_vec
#define TARGET_LITTLE_NAME	"elf32-wasm32"

#define elf_backend_can_gc_sections     1
#define elf_backend_rela_normal         1
/* For testing. */
#define elf_backend_want_dynrelro       1

#define bfd_elf32_bfd_reloc_type_lookup _bfd_norelocs_bfd_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup _bfd_norelocs_bfd_reloc_name_lookup

#define ELF_DYNAMIC_INTERPRETER  "/sbin/elf-dynamic-interpreter.so"

#define elf_backend_want_got_plt 	1
#define elf_backend_plt_readonly 	1
#define elf_backend_got_header_size 	0

#include "elf32-target.h"
