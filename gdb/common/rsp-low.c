/* Low-level RSP routines for GDB, the GNU debugger.

   Copyright (C) 1988-2014 Free Software Foundation, Inc.

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

#ifdef GDBSERVER
#include "server.h"
#else
#include "defs.h"
#endif

#include <string.h>

#include "rsp-low.h"

/* Convert hex digit A to a number.  */

int
fromhex (int a)
{
  if (a >= '0' && a <= '9')
    return a - '0';
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else if (a >= 'A' && a <= 'F')
    return a - 'A' + 10;
  else
    error (_("Reply contains invalid hex digit %d"), a);
}

int
tohex (int nib)
{
  if (nib < 10)
    return '0' + nib;
  else
    return 'a' + nib - 10;
}

/* Encode 64 bits in 16 chars of hex.  */

static const char hexchars[] = "0123456789abcdef";

static int
ishex (int ch, int *val)
{
  if ((ch >= 'a') && (ch <= 'f'))
    {
      *val = ch - 'a' + 10;
      return 1;
    }
  if ((ch >= 'A') && (ch <= 'F'))
    {
      *val = ch - 'A' + 10;
      return 1;
    }
  if ((ch >= '0') && (ch <= '9'))
    {
      *val = ch - '0';
      return 1;
    }
  return 0;
}

char *
pack_nibble (char *buf, int nibble)
{
  *buf++ = hexchars[(nibble & 0x0f)];
  return buf;
}

char *
pack_hex_byte (char *pkt, int byte)
{
  *pkt++ = hexchars[(byte >> 4) & 0xf];
  *pkt++ = hexchars[(byte & 0xf)];
  return pkt;
}

char *
unpack_varlen_hex (char *buff,	/* packet to parse */
		   ULONGEST *result)
{
  int nibble;
  ULONGEST retval = 0;

  while (ishex (*buff, &nibble))
    {
      buff++;
      retval = retval << 4;
      retval |= nibble & 0x0f;
    }
  *result = retval;
  return buff;
}

int
hex2bin (const char *hex, gdb_byte *bin, int count)
{
  int i;

  for (i = 0; i < count; i++)
    {
      if (hex[0] == 0 || hex[1] == 0)
	{
	  /* Hex string is short, or of uneven length.
	     Return the count that has been converted so far.  */
	  return i;
	}
      *bin++ = fromhex (hex[0]) * 16 + fromhex (hex[1]);
      hex += 2;
    }
  return i;
}

int
unhexify (char *bin, const char *hex, int count)
{
  int i;

  for (i = 0; i < count; i++)
    {
      if (hex[0] == 0 || hex[1] == 0)
	{
	  /* Hex string is short, or of uneven length.
	     Return the count that has been converted so far. */
	  return i;
	}
      *bin++ = fromhex (hex[0]) * 16 + fromhex (hex[1]);
      hex += 2;
    }
  return i;
}

void
convert_ascii_to_int (const char *from, unsigned char *to, int n)
{
  int nib1, nib2;
  while (n--)
    {
      nib1 = fromhex (*from++);
      nib2 = fromhex (*from++);
      *to++ = (((nib1 & 0x0f) << 4) & 0xf0) | (nib2 & 0x0f);
    }
}

int
bin2hex (const gdb_byte *bin, char *hex, int count)
{
  int i;

  for (i = 0; i < count; i++)
    {
      *hex++ = tohex ((*bin >> 4) & 0xf);
      *hex++ = tohex (*bin++ & 0xf);
    }
  *hex = 0;
  return i;
}

void
convert_int_to_ascii (const unsigned char *from, char *to, int n)
{
  int nib;
  int ch;
  while (n--)
    {
      ch = *from++;
      nib = ((ch & 0xf0) >> 4) & 0x0f;
      *to++ = tohex (nib);
      nib = ch & 0x0f;
      *to++ = tohex (nib);
    }
  *to++ = 0;
}

int
remote_escape_output (const gdb_byte *buffer, int len,
		      gdb_byte *out_buf, int *out_len,
		      int out_maxlen)
{
  int input_index, output_index;

  output_index = 0;
  for (input_index = 0; input_index < len; input_index++)
    {
      gdb_byte b = buffer[input_index];

      if (b == '$' || b == '#' || b == '}' || b == '*')
	{
	  /* These must be escaped.  */
	  if (output_index + 2 > out_maxlen)
	    break;
	  out_buf[output_index++] = '}';
	  out_buf[output_index++] = b ^ 0x20;
	}
      else
	{
	  if (output_index + 1 > out_maxlen)
	    break;
	  out_buf[output_index++] = b;
	}
    }

  *out_len = input_index;
  return output_index;
}

int
remote_unescape_input (const gdb_byte *buffer, int len,
		       gdb_byte *out_buf, int out_maxlen)
{
  int input_index, output_index;
  int escaped;

  output_index = 0;
  escaped = 0;
  for (input_index = 0; input_index < len; input_index++)
    {
      gdb_byte b = buffer[input_index];

      if (output_index + 1 > out_maxlen)
	error (_("Received too much data from the target."));

      if (escaped)
	{
	  out_buf[output_index++] = b ^ 0x20;
	  escaped = 0;
	}
      else if (b == '}')
	escaped = 1;
      else
	out_buf[output_index++] = b;
    }

  if (escaped)
    error (_("Unmatched escape character in target response."));

  return output_index;
}

