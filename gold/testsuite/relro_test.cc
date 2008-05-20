// relro_test.cc -- test -z relro for gold

// Copyright 2008 Free Software Foundation, Inc.
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

#include <cassert>
#include <csignal>
#include <stdint.h>
#include <unistd.h>

// This code is put into a shared library linked with -z relro.

// i1 and i2 are not relro variables.
int i1 = 1;
static int i2 = 2;

// P1 is a global relro variable.
int* const p1 = &i1;

// P2 is a local relro variable.
int* const p2 = &i2;

// Test symbol addresses.

bool
t1()
{
  void* i1addr = static_cast<void*>(&i1);
  void* i2addr = static_cast<void*>(&i2);
  const void* p1addr = static_cast<const void*>(&p1);
  const void* p2addr = static_cast<const void*>(&p2);

  // The relro variables should precede the non-relro variables in the
  // memory image.
  assert(i1addr > p1addr);
  assert(i1addr > p2addr);
  assert(i2addr > p1addr);
  assert(i2addr > p2addr);

  // The relro variables should not be on the same page as the
  // non-relro variables.
  const size_t page_size = getpagesize();
  uintptr_t i1page = reinterpret_cast<uintptr_t>(i1addr) & ~ (page_size - 1);
  uintptr_t i2page = reinterpret_cast<uintptr_t>(i2addr) & ~ (page_size - 1);
  uintptr_t p1page = reinterpret_cast<uintptr_t>(p1addr) & ~ (page_size - 1);
  uintptr_t p2page = reinterpret_cast<uintptr_t>(p2addr) & ~ (page_size - 1);
  assert(i1page != p1page);
  assert(i1page != p2page);
  assert(i2page != p1page);
  assert(i2page != p2page);

  return true;
}

// A signal handler for SIGSEGV.

extern "C"
void
sigsegv_handler(int)
{
  throw 0;
}

// Use a separate function to throw the exception, so that we don't
// need to use -fnon-call-exceptions.

void f2() __attribute__ ((noinline));
void
f2()
{
  int** pp1 = const_cast<int**>(&p1);
  *pp1 = &i2;

  // We shouldn't get here--the assignment to *pp1 should write to
  // memory which the dynamic linker marked as read-only, giving us a
  // SIGSEGV, causing sigsegv_handler to be invoked, to throw past us.
  assert(0);
}

// Changing a relro variable should give us a SIGSEGV.

bool
t2()
{
  signal(SIGSEGV, sigsegv_handler);

  try
    {
      f2();
      return false;
    }
  catch (int i)
    {
      assert(i == 0);
      return true;
    }
}
