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

static const char *const v850_reg_names[] =
{ "r0", "r1", "r2", "sp", "gp", "r5", "r6", "r7", 
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", 
  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", 
  "r24", "r25", "r26", "r27", "r28", "r29", "ep", "r31" };

static const char *const v850_sreg_names[] =
{ "eipc", "eipsw", "fepc", "fepsw", "ecr", "psw", "sr6", "sr7", 
  "sr8", "sr9", "sr10", "sr11", "sr12", "sr13", "sr14", "sr15", 
  "sr16", "sr17", "sr18", "sr19", "sr20", "sr21", "sr22", "sr23", 
  "sr24", "sr25", "sr26", "sr27", "sr28", "sr29", "sr30", "sr31" };

static const char *const v850_cc_names[] =
{ "v", "c/l", "z", "nh", "s/n", "t", "lt", "le", 
  "nv", "nc/nl", "nz", "h", "ns/p", "sa", "ge", "gt" };

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
  const struct v850_operand *operand;
  int match = 0;
  /* If this is a two byte insn, then mask off the high bits. */
  if ((insn & 0x0600) != 0x0600)
    insn &= 0xffff;

  /* Find the opcode.  */
  while (op->name)
    {
      if ((op->mask & insn) == op->opcode)
	{
	  const unsigned char *opindex_ptr;

	  match = 1;
	  (*info->fprintf_func) (info->stream, "%s\t", op->name);

	  /* Now print the operands.  */
	  for (opindex_ptr = op->operands; *opindex_ptr != 0; opindex_ptr++)
	    {
	      unsigned long value;

	      operand = &v850_operands[*opindex_ptr];

	      if (operand->extract)
		value = (operand->extract) (insn, 0);
	      else
		value = (insn >> operand->shift) & ((1 << operand->bits) - 1);

	      if ((operand->flags & V850_OPERAND_SIGNED) != 0)
		value = ((signed long)(value << (32 - operand->bits))
			  >> (32 - operand->bits));
	      if ((operand->flags & V850_OPERAND_REG) != 0)
		(*info->fprintf_func) (info->stream, "%s",
				      v850_reg_names[value]);
	      else if ((operand->flags & V850_OPERAND_SRG) != 0)
		(*info->fprintf_func) (info->stream, "%s",
				      v850_sreg_names[value]);
	      else if ((operand->flags & V850_OPERAND_CC) != 0)
		(*info->fprintf_func) (info->stream, "%s",
				      v850_cc_names[value]);
	      else if ((operand->flags & V850_OPERAND_EP) != 0)
		(*info->fprintf_func) (info->stream, "ep");
	      else
		(*info->fprintf_func) (info->stream, "%d", value);
	    }

	  /* All done. */
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
