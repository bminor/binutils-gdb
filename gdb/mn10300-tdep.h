/* Target-dependent interface for Matsushita MN10300 for GDB, the GNU debugger.

   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free
   Software Foundation, Inc.

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

/* mn10300 private data */
struct gdbarch_tdep
{
  int am33_mode;
#define AM33_MODE (gdbarch_tdep (current_gdbarch)->am33_mode)
};

extern CORE_ADDR mn10300_analyze_prologue (struct frame_info *, void **, CORE_ADDR);
extern struct trad_frame_cache *mn10300_frame_unwind_cache (struct frame_info *, void **);
