#include "ansidecl.h"
#include "opcode/v850.h"

/* Local insertion and extraction functions.  */
static unsigned long insert_d9 PARAMS ((unsigned long, long, const char **));
static long extract_d9 PARAMS ((unsigned long, int *));

/* regular opcode */
#define OP(x)		((x & 0x3f) << 5)
#define OP_MASK		OP(0x3f)

/* conditional branch opcode */
#define BOP(x)		((0x0b << 7) | (x & 0x0f))
#define BOP_MASK	((0x0b << 7) | 0x0f)

/* one-word opcodes */
#define one(x)		((unsigned int) (x))

/* two-word opcodes */
#define two(x,y)	((unsigned int) (x) | ((unsigned int) (y) << 16))



const struct v850_operand v850_operands[] = {
#define UNUSED	0
  { 0, 0, 0, 0, 0 }, 

/* The R1 field in a format 1, 6, 7, or 9 insn. */
#define R1	(UNUSED+1)
  { 5, 0, 0, 0, V850_OPERAND_REG }, 

/* The R2 field in a format 1, 2, 4, 5, 6, 7, 9 insn. */
#define R2	(R1+1)
  { 5, 11, 0, 0, V850_OPERAND_REG },

/* The IMM5 field in a format 2 insn. */
#define I5	(R2+1)
  { 5, 0, 0, 0, V850_OPERAND_SIGNED }, 

#define I5U	(I5+1)
  { 5, 0, 0, 0, 0 },

/* The IMM16 field in a format 6 insn. */
#define I16	(I5U+1)
  { 16, 16, 0, 0, V850_OPERAND_SIGNED }, 

/* The signed DISP7 field in a format 4 insn. */
#define D7S	(I16+1)
  { 7, 0, 0, 0, V850_OPERAND_SIGNED },

/* The DISP9 field in a format 3 insn. */
#define D9	(D7S+1)
  { 0, 0, insert_d9, extract_d9, V850_OPERAND_SIGNED },

/* The DISP16 field in a format 6 insn. */
#define D16	(D9+1)
  { 16, 16, 0, 0, V850_OPERAND_SIGNED }, 

/* The DISP22 field in a format 4 insn. */
#define D22	(D16+1)
  { 16, 0, 0, 0, 0 },

#define B3	(D22+1)
/* The 3 bit immediate field in format 8 insn.  */
  { 3, 11, 0, 0, 0 },

#define CCCC	(B3+1)
/* The 4 bit condition code in a setf instruction */
  { 4, 0, 0, 0, V850_OPERAND_CC },

/* The unsigned DISP8 field in a format 4 insn. */
#define D8	(CCCC+1)
  { 8, 0, 0, 0, 0 },

/* System register operands.  */
#define SR1	(D8+1)
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

} ; 


/* reg-reg instruction format (Format I) */
#define IF1	{R1, R2}

/* imm-reg instruction format (Format II) */
#define IF2	{I5, R2}

/* conditional branch instruction format (Format III) */
#define IF3	{D9}

/* 16-bit load/store instruction (Format IV) */
#define IF4A	{D7S, EP, R2}
#define IF4B	{R2, D7S, EP}
#define IF4C	{D8, EP, R2}
#define IF4D	{R2, D8, EP}

/* Jump instruction (Format V) */
#define IF5	{D22}

/* 3 operand instruction (Format VI) */
#define IF6	{I16, R1, R2}

/* 3 operand instruction (Format VI) */
#define IF6U	{I16U, R1, R2}

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
{ "sld.b",	one(0x0300),		one(0x0780),	IF4A, 2 },
{ "sld.h",	one(0x0400),		one(0x0780),	IF4A, 2 },
{ "sld.w",	one(0x0500),		one(0x0781),	IF4A, 2 },
{ "sst.b",	one(0x0380),		one(0x0780),	IF4B, 2 },
{ "sst.h",	one(0x0480),		one(0x0780),	IF4D, 2 },
{ "sst.w",	one(0x0501),		one(0x0781),	IF4D, 2 },

