/* Instruction printing code for the AMD 29000
   Copyright (C) 1993 Free Software Foundation, Inc.
   Contributed by Cygnus Support.  Written by Jim Kingdon.

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
#include "dis-asm.h"

/* Print the m68k instruction at address MEMADDR in debugged memory,
   on STREAM.  Returns length of the instruction, in bytes.  */

int
print_insn (memaddr, stream)
     CORE_ADDR memaddr;
     GDB_FILE *stream;
{
  disassemble_info info;

  GDB_INIT_DISASSEMBLE_INFO(info, stream);

#if TARGET_BYTE_ORDER == BIG_ENDIAN
  return print_insn_big_a29k (memaddr, &info);
#else
  return print_insn_little_a29k (memaddr, &info);
#endif
}
