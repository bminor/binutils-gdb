/* Opcode table for m680[01234]0/m6888[12]/m68851.
   Copyright (C) 1989, 1991 Free Software Foundation.

This file is part of GDB, the GNU Debugger and GAS, the GNU Assembler.

Both GDB and GAS are free software; you can redistribute and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB and GAS are distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB or GAS; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* These are used as bit flags for arch below. */

enum m68k_architecture { a,b };

#define	_m68k_undef  0
#define	m68000  0x01
#define	m68008  m68000 /* synonym for -m68000.  otherwise unused. */
#define	m68010  0x02
#define	m68020  0x04
#define	m68030  0x08
#define	m68040  0x10
#define	m68881  0x20
#define	m68882  m68881 /* synonym for -m68881.  otherwise unused. */
#define	m68851  0x40

 /* handy aliases */
#define	m68040up m68040
#define	m68030up  (m68030 | m68040up)
#define	m68020up  (m68020 | m68030up)
#define	m68010up  (m68010 | m68020up)
#define	m68000up  (m68000 | m68010up)

#define	mfloat  (m68881 | m68882 | m68040)
#define	mmmu    (m68851 | m68030 | m68040)


 /* note that differences in addressing modes that aren't distinguished
    in the following table are handled explicitly by gas. */

struct m68k_opcode {
  char *name;
  unsigned long opcode;
  unsigned long  match;
  char *args;
  int  arch;
};

#ifdef MOTOROLA_SYNTAX
#define V(MIT_VARIANT,MOTOROLA_VARIANT) MOTOROLA_VARIANT
#ifdef __STDC__
#define OP2(name,ext) #name "." #ext
#else
#define OP2(name,ext) "name.ext"
#endif
#else /* MIT/Unix syntax */
#define V(MIT_VARIANT,MOTOROLA_VARIANT) MIT_VARIANT
#ifdef __STDC__
#define OP2(name,ext) #name #ext
#else
#define OP2(name,ext) "name\
ext"
#endif
#endif


/* We store four bytes of opcode for all opcodes because that
   is the most any of them need.  The actual length of an instruction
   is always at least 2 bytes, and is as much longer as necessary to
   hold the operands it has.

   The match component is a mask saying which bits must match
   particular opcode in order for an instruction to be an instance
   of that opcode.

   The args component is a string containing two characters
   for each operand of the instruction.  The first specifies
   the kind of operand; the second, the place it is stored.  */

/* Kinds of operands:
   D  data register only.  Stored as 3 bits.
   A  address register only.  Stored as 3 bits.
   a  address register indirect only.  Stored as 3 bits.
   R  either kind of register.  Stored as 4 bits.
   F  floating point coprocessor register only.   Stored as 3 bits.
   O  an offset (or width): immediate data 0-31 or data register.
      Stored as 6 bits in special format for BF... insns.
   +  autoincrement only.  Stored as 3 bits (number of the address register).
   -  autodecrement only.  Stored as 3 bits (number of the address register).
   Q  quick immediate data.  Stored as 3 bits.
      This matches an immediate operand only when value is in range 1 .. 8.
   M  moveq immediate data.  Stored as 8 bits.
      This matches an immediate operand only when value is in range -128..127
   T  trap vector immediate data.  Stored as 4 bits.

   k  K-factor for fmove.p instruction.   Stored as a 7-bit constant or
      a three bit register offset, depending on the field type.

   #  immediate data.  Stored in special places (b, w or l)
      which say how many bits to store.
   ^  immediate data for floating point instructions.   Special places
      are offset by 2 bytes from '#'...
   B  pc-relative address, converted to an offset
      that is treated as immediate data.
   d  displacement and register.  Stores the register as 3 bits
      and stores the displacement in the entire second word.

   C  the CCR.  No need to store it; this is just for filtering validity.
   S  the SR.  No need to store, just as with CCR.
   U  the USP.  No need to store, just as with CCR.

   I  Coprocessor ID.   Not printed if 1.   The Coprocessor ID is always
      extracted from the 'd' field of word one, which means that an extended
      coprocessor opcode can be skipped using the 'i' place, if needed.

   s  System Control register for the floating point coprocessor.
   S  List of system control registers for floating point coprocessor.

   J  Misc register for movec instruction, stored in 'j' format.
	Possible values:
	0x000	SFC	Source Function Code reg	[40, 30, 20, 10]
	0x001	DFC	Data Function Code reg		[40, 30, 20, 10]
	0x002	CACR	Cache Control Register		[40, 30, 20]
	0x800	USP	User Stack Pointer		[40, 30, 20, 10]
	0x801	VBR	Vector Base reg			[40, 30, 20, 10]
	0x802	CAAR	Cache Address Register		[    30, 20]
	0x803	MSP	Master Stack Pointer		[40, 30, 20]
	0x804	ISP	Interrupt Stack Pointer		[40, 30, 20]
	0x003	TC	MMU Translation Control		[40]
	0x004	ITT0	Instruction Transparent
				Translation reg 0	[40]
	0x005	ITT1	Instruction Transparent
				Translation reg 1	[40]
	0x006	DTT0	Data Transparent
				Translation reg 0	[40]
	0x007	DTT1	Data Transparent
				Translation reg 1	[40]
	0x805	MMUSR	MMU Status reg			[40]
	0x806	URP	User Root Pointer		[40]
	0x807	SRP	Supervisor Root Pointer		[40]

    L  Register list of the type d0-d7/a0-a7 etc.
       (New!  Improved!  Can also hold fp0-fp7, as well!)
       The assembler tries to see if the registers match the insn by
       looking at where the insn wants them stored.

    l  Register list like L, but with all the bits reversed.
       Used for going the other way. . .

    c  cache identifier which may be "nc" for no cache, "ic"
       for instruction cache, "dc" for data cache, or "bc"
       for both caches.  Used in cinv and cpush.  Always
       stored in position "d".

 They are all stored as 6 bits using an address mode and a register number;
 they differ in which addressing modes they match.

   *  all					(modes 0-6,7.*)
   ~  alterable memory				(modes 2-6,7.0,7.1)(not 0,1,7.~)
   %  alterable					(modes 0-6,7.0,7.1)(not 7.~)
   ;  data					(modes 0,2-6,7.*)(not 1)
   @  data, but not immediate			(modes 0,2-6,7.? ? ?)(not 1,7.?)  This may really be ;, the 68020 book says it is
   !  control					(modes 2,5,6,7.*-)(not 0,1,3,4,7.4)
   &  alterable control				(modes 2,5,6,7.0,7.1)(not 0,1,7.? ? ?)
   $  alterable data				(modes 0,2-6,7.0,7.1)(not 1,7.~)
   ?  alterable control, or data register	(modes 0,2,5,6,7.0,7.1)(not 1,3,4,7.~)
   /  control, or data register			(modes 0,2,5,6,7.0,7.1,7.2,7.3)(not 1,3,4,7.4)
*/

/* JF: for the 68851 */
/*
   I didn't use much imagination in choosing the 
   following codes, so many of them aren't very
   mnemonic. -rab

   P  pmmu register
	Possible values:
	000	TC	Translation Control reg
	100	CAL	Current Access Level
	101	VAL	Validate Access Level
	110	SCC	Stack Change Control
	111	AC	Access Control

   W  wide pmmu registers
	Possible values:
	001	DRP	Dma Root Pointer
	010	SRP	Supervisor Root Pointer
	011	CRP	Cpu Root Pointer

   f	function code register
	0	SFC
	1	DFC

   V	VAL register only

   X	BADx, BACx
	100	BAD	Breakpoint Acknowledge Data
	101	BAC	Breakpoint Acknowledge Control

   Y	PSR
   Z	PCSR

   |	memory 		(modes 2-6, 7.*)

*/

/* Places to put an operand, for non-general operands:
   s  source, low bits of first word.
   d  dest, shifted 9 in first word
   1  second word, shifted 12
   2  second word, shifted 6
   3  second word, shifted 0
   4  third word, shifted 12
   5  third word, shifted 6
   6  third word, shifted 0
   7  second word, shifted 7
   8  second word, shifted 10
   D  store in both place 1 and place 3; for divul and divsl.
   B  first word, low byte, for branch displacements
   W  second word (entire), for branch displacements
   L  second and third words (entire), for branch displacements (also overloaded for move16)
   b  second word, low byte
   w  second word (entire) [variable word/long branch offset for dbra]
   l  second and third word (entire)
   g  variable branch offset for bra and similar instructions.
      The place to store depends on the magnitude of offset.
   t  store in both place 7 and place 8; for floating point operations
   c  branch offset for cpBcc operations.
      The place to store is word two if bit six of word one is zero,
      and words two and three if bit six of word one is one.
   i  Increment by two, to skip over coprocessor extended operands.   Only
      works with the 'I' format.
   k  Dynamic K-factor field.   Bits 6-4 of word 2, used as a register number.
      Also used for dynamic fmovem instruction.
   C  floating point coprocessor constant - 7 bits.  Also used for static
      K-factors...
   j  Movec register #, stored in 12 low bits of second word.

 Places to put operand, for general operands:
   d  destination, shifted 6 bits in first word
   b  source, at low bit of first word, and immediate uses one byte
   w  source, at low bit of first word, and immediate uses two bytes
   l  source, at low bit of first word, and immediate uses four bytes
   s  source, at low bit of first word.
      Used sometimes in contexts where immediate is not allowed anyway.
   f  single precision float, low bit of 1st word, immediate uses 4 bytes
   F  double precision float, low bit of 1st word, immediate uses 8 bytes
   x  extended precision float, low bit of 1st word, immediate uses 12 bytes
   p  packed float, low bit of 1st word, immediate uses 12 bytes
*/

#define one(x) ((unsigned int) (x) << 16)
#define two(x, y) (((unsigned int) (x) << 16) + y)

/*
	*** DANGER WILL ROBINSON ***

   The assembler requires that all instances of the same mnemonic must be
   consecutive.  If they aren't, the assembler will bomb at runtime
 */
#ifdef DONT_DEFINE_TABLE
extern struct m68k_opcode m68k_opcodes[];
#else
struct m68k_opcode m68k_opcodes[] =
{
{"abcd",	one(0140400),		one(0170770), "DsDd", m68000up },
{"abcd",	one(0140410),		one(0170770), "-s-d", m68000up },