{ "ld.b",	two(0x0700,0x0000),	two (0x07e0,0x0000),	IF7A, 4 },
{ "ld.h",	two(0x0720,0x0000),	two (0x07e0,0x0001),	IF7A, 4 },
{ "ld.w",	two(0x0720,0x0001),	two (0x07e0,0x0001),	IF7A, 4 },
{ "st.b",	two(0x0740,0x0000),	two (0x07e0,0x0000),	IF7B, 4 },
{ "st.h",	two(0x0760,0x0000),	two (0x07e0,0x0001),	IF7B, 4 },
{ "st.w",	two(0x0760,0x0001),	two (0x07e0,0x0001),	IF7B, 4 },

/* arithmetic operation instructions */
{ "mov",        OP(0x00),		OP_MASK,	IF1, 2 },
{ "mov",	OP(0x10),		OP_MASK,	IF2, 2 },
{ "movea",	OP(0x31),		OP_MASK,	IF6, 4 },
{ "movhi",	OP(0x32),		OP_MASK,	IF6, 4 },
{ "add",	OP(0x0e),		OP_MASK,	IF1, 2 },
{ "add",	OP(0x12),		OP_MASK,	IF2, 2 },
{ "addi",	OP(0x30),		OP_MASK,	IF6, 4 },
{ "sub",	OP(0x0d),		OP_MASK,	IF1, 2 },
{ "subr", 	OP(0x0c),		OP_MASK,	IF1, 2 },
{ "mulh",	OP(0x07),		OP_MASK,	IF1, 2 },
{ "mulh",	OP(0x17),		OP_MASK,	IF2, 2 },
{ "mulhi",	OP(0x37),		OP_MASK,	IF6, 4 },
{ "divh",	OP(0x02),		OP_MASK,	IF1, 2 },
{ "cmp",	OP(0x0f),		OP_MASK,	IF1, 2 },
{ "cmp",	OP(0x13),		OP_MASK,	IF2, 2 },
{ "setf",	two(0x07e0,0x0000),	two(0x07f0,0xffff), {CCCC,R2}, 4 },

/* saturated operation instructions */
{ "satadd",	OP(0x06),		OP_MASK,	IF1, 2 },
{ "satadd",	OP(0x11),		OP_MASK,	IF2, 2 },
{ "satsub",	OP(0x05),		OP_MASK,	IF1, 2 },
{ "satsubi",	OP(0x33),		OP_MASK,	IF6, 4 },
{ "satsubr",	OP(0x04),		OP_MASK,	IF1, 2 },

/* logical operation instructions */
{ "tst",	OP(0x0b),		OP_MASK,	IF1, 2 },
{ "or",		OP(0x08),		OP_MASK,	IF1, 2 },
{ "ori",	OP(0x34),		OP_MASK,	IF6U, 4 },
{ "and",	OP(0x0a),		OP_MASK,	IF1, 2 },
{ "andi",	OP(0x36),		OP_MASK,	IF6U, 4 },
{ "xor",	OP(0x09),		OP_MASK,	IF1, 2 },
{ "xori",	OP(0x35),		OP_MASK,	IF6U, 4 },
{ "not",	OP(0x01),		OP_MASK,	IF1, 2 },
{ "sar",	OP(0x15),		OP_MASK,	{I5U, R2}, 2 },
{ "sar",	two(0x07e0,0x00a0),	two(0x07e0,0xffff),	{R1,R2}, 4 },
{ "shl",	OP(0x16),		OP_MASK,		{I5U, R2}, 2 },
{ "shl",	two(0x07e0,0x00c0),	two(0x07e0,0xffff),	{R1,R2}, 4 },
{ "shr",	OP(0x14),		OP_MASK,		{I5U, R2}, 2 },
{ "shr",	two(0x07e0,0x0080),	two(0x07e0,0xffff),	{R1,R2}, 4 },

