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
#include "libcoff.h"
#include "m88k-bcs.h"


static reloc_howto_type howto_table[] = 
{
/* 	type 	rtshift size	bitsize	pc_rel	bitpos abs ovff sf  name partial inplace mask*/ 
	R_PCR16L, 2,	1, 	16, 	true,	0, false, true, 0,"PCR16L", false, 0x0000ffff,
	R_PCR26L, 2,	2,	26,	true,	0, false, true, 0,"PCR26L", false, 0x03ffffff,
	R_VRT16, 0,	1,	16,	false,	0, false, true, 0,"VRT16", false, 0x0000ffff,
	R_HVRT16,16,	1,	16,	false,	0, false, true, 0,"HVRT16", false, 0x0000ffff,
	R_LVRT16, 0,	1,	16,	false,	0, false, true, 0,"LVRT16", false, 0x0000ffff,
	R_VRT32,  0,	2,	32,	false,	0, false, true, 0,"VRT32", false, 0xffffffff,
};



#define BADMAG(x) MC88BADMAG(x)
#include "coff-code.h"



bfd_target m88k_bcs_vec =
{
  "m88k-bcs",		/* name */
  bfd_target_coff_flavour_enum,
  false,			/* data byte order is little */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  0,				/* valid reloc types */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  coff_close_and_cleanup,	/* _close_and_cleanup */
  coff_set_section_contents,	/* bfd_set_section_contents */
  coff_get_section_contents,	/* bfd_get_section_contents */
  coff_new_section_hook,	/* new_section_hook */
  _bfd_dummy_core_file_failing_command, /* _core_file_failing_command */
  _bfd_dummy_core_file_failing_signal, /* _core_file_failing_signal */
  _bfd_dummy_core_file_matches_executable_p, /* _core_file_matches_ex...p */

  bfd_slurp_coff_armap,		/* bfd_slurp_armap */
  _bfd_slurp_extended_name_table, /* bfd_slurp_extended_name_table*/
#if 0				/*  */
  bfd_dont_truncate_arname,	/* bfd_truncate_arname */
#else
  bfd_bsd_truncate_arname,
#endif

  coff_get_symtab_upper_bound,	/* get_symtab_upper_bound */
  coff_get_symtab,		/* canonicalize_symtab */
  (void (*)())bfd_false,	/* bfd_reclaim_symbol_table */
  coff_get_reloc_upper_bound,	/* get_reloc_upper_bound */
  coff_canonicalize_reloc,	/* bfd_canonicalize_reloc */
  (void (*)())bfd_false,	/* bfd_reclaim_reloc */

  coff_get_symcount_upper_bound, /* bfd_get_symcount_upper_bound */
  coff_get_first_symbol,	/* bfd_get_first_symbol */
  coff_get_next_symbol,		/* bfd_get_next_symbol */
  coff_classify_symbol,		/* bfd_classify_symbol */
  coff_symbol_hasclass,		/* bfd_symbol_hasclass */
  coff_symbol_name,		/* bfd_symbol_name */
  coff_symbol_value,		/* bfd_symbol_value */

  _do_getllong, _do_putllong, _do_getlshort, _do_putlshort, /* data */
  _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* hdrs */

  {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
     bfd_generic_archive_p, _bfd_dummy_target},
  {bfd_false, coff_mkobject, _bfd_generic_mkarchive, /* bfd_set_format */
     bfd_false},
  coff_make_empty_symbol,
  coff_print_symbol,
  coff_get_lineno,
  coff_set_arch_mach,
  coff_write_armap,
  bfd_generic_openr_next_archived_file,
    coff_find_nearest_line,
    bfd_generic_stat_arch_elt /* bfd_stat_arch_elt */
  };



