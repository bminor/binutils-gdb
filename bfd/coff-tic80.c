/* BFD back-end for Texas Instruments TMS320C80 Multimedia Video Processor (MVP).
   Copyright 1996 Free Software Foundation, Inc.

   Written by Fred Fish (fnf@cygnus.com)

   There is nothing new under the sun. This file draws a lot on other
   coff files.

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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "obstack.h"
#include "coff/tic80.h"
#include "coff/internal.h"
#include "libcoff.h"

#define COFF_DEFAULT_SECTION_ALIGNMENT_POWER (4)	/* FIXME: What should this be? */

#define coff_relocate_section _bfd_coff_generic_relocate_section

#define RTYPE2HOWTO(internal, relocentry) abort()	/* FIXME: hack */

#ifndef BADMAG
#define BADMAG(x) TIC80BADMAG(x)
#endif

#define TIC80 1		/* Customize coffcode.h */
#include "coffcode.h"

const bfd_target 
#ifdef TARGET_SYM
  TARGET_SYM =
#else
  tic80coff_vec =
#endif
{
#ifdef TARGET_NAME
  TARGET_NAME,
#else
  "coff-tic80",			/* name */
#endif
  bfd_target_coff_flavour,
  BFD_ENDIAN_LITTLE,		/* data byte order is little (arch supports both) */
  BFD_ENDIAN_LITTLE,		/* header byte order is little */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT | D_PAGED),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
#ifdef NAMES_HAVE_UNDERSCORE
  '_',
#else
  0,				/* leading underscore */
#endif
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* data */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
     bfd_getl32, bfd_getl_signed_32, bfd_putl32,
     bfd_getl16, bfd_getl_signed_16, bfd_putl16, /* hdrs */

 {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
   bfd_generic_archive_p, _bfd_dummy_target},
 {bfd_false, coff_mkobject, _bfd_generic_mkarchive, /* bfd_set_format */
   bfd_false},
 {bfd_false, coff_write_object_contents, /* bfd_write_contents */
   _bfd_write_archive_contents, bfd_false},

     BFD_JUMP_TABLE_GENERIC (coff),
     BFD_JUMP_TABLE_COPY (coff),
     BFD_JUMP_TABLE_CORE (_bfd_nocore),
     BFD_JUMP_TABLE_ARCHIVE (_bfd_archive_coff),
     BFD_JUMP_TABLE_SYMBOLS (coff),
     BFD_JUMP_TABLE_RELOCS (coff),
     BFD_JUMP_TABLE_WRITE (coff),
     BFD_JUMP_TABLE_LINK (coff),
     BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  COFF_SWAP_TABLE
 };
