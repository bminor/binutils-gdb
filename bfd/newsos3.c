/* BFD back-end for NewsOS3 (Sony, 68k) binaries.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.

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
#define TEXT_START_ADDR 0
#define ARCH 32
#define BYTES_IN_WORD 4

#include <ansidecl.h>
#include <sysdep.h>
#include "bfd.h"
#include "libbfd.h"
#include "aout64.h"

/**From: bothner@cs.wisc.edu***********************************************/
#undef N_TXTOFF
#define N_TXTOFF(x)   ( (N_MAGIC((x)) == ZMAGIC) ? PAGE_SIZE : EXEC_BYTES_SIZE)
/**************************************************************************/

#include "stab.gnu.h"
#include "ar.h"
#include "libaout.h"           /* BFD a.out internal data structures */
#if 0
int vfprintf(file, format, args) /* Temporary crock! */
     FILE *file; char *format; char *args;
{
    return _doprnt (format, args, file);
}
#endif


bfd_target *newsos3_callback ();

bfd_target *
DEFUN(newsos3_object_p,(abfd),
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
  return aout_32_some_aout_object_p (abfd, &exec, newsos3_callback);
}

/* Finish up the reading of a NEWS-OS a.out file header */
bfd_target *
DEFUN(newsos3_callback,(abfd),
      bfd *abfd)
{
  struct internal_exec *execp = exec_hdr (abfd);
  
  WORK_OUT_FILE_POSITIONS(abfd, execp) ;
  
  /* Determine the architecture and machine type of the object file.
   */
  bfd_default_set_arch_mach(abfd, bfd_arch_m68k, 0);


  return abfd->xvec;
}

/* Write an object file in NEWS-OS format.
   Section contents have already been written.  We write the
   file header, symbols, and relocation.  */

boolean
DEFUN(newsos3_write_object_contents,(abfd),
      bfd *abfd)
{
  bfd_size_type data_pad = 0;
  struct external_exec exec_bytes;
  struct internal_exec *execp = exec_hdr (abfd);

  WRITE_HEADERS(abfd, execp);
  return true;
}

/* Transfer vectors for NEWS-OS version 3 */

/* We use BFD generic archive files.  */
#define	newsos_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	newsos_generic_stat_arch_elt	bfd_generic_stat_arch_elt
#define	newsos_slurp_armap		bfd_slurp_bsd_armap
#define	newsos_slurp_extended_name_table	bfd_true
#define	newsos_write_armap		bsd_write_armap
#define	newsos_truncate_arname		bfd_bsd_truncate_arname

/* We don't support core files yet.  FIXME.  */
#define	newsos_core_file_failing_command	_bfd_dummy_core_file_failing_command
#define	newsos_core_file_failing_signal	_bfd_dummy_core_file_failing_signal
#define	newsos_core_file_matches_executable_p	\
				_bfd_dummy_core_file_matches_executable_p
#define	newsos_core_file_p		_bfd_dummy_target

#define newsos_bfd_debug_info_start		bfd_void
#define newsos_bfd_debug_info_end		bfd_void
#define newsos_bfd_debug_info_accumulate	(PROTO(void,(*),(bfd*, struct sec *))) bfd_void

#define newsos_mkobject aout_32_mkobject 
#define newsos_close_and_cleanup aout_32_close_and_cleanup 
#define newsos_set_section_contents aout_32_set_section_contents 
#define newsos_get_section_contents aout_32_get_section_contents 
#define newsos_new_section_hook aout_32_new_section_hook 
#define newsos_get_symtab_upper_bound aout_32_get_symtab_upper_bound 
#define newsos_get_symtab aout_32_get_symtab 
#define newsos_get_reloc_upper_bound aout_32_get_reloc_upper_bound 
#define newsos_canonicalize_reloc aout_32_canonicalize_reloc 
#define newsos_make_empty_symbol aout_32_make_empty_symbol 
#define newsos_print_symbol aout_32_print_symbol 
#define newsos_get_lineno aout_32_get_lineno 
#define newsos_set_arch_mach aout_32_set_arch_mach 
#define newsos_find_nearest_line aout_32_find_nearest_line 
#define newsos_sizeof_headers aout_32_sizeof_headers 


/* We define our own versions of these routines.  */


bfd_target newsos3_vec = /* Sony 68k-based machines running newsos3 */
{
  "a.out-newsos3",		/* name */
  bfd_target_aout_flavour,
  true,				/* target byte order */
  true,				/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  1,				/* minimum alignment */
  _do_getb64, _do_putb64,	_do_getb32, _do_putb32, _do_getb16, _do_putb16, /* data */
  _do_getb64, _do_putb64,	_do_getb32, _do_putb32, _do_getb16, _do_putb16, /* hdrs */

    {_bfd_dummy_target, newsos3_object_p, /* bfd_check_format */
       bfd_generic_archive_p, newsos_core_file_p},
    {bfd_false, newsos_mkobject,	/* bfd_set_format */
       _bfd_generic_mkarchive, bfd_false},
    {bfd_false, newsos3_write_object_contents, /* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},

  JUMP_TABLE(newsos)
};
