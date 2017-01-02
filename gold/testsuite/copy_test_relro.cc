// copy_test_relro.cc -- test copy relocs against read-only and relro symbols.

// Copyright (C) 2016-2017 Free Software Foundation, Inc.
// Written by Cary Coutant <ccoutant@gmail.com>.

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
#include <stdint.h>

extern char* _etext;
extern char* __data_start;
extern char* _edata;
extern char* _end;

extern int* const p;
extern const int b[];
int a = 123;

int main()
{
  assert(*p == 123);
  assert(b[0] == 100);
  assert(b[1] == 200);
  assert(b[2] == 300);
  assert(b[3] == 400);
  assert(reinterpret_cast<const void*>(&p) < reinterpret_cast<void*>(&__data_start));
  assert(reinterpret_cast<const void*>(b) < reinterpret_cast<void*>(&__data_start));
  return 0;
}
