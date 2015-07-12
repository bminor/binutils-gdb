/* This testcase is part of GDB, the GNU debugger.

   Copyright 2015 Free Software Foundation, Inc.

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

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

/* The following defines must correspond to solib-mismatch.exp .  */

/* DIRNAME and LIB must be defined at compile time.  */
#ifndef DIRNAME
# error DIRNAME not defined
#endif
#ifndef LIB
# error LIB not defined
#endif

int
main (int argc, char *argv[])
{
  void *h;
  int (*foo) (void);

  if (chdir (DIRNAME) != 0)
    {
      printf ("ERROR - Could not cd to %s\n", DIRNAME);
      return 1;
    }

  h = dlopen (LIB, RTLD_NOW);

  if (h == NULL)
    {
      printf ("ERROR - could not open lib %s\n", LIB);
      return 1;
    }
  foo = dlsym (h, "foo"); /* set breakpoint 1 here */
  dlclose (h);
  return 0;
}
