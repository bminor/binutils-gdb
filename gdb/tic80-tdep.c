/* Target-dependent code for the TI TMS320C80 (MVP) architecture,
   for GDB, the GNU Debugger.
   Copyright 1997 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"

CORE_ADDR
tic80_skip_prologue (pc)
     CORE_ADDR pc;
{
  error ("tic80_skip_prologue not implemented");
}

CORE_ADDR
tic80_frame_chain (finfo)
     struct frame_info *finfo;
{
  error ("tic80_frame_chain not implemented");
}

CORE_ADDR
tic80_frame_saved_pc (finfo)
     struct frame_info *finfo;
{
  error ("tic80_frame_saved_pc not implemented");
}

CORE_ADDR
tic80_saved_pc_after_call (finfo)
     struct frame_info *finfo;
{
  error ("tic80_saved_pc_after_call not implemented");
}

void
tic80_pop_frame ()
{
  error ("tic80_pop_frame not implemented");
}

void
tic80_store_return_value (valtype, valbuf)
    struct type *valtype;
    char *valbuf;
{
  error ("tic80_store_return_value not implemented");
}

/* Put here the code to store, into a struct frame_saved_regs, the
   addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special: the address we
   return for it IS the sp for the next frame. */

void
tic80_frame_find_saved_regs (fi, fsr)
     struct frame_info *fi;
     struct frame_saved_regs *fsr;
{
  error ("tic80_frame_find_saved_regs not implemented");
}

/* Given a return value in `regbuf' with a type `valtype', 
   extract and copy its value into `valbuf'.  */

void
tic80_extract_return_value (valtype, regbuf, valbuf)
    struct type *valtype;
    char regbuf[REGISTER_BYTES];
    char *valbuf;
{
  error ("tic80_extract_return_value not implemented");
}
