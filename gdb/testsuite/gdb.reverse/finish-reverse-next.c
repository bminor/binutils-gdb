/* This testcase is part of GDB, the GNU debugger.

   Copyright 2012-2023 Free Software Foundation, Inc.

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

/* The reverse finish command should return from a function and stop on
   the first instruction of the source line where the function call is made.
   Specifically, the behavior should match doing a reverse next from the
   first instruction in the function.  GDB should only require one reverse
   step or next statement to reach the previous source code line.

   This test verifies the fix for gdb bugzilla:

   https://sourceware.org/bugzilla/show_bug.cgi?id=29927
*/

int
function1 (int a, int b)   // FUNCTION1
{
  int ret = 0;

  ret = a + b;
  return ret;
}

int
main(int argc, char* argv[])
{
  int a, b;

  a = 1;
  b = 5;

  function1 (a, b);   // CALL FUNCTION
  return 0;
}
