/* Target definitions for 32-bit NLM (NetWare Loadable Module)
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

/* This structure contains everything that BFD knows about a target.
   It includes things like its byte order, name, what routines to call
   to do various operations, etc.  Every BFD points to a target structure
   with its "xvec" member.

   There are two such structures here:  one for big-endian machines and
   one for little-endian machines.   */

/* We don't have core files.  */

#define bfd_nlm32_core_file_p \
		((bfd_target *(*) PARAMS ((bfd *))) bfd_nullvoidptr)
#define	bfd_nlm32_core_file_failing_command \
		_bfd_dummy_core_file_failing_command
#define	bfd_nlm32_core_file_failing_signal \
		_bfd_dummy_core_file_failing_signal
#define	bfd_nlm32_core_file_matches_executable_p \
		_bfd_dummy_core_file_matches_executable_p

/* Archives are generic or unimplemented.  */

#define bfd_nlm32_slurp_armap \
		bfd_slurp_coff_armap
#define bfd_nlm32_slurp_extended_name_table \
		_bfd_slurp_extended_name_table
#define bfd_nlm32_truncate_arname \
		bfd_dont_truncate_arname
#define bfd_nlm32_openr_next_archived_file \
		bfd_generic_openr_next_archived_file
#define bfd_nlm32_generic_stat_arch_elt \
		bfd_generic_stat_arch_elt
#define	bfd_nlm32_write_armap \
		coff_write_armap

/* Ordinary section reading and writing */
#define bfd_nlm32_get_section_contents \
		bfd_generic_get_section_contents
#define	bfd_nlm32_close_and_cleanup \
		bfd_generic_close_and_cleanup

#define bfd_nlm32_bfd_debug_info_start \
		bfd_void
#define bfd_nlm32_bfd_debug_info_end \
		bfd_void
#define bfd_nlm32_bfd_debug_info_accumulate \
		(PROTO(void,(*),(bfd*, struct sec *))) bfd_void
#define bfd_nlm32_bfd_get_relocated_section_contents \
		bfd_generic_get_relocated_section_contents
#define bfd_nlm32_bfd_relax_section \
		bfd_generic_relax_section
#define bfd_nlm32_bfd_seclet_link \
		bfd_generic_seclet_link
#define bfd_nlm32_bfd_make_debug_symbol \
  ((asymbol *(*) PARAMS ((bfd *, void *, unsigned long))) bfd_nullvoidptr)

#define	bfd_nlm32_set_section_contents \
		bfd_generic_set_section_contents
#define bfd_nlm32_new_section_hook \
		_bfd_dummy_new_section_hook

#define bfd_nlm32_get_reloc_upper_bound \
  ((unsigned int (*) PARAMS ((bfd *, sec_ptr))) bfd_0u)

#define bfd_nlm32_canonicalize_reloc \
  ((unsigned int (*) PARAMS ((bfd *, sec_ptr, arelent **, asymbol **))) bfd_0u)

#define bfd_nlm32_print_symbol \
  ((void (*) PARAMS ((bfd *, PTR, asymbol *, bfd_print_symbol_type))) bfd_false)

#define bfd_nlm32_get_lineno \
  ((alent * (*) PARAMS ((bfd *, asymbol *))) bfd_false)

#define bfd_nlm32_find_nearest_line \
  ((boolean (*) PARAMS ((bfd *, asection *, asymbol **, bfd_vma, \
			 CONST char **, CONST char **, unsigned int *))) \
   bfd_false)

#define bfd_nlm32_sizeof_headers \
  ((int (*) PARAMS ((bfd *, boolean))) bfd_0u)

#define bfd_nlm32_write_object_contents \
  ((boolean (*) PARAMS ((bfd *))) bfd_false)

