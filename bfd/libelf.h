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
#ifdef SABER
#define CAT4(a,b,c,d)	a##b##c##d
#else
#ifdef __STDC__
#define XCAT2(a,b)	CAT(a,b)
#define CAT4(a,b,c,d)	XCAT2(CAT(a,b),CAT(c,d))
#else
#define CAT4(a,b,c,d)	a/**/b/**/c/**/d
#endif
#endif

/* If size isn't specified as 64 or 32, NAME macro should fail.  */
#ifndef NAME
#if ARCH_SIZE==64
#define NAME(x,y) CAT4(x,64,_,y)
#endif
#if ARCH_SIZE==32
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
  Elf32_External_Sym native_elf_sym;
} elf32_symbol_type;

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
  Elf64_External_Sym native_elf_sym;
} elf64_symbol_type;

struct elf_backend_data
{
  int use_rela_p;
  int elf_64_p;
  enum bfd_architecture arch;
  void (*elf_info_to_howto) PARAMS ((bfd *, arelent *,
				     Elf_Internal_Rela *));
  void (*elf_info_to_howto_rel) PARAMS ((bfd *, arelent *,
					 Elf_Internal_Rel *));
  void (*write_relocs) PARAMS ((bfd *, asection *, PTR));

  /* @@ I really don't think this should be here.  I don't know what
     global_sym is supposed to be used for, but I doubt it's something
     that would be considered global, e.g., if you've got a program
     reading and writing many BFDs.  My hunch is that it's specific to
     the output BFD.  If not, put a comment here explaining why.  */
  /* @@ Was pointer to elfNAME(symbol_type).  This makes it size-
     independent.  */
  PTR global_sym;
};

struct bfd_elf_arch_map {
  enum bfd_architecture bfd_arch;
  int elf_arch;
};

extern const struct bfd_elf_arch_map bfd_elf_arch_map[];
extern const int bfd_elf_arch_map_size;

struct bfd_elf_section_data {
  Elf_Internal_Shdr this_hdr;
  Elf_Internal_Shdr rel_hdr;
  int this_idx, rel_idx;
#if 0
  Elf_Internal_Shdr str_hdr;
  int str_idx;
#endif
};
#define elf_section_data(sec)  ((struct bfd_elf_section_data*)sec->used_by_bfd)
#define shdr_name(abfd,shdr)	(elf_shstrtab (abfd)->tab + (shdr)->sh_name)

#define get_elf_backend_data(abfd) \
  ((struct elf_backend_data *) (abfd)->xvec->backend_data)

struct strtab
{
  char *tab;
  int nentries;
  int length;
};

/* Some private data is stashed away for future use using the tdata pointer
   in the bfd structure.  */

struct elf_obj_tdata
{
  Elf_Internal_Ehdr elf_header[1];	/* Actual data, but ref like ptr */
  Elf_Internal_Shdr **elf_sect_ptr;
  struct strtab *strtab_ptr;
  int num_locals;
  int num_globals;
  int *symtab_map;
  PTR raw_syms;			/* Elf_External_Sym* */
  Elf_Internal_Sym *internal_syms;
  PTR symbols;			/* elf_symbol_type */
/*  struct strtab *shstrtab;*/
  Elf_Internal_Shdr symtab_hdr;
  Elf_Internal_Shdr shstrtab_hdr;
  Elf_Internal_Shdr strtab_hdr;
  int symtab_section, shstrtab_section, strtab_section;
  file_ptr next_file_pos;
  void *prstatus;		/* The raw /proc prstatus structure */
  void *prpsinfo;		/* The raw /proc prpsinfo structure */
};

#define elf_tdata(bfd)		((bfd) -> tdata.elf_obj_data)
#define elf_elfheader(bfd)	(elf_tdata(bfd) -> elf_header)
#define elf_elfsections(bfd)	(elf_tdata(bfd) -> elf_sect_ptr)
#define elf_shstrtab(bfd)	(elf_tdata(bfd) -> strtab_ptr)
#define elf_onesymtab(bfd)	(elf_tdata(bfd) -> symtab_section)
#define elf_num_locals(bfd)	(elf_tdata(bfd) -> num_locals)
#define elf_num_globals(bfd)	(elf_tdata(bfd) -> num_globals)
#define elf_symtab_map(bfd)	(elf_tdata(bfd) -> symtab_map)
#define core_prpsinfo(bfd)	(elf_tdata(bfd) -> prpsinfo)
#define core_prstatus(bfd)	(elf_tdata(bfd) -> prstatus)
#define obj_symbols(bfd)	((elf_symbol_type*)(elf_tdata(bfd) -> symbols))
#define obj_raw_syms(bfd)	((Elf_External_Sym*)(elf_tdata(bfd) -> raw_syms))
#define obj_internal_syms(bfd)	(elf_tdata(bfd) -> internal_syms)

