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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

typedef struct foo	/* struct foo defined here.  */
{
  int a;
  int b;
} foo;			/* typedef foo defined here.  */

struct bar		/* struct bar defined here.  */
{
  int a;
  int b;
};

/* Yes, this really is typedef-ing 'bar' to something other than 'struct
   bar'.  Just testing that GDB handles this.  This is not good code.  */
typedef struct foo bar;	/* typedef bar defined here.  */

/* The following must be a single line.  This tests the 'struct baz' and
   the 'typedef ... baz;' being on the same line.  */
typedef struct baz { int a; int b; } baz;	/* baz defined here.  */

volatile struct foo obj1 = { 1, 2 };
volatile foo obj2 = { 1, 2 };
volatile struct bar obj3 = { 1, 2 };
volatile bar obj4 = { 1, 2 };
volatile baz obj5 = { 1, 2 };

int
main ()
{
  return obj1.b + obj2.b + obj3.b + obj4.b + obj5.b;
}
