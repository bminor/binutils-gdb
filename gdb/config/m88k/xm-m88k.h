/* Host-machine dependent parameters for Motorola 88000, for GDB.
   Copyright 1993 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define HOST_BYTE_ORDER BIG_ENDIAN

#define MAKEVA_END(list) \
  va_list retval; \
  retval.__va_arg = 0; \
  retval.__va_stk = (int *) (list)->aligner.arg_bytes; \
  retval.__va_reg = (int *) (list)->aligner.arg_bytes; \
  return retval;

/* I don't know whether rounding the arguments to 4 or 8 bytes is correct
   for the 88k, or whether it is just code borrowed from the pa.  The issue
   is moot for now, since printf_command only uses argsize of sizeof (int),
   sizeof (double), or sizeof (long long).  */
#define MAKEVA_ARG(list, argaddr, argsize) \
  { \
    int rounded_argsize; \
    if (argsize > 8) \
      /* Currently this never happens; printf_command only uses argsize */ \
      /* of sizeof (int), sizeof (double), or sizeof (long long).  */ \
      error ("MAKEVA_ARG not fully written for m88k"); \
    if (argsize <= 4) \
      rounded_argsize = 4; \
    else if (argsize <= 8) \
      rounded_argsize = 8; \
    while ((int)(&list->aligner.arg_bytes[list->argindex]) % rounded_argsize) \
      list->argindex++; \
    memcpy (&list->aligner.arg_bytes[list->argindex], argaddr, argsize); \
    list->argindex += rounded_argsize; \
  }
