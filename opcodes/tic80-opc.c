/* Opcode table for TI TMS320C80 (MVP).
   Copyright 1996 Free Software Foundation, Inc.

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include "ansidecl.h"
#include "opcode/tic80.h"

/* This file holds the TMS320C80 (MVP) opcode table.  The table is
   strictly constant data, so the compiler should be able to put it in
   the .text section.

   This file also holds the operand table.  All knowledge about
   inserting operands into instructions and vice-versa is kept in this
   file.  */


/* The operands table.  The fields are:

	bits, shift, insertion function, extraction function, flags
 */

const struct tic80_operand tic80_operands[] =
{

  /* The zero index is used to indicate the end of the list of operands.  */

#define UNUSED (0)
  { 0, 0, 0, 0, 0 },

  /* Short signed immediate value in bits 14-0. */

#define SSI (UNUSED + 1)
  { 15, 0, NULL, NULL, TIC80_OPERAND_SIGNED },

  /* Short unsigned immediate value in bits 14-0 */

#define SUI (SSI + 1)
  { 15, 0, NULL, NULL, 0 },

  /* Short unsigned bitfield in bits 14-0.  We distinguish this
     from a regular unsigned immediate value only for the convenience
     of the disassembler and the user. */

#define SUBF (SUI + 1)
  { 15, 0, NULL, NULL, TIC80_OPERAND_BITFIELD },

  /* Long signed immediate in following 32 bit word */

#define LSI (SUBF + 1)
  { 32, 0, NULL, NULL, TIC80_OPERAND_SIGNED },

  /* Long unsigned immediate in following 32 bit word */

#define LUI (LSI + 1)
  { 32, 0, NULL, NULL, 0 },

  /* Long unsigned bitfield in following 32 bit word.  We distinguish
     this from a regular unsigned immediate value only for the
     convenience of the disassembler and the user. */

#define LUBF (LUI + 1)
  { 32, 0, NULL, NULL, TIC80_OPERAND_BITFIELD },

  /* Single precision floating point immediate in following 32 bit
     word. */

#define SPFI (LUBF + 1)
  { 32, 0, NULL, NULL, TIC80_OPERAND_FLOAT },

  /* Register in bits 4-0 */

#define REG_0 (SPFI + 1)
  { 5, 0, NULL, NULL, TIC80_OPERAND_GPR },

  /* Register in bits 26-22 */

#define REG_22 (REG_0 + 1)
  { 5, 22, NULL, NULL, TIC80_OPERAND_GPR },

  /* Register in bits 31-27 */

#define REG_DEST (REG_22 + 1)
  { 5, 27, NULL, NULL, TIC80_OPERAND_GPR },

  /* Short signed offset in bits 14-0 */

#define OFF_SS (REG_DEST + 1)
  { 15, 0, NULL, NULL, TIC80_OPERAND_RELATIVE | TIC80_OPERAND_SIGNED },

  /* Long signed offset in following 32 bit word */

#define OFF_SL (OFF_SS + 1)
  {32, 0, NULL, NULL, TIC80_OPERAND_RELATIVE | TIC80_OPERAND_SIGNED },

  /* BITNUM in bits 31-27 */

#define BITNUM (OFF_SL + 1)
  { 5, 27, NULL, NULL, TIC80_OPERAND_BITNUM },

  /* Condition code in bits 31-27 */

#define CC (BITNUM + 1)
  { 5, 27, NULL, NULL, TIC80_OPERAND_CC },

  /* Control register number in bits 14-0 */

#define CR_SI (CC + 1)
  { 15, 0, NULL, NULL, TIC80_OPERAND_CR },

  /* Control register number in next 32 bit word */

#define CR_LI (CR_SI + 1)
  { 32, 0, NULL, NULL, TIC80_OPERAND_CR },

  /* A base register in bits 26-22, enclosed in parens, with optional ":m"
     flag in bit 17 (short immediate instructions only) */

#define REG_BASE_M_SI (CR_LI + 1)
  { 5, 22, NULL, NULL, TIC80_OPERAND_GPR | TIC80_OPERAND_PARENS | TIC80_OPERAND_M_SI },

  /* A base register in bits 26-22, enclosed in parens, with optional ":m"
   flag in bit 15 (long immediate and register instructions only) */

#define REG_BASE_M_LI (REG_BASE_M_SI + 1)
  { 5, 22, NULL, NULL, TIC80_OPERAND_GPR | TIC80_OPERAND_PARENS | TIC80_OPERAND_M_LI },

  /* Scaled register in bits 4-0, with optional ":s" modifier flag in bit 11 */

#define REG_SCALED (REG_BASE_M_LI + 1)
  { 5, 0, NULL, NULL, TIC80_OPERAND_GPR | TIC80_OPERAND_SCALED },

  /* Long signed immediate in following 32 bit word, with optional ":s" modifier
     flag in bit 11 */

#define LSI_SCALED (REG_SCALED + 1)
  { 32, 0, NULL, NULL, TIC80_OPERAND_SIGNED | TIC80_OPERAND_SCALED },

  /* Unsigned immediate in bits 4-0, used only for shift instructions */

#define ROTATE (LSI_SCALED + 1)
  { 5, 0, NULL, NULL, 0 },

  /* Unsigned immediate in bits 9-5, used only for shift instructions */
#define ENDMASK (ROTATE + 1)
  { 5, 5, NULL, NULL, 0 },

};

