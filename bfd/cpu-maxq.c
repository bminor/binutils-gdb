/* BFD support for the MAXQ20/10 architecture.
   Copyright  2004  Free Software Foundation, Inc.

   Written by Vineet Sharma(vineets@noida.hcltech.com)
	      Inderpreet Singh(inderpreetb@noida.hcltech.com)		

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

/* MAXQ Archtecture info.  */
const bfd_arch_info_type bfd_maxq_arch =
{
  16,				/* 16 bits in a word */
  16,				/* 16 bits in an address */
  8,				/* 16 bits in a byte */
  bfd_arch_maxq,		/* enum bfd_archtecture arch */
  0,				/* only 1 machine */
  "maxq",			/* arch_name */
  "maxq",			/* printable name */
  0,				/* section align power */
  TRUE,				/* the_default: if this is the machine */
  bfd_default_compatible,	/* bfd_arch_info */
  bfd_default_scan,
  NULL
};
