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

  /* Register in bits 4-0 */

#define REG0 (LUBF + 1)
  { 5, 0, NULL, NULL, TIC80_OPERAND_GPR },

  /* Register in bits 26-22 */

#define REG22 (REG0 + 1)
  { 5, 22, NULL, NULL, TIC80_OPERAND_GPR },

  /* Register in bits 31-27 */

#define REG27 (REG22 + 1)
  { 5, 27, NULL, NULL, TIC80_OPERAND_GPR },

  /* Short signed offset in bits 14-0 */

#define SSOFF (REG27 + 1)
  { 15, 0, NULL, NULL, TIC80_OPERAND_RELATIVE | TIC80_OPERAND_SIGNED },

  /* Long signed offset in following 32 bit word */

#define LSOFF (SSOFF + 1)
  {32, 0, NULL, NULL, TIC80_OPERAND_RELATIVE | TIC80_OPERAND_SIGNED },

  /* BITNUM in bits 31-27 */

#define BITNUM (LSOFF + 1)
  { 5, 27, NULL, NULL, TIC80_OPERAND_BITNUM },

  /* Condition code in bits 31-27 */

#define CC (BITNUM + 1)
  { 5, 27, NULL, NULL, TIC80_OPERAND_CC },

  /* Control register number in bits 14-0 */

#define SICR (CC + 1)
  { 15, 0, NULL, NULL, TIC80_OPERAND_CR },

  /* Control register number in next 32 bit word */

#define LICR (SICR + 1)
  { 32, 0, NULL, NULL, TIC80_OPERAND_CR },

};

const int tic80_num_operands = sizeof (tic80_operands)/sizeof(*tic80_operands);


/* Macros used to generate entries for the opcodes table. */

#define FIXME 0

/* Short-Immediate Format Instructions */
#define OP_SI(x)	(((x) & 0x7F) << 15)
#define MASK_SI		OP_SI(0x7F)
#define MASK_SI_M	OP_SI(0x7B)	/* Short-Immediate with embedded M bit */

/* Long-Immediate Format Instructions */
#define OP_LI(x)	(((x) & 0x3FF) << 12)
#define MASK_LI		OP_LI(0x3FF)
#define MASK_LI_M	OP_LI(0x3F7)	/* Long-Immediate with embedded M bit */

/* Register Format Instructions */
#define OP_REG(x)	OP_LI(x)	/* For readability */
#define MASK_REG	MASK_LI		/* For readability */
#define MASK_REG_M	MASK_LI_M	/* For readability */