const int tic80_num_operands = sizeof (tic80_operands)/sizeof(*tic80_operands);


/* Macros used to generate entries for the opcodes table. */

#define FIXME 0

/* Short-Immediate Format Instructions - basic opcode */
#define OP_SI(x)	(((x) & 0x7F) << 15)
#define MASK_SI		OP_SI(0x7F)

/* Long-Immediate Format Instructions - basic opcode */
#define OP_LI(x)	(((x) & 0x3FF) << 12)
#define MASK_LI		OP_LI(0x3FF)

/* Register Format Instructions - basic opcode */
#define OP_REG(x)	OP_LI(x)	/* For readability */
#define MASK_REG	MASK_LI		/* For readability */

/* The 'n' bit at bit 10 */
#define n(x)		((x) << 10)

/* The 'i' bit at bit 11 */
#define i(x)		((x) << 11)

/* The 'F' bit at bit 27 */
#define F(x)		((x) << 27)

/* The 'E' bit at bit 27 */
#define E(x)		((x) << 27)

/* The 'M' bit at bit 15 in register and long immediate opcodes */
#define M_REG(x)	((x) << 15)
#define M_LI(x)		((x) << 15)

/* The 'M' bit at bit 17 in short immediate opcodes */
#define M_SI(x)		((x) << 17)

/* The 'SZ' field at bits 14-13 in register and long immediate opcodes */
#define SZ_REG(x)	((x) << 13)
#define SZ_LI(x)	((x) << 13)

/* The 'SZ' field at bits 16-15 in short immediate opcodes */
#define SZ_SI(x)	((x) << 15)

/* The 'D' (direct external memory access) bit at bit 10 in long immediate
   and register opcodes. */
#define D(x)		((x) << 10)

/* The 'S' (scale offset by data size) bit at bit 11 in long immediate
   and register opcodes. */
#define S(x)		((x) << 11)

/* The 'PD' field at bits 10-9 in floating point instructions */
#define PD(x)		((x) << 9)

/* The 'P2' field at bits 8-7 in floating point instructions */
#define P2(x)		((x) << 7)

/* The 'P1' field at bits 6-5 in floating point instructions */
#define P1(x)		((x) << 5)


