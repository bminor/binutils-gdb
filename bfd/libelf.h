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

/* This CAT4 hack is to avoid a problem with some strict ANSI C preprocessors.
   The problem is, "32_" is not a valid preprocessing token, and we don't
   want extra underscores (e.g., "elf_32_").  The XCAT2 macro will cause the
   inner CAT macros to be evaluated first, producing still-valid pp-tokens.
   Then the final concatenation can be done.  (Sigh.)  */
#ifdef __STDC__
#define XCAT2(a,b)	CAT(a,b)
#define CAT4(a,b,c,d)	XCAT2(CAT(a,b),CAT(c,d))
#else
#define CAT4(a,b,c,d)	a/**/b/**/c/**/d
#endif

#ifndef NAME
#if ARCH_SIZE==64
#define NAME(x,y) CAT4(x,64,_,y)
#else /* ARCH_SIZE==32 */
#define NAME(x,y) CAT4(x,32,_,y)
#endif
#endif

#define ElfNAME(X)	NAME(Elf,X)
#define elfNAME(X)	NAME(elf,X)

typedef struct
{
  asymbol symbol;
  Elf_Internal_Sym internal_elf_sym;
  /* these are used for the generation of .stabX symbols (?) */
  short desc;
  unsigned char type;
  char other;
  union
    {
      unsigned int hppa_arg_reloc;
      PTR any;
    }
  tc_data;
  ElfNAME (External_Sym) native_elf_sym;
}

elfNAME (symbol_type);

/* Lacking nested functions and nested types, set up for mapping over
   BFD sections to produce ELF sections.  */
typedef struct
{
  Elf_Internal_Ehdr * i_ehdr;
  Elf_Internal_Shdr * i_shdrp;
  struct strtab *shstrtab;
  int symtab_section;
}
elf_sect_thunk;

struct elf_backend_data
{
  int use_rela_p;
  int elf_64_p;
  enum bfd_architecture arch;
  void (*elf_info_to_howto) PARAMS ((bfd *, arelent *,
				     Elf_Internal_Rela *));
  void (*elf_info_to_howto_rel) PARAMS ((bfd *, arelent *,
					 Elf_Internal_Rel *));

  /* @@ I really don't think this should be here.  I don't know what
     global_sym is supposed to be used for, but I doubt it's something
     that would be considered global, e.g., if you've got a program
     reading and writing many BFDs.  My hunch is that it's specific to
     the output BFD.  If not, put a comment here explaining why.  */
  elfNAME (symbol_type) * global_sym;
};

extern bfd_target *bfd_elf32_object_p PARAMS ((bfd *));
extern bfd_target *bfd_elf32_core_file_p PARAMS ((bfd *));
extern boolean bfd_elf32_mkobject PARAMS ((bfd *));
extern boolean bfd_elf32_write_object_contents PARAMS ((bfd *));
extern char *bfd_elf32_core_file_failing_command PARAMS ((bfd *));
extern int bfd_elf32_core_file_failing_signal PARAMS ((bfd *));
extern boolean bfd_elf32_core_file_matches_executable_p PARAMS ((bfd *,
								 bfd *));
extern boolean bfd_elf32_set_section_contents PARAMS ((bfd *, sec_ptr, PTR,
						       file_ptr,
						       bfd_size_type));

extern boolean elf_get_sect_thunk PARAMS ((bfd *, elf_sect_thunk *));
extern unsigned int bfd_elf32_get_symtab_upper_bound PARAMS ((bfd *));
extern unsigned int bfd_elf32_get_symtab PARAMS ((bfd *, asymbol **));
extern unsigned int bfd_elf32_get_reloc_upper_bound PARAMS ((bfd *, sec_ptr));
extern unsigned int bfd_elf32_canonicalize_reloc PARAMS ((bfd *, sec_ptr,
							  arelent **,
							  asymbol **));
extern asymbol *bfd_elf32_make_empty_symbol PARAMS ((bfd *));
extern void bfd_elf32_print_symbol PARAMS ((bfd *, PTR, asymbol *,
					    bfd_print_symbol_type));
extern void bfd_elf32_get_symbol_info PARAMS ((bfd *, asymbol *,
					       symbol_info *));
extern alent *bfd_elf32_get_lineno PARAMS ((bfd *, asymbol *));
extern boolean bfd_elf32_set_arch_mach PARAMS ((bfd *, enum bfd_architecture,
						unsigned long));
extern boolean bfd_elf32_find_nearest_line PARAMS ((bfd *, asection *,
						    asymbol **,
						    bfd_vma, CONST char **,
						    CONST char **,
						    unsigned int *));
extern int bfd_elf32_sizeof_headers PARAMS ((bfd *, boolean));
extern unsigned long elf_hash PARAMS ((CONST unsigned char *));
extern boolean bfd_elf32_new_section_hook PARAMS ((bfd *, asection *));

/* If the target doesn't have reloc handling written yet:  */
extern void bfd_elf32_no_info_to_howto PARAMS ((bfd *, arelent *,
						Elf32_Internal_Rela *));

#define get_elf_backend_data(abfd) \
  ((struct elf_backend_data *) (abfd)->xvec->backend_data)

struct strtab
{
  char *tab;
  int nentries;
  int length;
};

extern struct elf32_internal_shdr *
bfd_elf_locate_sh PARAMS ((bfd *, struct strtab *,
			   struct elf32_internal_shdr *, CONST char *));

#endif /* _LIBELF_H_ */
