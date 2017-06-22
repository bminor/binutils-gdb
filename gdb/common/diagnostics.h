/* Copyright (C) 2017 Free Software Foundation, Inc.

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

#ifndef COMMON_DIAGNOSTICS_H
#define COMMON_DIAGNOSTICS_H

#include "common/preprocessor.h"

#ifdef __GNUC__
# define DIAGNOSTIC_PUSH _Pragma ("GCC diagnostic push")
# define DIAGNOSTIC_POP _Pragma ("GCC diagnostic pop")
# define DIAGNOSTIC_IGNORE(option) \
  _Pragma (STRINGIFY (GCC diagnostic ignored option))
#else
# define DIAGNOSTIC_PUSH
# define DIAGNOSTIC_POP
# define DIAGNOSTIC_IGNORE(option)
#endif

#ifdef __clang__
# define DIAGNOSTIC_IGNORE_SELF_MOVE DIAGNOSTIC_IGNORE ("-Wself-move")
#else
# define DIAGNOSTIC_IGNORE_SELF_MOVE
#endif

#endif /* COMMON_DIAGNOSTICS_H */
