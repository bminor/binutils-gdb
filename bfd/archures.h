/* Intel 960 machine types */

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
   Machine architecture and type definitions for BFD.

   These definitions are only used inside the BFD package.  External programs
   access them by calling bfd_scan_arch_mach() and bfd_arch_mach_string().

   The architectures themselves are defined in bfd.h since they are an
   enum needed for BFD structs.  Numeric machine types are simply used
   as-is, e.g. 68020.  Non-numeric machine types like "i960CA" have
   names in this file.  */
 

/* $Id$ */

#define	bfd_mach_i960_core	1
#define	bfd_mach_i960_kb_sb	2
#define	bfd_mach_i960_mc	3
#define	bfd_mach_i960_xa	4
#define	bfd_mach_i960_ca	5
#define	bfd_mach_i960_ka_sa	6