extern char * elf_string_from_elf_section PARAMS ((bfd *, unsigned, unsigned));
extern char * elf_get_str_section PARAMS ((bfd *, unsigned));

#define bfd_elf32_mkobject	bfd_elf_mkobject
#define bfd_elf64_mkobject	bfd_elf_mkobject
#define elf_mkobject		bfd_elf_mkobject

extern unsigned long elf_hash PARAMS ((CONST unsigned char *));

extern bfd_reloc_status_type bfd_elf_generic_reloc PARAMS ((bfd *,
							    arelent *,
							    asymbol *,
							    PTR,
							    asection *,
							    bfd *));
extern boolean bfd_elf_mkobject PARAMS ((bfd *));
extern boolean bfd_elf32_write_object_contents PARAMS ((bfd *));
extern boolean bfd_elf64_write_object_contents PARAMS ((bfd *));

extern bfd_target *bfd_elf32_object_p PARAMS ((bfd *));
extern bfd_target *bfd_elf32_core_file_p PARAMS ((bfd *));
extern char *bfd_elf32_core_file_failing_command PARAMS ((bfd *));
extern int bfd_elf32_core_file_failing_signal PARAMS ((bfd *));
extern boolean bfd_elf32_core_file_matches_executable_p PARAMS ((bfd *,
								 bfd *));
extern boolean bfd_elf32_set_section_contents PARAMS ((bfd *, sec_ptr, PTR,
						       file_ptr,
						       bfd_size_type));

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
extern void bfd_elf32__write_relocs PARAMS ((bfd *, asection *, PTR));
extern boolean bfd_elf32_new_section_hook PARAMS ((bfd *, asection *));

/* If the target doesn't have reloc handling written yet:  */
extern void bfd_elf32_no_info_to_howto PARAMS ((bfd *, arelent *,
						Elf32_Internal_Rela *));

extern bfd_target *bfd_elf64_object_p PARAMS ((bfd *));
extern bfd_target *bfd_elf64_core_file_p PARAMS ((bfd *));
extern char *bfd_elf64_core_file_failing_command PARAMS ((bfd *));
extern int bfd_elf64_core_file_failing_signal PARAMS ((bfd *));
extern boolean bfd_elf64_core_file_matches_executable_p PARAMS ((bfd *,
								 bfd *));
extern boolean bfd_elf64_set_section_contents PARAMS ((bfd *, sec_ptr, PTR,
						       file_ptr,
						       bfd_size_type));

extern unsigned int bfd_elf64_get_symtab_upper_bound PARAMS ((bfd *));
extern unsigned int bfd_elf64_get_symtab PARAMS ((bfd *, asymbol **));
extern unsigned int bfd_elf64_get_reloc_upper_bound PARAMS ((bfd *, sec_ptr));
extern unsigned int bfd_elf64_canonicalize_reloc PARAMS ((bfd *, sec_ptr,
							  arelent **,
							  asymbol **));
extern asymbol *bfd_elf64_make_empty_symbol PARAMS ((bfd *));
extern void bfd_elf64_print_symbol PARAMS ((bfd *, PTR, asymbol *,
					    bfd_print_symbol_type));
extern void bfd_elf64_get_symbol_info PARAMS ((bfd *, asymbol *,
					       symbol_info *));
extern alent *bfd_elf64_get_lineno PARAMS ((bfd *, asymbol *));
extern boolean bfd_elf64_set_arch_mach PARAMS ((bfd *, enum bfd_architecture,
						unsigned long));
extern boolean bfd_elf64_find_nearest_line PARAMS ((bfd *, asection *,
						    asymbol **,
						    bfd_vma, CONST char **,
						    CONST char **,
						    unsigned int *));
extern int bfd_elf64_sizeof_headers PARAMS ((bfd *, boolean));
extern void bfd_elf64__write_relocs PARAMS ((bfd *, asection *, PTR));
extern boolean bfd_elf64_new_section_hook PARAMS ((bfd *, asection *));

/* If the target doesn't have reloc handling written yet:  */
extern void bfd_elf64_no_info_to_howto PARAMS ((bfd *, arelent *,
						Elf64_Internal_Rela *));

#endif /* _LIBELF_H_ */
