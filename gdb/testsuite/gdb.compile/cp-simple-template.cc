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

  template <typename T = A>
  T tempmethod (void)
  {
    return value;
  }

  template <typename T = A, int V = -1>
  static T stempmethod (void)
  {
    return V;
  }

  template <typename T = A, int V = -2>
  static T stempmethod (T arg)
  {
    return arg + V;
  }

  int value;
};

template<>
int
A::tempmethod (void)
{
  return -value;
}

template <typename T>
T deduct (T a)
{
  return a;
}

extern char const g_str[] = "hello";

template <typename T>
int mod_test (T a) { return 1; }

template <typename T>
int mod_test (T* const a) { return 2; }

template <typename T>
int mod_test (T const* const a) { return 3; }

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

// A handful of operator templates
struct O
{
  O (int v) : v_ (v) { }

  template <typename T>
  operator T (void) { return -v_; }

  template <typename T>
  O operator+ (T val)
  {
    return v_ + val;
  }

  int v_;
};

template <typename T>
const T** ret_test (void) { return 0; }

template <typename T>
T const* const* ret2_test (void) { return 0; }

int
main (void)
{
  A a (20);
  O o (30);
  int var = 0xdeadbeef;
  int i = 1;
  const int j = 1;
  int* pi = &i;
  int const* const cpci = &j;
  int *const cpi = &i;

  int o_val = o + 30;
  int mod_value = mod_test (i) + mod_test (cpci) + mod_test (cpi);
  const char** cp = ret_test<char> ();
  char const* const* ccp = ret2_test<char> ();

  return mytemplate<int, 1> (0)
    + mytemplate<int, 1> ()
    + mytemplate<0> ()
    + mytemplate ()
    + a.tempmethod ()
    + a.tempmethod<int> ()
    + A::stempmethod ()
    + A::stempmethod (0)
    + defaultvals ()
    + defaultvals<int, 20> ()
    + deduct (0); // break here
}

