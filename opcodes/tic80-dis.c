/* Print TI TMS320C80 (MVP) instructions
   Copyright 1996, 1997 Free Software Foundation, Inc.

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

static int length;

static void print_operand_bitnum PARAMS ((struct disassemble_info *, long));
static void print_operand_condition_code PARAMS ((struct disassemble_info *, long));
static void print_operand_control_register PARAMS ((struct disassemble_info *, long));
static void print_operand_float PARAMS ((struct disassemble_info *, long));
static void print_operand_integer PARAMS ((struct disassemble_info *, long));
static void print_operand PARAMS ((struct disassemble_info *, long, unsigned long,
				   const struct tic80_operand *, bfd_vma));
static int print_one_instruction PARAMS ((struct disassemble_info *, bfd_vma,
				      unsigned long, const struct tic80_opcode *));
static int print_instruction PARAMS ((struct disassemble_info *, bfd_vma, unsigned long,
				      const struct tic80_opcode *));
static int fill_instruction PARAMS ((struct disassemble_info *, bfd_vma,
				     unsigned long *));


/* Print an integer operand.  Try to be somewhat smart about the
   format by assuming that small positive or negative integers are
   probably loop increment values, structure offsets, or similar
   values that are more meaningful printed as signed decimal values.
   Larger numbers are probably better printed as hex values. */

static void
print_operand_integer (info, value)
     struct disassemble_info *info;
     long value;
{
  if ((value > 9999 || value < -9999))
    {
      (*info -> fprintf_func) (info -> stream, "%#lx", value);
    }
  else
    {
      (*info -> fprintf_func) (info -> stream, "%ld", value);
    }
}


/* FIXME: depends upon sizeof (long) == sizeof (float) and
   also upon host floating point format matching target
   floating point format. */

static void
print_operand_float (info, value)
     struct disassemble_info *info;
     long value;
{
  union { float f; long l; } fval;

  fval.l = value;
  (*info -> fprintf_func) (info -> stream, "%g", fval.f);
}


