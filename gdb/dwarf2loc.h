/* Dwarf2 location expression support for GDB.
   Copyright 2003 Free Software Foundation, Inc.

   This file is part of GDB.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#if !defined (DWARF2LOC_H)
#define DWARF2LOC_H

/* This header is private to the DWARF-2 reader.  It is shared between
   dwarf2read.c and dwarf2loc.c.  */

/* The symbol location baton type used by the DWARF-2 reader (i.e.
   SYMBOL_LOCATION_BATON for a LOC_COMPUTED symbol).  */

struct dwarf2_locexpr_baton
{
  unsigned char *data;
  unsigned short size;
  struct objfile *objfile;
};

extern struct location_funcs dwarf2_locexpr_funcs;

#endif
