/* gdb.c -as supports gdb-
   Copyright (C) 1987 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler.

GAS is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* This code is independent of the underlying operating system. */

#include "as.h"

static long int		size;	/* 0 or size of GDB symbol file. */
static char *		where;	/* Where we put symbol file in memory. */

#define SUSPECT		/* JF */

long int			/* 0 means don't call gdb_... routines */
gdb_begin (filename)		/* because we failed to establish file */
				/* in memory. */
     char * filename;		/* NULL: we have nothing to do. */
{
  long int		gdb_file_size();
  char *		xmalloc();
  void			gdb_file_begin();
  void			gdb_file_read();
  void			gdb_block_begin();
  void			gdb_symbols_begin();

  gdb_file_begin();
  size = 0;
  if (filename && (size = gdb_file_size (filename)))
    {
      where = xmalloc( (long) size );
      gdb_file_read (where, filename);	/* Read, then close file. */
      gdb_block_begin();
      gdb_symbols_begin();
    }
  return (size);
}

void
gdb_end()
{
  void gdb_file_end();

  gdb_file_end();
}

void
gdb_emit (filename)	/* Append GDB symbols to object file. */
char *	filename;
{
  void gdb_block_emit();
  void gdb_symbols_emit();
  void gdb_lines_emit();
  void output_file_append();

  gdb_block_emit ();
  gdb_symbols_emit ();
  gdb_lines_emit();
  output_file_append (where, size, filename);
}



/*
	Notes:	We overwrite what was there.
		We assume all overwrites are 4-char numbers.
*/

void
gdb_alter (offset, value)	/* put value into GDB file + offset. */
     long int	offset;
     long int	value;
{
  void md_number_to_chars();

#ifdef SUSPECT
  if (offset > size - sizeof(long int) || offset < 0)
    {
      as_warn( "gdb_alter: offset=%d. size=%ld.\n", offset, size );
      return;
    }
#endif

#ifdef B_OUT
      /* Symbol info will be used on the host machine only (only executable
       * code is actually downloaded to the i80960).  Therefore, leave it
       * in host byte order.
       */

      *(long int *)(where + offset) = value;
#else
      md_number_to_chars (where + offset, value, 4);
#endif
}

/* end: gdb.c */