#ifdef TARGET_BIG_SYM
bfd_target TARGET_BIG_SYM =
{
  /* name: identify kind of target */
  TARGET_BIG_NAME,

  /* flavour: general indication about file */
  bfd_target_nlm_flavour,

  /* byteorder_big_p: data is big endian */
  true,

  /* header_byteorder_big_p: header is also big endian */
  true,

  /* object_flags: mask of all file flags */
  (HAS_RELOC | EXEC_P | HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS |
   DYNAMIC | WP_TEXT),
  
  /* section_flags: mask of all section flags */
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_READONLY |
   SEC_CODE | SEC_DATA), 

   /* leading_symbol_char: is the first char of a user symbol
      predictable, and if so what is it */
   0,

  /* ar_pad_char: pad character for filenames within an archive header
     FIXME:  this really has nothing to do with NLM, this is a characteristic
     of the archiver and/or os and should be independently tunable */
  '/',

  /* ar_max_namelen: maximum number of characters in an archive header
     FIXME:  this really has nothing to do with NLM, this is a characteristic
     of the archiver and should be independently tunable.  This value is
     a WAG (wild a** guess) */
  15,

  /* align_power_min: minimum alignment restriction for any section
     FIXME:  this value may be target machine dependent */
  3,

  /* Routines to byte-swap various sized integers from the data sections */
  _do_getb64, _do_getb_signed_64, _do_putb64,
    _do_getb32, _do_getb_signed_32, _do_putb32,
    _do_getb16, _do_getb_signed_16, _do_putb16,

  /* Routines to byte-swap various sized integers from the file headers */
  _do_getb64, _do_getb_signed_64, _do_putb64,
    _do_getb32, _do_getb_signed_32, _do_putb32,
    _do_getb16, _do_getb_signed_16, _do_putb16,

  /* bfd_check_format: check the format of a file being read */
  { _bfd_dummy_target,		/* unknown format */
    bfd_nlm32_object_p,		/* assembler/linker output (object file) */
    bfd_generic_archive_p,	/* an archive */
    bfd_nlm32_core_file_p	/* a core file */
  },

  /* bfd_set_format: set the format of a file being written */
  { bfd_false,
    bfd_nlm_mkobject,
    _bfd_generic_mkarchive,
    bfd_false
  },

  /* bfd_write_contents: write cached information into a file being written */
  { bfd_false,
    bfd_nlm32_write_object_contents,
    _bfd_write_archive_contents,
    bfd_false
  },

  /* Initialize a jump table with the standard macro.  All names start with
     "nlm" */
  JUMP_TABLE(bfd_nlm32),

  /* backend_data: */
  (PTR) NULL,
};
#endif

#ifdef TARGET_LITTLE_SYM
bfd_target TARGET_LITTLE_SYM =
{
  /* name: identify kind of target */
  TARGET_LITTLE_NAME,

  /* flavour: general indication about file */
  bfd_target_nlm_flavour,

  /* byteorder_big_p: data is big endian */
  false,		/* Nope -- this one's little endian */

  /* header_byteorder_big_p: header is also big endian */
  false,		/* Nope -- this one's little endian */

  /* object_flags: mask of all file flags */
  (HAS_RELOC | EXEC_P | HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS |
   DYNAMIC | WP_TEXT),
  
  /* section_flags: mask of all section flags */
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_READONLY |
   SEC_DATA), 

   /* leading_symbol_char: is the first char of a user symbol
      predictable, and if so what is it */
   0,

  /* ar_pad_char: pad character for filenames within an archive header
     FIXME:  this really has nothing to do with NLM, this is a characteristic
     of the archiver and/or os and should be independently tunable */
  '/',

  /* ar_max_namelen: maximum number of characters in an archive header
     FIXME:  this really has nothing to do with NLM, this is a characteristic
     of the archiver and should be independently tunable.  This value is
     a WAG (wild a** guess) */
  15,

  /* align_power_min: minimum alignment restriction for any section
     FIXME:  this value may be target machine dependent */
  3,

  /* Routines to byte-swap various sized integers from the data sections */
  _do_getl64, _do_getl_signed_64, _do_putl64,
    _do_getl32, _do_getl_signed_32, _do_putl32,
    _do_getl16, _do_getl_signed_16, _do_putl16,

  /* Routines to byte-swap various sized integers from the file headers */
  _do_getl64, _do_getl_signed_64, _do_putl64,
    _do_getl32, _do_getl_signed_32, _do_putl32,
    _do_getl16, _do_getl_signed_16, _do_putl16,

  /* bfd_check_format: check the format of a file being read */
  { _bfd_dummy_target,		/* unknown format */
    bfd_nlm32_object_p,		/* assembler/linker output (object file) */
    bfd_generic_archive_p,	/* an archive */
    bfd_nlm32_core_file_p	/* a core file */
  },

  /* bfd_set_format: set the format of a file being written */
  { bfd_false,
    bfd_nlm_mkobject,
    _bfd_generic_mkarchive,
    bfd_false
  },

  /* bfd_write_contents: write cached information into a file being written */
  { bfd_false,
    bfd_nlm32_write_object_contents,
    _bfd_write_archive_contents,
    bfd_false
  },

  /* Initialize a jump table with the standard macro.  All names start with
     "nlm" */
  JUMP_TABLE(bfd_nlm32),

  /* backend_data: */
  (PTR) NULL,
};
#endif
