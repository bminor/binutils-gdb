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

#ifndef COMMON_RSP_LOW_H
#define COMMON_RSP_LOW_H

/* Convert hex digit A to a number, or throw an exception.  */

extern int fromhex (int a);

/* Convert number NIB to a hex digit.  */

extern int tohex (int nib);

extern char *pack_nibble (char *buf, int nibble);

extern char *pack_hex_byte (char *pkt, int byte);

extern char *unpack_varlen_hex (char *buff, ULONGEST *result);

extern int hex2bin (const char *hex, gdb_byte *bin, int count);

extern int unhexify (char *bin, const char *hex, int count);

extern void convert_ascii_to_int (const char *from, unsigned char *to, int n);

extern int bin2hex (const gdb_byte *bin, char *hex, int count);

/* Convert BUFFER, binary data at least LEN bytes long, into escaped
   binary data in OUT_BUF.  Set *OUT_LEN to the length of the data
   encoded in OUT_BUF, and return the number of bytes in OUT_BUF
   (which may be more than *OUT_LEN due to escape characters).  The
   total number of bytes in the output buffer will be at most
   OUT_MAXLEN.  This function properly escapes '*', and so is suitable
   for the server side as well as the client.  */

extern int remote_escape_output (const gdb_byte *buffer, int len,
				 gdb_byte *out_buf, int *out_len,
				 int out_maxlen);

/* Convert BUFFER, escaped data LEN bytes long, into binary data
   in OUT_BUF.  Return the number of bytes written to OUT_BUF.
   Raise an error if the total number of bytes exceeds OUT_MAXLEN.

   This function reverses remote_escape_output.  */

extern int remote_unescape_input (const gdb_byte *buffer, int len,
				  gdb_byte *out_buf, int out_maxlen);

#endif /* COMMON_RSP_LOW_H */
