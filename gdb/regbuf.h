/* Register buffer sufficient to hold all raw registers

   Copyright 2002 Free Software Foundation, Inc.

   This file is part of GDB.

   Contributed by Red Hat, Inc.

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

#ifndef REGBUF_H
#define REGBUF_H

struct regbuf;

/* Create/destroy the register buffer.  */

extern struct regbuf *regbuf_xmalloc (struct gdbarch *gdbarch);
extern void regbuf_xfree (struct regbuf *regbuf);
extern struct regbuf *regbuf_xmalloc_with_cleanup (struct gdbarch *gdbarch);

/* Duplicate a regbuf and all contents.  */

extern struct regbuf *regbuf_dup (struct regbuf *regbuf);

/* Is the regbuf entry valid.  */

int regbuf_valid_p (struct regbuf *regbuf, int regnum);

/* Access the register buffer.  */

extern void regbuf_read (struct regbuf *regbuf, int rawnum, void *buf);
extern void regbuf_write (struct regbuf *regbuf, int rawnum, const void *buf);

/* Convenience routines for reading/writing registers using an
   internal format.  */

extern CORE_ADDR regbuf_read_as_address (struct regbuf *regbuf, int regnum);

/* FIXME: cagney/2002-05-11: This is here simply to prop up the
   `registers' and `register_valid' global arrays.  */

extern char *grub_around_regbuf_for_registers (struct regbuf *regbuf);
extern char *grub_around_regbuf_for_register_valid (struct regbuf *regbuf);

#endif /* REGBUF_H */
