/* BFD library support routines for the Experiment SOP architecture.
   Copyright (C) 1993 Free Software Foundation, Inc.

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


int bfd_default_scan_num_mach();

static boolean 
DEFUN(scan_mach,(info, string),
CONST struct bfd_arch_info *info AND
CONST char *string)
{
  if (strcmp(string,"sop") == 0) return true;
  if (strcmp(string,"SOP") == 0) return true;
  return false;
}


#if 0
/* This routine is provided two arch_infos and returns whether
   they'd be compatible */

static CONST bfd_arch_info_type *
DEFUN(compatible,(a,b),
      CONST bfd_arch_info_type *a AND
      CONST bfd_arch_info_type *b)
{
  if (a->arch != b->arch || a->mach != b->mach)
   return NULL;
  return a;
}
#endif

static bfd_arch_info_type arch_info_struct[] = 
{ 
  {
    32,				/* 32 bits in a word */
    32,				/* 32 bits in an address */
    8,				/* 8 bits in a byte */
    bfd_arch_rce,
    0,				/* only 1 machine */
    "SOP",			/* arch_name  */
    "SOP",			/* printable name */
    1,
    true,			/* the default machine */
    bfd_default_compatible,
    scan_mach,
    0,
    0,
  },
};



void
DEFUN_VOID(bfd_sop_arch)
{
  bfd_arch_linkin(&arch_info_struct[0]);
}

