/* Copyright (C) 2024-2025 Free Software Foundation, Inc.

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

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))

/* Use uintptr_t as a general purpose integer type to reduce the number of
   types we need to define in the DWARF, and to make arithmetic simpler.  */

struct array_type
{
  uintptr_t *data;
  uintptr_t count;
  uintptr_t lower_bound;
};

struct outer_type
{
  struct array_type assoc;
  struct array_type non_assoc;
};

/* We spotted some code paths in GDB that would confuse the size of the
   `.assoc` array with the size of the `.assoc` field within `g_outer_var`,
   which is bogus.  Make the array quite large, to make any such bugs more
   apparent.  */

static uintptr_t g_outer_var_assoc_data[]
  = { 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
      18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
      35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
      52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,
      69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
      86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100 };

struct outer_type g_outer_var = {
  .assoc = {
    .data = g_outer_var_assoc_data,
    .count = ARRAY_SIZE (g_outer_var_assoc_data),
    .lower_bound = 6,
  },

  .non_assoc = {
    .data = NULL,
    .count = INT_MAX,
    .lower_bound = INT_MAX,
  },
};

int
main ()
{}
