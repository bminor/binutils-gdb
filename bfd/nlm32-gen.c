/* Generic support for 32-bit NLM (NetWare Loadable Module)
   Copyright (C) 1993 Free Software Foundation, Inc.

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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#define ARCH_SIZE 32
#include "libnlm.h"

/* This does not include any relocations, but should be good enough
   for GDB to read the file.  */

#define TARGET_LITTLE_NAME		"nlm32-little"
#define TARGET_LITTLE_SYM		nlmNAME(little_generic_vec)
#define TARGET_BIG_NAME			"nlm32-big"
#define TARGET_BIG_SYM			nlmNAME(big_generic_vec)
#define TARGET_BACKEND_DATA		NULL

#include "nlm-target.h"