		/* Add instructions */
{OP2(adda,l),	one(0150700),		one(0170700), "*lAd", m68000up },
{OP2(adda,w),	one(0150300),		one(0170700), "*wAd", m68000up },
{OP2(addi,b),	one(0003000),		one(0177700), "#b$b", m68000up },

{OP2(addi,l),	one(0003200),		one(0177700), "#l$l", m68000up },
{OP2(addi,w),	one(0003100),		one(0177700), "#w$w", m68000up },
{OP2(addq,b),	one(0050000),		one(0170700), "Qd$b", m68000up },
#ifdef MOTOROLA_SYNTAX
{OP2(addq,b),	one(0003000),		one(0177700), "#b$b", m68000up }, /* FIXNIEL: addi instead of addq because to many frags */
#endif
{OP2(addq,l),	one(0050200),		one(0170700), "Qd%l", m68000up },
#ifdef MOTOROLA_SYNTAX
{OP2(addq,l),	one(0003200),		one(0177700), "#l$l", m68000up }, /* FIXNIEL: addi instead of addq because to many frags */
#endif
{OP2(addq,w),	one(0050100),		one(0170700), "Qd%w", m68000up },
#ifdef MOTOROLA_SYNTAX
{OP2(addq,w),	one(0003100),		one(0177700), "#w$w", m68000up }, /* FIXNIEL: addi instead of addq because to many frags */
#endif

{OP2(add,b),	one(0050000),		one(0170700), "Qd$b", m68000up },	/* addq written as add */
{OP2(add,b),	one(0003000),		one(0177700), "#b$b", m68000up },	/* addi written as add */
{OP2(add,b),	one(0150000),		one(0170700), ";bDd", m68000up },	/* addb <ea>,	Dd */
{OP2(add,b),	one(0150400),		one(0170700), "Dd~b", m68000up },	/* add.b Dd,	<ea> */

{OP2(add,w),	one(0150300),		one(0170700), "*wAd", m68000up },	/* adda written as add */
{OP2(add,w),	one(0050100),		one(0170700), "Qd%w", m68000up },	/* addq written as add */
{OP2(add,w),	one(0003100),		one(0177700), "#w$w", m68000up },	/* addi written as add */
{OP2(add,w),	one(0150100),		one(0170700), "*wDd", m68000up },	/* addw <ea>,	Dd */
{OP2(add,w),	one(0150500),		one(0170700), "Dd~w", m68000up },	/* addw Dd,	<ea> */

{OP2(add,l),	one(0050200),		one(0170700), "Qd%l", m68000up },	/* addq written as add */
{OP2(add,l),	one(0003200),		one(0177700), "#l$l", m68000up },	/* addi written as add */
{OP2(add,l),	one(0150700),		one(0170700), "*lAd", m68000up },	/* adda written as add */
{OP2(add,l),	one(0150200),		one(0170700), "*lDd", m68000up },	/* addl <ea>,	Dd */
{OP2(add,l),	one(0150600),		one(0170700), "Dd~l", m68000up },	/* addl Dd,	<ea> */

#ifdef MOTOROLA_SYNTAX
{"addi",	one(0050200),		one(0170700), "Qd%l", m68000up },
{"addi",	one(0003200),		one(0177700), "#l$l", m68000up },	/* addi written as add */
{"addq",	one(0050100),		one(0170700), "Qd%w", m68000up },
#endif

{OP2(addx,b),	one(0150400),		one(0170770), "DsDd", m68000up },
{OP2(addx,b),	one(0150410),		one(0170770), "-s-d", m68000up },
{OP2(addx,l),	one(0150600),		one(0170770), "DsDd", m68000up },
{OP2(addx,l),	one(0150610),		one(0170770), "-s-d", m68000up },
{OP2(addx,w),	one(0150500),		one(0170770), "DsDd", m68000up },
{OP2(addx,w),	one(0150510),		one(0170770), "-s-d", m68000up },

{OP2(andi,b),	one(0001000),		one(0177700), "#b$b", m68000up },
{OP2(andi,b),	one(0001074),		one(0177777), "#bCb", m68000up },	/* andi to ccr */
{OP2(andi,w),	one(0001100),		one(0177700), "#w$w", m68000up },
{OP2(andi,w),	one(0001174),		one(0177777), "#wSw", m68000up },	/* andi to sr */
{OP2(andi,l),	one(0001200),		one(0177700), "#l$l", m68000up },
{OP2(and,b),	one(0001000),		one(0177700), "#b$b", m68000up },	/* andi written as or */
{OP2(and,b),	one(0001074),		one(0177777), "#bCb", m68000up },	/* andi to ccr */
{OP2(and,b),	one(0140000),		one(0170700), ";bDd", m68000up },	/* memory to register */
{OP2(and,b),	one(0140400),		one(0170700), "Dd~b", m68000up },	/* register to memory */
{OP2(and,w),	one(0001100),		one(0177700), "#w$w", m68000up },	/* andi written as or */
{OP2(and,w),	one(0001174),		one(0177777), "#wSw", m68000up },	/* andi to sr */
{OP2(and,w),	one(0140100),		one(0170700), ";wDd", m68000up },	/* memory to register */
{OP2(and,w),	one(0140500),		one(0170700), "Dd~w", m68000up },	/* register to memory */
{OP2(and,l),	one(0001200),		one(0177700), "#l$l", m68000up },	/* andi written as or */
{OP2(and,l),	one(0140200),		one(0170700), ";lDd", m68000up },	/* memory to register */
{OP2(and,l),	one(0140600),		one(0170700), "Dd~l", m68000up },	/* register to memory */

#ifdef MOTOROLA_SYNTAX
{"andi",	one(0001200),		one(0177700), "#l$l", m68000up },
{"andi",	one(0001174),		one(0177777), "#wSw", m68000up },	/* andi to sr */
{"andi",	one(0001074),		one(0177777), "#bCb", m68000up },	/* andi to ccr */
#endif

{OP2(asl,b),	one(0160400),		one(0170770), "QdDs", m68000up },
{OP2(asl,b),	one(0160440),		one(0170770), "DdDs", m68000up },
{OP2(asl,l),	one(0160600),		one(0170770), "QdDs", m68000up },
{OP2(asl,l),	one(0160640),		one(0170770), "DdDs", m68000up },
{OP2(asl,w),	one(0160500),		one(0170770), "QdDs", m68000up },
{OP2(asl,w),	one(0160540),		one(0170770), "DdDs", m68000up },
{OP2(asl,w),	one(0160700),		one(0177700), "~s",   m68000up },	/* Shift memory */
{OP2(asr,b),	one(0160000),		one(0170770), "QdDs", m68000up },
{OP2(asr,b),	one(0160040),		one(0170770), "DdDs", m68000up },
{OP2(asr,l),	one(0160200),		one(0170770), "QdDs", m68000up },
{OP2(asr,l),	one(0160240),		one(0170770), "DdDs", m68000up },
{OP2(asr,w),	one(0160100),		one(0170770), "QdDs", m68000up },
{OP2(asr,w),	one(0160140),		one(0170770), "DdDs", m68000up },
{OP2(asr,w),	one(0160300),		one(0177700), "~s",   m68000up },	/* Shift memory */

#ifdef BRANCH_IS_VARIABLE_SIZED
#define BRANCH "Bg"
#define BRANCH_MASK one(0xFF00)
#else
/* Fixed-size branches with 16-bit offsets */
#define BRANCH "BW"
#define BRANCH_MASK one(0xFFFF) /* 8-bit displacement must be 0 */
#endif

{"bhi",		one(0061000),		BRANCH_MASK, BRANCH, m68000up },
{"bls",		one(0061400),		BRANCH_MASK, BRANCH, m68000up },
{"bcc",		one(0062000),		BRANCH_MASK, BRANCH, m68000up },
#ifndef MOTOROLA_SYNTAX
{"jfnlt",	one(0062000),		one(0177777), "BW", m68000up }, /* apparently a sun alias */
#endif
{"bcs",		one(0062400),		BRANCH_MASK, BRANCH, m68000up },
{"bne",		one(0063000),		BRANCH_MASK, BRANCH, m68000up },
{"beq",		one(0063400),		BRANCH_MASK, BRANCH, m68000up },
{"bvc",		one(0064000),		BRANCH_MASK, BRANCH, m68000up },
{"bvs",		one(0064400),		BRANCH_MASK, BRANCH, m68000up },
{"bpl",		one(0065000),		BRANCH_MASK, BRANCH, m68000up },
{"bmi",		one(0065400),		BRANCH_MASK, BRANCH, m68000up },
{"bge",		one(0066000),		BRANCH_MASK, BRANCH, m68000up },
{"blt",		one(0066400),		BRANCH_MASK, BRANCH, m68000up },
{"bgt",		one(0067000),		BRANCH_MASK, BRANCH, m68000up },
{"ble",		one(0067400),		BRANCH_MASK, BRANCH, m68000up },
#ifndef MOTOROLA_SYNTAX
{"jfngt",	one(0067400),		one(0177777), "BW", m68000up }, /* apparently a sun alias */
#endif

#ifdef MOTOROLA_SYNTAX
{"bsr",		one(0060400),		one(0177400), "Bg", m68000up },
{"bsr",		one(0047200),		one(0177700),  "!s", m68000up },
{"bra",		one(0060000),		one(0177400),  "Bg", m68000up },
{"bra",		one(0047300),		one(0177700),  "!s", m68000up },
#else
{"bra",		one(0060000),		one(0177777), "BW", m68000up },
{"bsr",		one(0060400),		one(0177777), "BW", m68000up },
#endif

/* Fixed-size branches with short (byte) offsets */

{OP2(bhi,s),	one(0061000),		one(0177400), "BB", m68000up },
{OP2(bls,s),	one(0061400),		one(0177400), "BB", m68000up },
{OP2(bcc,s),	one(0062000),		one(0177400), "BB", m68000up },
{OP2(bcs,s),	one(0062400),		one(0177400), "BB", m68000up },
{OP2(bne,s),	one(0063000),		one(0177400), "BB", m68000up },
{OP2(beq,s),	one(0063400),		one(0177400), "BB", m68000up },
{"jfeq",	one(0063400),		one(0177400), "BB", m68000up }, /* apparently a sun alias */
{OP2(bvc,s),	one(0064000),		one(0177400), "BB", m68000up },
{OP2(bvs,s),	one(0064400),		one(0177400), "BB", m68000up },
{OP2(bpl,s),	one(0065000),		one(0177400), "BB", m68000up },
{OP2(bmi,s),	one(0065400),		one(0177400), "BB", m68000up },
{OP2(bge,s),	one(0066000),		one(0177400), "BB", m68000up },
{OP2(blt,s),	one(0066400),		one(0177400), "BB", m68000up },
{OP2(bgt,s),	one(0067000),		one(0177400), "BB", m68000up },
{OP2(ble,s),	one(0067400),		one(0177400), "BB", m68000up },
{OP2(bra,s),	one(0060000),		one(0177400), "BB", m68000up },
{OP2(bsr,s),	one(0060400),		one(0177400), "BB", m68000up },

/* Fixed-size branches with long (32-bit) offsets */

{OP2(bhi,l),	one(0061377),		one(0177777), "BL", m68020up },
{OP2(bls,l),	one(0061777),		one(0177777), "BL", m68020up },
{OP2(bcc,l),	one(0062377),		one(0177777), "BL", m68020up },
{OP2(bcs,l),	one(0062777),		one(0177777), "BL", m68020up },
{OP2(bne,l),	one(0063377),		one(0177777), "BL", m68020up },
{OP2(beq,l),	one(0063777),		one(0177777), "BL", m68020up },
{OP2(bvc,l),	one(0064377),		one(0177777), "BL", m68020up },
{OP2(bvs,l),	one(0064777),		one(0177777), "BL", m68020up },
{OP2(bpl,l),	one(0065377),		one(0177777), "BL", m68020up },
{OP2(bmi,l),	one(0065777),		one(0177777), "BL", m68020up },
{OP2(bge,l),	one(0066377),		one(0177777), "BL", m68020up },
{OP2(blt,l),	one(0066777),		one(0177777), "BL", m68020up },
{OP2(bgt,l),	one(0067377),		one(0177777), "BL", m68020up },
{OP2(ble,l),	one(0067777),		one(0177777), "BL", m68020up },
{OP2(bra,l),	one(0060377),		one(0177777), "BL", m68020up },
{OP2(bsr,l),	one(0060777),		one(0177777), "BL", m68020up },

/* We now return you to our regularly scheduled instruction set */

{"bchg",	one(0000500),		one(0170700),		"Dd$s", m68000up },
{"bchg",	one(0004100),		one(0177700),		"#b$s", m68000up },
{"bclr",	one(0000600),		one(0170700),		"Dd$s", m68000up },
{"bclr",	one(0004200),		one(0177700),		"#b$s", m68000up },

#ifdef MOTOROLA_SYNTAX
{OP2(bchg,b),	one(0000500),		one(0170700),		"Dd$s", m68000up },
{OP2(bchg,b),	one(0004100),		one(0177700),		"#b$s", m68000up },
{OP2(bclr,b),	one(0000600),		one(0170700),		"Dd$s", m68000up },
{OP2(bclr,b),	one(0004200),		one(0177700),		"#b$s", m68000up },

{OP2(bchg,l),	one(0000500),		one(0170700),		"Dd$s", m68000up },
{OP2(bchg,l),	one(0004100),		one(0177700),		"#b$s", m68000up },
{OP2(bclr,l),	one(0000600),		one(0170700),		"Dd$s", m68000up },
{OP2(bclr,l),	one(0004200),		one(0177700),		"#b$s", m68000up },
#endif

{"bfchg",	two(0165300, 0),	two(0177700, 0170000),	"?sO2O3",   m68020up },
{"bfclr",	two(0166300, 0),	two(0177700, 0170000),	"?sO2O3",   m68020up },
{"bfexts",	two(0165700, 0),	two(0177700, 0100000),	"/sO2O3D1", m68020up },
{"bfextu",	two(0164700, 0),	two(0177700, 0100000),	"/sO2O3D1", m68020up },
{"bfffo",	two(0166700, 0),	two(0177700, 0100000),	"/sO2O3D1", m68020up },
{"bfins",	two(0167700, 0),	two(0177700, 0100000),	"D1?sO2O3", m68020up },
{"bfset",	two(0167300, 0),	two(0177700, 0170000),	"?sO2O3",   m68020up },
{"bftst",	two(0164300, 0),	two(0177700, 0170000),	"/sO2O3",   m68020up },
{"bkpt",	one(0044110),		one(0177770),		"Qs",       m68020up },

{"bset",	one(0000700),		one(0170700),		"Dd$s", m68000up },
{"bset",	one(0004300),		one(0177700),		"#b$s", m68000up },
{"btst",	one(0000400),		one(0170700),		"Dd@s", m68000up },
{"btst",	one(0004000),		one(0177700),		"#b@s", m68000up },
#ifdef MOTOROLA_SYNTAX
{OP2(bset,b),	one(0000700),		one(0170700),		"Dd$s", m68000up },
{OP2(bset,b),	one(0004300),		one(0177700),		"#b$s", m68000up },
{OP2(btst,b),	one(0000400),		one(0170700),		"Dd@s", m68000up },
{OP2(btst,b),	one(0004000),		one(0177700),		"#b@s", m68000up },

{OP2(bset,l),	one(0000700),		one(0170700),		"Dd$s", m68000up },
{OP2(bset,l),	one(0004300),		one(0177700),		"#b$s", m68000up },
{OP2(btst,l),	one(0000400),		one(0170700),		"Dd@s", m68000up },
{OP2(btst,l),	one(0004000),		one(0177700),		"#b@s", m68000up },
#endif

{"callm",	one(0003300),		one(0177700),		"#b!s", m68020 },

{OP2(cas2,l),	two(0007374, 0),	two(0177777, 0107070),	"D3D6D2D5R1R4", m68020up }, /* JF FOO really a 3 word ins */
{OP2(cas2,w),	two(0006374, 0),	two(0177777, 0107070),	"D3D6D2D5R1R4", m68020up }, /* JF ditto */
{OP2(cas,b),	two(0005300, 0),	two(0177700, 0177070),	"D3D2~s", m68020up },
{OP2(cas,l),	two(0007300, 0),	two(0177700, 0177070),	"D3D2~s", m68020up },
{OP2(cas,w),	two(0006300, 0),	two(0177700, 0177070),	"D3D2~s", m68020up },

#ifdef ALLOW_DEFAULT_SIZES
{"casw",	two(0006300, 0),	two(0177700, 0177070),	"D3D2~s", m68020up },
{"cas",		two(0006300, 0),	two(0177700, 0177070),	"D3D2~s", m68020up },
{"cas2",	two(0006374, 0),	two(0177777, 0107070),	"D3D6D2D5R1R4", m68020up }, /* JF ditto */
#endif

{OP2(chk2,b),	two(0000300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },
{OP2(chk2,l),	two(0002300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },
{OP2(chk2,w),	two(0001300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },
#ifdef MOTOROLA_SYNTAX
{OP2(chk2,w),	two(0001300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },
#endif

#ifdef ALLOW_DEFAULT_SIZES
{"chk",  	one(0040600),		one(0170700),		";wDd", m68000up },
{"chk",		one(0040400),		one(0170700),		";lDd", m68000up },

/*  {"chk",	one(0040600),		one(0170700),		";wDd"}, JF FOO this looks wrong */

{"chk2",	two(0001300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },
#endif

{OP2(chk,l),	one(0040400),		one(0170700),		";lDd", m68000up },
#ifdef MOTOROLA_SYNTAX
{OP2(chk,w),	two(0001300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },
#endif
 {OP2(chk,w),	one(0040600),		one(0170700),		";wDd", m68000up },

#define SCOPE_LINE (0x1 << 3)
#define SCOPE_PAGE (0x2 << 3)
#define SCOPE_ALL  (0x3 << 3)

{"cinva",	one(0xf400|SCOPE_ALL),  one(0xff38), "ce",   m68040 },
{"cinvl",	one(0xf400|SCOPE_LINE), one(0xff38), "ceas", m68040 },
{"cinvp",	one(0xf400|SCOPE_PAGE), one(0xff38), "ceas", m68040 },

{"cpusha",	one(0xf420|SCOPE_ALL),  one(0xff38), "ce",   m68040 },
{"cpushl",	one(0xf420|SCOPE_LINE), one(0xff38), "ceas", m68040 },
{"cpushp",	one(0xf420|SCOPE_PAGE), one(0xff38), "ceas", m68040 },

#undef SCOPE_LINE
#undef SCOPE_PAGE
#undef SCOPE_ALL
{OP2(clr,b),	one(0041000),		one(0177700),		"$s", m68000up },
{OP2(clr,l),	one(0041200),		one(0177700),		"$s", m68000up },
{OP2(clr,w),	one(0041100),		one(0177700),		"$s", m68000up },


{OP2(cmp2,b),	two(0000300, 0),	two(0177700, 07777),	"!sR1", m68020up },
{OP2(cmp2,l),	two(0002300, 0),	two(0177700, 07777),	"!sR1", m68020up },
{OP2(cmp2,w),	two(0001300, 0),	two(0177700, 07777),	"!sR1", m68020up },
{OP2(cmpa,l),	one(0130700),		one(0170700),		"*lAd", m68000up },
{OP2(cmpa,w),	one(0130300),		one(0170700),		"*wAd", m68000up },

{OP2(cmpi,b),	one(0006000),		one(0177700),		"#b;b", m68000up },
{OP2(cmpi,l),	one(0006200),		one(0177700),		"#l;l", m68000up },
{OP2(cmpi,w),	one(0006100),		one(0177700),		"#w;w", m68000up },
{OP2(cmp,b),	one(0006000),		one(0177700),		"#b;b", m68000up },	/* cmpi written as cmp */
{OP2(cmp,b),	one(0130000),		one(0170700),		";bDd", m68000up },
#ifdef MOTOROLA_SYNTAX
{OP2(cmp,b),	one(0130410),		one(0170770),		"+s+d", m68000up },     /* cmpm written as cmp */
#endif
{OP2(cmp,w),	one(0006100),		one(0177700),		"#w;w", m68000up },
{OP2(cmp,w),	one(0130100),		one(0170700),		"*wDd", m68000up },
{OP2(cmp,w),	one(0130300),		one(0170700),		"*wAd", m68000up },	/* cmpa written as cmp */
#ifdef MOTOROLA_SYNTAX
{OP2(cmp,w),	one(0130510),		one(0170770),		"+s+d", m68000up },	/* cmpm written as cmp */
#endif
{OP2(cmp,l),	one(0006200),		one(0177700),		"#l;l", m68000up },
{OP2(cmp,l),	one(0130200),		one(0170700),		"*lDd", m68000up },
{OP2(cmp,l),	one(0130700),		one(0170700),		"*lAd", m68000up },
#ifdef MOTOROLA_SYNTAX
{"cmpl",	one(0130610),		one(0170770),		"+s+d", m68000up },	/* cmpm written as cmp */
#endif

{OP2(cmpm,b),	one(0130410),		one(0170770),		"+s+d", m68000up },
{OP2(cmpm,l),	one(0130610),		one(0170770),		"+s+d", m68000up },
{OP2(cmpm,w),	one(0130510),		one(0170770),		"+s+d", m68000up },
#ifdef ALLOW_DEFAULT_SIZES
{"cmp",		one(0006200),		one(0177700),		"#l;l", m68000up },
{"cmp",		one(0130200),		one(0170700),		"*lDd", m68000up },
{"cmp",		one(0130700),		one(0170700),		"*lAd", m68000up },
{"cmp",		one(0130510),		one(0170770),		"+s+d", m68000up },	/* cmpm written as cmp */

{"cmpi",	one(0006200),		one(0177700),		"#l;l", m68000up },

{"cmp2",	two(0001300, 0),	two(0177700, 07777),	"!sR1", m68020up },
{"cmpm",	one(0130510),		one(0170770),		"+s+d", m68000up },
#endif

{"dbcc",	one(0052310),		one(0177770),		"DsBw", m68000up },
{"dbcs",	one(0052710),		one(0177770),		"DsBw", m68000up },
{"dbeq",	one(0053710),		one(0177770),		"DsBw", m68000up },
{"dbf",		one(0050710),		one(0177770),		"DsBw", m68000up },
{"dbge",	one(0056310),		one(0177770),		"DsBw", m68000up },
{"dbgt",	one(0057310),		one(0177770),		"DsBw", m68000up },
{"dbhi",	one(0051310),		one(0177770),		"DsBw", m68000up },
{"dble",	one(0057710),		one(0177770),		"DsBw", m68000up },
{"dbls",	one(0051710),		one(0177770),		"DsBw", m68000up },
{"dblt",	one(0056710),		one(0177770),		"DsBw", m68000up },
{"dbmi",	one(0055710),		one(0177770),		"DsBw", m68000up },
{"dbne",	one(0053310),		one(0177770),		"DsBw", m68000up },
{"dbpl",	one(0055310),		one(0177770),		"DsBw", m68000up },
{"dbra",	one(0050710),		one(0177770),		"DsBw", m68000up },
{"dbt",		one(0050310),		one(0177770),		"DsBw", m68000up },
{"dbvc",	one(0054310),		one(0177770),		"DsBw", m68000up },
{"dbvs",	one(0054710),		one(0177770),		"DsBw", m68000up },

#ifdef MOTOROLA_SYNTAX
{OP2(tdivs,l),	two(0046100, 0006000),	two(0177700, 0107770),	";lD3D1", m68020up },
{OP2(tdivs,l),	two(0046100, 0004000),	two(0177700, 0107770),	";lDD", m68020up },
#endif

{OP2(divs,l),	two(0046100, 0006000),	two(0177700, 0107770),	";lD3D1", m68020up },
{OP2(divs,l),	two(0046100, 0004000),	two(0177700, 0107770),	";lDD", m68020up },
{OP2(divs,ll),	two(0046100, 0004000),	two(0177700, 0107770),	";lD3D1", m68020up },
{OP2(divs,w),	one(0100700),		one(0170700),		";wDd", m68000up },
{"divs",	one(0100700),		one(0170700),		";wDd", m68000up },
{OP2(divu,l),	two(0046100, 0002000),	two(0177700, 0107770),	";lD3D1", m68020up },
{OP2(divu,l),	two(0046100, 0000000),	two(0177700, 0107770),	";lDD", m68020up },
{OP2(divu,ll),	two(0046100, 0000000),	two(0177700, 0107770),	";lD3D1", m68020up },
{OP2(divu,w),	one(0100300),		one(0170700),		";wDd", m68000up },
{"divu",	one(0100300),		one(0170700),		";wDd", m68000up },

{OP2(eor,b),	one(0005000),		one(0177700),		"#b$s", m68000up },	/* eori written as or */
{OP2(eor,b),	one(0005074),		one(0177777),		"#bCs", m68000up },	/* eori to ccr */
{OP2(eor,b),	one(0130400),		one(0170700),		"Dd$s", m68000up },	/* register to memory */
{OP2(eori,b),	one(0005000),		one(0177700),		"#b$s", m68000up },
{OP2(eori,b),	one(0005074),		one(0177777),		"#bCs", m68000up },	/* eori to ccr */
{OP2(eori,l),	one(0005200),		one(0177700),		"#l$s", m68000up },
{OP2(eori,w),	one(0005100),		one(0177700),		"#w$s", m68000up },
{OP2(eori,w),	one(0005174),		one(0177777),		"#wSs", m68000up },	/* eori to sr */
{OP2(eor,l),	one(0005200),		one(0177700),		"#l$s", m68000up },
{OP2(eor,l),	one(0130600),		one(0170700),		"Dd$s", m68000up },
{OP2(eor,w),	one(0005100),		one(0177700),		"#w$s", m68000up },
{OP2(eor,w),	one(0005174),		one(0177777),		"#wSs", m68000up },	/* eori to sr */
{OP2(eor,w),	one(0130500),		one(0170700),		"Dd$s", m68000up },
#ifdef ALLOW_DEFAULT_SIZES
{"eor",		one(0005074),		one(0177777),		"#bCs", m68000up },	/* eorb to ccr */
{"eor",		one(0005174),		one(0177777),		"#wSs", m68000up },	/* eori to sr */
{"eori",	one(0005074),		one(0177777),		"#bCs", m68000up },	/* eori to ccr */
{"eori",	one(0005174),		one(0177777),		"#wSs", m68000up },	/* eoriw to sr */
#endif

{"exg",		one(0140500),		one(0170770),		"DdDs", m68000up },
{"exg",		one(0140510),		one(0170770),		"AdAs", m68000up },
{"exg",		one(0140610),		one(0170770),		"DdAs", m68000up },
{"exg",		one(0140610),		one(0170770),		"AsDd", m68000up },

{OP2(ext,w),	one(0044200),		one(0177770),		"Ds", m68000up },
{OP2(ext,l),	one(0044300),		one(0177770),		"Ds", m68000up },
{OP2(extb,l),	one(0044700),		one(0177770),		"Ds", m68020up },
#ifdef MOTOROLA_SYNTAX
{"ext",		one(0044200),		one(0177770),		"Ds", m68000up },
{"ext.wl",	one(0044200),		one(0177770),		"Ds", m68000up },
{"ext.bw",	one(0044200),		one(0177770),		"Ds", m68000up },
#endif

/* float stuff starts here */

{OP2(fabs,b),	two(0xF000, 0x5818),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fabs,d),	two(0xF000, 0x5418),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fabs,l),	two(0xF000, 0x4018),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fabs,p),	two(0xF000, 0x4C18),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fabs,s),	two(0xF000, 0x4418),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fabs,w),	two(0xF000, 0x5018),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fabs,x),	two(0xF000, 0x0018),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fabs,x),	two(0xF000, 0x4818),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fabs,x),	two(0xF000, 0x0018),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },
{OP2(fsabs,b),	two(0xF000, 0x5858),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fsabs,d),	two(0xF000, 0x5458),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fsabs,l),	two(0xF000, 0x4058),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fsabs,p),	two(0xF000, 0x4C58),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fsabs,s),	two(0xF000, 0x4458),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fsabs,w),	two(0xF000, 0x5058),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fsabs,x),	two(0xF000, 0x0058),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fsabs,x),	two(0xF000, 0x4858),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{OP2(fsabs,x),	two(0xF000, 0x0058),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{OP2(fdabs,b),	two(0xF000, 0x585c),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040},
{OP2(fdabs,d),	two(0xF000, 0x545c),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040},
{OP2(fdabs,l),	two(0xF000, 0x405c),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040},
{OP2(fdabs,p),	two(0xF000, 0x4C5c),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040},
{OP2(fdabs,s),	two(0xF000, 0x445c),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040},
{OP2(fdabs,w),	two(0xF000, 0x505c),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040},
{OP2(fdabs,x),	two(0xF000, 0x005c),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040},
{OP2(fdabs,x),	two(0xF000, 0x485c),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040},
{OP2(fdabs,x),	two(0xF000, 0x005c),	two(0xF1C0, 0xE07F),	"IiFt",   m68040},
{OP2(facos,b),	two(0xF000, 0x581C),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(facos,d),	two(0xF000, 0x541C),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(facos,l),	two(0xF000, 0x401C),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(facos,p),	two(0xF000, 0x4C1C),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(facos,s),	two(0xF000, 0x441C),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(facos,w),	two(0xF000, 0x501C),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(facos,x),	two(0xF000, 0x001C),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(facos,x),	two(0xF000, 0x481C),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(facos,x),	two(0xF000, 0x001C),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fadd,b),	two(0xF000, 0x5822),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fadd,d),	two(0xF000, 0x5422),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fadd,l),	two(0xF000, 0x4022),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fadd,p),	two(0xF000, 0x4C22),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fadd,s),	two(0xF000, 0x4422),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fadd,w),	two(0xF000, 0x5022),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fadd,x),	two(0xF000, 0x0022),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fadd,x),	two(0xF000, 0x4822),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {OP2(fadd,x),	two(0xF000, 0x0022),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF removed */

{OP2(fsadd,b),	two(0xF000, 0x5862),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fsadd,d),	two(0xF000, 0x5462),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fsadd,l),	two(0xF000, 0x4062),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fsadd,p),	two(0xF000, 0x4C62),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fsadd,s),	two(0xF000, 0x4462),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fsadd,w),	two(0xF000, 0x5062),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fsadd,x),	two(0xF000, 0x0062),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fsadd,x),	two(0xF000, 0x4862),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {OP2(fsadd,x),	two(0xF000, 0x0062),	two(0xF1C0, 0xE07F),	"IiFt", m68040 }, JF removed */

