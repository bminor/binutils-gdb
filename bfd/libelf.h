/* BFD back-end data structures for ELF files.
   Copyright (C) 1992, 1993 Free Software Foundation, Inc.
   Written by Cygnus Support.

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

#ifndef _LIBELF_H_
#define _LIBELF_H_ 1

#include "elf/common.h"
#include "elf/internal.h"
#include "elf/external.h"

typedef struct
{
  asymbol symbol;
  Elf_Internal_Sym internal_elf_sym;
  Elf_External_Sym native_elf_sym;
  /* these are used for the generation of .stabX symbols (?) */
  short desc;
  unsigned char type;
  char other;
} elf_symbol_type;

struct elf_backend_data {
  void (*elf_info_to_howto) PARAMS ((bfd *, arelent *, Elf_Internal_Rela *));
  enum bfd_architecture arch;

  /* @@ I really don't think this should be here.  I don't know what
     global_sym is supposed to be used for, but I doubt it's something
     that would be considered global, e.g., if you've got a program
     reading and writing many BFDs.  My hunch is that it's specific to
     the output BFD.  If not, put a comment here explaining why.  */
  elf_symbol_type *global_sym;
};

extern bfd_target *elf_object_p PARAMS ((bfd *));
extern bfd_target *elf_core_file_p PARAMS ((bfd *));
extern boolean elf_mkobject PARAMS ((bfd *));
extern boolean elf_write_object_contents PARAMS ((bfd *));
extern char *elf_core_file_failing_command PARAMS ((bfd *));
extern int elf_core_file_failing_signal PARAMS ((bfd *));
extern boolean elf_core_file_matches_executable_p PARAMS ((bfd *, bfd *));
extern boolean elf_set_section_contents PARAMS ((bfd *, sec_ptr, PTR,
						 file_ptr, bfd_size_type));
extern unsigned int elf_get_symtab_upper_bound PARAMS ((bfd *));
extern unsigned int elf_get_symtab PARAMS ((bfd *, asymbol **));
extern unsigned int elf_get_reloc_upper_bound PARAMS ((bfd *, sec_ptr));
extern unsigned int elf_canonicalize_reloc PARAMS ((bfd *, sec_ptr,
						    arelent **, asymbol **));
extern asymbol *elf_make_empty_symbol PARAMS ((bfd *));
extern void elf_print_symbol PARAMS ((bfd *, PTR, asymbol *,
				      bfd_print_symbol_type));
extern void elf_get_symbol_info PARAMS ((bfd *, asymbol *,
					 symbol_info *));
extern alent *elf_get_lineno PARAMS ((bfd *, asymbol *));
extern boolean elf_set_arch_mach PARAMS ((bfd *, enum bfd_architecture,
					  unsigned long));
extern boolean elf_find_nearest_line PARAMS ((bfd *, asection *, asymbol **,
					      bfd_vma, CONST char **,
					      CONST char **, unsigned int *));
extern int elf_sizeof_headers PARAMS ((bfd *, boolean));

/* If the target doesn't have reloc handling written yet:  */
extern void elf_no_info_to_howto PARAMS ((bfd *, arelent *,
					  Elf_Internal_Rela *));

#endif /* _LIBELF_H_ */