const struct tic80_opcode tic80_opcodes[] = {

  /* The "nop" instruction is really "rdcr 0,r0".  We put it first so that this
     specific bit pattern will get disassembled as a nop rather than an rdcr. The
     mask of all ones ensures that this will happen. */

  {"nop",	OP_SI(0x4),	~0,		0,		{0}			},

  /* The "br" instruction is really "bbz target,r0,31".  We put it first so that
     this specific bit pattern will get disassembled as a br rather than bbz. */

  {"br",	OP_SI(0x48),	0xFFFF8000,	0,		{OFF_SS}			},
  {"br",	OP_LI(0x391),	0xFFFFF000,	0,		{OFF_SL}			},
  {"br",	OP_REG(0x390),	0xFFFFF000,	0,		{REG_0}			},

  {"br.a",	OP_SI(0x49),	0xFFFF8000,	0,		{OFF_SS}			},
  {"br.a",	OP_LI(0x393),	0xFFFFF000,	0,		{OFF_SL}			},
  {"br.a",	OP_REG(0x392),	0xFFFFF000,	0,		{REG_0}			},

  /* Signed integer ADD */

  {"add",	OP_SI(0x58),	MASK_SI,	FMT_SI,		{SSI, REG_22, REG_DEST}	},
  {"add",	OP_LI(0x3B1),	MASK_LI,	FMT_LI,		{LSI, REG_22, REG_DEST}	},
  {"add",	OP_REG(0x3B0),	MASK_REG,	FMT_REG,	{REG_0, REG_22, REG_DEST}	},

  /* Unsigned integer ADD */

  {"addu",	OP_SI(0x59),	MASK_SI,	FMT_SI,		{SSI, REG_22, REG_DEST}	},
  {"addu",	OP_LI(0x3B3),	MASK_LI,	FMT_LI,		{LSI, REG_22, REG_DEST}	},
  {"addu",	OP_REG(0x3B2),	MASK_REG,	FMT_REG,	{REG_0, REG_22, REG_DEST}	},

  /* Bitwise AND */

  {"and",	OP_SI(0x11),	MASK_SI,	FMT_SI,		{SUBF, REG_22, REG_DEST}	},
  {"and",	OP_LI(0x323),	MASK_LI,	FMT_LI,		{LUBF, REG_22, REG_DEST}	},
  {"and",	OP_REG(0x322),	MASK_REG,	FMT_REG,	{REG_0, REG_22, REG_DEST}	},

  {"and.tt",	OP_SI(0x11),	MASK_SI,	FMT_SI,		{SUBF, REG_22, REG_DEST}	},
  {"and.tt",	OP_LI(0x323),	MASK_LI,	FMT_LI,		{LUBF, REG_22, REG_DEST}	},
  {"and.tt",	OP_REG(0x322),	MASK_REG,	FMT_REG,	{REG_0, REG_22, REG_DEST}	},

  /* Bitwise AND with ones complement of both sources */

  {"and.ff",	OP_SI(0x18),	MASK_SI,	FMT_SI,		{SUBF, REG_22, REG_DEST}	},
  {"and.ff",	OP_LI(0x331),	MASK_LI,	FMT_LI,		{LUBF, REG_22, REG_DEST}	},
  {"and.ff",	OP_REG(0x330),	MASK_REG,	FMT_REG,	{REG_0, REG_22, REG_DEST}	},

  /* Bitwise AND with ones complement of source 1 */

  {"and.ft",	OP_SI(0x14),	MASK_SI,	FMT_SI,		{SUBF, REG_22, REG_DEST}	},
  {"and.ft",	OP_LI(0x329),	MASK_LI,	FMT_LI,		{LUBF, REG_22, REG_DEST}	},
  {"and.ft",	OP_REG(0x328),	MASK_REG,	FMT_REG,	{REG_0, REG_22, REG_DEST}	},

  /* Bitwise AND with ones complement of source 2 */

  {"and.tf",	OP_SI(0x12),	MASK_SI,	FMT_SI,		{SUBF, REG_22, REG_DEST}	},
  {"and.tf",	OP_LI(0x325),	MASK_LI,	FMT_LI,		{LUBF, REG_22, REG_DEST}	},
  {"and.tf",	OP_REG(0x324),	MASK_REG,	FMT_REG,	{REG_0, REG_22, REG_DEST}	},

  /* Branch Bit One - nonannulled */

  {"bbo",	OP_SI(0x4A),	MASK_SI,	FMT_SI, 	{OFF_SS, REG_22, BITNUM}	},
  {"bbo",	OP_LI(0x395),	MASK_LI,	FMT_LI, 	{OFF_SL, REG_22, BITNUM}	},
  {"bbo",	OP_REG(0x394),	MASK_REG,	FMT_REG, 	{REG_0, REG_22, BITNUM}	},

  /* Branch Bit One - annulled */

  {"bbo.a",	OP_SI(0x4B),	MASK_SI,	FMT_SI, 	{OFF_SS, REG_22, BITNUM}	},
  {"bbo.a",	OP_LI(0x397),	MASK_LI,	FMT_LI, 	{OFF_SL, REG_22, BITNUM}	},
  {"bbo.a",	OP_REG(0x396),	MASK_REG,	FMT_REG, 	{REG_0, REG_22, BITNUM}	},

  /* Branch Bit Zero - nonannulled */

  {"bbz",	OP_SI(0x48),	MASK_SI,	FMT_SI, 	{OFF_SS, REG_22, BITNUM}	},
  {"bbz",	OP_LI(0x391),	MASK_LI,	FMT_LI, 	{OFF_SL, REG_22, BITNUM}	},
  {"bbz",	OP_REG(0x390),	MASK_REG,	FMT_REG, 	{REG_0, REG_22, BITNUM}	},

  /* Branch Bit Zero - annulled */

  {"bbz.a",	OP_SI(0x49),	MASK_SI,	FMT_SI, 	{OFF_SS, REG_22, BITNUM}	},
  {"bbz.a",	OP_LI(0x393),	MASK_LI,	FMT_LI, 	{OFF_SL, REG_22, BITNUM}	},
  {"bbz.a",	OP_REG(0x392),	MASK_REG,	FMT_REG, 	{REG_0, REG_22, BITNUM}	},

  /* Branch Conditional - nonannulled */

  {"bcnd",	OP_SI(0x4C),	MASK_SI,	FMT_SI, 	{OFF_SS, REG_22, CC}	},
  {"bcnd",	OP_LI(0x399),	MASK_LI,	FMT_LI, 	{OFF_SL, REG_22, CC}	},
  {"bcnd",	OP_REG(0x398),	MASK_REG,	FMT_REG, 	{REG_0, REG_22, CC}	},

  /* Branch Conditional - annulled */

  {"bcnd.a",	OP_SI(0x4D),	MASK_SI,	FMT_SI, 	{OFF_SS, REG_22, CC}	},
  {"bcnd.a",	OP_LI(0x39B),	MASK_LI,	FMT_LI, 	{OFF_SL, REG_22, CC}	},
  {"bcnd.a",	OP_REG(0x39A),	MASK_REG,	FMT_REG, 	{REG_0, REG_22, CC}	},

  /* Branch Control Register */

  {"brcr",	OP_SI(0x6),	MASK_SI,	FMT_SI,		{CR_SI}			},
  {"brcr",	OP_LI(0x30D),	MASK_LI,	FMT_LI,		{CR_LI}			},
  {"brcr",	OP_REG(0x30C),	MASK_REG,	FMT_REG,	{REG_0}			},

  /* Branch and save return - nonannulled */

  {"bsr",	OP_SI(0x40),	MASK_SI,	FMT_SI, 	{OFF_SS, REG_DEST}	},
  {"bsr",	OP_LI(0x381),	MASK_LI,	FMT_LI, 	{OFF_SL, REG_DEST}	},
  {"bsr",	OP_REG(0x380),	MASK_REG,	FMT_REG, 	{REG_0, REG_DEST}	},

  /* Branch and save return - annulled */

  {"bsr.a",	OP_SI(0x41),	MASK_SI,	FMT_SI, 	{OFF_SS, REG_DEST}	},
  {"bsr.a",	OP_LI(0x383),	MASK_LI,	FMT_LI, 	{OFF_SL, REG_DEST}	},
  {"bsr.a",	OP_REG(0x382),	MASK_REG,	FMT_REG, 	{REG_0, REG_DEST}	},

  /* Send command */

  {"cmnd",	OP_SI(0x2),	MASK_SI,	FMT_SI, 	{SUI}			},
  {"cmnd",	OP_LI(0x305),	MASK_LI,	FMT_LI, 	{LUI}			},
  {"cmnd",	OP_REG(0x304),	MASK_REG,	FMT_REG, 	{REG_0}			},

  /* Integer compare */

  {"cmp",	OP_SI(0x50),	MASK_SI,	FMT_SI, 	{SSI, REG_22, REG_DEST}		},
  {"cmp",	OP_LI(0x3A1),	MASK_LI,	FMT_LI, 	{LSI, REG_22, REG_DEST}		},
  {"cmp",	OP_REG(0x3A0),	MASK_REG,	FMT_REG, 	{REG_0, REG_22, REG_DEST}	},

  /* Flush data cache subblock - don't clear subblock preset flag */

  {"dcachec",	OP_SI(0x38),	F(1) | (MASK_SI & ~M_SI(1)),			FMT_SI, {SSI, REG_BASE_M_SI}	},
  {"dcachec",	OP_LI(0x371),	F(1) | (MASK_LI & ~M_LI(1)) | S(1) | D(1),	FMT_LI, {LSI, REG_BASE_M_LI}	},
  {"dcachec",	OP_REG(0x370),	F(1) | (MASK_REG & ~M_REG(1)) | S(1) | D(1),	FMT_REG, {REG_0, REG_BASE_M_LI}	},

  /* Flush data cache subblock - clear subblock preset flag */

  {"dcachef",	OP_SI(0x38) | F(1),	F(1) | (MASK_SI & ~M_SI(1)),			FMT_SI, {SSI, REG_BASE_M_SI}	},
  {"dcachef",	OP_LI(0x371) | F(1),	F(1) | (MASK_LI & ~M_LI(1)) | S(1) | D(1),	FMT_LI, {LSI, REG_BASE_M_LI}	},
  {"dcachef",	OP_REG(0x370) | F(1),	F(1) | (MASK_REG & ~M_REG(1)) | S(1) | D(1),	FMT_REG, {REG_0, REG_BASE_M_LI}	},

  /* Direct load signed data into register */

  {"dld.b",	OP_LI(0x341) | D(1),	(MASK_LI & ~M_REG(1)) | D(1),	FMT_LI,		{LSI_SCALED, REG_BASE_M_LI, REG_DEST}	},
  {"dld.b",	OP_REG(0x340) | D(1),	(MASK_REG & ~M_REG(1)) | D(1),	FMT_REG,	{REG_SCALED, REG_BASE_M_LI, REG_DEST}	},

  {"dld.h",	OP_LI(0x343) | D(1),	(MASK_LI & ~M_REG(1)) | D(1),	FMT_LI,		{LSI_SCALED, REG_BASE_M_LI, REG_DEST}	},
  {"dld.h",	OP_REG(0x342) | D(1),	(MASK_REG & ~M_REG(1)) | D(1),	FMT_REG,	{REG_SCALED, REG_BASE_M_LI, REG_DEST}	},

  {"dld",	OP_LI(0x345) | D(1),	(MASK_LI & ~M_REG(1)) | D(1),	FMT_LI,		{LSI_SCALED, REG_BASE_M_LI, REG_DEST}	},
  {"dld",	OP_REG(0x344) | D(1),	(MASK_REG & ~M_REG(1)) | D(1),	FMT_REG,	{REG_SCALED, REG_BASE_M_LI, REG_DEST}	},

  {"dld.d",	OP_LI(0x347) | D(1),	(MASK_LI & ~M_REG(1)) | D(1),	FMT_LI,		{LSI_SCALED, REG_BASE_M_LI, REG_DEST}	},
  {"dld.d",	OP_REG(0x346) | D(1),	(MASK_REG & ~M_REG(1)) | D(1),	FMT_REG,	{REG_SCALED, REG_BASE_M_LI, REG_DEST}	},

  /* Direct load unsigned data into register */

  {"dld.ub",	OP_LI(0x351) | D(1),	(MASK_LI & ~M_REG(1)) | D(1),	FMT_LI,		{LSI_SCALED, REG_BASE_M_LI, REG_DEST}	},
  {"dld.ub",	OP_REG(0x350) | D(1),	(MASK_REG & ~M_REG(1)) | D(1),	FMT_REG,	{REG_SCALED, REG_BASE_M_LI, REG_DEST}	},

  {"dld.uh",	OP_LI(0x353) | D(1),	(MASK_LI & ~M_REG(1)) | D(1),	FMT_LI,		{LSI_SCALED, REG_BASE_M_LI, REG_DEST}	},
  {"dld.uh",	OP_REG(0x352) | D(1),	(MASK_REG & ~M_REG(1)) | D(1),	FMT_REG,	{REG_SCALED, REG_BASE_M_LI, REG_DEST}	},

  /* Direct store data into memory */

  {"dst.b",	OP_LI(0x361) | D(1),	(MASK_LI & ~M_REG(1)) | D(1),	FMT_LI,		{LSI_SCALED, REG_BASE_M_LI, REG_DEST}	},
  {"dst.b",	OP_REG(0x360) | D(1),	(MASK_REG & ~M_REG(1)) | D(1),	FMT_REG,	{REG_SCALED, REG_BASE_M_LI, REG_DEST}	},

  {"dst.h",	OP_LI(0x363) | D(1),	(MASK_LI & ~M_REG(1)) | D(1),	FMT_LI,		{LSI_SCALED, REG_BASE_M_LI, REG_DEST}	},
  {"dst.h",	OP_REG(0x362) | D(1),	(MASK_REG & ~M_REG(1)) | D(1),	FMT_REG,	{REG_SCALED, REG_BASE_M_LI, REG_DEST}	},

  {"dst",	OP_LI(0x365) | D(1),	(MASK_LI & ~M_REG(1)) | D(1),	FMT_LI,		{LSI_SCALED, REG_BASE_M_LI, REG_DEST}	},
  {"dst",	OP_REG(0x364) | D(1),	(MASK_REG & ~M_REG(1)) | D(1),	FMT_REG,	{REG_SCALED, REG_BASE_M_LI, REG_DEST}	},

  {"dst.d",	OP_LI(0x367) | D(1),	(MASK_LI & ~M_REG(1)) | D(1),	FMT_LI,		{LSI_SCALED, REG_BASE_M_LI, REG_DEST}	},
  {"dst.d",	OP_REG(0x366) | D(1),	(MASK_REG & ~M_REG(1)) | D(1),	FMT_REG,	{REG_SCALED, REG_BASE_M_LI, REG_DEST}	},

  /* Emulation stop */

  {"estop",	OP_LI(0x3FC),	MASK_LI,	FMT_LI,		{0}	},

  /* Emulation trap */

  {"etrap",	OP_SI(0x1) | E(1),	MASK_SI | E(1),		FMT_SI,		{SUI}	},
  {"etrap",	OP_LI(0x303) | E(1),	MASK_LI | E(1),		FMT_LI,		{LUI}	},
  {"etrap",	OP_REG(0x302) | E(1),	MASK_REG | E(1),	FMT_REG,	{REG_0}	},

  /* Extract signed field (actually an sr.ds instruction) */

  {"exts",	OP_SI(0xA) | i(0) | n(1),	MASK_SI | i(1) | n(1),	FMT_SI, 	{ROTATE, ENDMASK, REG_22, REG_DEST}	},
  {"exts",	OP_REG(0x314) | i(0) | n(1),	MASK_REG | i(1) | n(1),	FMT_REG, 	{REG_0, ENDMASK, REG_22, REG_DEST}	},

  /* Extract unsigned field (actually an sr.dz instruction */

  {"extu",	OP_SI(0x8) | i(0) | n(1),	MASK_SI | i(1) | n(1),	FMT_SI, 	{ROTATE, ENDMASK, REG_22, REG_DEST}	},
  {"extu",	OP_REG(0x310) | i(0) | n(1),	MASK_REG | i(1) | n(1),	FMT_REG, 	{REG_0, ENDMASK, REG_22, REG_DEST}	},

  /* Floating-point addition */

  {"fadd.sss",	OP_LI(0x3E1) | PD(0) | P2(0) | P1(0),	MASK_LI | PD(3) | P2(3) | P1(3), FMT_LI, {SPFI, REG_22, REG_DEST}	},
  {"fadd.ssd",	OP_LI(0x3E1) | PD(1) | P2(0) | P1(0),	MASK_LI | PD(3) | P2(3) | P1(3), FMT_LI, {SPFI, REG_22, REG_DEST}	},
  {"fadd.sdd",	OP_LI(0x3E1) | PD(1) | P2(1) | P1(0),	MASK_LI | PD(3) | P2(3) | P1(3), FMT_LI, {SPFI, REG_22, REG_DEST}	},
  {"fadd.dsd",	OP_LI(0x3E1) | PD(1) | P2(0) | P1(1),	MASK_LI | PD(3) | P2(3) | P1(3), FMT_LI, {SPFI, REG_22, REG_DEST}	},
  {"fadd.ddd",	OP_LI(0x3E1) | PD(1) | P2(1) | P1(1),	MASK_LI | PD(3) | P2(3) | P1(3), FMT_LI, {SPFI, REG_22, REG_DEST}	},

  {"fadd.sss",	OP_REG(0x3E0) | PD(0) | P2(0) | P1(0),	MASK_REG | PD(3) | P2(3) | P1(3), FMT_REG, {REG_0, REG_22, REG_DEST}	},
  {"fadd.ssd",	OP_REG(0x3E0) | PD(1) | P2(0) | P1(0),	MASK_REG | PD(3) | P2(3) | P1(3), FMT_REG, {REG_0, REG_22, REG_DEST}	},
  {"fadd.sdd",	OP_REG(0x3E0) | PD(1) | P2(1) | P1(0),	MASK_REG | PD(3) | P2(3) | P1(3), FMT_REG, {REG_0, REG_22, REG_DEST}	},
  {"fadd.dsd",	OP_REG(0x3E0) | PD(1) | P2(0) | P1(1),	MASK_REG | PD(3) | P2(3) | P1(3), FMT_REG, {REG_0, REG_22, REG_DEST}	},
  {"fadd.ddd",	OP_REG(0x3E0) | PD(1) | P2(1) | P1(1),	MASK_REG | PD(3) | P2(3) | P1(3), FMT_REG, {REG_0, REG_22, REG_DEST}	},

  /* Floating point compare */

  {"fcmp.ss",	OP_LI(0x3EB) | PD(0) | P2(0) | P1(0),	MASK_LI | PD(3) | P2(3) | P1(3),  FMT_REG, {SPFI, REG_22, REG_DEST}	},
  {"fcmp.sd",	OP_LI(0x3EB) | PD(0) | P2(1) | P1(0),	MASK_LI | PD(3) | P2(3) | P1(3),  FMT_REG, {SPFI, REG_22, REG_DEST}	},
  {"fcmp.ds",	OP_LI(0x3EB) | PD(0) | P2(0) | P1(1),	MASK_LI | PD(3) | P2(3) | P1(3),  FMT_REG, {SPFI, REG_22, REG_DEST}	},
  {"fcmp.dd",	OP_LI(0x3EB) | PD(0) | P2(1) | P1(1),	MASK_LI | PD(3) | P2(3) | P1(3),  FMT_REG, {SPFI, REG_22, REG_DEST}	},

  {"fcmp.ss",	OP_REG(0x3EA) | PD(0) | P2(0) | P1(0),	MASK_REG | PD(3) | P2(3) | P1(3),  FMT_REG, {REG_0, REG_22, REG_DEST}	},
  {"fcmp.sd",	OP_REG(0x3EA) | PD(0) | P2(1) | P1(0),	MASK_REG | PD(3) | P2(3) | P1(3),  FMT_REG, {REG_0, REG_22, REG_DEST}	},
  {"fcmp.ds",	OP_REG(0x3EA) | PD(0) | P2(0) | P1(1),	MASK_REG | PD(3) | P2(3) | P1(3),  FMT_REG, {REG_0, REG_22, REG_DEST}	},
  {"fcmp.dd",	OP_REG(0x3EA) | PD(0) | P2(1) | P1(1),	MASK_REG | PD(3) | P2(3) | P1(3),  FMT_REG, {REG_0, REG_22, REG_DEST}	},

  /* Floating point divide */

  {"fdiv.sss",	OP_LI(0x3E7) | PD(0) | P2(0) | P1(0),	MASK_LI | PD(3) | P2(3) | P1(3), FMT_LI, {SPFI, REG_22, REG_DEST}	},
  {"fdiv.ssd",	OP_LI(0x3E7) | PD(1) | P2(0) | P1(0),	MASK_LI | PD(3) | P2(3) | P1(3), FMT_LI, {SPFI, REG_22, REG_DEST}	},
  {"fdiv.sdd",	OP_LI(0x3E7) | PD(1) | P2(1) | P1(0),	MASK_LI | PD(3) | P2(3) | P1(3), FMT_LI, {SPFI, REG_22, REG_DEST}	},
  {"fdiv.dsd",	OP_LI(0x3E7) | PD(1) | P2(0) | P1(1),	MASK_LI | PD(3) | P2(3) | P1(3), FMT_LI, {SPFI, REG_22, REG_DEST}	},
  {"fdiv.ddd",	OP_LI(0x3E7) | PD(1) | P2(1) | P1(1),	MASK_LI | PD(3) | P2(3) | P1(3), FMT_LI, {SPFI, REG_22, REG_DEST}	},

  {"fdiv.sss",	OP_REG(0x3E6) | PD(0) | P2(0) | P1(0),	MASK_REG | PD(3) | P2(3) | P1(3), FMT_REG, {REG_0, REG_22, REG_DEST}	},
  {"fdiv.ssd",	OP_REG(0x3E6) | PD(1) | P2(0) | P1(0),	MASK_REG | PD(3) | P2(3) | P1(3), FMT_REG, {REG_0, REG_22, REG_DEST}	},
  {"fdiv.sdd",	OP_REG(0x3E6) | PD(1) | P2(1) | P1(0),	MASK_REG | PD(3) | P2(3) | P1(3), FMT_REG, {REG_0, REG_22, REG_DEST}	},
  {"fdiv.dsd",	OP_REG(0x3E6) | PD(1) | P2(0) | P1(1),	MASK_REG | PD(3) | P2(3) | P1(3), FMT_REG, {REG_0, REG_22, REG_DEST}	},
  {"fdiv.ddd",	OP_REG(0x3E6) | PD(1) | P2(1) | P1(1),	MASK_REG | PD(3) | P2(3) | P1(3), FMT_REG, {REG_0, REG_22, REG_DEST}	},

  /* WORK IN PROGRESS BELOW THIS POINT */

  {"illop0",	OP_SI(0),	MASK_SI,	FMT_SI,		FIXME},

  {"ld",	OP_LI(0x345),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"ld",	OP_REG(0x344),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"ld",	OP_SI(0x22),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"ld.b",	OP_LI(0x341),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"ld.b",	OP_REG(0x340),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"ld.b",	OP_SI(0x20),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"ld.d",	OP_LI(0x347),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"ld.d",	OP_REG(0x346),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"ld.d",	OP_SI(0x23),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"ld.h",	OP_LI(0x343),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"ld.h",	OP_REG(0x342),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"ld.h",	OP_SI(0x21),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"ld.u",	OP_LI(0x355),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"ld.u",	OP_REG(0x354),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"ld.u",	OP_SI(0x2A),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"ld.ub",	OP_LI(0x351),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"ld.ub",	OP_REG(0x350),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"ld.ub",	OP_SI(0x28),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"ld.ud",	OP_LI(0x357),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"ld.ud",	OP_REG(0x356),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"ld.ud",	OP_SI(0x2B),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"ld.uh",	OP_LI(0x353),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"ld.uh",	OP_REG(0x352),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"ld.uh",	OP_SI(0x29),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"or.ff",	OP_LI(0x33D),	MASK_LI,	FMT_LI,		FIXME},
  {"or.ff",	OP_REG(0x33C),	MASK_REG,	FMT_REG,	FIXME},
  {"or.ff",	OP_SI(0x1E),	MASK_SI,	FMT_SI,		FIXME},
  {"or.ft",	OP_LI(0x33B),	MASK_LI,	FMT_LI,		FIXME},
  {"or.ft",	OP_REG(0x33A),	MASK_REG,	FMT_REG,	FIXME},
  {"or.ft",	OP_SI(0x1D),	MASK_SI,	FMT_SI,		FIXME},
  {"or.tf",	OP_LI(0x337),	MASK_LI,	FMT_LI,		FIXME},
  {"or.tf",	OP_REG(0x336),	MASK_REG,	FMT_REG,	FIXME},
  {"or.tf",	OP_SI(0x1B),	MASK_SI,	FMT_SI,		FIXME},
  {"or.tt",	OP_LI(0x32F),	MASK_LI,	FMT_LI,		FIXME},
  {"or.tt",	OP_REG(0x32E),	MASK_REG,	FMT_REG,	FIXME},
  {"or.tt",	OP_SI(0x17),	MASK_SI,	FMT_SI,		FIXME},
  {"rdcr",	OP_LI(0x309),	MASK_LI,	FMT_LI,		FIXME},
  {"rdcr",	OP_REG(0x308),	MASK_REG,	FMT_REG,	FIXME},
  {"rdcr",	OP_SI(0x4),	MASK_SI,	FMT_SI,		FIXME},
  {"shift.dm",	OP_REG(0x312),	MASK_REG,	FMT_REG,	FIXME},
  {"shift.dm",	OP_SI(0x9),	MASK_SI,	FMT_SI,		FIXME},
  {"shift.ds",	OP_REG(0x314),	MASK_REG,	FMT_REG,	FIXME},
  {"shift.ds",	OP_SI(0xA),	MASK_SI,	FMT_SI,		FIXME},
  {"shift.dz",	OP_REG(0x310),	MASK_REG,	FMT_REG,	FIXME},
  {"shift.dz",	OP_SI(0x8),	MASK_SI,	FMT_SI,		FIXME},
  {"shift.em",	OP_REG(0x318),	MASK_REG,	FMT_REG,	FIXME},
  {"shift.em",	OP_SI(0xC),	MASK_SI,	FMT_SI,		FIXME},
  {"shift.es",	OP_REG(0x31A),	MASK_REG,	FMT_REG,	FIXME},
  {"shift.es",	OP_SI(0xD),	MASK_SI,	FMT_SI,		FIXME},
  {"shift.ez",	OP_REG(0x316),	MASK_REG,	FMT_REG,	FIXME},
  {"shift.ez",	OP_SI(0xB),	MASK_SI,	FMT_SI,		FIXME},
  {"shift.im",	OP_REG(0x31E),	MASK_REG,	FMT_REG,	FIXME},
  {"shift.im",	OP_SI(0xF),	MASK_SI,	FMT_SI,		FIXME},
  {"shift.iz",	OP_REG(0x31C),	MASK_REG,	FMT_REG,	FIXME},
  {"shift.iz",	OP_SI(0xE),	MASK_SI,	FMT_SI,		FIXME},
  {"st",	OP_LI(0x365),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"st",	OP_REG(0x364),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"st",	OP_SI(0x32),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"st.b",	OP_LI(0x361),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"st.b",	OP_REG(0x360),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"st.b",	OP_SI(0x30),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"st.d",	OP_LI(0x367),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"st.d",	OP_REG(0x366),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"st.d",	OP_SI(0x33),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"st.h",	OP_LI(0x363),	MASK_LI & ~M_LI(1),	FMT_LI,		FIXME},
  {"st.h",	OP_REG(0x362),	MASK_REG & ~M_REG(1),	FMT_REG,	FIXME},
  {"st.h",	OP_SI(0x31),	MASK_SI & ~M_SI(1),	FMT_SI,		FIXME},
  {"swcr",	OP_LI(0x30B),	MASK_LI,	FMT_LI,		FIXME},
  {"swcr",	OP_REG(0x30A),	MASK_REG,	FMT_REG,	FIXME},
  {"swcr",	OP_SI(0x5),	MASK_SI,	FMT_SI,		FIXME},
  {"trap",	OP_LI(0x303),	MASK_LI,	FMT_LI,		FIXME},
  {"trap",	OP_REG(0x302),	MASK_REG,	FMT_REG,	FIXME},
  {"trap",	OP_SI(0x1),	MASK_SI,	FMT_SI,		FIXME},
  {"xnor",	OP_LI(0x333),	MASK_LI,	FMT_LI,		FIXME},
  {"xnor",	OP_REG(0x332),	MASK_REG,	FMT_REG,	FIXME},
  {"xnor",	OP_SI(0x19),	MASK_SI,	FMT_SI,		FIXME},
  {"xor",	OP_LI(0x32D),	MASK_LI,	FMT_LI,		FIXME},
  {"xor",	OP_REG(0x32C),	MASK_REG,	FMT_REG,	FIXME},
  {"xor",	OP_SI(0x16),	MASK_SI,	FMT_SI,		FIXME},

};

const int tic80_num_opcodes = sizeof (tic80_opcodes) / sizeof (tic80_opcodes[0]);

