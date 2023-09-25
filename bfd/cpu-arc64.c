/* BFD support for the ARC64 processor
   Copyright (C) 2023 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

/* ARC64-specific "compatible" function.  The general rule is that if
   A and B are compatible, then this function should return
   architecture that is more "feature-rich", that is, can run both A
   and B.  */

static const bfd_arch_info_type *
compatible (const bfd_arch_info_type *a ATTRIBUTE_UNUSED,
	    const bfd_arch_info_type *b ATTRIBUTE_UNUSED)
{
  /* If a & b are for different architecture we can do nothing.  */
  if (a->arch != b->arch)
    return NULL;

  /* If a & b are for the same machine then all is well.  */
  if (a->mach == b->mach)
    return a;

  /* Otherwise if either a or b is the 'default' machine
     then it can be polymorphed into the other.  */
  if (a->the_default)
    return b;

  if (b->the_default)
    return a;

  return NULL;
}

#define ARC64(WORDSIZE, MACH, PRINT_NAME, DEFAULT_P, NEXT)	\
  {								\
    WORDSIZE,	/* 64/32 bits in a word.  */			\
    WORDSIZE,	/* 64/32 bits in an address.  */		\
    8,	/* 8 bits in a byte.  */				\
    bfd_arch_arc64,						\
    MACH,							\
    "arc64",							\
    PRINT_NAME,							\
    4, /* section alignment power.  */				\
    DEFAULT_P,							\
    compatible,							\
    bfd_default_scan,						\
    bfd_arch_default_fill,					\
    NEXT,							\
    0 /* Maximum offset of a reloc from the start of an insn.  */ \
  }

static const bfd_arch_info_type bfd_arc64_arch_32 =
  ARC64 (32, bfd_mach_arcv3_32, "arc64:32", false, NULL);

const bfd_arch_info_type bfd_arc64_arch =
  ARC64 (64, bfd_mach_arcv3_64, "arc64:64", true, &bfd_arc64_arch_32);
