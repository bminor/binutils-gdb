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
#include "bfdlink.h"

/* If size isn't specified as 64 or 32, NAME macro should fail.  */
#ifndef NAME
#if ARCH_SIZE==64
#define NAME(x,y) CAT4(x,64,_,y)
#endif
#if ARCH_SIZE==32
#define NAME(x,y) CAT4(x,32,_,y)
#endif
#endif

#ifndef NAME
#define NAME(x,y) CAT4(x,NOSIZE,_,y)
#endif

#define ElfNAME(X)	NAME(Elf,X)
#define elfNAME(X)	NAME(elf,X)

/* Information held for an ELF symbol.  The first field is the
   corresponding asymbol.  Every symbol is an ELF file is actually a
   pointer to this structure, although it is often handled as a
   pointer to an asymbol.  */

typedef struct
{
  /* The BFD symbol.  */
  asymbol symbol;
  /* ELF symbol information.  */
  Elf_Internal_Sym internal_elf_sym;
  /* Backend specific information.  */
  union
    {
      unsigned int hppa_arg_reloc;
      PTR mips_extr;
      PTR any;
    }
  tc_data;
} elf_symbol_type;

/* Constant information held for an ELF backend.  */

struct elf_backend_data
{
  /* Whether the backend uses REL or RELA relocations.  FIXME: some
     ELF backends use both.  When we need to support one, this whole
     approach will need to be changed.  */
  int use_rela_p;

  /* Whether this backend is 64 bits or not.  FIXME: Who cares?  */
  int elf_64_p;

  /* The architecture for this backend.  */
  enum bfd_architecture arch;

  /* The ELF machine code (EM_xxxx) for this backend.  */
  int elf_machine_code;

  /* The maximum page size for this backend.  */
  bfd_vma maxpagesize;

  /* This is true if the linker should act like collect and gather
     global constructors and destructors by name.  This is true for
     MIPS ELF because the Irix 5 tools can not handle the .init
     section.  */
  boolean collect;

  /* A function to translate an ELF RELA relocation to a BFD arelent
     structure.  */
  void (*elf_info_to_howto) PARAMS ((bfd *, arelent *,
				     Elf_Internal_Rela *));

  /* A function to translate an ELF REL relocation to a BFD arelent
     structure.  */
  void (*elf_info_to_howto_rel) PARAMS ((bfd *, arelent *,
					 Elf_Internal_Rel *));

  /* A function to determine whether a symbol is global when
     partitioning the symbol table into local and global symbols.
     This should be NULL for most targets, in which case the correct
     thing will be done.  MIPS ELF, at least on the Irix 5, has
     special requirements.  */
  boolean (*elf_backend_sym_is_global) PARAMS ((bfd *, asymbol *));

  /* The remaining functions are hooks which are called only if they
     are not NULL.  */

  /* A function to permit a backend specific check on whether a
     particular BFD format is relevant for an object file, and to
     permit the backend to set any global information it wishes.  When
     this is called elf_elfheader is set, but anything else should be
     used with caution.  If this returns false, the check_format
     routine will return a bfd_error_wrong_format error.  */
  boolean (*elf_backend_object_p) PARAMS ((bfd *));

  /* A function to do additional symbol processing when reading the
     ELF symbol table.  This is where any processor-specific special
     section indices are handled.  */
  void (*elf_backend_symbol_processing) PARAMS ((bfd *, asymbol *));

  /* A function to do additional symbol processing after reading the
     entire ELF symbol table.  */
  boolean (*elf_backend_symbol_table_processing) PARAMS ((bfd *,
							  elf_symbol_type *,
							  int));

  /* A function to do additional processing on the ELF section header
     just before writing it out.  This is used to set the flags and
     type fields for some sections, or to actually write out data for
     unusual sections.  */
  boolean (*elf_backend_section_processing) PARAMS ((bfd *,
						     Elf32_Internal_Shdr *));

  /* A function to handle unusual section types when creating BFD
     sections from ELF sections.  */
  boolean (*elf_backend_section_from_shdr) PARAMS ((bfd *,
						    Elf32_Internal_Shdr *,
						    char *));

  /* A function to set up the ELF section header for a BFD section in
     preparation for writing it out.  This is where the flags and type
     fields are set for unusual sections.  */
  boolean (*elf_backend_fake_sections) PARAMS ((bfd *, Elf32_Internal_Shdr *,
						asection *));

  /* A function to get the ELF section index for a BFD section.  If
     this returns true, the section was found.  If it is a normal ELF
     section, *RETVAL should be left unchanged.  If it is not a normal
     ELF section *RETVAL should be set to the SHN_xxxx index.  */
  boolean (*elf_backend_section_from_bfd_section)
    PARAMS ((bfd *, Elf32_Internal_Shdr *, asection *, int *retval));

