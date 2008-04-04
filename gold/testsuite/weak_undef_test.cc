// weak_undef_test.cc -- test handling of weak undefined symbols for gold

// Copyright 2008 Free Software Foundation, Inc.
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

// We test that we correctly deal with weak undefined symbols.
// We need to make sure that the symbol is resolved to zero
// by the linker and that no dynamic relocation is generated.

// This file will be linked with a shared library that does not
// define the symbol, so that the symbol remains undefined.
// Through the use of LD_LIBRARY_PATH, the program will load
// an alternate shared library that does define the symbol,
// so that we can detect whether the symbol was left for runtime
// resolution.


#include <cstdio>

extern int no_such_symbol_ __attribute__ ((weak));

int
main()
{
  if (&no_such_symbol_ != NULL)
    {
      fprintf(stderr, "FAILED the weak undef test: &no_such_symbol_ is not NULL\n");
      return 1;
    }
  return 0;
}
