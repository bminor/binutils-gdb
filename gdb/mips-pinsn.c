/* Print mips instructions for GDB, the GNU debugger.
   Copyright 1989, 1991, 1992, 1993 Free Software Foundation, Inc.

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

#include "defs.h"

/* Mips instructions are never longer than this many bytes.  */
#define MAXLEN 4

/* Print the mips instruction at address MEMADDR in debugged memory,
   on STREAM.  Returns length of the instruction, in bytes, which
   is always 4.  */

int
print_insn (memaddr, stream)
     CORE_ADDR memaddr;
     FILE *stream;
{
  unsigned char buffer[MAXLEN];

  read_memory (memaddr, buffer, MAXLEN);

  /* print_insn_mips is in opcodes/mips-dis.c.  */
  return print_insn_mips (memaddr, buffer, stream,
			  TARGET_BYTE_ORDER == BIG_ENDIAN);
}