static void
print_operand_control_register (info, value)
     struct disassemble_info *info;
     long value;
{
  char *tmp;

  switch (value)
    {
    case 0:		tmp = "EPC";		break;
    case 1:		tmp = "EIP";		break;
    case 2:		tmp = "CONFIG";		break;
    case 4:		tmp = "INTPEN";		break;
    case 6:		tmp = "IE";		break;
    case 8:		tmp = "FPST";		break;
    case 0xA:		tmp = "PPERROR";	break;
    case 0xD:		tmp = "PKTREQ";		break;
    case 0xE:		tmp = "TCOUNT";		break;
    case 0xF:		tmp = "TSCALE";		break;
    case 0x10:		tmp = "FLTOP";		break;
    case 0x11:		tmp = "FLTADR";		break;
    case 0x12:		tmp = "FLTTAG";		break;
    case 0x13:		tmp = "FLTDTL";		break;
    case 0x14:		tmp = "FLTDTH";		break;
    case 0x20:		tmp = "SYSSTK";		break;
    case 0x21:		tmp = "SYSTMP";		break;
    case 0x30:		tmp = "MPC";		break;
    case 0x31:		tmp = "MIP";		break;
    case 0x33:		tmp = "ECOMCNTL";	break;
    case 0x34:		tmp = "ANASTAT";	break;
    case 0x39:		tmp = "BRK1";		break;
    case 0x3A:		tmp = "BRK2";		break;
    case 0x200:		tmp = "ITAG0";		break;
    case 0x201:		tmp = "ITAG1";		break;
    case 0x202:		tmp = "ITAG2";		break;
    case 0x203:		tmp = "ITAG3";		break;
    case 0x204:		tmp = "ITAG4";		break;
    case 0x205:		tmp = "ITAG5";		break;
    case 0x206:		tmp = "ITAG6";		break;
    case 0x207:		tmp = "ITAG7";		break;
    case 0x208:		tmp = "ITAG8";		break;
    case 0x209:		tmp = "ITAG9";		break;
    case 0x20A:		tmp = "ITAG10";		break;
    case 0x20B:		tmp = "ITAG11";		break;
    case 0x20C:		tmp = "ITAG12";		break;
    case 0x20D:		tmp = "ITAG13";		break;
    case 0x20E:		tmp = "ITAG14";		break;
    case 0x20F:		tmp = "ITAG15";		break;
    case 0x300:		tmp = "ILRU";		break;
    case 0x400:		tmp = "DTAG0";		break;
    case 0x401:		tmp = "DTAG1";		break;
    case 0x402:		tmp = "DTAG2";		break;
    case 0x403:		tmp = "DTAG3";		break;
    case 0x404:		tmp = "DTAG4";		break;
    case 0x405:		tmp = "DTAG5";		break;
    case 0x406:		tmp = "DTAG6";		break;
    case 0x407:		tmp = "DTAG7";		break;
    case 0x408:		tmp = "DTAG8";		break;
    case 0x409:		tmp = "DTAG9";		break;
    case 0x40A:		tmp = "DTAG10";		break;
    case 0x40B:		tmp = "DTAG11";		break;
    case 0x40C:		tmp = "DTAG12";		break;
    case 0x40D:		tmp = "DTAG13";		break;
    case 0x40E:		tmp = "DTAG14";		break;
    case 0x40F:		tmp = "DTAG15";		break;
    case 0x500:		tmp = "DLRU";		break;
    case 0x4000:	tmp = "IN0P";		break;
    case 0x4001:	tmp = "IN1P";		break;
    case 0x4002:	tmp = "OUTP";		break;
    default:		tmp = NULL;		break;
    }
  if (tmp != NULL)
    {
      (*info -> fprintf_func) (info -> stream, "%s", tmp);
    }
  else
    {
      (*info -> fprintf_func) (info -> stream, "%#lx", value);
    }
}


static void
print_operand_condition_code (info, value)
     struct disassemble_info *info;
     long value;
{
  const char *syms[] = {
    "nev.b", "gt0.b", "eq0.b", "ge0.b", "lt0.b", "ne0.b", "le0.b", "alw.b",
    "nev.h", "gt0.h", "eq0.h", "ge0.h", "lt0.h", "ne0.h", "le0.h", "alw.h",
    "nev.w", "gt0.w", "eq0.w", "ge0.w", "lt0.w", "ne0.w", "le0.w", "alw.w"
  };

  if (value < (sizeof (syms) / sizeof (syms[0])))
    {
      /* Found a value within range */
      (*info -> fprintf_func) (info -> stream, "%s", syms[value]);
    }
  else
    {
      /* Not in range, just print as decimal digit. */
      (*info -> fprintf_func) (info -> stream, "%ld", value);
    }
}


static void
print_operand_bitnum (info, value)
     struct disassemble_info *info;
     long value;
{
  int bitnum;
  const char *syms[] = {
    "eq.b", "ne.b", "gt.b", "le.b", "lt.b", "ge.b",
    "hi.b", "ls.b", "lo.b", "hs.b", "eq.h", "ne.h",
    "gt.h", "le.h", "lt.h", "ge.h", "hi.h", "ls.h",
    "lo.h", "hs.h", "eq.w", "ne.w", "gt.w", "le.w",
    "lt.w", "ge.w", "hi.w", "ls.w", "lo.w", "hs.w"
  };

  bitnum = ~value & 0x1F;
  if (bitnum < (sizeof (syms) / sizeof (syms[0])))
    {
      /* Found a value within range */
      (*info -> fprintf_func) (info -> stream, "%s", syms[bitnum]);
    }
  else
    {
      /* Not in range, just print as bit number */
      (*info -> fprintf_func) (info -> stream, "%ld", bitnum);
    }
}


/* Print the operand as directed by the flags.  */

