/* Intel 960 COFF support for BFD.  */

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

#define I960 1
#define BADMAG(x) I960BADMAG(x)

#include <ansidecl.h>
#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "obstack.h"
#include "intel-coff.h"
#include "libcoff.h"		/* to allow easier abstraction-breaking */


#define CALLS	 0x66003800	/* Template for 'calls' instruction	*/
#define BAL	 0x0b000000	/* Template for 'bal' instruction	*/
#define BAL_MASK 0x00ffffff

static bfd_reloc_status_enum_type 
optcall_callback(abfd, reloc_entry, symbol_in, data, ignore_input_section)
bfd *abfd;
arelent *reloc_entry;
asymbol *symbol_in;
unsigned char *data;
asection *ignore_input_section;
{
  /* This item has already been relocated correctly, but we may be
   * able to patch in yet better code - done by digging out the
   * correct info on this symbol */
  bfd_reloc_status_enum_type result;
  coff_symbol_type *cs = coffsymbol(symbol_in);

  /* So the target symbol has to be of coff type, and the symbol 
     has to have the correct native information within it */
  if ((cs->symbol.the_bfd->xvec->flavour != bfd_target_coff_flavour_enum)
      || (cs->native == (struct internal_syment *)NULL)) {
     /* This is interesting, consider the case where we're outputting */
     /* coff from a mix n match input, linking from coff to a symbol */
     /* defined in a bout file will cause this match to be true. Should */
     /* I complain ? - This will only work if the bout symbol is non */
     /* leaf. */
     result = bfd_reloc_dangerous;
  }
  else  {
    switch (bfd_h_get_x(abfd, & cs->native->n_sclass)) 
      {
      case C_LEAFSTAT:
      case C_LEAFEXT:
  	/* This is a call to a leaf procedure, replace instruction with a bal
	 to the correct location */
	{
	 AUXENT *aux = (AUXENT *)(cs->native+2);
	  int word = bfd_getlong(abfd, data + reloc_entry->address);
	  BFD_ASSERT(bfd_h_get_x(abfd, &cs->native->n_numaux)==2);
	  /* We replace the original call instruction with a bal to */
	  /* the bal entry point - the offset of which is described in the */
	  /* 2nd auxent of the original symbol. We keep the native sym and */
	  /* auxents untouched, so the delta between the two is the */
	  /* offset of the bal entry point */
	  word = ((word + (bfd_h_get_x(abfd, &aux->x_bal.x_balntry) -
			   bfd_h_get_x(abfd, &cs->native->n_value)))
		  & BAL_MASK) | BAL;
  	  bfd_putlong(abfd, word,  data+reloc_entry->address);
  	}
	result = bfd_reloc_ok;
	break;
      case C_SCALL:
	{
	  /* This is a call to a system call, replace with a calls to # */
	  BFD_ASSERT(0);
	}
	break;
      default:
	result = bfd_reloc_ok;
	break;
      }
  }
  return result;
}



static reloc_howto_type howto_table[] = 
{
  {0},
  {1},
  {2},
  {3},
  {4},
  {5},
  {6},
  {7},
  {8},
  {9},
  {10},
  {11},
  {12},
  {13},
  {14},
  {15},
  {16},

  { (unsigned int) R_RELLONG, 0, 2, 32,false, 0, true, true,
      0,"rellong", true, 0xffffffff, 0xffffffff},
  {18},
  {19},
  {20},
  {21},
  {22},
  {23},
  {24},

  {  R_IPRMED, 0, 2, 24,true,0, true, true,0,"iprmed ", true,
       0x00ffffff, 0x00ffffff},
  {26},
 
  {  R_OPTCALL, 0,2,24,true,0, true, true, optcall_callback,
       "optcall", true, 0x00ffffff, 0x00ffffff},

};

/* The real code is in coff-code.h */

#include "coff-code.h"

bfd_target icoff_little_vec =
{
  "coff-Intel-little",		/* name */
  bfd_target_coff_flavour_enum,
  false,			/* data byte order is little */
  false,			/* header byte order is little */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */

  _do_getllong, _do_putllong, _do_getlshort, _do_putlshort, /* data */
  _do_getllong, _do_putllong, _do_getlshort, _do_putlshort, /* hdrs */

  {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
     bfd_generic_archive_p, _bfd_dummy_target},
  {bfd_false, coff_mkobject,	/* bfd_set_format */
     _bfd_generic_mkarchive, bfd_false},

  JUMP_TABLE(coff)
};




bfd_target icoff_big_vec =
{
  "coff-Intel-big",		/* name */
  bfd_target_coff_flavour_enum,
  false,			/* data byte order is little */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */

  _do_getllong, _do_putllong, _do_getlshort, _do_putlshort, /* data */
  _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* hdrs */

  {_bfd_dummy_target, coff_object_p,  bfd_generic_archive_p, _bfd_dummy_target},
  {bfd_false, coff_mkobject, _bfd_generic_mkarchive,     bfd_false},

  JUMP_TABLE(coff)
};

