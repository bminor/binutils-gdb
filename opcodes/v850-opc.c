#include "ansidecl.h"
#include "opcode/v850.h"

/* regular opcode */
#define OP(x)		((x & 0x3f) << 5)
#define OP_MASK		OP(0x3f)

/* conditional branch opcode */
#define BOP(x)		((0x0b << 7) | (x & 0x0f))
#define BOP_MASK	((0x0b << 7) | 0x0f)

/* one-word opcodes */
#define one(x)		((unsigned int) (x))

/* two-word opcodes */
#define two(x,y)	((unsigned int) (y) | ((unsigned int) (x) << 16))



const struct v850_operand v850_operands[] = {
#define UNUSED	0
  { 0, 0, 0 }, 

/* The R1 field in a format 1, 6, 7, or 9 insn. */
#define R1	(UNUSED+1)
  { 5, 0, OPERAND_REG }, 

/* The R2 field in a format 1, 2, 4, 5, 6, 7, 9 insn. */
#define R2	(R1+1)
  { 5, 11, OPERAND_REG },

/* The IMM5 field in a format 2 insn. */
#define I5	(R2+1)
  { 5, 0, OPERAND_NUM }, 

#define IMM16 field in a format 6 insn. */
#define I16	(I5+1)
  { 16, 0, OPERAND_NUM }, 

/* The DISP6 field in a format 4 insn. */
#define D6	(I16+1)
  { 6, 1, OPERAND_NUM },

/* The DISP8 field in a format 3 insn. */
#define D8	(D6+1)
  { 9, 0, OPERAND_NUM },

/* The DISP16 field in a format 6 insn. */
#define D16	(D8+1)
  { 16, 0, OPERAND_NUM }, 

/* The DISP22 field in a format 4 insn. */
#define D22	(D16+1)
  { 16, 0, OPERAND_NUM },

#define B3	(D22+1)
/* The 3 bit immediate field in format 8 insn.  */
  { 3, 11, OPERAND_NUM }
} ; 


/* reg-reg instruction format (Format I) */
#define IF1	{R1, R2}

/* imm-reg instruction format (Format II) */
#define IF2	{I5, R2}

/* conditional branch instruction format (Format III) */
#define IF3	{D8}

/* 16-bit load/store instruction (Format IV) */
#define IF4A	{D6, R2}
#define IF4B	{R2, D6}

/* Jump instruction (Format V) */
#define IF5	{D22}

/* 3 operand instruction (Format VI) */
#define IF6	{I16, R1, R2}

/* 32-bit load/store instruction (Format VII) */
#define IF7A	{D16, R1, R2}
#define IF7B	{R2, D16, R1}

/* Bit manipulation function.  */



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

