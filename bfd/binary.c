/* BFD back-end for binary objects.
   Copyright 1994 Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Cygnus Support, <ian@cygnus.com>

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

/* This is a BFD backend which may be used to write binary objects.
   It may only be used for output, not input.  The intention is that
   this may be used as an output format for objcopy in order to
   generate raw binary data.

   This is very simple.  The only complication is that the real data
   will start at some address X, and in some cases we will not want to
   include X zeroes just to get to that point.  Since the start
   address is not meaningful for this object file format, we use it
   instead to indicate the number of zeroes to skip at the start of
   the file.  objcopy cooperates by specially setting the start
   address to zero by default.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

static boolean binary_mkobject PARAMS ((bfd *));
static asymbol *binary_make_empty_symbol PARAMS ((bfd *));
static boolean binary_set_section_contents
  PARAMS ((bfd *, asection *, PTR, file_ptr, bfd_size_type));

/* Create a binary object.  Invoked via bfd_set_format.  */

static boolean
binary_mkobject (abfd)
     bfd *abfd;
{
  return true;
}

/* Most of the symbol routines can just return an error.  */
#define binary_get_symtab_upper_bound _bfd_nosymbols_get_symtab_upper_bound
#define binary_get_symtab _bfd_nosymbols_get_symtab
#define binary_print_symbol _bfd_nosymbols_print_symbol
#define binary_get_symbol_info _bfd_nosymbols_get_symbol_info
#define binary_bfd_is_local_label _bfd_nosymbols_bfd_is_local_label
#define binary_get_lineno _bfd_nosymbols_get_lineno
#define binary_find_nearest_line _bfd_nosymbols_find_nearest_line
#define binary_bfd_make_debug_symbol _bfd_nosymbols_bfd_make_debug_symbol

/* We do have to provide a routine to make an empty symbol.  */

static asymbol *
binary_make_empty_symbol (abfd)
     bfd *abfd;
{
  asymbol *ret;

  ret = (asymbol *) bfd_alloc (abfd, sizeof (asymbol));
  if (ret == NULL)
    bfd_set_error (bfd_error_no_memory);
  return ret;
}

/* Set the architecture of a binary file.  */
#define binary_set_arch_mach _bfd_generic_set_arch_mach

/* Write section contents of a binary file.  */

static boolean
binary_set_section_contents (abfd, sec, data, offset, size)
     bfd *abfd;
     asection *sec;
     PTR data;
     file_ptr offset;
     bfd_size_type size;
{
  /* In a binary file, the file position of a section is just the VMA
     minus the start address.  */
  sec->filepos = bfd_section_vma (abfd, sec) - bfd_get_start_address (abfd);

  if (sec->filepos + offset < 0)
    {
      file_ptr adjust;

      adjust = - (sec->filepos + offset);
      if (size <= adjust)
	return true;
      size -= adjust;
      data = (PTR) ((bfd_byte *) data + adjust);
      offset += adjust;
    }

  return _bfd_generic_set_section_contents (abfd, sec, data, offset, size);
}

const bfd_target binary_vec =
{
  "binary",			/* name */
  bfd_target_unknown_flavour,	/* flavour */
  true,				/* byteorder_big_p */
  true,				/* header_byteorder_big_p */
  EXEC_P,			/* object_flags */
  (SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_CODE | SEC_DATA
   | SEC_ROM | SEC_HAS_CONTENTS), /* section_flags */
  0,				/* symbol_leading_char */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  1,				/* align_power_min */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* hdrs */
  {				/* bfd_check_format */
    _bfd_dummy_target,
    _bfd_dummy_target,
    _bfd_dummy_target,
    _bfd_dummy_target,
  },
  {				/* bfd_set_format */
    bfd_false,
    binary_mkobject,
    bfd_false,
    bfd_false,
  },
  {				/* bfd_write_contents */
    bfd_false,
    bfd_true,
    bfd_false,
    bfd_false,
  },

  BFD_JUMP_TABLE_GENERIC (_bfd_generic),
  BFD_JUMP_TABLE_COPY (_bfd_generic),
  BFD_JUMP_TABLE_CORE (_bfd_nocore),
  BFD_JUMP_TABLE_ARCHIVE (_bfd_noarchive),
  BFD_JUMP_TABLE_SYMBOLS (binary),
  BFD_JUMP_TABLE_RELOCS (_bfd_norelocs),
  BFD_JUMP_TABLE_WRITE (binary),
  BFD_JUMP_TABLE_LINK (_bfd_nolink),
  BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  NULL
};
