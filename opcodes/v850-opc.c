/* Assemble V850 instructions.
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

#include "ansidecl.h"
#include "opcode/v850.h"
#include <stdio.h>

/* regular opcode */
#define OP(x)		((x & 0x3f) << 5)
#define OP_MASK		OP (0x3f)

/* conditional branch opcode */
#define BOP(x)		((0x0b << 7) | (x & 0x0f))
#define BOP_MASK	((0x0f << 7) | 0x0f)

/* one-word opcodes */
#define one(x)		((unsigned int) (x))

/* two-word opcodes */
#define two(x,y)	((unsigned int) (x) | ((unsigned int) (y) << 16))



/* The functions used to insert and extract complicated operands.  */

static unsigned long
insert_d9 (insn, value, errmsg)
     unsigned long insn;
     long value;
     const char **errmsg;
{
  if (value > 0xff || value < -0x100)
    *errmsg = "branch value out of range";

  if ((value % 2) != 0)
    *errmsg = "branch to odd offset";

  return (insn | ((value & 0x1f0) << 7) | ((value & 0x0e) << 3));
}

static unsigned long
extract_d9 (insn, invalid)
     unsigned long insn;
     int *invalid;
{
  unsigned long ret = ((insn & 0xf800) >> 7) | ((insn & 0x0070) >> 3);

  if ((insn & 0x8000) != 0)
    ret -= 0x0200;

  return ret;
}

static unsigned long
insert_d22 (insn, value, errmsg)
     unsigned long insn;
     long value;
     const char **errmsg;
{
  if (value > 0x1fffff || value < -0x200000)
    *errmsg = "branch value out of range";

  if ((value % 2) != 0)
    *errmsg = "branch to odd offset";

  return (insn | ((value & 0xfffe) << 16) | ((value & 0x3f0000) >> 16));
}

static unsigned long
extract_d22 (insn, invalid)
     unsigned long insn;
     int *invalid;
{
  unsigned long ret = ((insn & 0xfffe0000) >> 16) | ((insn & 0x3f) << 16);

  return ((ret << 10) >> 10);
}

static unsigned long
insert_d16_15 (insn, value, errmsg)
     unsigned long insn;
     long value;
     const char **errmsg;
{
  if (value > 0x7fff || value < -0x8000)
    *errmsg = "value out of range";

  if ((value % 2) != 0)
    *errmsg = "load/store half/word at odd offset";

  return insn | ((value & 0xfffe) << 16);
}

static unsigned long
extract_d16_15 (insn, invalid)
     unsigned long insn;
     int *invalid;
{
  signed long ret = (insn & 0xfffe0000);

  return ret >> 16;
}

static unsigned long
insert_d8_7 (insn, value, errmsg)
     unsigned long insn;
     long value;
     const char **errmsg;
{
  if (value > 0xff || value < 0)
    *errmsg = "short load/store half value out of range";

  if ((value % 2) != 0)
    *errmsg = "short load/store half at odd offset";

  value >>= 1;

  return (insn | (value & 0x7f));
}

static unsigned long
extract_d8_7 (insn, invalid)
     unsigned long insn;
     int *invalid;
{
  unsigned long ret = (insn & 0x7f);

  return ret << 1;
}

static unsigned long
insert_d8_6 (insn, value, errmsg)
     unsigned long insn;
     long value;
     const char **errmsg;
{
  if (value > 0xff || value < 0)
    *errmsg = "short load/store word value out of range";

  if ((value % 4) != 0)
    *errmsg = "short load/store word at odd offset";

  value >>= 1;

  return (insn | (value & 0x7e));
}

static unsigned long
extract_d8_6 (insn, invalid)
     unsigned long insn;
     int *invalid;
{
  unsigned long ret = (insn & 0x7e);

  return ret << 1;
}

/* start-sanitize-v850e */

static unsigned long
insert_d5_4 (insn, value, errmsg)
     unsigned long insn;
     long          value;
     const char ** errmsg;
{
  if (value > 0x1f || value < 0)
    *errmsg = "unsigned short load half value out of range";

  if (value & 1)
    *errmsg = "unsigned short load half at odd offset";

  value >>= 1;

  return (insn | (value & 0x0f));
}

static unsigned long
extract_d5_4 (insn, invalid)
     unsigned long insn;
     int *         invalid;
{
  unsigned long ret = (insn & 0x0f);

  return ret << 1;
}

