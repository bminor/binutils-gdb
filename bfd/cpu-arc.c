/* BFD support for the ARC processor
   Copyright 1994, 1995 Free Software Foundation, Inc.
   Contributed by Doug Evans (dje@cygnus.com).

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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#define ARC(mach, print_name, default_p) \
{					\
    32,	/* 32 bits in a word */		\
    32,	/* 32 bits in an address */	\
    8,	/* 8 bits in a byte */		\
    bfd_arch_arc,			\
    mach,				\
    "arc",				\
    print_name,				\
    4, /* section alignment power */	\
    default_p,				\
    bfd_default_compatible, 		\
    bfd_default_scan,			\
    0,					\
  }

static bfd_arch_info_type arch_info_struct[] =
{
  ARC (bfd_mach_arc_base, "arc-base", true),
  ARC (bfd_mach_arc_host, "arc-host", false),
  ARC (bfd_mach_arc_graphics, "arc-graphics", false),
  ARC (bfd_mach_arc_audio, "arc-audio", false),
};

void
bfd_arc_arch ()
{
  register unsigned int i;

  for (i = 0; i < sizeof (arch_info_struct) / sizeof (arch_info_struct[0]); i++)
    bfd_arch_linkin (&arch_info_struct[i]);
}

/* Utility routines.  */

/* Given cpu type NAME, return its bfd_mach_arc_xxx value.
   NAME is one of "base, audio, etc.".
   Returns -1 if not found.  */

int
arc_get_mach (name)
     char *name;
{
  register unsigned int i;

  for (i = 0; i < sizeof (arch_info_struct) / sizeof (arch_info_struct[0]); i++)
    {
      /* +4: skip over "arc-" */
      if (strcmp (name, arch_info_struct[i].printable_name + 4) == 0)
	return arch_info_struct[i].mach;
    }
  return -1;
}
