/* SH ELF support for BFD.
   Copyright (C) 1998 Free Software Foundation, Inc.

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
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _ELF_SH_H
#define _ELF_SH_H

#define R_SH_NONE	0
#define R_SH_DIR32	1
#define R_SH_REL32	2
#define R_SH_DIR8WPN	3
#define R_SH_IND12W	4
#define R_SH_DIR8WPL	5
#define R_SH_DIR8WPZ	6
#define R_SH_DIR8BP	7
#define R_SH_DIR8W	8
#define R_SH_DIR8L	9
#define R_SH_SWITCH16	25
#define R_SH_SWITCH32	26
#define R_SH_USES	27
#define R_SH_COUNT	28
#define R_SH_ALIGN	29
#define R_SH_CODE	30
#define R_SH_DATA	31
#define R_SH_LABEL	32

#endif
