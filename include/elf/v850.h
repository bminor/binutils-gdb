/* V850 ELF support for BFD.
   Copyright (C) 1997 Free Software Foundation, Inc.
   Created by Michael Meissner, Cygnus Support <meissner@cygnus.com>

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

/* This file holds definitions specific to the MIPS ELF ABI.  Note
   that most of this is not actually implemented by BFD.  */

#ifndef _ELF_V850_H
#define _ELF_V850_H

/* Flags for the st_other field */
#define V850_OTHER_SDA		0x01	/* symbol had SDA relocations */
#define V850_OTHER_ZDA		0x02	/* symbol had ZDA relocations */
#define V850_OTHER_TDA		0x04	/* symbol had TDA relocations */
#define V850_OTHER_TDA_BYTE	0x08	/* symbol had TDA byte relocations */
#define V850_OTHER_ERROR	0x80	/* symbol had an error reported */

#endif /* _ELF_V850_H */
