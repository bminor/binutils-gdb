/* BFD XCOFF object file private structure.
   Copyright (C) 2001 Free Software Foundation, Inc.
   Written by Tom Rix, Redhat.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef LIBXCOFF_H
#define LIBXCOFF_H

/* This is the backend information kept for XCOFF files.  This
   structure is constant for a particular backend.  The first element
   is the COFF backend data structure, so that XCOFF targets can use
   the generic COFF code.  */

struct xcoff_backend_data_rec
{
  /* 
   * COFF backend information.  Must be the first field.  
   * This is where the std coff swap table goes
   */
  bfd_coff_backend_data coff;

  /* Magic number */
  unsigned short _xcoff_magic_number;              

  /* Architecture and machine for coff_set_arch_mach_hook */
  enum bfd_architecture _xcoff_architecture;
  long _xcoff_machine;

  /* function pointers to xcoff specific swap routines */
  void (* _xcoff_swap_ldhdr_in)(bfd *, PTR, struct internal_ldhdr *);
  void (* _xcoff_swap_ldhdr_out)(bfd *, struct internal_ldhdr *, PTR);
  void (* _xcoff_swap_ldsym_in)(bfd *, PTR, struct internal_ldsym *);
  void (* _xcoff_swap_ldsym_out)(bfd *, struct internal_ldsym *, PTR);
  void (* _xcoff_swap_ldrel_in)(bfd *, PTR, struct internal_ldrel *);
  void (* _xcoff_swap_ldrel_out)(bfd *, struct internal_ldrel *, PTR);

  /* size of the the external struct */
  unsigned int _xcoff_ldhdrsz; 
  unsigned int _xcoff_ldsymsz;
  unsigned int _xcoff_ldrelsz;

  /* size an entry in a descriptor section */
  unsigned int _xcoff_function_descriptor_size;

  /* size of the small aout file header */
  unsigned int _xcoff_small_aout_header_size;

  /* 
   * version 
   * loader version 
   * 1 : xcoff32
   * 2 : xcoff64
   */
  unsigned long _xcoff_ldhdr_version;

  boolean (* _xcoff_put_symbol_name)(bfd *, struct bfd_strtab_hash *,
				     struct internal_syment *,
				     const char *);

  boolean (* _xcoff_put_ldsymbol_name)(bfd *, struct xcoff_loader_info *, 
				       struct internal_ldsym *,
				       const char *);

  reloc_howto_type *_xcoff_dynamic_reloc;

  asection * (* _xcoff_create_csect_from_smclas) (bfd *, union internal_auxent *, const char *);

  /* 
   * line_no and reloc overflow 
   * 32 overflows to another section when the line_no or reloc count go
   * over 0xffff.  64 does not overflow. 
   */
  boolean (*_xcoff_is_lineno_count_overflow)(bfd *, bfd_vma);
  boolean (*_xcoff_is_reloc_count_overflow)(bfd *, bfd_vma);

  /* 
   * .loader symbol table offset
   * 32 is after the .loader header
   * 64 is offset in .loader header
   * 
   * Similar for the reloc table
   */
  bfd_vma (*_xcoff_loader_symbol_offset)(bfd *, struct internal_ldhdr *);
  bfd_vma (*_xcoff_loader_reloc_offset)(bfd *, struct internal_ldhdr *);


  /* 
   * Global linkage 
   * 
   * The first word of global linkage code must be be modified by
   * filling in the correct TOC offset.  
   */
  unsigned long *_xcoff_glink_code;

  /* 
   * Size of the global link code in bytes of the xcoff_glink_code table
   */
  unsigned long _xcoff_glink_size;

};

/* Look up an entry in an XCOFF link hash table.  */

#define xcoff_link_hash_lookup(table, string, create, copy, follow) \
  ((struct xcoff_link_hash_entry *) \
   bfd_link_hash_lookup (&(table)->root, (string), (create), (copy),\
			 (follow)))

/* Traverse an XCOFF link hash table.  */