  /* If this field is not NULL, it is called by the add_symbols phase
     of a link just before adding a symbol to the global linker hash
     table.  It may modify any of the fields as it wishes.  If *NAME
     is set to NULL, the symbol will be skipped rather than being
     added to the hash table.  This function is responsible for
     handling all processor dependent symbol bindings and section
     indices, and must set at least *FLAGS and *SEC for each processor
     dependent case; failure to do so will cause a link error.  */
  boolean (*elf_add_symbol_hook)
    PARAMS ((bfd *abfd, struct bfd_link_info *info,
	     const Elf_Internal_Sym *, const char **name,
	     flagword *flags, asection **sec, bfd_vma *value));

  /* The RELOCATE_SECTION function is called by the ELF backend linker
     to handle the relocations for a section.

     The relocs are always passed as Rela structures; if the section
     actually uses Rel structures, the r_addend field will always be
     zero.

     This function is responsible for adjust the section contents as
     necessary, and (if using Rela relocs and generating a
     relocateable output file) adjusting the reloc addend as
     necessary.

     This function does not have to worry about setting the reloc
     address or the reloc symbol index.

     LOCAL_SYMS is a pointer to the swapped in local symbols.

     LOCAL_SECTIONS is an array giving the section in the input file
     corresponding to the st_shndx field of each local symbol.

     The global hash table entry for the global symbols can be found
     via elf_sym_hashes (input_bfd).

     When generating relocateable output, this function must handle
     STB_LOCAL/STT_SECTION symbols specially.  The output symbol is
     going to be the section symbol corresponding to the output
     section, which means that the addend must be adjusted
     accordingly.  */
  boolean (*elf_backend_relocate_section)
    PARAMS ((bfd *output_bfd, struct bfd_link_info *info,
	     bfd *input_bfd, asection *input_section, bfd_byte *contents,
	     Elf_Internal_Rela *relocs, Elf_Internal_Sym *local_syms,
	     asection **local_sections));

  /* A function to do any beginning processing needed for the ELF file
     before building the ELF headers and computing file positions.  */
  void (*elf_backend_begin_write_processing) PARAMS ((bfd *));

  /* A function to do any final processing needed for the ELF file
     before writing it out.  */
  void (*elf_backend_final_write_processing) PARAMS ((bfd *));

  /* The swapping table to use when dealing with ECOFF information.
     Used for the MIPS ELF .mdebug section.  */
  const struct ecoff_debug_swap *elf_backend_ecoff_debug_swap;
};

struct elf_sym_extra
{
  int elf_sym_num;		/* sym# after locals/globals are reordered */
};

typedef struct elf_sym_extra Elf_Sym_Extra;

struct bfd_elf_section_data {
  Elf_Internal_Shdr this_hdr;
  Elf_Internal_Shdr rel_hdr;
  int this_idx, rel_idx;
  struct elf_link_hash_entry **rel_hashes;
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
  Elf_Internal_Phdr *phdr;
  struct strtab *strtab_ptr;
  int num_locals;
  int num_globals;
  Elf_Sym_Extra *sym_extra;
  asymbol **section_syms;	/* STT_SECTION symbols for each section */
  int num_section_syms;		/* number of section_syms allocated */
  Elf_Internal_Shdr symtab_hdr;
  Elf_Internal_Shdr shstrtab_hdr;
  Elf_Internal_Shdr strtab_hdr;
  Elf_Internal_Shdr dynsymtab_hdr;
  Elf_Internal_Shdr dynstrtab_hdr;
  int symtab_section, shstrtab_section, strtab_section, dynsymtab_section;
  file_ptr next_file_pos;
  void *prstatus;		/* The raw /proc prstatus structure */
  void *prpsinfo;		/* The raw /proc prpsinfo structure */
  bfd_vma gp;			/* The gp value (MIPS only, for now) */
  int gp_size;			/* The gp size (MIPS only, for now) */

  /* A mapping from external symbols to entries in the linker hash
     table, used when linking.  This is indexed by the symbol index
     minus the sh_info field of the symbol table header.  */
  struct elf_link_hash_entry **sym_hashes;
};

#define elf_tdata(bfd)		((bfd) -> tdata.elf_obj_data)
#define elf_elfheader(bfd)	(elf_tdata(bfd) -> elf_header)
#define elf_elfsections(bfd)	(elf_tdata(bfd) -> elf_sect_ptr)
#define elf_shstrtab(bfd)	(elf_tdata(bfd) -> strtab_ptr)
#define elf_onesymtab(bfd)	(elf_tdata(bfd) -> symtab_section)
#define elf_dynsymtab(bfd)	(elf_tdata(bfd) -> dynsymtab_section)
#define elf_num_locals(bfd)	(elf_tdata(bfd) -> num_locals)
#define elf_num_globals(bfd)	(elf_tdata(bfd) -> num_globals)
#define elf_sym_extra(bfd)	(elf_tdata(bfd) -> sym_extra)
#define elf_section_syms(bfd)	(elf_tdata(bfd) -> section_syms)
#define elf_num_section_syms(bfd) (elf_tdata(bfd) -> num_section_syms)
#define core_prpsinfo(bfd)	(elf_tdata(bfd) -> prpsinfo)
#define core_prstatus(bfd)	(elf_tdata(bfd) -> prstatus)
#define elf_gp(bfd)		(elf_tdata(bfd) -> gp)
#define elf_gp_size(bfd)	(elf_tdata(bfd) -> gp_size)
#define elf_sym_hashes(bfd)	(elf_tdata(bfd) -> sym_hashes)

