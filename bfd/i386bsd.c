/* BFD back-end for i386 a.out binaries under 386BSD.
   Copyright (C) 1990, 1991 Free Software Foundation, Inc.

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


/* This data should be correct (or made correct) for the format used
   under 386BSD (from Bill and Lynne Jolitz).  It may be good for BSD/386
   (from BSDI) as well; I haven't had a chance to test it.  */

#define TARGET_IS_LITTLE_ENDIAN_P
#define N_HEADER_IN_TEXT(x) 0
#define BYTES_IN_WORD 4
#define ARCH 32

#define N_TXTOFF(x)	(N_MAGIC(x) == ZMAGIC ? 0x1000 : 0x20)
#define N_TXTADDR(x)	0

#define N_TXTSIZE(x) ((x).a_text)
#define PAGE_SIZE 4096
#define SEGMENT_SIZE PAGE_SIZE
#define DEFAULT_ARCH bfd_arch_i386

#define MY(OP) CAT(i386bsd_,OP)
#define TARGETNAME "a.out-i386-bsd"

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libaout.h"

#include "aout-target.h"
