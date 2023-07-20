/* This testcase is part of GDB, the GNU debugger.

   Copyright 2023-2025 Free Software Foundation, Inc.

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

/* Simple test, do some work with the arguments so GDB has a chance to
   break and check that the arguments are correct.  */

volatile int global_counter;

int
main (int argc, char *argv[])
{
  int i;

  global_counter = 0;		/* Break here.  */

  for (i = 0; i < argc; ++i)
    argv[i] = (char *) 0;

  return 0;
}
