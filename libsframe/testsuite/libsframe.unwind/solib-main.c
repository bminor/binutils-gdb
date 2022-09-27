/* Copyright (C) 2022 Free Software Foundation, Inc.

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

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include "sframe-backtrace-api.h"
#include "solib-lib1.h"
#include "solib-lib2.h"

#define BT_BUF_SIZE 100

int foo (int x)
{
  return ++x;
}

int bar (int x)
{
  x = adder2 (x, x+1, foo);

  return ++x;
}

int main (void)
{
  unsigned int a = 1;
  unsigned int b = 2;
  unsigned int result = 0;

  result = adder (a,b, bar);

  return 0;
}