{OP2(fdadd,b),	two(0xF000, 0x5866),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fdadd,d),	two(0xF000, 0x5466),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fdadd,l),	two(0xF000, 0x4066),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fdadd,p),	two(0xF000, 0x4C66),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fdadd,s),	two(0xF000, 0x4466),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fdadd,w),	two(0xF000, 0x5066),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fdadd,x),	two(0xF000, 0x0066),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fdadd,x),	two(0xF000, 0x4866),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {"faddx",	two(0xF000, 0x0066),	two(0xF1C0, 0xE07F),	"IiFt", m68040 }, JF removed */

{OP2(fasin,b),	two(0xF000, 0x580C),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fasin,d),	two(0xF000, 0x540C),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fasin,l),	two(0xF000, 0x400C),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fasin,p),	two(0xF000, 0x4C0C),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fasin,s),	two(0xF000, 0x440C),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fasin,w),	two(0xF000, 0x500C),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fasin,x),	two(0xF000, 0x000C),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fasin,x),	two(0xF000, 0x480C),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fasin,x),	two(0xF000, 0x000C),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fatan,b),	two(0xF000, 0x580A),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fatan,d),	two(0xF000, 0x540A),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fatan,l),	two(0xF000, 0x400A),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fatan,p),	two(0xF000, 0x4C0A),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fatan,s),	two(0xF000, 0x440A),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fatan,w),	two(0xF000, 0x500A),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fatan,x),	two(0xF000, 0x000A),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fatan,x),	two(0xF000, 0x480A),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fatan,x),	two(0xF000, 0x000A),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fatanh,b),	two(0xF000, 0x580D),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fatanh,d),	two(0xF000, 0x540D),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fatanh,l),	two(0xF000, 0x400D),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fatanh,p),	two(0xF000, 0x4C0D),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fatanh,s),	two(0xF000, 0x440D),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fatanh,w),	two(0xF000, 0x500D),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fatanh,x),	two(0xF000, 0x000D),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fatanh,x),	two(0xF000, 0x480D),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fatanh,x),	two(0xF000, 0x000D),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

/* Fixed-size Float branches */

{"fbeq",	one(0xF081),		one(0xF1BF),		"IdBW", mfloat },
{"fbf",		one(0xF080),		one(0xF1BF),		"IdBW", mfloat },
{"fbge",	one(0xF093),		one(0xF1BF),		"IdBW", mfloat },
{"fbgl",	one(0xF096),		one(0xF1BF),		"IdBW", mfloat },
{"fbgle",	one(0xF097),		one(0xF1BF),		"IdBW", mfloat },
{"fbgt",	one(0xF092),		one(0xF1BF),		"IdBW", mfloat },
{"fble",	one(0xF095),		one(0xF1BF),		"IdBW", mfloat },
{"fblt",	one(0xF094),		one(0xF1BF),		"IdBW", mfloat },
{"fbne",	one(0xF08E),		one(0xF1BF),		"IdBW", mfloat },
#ifndef MOTOROLA_SYNTAX
{"fbneq",	one(0xF08E),		one(0xF1BF),		"IdBW", mfloat },
#endif
{"fbnge",	one(0xF09C),		one(0xF1BF),		"IdBW", mfloat },
{"fbngl",	one(0xF099),		one(0xF1BF),		"IdBW", mfloat },
{"fbngle",	one(0xF098),		one(0xF1BF),		"IdBW", mfloat },
{"fbngt",	one(0xF09D),		one(0xF1BF),		"IdBW", mfloat },
{"fbnle",	one(0xF09A),		one(0xF1BF),		"IdBW", mfloat },
{"fbnlt",	one(0xF09B),		one(0xF1BF),		"IdBW", mfloat },
{"fboge",	one(0xF083),		one(0xF1BF),		"IdBW", mfloat },
{"fbogl",	one(0xF086),		one(0xF1BF),		"IdBW", mfloat },
{"fbogt",	one(0xF082),		one(0xF1BF),		"IdBW", mfloat },
{"fbole",	one(0xF085),		one(0xF1BF),		"IdBW", mfloat },
{"fbolt",	one(0xF084),		one(0xF1BF),		"IdBW", mfloat },
{"fbor",	one(0xF087),		one(0xF1BF),		"IdBW", mfloat },
{"fbseq",	one(0xF091),		one(0xF1BF),		"IdBW", mfloat },
{"fbsf",	one(0xF090),		one(0xF1BF),		"IdBW", mfloat },
{"fbsne",	one(0xF09E),		one(0xF1BF),		"IdBW", mfloat },
#ifndef MOTOROLA_SYNTAX
{"fbsneq",	one(0xF09E),		one(0xF1BF),		"IdBW", mfloat },
#endif
{"fbst",	one(0xF09F),		one(0xF1BF),		"IdBW", mfloat },
{"fbt",		one(0xF08F),		one(0xF1BF),		"IdBW", mfloat },
{"fbueq",	one(0xF089),		one(0xF1BF),		"IdBW", mfloat },
{"fbuge",	one(0xF08B),		one(0xF1BF),		"IdBW", mfloat },
{"fbugt",	one(0xF08A),		one(0xF1BF),		"IdBW", mfloat },
{"fbule",	one(0xF08D),		one(0xF1BF),		"IdBW", mfloat },
{"fbult",	one(0xF08C),		one(0xF1BF),		"IdBW", mfloat },
{"fbun",	one(0xF088),		one(0xF1BF),		"IdBW", mfloat },

/* Float branches -- long (32-bit) displacements */

{"fbeql",	one(0xF081),		one(0xF1BF),		"IdBC", mfloat },
{"fbfl",	one(0xF080),		one(0xF1BF),		"IdBC", mfloat },
{"fbgel",	one(0xF093),		one(0xF1BF),		"IdBC", mfloat },
{"fbgll",	one(0xF096),		one(0xF1BF),		"IdBC", mfloat },
{"fbglel",	one(0xF097),		one(0xF1BF),		"IdBC", mfloat },
{"fbgtl",	one(0xF092),		one(0xF1BF),		"IdBC", mfloat },
{"fblel",	one(0xF095),		one(0xF1BF),		"IdBC", mfloat },
{"fbltl",	one(0xF094),		one(0xF1BF),		"IdBC", mfloat },
{"fbnel",	one(0xF08E),		one(0xF1BF),		"IdBC", mfloat },
{"fbngel",	one(0xF09C),		one(0xF1BF),		"IdBC", mfloat },
{"fbngll",	one(0xF099),		one(0xF1BF),		"IdBC", mfloat },
{"fbnglel",	one(0xF098),		one(0xF1BF),		"IdBC", mfloat },
{"fbngtl",	one(0xF09D),		one(0xF1BF),		"IdBC", mfloat },
{"fbnlel",	one(0xF09A),		one(0xF1BF),		"IdBC", mfloat },
{"fbnltl",	one(0xF09B),		one(0xF1BF),		"IdBC", mfloat },
{"fbogel",	one(0xF083),		one(0xF1BF),		"IdBC", mfloat },
{"fbogll",	one(0xF086),		one(0xF1BF),		"IdBC", mfloat },
{"fbogtl",	one(0xF082),		one(0xF1BF),		"IdBC", mfloat },
{"fbolel",	one(0xF085),		one(0xF1BF),		"IdBC", mfloat },
{"fboltl",	one(0xF084),		one(0xF1BF),		"IdBC", mfloat },
{"fborl",	one(0xF087),		one(0xF1BF),		"IdBC", mfloat },
{"fbseql",	one(0xF091),		one(0xF1BF),		"IdBC", mfloat },
{"fbsfl",	one(0xF090),		one(0xF1BF),		"IdBC", mfloat },
{"fbsnel",	one(0xF09E),		one(0xF1BF),		"IdBC", mfloat },
{"fbstl",	one(0xF09F),		one(0xF1BF),		"IdBC", mfloat },
{"fbtl",	one(0xF08F),		one(0xF1BF),		"IdBC", mfloat },
{"fbueql",	one(0xF089),		one(0xF1BF),		"IdBC", mfloat },
{"fbugel",	one(0xF08B),		one(0xF1BF),		"IdBC", mfloat },
{"fbugtl",	one(0xF08A),		one(0xF1BF),		"IdBC", mfloat },
{"fbulel",	one(0xF08D),		one(0xF1BF),		"IdBC", mfloat },
{"fbultl",	one(0xF08C),		one(0xF1BF),		"IdBC", mfloat },
{"fbunl",	one(0xF088),		one(0xF1BF),		"IdBC", mfloat },

{OP2(fcmp,b),	two(0xF000, 0x5838),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fcmp,d),	two(0xF000, 0x5438),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fcmp,l),	two(0xF000, 0x4038),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fcmp,p),	two(0xF000, 0x4C38),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fcmp,s),	two(0xF000, 0x4438),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fcmp,w),	two(0xF000, 0x5038),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fcmp,x),	two(0xF000, 0x0038),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fcmp,x),	two(0xF000, 0x4838),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {"fcmpx",	two(0xF000, 0x0038),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF removed */

{OP2(fcos,b),	two(0xF000, 0x581D),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fcos,d),	two(0xF000, 0x541D),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fcos,l),	two(0xF000, 0x401D),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fcos,p),	two(0xF000, 0x4C1D),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fcos,s),	two(0xF000, 0x441D),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fcos,w),	two(0xF000, 0x501D),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fcos,x),	two(0xF000, 0x001D),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fcos,x),	two(0xF000, 0x481D),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fcos,x),	two(0xF000, 0x001D),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fcosh,b),	two(0xF000, 0x5819),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fcosh,d),	two(0xF000, 0x5419),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fcosh,l),	two(0xF000, 0x4019),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fcosh,p),	two(0xF000, 0x4C19),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fcosh,s),	two(0xF000, 0x4419),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fcosh,w),	two(0xF000, 0x5019),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fcosh,x),	two(0xF000, 0x0019),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fcosh,x),	two(0xF000, 0x4819),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fcosh,x),	two(0xF000, 0x0019),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fdbeq",	two(0xF048, 0x0001),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbf",	two(0xF048, 0x0000),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbge",	two(0xF048, 0x0013),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbgl",	two(0xF048, 0x0016),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbgle",	two(0xF048, 0x0017),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbgt",	two(0xF048, 0x0012),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdble",	two(0xF048, 0x0015),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdblt",	two(0xF048, 0x0014),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbne",	two(0xF048, 0x000E),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
#ifndef MOTOROLA_SYNTAX
{"fdbneq",	two(0xF048, 0x000E),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
#endif
{"fdbnge",	two(0xF048, 0x001C),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbngl",	two(0xF048, 0x0019),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbngle",	two(0xF048, 0x0018),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbngt",	two(0xF048, 0x001D),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbnle",	two(0xF048, 0x001A),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbnlt",	two(0xF048, 0x001B),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdboge",	two(0xF048, 0x0003),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbogl",	two(0xF048, 0x0006),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbogt",	two(0xF048, 0x0002),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbole",	two(0xF048, 0x0005),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbolt",	two(0xF048, 0x0004),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbor",	two(0xF048, 0x0007),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbseq",	two(0xF048, 0x0011),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbsf",	two(0xF048, 0x0010),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbsne",	two(0xF048, 0x001E),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
#ifndef MOTOROLA_SYNTAX
{"fdbsneq",	two(0xF048, 0x001E),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
#endif
{"fdbst",	two(0xF048, 0x001F),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbt",	two(0xF048, 0x000F),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbueq",	two(0xF048, 0x0009),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbuge",	two(0xF048, 0x000B),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbugt",	two(0xF048, 0x000A),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbule",	two(0xF048, 0x000D),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbult",	two(0xF048, 0x000C),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbun",	two(0xF048, 0x0008),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },

{OP2(fdiv,b),	two(0xF000, 0x5820),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fdiv,d),	two(0xF000, 0x5420),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fdiv,l),	two(0xF000, 0x4020),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fdiv,p),	two(0xF000, 0x4C20),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fdiv,s),	two(0xF000, 0x4420),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fdiv,w),	two(0xF000, 0x5020),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fdiv,x),	two(0xF000, 0x0020),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fdiv,x),	two(0xF000, 0x4820),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {OP2(fdiv,x),	two(0xF000, 0x0020),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF */

{OP2(fsdiv,b),	two(0xF000, 0x5860),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fsdiv,d),	two(0xF000, 0x5460),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fsdiv,l),	two(0xF000, 0x4060),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fsdiv,p),	two(0xF000, 0x4C60),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fsdiv,s),	two(0xF000, 0x4460),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fsdiv,w),	two(0xF000, 0x5060),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fsdiv,x),	two(0xF000, 0x0060),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fsdiv,x),	two(0xF000, 0x4860),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {OP2(fsdiv,x),	two(0xF000, 0x0060),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 }, JF */

{OP2(fddiv,b),	two(0xF000, 0x5864),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fddiv,d),	two(0xF000, 0x5464),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fddiv,l),	two(0xF000, 0x4064),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fddiv,p),	two(0xF000, 0x4C64),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fddiv,s),	two(0xF000, 0x4464),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fddiv,w),	two(0xF000, 0x5064),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fddiv,x),	two(0xF000, 0x0064),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fddiv,x),	two(0xF000, 0x4864),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {OP2(fddiv,x),	two(0xF000, 0x0064),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 }, JF */

