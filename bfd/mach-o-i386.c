/* Intel i386 Mach-O support for BFD.
   Copyright 2009
   Free Software Foundation, Inc.

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
#include "mach-o.h"
#include "bfd.h"
#include "libbfd.h"
#include "libiberty.h"

#define bfd_mach_o_object_p bfd_mach_o_i386_object_p
#define bfd_mach_o_core_p bfd_mach_o_i386_core_p
#define bfd_mach_o_mkobject bfd_mach_o_i386_mkobject

static const bfd_target *
bfd_mach_o_i386_object_p (bfd *abfd)
{
  return bfd_mach_o_header_p (abfd, 0, BFD_MACH_O_CPU_TYPE_I386);
}

static const bfd_target *
bfd_mach_o_i386_core_p (bfd *abfd)
{
  return bfd_mach_o_header_p (abfd,
                              BFD_MACH_O_MH_CORE, BFD_MACH_O_CPU_TYPE_I386);
}

static bfd_boolean
bfd_mach_o_i386_mkobject (bfd *abfd)
{
  bfd_mach_o_data_struct *mdata;

  if (!bfd_mach_o_mkobject_init (abfd))
    return FALSE;

  mdata = abfd->tdata.mach_o_data;
  mdata->header.magic = BFD_MACH_O_MH_MAGIC;
  mdata->header.cputype = BFD_MACH_O_CPU_TYPE_I386;
  mdata->header.cpusubtype = BFD_MACH_O_CPU_SUBTYPE_X86_ALL;
  mdata->header.byteorder = BFD_ENDIAN_LITTLE;
  mdata->header.version = 1;

  return TRUE;
}

#define TARGET_NAME 		mach_o_i386_vec
#define TARGET_STRING 		"mach-o-i386"
#define TARGET_BIG_ENDIAN 	0
#define TARGET_ARCHIVE 		0

#include "mach-o-target.c"
