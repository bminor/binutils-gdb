/* Assemble Matsushita MN10300 instructions.
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
#include "opcode/mn10300.h"

/* Formats.  Right now we don't use this.  We probably will later for
   the size of the instruction and other random stuff.  */
#define FMT_S0 1
#define FMT_S1 2
#define FMT_S2 3
#define FMT_S4 4
#define FMT_S6 5
#define FMT_D0 6
#define FMT_D1 7
#define FMT_D2 8
#define FMT_D4 9
#define FMT_D5 10

/* Operands currently used.  This is temporary.  */


const struct mn10300_operand mn10300_operands[] = {
#define UNUSED	0
  {0, 0, 0}, 

#define DN      (UNUSED+1)
  {2, 0, MN10300_OPERAND_DREG},

#define DM      (DN+1)
  {2, 0, MN10300_OPERAND_DREG},

#define AN      (DM+1)
  {2, 0, MN10300_OPERAND_AREG},

#define AM      (AN+1)
  {2, 0, MN10300_OPERAND_AREG},

#define IMM8    (AM+1)
  {8, 0, MN10300_OPERAND_PROMOTE},

#define IMM16    (IMM8+1)
  {16, 0, MN10300_OPERAND_PROMOTE},

#define IMM32    (IMM16+1)
  {32, 0, 0},

#define D8    (IMM32+1)
  {8, 0, MN10300_OPERAND_PROMOTE},

#define D16    (D8+1)
  {16, 0, MN10300_OPERAND_PROMOTE},

#define D32    (D16+1)
  {32, 0, 0},

#define SP    (D32+1)
  {8, 0, MN10300_OPERAND_SP},

#define PSW    (SP+1)
  {0, 0, MN10300_OPERAND_PSW},

#define MDR    (PSW+1)
  {0, 0, MN10300_OPERAND_MDR},

#define ABS16 (MDR+1)
  {16, 0, MN10300_OPERAND_PROMOTE},

#define ABS32 (ABS16+1)
  {32, 0, 0},

#define DI (ABS32+1)
  {2, 0, MN10300_OPERAND_DREG},

#define REGS (DI+1)
  {0, 0, 0},

#define SD8    (REGS+1)
  {8, 0, MN10300_OPERAND_SIGNED | MN10300_OPERAND_PROMOTE},

#define SD16    (SD8+1)
  {16, 0, MN10300_OPERAND_SIGNED | MN10300_OPERAND_PROMOTE},

#define SD8N    (SD16+1)
  {8, 0, MN10300_OPERAND_SIGNED},

#define SIMM8    (SD8N+1)
  {8, 0, MN10300_OPERAND_SIGNED | MN10300_OPERAND_PROMOTE},

#define SIMM16    (SIMM8+1)
  {16, 0, MN10300_OPERAND_SIGNED | MN10300_OPERAND_PROMOTE},
} ; 


/* The opcode table.

   The format of the opcode table is:

   NAME		OPCODE		MASK		{ OPERANDS }

   NAME is the name of the instruction.
   OPCODE is the instruction opcode.
   MASK is the opcode mask; this is used to tell the disassembler
     which bits in the actual opcode must match OPCODE.
   OPERANDS is the list of operands.

   The disassembler reads the table in order and prints the first
   instruction which matches, so this table is sorted to put more
   specific instructions before more general instructions.  It is also
   sorted by major opcode.  */

