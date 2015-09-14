/* This test program is part of GDB, the GNU debugger.

   Copyright 1997-2015 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int global_i = 100;

int main (void)
{
  int local_j = global_i + 1;
  int local_k = local_j + 1;

  printf ("foll-exec is about to execlp(execd-prog)...\n");

  execlp (BASEDIR "/execd-prog",
	  BASEDIR "/execd-prog",
	  "execlp arg1 from foll-exec",
	  (char *) 0);

  printf ("foll-exec is about to execl(execd-prog)...\n");

  execl (BASEDIR "/execd-prog",	/* tbreak-execl */
	 BASEDIR "/execd-prog",
	 "execl arg1 from foll-exec",
	 "execl arg2 from foll-exec",
	 (char *) 0);

  {
    static char * argv[] = {
      (char *) BASEDIR "/execd-prog",
      (char *) "execv arg1 from foll-exec",
      (char *) 0};

    printf ("foll-exec is about to execv(execd-prog)...\n");

    execv (BASEDIR "/execd-prog", argv); /* tbreak-execv */
  }
}
