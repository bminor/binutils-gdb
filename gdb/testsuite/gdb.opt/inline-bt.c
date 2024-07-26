/* Copyright (C) 2008-2025 Free Software Foundation, Inc.

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

/* This is only ever run if it is compiled with a new-enough GCC, but
   we don't want the compilation to fail if compiled by some other
   compiler.  */
#ifdef __GNUC__
#define ATTR __attribute__((always_inline))
#else
#define ATTR
#endif

int x, y;
volatile int z = 0;
volatile int result;

void bar(void);

inline ATTR int func1(int s)
{
  bar ();
  return x * y + s;
}

inline ATTR int func2(void)
{
  return x * func1 (1);
}

inline ATTR int
return_one (void)
{
  /* The following empty asm() statement prevents older (< 11.x) versions
     of gcc from completely optimising away this function.  And for newer
     versions of gcc (>= 11.x) this ensures that we have two line table
     entries in main for the inline call to this function, with the second
     of these lines being a non-statement, which is critical for this
     test.  These two behaviours have been checked for versions of gcc
     between 8.4.0 and 14.2.0.  */
  asm ("");
  return 1;
}

volatile int global = 0;

__attribute__((noinline)) ATTRIBUTE_NOCLONE void
not_inline_func (int count)
{
  global += count;
  global += count;	/* b/p in not_inline_func */
  global += count;
}

int main (void)
{
  int val;

  x = 7;
  y = 8;
  bar ();

  val = func1 (2);
  result = val;

  val = func2 ();
  result = val;

  not_inline_func (return_one ());	/* bt line in main */

  return 0;
}