#define M_SI(insn,op) ((((op) -> flags & TIC80_OPERAND_M_SI) != 0) && ((insn) & (1 << 17)))
#define M_LI(insn,op) ((((op) -> flags & TIC80_OPERAND_M_LI) != 0) && ((insn) & (1 << 15)))
#define R_SCALED(insn,op) ((((op) -> flags & TIC80_OPERAND_SCALED) != 0) && ((insn) & (1 << 11)))

static void
print_operand (info, value, insn, operand, memaddr)
     struct disassemble_info *info;
     long value;
     unsigned long insn;
     const struct tic80_operand *operand;
     bfd_vma memaddr;
{
  if ((operand -> flags & TIC80_OPERAND_GPR) != 0)
    {
      (*info -> fprintf_func) (info -> stream, "r%ld", value);
      if (M_SI (insn, operand) || M_LI (insn, operand))
	{
	  (*info -> fprintf_func) (info -> stream, ":m");
	}
    }
  else if ((operand -> flags & TIC80_OPERAND_FPA) != 0)
    {
      (*info -> fprintf_func) (info -> stream, "a%ld", value);
    }
  else if ((operand -> flags & TIC80_OPERAND_PCREL) != 0)
    {
      (*info -> print_address_func) (memaddr + 4 * value, info);
    }
  else if ((operand -> flags & TIC80_OPERAND_BASEREL) != 0)
    {
      (*info -> print_address_func) (value, info);
    }
  else if ((operand -> flags & TIC80_OPERAND_BITNUM) != 0)
    {
      print_operand_bitnum (info, value);
    }
  else if ((operand -> flags & TIC80_OPERAND_CC) != 0)
    {
      print_operand_condition_code (info, value);
    }
  else if ((operand -> flags & TIC80_OPERAND_CR) != 0)
    {
      print_operand_control_register (info, value);
    }
  else if ((operand -> flags & TIC80_OPERAND_FLOAT) != 0)
    {
      print_operand_float (info, value);
    }
  else if ((operand -> flags & TIC80_OPERAND_BITFIELD))
    {
      (*info -> fprintf_func) (info -> stream, "%#lx", value);
    }
  else
    {
      print_operand_integer (info, value);
    }

  /* If this is a scaled operand, then print the modifier */

  if (R_SCALED (insn, operand))
    {
      (*info -> fprintf_func) (info -> stream, ":s");
    }
}


/* We have chosen an opcode table entry */

static int
print_one_instruction (info, memaddr, insn, opcode)
     struct disassemble_info *info;
     bfd_vma memaddr;
     unsigned long insn;
     const struct tic80_opcode *opcode;
{
  const struct tic80_operand *operand;
  long value;
  int status;
  const unsigned char *opindex;
  bfd_byte buffer[4];
  int close_paren;

  (*info -> fprintf_func) (info -> stream, "%-10s", opcode -> name);

  for (opindex = opcode -> operands; *opindex != 0; opindex++)
    {
      operand = tic80_operands + *opindex;

      /* Extract the value from the instruction.  */
      if (operand -> extract)
	{
	  value = (*operand -> extract) (insn, (int *) NULL);
	}
      else if (operand -> bits == 32)
	{
	  status = fill_instruction (info, memaddr, (unsigned long *) &value);
	  if (status == -1)
	    {
	      return (status);
	    }
	}
      else
	{
	  value = (insn >> operand -> shift) & ((1 << operand -> bits) - 1);
	  if ((operand -> flags & TIC80_OPERAND_SIGNED) != 0
	      && (value & (1 << (operand -> bits - 1))) != 0)
	    {
	      value -= 1 << operand -> bits;
	    }
	}

      /* If this operand is enclosed in parenthesis, then print
	 the open paren, otherwise just print the regular comma
	 separator, except for the first operand. */

      if ((operand -> flags & TIC80_OPERAND_PARENS) == 0)
	{
	  close_paren = 0;
	  if (opindex != opcode -> operands)
	    {
	      (*info -> fprintf_func) (info -> stream, ",");
	    }
	}
      else
	{
	  close_paren = 1;
	  (*info -> fprintf_func) (info -> stream, "(");
	}

      print_operand (info, value, insn, operand, memaddr);

      /* If we printed an open paren before printing this operand, close
	 it now. The flag gets reset on each loop. */

      if (close_paren)
	{
	  (*info -> fprintf_func) (info -> stream, ")");
	}
    }
  return (length);
}