const struct tic80_opcode tic80_opcodes[] = {

  /* The "nop" instruction is really "rdcr 0,r0".  We put it first so that this
     specific bit pattern will get dissembled as a nop rather than an rdcr. The
     mask of all ones ensures that this will happen. */

  {"nop",	OP_SI(0x4),	~0,		0,		{0}			},

  /* The "br" instruction is really "bbz target,r0,31".  We put it first so that
     this specific bit pattern will get disassembled as a br rather than bbz. */

  {"br",	OP_SI(0x48),	0xFFFF8000,	0,		{SSOFF}			},
  {"br",	OP_LI(0x391),	0xFFFFF000,	0,		{LSOFF}			},
  {"br",	OP_REG(0x390),	0xFFFFF000,	0,		{REG0}			},

  {"br.a",	OP_SI(0x49),	0xFFFF8000,	0,		{SSOFF}			},
  {"br.a",	OP_LI(0x393),	0xFFFFF000,	0,		{LSOFF}			},
  {"br.a",	OP_REG(0x392),	0xFFFFF000,	0,		{REG0}			},

  /* Signed integer ADD */

  {"add",	OP_SI(0x58),	MASK_SI,	FMT_SI,		{SSI, REG22, REG27}	},
  {"add",	OP_LI(0x3B1),	MASK_LI,	FMT_LI,		{LSI, REG22, REG27}	},
  {"add",	OP_REG(0x3B0),	MASK_REG,	FMT_REG,	{REG0, REG22, REG27}	},

  /* Unsigned integer ADD */

  {"addu",	OP_SI(0x59),	MASK_SI,	FMT_SI,		{SSI, REG22, REG27}	},
  {"addu",	OP_LI(0x3B3),	MASK_LI,	FMT_LI,		{LSI, REG22, REG27}	},
  {"addu",	OP_REG(0x3B2),	MASK_REG,	FMT_REG,	{REG0, REG22, REG27}	},

  /* Bitwise AND */

  {"and",	OP_SI(0x11),	MASK_SI,	FMT_SI,		{SUBF, REG22, REG27}	},
  {"and",	OP_LI(0x323),	MASK_LI,	FMT_LI,		{LUBF, REG22, REG27}	},
  {"and",	OP_REG(0x322),	MASK_REG,	FMT_REG,	{REG0, REG22, REG27}	},

  {"and.tt",	OP_SI(0x11),	MASK_SI,	FMT_SI,		{SUBF, REG22, REG27}	},
  {"and.tt",	OP_LI(0x323),	MASK_LI,	FMT_LI,		{LUBF, REG22, REG27}	},
  {"and.tt",	OP_REG(0x322),	MASK_REG,	FMT_REG,	{REG0, REG22, REG27}	},

  /* Bitwise AND with ones complement of both sources */

  {"and.ff",	OP_SI(0x18),	MASK_SI,	FMT_SI,		{SUBF, REG22, REG27}	},
  {"and.ff",	OP_LI(0x331),	MASK_LI,	FMT_LI,		{LUBF, REG22, REG27}	},
  {"and.ff",	OP_REG(0x330),	MASK_REG,	FMT_REG,	{REG0, REG22, REG27}	},

  /* Bitwise AND with ones complement of source 1 */

  {"and.ft",	OP_SI(0x14),	MASK_SI,	FMT_SI,		{SUBF, REG22, REG27}	},
  {"and.ft",	OP_LI(0x329),	MASK_LI,	FMT_LI,		{LUBF, REG22, REG27}	},
  {"and.ft",	OP_REG(0x328),	MASK_REG,	FMT_REG,	{REG0, REG22, REG27}	},

  /* Bitwise AND with ones complement of source 2 */

  {"and.tf",	OP_SI(0x12),	MASK_SI,	FMT_SI,		{SUBF, REG22, REG27}	},
  {"and.tf",	OP_LI(0x325),	MASK_LI,	FMT_LI,		{LUBF, REG22, REG27}	},
  {"and.tf",	OP_REG(0x324),	MASK_REG,	FMT_REG,	{REG0, REG22, REG27}	},

  /* Branch Bit One - nonannulled */

  {"bbo",	OP_SI(0x4A),	MASK_SI,	FMT_SI, 	{SSOFF, REG22, BITNUM}	},
  {"bbo",	OP_LI(0x395),	MASK_LI,	FMT_LI, 	{LSOFF, REG22, BITNUM}	},
  {"bbo",	OP_REG(0x394),	MASK_REG,	FMT_REG, 	{REG0, REG22, BITNUM}	},

  /* Branch Bit One - annulled */

  {"bbo.a",	OP_SI(0x4B),	MASK_SI,	FMT_SI, 	{SSOFF, REG22, BITNUM}	},
  {"bbo.a",	OP_LI(0x397),	MASK_LI,	FMT_LI, 	{LSOFF, REG22, BITNUM}	},
  {"bbo.a",	OP_REG(0x396),	MASK_REG,	FMT_REG, 	{REG0, REG22, BITNUM}	},

  /* Branch Bit Zero - nonannulled */

  {"bbz",	OP_SI(0x48),	MASK_SI,	FMT_SI, 	{SSOFF, REG22, BITNUM}	},
  {"bbz",	OP_LI(0x391),	MASK_LI,	FMT_LI, 	{LSOFF, REG22, BITNUM}	},
  {"bbz",	OP_REG(0x390),	MASK_REG,	FMT_REG, 	{REG0, REG22, BITNUM}	},

  /* Branch Bit Zero - annulled */

  {"bbz.a",	OP_SI(0x49),	MASK_SI,	FMT_SI, 	{SSOFF, REG22, BITNUM}	},
  {"bbz.a",	OP_LI(0x393),	MASK_LI,	FMT_LI, 	{LSOFF, REG22, BITNUM}	},
  {"bbz.a",	OP_REG(0x392),	MASK_REG,	FMT_REG, 	{REG0, REG22, BITNUM}	},

  /* Branch Conditional - nonannulled */

  {"bcnd",	OP_SI(0x4C),	MASK_SI,	FMT_SI, 	{SSOFF, REG22, CC}	},
  {"bcnd",	OP_LI(0x399),	MASK_LI,	FMT_LI, 	{LSOFF, REG22, CC}	},
  {"bcnd",	OP_REG(0x398),	MASK_REG,	FMT_REG, 	{REG0, REG22, CC}	},

  /* Branch Conditional - annulled */

  {"bcnd.a",	OP_SI(0x4D),	MASK_SI,	FMT_SI, 	{SSOFF, REG22, CC}	},
  {"bcnd.a",	OP_LI(0x39B),	MASK_LI,	FMT_LI, 	{LSOFF, REG22, CC}	},
  {"bcnd.a",	OP_REG(0x39A),	MASK_REG,	FMT_REG, 	{REG0, REG22, CC}	},

  /* Branch Control Register */

  {"brcr",	OP_SI(0x6),	MASK_SI,	FMT_SI,		{SICR}			},
  {"brcr",	OP_LI(0x30D),	MASK_LI,	FMT_LI,		{LICR}			},
  {"brcr",	OP_REG(0x30C),	MASK_REG,	FMT_REG,	{REG0}			},

  /* WORK IN PROGRESS BELOW THIS POINT */

  {"cmnd",	OP_LI(0x305),	MASK_LI,	FMT_LI,		FIXME},
  {"cmnd",	OP_REG(0x304),	MASK_REG,	FMT_REG,	FIXME},
  {"cmnd",	OP_SI(0x2),	MASK_SI,	FMT_SI,		FIXME},
  {"illop0",	OP_SI(0),	MASK_SI,	FMT_SI,		FIXME},
  {"ld",	OP_LI(0x345),	MASK_LI_M,	FMT_LI,		FIXME},
  {"ld",	OP_REG(0x344),	MASK_REG_M,	FMT_REG,	FIXME},
  {"ld",	OP_SI(0x22),	MASK_SI_M,	FMT_SI,		FIXME},
  {"ld.b",	OP_LI(0x341),	MASK_LI_M,	FMT_LI,		FIXME},
  {"ld.b",	OP_REG(0x340),	MASK_REG_M,	FMT_REG,	FIXME},
  {"ld.b",	OP_SI(0x20),	MASK_SI_M,	FMT_SI,		FIXME},
  {"ld.d",	OP_LI(0x347),	MASK_LI_M,	FMT_LI,		FIXME},
  {"ld.d",	OP_REG(0x346),	MASK_REG_M,	FMT_REG,	FIXME},
  {"ld.d",	OP_SI(0x23),	MASK_SI_M,	FMT_SI,		FIXME},
  {"ld.h",	OP_LI(0x343),	MASK_LI_M,	FMT_LI,		FIXME},
  {"ld.h",	OP_REG(0x342),	MASK_REG_M,	FMT_REG,	FIXME},
  {"ld.h",	OP_SI(0x21),	MASK_SI_M,	FMT_SI,		FIXME},
  {"ld.u",	OP_LI(0x355),	MASK_LI_M,	FMT_LI,		FIXME},
  {"ld.u",	OP_REG(0x354),	MASK_REG_M,	FMT_REG,	FIXME},
  {"ld.u",	OP_SI(0x2A),	MASK_SI_M,	FMT_SI,		FIXME},
  {"ld.ub",	OP_LI(0x351),	MASK_LI_M,	FMT_LI,		FIXME},
  {"ld.ub",	OP_REG(0x350),	MASK_REG_M,	FMT_REG,	FIXME},
  {"ld.ub",	OP_SI(0x28),	MASK_SI_M,	FMT_SI,		FIXME},
  {"ld.ud",	OP_LI(0x357),	MASK_LI_M,	FMT_LI,		FIXME},
  {"ld.ud",	OP_REG(0x356),	MASK_REG_M,	FMT_REG,	FIXME},
  {"ld.ud",	OP_SI(0x2B),	MASK_SI_M,	FMT_SI,		FIXME},
  {"ld.uh",	OP_LI(0x353),	MASK_LI_M,	FMT_LI,		FIXME},
  {"ld.uh",	OP_REG(0x352),	MASK_REG_M,	FMT_REG,	FIXME},
  {"ld.uh",	OP_SI(0x29),	MASK_SI_M,	FMT_SI,		FIXME},
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
  {"st",	OP_LI(0x365),	MASK_LI_M,	FMT_LI,		FIXME},
  {"st",	OP_REG(0x364),	MASK_REG_M,	FMT_REG,	FIXME},
  {"st",	OP_SI(0x32),	MASK_SI_M,	FMT_SI,		FIXME},
  {"st.b",	OP_LI(0x361),	MASK_LI_M,	FMT_LI,		FIXME},
  {"st.b",	OP_REG(0x360),	MASK_REG_M,	FMT_REG,	FIXME},
  {"st.b",	OP_SI(0x30),	MASK_SI_M,	FMT_SI,		FIXME},
  {"st.d",	OP_LI(0x367),	MASK_LI_M,	FMT_LI,		FIXME},
  {"st.d",	OP_REG(0x366),	MASK_REG_M,	FMT_REG,	FIXME},
  {"st.d",	OP_SI(0x33),	MASK_SI_M,	FMT_SI,		FIXME},
  {"st.h",	OP_LI(0x363),	MASK_LI_M,	FMT_LI,		FIXME},
  {"st.h",	OP_REG(0x362),	MASK_REG_M,	FMT_REG,	FIXME},
  {"st.h",	OP_SI(0x31),	MASK_SI_M,	FMT_SI,		FIXME},
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

