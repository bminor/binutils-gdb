/* bucomm.h -- binutils common include file.
   Copyright 1992 Free Software Foundation, Inc.

This file is part of GNU Binutils.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

void bfd_nonfatal PARAMS ((CONST char *));

void bfd_fatal PARAMS ((CONST char *));

PTR xmalloc PARAMS ((size_t));

PTR xrealloc PARAMS ((char *, size_t));

void fatal PARAMS ((CONST char *, ...));

void print_arelt_descr PARAMS ((FILE *file, bfd *abfd, boolean verbose));

extern char *program_name;
