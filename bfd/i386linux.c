/* BFD back-end for linux flavored i386 a.out binaries.
   Copyright (C) 1992, 1993, 1994 Free Software Foundation, Inc.

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

#define	PAGE_SIZE	4096
#define ZMAGIC_DISK_BLOCK_SIZE 1024
#define	SEGMENT_SIZE	4096
#define TEXT_START_ADDR	0x0
#define N_SHARED_LIB(x) 0
#define ARCH 32
#define BYTES_IN_WORD 4

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "aout/aout64.h"
#include "aout/stab_gnu.h"
#include "aout/ar.h"
#include "libaout.h"           /* BFD a.out internal data structures */

#define DEFAULT_ARCH bfd_arch_i386
#define MY(OP) CAT(i386linux_,OP)
#define MY_text_includes_header 1
#define TARGETNAME "a.out-i386-linux"

/* We always generate QMAGIC files in preference to ZMAGIC files.  It
   would be possible to make this a linker option, if that ever
   becomes important.  */

static void MY_final_link_callback
  PARAMS ((bfd *, file_ptr *, file_ptr *, file_ptr *));

static boolean
i386linux_bfd_final_link (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  obj_aout_subformat (abfd) = q_magic_format;
  return NAME(aout,final_link) (abfd, info, MY_final_link_callback);
}

#define MY_bfd_final_link i386linux_bfd_final_link

#include "aout-target.h"
