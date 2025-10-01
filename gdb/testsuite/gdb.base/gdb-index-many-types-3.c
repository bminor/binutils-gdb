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

#include "gdb-index-many-types.h"

typedef struct woof_t
{
  double woof_t_3;
  double woof_t_4;
} woof_t;

static void
woof_func (woof_t *obj)
{
  (void) obj;
}

typedef struct bar_t
{
  int bar_t_1;
  int bar_t_2;
} bar_t;

void
bar_func_b (bar_t *obj)
{
  woof_func (0);
  (void) obj;
}

void
baz_func_b (baz_t *obj)
{
  (void) obj;
}

void
foo_func_b (foo_t *obj)
{
  woof_func (0);
  (void) obj;
}
