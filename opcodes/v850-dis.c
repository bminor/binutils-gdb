/* Disassemble V850 instructions.
   Copyright (C) 1996 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include <stdio.h>

#include "ansidecl.h"
#include "opcode/v850.h" 
#include "dis-asm.h"

int 
print_insn_v850 (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  int status;
  bfd_byte buffer[4];
  unsigned long insn;

  status = (*info->read_memory_func) (memaddr, buffer, 4, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
  insn = bfd_getl32 (buffer);

  disassemble (insn, info);
  if ((insn & 0x0600) == 0x0600)
    return 4;
  else
    return 2;
}

disassemble (insn, info)
     unsigned long insn;
     struct disassemble_info *info;
{
  struct v850_opcode *op = (struct v850_opcode *)v850_opcodes;
  int match = 0;
  /* If this is a two byte insn, then mask off the high bits. */
  if ((insn & 0x0600) != 0x0600)
    insn &= 0xffff;

  /* Find the opcode.  */
  while (op->name)
    {
      if ((op->mask & insn) == op->opcode)
	{
	  match = 1;
	  (*info->fprintf_func) (info->stream, "%s\t", op->name);
	  break;
	}
      op++;
    }

  if (!match)
    {
      if ((insn & 0x0600) != 0x0600)
	(*info->fprintf_func) (info->stream, ".short\t0x%04x", insn);
      else
	(*info->fprintf_func) (info->stream, ".long\t0x%08x", insn);
    }
}