/* ELF linker hash table entries.  */

struct elf_link_hash_entry
{
  struct bfd_link_hash_entry root;
  /* Symbol index in output file.  This is initialized to -1.  It is
     set to -2 if the symbol is used by a reloc.  */
  long indx;
  /* Symbol size.  */
  bfd_size_type size;
  /* Symbol alignment (common symbols only).  */
  unsigned short align;
  /* Symbol type (STT_NOTYPE, STT_OBJECT, etc.).  */
  char type;
};

/* ELF linker hash table.  */

struct elf_link_hash_table
{
  struct bfd_link_hash_table root;
};

/* Look up an entry in an ELF linker hash table.  */

#define elf_link_hash_lookup(table, string, create, copy, follow)	\
  ((struct elf_link_hash_entry *)					\
   bfd_link_hash_lookup (&(table)->root, (string), (create),		\
			 (copy), (follow)))

/* Traverse an ELF linker hash table.  */

#define elf_link_hash_traverse(table, func, info)			\
  (bfd_link_hash_traverse						\
   (&(table)->root,							\
    (boolean (*) PARAMS ((struct bfd_link_hash_entry *, PTR))) (func),	\
    (info)))

/* Get the ELF linker hash table from a link_info structure.  */

#define elf_hash_table(p) ((struct elf_link_hash_table *) ((p)->hash))

extern char * elf_string_from_elf_section PARAMS ((bfd *, unsigned, unsigned));
extern char * elf_get_str_section PARAMS ((bfd *, unsigned));

#define bfd_elf32_mkobject	bfd_elf_mkobject
#define bfd_elf64_mkobject	bfd_elf_mkobject
#define elf_mkobject		bfd_elf_mkobject

extern unsigned long bfd_elf_hash PARAMS ((CONST unsigned char *));

extern bfd_reloc_status_type bfd_elf_generic_reloc PARAMS ((bfd *,
							    arelent *,
							    asymbol *,
							    PTR,
							    asection *,
							    bfd *,
							    char **));
extern boolean bfd_elf_mkobject PARAMS ((bfd *));
extern Elf_Internal_Shdr *bfd_elf_find_section PARAMS ((bfd *, char *));

extern struct bfd_link_hash_table *_bfd_elf_link_hash_table_create
  PARAMS ((bfd *));

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

extern long bfd_elf32_get_symtab_upper_bound PARAMS ((bfd *));
extern long bfd_elf32_get_symtab PARAMS ((bfd *, asymbol **));
extern long bfd_elf32_get_dynamic_symtab_upper_bound PARAMS ((bfd *));
extern long bfd_elf32_canonicalize_dynamic_symtab PARAMS ((bfd *, asymbol **));
extern long bfd_elf32_get_reloc_upper_bound PARAMS ((bfd *, sec_ptr));
extern long bfd_elf32_canonicalize_reloc PARAMS ((bfd *, sec_ptr,
						  arelent **, asymbol **));
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
extern boolean bfd_elf32_bfd_link_add_symbols
  PARAMS ((bfd *, struct bfd_link_info *));
extern boolean bfd_elf32_bfd_final_link
  PARAMS ((bfd *, struct bfd_link_info *));

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

extern long bfd_elf64_get_symtab_upper_bound PARAMS ((bfd *));
extern long bfd_elf64_get_symtab PARAMS ((bfd *, asymbol **));
extern long bfd_elf64_get_dynamic_symtab_upper_bound PARAMS ((bfd *));
extern long bfd_elf64_canonicalize_dynamic_symtab PARAMS ((bfd *, asymbol **));
extern long bfd_elf64_get_reloc_upper_bound PARAMS ((bfd *, sec_ptr));
extern long bfd_elf64_canonicalize_reloc PARAMS ((bfd *, sec_ptr,
						  arelent **, asymbol **));
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
extern boolean bfd_elf64_bfd_link_add_symbols
  PARAMS ((bfd *, struct bfd_link_info *));
extern boolean bfd_elf64_bfd_final_link
  PARAMS ((bfd *, struct bfd_link_info *));

/* If the target doesn't have reloc handling written yet:  */
extern void bfd_elf64_no_info_to_howto PARAMS ((bfd *, arelent *,
						Elf64_Internal_Rela *));

#endif /* _LIBELF_H_ */
