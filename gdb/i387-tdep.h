/* Target-dependent code for the i387.

   Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

#ifndef I387_TDEP_H
#define I387_TDEP_H

struct gdbarch;
struct ui_file;
struct frame_info;
struct type;

/* Print out the i387 floating point state.  */

extern void i387_print_float_info (struct gdbarch *gdbarch,
				   struct ui_file *file,
				   struct frame_info *frame,
				   const char *args);

/* Read a value of type TYPE from register REGNUM in frame FRAME, and
   return its contents in TO.  */

extern void i387_register_to_value (struct frame_info *frame, int regnum,
				    struct type *type, void *to);

/* Write the contents FROM of a value of type TYPE into register
   REGNUM in frame FRAME.  */

extern void i387_value_to_register (struct frame_info *frame, int regnum,
				    struct type *type, const void *from);


/* Size of the memory area use by the 'fsave' and 'fxsave'
   instructions.  */
#define I387_SIZEOF_FSAVE	108
#define I387_SIZEOF_FXSAVE	512

/* Fill register REGNUM in GDB's register cache with the appropriate
   value from *FSAVE.  This function masks off any of the reserved
   bits in *FSAVE.  */

extern void i387_supply_fsave (const char *fsave, int regnum);

/* Fill register REGNUM (if it is a floating-point register) in *FSAVE
   with the value in GDB's register cache.  If REGNUM is -1, do this
   for all registers.  This function doesn't touch any of the reserved
   bits in *FSAVE.  */

extern void i387_fill_fsave (char *fsave, int regnum);

/* Fill register REGNUM in GDB's register cache with the appropriate
   floating-point or SSE register value from *FXSAVE.  This function
   masks off any of the reserved bits in *FXSAVE.  */

extern void i387_supply_fxsave (const char *fxsave, int regnum);

/* Fill register REGNUM (if it is a floating-point or SSE register) in
   *FXSAVE with the value in GDB's register cache.  If REGNUM is -1, do
   this for all registers.  This function doesn't touch any of the
   reserved bits in *FXSAVE.  */

extern void i387_fill_fxsave (char *fxsave, int regnum);

#endif /* i387-tdep.h */