{OP2(fetox,b),	two(0xF000, 0x5810),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fetox,d),	two(0xF000, 0x5410),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fetox,l),	two(0xF000, 0x4010),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fetox,p),	two(0xF000, 0x4C10),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fetox,s),	two(0xF000, 0x4410),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fetox,w),	two(0xF000, 0x5010),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fetox,x),	two(0xF000, 0x0010),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fetox,x),	two(0xF000, 0x4810),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fetox,x),	two(0xF000, 0x0010),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fetoxm1,b),	two(0xF000, 0x5808),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fetoxm1,d),	two(0xF000, 0x5408),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fetoxm1,l),	two(0xF000, 0x4008),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fetoxm1,p),	two(0xF000, 0x4C08),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fetoxm1,s),	two(0xF000, 0x4408),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fetoxm1,w),	two(0xF000, 0x5008),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fetoxm1,x),	two(0xF000, 0x0008),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fetoxm1,x),	two(0xF000, 0x4808),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fetoxm1,x),	two(0xF000, 0x0008),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fgetexp,b),	two(0xF000, 0x581E),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fgetexp,d),	two(0xF000, 0x541E),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fgetexp,l),	two(0xF000, 0x401E),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fgetexp,p),	two(0xF000, 0x4C1E),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fgetexp,s),	two(0xF000, 0x441E),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fgetexp,w),	two(0xF000, 0x501E),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fgetexp,x),	two(0xF000, 0x001E),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fgetexp,x),	two(0xF000, 0x481E),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fgetexp,x),	two(0xF000, 0x001E),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fgetman,b),	two(0xF000, 0x581F),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fgetman,d),	two(0xF000, 0x541F),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fgetman,l),	two(0xF000, 0x401F),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fgetman,p),	two(0xF000, 0x4C1F),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fgetman,s),	two(0xF000, 0x441F),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fgetman,w),	two(0xF000, 0x501F),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fgetman,x),	two(0xF000, 0x001F),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fgetman,x),	two(0xF000, 0x481F),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fgetman,x),	two(0xF000, 0x001F),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fint,b),	two(0xF000, 0x5801),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fint,d),	two(0xF000, 0x5401),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fint,l),	two(0xF000, 0x4001),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fint,p),	two(0xF000, 0x4C01),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fint,s),	two(0xF000, 0x4401),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fint,w),	two(0xF000, 0x5001),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fint,x),	two(0xF000, 0x0001),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fint,x),	two(0xF000, 0x4801),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fint,x),	two(0xF000, 0x0001),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fintrz,b),	two(0xF000, 0x5803),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fintrz,d),	two(0xF000, 0x5403),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fintrz,l),	two(0xF000, 0x4003),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fintrz,p),	two(0xF000, 0x4C03),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fintrz,s),	two(0xF000, 0x4403),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fintrz,w),	two(0xF000, 0x5003),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fintrz,x),	two(0xF000, 0x0003),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fintrz,x),	two(0xF000, 0x4803),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fintrz,x),	two(0xF000, 0x0003),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(flog10,b),	two(0xF000, 0x5815),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(flog10,d),	two(0xF000, 0x5415),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(flog10,l),	two(0xF000, 0x4015),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(flog10,p),	two(0xF000, 0x4C15),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(flog10,s),	two(0xF000, 0x4415),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(flog10,w),	two(0xF000, 0x5015),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(flog10,x),	two(0xF000, 0x0015),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(flog10,x),	two(0xF000, 0x4815),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(flog10,x),	two(0xF000, 0x0015),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(flog2,b),	two(0xF000, 0x5816),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(flog2,d),	two(0xF000, 0x5416),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(flog2,l),	two(0xF000, 0x4016),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(flog2,p),	two(0xF000, 0x4C16),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(flog2,s),	two(0xF000, 0x4416),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(flog2,w),	two(0xF000, 0x5016),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(flog2,x),	two(0xF000, 0x0016),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(flog2,x),	two(0xF000, 0x4816),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(flog2,x),	two(0xF000, 0x0016),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(flogn,b),	two(0xF000, 0x5814),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(flogn,d),	two(0xF000, 0x5414),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(flogn,l),	two(0xF000, 0x4014),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(flogn,p),	two(0xF000, 0x4C14),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(flogn,s),	two(0xF000, 0x4414),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(flogn,w),	two(0xF000, 0x5014),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(flogn,x),	two(0xF000, 0x0014),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(flogn,x),	two(0xF000, 0x4814),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(flogn,x),	two(0xF000, 0x0014),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(flognp1,b),	two(0xF000, 0x5806),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(flognp1,d),	two(0xF000, 0x5406),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(flognp1,l),	two(0xF000, 0x4006),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(flognp1,p),	two(0xF000, 0x4C06),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(flognp1,s),	two(0xF000, 0x4406),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(flognp1,w),	two(0xF000, 0x5006),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(flognp1,x),	two(0xF000, 0x0006),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(flognp1,x),	two(0xF000, 0x4806),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(flognp1,x),	two(0xF000, 0x0006),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fmod,b),	two(0xF000, 0x5821),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fmod,d),	two(0xF000, 0x5421),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fmod,l),	two(0xF000, 0x4021),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fmod,p),	two(0xF000, 0x4C21),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fmod,s),	two(0xF000, 0x4421),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fmod,w),	two(0xF000, 0x5021),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fmod,x),	two(0xF000, 0x0021),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fmod,x),	two(0xF000, 0x4821),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {OP2(fmod,x),	two(0xF000, 0x0021),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF */

{OP2(fmove,b),	two(0xF000, 0x5800),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },		/* fmove from <ea> to fp<n> */
{OP2(fmove,b),	two(0xF000, 0x7800),	two(0xF1C0, 0xFC7F),	"IiF7@b", mfloat },		/* fmove from fp<n> to <ea> */
{OP2(fmove,d),	two(0xF000, 0x5400),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },		/* fmove from <ea> to fp<n> */
{OP2(fmove,d),	two(0xF000, 0x7400),	two(0xF1C0, 0xFC7F),	"IiF7@F", mfloat },		/* fmove from fp<n> to <ea> */
{OP2(fmove,l),	two(0xF000, 0x4000),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },		/* fmove from <ea> to fp<n> */
{OP2(fmove,l),	two(0xF000, 0x6000),	two(0xF1C0, 0xFC7F),	"IiF7@l", mfloat },		/* fmove from fp<n> to <ea> */
/* Warning:  The addressing modes on these are probably not right:
   esp, Areg direct is only allowed for FPI */
		/* fmove.l from/to system control registers: */
{OP2(fmove,l),	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"Iis8@s", mfloat },
{OP2(fmove,l),	two(0xF000, 0x8000),	two(0xF1C0, 0xE3FF),	"Ii*ls8", mfloat },

/* {OP2(fmove,l),	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"Iis8@s", mfloat },
{OP2(fmove,l),	two(0xF000, 0x8000),	two(0xF2C0, 0xE3FF),	"Ii*ss8", mfloat }, */

{OP2(fmove,p),	two(0xF000, 0x4C00),	two(0xF1C0, 0xFC7F),	"Ii;pF7",   mfloat },		/* fmove from <ea> to fp<n> */
{OP2(fmove,p),	two(0xF000, 0x6C00),	two(0xF1C0, 0xFC00),	"IiF7@pkC", mfloat },		/* fmove.p with k-factors: */
{OP2(fmove,p),	two(0xF000, 0x7C00),	two(0xF1C0, 0xFC0F),	"IiF7@pDk", mfloat },		/* fmove.p with k-factors: */

{OP2(fmove,s),	two(0xF000, 0x4400),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },		/* fmove from <ea> to fp<n> */
{OP2(fmove,s),	two(0xF000, 0x6400),	two(0xF1C0, 0xFC7F),	"IiF7@f", mfloat },		/* fmove from fp<n> to <ea> */
{OP2(fmove,w),	two(0xF000, 0x5000),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },		/* fmove from <ea> to fp<n> */
{OP2(fmove,w),	two(0xF000, 0x7000),	two(0xF1C0, 0xFC7F),	"IiF7@w", mfloat },		/* fmove from fp<n> to <ea> */
{OP2(fmove,x),	two(0xF000, 0x0000),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },		/* fmove from <ea> to fp<n> */
{OP2(fmove,x),	two(0xF000, 0x4800),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },		/* fmove from <ea> to fp<n> */
{OP2(fmove,x),	two(0xF000, 0x6800),	two(0xF1C0, 0xFC7F),	"IiF7@x", mfloat },		/* fmove from fp<n> to <ea> */
/* JF removed {OP2(fmove,x),	two(0xF000, 0x0000),	two(0xF1C0, 0xE07F),	"IiFt", mfloat }, / * fmove from <ea> to fp<n> */

{OP2(fsmove,b),	two(0xF000, 0x5840),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fsmove,d),	two(0xF000, 0x5440),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fsmove,l),	two(0xF000, 0x4040),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fsmove,s),	two(0xF000, 0x4440),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fsmove,w),	two(0xF000, 0x5040),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fsmove,x),	two(0xF000, 0x0040),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fsmove,x),	two(0xF000, 0x4840),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fsmove,p),	two(0xF000, 0x4C40),	two(0xF1C0, 0xFC7F),	"Ii;pF7",   mfloat },		/* fmove from <ea> to fp<n> */
/* JF removed {OP2(fsmove,x),	two(0xF000, 0x0000),	two(0xF1C0, 0xE07F),	"IiFt", m68040 }, / * fmove from <ea> to fp<n> */

{OP2(fdmove,b),	two(0xF000, 0x5844),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fdmove,d),	two(0xF000, 0x5444),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fdmove,l),	two(0xF000, 0x4044),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fdmove,s),	two(0xF000, 0x4444),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fdmove,w),	two(0xF000, 0x5044),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fdmove,x),	two(0xF000, 0x0044),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fdmove,x),	two(0xF000, 0x4844),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 }, /* fmove from <ea> to fp<n> */
{OP2(fdmove,p),	two(0xF000, 0x4C44),	two(0xF1C0, 0xFC7F),	"Ii;pF7",   mfloat },		/* fmove from <ea> to fp<n> */
/* JF removed {OP2(fdmove,x),	two(0xF000, 0x0000),	two(0xF1C0, 0xE07F),	"IiFt", m68040 }, / * fmove from <ea> to fp<n> */

{OP2(fmovecr,x),	two(0xF000, 0x5C00),	two(0xF1FF, 0xFC00),	"Ii#CF7", mfloat },		/* fmovecr.x #ccc, FPn */
{"fmovecr",	two(0xF000, 0x5C00),	two(0xF1FF, 0xFC00),	"Ii#CF7", mfloat },

/* Other fmovemx.  */
{OP2(fmovem,x), two(0xF000, 0xF800), two(0xF1C0, 0xFF8F), "IiDk&s", mfloat }, /* reg to control,	static and dynamic: */
{OP2(fmovem,x), two(0xF000, 0xD800), two(0xF1C0, 0xFF8F), "Ii&sDk", mfloat }, /* from control to reg, static and dynamic: */

{OP2(fmovem,x), two(0xF000, 0xF000), two(0xF1C0, 0xFF00), "Idl3&s", mfloat }, /* to control, static and dynamic: */
{OP2(fmovem,x), two(0xF000, 0xF000), two(0xF1C0, 0xFF00), "Id#3&s", mfloat }, /* to control, static and dynamic: */

{OP2(fmovem,x), two(0xF000, 0xD000), two(0xF1C0, 0xFF00), "Id&sl3", mfloat }, /* from control, static and dynamic: */
{OP2(fmovem,x), two(0xF000, 0xD000), two(0xF1C0, 0xFF00), "Id&s#3", mfloat }, /* from control, static and dynamic: */

{OP2(fmovem,x), two(0xF020, 0xE800), two(0xF1F8, 0xFF8F), "IiDk-s", mfloat }, /* reg to autodecrement, static and dynamic */
{OP2(fmovem,x), two(0xF020, 0xE000), two(0xF1F8, 0xFF00), "IdL3-s", mfloat }, /* to autodecrement, static and dynamic */
{OP2(fmovem,x), two(0xF020, 0xE000), two(0xF1F8, 0xFF00), "Id#3-s", mfloat }, /* to autodecrement, static and dynamic */

{OP2(fmovem,x), two(0xF018, 0xD800), two(0xF1F8, 0xFF8F), "Ii+sDk", mfloat }, /* from autoinc to reg, static and dynamic: */
{OP2(fmovem,x), two(0xF018, 0xD000), two(0xF1F8, 0xFF00), "Id+sl3", mfloat }, /* from autoincrement, static and dynamic: */
{OP2(fmovem,x), two(0xF018, 0xD000), two(0xF1F8, 0xFF00), "Id+s#3", mfloat }, /* from autoincrement, static and dynamic: */

{OP2(fmovem,l),	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"IiL8@s", mfloat },
{OP2(fmovem,l),	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"Ii#8@s", mfloat },
{OP2(fmovem,l),	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"Iis8@s", mfloat },

{OP2(fmovem,l),	two(0xF000, 0x8000),	two(0xF2C0, 0xE3FF),	"Ii*sL8", mfloat },
{OP2(fmovem,l),	two(0xF000, 0x8000),	two(0xF1C0, 0xE3FF),	"Ii*s#8", mfloat },
{OP2(fmovem,l),	two(0xF000, 0x8000),	two(0xF1C0, 0xE3FF),	"Ii*ss8", mfloat },

/* fmovemx with register lists */
{"fmovem",	two(0xF020, 0xE000),	two(0xF1F8, 0xFF00),	"IdL3-s", mfloat }, /* to autodec, static & dynamic */
{"fmovem",	two(0xF000, 0xF000),	two(0xF1C0, 0xFF00),	"Idl3&s", mfloat }, /* to control, static and dynamic */
{"fmovem",	two(0xF018, 0xD000),	two(0xF1F8, 0xFF00),	"Id+sl3", mfloat }, /* from autoinc, static & dynamic */
{"fmovem",	two(0xF000, 0xD000),	two(0xF1C0, 0xFF00),	"Id&sl3", mfloat }, /* from control, static and dynamic */

	/* Alternate mnemonics for GNU as and GNU CC */
{"fmovem",	two(0xF020, 0xE000),	two(0xF1F8, 0xFF00),	"Id#3-s", mfloat }, /* to autodecrement, static and dynamic */
{"fmovem",	two(0xF020, 0xE800),	two(0xF1F8, 0xFF8F),	"IiDk-s", mfloat }, /* to autodecrement, static and dynamic */

{"fmovem",	two(0xF000, 0xF000),	two(0xF1C0, 0xFF00),	"Id#3&s", mfloat }, /* to control, static and dynamic: */
{"fmovem",	two(0xF000, 0xF800),	two(0xF1C0, 0xFF8F),	"IiDk&s", mfloat }, /* to control, static and dynamic: */

{"fmovem",	two(0xF018, 0xD000),	two(0xF1F8, 0xFF00),	"Id+s#3", mfloat }, /* from autoincrement, static and dynamic: */
{"fmovem",	two(0xF018, 0xD800),	two(0xF1F8, 0xFF8F),	"Ii+sDk", mfloat }, /* from autoincrement, static and dynamic: */
  
{"fmovem",	two(0xF000, 0xD000),	two(0xF1C0, 0xFF00),	"Id&s#3", mfloat }, /* from control, static and dynamic: */
{"fmovem",	two(0xF000, 0xD800),	two(0xF1C0, 0xFF8F),	"Ii&sDk", mfloat }, /* from control, static and dynamic: */

/* fmoveml a FP-control register */
{"fmovem",	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"Iis8@s", mfloat },
{"fmovem",	two(0xF000, 0x8000),	two(0xF1C0, 0xE3FF),	"Ii*ss8", mfloat },

/* fmoveml a FP-control reglist */
{"fmovem",	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"IiL8@s", mfloat },
{"fmovem",	two(0xF000, 0x8000),	two(0xF2C0, 0xE3FF),	"Ii*sL8", mfloat },

{OP2(fmul,b),	two(0xF000, 0x5823),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fmul,d),	two(0xF000, 0x5423),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fmul,l),	two(0xF000, 0x4023),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fmul,p),	two(0xF000, 0x4C23),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fmul,s),	two(0xF000, 0x4423),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fmul,w),	two(0xF000, 0x5023),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fmul,x),	two(0xF000, 0x0023),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fmul,x),	two(0xF000, 0x4823),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {OP2(fmul,x),	two(0xF000, 0x0023),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF */

{OP2(fsmul,b),	two(0xF000, 0x5863),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fsmul,d),	two(0xF000, 0x5463),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fsmul,l),	two(0xF000, 0x4063),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fsmul,p),	two(0xF000, 0x4C63),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fsmul,s),	two(0xF000, 0x4463),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fsmul,w),	two(0xF000, 0x5063),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fsmul,x),	two(0xF000, 0x0063),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fsmul,x),	two(0xF000, 0x4863),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {OP2(fsmul,x),	two(0xF000, 0x0033),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 }, JF */

{OP2(fdmul,b),	two(0xF000, 0x5867),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fdmul,d),	two(0xF000, 0x5467),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fdmul,l),	two(0xF000, 0x4067),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fdmul,p),	two(0xF000, 0x4C67),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fdmul,s),	two(0xF000, 0x4467),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fdmul,w),	two(0xF000, 0x5067),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fdmul,x),	two(0xF000, 0x0067),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fdmul,x),	two(0xF000, 0x4867),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {OP2(dfmul,x),	two(0xF000, 0x0067),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 }, JF */

