/* Filter stdin to a suitable C string literal.
   Copyright (C) 1991 Free Software Foundation, Inc.
   
This file is part of GLD, the Gnu Linker.

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

#include <stdio.h>

int
main() 
{
  int ch;
  ch = getchar();
  printf("/* Generated through mkscript */\n");
  printf("\"{ \\\n");
  while (ch != EOF) {
    if (ch == '\"' || ch == '\\' || ch == '\'') {
      putchar('\\');
      putchar(ch);
    }
    else { if (ch == '\n')
      fputs("\\n\\", stdout);
    putchar(ch);
     }
    ch = getchar();
  }
  printf("}\"\n");
  return 0;
}
