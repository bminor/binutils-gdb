/* Print TI TMS320C80 (MVP) instructions
   Copyright 1996 Free Software Foundation, Inc.

This file is free software; you can redistribute it and/or modify
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
#include "opcode/tic80.h"
#include "dis-asm.h"

int 
print_insn_tic80 (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  bfd_byte buffer[4];
  int status;
  unsigned long insn[2];
  const struct tic80_opcode *opcode;
  const struct tic80_opcode *opcode_end;
  const unsigned char *opindex;
  const struct tic80_operand *operand;
  int need_comma;
  int need_paren;
  int length = 4;

  status = (*info->read_memory_func) (memaddr, buffer, 4, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }

  if (info -> endian == BFD_ENDIAN_LITTLE)
    {
      insn[0] = bfd_getl32 (buffer);
    }
  else if (info -> endian == BFD_ENDIAN_BIG)
    {
      insn[0] = bfd_getb32 (buffer);
    }
  else
    {
      /* FIXME: Should probably just default to one or the other */
      abort ();
    }

  /* Find the first opcode match in the opcodes table.  FIXME: there should
     be faster ways to find one (hash table or binary search), but don't
     worry too much about it until other TIc80 support is finished. */

  opcode_end = tic80_opcodes + tic80_num_opcodes;
  for (opcode = tic80_opcodes; opcode < opcode_end; opcode++)
    {
      if ((insn[0] & opcode -> mask) == opcode -> opcode)
	{
	  break;
	}
    }

  if (opcode == opcode_end)
    {
      /* No match found, just print the bits as a .word directive */
      (*info -> fprintf_func) (info -> stream, ".word %#08lx", insn[0]);
    }
  else
    {
      /* Match found, decode the instruction.  */
      (*info -> fprintf_func) (info -> stream, "%s", opcode -> name);

      /* Now extract and print the operands. */
      need_comma = 0;
      need_paren = 0;
      if (opcode -> operands[0] != 0)
	{
	  (*info -> fprintf_func) (info -> stream, "\t");
	}
      for (opindex = opcode -> operands; *opindex != 0; opindex++)
	{
	  long value;

	  operand = tic80_operands + *opindex;

	  /* Extract the value from the instruction.  */
	  if (operand -> extract)
	    {
	      value = (*operand -> extract) (insn[0], (int *) NULL);
	    }
	  else if (operand -> bits == 32)
	    {
	      status = (*info->read_memory_func) (memaddr + 4, buffer, 4, info);
	      if (status != 0)
		{
		  (*info->memory_error_func) (status, memaddr, info);
		  return -1;
		}

	      if (info -> endian == BFD_ENDIAN_LITTLE)
		{
		  insn[1] = bfd_getl32 (buffer);
		}
	      else if (info -> endian == BFD_ENDIAN_BIG)
		{
		  insn[1] = bfd_getb32 (buffer);
		}
	      value = (long) insn[1];
	      length += 4;
	    }
	  else
	    {
	      value = (insn[0] >> operand -> shift) & ((1 << operand -> bits) - 1);
	      if ((operand -> flags & TIC80_OPERAND_SIGNED) != 0
		  && (value & (1 << (operand -> bits - 1))) != 0)
		value -= 1 << operand -> bits;
	    }

	  if (need_comma)
	    {
	      (*info -> fprintf_func) (info -> stream, ",");
	      need_comma = 0;
	    }

	  /* Print the operand as directed by the flags.  */
	  if ((operand -> flags & TIC80_OPERAND_GPR) != 0)
	    {
	      (*info -> fprintf_func) (info -> stream, "r%ld", value);
	    }
	  else if ((operand -> flags & TIC80_OPERAND_FPA) != 0)
	    {
	      (*info -> fprintf_func) (info -> stream, "a%ld", value);
	    }
	  else if ((operand -> flags & TIC80_OPERAND_RELATIVE) != 0)
	    {
	      (*info -> print_address_func) (memaddr + value, info);
	    }
	  else if ((operand -> flags & TIC80_OPERAND_CC_SZ) != 0)
	    {
#if 0	/* FIXME */	      
	      if (operand -> bits == 3)
		(*info -> fprintf_func) (info -> stream, "cr%d", value);
	      else
		{
		  static const char *cbnames[4] = { "lt", "gt", "eq", "so" };
		  int cr;
		  int cc;

		  cr = value >> 2;
		  if (cr != 0)
		    (*info -> fprintf_func) (info -> stream, "4*cr%d", cr);
		  cc = value & 3;
		  if (cc != 0)
		    {
		      if (cr != 0)
			(*info -> fprintf_func) (info -> stream, "+");
		      (*info -> fprintf_func) (info -> stream, "%s", cbnames[cc]);
		    }
		}
#endif
	    }
	  else
	    {
	      if ((value > 999 || value < -999)
		  || operand -> flags & TIC80_OPERAND_BITFIELD)
		{
		  (*info -> fprintf_func) (info -> stream, "%#lx", value);
		}
	      else
		{
		  (*info -> fprintf_func) (info -> stream, "%ld", value);
		}
	    }

	  if (need_paren)
	    {
	      (*info -> fprintf_func) (info -> stream, ")");
	      need_paren = 0;
	    }

	  if ((operand -> flags & TIC80_OPERAND_PARENS) == 0)
	    {
	      need_comma = 1;
	    }
	  else
	    {
	      (*info -> fprintf_func) (info -> stream, "(");
	      need_paren = 1;
	    }
	}
    }

  return (length);
}
