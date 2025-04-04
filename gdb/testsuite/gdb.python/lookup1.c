/* This testcase is part of GDB, the GNU debugger.

   Copyright 2025 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see  <http://www.gnu.org/licenses/>.  */

struct the_struct {
  int one;
};

struct the_struct struct1;

enum the_enum {
  one
};

enum the_enum enum1;

union the_union {
  int one;
  void *ptr;
};

union the_union union1;

typedef struct the_struct the_typedef;

the_typedef typedef1;

int function (void)
{
  return 0;
}