#define xcoff_link_hash_traverse(table, func, info)			\
  (bfd_link_hash_traverse						\
   (&(table)->root,							\
    (boolean (*) PARAMS ((struct bfd_link_hash_entry *, PTR))) (func),	\
    (info)))

/* Get the XCOFF link hash table from the info structure.  This is
   just a cast.  */

#define xcoff_hash_table(p) ((struct xcoff_link_hash_table *) ((p)->hash))


#define xcoff_backend(abfd) \
  ((struct xcoff_backend_data_rec *) (abfd)->xvec->backend_data)

#define bfd_xcoff_magic_number(a) ((xcoff_backend(a)->_xcoff_magic_number))
#define bfd_xcoff_architecture(a) ((xcoff_backend(a)->_xcoff_architecture))
#define bfd_xcoff_machine(a) ((xcoff_backend(a)->_xcoff_machine))

#define bfd_xcoff_swap_ldhdr_in(a, b, c) \
  ((xcoff_backend(a)->_xcoff_swap_ldhdr_in) ((a), (b), (c)))

#define bfd_xcoff_swap_ldhdr_out(a, b, c) \
  ((xcoff_backend(a)->_xcoff_swap_ldhdr_out) ((a), (b), (c)))

#define bfd_xcoff_swap_ldsym_in(a, b, c) \
  ((xcoff_backend(a)->_xcoff_swap_ldsym_in) ((a), (b), (c)))

#define bfd_xcoff_swap_ldsym_out(a, b, c) \
  ((xcoff_backend(a)->_xcoff_swap_ldsym_out) ((a), (b), (c)))

#define bfd_xcoff_swap_ldrel_in(a, b, c) \
  ((xcoff_backend(a)->_xcoff_swap_ldrel_in) ((a), (b), (c)))

#define bfd_xcoff_swap_ldrel_out(a, b, c) \
  ((xcoff_backend(a)->_xcoff_swap_ldrel_out) ((a), (b), (c)))

#define bfd_xcoff_ldhdrsz(a) ((xcoff_backend(a)->_xcoff_ldhdrsz))
#define bfd_xcoff_ldsymsz(a) ((xcoff_backend(a)->_xcoff_ldsymsz))
#define bfd_xcoff_ldrelsz(a) ((xcoff_backend(a)->_xcoff_ldrelsz))
#define bfd_xcoff_function_descriptor_size(a) \
  ((xcoff_backend(a)->_xcoff_function_descriptor_size))
#define bfd_xcoff_small_aout_header_size(a) \
  ((xcoff_backend(a)->_xcoff_small_aout_header_size))

#define bfd_xcoff_ldhdr_version(a) ((xcoff_backend(a)->_xcoff_ldhdr_version))

#define bfd_xcoff_put_symbol_name(a, b, c, d) \
  ((xcoff_backend(a)->_xcoff_put_symbol_name) ((a), (b), (c), (d)))

#define bfd_xcoff_put_ldsymbol_name(a, b, c, d) \
  ((xcoff_backend(a)->_xcoff_put_ldsymbol_name) ((a), (b), (c), (d)))

/* Get the XCOFF hash table entries for a BFD.  */
#define obj_xcoff_sym_hashes(bfd) \
  ((struct xcoff_link_hash_entry **) obj_coff_sym_hashes (bfd))

#define bfd_xcoff_dynamic_reloc_howto(a) \
   ((xcoff_backend(a)->_xcoff_dynamic_reloc))

#define bfd_xcoff_create_csect_from_smclas(a, b, c) \
   ((xcoff_backend(a)->_xcoff_create_csect_from_smclas((a), (b), (c))))

#define bfd_xcoff_is_lineno_count_overflow(a, b) \
   ((xcoff_backend(a)->_xcoff_is_lineno_count_overflow((a), (b))))

#define bfd_xcoff_is_reloc_count_overflow(a, b) \
   ((xcoff_backend(a)->_xcoff_is_reloc_count_overflow((a), (b))))

#define bfd_xcoff_loader_symbol_offset(a, b) \
 ((xcoff_backend(a)->_xcoff_loader_symbol_offset((a), (b))))

