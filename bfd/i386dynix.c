/* BFD back-end for i386 a.out binaries under dynix.
   Copyright (C) 1994 Free Software Foundation, Inc.

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

/* This BFD is currently only tested with gdb, writing object files
   may not work.  */

#define BYTES_IN_WORD	4
#define ARCH	32

#define TEXT_START_ADDR 4096
#define PAGE_SIZE	4096
#define SEGMENT_SIZE	PAGE_SIZE

#include "aout/dynix3.h"

#define DEFAULT_ARCH	bfd_arch_i386
#define MACHTYPE_OK(mtype) ((mtype) == M_386 || (mtype) == M_UNKNOWN)

#define MY(OP) CAT(i386dynix_,OP)
#define TARGETNAME "a.out-i386-dynix"

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libaout.h"

/* (Ab)use some fields in the internal exec header to be able to read
   executables that contain shared data.  */

#define a_shdata a_tload
#define a_shdrsize a_dload
#define aout_32_swap_exec_header_in dynix_swap_exec_header_in

void
dynix_swap_exec_header_in (abfd, raw_bytes, execp)
     bfd *abfd;
     struct external_exec *raw_bytes;
     struct internal_exec *execp;
{
  struct external_exec *bytes = (struct external_exec *)raw_bytes;

  /* The internal_exec structure has some fields that are unused in this
     configuration (IE for i960), so ensure that all such uninitialized
     fields are zero'd out.  There are places where two of these structs
     are memcmp'd, and thus the contents do matter. */
  memset ((PTR) execp, 0, sizeof (struct internal_exec));
  /* Now fill in fields in the execp, from the bytes in the raw data.  */
  execp->a_info   = bfd_h_get_32 (abfd, bytes->e_info);
  execp->a_text   = GET_WORD (abfd, bytes->e_text);
  execp->a_data   = GET_WORD (abfd, bytes->e_data);
  execp->a_bss    = GET_WORD (abfd, bytes->e_bss);
  execp->a_syms   = GET_WORD (abfd, bytes->e_syms);
  execp->a_entry  = GET_WORD (abfd, bytes->e_entry);
  execp->a_trsize = GET_WORD (abfd, bytes->e_trsize);
  execp->a_drsize = GET_WORD (abfd, bytes->e_drsize);
  execp->a_shdata = GET_WORD (abfd, bytes->e_shdata);
  execp->a_shdrsize = GET_WORD (abfd, bytes->e_shdrsize);
}

#include "aout-target.h"
