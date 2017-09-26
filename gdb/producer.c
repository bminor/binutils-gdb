/* Producer string parsers for GDB.

   Copyright (C) 2012-2017 Free Software Foundation, Inc.

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

#include "defs.h"
#include "producer.h"

/* See producer.h.  */

int
producer_is_gcc_ge_4 (const char *producer)
{
  int major, minor;

  if (! producer_is_gcc (producer, &major, &minor))
    return -1;
  if (major < 4)
    return -1;
  if (major > 4)
    return INT_MAX;
  return minor;
}

/* See producer.h.  */

int
producer_is_gcc (const char *producer, int *major, int *minor)
{
  const char *cs;

  if (producer != NULL && startswith (producer, "GNU "))
    {
      int maj, min;

      if (major == NULL)
	major = &maj;
      if (minor == NULL)
	minor = &min;

      /* Skip any identifier after "GNU " - such as "C11" "C++" or "Java".
	 A full producer string might look like:
	 "GNU C 4.7.2"
	 "GNU Fortran 4.8.2 20140120 (Red Hat 4.8.2-16) -mtune=generic ..."
	 "GNU C++14 5.0.0 20150123 (experimental)"
      */
      cs = &producer[strlen ("GNU ")];
      while (*cs && !isspace (*cs))
        cs++;
      if (*cs && isspace (*cs))
        cs++;
      if (sscanf (cs, "%d.%d", major, minor) == 2)
	return 1;
    }

  /* Not recognized as GCC.  */
  return 0;
}

