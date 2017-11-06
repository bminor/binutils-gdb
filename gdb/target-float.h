/* Floating point definitions for GDB.

   Copyright (C) 1986-2017 Free Software Foundation, Inc.

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

#ifndef TYPED_FLOAT_H
#define TYPED_FLOAT_H

extern bool target_float_is_valid (const gdb_byte *addr,
				   const struct type *type);
extern bool target_float_is_zero (const gdb_byte *addr,
				  const struct type *type);

extern std::string target_float_to_string (const gdb_byte *addr,
					   const struct type *type,
					   const char *format = nullptr);
extern bool target_float_from_string (gdb_byte *addr,
				      const struct type *type,
				      const std::string &string);

#endif
