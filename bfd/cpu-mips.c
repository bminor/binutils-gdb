/* bfd back-end for mips support
   Copyright (C) 1990, 91-97, 1998 Free Software Foundation, Inc.
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

enum {
I_mips3000, 
I_mips3900, 
I_mips4000, 
I_mips4010, 
I_mips4100, 
  /* start-sanitize-vr4xxx */
I_mips4121,
  /* end-sanitize-vr4xxx */
I_mips4300, 
I_mips4400, 
I_mips4600, 
I_mips4650, 
I_mips5000, 
I_mips6000, 
I_mips8000, 
I_mips10000,
  /* start-sanitize-tx19 */
I_mips1900,
  /* end-sanitize-tx19 */
  /* start-sanitize-vr4320 */
I_mips4320,
  /* end-sanitize-vr4320 */
  /* start-sanitize-tx49 */
I_mips4900,
  /* end-sanitize-tx49 */
  /* start-sanitize-cygnus */
I_mips5400,
  /* end-sanitize-cygnus */
  /* start-sanitize-r5900 */
I_mips5900,
  /* end-sanitize-r5900 */
  /* start-sanitize-sky */
I_dvp_dma,
I_dvp_vif,
I_dvp_vu,
I_dvp_gif,
  /* end-sanitize-sky */
I_mips16
};


#define NN(index) (&arch_info_struct[(index)+1])

static const bfd_arch_info_type arch_info_struct[] = 
{
  N (32, 32, bfd_mach_mips3000, "mips:3000", false, NN(I_mips3000)),
  N (32, 32, bfd_mach_mips3900, "mips:3900", false, NN(I_mips3900)),
  N (64, 64, bfd_mach_mips4000, "mips:4000", false, NN(I_mips4000)),
  N (64, 64, bfd_mach_mips4010, "mips:4010", false, NN(I_mips4010)),
  N (64, 64, bfd_mach_mips4100, "mips:4100", false, NN(I_mips4100)),
  /* start-sanitize-vr4xxx */
  N (64, 64, bfd_mach_mips4121, "mips:4121", false, NN(I_mips4121)),
  /* end-sanitize-vr4xxx */
  N (64, 64, bfd_mach_mips4300, "mips:4300", false, NN(I_mips4300)),
  N (64, 64, bfd_mach_mips4400, "mips:4400", false, NN(I_mips4400)),
  N (64, 64, bfd_mach_mips4600, "mips:4600", false, NN(I_mips4600)),
  N (64, 64, bfd_mach_mips4650, "mips:4650", false, NN(I_mips4650)),
  N (64, 64, bfd_mach_mips5000, "mips:5000", false, NN(I_mips5000)),
  N (32, 32, bfd_mach_mips6000, "mips:6000", false, NN(I_mips6000)),
  N (64, 64, bfd_mach_mips8000, "mips:8000", false, NN(I_mips8000)),
  N (64, 64, bfd_mach_mips10000, "mips:10000", false, NN(I_mips10000)),

  /* start-sanitize-tx19 */
  N (32, 32, bfd_mach_mips1900, "mips:1900", false, NN(I_mips1900)),
  /* end-sanitize-tx19 */
  /* start-sanitize-vr4320 */
  N (64, 64, bfd_mach_mips4320, "mips:4320", false, NN(I_mips4320)),
  /* end-sanitize-vr4320 */
  /* start-sanitize-tx49 */
  N (64, 64, bfd_mach_mips4900, "mips:4900", false, NN(I_mips4900)),
  /* end-sanitize-tx49 */
  /* start-sanitize-cygnus */
  N (64, 64, bfd_mach_mips5400, "mips:5400", false, NN(I_mips5400)),
  /* end-sanitize-cygnus */
  /* start-sanitize-r5900 */
  N (64, 32, bfd_mach_mips5900, "mips:5900", false, NN(I_mips5900)),
  /* end-sanitize-r5900 */
  /* start-sanitize-sky */
  N (32, 32, bfd_mach_dvp_dma,  "dvp:dma",   false, NN(I_dvp_dma)),
  N (32, 32, bfd_mach_dvp_vif,  "dvp:vif",   false, NN(I_dvp_vif)),
  N (32, 32, bfd_mach_dvp_vu,   "dvp:vu",    false, NN(I_dvp_vu)),
  N (32, 32, bfd_mach_dvp_gif,  "dvp:gif",   false, NN(I_dvp_gif)),
  /* end-sanitize-sky */

  N (64, 64, bfd_mach_mips16,   "mips:16",   false, 0),
};

/* The default architecture is mips:3000, but with a machine number of
   zero.  This lets the linker distinguish between a default setting
   of mips, and an explicit setting of mips:3000.  */

const bfd_arch_info_type bfd_mips_arch =
N (32, 32, 0, "mips", true, &arch_info_struct[0]);
