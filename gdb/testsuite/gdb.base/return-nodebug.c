/* This testcase is part of GDB, the GNU debugger.

   Copyright 2009 Free Software Foundation, Inc.

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

static TYPE
init (void)
{
  return 0;
}

static TYPE
func (void)
{
  return 31;
}

static void
marker (void)
{
}

int
main (void)
{
  /* Preinitialize registers to 0 to avoid false PASS by leftover garbage.  */
  init ();

  printf ("result=" FORMAT "\n", CAST func ());

  /* Cannot `next' with no debug info.  */
  marker ();

  return 0;
}
