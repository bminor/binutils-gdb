// tls_test.cc -- test TLS variables for gold

// Copyright 2006, 2007 Free Software Foundation, Inc.
// Written by Ian Lance Taylor <iant@google.com>.

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

// This provides a set of test functions for TLS variables.  The
// functions are called by a main function in tls_test_main.cc.  This
// lets us test TLS access from a shared library.  We currently don't
// bother to test TLS access between two different files, on the
// theory that that is no more complicated than ordinary variable
// access between files.

// We start two threads, and stop the second one.  Then we run the
// first thread through the following cases.  Then we let the second
// thread continue, and run it through the same set of cases.  All the
// actual thread manipulation is in tls_test_main.cc.

// 1  Access to an uninitialized global thread variable.
// 2  Access to an uninitialized static thread variable.
// 3  Access to an initialized global thread variable.
// 4  Access to an initialized static thread variable.
// 5  Taking the address of a global thread variable.
// 6  Taking the address of a static thread variable.
// 8  Like test 1, but with the thread variable defined in another file.
// 9  Like test 3, but with the thread variable defined in another file.
// 10 Like test 5, but with the thread variable defined in another file.
// last  Verify that the above tests left the variables set correctly.


#include "tls_test.h"

__thread int v1;
static __thread int v2;
__thread int v3 = 3;
static __thread int v4 = 4;
__thread int v5;
static __thread int v6;

// These variables are defined in tls_test_file2.cc
extern __thread int o1;
extern __thread int o2;
extern __thread int o3;

bool
t1()
{
  if (v1 != 0)
    return false;
  v1 = 10;
  return true;
}

bool
t2()
{
  if (v2 != 0)
    return false;
  v2 = 20;
  return true;
}

bool
t3()
{
  if (v3 != 3)
    return false;
  v3 = 30;
  return true;
}

bool
t4()
{
  if (v4 != 4)
    return false;
  v4 = 40;
  return true;
}

// For test 5 the main function calls f5b(f5a()), then calls t5().

int*
f5a()
{
  return &v5;
}

void
f5b(int* p)
{
  *p = 50;
}

bool
t5()
{
  return v5 == 50;
}

// For test 6 the main function calls f6b(f6a()), then calls t6().

int*
f6a()
{
  return &v6;
}

void
f6b(int* p)
{
  *p = 60;
}

bool
t6()
{
  return v6 == 60;
}

// The slot for t7() is unused.

bool
t8()
{
  if (o1 != 0)
    return false;
  o1 = 10;
  return true;
}

bool
t9()
{
  if (o2 != 2)
    return false;
  o2 = 20;
  return true;
}

// For test 10 the main function calls f10b(f10a()), then calls t10().

int*
f10a()
{
  return &o3;
}

void
f10b(int* p)
{
  *p = 30;
}

bool
t10()
{
  return o3 == 30;
}

bool
t_last()
{
  return (v1 == 10
	  && v2 == 20
	  && v3 == 30
	  && v4 == 40
	  && v5 == 50
	  && v6 == 60
          && o1 == 10
          && o2 == 20
          && o3 == 30);
}
