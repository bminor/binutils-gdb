/* BFD back-end for Apple et al PowerPC Mac "XCOFF" files.
   Copyright 1990, 1991, 1992, 1993, 1994, 1995 Free Software Foundation, Inc.
   FIXME: Can someone provide a transliteration of this name into ASCII?
   Using the following chars caused a compiler warning on HIUX (so I replaced
   them with octal escapes), and isn't useful without an understanding of what
   character set it is.
   Written by Metin G. Ozisik, Mimi Ph\373\364ng-Th\345o V\365, 
     and John Gilmore.
   Contributed by IBM Corporation and Cygnus Support.

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

/* (eventually lose this) */
/* coffcode.h modifies itself based on this flag.  */
#define RS6000COFF_C 1

/* Tweak coffcode.h based on this being a PowerMac instead of RS/6000. */

#define POWERMAC

#include "coff-rs6000.c"


const bfd_target pmac_xcoff_vec =
{
  "xcoff-powermac",		/* name */
  bfd_target_coff_flavour,	
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG | DYNAMIC |
   HAS_SYMS | HAS_LOCALS | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  0,				/* leading char */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen??? FIXMEmgo */

  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* hdrs */

  {_bfd_dummy_target, coff_object_p, 	/* bfd_check_format */
     xcoff_archive_p, _bfd_dummy_target},
  {bfd_false, coff_mkobject, _bfd_generic_mkarchive, /* bfd_set_format */
     bfd_false},
  {bfd_false, coff_write_object_contents,	/* bfd_write_contents */
     xcoff_write_archive_contents, bfd_false},

     BFD_JUMP_TABLE_GENERIC (coff),
     BFD_JUMP_TABLE_COPY (coff),
     BFD_JUMP_TABLE_CORE (_bfd_nocore),
     BFD_JUMP_TABLE_ARCHIVE (xcoff),
     BFD_JUMP_TABLE_SYMBOLS (coff),
     BFD_JUMP_TABLE_RELOCS (coff),
     BFD_JUMP_TABLE_WRITE (coff),
     BFD_JUMP_TABLE_LINK (_bfd_xcoff),
     BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  COFF_SWAP_TABLE,
};