{OP2(fneg,b),	two(0xF000, 0x581A),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fneg,d),	two(0xF000, 0x541A),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fneg,l),	two(0xF000, 0x401A),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fneg,p),	two(0xF000, 0x4C1A),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fneg,s),	two(0xF000, 0x441A),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fneg,w),	two(0xF000, 0x501A),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fneg,x),	two(0xF000, 0x001A),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fneg,x),	two(0xF000, 0x481A),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fneg,x),	two(0xF000, 0x001A),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fsneg,b),	two(0xF000, 0x585A),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fsneg,d),	two(0xF000, 0x545A),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fsneg,l),	two(0xF000, 0x405A),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fsneg,p),	two(0xF000, 0x4C5A),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fsneg,s),	two(0xF000, 0x445A),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fsneg,w),	two(0xF000, 0x505A),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fsneg,x),	two(0xF000, 0x005A),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fsneg,x),	two(0xF000, 0x485A),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{OP2(fsneg,x),	two(0xF000, 0x005A),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{OP2(fdneg,b),	two(0xF000, 0x585E),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fdneg,d),	two(0xF000, 0x545E),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fdneg,l),	two(0xF000, 0x405E),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fdneg,p),	two(0xF000, 0x4C5E),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fdneg,s),	two(0xF000, 0x445E),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fdneg,w),	two(0xF000, 0x505E),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fdneg,x),	two(0xF000, 0x005E),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fdneg,x),	two(0xF000, 0x485E),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{OP2(fdneg,x),	two(0xF000, 0x005E),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{"fnop",	two(0xF280, 0x0000),	two(0xFFFF, 0xFFFF),	"Ii", mfloat },

{OP2(frem,b),	two(0xF000, 0x5825),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(frem,d),	two(0xF000, 0x5425),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(frem,l),	two(0xF000, 0x4025),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(frem,p),	two(0xF000, 0x4C25),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(frem,s),	two(0xF000, 0x4425),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(frem,w),	two(0xF000, 0x5025),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(frem,x),	two(0xF000, 0x0025),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(frem,x),	two(0xF000, 0x4825),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {"fremx",	two(0xF000, 0x0025),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF */

{"frestore",	one(0xF140),		one(0xF1C0),		"Id&s", mfloat },
{"frestore",	one(0xF158),		one(0xF1F8),		"Id+s", mfloat },
{"fsave",	one(0xF100),		one(0xF1C0),		"Id&s", mfloat },
{"fsave",	one(0xF120),		one(0xF1F8),		"Id-s", mfloat },

{OP2(fscale,b),	two(0xF000, 0x5826),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fscale,d),	two(0xF000, 0x5426),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fscale,l),	two(0xF000, 0x4026),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fscale,p),	two(0xF000, 0x4C26),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fscale,s),	two(0xF000, 0x4426),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fscale,w),	two(0xF000, 0x5026),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fscale,x),	two(0xF000, 0x0026),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fscale,x),	two(0xF000, 0x4826),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {"fscalex",	two(0xF000, 0x0026),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF */

/* $ is necessary to prevent the assembler from using PC-relative.
   If @ were used, "label: fseq label" could produce "ftrapeq",
   because "label" became "pc@label".  */
{"fseq",	two(0xF040, 0x0001),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsf",		two(0xF040, 0x0000),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsge",	two(0xF040, 0x0013),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsgl",	two(0xF040, 0x0016),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsgle",	two(0xF040, 0x0017),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsgt",	two(0xF040, 0x0012),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsle",	two(0xF040, 0x0015),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fslt",	two(0xF040, 0x0014),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsne",	two(0xF040, 0x000E),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
#ifndef MOTOROLA_SYNTAX
{"fsneq",	two(0xF040, 0x000E),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
#endif
{"fsnge",	two(0xF040, 0x001C),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsngl",	two(0xF040, 0x0019),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsngle",	two(0xF040, 0x0018),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsngt",	two(0xF040, 0x001D),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsnle",	two(0xF040, 0x001A),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsnlt",	two(0xF040, 0x001B),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsoge",	two(0xF040, 0x0003),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsogl",	two(0xF040, 0x0006),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsogt",	two(0xF040, 0x0002),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsole",	two(0xF040, 0x0005),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsolt",	two(0xF040, 0x0004),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsor",	two(0xF040, 0x0007),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsseq",	two(0xF040, 0x0011),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fssf",	two(0xF040, 0x0010),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fssne",	two(0xF040, 0x001E),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
#ifndef MOTOROLA_SYNTAX
{"fssneq",	two(0xF040, 0x001E),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
#endif
{"fsst",	two(0xF040, 0x001F),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fst",		two(0xF040, 0x000F),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsueq",	two(0xF040, 0x0009),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsuge",	two(0xF040, 0x000B),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsugt",	two(0xF040, 0x000A),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsule",	two(0xF040, 0x000D),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsult",	two(0xF040, 0x000C),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsun",	two(0xF040, 0x0008),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },

{OP2(fsgldiv,b),	two(0xF000, 0x5824),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fsgldiv,d),	two(0xF000, 0x5424),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fsgldiv,l),	two(0xF000, 0x4024),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fsgldiv,p),	two(0xF000, 0x4C24),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fsgldiv,s),	two(0xF000, 0x4424),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fsgldiv,w),	two(0xF000, 0x5024),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fsgldiv,x),	two(0xF000, 0x0024),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fsgldiv,x),	two(0xF000, 0x4824),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fsgldiv,x),	two(0xF000, 0x0024),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fsglmul,b),	two(0xF000, 0x5827),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fsglmul,d),	two(0xF000, 0x5427),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fsglmul,l),	two(0xF000, 0x4027),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fsglmul,p),	two(0xF000, 0x4C27),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fsglmul,s),	two(0xF000, 0x4427),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fsglmul,w),	two(0xF000, 0x5027),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fsglmul,x),	two(0xF000, 0x0027),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fsglmul,x),	two(0xF000, 0x4827),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fsglmul,x),	two(0xF000, 0x0027),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fsin,b),	two(0xF000, 0x580E),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fsin,d),	two(0xF000, 0x540E),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fsin,l),	two(0xF000, 0x400E),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fsin,p),	two(0xF000, 0x4C0E),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fsin,s),	two(0xF000, 0x440E),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fsin,w),	two(0xF000, 0x500E),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fsin,x),	two(0xF000, 0x000E),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fsin,x),	two(0xF000, 0x480E),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fsin,x),	two(0xF000, 0x000E),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fsinh,b),	two(0xF000, 0x5802),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fsinh,d),	two(0xF000, 0x5402),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fsinh,l),	two(0xF000, 0x4002),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fsinh,p),	two(0xF000, 0x4C02),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fsinh,s),	two(0xF000, 0x4402),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fsinh,w),	two(0xF000, 0x5002),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fsinh,x),	two(0xF000, 0x0002),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fsinh,x),	two(0xF000, 0x4802),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fsinh,x),	two(0xF000, 0x0002),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fsincos,b),	two(0xF000, 0x5830),	two(0xF1C0, 0xFC78),	"Ii;bF3F7", mfloat },
{OP2(fsincos,d),	two(0xF000, 0x5430),	two(0xF1C0, 0xFC78),	"Ii;FF3F7", mfloat },
{OP2(fsincos,l),	two(0xF000, 0x4030),	two(0xF1C0, 0xFC78),	"Ii;lF3F7", mfloat },
{OP2(fsincos,p),	two(0xF000, 0x4C30),	two(0xF1C0, 0xFC78),	"Ii;pF3F7", mfloat },
{OP2(fsincos,s),	two(0xF000, 0x4430),	two(0xF1C0, 0xFC78),	"Ii;fF3F7", mfloat },
{OP2(fsincos,w),	two(0xF000, 0x5030),	two(0xF1C0, 0xFC78),	"Ii;wF3F7", mfloat },
{OP2(fsincos,x),	two(0xF000, 0x0030),	two(0xF1C0, 0xE078),	"IiF8F3F7", mfloat },
{OP2(fsincos,x),	two(0xF000, 0x4830),	two(0xF1C0, 0xFC78),	"Ii;xF3F7", mfloat },

{OP2(fsqrt,b),	two(0xF000, 0x5804),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fsqrt,d),	two(0xF000, 0x5404),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fsqrt,l),	two(0xF000, 0x4004),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fsqrt,p),	two(0xF000, 0x4C04),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fsqrt,s),	two(0xF000, 0x4404),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fsqrt,w),	two(0xF000, 0x5004),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fsqrt,x),	two(0xF000, 0x0004),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fsqrt,x),	two(0xF000, 0x4804),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fsqrt,x),	two(0xF000, 0x0004),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fssqrt,b),	two(0xF000, 0x5841),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fssqrt,d),	two(0xF000, 0x5441),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fssqrt,l),	two(0xF000, 0x4041),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fssqrt,p),	two(0xF000, 0x4C41),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fssqrt,s),	two(0xF000, 0x4441),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fssqrt,w),	two(0xF000, 0x5041),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fssqrt,x),	two(0xF000, 0x0041),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fssqrt,x),	two(0xF000, 0x4841),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{OP2(fssqrt,x),	two(0xF000, 0x0041),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{OP2(fdsqrt,b),	two(0xF000, 0x5845),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fdsqrt,d),	two(0xF000, 0x5445),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fdsqrt,l),	two(0xF000, 0x4045),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fdsqrt,p),	two(0xF000, 0x4C45),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fdsqrt,s),	two(0xF000, 0x4445),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fdsqrt,w),	two(0xF000, 0x5045),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fdsqrt,x),	two(0xF000, 0x0045),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fdsqrt,x),	two(0xF000, 0x4845),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{OP2(fdsqrt,x),	two(0xF000, 0x0045),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{OP2(fsub,b),	two(0xF000, 0x5828),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(fsub,d),	two(0xF000, 0x5428),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(fsub,l),	two(0xF000, 0x4028),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(fsub,p),	two(0xF000, 0x4C28),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(fsub,s),	two(0xF000, 0x4428),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(fsub,w),	two(0xF000, 0x5028),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(fsub,x),	two(0xF000, 0x0028),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(fsub,x),	two(0xF000, 0x4828),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(fsub,x),	two(0xF000, 0x0028),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(fssub,b),	two(0xF000, 0x5868),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fssub,d),	two(0xF000, 0x5468),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fssub,l),	two(0xF000, 0x4068),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fssub,p),	two(0xF000, 0x4C68),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fssub,s),	two(0xF000, 0x4468),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fssub,w),	two(0xF000, 0x5068),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fssub,x),	two(0xF000, 0x0068),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fssub,x),	two(0xF000, 0x4868),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{OP2(fssub,x),	two(0xF000, 0x0068),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{OP2(fdsub,b),	two(0xF000, 0x586c),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{OP2(fdsub,d),	two(0xF000, 0x546c),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{OP2(fdsub,l),	two(0xF000, 0x406c),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{OP2(fdsub,p),	two(0xF000, 0x4C6c),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{OP2(fdsub,s),	two(0xF000, 0x446c),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{OP2(fdsub,w),	two(0xF000, 0x506c),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{OP2(fdsub,x),	two(0xF000, 0x006c),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{OP2(fdsub,x),	two(0xF000, 0x486c),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{OP2(fdsub,x),	two(0xF000, 0x006c),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{OP2(ftan,b),	two(0xF000, 0x580F),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(ftan,d),	two(0xF000, 0x540F),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(ftan,l),	two(0xF000, 0x400F),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(ftan,p),	two(0xF000, 0x4C0F),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(ftan,s),	two(0xF000, 0x440F),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(ftan,w),	two(0xF000, 0x500F),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(ftan,x),	two(0xF000, 0x000F),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(ftan,x),	two(0xF000, 0x480F),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(ftan,x),	two(0xF000, 0x000F),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(ftanh,b),	two(0xF000, 0x5809),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(ftanh,d),	two(0xF000, 0x5409),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(ftanh,l),	two(0xF000, 0x4009),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(ftanh,p),	two(0xF000, 0x4C09),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(ftanh,s),	two(0xF000, 0x4409),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(ftanh,w),	two(0xF000, 0x5009),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(ftanh,x),	two(0xF000, 0x0009),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(ftanh,x),	two(0xF000, 0x4809),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(ftanh,x),	two(0xF000, 0x0009),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{OP2(ftentox,b),	two(0xF000, 0x5812),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(ftentox,d),	two(0xF000, 0x5412),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(ftentox,l),	two(0xF000, 0x4012),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(ftentox,p),	two(0xF000, 0x4C12),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(ftentox,s),	two(0xF000, 0x4412),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(ftentox,w),	two(0xF000, 0x5012),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(ftentox,x),	two(0xF000, 0x0012),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(ftentox,x),	two(0xF000, 0x4812),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(ftentox,x),	two(0xF000, 0x0012),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"ftrapeq",	two(0xF07C, 0x0001),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapf",	two(0xF07C, 0x0000),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapge",	two(0xF07C, 0x0013),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapgl",	two(0xF07C, 0x0016),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapgle",	two(0xF07C, 0x0017),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapgt",	two(0xF07C, 0x0012),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftraple",	two(0xF07C, 0x0015),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftraplt",	two(0xF07C, 0x0014),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapne",	two(0xF07C, 0x000E),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
#ifndef MOTOROLA_SYNTAX
{"ftrapneq",	two(0xF07C, 0x000E),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
#endif
{"ftrapnge",	two(0xF07C, 0x001C),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapngl",	two(0xF07C, 0x0019),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapngle",	two(0xF07C, 0x0018),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapngt",	two(0xF07C, 0x001D),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapnle",	two(0xF07C, 0x001A),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapnlt",	two(0xF07C, 0x001B),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapoge",	two(0xF07C, 0x0003),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapogl",	two(0xF07C, 0x0006),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapogt",	two(0xF07C, 0x0002),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapole",	two(0xF07C, 0x0005),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapolt",	two(0xF07C, 0x0004),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapor",	two(0xF07C, 0x0007),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapseq",	two(0xF07C, 0x0011),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapsf",	two(0xF07C, 0x0010),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapsne",	two(0xF07C, 0x001E),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
#ifndef MOTOROLA_SYNTAX
{"ftrapsneq",	two(0xF07C, 0x001E),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
#endif
{"ftrapst",	two(0xF07C, 0x001F),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapt",	two(0xF07C, 0x000F),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapueq",	two(0xF07C, 0x0009),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapuge",	two(0xF07C, 0x000B),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapugt",	two(0xF07C, 0x000A),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapule",	two(0xF07C, 0x000D),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapult",	two(0xF07C, 0x000C),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapun",	two(0xF07C, 0x0008),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
        
{OP2(ftrapeq,w),	two(0xF07A, 0x0001),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapf,w),	two(0xF07A, 0x0000),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapge,w),	two(0xF07A, 0x0013),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapgl,w),	two(0xF07A, 0x0016),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapgle,w),	two(0xF07A, 0x0017),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapgt,w),	two(0xF07A, 0x0012),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftraple,w),	two(0xF07A, 0x0015),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftraplt,w),	two(0xF07A, 0x0014),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapne,w),	two(0xF07A, 0x000E),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapnge,w),	two(0xF07A, 0x001C),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapngl,w),	two(0xF07A, 0x0019),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapngle,w),	two(0xF07A, 0x0018),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapngt,w),	two(0xF07A, 0x001D),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapnle,w),	two(0xF07A, 0x001A),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapnlt,w),	two(0xF07A, 0x001B),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapoge,w),	two(0xF07A, 0x0003),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapogl,w),	two(0xF07A, 0x0006),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapogt,w),	two(0xF07A, 0x0002),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapole,w),	two(0xF07A, 0x0005),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapolt,w),	two(0xF07A, 0x0004),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapor,w),	two(0xF07A, 0x0007),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapseq,w),	two(0xF07A, 0x0011),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapsf,w),	two(0xF07A, 0x0010),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapsne,w),	two(0xF07A, 0x001E),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapst,w),	two(0xF07A, 0x001F),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapt,w),	two(0xF07A, 0x000F),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapueq,w),	two(0xF07A, 0x0009),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapuge,w),	two(0xF07A, 0x000B),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapugt,w),	two(0xF07A, 0x000A),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapule,w),	two(0xF07A, 0x000D),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapult,w),	two(0xF07A, 0x000C),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{OP2(ftrapun,w),	two(0xF07A, 0x0008),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },

{OP2(ftrapeq,l),	two(0xF07B, 0x0001),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapf,l),	two(0xF07B, 0x0000),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapge,l),	two(0xF07B, 0x0013),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapgl,l),	two(0xF07B, 0x0016),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapgle,l),	two(0xF07B, 0x0017),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapgt,l),	two(0xF07B, 0x0012),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftraple,l),	two(0xF07B, 0x0015),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftraplt,l),	two(0xF07B, 0x0014),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapne,l),	two(0xF07B, 0x000E),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapnge,l),	two(0xF07B, 0x001C),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapngl,l),	two(0xF07B, 0x0019),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapngle,l),	two(0xF07B, 0x0018),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapngt,l),	two(0xF07B, 0x001D),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapnle,l),	two(0xF07B, 0x001A),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapnlt,l),	two(0xF07B, 0x001B),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapoge,l),	two(0xF07B, 0x0003),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapogl,l),	two(0xF07B, 0x0006),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapogt,l),	two(0xF07B, 0x0002),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapole,l),	two(0xF07B, 0x0005),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapolt,l),	two(0xF07B, 0x0004),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapor,l),	two(0xF07B, 0x0007),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapseq,l),	two(0xF07B, 0x0011),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapsf,l),	two(0xF07B, 0x0010),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapsne,l),	two(0xF07B, 0x001E),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapst,l),	two(0xF07B, 0x001F),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapt,l),	two(0xF07B, 0x000F),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapueq,l),	two(0xF07B, 0x0009),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapuge,l),	two(0xF07B, 0x000B),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapugt,l),	two(0xF07B, 0x000A),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapule,l),	two(0xF07B, 0x000D),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapult,l),	two(0xF07B, 0x000C),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{OP2(ftrapun,l),	two(0xF07B, 0x0008),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },

{OP2(ftst,b),	two(0xF000, 0x583A),	two(0xF1C0, 0xFC7F),	"Ii;b", mfloat },
{OP2(ftst,d),	two(0xF000, 0x543A),	two(0xF1C0, 0xFC7F),	"Ii;F", mfloat },
{OP2(ftst,l),	two(0xF000, 0x403A),	two(0xF1C0, 0xFC7F),	"Ii;l", mfloat },
{OP2(ftst,p),	two(0xF000, 0x4C3A),	two(0xF1C0, 0xFC7F),	"Ii;p", mfloat },
{OP2(ftst,s),	two(0xF000, 0x443A),	two(0xF1C0, 0xFC7F),	"Ii;f", mfloat },
{OP2(ftst,w),	two(0xF000, 0x503A),	two(0xF1C0, 0xFC7F),	"Ii;w", mfloat },
{OP2(ftst,x),	two(0xF000, 0x003A),	two(0xF1C0, 0xE07F),	"IiF8", mfloat },
{OP2(ftst,x),	two(0xF000, 0x483A),	two(0xF1C0, 0xFC7F),	"Ii;x", mfloat },

