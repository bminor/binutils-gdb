/* This testcase is part of GDB, the GNU debugger.

   Copyright 2024 Free Software Foundation, Inc.

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

#include "attributes.h"

static int
test0 (void)
{
  asm (""); /* First line of test0.  */
  return 1; /* Second line of test0.  */
}

int __attribute__((noinline)) ATTRIBUTE_NOCLONE
test1 (int x)
{
  asm ("");
  return x + 1; /* Second line of test1.  */
}

int
main (void)
{
  test1 (test0 ()); /* First line of main.  */
  test1 (test0 ()); /* Second line of main.  */
  return 0;         /* Third line of main.  */
}
