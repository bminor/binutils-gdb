/* Copyright (C) 1993 Free Software Foundation, Inc.

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

/* Mips hosts need aligned va_list arguments.  */

#define MAKEVA_ARG(list, argaddr, argsize) \
  { \
    int rounded_argsize; \
    if (argsize > 8) \
      /* Currently this never happens; printf_command only uses argsize */ \
      /* of sizeof (int), sizeof (double), or sizeof (long long).  */ \
      error ("MAKEVA_ARG not fully written for mips"); \
    if (argsize <= 4) \
      rounded_argsize = 4; \
    else if (argsize <= 8) \
      rounded_argsize = 8; \
    list->argindex = (list->argindex + rounded_argsize - 1) & -rounded_argsize; \
    memcpy (&list->aligner.arg_bytes[list->argindex], argaddr, argsize); \
    list->argindex += argsize; \
  }