{OP2(ftwotox,b),	two(0xF000, 0x5811),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{OP2(ftwotox,d),	two(0xF000, 0x5411),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{OP2(ftwotox,l),	two(0xF000, 0x4011),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{OP2(ftwotox,p),	two(0xF000, 0x4C11),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{OP2(ftwotox,s),	two(0xF000, 0x4411),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{OP2(ftwotox,w),	two(0xF000, 0x5011),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{OP2(ftwotox,x),	two(0xF000, 0x0011),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{OP2(ftwotox,x),	two(0xF000, 0x4811),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{OP2(ftwotox,x),	two(0xF000, 0x0011),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

/* Variable-sized float branches */

{"fjeq",	one(0xF081),		one(0xF1FF),		"IdBc", mfloat },
{"fjf",		one(0xF080),		one(0xF1FF),		"IdBc", mfloat },
{"fjge",	one(0xF093),		one(0xF1FF),		"IdBc", mfloat },
{"fjgl",	one(0xF096),		one(0xF1FF),		"IdBc", mfloat },
{"fjgle",	one(0xF097),		one(0xF1FF),		"IdBc", mfloat },
{"fjgt",	one(0xF092),		one(0xF1FF),		"IdBc", mfloat },
{"fjle",	one(0xF095),		one(0xF1FF),		"IdBc", mfloat },
{"fjlt",	one(0xF094),		one(0xF1FF),		"IdBc", mfloat },
{"fjne",	one(0xF08E),		one(0xF1FF),		"IdBc", mfloat },
#ifndef MOTOROLA_SYNTAX
{"fjneq",	one(0xF08E),		one(0xF1FF),		"IdBc", mfloat },
#endif
{"fjnge",	one(0xF09C),		one(0xF1FF),		"IdBc", mfloat },
{"fjngl",	one(0xF099),		one(0xF1FF),		"IdBc", mfloat },
{"fjngle",	one(0xF098),		one(0xF1FF),		"IdBc", mfloat },
{"fjngt",	one(0xF09D),		one(0xF1FF),		"IdBc", mfloat },
{"fjnle",	one(0xF09A),		one(0xF1FF),		"IdBc", mfloat },
{"fjnlt",	one(0xF09B),		one(0xF1FF),		"IdBc", mfloat },
{"fjoge",	one(0xF083),		one(0xF1FF),		"IdBc", mfloat },
{"fjogl",	one(0xF086),		one(0xF1FF),		"IdBc", mfloat },
{"fjogt",	one(0xF082),		one(0xF1FF),		"IdBc", mfloat },
{"fjole",	one(0xF085),		one(0xF1FF),		"IdBc", mfloat },
{"fjolt",	one(0xF084),		one(0xF1FF),		"IdBc", mfloat },
{"fjor",	one(0xF087),		one(0xF1FF),		"IdBc", mfloat },
{"fjseq",	one(0xF091),		one(0xF1FF),		"IdBc", mfloat },
{"fjsf",	one(0xF090),		one(0xF1FF),		"IdBc", mfloat },
{"fjsne",	one(0xF09E),		one(0xF1FF),		"IdBc", mfloat },
#ifndef MOTOROLA_SYNTAX
{"fjsneq",	one(0xF09E),		one(0xF1FF),		"IdBc", mfloat },
#endif
{"fjst",	one(0xF09F),		one(0xF1FF),		"IdBc", mfloat },
{"fjt",		one(0xF08F),		one(0xF1FF),		"IdBc", mfloat },
{"fjueq",	one(0xF089),		one(0xF1FF),		"IdBc", mfloat },
{"fjuge",	one(0xF08B),		one(0xF1FF),		"IdBc", mfloat },
{"fjugt",	one(0xF08A),		one(0xF1FF),		"IdBc", mfloat },
{"fjule",	one(0xF08D),		one(0xF1FF),		"IdBc", mfloat },
{"fjult",	one(0xF08C),		one(0xF1FF),		"IdBc", mfloat },
{"fjun",	one(0xF088),		one(0xF1FF),		"IdBc", mfloat },
/* float stuff ends here */
{"illegal",	one(0045374),		one(0177777),		"",     m68000up },


#ifdef MOTOROLA_SYNTAX
{OP2(jmp,s),		one(0047300),		one(0177700),		"!s",   m68000up },
{OP2(jmp,l),		one(0047300),		one(0177700),		"!s",   m68000up },
#endif

{"jmp",		one(0047300),		one(0177700),		"!s",   m68000up },
{"jsr",		one(0047200),		one(0177700),		"!s",   m68000up },
{"lea",		one(0040700),		one(0170700),		"!sAd", m68000up },
#ifdef MOTOROLA_SYNTAX
{OP2(lea,l),	one(0040700),		one(0170700),		"!sAd", m68000up },	/* allow lea as leal */
#endif

{OP2(link,w),	one(0047120),		one(0177770),		"As#w", m68000up },
{OP2(link,l),	one(0044010),		one(0177770),		"As#l", m68020up },
{"link",	one(0047120),		one(0177770),		"As#w", m68000up },
{"link",	one(0044010),		one(0177770),		"As#l", m68020up },

{OP2(lsl,b),	one(0160410),		one(0170770),		"QdDs", m68000up },	/* lsrb #Q,	Ds */
{OP2(lsl,b),	one(0160450),		one(0170770),		"DdDs", m68000up },	/* lsrb Dd,	Ds */
{OP2(lsl,w),	one(0160510),		one(0170770),		"QdDs", m68000up },	/* lsrb #Q,	Ds */
{OP2(lsl,w),	one(0160550),		one(0170770),		"DdDs", m68000up },	/* lsrb Dd,	Ds */
{OP2(lsl,w),	one(0161700),		one(0177700),		"~s",   m68000up },	/* Shift memory */
{OP2(lsl,l),	one(0160610),		one(0170770),		"QdDs", m68000up },	/* lsrb #Q,	Ds */
{OP2(lsl,l),	one(0160650),		one(0170770),		"DdDs", m68000up },	/* lsrb Dd,	Ds */

{OP2(lsr,b),	one(0160010),		one(0170770),		"QdDs", m68000up }, /* lsrb #Q,	Ds */
{OP2(lsr,b),	one(0160050),		one(0170770),		"DdDs", m68000up },	/* lsrb Dd,	Ds */
{OP2(lsr,l),	one(0160210),		one(0170770),		"QdDs", m68000up },	/* lsrb #Q,	Ds */
{OP2(lsr,l),	one(0160250),		one(0170770),		"DdDs", m68000up },	/* lsrb #Q,	Ds */
{OP2(lsr,w),	one(0160110),		one(0170770),		"QdDs", m68000up },	/* lsrb #Q,	Ds */
{OP2(lsr,w),	one(0160150),		one(0170770),		"DdDs", m68000up },	/* lsrb #Q,	Ds */
{OP2(lsr,w),	one(0161300),		one(0177700),		"~s",   m68000up },	/* Shift memory */

{OP2(movea,l),	one(0020100),		one(0170700),		"*lAd", m68000up },
{OP2(movea,w),	one(0030100),		one(0170700),		"*wAd", m68000up },
{OP2(move,b),	one(0010000),		one(0170000),		";b$d", m68000up },	/* move */
{OP2(move,l),	one(0070000),		one(0170400),		"MsDd", m68000up },	/* moveq written as move */
{OP2(move,l),	one(0020000),		one(0170000),		"*l$d", m68000up },
{OP2(move,l),	one(0020100),		one(0170700),		"*lAd", m68000up },
{OP2(move,l),	one(0047140),		one(0177770),		"AsUd", m68000up },	/* move to USP */
{OP2(move,l),	one(0047150),		one(0177770),		"UdAs", m68000up },	/* move from USP */

{"movec",	one(0047173),		one(0177777),		"R1Jj", m68010up },
{"movec",	one(0047173),		one(0177777),		"R1#j", m68010up },
{"movec",	one(0047172),		one(0177777),		"JjR1", m68010up },
{"movec",	one(0047172),		one(0177777),		"#jR1", m68010up },

/* JF added these next four for the assembler */
{OP2(movem,l),	one(0044300),		one(0177700),		"Lw&s", m68000up },	/* movem reg to mem. */
{OP2(movem,l),	one(0044340),		one(0177770),		"lw-s", m68000up },	/* movem reg to autodecrement. */
{OP2(movem,l),	one(0046300),		one(0177700),		"!sLw", m68000up },	/* movem mem to reg. */
{OP2(movem,l),	one(0046330),		one(0177770),		"+sLw", m68000up },	/* movem autoinc to reg. */

{OP2(movem,l),	one(0044300),		one(0177700),		"#w&s", m68000up },	/* movem reg to mem. */
{OP2(movem,l),	one(0044340),		one(0177770),		"#w-s", m68000up },	/* movem reg to autodecrement. */
{OP2(movem,l),	one(0046300),		one(0177700),		"!s#w", m68000up },	/* movem mem to reg. */
{OP2(movem,l),	one(0046330),		one(0177770),		"+s#w", m68000up },	/* movem autoinc to reg. */

/* JF added these next four for the assembler */
{OP2(movem,w),	one(0044200),		one(0177700),		"Lw&s", m68000up },	/* movem reg to mem. */
{OP2(movem,w),	one(0044240),		one(0177770),		"lw-s", m68000up },	/* movem reg to autodecrement. */
{OP2(movem,w),	one(0046200),		one(0177700),		"!sLw", m68000up },	/* movem mem to reg. */
{OP2(movem,w),	one(0046230),		one(0177770),		"+sLw", m68000up },	/* movem autoinc to reg. */

{OP2(movem,w),	one(0044200),		one(0177700),		"#w&s", m68000up },	/* movem reg to mem. */
{OP2(movem,w),	one(0044240),		one(0177770),		"#w-s", m68000up },	/* movem reg to autodecrement. */
{OP2(movem,w),	one(0046200),		one(0177700),		"!s#w", m68000up },	/* movem mem to reg. */
{OP2(movem,w),	one(0046230),		one(0177770),		"+s#w", m68000up },	/* movem autoinc to reg. */
#ifdef ALLOW_DEFAULT_SIZES
{"movem",	one(0044200),		one(0177700),		"Lw&s", m68000up },	/* movem reg to mem. */
{"movem",	one(0044240),		one(0177770),		"lw-s", m68000up },	/* movem reg to autodecrement. */
{"movem",	one(0046200),		one(0177700),		"!sLw", m68000up },	/* movem mem to reg. */
{"movem",	one(0046230),		one(0177770),		"+sLw", m68000up },	/* movem autoinc to reg. */

{"movem",	one(0044200),		one(0177700),		"#w&s", m68000up },	/* movem reg to mem. */
{"movem",	one(0044240),		one(0177770),		"#w-s", m68000up },	/* movem reg to autodecrement. */
{"movem",	one(0046200),		one(0177700),		"!s#w", m68000up },	/* movem mem to reg. */
{"movem",	one(0046230),		one(0177770),		"+s#w", m68000up },	/* movem autoinc to reg. */
#endif

{OP2(movep,l),	one(0000510),		one(0170770),		"dsDd", m68000up },	/* memory to register */
{OP2(movep,l),	one(0000710),		one(0170770),		"Ddds", m68000up },	/* register to memory */
{OP2(movep,w),	one(0000410),		one(0170770),		"dsDd", m68000up },	/* memory to register */
{OP2(movep,w),	one(0000610),		one(0170770),		"Ddds", m68000up },	/* register to memory */
#ifdef ALLOW_DEFAULT_SIZES
{"movep",	one(0000410),		one(0170770),		"dsDd", m68000up },	/* memory to register */
{"movep",	one(0000610),		one(0170770),		"Ddds", m68000up },	/* register to memory */
#endif
{OP2(move,q),	one(0070000),		one(0170400),		"MsDd", m68000up },
{OP2(move,w),	one(0030000),		one(0170000),		"*w$d", m68000up },
{OP2(move,w),	one(0030100),		one(0170700),		"*wAd", m68000up },	/* movea,	written as move */
{OP2(move,w),	one(0040300),		one(0177700),		"Ss$s", m68000up },	/* Move from sr */
{OP2(move,w),	one(0041300),		one(0177700),		"Cs$s", m68010up },	/* Move from ccr */
{OP2(move,w),	one(0042300),		one(0177700),		";wCd", m68000up },	/* move to ccr */
{OP2(move,w),	one(0043300),		one(0177700),		";wSd", m68000up },	/* move to sr */


{OP2(moves,b),	two(0007000, 0),	two(0177700, 07777),	"~sR1", m68010up },	 /* moves from memory */
{OP2(moves,b),	two(0007000, 04000),	two(0177700, 07777),	"R1~s", m68010up },	 /* moves to memory */
{OP2(moves,l),	two(0007200, 0),	two(0177700, 07777),	"~sR1", m68010up },	 /* moves from memory */
{OP2(moves,l),	two(0007200, 04000),	two(0177700, 07777),	"R1~s", m68010up },	 /* moves to memory */
{OP2(moves,w),	two(0007100, 0),	two(0177700, 07777),	"~sR1", m68010up },	 /* moves from memory */
{OP2(moves,w),	two(0007100, 04000),	two(0177700, 07777),	"R1~s", m68010up },	 /* moves to memory */

{"move16",	two(0xf620, 0x8000),	two(0xfff8, 0x8fff),	"+s+1", m68040 },
{"move16",	one(0xf600),	one(0xfff8),	"+s_L", m68040 },
{"move16",	one(0xf608),	one(0xfff8),	"_L+s", m68040 },
{"move16",	one(0xf610),	one(0xfff8),	"as_L", m68040 },
{"move16",	one(0xf618),	one(0xfff8),	"_Las", m68040 },

{OP2(muls,l),	two(0046000, 004000),	two(0177700, 0107770),	";lD1", m68020up },
{OP2(muls,l),	two(0046000, 006000),	two(0177700, 0107770),	";lD3D1", m68020up },
{OP2(muls,w),	one(0140700),		one(0170700),		";wDd", m68000up },
{"muls",	one(0140700),		one(0170700),		";wDd", m68000up },
{OP2(mulu,l),	two(0046000, 000000),	two(0177700, 0107770),	";lD1", m68020up },
{OP2(mulu,l),	two(0046000, 002000),	two(0177700, 0107770),	";lD3D1", m68020up },
{OP2(mulu,w),	one(0140300),		one(0170700),		";wDd", m68000up },
{"mulu",	one(0140300),		one(0170700),		";wDd", m68000up },
{"nbcd",	one(0044000),		one(0177700),		"$s", m68000up },

{OP2(neg,b),	one(0042000),		one(0177700),		"$s", m68000up },
{OP2(neg,l),	one(0042200),		one(0177700),		"$s", m68000up },
{OP2(neg,w),	one(0042100),		one(0177700),		"$s", m68000up },
#ifdef ALLOW_DEFAULT_SIZES
{"neg",		one(0042200),		one(0177700),		"$s", m68000up },
#endif

{OP2(negx,b),	one(0040000),		one(0177700),		"$s", m68000up },
{OP2(negx,l),	one(0040200),		one(0177700),		"$s", m68000up },
{OP2(negx,w),	one(0040100),		one(0177700),		"$s", m68000up },
#ifdef ALLOW_DEFAULT_SIZES
{"negx",	one(0040200),		one(0177700),		"$s", m68000up },
#endif

{"nop",		one(0047161),		one(0177777),		"", m68000up },

{OP2(not,b),	one(0043000),		one(0177700),		"$s", m68000up },
{OP2(not,l),	one(0043200),		one(0177700),		"$s", m68000up },
{OP2(not,w),	one(0043100),		one(0177700),		"$s", m68000up },
#ifdef ALLOW_DEFAULT_SIZES
{"not",		one(0043200),		one(0177700),		"$s", m68000up },
#endif

{OP2(or,b),		one(0000000),		one(0177700),		"#b$s", m68000up },	/* ori written as or */
{OP2(or,b),		one(0000074),		one(0177777),		"#bCs", m68000up },	/* ori to ccr */
{OP2(or,b),		one(0100000),		one(0170700),		";bDd", m68000up },	/* memory to register */
{OP2(or,b),		one(0100400),		one(0170700),		"Dd~s", m68000up },	/* register to memory */
{OP2(ori,b),	one(0000000),		one(0177700),		"#b$s", m68000up },
{OP2(ori,b),	one(0000074),		one(0177777),		"#bCs", m68000up },	/* ori to ccr */
{OP2(ori,l),	one(0000200),		one(0177700),		"#l$s", m68000up },
{OP2(ori,w),	one(0000100),		one(0177700),		"#w$s", m68000up },
{OP2(ori,w),	one(0000174),		one(0177777),		"#wSs", m68000up },	/* ori to sr */
{OP2(or,l),		one(0000200),		one(0177700),		"#l$s", m68000up },
{OP2(or,l),		one(0100200),		one(0170700),		";lDd", m68000up },	/* memory to register */
{OP2(or,l),		one(0100600),		one(0170700),		"Dd~s", m68000up },	/* register to memory */
{OP2(or,w),		one(0000100),		one(0177700),		"#w$s", m68000up },
{OP2(or,w),		one(0000174),		one(0177777),		"#wSs", m68000up },	/* ori to sr */
{OP2(or,w),		one(0100100),		one(0170700),		";wDd", m68000up },	/* memory to register */
{OP2(or,w),		one(0100500),		one(0170700),		"Dd~s", m68000up },	/* register to memory */
#ifdef ALLOW_DEFAULT_SIZES
{"ori",		one(0000174),		one(0177777),		"#wSs", m68000up },	/* ori to sr */

{"or",		one(0000100),		one(0177700),		"#w$s", m68000up },
{"or",		one(0000174),		one(0177777),		"#wSs", m68000up },	/* ori to sr */
{"or",		one(0100100),		one(0170700),		";wDd", m68000up },	/* memory to register */
{"or",		one(0100500),		one(0170700),		"Dd~s", m68000up },	/* register to memory */
#endif

{"pack",	one(0100500),		one(0170770),		"DsDd#w", m68020up },	/* pack Ds,	Dd,	#w */
{"pack",	one(0100510),		one(0170770),		"-s-d#w", m68020up },	/* pack -(As),	-(Ad),	#w */

#ifndef NO_68851
{"pbac",	one(0xf0c7),		one(0xffbf),		"Bc", m68851 },
{OP2(pbac,w),	one(0xf087),		one(0xffbf),		"Bc", m68851 },
{"pbas",	one(0xf0c6),		one(0xffbf),		"Bc", m68851 },
{OP2(pbas,w),	one(0xf086),		one(0xffbf),		"Bc", m68851 },
{"pbbc",	one(0xf0c1),		one(0xffbf),		"Bc", m68851 },
{OP2(pbbc,w),	one(0xf081),		one(0xffbf),		"Bc", m68851 },
{"pbbs",	one(0xf0c0),		one(0xffbf),		"Bc", m68851 },
{OP2(pbbs,w),	one(0xf080),		one(0xffbf),		"Bc", m68851 },
{"pbcc",	one(0xf0cf),		one(0xffbf),		"Bc", m68851 },
{OP2(pbcc,w),	one(0xf08f),		one(0xffbf),		"Bc", m68851 },
{"pbcs",	one(0xf0ce),		one(0xffbf),		"Bc", m68851 },
{OP2(pbcs,w),	one(0xf08e),		one(0xffbf),		"Bc", m68851 },
{"pbgc",	one(0xf0cd),		one(0xffbf),		"Bc", m68851 },
{OP2(pbgc,w),	one(0xf08d),		one(0xffbf),		"Bc", m68851 },
{"pbgs",	one(0xf0cc),		one(0xffbf),		"Bc", m68851 },
{OP2(pbgs,w),	one(0xf08c),		one(0xffbf),		"Bc", m68851 },
{"pbic",	one(0xf0cb),		one(0xffbf),		"Bc", m68851 },
{OP2(pbic,w),	one(0xf08b),		one(0xffbf),		"Bc", m68851 },
{"pbis",	one(0xf0ca),		one(0xffbf),		"Bc", m68851 },
{OP2(pbis,w),	one(0xf08a),		one(0xffbf),		"Bc", m68851 },
{"pblc",	one(0xf0c3),		one(0xffbf),		"Bc", m68851 },
{OP2(pblc,w),	one(0xf083),		one(0xffbf),		"Bc", m68851 },
{"pbls",	one(0xf0c2),		one(0xffbf),		"Bc", m68851 },
{OP2(pbls,w),	one(0xf082),		one(0xffbf),		"Bc", m68851 },
{"pbsc",	one(0xf0c5),		one(0xffbf),		"Bc", m68851 },
{OP2(pbsc,w),	one(0xf085),		one(0xffbf),		"Bc", m68851 },
{"pbss",	one(0xf0c4),		one(0xffbf),		"Bc", m68851 },
{OP2(pbss,w),	one(0xf084),		one(0xffbf),		"Bc", m68851 },
{"pbwc",	one(0xf0c9),		one(0xffbf),		"Bc", m68851 },
{OP2(pbwc,w),	one(0xf089),		one(0xffbf),		"Bc", m68851 },
{"pbws",	one(0xf0c8),		one(0xffbf),		"Bc", m68851 },
{OP2(pbws,w),	one(0xf088),		one(0xffbf),		"Bc", m68851 },

{"pdbac",	two(0xf048, 0x0007),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbas",	two(0xf048, 0x0006),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbbc",	two(0xf048, 0x0001),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbbs",	two(0xf048, 0x0000),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbcc",	two(0xf048, 0x000f),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbcs",	two(0xf048, 0x000e),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbgc",	two(0xf048, 0x000d),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbgs",	two(0xf048, 0x000c),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbic",	two(0xf048, 0x000b),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbis",	two(0xf048, 0x000a),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdblc",	two(0xf048, 0x0003),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbls",	two(0xf048, 0x0002),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbsc",	two(0xf048, 0x0005),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbss",	two(0xf048, 0x0004),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbwc",	two(0xf048, 0x0009),	two(0xfff8, 0xffff),	"DsBw", m68851 },
{"pdbws",	two(0xf048, 0x0008),	two(0xfff8, 0xffff),	"DsBw", m68851 },
#endif /* NO_68851 */

{"pea",		one(0044100),		one(0177700),		"!s", m68000up },

#ifndef NO_68851
{"pflusha",	two(0xf000, 0x2400),	two(0xffff, 0xffff),	"",		m68030 | m68851 },
{"pflusha",	one(0xf510),		one(0xfff8), 		"",		m68040 },

{"pflush",	two(0xf000, 0x3010),	two(0xffc0, 0xfe10),	"T3T9",		m68030 | m68851 },
{"pflush",	two(0xf000, 0x3810),	two(0xffc0, 0xfe10),	"T3T9&s",	m68030 | m68851 },
{"pflush",	two(0xf000, 0x3008),	two(0xffc0, 0xfe18),	"D3T9",		m68030 | m68851 },
{"pflush",	two(0xf000, 0x3808),	two(0xffc0, 0xfe18),	"D3T9&s",	m68030 | m68851 },
{"pflush",	two(0xf000, 0x3000),	two(0xffc0, 0xfe1e),	"f3T9",		m68030 | m68851 },
{"pflush",	two(0xf000, 0x3800),	two(0xffc0, 0xfe1e),	"f3T9&s",	m68030 | m68851 },
{"pflush",	one(0xf500),		one(0xfff8), 		"as",		m68040 },

{"pflushan",	one(0xf518),		one(0xfff8),		"",		m68040 },
{"pflushn",	one(0xf508),		one(0xfff8),		"as",		m68040 },

{"pflushr",	two(0xf000, 0xa000),	two(0xffc0, 0xffff),	"|s",		m68851 },

{"pflushs",	two(0xf000, 0x3410),	two(0xfff8, 0xfe10),	"T3T9",		m68851 },
{"pflushs",	two(0xf000, 0x3c10),	two(0xfff8, 0xfe00),	"T3T9&s",	m68851 },
{"pflushs",	two(0xf000, 0x3408),	two(0xfff8, 0xfe18),	"D3T9",		m68851 },
{"pflushs",	two(0xf000, 0x3c08),	two(0xfff8, 0xfe18),	"D3T9&s",	m68851 },
{"pflushs",	two(0xf000, 0x3400),	two(0xfff8, 0xfe1e),	"f3T9",		m68851 },
{"pflushs",	two(0xf000, 0x3c00),	two(0xfff8, 0xfe1e),	"f3T9&s",	m68851 },

{"ploadr",	two(0xf000, 0x2210),	two(0xffc0, 0xfff0),	"T3&s",	m68030 | m68851 },
{"ploadr",	two(0xf000, 0x2208),	two(0xffc0, 0xfff8),	"D3&s",	m68030 | m68851 },
{"ploadr",	two(0xf000, 0x2200),	two(0xffc0, 0xfffe),	"f3&s",	m68030 | m68851 },
{"ploadw",	two(0xf000, 0x2010),	two(0xffc0, 0xfff0),	"T3&s",	m68030 | m68851 },
{"ploadw",	two(0xf000, 0x2008),	two(0xffc0, 0xfff8),	"D3&s",	m68030 | m68851 },
{"ploadw",	two(0xf000, 0x2000),	two(0xffc0, 0xfffe),	"f3&s",	m68030 | m68851 },

/* TC, CRP, DRP, SRP, CAL, VAL, SCC, AC */
{"pmove",	two(0xf000, 0x4000),	two(0xffc0, 0xe3ff),	"*sP8",	m68030 | m68851 },
{"pmove",	two(0xf000, 0x4200),	two(0xffc0, 0xe3ff),	"P8%s",	m68030 | m68851 },
{"pmove",	two(0xf000, 0x4000),	two(0xffc0, 0xe3ff),	"|sW8",	m68030 | m68851 },
{"pmove",	two(0xf000, 0x4200),	two(0xffc0, 0xe3ff),	"W8~s",	m68030 | m68851 },

/* BADx, BACx */
{"pmove",	two(0xf000, 0x6200),	two(0xffc0, 0xe3e3),	"*sX3",	m68030 | m68851 },
{"pmove",	two(0xf000, 0x6000),	two(0xffc0, 0xe3e3),	"X3%s",	m68030 | m68851 },

/* PSR, PCSR */
/* {"pmove",	two(0xf000, 0x6100),	two(oxffc0, oxffff),	"*sZ8",	m68030 | m68851 }, */
{"pmove",	two(0xf000, 0x6000),	two(0xffc0, 0xffff),	"*sY8",	m68030 | m68851 },
{"pmove",	two(0xf000, 0x6200),	two(0xffc0, 0xffff),	"Y8%s",	m68030 | m68851 },
{"pmove",	two(0xf000, 0x6600),	two(0xffc0, 0xffff),	"Z8%s",	m68030 | m68851 },

{"prestore",	one(0xf140),		one(0xffc0),		"&s", m68851 },
{"prestore",	one(0xf158),		one(0xfff8),		"+s", m68851 },
#ifndef MOTOROLA_SYNTAX
/* The two identical psave instructions seems dubious. FIXME */
{"psave",	one(0xf100),		one(0xffc0),		"&s", m68851 },
{"psave",	one(0xf100),		one(0xffc0),		"+s", m68851 },
#endif

{"psac",	two(0xf040, 0x0007),	two(0xffc0, 0xffff),	"@s", m68851 },
{"psas",	two(0xf040, 0x0006),	two(0xffc0, 0xffff),	"@s", m68851 },
{"psbc",	two(0xf040, 0x0001),	two(0xffc0, 0xffff),	"@s", m68851 },
{"psbs",	two(0xf040, 0x0000),	two(0xffc0, 0xffff),	"@s", m68851 },
{"pscc",	two(0xf040, 0x000f),	two(0xffc0, 0xffff),	"@s", m68851 },
{"pscs",	two(0xf040, 0x000e),	two(0xffc0, 0xffff),	"@s", m68851 },
{"psgc",	two(0xf040, 0x000d),	two(0xffc0, 0xffff),	"@s", m68851 },
{"psgs",	two(0xf040, 0x000c),	two(0xffc0, 0xffff),	"@s", m68851 },
{"psic",	two(0xf040, 0x000b),	two(0xffc0, 0xffff),	"@s", m68851 },
{"psis",	two(0xf040, 0x000a),	two(0xffc0, 0xffff),	"@s", m68851 },
{"pslc",	two(0xf040, 0x0003),	two(0xffc0, 0xffff),	"@s", m68851 },
{"psls",	two(0xf040, 0x0002),	two(0xffc0, 0xffff),	"@s", m68851 },
{"pssc",	two(0xf040, 0x0005),	two(0xffc0, 0xffff),	"@s", m68851 },
{"psss",	two(0xf040, 0x0004),	two(0xffc0, 0xffff),	"@s", m68851 },
{"pswc",	two(0xf040, 0x0009),	two(0xffc0, 0xffff),	"@s", m68851 },
{"psws",	two(0xf040, 0x0008),	two(0xffc0, 0xffff),	"@s", m68851 },

{"ptestr",	two(0xf000, 0x8210),	two(0xffc0, 0xe3f0),	"T3&sQ8",	m68030 | m68851 },
{"ptestr",	two(0xf000, 0x8310),	two(0xffc0, 0xe310),	"T3&sQ8A9",	m68030 | m68851 },
{"ptestr",	two(0xf000, 0x8208),	two(0xffc0, 0xe3f8),	"D3&sQ8",	m68030 | m68851 },
{"ptestr",	two(0xf000, 0x8308),	two(0xffc0, 0xe318),	"D3&sQ8A9",	m68030 | m68851 },
{"ptestr",	two(0xf000, 0x8200),	two(0xffc0, 0xe3fe),	"f3&sQ8",	m68030 | m68851 },
{"ptestr",	two(0xf000, 0x8300),	two(0xffc0, 0xe31e),	"f3&sQ8A9",	m68030 | m68851 },

{"ptestr",	one(0xf568),		one(0xfff8),		"as",		m68040 },

{"ptestw",	two(0xf000, 0x8010),	two(0xffc0, 0xe3f0),	"T3&sQ8",	m68030 | m68851 },
{"ptestw",	two(0xf000, 0x8110),	two(0xffc0, 0xe310),	"T3&sQ8A9",	m68030 | m68851 },
{"ptestw",	two(0xf000, 0x8008),	two(0xffc0, 0xe3f8),	"D3&sQ8",	m68030 | m68851 },
{"ptestw",	two(0xf000, 0x8108),	two(0xffc0, 0xe318),	"D3&sQ8A9",	m68030 | m68851 },
{"ptestw",	two(0xf000, 0x8000),	two(0xffc0, 0xe3fe),	"f3&sQ8",	m68030 | m68851 },
{"ptestw",	two(0xf000, 0x8100),	two(0xffc0, 0xe31e),	"f3&sQ8A9",	m68030 | m68851 },

{"ptestw",	one(0xf548),		one(0xfff8),		"as",		m68040 },

{OP2(ptrapac,w),	two(0xf07a, 0x0007),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapac,l),	two(0xf07b, 0x0007),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapac",	two(0xf07c, 0x0007),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapas,w),	two(0xf07a, 0x0006),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapas,l),	two(0xf07b, 0x0006),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapas",	two(0xf07c, 0x0006),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapbc,w),	two(0xf07a, 0x0001),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapbc,l),	two(0xf07b, 0x0001),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapbc",	two(0xf07c, 0x0001),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapbs,w),	two(0xf07a, 0x0000),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapbs,l),	two(0xf07b, 0x0000),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapbs",	two(0xf07c, 0x0000),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapcc,w),	two(0xf07a, 0x000f),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapcc,l),	two(0xf07b, 0x000f),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapcc",	two(0xf07c, 0x000f),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapcs,w),	two(0xf07a, 0x000e),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapcs,l),	two(0xf07b, 0x000e),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapcs",	two(0xf07c, 0x000e),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapgc,w),	two(0xf07a, 0x000d),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapgc,l),	two(0xf07b, 0x000d),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapgc",	two(0xf07c, 0x000d),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapgs,w),	two(0xf07a, 0x000c),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapgs,l),	two(0xf07b, 0x000c),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapgs",	two(0xf07c, 0x000c),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapic,w),	two(0xf07a, 0x000b),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapic,l),	two(0xf07b, 0x000b),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapic",	two(0xf07c, 0x000b),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapis,w),	two(0xf07a, 0x000a),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapis,l),	two(0xf07b, 0x000a),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapis",	two(0xf07c, 0x000a),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptraplc,w),	two(0xf07a, 0x0003),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptraplc,l),	two(0xf07b, 0x0003),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptraplc",	two(0xf07c, 0x0003),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapls,w),	two(0xf07a, 0x0002),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapls,l),	two(0xf07b, 0x0002),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapls",	two(0xf07c, 0x0002),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapsc,w),	two(0xf07a, 0x0005),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapsc,l),	two(0xf07b, 0x0005),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapsc",	two(0xf07c, 0x0005),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapss,w),	two(0xf07a, 0x0004),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapss,l),	two(0xf07b, 0x0004),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapss",	two(0xf07c, 0x0004),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapwc,w),	two(0xf07a, 0x0009),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapwc,l),	two(0xf07b, 0x0009),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapwc",	two(0xf07c, 0x0009),	two(0xffff, 0xffff),	"",   m68851 },

