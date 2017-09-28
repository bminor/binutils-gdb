/* Self tests for general utility routines for GDB, the GNU debugger.

   Copyright (C) 2016-2017 Free Software Foundation, Inc.

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

#include "common-defs.h"
#include "selftest.h"

namespace selftests {

static void
string_printf_tests ()
{
  SELF_CHECK (string_printf ("%s", "") == "");
  SELF_CHECK (string_printf ("%d comes before 2", 1) == "1 comes before 2");
  SELF_CHECK (string_printf ("hello %s", "world") == "hello world");

#define X10 "0123456789"
#define X100 X10 X10 X10 X10 X10 X10 X10 X10 X10 X10
#define X1000 X100 X100 X100 X100 X100 X100 X100 X100 X100 X100
#define X10000 X1000 X1000 X1000 X1000 X1000 X1000 X1000 X1000 X1000 X1000
#define X100000 X10000 X10000 X10000 X10000 X10000 X10000 X10000 X10000 X10000 X10000
  SELF_CHECK (string_printf ("%s", X10) == X10);
  SELF_CHECK (string_printf ("%s", X100) == X100);
  SELF_CHECK (string_printf ("%s", X1000) == X1000);
  SELF_CHECK (string_printf ("%s", X10000) == X10000);
  SELF_CHECK (string_printf ("%s", X100000) == X100000);
}

static std::string
format (const char *fmt, ...)
{
  va_list vp;

  va_start (vp, fmt);
  std::string result = string_vprintf (fmt, vp);
  va_end (vp);
  return result;
}

static void
string_vprintf_tests ()
{
  /* Basic smoke tests.  */
  SELF_CHECK (format ("%s", "test") == "test");
  SELF_CHECK (format ("%d", 23) == "23");
  SELF_CHECK (format ("%s %d %s", "test", 23, "done")
	      == "test 23 done");
  SELF_CHECK (format ("nothing") == "nothing");
}

} /* namespace selftests */

void
_initialize_common_utils_selftests ()
{
  selftests::register_test ("string_printf", selftests::string_printf_tests);
  selftests::register_test ("string_vprintf", selftests::string_vprintf_tests);
}
