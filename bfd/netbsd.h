/* BFD back-end definitions used by all NetBSD targets.
   Copyright (C) 1990, 1991, 1992 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* NetBSD fits its header into the start of its text segment */
#define	N_HEADER_IN_TEXT(x)	1
#define TEXT_START_ADDR		PAGE_SIZE

#define N_MAGIC(ex) \
    ( (((ex).a_info)&0xffff0000) ? (((ex).a_info)&0xffff) : ((ex).a_info) )
#define N_MACHTYPE(ex) \
    ( (((ex).a_info)&0xffff0000) ? ((((ex).a_info)>>16)&0x03ff) : 0 )
# define N_FLAGS(ex) \
    ( (((ex).a_info)&0xffff0000) ? ((((ex).a_info)>>26)&0x3f) : 0 )
#define N_SET_INFO(ex, mag,mid,flag) \
    ( (ex).a_info = htonl( (((flag)&0x3f)<<26) | (((mid)&0x03ff)<<16) | \
    (((mag)&0xffff)) ) )
#define N_SET_MAGIC(exec,magic) \
  ((exec).a_info = (((exec).a_info & ~0xffff) | ((magic) & 0xffff)))
#define N_SET_MACHTYPE(exec,machtype) \
  ((exec).a_info = \
   (((exec).a_info & ~(0x3ff<<16)) | (((machtype)&0xff) << 16)))
#define N_SET_FLAGS(exec, flags) \
  ((exec).a_info = ((exec).a_info & 0xffff) | (flags & 0xffff))

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libaout.h"

#define N_GETMAGIC(ex) \
    ( (((ex).a_info)&0xffff0000) ? (((ex).a_info)&0xffff) : ((ex).a_info))
#define N_GETMAGIC2(ex) \
    ( (((ex).a_info)&0xffff0000) ? (((ex).a_info)&0xffff) : \
    (((ex).a_info) | 0x10000) )

#define N_TXTADDR(ex)	(N_GETMAGIC2(ex) == (ZMAGIC|0x10000) ? 0 : __LDPGSZ)
#define N_TXTOFF(ex) \
	( N_GETMAGIC2(ex)==ZMAGIC || N_GETMAGIC2(ex)==(QMAGIC|0x10000) ? \
	0 : (N_GETMAGIC2(ex)==(ZMAGIC|0x10000) ? __LDPGSZ : EXEC_BYTES_SIZE ))
#define N_ALIGN(ex,x) \
	(N_MAGIC(ex) == ZMAGIC || N_MAGIC(ex) == QMAGIC ? \
	 ((x) + __LDPGSZ - 1) & ~(__LDPGSZ - 1) : (x))
#define	N_DATADDR(ex) \
	(N_GETMAGIC(ex) == OMAGIC ? N_TXTADDR(ex) + (ex).a_text : \
		(N_TXTADDR(ex) + (ex).a_text + __LDPGSZ - 1) & ~(__LDPGSZ - 1))

/* Data segment offset. */
#define	N_DATOFF(ex) \
	N_ALIGN(ex, N_TXTOFF(ex) + (ex).a_text)

/* On NetBSD, the magic number is always in ntohl's "network" (big-endian)
   format.  */
#define SWAP_MAGIC(ext) bfd_getb32 (ext)

#include "aout-target.h"
