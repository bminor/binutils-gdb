/* Generic support for 64-bit NLM (NetWare Loadable Module)
   Copyright (C) 1993 Free Software Foundation, Inc.

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

#define ARCH_SIZE 64
#include "libnlm.h"

/* This does not include any relocations, but should be good enough
   for GDB to read the file.  */

#define TARGET_LITTLE_NAME		"nlm64-little"
#define TARGET_LITTLE_SYM		nlmNAME(little_generic_vec)
#define TARGET_BIG_NAME			"nlm64-big"
#define TARGET_BIG_SYM			nlmNAME(big_generic_vec)
#define JUMP_TABLE_INIT			JUMP_TABLE(nlm64)

/* We don't have core files.  */

#define nlm64_core_file_p \
  ((bfd_target *(*) PARAMS ((bfd *))) bfd_nullvoidptr)
#define	nlm64_core_file_failing_command \
  _bfd_dummy_core_file_failing_command
#define	nlm64_core_file_failing_signal \
  _bfd_dummy_core_file_failing_signal
#define	nlm64_core_file_matches_executable_p \
  _bfd_dummy_core_file_matches_executable_p

/* Archives are generic or unimplemented.  */

#define nlm64_slurp_armap \
  bfd_slurp_coff_armap
#define nlm64_slurp_extended_name_table \
  _bfd_slurp_extended_name_table
#define nlm64_truncate_arname \
  bfd_dont_truncate_arname
#define nlm64_openr_next_archived_file \
  bfd_generic_openr_next_archived_file
#define nlm64_generic_stat_arch_elt \
  bfd_generic_stat_arch_elt
#define	nlm64_write_armap \
  coff_write_armap

/* Ordinary section reading and writing */
#define nlm64_get_section_contents \
  bfd_generic_get_section_contents
#define	nlm64_close_and_cleanup \
  bfd_generic_close_and_cleanup

#define nlm64_bfd_debug_info_start \
  bfd_void
#define nlm64_bfd_debug_info_end \
  bfd_void
#define nlm64_bfd_debug_info_accumulate \
  (PROTO(void,(*),(bfd*, struct sec *))) bfd_void
#define nlm64_bfd_get_relocated_section_contents \
  bfd_generic_get_relocated_section_contents
#define nlm64_bfd_relax_section \
  bfd_generic_relax_section
#define nlm64_bfd_seclet_link \
  bfd_generic_seclet_link
#define nlm64_bfd_make_debug_symbol \
  ((asymbol *(*) PARAMS ((bfd *, void *, unsigned long))) bfd_nullvoidptr)

#define	nlm64_set_section_contents \
  bfd_generic_set_section_contents
#define nlm64_new_section_hook \
  _bfd_dummy_new_section_hook

#define nlm64_get_reloc_upper_bound \
  ((unsigned int (*) PARAMS ((bfd *, sec_ptr))) bfd_0u)

#define nlm64_canonicalize_reloc \
  ((unsigned int (*) PARAMS ((bfd *, sec_ptr, arelent **, asymbol **))) bfd_0u)

#define nlm64_print_symbol \
  ((void (*) PARAMS ((bfd *, PTR, asymbol *, bfd_print_symbol_type))) bfd_false)

#define nlm64_get_lineno \
  ((alent * (*) PARAMS ((bfd *, asymbol *))) bfd_false)

#define nlm64_find_nearest_line \
  ((boolean (*) PARAMS ((bfd *, asection *, asymbol **, bfd_vma, \
			 CONST char **, CONST char **, unsigned int *))) \
   bfd_false)

#define nlm64_sizeof_headers \
  ((int (*) PARAMS ((bfd *, boolean))) bfd_0u)

#define nlm64_write_object_contents \
  ((boolean (*) PARAMS ((bfd *))) bfd_false)

#define nlm64_bfd_reloc_type_lookup \
  bfd_default_reloc_type_lookup

#include "nlm-target.h"
