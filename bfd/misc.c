/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* $Id$ */

#if 0
 /* xoxorich.  coelesced from other binutils. */
/* This crap should all be bundled with the binutils, or else be in its
   own library, but for expediency we are doing it this way right now. */

/*
 * Last Mod Mon Feb 18 14:49:39 PST 1991, by rich@cygint.cygnus.com
 */

#include <stdio.h>
#include "misc.h"
#if __STDC__
extern char *realloc (char * ptr, int size);
extern char *malloc (int size);
#else
extern char *realloc ();
extern char *malloc ();
#endif

/* Print the filename of the current file on 'outfile' (a stdio stream).  */

/* Current file's name */

char *input_name;

/* Current member's name, or 0 if processing a non-library file.  */

char *input_member;

void print_file_name (outfile)
     FILE *outfile;
{
  fprintf (outfile, "%s", input_name);
  if (input_member)
    fprintf (outfile, "(%s)", input_member);
}

/* process one input file */
void scan_library ();

char *program_name;

/* Report a nonfatal error.
   STRING is a format for printf, and ARG1 ... ARG3 are args for it.  */
/*VARARGS*/
void
error (string, arg1, arg2, arg3)
     char *string, *arg1, *arg2, *arg3;
{
  fprintf (stderr, "%s: ", program_name);
  fprintf (stderr, string, arg1, arg2, arg3);
  fprintf (stderr, "\n");
}



/* Report a nonfatal error.
   STRING is printed, followed by the current file name.  */

void
error_with_file (string)
     char *string;
{
  fprintf (stderr, "%s: ", program_name);
  print_file_name (stderr);
  fprintf (stderr, ": ");
  fprintf (stderr, string);
  fprintf (stderr, "\n");
}

/* Like malloc but get fatal error if memory is exhausted.  */


/* Like realloc but get fatal error if memory is exhausted.  */


/* end of misc.c */
#endif
