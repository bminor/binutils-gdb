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
{"adda.l",	one(0150700),		one(0170700), "*lAd", m68000up },
{"adda.w",	one(0150300),		one(0170700), "*wAd", m68000up },
{"addi.b",	one(0003000),		one(0177700), "#b$b", m68000up },


{"addi",	one(0050200),		one(0170700), "Qd%l", m68000up },
{"addi",	one(0003200),		one(0177700), "#l$l", m68000up },	/* addi written as add */
{"addq",	one(0050100),		one(0170700), "Qd%w", m68000up },


{"addi.l",	one(0003200),		one(0177700), "#l$l", m68000up },
{"addi.w",	one(0003100),		one(0177700), "#w$w", m68000up },
{"addq.b",	one(0050000),		one(0170700), "Qd$b", m68000up },
{"addq.b",	one(0003000),		one(0177700), "#b$b", m68000up }, /* FIXNIEL: addi instead of addq because to many frags */
{"addq.l",	one(0050200),		one(0170700), "Qd%l", m68000up },
{"addq.l",	one(0003200),		one(0177700), "#l$l", m68000up }, /* FIXNIEL: addi instead of addq because to many frags */
{"addq.w",	one(0050100),		one(0170700), "Qd%w", m68000up },
{"addq.w",	one(0003100),		one(0177700), "#w$w", m68000up }, /* FIXNIEL: addi instead of addq because to many frags */

{"add.b",	one(0050000),		one(0170700), "Qd$b", m68000up },	/* addq written as add */
{"add.b",	one(0003000),		one(0177700), "#b$b", m68000up },	/* addi written as add */
{"add.b",	one(0150000),		one(0170700), ";bDd", m68000up },	/* addb <ea>,	Dd */
{"add.b",	one(0150400),		one(0170700), "Dd~b", m68000up },	/* add.b Dd,	<ea> */

{"add.w",	one(0150300),		one(0170700), "*wAd", m68000up },	/* adda written as add */
{"add.w",	one(0050100),		one(0170700), "Qd%w", m68000up },	/* addq written as add */
{"add.w",	one(0003100),		one(0177700), "#w$w", m68000up },	/* addi written as add */
{"add.w",	one(0150100),		one(0170700), "*wDd", m68000up },	/* addw <ea>,	Dd */
{"add.w",	one(0150500),		one(0170700), "Dd~w", m68000up },	/* addw Dd,	<ea> */

{"add.l",	one(0050200),		one(0170700), "Qd%l", m68000up },	/* addq written as add */
{"add.l",	one(0003200),		one(0177700), "#l$l", m68000up },	/* addi written as add */
{"add.l",	one(0150700),		one(0170700), "*lAd", m68000up },	/* adda written as add */
{"add.l",	one(0150200),		one(0170700), "*lDd", m68000up },	/* addl <ea>,	Dd */
{"add.l",	one(0150600),		one(0170700), "Dd~l", m68000up },	/* addl Dd,	<ea> */

{"addx.b",	one(0150400),		one(0170770), "DsDd", m68000up },
{"addx.b",	one(0150410),		one(0170770), "-s-d", m68000up },
{"addx.l",	one(0150600),		one(0170770), "DsDd", m68000up },
{"addx.l",	one(0150610),		one(0170770), "-s-d", m68000up },
{"addx.w",	one(0150500),		one(0170770), "DsDd", m68000up },
{"addx.w",	one(0150510),		one(0170770), "-s-d", m68000up },
{"andi",	one(0001200),		one(0177700), "#l$l", m68000up },
{"andi",	one(0001174),		one(0177777), "#wSw", m68000up },	/* andi to sr */
{"andi",	one(0001074),		one(0177777), "#bCb", m68000up },	/* andi to ccr */
{"andi.b",	one(0001000),		one(0177700), "#b$b", m68000up },
{"andi.b",	one(0001074),		one(0177777), "#bCb", m68000up },	/* andi to ccr */
{"andi.w",	one(0001100),		one(0177700), "#w$w", m68000up },
{"andi.w",	one(0001174),		one(0177777), "#wSw", m68000up },	/* andi to sr */
{"andi.l",	one(0001200),		one(0177700), "#l$l", m68000up },
{"and.b",	one(0001000),		one(0177700), "#b$b", m68000up },	/* andi written as or */
{"and.b",	one(0001074),		one(0177777), "#bCb", m68000up },	/* andi to ccr */
{"and.b",	one(0140000),		one(0170700), ";bDd", m68000up },	/* memory to register */
{"and.b",	one(0140400),		one(0170700), "Dd~b", m68000up },	/* register to memory */
{"and.w",	one(0001100),		one(0177700), "#w$w", m68000up },	/* andi written as or */
{"and.w",	one(0001174),		one(0177777), "#wSw", m68000up },	/* andi to sr */
{"and.w",	one(0140100),		one(0170700), ";wDd", m68000up },	/* memory to register */
{"and.w",	one(0140500),		one(0170700), "Dd~w", m68000up },	/* register to memory */
{"and.l",	one(0001200),		one(0177700), "#l$l", m68000up },	/* andi written as or */
{"and.l",	one(0140200),		one(0170700), ";lDd", m68000up },	/* memory to register */
{"and.l",	one(0140600),		one(0170700), "Dd~l", m68000up },	/* register to memory */

{"asl.b",	one(0160400),		one(0170770), "QdDs", m68000up },
{"asl.b",	one(0160440),		one(0170770), "DdDs", m68000up },
{"asl.l",	one(0160600),		one(0170770), "QdDs", m68000up },
{"asl.l",	one(0160640),		one(0170770), "DdDs", m68000up },
{"asl.w",	one(0160500),		one(0170770), "QdDs", m68000up },
{"asl.w",	one(0160540),		one(0170770), "DdDs", m68000up },
{"asl.w",	one(0160700),		one(0177700), "~s",   m68000up },	/* Shift memory */
{"asr.b",	one(0160000),		one(0170770), "QdDs", m68000up },
{"asr.b",	one(0160040),		one(0170770), "DdDs", m68000up },
{"asr.l",	one(0160200),		one(0170770), "QdDs", m68000up },
{"asr.l",	one(0160240),		one(0170770), "DdDs", m68000up },
{"asr.w",	one(0160100),		one(0170770), "QdDs", m68000up },
{"asr.w",	one(0160140),		one(0170770), "DdDs", m68000up },
{"asr.w",	one(0160300),		one(0177700), "~s",   m68000up },	/* Shift memory */

/* Customers want bhi and friends to be variable sized - stolen from jhi */
{"bhi",		one(0061000),		one(0177400),		"Bg", m68000up },
{"bls",		one(0061400),		one(0177400),		"Bg", m68000up },
{"bcc",		one(0062000),		one(0177400),		"Bg", m68000up },
{"bcs",		one(0062400),		one(0177400),		"Bg", m68000up },
{"bne",		one(0063000),		one(0177400),		"Bg", m68000up },
{"beq",		one(0063400),		one(0177400),		"Bg", m68000up },
{"bvc",		one(0064000),		one(0177400),		"Bg", m68000up },
{"bvs",		one(0064400),		one(0177400),		"Bg", m68000up },
{"bpl",		one(0065000),		one(0177400),		"Bg", m68000up },
{"bmi",		one(0065400),		one(0177400),		"Bg", m68000up },
{"bge",		one(0066000),		one(0177400),		"Bg", m68000up },
{"blt",		one(0066400),		one(0177400),		"Bg", m68000up },
{"bgt",		one(0067000),		one(0177400),		"Bg", m68000up },
{"ble",		one(0067400),		one(0177400),		"Bg", m68000up },

{"bsr",	one(0060400),		one(0177400),		"Bg", m68000up },
{"bsr",	one(0047200),		one(0177700),		"!s", m68000up },
{"bra",		one(0060000),		one(0177400),		"Bg", m68000up },
{"bra",		one(0047300),		one(0177700),		"!s", m68000up },

#if 0
/* Fixed-size branches with 16-bit offsets */
{"bhi",		one(0061000),		one(0177777), "BW", m68000up },
{"bls",		one(0061400),		one(0177777), "BW", m68000up },
{"bcc",		one(0062000),		one(0177777), "BW", m68000up },
{"jfnlt",	one(0062000),		one(0177777), "BW", m68000up }, /* apparently a sun alias */
{"bcs",		one(0062400),		one(0177777), "BW", m68000up },
{"bne",		one(0063000),		one(0177777), "BW", m68000up },
{"beq",		one(0063400),		one(0177777), "BW", m68000up },
{"bvc",		one(0064000),		one(0177777), "BW", m68000up },
{"bvs",		one(0064400),		one(0177777), "BW", m68000up },
{"bpl",		one(0065000),		one(0177777), "BW", m68000up },
{"bmi",		one(0065400),		one(0177777), "BW", m68000up },
{"bge",		one(0066000),		one(0177777), "BW", m68000up },
{"blt",		one(0066400),		one(0177777), "BW", m68000up },
{"bgt",		one(0067000),		one(0177777), "BW", m68000up },
{"ble",		one(0067400),		one(0177777), "BW", m68000up },
{"jfngt",	one(0067400),		one(0177777), "BW", m68000up }, /* apparently a sun alias */
{"bra",		one(0060000),		one(0177777), "BW", m68000up },
{"bsr",		one(0060400),		one(0177777), "BW", m68000up },
#endif
/* Fixed-size branches with short (byte) offsets */

{"bhi.s",	one(0061000),		one(0177400), "BB", m68000up },
{"bls.s",	one(0061400),		one(0177400), "BB", m68000up },
{"bcc.s",	one(0062000),		one(0177400), "BB", m68000up },
{"bcs.s",	one(0062400),		one(0177400), "BB", m68000up },
{"bne.s",	one(0063000),		one(0177400), "BB", m68000up },
{"beq.s",	one(0063400),		one(0177400), "BB", m68000up },
{"jfeq",	one(0063400),		one(0177400), "BB", m68000up }, /* apparently a sun alias */
{"bvc.s",	one(0064000),		one(0177400), "BB", m68000up },
{"bvs.s",	one(0064400),		one(0177400), "BB", m68000up },
{"bpl.s",	one(0065000),		one(0177400), "BB", m68000up },
{"bmi.s",	one(0065400),		one(0177400), "BB", m68000up },
{"bge.s",	one(0066000),		one(0177400), "BB", m68000up },
{"blt.s",	one(0066400),		one(0177400), "BB", m68000up },
{"bgt.s",	one(0067000),		one(0177400), "BB", m68000up },
{"ble.s",	one(0067400),		one(0177400), "BB", m68000up },
{"bra.s",	one(0060000),		one(0177400), "BB", m68000up },
{"bsr.s",	one(0060400),		one(0177400), "BB", m68000up },
{"bhi.s",	one(0061000),		one(0177400), "BB", m68000up },
{"bls.s",	one(0061400),		one(0177400), "BB", m68000up },
{"bcc.s",	one(0062000),		one(0177400), "BB", m68000up },
{"bcs.s",	one(0062400),		one(0177400), "BB", m68000up },
{"bne.s",	one(0063000),		one(0177400), "BB", m68000up },
{"beq.s",	one(0063400),		one(0177400), "BB", m68000up },
{"bvc.s",	one(0064000),		one(0177400), "BB", m68000up },
{"bvs.s",	one(0064400),		one(0177400), "BB", m68000up },
{"bpl.s",	one(0065000),		one(0177400), "BB", m68000up },
{"bmi.s",	one(0065400),		one(0177400), "BB", m68000up },
{"bge.s",	one(0066000),		one(0177400), "BB", m68000up },
{"blt.s",	one(0066400),		one(0177400), "BB", m68000up },
{"bgt.s",	one(0067000),		one(0177400), "BB", m68000up },
{"ble.s",	one(0067400),		one(0177400), "BB", m68000up },
{"bra.s",	one(0060000),		one(0177400), "BB", m68000up },
{"bsr.s",	one(0060400),		one(0177400), "BB", m68000up },
/* Fixed-size branches with long (32-bit) offsets */

{"bhi.l",	one(0061377),		one(0177777), "BL", m68020up },
{"bls.l",	one(0061777),		one(0177777), "BL", m68020up },
{"bcc.l",	one(0062377),		one(0177777), "BL", m68020up },
{"bcs.l",	one(0062777),		one(0177777), "BL", m68020up },
{"bne.l",	one(0063377),		one(0177777), "BL", m68020up },
{"beq.l",	one(0063777),		one(0177777), "BL", m68020up },
{"bvc.l",	one(0064377),		one(0177777), "BL", m68020up },
{"bvs.l",	one(0064777),		one(0177777), "BL", m68020up },
{"bpl.l",	one(0065377),		one(0177777), "BL", m68020up },
{"bmi.l",	one(0065777),		one(0177777), "BL", m68020up },
{"bge.l",	one(0066377),		one(0177777), "BL", m68020up },
{"blt.l",	one(0066777),		one(0177777), "BL", m68020up },
{"bgt.l",	one(0067377),		one(0177777), "BL", m68020up },
{"ble.l",	one(0067777),		one(0177777), "BL", m68020up },
{"bra.l",	one(0060377),		one(0177777), "BL", m68020up },
{"bsr.l",	one(0060777),		one(0177777), "BL", m68020up },

/* We now return you to our regularly scheduled instruction set */

