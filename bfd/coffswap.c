/* Byte-swapping routines for COFF files */

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify it under the
   terms of the GNU General Public License as published by the Free Software
   Foundation; either version 1, or (at your option) any later version.

BFD is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
   details.

You should have received a copy of the GNU General Public License along with
   BFD; see the file COPYING.  If not, write to the Free Software Foundation,
   675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Most of this hacked by Steve Chamberlain, steve@cygnus.com */

#include <ansidecl.h>
#include "intel-coff.h"
#include "bfd.h"
#include "libcoff.h"		/* to allow easier abstraction-breaking */

#define sp(x) bfd_h_put_x(abfd, x, &x)

/* All the swapping routines:

   FIXME, these routines assume that the sizes, alignments, and offsets of
   these fields are the same in the host and target.  This is an invalid
   assumption, which in particular breaks on the 386 and SPARC.  Fix this
   the same way that a.out.h and sunos.c were fixed:  define char arrays
   that represent the headers in the target systems' file, and have these
   functions translate in from that format, and out to that format.  */

void 
DEFUN(bfd_coff_swap_name,(abfd, ptr),
      bfd            *abfd AND
      long           *ptr)
{
    if (ptr[0] == 0) {
	/* There is an index which needs to be swapped */
	bfd_h_put_x(abfd, ptr[1], (ptr + 1));
    }
    else {
	/* This is a string .. leave it alone */
    }
}

void 
DEFUN(bfd_coff_swap_sym,(abfd, se),
      bfd            *abfd AND
      SYMENT         *se)
{
    bfd_coff_swap_name(abfd, (long*)(se->n_name));
    bfd_h_put_x(abfd, se->n_value, &se->n_value);
    bfd_h_put_x(abfd, se->n_scnum, &se->n_scnum);
    bfd_h_put_x(abfd, se->n_type, &se->n_type);
    bfd_h_put_x(abfd, se->n_sclass, &se->n_sclass);
    bfd_h_put_x(abfd, se->n_numaux, &se->n_numaux);
}

void
DEFUN(bfd_coff_swap_aux,(abfd, au, type, class),
      bfd            *abfd AND
      AUXENT         *au AND
      int             type AND
      int             class)
{
    switch (class) {
    case C_FILE:
	bfd_coff_swap_name(abfd, (long *)(&au->x_file.x_n));
	break;
    case C_STAT:
#ifdef C_LEAFSTAT
    case C_LEAFSTAT:
#endif
    case C_HIDDEN:
	if (type == T_NULL) {
	    sp(au->x_scn.x_scnlen);
	    sp(au->x_scn.x_nreloc);
	    sp(au->x_scn.x_nlinno);
	    break;
	}
    default:
	sp(au->x_sym.x_tagndx);
	sp(au->x_sym.x_tvndx);

	if (ISARY(type) || class == C_BLOCK) {
	    sp(au->x_sym.x_fcnary.x_ary.x_dimen[0]);
	    sp(au->x_sym.x_fcnary.x_ary.x_dimen[1]);
	    sp(au->x_sym.x_fcnary.x_ary.x_dimen[2]);
	    sp(au->x_sym.x_fcnary.x_ary.x_dimen[3]);
	}
	else {
	    sp(au->x_sym.x_fcnary.x_fcn.x_lnnoptr);
	    sp(au->x_sym.x_fcnary.x_fcn.x_endndx);
	}
	if (ISFCN(type)) {
	    sp(au->x_sym.x_misc.x_fsize);
	}
	else {
	    sp(au->x_sym.x_misc.x_lnsz.x_lnno);
	    sp(au->x_sym.x_misc.x_lnsz.x_size);
	}
    }
}

void
DEFUN(bfd_coff_swap_lineno,(abfd, lineno),
      bfd            *abfd AND
      LINENO         *lineno)
{
    sp(lineno->l_addr.l_symndx);
    sp(lineno->l_lnno);
}