{OP2(ptrapws,w),	two(0xf07a, 0x0008),	two(0xffff, 0xffff),	"#w", m68851 },
{OP2(ptrapws,l),	two(0xf07b, 0x0008),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapws",	two(0xf07c, 0x0008),	two(0xffff, 0xffff),	"",   m68851 },

{"pvalid",	two(0xf000, 0x2800),	two(0xffc0, 0xffff),	"Vs&s", m68851 },
{"pvalid",	two(0xf000, 0x2c00),	two(0xffc0, 0xfff8),	"A3&s", m68851 },

#endif /* NO_68851 */

{"reset",	one(0047160),		one(0177777),		"", m68000up },

{OP2(rol,b),	one(0160430),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{OP2(rol,b),	one(0160470),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{OP2(rol,l),	one(0160630),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{OP2(rol,l),	one(0160670),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{OP2(rol,w),	one(0160530),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{OP2(rol,w),	one(0160570),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{OP2(rol,w),	one(0163700),		one(0177700),		"~s",   m68000up },	/* Rotate memory */
{OP2(ror,b),	one(0160030),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{OP2(ror,b),	one(0160070),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{OP2(ror,l),	one(0160230),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{OP2(ror,l),	one(0160270),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{OP2(ror,w),	one(0160130),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{OP2(ror,w),	one(0160170),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{OP2(ror,w),	one(0163300),		one(0177700),		"~s",   m68000up },	/* Rotate memory */

{OP2(roxl,b),	one(0160420),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{OP2(roxl,b),	one(0160460),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{OP2(roxl,l),	one(0160620),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{OP2(roxl,l),	one(0160660),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{OP2(roxl,w),	one(0160520),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{OP2(roxl,w),	one(0160560),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{OP2(roxl,w),	one(0162700),		one(0177700),		"~s",   m68000up },	/* Rotate memory */
{OP2(roxr,b),	one(0160020),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{OP2(roxr,b),	one(0160060),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{OP2(roxr,l),	one(0160220),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{OP2(roxr,l),	one(0160260),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{OP2(roxr,w),	one(0160120),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{OP2(roxr,w),	one(0160160),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{OP2(roxr,w),	one(0162300),		one(0177700),		"~s",   m68000up },	/* Rotate memory */

{"rtd",		one(0047164),		one(0177777),		"#w", m68010up },
{"rte",		one(0047163),		one(0177777),		"",   m68000up },
{"rtm",		one(0003300),		one(0177760),		"Rs", m68020 },
{"rtr",		one(0047167),		one(0177777),		"",   m68000up },
{"rts",		one(0047165),		one(0177777),		"",   m68000up },

{"sbcd",	one(0100400),		one(0170770),		"DsDd", m68000up },
{"sbcd",	one(0100410),		one(0170770),		"-s-d", m68000up },

{"scc",		one(0052300),		one(0177700),		"$s", m68000up },
{"scs",		one(0052700),		one(0177700),		"$s", m68000up },
{"seq",		one(0053700),		one(0177700),		"$s", m68000up },
{"sf",		one(0050700),		one(0177700),		"$s", m68000up },
{"sge",		one(0056300),		one(0177700),		"$s", m68000up },
{"sfge",	one(0056300),		one(0177700),		"$s", m68000up },
{"sgt",		one(0057300),		one(0177700),		"$s", m68000up },
{"sfgt",	one(0057300),		one(0177700),		"$s", m68000up },
{"shi",		one(0051300),		one(0177700),		"$s", m68000up },
{"sle",		one(0057700),		one(0177700),		"$s", m68000up },
{"sfle",	one(0057700),		one(0177700),		"$s", m68000up },
{"sls",		one(0051700),		one(0177700),		"$s", m68000up },
{"slt",		one(0056700),		one(0177700),		"$s", m68000up },
{"sflt",	one(0056700),		one(0177700),		"$s", m68000up },
{"smi",		one(0055700),		one(0177700),		"$s", m68000up },
{"sne",		one(0053300),		one(0177700),		"$s", m68000up },
{"sfneq",	one(0053300),		one(0177700),		"$s", m68000up },
{"spl",		one(0055300),		one(0177700),		"$s", m68000up },
{"st",		one(0050300),		one(0177700),		"$s", m68000up },
{"svc",		one(0054300),		one(0177700),		"$s", m68000up },
{"svs",		one(0054700),		one(0177700),		"$s", m68000up },

{"stop",	one(0047162),		one(0177777),		"#w", m68000up },

{OP2(suba,l),	one(0110700),		one(0170700),		"*lAd", m68000up },
{OP2(suba,w),	one(0110300),		one(0170700),		"*wAd", m68000up },
{OP2(sub,b),	one(0050400),		one(0170700),		"Qd%s", m68000up },	/* subq written as sub */
{OP2(sub,b),	one(0002000),		one(0177700),		"#b$s", m68000up },	/* subi written as sub */
{OP2(sub,b),	one(0110000),		one(0170700),		";bDd", m68000up },	/* subb ? ?,	Dd */
{OP2(sub,b),	one(0110400),		one(0170700),		"Dd~s", m68000up },	/* subb Dd,	? ? */

{OP2(subi,b),	one(0002000),		one(0177700),		"#b$s", m68000up },
{OP2(subi,l),	one(0002200),		one(0177700),		"#l$s", m68000up },
{OP2(subi,w),	one(0002100),		one(0177700),		"#w$s", m68000up },
{OP2(sub,l),	one(0050600),		one(0170700),		"Qd%s", m68000up },
{OP2(sub,l),	one(0002200),		one(0177700),		"#l$s", m68000up },
{OP2(sub,l),	one(0110700),		one(0170700),		"*lAd", m68000up },
{OP2(sub,l),	one(0110200),		one(0170700),		"*lDd", m68000up },
{OP2(sub,l),	one(0110600),		one(0170700),		"Dd~s", m68000up },
{OP2(subq,b),	one(0050400),		one(0170700),		"Qd%s", m68000up },
#ifdef MOTOROLA_SYNTAX 
{OP2(subq,b),	one(0002000),		one(0177700),		"#b$s", m68000up }, /* FIXNIEL: sub instead of subq because too many frags */
#endif
{OP2(subq,l),	one(0050600),		one(0170700),		"Qd%s", m68000up },
#ifdef MOTOROLA_SYNTAX 
{OP2(subq,l),	one(0002200),		one(0177700),		"#l$s", m68000up }, /* FIXNIEL: sub instead of subq because too many frags */
#endif
{OP2(subq,w),	one(0050500),		one(0170700),		"Qd%s", m68000up },
#ifdef MOTOROLA_SYNTAX 
{OP2(subq,w),	one(0002100),		one(0177700),		"#w$s", m68000up }, /* FIXNIEL: sub instead of subq because too many frags */
#endif
{OP2(sub,w),	one(0050500),		one(0170700),		"Qd%s", m68000up },
{OP2(sub,w),	one(0002100),		one(0177700),		"#w$s", m68000up },
{OP2(sub,w),	one(0110100),		one(0170700),		"*wDd", m68000up },
{OP2(sub,w),	one(0110300),		one(0170700),		"*wAd", m68000up },	/* suba written as sub */
{OP2(sub,w),	one(0110500),		one(0170700),		"Dd~s", m68000up },
#ifdef ALLOW_DEFAULT_SIZES
{"subi",	one(0002200),		one(0177700),		"#l$s", m68000up },
{"suba",	one(0110700),		one(0170700),		"*lAd", m68000up },
{"sub",	one(0050500),		one(0170700),		"Qd%s", m68000up },
{"sub",	one(0002100),		one(0177700),		"#w$s", m68000up },
{"sub",	one(0110100),		one(0170700),		"*wDd", m68000up },
{"sub",	one(0110300),		one(0170700),		"*wAd", m68000up },	/* suba written as sub */
{"sub",	one(0110500),		one(0170700),		"Dd~s", m68000up },
#endif

{OP2(subx,b),	one(0110400),		one(0170770),		"DsDd", m68000up },	/* subxb Ds,	Dd */
{OP2(subx,b),	one(0110410),		one(0170770),		"-s-d", m68000up },	/* subxb -(As),	-(Ad) */
{OP2(subx,l),	one(0110600),		one(0170770),		"DsDd", m68000up },
{OP2(subx,l),	one(0110610),		one(0170770),		"-s-d", m68000up },
{OP2(subx,w),	one(0110500),		one(0170770),		"DsDd", m68000up },
{OP2(subx,w),	one(0110510),		one(0170770),		"-s-d", m68000up },
#ifdef ALLOW_DEFAULT_SIZES
{"subx",	one(0110500),		one(0170770),		"DsDd", m68000up },
{"subx",	one(0110510),		one(0170770),		"-s-d", m68000up },
#endif

{"swap",	one(0044100),		one(0177770),		"Ds", m68000up },
#ifdef MOTOROLA_SYNTAX
{OP2(swap,w),	one(0044100),		one(0177770),		"Ds", m68000up },
#endif
	
{"tas",		one(0045300),		one(0177700),		"$s", m68000up },

#ifdef MOTOROLA_SYNTAX
{"tpcc",	one(0047100),		one(0177760),		"Ts", m68000up },
#endif

{"trap",	one(0047100),		one(0177760),		"Ts", m68000up },


#ifdef MOTOROLA_SYNTAX
{"tcc",		one(0052374),		one(0177777),		"", m68020up },
#endif

{"trapcc",	one(0052374),		one(0177777),		"", m68020up },
{"trapcs",	one(0052774),		one(0177777),		"", m68020up },
{"trapeq",	one(0053774),		one(0177777),		"", m68020up },
{"trapf",	one(0050774),		one(0177777),		"", m68020up },
{"trapge",	one(0056374),		one(0177777),		"", m68020up },
{"trapgt",	one(0057374),		one(0177777),		"", m68020up },
{"traphi",	one(0051374),		one(0177777),		"", m68020up },
{"traple",	one(0057774),		one(0177777),		"", m68020up },
{"trapls",	one(0051774),		one(0177777),		"", m68020up },
{"traplt",	one(0056774),		one(0177777),		"", m68020up },
{"trapmi",	one(0055774),		one(0177777),		"", m68020up },
{"trapne",	one(0053374),		one(0177777),		"", m68020up },
{"trappl",	one(0055374),		one(0177777),		"", m68020up },
{"trapt",	one(0050374),		one(0177777),		"", m68020up },
{"trapvc",	one(0054374),		one(0177777),		"", m68020up },
{"trapvs",	one(0054774),		one(0177777),		"", m68020up },

{"trapcc.w",	one(0052372),		one(0177777),	V("","#w"), m68020up },
{"trapcs.w",	one(0052772),		one(0177777),	V("","#w"), m68020up },
{"trapeq.w",	one(0053772),		one(0177777),	V("","#w"), m68020up },
{"trapf.w",	one(0050772),		one(0177777),	V("","#w"), m68020up },
{"trapge.w",	one(0056372),		one(0177777),	V("","#w"), m68020up },
{"trapgt.w",	one(0057372),		one(0177777),	V("","#w"), m68020up },
{"traphi.w",	one(0051372),		one(0177777),	V("","#w"), m68020up },
{"traple.w",	one(0057772),		one(0177777),	V("","#w"), m68020up },
{"trapls.w",	one(0051772),		one(0177777),	V("","#w"), m68020up },
{"traplt.w",	one(0056772),		one(0177777),	V("","#w"), m68020up },
{"trapmi.w",	one(0055772),		one(0177777),	V("","#w"), m68020up },
{"trapne.w",	one(0053372),		one(0177777),	V("","#w"), m68020up },
{"trappl.w",	one(0055372),		one(0177777),	V("","#w"), m68020up },
{"trapt.w",	one(0050372),		one(0177777),	V("","#w"), m68020up },
{"trapvc.w",	one(0054372),		one(0177777),	V("","#w"), m68020up },
{"trapvs.w",	one(0054772),		one(0177777),	V("","#w"), m68020up },

{"trapcc.l",	one(0052373),		one(0177777),	V("","#l"), m68020up },
{"trapcs.l",	one(0052773),		one(0177777),	V("","#l"), m68020up },
{"trapeq.l",	one(0053773),		one(0177777),	V("","#l"), m68020up },
{"trapf.l",	one(0050773),		one(0177777),	V("","#l"), m68020up },
{"trapge.l",	one(0056373),		one(0177777),	V("","#l"), m68020up },
{"trapgt.l",	one(0057373),		one(0177777),	V("","#l"), m68020up },
{"traphi.l",	one(0051373),		one(0177777),	V("","#l"), m68020up },
{"traple.l",	one(0057773),		one(0177777),	V("","#l"), m68020up },
{"trapls.l",	one(0051773),		one(0177777),	V("","#l"), m68020up },
{"traplt.l",	one(0056773),		one(0177777),	V("","#l"), m68020up },
{"trapmi.l",	one(0055773),		one(0177777),	V("","#l"), m68020up },
{"trapne.l",	one(0053373),		one(0177777),	V("","#l"), m68020up },
{"trappl.l",	one(0055373),		one(0177777),	V("","#l"), m68020up },
{"trapt.l",	one(0050373),		one(0177777),	V("","#l"), m68020up },
{"trapvc.l",	one(0054373),		one(0177777),	V("","#l"), m68020up },
{"trapvs.l",	one(0054773),		one(0177777),	V("","#l"), m68020up },

{"trapv",	one(0047166),		one(0177777),		"", m68000up },


{OP2(tst,b),	one(0045000),		one(0177700),		";b", m68000up },
{OP2(tst,w),	one(0045100),		one(0177700),		"*w", m68000up },
{OP2(tst,l),	one(0045200),		one(0177700),		"*l", m68000up },
#ifdef ALLOW_DEFAULT_SIZES
{"tst",		one(0045200),		one(0177700),		"*l", m68000up },
#endif

{"unlk",	one(0047130),		one(0177770),		"As", m68000up },
{"unpk",	one(0100600),		one(0170770),		"DsDd#w", m68020up },
{"unpk",	one(0100610),		one(0170770),		"-s-d#w", m68020up },

/* Variable-sized branches */

{"jbsr",	one(0060400),		one(0177400),		"Bg", m68000up },
{"jbsr",	one(0047200),		one(0177700),		"!s", m68000up },
{"jra",		one(0060000),		one(0177400),		"Bg", m68000up },
{"jra",		one(0047300),		one(0177700),		"!s", m68000up },

#ifdef MOTOROLA_SYNTAX
{"jbra",	one(0060000),		one(0177400),		"Bg", m68000up },
{"jbra",	one(0047300),		one(0177700),		"!s", m68000up },
#endif

{"jhi",		one(0061000),		one(0177400),		"Bg", m68000up },
{"jls",		one(0061400),		one(0177400),		"Bg", m68000up },
{"jcc",		one(0062000),		one(0177400),		"Bg", m68000up },
{"jcs",		one(0062400),		one(0177400),		"Bg", m68000up },
{"jne",		one(0063000),		one(0177400),		"Bg", m68000up },
{"jeq",		one(0063400),		one(0177400),		"Bg", m68000up },
{"jvc",		one(0064000),		one(0177400),		"Bg", m68000up },
{"jvs",		one(0064400),		one(0177400),		"Bg", m68000up },
{"jpl",		one(0065000),		one(0177400),		"Bg", m68000up },
{"jmi",		one(0065400),		one(0177400),		"Bg", m68000up },
{"jge",		one(0066000),		one(0177400),		"Bg", m68000up },
{"jlt",		one(0066400),		one(0177400),		"Bg", m68000up },
{"jgt",		one(0067000),		one(0177400),		"Bg", m68000up },
{"jle",		one(0067400),		one(0177400),		"Bg", m68000up },


#ifdef MOTOROLA_SYNTAX
/* Aliases:  jbCC is the same as jCC */
{"jbhi",		one(0061000),		one(0177400),		"Bg", m68000up },
{"jbls",		one(0061400),		one(0177400),		"Bg", m68000up },
{"jbcc",		one(0062000),		one(0177400),		"Bg", m68000up },
{"jbcs",		one(0062400),		one(0177400),		"Bg", m68000up },
{"jbne",		one(0063000),		one(0177400),		"Bg", m68000up },
{"jbeq",		one(0063400),		one(0177400),		"Bg", m68000up },
{"jbvc",		one(0064000),		one(0177400),		"Bg", m68000up },
{"jbvs",		one(0064400),		one(0177400),		"Bg", m68000up },
{"jbpl",		one(0065000),		one(0177400),		"Bg", m68000up },
{"jbmi",		one(0065400),		one(0177400),		"Bg", m68000up },
{"jbge",		one(0066000),		one(0177400),		"Bg", m68000up },
{"jblt",		one(0066400),		one(0177400),		"Bg", m68000up },
{"jbgt",		one(0067000),		one(0177400),		"Bg", m68000up },
{"jble",		one(0067400),		one(0177400),		"Bg", m68000up },
#endif /* MOTOROLA_SYNTAX */

/* aliases */

#ifdef MOTOROLA_SYNTAX
{"move",	one(0030000),		one(0170000),		"*w$d", m68000up },
{"move",	one(0030100),		one(0170700),		"*wAd", m68000up },	/* mova,	written as mov */
{"move",	one(0040300),		one(0177700),		"Ss$s", m68000up },	/* Move from sr */
{"move",	one(0041300),		one(0177700),		"Cs$s", m68010up },	/* Move from ccr */
{"move",	one(0042300),		one(0177700),		";wCd", m68000up },	/* mov to ccr */
{"move",	one(0043300),		one(0177700),		";wSd", m68000up },	/* mov to sr */


{"move",	one(0020000),		one(0170000),		"*l$d", m68000up },
{"move",	one(0020100),		one(0170700),		"*lAd", m68000up },
{"move",	one(0070000),		one(0170400),		"MsDd", m68000up },	/* movq written as mov */
{"move",	one(0047140),		one(0177770),		"AsUd", m68000up },	/* mov to USP */
{"move",	one(0047150),		one(0177770),		"UdAs", m68000up },	/* mov from USP */
#endif /* MOTOROLA_SYNTAX */


{OP2(movq,l),	one(0070000),		one(0170400),		"MsDd", m68000up },
#ifdef MOTOROLA_SYNTAX
{OP2(movq,l),	one(0020000),		one(0170000),		"*l$d", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{OP2(movq,l),	one(0020100),		one(0170700),		"*lAd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{OP2(movq,l),	one(0047140),		one(0177770),		"AsUd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{OP2(movq,l),	one(0047150),		one(0177770),		"UdAs", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
#endif
{OP2(moveq,l),	one(0070000),		one(0170400),		"MsDd", m68000up },
#ifdef MOTOROLA_SYNTAX
{OP2(moveq,l),	one(0020000),		one(0170000),		"*l$d", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{OP2(moveq,l),	one(0020100),		one(0170700),		"*lAd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{OP2(moveq,l),	one(0047140),		one(0177770),		"AsUd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{OP2(moveq,l),	one(0047150),		one(0177770),		"UdAs", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
#endif
{OP2(mova,l),	one(0020100),		one(0170700),		"*lAd", m68000up },
{OP2(mova,w),	one(0030100),		one(0170700),		"*wAd", m68000up },
{OP2(mov,b),	one(0010000),		one(0170000),		";b$d", m68000up },	/* mov */

{OP2(mov,l),	one(0070000),		one(0170400),		"MsDd", m68000up },	/* movq written as mov */
{OP2(mov,l),	one(0020000),		one(0170000),		"*l$d", m68000up },
{OP2(mov,l),	one(0020100),		one(0170700),		"*lAd", m68000up },
{OP2(mov,l),	one(0047140),		one(0177770),		"AsUd", m68000up },	/* mov to USP */
{OP2(mov,l),	one(0047150),		one(0177770),		"UdAs", m68000up },	/* mov from USP */

{"movc",	one(0047173),		one(0177777),		"R1Jj", m68010up },
{"movc",	one(0047173),		one(0177777),		"R1#j", m68010up },
{"movc",	one(0047172),		one(0177777),		"JjR1", m68010up },
{"movc",	one(0047172),		one(0177777),		"#jR1", m68010up },
{OP2(movm,l),	one(0044300),		one(0177700),		"#w&s", m68000up },	/* movm reg to mem. */
{OP2(movm,l),	one(0044340),		one(0177770),		"#w-s", m68000up },	/* movm reg to autodecrement. */
{"movml",	one(0046300),		one(0177700),		"!s#w", m68000up },	/* movm mem to reg. */
{"movml",	one(0046330),		one(0177770),		"+s#w", m68000up },	/* movm autoinc to reg. */
{"movml",	one(0044300),		one(0177700),		"Lw&s", m68000up },	/* movm reg to mem. */
{"movml",	one(0044340),		one(0177770),		"lw-s", m68000up },	/* movm reg to autodecrement. */
{"movml",	one(0046300),		one(0177700),		"!sLw", m68000up },	/* movm mem to reg. */
{"movml",	one(0046330),		one(0177770),		"+sLw", m68000up },	/* movm autoinc to reg. */
{OP2(movm,w),	one(0044200),		one(0177700),		"#w&s", m68000up },	/* movm reg to mem. */
{OP2(movm,w),	one(0044240),		one(0177770),		"#w-s", m68000up },	/* movm reg to autodecrement. */
{OP2(movm,w),	one(0046200),		one(0177700),		"!s#w", m68000up },	/* movm mem to reg. */
{OP2(movm,w),	one(0046230),		one(0177770),		"+s#w", m68000up },	/* movm autoinc to reg. */
{OP2(movm,w),	one(0044200),		one(0177700),		"Lw&s", m68000up },	/* movm reg to mem. */
{OP2(movm,w),	one(0044240),		one(0177770),		"lw-s", m68000up },	/* movm reg to autodecrement. */
{OP2(movm,w),	one(0046200),		one(0177700),		"!sLw", m68000up },	/* movm mem to reg. */
{OP2(movm,w),	one(0046230),		one(0177770),		"+sLw", m68000up },	/* movm autoinc to reg. */
{OP2(movp,l),	one(0000510),		one(0170770),		"dsDd", m68000up },	/* memory to register */
{OP2(movp,l),	one(0000710),		one(0170770),		"Ddds", m68000up },	/* register to memory */
{OP2(movp,w),	one(0000410),		one(0170770),		"dsDd", m68000up },	/* memory to register */
{OP2(movp,w),	one(0000610),		one(0170770),		"Ddds", m68000up },	/* register to memory */
{"movq",	one(0070000),		one(0170400),		"MsDd", m68000up },

#ifdef MOTOROLA_SYNTAX
{"movq",	one(0020000),		one(0170000),		"*l$d", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"movq",	one(0020100),		one(0170700),		"*lAd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"movq",	one(0047140),		one(0177770),		"AsUd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"movq",	one(0047150),		one(0177770),		"UdAs", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
#endif

{OP2(mov,w),	one(0030000),		one(0170000),		"*w$d", m68000up },
{OP2(mov,w),	one(0030100),		one(0170700),		"*wAd", m68000up },	/* mova,	written as mov */
{OP2(mov,w),	one(0040300),		one(0177700),		"Ss$s", m68000up },	/* Move from sr */
{OP2(mov,w),	one(0041300),		one(0177700),		"Cs$s", m68010up },	/* Move from ccr */
{OP2(mov,w),	one(0042300),		one(0177700),		";wCd", m68000up },	/* mov to ccr */
{OP2(mov,w),	one(0043300),		one(0177700),		";wSd", m68000up },	/* mov to sr */
{OP2(movs,b),	two(0007000, 0),	two(0177700, 07777),	"~sR1", m68010up },
{OP2(movs,b),	two(0007000, 04000),	two(0177700, 07777),	"R1~s", m68010up },
{OP2(movs,l),	two(0007200, 0),	two(0177700, 07777),	"~sR1", m68010up },
{OP2(movs,l),	two(0007200, 04000),	two(0177700, 07777),	"R1~s", m68010up },
{OP2(movs,w),	two(0007100, 0),	two(0177700, 07777),	"~sR1", m68010up },
{OP2(movs,w),	two(0007100, 04000),	two(0177700, 07777),	"R1~s", m68010up },

#ifdef MOTOROLA_SYNTAX
{OP2(moveq,l),	one(0070000),		one(0170400),		"MsDd", m68000up },
{OP2(moveq,l),	one(0070000),		one(0170400),		"MsDd", m68000up },
{OP2(movea,l),	one(0020100),		one(0170700),		"*lAd", m68000up },
{OP2(movea,w),	one(0030100),		one(0170700),		"*wAd", m68000up },
{OP2(move,b),	one(0010000),		one(0170000),		";b$d", m68000up },	/* mov */
{OP2(move,l),	one(0070000),		one(0170400),		"MsDd", m68000up },	/* movq written as mov */
{OP2(move,l),	one(0020000),		one(0170000),		"*l$d", m68000up },
{OP2(move,l),	one(0020100),		one(0170700),		"*lAd", m68000up },
{OP2(move,l),	one(0047140),		one(0177770),		"AsUd", m68000up },	/* mov to USP */
{OP2(move,l),	one(0047150),		one(0177770),		"UdAs", m68000up },	/* mov from USP */
{"movec",	one(0047173),		one(0177777),		"R1Jj", m68010up },
{"movec",	one(0047173),		one(0177777),		"R1#j", m68010up },
{"movec",	one(0047172),		one(0177777),		"JjR1", m68010up },
{"movec",	one(0047172),		one(0177777),		"#jR1", m68010up },
{OP2(movec,l),	one(0047173),		one(0177777),		"R1Jj", m68010up },
{OP2(movec,l),	one(0047173),		one(0177777),		"R1#j", m68010up },
{OP2(movec,l),	one(0047172),		one(0177777),		"JjR1", m68010up },
{OP2(movec,l),	one(0047172),		one(0177777),		"#jR1", m68010up },

{"moveq",	one(0070000),		one(0170400),		"MsDd", m68000up },
{"moveq",	one(0020000),		one(0170000),		"*l$d", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"moveq",	one(0020100),		one(0170700),		"*lAd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"moveq",	one(0047140),		one(0177770),		"AsUd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"moveq",	one(0047150),		one(0177770),		"UdAs", m68000up }, /* FIXNIEL: move instead of movw because too many frags */

#endif /* MOTOROLA_SYNTAX */
 
};

int numopcodes=sizeof(m68k_opcodes)/sizeof(m68k_opcodes[0]);

struct m68k_opcode *endop = m68k_opcodes+sizeof(m68k_opcodes)/sizeof(m68k_opcodes[0]);
#endif
/*
 * Local Variables:
 * fill-column: 131
 * End:
 */


/* end of m68k-opcode.h */
