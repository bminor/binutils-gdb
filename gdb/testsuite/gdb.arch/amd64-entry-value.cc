/* This testcase is part of GDB, the GNU debugger.

   Copyright 2011 Free Software Foundation, Inc.

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

static volatile int v;

static void __attribute__((noinline, noclone))
e (int i, double j)
{
  v = 0;
}

static void __attribute__((noinline, noclone))
d (int i, double j)
{
  i++;
  j++;
  e (i, j);
  e (v, v);
asm ("breakhere:");
  e (v, v);
}

static void __attribute__((noinline, noclone))
c (int i, double j)
{
  d (i * 10, j * 10);
}

static void __attribute__((noinline, noclone))
a (int i, double j)
{
  c (i + 1, j + 1);
}

static void __attribute__((noinline, noclone))
b (int i, double j)
{
  c (i + 2, j + 2);
}

static void __attribute__((noinline, noclone))
amb_z (int i)
{
  d (i + 7, i + 7.5);
}

static void __attribute__((noinline, noclone))
amb_y (int i)
{
  amb_z (i + 6);
}

static void __attribute__((noinline, noclone))
amb_x (int i)
{
  amb_y (i + 5);
}

static void __attribute__((noinline, noclone))
amb (int i)
{
  if (i < 0)
    amb_x (i + 3);
  else
    amb_x (i + 4);
}

static void __attribute__((noinline, noclone))
amb_b (int i)
{
  amb (i + 2);
}

static void __attribute__((noinline, noclone))
amb_a (int i)
{
  amb_b (i + 1);
}

int
main ()
{
  d (30, 30.5);
  if (v)
    a (1, 1.25);
  else
    b (5, 5.25);
  amb_a (100);
  return 0;
}
