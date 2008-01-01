/* Decimal floating point support for GDB.

   Copyright 2007, 2008 Free Software Foundation, Inc.

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

/* The order of the following headers is important for making sure
   decNumber structure is large enough to hold decimal128 digits.  */

#include "dpd/decimal128.h"
#include "dpd/decimal64.h"
#include "dpd/decimal32.h"

/* In GDB, we are using an array of gdb_byte to represent decimal values.
   They are stored in host byte order.  This routine does the conversion if
   the target byte order is different.  */
static void
match_endianness (const gdb_byte *from, int len, gdb_byte *to)
{
  int i;

#if WORDS_BIGENDIAN
#define OPPOSITE_BYTE_ORDER BFD_ENDIAN_LITTLE
#else
#define OPPOSITE_BYTE_ORDER BFD_ENDIAN_BIG
#endif

  if (gdbarch_byte_order (current_gdbarch) == OPPOSITE_BYTE_ORDER)
    for (i = 0; i < len; i++)
      to[i] = from[len - i - 1];
  else
    for (i = 0; i < len; i++)
      to[i] = from[i];

  return;
}

/* Convert decimal type to its string representation.  LEN is the length
   of the decimal type, 4 bytes for decimal32, 8 bytes for decimal64 and
   16 bytes for decimal128.  */
void
decimal_to_string (const gdb_byte *decbytes, int len, char *s)
{
  gdb_byte dec[16];

  match_endianness (decbytes, len, dec);
  switch (len)
    {
      case 4:
	decimal32ToString ((decimal32 *) dec, s);
	break;
      case 8:
	decimal64ToString ((decimal64 *) dec, s);
	break;
      case 16:
	decimal128ToString ((decimal128 *) dec, s);
	break;
      default:
	error (_("Unknown decimal floating point type."));
	break;
    }
}

/* Convert the string form of a decimal value to its decimal representation.
   LEN is the length of the decimal type, 4 bytes for decimal32, 8 bytes for
   decimal64 and 16 bytes for decimal128.  */
int
decimal_from_string (gdb_byte *decbytes, int len, const char *string)
{
  decContext set;
  gdb_byte dec[16];

  switch (len)
    {
      case 4:
	decContextDefault (&set, DEC_INIT_DECIMAL32);
	set.traps = 0;
	decimal32FromString ((decimal32 *) dec, string, &set);
	break;
      case 8:
	decContextDefault (&set, DEC_INIT_DECIMAL64);
	set.traps = 0;
	decimal64FromString ((decimal64 *) dec, string, &set);
	break;
      case 16:
	decContextDefault (&set, DEC_INIT_DECIMAL128);
	set.traps = 0;
	decimal128FromString ((decimal128 *) dec, string, &set);
	break;
      default:
	error (_("Unknown decimal floating point type."));
	break;
    }

  match_endianness (dec, len, decbytes);

  return 1;
}
