/* BFD back-end for Motorola 68000 COFF binaries.
   Copyright 1990, 1991, 1992, 1993 Free Software Foundation, Inc.
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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "obstack.h"
#include "coff/m68k.h"
#include "coff/internal.h"
#include "libcoff.h"

#ifdef ONLY_DECLARE_RELOCS
extern reloc_howto_type m68kcoff_howto_table[];
#else
reloc_howto_type m68kcoff_howto_table[] = 
{
  HOWTO(R_RELBYTE,	       0,  0,  	8,  false, 0, true,  true,0,"8",	true, 0x000000ff,0x000000ff, false),
  HOWTO(R_RELWORD,	       0,  1, 	16, false, 0, true,  true,0,"16",	true, 0x0000ffff,0x0000ffff, false),
  HOWTO(R_RELLONG,	       0,  2, 	32, false, 0, true,  true,0,"32",	true, 0xffffffff,0xffffffff, false),
  HOWTO(R_PCRBYTE,	       0,  0, 	8,  true,  0, false, true,0,"DISP8",    true, 0x000000ff,0x000000ff, false),
  HOWTO(R_PCRWORD,	       0,  1, 	16, true,  0, false, true,0,"DISP16",   true, 0x0000ffff,0x0000ffff, false),
  HOWTO(R_PCRLONG,	       0,  2, 	32, true,  0, false, true,0,"DISP32",   true, 0xffffffff,0xffffffff, false),
  HOWTO(R_RELLONG_NEG,	       0,  -2, 	32, false, 0, true,  true,0,"-32",	true, 0xffffffff,0xffffffff, false),
};
#endif /* not ONLY_DECLARE_RELOCS */

#ifndef BADMAG
#define BADMAG(x) M68KBADMAG(x)
#endif
#define M68 1		/* Customize coffcode.h */

/* Turn a howto into a reloc number */

#ifdef ONLY_DECLARE_RELOCS
extern void m68k_rtype2howto PARAMS ((arelent *internal, int relocentry));
extern int m68k_howto2rtype PARAMS ((struct reloc_howto_struct *));
#else
void
m68k_rtype2howto(internal, relocentry)
     arelent *internal;
     int relocentry;
{
  switch (relocentry) 
  {
   case R_RELBYTE:	internal->howto = m68kcoff_howto_table + 0; break;
   case R_RELWORD:	internal->howto = m68kcoff_howto_table + 1; break;
   case R_RELLONG:	internal->howto = m68kcoff_howto_table + 2; break;
   case R_PCRBYTE:	internal->howto = m68kcoff_howto_table + 3; break;
   case R_PCRWORD:	internal->howto = m68kcoff_howto_table + 4; break;
   case R_PCRLONG:	internal->howto = m68kcoff_howto_table + 5; break;
   case R_RELLONG_NEG:	internal->howto = m68kcoff_howto_table + 6; break;
  }
}

int 
m68k_howto2rtype (internal)
     struct reloc_howto_struct *internal;
{
  if (internal->pc_relative) 
  {
    switch (internal->bitsize) 
    {
     case 32: return R_PCRLONG;
     case 16: return R_PCRWORD;
     case 8: return R_PCRBYTE;
    }
  }
  else 
  {
    switch (internal->bitsize) 
     {
      case 32: return R_RELLONG;
      case 16: return R_RELWORD;
      case 8: return R_RELBYTE;
     }
  }
  return R_RELLONG;    
}
#endif /* not ONLY_DECLARE_RELOCS */

#define RTYPE2HOWTO(internal, relocentry) \
  m68k_rtype2howto(internal, (relocentry)->r_type)

#define SELECT_RELOC(external, internal) \
  external = m68k_howto2rtype(internal);

#include "coffcode.h"

bfd_target 
#ifdef TARGET_SYM
  TARGET_SYM =
#else
  m68kcoff_vec =
#endif
{
#ifdef TARGET_NAME
  TARGET_NAME,
#else
  "coff-m68k",			/* name */
#endif
  bfd_target_coff_flavour,
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
#ifdef NAMES_HAVE_UNDERSCORE
  '_',
#else
  0,				/* leading underscore */
#endif
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  1,				/* minimum section alignment */
  _do_getb64, _do_getb_signed_64, _do_putb64,
     _do_getb32, _do_getb_signed_32, _do_putb32,
     _do_getb16, _do_getb_signed_16, _do_putb16, /* data */
  _do_getb64, _do_getb_signed_64, _do_putb64,
     _do_getb32, _do_getb_signed_32, _do_putb32,
     _do_getb16, _do_getb_signed_16, _do_putb16, /* hdrs */

 {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
   bfd_generic_archive_p, _bfd_dummy_target},
 {bfd_false, coff_mkobject, _bfd_generic_mkarchive, /* bfd_set_format */
   bfd_false},
 {bfd_false, coff_write_object_contents, /* bfd_write_contents */
   _bfd_write_archive_contents, bfd_false},

  JUMP_TABLE(coff),
  COFF_SWAP_TABLE
 };
