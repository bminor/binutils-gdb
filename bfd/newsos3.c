/*** bfd backend for NewsOS3 (Sony, 68k) binaries */

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define TARGET_BYTE_ORDER_BIG_P 1

#include <ansidecl.h>
#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

int vfprintf(file, format, args) /* Temporary crock! */
     FILE *file; char *format; char *args;
{
    return _doprnt (format, args, file);
}

#define	PAGE_SIZE	4096
#define	SEGMENT_SIZE	0x2000		/* FIXME, is this right?? */
#define TEXT_START_ADDR PAGE_SIZE

#include "a.out.gnu.h"
#include "stab.gnu.h"
#include "ar.h"
#include "liba.out.h"           /* BFD a.out internal data structures */

bfd_target *newsos3_callback ();

bfd_target *
newsos3_object_p (abfd)
     bfd *abfd;
{
  unsigned char magicbuf[LONG_SIZE]; /* Raw bytes of magic number from file */
  unsigned long magic;		/* Swapped magic number */

  bfd_error = system_call_error;

  if (bfd_read ((PTR)magicbuf, 1, sizeof (magicbuf), abfd) !=
      sizeof (magicbuf))
    return 0;
  magic = bfd_h_getlong (abfd, magicbuf);

  if (N_BADMAG (*((struct exec *) &magic))) return 0;

  return some_aout_object_p (abfd, newsos3_callback);
}

/* Finish up the reading of a NEWS-OS a.out file header */
bfd_target *
newsos3_callback (abfd)
     bfd *abfd;
{
  struct exec *execp = exec_hdr (abfd);

  /* The virtual memory addresses of the sections */
  obj_datasec (abfd)->vma = N_DATADDR(*execp);
  obj_bsssec (abfd)->vma = N_BSSADDR(*execp);
  obj_textsec (abfd)->vma = N_TXTADDR(*execp);

  /* The file offsets of the sections */
  obj_textsec (abfd)->filepos = N_TXTOFF(*execp);
  obj_datasec (abfd)->filepos = N_DATOFF(*execp);

  /* The file offsets of the relocation info */
  obj_textsec (abfd)->rel_filepos = N_TRELOFF(*execp);
  obj_datasec (abfd)->rel_filepos = N_DRELOFF(*execp);

  /* The file offsets of the string table and symbol table.  */
  obj_str_filepos (abfd) = N_STROFF (*execp);
  obj_sym_filepos (abfd) = N_SYMOFF (*execp);

  /* Determine the architecture and machine type of the object file.  */
  abfd->obj_arch = bfd_arch_m68k;
  abfd->obj_machine = 0;

  return abfd->xvec;
}

/* Write an object file in NEWS-OS format.
   Section contents have already been written.  We write the
   file header, symbols, and relocation.  */

boolean
newsos3_write_object_contents (abfd)
     bfd *abfd;
{
  size_t data_pad = 0;
  unsigned char exec_bytes[EXEC_BYTES_SIZE];
  struct exec *execp = exec_hdr (abfd);

  execp->a_text = obj_textsec (abfd)->size;

  if (abfd->flags & D_PAGED)
    execp->a_info = ZMAGIC;
  else if (abfd->flags & WP_TEXT)
    execp->a_info = NMAGIC;
  else
    execp->a_info = OMAGIC;

  if (abfd->flags & D_PAGED) 
      {
	data_pad = ((obj_datasec(abfd)->size + PAGE_SIZE -1)
		    & (- PAGE_SIZE)) - obj_datasec(abfd)->size;

	if (data_pad > obj_bsssec(abfd)->size)
	  execp->a_bss = 0;
	else 
	  execp->a_bss = obj_bsssec(abfd)->size - data_pad;
	execp->a_data = obj_datasec(abfd)->size + data_pad;

      }
  else {
    execp->a_data = obj_datasec (abfd)->size;
    execp->a_bss = obj_bsssec (abfd)->size;
  }

  execp->a_syms = bfd_get_symcount (abfd) * sizeof (struct nlist);
  execp->a_entry = bfd_get_start_address (abfd);

  execp->a_trsize = ((obj_textsec (abfd)->reloc_count) *
		     obj_reloc_entry_size (abfd));
		       
  execp->a_drsize = ((obj_datasec (abfd)->reloc_count) *
		     obj_reloc_entry_size (abfd));

  bfd_aout_swap_exec_header_out (abfd, execp, exec_bytes);

  bfd_seek (abfd, 0L, false);
  bfd_write ((PTR) exec_bytes, 1, EXEC_BYTES_SIZE, abfd);

  /* Now write out reloc info, followed by syms and strings */

  if (bfd_get_symcount (abfd) != 0) 
    {
      bfd_seek (abfd, (long)(N_SYMOFF(*execp)), false);
      aout_write_syms (abfd);

      bfd_seek (abfd, (long)(N_TRELOFF(*execp)), false);
      if (!aout_squirt_out_relocs (abfd, obj_textsec (abfd))) return false;

      bfd_seek (abfd, (long)(N_DRELOFF(*execp)), false);
      if (!aout_squirt_out_relocs (abfd, obj_datasec (abfd))) return false;
    }
  return true;
}

/* Transfer vectors for NEWS-OS version 3 */

/* We use BFD generic archive files.  */
#define	aout_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	aout_generic_stat_arch_elt	bfd_generic_stat_arch_elt
#define	aout_slurp_armap		bfd_slurp_bsd_armap
#define	aout_slurp_extended_name_table	bfd_true
#define	aout_write_armap		bsd_write_armap
#define	aout_truncate_arname		bfd_bsd_truncate_arname

/* We don't support core files yet.  FIXME.  */
#define	aout_core_file_failing_command	_bfd_dummy_core_file_failing_command
#define	aout_core_file_failing_signal	_bfd_dummy_core_file_failing_signal
#define	aout_core_file_matches_executable_p	\
				_bfd_dummy_core_file_matches_executable_p
#define	aout_core_file_p		_bfd_dummy_target

/* We define our own versions of these routines.  */


bfd_target newsos3_vec = /* Sony 68k-based machines running newos3 */
{
  "a.out-newos3",		/* name */
  bfd_target_aout_flavour_enum,
  true,				/* target byte order */
  true,				/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */

  _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* data */
  _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* hdrs */

  {_bfd_dummy_target, newsos3_object_p, /* bfd_check_format */
     bfd_generic_archive_p, aout_core_file_p},
  {bfd_false, aout_mkobject,		/* bfd_set_format */
     _bfd_generic_mkarchive, bfd_false},
  {bfd_false, newsos3_write_object_contents,	/* bfd_write_contents */
     _bfd_write_archive_contents, bfd_false},

  JUMP_TABLE(aout)
};
