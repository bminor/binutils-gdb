/* Disassemble D10V instructions.
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

#include "opcode/d10v.h" 
#include "dis-asm.h"

static void dis_2_short PARAMS ((unsigned long insn, char *str, int order));
static void dis_long PARAMS ((unsigned long insn, char *str));

int 
print_insn_d10v (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  int status;
  bfd_byte buffer[4];
  unsigned long insn;
  char str[64];

  strcpy (str, "unknown");

  status = (*info->read_memory_func) (memaddr, buffer, 4, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
  insn = bfd_getb32 (buffer);

  status = insn & FM11;
  switch (status) {
  case 0:
    dis_2_short (insn, str, 2);
    break;
  case FM01:
    dis_2_short (insn, str, 0);
    break;
  case FM10:
    dis_2_short (insn, str, 1);
    break;
  case FM11:
    dis_long (insn, str);
    break;
  }
  (*info->fprintf_func) (info->stream, "\t%s", str, insn);  
  return 4;
}

static void
print_operand (buf, oper, insn, op)
     char *buf;
     struct d10v_operand *oper;
     unsigned long insn;
     struct d10v_opcode *op;
{
  int num, shift;

  if (oper->flags == OPERAND_ATMINUS)
    {
      strcpy (buf,"@-");
      return;
    }
  if (oper->flags == OPERAND_MINUS)
    {
      strcpy (buf,"-");
      return;
    }
  if (oper->flags == OPERAND_PLUS)
    {
      strcpy (buf,"+");
      return;
    }
  if (oper->flags == OPERAND_ATSIGN)
    {
      strcpy (buf,"@");
      return;
    }
  if (oper->flags == OPERAND_ATPAR)
    {
      strcpy (buf,"@(");
      return;
    }

  shift = oper->shift;

  /* the LONG_L format shifts registers over by 15 */
  if (op->format == LONG_L && (oper->flags & OPERAND_REG))
    shift += 15;

  num = (insn >> shift) & (0x7FFFFFFF >> (31 - oper->bits));

  if (oper->flags & OPERAND_REG)
    {
      int i;
      int match=0;
      num += oper->flags & (OPERAND_ACC|OPERAND_FLAG|OPERAND_CONTROL);
      for (i=0;i<reg_name_cnt();i++)
	{
	  if (num == pre_defined_registers[i].value)
	    {
	      if (pre_defined_registers[i].pname)
		strcpy(buf,pre_defined_registers[i].pname);
	      else
		strcpy(buf,pre_defined_registers[i].name);
	      match=1;
	      break;
	    }
	}
      if (match==0)
	{
	  if (oper->flags & OPERAND_ACC)
	    *buf++ = 'a';
	  else if (oper->flags & OPERAND_CONTROL)
	    {
	      *buf++ ='c';
	      *buf++ ='r';
	    }
	  else if(oper->flags & OPERAND_REG)
	    *buf++ = 'r';
	  sprintf (buf, "%d", num);
	}
    }
  else
    sprintf (buf, "0x%x", num);
}


static void
dis_long (insn, str)
     unsigned long insn;
     char *str;
{
  int i;
  char buf[32];
  struct d10v_opcode *op = (struct d10v_opcode *)d10v_opcodes;
  struct d10v_operand *oper;
  int need_paren = 0;

  while (op->name)
    {
      if ((op->format & LONG_OPCODE) && ((op->mask & insn) == op->opcode))
	{
	  strcpy (str, op->name);
	  strcat (str, "\t");
	  for ( i=0; op->operands[i]; i++)
	    {
	      oper = (struct d10v_operand *)&d10v_operands[op->operands[i]];
	      if (oper->flags == OPERAND_ATPAR)
		need_paren = 1;
	      print_operand (buf, oper, insn, op);
	      strcat (str, buf);
	      if (op->operands[i+1] && oper->bits &&
		  d10v_operands[op->operands[i+1]].flags != OPERAND_PLUS &&
		  d10v_operands[op->operands[i+1]].flags != OPERAND_MINUS)
		strcat (str,", ");
	    }
	  break;
	}
      op++;
    }
  if (need_paren)
    strcat (str, ")");
}

static void
dis_2_short (insn, str, order)
     unsigned long insn;
     char *str;
     int order;
{
  int i,j;
  char astr[2][32];
  unsigned int ins[2];
  struct d10v_opcode *op;
  char buf[32];
  int match, num_match=0;
  struct d10v_operand *oper;
  int need_paren = 0;

  ins[0] = (insn & 0x3FFFFFFF) >> 15;
  ins[1] = insn & 0x00007FFF;

  *str = 0;

  for(j=0;j<2;j++)
    {
      op = (struct d10v_opcode *)d10v_opcodes;
      match=0;
      while (op->name)
	{
	  if ((op->format & SHORT_OPCODE) && ((op->mask & ins[j]) == op->opcode))
	    {
	      strcat (str, op->name);
	      strcat (str, "\t");
	      for (i=0; op->operands[i]; i++)
		{
		  oper = (struct d10v_operand *)&d10v_operands[op->operands[i]];
		  if (oper->flags == OPERAND_ATPAR)
		    need_paren = 1;
		  print_operand (buf, oper, ins[j], op);
		  strcat (str, buf);
		  if (op->operands[i+1] && oper->bits && 
		  d10v_operands[op->operands[i+1]].flags != OPERAND_PLUS &&
		  d10v_operands[op->operands[i+1]].flags != OPERAND_MINUS)
		    strcat( str,", ");
		}
	      match = 1;
	      num_match++;
	      break;
	    }
	  op++;
	}
      if (!match)
	  strcat (str, "unknown");

      switch (order)
	{
	case 0:
	  strcat ( str, "\t->\t");
	  order = -1;
	  break;
	case 1:
	  strcat (str, "\t<-\t");
	  order = -1;
	  break;
	case 2:
	  strcat (str, "\t||\t");
	  order = -1;
	  break;
	default:
	  break;
	}
    }

  if (num_match == 0)
    sprintf (str, ".long\t0x%08x", insn);

  if (need_paren)
    strcat (str, ")");
}
