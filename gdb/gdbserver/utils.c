/* General utility routines for the remote server for GDB.
   Copyright (C) 1986, 1989, 1993 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "server.h"
#include <stdio.h>

/* Generally useful subroutines used throughout the program.  */

/* Print the system error message for errno, and also mention STRING
   as the file name for which the error was encountered.
   Then return to command level.  */

void
perror_with_name (string)
     char *string;
{
  extern int sys_nerr;
  extern char *sys_errlist[];
  extern int errno;
  char *err;
  char *combined;

  if (errno < sys_nerr)
    err = sys_errlist[errno];
  else
    err = "unknown error";

  combined = (char *) alloca (strlen (err) + strlen (string) + 3);
  strcpy (combined, string);
  strcat (combined, ": ");
  strcat (combined, err);

  error ("%s.", combined);
}

/* Print an error message and return to command level.
   STRING is the error message, used as a fprintf string,
   and ARG is passed as an argument to it.  */

NORETURN void
error (string, arg1, arg2, arg3)
     char *string;
     int arg1, arg2, arg3;
{
  extern jmp_buf toplevel;

  fflush (stdout);
  fprintf (stderr, string, arg1, arg2, arg3);
  fprintf (stderr, "\n");
  longjmp(toplevel, 1);
}

/* Print an error message and exit reporting failure.
   This is for a error that we cannot continue from.
   STRING and ARG are passed to fprintf.  */

void
fatal (string, arg)
     char *string;
     int arg;
{
  fprintf (stderr, "gdb: ");
  fprintf (stderr, string, arg);
  fprintf (stderr, "\n");
  exit (1);
}