{"bchg.b",	one(0000500),		one(0170700),		"Dd$s", m68000up },
{"bchg.b",	one(0004100),		one(0177700),		"#b$s", m68000up },
{"bclr.b",	one(0000600),		one(0170700),		"Dd$s", m68000up },
{"bclr.b",	one(0004200),		one(0177700),		"#b$s", m68000up },

{"bchg.l",	one(0000500),		one(0170700),		"Dd$s", m68000up },
{"bchg.l",	one(0004100),		one(0177700),		"#b$s", m68000up },
{"bclr.l",	one(0000600),		one(0170700),		"Dd$s", m68000up },
{"bclr.l",	one(0004200),		one(0177700),		"#b$s", m68000up },



{"bchg",	one(0000500),		one(0170700),		"Dd$s", m68000up },
{"bchg",	one(0004100),		one(0177700),		"#b$s", m68000up },
{"bclr",	one(0000600),		one(0170700),		"Dd$s", m68000up },
{"bclr",	one(0004200),		one(0177700),		"#b$s", m68000up },

{"bfchg",	two(0165300, 0),	two(0177700, 0170000),	"?sO2O3",   m68020up },
{"bfclr",	two(0166300, 0),	two(0177700, 0170000),	"?sO2O3",   m68020up },
{"bfexts",	two(0165700, 0),	two(0177700, 0100000),	"/sO2O3D1", m68020up },
{"bfextu",	two(0164700, 0),	two(0177700, 0100000),	"/sO2O3D1", m68020up },
{"bfffo",	two(0166700, 0),	two(0177700, 0100000),	"/sO2O3D1", m68020up },
{"bfins",	two(0167700, 0),	two(0177700, 0100000),	"D1?sO2O3", m68020up },
{"bfset",	two(0167300, 0),	two(0177700, 0170000),	"?sO2O3",   m68020up },
{"bftst",	two(0164300, 0),	two(0177700, 0170000),	"/sO2O3",   m68020up },
{"bkpt",	one(0044110),		one(0177770),		"Qs",       m68020up },


{"bset.b",	one(0000700),		one(0170700),		"Dd$s", m68000up },
{"bset.b",	one(0004300),		one(0177700),		"#b$s", m68000up },
{"btst.b",	one(0000400),		one(0170700),		"Dd@s", m68000up },
{"btst.b",	one(0004000),		one(0177700),		"#b@s", m68000up },

{"bset.l",	one(0000700),		one(0170700),		"Dd$s", m68000up },
{"bset.l",	one(0004300),		one(0177700),		"#b$s", m68000up },
{"btst.l",	one(0000400),		one(0170700),		"Dd@s", m68000up },
{"btst.l",	one(0004000),		one(0177700),		"#b@s", m68000up },

{"bset",	one(0000700),		one(0170700),		"Dd$s", m68000up },
{"bset",	one(0004300),		one(0177700),		"#b$s", m68000up },
{"btst",	one(0000400),		one(0170700),		"Dd@s", m68000up },
{"btst",	one(0004000),		one(0177700),		"#b@s", m68000up },


{"callm",	one(0003300),		one(0177700),		"#b!s", m68020 },


{"casw",	two(0006300, 0),	two(0177700, 0177070),	"D3D2~s", m68020up },
{"cas",		two(0006300, 0),	two(0177700, 0177070),	"D3D2~s", m68020up },
{"cas2",	two(0006374, 0),	two(0177777, 0107070),	"D3D6D2D5R1R4", m68020up }, /* JF ditto */


{"cas2.l",	two(0007374, 0),	two(0177777, 0107070),	"D3D6D2D5R1R4", m68020up }, /* JF FOO really a 3 word ins */
{"cas2.w",	two(0006374, 0),	two(0177777, 0107070),	"D3D6D2D5R1R4", m68020up }, /* JF ditto */
{"cas.b",	two(0005300, 0),	two(0177700, 0177070),	"D3D2~s", m68020up },
{"cas.l",	two(0007300, 0),	two(0177700, 0177070),	"D3D2~s", m68020up },
{"cas.w",	two(0006300, 0),	two(0177700, 0177070),	"D3D2~s", m68020up },

{"chk",  	one(0040600),		one(0170700),		";wDd", m68000up },
{"chk",		one(0040400),		one(0170700),		";lDd", m68000up },

/*  {"chk",	one(0040600),		one(0170700),		";wDd"}, JF FOO this looks wrong */


