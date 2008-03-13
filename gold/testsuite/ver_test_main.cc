// ver_test_main.cc -- a test case for gold

// Copyright 2007, 2008 Free Software Foundation, Inc.
// Written by Cary Coutant <ccoutant@google.com>.

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

#include <cassert>

#include "ver_test.h"

int
main()
{
  assert(t1());
  assert(t2());
  assert(t3());
  return 0;
}

// Calls a function in file2 that has two versions and verifies
// that the default version is called.

bool
t2()
{
  TRACE
  return t2_2() == 22;
}

// Call a function in a shared library that calls explicitly
// the version of t1_2() defined in another shared library.

bool
t3()
{
  TRACE
  return t3_2() == 12;
}
