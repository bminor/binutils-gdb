/* BFD back-end for i386 a.out binaries.
   Copyright (C) 1990, 1991 Free Software Foundation, Inc.

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

#define	PAGE_SIZE	4096
#define	SEGMENT_SIZE	PAGE_SIZE
#define TEXT_START_ADDR	0x8000 
#define ARCH 32
#define BYTES_IN_WORD 4

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "aout64.h"
#include "stab.gnu.h"
#include "ar.h"
#include "libaout.h"           /* BFD a.out internal data structures */

bfd_target *aout386_callback ();

bfd_target *
DEFUN(aout386_object_p,(abfd),
     bfd *abfd)
{
  struct external_exec exec_bytes;
  struct internal_exec exec;

  if (bfd_read ((PTR) &exec_bytes, 1, EXEC_BYTES_SIZE, abfd)
      != EXEC_BYTES_SIZE) {
    bfd_error = wrong_format;
    return 0;
  }

  exec.a_info = bfd_h_get_32 (abfd, exec_bytes.e_info);

  if (N_BADMAG (exec)) return 0;

  NAME(aout,swap_exec_header_in)(abfd, &exec_bytes, &exec);
  return aout_32_some_aout_object_p (abfd, &exec, aout386_callback);
}

/* Finish up the reading of the file header */
bfd_target *
DEFUN(aout386_callback,(abfd),
      bfd *abfd)
{
  struct internal_exec *execp = exec_hdr (abfd);
  
  WORK_OUT_FILE_POSITIONS(abfd, execp) ;
  
  /* Determine the architecture and machine type of the object file.  */
  bfd_default_set_arch_mach(abfd, bfd_arch_i386, 0);

  return abfd->xvec;
}

/* Write an object file.
   Section contents have already been written.  We write the
   file header, symbols, and relocation.  */

boolean
DEFUN(aout386_write_object_contents,(abfd),
      bfd *abfd)
{
  bfd_size_type data_pad = 0;
  struct external_exec exec_bytes;
  struct internal_exec *execp = exec_hdr (abfd);

  WRITE_HEADERS(abfd, execp);
  return true;
}

/* Transfer vector */

/* We use BSD archive files.  */
#define	aout386_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	aout386_generic_stat_arch_elt	bfd_generic_stat_arch_elt
#define	aout386_slurp_armap		bfd_slurp_bsd_armap
#define	aout386_slurp_extended_name_table	bfd_true
#define	aout386_write_armap		bsd_write_armap
#define	aout386_truncate_arname		bfd_bsd_truncate_arname

/* We don't support core files here.  */
#define	aout386_core_file_failing_command _bfd_dummy_core_file_failing_command
#define	aout386_core_file_failing_signal _bfd_dummy_core_file_failing_signal
#define	aout386_core_file_matches_executable_p	\
				_bfd_dummy_core_file_matches_executable_p
#define	aout386_core_file_p		_bfd_dummy_target

#define aout386_bfd_debug_info_start		bfd_void
#define aout386_bfd_debug_info_end		bfd_void
#define aout386_bfd_debug_info_accumulate	(PROTO(void,(*),(bfd*, struct sec *))) bfd_void

#define aout386_mkobject		aout_32_mkobject 
#define aout386_close_and_cleanup	aout_32_close_and_cleanup
#define aout386_set_section_contents	aout_32_set_section_contents
#define aout386_get_section_contents	aout_32_get_section_contents
#define aout386_new_section_hook	aout_32_new_section_hook
#define aout386_get_symtab_upper_bound	aout_32_get_symtab_upper_bound
#define aout386_get_symtab		aout_32_get_symtab
#define aout386_get_reloc_upper_bound	aout_32_get_reloc_upper_bound
#define aout386_canonicalize_reloc	aout_32_canonicalize_reloc
#define aout386_make_empty_symbol	aout_32_make_empty_symbol
#define aout386_print_symbol		aout_32_print_symbol
#define aout386_get_lineno		aout_32_get_lineno
#define aout386_set_arch_mach		aout_32_set_arch_mach
#define aout386_find_nearest_line	aout_32_find_nearest_line
#define aout386_sizeof_headers		aout_32_sizeof_headers

bfd_target i386aout_vec = 	/* Intel 386 running a.out, embedded. */
{
  "a.out-i386",			/* name */
  bfd_target_aout_flavour,
  false,			/* target byte order */
  false,			/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  1,				/* minimum alignment */
 				/* data swap routines */
  _do_getl64, _do_putl64, _do_getl32, _do_putl32, _do_getl16, _do_putl16,
				/* header swap routines */
  _do_getl64, _do_putl64, _do_getl32, _do_putl32, _do_getl16, _do_putl16,

  {_bfd_dummy_target, aout386_object_p, /* bfd_check_format */
     bfd_generic_archive_p, aout386_core_file_p},
  {bfd_false, aout386_mkobject,	/* bfd_set_format */
     _bfd_generic_mkarchive, bfd_false},
  {bfd_false, aout386_write_object_contents, /* bfd_write_contents */
     _bfd_write_archive_contents, bfd_false},

  JUMP_TABLE(aout386)
};
