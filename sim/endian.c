/* hostinfo.c
   Copyright (C) 1992, 1993 Free Software Foundation, Inc.

This file is part of Z8KSIM

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* This prog writes a header file with some host info in it */

union {
  int  as_int;
  char as_char[4];
} u;


int
main()
{
  u.as_int = 0x01020304;
  if (u.as_char[0] == 0x01) 
  {
    printf("#define BIG_ENDIAN_HOST\n");
  }
  else if (u.as_char[0] == 0x04) 
  {
    printf("#define LITTLE_ENDIAN_HOST\n");
  }

  return 0;
  
}
