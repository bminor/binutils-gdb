/* This testcase is part of GDB, the GNU debugger.

   Copyright 2025 Free Software Foundation, Inc.

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

#include <stdint.h>
#include <assert.h>

typedef unsigned long long type_ll;

#ifndef VAR_TYPE
#  error "VAR_TYPE not defined"
#endif

/* Place A and B within this wrapper struct.  FIRST ensures that A is
   (usually) going to start at an 8-byte boundary.  The goal here is
   that, when VAR_TYPE is less than 8 bytes, both A and B are placed
   within the same 8-byte region, and that the region starts at an
   8-byte boundary.  */

struct wrapper
{
  unsigned long long first;

  VAR_TYPE a, b;
};

volatile struct wrapper obj;

/* Write to obj.a and obj.b, but don't read these fields.  */
void
writer (void)
{
  obj.a = 1;
  obj.b = 2;
}

/* Read from obj.a and obj.b, but don't write to these fields.  */
int
reader (void)
{
  int v = obj.b - obj.a;
  v--;
  return v;
}

int
main (void)
{
  /* Ensure that obj.a, obj.b, and obj.c were placed as we needed.  */
  assert ((((uintptr_t) &obj.a) & 0x7) == 0);
  assert ((((uintptr_t) &obj.a) + sizeof (obj.a)) == (((uintptr_t) &obj.b)));
  assert (sizeof (obj.a) == sizeof (obj.b));

  writer ();

  int val = reader ();	/* Break for read test.  */

  return val;
}
