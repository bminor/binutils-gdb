/* Copyright 2016 Free Software Foundation, Inc.

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

// NOTE: We cannot currently use namespaces until namespace-qualified
// symbol lookups are fixed in gdb

template <typename T, int V>
T mytemplate (int a)
{
  return static_cast<T> (a) + V;
}

template <typename T, int V>
T mytemplate (void)
{
  return -V;
}

template <int V = 100>
int mytemplate (void)
{
  return V;
}

struct A
{
  A (int val) : value (val) { }
  operator int () const { return value; }

  template <typename T>
  T tempmethod (void)
  {
    return static_cast<T> (0);
  }

  int value;
};

template <typename T>
T deduct (T a)
{
  return a;
}

extern char const g_str[] = "hello";

#if 0
/* This chaining of defaults has no good representation in the debug info.
   For each instance where T2 defaulted to T1, we will have as many
   default values in the debug info for T2, one for each such instance.  */
template <typename T1 = int, typename T2 = T1, typename T3 = T2,
	  int V1 = 10, int V2 = 20, const char* V3 = g_str>
T1 defaultvals (void)
{
  return static_cast<T1> (V1);
}
#else
template <typename T = A, int V = 10, const char* S = g_str>
T defaultvals (void)
{
  return static_cast<T> (V);
}
#endif

int
main (void)
{
  A a (20);
  int var = 0xdeadbeef;

  return mytemplate<int, 1> (0)
    + mytemplate<int, 1> ()
    + mytemplate<0> ()
    + mytemplate ()
    + a.tempmethod<int> ()
    + defaultvals ()
    + defaultvals<int> ()
    + deduct (0); // break here
}

