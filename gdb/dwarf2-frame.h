/* Frame unwinder for frames with DWARF Call Frame Information.

   Copyright 2003 Free Software Foundation, Inc.

   Contributed by Mark Kettenis.

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

#ifndef DWARF2_FRAME_H
#define DWARF2_FRAME_H 1

struct objfile;
struct frame_info;

/* Return the frame unwind methods for the function that contains PC,
   or NULL if it can't be handled by DWARF CFI frame unwinder.  */

const struct frame_unwind *dwarf2_frame_sniffer (struct frame_info *next_frame);

/* Return the frame base methods for the function that contains PC, or
   NULL if it can't be handled by the DWARF CFI frame unwinder.  */

const struct frame_base *dwarf2_frame_base_sniffer (struct frame_info *next_frame);

/* Register the DWARF CFI for OBJFILE.  */

void dwarf2_frame_build_info (struct objfile *objfile);

#endif /* dwarf2-frame.h */