const struct v850_opcode v850_opcodes[] = {
/* load/store instructions */
{ "sld.b",	OP(0x00),		OP_MASK,	IF4A },
{ "sld.h",	OP(0x00),		OP_MASK,	IF4A },
{ "sld.w",	OP(0x00),		OP_MASK,	IF4A },
{ "sst.b",	OP(0x00),		OP_MASK,	IF4B },
{ "sst.w",	OP(0x00),		OP_MASK,	IF4B },
{ "sst.l",	OP(0x00),		OP_MASK,	IF4B },

{ "ld.b",	OP(0x00),		OP_MASK,	IF7A },
{ "ld.h",	OP(0x00),		OP_MASK,	IF7A },
{ "ld.w",	OP(0x00),		OP_MASK,	IF7A },
{ "st.b",	OP(0x00),		OP_MASK,	IF7B },
{ "st.h",	OP(0x00),		OP_MASK,	IF7B },
{ "st.w",	OP(0x00),		OP_MASK,	IF7B },

/* arithmetic operation instructions */
{ "mov",        OP(0x00),		OP_MASK,	IF1 },
{ "mov",	OP(0x08),		OP_MASK,	IF2 },
{ "movea",	OP(0x31),		OP_MASK,	IF6 },
{ "movhi",	OP(0x31),		OP_MASK,	IF6 },
{ "add",	OP(0x0e),		OP_MASK,	IF1 },
{ "add",	OP(0x12),		OP_MASK,	IF2 },
{ "addi",	OP(0x30),		OP_MASK,	IF6 },
{ "sub",	OP(0x0d),		OP_MASK,	IF1 },
{ "subr", 	OP(0x0c),		OP_MASK,	IF1 },
{ "mulh",	OP(0x07),		OP_MASK,	IF1 },
{ "mulh",	OP(0x17),		OP_MASK,	IF2 },
{ "mulhi",	OP(0x37),		OP_MASK,	IF6 },
{ "divh",	OP(0x02),		OP_MASK,	IF1 },
{ "cmp",	OP(0x0f),		OP_MASK,	IF1 },
{ "cmp",	OP(0x13),		OP_MASK,	IF2 },
/* XXX missing setf */

/* saturated operation instructions */
{ "satadd",	OP(0x06),		OP_MASK,	IF1 },
{ "satadd",	OP(0x11),		OP_MASK,	IF2 },
{ "satsub",	OP(0x05),		OP_MASK,	IF1 },
{ "satsubi",	OP(0x33),		OP_MASK,	IF6 },
{ "satsubr",	OP(0x04),		OP_MASK,	IF1 },

/* logical operation instructions */
{ "tst",	OP(0x0b),		OP_MASK,	IF1 },
{ "or",		OP(0x08),		OP_MASK,	IF1 },
{ "ori",	OP(0x34),		OP_MASK,	IF6 },
{ "and",	OP(0x0a),		OP_MASK,	IF1 },
{ "andi",	OP(0x36),		OP_MASK,	IF6 },
{ "xor",	OP(0x09),		OP_MASK,	IF1 },
{ "xori",	OP(0x35),		OP_MASK,	IF6 },
{ "not",	OP(0x01),		OP_MASK,	IF1 },
{ "sar",	OP(0x15),		OP_MASK,	IF2 },
{ "sar",	two(0x07e0,0x00a0),	two(0x07e0,0xffff),	{R1,R2} },
{ "shl",	OP(0x16),		OP_MASK,		IF2 },
{ "shl",	two(0x07e0,0x00c0),	two(0x07e0,0xffff),	{R1,R2} },
{ "shr",	OP(0x14),		OP_MASK,		IF2 },
{ "shr",	two(0x07e0,0x0080),	two(0x07e0,0xffff),	{R1,R2} },

/* branch instructions */
	/* signed integer */
{ "bgt",	BOP(0xf),		BOP_MASK,	IF3 },
{ "bge",	BOP(0xe),		BOP_MASK,	IF3 },
{ "blt",	BOP(0x6),		BOP_MASK,	IF3 },
{ "ble",	BOP(0x7),		BOP_MASK,	IF3 },
	/* unsigned integer */
{ "bh",		BOP(0xb),		BOP_MASK,	IF3 },
{ "bnh",	BOP(0x3),		BOP_MASK,	IF3 },
{ "bl",		BOP(0x1),		BOP_MASK,	IF3 },
{ "bnl",	BOP(0x9),		BOP_MASK,	IF3 },
	/* common */
{ "be",		BOP(0x2),		BOP_MASK,	IF3 },
{ "bne",	BOP(0xa),		BOP_MASK,	IF3 },
	/* others */
{ "bv",		BOP(0x0),		BOP_MASK,	IF3 },
{ "bnv",	BOP(0x8),		BOP_MASK,	IF3 },
{ "bn",		BOP(0x4),		BOP_MASK,	IF3 },
{ "bp",		BOP(0xc),		BOP_MASK,	IF3 },
{ "bc",		BOP(0x1),		BOP_MASK,	IF3 },
{ "bnc",	BOP(0x9),		BOP_MASK,	IF3 },
{ "bz",		BOP(0x2),		BOP_MASK,	IF3 },
{ "bnz",	BOP(0xa),		BOP_MASK,	IF3 },
{ "br",		BOP(0x5),		BOP_MASK,	IF3 },
{ "bsa",	BOP(0xd),		BOP_MASK,	IF3 },

{ "jmp",	one(0x0060),		one(0xffe0),	R1 },
{ "jarl",	one(0x0780),		one(0xf83f),	{ D22, R2 } }, 
{ "jr",		one(0x0780),		one(0xffe0),	{ D22 } },

/* bit manipulation instructions */
{ "set1",	two(0x07c0,0x0000),	two(0xc7e0,0x0000),	{B3, R1, D16} },
{ "not1",	two(0x47c0,0x0000),	two(0xc7e0,0x0000),	{B3, R1, D16} },
{ "clr1",	two(0x87c0,0x0000),	two(0xc7e0,0x0000),	{B3, R1, D16} },
{ "tst1",	two(0xc7c0,0x0000),	two(0xc7e0,0x0000),	{B3, R1, D16} },

/* special instructions */
{ "di",		two(0x07e0,0x0160),	two(0xffff,0xffff),	{0} },
{ "ei",		two(0x87e0,0x0160),	two(0xffff,0xffff),	{0} },
{ "halt",	two(0x07e0,0x0120),	two(0xffff,0xffff),	{0} },
{ "reti",	two(0x07e0,0x0140),	two(0xffff,0xffff),	{0} },
#if 0
{ "trap",	two(0x07e0,0x0100),	two(0xffe0,0xffff),	{I5} },
#endif
{ "ldsr",	two(0x07e0,0x0020),	two(0x07e0,0xffff),	{0} },
{ "stsr",	two(0x07e0,0x0040),	two(0x07e0,0xffff),	{0} },
{ "nop",	one(0x00),		one(0xff),		{0} },

} ;

const int v850_num_opcodes =
  sizeof (v850_opcodes) / sizeof (v850_opcodes[0]);

