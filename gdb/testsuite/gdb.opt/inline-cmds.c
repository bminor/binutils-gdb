/* Copyright (C) 2008, 2010 Free Software Foundation, Inc.

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

int x, y;
volatile int result;

void bar(void);
void marker(void);
void noinline(void);

inline int func1(void)
{
  bar ();
  return x * y;
}

inline int func2(void)
{
  return x * func1 ();
}

inline void func3(void)
{
  bar ();
}

inline void outer_inline1(void)
{
  noinline ();
}

inline void outer_inline2(void)
{
  outer_inline1 ();
}

int main (void)
{ /* start of main */
  int val;

  x = 7;
  y = 8;

  result = func1 ();
  result = func2 ();
  marker ();

  result = 0;
  result = 0; /* set breakpoint 3 here */

  func1 (); /* first call */
  func1 (); /* second call */
  marker ();

  result = 0;
  result = 0; /* set breakpoint 4 here */

  func1 ();
  func3 ();
  marker ();

  result = 0;
  result = 0; /* set breakpoint 5 here */

  marker ();
  func1 ();
  func3 ();
  marker ();  /* set breakpoint 6 here */

  outer_inline2 ();

  return 0;
}