const struct mn10300_opcode mn10300_opcodes[] = {
{ "mov",	0x8000,		0xf000,		FMT_S1, {SIMM8, DN}, },
{ "mov",	0x80,		0xf0,		FMT_S0, {DM, DN}, },
{ "mov",	0xf1e0,		0xfff0,		FMT_D0, {DM, AN}, },
{ "mov",	0xf1d0,		0xfff0,		FMT_D0, {AM, DN}, },
{ "mov",	0x9000,		0xf000,		FMT_S1, {IMM8, AN}, },
{ "mov",	0x90,		0xf0,		FMT_S0, {AM, AN}, },
{ "mov",	0x3c,		0xfc,		FMT_S0, {SP, AN}, },
{ "mov",	0xf2f0,		0xfff3,		FMT_D0, {AM, SP}, },
{ "mov",	0xf2e4,		0xfffc,		FMT_D0, {PSW, DN}, },
{ "mov",	0xf2f3,		0xfff3,		FMT_D0, {DM, PSW}, },
{ "mov",	0xf2e0,		0xfffc,		FMT_D0, {MDR, DN}, },
{ "mov",	0xf2f2,		0xfff3,		FMT_D0, {DM, MDR}, },
{ "mov",	0x70,		0xf0,		FMT_S0, {AM, DN}, },
{ "mov",	0xf80000,	0xfff000,	FMT_D1, {SD8, AM, DN}, },
{ "mov",	0xfa000000,	0xfff00000,	FMT_D2, {SD16, AM, DN}, },
{ "mov",	0xfc000000,	0xfff00000,	FMT_D4, {D32, AM, DN}, },
{ "mov",	0x5800,		0xfc00,		FMT_S1, {D8, SP, DN}, },
{ "mov",	0xfab40000,	0xfffc0000,	FMT_D2, {D16, SP, DN}, },
{ "mov",	0xfcb40000,	0xfffc0000,	FMT_D4, {D32, SP, DN}, },
{ "mov",	0xf300,		0xffc0,		FMT_D0, {DI, AM, DN}, },
{ "mov",	0x300000,	0xfc0000,	FMT_S2, {ABS16, DN}, },
{ "mov",	0xfca40000,	0xfffc0000,	FMT_D4, {ABS32, DN}, },
{ "mov",	0xf000,		0xfff0,		FMT_D0, {AM, AN}, },
{ "mov",	0xf82000,	0xfff000,	FMT_D1, {D8, AM, AN}, },
{ "mov",	0xfa200000,	0xfff00000,	FMT_D2, {D16, AM, AN}, },
{ "mov",	0xfc200000,	0xfff00000,	FMT_D4, {D32, AM, AN}, },
{ "mov",	0x5c00,		0xfc00,		FMT_S1, {D8, SP, AN}, },
{ "mov",	0xfab00000,	0xfffc0000,	FMT_D2, {D16, SP, AN}, },
{ "mov",	0xfcb00000,	0xfffc0000,	FMT_D4, {D32, SP, AN}, },
{ "mov",	0xf380,		0xffc0,		FMT_D0, {DI, AM, AN}, },
{ "mov",	0xfaa00000,	0xfffc0000,	FMT_D2, {ABS16, AN}, },
{ "mov",	0xfca00000,	0xfffc0000,	FMT_D4, {ABS32, AN}, },
{ "mov",	0xf8f000,	0xfffc00,	FMT_D1, {SD8N, AM, SP}, },
{ "mov",	0x60,		0xf0,		FMT_S0, {DM, AN}, },
{ "mov",	0xf81000,	0xfff000,	FMT_D1, {DM, SD8, AN}, },
{ "mov",	0xfa100000,	0xfff00000,	FMT_D2, {DM, SD16, AN}, },
{ "mov",	0xfc100000,	0xfff00000,	FMT_D4, {DM, D32, AN}, },
{ "mov",	0x4200,		0xf300,		FMT_S1, {DM, D8, SP}, },
{ "mov",	0xfa910000,	0xfff30000,	FMT_D2, {DM, D16, SP}, },
{ "mov",	0xfc910000,	0xfff30000,	FMT_D4, {DM, D32, SP}, },
{ "mov",	0xf340,		0xffc0,		FMT_D0, {DM, DI, AN}, },
{ "mov",	0x010000,	0xf30000,	FMT_S2, {DM, ABS16}, },
{ "mov",	0xfc810000,	0xfff30000,	FMT_D4, {DM, ABS32}, },
{ "mov",	0xf010,		0xfff0,		FMT_D0, {AM, AN}, },
{ "mov",	0xf83000,	0xfff000,	FMT_D1, {AM, SD8, AN}, },
{ "mov",	0xfa300000,	0xfff00000,	FMT_D2, {AM, SD16, AN}, },
{ "mov",	0xfc300000,	0xfff00000,	FMT_D4, {AM, D32, AN}, },
{ "mov",	0x4300,		0xf300,		FMT_S1, {AM, D8, SP}, },
{ "mov",	0xfa900000,	0xfff30000,	FMT_D2, {AM, D16, SP}, },
{ "mov",	0xfc900000,	0xfc930000,	FMT_D4, {AM, D32, SP}, },
{ "mov",	0xf3c0,		0xffc0,		FMT_D0, {AM, DI, AN}, },
{ "mov",	0xfa800000,	0xfff30000,	FMT_D2, {AM, ABS16}, },
{ "mov",	0xfc800000,	0xfff30000,	FMT_D4, {AM, ABS32}, },
{ "mov",	0xf8f400,	0xfffc00,	FMT_D1, {SP, SD8N, AN}, },
{ "mov",	0x2c0000,	0xfc0000,	FMT_S2, {SIMM16, DN}, },
{ "mov",	0xfcdc0000,	0xfffc0000,	FMT_D4, {IMM32, DN}, },
{ "mov",	0x240000,	0xfc0000,	FMT_S2, {IMM16, AN}, },
{ "mov",	0xfcdc0000,	0xfffc0000,	FMT_D4, {IMM32, AN}, },

{ "movbu",	0xf040,		0xfff0,		FMT_D0, {AM, DN}, },
{ "movbu",	0xf84000,	0xfff000,	FMT_D1, {SD8, AM, DN}, },
{ "movbu",	0xf8400000,	0xfff00000,	FMT_D2, {SD16, AM, DN}, },
{ "movbu",	0xfc400000,	0xfff00000,	FMT_D4, {D32, AM, DN}, },
{ "movbu",	0xf8b800,	0xfffc00,	FMT_D1, {D8, SP, DN}, },
{ "movbu",	0xfab80000,	0xfffc0000,	FMT_D2, {D16, SP, DN}, },
{ "movbu",	0xfcd80000,	0xfffc0000,	FMT_D4, {D32, SP, DN}, },
{ "movbu",	0xf400,		0xffc0,		FMT_D0, {DI, AM, DN}, },
{ "movbu",	0x340000,	0xfc0000,	FMT_S2, {ABS16, DN}, },
{ "movbu",	0xfca80000,	0xfffc0000,	FMT_D4, {ABS32, DN}, },
{ "movbu",	0xf050,		0xfff0,		FMT_D0, {DM, AN}, },
{ "movbu",	0xf85000,	0xfff000,	FMT_D1, {DM, SD8, AN}, },
{ "movbu",	0xfa500000,	0xfff00000,	FMT_D2, {DM, SD16, AN}, },
{ "movbu",	0xfc500000,	0xfff00000,	FMT_D4, {DM, D32, AN}, },
{ "movbu",	0xf89200,	0xfff300,	FMT_D1, {DM, D8, SP}, },
{ "movbu",	0xfa920000,	0xfff30000,	FMT_D2, {DM, D16, SP}, },
{ "movbu",	0xfc920000,	0xfff30000,	FMT_D4, {DM, D32, SP}, },
{ "movbu",	0xf440,		0xffc0,		FMT_D0, {DM, DI, AN}, },
{ "movbu",	0x020000,	0xf30000,	FMT_S2, {DM, ABS16}, },
{ "movbu",	0xfc820000,	0xfff30000,	FMT_D4, {DM, ABS32}, },

{ "movhu",	0xf060,		0xfff0,		FMT_D0, {AM, DN}, },
{ "movhu",	0xf86000,	0xfff000,	FMT_D1, {SD8, AM, DN}, },
{ "movhu",	0xfa600000,	0xfff00000,	FMT_D2, {SD16, AM, DN}, },
{ "movhu",	0xfc600000,	0xfff00000,	FMT_D4, {D32, AM, DN}, },
{ "movhu",	0xf8bc00,	0xfffc00,	FMT_D1, {D8, SP, DN}, },
{ "movhu",	0xfabc0000,	0xfffc0000,	FMT_D2, {D16, SP, DN}, },
{ "movhu",	0xfcdc0000,	0xfffc0000,	FMT_D4, {D32, SP, DN}, }, /* XXX */
{ "movhu",	0xf480,		0xffc0,		FMT_D0, {DI, AM, DN}, },
{ "movhu",	0xc80000,	0xfc0000,	FMT_S2, {ABS16, DN}, },
{ "movhu",	0xfcac0000,	0xfffc0000,	FMT_D4, {ABS32, DN}, },
{ "movhu",	0xf070,		0xfff0,		FMT_D0, {DM, AN}, },
{ "movhu",	0xf87000,	0xfff000,	FMT_D1, {DM, SD8, AN}, },
{ "movhu",	0xfa700000,	0xfff00000,	FMT_D2, {DM, SD16, AN}, },
{ "movhu",	0xfc700000,	0xfff00000,	FMT_D4, {DM, D32, AN}, },
{ "movhu",	0xf89300,	0xfff300,	FMT_D1, {DM, D8, SP}, },
{ "movhu",	0xfa930000,	0xfff30000,	FMT_D2, {DM, D16, SP}, },
{ "movhu",	0xfa930000,	0xfff30000,	FMT_D4, {DM, D32, SP}, },
{ "movhu",	0xf4c0,		0xffc0,		FMT_D0, {DM, DI, AN}, },
{ "movhu",	0x030000,	0xf30000,	FMT_S2, {DM, ABS16}, },
{ "movhu",	0xfc830000,	0xfff30000,	FMT_D4, {DM, ABS32}, },

{ "ext",	0xf2d0,		0xfffc,		FMT_D0, {DN}, },
{ "extb",	0x10, 		0xfc,		FMT_S0, {DN}, },
{ "extbu",	0x14,		0xfc,		FMT_S0, {DN}, },
{ "exth",	0x18,		0xfc,		FMT_S0, {DN}, },
{ "exthu",	0x1c,		0xfc,		FMT_S0, {DN}, },

{ "movm",	0xce00,		0xff00,		FMT_S1, {SP, REGS}, },
{ "movm",	0xcf00,		0xff00,		FMT_S1, {REGS, SP}, },

{ "clr",	0x00,		0xf3,		FMT_S0, {DN}, },

{ "add",	0xe0,		0xf0,		FMT_S0, {DM, DN}, },
{ "add",	0xf160,		0xfff0,		FMT_D0, {DM, AN}, },
{ "add",	0xf150,		0xfff0,		FMT_D0, {AM, DN}, },
{ "add",	0xf170,		0xfff0,		FMT_D0, {AM, AN}, },
{ "add",	0x2800,		0xfc00,		FMT_S1, {SIMM8, DN}, },
{ "add",	0xfac00000,	0xfffc0000,	FMT_D2, {SIMM16, DN}, },
{ "add",	0xfcc00000,	0xfffc0000,	FMT_D4, {IMM32, DN}, },
{ "add",	0x2000,		0xfc00,		FMT_S1, {SIMM8, AN}, },
{ "add",	0xfad00000,	0xfffc0000,	FMT_D2, {SIMM16, AN}, },
{ "add",	0xfcd00000,	0xfffc0000,	FMT_D4, {IMM32, AN}, },
{ "add",	0xf8fe00,	0xffff00,	FMT_D1, {SIMM8, SP}, },
{ "add",	0xfafe0000,	0xfffc0000,	FMT_D2, {SIMM16, SP}, },
{ "add",	0xfcfe0000,	0xfff0000,	FMT_D4, {IMM32, SP}, },
{ "addc",	0xf140,		0xfff0,		FMT_D0, {DM, DN}, },

{ "sub",	0xf100,		0xfff0,		FMT_D0, {DM, DN}, },
{ "sub",	0xf120,		0xfff0,		FMT_D0, {DM, AN}, },
{ "sub",	0xf110,		0xfff0,		FMT_D0, {AM, DN}, },
{ "sub",	0xf130,		0xfff0,		FMT_D0, {AM, AN}, },
{ "sub",	0xfcc40000,	0xfffc0000,	FMT_D4, {IMM32, DN}, },
{ "sub",	0xfcd80000,	0xfffc0000,	FMT_D4, {IMM32, AN}, },
{ "subc",	0xf180,		0xfff0,		FMT_D0, {DM, DN}, },

{ "mul",	0xf240,		0xfff0,		FMT_D0, {DM, DN}, },
{ "mulu",	0xf250,		0xfff0,		FMT_D0, {DM, DN}, },

{ "div",	0xf260,		0xfff0,		FMT_D0, {DM, DN}, },
{ "divu",	0xf270,		0xfff0,		FMT_D0, {DM, DN}, },

{ "inc",	0x40,		0xf3,		FMT_S0, {DN}, },
{ "inc",	0x41,		0xf3,		FMT_S0, {AN}, },
{ "inc4",	0x50,		0xfc,		FMT_S0, {AN}, },

{ "cmp",	0xa000,		0xf000,		FMT_S1, {SIMM8, DN}, },
{ "cmp",	0xa0,		0xf0,		FMT_S0, {DM, DN}, },
{ "cmp",	0xf1a0,		0xfff0,		FMT_D0, {DM, AN}, },
{ "cmp",	0xf190,		0xfff0,		FMT_D0, {AM, DN}, },
{ "cmp",	0xb000,		0xf000,		FMT_S1, {IMM8, AN}, },
{ "cmp",	0xb0,		0xf0,		FMT_S0, {AM, AN}, },
{ "cmp",	0xfac80000,	0xfffc0000,	FMT_D2, {SIMM16, DN}, },
{ "cmp",	0xfcc80000,	0xfffc0000,	FMT_D4, {IMM32, DN}, },
{ "cmp",	0xfad80000,	0xfffc0000,	FMT_D2, {IMM16, AN}, },
{ "cmp",	0xfcd80000,	0xfffc0000,	FMT_D4, {IMM32, AN}, },

{ "and",	0xf200,		0xfff0,		FMT_D0, {DM, DN}, },
{ "and",	0xf8e000,	0xfffc00,	FMT_D1, {IMM8, DN}, },
{ "and",	0xfae00000,	0xfffc0000,	FMT_D2, {IMM16, DN}, },
{ "and",	0xfce00000,	0xfffc0000,	FMT_D4, {IMM32, DN}, },
{ "and",	0xfafc0000,	0xfffc0000,	FMT_D2, {IMM16, PSW}, },
{ "or",		0xf210,		0xfff0,		FMT_D0, {DM, DN}, },
{ "or",		0xf8e400,	0xfffc00,	FMT_D1, {IMM8, DN}, },
{ "or",		0xfae40000,	0xfffc0000,	FMT_D2, {IMM16, DN}, },
{ "or",		0xfce40000,	0xfffc0000,	FMT_D4, {IMM32, DN}, },
{ "or",		0xfafd0000,	0xfffc0000,	FMT_D2, {IMM16, PSW}, },
{ "xor",	0xf220,		0xfff0,		FMT_D0, {DM, DN}, },
{ "xor",	0xfae80000,	0xfffc0000,	FMT_D2, {IMM16, DN}, },
{ "xor",	0xfce80000,	0xfffc0000,	FMT_D4, {IMM32, DN}, },
{ "not",	0xf230,		0xfffc,		FMT_D0, {DN}, },

{ "btst",	0xf8ec00,	0xfffc00,	FMT_D1, {IMM8, DN}, },
{ "btst",	0xfaec0000,	0xfffc0000,	FMT_D2, {IMM16, DN}, },
{ "btst",	0xfcec0000,	0xfffc0000,	FMT_D4, {IMM32, DN}, },
{ "btst",	0xfaf80000,	0xfffc0000,	FMT_D2, {IMM8, SD8N, AN}, },
{ "btst",	0xfe020000,	0xffff0000,	FMT_D5, {IMM8, ABS32}, },
{ "bset",	0xf080,		0xfff0,		FMT_D0, {DM, AN}, },
{ "bset",	0xfaf00000,	0xfffc0000,	FMT_D2, {IMM8, SD8N, AN}, },
{ "bset",	0xfe000000,	0xffff0000,	FMT_D5, {IMM8, ABS32}, },
{ "bclr",	0xf090,		0xfff0,		FMT_D0, {DM, AN}, },
{ "bclr",	0xfaf40000,	0xfffc0000,	FMT_D2, {IMM8, SD8N, AN}, },
{ "bclr",	0xfe010000,	0xffff0000,	FMT_D5, {IMM8, ABS32}, },

{ "asr",	0xf2b0,		0xfff0,		FMT_D0, {DM, DN}, },
{ "asr",	0xf8c800,	0xfffc00,	FMT_D1, {IMM8, DN}, },
{ "lsr",	0xf2a0,		0xfff0,		FMT_D0, {DM, DN}, },
{ "lsr",	0xf8c4,		0xfffc00,	FMT_D0, {IMM8, DN}, },
{ "asl",	0xf290,		0xfff0,		FMT_D0, {DM, DN}, },
{ "asl",	0xf8c000,	0xfffc00,	FMT_D0, {IMM8, DN}, },
{ "asl2",	0x54,		0xfc,		FMT_S0, {DN}, },
{ "ror",	0xf284,		0xfffc,		FMT_D0, {DN}, },
{ "rol",	0xf280,		0xfffc,		FMT_D0, {DN}, },

{ "beq",	0xc800,		0xff00,		FMT_S1, {SD8N}, },
{ "bne",	0xc900,		0xff00,		FMT_S1, {SD8N}, },
{ "bgt",	0xc100,		0xff00,		FMT_S1, {SD8N}, },
{ "bge",	0xc200,		0xff00,		FMT_S1, {SD8N}, },
{ "ble",	0xc300,		0xff00,		FMT_S1, {SD8N}, },
{ "blt",	0xc000,		0xff00,		FMT_S1, {SD8N}, },
{ "bhi",	0xc500,		0xff00,		FMT_S1, {SD8N}, },
{ "bcc",	0xc600,		0xff00,		FMT_S1, {SD8N}, },
{ "bls",	0xc700,		0xff00,		FMT_S1, {SD8N}, },
{ "bcs",	0xc400,		0xff00,		FMT_S1, {SD8N}, },
{ "bvc",	0xf8e800,	0xffff00,	FMT_D1, {SD8N}, },
{ "bvs",	0xf8e900,	0xffff00,	FMT_D1, {SD8N}, },
{ "bnc",	0xf8ea00,	0xffff00,	FMT_D1, {SD8N}, },
{ "bns",	0xf8eb00,	0xffff00,	FMT_D1, {SD8N}, },
{ "bra",	0xca00,		0xff00,		FMT_S1, {SD8N}, },

{ "leq",	0xd8,		0xff,		FMT_S0, {UNUSED} },
{ "lne",	0xd9,		0xff,		FMT_S0, {UNUSED} },
{ "lgt",	0xd1,		0xff,		FMT_S0, {UNUSED} },
{ "lge",	0xd2,		0xff,		FMT_S0, {UNUSED} },
{ "lle",	0xd3,		0xff,		FMT_S0, {UNUSED} },
{ "llt",	0xd0,		0xff,		FMT_S0, {UNUSED} },
{ "lhi",	0xd5,		0xff,		FMT_S0, {UNUSED} },
{ "lcc",	0xd6,		0xff,		FMT_S0, {UNUSED} },
{ "lls",	0xd7,		0xff,		FMT_S0, {UNUSED} },
{ "lcs",	0xd4,		0xff,		FMT_S0, {UNUSED} },
{ "lra",	0xda,		0xff,		FMT_S0, {UNUSED} },
{ "lcc",	0xd6,		0xff,		FMT_S0, {UNUSED} },
{ "setlb",	0xdb,		0xff,	 	FMT_S0, {UNUSED} },

{ "jmp",	0xf0f4,		0xfffc,		FMT_D0, {AN}, },
{ "jmp",	0xcc0000,	0xff0000,	FMT_S2, {D16}, },
{ "jmp",	0xdc0000,	0xff0000,	FMT_S4, {D32}, },
{ "call", 0, 0, FMT_S4, 0, {D16}, },
{ "call",	0xdd000000,	0xff000000,	FMT_S6, {D32,}, },
{ "calls",	0xf0f0,		0xfffc,		FMT_D0, {AN}, },
{ "calls",	0xfaff0000,	0xffff0000,	FMT_D2, {D16}, },
{ "calls",	0xfcff0000,	0xffff0000,	FMT_D4, {D32}, },

{ "ret",	0xdf0000,	0xff00000,	FMT_S2, {UNUSED} },
{ "retf",	0xde0000,	0xff00000,	FMT_S2, {UNUSED} },
{ "rets",	0xf0fc,		0xffff,		FMT_D0, {UNUSED} },
{ "rti",	0xf0fd,		0xffff,		FMT_D0, {UNUSED} },
{ "trap",	0xf0fe,		0xffff,		FMT_D0, {UNUSED} },
{ "rtm",	0xf0ff,		0xffff,		FMT_D0, {UNUSED} },
{ "nop",	0xcb,		0xff,		FMT_S0, {UNUSED} },
/* { "udf", 0, 0, {0}, },  */
{ 0, 0, 0, 0, 0, {0}, },

} ;

const int mn10300_num_opcodes =
  sizeof (mn10300_opcodes) / sizeof (mn10300_opcodes[0]);