{"chk2",	two(0001300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },

{"chk2.b",	two(0000300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },
{"chk2.l",	two(0002300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },
{"chk2.w",	two(0001300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },

{"chk.w",	two(0001300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },

{"chk.l",	one(0040400),		one(0170700),		";lDd", m68000up },
{"chk2.w",	two(0001300, 0004000),	two(0177700, 07777),	"!sR1", m68020up },
 {"chk.w",	one(0040600),		one(0170700),		";wDd", m68000up },

#define SCOPE_LINE (0x1 << 3)
#define SCOPE_PAGE (0x2 << 3)
#define SCOPE_ALL  (0x3 << 3)

{"cinva",	one(0xf400|SCOPE_ALL),  one(0xff20), "ce",   m68040 },
{"cinvl",	one(0xf400|SCOPE_LINE), one(0xff20), "ceas", m68040 },
{"cinvp",	one(0xf400|SCOPE_PAGE), one(0xff20), "ceas", m68040 },

{"cpusha",	one(0xf420|SCOPE_ALL),  one(0xff20), "ce",   m68040 },
{"cpushl",	one(0xf420|SCOPE_LINE), one(0xff20), "ceas", m68040 },
{"cpushp",	one(0xf420|SCOPE_PAGE), one(0xff20), "ceas", m68040 },

#undef SCOPE_LINE
#undef SCOPE_PAGE
#undef SCOPE_ALL
{"clr.b",	one(0041000),		one(0177700),		"$s", m68000up },
{"clr.l",	one(0041200),		one(0177700),		"$s", m68000up },
{"clr.w",	one(0041100),		one(0177700),		"$s", m68000up },


{"cmp",		one(0006200),		one(0177700),		"#l;l", m68000up },
{"cmp",		one(0130200),		one(0170700),		"*lDd", m68000up },
{"cmp",		one(0130700),		one(0170700),		"*lAd", m68000up },
{"cmp",		one(0130510),		one(0170770),		"+s+d", m68000up },	/* cmpm written as cmp */

{"cmpi",	one(0006200),		one(0177700),		"#l;l", m68000up },




{"cmp2",	two(0001300, 0),	two(0177700, 07777),	"!sR1", m68020up },

{"cmp2.b",	two(0000300, 0),	two(0177700, 07777),	"!sR1", m68020up },
{"cmp2.l",	two(0002300, 0),	two(0177700, 07777),	"!sR1", m68020up },
{"cmp2.w",	two(0001300, 0),	two(0177700, 07777),	"!sR1", m68020up },
{"cmpa.l",	one(0130700),		one(0170700),		"*lAd", m68000up },
{"cmpa.w",	one(0130300),		one(0170700),		"*wAd", m68000up },

{"cmpi.b",	one(0006000),		one(0177700),		"#b;b", m68000up },
{"cmpi.l",	one(0006200),		one(0177700),		"#l;l", m68000up },
{"cmpi.w",	one(0006100),		one(0177700),		"#w;w", m68000up },
{"cmp.b",	one(0006000),		one(0177700),		"#b;b", m68000up },	/* cmpi written as cmp */
{"cmp.b",	one(0130000),		one(0170700),		";bDd", m68000up },
{"cmp.b",	one(0130410),		one(0170770),		"+s+d", m68000up },     /* cmpm written as cmp */
{"cmp.w",	one(0006100),		one(0177700),		"#w;w", m68000up },
{"cmp.w",	one(0130100),		one(0170700),		"*wDd", m68000up },
{"cmp.w",	one(0130300),		one(0170700),		"*wAd", m68000up },	/* cmpa written as cmp */
{"cmp.w",	one(0130510),		one(0170770),		"+s+d", m68000up },	/* cmpm written as cmp */
{"cmp.l",	one(0006200),		one(0177700),		"#l;l", m68000up },
{"cmp.l",	one(0130200),		one(0170700),		"*lDd", m68000up },
{"cmp.l",	one(0130700),		one(0170700),		"*lAd", m68000up },
{"cmpl",	one(0130610),		one(0170770),		"+s+d", m68000up },	/* cmpm written as cmp */

{"cmpm",	one(0130510),		one(0170770),		"+s+d", m68000up },

{"cmpm.b",	one(0130410),		one(0170770),		"+s+d", m68000up },
{"cmpm.l",	one(0130610),		one(0170770),		"+s+d", m68000up },
{"cmpm.w",	one(0130510),		one(0170770),		"+s+d", m68000up },

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

{"tdivs.l",	two(0046100, 0006000),	two(0177700, 0107770),	";lD3D1", m68020up },
{"tdivs.l",	two(0046100, 0004000),	two(0177700, 0107770),	";lDD", m68020up },


{"divs.l",	two(0046100, 0006000),	two(0177700, 0107770),	";lD3D1", m68020up },
{"divs.l",	two(0046100, 0004000),	two(0177700, 0107770),	";lDD", m68020up },
{"divs.ll",	two(0046100, 0004000),	two(0177700, 0107770),	";lD3D1", m68020up },
{"divs.w",	one(0100700),		one(0170700),		";wDd", m68000up },
{"divs",	one(0100700),		one(0170700),		";wDd", m68000up },
{"divu.l",	two(0046100, 0002000),	two(0177700, 0107770),	";lD3D1", m68020up },
{"divu.l",	two(0046100, 0000000),	two(0177700, 0107770),	";lDD", m68020up },
{"divu.ll",	two(0046100, 0000000),	two(0177700, 0107770),	";lD3D1", m68020up },
{"divu.w",	one(0100300),		one(0170700),		";wDd", m68000up },
{"divu",	one(0100300),		one(0170700),		";wDd", m68000up },

{"eor",		one(0005074),		one(0177777),		"#bCs", m68000up },	/* eori to ccr */
{"eor",		one(0005174),		one(0177777),		"#wSs", m68000up },	/* eori to sr */
{"eori",	one(0005074),		one(0177777),		"#bCs", m68000up },	/* eori to ccr */
{"eori",	one(0005174),		one(0177777),		"#wSs", m68000up },	/* eori to sr */


{"eor.b",	one(0005000),		one(0177700),		"#b$s", m68000up },	/* eori written as or */
{"eor.b",	one(0005074),		one(0177777),		"#bCs", m68000up },	/* eori to ccr */
{"eor.b",	one(0130400),		one(0170700),		"Dd$s", m68000up },	/* register to memory */
{"eori.b",	one(0005000),		one(0177700),		"#b$s", m68000up },
{"eori.b",	one(0005074),		one(0177777),		"#bCs", m68000up },	/* eori to ccr */
{"eori.l",	one(0005200),		one(0177700),		"#l$s", m68000up },
{"eori.w",	one(0005100),		one(0177700),		"#w$s", m68000up },
{"eori.w",	one(0005174),		one(0177777),		"#wSs", m68000up },	/* eori to sr */
{"eor.l",	one(0005200),		one(0177700),		"#l$s", m68000up },
{"eor.l",	one(0130600),		one(0170700),		"Dd$s", m68000up },
{"eor.w",	one(0005100),		one(0177700),		"#w$s", m68000up },
{"eor.w",	one(0005174),		one(0177777),		"#wSs", m68000up },	/* eori to sr */
{"eor.w",	one(0130500),		one(0170700),		"Dd$s", m68000up },

{"exg",		one(0140500),		one(0170770),		"DdDs", m68000up },
{"exg",		one(0140510),		one(0170770),		"AdAs", m68000up },
{"exg",		one(0140610),		one(0170770),		"DdAs", m68000up },
{"exg",		one(0140610),		one(0170770),		"AsDd", m68000up },

{"ext",	one(0044200),		one(0177770),		"Ds", m68000up },
{"ext.wl",	one(0044200),		one(0177770),		"Ds", m68000up },
{"ext.bw",	one(0044200),		one(0177770),		"Ds", m68000up },
{"ext.w",	one(0044200),		one(0177770),		"Ds", m68000up },
{"ext.l",	one(0044300),		one(0177770),		"Ds", m68000up },
{"ext.bl",	one(0044700),		one(0177770),		"Ds", m68020up },
{"ext.bl",	one(0044700),		one(0177770),		"Ds", m68020up }, /* Not sure we should support this one */

/* float stuff starts here */

{"fabs.b",	two(0xF000, 0x5818),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fabs.d",	two(0xF000, 0x5418),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fabs.l",	two(0xF000, 0x4018),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fabs.p",	two(0xF000, 0x4C18),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fabs.s",	two(0xF000, 0x4418),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fabs.w",	two(0xF000, 0x5018),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fabs.x",	two(0xF000, 0x0018),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fabs.x",	two(0xF000, 0x4818),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fabs.x",	two(0xF000, 0x0018),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

/* FIXME-NOW: The '040 book that I have claims that these should be
   coded exactly like fadd.  In fact, the table of opmodes calls them
   fadd, fsadd, fdadd.  That can't be right.  If someone can give me the
   right encoding, I'll fix it.  By induction, I *think* the right
   encoding is 38 & 3c, but I'm not sure.

   in the mean time, if you know the encoding for the opmode field, you
   can replace all of the "38),"'s and "3c),"'s below with the corrected
   values and these guys should then just work.  xoxorich. 31Aug91 */

#ifdef comment
{"fsabsb",	two(0xF000, 0x5838),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fsabsd",	two(0xF000, 0x5438),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fsabsl",	two(0xF000, 0x4038),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fsabsp",	two(0xF000, 0x4C38),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fsabss",	two(0xF000, 0x4438),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fsabsw",	two(0xF000, 0x5038),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fsabsx",	two(0xF000, 0x0038),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fsabsx",	two(0xF000, 0x4838),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{"fsabsx",	two(0xF000, 0x0038),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{"fdabsb",	two(0xF000, 0x583c),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040},
{"fdabsd",	two(0xF000, 0x543c),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040},
{"fdabsl",	two(0xF000, 0x403c),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040},
{"fdabsp",	two(0xF000, 0x4C3c),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040},
{"fdabss",	two(0xF000, 0x443c),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040},
{"fdabsw",	two(0xF000, 0x503c),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040},
{"fdabsx",	two(0xF000, 0x003c),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040},
{"fdabsx",	two(0xF000, 0x483c),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040},
{"fdabsx",	two(0xF000, 0x003c),	two(0xF1C0, 0xE07F),	"IiFt",   m68040},
#endif /* comment */

{"facos.b",	two(0xF000, 0x581C),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"facos.d",	two(0xF000, 0x541C),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"facos.l",	two(0xF000, 0x401C),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"facos.p",	two(0xF000, 0x4C1C),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"facos.s",	two(0xF000, 0x441C),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"facos.w",	two(0xF000, 0x501C),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"facos.x",	two(0xF000, 0x001C),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"facos.x",	two(0xF000, 0x481C),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"facos.x",	two(0xF000, 0x001C),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fadd.b",	two(0xF000, 0x5822),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fadd.d",	two(0xF000, 0x5422),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fadd.l",	two(0xF000, 0x4022),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fadd.p",	two(0xF000, 0x4C22),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fadd.s",	two(0xF000, 0x4422),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fadd.w",	two(0xF000, 0x5022),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fadd.x",	two(0xF000, 0x0022),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fadd.x",	two(0xF000, 0x4822),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {"faddx",	two(0xF000, 0x0022),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF removed */

{"fsadd.b",	two(0xF000, 0x5832),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fsadd.d",	two(0xF000, 0x5432),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fsadd.l",	two(0xF000, 0x4032),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fsadd.p",	two(0xF000, 0x4C32),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fsadd.s",	two(0xF000, 0x4432),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fsadd.w",	two(0xF000, 0x5032),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fsadd.x",	two(0xF000, 0x0032),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fsadd.x",	two(0xF000, 0x4832),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {"fsaddx",	two(0xF000, 0x0032),	two(0xF1C0, 0xE07F),	"IiFt", m68040 }, JF removed */

{"fdadd.b",	two(0xF000, 0x5836),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fdadd.d",	two(0xF000, 0x5436),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fdadd.l",	two(0xF000, 0x4036),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fdadd.p",	two(0xF000, 0x4C36),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fdadd.s",	two(0xF000, 0x4436),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fdadd.w",	two(0xF000, 0x5036),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fdadd.x",	two(0xF000, 0x0036),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fdadd.x",	two(0xF000, 0x4836),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {"faddx",	two(0xF000, 0x0036),	two(0xF1C0, 0xE07F),	"IiFt", m68040 }, JF removed */

{"fasin.b",	two(0xF000, 0x580C),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fasin.d",	two(0xF000, 0x540C),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fasin.l",	two(0xF000, 0x400C),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fasin.p",	two(0xF000, 0x4C0C),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fasin.s",	two(0xF000, 0x440C),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fasin.w",	two(0xF000, 0x500C),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fasin.x",	two(0xF000, 0x000C),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fasin.x",	two(0xF000, 0x480C),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fasin.x",	two(0xF000, 0x000C),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fatan.b",	two(0xF000, 0x580A),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fatan.d",	two(0xF000, 0x540A),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fatan.l",	two(0xF000, 0x400A),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fatan.p",	two(0xF000, 0x4C0A),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fatan.s",	two(0xF000, 0x440A),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fatan.w",	two(0xF000, 0x500A),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fatan.x",	two(0xF000, 0x000A),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fatan.x",	two(0xF000, 0x480A),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fatan.x",	two(0xF000, 0x000A),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fatanh.b",	two(0xF000, 0x580D),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fatanh.d",	two(0xF000, 0x540D),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fatanh.l",	two(0xF000, 0x400D),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fatanh.p",	two(0xF000, 0x4C0D),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fatanh.s",	two(0xF000, 0x440D),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fatanh.w",	two(0xF000, 0x500D),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fatanh.x",	two(0xF000, 0x000D),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fatanh.x",	two(0xF000, 0x480D),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fatanh.x",	two(0xF000, 0x000D),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

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

{"fcmp.b",	two(0xF000, 0x5838),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fcmp.d",	two(0xF000, 0x5438),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fcmp.l",	two(0xF000, 0x4038),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fcmp.p",	two(0xF000, 0x4C38),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fcmp.s",	two(0xF000, 0x4438),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fcmp.w",	two(0xF000, 0x5038),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fcmp.x",	two(0xF000, 0x0038),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fcmp.x",	two(0xF000, 0x4838),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {"fcmpx",	two(0xF000, 0x0038),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF removed */

{"fcos.b",	two(0xF000, 0x581D),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fcos.d",	two(0xF000, 0x541D),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fcos.l",	two(0xF000, 0x401D),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fcos.p",	two(0xF000, 0x4C1D),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fcos.s",	two(0xF000, 0x441D),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fcos.w",	two(0xF000, 0x501D),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fcos.x",	two(0xF000, 0x001D),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fcos.x",	two(0xF000, 0x481D),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fcos.x",	two(0xF000, 0x001D),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fcosh.b",	two(0xF000, 0x5819),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fcosh.d",	two(0xF000, 0x5419),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fcosh.l",	two(0xF000, 0x4019),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fcosh.p",	two(0xF000, 0x4C19),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fcosh.s",	two(0xF000, 0x4419),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fcosh.w",	two(0xF000, 0x5019),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fcosh.x",	two(0xF000, 0x0019),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fcosh.x",	two(0xF000, 0x4819),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fcosh.x",	two(0xF000, 0x0019),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fdbeq",	two(0xF048, 0x0001),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbf",	two(0xF048, 0x0000),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbge",	two(0xF048, 0x0013),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbgl",	two(0xF048, 0x0016),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbgle",	two(0xF048, 0x0017),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbgt",	two(0xF048, 0x0012),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdble",	two(0xF048, 0x0015),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdblt",	two(0xF048, 0x0014),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbne",	two(0xF048, 0x000E),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
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
{"fdbst",	two(0xF048, 0x001F),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbt",	two(0xF048, 0x000F),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbueq",	two(0xF048, 0x0009),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbuge",	two(0xF048, 0x000B),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbugt",	two(0xF048, 0x000A),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbule",	two(0xF048, 0x000D),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbult",	two(0xF048, 0x000C),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },
{"fdbun",	two(0xF048, 0x0008),	two(0xF1F8, 0xFFFF),	"IiDsBw", mfloat },

{"fdiv.b",	two(0xF000, 0x5820),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fdiv.d",	two(0xF000, 0x5420),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fdiv.l",	two(0xF000, 0x4020),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fdiv.p",	two(0xF000, 0x4C20),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fdiv.s",	two(0xF000, 0x4420),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fdiv.w",	two(0xF000, 0x5020),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fdiv.x",	two(0xF000, 0x0020),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fdiv.x",	two(0xF000, 0x4820),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {"fdivx",	two(0xF000, 0x0020),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF */

{"fsdiv.b",	two(0xF000, 0x5830),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fsdiv.d",	two(0xF000, 0x5430),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fsdiv.l",	two(0xF000, 0x4030),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fsdiv.p",	two(0xF000, 0x4C30),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fsdiv.s",	two(0xF000, 0x4430),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fsdiv.w",	two(0xF000, 0x5030),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fsdiv.x",	two(0xF000, 0x0030),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fsdiv.x",	two(0xF000, 0x4830),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {"fsdivx",	two(0xF000, 0x0030),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 }, JF */

{"fddiv.b",	two(0xF000, 0x5834),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fddiv.d",	two(0xF000, 0x5434),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fddiv.l",	two(0xF000, 0x4034),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fddiv.p",	two(0xF000, 0x4C34),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fddiv.s",	two(0xF000, 0x4434),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fddiv.w",	two(0xF000, 0x5034),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fddiv.x",	two(0xF000, 0x0034),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fddiv.x",	two(0xF000, 0x4834),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {"fddivx",	two(0xF000, 0x0034),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 }, JF */

{"fetox.b",	two(0xF000, 0x5810),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fetox.d",	two(0xF000, 0x5410),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fetox.l",	two(0xF000, 0x4010),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fetox.p",	two(0xF000, 0x4C10),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fetox.s",	two(0xF000, 0x4410),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fetox.w",	two(0xF000, 0x5010),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fetox.x",	two(0xF000, 0x0010),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fetox.x",	two(0xF000, 0x4810),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fetox.x",	two(0xF000, 0x0010),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fetoxm1.b",	two(0xF000, 0x5808),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fetoxm1.d",	two(0xF000, 0x5408),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fetoxm1.l",	two(0xF000, 0x4008),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fetoxm1.p",	two(0xF000, 0x4C08),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fetoxm1.s",	two(0xF000, 0x4408),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fetoxm1.w",	two(0xF000, 0x5008),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fetoxm1.x",	two(0xF000, 0x0008),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fetoxm1.x",	two(0xF000, 0x4808),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fetoxm1.x",	two(0xF000, 0x0008),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fgetexp.b",	two(0xF000, 0x581E),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fgetexp.d",	two(0xF000, 0x541E),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fgetexp.l",	two(0xF000, 0x401E),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fgetexp.p",	two(0xF000, 0x4C1E),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fgetexp.s",	two(0xF000, 0x441E),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fgetexp.w",	two(0xF000, 0x501E),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fgetexp.x",	two(0xF000, 0x001E),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fgetexp.x",	two(0xF000, 0x481E),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fgetexp.x",	two(0xF000, 0x001E),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fgetman.b",	two(0xF000, 0x581F),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fgetman.d",	two(0xF000, 0x541F),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fgetman.l",	two(0xF000, 0x401F),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fgetman.p",	two(0xF000, 0x4C1F),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fgetman.s",	two(0xF000, 0x441F),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fgetman.w",	two(0xF000, 0x501F),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fgetman.x",	two(0xF000, 0x001F),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fgetman.x",	two(0xF000, 0x481F),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fgetman.x",	two(0xF000, 0x001F),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fint.b",	two(0xF000, 0x5801),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fint.d",	two(0xF000, 0x5401),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fint.l",	two(0xF000, 0x4001),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fint.p",	two(0xF000, 0x4C01),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fint.s",	two(0xF000, 0x4401),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fint.w",	two(0xF000, 0x5001),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fint.x",	two(0xF000, 0x0001),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fint.x",	two(0xF000, 0x4801),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fint.x",	two(0xF000, 0x0001),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fintrz.b",	two(0xF000, 0x5803),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fintrz.d",	two(0xF000, 0x5403),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fintrz.l",	two(0xF000, 0x4003),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fintrz.p",	two(0xF000, 0x4C03),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fintrz.s",	two(0xF000, 0x4403),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fintrz.w",	two(0xF000, 0x5003),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fintrz.x",	two(0xF000, 0x0003),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fintrz.x",	two(0xF000, 0x4803),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fintrz.x",	two(0xF000, 0x0003),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"flog10.b",	two(0xF000, 0x5815),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"flog10.d",	two(0xF000, 0x5415),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"flog10.l",	two(0xF000, 0x4015),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"flog10.p",	two(0xF000, 0x4C15),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"flog10.s",	two(0xF000, 0x4415),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"flog10.w",	two(0xF000, 0x5015),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"flog10.x",	two(0xF000, 0x0015),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"flog10.x",	two(0xF000, 0x4815),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"flog10.x",	two(0xF000, 0x0015),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"flog2.b",	two(0xF000, 0x5816),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"flog2.d",	two(0xF000, 0x5416),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"flog2.l",	two(0xF000, 0x4016),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"flog2.p",	two(0xF000, 0x4C16),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"flog2.s",	two(0xF000, 0x4416),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"flog2.w",	two(0xF000, 0x5016),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"flog2.x",	two(0xF000, 0x0016),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"flog2.x",	two(0xF000, 0x4816),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"flog2.x",	two(0xF000, 0x0016),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"flogn.b",	two(0xF000, 0x5814),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"flogn.d",	two(0xF000, 0x5414),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"flogn.l",	two(0xF000, 0x4014),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"flogn.p",	two(0xF000, 0x4C14),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"flogn.s",	two(0xF000, 0x4414),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"flogn.w",	two(0xF000, 0x5014),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"flogn.x",	two(0xF000, 0x0014),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"flogn.x",	two(0xF000, 0x4814),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"flogn.x",	two(0xF000, 0x0014),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"flognp1.b",	two(0xF000, 0x5806),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"flognp1.d",	two(0xF000, 0x5406),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"flognp1.l",	two(0xF000, 0x4006),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"flognp1.p",	two(0xF000, 0x4C06),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"flognp1.s",	two(0xF000, 0x4406),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"flognp1.w",	two(0xF000, 0x5006),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"flognp1.x",	two(0xF000, 0x0006),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"flognp1.x",	two(0xF000, 0x4806),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"flognp1.x",	two(0xF000, 0x0006),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fmod.b",	two(0xF000, 0x5821),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fmod.d",	two(0xF000, 0x5421),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fmod.l",	two(0xF000, 0x4021),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fmod.p",	two(0xF000, 0x4C21),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fmod.s",	two(0xF000, 0x4421),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fmod.w",	two(0xF000, 0x5021),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fmod.x",	two(0xF000, 0x0021),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fmod.x",	two(0xF000, 0x4821),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {"fmod.x",	two(0xF000, 0x0021),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF */

{"fmove.b",	two(0xF000, 0x5800),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },		/* fmove from <ea> to fp<n> */
{"fmove.b",	two(0xF000, 0x7800),	two(0xF1C0, 0xFC7F),	"IiF7@b", mfloat },		/* fmove from fp<n> to <ea> */
{"fmove.d",	two(0xF000, 0x5400),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },		/* fmove from <ea> to fp<n> */
{"fmove.d",	two(0xF000, 0x7400),	two(0xF1C0, 0xFC7F),	"IiF7@F", mfloat },		/* fmove from fp<n> to <ea> */
{"fmove.l",	two(0xF000, 0x4000),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },		/* fmove from <ea> to fp<n> */
{"fmove.l",	two(0xF000, 0x6000),	two(0xF1C0, 0xFC7F),	"IiF7@l", mfloat },		/* fmove from fp<n> to <ea> */
/* Warning:  The addressing modes on these are probably not right:
   esp, Areg direct is only allowed for FPI */
		/* fmove.l from/to system control registers: */
{"fmove.l",	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"Iis8@s", mfloat },
{"fmove.l",	two(0xF000, 0x8000),	two(0xF1C0, 0xE3FF),	"Ii*ls8", mfloat },

/* {"fmove.l",	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"Iis8@s", mfloat },
{"fmove.l",	two(0xF000, 0x8000),	two(0xF2C0, 0xE3FF),	"Ii*ss8", mfloat }, */

{"fmove.p",	two(0xF000, 0x4C00),	two(0xF1C0, 0xFC7F),	"Ii;pF7",   mfloat },		/* fmove from <ea> to fp<n> */
{"fmove.p",	two(0xF000, 0x6C00),	two(0xF1C0, 0xFC00),	"IiF7@pkC", mfloat },		/* fmove.p with k-factors: */
{"fmove.p",	two(0xF000, 0x7C00),	two(0xF1C0, 0xFC0F),	"IiF7@pDk", mfloat },		/* fmove.p with k-factors: */

{"fmove.s",	two(0xF000, 0x4400),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },		/* fmove from <ea> to fp<n> */
{"fmove.s",	two(0xF000, 0x6400),	two(0xF1C0, 0xFC7F),	"IiF7@f", mfloat },		/* fmove from fp<n> to <ea> */
{"fmove.w",	two(0xF000, 0x5000),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },		/* fmove from <ea> to fp<n> */
{"fmove.w",	two(0xF000, 0x7000),	two(0xF1C0, 0xFC7F),	"IiF7@w", mfloat },		/* fmove from fp<n> to <ea> */
{"fmove.x",	two(0xF000, 0x0000),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },		/* fmove from <ea> to fp<n> */
{"fmove.x",	two(0xF000, 0x4800),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },		/* fmove from <ea> to fp<n> */
{"fmove.x",	two(0xF000, 0x6800),	two(0xF1C0, 0xFC7F),	"IiF7@x", mfloat },		/* fmove from fp<n> to <ea> */
/* JF removed {"fmovex",	two(0xF000, 0x0000),	two(0xF1C0, 0xE07F),	"IiFt", mfloat }, / * fmove from <ea> to fp<n> */

{"fsmove.b",	two(0xF000, 0x5800),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 }, /* fmove from <ea> to fp<n> */
{"fsmove.d",	two(0xF000, 0x5400),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 }, /* fmove from <ea> to fp<n> */
{"fsmove.l",	two(0xF000, 0x4000),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 }, /* fmove from <ea> to fp<n> */
{"fsmove.s",	two(0xF000, 0x4400),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 }, /* fmove from <ea> to fp<n> */
{"fsmove.w",	two(0xF000, 0x5000),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 }, /* fmove from <ea> to fp<n> */
{"fsmove.x",	two(0xF000, 0x0000),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 }, /* fmove from <ea> to fp<n> */
{"fsmove.x",	two(0xF000, 0x4800),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 }, /* fmove from <ea> to fp<n> */
/* JF removed {"fsmovex",	two(0xF000, 0x0000),	two(0xF1C0, 0xE07F),	"IiFt", m68040 }, / * fmove from <ea> to fp<n> */

{"fdmove.b",	two(0xF000, 0x5800),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 }, /* fmove from <ea> to fp<n> */
{"fdmove.d",	two(0xF000, 0x5400),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 }, /* fmove from <ea> to fp<n> */
{"fdmove.l",	two(0xF000, 0x4000),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 }, /* fmove from <ea> to fp<n> */
{"fdmove.s",	two(0xF000, 0x4400),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 }, /* fmove from <ea> to fp<n> */
{"fdmove.w",	two(0xF000, 0x5000),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 }, /* fmove from <ea> to fp<n> */
{"fdmove.x",	two(0xF000, 0x0000),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 }, /* fmove from <ea> to fp<n> */
{"fdmove.x",	two(0xF000, 0x4800),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 }, /* fmove from <ea> to fp<n> */
/* JF removed {"fdmovex",	two(0xF000, 0x0000),	two(0xF1C0, 0xE07F),	"IiFt", m68040 }, / * fmove from <ea> to fp<n> */

{"fmovecr.x",	two(0xF000, 0x5C00),	two(0xF1FF, 0xFC00),	"Ii#CF7", mfloat },		/* fmovecr.x #ccc, FPn */
{"fmovecr",	two(0xF000, 0x5C00),	two(0xF1FF, 0xFC00),	"Ii#CF7", mfloat },

/* Other fmovemx.  */
{"fmovem.x", two(0xF000, 0xF800), two(0xF1C0, 0xFF8F), "IiDk&s", mfloat }, /* reg to control,	static and dynamic: */
{"fmovem.x", two(0xF000, 0xD800), two(0xF1C0, 0xFF8F), "Ii&sDk", mfloat }, /* from control to reg, static and dynamic: */

{"fmovem.x", two(0xF000, 0xF000), two(0xF1C0, 0xFF00), "Idl3&s", mfloat }, /* to control, static and dynamic: */
{"fmovem.x", two(0xF000, 0xF000), two(0xF1C0, 0xFF00), "Id#3&s", mfloat }, /* to control, static and dynamic: */

{"fmovem.x", two(0xF000, 0xD000), two(0xF1C0, 0xFF00), "Id&sl3", mfloat }, /* from control, static and dynamic: */
{"fmovem.x", two(0xF000, 0xD000), two(0xF1C0, 0xFF00), "Id&s#3", mfloat }, /* from control, static and dynamic: */

{"fmovem.x", two(0xF020, 0xE800), two(0xF1F8, 0xFF8F), "IiDk-s", mfloat }, /* reg to autodecrement, static and dynamic */
{"fmovem.x", two(0xF020, 0xE000), two(0xF1F8, 0xFF00), "IdL3-s", mfloat }, /* to autodecrement, static and dynamic */
{"fmovem.x", two(0xF020, 0xE000), two(0xF1F8, 0xFF00), "Id#3-s", mfloat }, /* to autodecrement, static and dynamic */

{"fmovem.x", two(0xF018, 0xD800), two(0xF1F8, 0xFF8F), "Ii+sDk", mfloat }, /* from autoinc to reg, static and dynamic: */
{"fmovem.x", two(0xF018, 0xD000), two(0xF1F8, 0xFF00), "Id+sl3", mfloat }, /* from autoincrement, static and dynamic: */
{"fmovem.x", two(0xF018, 0xD000), two(0xF1F8, 0xFF00), "Id+s#3", mfloat }, /* from autoincrement, static and dynamic: */

{"fmovem.l",	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"IiL8@s", mfloat },
{"fmovem.l",	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"Ii#8@s", mfloat },
{"fmovem.l",	two(0xF000, 0xA000),	two(0xF1C0, 0xE3FF),	"Iis8@s", mfloat },

{"fmovem.l",	two(0xF000, 0x8000),	two(0xF2C0, 0xE3FF),	"Ii*sL8", mfloat },
{"fmovem.l",	two(0xF000, 0x8000),	two(0xF1C0, 0xE3FF),	"Ii*s#8", mfloat },
{"fmovem.l",	two(0xF000, 0x8000),	two(0xF1C0, 0xE3FF),	"Ii*ss8", mfloat },

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

{"fmul.b",	two(0xF000, 0x5823),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fmul.d",	two(0xF000, 0x5423),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fmul.l",	two(0xF000, 0x4023),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fmul.p",	two(0xF000, 0x4C23),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fmul.s",	two(0xF000, 0x4423),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fmul.w",	two(0xF000, 0x5023),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fmul.x",	two(0xF000, 0x0023),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fmul.x",	two(0xF000, 0x4823),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {"fmulx",	two(0xF000, 0x0023),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF */

{"fsmul.b",	two(0xF000, 0x5833),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fsmul.d",	two(0xF000, 0x5433),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fsmul.l",	two(0xF000, 0x4033),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fsmul.p",	two(0xF000, 0x4C33),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fsmul.s",	two(0xF000, 0x4433),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fsmul.w",	two(0xF000, 0x5033),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fsmul.x",	two(0xF000, 0x0033),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fsmul.x",	two(0xF000, 0x4833),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {"fsmulx",	two(0xF000, 0x0033),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 }, JF */

{"fdmul.b",	two(0xF000, 0x5837),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fdmul.d",	two(0xF000, 0x5437),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fdmul.l",	two(0xF000, 0x4037),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fdmul.p",	two(0xF000, 0x4C37),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fdmul.s",	two(0xF000, 0x4437),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fdmul.w",	two(0xF000, 0x5037),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fdmul.x",	two(0xF000, 0x0037),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fdmul.x",	two(0xF000, 0x4837),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
/* {"dfmulx",	two(0xF000, 0x0037),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 }, JF */

{"fneg.b",	two(0xF000, 0x581A),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fneg.d",	two(0xF000, 0x541A),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fneg.l",	two(0xF000, 0x401A),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fneg.p",	two(0xF000, 0x4C1A),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fneg.s",	two(0xF000, 0x441A),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fneg.w",	two(0xF000, 0x501A),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fneg.x",	two(0xF000, 0x001A),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fneg.x",	two(0xF000, 0x481A),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fneg.x",	two(0xF000, 0x001A),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fsneg.b",	two(0xF000, 0x585A),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fsneg.d",	two(0xF000, 0x545A),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fsneg.l",	two(0xF000, 0x405A),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fsneg.p",	two(0xF000, 0x4C5A),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fsneg.s",	two(0xF000, 0x445A),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fsneg.w",	two(0xF000, 0x505A),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fsneg.x",	two(0xF000, 0x005A),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fsneg.x",	two(0xF000, 0x485A),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{"fsneg.x",	two(0xF000, 0x005A),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{"fdneg.b",	two(0xF000, 0x585E),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fdneg.d",	two(0xF000, 0x545E),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fdneg.l",	two(0xF000, 0x405E),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fdneg.p",	two(0xF000, 0x4C5E),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fdneg.s",	two(0xF000, 0x445E),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fdneg.w",	two(0xF000, 0x505E),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fdneg.x",	two(0xF000, 0x005E),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fdneg.x",	two(0xF000, 0x485E),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{"fdneg.x",	two(0xF000, 0x005E),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{"fnop",	two(0xF280, 0x0000),	two(0xFFFF, 0xFFFF),	"Ii", mfloat },

{"frem.b",	two(0xF000, 0x5825),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"frem.d",	two(0xF000, 0x5425),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"frem.l",	two(0xF000, 0x4025),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"frem.p",	two(0xF000, 0x4C25),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"frem.s",	two(0xF000, 0x4425),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"frem.w",	two(0xF000, 0x5025),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"frem.x",	two(0xF000, 0x0025),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"frem.x",	two(0xF000, 0x4825),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
/* {"fremx",	two(0xF000, 0x0025),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat }, JF */

{"frestore",	one(0xF140),		one(0xF1C0),		"Id&s", mfloat },
{"frestore",	one(0xF158),		one(0xF1F8),		"Id+s", mfloat },
{"fsave",	one(0xF100),		one(0xF1C0),		"Id&s", mfloat },
{"fsave",	one(0xF120),		one(0xF1F8),		"Id-s", mfloat },

{"fscale.b",	two(0xF000, 0x5826),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fscale.d",	two(0xF000, 0x5426),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fscale.l",	two(0xF000, 0x4026),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fscale.p",	two(0xF000, 0x4C26),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fscale.s",	two(0xF000, 0x4426),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fscale.w",	two(0xF000, 0x5026),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fscale.x",	two(0xF000, 0x0026),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fscale.x",	two(0xF000, 0x4826),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
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
{"fsst",	two(0xF040, 0x001F),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fst",		two(0xF040, 0x000F),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsueq",	two(0xF040, 0x0009),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsuge",	two(0xF040, 0x000B),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsugt",	two(0xF040, 0x000A),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsule",	two(0xF040, 0x000D),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsult",	two(0xF040, 0x000C),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },
{"fsun",	two(0xF040, 0x0008),	two(0xF1C0, 0xFFFF),	"Ii$s", mfloat },

{"fsgldiv.b",	two(0xF000, 0x5824),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fsgldiv.d",	two(0xF000, 0x5424),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fsgldiv.l",	two(0xF000, 0x4024),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fsgldiv.p",	two(0xF000, 0x4C24),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fsgldiv.s",	two(0xF000, 0x4424),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fsgldiv.w",	two(0xF000, 0x5024),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fsgldiv.x",	two(0xF000, 0x0024),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fsgldiv.x",	two(0xF000, 0x4824),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fsgldiv.x",	two(0xF000, 0x0024),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fsglmul.b",	two(0xF000, 0x5827),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fsglmul.d",	two(0xF000, 0x5427),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fsglmul.l",	two(0xF000, 0x4027),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fsglmul.p",	two(0xF000, 0x4C27),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fsglmul.s",	two(0xF000, 0x4427),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fsglmul.w",	two(0xF000, 0x5027),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fsglmul.x",	two(0xF000, 0x0027),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fsglmul.x",	two(0xF000, 0x4827),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fsglmul.x",	two(0xF000, 0x0027),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fsin.b",	two(0xF000, 0x580E),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fsin.d",	two(0xF000, 0x540E),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fsin.l",	two(0xF000, 0x400E),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fsin.p",	two(0xF000, 0x4C0E),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fsin.s",	two(0xF000, 0x440E),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fsin.w",	two(0xF000, 0x500E),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fsin.x",	two(0xF000, 0x000E),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fsin.x",	two(0xF000, 0x480E),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fsin.x",	two(0xF000, 0x000E),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fsinh.b",	two(0xF000, 0x5802),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fsinh.d",	two(0xF000, 0x5402),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fsinh.l",	two(0xF000, 0x4002),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fsinh.p",	two(0xF000, 0x4C02),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fsinh.s",	two(0xF000, 0x4402),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fsinh.w",	two(0xF000, 0x5002),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fsinh.x",	two(0xF000, 0x0002),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fsinh.x",	two(0xF000, 0x4802),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fsinh.x",	two(0xF000, 0x0002),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fsincos.b",	two(0xF000, 0x5830),	two(0xF1C0, 0xFC78),	"Ii;bF3F7", mfloat },
{"fsincos.d",	two(0xF000, 0x5430),	two(0xF1C0, 0xFC78),	"Ii;FF3F7", mfloat },
{"fsincos.l",	two(0xF000, 0x4030),	two(0xF1C0, 0xFC78),	"Ii;lF3F7", mfloat },
{"fsincos.p",	two(0xF000, 0x4C30),	two(0xF1C0, 0xFC78),	"Ii;pF3F7", mfloat },
{"fsincos.s",	two(0xF000, 0x4430),	two(0xF1C0, 0xFC78),	"Ii;fF3F7", mfloat },
{"fsincos.w",	two(0xF000, 0x5030),	two(0xF1C0, 0xFC78),	"Ii;wF3F7", mfloat },
{"fsincos.x",	two(0xF000, 0x0030),	two(0xF1C0, 0xE078),	"IiF8F3F7", mfloat },
{"fsincos.x",	two(0xF000, 0x4830),	two(0xF1C0, 0xFC78),	"Ii;xF3F7", mfloat },

{"fsqrt.b",	two(0xF000, 0x5804),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fsqrt.d",	two(0xF000, 0x5404),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fsqrt.l",	two(0xF000, 0x4004),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fsqrt.p",	two(0xF000, 0x4C04),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fsqrt.s",	two(0xF000, 0x4404),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fsqrt.w",	two(0xF000, 0x5004),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fsqrt.x",	two(0xF000, 0x0004),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fsqrt.x",	two(0xF000, 0x4804),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fsqrt.x",	two(0xF000, 0x0004),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fssqrt.b",	two(0xF000, 0x5841),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fssqrt.d",	two(0xF000, 0x5441),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fssqrt.l",	two(0xF000, 0x4041),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fssqrt.p",	two(0xF000, 0x4C41),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fssqrt.s",	two(0xF000, 0x4441),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fssqrt.w",	two(0xF000, 0x5041),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fssqrt.x",	two(0xF000, 0x0041),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fssqrt.x",	two(0xF000, 0x4841),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{"fssqrt.x",	two(0xF000, 0x0041),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{"fdsqrt.b",	two(0xF000, 0x5845),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fdsqrt.d",	two(0xF000, 0x5445),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fdsqrt.l",	two(0xF000, 0x4045),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fdsqrt.p",	two(0xF000, 0x4C45),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fdsqrt.s",	two(0xF000, 0x4445),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fdsqrt.w",	two(0xF000, 0x5045),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fdsqrt.x",	two(0xF000, 0x0045),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fdsqrt.x",	two(0xF000, 0x4845),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{"fdsqrt.x",	two(0xF000, 0x0045),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{"fsub.b",	two(0xF000, 0x5828),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"fsub.d",	two(0xF000, 0x5428),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"fsub.l",	two(0xF000, 0x4028),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"fsub.p",	two(0xF000, 0x4C28),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"fsub.s",	two(0xF000, 0x4428),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"fsub.w",	two(0xF000, 0x5028),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"fsub.x",	two(0xF000, 0x0028),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"fsub.x",	two(0xF000, 0x4828),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"fsub.x",	two(0xF000, 0x0028),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"fssub.b",	two(0xF000, 0x5838),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fssub.d",	two(0xF000, 0x5438),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fssub.l",	two(0xF000, 0x4038),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fssub.p",	two(0xF000, 0x4C38),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fssub.s",	two(0xF000, 0x4438),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fssub.w",	two(0xF000, 0x5038),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fssub.x",	two(0xF000, 0x0038),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fssub.x",	two(0xF000, 0x4838),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{"fssub.x",	two(0xF000, 0x0038),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{"fdsub.b",	two(0xF000, 0x583c),	two(0xF1C0, 0xFC7F),	"Ii;bF7", m68040 },
{"fdsub.d",	two(0xF000, 0x543c),	two(0xF1C0, 0xFC7F),	"Ii;FF7", m68040 },
{"fdsub.l",	two(0xF000, 0x403c),	two(0xF1C0, 0xFC7F),	"Ii;lF7", m68040 },
{"fdsub.p",	two(0xF000, 0x4C3c),	two(0xF1C0, 0xFC7F),	"Ii;pF7", m68040 },
{"fdsub.s",	two(0xF000, 0x443c),	two(0xF1C0, 0xFC7F),	"Ii;fF7", m68040 },
{"fdsub.w",	two(0xF000, 0x503c),	two(0xF1C0, 0xFC7F),	"Ii;wF7", m68040 },
{"fdsub.x",	two(0xF000, 0x003c),	two(0xF1C0, 0xE07F),	"IiF8F7", m68040 },
{"fdsub.x",	two(0xF000, 0x483c),	two(0xF1C0, 0xFC7F),	"Ii;xF7", m68040 },
{"fdsub.x",	two(0xF000, 0x003c),	two(0xF1C0, 0xE07F),	"IiFt",   m68040 },

{"ftan.b",	two(0xF000, 0x580F),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"ftan.d",	two(0xF000, 0x540F),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"ftan.l",	two(0xF000, 0x400F),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"ftan.p",	two(0xF000, 0x4C0F),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"ftan.s",	two(0xF000, 0x440F),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"ftan.w",	two(0xF000, 0x500F),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"ftan.x",	two(0xF000, 0x000F),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"ftan.x",	two(0xF000, 0x480F),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"ftan.x",	two(0xF000, 0x000F),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"ftanh.b",	two(0xF000, 0x5809),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"ftanh.d",	two(0xF000, 0x5409),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"ftanh.l",	two(0xF000, 0x4009),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"ftanh.p",	two(0xF000, 0x4C09),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"ftanh.s",	two(0xF000, 0x4409),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"ftanh.w",	two(0xF000, 0x5009),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"ftanh.x",	two(0xF000, 0x0009),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"ftanh.x",	two(0xF000, 0x4809),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"ftanh.x",	two(0xF000, 0x0009),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"ftentox.b",	two(0xF000, 0x5812),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"ftentox.d",	two(0xF000, 0x5412),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"ftentox.l",	two(0xF000, 0x4012),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"ftentox.p",	two(0xF000, 0x4C12),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"ftentox.s",	two(0xF000, 0x4412),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"ftentox.w",	two(0xF000, 0x5012),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"ftentox.x",	two(0xF000, 0x0012),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"ftentox.x",	two(0xF000, 0x4812),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"ftentox.x",	two(0xF000, 0x0012),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

{"ftrapeq",	two(0xF07C, 0x0001),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapf",	two(0xF07C, 0x0000),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapge",	two(0xF07C, 0x0013),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapgl",	two(0xF07C, 0x0016),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapgle",	two(0xF07C, 0x0017),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapgt",	two(0xF07C, 0x0012),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftraple",	two(0xF07C, 0x0015),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftraplt",	two(0xF07C, 0x0014),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapne",	two(0xF07C, 0x000E),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
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
{"ftrapst",	two(0xF07C, 0x001F),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapt",	two(0xF07C, 0x000F),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapueq",	two(0xF07C, 0x0009),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapuge",	two(0xF07C, 0x000B),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapugt",	two(0xF07C, 0x000A),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapule",	two(0xF07C, 0x000D),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapult",	two(0xF07C, 0x000C),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
{"ftrapun",	two(0xF07C, 0x0008),	two(0xF1FF, 0xFFFF),	"Ii", mfloat },
        
{"ftrapeq.w",	two(0xF07A, 0x0001),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapf.w",	two(0xF07A, 0x0000),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapge.w",	two(0xF07A, 0x0013),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapgl.w",	two(0xF07A, 0x0016),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapgle.w",	two(0xF07A, 0x0017),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapgt.w",	two(0xF07A, 0x0012),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftraple.w",	two(0xF07A, 0x0015),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftraplt.w",	two(0xF07A, 0x0014),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapne.w",	two(0xF07A, 0x000E),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapnge.w",	two(0xF07A, 0x001C),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapngl.w",	two(0xF07A, 0x0019),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapngle.w",	two(0xF07A, 0x0018),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapngt.w",	two(0xF07A, 0x001D),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapnle.w",	two(0xF07A, 0x001A),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapnlt.w",	two(0xF07A, 0x001B),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapoge.w",	two(0xF07A, 0x0003),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapogl.w",	two(0xF07A, 0x0006),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapogt.w",	two(0xF07A, 0x0002),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapole.w",	two(0xF07A, 0x0005),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapolt.w",	two(0xF07A, 0x0004),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapor.w",	two(0xF07A, 0x0007),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapseq.w",	two(0xF07A, 0x0011),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapsf.w",	two(0xF07A, 0x0010),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapsne.w",	two(0xF07A, 0x001E),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapst.w",	two(0xF07A, 0x001F),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapt.w",	two(0xF07A, 0x000F),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapueq.w",	two(0xF07A, 0x0009),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapuge.w",	two(0xF07A, 0x000B),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapugt.w",	two(0xF07A, 0x000A),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapule.w",	two(0xF07A, 0x000D),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapult.w",	two(0xF07A, 0x000C),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },
{"ftrapun.w",	two(0xF07A, 0x0008),	two(0xF1FF, 0xFFFF),	"Ii^w", mfloat },

{"ftrapeq.l",	two(0xF07B, 0x0001),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapf.l",	two(0xF07B, 0x0000),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapge.l",	two(0xF07B, 0x0013),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapgl.l",	two(0xF07B, 0x0016),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapgle.l",	two(0xF07B, 0x0017),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapgt.l",	two(0xF07B, 0x0012),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftraple.l",	two(0xF07B, 0x0015),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftraplt.l",	two(0xF07B, 0x0014),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapne.l",	two(0xF07B, 0x000E),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapnge.l",	two(0xF07B, 0x001C),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapngl.l",	two(0xF07B, 0x0019),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapngle.l",	two(0xF07B, 0x0018),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapngt.l",	two(0xF07B, 0x001D),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapnle.l",	two(0xF07B, 0x001A),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapnlt.l",	two(0xF07B, 0x001B),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapoge.l",	two(0xF07B, 0x0003),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapogl.l",	two(0xF07B, 0x0006),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapogt.l",	two(0xF07B, 0x0002),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapole.l",	two(0xF07B, 0x0005),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapolt.l",	two(0xF07B, 0x0004),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapor.l",	two(0xF07B, 0x0007),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapseq.l",	two(0xF07B, 0x0011),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapsf.l",	two(0xF07B, 0x0010),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapsne.l",	two(0xF07B, 0x001E),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapst.l",	two(0xF07B, 0x001F),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapt.l",	two(0xF07B, 0x000F),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapueq.l",	two(0xF07B, 0x0009),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapuge.l",	two(0xF07B, 0x000B),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapugt.l",	two(0xF07B, 0x000A),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapule.l",	two(0xF07B, 0x000D),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapult.l",	two(0xF07B, 0x000C),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },
{"ftrapun.l",	two(0xF07B, 0x0008),	two(0xF1FF, 0xFFFF),	"Ii^l", mfloat },

{"ftst.b",	two(0xF000, 0x583A),	two(0xF1C0, 0xFC7F),	"Ii;b", mfloat },
{"ftst.d",	two(0xF000, 0x543A),	two(0xF1C0, 0xFC7F),	"Ii;F", mfloat },
{"ftst.l",	two(0xF000, 0x403A),	two(0xF1C0, 0xFC7F),	"Ii;l", mfloat },
{"ftst.p",	two(0xF000, 0x4C3A),	two(0xF1C0, 0xFC7F),	"Ii;p", mfloat },
{"ftst.s",	two(0xF000, 0x443A),	two(0xF1C0, 0xFC7F),	"Ii;f", mfloat },
{"ftst.w",	two(0xF000, 0x503A),	two(0xF1C0, 0xFC7F),	"Ii;w", mfloat },
{"ftst.x",	two(0xF000, 0x003A),	two(0xF1C0, 0xE07F),	"IiF8", mfloat },
{"ftst.x",	two(0xF000, 0x483A),	two(0xF1C0, 0xFC7F),	"Ii;x", mfloat },

{"ftwotox.b",	two(0xF000, 0x5811),	two(0xF1C0, 0xFC7F),	"Ii;bF7", mfloat },
{"ftwotox.d",	two(0xF000, 0x5411),	two(0xF1C0, 0xFC7F),	"Ii;FF7", mfloat },
{"ftwotox.l",	two(0xF000, 0x4011),	two(0xF1C0, 0xFC7F),	"Ii;lF7", mfloat },
{"ftwotox.p",	two(0xF000, 0x4C11),	two(0xF1C0, 0xFC7F),	"Ii;pF7", mfloat },
{"ftwotox.s",	two(0xF000, 0x4411),	two(0xF1C0, 0xFC7F),	"Ii;fF7", mfloat },
{"ftwotox.w",	two(0xF000, 0x5011),	two(0xF1C0, 0xFC7F),	"Ii;wF7", mfloat },
{"ftwotox.x",	two(0xF000, 0x0011),	two(0xF1C0, 0xE07F),	"IiF8F7", mfloat },
{"ftwotox.x",	two(0xF000, 0x4811),	two(0xF1C0, 0xFC7F),	"Ii;xF7", mfloat },
{"ftwotox.x",	two(0xF000, 0x0011),	two(0xF1C0, 0xE07F),	"IiFt",   mfloat },

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


{"jmp.s",		one(0047300),		one(0177700),		"!s",   m68000up },
{"jmp.l",		one(0047300),		one(0177700),		"!s",   m68000up },


{"jmp",		one(0047300),		one(0177700),		"!s",   m68000up },
{"jsr",		one(0047200),		one(0177700),		"!s",   m68000up },
{"lea",		one(0040700),		one(0170700),		"!sAd", m68000up },
{"lea.l",	one(0040700),		one(0170700),		"!sAd", m68000up },	/* allow lea as leal */

{"link.w",	one(0047120),		one(0177770),		"As#w", m68000up },
{"link.l",	one(0044010),		one(0177770),		"As#l", m68020up },
{"link",	one(0047120),		one(0177770),		"As#w", m68000up },
{"link",	one(0044010),		one(0177770),		"As#l", m68020up },

{"lsl.b",	one(0160410),		one(0170770),		"QdDs", m68000up },	/* lsrb #Q,	Ds */
{"lsl.b",	one(0160450),		one(0170770),		"DdDs", m68000up },	/* lsrb Dd,	Ds */
{"lsl.w",	one(0160510),		one(0170770),		"QdDs", m68000up },	/* lsrb #Q,	Ds */
{"lsl.w",	one(0160550),		one(0170770),		"DdDs", m68000up },	/* lsrb Dd,	Ds */
{"lsl.w",	one(0161700),		one(0177700),		"~s",   m68000up },	/* Shift memory */
{"lsl.l",	one(0160610),		one(0170770),		"QdDs", m68000up },	/* lsrb #Q,	Ds */
{"lsl.l",	one(0160650),		one(0170770),		"DdDs", m68000up },	/* lsrb Dd,	Ds */

{"lsr.b",	one(0160010),		one(0170770),		"QdDs", m68000up }, /* lsrb #Q,	Ds */
{"lsr.b",	one(0160050),		one(0170770),		"DdDs", m68000up },	/* lsrb Dd,	Ds */
{"lsr.l",	one(0160210),		one(0170770),		"QdDs", m68000up },	/* lsrb #Q,	Ds */
{"lsr.l",	one(0160250),		one(0170770),		"DdDs", m68000up },	/* lsrb #Q,	Ds */
{"lsr.w",	one(0160110),		one(0170770),		"QdDs", m68000up },	/* lsrb #Q,	Ds */
{"lsr.w",	one(0160150),		one(0170770),		"DdDs", m68000up },	/* lsrb #Q,	Ds */
{"lsr.w",	one(0161300),		one(0177700),		"~s",   m68000up },	/* Shift memory */

{"movea.l",	one(0020100),		one(0170700),		"*lAd", m68000up },
{"movea.w",	one(0030100),		one(0170700),		"*wAd", m68000up },
{"move.b",	one(0010000),		one(0170000),		";b$d", m68000up },	/* move */
{"move.l",	one(0070000),		one(0170400),		"MsDd", m68000up },	/* moveq written as move */
{"move.l",	one(0020000),		one(0170000),		"*l$d", m68000up },
{"move.l",	one(0020100),		one(0170700),		"*lAd", m68000up },
{"move.l",	one(0047140),		one(0177770),		"AsUd", m68000up },	/* move to USP */
{"move.l",	one(0047150),		one(0177770),		"UdAs", m68000up },	/* move from USP */

{"movec",	one(0047173),		one(0177777),		"R1Jj", m68010up },
{"movec",	one(0047173),		one(0177777),		"R1#j", m68010up },
{"movec",	one(0047172),		one(0177777),		"JjR1", m68010up },
{"movec",	one(0047172),		one(0177777),		"#jR1", m68010up },



{"movem",	one(0044200),		one(0177700),		"Lw&s", m68000up },	/* movem reg to mem. */
{"movem",	one(0044240),		one(0177770),		"lw-s", m68000up },	/* movem reg to autodecrement. */
{"movem",	one(0046200),		one(0177700),		"!sLw", m68000up },	/* movem mem to reg. */
{"movem",	one(0046230),		one(0177770),		"+sLw", m68000up },	/* movem autoinc to reg. */

{"movem",	one(0044200),		one(0177700),		"#w&s", m68000up },	/* movem reg to mem. */
{"movem",	one(0044240),		one(0177770),		"#w-s", m68000up },	/* movem reg to autodecrement. */
{"movem",	one(0046200),		one(0177700),		"!s#w", m68000up },	/* movem mem to reg. */
{"movem",	one(0046230),		one(0177770),		"+s#w", m68000up },	/* movem autoinc to reg. */



/* JF added these next four for the assembler */
{"movem.l",	one(0044300),		one(0177700),		"Lw&s", m68000up },	/* movem reg to mem. */
{"movem.l",	one(0044340),		one(0177770),		"lw-s", m68000up },	/* movem reg to autodecrement. */
{"movem.l",	one(0046300),		one(0177700),		"!sLw", m68000up },	/* movem mem to reg. */
{"movem.l",	one(0046330),		one(0177770),		"+sLw", m68000up },	/* movem autoinc to reg. */

{"movem.l",	one(0044300),		one(0177700),		"#w&s", m68000up },	/* movem reg to mem. */
{"movem.l",	one(0044340),		one(0177770),		"#w-s", m68000up },	/* movem reg to autodecrement. */
{"movem.l",	one(0046300),		one(0177700),		"!s#w", m68000up },	/* movem mem to reg. */
{"movem.l",	one(0046330),		one(0177770),		"+s#w", m68000up },	/* movem autoinc to reg. */

/* JF added these next four for the assembler */
{"movem.w",	one(0044200),		one(0177700),		"Lw&s", m68000up },	/* movem reg to mem. */
{"movem.w",	one(0044240),		one(0177770),		"lw-s", m68000up },	/* movem reg to autodecrement. */
{"movem.w",	one(0046200),		one(0177700),		"!sLw", m68000up },	/* movem mem to reg. */
{"movem.w",	one(0046230),		one(0177770),		"+sLw", m68000up },	/* movem autoinc to reg. */

{"movem.w",	one(0044200),		one(0177700),		"#w&s", m68000up },	/* movem reg to mem. */
{"movem.w",	one(0044240),		one(0177770),		"#w-s", m68000up },	/* movem reg to autodecrement. */
{"movem.w",	one(0046200),		one(0177700),		"!s#w", m68000up },	/* movem mem to reg. */
{"movem.w",	one(0046230),		one(0177770),		"+s#w", m68000up },	/* movem autoinc to reg. */


{"movep",	one(0000410),		one(0170770),		"dsDd", m68000up },	/* memory to register */
{"movep",	one(0000610),		one(0170770),		"Ddds", m68000up },	/* register to memory */

{"movep.l",	one(0000510),		one(0170770),		"dsDd", m68000up },	/* memory to register */
{"movep.l",	one(0000710),		one(0170770),		"Ddds", m68000up },	/* register to memory */
{"movep.w",	one(0000410),		one(0170770),		"dsDd", m68000up },	/* memory to register */
{"movep.w",	one(0000610),		one(0170770),		"Ddds", m68000up },	/* register to memory */
{"move.q",	one(0070000),		one(0170400),		"MsDd", m68000up },
{"move.w",	one(0030000),		one(0170000),		"*w$d", m68000up },
{"move.w",	one(0030100),		one(0170700),		"*wAd", m68000up },	/* movea,	written as move */
{"move.w",	one(0040300),		one(0177700),		"Ss$s", m68000up },	/* Move from sr */
{"move.w",	one(0041300),		one(0177700),		"Cs$s", m68010up },	/* Move from ccr */
{"move.w",	one(0042300),		one(0177700),		";wCd", m68000up },	/* move to ccr */
{"move.w",	one(0043300),		one(0177700),		";wSd", m68000up },	/* move to sr */


{"moves.b",	two(0007000, 0),	two(0177700, 07777),	"~sR1", m68010up },	 /* moves from memory */
{"moves.b",	two(0007000, 04000),	two(0177700, 07777),	"R1~s", m68010up },	 /* moves to memory */
{"moves.l",	two(0007200, 0),	two(0177700, 07777),	"~sR1", m68010up },	 /* moves from memory */
{"moves.l",	two(0007200, 04000),	two(0177700, 07777),	"R1~s", m68010up },	 /* moves to memory */
{"moves.w",	two(0007100, 0),	two(0177700, 07777),	"~sR1", m68010up },	 /* moves from memory */
{"moves.w",	two(0007100, 04000),	two(0177700, 07777),	"R1~s", m68010up },	 /* moves to memory */

{"move16",	two(0xf620, 0x8000),	two(0xfff8, 0x8fff),	"+s+1", m68040 },
{"move16",	one(0xf600),	one(0xfff8),	"+s_L", m68040 },
{"move16",	one(0xf608),	one(0xfff8),	"_L+s", m68040 },
{"move16",	one(0xf610),	one(0xfff8),	"as_L", m68040 },
{"move16",	one(0xf618),	one(0xfff8),	"_Las", m68040 },

{"muls.l",	two(0046000, 004000),	two(0177700, 0107770),	";lD1", m68020up },
{"muls.l",	two(0046000, 006000),	two(0177700, 0107770),	";lD3D1", m68020up },
{"muls.w",	one(0140700),		one(0170700),		";wDd", m68000up },
{"muls",	one(0140700),		one(0170700),		";wDd", m68000up },
{"mulu.l",	two(0046000, 000000),	two(0177700, 0107770),	";lD1", m68020up },
{"mulu.l",	two(0046000, 002000),	two(0177700, 0107770),	";lD3D1", m68020up },
{"mulu.w",	one(0140300),		one(0170700),		";wDd", m68000up },
{"mulu",	one(0140300),		one(0170700),		";wDd", m68000up },
{"nbcd",	one(0044000),		one(0177700),		"$s", m68000up },

{"neg",		one(0042200),		one(0177700),		"$s", m68000up },

{"neg.b",	one(0042000),		one(0177700),		"$s", m68000up },
{"neg.l",	one(0042200),		one(0177700),		"$s", m68000up },
{"neg.w",	one(0042100),		one(0177700),		"$s", m68000up },

{"neg.x",	one(0040200),		one(0177700),		"$s", m68000up },

{"negx.b",	one(0040000),		one(0177700),		"$s", m68000up },
{"negx.l",	one(0040200),		one(0177700),		"$s", m68000up },
{"negx.w",	one(0040100),		one(0177700),		"$s", m68000up },
{"nop",		one(0047161),		one(0177777),		"", m68000up },

{"not",		one(0043200),		one(0177700),		"$s", m68000up },

{"not.b",	one(0043000),		one(0177700),		"$s", m68000up },
{"not.l",	one(0043200),		one(0177700),		"$s", m68000up },
{"not.w",	one(0043100),		one(0177700),		"$s", m68000up },

{"ori",		one(0000174),		one(0177777),		"#wSs", m68000up },	/* ori to sr */

{"or",		one(0000100),		one(0177700),		"#w$s", m68000up },
{"or",		one(0000174),		one(0177777),		"#wSs", m68000up },	/* ori to sr */
{"or",		one(0100100),		one(0170700),		";wDd", m68000up },	/* memory to register */
{"or",		one(0100500),		one(0170700),		"Dd~s", m68000up },	/* register to memory */


{"or.b",		one(0000000),		one(0177700),		"#b$s", m68000up },	/* ori written as or */
{"or.b",		one(0000074),		one(0177777),		"#bCs", m68000up },	/* ori to ccr */
{"or.b",		one(0100000),		one(0170700),		";bDd", m68000up },	/* memory to register */
{"or.b",		one(0100400),		one(0170700),		"Dd~s", m68000up },	/* register to memory */
{"ori.b",	one(0000000),		one(0177700),		"#b$s", m68000up },
{"ori.b",	one(0000074),		one(0177777),		"#bCs", m68000up },	/* ori to ccr */
{"ori.l",	one(0000200),		one(0177700),		"#l$s", m68000up },
{"ori.w",	one(0000100),		one(0177700),		"#w$s", m68000up },
{"ori.w",	one(0000174),		one(0177777),		"#wSs", m68000up },	/* ori to sr */
{"or.l",		one(0000200),		one(0177700),		"#l$s", m68000up },
{"or.l",		one(0100200),		one(0170700),		";lDd", m68000up },	/* memory to register */
{"or.l",		one(0100600),		one(0170700),		"Dd~s", m68000up },	/* register to memory */
{"or.w",		one(0000100),		one(0177700),		"#w$s", m68000up },
{"or.w",		one(0000174),		one(0177777),		"#wSs", m68000up },	/* ori to sr */
{"or.w",		one(0100100),		one(0170700),		";wDd", m68000up },	/* memory to register */
{"or.w",		one(0100500),		one(0170700),		"Dd~s", m68000up },	/* register to memory */

{"pack",	one(0100500),		one(0170770),		"DsDd#w", m68020up },	/* pack Ds,	Dd,	#w */
{"pack",	one(0100510),		one(0170770),		"-s-d#w", m68020up },	/* pack -(As),	-(Ad),	#w */

#ifndef NO_68851
{"pbac",	one(0xf0c7),		one(0xffbf),		"Bc", m68851 },
{"pbac.w",	one(0xf087),		one(0xffbf),		"Bc", m68851 },
{"pbas",	one(0xf0c6),		one(0xffbf),		"Bc", m68851 },
{"pbas.w",	one(0xf086),		one(0xffbf),		"Bc", m68851 },
{"pbbc",	one(0xf0c1),		one(0xffbf),		"Bc", m68851 },
{"pbbc.w",	one(0xf081),		one(0xffbf),		"Bc", m68851 },
{"pbbs",	one(0xf0c0),		one(0xffbf),		"Bc", m68851 },
{"pbbs.w",	one(0xf080),		one(0xffbf),		"Bc", m68851 },
{"pbcc",	one(0xf0cf),		one(0xffbf),		"Bc", m68851 },
{"pbcc.w",	one(0xf08f),		one(0xffbf),		"Bc", m68851 },
{"pbcs",	one(0xf0ce),		one(0xffbf),		"Bc", m68851 },
{"pbcs.w",	one(0xf08e),		one(0xffbf),		"Bc", m68851 },
{"pbgc",	one(0xf0cd),		one(0xffbf),		"Bc", m68851 },
{"pbgc.w",	one(0xf08d),		one(0xffbf),		"Bc", m68851 },
{"pbgs",	one(0xf0cc),		one(0xffbf),		"Bc", m68851 },
{"pbgs.w",	one(0xf08c),		one(0xffbf),		"Bc", m68851 },
{"pbic",	one(0xf0cb),		one(0xffbf),		"Bc", m68851 },
{"pbic.w",	one(0xf08b),		one(0xffbf),		"Bc", m68851 },
{"pbis",	one(0xf0ca),		one(0xffbf),		"Bc", m68851 },
{"pbis.w",	one(0xf08a),		one(0xffbf),		"Bc", m68851 },
{"pblc",	one(0xf0c3),		one(0xffbf),		"Bc", m68851 },
{"pblc.w",	one(0xf083),		one(0xffbf),		"Bc", m68851 },
{"pbls",	one(0xf0c2),		one(0xffbf),		"Bc", m68851 },
{"pbls.w",	one(0xf082),		one(0xffbf),		"Bc", m68851 },
{"pbsc",	one(0xf0c5),		one(0xffbf),		"Bc", m68851 },
{"pbsc.w",	one(0xf085),		one(0xffbf),		"Bc", m68851 },
{"pbss",	one(0xf0c4),		one(0xffbf),		"Bc", m68851 },
{"pbss.w",	one(0xf084),		one(0xffbf),		"Bc", m68851 },
{"pbwc",	one(0xf0c9),		one(0xffbf),		"Bc", m68851 },
{"pbwc.w",	one(0xf089),		one(0xffbf),		"Bc", m68851 },
{"pbws",	one(0xf0c8),		one(0xffbf),		"Bc", m68851 },
{"pbws.w",	one(0xf088),		one(0xffbf),		"Bc", m68851 },

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
{"pflush",	one(0xf500),		one(0xfff8), 		"As",		m68040 },

{"pflushan",	one(0xf518),		one(0xfff8),		"",		m68040 },
{"pflushn",	one(0xf508),		one(0xfff8),		"As",		m68040 },

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

{"ptestr",	one(0xf568),		one(0xfff8),		"As",		m68040 },

{"ptestw",	two(0xf000, 0x8010),	two(0xffc0, 0xe3f0),	"T3&sQ8",	m68030 | m68851 },
{"ptestw",	two(0xf000, 0x8110),	two(0xffc0, 0xe310),	"T3&sQ8A9",	m68030 | m68851 },
{"ptestw",	two(0xf000, 0x8008),	two(0xffc0, 0xe3f8),	"D3&sQ8",	m68030 | m68851 },
{"ptestw",	two(0xf000, 0x8108),	two(0xffc0, 0xe318),	"D3&sQ8A9",	m68030 | m68851 },
{"ptestw",	two(0xf000, 0x8000),	two(0xffc0, 0xe3fe),	"f3&sQ8",	m68030 | m68851 },
{"ptestw",	two(0xf000, 0x8100),	two(0xffc0, 0xe31e),	"f3&sQ8A9",	m68030 | m68851 },

{"ptestw",	one(0xf548),		one(0xfff8),		"As",		m68040 },

{"ptrapac.w",	two(0xf07a, 0x0007),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapac.l",	two(0xf07b, 0x0007),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapac",	two(0xf07c, 0x0007),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapas.w",	two(0xf07a, 0x0006),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapas.l",	two(0xf07b, 0x0006),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapas",	two(0xf07c, 0x0006),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapbc.w",	two(0xf07a, 0x0001),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapbc.l",	two(0xf07b, 0x0001),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapbc",	two(0xf07c, 0x0001),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapbs.w",	two(0xf07a, 0x0000),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapbs.l",	two(0xf07b, 0x0000),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapbs",	two(0xf07c, 0x0000),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapcc.w",	two(0xf07a, 0x000f),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapcc.l",	two(0xf07b, 0x000f),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapcc",	two(0xf07c, 0x000f),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapcs.w",	two(0xf07a, 0x000e),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapcs.l",	two(0xf07b, 0x000e),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapcs",	two(0xf07c, 0x000e),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapgc.w",	two(0xf07a, 0x000d),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapgc.l",	two(0xf07b, 0x000d),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapgc",	two(0xf07c, 0x000d),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapgs.w",	two(0xf07a, 0x000c),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapgs.l",	two(0xf07b, 0x000c),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapgs",	two(0xf07c, 0x000c),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapic.w",	two(0xf07a, 0x000b),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapic.l",	two(0xf07b, 0x000b),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapic",	two(0xf07c, 0x000b),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapis.w",	two(0xf07a, 0x000a),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapis.l",	two(0xf07b, 0x000a),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapis",	two(0xf07c, 0x000a),	two(0xffff, 0xffff),	"",   m68851 },

{"ptraplc.w",	two(0xf07a, 0x0003),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptraplc.l",	two(0xf07b, 0x0003),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptraplc",	two(0xf07c, 0x0003),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapls.w",	two(0xf07a, 0x0002),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapls.l",	two(0xf07b, 0x0002),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapls",	two(0xf07c, 0x0002),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapsc.w",	two(0xf07a, 0x0005),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapsc.l",	two(0xf07b, 0x0005),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapsc",	two(0xf07c, 0x0005),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapss.w",	two(0xf07a, 0x0004),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapss.l",	two(0xf07b, 0x0004),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapss",	two(0xf07c, 0x0004),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapwc.w",	two(0xf07a, 0x0009),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapwc.l",	two(0xf07b, 0x0009),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapwc",	two(0xf07c, 0x0009),	two(0xffff, 0xffff),	"",   m68851 },

{"ptrapws.w",	two(0xf07a, 0x0008),	two(0xffff, 0xffff),	"#w", m68851 },
{"ptrapws.l",	two(0xf07b, 0x0008),	two(0xffff, 0xffff),	"#l", m68851 },
{"ptrapws",	two(0xf07c, 0x0008),	two(0xffff, 0xffff),	"",   m68851 },

{"pvalid",	two(0xf000, 0x2800),	two(0xffc0, 0xffff),	"Vs&s", m68851 },
{"pvalid",	two(0xf000, 0x2c00),	two(0xffc0, 0xfff8),	"A3&s", m68851 },

#endif /* NO_68851 */

{"reset",	one(0047160),		one(0177777),		"", m68000up },

{"rol.b",	one(0160430),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{"rol.b",	one(0160470),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{"rol.l",	one(0160630),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{"rol.l",	one(0160670),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{"rol.w",	one(0160530),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{"rol.w",	one(0160570),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{"rol.w",	one(0163700),		one(0177700),		"~s",   m68000up },	/* Rotate memory */
{"ror.b",	one(0160030),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{"ror.b",	one(0160070),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{"ror.l",	one(0160230),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{"ror.l",	one(0160270),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{"ror.w",	one(0160130),		one(0170770),		"QdDs", m68000up },	/* rorb #Q,	Ds */
{"ror.w",	one(0160170),		one(0170770),		"DdDs", m68000up },	/* rorb Dd,	Ds */
{"ror.w",	one(0163300),		one(0177700),		"~s",   m68000up },	/* Rotate memory */

{"roxl.b",	one(0160420),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{"roxl.b",	one(0160460),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{"roxl.l",	one(0160620),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{"roxl.l",	one(0160660),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{"roxl.w",	one(0160520),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{"roxl.w",	one(0160560),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{"roxl.w",	one(0162700),		one(0177700),		"~s",   m68000up },	/* Rotate memory */
{"roxr.b",	one(0160020),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{"roxr.b",	one(0160060),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{"roxr.l",	one(0160220),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{"roxr.l",	one(0160260),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{"roxr.w",	one(0160120),		one(0170770),		"QdDs", m68000up },	/* roxrb #Q,	Ds */
{"roxr.w",	one(0160160),		one(0170770),		"DdDs", m68000up },	/* roxrb Dd,	Ds */
{"roxr.w",	one(0162300),		one(0177700),		"~s",   m68000up },	/* Rotate memory */

{"rtd",		one(0047164),		one(0177777),		"#w", m68010up },
{"rte",		one(0047163),		one(0177777),		"",   m68000up },
{"rtm",		one(0003300),		one(0177760),		"Rs", m68020 },
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

{"suba.l",	one(0110700),		one(0170700),		"*lAd", m68000up },
{"suba.w",	one(0110300),		one(0170700),		"*wAd", m68000up },
{"sub.b",	one(0050400),		one(0170700),		"Qd%s", m68000up },	/* subq written as sub */
{"sub.b",	one(0002000),		one(0177700),		"#b$s", m68000up },	/* subi written as sub */
{"sub.b",	one(0110000),		one(0170700),		";bDd", m68000up },	/* subb ? ?,	Dd */
{"sub.b",	one(0110400),		one(0170700),		"Dd~s", m68000up },	/* subb Dd,	? ? */

{"subi",	one(0002200),		one(0177700),		"#l$s", m68000up },

{"sub",	one(0050500),		one(0170700),		"Qd%s", m68000up },
{"sub",	one(0002100),		one(0177700),		"#w$s", m68000up },
{"sub",	one(0110100),		one(0170700),		"*wDd", m68000up },
{"sub",	one(0110300),		one(0170700),		"*wAd", m68000up },	/* suba written as sub */
{"sub",	one(0110500),		one(0170700),		"Dd~s", m68000up },


{"subi.b",	one(0002000),		one(0177700),		"#b$s", m68000up },
{"subi.l",	one(0002200),		one(0177700),		"#l$s", m68000up },
{"subi.w",	one(0002100),		one(0177700),		"#w$s", m68000up },
{"sub.l",	one(0050600),		one(0170700),		"Qd%s", m68000up },
{"suba",	one(0110700),		one(0170700),		"*lAd", m68000up },
{"sub.l",	one(0002200),		one(0177700),		"#l$s", m68000up },
{"sub.l",	one(0110700),		one(0170700),		"*lAd", m68000up },
{"sub.l",	one(0110200),		one(0170700),		"*lDd", m68000up },
{"sub.l",	one(0110600),		one(0170700),		"Dd~s", m68000up },
{"subq.b",	one(0050400),		one(0170700),		"Qd%s", m68000up },
{"subq.b",	one(0002000),		one(0177700),		"#b$s", m68000up }, /* FIXNIEL: sub instead of subq because too many frags */
{"subq.l",	one(0050600),		one(0170700),		"Qd%s", m68000up },
{"subq.l",	one(0002200),		one(0177700),		"#l$s", m68000up }, /* FIXNIEL: sub instead of subq because too many frags */
{"subq.w",	one(0050500),		one(0170700),		"Qd%s", m68000up },
{"subq.w",	one(0002100),		one(0177700),		"#w$s", m68000up }, /* FIXNIEL: sub instead of subq because too many frags */
{"sub.w",	one(0050500),		one(0170700),		"Qd%s", m68000up },
{"sub.w",	one(0002100),		one(0177700),		"#w$s", m68000up },
{"sub.w",	one(0110100),		one(0170700),		"*wDd", m68000up },
{"sub.w",	one(0110300),		one(0170700),		"*wAd", m68000up },	/* suba written as sub */
{"sub.w",	one(0110500),		one(0170700),		"Dd~s", m68000up },

{"subx",	one(0110500),		one(0170770),		"DsDd", m68000up },
{"subx",	one(0110510),		one(0170770),		"-s-d", m68000up },

{"subx.b",	one(0110400),		one(0170770),		"DsDd", m68000up },	/* subxb Ds,	Dd */
{"subx.b",	one(0110410),		one(0170770),		"-s-d", m68000up },	/* subxb -(As),	-(Ad) */
{"subx.l",	one(0110600),		one(0170770),		"DsDd", m68000up },
{"subx.l",	one(0110610),		one(0170770),		"-s-d", m68000up },
{"subx.w",	one(0110500),		one(0170770),		"DsDd", m68000up },
{"subx.w",	one(0110510),		one(0170770),		"-s-d", m68000up },

{"swap",	one(0044100),		one(0177770),		"Ds", m68000up },
{"swap.w",	one(0044100),		one(0177770),		"Ds", m68000up },
	
{"tas",		one(0045300),		one(0177700),		"$s", m68000up },

{"tpcc",	one(0047100),		one(0177760),		"Ts", m68000up },

{"trap",	one(0047100),		one(0177760),		"Ts", m68000up },


{"tcc",		one(0052374),		one(0177777),		"", m68020up },

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

{"trapcc.w",	one(0052372),		one(0177777),		"#w", m68020up },
{"trapcs.w",	one(0052772),		one(0177777),		"#w", m68020up },
{"trapeq.w",	one(0053772),		one(0177777),		"#w", m68020up },
{"trapf.w",	one(0050772),		one(0177777),		"#w", m68020up },
{"trapge.w",	one(0056372),		one(0177777),		"#w", m68020up },
{"trapgt.w",	one(0057372),		one(0177777),		"#w", m68020up },
{"traphi.w",	one(0051372),		one(0177777),		"#w", m68020up },
{"traple.w",	one(0057772),		one(0177777),		"#w", m68020up },
{"trapls.w",	one(0051772),		one(0177777),		"#w", m68020up },
{"traplt.w",	one(0056772),		one(0177777),		"#w", m68020up },
{"trapmi.w",	one(0055772),		one(0177777),		"#w", m68020up },
{"trapne.w",	one(0053372),		one(0177777),		"#w", m68020up },
{"trappl.w",	one(0055372),		one(0177777),		"#w", m68020up },
{"trapt.w",	one(0050372),		one(0177777),		"#w", m68020up },
{"trapvc.w",	one(0054372),		one(0177777),		"#w", m68020up },
{"trapvs.w",	one(0054772),		one(0177777),		"#w", m68020up },

{"trapcc.l",	one(0052373),		one(0177777),		"#l", m68020up },
{"trapcs.l",	one(0052773),		one(0177777),		"#l", m68020up },
{"trapeq.l",	one(0053773),		one(0177777),		"#l", m68020up },
{"trapf.l",	one(0050773),		one(0177777),		"#l", m68020up },
{"trapge.l",	one(0056373),		one(0177777),		"#l", m68020up },
{"trapgt.l",	one(0057373),		one(0177777),		"#l", m68020up },
{"traphi.l",	one(0051373),		one(0177777),		"#l", m68020up },
{"traple.l",	one(0057773),		one(0177777),		"#l", m68020up },
{"trapls.l",	one(0051773),		one(0177777),		"#l", m68020up },
{"traplt.l",	one(0056773),		one(0177777),		"#l", m68020up },
{"trapmi.l",	one(0055773),		one(0177777),		"#l", m68020up },
{"trapne.l",	one(0053373),		one(0177777),		"#l", m68020up },
{"trappl.l",	one(0055373),		one(0177777),		"#l", m68020up },
{"trapt.l",	one(0050373),		one(0177777),		"#l", m68020up },
{"trapvc.l",	one(0054373),		one(0177777),		"#l", m68020up },
{"trapvs.l",	one(0054773),		one(0177777),		"#l", m68020up },

{"trapv",	one(0047166),		one(0177777),		"", m68000up },


{"tst",		one(0045200),		one(0177700),		"*l", m68000up },

{"tst.b",	one(0045000),		one(0177700),		";b", m68000up },
{"tst.w",	one(0045100),		one(0177700),		"*w", m68000up },
{"tst.l",	one(0045200),		one(0177700),		"*l", m68000up },

{"unlk",	one(0047130),		one(0177770),		"As", m68000up },
{"unpk",	one(0100600),		one(0170770),		"DsDd#w", m68020up },
{"unpk",	one(0100610),		one(0170770),		"-s-d#w", m68020up },

/* Variable-sized branches */

{"jbsr",	one(0060400),		one(0177400),		"Bg", m68000up },
{"jbsr",	one(0047200),		one(0177700),		"!s", m68000up },
{"jra",		one(0060000),		one(0177400),		"Bg", m68000up },
{"jra",		one(0047300),		one(0177700),		"!s", m68000up },

{"jbra",	one(0060000),		one(0177400),		"Bg", m68000up },
{"jbra",	one(0047300),		one(0177700),		"!s", m68000up },

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

/* aliases */


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


{"movq.l",	one(0070000),		one(0170400),		"MsDd", m68000up },
{"movq.l",	one(0020000),		one(0170000),		"*l$d", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"movq.l",	one(0020100),		one(0170700),		"*lAd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"movq.l",	one(0047140),		one(0177770),		"AsUd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"movq.l",	one(0047150),		one(0177770),		"UdAs", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"moveq.l",	one(0070000),		one(0170400),		"MsDd", m68000up },
{"moveq.l",	one(0020000),		one(0170000),		"*l$d", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"moveq.l",	one(0020100),		one(0170700),		"*lAd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"moveq.l",	one(0047140),		one(0177770),		"AsUd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"moveq.l",	one(0047150),		one(0177770),		"UdAs", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"mova.l",	one(0020100),		one(0170700),		"*lAd", m68000up },
{"mova.w",	one(0030100),		one(0170700),		"*wAd", m68000up },
{"mov.b",	one(0010000),		one(0170000),		";b$d", m68000up },	/* mov */
{"mov.l",	one(0070000),		one(0170400),		"MsDd", m68000up },	/* movq written as mov */
{"mov.l",	one(0020000),		one(0170000),		"*l$d", m68000up },
{"mov.l",	one(0020100),		one(0170700),		"*lAd", m68000up },
{"mov.l",	one(0047140),		one(0177770),		"AsUd", m68000up },	/* mov to USP */
{"mov.l",	one(0047150),		one(0177770),		"UdAs", m68000up },	/* mov from USP */
{"mov.l",	one(0070000),		one(0170400),		"MsDd", m68000up },	/* movq written as mov */
{"mov.l",	one(0020000),		one(0170000),		"*l$d", m68000up },
{"mov.l",	one(0020100),		one(0170700),		"*lAd", m68000up },
{"mov.l",	one(0047140),		one(0177770),		"AsUd", m68000up },	/* mov to USP */
{"mov.l",	one(0047150),		one(0177770),		"UdAs", m68000up },	/* mov from USP */
{"movc",	one(0047173),		one(0177777),		"R1Jj", m68010up },
{"movc",	one(0047173),		one(0177777),		"R1#j", m68010up },
{"movc",	one(0047172),		one(0177777),		"JjR1", m68010up },
{"movc",	one(0047172),		one(0177777),		"#jR1", m68010up },
{"movm.l",	one(0044300),		one(0177700),		"#w&s", m68000up },	/* movm reg to mem. */
{"movm.l",	one(0044340),		one(0177770),		"#w-s", m68000up },	/* movm reg to autodecrement. */
{"movml",	one(0046300),		one(0177700),		"!s#w", m68000up },	/* movm mem to reg. */
{"movml",	one(0046330),		one(0177770),		"+s#w", m68000up },	/* movm autoinc to reg. */
{"movml",	one(0044300),		one(0177700),		"Lw&s", m68000up },	/* movm reg to mem. */
{"movml",	one(0044340),		one(0177770),		"lw-s", m68000up },	/* movm reg to autodecrement. */
{"movml",	one(0046300),		one(0177700),		"!sLw", m68000up },	/* movm mem to reg. */
{"movml",	one(0046330),		one(0177770),		"+sLw", m68000up },	/* movm autoinc to reg. */
{"movm.w",	one(0044200),		one(0177700),		"#w&s", m68000up },	/* movm reg to mem. */
{"movm.w",	one(0044240),		one(0177770),		"#w-s", m68000up },	/* movm reg to autodecrement. */
{"movm.w",	one(0046200),		one(0177700),		"!s#w", m68000up },	/* movm mem to reg. */
{"movm.w",	one(0046230),		one(0177770),		"+s#w", m68000up },	/* movm autoinc to reg. */
{"movm.w",	one(0044200),		one(0177700),		"Lw&s", m68000up },	/* movm reg to mem. */
{"movm.w",	one(0044240),		one(0177770),		"lw-s", m68000up },	/* movm reg to autodecrement. */
{"movm.w",	one(0046200),		one(0177700),		"!sLw", m68000up },	/* movm mem to reg. */
{"movm.w",	one(0046230),		one(0177770),		"+sLw", m68000up },	/* movm autoinc to reg. */
{"movp.l",	one(0000510),		one(0170770),		"dsDd", m68000up },	/* memory to register */
{"movp.l",	one(0000710),		one(0170770),		"Ddds", m68000up },	/* register to memory */
{"movp.w",	one(0000410),		one(0170770),		"dsDd", m68000up },	/* memory to register */
{"movp.w",	one(0000610),		one(0170770),		"Ddds", m68000up },	/* register to memory */
{"movq",	one(0070000),		one(0170400),		"MsDd", m68000up },
{"movq",	one(0020000),		one(0170000),		"*l$d", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"movq",	one(0020100),		one(0170700),		"*lAd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"movq",	one(0047140),		one(0177770),		"AsUd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"movq",	one(0047150),		one(0177770),		"UdAs", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"mov.w",	one(0030000),		one(0170000),		"*w$d", m68000up },
{"mov.w",	one(0030100),		one(0170700),		"*wAd", m68000up },	/* mova,	written as mov */
{"mov.w",	one(0040300),		one(0177700),		"Ss$s", m68000up },	/* Move from sr */
{"mov.w",	one(0041300),		one(0177700),		"Cs$s", m68010up },	/* Move from ccr */
{"mov.w",	one(0042300),		one(0177700),		";wCd", m68000up },	/* mov to ccr */
{"mov.w",	one(0043300),		one(0177700),		";wSd", m68000up },	/* mov to sr */
{"movs.b",	two(0007000, 0),	two(0177700, 07777),	"~sR1", m68010up },
{"movs.b",	two(0007000, 04000),	two(0177700, 07777),	"R1~s", m68010up },
{"movs.l",	two(0007200, 0),	two(0177700, 07777),	"~sR1", m68010up },
{"movs.l",	two(0007200, 04000),	two(0177700, 07777),	"R1~s", m68010up },
{"movs.w",	two(0007100, 0),	two(0177700, 07777),	"~sR1", m68010up },
{"movs.w",	two(0007100, 04000),	two(0177700, 07777),	"R1~s", m68010up },

{"moveq.l",	one(0070000),		one(0170400),		"MsDd", m68000up },
{"moveq.l",	one(0070000),		one(0170400),		"MsDd", m68000up },
{"movea.l",	one(0020100),		one(0170700),		"*lAd", m68000up },
{"movea.w",	one(0030100),		one(0170700),		"*wAd", m68000up },
{"move.b",	one(0010000),		one(0170000),		";b$d", m68000up },	/* mov */
{"move.l",	one(0070000),		one(0170400),		"MsDd", m68000up },	/* movq written as mov */
{"move.l",	one(0020000),		one(0170000),		"*l$d", m68000up },
{"move.l",	one(0020100),		one(0170700),		"*lAd", m68000up },
{"move.l",	one(0047140),		one(0177770),		"AsUd", m68000up },	/* mov to USP */
{"move.l",	one(0047150),		one(0177770),		"UdAs", m68000up },	/* mov from USP */
{"movec",	one(0047173),		one(0177777),		"R1Jj", m68010up },
{"movec",	one(0047173),		one(0177777),		"R1#j", m68010up },
{"movec",	one(0047172),		one(0177777),		"JjR1", m68010up },
{"movec",	one(0047172),		one(0177777),		"#jR1", m68010up },
{"movec.l",	one(0047173),		one(0177777),		"R1Jj", m68010up },
{"movec.l",	one(0047173),		one(0177777),		"R1#j", m68010up },
{"movec.l",	one(0047172),		one(0177777),		"JjR1", m68010up },
{"movec.l",	one(0047172),		one(0177777),		"#jR1", m68010up },

{"moveq",	one(0070000),		one(0170400),		"MsDd", m68000up },
{"moveq",	one(0020000),		one(0170000),		"*l$d", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"moveq",	one(0020100),		one(0170700),		"*lAd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"moveq",	one(0047140),		one(0177770),		"AsUd", m68000up }, /* FIXNIEL: move instead of movw because too many frags */
{"moveq",	one(0047150),		one(0177770),		"UdAs", m68000up }, /* FIXNIEL: move instead of movw because too many frags */



 

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