#define bfd_xcoff_loader_reloc_offset(a, b) \
 ((xcoff_backend(a)->_xcoff_loader_reloc_offset((a), (b))))

#define bfd_xcoff_glink_code(a, b) ((xcoff_backend(a)->_xcoff_glink_code[(b)]))
#define bfd_xcoff_glink_code_size(a) ((xcoff_backend(a)->_xcoff_glink_size))

#define bfd_xcoff_is_xcoff64(a) (0x01EF == (bfd_xcoff_magic_number(a)))
#define bfd_xcoff_is_xcoff32(a) (0x01DF == (bfd_xcoff_magic_number(a)))

/* Functions in xcofflink.c.  */

extern long _bfd_xcoff_get_dynamic_symtab_upper_bound PARAMS ((bfd *));
extern long _bfd_xcoff_canonicalize_dynamic_symtab
  PARAMS ((bfd *, asymbol **));
extern long _bfd_xcoff_get_dynamic_reloc_upper_bound PARAMS ((bfd *));
extern long _bfd_xcoff_canonicalize_dynamic_reloc
  PARAMS ((bfd *, arelent **, asymbol **));
extern struct bfd_link_hash_table *_bfd_xcoff_bfd_link_hash_table_create
  PARAMS ((bfd *));
extern boolean _bfd_xcoff_bfd_link_add_symbols
  PARAMS ((bfd *, struct bfd_link_info *));
extern boolean _bfd_xcoff_bfd_final_link
  PARAMS ((bfd *, struct bfd_link_info *));
extern boolean _bfd_xcoff_slurp_symbol_table 
  PARAMS ((bfd *));
extern long _bfd_xcoff_get_symbol_table 
  PARAMS ((bfd *, asymbol **));
extern asymbol *_bfd_xcoff_make_empty_symbol 
  PARAMS ((bfd *));
extern long _bfd_xcoff_get_symbol_table_upper_bound 
  PARAMS ((bfd *));
extern void _bfd_xcoff_print_symbol 
  PARAMS ((bfd *, PTR, asymbol *, bfd_print_symbol_type));
extern void _bfd_xcoff_get_symbol_info 
  PARAMS ((bfd *, asymbol *, symbol_info *));
extern long _bfd_xcoff_canonicalize_reloc 
  PARAMS((bfd *, sec_ptr, arelent **, asymbol **));
extern reloc_howto_type *_bfd_xcoff_rtype_to_howto
  PARAMS ((bfd *, asection *, struct internal_reloc *,
	   struct coff_link_hash_entry *, struct internal_syment *,
	   bfd_vma *));
extern boolean _bfd_xcoff_set_section_contents
  PARAMS ((bfd *, asection *, PTR, file_ptr, bfd_size_type));
boolean _bfd_xcoff_write_object_contents PARAMS ((bfd *));


/* XCOFF support routines for the linker.  */

extern boolean bfd_xcoff_link_record_set
  PARAMS ((bfd *, struct bfd_link_info *, struct bfd_link_hash_entry *,
	   bfd_size_type));
extern boolean bfd_xcoff_import_symbol
  PARAMS ((bfd *, struct bfd_link_info *, struct bfd_link_hash_entry *,
	   bfd_vma, const char *, const char *, const char *));
extern boolean bfd_xcoff_export_symbol
  PARAMS ((bfd *, struct bfd_link_info *, struct bfd_link_hash_entry *,
	   boolean));
extern boolean bfd_xcoff_link_count_reloc
  PARAMS ((bfd *, struct bfd_link_info *, const char *));
extern boolean bfd_xcoff_record_link_assignment
  PARAMS ((bfd *, struct bfd_link_info *, const char *));
extern boolean bfd_xcoff_size_dynamic_sections
  PARAMS ((bfd *, struct bfd_link_info *, const char *, const char *,
	   unsigned long, unsigned long, unsigned long, boolean,
	   int, boolean, boolean, struct sec **));
extern boolean xcoff_slurp_reloc_table 
  PARAMS ((bfd *, asection *, asymbol **));

#endif /* LIBXCOFF_H */
