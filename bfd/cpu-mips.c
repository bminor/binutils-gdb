/* bfd back-end for mips support
   Copyright (C) 1990, 91, 92, 93, 94, 95, 1996 Free Software Foundation, Inc.
   Written by Steve Chamberlain of Cygnus Support.

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

#define N(BITS_WORD, BITS_ADDR, NUMBER, PRINT, DEFAULT, NEXT)		\
  {							\
    BITS_WORD, /*  bits in a word */			\
    BITS_ADDR, /* bits in an address */			\
    8,	/* 8 bits in a byte */				\
    bfd_arch_mips,					\
    NUMBER,						\
    "mips",						\
    PRINT,						\
    3,							\
    DEFAULT,						\
    bfd_default_compatible, 				\
    bfd_default_scan,					\
    NEXT,						\
  }


#define A 0
#define B 0
#define C 0
#define D 0

static const bfd_arch_info_type arch_info_struct[] = 
{
  N (32, 32, bfd_mach_mips3000, "mips:3000", false, &arch_info_struct[1]),
  N (32, 32, bfd_mach_mips3900, "mips:3900", false, &arch_info_struct[2]),
  N (64, 64, bfd_mach_mips4000, "mips:4000", false, &arch_info_struct[3]),
  N (64, 64, bfd_mach_mips4010, "mips:4010", false, &arch_info_struct[4]),
  N (64, 64, bfd_mach_mips4100, "mips:4100", false, &arch_info_struct[5]),
  N (64, 64, bfd_mach_mips4300, "mips:4300", false, &arch_info_struct[6]),
  N (64, 64, bfd_mach_mips4400, "mips:4400", false, &arch_info_struct[7]),
  N (64, 64, bfd_mach_mips4600, "mips:4600", false, &arch_info_struct[8]),
  N (64, 64, bfd_mach_mips4650, "mips:4650", false, &arch_info_struct[9]),
  N (64, 64, bfd_mach_mips5000, "mips:5000", false, &arch_info_struct[10]),
  N (32, 32, bfd_mach_mips6000, "mips:6000", false, &arch_info_struct[11]),
  N (64, 64, bfd_mach_mips8000, "mips:8000", false, &arch_info_struct[12]),
  N (64, 64, bfd_mach_mips10000, "mips:10000", false, &arch_info_struct[13]),

  /* start-sanitize-tx19
#undef A
#define A 1
  N (32, 32, bfd_mach_mips1900, "mips:1900", false, &arch_info_struct[13+A]),
  /* end-sanitize-tx19 */
  /* start-sanitize-tx49 */
#undef B
#define B 1
  N (64, 64, bfd_mach_mips4900, "mips:4900", false, &arch_info_struct[13+A+B]),
  /* end-sanitize-tx49 */
  /* start-sanitize-vr5400 */
#undef C
#define C 1
  N (64, 64, bfd_mach_mips5400, "mips:5400", false, &arch_info_struct[13+A+B+C]),
  /* end-sanitize-vr5400 */
  /* start-sanitize-r5900 */
#undef D
#define D 1
  N (64, 64, bfd_mach_mips5900, "mips:5900", false, &arch_info_struct[13+A+B+C+D]),
  /* end-sanitize-r5900 */
  N (64, 64, bfd_mach_mips16,   "mips:16",   false, 0),
};

#undef A
#undef B
#undef C
#undef D

/* The default architecture is mips:3000, but with a machine number of
   zero.  This lets the linker distinguish between a default setting
   of mips, and an explicit setting of mips:3000.  */

const bfd_arch_info_type bfd_mips_arch =
N (32, 32, 0, "mips", true, &arch_info_struct[0]);