static unsigned long
insert_d16_16 (insn, value, errmsg)
     unsigned long insn;
     signed long   value;
     const char ** errmsg;
{
  if (value > 0x7fff || value < -0x8000)
    *errmsg = "value out of range";

  return (insn | ((value & 0xfffe) << 16) | ((value & 1) << 5));
}

static unsigned long
extract_d16_16 (insn, invalid)
     unsigned long insn;
     int *         invalid;
{
  signed long ret = insn & 0xfffe0000;

  ret >>= 16;

  ret |= ((insn & 0x20) >> 5);
  
  return ret;
}

static unsigned long
insert_i9 (insn, value, errmsg)
     unsigned long insn;
     signed long   value;
     const char ** errmsg;
{
  if (value > 0xff || value < -0x100)
    *errmsg = "value out of range";

  return insn | ((value & 0x1e0) << 13) | (value & 0x1f);
}

static unsigned long
extract_i9 (insn, invalid)
     unsigned long insn;
     int *         invalid;
{
  signed long ret = insn & 0x003c0000;

  ret <<= 10;
  ret >>= 23;

  ret |= (insn & 0x1f);
  
  return ret;
}

static unsigned long
insert_u9 (insn, value, errmsg)
     unsigned long insn;
     unsigned long value;
     const char ** errmsg;
{
  if (value > 0x1ff)
    *errmsg = "value out of range";

  return insn | ((value & 0x1e0) << 13) | (value & 0x1f);
}

static unsigned long
extract_u9 (insn, invalid)
     unsigned long insn;
     int *         invalid;
{
  unsigned long ret = insn & 0x003c0000;

  ret >>= 13;

  ret |= (insn & 0x1f);
  
  return ret;
}

/* start-sanitize-v850e */
static unsigned long
insert_spe (insn, value, errmsg)
     unsigned long insn;
     unsigned long value;
     const char ** errmsg;
{
  if (value != 3)
    *errmsg = "invalid register for stack adjustment";

  return insn & (~ 0x180000);
}

static unsigned long
extract_spe (insn, invalid)
     unsigned long insn;
     int *         invalid;
{
  return 3;
}

/* end-sanitize-v850e */
/* start-sanitize-v850eq */

static unsigned long
insert_i5div (insn, value, errmsg)
     unsigned long insn;
     unsigned long value;
     const char ** errmsg;
{
  if (value > 0x1ff)
    *errmsg = "value out of range";

  if (value & 1)
    *errmsg = "value must be even";

  value = 32 - value;
  
  return insn | ((value & 0x1e) << 17);
}

static unsigned long
extract_i5div (insn, invalid)
     unsigned long insn;
     int *         invalid;
{
  unsigned long ret = insn & 0x3c0000;

  ret >>= 17;

  ret = 32 - ret;
  
  return ret;
}

/* end-sanitize-v850eq */


