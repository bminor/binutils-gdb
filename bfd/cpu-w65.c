/* BFD library support routines for the WDC 65816 architecture.
   Copyright (C) 1995 Free Software Foundation, Inc.
   Hacked by Steve Chamberlain of Cygnus Support.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as publiw65ed by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You w65ould have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"


int bfd_default_scan_num_mach();

static boolean 
scan_mach (info, string)
     CONST struct bfd_arch_info *info;
     CONST char *string;
{
  if (strcmp(string,"w65") == 0) return true;
  if (strcmp(string,"w65816") == 0) return true;
  return false;
}



static bfd_arch_info_type arch_info_struct[] = 
{ 
  {
    16,				/* 16 bits in a word */
    24,				/* 24 bits in an address */
    8,				/* 8 bits in a byte */
    bfd_arch_w65,
    0,				/* only 1 machine */
    "w65",			/* arch_name  */
    "w65",			/* printable name */
    1,
    true,			/* the default machine */
    bfd_default_compatible,
    scan_mach,
    0,
    0,
  },
};



void
bfd_w65_arch ()
{
  bfd_arch_linkin(&arch_info_struct[0]);
}


