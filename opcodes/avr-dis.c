/* Disassemble AVR instructions.
   Copyright (C) 1999, 2000 Free Software Foundation, Inc.

   Contributed by Denis Chertykov <denisc@overta.ru>

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

#include <assert.h>
#include "sysdep.h"
#include "dis-asm.h"
#include "opintl.h"


struct avr_opcodes_s
{
  char *name;
  char *constraints;
  char *opcode;
  int insn_size;		/* in words */
  int isa;
  unsigned int bin_opcode;
  unsigned int bin_mask;
};

#define AVR_INSN(NAME, CONSTR, OPCODE, SIZE, ISA, BIN) \
{#NAME, CONSTR, OPCODE, SIZE, ISA, BIN, 0},

struct avr_opcodes_s avr_opcodes[] =
{
  #include "opcode/avr.h"
  {NULL, NULL, NULL, 0, 0, 0, 0}
};


static void avr_operand (unsigned int insn, unsigned int insn2,
			 unsigned int pc, int constraint, char *buf,
			 char *comment, int regs);

static void
avr_operand (insn, insn2, pc, constraint, buf, comment, regs)
     unsigned int insn;
     unsigned int insn2;
     unsigned int pc;
     int constraint;
     char *buf;
     char *comment;
     int regs;
{
  switch (constraint)
    {
      /* Any register operand.  */
    case 'r':
      if (regs)
	insn = (insn & 0xf) | ((insn & 0x0200) >> 5); /* source register */
      else
	insn = (insn & 0x01f0) >> 4; /* destination register */
      
      sprintf (buf, "r%d", insn);
      break;

    case 'd':
      if (regs)
	sprintf (buf, "r%d", 16 + (insn & 0xf));
      else
	sprintf (buf, "r%d", 16 + ((insn & 0xf0) >> 4));
      break;
      
    case 'w':
      sprintf (buf, "r%d", 24 + ((insn & 0x30) >> 3));
      break;
      
    case 'a':
      if (regs)
	sprintf (buf, "r%d", 16 + (insn & 7));
      else
	sprintf (buf, "r%d", 16 + ((insn >> 4) & 7));
      break;

    case 'v':
      if (regs)
	sprintf (buf, "r%d", (insn & 0xf) * 2);
      else
	sprintf (buf, "r%d", ((insn & 0xf0) >> 3));
      break;

    case 'e':
      if (insn & 0x2)
	*buf++ = '-';
      switch ((insn >> 2) & 0x3)
	{
	case 0: *buf++ = 'Z'; break;
	case 2: *buf++ = 'Y'; break;
	case 3: *buf++ = 'X'; break;
	default: buf += sprintf (buf, _ (" unknown register ")); break;
	}
      if (insn & 0x1)
	*buf++ = '+';
      *buf = '\0';
      break;

    case 'z':
      *buf++ = 'Z';
      if (insn & 0x1)
	*buf++ = '+';
      *buf = '\0';
      break;

    case 'b':
      {
	unsigned int x = insn;
	
	x = (insn & 7);
	x |= (insn >> 7) & (3 << 3);
	x |= (insn >> 8) & (1 << 5);
	
	if (insn & 0x8)
	  *buf++ = 'Y';
	else
	  *buf++ = 'Z';
	sprintf (buf, "+%d", x);
	sprintf (comment, "0x%02x", x);
      }
      break;
      
    case 'h':
      sprintf (buf, "0x%x",
	       ((((insn & 1) | ((insn & 0x1f0) >> 3)) << 16) | insn2) * 2);
      break;
      
    case 'L':
      {
	int rel_addr = (((insn & 0xfff) ^ 0x800) - 0x800) * 2;
	sprintf (buf, ".%+-8d", rel_addr);
	sprintf (comment, "0x%x", pc + 2 + rel_addr);
      }
      break;

    case 'l':
      {
	int rel_addr = ((((insn >> 3) & 0x7f) ^ 0x40) - 0x40) * 2;
	sprintf (buf, ".%+-8d", rel_addr);
	sprintf (comment, "0x%x", pc + 2 + rel_addr);
      }
      break;

    case 'i':
      sprintf (buf, "0x%04X", insn2);
      break;
      
    case 'M':
      sprintf (buf, "0x%02X", ((insn & 0xf00) >> 4) | (insn & 0xf));
      sprintf (comment, "%d", ((insn & 0xf00) >> 4) | (insn & 0xf));
      break;

    case 'n':
      sprintf (buf, _ ("Internal disassembler error"));
      break;
      
    case 'K':
      sprintf (buf, "%d", (insn & 0xf) | ((insn >> 2) & 0x30));
      break;
      
    case 's':
      sprintf (buf, "%d", insn & 7);
      break;
      
    case 'S':
      sprintf (buf, "%d", (insn >> 4) & 7);
      break;
      
    case 'P':
      {
	unsigned int x;
	x = (insn & 0xf);
	x |= (insn >> 5) & 0x30;
	sprintf (buf, "0x%02x", x);
	sprintf (comment, "%d", x);
      }
      break;

    case 'p':
      {
	unsigned int x;
	
	x = (insn >> 3) & 0x1f;
	sprintf (buf, "0x%02x", x);
	sprintf (comment, "%d", x);
      }
      break;
      
    case '?':
      *buf = '\0';
      break;
      
    default:
      sprintf (buf, _ ("unknown constraint `%c'"), constraint);
    }
}

static unsigned short avrdis_opcode PARAMS ((bfd_vma, disassemble_info *));

static unsigned short
avrdis_opcode (addr, info)
     bfd_vma addr;
     disassemble_info *info;
{
  bfd_byte buffer[2];
  int status;
  status = info->read_memory_func(addr, buffer, 2, info);
  if (status != 0)
    {
      info->memory_error_func(status, addr, info);
      return -1;
    }
  return bfd_getl16 (buffer);
}


int
print_insn_avr(addr, info)
     bfd_vma addr;
     disassemble_info *info;
{
  unsigned int insn, insn2;
  struct avr_opcodes_s *opcode;
  void *stream = info->stream;
  fprintf_ftype prin = info->fprintf_func;
  static int initialized;
  int cmd_len = 2;

  if (!initialized)
    {
      initialized = 1;
      
      for (opcode = avr_opcodes; opcode->name; opcode++)
	{
	  char * s;
	  unsigned int bin = 0;
	  unsigned int mask = 0;
	
	  for (s = opcode->opcode; *s; ++s)
	    {
	      bin <<= 1;
	      mask <<= 1;
	      bin |= (*s == '1');
	      mask |= (*s == '1' || *s == '0');
	    }
	  assert (s - opcode->opcode == 16);
	  assert (opcode->bin_opcode == bin);
	  opcode->bin_mask = mask;
	}
    }

  insn = avrdis_opcode (addr, info);
  
  for (opcode = avr_opcodes; opcode->name; opcode++)
    {
      if ((insn & opcode->bin_mask) == opcode->bin_opcode)
	break;
    }
  
  if (opcode->name)
    {
      char op1[20], op2[20], comment1[40], comment2[40];
      char *op = opcode->constraints;

      op1[0] = 0;
      op2[0] = 0;
      comment1[0] = 0;
      comment2[0] = 0;

      if (opcode->insn_size > 1)
	{
	  insn2 = avrdis_opcode (addr + 2, info);
	  cmd_len = 4;
	}

      if (*op && *op != '?')
	{
	  int regs = REGISTER_P (*op);

	  avr_operand (insn, insn2, addr, *op, op1, comment1, 0);

	  if (*(++op) == ',')
	    avr_operand (insn, insn2, addr, *(++op), op2,
			 *comment1 ? comment2 : comment1, regs);
	}

      (*prin) (stream, "    %-8s", opcode->name);

      if (*op1)
	(*prin) (stream, "%s", op1);

      if (*op2)
	(*prin) (stream, ", %s", op2);

      if (*comment1)
	(*prin) (stream, "\t; %s", comment1);

      if (*comment2)
	(*prin) (stream, " %s", comment2);
    }
  else
   (*prin) (stream, ".word 0x%04x\t; ????", insn);
  
  return cmd_len;
}