const struct v850_operand v850_operands[] =
{
#define UNUSED	0
  { 0, 0, 0, 0, 0 }, 

/* The R1 field in a format 1, 6, 7, or 9 insn. */
#define R1	(UNUSED+1)
  { 5, 0, 0, 0, V850_OPERAND_REG }, 

/* The R1 field in a format 1, 6, 7, or 9 insn. */
#define R1_NOTR0 (R1 + 1)
  { 5, 0, 0, 0, V850_OPERAND_REG | V850_NOT_R0 }, 

/* The R2 field in a format 1, 2, 4, 5, 6, 7, 9 insn. */
#define R2	(R1_NOTR0 + 1)
  { 5, 11, 0, 0, V850_OPERAND_REG },

/* The R2 field in a format 1, 2, 4, 5, 6, 7, 9 insn. */
#define R2_NOTR0 (R2 + 1)
  { 5, 11, 0, 0, V850_OPERAND_REG | V850_NOT_R0 },

/* The IMM5 field in a format 2 insn. */
#define I5	(R2_NOTR0 + 1)
  { 5, 0, 0, 0, V850_OPERAND_SIGNED }, 

#define I5U	(I5+1)
  { 5, 0, 0, 0, 0 },

/* The IMM16 field in a format 6 insn. */
#define I16	(I5U+1)
  { 16, 16, 0, 0, V850_OPERAND_SIGNED }, 

/* The signed DISP7 field in a format 4 insn. */
#define D7	(I16+1)
  { 7, 0, 0, 0, 0},

/* The DISP16 field in a format 6 insn. */
#define D16_15	(D7+1)
  { 16, 16, insert_d16_15, extract_d16_15, V850_OPERAND_SIGNED }, 

#define B3	(D16_15+1)
/* The 3 bit immediate field in format 8 insn.  */
  { 3, 11, 0, 0, 0 },

#define CCCC	(B3+1)
/* The 4 bit condition code in a setf instruction */
  { 4, 0, 0, 0, V850_OPERAND_CC },

/* The unsigned DISP8_7 field in a format 4 insn. */
#define D8_7	(CCCC+1)
  { 8, 0, insert_d8_7, extract_d8_7, V850_OPERAND_ADJUST_SHORT_MEMORY },

/* The unsigned DISP8_6 field in a format 4 insn. */
#define D8_6	(D8_7+1)
  { 8, 0, insert_d8_6, extract_d8_6, V850_OPERAND_ADJUST_SHORT_MEMORY },

/* System register operands.  */
#define SR1	(D8_6+1)
  { 5, 0, 0, 0, V850_OPERAND_SRG },

/* EP Register.  */
#define EP	(SR1+1)
  { 0, 0, 0, 0, V850_OPERAND_EP },

/* The IMM16 field (unsigned0 in a format 6 insn. */
#define I16U	(EP+1)
  { 16, 16, 0, 0, 0}, 

/* The R2 field as a system register.  */
#define SR2	(I16U+1)
  { 5, 11, 0, 0, V850_OPERAND_SRG },

/* The DISP16 field in a format 8 insn. */
#define D16	(SR2+1)
  { 16, 16, 0, 0, V850_OPERAND_SIGNED }, 

/* The DISP22 field in a format 4 insn, relaxable. */
#define D9_RELAX	(D16+1)
  { 9, 0, insert_d9, extract_d9, V850_OPERAND_RELAX | V850_OPERAND_SIGNED | V850_OPERAND_DISP },

/* The DISP22 field in a format 4 insn.

   This _must_ follow D9_RELAX; the assembler assumes that the longer
   version immediately follows the shorter version for relaxing.  */
#define D22	(D9_RELAX+1)
  { 22, 0, insert_d22, extract_d22, V850_OPERAND_SIGNED | V850_OPERAND_DISP },

/* start-sanitize-v850e */
  
/* The signed DISP4 field in a format 4 insn. */
#define D4	(D22+1)
  { 4, 0, 0, 0, 0},

/* The unsigned DISP5_4 field in a format 4 insn. */
#define D5_4	(D4 + 1)
  { 5, 0, insert_d5_4, extract_d5_4, V850_OPERAND_ADJUST_SHORT_MEMORY },

/* The DISP16 field in an unsigned format 7 byte load insn. */
#define D16_16	(D5_4 + 1)
  { 16, 16, insert_d16_16, extract_d16_16, 0 }, 

/* Third register in conditional moves. */
#define R3	(D16_16 + 1)
  { 5, 27, 0, 0, V850_OPERAND_REG },

/* Condition code in conditional moves.  */
#define MOVCC	(R3 + 1)
  { 4, 17, 0, 0, V850_OPERAND_CC },

/* The IMM9 field in a multiply word. */
#define I9	(MOVCC + 1)
  { 9, 0, insert_i9, extract_i9, V850_OPERAND_SIGNED }, 

/* The IMM9 field in a multiply word. */
#define U9	(I9 + 1)
  { 9, 0, insert_u9, extract_u9, 0 }, 

/* A list of registers in a prepare/dispose instruction.  */
#define LIST12	(U9 + 1)
  { -1, 0xffe00001, 0, 0, V850E_PUSH_POP }, 

/* The IMM6 field in a call instruction. */
#define I6	(LIST12 + 1)
  { 6, 0, 0, 0, 0 }, 

/* The 16 bit immediate following a 32 bit instruction.  */
#define IMM16	(I6 + 1)
  { 16, 16, 0, 0, V850_OPERAND_SIGNED | V850E_IMMEDIATE16 }, 

/* The 32 bit immediate following a 32 bit instruction.  */
#define IMM32	(IMM16 + 1)
  { 0, 0, 0, 0, V850E_IMMEDIATE32 }, 

/* The IMM5 field in a push/pop instruction. */
#define IMM5	(IMM32 + 1)
  { 5, 1, 0, 0, 0 }, 

/* Reg2 in dispose instruction. */
#define R2DISPOSE	(IMM5 + 1)
  { 5, 16, 0, 0, V850_OPERAND_REG | V850_NOT_R0 },

/* Stack pointer in prepare instruction. */
#define SP	(R2DISPOSE + 1)
  { 2, 19, insert_spe, extract_spe, V850_OPERAND_REG },

/* end-sanitize-v850e */
/* start-sanitize-v850eq */

/* The IMM5 field in a divide N step instruction. */
#define I5DIV	(SP + 1)
  { 9, 0, insert_i5div, extract_i5div, V850_OPERAND_SIGNED }, 

  /* The list of registers in a PUSHMH/POPMH instruction.  */
#define LIST18_H (I5DIV + 1)
  { -1, 0xfff8000f, 0, 0, V850E_PUSH_POP }, 

  /* The list of registers in a PUSHML/POPML instruction.  */
#define LIST18_L (LIST18_H + 1)
  { -1, 0xfff8001f, 0, 0, V850E_PUSH_POP }, /* The setting of the 4th bit is a flag to disassmble() in v850-dis.c */

/* end-sanitize-v850eq */
} ; 


