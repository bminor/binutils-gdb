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

#ifndef GDB_INDEX_MANY_TYPES_H
#define GDB_INDEX_MANY_TYPES_H

typedef struct foo_t foo_t;
typedef struct bar_t bar_t;

extern void foo_func_a (foo_t *obj);
extern void foo_func_b (foo_t *obj);
extern void foo_func_c (foo_t *obj);

extern void bar_func_a (bar_t *obj);
extern void bar_func_b (bar_t *obj);
extern void bar_func_c (bar_t *obj);

typedef struct baz_t
{
  int baz_t_1;
  int baz_t_2;
} baz_t;

extern void baz_func_a (baz_t *obj);
extern void baz_func_b (baz_t *obj);
extern void baz_func_c (baz_t *obj);

#endif /* GDB_INDEX_MANY_TYPES_H */
