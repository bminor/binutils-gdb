/* Copyright (C) 2021-2022 Free Software Foundation, Inc.

   This file is part of GDB.

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

#include "locexpr-data-member-location.h"

struct A g_A = {3, 4};
struct B g_B = { {8, 9}, 10, 11 };

B *
foo ()
{						/* foo prologue */
  asm ("foo_label: .globl foo_label");
  return &g_B;					/* foo return */
}						/* foo end */

B *
bar (B *v)
{						/* bar prologue */
  asm ("bar_label: .globl bar_label");
  return v;					/* bar return */
}						/* bar end */

/* Some of the DWARF assembler procs (e.g. function_range) compile
   this file, expecting it to be a complete program with a main()
   function.  When IS_SHAREDLIB is NOT defined, we have main() as
   defined below.  */

#ifndef IS_SHAREDLIB
int
main ()
{
  B *b = foo ();
}
#endif
