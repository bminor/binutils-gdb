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


/* $Id$ */

#define M88 1
#include <ansidecl.h>
#include <sysdep.h>
#include "bfd.h"
#include "libbfd.h"
#include "obstack.h"
#include "m88k-bcs.h"
#include "libcoff.h"



static reloc_howto_type howto_table[] = 
{
/* 	type 	rtshift size	bitsize	pc_rel	bitpos abs ovff sf  name partial inplace mask*/ 
	R_PCR16L, 2,	1, 	16, 	true,	0, false, true, 0,"PCR16L", false, 0x0000ffff,0x0000ffff,
	R_PCR26L, 2,	2,	26,	true,	0, false, true, 0,"PCR26L", false, 0x03ffffff,0x03ffffff,
	R_VRT16, 0,	1,	16,	false,	0, false, true, 0,"VRT16", false, 0x0000ffff,0x0000ffff,
	R_HVRT16,16,	1,	16,	false,	0, false, true, 0,"HVRT16", false, 0x0000ffff,0x0000ffff,
	R_LVRT16, 0,	1,	16,	false,	0, false, true, 0,"LVRT16", false, 0x0000ffff,0x0000ffff,
	R_VRT32,  0,	2,	32,	false,	0, false, true, 0,"VRT32", false, 0xffffffff,0xffffffff,
};



#define BADMAG(x) MC88BADMAG(x)
#include "coff-code.h"




#define coff_write_armap bsd_write_armap


bfd_target m88k_bcs_vec =
{
  "m88kbcs",		/* name */
  bfd_target_coff_flavour_enum,
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */

  _do_getllong, _do_putllong, _do_getlshort, _do_putlshort, /* data */
  _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* hdrs */

  {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
     bfd_generic_archive_p, _bfd_dummy_target},
  {bfd_false, coff_mkobject, _bfd_generic_mkarchive, /* bfd_set_format */
     bfd_false},

     JUMP_TABLE(coff)

  };