/* There are no specific bits that tell us for certain whether a vector
   instruction opcode contains one or two instructions.  However since
   a destination register of r0 is illegal, we can check for nonzero
   values in both destination register fields.  Only opcodes that have
   two valid instructions will have non-zero in both */

#define TWO_INSN(insn) ((((insn) & (0x1F << 27)) != 0) && (((insn) & (0x1F << 22)) != 0))

static int
print_instruction (info, memaddr, insn, vec_opcode)
     struct disassemble_info *info;
     bfd_vma memaddr;
     unsigned long insn;
     const struct tic80_opcode *vec_opcode;
{
  const struct tic80_opcode *opcode;
  const struct tic80_opcode *opcode_end;

  /* Find the first opcode match in the opcodes table.  For vector
     opcodes (vec_opcode != NULL) find the first match that is not the
     previously found match.  FIXME: there should be faster ways to
     search (hash table or binary search), but don't worry too much
     about it until other TIc80 support is finished. */

  opcode_end = tic80_opcodes + tic80_num_opcodes;
  for (opcode = tic80_opcodes; opcode < opcode_end; opcode++)
    {
      if ((insn & opcode -> mask) == opcode -> opcode &&
	  opcode != vec_opcode)
	{
	  break;
	}
    }

  if (opcode == opcode_end)
    {
      /* No match found, just print the bits as a .word directive */
      (*info -> fprintf_func) (info -> stream, ".word %#08lx", insn);
    }
  else
    {
      /* Match found, decode the instruction.  */
      length = print_one_instruction (info, memaddr, insn, opcode);
      if (opcode -> flags & TIC80_VECTOR && vec_opcode == NULL && TWO_INSN (insn))
	{
	  /* There is another instruction to print from the same opcode.
	     Print the separator and then find and print the other
	     instruction. */
	  (*info -> fprintf_func) (info -> stream, "   ||   ");
	  length = print_instruction (info, memaddr, insn, opcode);
	}
    }
  return (length);
}

/* Get the next 32 bit word from the instruction stream and convert it
   into internal format in the unsigned long INSN, for which we are
   passed the address.  Return 0 on success, -1 on error. */

static int
fill_instruction (info, memaddr, insnp)
     struct disassemble_info *info;
     bfd_vma memaddr;
     unsigned long *insnp;
{
  bfd_byte buffer[4];
  int status;

  /* Get the bits for the next 32 bit word and put in buffer */

  status = (*info -> read_memory_func) (memaddr + length, buffer, 4, info);
  if (status != 0)
    {
      (*info -> memory_error_func) (status, memaddr, info);
      return (-1);
    }

  /* Read was successful, so increment count of bytes read and convert
     the bits into internal format. */
     
  length += 4;
  if (info -> endian == BFD_ENDIAN_LITTLE)
    {
      *insnp = bfd_getl32 (buffer);
    }
  else if (info -> endian == BFD_ENDIAN_BIG)
    {
      *insnp = bfd_getb32 (buffer);
    }
  else
    {
      /* FIXME: Should probably just default to one or the other */
      abort ();
    }
  return (0);
}


int 
print_insn_tic80 (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  unsigned long insn;
  int status;

  length = 0;
  status = fill_instruction (info, memaddr, &insn);
  if (status != -1)
    {
      status = print_instruction (info, memaddr, insn, NULL);
    }
  return (status);
}
