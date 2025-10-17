/* Copyright (C) 2025 Free Software Foundation, Inc.

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

#include <stdlib.h>

#ifdef GEN_CORE
static int
crashfunc (void)
{
  abort ();
  return 0;
}
#endif

int
main (void)
{
#ifdef GEN_CORE
  int ret = crashfunc ();
#else
  int ret = 0;
#endif
  return ret;
}
