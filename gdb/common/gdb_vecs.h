/* Some commonly-used VEC types.

   Copyright (C) 2012-2018 Free Software Foundation, Inc.

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

#ifndef GDB_VECS_H
#define GDB_VECS_H

#include "vec.h"

typedef char *char_ptr;
typedef const char *const_char_ptr;

DEF_VEC_P (char_ptr);

DEF_VEC_P (const_char_ptr);

extern void free_char_ptr_vec (VEC (char_ptr) *char_ptr_vec);

/* Split STR, a list of DELIMITER-separated fields, into a char pointer vector.

   You may modify the returned strings.  */

extern std::vector<gdb::unique_xmalloc_ptr<char>>
  delim_string_to_char_ptr_vec (const char *str, char delimiter);

/* Like dirnames_to_char_ptr_vec, but append the directories to *VECP.  */

extern void dirnames_to_char_ptr_vec_append
  (std::vector<gdb::unique_xmalloc_ptr<char>> *vecp, const char *dirnames);

/* Split DIRNAMES by DIRNAME_SEPARATOR delimiter and return a list of all the
   elements in their original order.  For empty string ("") DIRNAMES return
   list of one empty string ("") element.

   You may modify the returned strings.  */

extern std::vector<gdb::unique_xmalloc_ptr<char>>
  dirnames_to_char_ptr_vec (const char *dirnames);

#endif /* GDB_VECS_H */
