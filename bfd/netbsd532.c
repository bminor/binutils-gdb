/* BFD back-end for NetBSD/532 a.out-ish binaries.
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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */
/*
 * Created by Ian Dall
 *            5-Jun-94
 *
 * Largely a copy of netbsd386.c plus some code from aout-pc532-mach.c
 */

#define	BYTES_IN_WORD	4
#define	ARCH	32

/* ZMAGIC files never include the header in the text.  */
#define	N_HEADER_IN_TEXT(x)	1

#define	PAGE_SIZE	0x1000

/* ZMAGIC files start at address 0.  This does not apply to QMAGIC.  */
#define TEXT_START_ADDR PAGE_SIZE
#define N_SHARED_LIB(x) 0

/* Use a_entry of 0 to distinguish object files from OMAGIC executables */
#define N_TXTADDR(x) \
    ((N_MAGIC(x) == OMAGIC) && \
      ((x).a_entry < TEXT_START_ADDR) ? 0 :	/* object file or NMAGIC */\
     N_SHARED_LIB(x) ? 0 :	\
     N_HEADER_IN_TEXT(x)  ?	\
	    TEXT_START_ADDR + EXEC_BYTES_SIZE :	/* no padding */\
	    TEXT_START_ADDR			/* a page of padding */\
    )

#define	SEGMENT_SIZE	PAGE_SIZE

#define	DEFAULT_ARCH	bfd_arch_ns32k

#define MY(OP) CAT(netbsd532_,OP)
/* This needs to start with a.out so GDB knows it is an a.out variant.  */
#define TARGETNAME "a.out-netbsd-532"
#define MACHTYPE_OK(mtype) ((mtype) == M_532_NETBSD)

#define N_MAGIC(ex) \
    ( (((ex).a_info)&0xffff0000) ? ((((ex).a_info))&0xffff) : ((ex).a_info))
#define N_MACHTYPE(ex) \
    ( (((ex).a_info)&0xffff0000) ? (((((ex).a_info))>>16)&0x03ff) : 0 )
#define N_FLAGS(ex) \
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
  ((exec).a_info = \
      (((exec).a_info & ~(0x3f<<26)) | (((flags)&0x3f) << 26)))

#define N_MAG(MAG) ((MAG) == o_magic? OMAGIC: ((MAG) == z_magic? ZMAGIC: ((MAG) == n_magic? NMAGIC: 0)))

#define WRITE_HEADERS(abfd, execp)					      \
      {									      \
	bfd_size_type text_size; /* dummy vars */			      \
        int mag;							      \
	file_ptr text_end;						      \
	if (adata(abfd).magic == undecided_magic)			      \
	  NAME(aout,adjust_sizes_and_vmas) (abfd, &text_size, &text_end);     \
        mag = adata(abfd).magic; 					      \
	N_SET_INFO(*execp, N_MAG(mag), M_532_NETBSD, aout_backend_info (abfd)->exec_hdr_flags);			      \
    									      \
	execp->a_syms = bfd_get_symcount (abfd) * EXTERNAL_NLIST_SIZE;	      \
	execp->a_entry = bfd_get_start_address (abfd);			      \
    									      \
	execp->a_trsize = ((obj_textsec (abfd)->reloc_count) *		      \
			   obj_reloc_entry_size (abfd));		      \
	execp->a_drsize = ((obj_datasec (abfd)->reloc_count) *		      \
			   obj_reloc_entry_size (abfd));		      \
	NAME(aout,swap_exec_header_out) (abfd, execp, &exec_bytes);	      \
									      \
	if (bfd_seek (abfd, (file_ptr) 0, SEEK_SET) != 0) return false;	      \
	if (bfd_write ((PTR) &exec_bytes, 1, EXEC_BYTES_SIZE, abfd)	      \
	    != EXEC_BYTES_SIZE)						      \
	  return false;							      \
	/* Now write out reloc info, followed by syms and strings */	      \
  									      \
	if (bfd_get_outsymbols (abfd) != (asymbol **) NULL		      \
	    && bfd_get_symcount (abfd) != 0) 				      \
	    {								      \
	      if (bfd_seek (abfd, (file_ptr)(N_SYMOFF(*execp)), SEEK_SET)     \
		  != 0)							      \
	        return false;						      \
									      \
	      if (! NAME(aout,write_syms)(abfd)) return false;		      \
									      \
	      if (bfd_seek (abfd, (file_ptr)(N_TRELOFF(*execp)), SEEK_SET)    \
		  != 0)							      \
	        return false;						      \
									      \
	      if (!NAME(aout,squirt_out_relocs) (abfd, obj_textsec (abfd)))   \
		return false;						      \
	      if (bfd_seek (abfd, (file_ptr)(N_DRELOFF(*execp)), SEEK_SET)    \
		  != 0)							      \
	        return false;						      \
									      \
	      if (!NAME(aout,squirt_out_relocs)(abfd, obj_datasec (abfd)))    \
		return false;						      \
	    }								      \
      }									      


#include "bfd.h"
#include "sysdep.h"
#include "aout-ns32k.h"
#include "libbfd.h"
#include "libaout.h"


/* On NetBSD, the magic number is always in ntohl's "network" (big-endian)
   format.  */
#define SWAP_MAGIC(ext) bfd_getb32 (ext)

#include "aout/aout64.h"

/* Forward declaration. Defined in aout-target.h */
static boolean MY(set_sizes)();

static CONST struct aout_backend_data MY(backend_data) = {
  0,				/* zmagic contiguous */
  1,				/* text incl header */
  0,
  0,				/* text vma? */
  MY(set_sizes),
  0,				/* exec header is counted */
};

#define MY_backend_data &MY(backend_data)


/* We can`t use the MYNS macro here for cpp reasons too subtle
 * for me -- IWD
 */
#define MY_bfd_reloc_type_lookup ns32kaout_bfd_reloc_type_lookup

/* libaout doesn't use NAME for these ... */
#define MY_get_section_contents aout_32_get_section_contents

static boolean
MY(write_object_contents) (abfd)
     bfd *abfd;
{
  struct external_exec exec_bytes;
  struct internal_exec *execp = exec_hdr (abfd);

#if CHOOSE_RELOC_SIZE
  CHOOSE_RELOC_SIZE(abfd);
#else
  obj_reloc_entry_size (abfd) = RELOC_STD_SIZE;
#endif

  BFD_ASSERT(bfd_get_arch(abfd) == bfd_arch_ns32k);
  BFD_ASSERT(bfd_get_mach(abfd) == 32532);
  N_SET_MACHTYPE (*execp, M_532_NETBSD);

  N_SET_FLAGS (*execp, aout_backend_info (abfd)->exec_hdr_flags);

  WRITE_HEADERS(abfd, execp);

  return true;
}

#define MY_write_object_contents MY(write_object_contents)

#include "aout-target.h"