/* branch instructions */
	/* signed integer */
{ "bgt",	BOP(0xf),		BOP_MASK,	IF3, 2 },
{ "bge",	BOP(0xe),		BOP_MASK,	IF3, 2 },
{ "blt",	BOP(0x6),		BOP_MASK,	IF3, 2 },
{ "ble",	BOP(0x7),		BOP_MASK,	IF3, 2 },
	/* unsigned integer */
{ "bh",		BOP(0xb),		BOP_MASK,	IF3, 2 },
{ "bnh",	BOP(0x3),		BOP_MASK,	IF3, 2 },
{ "bl",		BOP(0x1),		BOP_MASK,	IF3, 2 },
{ "bnl",	BOP(0x9),		BOP_MASK,	IF3, 2 },
	/* common */
{ "be",		BOP(0x2),		BOP_MASK,	IF3, 2 },
{ "bne",	BOP(0xa),		BOP_MASK,	IF3, 2 },
	/* others */
{ "bv",		BOP(0x0),		BOP_MASK,	IF3, 2 },
{ "bnv",	BOP(0x8),		BOP_MASK,	IF3, 2 },
{ "bn",		BOP(0x4),		BOP_MASK,	IF3, 2 },
{ "bp",		BOP(0xc),		BOP_MASK,	IF3, 2 },
{ "bc",		BOP(0x1),		BOP_MASK,	IF3, 2 },
{ "bnc",	BOP(0x9),		BOP_MASK,	IF3, 2 },
{ "bz",		BOP(0x2),		BOP_MASK,	IF3, 2 },
{ "bnz",	BOP(0xa),		BOP_MASK,	IF3, 2 },
{ "br",		BOP(0x5),		BOP_MASK,	IF3, 2 },
{ "bsa",	BOP(0xd),		BOP_MASK,	IF3, 2 },

{ "jmp",	one(0x0060),		one(0xffe0),	{ R1}, 2 },
{ "jarl",	one(0x0780),		one(0xf83f),	{ D22, R2 }, 4 }, 
{ "jr",		one(0x0780),		one(0xffe0),	{ D22 }, 4 },

/* bit manipulation instructions */
{ "set1",	two(0x07c0,0x0000),	two(0xc7e0,0x0000),	{B3, D16, R1}, 4 },
{ "not1",	two(0x47c0,0x0000),	two(0xc7e0,0x0000),	{B3, D16, R1}, 4 },
{ "clr1",	two(0x87c0,0x0000),	two(0xc7e0,0x0000),	{B3, D16, R1}, 4 },
{ "tst1",	two(0xc7c0,0x0000),	two(0xc7e0,0x0000),	{B3, D16, R1}, 4 },

/* special instructions */
{ "di",		two(0x07e0,0x0160),	two(0xffff,0xffff),	{0}, 4 },
{ "ei",		two(0x87e0,0x0160),	two(0xffff,0xffff),	{0}, 4 },
{ "halt",	two(0x07e0,0x0120),	two(0xffff,0xffff),	{0}, 4 },
{ "reti",	two(0x07e0,0x0140),	two(0xffff,0xffff),	{0}, 4 },
{ "trap",	two(0x07e0,0x0100),	two(0xffe0,0xffff),	{I5U}, 4 },
{ "ldsr",	two(0x07e0,0x0020),	two(0x07e0,0xffff),	{R1,SR2}, 4 },
{ "stsr",	two(0x07e0,0x0040),	two(0x07e0,0xffff),	{SR1,R2}, 4 },
{ "nop",	one(0x00),		one(0xffff),		{0}, 2 },
{ 0, 0, 0, {0}, 0 },

} ;

const int v850_num_opcodes =
  sizeof (v850_opcodes) / sizeof (v850_opcodes[0]);


/* The functions used to insert and extract complicated operands.  */

static unsigned long
insert_d9 (insn, value, errmsg)
     unsigned long insn;
     long value;
     const char **errmsg;
{
  if (value > 511 || value <= -512)
    *errmsg = "value out of range";

  return (insn | ((value & 0x1f0) << 7) | ((value & 0x0e) << 3));
}

static long
extract_d9 (insn, invalid)
     unsigned long insn;
     int *invalid;
{
  long ret = ((insn & 0xf800) >> 7) | ((insn & 0x0070) >> 3);

  if ((insn & 0x8000) != 0)
    ret -= 0x0200;

  return ret;
}
