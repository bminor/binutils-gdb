/* Copyright (C) 2017-2018 Free Software Foundation, Inc.

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

#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#ifdef __GNUC__
# define DIAGNOSTIC_PUSH _Pragma ("GCC diagnostic push")
# define DIAGNOSTIC_POP _Pragma ("GCC diagnostic pop")

/* Stringification.  */
# define DIAGNOSTIC_STRINGIFY_1(x) #x
# define DIAGNOSTIC_STRINGIFY(x) DIAGNOSTIC_STRINGIFY_1 (x)

# define DIAGNOSTIC_IGNORE(option) \
  _Pragma (DIAGNOSTIC_STRINGIFY (GCC diagnostic ignored option))
#else
# define DIAGNOSTIC_PUSH
# define DIAGNOSTIC_POP
# define DIAGNOSTIC_IGNORE(option)
#endif

#if defined (__clang__) /* clang */

# define DIAGNOSTIC_IGNORE_SELF_MOVE DIAGNOSTIC_IGNORE ("-Wself-move")
# define DIAGNOSTIC_IGNORE_DEPRECATED_REGISTER \
  DIAGNOSTIC_IGNORE ("-Wdeprecated-register")
# define DIAGNOSTIC_IGNORE_UNUSED_FUNCTION \
  DIAGNOSTIC_IGNORE ("-Wunused-function")
# if __has_warning ("-Wenum-compare-switch")
#  define DIAGNOSTIC_IGNORE_SWITCH_DIFFERENT_ENUM_TYPES \
   DIAGNOSTIC_IGNORE ("-Wenum-compare-switch")
# endif
#elif defined (__GNUC__) /* GCC */

# define DIAGNOSTIC_IGNORE_UNUSED_FUNCTION \
  DIAGNOSTIC_IGNORE ("-Wunused-function")

# define DIAGNOSTIC_IGNORE_STRINGOP_TRUNCATION \
  DIAGNOSTIC_IGNORE ("-Wstringop-truncation")
#endif

#ifndef DIAGNOSTIC_IGNORE_SELF_MOVE
# define DIAGNOSTIC_IGNORE_SELF_MOVE
#endif

#ifndef DIAGNOSTIC_IGNORE_DEPRECATED_REGISTER
# define DIAGNOSTIC_IGNORE_DEPRECATED_REGISTER
#endif

#ifndef DIAGNOSTIC_IGNORE_UNUSED_FUNCTION
# define DIAGNOSTIC_IGNORE_UNUSED_FUNCTION
#endif

#ifndef DIAGNOSTIC_IGNORE_SWITCH_DIFFERENT_ENUM_TYPES
# define DIAGNOSTIC_IGNORE_SWITCH_DIFFERENT_ENUM_TYPES
#endif

#ifndef DIAGNOSTIC_IGNORE_STRINGOP_TRUNCATION
# define DIAGNOSTIC_IGNORE_STRINGOP_TRUNCATION
#endif

#endif /* DIAGNOSTICS_H */