/* reg-reg instruction format (Format I) */
#define IF1	{R1, R2}

/* imm-reg instruction format (Format II) */
#define IF2	{I5, R2}

/* conditional branch instruction format (Format III) */
#define IF3	{D9_RELAX}

/* 3 operand instruction (Format VI) */
#define IF6	{I16, R1, R2}

/* 3 operand instruction (Format VI) */
#define IF6U	{I16U, R1, R2}



/* The opcode table.

   The format of the opcode table is:

   NAME		OPCODE			MASK		       { OPERANDS }	   MEMOP

   NAME is the name of the instruction.
   OPCODE is the instruction opcode.
   MASK is the opcode mask; this is used to tell the disassembler
     which bits in the actual opcode must match OPCODE.
   OPERANDS is the list of operands.
   MEMOP specifies which operand (if any) is a memory operand.
   
   The disassembler reads the table in order and prints the first
   instruction which matches, so this table is sorted to put more
   specific instructions before more general instructions.  It is also
   sorted by major opcode.  */

const struct v850_opcode v850_opcodes[] =
{
{ "breakpoint",	0xffff,			0xffff,		      	{UNUSED},   		0 },

{ "jmp",	one (0x0060),		one (0xffe0),	      	{R1}, 			1 },
  
/* load/store instructions */
#ifdef ARCH_v850eq
{ "sld.bu",	one (0x0300),		one (0x0780),	      	{D7,   EP,   R2_NOTR0},	1 },
{ "sld.hu",	one (0x0400),		one (0x0780),	      	{D8_7, EP,   R2_NOTR0},	1 },
{ "sld.b",      one (0x0060),		one (0x07f0),         	{D4,   EP,   R2}, 	1 },
{ "sld.h",      one (0x0070),		one (0x07f0),         	{D5_4, EP,   R2}, 	1 },
#else
{ "sld.b",	one (0x0300),		one (0x0780),	      	{D7,   EP,   R2},	1 },
{ "sld.h",	one (0x0400),		one (0x0780),	      	{D8_7, EP,   R2}, 	1 },
{ "sld.bu",     one (0x0060),		one (0x07f0),         	{D4,   EP,   R2_NOTR0},	1 },
{ "sld.hu",     one (0x0070),		one (0x07f0),         	{D5_4, EP,   R2_NOTR0},	1 },
#endif
{ "sld.w",	one (0x0500),		one (0x0781),	      	{D8_6, EP,   R2}, 	1 },
{ "sst.b",	one (0x0380),		one (0x0780),	      	{R2,   D7,   EP}, 	2 },
{ "sst.h",	one (0x0480),		one (0x0780),	      	{R2,   D8_7, EP}, 	2 },
{ "sst.w",	one (0x0501),		one (0x0781),	      	{R2,   D8_6, EP}, 	2 },

/* start-sanitize-v850eq */
{ "pushml",	two (0x07e0, 0x0001),	two (0xfff0, 0x0007), 	{LIST18_L}, 		0 },
{ "pushmh",	two (0x07e0, 0x0003),	two (0xfff0, 0x0007), 	{LIST18_H}, 		0 },
{ "popml",	two (0x07f0, 0x0001),	two (0xfff0, 0x0007), 	{LIST18_L}, 		0 },
{ "popmh",	two (0x07f0, 0x0003),	two (0xfff0, 0x0007), 	{LIST18_H}, 		0 },
/* end-sanitize-v850e */
  
/* start-sanitize-v850e */  
{ "prepare",    two (0x0780, 0x0003),	two (0xffc0, 0x001f), 	{LIST12, IMM5, SP}, 	0 },
{ "prepare",    two (0x0780, 0x000b),	two (0xffc0, 0x001f), 	{LIST12, IMM5, IMM16}, 	0 },
{ "prepare",    two (0x0780, 0x0013),	two (0xffc0, 0x001f), 	{LIST12, IMM5, IMM16}, 	0 },
{ "prepare",    two (0x0780, 0x001b),	two (0xffc0, 0x001f), 	{LIST12, IMM5, IMM32}, 	0 },
{ "prepare",    two (0x0780, 0x0001),	two (0xffc0, 0x001f), 	{LIST12, IMM5}, 	0 },
{ "dispose",	one (0x0640),           one (0xffc0),         	{IMM5, LIST12, R2DISPOSE},0 },
{ "dispose",	two (0x0640, 0x0000),   two (0xffc0, 0x001f), 	{IMM5, LIST12}, 	0 },
/* end-sanitize-v850e */

{ "ld.b",	two (0x0700, 0x0000),	two (0x07e0, 0x0000), 	{D16, R1, R2}, 		1 },
{ "ld.h",	two (0x0720, 0x0000),	two (0x07e0, 0x0001), 	{D16_15, R1, R2}, 	1 },
{ "ld.w",	two (0x0720, 0x0001),	two (0x07e0, 0x0001), 	{D16_15, R1, R2}, 	1 },
/* start-sanitize-v850e */
{ "ld.bu",	two (0x0780, 0x0001),   two (0x07c0, 0x0001), 	{D16_16, R1, R2_NOTR0},	1 },
{ "ld.hu",	two (0x07e0, 0x0001),   two (0x07e0, 0x0001), 	{D16_15, R1, R2_NOTR0},	1 },  
/* end-sanitize-v850e */
{ "st.b",	two (0x0740, 0x0000),	two (0x07e0, 0x0000), 	{R2, D16, R1}, 		2 },
{ "st.h",	two (0x0760, 0x0000),	two (0x07e0, 0x0001), 	{R2, D16_15, R1}, 	2 },
{ "st.w",	two (0x0760, 0x0001),	two (0x07e0, 0x0001), 	{R2, D16_15, R1}, 	2 },

/* start-sanitize-v850e */
/* byte swap/extend instructions */
{ "zxb",	one (0x0080),		one (0xffe0), 	      	{R1_NOTR0},		0 },
{ "zxh",	one (0x00c0),		one (0xffe0), 	      	{R1_NOTR0}, 		0 },
{ "sxb",	one (0x00a0),		one (0xffe0), 	      	{R1_NOTR0},		0 },
{ "sxh",	one (0x00e0),		one (0xffe0),	      	{R1_NOTR0},		0 },
{ "bsh",	two (0x07e0, 0x0342),	two (0x07ff, 0x07ff), 	{R2, R3}, 		0 },
{ "bsw",	two (0x07e0, 0x0340),	two (0x07ff, 0x07ff), 	{R2, R3}, 		0 },
{ "hsw",	two (0x07e0, 0x0344),	two (0x07ff, 0x07ff), 	{R2, R3}, 		0 },

/* jump table instructions */
{ "switch",	one (0x0040),		one (0xffe0), 	      	{R1}, 			1 },
{ "callt",	one (0x0200),		one (0xffc0), 	      	{I6}, 			0 },
{ "ctret", 	two (0x07e0, 0x0144),	two (0xffff, 0xffff), 	{0}, 			0 },
/* end-sanitize-v850e */

/* arithmetic operation instructions */
{ "setf",	two (0x07e0, 0x0000),	two (0x07f0, 0xffff), 	{CCCC, R2}, 		0 },
/* start-sanitize-v850e */
{ "cmov",	two (0x07e0, 0x0320),	two (0x07e0, 0x07e1), 	{MOVCC, R2, R1, R3}, 	0 },
{ "cmov",	two (0x07e0, 0x0300),	two (0x07e0, 0x07e1), 	{MOVCC, I5, R2, R3}, 	0 },
/* end-sanitize-v850e */
{ "mul",	two (0x07e0, 0x0220),	two (0x07e0, 0x07ff), 	{R1, R2, R3}, 		0 },
/* start-sanitize-v850e */
{ "mul",	two (0x07e0, 0x0240),	two (0x07e0, 0x07c3), 	{I9, R2, R3}, 		0 },
/* end-sanitize-v850e */
{ "mulu",	two (0x07e0, 0x0222),	two (0x07e0, 0x07ff), 	{R1, R2, R3}, 		0 },
  
/* start-sanitize-v850e */
{ "mulu",	two (0x07e0, 0x0242),	two (0x07e0, 0x07c3), 	{U9, R2, R3}, 		0 },
{ "div",	two (0x07e0, 0x02c0),	two (0x07e0, 0x07ff), 	{R1, R2, R3}, 		0 },
{ "divu",	two (0x07e0, 0x02c2),	two (0x07e0, 0x07ff), 	{R1, R2, R3}, 		0 },
{ "divhu",	two (0x07e0, 0x0282),   two (0x07e0, 0x07ff), 	{R1, R2, R3}, 		0 },
{ "divh",	two (0x07e0, 0x0280),   two (0x07e0, 0x07ff), 	{R1, R2, R3}, 		0 },
/* end-sanitize-v850e */
{ "divh",	OP  (0x02),		OP_MASK,		{R1, R2_NOTR0},		0 },
  
/* start-sanitize-v850eq */
{ "divhn",	two (0x07e0, 0x0280),   two (0x07e0, 0x07c3), 	{I5DIV, R1, R2, R3}, 	0 },
{ "divhun",	two (0x07e0, 0x0282),   two (0x07e0, 0x07c3), 	{I5DIV, R1, R2, R3}, 	0 },
{ "divn",	two (0x07e0, 0x02c0),   two (0x07e0, 0x07c3), 	{I5DIV, R1, R2, R3}, 	0 },
{ "divun",	two (0x07e0, 0x02c2),   two (0x07e0, 0x07c3), 	{I5DIV, R1, R2, R3}, 	0 },
{ "sdivhn",	two (0x07e0, 0x0180),   two (0x07e0, 0x07c3), 	{I5DIV, R1, R2, R3}, 	0 },
{ "sdivhun",	two (0x07e0, 0x0182),   two (0x07e0, 0x07c3), 	{I5DIV, R1, R2, R3}, 	0 },
{ "sdivn",	two (0x07e0, 0x01c0),   two (0x07e0, 0x07c3), 	{I5DIV, R1, R2, R3}, 	0 },
{ "sdivun",	two (0x07e0, 0x01c2),   two (0x07e0, 0x07c3), 	{I5DIV, R1, R2, R3}, 	0 },
/* end-sanitize-v850eq */
  
{ "nop",	one (0x00),		one (0xffff),		{0}, 			0 },
{ "mov",	OP  (0x10),		OP_MASK,		{I5, R2_NOTR0},		0 },
/* start-sanitize-v850e */
{ "mov",	one (0x0620),		one (0xffe0),		{IMM32, R1_NOTR0},	0 },
/* end-sanitize-v850e */
{ "mov",        OP  (0x00),		OP_MASK,		{R1, R2_NOTR0},		0 },
{ "movea",	OP  (0x31),		OP_MASK,		{I16, R1, R2_NOTR0},	0 },
{ "movhi",	OP  (0x32),		OP_MASK,		{I16U, R1, R2_NOTR0},	0 },
{ "add",	OP  (0x0e),		OP_MASK,		IF1, 			0 },
{ "add",	OP  (0x12),		OP_MASK,		IF2, 			0 },
{ "addi",	OP  (0x30),		OP_MASK,		IF6, 			0 },
{ "sub",	OP  (0x0d),		OP_MASK,		IF1, 			0 },
{ "subr", 	OP  (0x0c),		OP_MASK,		IF1, 			0 },
{ "mulh",	OP  (0x17),		OP_MASK,		{I5, R2_NOTR0},		0 },
{ "mulh",	OP  (0x07),		OP_MASK,		{R1, R2_NOTR0},		0 },
{ "mulhi",	OP  (0x37),		OP_MASK,		{I16, R1, R2_NOTR0},	0 },
{ "cmp",	OP  (0x0f),		OP_MASK,		IF1, 			0 },
{ "cmp",	OP  (0x13),		OP_MASK,		IF2, 			0 },
  
/* saturated operation instructions */
{ "satadd",	OP (0x11),		OP_MASK,		{I5, R2_NOTR0},		0 },
{ "satadd",	OP (0x06),		OP_MASK,		{R1, R2_NOTR0},		0 },
{ "satsub",	OP (0x05),		OP_MASK,		{R1, R2_NOTR0},		0 },
{ "satsubi",	OP (0x33),		OP_MASK,		{I16, R1, R2_NOTR0},	0 },
{ "satsubr",	OP (0x04),		OP_MASK,		{R1, R2_NOTR0},		0 },

/* logical operation instructions */
{ "tst",	OP (0x0b),		OP_MASK,		IF1, 			0 },
{ "or",		OP (0x08),		OP_MASK,		IF1, 			0 },
{ "ori",	OP (0x34),		OP_MASK,		IF6U, 			0 },
{ "and",	OP (0x0a),		OP_MASK,		IF1, 			0 },
{ "andi",	OP (0x36),		OP_MASK,		IF6U, 			0 },
{ "xor",	OP (0x09),		OP_MASK,		IF1, 			0 },
{ "xori",	OP (0x35),		OP_MASK,		IF6U, 			0 },
{ "not",	OP (0x01),		OP_MASK,		IF1, 			0 },
{ "sar",	OP (0x15),		OP_MASK,		{I5U, R2}, 		0 },
{ "sar",	two (0x07e0, 0x00a0),	two (0x07e0, 0xffff), 	{R1,  R2}, 		0 },
{ "shl",	OP  (0x16),		OP_MASK,	      	{I5U, R2}, 		0 },
{ "shl",	two (0x07e0, 0x00c0),	two (0x07e0, 0xffff), 	{R1,  R2}, 		0 },
{ "shr",	OP  (0x14),		OP_MASK,	      	{I5U, R2}, 		0 },
{ "shr",	two (0x07e0, 0x0080),	two (0x07e0, 0xffff), 	{R1,  R2}, 		0 },
/* start-sanitize-v850e */
{ "sasf",       two (0x07e0, 0x0200),	two (0x07f0, 0xffff), 	{CCCC, R2}, 		0 },
/* end-sanitize-v850e */

/* branch instructions */
	/* signed integer */
{ "bgt",	BOP (0xf),		BOP_MASK,		IF3, 			0 },
{ "bge",	BOP (0xe),		BOP_MASK,		IF3, 			0 },
{ "blt",	BOP (0x6),		BOP_MASK,		IF3, 			0 },
{ "ble",	BOP (0x7),		BOP_MASK,		IF3, 			0 },
	/* unsigned integer */
{ "bh",		BOP (0xb),		BOP_MASK,		IF3, 			0 },
{ "bnh",	BOP (0x3),		BOP_MASK,		IF3, 			0 },
{ "bl",		BOP (0x1),		BOP_MASK,		IF3, 			0 },
{ "bnl",	BOP (0x9),		BOP_MASK,		IF3, 			0 },
	/* common */
{ "be",		BOP (0x2),		BOP_MASK,		IF3, 			0 },
{ "bne",	BOP (0xa),		BOP_MASK,		IF3, 			0 },
	/* others */
{ "bv",		BOP (0x0),		BOP_MASK,		IF3, 			0 },
{ "bnv",	BOP (0x8),		BOP_MASK,		IF3, 			0 },
{ "bn",		BOP (0x4),		BOP_MASK,		IF3, 			0 },
{ "bp",		BOP (0xc),		BOP_MASK,		IF3, 			0 },
{ "bc",		BOP (0x1),		BOP_MASK,		IF3, 			0 },
{ "bnc",	BOP (0x9),		BOP_MASK,		IF3, 			0 },
{ "bz",		BOP (0x2),		BOP_MASK,		IF3, 			0 },
{ "bnz",	BOP (0xa),		BOP_MASK,		IF3, 			0 },
{ "br",		BOP (0x5),		BOP_MASK,		IF3, 			0 },
{ "bsa",	BOP (0xd),		BOP_MASK,		IF3, 			0 },

/* Branch macros.

   We use the short form in the opcode/mask fields.  The assembler
   will twiddle bits as necessary if the long form is needed.  */

	/* signed integer */
{ "jgt",	BOP (0xf),		BOP_MASK,		IF3, 			0 },
{ "jge",	BOP (0xe),		BOP_MASK,		IF3, 			0 },
{ "jlt",	BOP (0x6),		BOP_MASK,		IF3, 			0 },
{ "jle",	BOP (0x7),		BOP_MASK,		IF3, 			0 },
	/* unsigned integer */
{ "jh",		BOP (0xb),		BOP_MASK,		IF3, 			0 },
{ "jnh",	BOP (0x3),		BOP_MASK,		IF3, 			0 },
{ "jl",		BOP (0x1),		BOP_MASK,		IF3, 			0 },
{ "jnl",	BOP (0x9),		BOP_MASK,		IF3, 			0 },
	/* common */
{ "je",		BOP (0x2),		BOP_MASK,		IF3, 			0 },
{ "jne",	BOP (0xa),		BOP_MASK,		IF3, 			0 },
	/* others */
{ "jv",		BOP (0x0),		BOP_MASK,		IF3, 			0 },
{ "jnv",	BOP (0x8),		BOP_MASK,		IF3, 			0 },
{ "jn",		BOP (0x4),		BOP_MASK,		IF3, 			0 },
{ "jp",		BOP (0xc),		BOP_MASK,		IF3, 			0 },
{ "jc",		BOP (0x1),		BOP_MASK,		IF3, 			0 },
{ "jnc",	BOP (0x9),		BOP_MASK,		IF3, 			0 },
{ "jz",		BOP (0x2),		BOP_MASK,		IF3, 			0 },
{ "jnz",	BOP (0xa),		BOP_MASK,		IF3, 			0 },
{ "jsa",	BOP (0xd),		BOP_MASK,		IF3, 			0 },
{ "jbr",	BOP (0x5),		BOP_MASK,		IF3, 			0 },
  
{ "jr",		one (0x0780),		two (0xffc0, 0x0001),	{D22}, 			0 },
{ "jarl",	one (0x0780),		two (0x07c0, 0x0001),	{D22, R2}, 		0 }, 

/* bit manipulation instructions */
{ "set1",	two (0x07c0, 0x0000),	two (0xc7e0, 0x0000),	{B3, D16, R1}, 		2 },
/* start-sanitize-v850e */
{ "set1",	two (0x07e0, 0x00e0),	two (0x07e0, 0xffff),	{R2, R1},    		2 },
/* end-sanitize-v850e */
{ "not1",	two (0x47c0, 0x0000),	two (0xc7e0, 0x0000),	{B3, D16, R1}, 		2 },
/* start-sanitize-v850e */
{ "not1",	two (0x07e0, 0x00e2),	two (0x07e0, 0xffff),	{R2, R1},    		2 },
/* end-sanitize-v850e */
{ "clr1",	two (0x87c0, 0x0000),	two (0xc7e0, 0x0000),	{B3, D16, R1}, 		2 },
/* start-sanitize-v850e */
{ "clr1",	two (0x07e0, 0x00e4),	two (0x07e0, 0xffff),   {R2, R1},    		2 },
/* end-sanitize-v850e */
{ "tst1",	two (0xc7c0, 0x0000),	two (0xc7e0, 0x0000),	{B3, D16, R1}, 		2 },
/* start-sanitize-v850e */
{ "tst1",	two (0x07e0, 0x00e6),	two (0x07e0, 0xffff),	{R2, R1},    		2 },
/* end-sanitize-v850e */

/* special instructions */
{ "di",		two (0x07e0, 0x0160),	two (0xffff, 0xffff),	{0}, 			0 },
{ "ei",		two (0x87e0, 0x0160),	two (0xffff, 0xffff),	{0}, 			0 },
{ "halt",	two (0x07e0, 0x0120),	two (0xffff, 0xffff),	{0}, 			0 },
{ "reti",	two (0x07e0, 0x0140),	two (0xffff, 0xffff),	{0}, 			0 },
{ "trap",	two (0x07e0, 0x0100),	two (0xffe0, 0xffff),	{I5U}, 			0 },
{ "ldsr",	two (0x07e0, 0x0020),	two (0x07e0, 0xffff),	{R1, SR2}, 		0 },
{ "stsr",	two (0x07e0, 0x0040),	two (0x07e0, 0xffff),	{SR1, R2}, 		0 },
{ 0, 0, 0, {0}, 0 },

} ;

const int v850_num_opcodes =
  sizeof (v850_opcodes) / sizeof (v850_opcodes[0]);

