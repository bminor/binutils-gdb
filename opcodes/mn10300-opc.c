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


const struct mn10300_operand mn10300_operands[] = {
#define UNUSED	0
  { 0, 0, 0 }, 
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

#define S0 0
#define S1 0
#define D0 0
#define D1 0
#define D2 0
#define D4 0
#define D5 0
#define S2 0
#define S4 0
#define S6 0

#define DN 0
#define DM 0
#define AN 0
#define AM 0
#define IMM8 0
#define IMM16 0
#define IMM32 0
#define D8 0
#define D16 0
#define UIMM8 0
#define SP 0
#define PSW 0
#define D32 0
#define MDR 0
#define ABS16 0
#define ABS32 0
#define DI 0
#define REGS 0
const struct mn10300_opcode mn10300_opcodes[] = {
{ "mov",	0x8000,		0xf000,		{S1, IMM8, DN}, },
{ "mov",	0x80,		0xf0,		{S0, DM, DN}, },
{ "mov",	0xf1d0,		0xfff0,		{D0, AM, DN}, },
{ "mov",	0x9000,		0xf000,		{S1, IMM8, AN}, },
{ "mov",	0x90,		0xf0,		{S0, AM, AN}, },
{ "mov",	0x3c,		0xfc,		{S0, SP, AN}, },
{ "mov",	0xf2f0,		0xfff3,		{D0, AM, SP}, },
{ "mov",	0xf2e4,		0xfffc,		{D0, PSW, DN}, },
{ "mov",	0xf2f3,		0xfff3,		{D0, DM, PSW}, },
{ "mov",	0xf2e0,		0xfffc,		{D0, MDR, DN}, },
{ "mov",	0xf2f2,		0xfff3,		{D0, DM, MDR}, },
{ "mov",	0x70,		0xf0,		{S0, AM, DN}, },
{ "mov",	0xf80000,	0xfff000,	{D1, D8, AM, DN}, },
{ "mov",	0xfa000000,	0xfff00000,	{D2, D16, AM, DN}, },
{ "mov",	0xfc000000,	0xfff00000,	{D4, D32, AM, DN}, },
{ "mov",	0x5800,	0xfc00,			{S1, IMM8, SP, DN}, },
{ "mov",	0xfab40000,	0xfffc0000,	{D2, D16, SP, DN}, },
{ "mov",	0xfcb40000,	0xfffc0000,	{D4, D32, SP, DN}, },
{ "mov",	0xf300,		0xffc0,		{D0, DI, AM, DN}, },
{ "mov",	0x300000,	0xfc0000,	{S2, ABS16, DN}, },
{ "mov",	0xfca40000,	0xfffc0000,	{D4, ABS32, DN}, },
{ "mov",	0x0,	0x0,	{D0, AM, AN}, },
{ "mov",	0x0,	0x0,	{D1, D8, AM, AN}, },
{ "mov",	0x0,	0x0,	{D2, D16, AM, AN}, },
{ "mov",	0x0,	0x0,	{D4, D32, AM, AN}, },
{ "mov",	0x0,	0x0,	{S1, D8, SP, AN}, },
{ "mov",	0x0,	0x0,	{D2, D16, SP, AN}, },
{ "mov",	0x0,	0x0,	{D4, D32, SP, AN}, },
{ "mov",	0x0,	0x0,	{D0, DI, AM, AN}, },
{ "mov",	0x0,	0x0,	{D2, ABS16, DN}, },
{ "mov",	0x0,	0x0,	{D4, ABS32, DN}, },
{ "mov",	0x0,	0x0,	{D1, D8, AM, SP}, },
{ "mov",	0x0,	0x0,	{S0, DM, AN}, },
{ "mov",	0x0,	0x0,	{D1, DM, D8, AN}, },
{ "mov",	0x0,	0x0,	{D2, DM, D16, AN}, },
{ "mov",	0x0,	0x0,	{D4, DM, D32, AN}, },
{ "mov",	0x0,	0x0,	{S1, DM, D8, SP}, },
{ "mov",	0x0,	0x0,	{D2, DM, D16, SP}, },
{ "mov",	0x0,	0x0,	{D4, DM, D32, SP}, },
{ "mov",	0x0,	0x0,	{D0, DM, DI, AN}, },
{ "mov",	0x0,	0x0,	{S2, DM, ABS16}, },
{ "mov",	0x0,	0x0,	{D4, DM, ABS32}, },
{ "mov",	0x0,	0x0,	{D0, AM, AN}, },
{ "mov",	0x0,	0x0,	{D1, AM, D8, AN}, },
{ "mov",	0x0,	0x0,	{D2, AM, D16, AN}, },
{ "mov",	0x0,	0x0,	{D4, AM, D32, AN}, },
{ "mov",	0x0,	0x0,	{S1, AM, D32, SP}, },
{ "mov",	0x0,	0x0,	{D2, AM, D16, SP}, },
{ "mov",	0x0,	0x0,	{D4, AM, D32, SP}, },
{ "mov",	0x0,	0x0,	{D0, AM, DI, AN}, },
{ "mov",	0x0,	0x0,	{D2, AM, ABS16}, },
{ "mov",	0x0,	0x0,	{D4, AM, ABS32}, },
{ "mov",	0x0,	0x0,	{D1, SP, D8, AN}, },
{ "mov",	0x0,	0x0,	{S2, IMM16, DN}, },
{ "mov",	0x0,	0x0,	{D4, IMM32, DN}, },
{ "mov",	0x0,	0x0,	{S2, IMM16, AN}, },
{ "mov",	0x0,	0x0,	{D4, IMM32, AN}, },
{ "movbu", 0, 0, {D0, AM, DN}, },
{ "movbu", 0, 0, {D1, D8, AM, DN}, },
{ "movbu", 0, 0, {D2, D16, AM, DN}, },
{ "movbu", 0, 0, {D4, D32, AM, DN}, },
{ "movbu", 0, 0, {D1, D8, SP, DN}, },
{ "movbu", 0, 0, {D2, D16, SP, DN}, },
{ "movbu", 0, 0, {D4, D32, SP, DN}, },
{ "movbu", 0, 0, {D0, DI, AM, DN}, },
{ "movbu", 0, 0, {S2, ABS16, DN}, },
{ "movbu", 0, 0, {D4, ABS32, DN}, },
{ "movbu", 0, 0, {D0, DM, AN}, },
{ "movbu", 0, 0, {D1, DM, D8, AN}, },
{ "movbu", 0, 0, {D2, DM, D16, AN}, },
{ "movbu", 0, 0, {D4, DM, D32, AN}, },
{ "movbu", 0, 0, {D1, DM, D8, SP}, },
{ "movbu", 0, 0, {D2, DM, D16, SP}, },
{ "movbu", 0, 0, {D4, DM, D32, SP}, },
{ "movbu", 0, 0, {D0, DM, DI, AN}, },
{ "movbu", 0, 0, {S2, ABS16, AN}, },
{ "movbu", 0, 0, {D4, ABS32, AN}, },
{ "movhu", 0, 0, {D0, AM, DN}, },
{ "movhu", 0, 0, {D1, D8, AM, DN}, },
{ "movhu", 0, 0, {D2, D16, AM, DN}, },
{ "movhu", 0, 0, {D4, D32, AM, DN}, },
{ "movhu", 0, 0, {D1, D8, SP, DN}, },
{ "movhu", 0, 0, {D2, D16, SP, DN}, },
{ "movhu", 0, 0, {D4, D32, SP, DN}, },
{ "movhu", 0, 0, {D0, DI, AM, DN}, },
{ "movhu", 0, 0, {S2, ABS16, DN}, },
{ "movhu", 0, 0, {D4, ABS32, DN}, },
{ "movhu", 0, 0, {D0, DM, AN}, },
{ "movhu", 0, 0, {D1, DM, D8, AN}, },
{ "movhu", 0, 0, {D2, DM, D16, AN}, },
{ "movhu", 0, 0, {D4, DM, D32, AN}, },
{ "movhu", 0, 0, {D1, DM, D8, SP}, },
{ "movhu", 0, 0, {D2, DM, D16, SP}, },
{ "movhu", 0, 0, {D4, DM, D32, SP}, },
{ "movhu", 0, 0, {D0, DM, DI, AN}, },
{ "movhu", 0, 0, {S2, ABS16, AN}, },
{ "movbu", 0, 0, {D4, ABS32, AN}, },
{ "ext",	0xf2d0,		0xfffc,		{D0, DN}, },
{ "extb",	0x10, 		0xfc,		{S0, DN}, },
{ "extbu",	0x14,		0xfc,		{S0, DN}, },
{ "exth",	0x18,		0xfc,		{S0, DN}, },
{ "exthu",	0x1c,		0xfc,		{S0, DN}, },
{ "movm", 0, 0, {S1, SP, REGS}, },
{ "movm", 0, 0, {S1, REGS, SP}, },
{ "clr",	0x00,		0xf3,		{S0, DN}, },
{ "add", 0, 0, {S0, DM, DN}, },
{ "add", 0, 0, {D0, DM, AN}, },
{ "add", 0, 0, {D0, AM, DN}, },
{ "add", 0, 0, {D0, AM, AN}, },
{ "add", 0, 0, {S1, IMM8, DN}, },
{ "add", 0, 0, {D2, IMM16, DN}, },
{ "add", 0, 0, {D4, IMM32, DN}, },
{ "add", 0, 0, {S1, IMM8, AN}, },
{ "add", 0, 0, {D2, IMM16, AN}, },
{ "add", 0, 0, {D4, IMM32, AN}, },
{ "add", 0, 0, {D1, IMM8, SP}, },
{ "add", 0, 0, {D2, IMM16, SP}, },
{ "add", 0, 0, {D4, IMM32, SP}, },
{ "addc",	0xf140,		0xfff0,		{D0, DM, DN}, },
{ "sub", 0, 0, {D0, DM, DN}, },
{ "sub", 0, 0, {D0, DM, AN}, },
{ "sub", 0, 0, {D0, AM, DN}, },
{ "sub", 0, 0, {D0, AM, AN}, },
{ "sub", 0, 0, {D4, IMM32, DN}, },
{ "sub", 0, 0, {D4, IMM32, AN}, },
{ "subc",	0xf180,		0xfff0,		{D0, DM, DN}, },
{ "mul", 0, 0, {D0, DM, DN}, },
{ "mulu", 0, 0, {D0, DM, DN}, },
{ "div", 0, 0, {D0, DM, DN}, },
{ "divu", 0, 0, {D0, DM, DN}, },
{ "inc",	0x40,		0xf3,		{S0, DN}, },
{ "inc",	0x41,		0xf3,		{S0, AN}, },
{ "inc4",	0x50,		0xfc,		{S0, AN}, },
{ "cmp", 0, 0, {S1, IMM8, DN}, },
{ "cmp", 0, 0, {S0, DM, DN}, },
{ "cmp", 0, 0, {D0, DM, AN}, },
{ "cmp", 0, 0, {D0, AM, DN}, },
{ "cmp", 0, 0, {S1, IMM8, AN}, },
{ "cmp", 0, 0, {S0, AM, AN}, },
{ "cmp", 0, 0, {D2, IMM16, DN}, },
{ "cmp", 0, 0, {D4, IMM32, DN}, },
{ "cmp", 0, 0, {D2, IMM16, AN}, },
{ "cmp", 0, 0, {D4, IMM32, AN}, },
{ "and", 0, 0, {D0, DM, DN}, },
{ "and", 0, 0, {D1, IMM8, DN}, },
{ "and", 0, 0, {D2, IMM16, DN}, },
{ "and", 0, 0, {D4, IMM32, DN}, },
{ "and", 0, 0, {D2, PSW, DN}, },
{ "or", 0, 0, {D0, DM, DN}, },
{ "or", 0, 0, {D1, IMM8, DN}, },
{ "or", 0, 0, {D2, IMM16, DN}, },
{ "or", 0, 0, {D4, IMM32, DN}, },
{ "or", 0, 0, {D2, PSW, DN}, },
{ "xor", 0, 0, {D0, DM, DN}, },
{ "xor", 0, 0, {D2, IMM16, DN}, },
{ "xor", 0, 0, {D4, IMM32, DN}, },
{ "not",	0xf230,		0xfffc,		{D0, DN}, },
{ "btst", 0, 0, {D1, IMM8, DN}, },
{ "btst", 0, 0, {D2, IMM16, DN}, },
{ "btst", 0, 0, {D4, IMM32, DN}, },
{ "btst", 0, 0, {D2, IMM8, D8, AN}, },
{ "btst", 0, 0, {D5, IMM8, ABS32}, },
{ "bset", 0, 0, {D0, DM, AN}, },
{ "bset", 0, 0, {D2, IMM8, D8, AN}, },
{ "bset", 0, 0, {D5, IMM8, ABS32}, },
{ "bclr", 0, 0, {D0, DM, AN}, },
{ "bclr", 0, 0, {D2, IMM8, D8, AN}, },
{ "bclr", 0, 0, {D5, IMM8, ABS32}, },
{ "asr",	0xf2b0,		0xfff0,		{D0, DM, DN}, },
{ "asr",	0xf8c800,	0xfffc00,	{D1, UIMM8, DN}, },
{ "lsr",	0xf2a0,		0xfff0,		{D0, DM, DN}, },
{ "lsr",	0xf8c4,		0xfffc00,	{D0, UIMM8, DN}, },
{ "asl",	0xf290,		0xfff0,		{D0, DM, DN}, },
{ "asl",	0xf8c000,	0xfffc00,	{D0, UIMM8, DN}, },
{ "asl2",	0x54,		0xfc,		{S0, DN}, },
{ "ror",	0xf284,		0xfffc,		{D0, DN}, },
{ "rol",	0xf280,		0xfffc,		{D0, DN}, },
{ "beq",	0xc800,		0xff00,		{S1, D8}, },
{ "bne",	0xc900,		0xff00,		{S1, D8}, },
{ "bgt",	0xc100,		0xff00,		{S1, D8}, },
{ "bge",	0xc200,		0xff00,		{S1, D8}, },
{ "ble",	0xc300,		0xff00,		{S1, D8}, },
{ "blt",	0xc000,		0xff00,		{S1, D8}, },
{ "bhi",	0xc500,		0xff00,		{S1, D8}, },
{ "bcc",	0xc600,		0xff00,		{S1, D8}, },
{ "bls",	0xc700,		0xff00,		{S1, D8}, },
{ "bcs",	0xc400,		0xff00,		{S1, D8}, },
{ "bvc",	0xf8e800,	0xffff00,	{D1, D8}, },
{ "bvs",	0xf8e900,	0xffff00,	{D1, D8}, },
{ "bnc",	0xf8ea00,	0xffff00,	{D1, D8}, },
{ "bns",	0xf8eb00,	0xffff00,	{D1, D8}, },
{ "bra",	0xca00,		0xff00,		{S1, D8}, },
{ "leq",	0xd8,		0xff,		{S0}, },
{ "lne",	0xd9,		0xff,		{S0}, },
{ "lgt",	0xd1,		0xff,		{S0}, },
{ "lge",	0xd2,		0xff,		{S0}, },
{ "lle",	0xd3,		0xff,		{S0}, },
{ "llt",	0xd0,		0xff,		{S0}, },
{ "lhi",	0xd5,		0xff,		{S0}, },
{ "lcc",	0xd6,		0xff,		{S0}, },
{ "lls",	0xd7,		0xff,		{S0}, },
{ "lcs",	0xd4,		0xff,		{S0}, },
{ "lra",	0xda,		0xff,		{S0}, },
{ "lcc",	0xd6,		0xff,		{S0}, },
{ "setlb",	0xdb,		0xff,	 	{S0}, },
{ "jmp",	0xf0f4,		0xfffc,		{D0, AN}, },
{ "jmp",	0xcc0000,	0xff0000,	{S2, D16}, },
{ "jmp",	0xdc0000,	0xff0000,	{S4, D32}, },
{ "call", 0, 0, {S4, D16,}, },
{ "call", 0, 0, {S6, D32,}, },
{ "calls", 0, 0, {D0, AN}, },
{ "calls", 0, 0, {D2, D16}, },
{ "calls", 0, 0, {D4, D32}, },
{ "ret", 0, 0, {S2}, },
{ "retf", 0, 0, {S2}, },
{ "rets",	0xf0fc,		0xffff,		{D0}, },
{ "rti",	0xf0fd,		0xffff,		{D0}, },
{ "trap",	0xf0fe,		0xffff,		{D0}, },
{ "nop",	0xcb,		0xff,		{S0}, },
/* { "udf", 0, 0, {0}, },  */
} ;

const int mn10300_num_opcodes =
  sizeof (mn10300_opcodes) / sizeof (mn10300_opcodes[0]);


