/* gdb_file.c -o/s specific-
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

static long file_len;
static FILE *file;
extern long get_len();


void
gdb_file_begin ()
{
}

void
gdb_file_end()
{
}

long int			/* Open file, return size. 0: failed. */
gdb_file_size (filename)
char *filename;
{
  struct stat stat_buf;
  void as_perror();

  file= fopen (filename, "r");
  if (file == (FILE *)NULL)
    {
      as_perror ("Can't read GDB symbolic information file", filename);
      file_len=0;
    } else {
	(void)fstat (fileno(file), &stat_buf);
	file_len=stat_buf . st_size;
    }
  return ((long int)file_len );
}

void				/* Read the file, don't return if failed. */
gdb_file_read (buffer, filename)
     char *	buffer;
     char *	filename;
{
  register off_t	size_wanted;
  void as_perror();

  size_wanted = file_len;
  if (fread (buffer, size_wanted, 1, file) != 1)
    {
      as_perror ("Can't read GDB symbolic info file", filename);
      as_fatal ("Failed to read %ld. chars of GDB symbolic information",
		size_wanted);
    }
  if (fclose(file)==EOF)
    {
      as_perror ("Can't close GDB symbolic info file", filename);
      as_fatal ("I quit in disgust");
    }
}

/* end: gdb_file.c */
