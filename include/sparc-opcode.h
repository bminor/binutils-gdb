/* to sanitize this file, grep -v v9 < sparc-opcode.h > clean-sparc-opcode.h */

/* Table of opcodes for the sparc.
	Copyright (C) 1989, 1991 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler, and GDB, the GNU disassembler.

GAS/GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS/GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS or GDB; see the file COPYING.	If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.	*/

#if !defined(__STDC__) && !defined(const)
#define const
#endif

/*
 * Structure of an opcode table entry.
 */
enum sparc_architecture {
	v6 = 0,
	v7,
	cypress,
	v8,
	v9,
};

static const char *architecture_pname[] = {
	"v6",
	"v7",
	"cypress",
	"v8",
	"v9",
	NULL,
};

struct sparc_opcode {
	const char *name;
	unsigned long match;	/* Bits that must be set. */
	unsigned long lose;	/* Bits that must not be set. */
	const char *args;
 /* This was called "delayed" in versions before the flags. */
	char flags;
	enum sparc_architecture architecture;
};

#define	F_DELAYED	1	/* Delayed branch */
#define	F_ALIAS		2	/* Alias for a "real" instruction */

/*

All sparc opcodes are 32 bits, except for the `set' instruction (really a
macro), which is 64 bits. It is handled as a special case.

The match component is a mask saying which bits must match a particular
opcode in order for an instruction to be an instance of that opcode.

The args component is a string containing one character for each operand of the
instruction.

Kinds of operands:
	#	Number used by optimizer.	It is ignored.
	1	rs1 register.
	2	rs2 register.
	d	rd register.
	e	frs1 floating point register.
	f	frs2 floating point register.
	j	frs3 floating point register.
	g	frsd floating point register.
	b	crs1 coprocessor register
	c	crs2 coprocessor register
	D	crsd coprocessor register
	m	alternate space register (asr) in rd
	M	alternate space register (asr) in rs1
	h	22 high bits.
	i	13 bit Immediate.
	k	14 bit PC relative immediate. (v9)
	K	21 bit PC relative immediate. (v9)
	l	22 bit PC relative immediate.
	L	30 bit PC relative immediate.
	a	Annul.	The annul bit is set.
	A	Alternate address space. Stored as 8 bits.
	C	Coprocessor state register.
	F	floating point state register.
	p	Processor state register.
	N	Branch predict clear ",pn" (v9)
	T	Branch predict set ",pt" (v9)
	q	Floating point queue.
	r	Single register that is both rs1 and rsd.
	Q	Coprocessor queue.
	S	Special case.
	t	Trap base register.
	w	Window invalid mask register.
	y	Y register.
	Y	amr register. (v9)

*/

/* The order of the opcodes in this table is significant:
	
	* The assembler requires that all instances of the same mnemonic must
	be consecutive.	If they aren't, the assembler will bomb at runtime.

	* The disassembler should not care about the order of the opcodes.

*/

#define OP2(x)		(((x)&0x7) << 22) /* op2 field of format2 insns */
#define OP3(x)		(((x)&0x3f) << 19) /* op3 field of format3 insns */
#define OP(x)		(((x)&0x3) << 30) /* op field of all insns */
#define OPF(x)		(((x)&0x1ff) << 5) /* opf field of float insns */
#define F3F(x, y, z)	(OP(x) | OP3(y) | OPF(z)) /* format 3 float insns */
#define F3I(x)		(((x)&0x1) << 13) /* immediate field of format 3 insns */
#define F2(x, y)	(OP(x) | OP2(y)) /* format 2 insns */
#define F3(x, y, z)	(OP(x) | OP3(y) | F3I(z)) /* format 3 insns */
#define ASI(x)		(((x)&0xff) << 5) /* asi field of format 3 insns */
#define RS2(x)		((x)&0x1f) /* rs2 field */
#define SIMM13(x)	((x)&0x1fff) /* simm13 field */
#define RD(x)		(((x)&0x1f) << 25) /* destination register field */
#define RS1(x)		(((x)&0x1f) << 14) /* rs1 field */

#define COND(x)		(((x)&0xf) << 25)

#define CONDN	(COND(0x0))
#define CONDE	(COND(0x1))
#define CONDLE	(COND(0x2))
#define CONDL	(COND(0x3))
#define CONDLEU	(COND(0x4))
#define CONDCS	(COND(0x5))
#define CONDNEG	(COND(0x6))
#define CONDVS	(COND(0x7))
#define CONDA	(COND(0x8))
#define CONDNE	(COND(0x9))
#define CONDG	(COND(0xa))
#define CONDGE	(COND(0xb))
#define CONDGU	(COND(0xc))
#define CONDCC	(COND(0xd))
#define CONDPOS	(COND(0xe))
#define CONDVC	(COND(0xf))

#define CONDNZ	CONDNE
#define CONDZ	CONDE
#define CONDGEU	CONDCC
#define CONDLU	CONDCS

static const struct sparc_opcode sparc_opcodes[] = {

{ "ld",	0xc0000000, 0x01f82000, "[1+2],d", 0, v6 },
{ "ld",	0xc0000000, 0x01f8201f, "[1],d", 0, v6 }, /* ld [rs1+%g0],d */
{ "ld",	0xc0002000, 0x01f80000, "[1+i],d", 0, v6 },
{ "ld",	0xc0002000, 0x01f80000, "[i+1],d", 0, v6 },
{ "ld",	0xc0002000, 0x01f81fff, "[1],d", 0, v6 }, /* ld [rs1+0],d */
{ "ld",	0xc1000000, 0x00f82000, "[1+2],g", 0, v6 },
{ "ld",	0xc1000000, 0x00f8201f, "[1],g", 0, v6 }, /* ld [rs1+%g0],d */
{ "ld",	0xc1002000, 0x00f80000, "[1+i],g", 0, v6 },
{ "ld",	0xc1002000, 0x00f80000, "[i+1],g", 0, v6 },
{ "ld",	0xc1002000, 0x00f81fff, "[1],g", 0, v6 }, /* ld [rs1+0],d */
{ "ld",	0xc1080000, 0x00f02000, "[1+2],F", 0, v6 },
{ "ld",	0xc1080000, 0x00f0201f, "[1],F", 0, v6 }, /* ld [rs1+%g0],d */
{ "ld",	0xc1082000, 0x00f00000, "[1+i],F", 0, v6 },
{ "ld",	0xc1082000, 0x00f00000, "[i+1],F", 0, v6 },
{ "ld",	0xc1082000, 0x00f01fff, "[1],F", 0, v6 }, /* ld [rs1+0],d */
{ "ld",	0xc1800000, 0x00782000, "[1+2],D", 0, v6 },
{ "ld",	0xc1800000, 0x0078201f, "[1],D", 0, v6 }, /* ld [rs1+%g0],d */
{ "ld",	0xc1802000, 0x00780000, "[1+i],D", 0, v6 },
{ "ld",	0xc1802000, 0x00780000, "[i+1],D", 0, v6 },
{ "ld",	0xc1802000, 0x00781fff, "[1],D", 0, v6 }, /* ld [rs1+0],d */
{ "ld",	0xc1880000, 0x00702000, "[1+2],C", 0, v6 },
{ "ld",	0xc1880000, 0x0070201f, "[1],C", 0, v6 }, /* ld [rs1+%g0],d */
{ "ld",	0xc1882000, 0x00700000, "[1+i],C", 0, v6 },
{ "ld",	0xc1882000, 0x00700000, "[i+1],C", 0, v6 },
{ "ld",	0xc1882000, 0x00701fff, "[1],C", 0, v6 }, /* ld [rs1+0],d */

{ "lduw",	0xc0000000, 0x01f82000, "[1+2],d", F_ALIAS, v9 }, /* lduw === ld */
{ "lduw",	0xc0000000, 0x01f8201f, "[1],d", F_ALIAS, v9 }, /* ld [rs1+%g0],d */
{ "lduw",	0xc0002000, 0x01f80000, "[1+i],d", F_ALIAS, v9 },
{ "lduw",	0xc0002000, 0x01f80000, "[i+1],d", F_ALIAS, v9 },
{ "lduw",	0xc0002000, 0x01f81fff, "[1],d", F_ALIAS, v9 }, /* ld [rs1+0],d */
{ "lduw",	0xc1000000, 0x00f82000, "[1+2],g", F_ALIAS, v9 },
{ "lduw",	0xc1000000, 0x00f8201f, "[1],g", F_ALIAS, v9 }, /* ld [rs1+%g0],d */
{ "lduw",	0xc1002000, 0x00f80000, "[1+i],g", F_ALIAS, v9 },
{ "lduw",	0xc1002000, 0x00f80000, "[i+1],g", F_ALIAS, v9 },
{ "lduw",	0xc1002000, 0x00f81fff, "[1],g", F_ALIAS, v9 }, /* ld [rs1+0],d */
{ "lduw",	0xc1080000, 0x00f02000, "[1+2],F", F_ALIAS, v9 },
{ "lduw",	0xc1080000, 0x00f0201f, "[1],F", F_ALIAS, v9 }, /* ld [rs1+%g0],d */
{ "lduw",	0xc1082000, 0x00f00000, "[1+i],F", F_ALIAS, v9 },
{ "lduw",	0xc1082000, 0x00f00000, "[i+1],F", F_ALIAS, v9 },
{ "lduw",	0xc1082000, 0x00f01fff, "[1],F", F_ALIAS, v9 }, /* ld [rs1+0],d */
{ "lduw",	0xc1800000, 0x00782000, "[1+2],D", F_ALIAS, v9 },
{ "lduw",	0xc1800000, 0x0078201f, "[1],D", F_ALIAS, v9 }, /* ld [rs1+%g0],d */
{ "lduw",	0xc1802000, 0x00780000, "[1+i],D", F_ALIAS, v9 },
{ "lduw",	0xc1802000, 0x00780000, "[i+1],D", F_ALIAS, v9 },
{ "lduw",	0xc1802000, 0x00781fff, "[1],D", F_ALIAS, v9 }, /* ld [rs1+0],d */
{ "lduw",	0xc1880000, 0x00702000, "[1+2],C", F_ALIAS, v9 },
{ "lduw",	0xc1880000, 0x0070201f, "[1],C", F_ALIAS, v9 }, /* ld [rs1+%g0],d */
{ "lduw",	0xc1882000, 0x00700000, "[1+i],C", F_ALIAS, v9 },
{ "lduw",	0xc1882000, 0x00700000, "[i+1],C", F_ALIAS, v9 },
{ "lduw",	0xc1882000, 0x00701fff, "[1],C", F_ALIAS, v9 }, /* ld [rs1+0],d */

{ "lda",	0xc0800000, 0x01782000, "[1+2]A,d", 0, v6 },
{ "lda",	0xc0800000, 0x0178201f, "[1]A,d", 0, v6 }, /* lda [rs1+%g0],d */

{ "lduwa",	0xc0800000, 0x01782000, "[1+2]A,d", F_ALIAS, v9 }, /* lduwa === lda */
{ "lduwa",	0xc0800000, 0x0178201f, "[1]A,d", F_ALIAS, v9 }, /* lda [rs1+%g0],d */

{ "ldd",	0xc0180000, 0x01e02000, "[1+2],d", 0, v6 },
{ "ldd",	0xc0180000, 0x01e0201f, "[1],d", 0, v6 }, /* ldd [rs1+%g0],d */
{ "ldd",	0xc0182000, 0x01e00000, "[1+i],d", 0, v6 },
{ "ldd",	0xc0182000, 0x01e00000, "[i+1],d", 0, v6 },
{ "ldd",	0xc0182000, 0x01e01fff, "[1],d", 0, v6 }, /* ldd [rs1+0],d */
{ "ldd",	0xc1180000, 0x00e02000, "[1+2],g", 0, v6 },
{ "ldd",	0xc1180000, 0x00e0201f, "[1],g", 0, v6 }, /* ldd [rs1+%g0],d */
{ "ldd",	0xc1182000, 0x00e00000, "[1+i],g", 0, v6 },
{ "ldd",	0xc1182000, 0x00e00000, "[i+1],g", 0, v6 },
{ "ldd",	0xc1182000, 0x00e01fff, "[1],g", 0, v6 }, /* ldd [rs1+0],d */
{ "ldd",	0xc1980000, 0x00602000, "[1+2],D", 0, v6 },
{ "ldd",	0xc1980000, 0x0060201f, "[1],D", 0, v6 }, /* ldd [rs1+%g0],d */
{ "ldd",	0xc1982000, 0x00600000, "[1+i],D", 0, v6 },
{ "ldd",	0xc1982000, 0x00600000, "[i+1],D", 0, v6 },
{ "ldd",	0xc1982000, 0x00601fff, "[1],D", 0, v6 }, /* ldd [rs1+0],d */
{ "ldda",	0xc0980000, 0x01602000, "[1+2]A,d", 0, v6 },
{ "ldda",	0xc0980000, 0x0160201f, "[1]A,d", 0, v6 }, /* ldda [rs1+%g0],d */
{ "ldsb",	0xc0480000, 0x01b02000, "[1+2],d", 0, v6 },
{ "ldsb",	0xc0480000, 0x01b0201f, "[1],d", 0, v6 }, /* ldsb [rs1+%g0],d */
{ "ldsb",	0xc0482000, 0x01b00000, "[1+i],d", 0, v6 },
{ "ldsb",	0xc0482000, 0x01b00000, "[i+1],d", 0, v6 },
{ "ldsb",	0xc0482000, 0x01b01fff, "[1],d", 0, v6 }, /* ldsb [rs1+0],d */
{ "ldsba",	0xc0c80000, 0x01302000, "[1+2]A,d", 0, v6 },
{ "ldsba",	0xc0c80000, 0x0130201f, "[1]A,d", 0, v6 }, /* ldsba [rs1+%g0],d */
{ "ldsh",	0xc0500000, 0x0000000d, "[1],d", 0, v6 }, /* ldsh [rs1+%g0],d */
{ "ldsh",	0xc0500000, 0x01a82000, "[1+2],d", 0, v6 },
{ "ldsh",	0xc0502000, 0x01a80000, "[1+i],d", 0, v6 },
{ "ldsh",	0xc0502000, 0x01a80000, "[i+1],d", 0, v6 },
{ "ldsh",	0xc0502000, 0x01a81fff, "[1],d", 0, v6 }, /* ldsh [rs1+0],d */
{ "ldsha",	0xc0d00000, 0x01282000, "[1+2]A,d", 0, v6 },
{ "ldsha",	0xc0d00000, 0x0128201f, "[1]A,d", 0, v6 }, /* ldsha [rs1+%g0],d */
{ "ldstub",	0xc0680000, 0x01902000, "[1+2],d", 0, v6 },
{ "ldstub",	0xc0680000, 0x0190201f, "[1],d", 0, v6 }, /* ldstub [rs1+%g0],d */
{ "ldstub",	0xc0682000, 0x01900000, "[1+i],d", 0, v6 },
{ "ldstub",	0xc0682000, 0x01900000, "[i+1],d", 0, v6 },
{ "ldstuba",	0xc0d80000, 0x01002000, "[1+2]A,d", 0, v6 },
{ "ldstuba",	0xc0d80000, 0x0100201f, "[1]A,d", 0, v6 }, /* ldstuba [rs1+%g0],d */
{ "ldsw",	0xc0400000, 0x01b12000, "[1+2],d", 0, v9 },
{ "ldsw",	0xc0400000, 0x01b1201f, "[1],d", 0, v9 }, /* ldsw [rs1+%g0],d */
{ "ldsw",	0xc0402000, 0x01b10000, "[1+i],d", 0, v9 },
{ "ldsw",	0xc0402000, 0x01b10000, "[i+1],d", 0, v9 },
{ "ldsw",	0xc0402000, 0x01b11fff, "[1],d", 0, v9 }, /* ldsw [rs1+0],d */
{ "ldub",	0xc0080000, 0x01f02000, "[1+2],d", 0, v6 },
{ "ldub",	0xc0080000, 0x01f0201f, "[1],d", 0, v6 }, /* ldub [rs1+%g0],d */
{ "ldub",	0xc0082000, 0x01f00000, "[1+i],d", 0, v6 },
{ "ldub",	0xc0082000, 0x01f00000, "[i+1],d", 0, v6 },
{ "ldub",	0xc0082000, 0x01f01fff, "[1],d", 0, v6 }, /* ldub [rs1+0],d */
{ "lduba",	0xc0880000, 0x01702000, "[1+2]A,d", 0, v6 },
{ "lduba",	0xc0880000, 0x0170201f, "[1]A,d", 0, v6 }, /* lduba [rs1+%g0],d */
{ "lduh",	0xc0100000, 0x01e82000, "[1+2],d", 0, v6 },
{ "lduh",	0xc0100000, 0x01e8201f, "[1],d", 0, v6 }, /* lduh [rs1+%g0],d */
{ "lduh",	0xc0102000, 0x01e80000, "[1+i],d", 0, v6 },
{ "lduh",	0xc0102000, 0x01e80000, "[i+1],d", 0, v6 },
{ "lduh",	0xc0102000, 0x01e81fff, "[1],d", 0, v6 }, /* lduh [rs1+0],d */
{ "lduha",	0xc0900000, 0x01682000, "[1+2]A,d", 0, v6 },
{ "lduha",	0xc0900000, 0x0168201f, "[1]A,d", 0, v6 }, /* lduha [rs1+%g0],d */
{ "ldx",	0xc0580000, 0x01a12000, "[1+2],d", 0, v9 },
{ "ldx",	0xc0580000, 0x01a1201f, "[1],d", 0, v9 }, /* ldx [rs1+%g0],d */
{ "ldx",	0xc0582000, 0x01a10000, "[1+i],d", 0, v9 },
{ "ldx",	0xc0582000, 0x01a10000, "[i+1],d", 0, v9 },
{ "ldx",	0xc0582000, 0x01a11fff, "[1],d", 0, v9 }, /* ldx [rs1+0],d */
{ "ldxpc",	0xc0600000, 0x01912000, "[1+2],d", 0, v9 },
{ "ldxpc",	0xc0600000, 0x0191201f, "[1],d", 0, v9 }, /* ldxpc [rs1+%g0],d */
{ "ldxpc",	0xc0602000, 0x01910000, "[1+i],d", 0, v9 },
{ "ldxpc",	0xc0602000, 0x01910000, "[i+1],d", 0, v9 },
{ "ldxpc",	0xc0602000, 0x01911fff, "[1],d", 0, v9 }, /* ldxpc [rs1+0],d */

{ "st",	0xc0200000, 0x01d82000, "d,[1+2]", 0, v6 },
{ "st",	0xc0200000, 0x01d8201f, "d,[1]", 0, v6 }, /* st d,[rs1+%g0] */
{ "st",	0xc0202000, 0x01d80000, "d,[1+i]", 0, v6 },
{ "st",	0xc0202000, 0x01d80000, "d,[i+1]", 0, v6 },
{ "st",	0xc0202000, 0x01d81fff, "d,[1]", 0, v6 }, /* st d,[rs1+0] */
{ "st",	0xc1200000, 0x00d82000, "g,[1+2]", 0, v6 },
{ "st",	0xc1200000, 0x00d8201f, "g,[1]", 0, v6 }, /* st d[rs1+%g0] */
{ "st",	0xc1202000, 0x00d80000, "g,[1+i]", 0, v6 },
{ "st",	0xc1202000, 0x00d80000, "g,[i+1]", 0, v6 },
{ "st",	0xc1202000, 0x00d81fff, "g,[1]", 0, v6 }, /* st d,[rs1+0] */
{ "st",	0xc1a00000, 0x00582000, "D,[1+2]", 0, v6 },
{ "st",	0xc1a00000, 0x0058201f, "D,[1]", 0, v6 }, /* st d,[rs1+%g0] */
{ "st",	0xc1a02000, 0x00580000, "D,[1+i]", 0, v6 },
{ "st",	0xc1a02000, 0x00580000, "D,[i+1]", 0, v6 },
{ "st",	0xc1a02000, 0x00581fff, "D,[1]", 0, v6 }, /* st d,[rs1+0] */
{ "st",	0xc1a80000, 0x00502000, "C,[1+2]", 0, v6 },
{ "st",	0xc1a80000, 0x0050201f, "C,[1]", 0, v6 }, /* st d,[rs1+%g0] */
{ "st",	0xc1a82000, 0x00500000, "C,[1+i]", 0, v6 },
{ "st",	0xc1a82000, 0x00500000, "C,[i+1]", 0, v6 },
{ "st",	0xc1a82000, 0x00501fff, "C,[1]", 0, v6 }, /* st d,[rs1+0] */
{ "st",	F3(0x3, 0x25, 0), F3(~0x3, ~0x25, ~0)|RD(~0)|ASI(~0),		"F,[1+2]", 0, v6 },
{ "st",	F3(0x3, 0x25, 0), F3(~0x3, ~0x25, ~0)|RD(~0)|ASI(~0)|RS2(~0),	"F,[1]", 0, v6 }, /* st d,[rs1+%g0] */
{ "st",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|RD(~0),		"F,[1+i]", 0, v6 },
{ "st",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|RD(~0),		"F,[i+1]", 0, v6 },
{ "st",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|SIMM13(~0),	"F,[1]", 0, v6 }, /* st d,[rs1+0] */

{ "stw",	0xc0200000, 0x01d82000, "d,[1+2]", F_ALIAS, v9 }, /* stw === st */
{ "stw",	0xc0200000, 0x01d8201f, "d,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stw",	0xc0202000, 0x01d80000, "d,[1+i]", F_ALIAS, v9 },
{ "stw",	0xc0202000, 0x01d80000, "d,[i+1]", F_ALIAS, v9 },
{ "stw",	0xc0202000, 0x01d81fff, "d,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stw",	0xc1200000, 0x00d82000, "g,[1+2]", F_ALIAS, v9 },
{ "stw",	0xc1200000, 0x00d8201f, "g,[1]", F_ALIAS, v9 }, /* st d[rs1+%g0] */
{ "stw",	0xc1202000, 0x00d80000, "g,[1+i]", F_ALIAS, v9 },
{ "stw",	0xc1202000, 0x00d80000, "g,[i+1]", F_ALIAS, v9 },
{ "stw",	0xc1202000, 0x00d81fff, "g,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stw",	0xc1a00000, 0x00582000, "D,[1+2]", F_ALIAS, v9 },
{ "stw",	0xc1a00000, 0x0058201f, "D,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stw",	0xc1a02000, 0x00580000, "D,[1+i]", F_ALIAS, v9 },
{ "stw",	0xc1a02000, 0x00580000, "D,[i+1]", F_ALIAS, v9 },
{ "stw",	0xc1a02000, 0x00581fff, "D,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stw",	0xc1a80000, 0x00502000, "C,[1+2]", F_ALIAS, v9 },
{ "stw",	0xc1a80000, 0x0050201f, "C,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stw",	0xc1a82000, 0x00500000, "C,[1+i]", F_ALIAS, v9 },
{ "stw",	0xc1a82000, 0x00500000, "C,[i+1]", F_ALIAS, v9 },
{ "stw",	0xc1a82000, 0x00501fff, "C,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stw",	F3(0x3, 0x25, 0), F3(~0x3, ~0x25, ~0)|RD(~0)|ASI(~0),		"F,[1+2]", F_ALIAS, v9 },
{ "stw",	F3(0x3, 0x25, 0), F3(~0x3, ~0x25, ~0)|RD(~0)|ASI(~0)|RS2(~0),	"F,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stw",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|RD(~0),		"F,[1+i]", F_ALIAS, v9 },
{ "stw",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|RD(~0),		"F,[i+1]", F_ALIAS, v9 },
{ "stw",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|SIMM13(~0),	"F,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */

{ "stuw",	0xc0200000, 0x01d82000, "d,[1+2]", F_ALIAS, v9 }, /* stuw === st */
{ "stuw",	0xc0200000, 0x01d8201f, "d,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stuw",	0xc0202000, 0x01d80000, "d,[1+i]", F_ALIAS, v9 },
{ "stuw",	0xc0202000, 0x01d80000, "d,[i+1]", F_ALIAS, v9 },
{ "stuw",	0xc0202000, 0x01d81fff, "d,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stuw",	0xc1200000, 0x00d82000, "g,[1+2]", F_ALIAS, v9 },
{ "stuw",	0xc1200000, 0x00d8201f, "g,[1]", F_ALIAS, v9 }, /* st d[rs1+%g0] */
{ "stuw",	0xc1202000, 0x00d80000, "g,[1+i]", F_ALIAS, v9 },
{ "stuw",	0xc1202000, 0x00d80000, "g,[i+1]", F_ALIAS, v9 },
{ "stuw",	0xc1202000, 0x00d81fff, "g,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stuw",	0xc1a00000, 0x00582000, "D,[1+2]", F_ALIAS, v9 },
{ "stuw",	0xc1a00000, 0x0058201f, "D,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stuw",	0xc1a02000, 0x00580000, "D,[1+i]", F_ALIAS, v9 },
{ "stuw",	0xc1a02000, 0x00580000, "D,[i+1]", F_ALIAS, v9 },
{ "stuw",	0xc1a02000, 0x00581fff, "D,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stuw",	0xc1a80000, 0x00502000, "C,[1+2]", F_ALIAS, v9 },
{ "stuw",	0xc1a80000, 0x0050201f, "C,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stuw",	0xc1a82000, 0x00500000, "C,[1+i]", F_ALIAS, v9 },
{ "stuw",	0xc1a82000, 0x00500000, "C,[i+1]", F_ALIAS, v9 },
{ "stuw",	0xc1a82000, 0x00501fff, "C,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stuw",	F3(0x3, 0x25, 0), F3(~0x3, ~0x25, ~0)|RD(~0)|ASI(~0),		"F,[1+2]", F_ALIAS, v9 },
{ "stuw",	F3(0x3, 0x25, 0), F3(~0x3, ~0x25, ~0)|RD(~0)|ASI(~0)|RS2(~0),	"F,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stuw",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|RD(~0),		"F,[1+i]", F_ALIAS, v9 },
{ "stuw",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|RD(~0),		"F,[i+1]", F_ALIAS, v9 },
{ "stuw",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|SIMM13(~0),	"F,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */

{ "stsw",	0xc0200000, 0x01d82000, "d,[1+2]", F_ALIAS, v9 }, /* stsw === st */
{ "stsw",	0xc0200000, 0x01d8201f, "d,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stsw",	0xc0202000, 0x01d80000, "d,[1+i]", F_ALIAS, v9 },
{ "stsw",	0xc0202000, 0x01d80000, "d,[i+1]", F_ALIAS, v9 },
{ "stsw",	0xc0202000, 0x01d81fff, "d,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stsw",	0xc1200000, 0x00d82000, "g,[1+2]", F_ALIAS, v9 },
{ "stsw",	0xc1200000, 0x00d8201f, "g,[1]", F_ALIAS, v9 }, /* st d[rs1+%g0] */
{ "stsw",	0xc1202000, 0x00d80000, "g,[1+i]", F_ALIAS, v9 },
{ "stsw",	0xc1202000, 0x00d80000, "g,[i+1]", F_ALIAS, v9 },
{ "stsw",	0xc1202000, 0x00d81fff, "g,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stsw",	0xc1a00000, 0x00582000, "D,[1+2]", F_ALIAS, v9 },
{ "stsw",	0xc1a00000, 0x0058201f, "D,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stsw",	0xc1a02000, 0x00580000, "D,[1+i]", F_ALIAS, v9 },
{ "stsw",	0xc1a02000, 0x00580000, "D,[i+1]", F_ALIAS, v9 },
{ "stsw",	0xc1a02000, 0x00581fff, "D,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stsw",	0xc1a80000, 0x00502000, "C,[1+2]", F_ALIAS, v9 },
{ "stsw",	0xc1a80000, 0x0050201f, "C,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stsw",	0xc1a82000, 0x00500000, "C,[1+i]", F_ALIAS, v9 },
{ "stsw",	0xc1a82000, 0x00500000, "C,[i+1]", F_ALIAS, v9 },
{ "stsw",	0xc1a82000, 0x00501fff, "C,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */
{ "stsw",	F3(0x3, 0x25, 0), F3(~0x3, ~0x25, ~0)|RD(~0)|ASI(~0),		"F,[1+2]", F_ALIAS, v9 },
{ "stsw",	F3(0x3, 0x25, 0), F3(~0x3, ~0x25, ~0)|RD(~0)|ASI(~0)|RS2(~0),	"F,[1]", F_ALIAS, v9 }, /* st d,[rs1+%g0] */
{ "stsw",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|RD(~0),		"F,[1+i]", F_ALIAS, v9 },
{ "stsw",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|RD(~0),		"F,[i+1]", F_ALIAS, v9 },
{ "stsw",	F3(0x3, 0x25, 1), F3(~0x3, ~0x25, ~1)|SIMM13(~0),	"F,[1]", F_ALIAS, v9 }, /* st d,[rs1+0] */

{ "sta",	0xc0a00000, 0x01082000, "d,[1+2]A", 0, v6 },
{ "sta",	0xc0a00000, 0x0108201f, "d,[1]A", 0, v6 }, /* sta d,[rs1+%g0] */

{ "stwa",	0xc0a00000, 0x01082000, "d,[1+2]A", F_ALIAS, v9 },
{ "stwa",	0xc0a00000, 0x0108201f, "d,[1]A", F_ALIAS, v9 }, /* sta d,[rs1+%g0] */

{ "stswa",	0xc0a00000, 0x01082000, "d,[1+2]A", F_ALIAS, v9 },
{ "stswa",	0xc0a00000, 0x0108201f, "d,[1]A", F_ALIAS, v9 }, /* sta d,[rs1+%g0] */

{ "stuwa",	0xc0a00000, 0x01082000, "d,[1+2]A", F_ALIAS, v9 },
{ "stuwa",	0xc0a00000, 0x0108201f, "d,[1]A", F_ALIAS, v9 }, /* sta d,[rs1+%g0] */

{ "stb",	0xc0280000, 0x01d02000, "d,[1+2]", 0, v6 },
{ "stb",	0xc0280000, 0x01d0201f, "d,[1]", 0, v6 }, /* stb d,[rs1+%g0] */
{ "stb",	0xc0282000, 0x01d00000, "d,[1+i]", 0, v6 },
{ "stb",	0xc0282000, 0x01d00000, "d,[i+1]", 0, v6 },
{ "stb",	0xc0282000, 0x01d01fff, "d,[1]", 0, v6 }, /* stb d,[rs1+0] */

{ "stub",	0xc0280000, 0x01d02000, "d,[1+2]", F_ALIAS, v9 },
{ "stub",	0xc0280000, 0x01d0201f, "d,[1]", F_ALIAS, v9 }, /* stb d,[rs1+%g0] */
{ "stub",	0xc0282000, 0x01d00000, "d,[1+i]", F_ALIAS, v9 },
{ "stub",	0xc0282000, 0x01d00000, "d,[i+1]", F_ALIAS, v9 },
{ "stub",	0xc0282000, 0x01d01fff, "d,[1]", F_ALIAS, v9 }, /* stb d,[rs1+0] */

{ "stsb",	0xc0280000, 0x01d02000, "d,[1+2]", F_ALIAS, v9 },
{ "stsb",	0xc0280000, 0x01d0201f, "d,[1]", F_ALIAS, v9 }, /* stb d,[rs1+%g0] */
{ "stsb",	0xc0282000, 0x01d00000, "d,[1+i]", F_ALIAS, v9 },
{ "stsb",	0xc0282000, 0x01d00000, "d,[i+1]", F_ALIAS, v9 },
{ "stsb",	0xc0282000, 0x01d01fff, "d,[1]", F_ALIAS, v9 }, /* stb d,[rs1+0] */

{ "stba",	0xc0a80000, 0x01002000, "d,[1+2]A", 0, v6 },
{ "stba",	0xc0a80000, 0x0100201f, "d,[1]A", 0, v6 }, /* stba d,[rs1+%g0] */

{ "stsba",	0xc0a80000, 0x01002000, "d,[1+2]A", F_ALIAS, v9 },
{ "stsba",	0xc0a80000, 0x0100201f, "d,[1]A", F_ALIAS, v9 }, /* stba d,[rs1+%g0] */

{ "stuba",	0xc0a80000, 0x01002000, "d,[1+2]A", F_ALIAS, v9 },
{ "stuba",	0xc0a80000, 0x0100201f, "d,[1]A", F_ALIAS, v9 }, /* stba d,[rs1+%g0] */

{ "std",	0xc0380000, 0x01c02000, "d,[1+2]", 0, v6 },
{ "std",	0xc0380000, 0x01c0201f, "d,[1]", 0, v6 }, /* std d,[rs1+%g0] */
{ "std",	0xc0382000, 0x01c00000, "d,[1+i]", 0, v6 },
{ "std",	0xc0382000, 0x01c00000, "d,[i+1]", 0, v6 },
{ "std",	0xc0382000, 0x01c01fff, "d,[1]", 0, v6 }, /* std d,[rs1+0] */
{ "std",	0xc1300000, 0x00c82000, "q,[1+2]", 0, v6 },
{ "std",	0xc1300000, 0x00c8201f, "q,[1]", 0, v6 }, /* std d,[rs1+%g0] */
{ "std",	0xc1302000, 0x00c80000, "q,[1+i]", 0, v6 },
{ "std",	0xc1302000, 0x00c80000, "q,[i+1]", 0, v6 },
{ "std",	0xc1302000, 0x00c81fff, "q,[1]", 0, v6 }, /* std d,[rs1+0] */
{ "std",	0xc1380000, 0x00c02000, "g,[1+2]", 0, v6 },
{ "std",	0xc1380000, 0x00c0201f, "g,[1]", 0, v6 }, /* std d,[rs1+%g0] */
{ "std",	0xc1382000, 0x00c00000, "g,[1+i]", 0, v6 },
{ "std",	0xc1382000, 0x00c00000, "g,[i+1]", 0, v6 },
{ "std",	0xc1382000, 0x00c01fff, "g,[1]", 0, v6 }, /* std d,[rs1+0] */
{ "std",	0xc1b00000, 0x00482000, "Q,[1+2]", 0, v6 },
{ "std",	0xc1b00000, 0x0048201f, "Q,[1]", 0, v6 }, /* std d,[rs1+%g0] */
{ "std",	0xc1b02000, 0x00480000, "Q,[1+i]", 0, v6 },
{ "std",	0xc1b02000, 0x00480000, "Q,[i+1]", 0, v6 },
{ "std",	0xc1b02000, 0x00481fff, "Q,[1]", 0, v6 }, /* std d,[rs1+0] */
{ "std",	0xc1b80000, 0x00402000, "D,[1+2]", 0, v6 },
{ "std",	0xc1b80000, 0x0040201f, "D,[1]", 0, v6 }, /* std d,[rs1+%g0] */
{ "std",	0xc1b82000, 0x00400000, "D,[1+i]", 0, v6 },
{ "std",	0xc1b82000, 0x00400000, "D,[i+1]", 0, v6 },
{ "std",	0xc1b82000, 0x00401fff, "D,[1]", 0, v6 }, /* std d,[rs1+0] */

{ "stda",	0xc0b80000, 0x01402000, "d,[1+2]A", 0, v6 },
{ "stda",	0xc0b80000, 0x0140201f, "d,[1]A", 0, v6 }, /* stda d,[rs1+%g0] */

{ "sth",	0xc0300000, 0x01c82000, "d,[1+2]", 0, v6 },
{ "sth",	0xc0300000, 0x01c8201f, "d,[1]", 0, v6 }, /* sth d,[rs1+%g0] */
{ "sth",	0xc0302000, 0x01c80000, "d,[1+i]", 0, v6 },
{ "sth",	0xc0302000, 0x01c80000, "d,[i+1]", 0, v6 },
{ "sth",	0xc0302000, 0x01c81fff, "d,[1]", 0, v6 }, /* sth d,[rs1+0] */

{ "stsh",	0xc0300000, 0x01c82000, "d,[1+2]", F_ALIAS, v9 },
{ "stsh",	0xc0300000, 0x01c8201f, "d,[1]", F_ALIAS, v9 }, /* sth d,[rs1+%g0] */
{ "stsh",	0xc0302000, 0x01c80000, "d,[1+i]", F_ALIAS, v9 },
{ "stsh",	0xc0302000, 0x01c80000, "d,[i+1]", F_ALIAS, v9 },
{ "stsh",	0xc0302000, 0x01c81fff, "d,[1]", F_ALIAS, v9 }, /* sth d,[rs1+0] */

{ "stuh",	0xc0300000, 0x01c82000, "d,[1+2]", F_ALIAS, v9 },
{ "stuh",	0xc0300000, 0x01c8201f, "d,[1]", F_ALIAS, v9 }, /* sth d,[rs1+%g0] */
{ "stuh",	0xc0302000, 0x01c80000, "d,[1+i]", F_ALIAS, v9 },
{ "stuh",	0xc0302000, 0x01c80000, "d,[i+1]", F_ALIAS, v9 },
{ "stuh",	0xc0302000, 0x01c81fff, "d,[1]", F_ALIAS, v9 }, /* sth d,[rs1+0] */

{ "stha",	0xc0b00000, 0x01482000, "d,[1+2]A", 0, v6 },
{ "stha",	0xc0b00000, 0x0148201f, "d,[1]A", 0, v6 }, /* stha d,[rs1+%g0] */

{ "stsha",	0xc0b00000, 0x01482000, "d,[1+2]A", F_ALIAS, v9 },
{ "stsha",	0xc0b00000, 0x0148201f, "d,[1]A", F_ALIAS, v9 }, /* stha d,[rs1+%g0] */

{ "stuha",	0xc0b00000, 0x01482000, "d,[1+2]A", F_ALIAS, v9 },
{ "stuha",	0xc0b00000, 0x0148201f, "d,[1]A", F_ALIAS, v9 }, /* stha d,[rs1+%g0] */

{ "stx",	0xc0d80000, 0x01202000, "d,[1+2]", 0, v9 },
{ "stx",	0xc0d80000, 0x0120201f, "d,[1]", 0, v9 }, /* stx d,[rs1+%g0] */
{ "stx",	0xc0d82000, 0x01200000, "d,[1+i]", 0, v9 },
{ "stx",	0xc0d82000, 0x01200000, "d,[i+1]", 0, v9 },
{ "stx",	0xc0d82000, 0x01201fff, "d,[1]", 0, v9 }, /* stx d,[rs1+0] */

{ "swap",	0xc0780000, 0x01802000, "[1+2],d", 0, v7 },
{ "swap",	0xc0780000, 0x0180201f, "[1],d", 0, v7 }, /* swap [rs1+%g0],d */
{ "swap",	0xc0782000, 0x01800000, "[1+i],d", 0, v7 },
{ "swap",	0xc0782000, 0x01800000, "[i+1],d", 0, v7 },
{ "swap",	0xc0782000, 0x01801fff, "[1],d", 0, v7 }, /* swap [rs1+0],d */

{ "swapa",	0xc0f80000, 0x01002000, "[1+2]A,d", 0, v7 },
{ "swapa",	0xc0f80000, 0x0100201f, "[1]A,d", 0, v7 }, /* swapa [rs1+%g0],d */

{ "restore",	0x81e80000, 0x00000000, "1,2,d", 0, v6 },
{ "restore",	0x81e80000, 0x7e17e01f, "", 0, v6 }, /* restore %g0,%g0,%g0 */
{ "restore",	0x81e82000, 0x00000000, "1,i,d", 0, v6 },
{ "restore",	0x81e82000, 0x7e14dfff, "", 0, v6 }, /* restore %g0,0,%g0 */

{ "rett",	F3(2, 0x39, 0), F3(~2, ~0x39, ~0)|RD(~0)|ASI(~0),	  "1+2", F_DELAYED, v6 }, /* rett rs1+rs2 */
{ "rett",	F3(2, 0x39, 0), F3(~2, ~0x39, ~0)|RD(~0)|ASI(~0)|RS2(~0), "1", F_DELAYED, v6 },	/* rett rs1,%g0 */
{ "rett",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1)|RD(~0),		  "1+i", F_DELAYED, v6 }, /* rett rs1+X */
{ "rett",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1)|RD(~0),		  "i+1", F_DELAYED, v6 }, /* rett X+rs1 */
{ "rett",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1)|RD(~0)|RS1(~0),	  "i", F_DELAYED, v6 },	/* rett X */
{ "rett",	F3(2, 0x39, 1), F3(~2, ~0x39, ~1)|RD(~0)|SIMM13(~0),	  "1", F_DELAYED, v6 },	/* rett rs1+0 */

{ "save",	0x81e00000, 0x40180000, "1,2,d", 0, v6 },
{ "save",	0x81e02000, 0x40180000, "1,i,d", 0, v6 },

{ "ret",	0x81c7e008, 0x00001ff7, "", F_DELAYED, v6 }, /* jmpl %i7+8,%g0 */
{ "retl",	0x81c3e008, 0x00001ff7, "", F_DELAYED, v6 }, /* jmpl %o7+8,%g0 */

{ "jmpl",	0x81c00000, 0x40382000, "1+2,d", F_DELAYED, v6 },
{ "jmpl",	0x81c00000, 0x4038201f, "1,d", F_DELAYED, v6 }, /* jmpl rs1+%g0,d */
{ "jmpl",	0x81c02000, 0x4037c000, "i,d", F_DELAYED, v6 }, /* jmpl %g0+i,d */
{ "jmpl",	0x81c02000, 0x40380000, "1+i,d", F_DELAYED, v6 },
{ "jmpl",	0x81c02000, 0x40380000, "i+1,d", F_DELAYED, v6 },

{ "sll",	F3(0x2, 0x25, 0), F3(~0x2, ~0x25, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "sll",	F3(0x2, 0x25, 1), F3(~0x2, ~0x25, ~1),		"1,i,d", 0, v6 },
{ "sra",	F3(0x2, 0x27, 0), F3(~0x2, ~0x27, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "sra",	F3(0x2, 0x27, 1), F3(~0x2, ~0x27, ~1),		"1,i,d", 0, v6 },
{ "srl",	F3(0x2, 0x26, 0), F3(~0x2, ~0x26, ~0)|ASI(~0),	"1,2,d", 0, v6 },
{ "srl",	F3(0x2, 0x26, 1), F3(~0x2, ~0x26, ~1),		"1,i,d", 0, v6 },

{ "sllw",	F3(0x2, 0x25, 0), F3(~0x2, ~0x25, ~0)|ASI(~0),	"1,2,d", F_ALIAS, v9 },	/* sllw === sll */
{ "sllw",	F3(0x2, 0x25, 1), F3(~0x2, ~0x25, ~1)|ASI(~0),	"1,i,d", F_ALIAS, v9 },
{ "sraw",	F3(0x2, 0x27, 0), F3(~0x2, ~0x27, ~0)|ASI(~0),	"1,2,d", F_ALIAS, v9 },	/* sraw === sra */
{ "sraw",	F3(0x2, 0x27, 1), F3(~0x2, ~0x27, ~1)|ASI(~0),	"1,i,d", F_ALIAS, v9 },
{ "srlw",	F3(0x2, 0x26, 0), F3(~0x2, ~0x26, ~0)|ASI(~0),	"1,2,d", F_ALIAS, v9 },	/* srlw === srl */
{ "srlw",	F3(0x2, 0x26, 1), F3(~0x2, ~0x26, ~1)|ASI(~0),	"1,i,d", F_ALIAS, v9 },

{ "sllx",	F3(0x2, 0x2d, 0), F3(~0x2, ~0x2d, ~0)|ASI(~0),	"1,2,d", 0, v9 },
{ "sllx",	F3(0x2, 0x2d, 1), F3(~0x2, ~0x2d, ~1)|(0x7f<<6),	"1,i,d", 0, v9 },
{ "srax",	F3(0x2, 0x2f, 0), F3(~0x2, ~0x2f, ~0)|ASI(~0),	"1,2,d", 0, v9 },
{ "srax",	F3(0x2, 0x2f, 1), F3(~0x2, ~0x2f, ~1)|(0x7f<<6),	"1,i,d", 0, v9 },
{ "srlx",	F3(0x2, 0x2e, 0), F3(~0x2, ~0x2e, ~0)|ASI(~0),	"1,2,d", 0, v9 },
{ "srlx",	F3(0x2, 0x2e, 1), F3(~0x2, ~0x2e, ~1)|(0x7f<<6),	"1,i,d", 0, v9 },

{ "mulscc",	0x81200000, 0x40d80000, "1,2,d", 0, v6 },
{ "mulscc",	0x81202000, 0x40d80000, "1,i,d", 0, v6 },

{ "clr",	0x80100000, 0x4e87e01f, "d", F_ALIAS, v6 }, /* or %g0,%g0,d */
{ "clr",	0x80102000, 0x41efdfff, "d", F_ALIAS, v6 }, /* or %g0,0,d	*/
{ "clr",	0xc0200000, 0x3fd82000, "[1+2]", F_ALIAS, v6 },
{ "clr",	0xc0200000, 0x3fd8201f, "[1]", F_ALIAS, v6 }, /* st %g0,[rs1+%g0] */
{ "clr",	0xc0202000, 0x3fd80000, "[1+i]", F_ALIAS, v6 },
{ "clr",	0xc0202000, 0x3fd80000, "[i+1]", F_ALIAS, v6 },
{ "clr",	0xc0202000, 0x3fd81fff, "[1]", F_ALIAS, v6 }, /* st %g0,[rs1+0] */

{ "clrb",	0xc0280000, 0x3fd02000, "[1+2]", F_ALIAS, v6 },
{ "clrb",	0xc0280000, 0x3fd0201f, "[1]", F_ALIAS, v6 }, /* stb %g0,[rs1+%g0] */
{ "clrb",	0xc0282000, 0x3fd00000, "[1+i]", F_ALIAS, v6 },
{ "clrb",	0xc0282000, 0x3fd00000, "[i+1]", F_ALIAS, v6 },

{ "clrh",	0xc0300000, 0x3fc82000, "[1+2]", F_ALIAS, v6 },
{ "clrh",	0xc0300000, 0x3fc8201f, "[1]", F_ALIAS, v6 }, /* sth %g0,[rs1+%g0] */
{ "clrh",	0xc0302000, 0x3fc80000, "[1+i]", F_ALIAS, v6 },
{ "clrh",	0xc0302000, 0x3fc80000, "[i+1]", F_ALIAS, v6 },

{ "orcc",	0x80900000, 0x41680000, "1,2,d", 0, v6 },
{ "orcc",	0x80902000, 0x41680000, "1,i,d", 0, v6 },
{ "orcc",	0x80902000, 0x41680000, "i,1,d", 0, v6 },

{ "orncc",	0x80b00000, 0x04048000, "1,2,d", 0, v6 },
{ "orncc",	0x80b02000, 0x04048000, "1,i,d", 0, v6 },
{ "orncc",	0x80b02000, 0x04048000, "i,1,d", 0, v6 },

{ "orn",	0x80300000, 0x41c80000, "1,2,d", 0, v6 },
{ "orn",	0x80302000, 0x41c80000, "1,i,d", 0, v6 },
{ "orn",	0x80302000, 0x41c80000, "i,1,d", 0, v6 },

{ "tst",	0x80900000, 0x7f68201f, "1", 0, v6 }, /* orcc rs1, %g0, %g0 */
{ "tst",	0x80900000, 0x7f6fe000, "2", 0, v6 }, /* orcc %g0, rs2, %g0 */
{ "tst",	0x80902000, 0x7f681fff, "1", 0, v6 }, /* orcc rs1, 0, %g0 */

{ "wr",	F3(0x2, 0x30, 0), F3(~0x2, ~0x30, ~0)|ASI(~0),			"1,2,m", 0, v8 }, /* wr r,r,%asrX */
{ "wr",	F3(0x2, 0x30, 0)|RD(0), F3(~0x2, ~0x30, ~0)|RD(~0)|ASI(~0),	"1,2,y", 0, v6 }, /* wr r,r,%y */
{ "wr",	F3(0x2, 0x30, 0)|RD(0xe), F3(~0x2, ~0x30, ~0)|RD(0x1)|ASI(~0),	"1,2,Y", 0, v9 }, /* wr r,r,%amr */
{ "wr",	F3(0x2, 0x30, 1), F3(~0x2, ~0x30, ~1),				"1,i,m", 0, v8 }, /* wr r,i,%asrX */
{ "wr",	F3(0x2, 0x30, 1)|RD(0), F3(~0x2, ~0x30, ~1)|RD(~0),		"1,i,y", 0, v6 }, /* wr r,i,%y */
{ "wr",	F3(0x2, 0x30, 1)|RD(0xe), F3(~0x2, ~0x30, ~1)|RD(0x1),		"1,i,Y", 0, v9 }, /* wr r,i,%amr */
{ "wr",	F3(0x2, 0x31, 0)|RD(0), F3(~0x2, ~0x31, ~0)|RD(~0)|ASI(~0),	"1,2,p", 0, v6 }, /* wr r,r,%psr */
{ "wr",	F3(0x2, 0x31, 1)|RD(0), F3(~0x2, ~0x31, ~1)|RD(~0),		"1,i,p", 0, v6 }, /* wr r,i,%psr */
{ "wr",	F3(0x2, 0x32, 0)|RD(0), F3(~0x2, ~0x32, ~0)|RD(~0)|ASI(~0),	"1,2,w", 0, v6 }, /* wr r,r,%wim */
{ "wr",	F3(0x2, 0x32, 1)|RD(0), F3(~0x2, ~0x32, ~1)|RD(~0),		"1,i,w", 0, v6 }, /* wr r,i,%wim */
{ "wr",	F3(0x2, 0x33, 0)|RD(0), F3(~0x2, ~0x33, ~0)|RD(~0)|ASI(~0),	"1,2,t", 0, v6 }, /* wr r,r,%tbr */
{ "wr",	F3(0x2, 0x33, 1)|RD(0), F3(~0x2, ~0x33, ~1)|RD(~0),		"1,i,t", 0, v6 }, /* wr r,i,%tbr */

{ "rd",	F3(0x2, 0x28, 0), F3(~0x2, ~0x28, ~0)|SIMM13(~0),			"M,d", 0, v8 }, /* rd %asr1,r */
{ "rd",	F3(0x2, 0x28, 0)|RS1(0), F3(~0x2, ~0x28, ~0)|RS1(0x1f)|SIMM13(~0),	"y,d", 0, v6 }, /* rd %y,r */
{ "rd",	F3(0x2, 0x28, 0)|RS1(0xe), F3(~0x2, ~0x28, ~0)|RS1(0x1)|SIMM13(~0),	"Y,d", 0, v9 }, /* rd %amr,r */
{ "rd",	F3(0x2, 0x29, 0)|RS1(0), F3(~0x2, ~0x29, ~0)|RS1(0x1f)|SIMM13(~0),	"p,d", 0, v6 }, /* rd %psr,r */
{ "rd",	F3(0x2, 0x2a, 0)|RS1(0), F3(~0x2, ~0x2a, ~0)|RS1(0x1f)|SIMM13(~0),	"w,d", 0, v6 }, /* rd %wim,r */
{ "rd",	F3(0x2, 0x2b, 0)|RS1(0), F3(~0x2, ~0x2b, ~0)|RS1(0x1f)|SIMM13(~0),	"t,d", 0, v6 }, /* rd %tbr,r */

{ "mov",	F3(0x2, 0x30, 0), F3(~0x2, ~0x30, ~0)|ASI(~0),			"1,2,m", F_ALIAS, v8 }, /* wr r,r,%asrX */
{ "mov",	F3(0x2, 0x30, 0)|RD(0), F3(~0x2, ~0x30, ~0)|RD(~0)|ASI(~0),	"1,2,y", F_ALIAS, v6 }, /* wr r,r,%y */
{ "mov",	F3(0x2, 0x30, 0)|RD(0xe), F3(~0x2, ~0x30, ~0)|RD(0x1)|ASI(~0),	"1,2,Y", F_ALIAS, v9 }, /* wr r,r,%amr */
{ "mov",	F3(0x2, 0x30, 1), F3(~0x2, ~0x30, ~1),				"1,i,m", F_ALIAS, v8 }, /* wr r,i,%asrX */
{ "mov",	F3(0x2, 0x30, 1)|RD(0), F3(~0x2, ~0x30, ~1)|RD(~0),		"1,i,y", F_ALIAS, v6 }, /* wr r,i,%y */
{ "mov",	F3(0x2, 0x30, 1)|RD(0xe), F3(~0x2, ~0x30, ~1)|RD(0x1),		"1,i,Y", F_ALIAS, v9 }, /* wr r,i,%amr */
{ "mov",	F3(0x2, 0x31, 0)|RD(0), F3(~0x2, ~0x31, ~0)|RD(~0)|ASI(~0),	"1,2,p", F_ALIAS, v6 }, /* wr r,r,%psr */
{ "mov",	F3(0x2, 0x31, 1)|RD(0), F3(~0x2, ~0x31, ~1)|RD(~0),		"1,i,p", F_ALIAS, v6 }, /* wr r,i,%psr */
{ "mov",	F3(0x2, 0x32, 0)|RD(0), F3(~0x2, ~0x32, ~0)|RD(~0)|ASI(~0),	"1,2,w", F_ALIAS, v6 }, /* wr r,r,%wim */
{ "mov",	F3(0x2, 0x32, 1)|RD(0), F3(~0x2, ~0x32, ~1)|RD(~0),		"1,i,w", F_ALIAS, v6 }, /* wr r,i,%wim */
{ "mov",	F3(0x2, 0x33, 0)|RD(0), F3(~0x2, ~0x33, ~0)|RD(~0)|ASI(~0),	"1,2,t", F_ALIAS, v6 }, /* wr r,r,%tbr */
{ "mov",	F3(0x2, 0x33, 1)|RD(0), F3(~0x2, ~0x33, ~1)|RD(~0),		"1,i,t", F_ALIAS, v6 }, /* wr r,i,%tbr */

{ "mov",	F3(0x2, 0x28, 0), F3(~0x2, ~0x28, ~0)|SIMM13(~0),			"M,d", F_ALIAS, v8 }, /* rd %asr1,r */
{ "mov",	F3(0x2, 0x28, 0)|RS1(0), F3(~0x2, ~0x28, ~0)|RS1(0x1f)|SIMM13(~0),	"y,d", F_ALIAS, v6 }, /* rd %y,r */
{ "mov",	F3(0x2, 0x28, 0)|RS1(0xe), F3(~0x2, ~0x28, ~0)|RS1(0x1)|SIMM13(~0),	"Y,d", F_ALIAS, v9 }, /* rd %amr,r */
{ "mov",	F3(0x2, 0x29, 0)|RS1(0), F3(~0x2, ~0x29, ~0)|RS1(0x1f)|SIMM13(~0),	"p,d", F_ALIAS, v6 }, /* rd %psr,r */
{ "mov",	F3(0x2, 0x2a, 0)|RS1(0), F3(~0x2, ~0x2a, ~0)|RS1(0x1f)|SIMM13(~0),	"w,d", F_ALIAS, v6 }, /* rd %wim,r */
{ "mov",	F3(0x2, 0x2b, 0)|RS1(0), F3(~0x2, ~0x2b, ~0)|RS1(0x1f)|SIMM13(~0),	"t,d", F_ALIAS, v6 }, /* rd %tbr,r */

{ "mov",	0x81800000, 0x4078201f, "1,y", F_ALIAS, v6 }, /* wr rs1,%g0,%y */
{ "mov",	0x81802000, 0x40780000, "i,y", F_ALIAS, v6 },
{ "mov",	0x81802000, 0x40781fff, "1,y", F_ALIAS, v6 }, /* wr rs1,0,%y */
{ "mov",	0x81880000, 0x4070201f, "1,p", F_ALIAS, v6 }, /* wr rs1,%g0,%psr */
{ "mov",	0x81882000, 0x40700000, "i,p", F_ALIAS, v6 },
{ "mov",	0x81882000, 0x40701fff, "1,p", F_ALIAS, v6 }, /* wr rs1,0,%psr */
{ "mov",	0x81900000, 0x4068201f, "1,w", F_ALIAS, v6 }, /* wr rs1,%g0,%wim */
{ "mov",	0x81902000, 0x40680000, "i,w", F_ALIAS, v6 },
{ "mov",	0x81902000, 0x40681fff, "1,w", F_ALIAS, v6 }, /* wr rs1,0,%wim */
{ "mov",	0x81980000, 0x4060201f, "1,t", F_ALIAS, v6 }, /* wr rs1,%g0,%tbr */
{ "mov",	0x81982000, 0x40600000, "i,t", F_ALIAS, v6 },
{ "mov",	0x81982000, 0x40601fff, "1,t", F_ALIAS, v6 }, /* wr rs1,0,%tbr */

{ "mov",	0x80100000, 0x41efe000, "2,d", 0, v6 }, /* or %g0,rs2,d */
{ "mov",	0x80102000, 0x41efc000, "i,d", 0, v6 }, /* or %g0,i,d	*/
{ "mov",        0x80100000, 0x41e8201f, "1,d", 0 }, /* or rs1,%g0,d   */
{ "mov",        0x80102000, 0x41e81fff, "1,d", 0 }, /* or rs1,0,d */

{ "or",	0x80100000, 0x40800000, "1,2,d", 0, v6 },
{ "or",	0x80102000, 0x40800000, "1,i,d", 0, v6 },
{ "or",	0x80102000, 0x40800000, "i,1,d", 0, v6 },

{ "bset",	0x80100000, 0x40800000, "2,r", F_ALIAS, v6 },	/* or rd,rs2,rd */
{ "bset",	0x80102000, 0x40800000, "i,r", F_ALIAS, v6 },	/* or rd,i,rd */

{ "andn",	0x80280000, 0x41d00000, "1,2,d", 0, v6 },
{ "andn",	0x80282000, 0x41d00000, "1,i,d", 0, v6 },
{ "andn",	0x80282000, 0x41d00000, "i,1,d", 0, v6 },

{ "andncc",	0x80a80000, 0x41500000, "1,2,d", 0, v6 },
{ "andncc",	0x80a82000, 0x41500000, "1,i,d", 0, v6 },
{ "andncc",	0x80a82000, 0x41500000, "i,1,d", 0, v6 },

{ "bclr",	0x80280000, 0x41d00000, "2,r", F_ALIAS, v6 },	/* andn rd,rs2,rd */
{ "bclr",	0x80282000, 0x41d00000, "i,r", F_ALIAS, v6 },	/* andn rd,i,rd */

{ "cmp",	F3(0x2, 0x14, 0), F3(~0x2, ~0x14, ~0)|RD(~0)|ASI(~0),	"1,2", 0, v6 },	/* subcc rs1,rs2,%g0 */
{ "cmp",	F3(0x2, 0x14, 1), F3(~0x2, ~0x14, ~1)|RD(~0),		"1,i", 0, v6 },	/* subcc rs1,i,%g0 */

{ "sub",	0x80200000, 0x41d80000, "1,2,d", 0, v6 },
{ "sub",	0x80202000, 0x41d80000, "1,i,d", 0, v6 },

{ "subcc",	0x80a00000, 0x41580000, "1,2,d", 0, v6 },
{ "subcc",	0x80a02000, 0x41580000, "1,i,d", 0, v6 },

{ "subx",	0x80600000, 0x41980000, "1,2,d", 0, v6 },
{ "subx",	0x80602000, 0x41980000, "1,i,d", 0, v6 },

{ "subxcc",	0x80e00000, 0x41180000, "1,2,d", 0, v6 },
{ "subxcc",	0x80e02000, 0x41180000, "1,i,d", 0, v6 },

{ "and",	0x80080000, 0x41f00000, "1,2,d", 0, v6 },
{ "and",	0x80082000, 0x41f00000, "1,i,d", 0, v6 },
{ "and",	0x80082000, 0x41f00000, "i,1,d", 0, v6 },

{ "andcc",	0x80880000, 0x41700000, "1,2,d", 0, v6 },
{ "andcc",	0x80882000, 0x41700000, "1,i,d", 0, v6 },
{ "andcc",	0x80882000, 0x41700000, "i,1,d", 0, v6 },

{ "dec",	0x80202001, 0x41d81ffe, "r", F_ALIAS, v6 },	/* sub rd,1,rd */
{ "deccc",	0x80a02001, 0x41581ffe, "r", F_ALIAS, v6 },	/* subcc rd,1,rd */
{ "inc",	0x80002001, 0x41f81ffe, "r", F_ALIAS, v6 },	/* add rs1,1,rsd */
{ "inccc",	0x80802001, 0x41781ffe, "r", F_ALIAS, v6 },	/* addcc rd,1,rd */

{ "btst",	F3(2, 0x11, 0), F3(~2, ~0x11, ~0)|RD(~0)|ASI(~0), "1,2", F_ALIAS, v6 },	/* andcc rs1,rs2,%g0 */
{ "btst",	F3(2, 0x11, 1), F3(~2, ~0x11, ~1)|RD(~0), "i,1", F_ALIAS, v6 },	/* andcc rs1,i,%g0 */

{ "neg",	F3(2, 0x04, 0), F3(~2, ~0x04, ~0)|RS1(~0)|ASI(~0), "2,d", F_ALIAS, v6 }, /* sub %g0,rs2,rd */
{ "neg",	F3(2, 0x04, 0), F3(~2, ~0x04, ~0)|RS1(~0)|ASI(~0), "r", F_ALIAS, v6 }, /* sub %g0,rd,rd */

{ "add",	0x80000000, 0x41f80000, "1,2,d", 0, v6 },
{ "add",	0x80002000, 0x41f80000, "1,i,d", 0, v6 },
{ "add",	0x80002000, 0x41f80000, "i,1,d", 0, v6 },
{ "addcc",	0x80800000, 0x41780000, "1,2,d", 0, v6 },
{ "addcc",	0x80802000, 0x41780000, "1,i,d", 0, v6 },
{ "addcc",	0x80802000, 0x41780000, "i,1,d", 0, v6 },
{ "addx",	0x80400000, 0x41b80000, "1,2,d", 0, v6 },
{ "addx",	0x80402000, 0x41b80000, "1,i,d", 0, v6 },
{ "addx",	0x80402000, 0x41b80000, "i,1,d", 0, v6 },
{ "addxcc",	0x80c00000, 0x41380000, "1,2,d", 0, v6 },
{ "addxcc",	0x80c02000, 0x41380000, "1,i,d", 0, v6 },
{ "addxcc",	0x80c02000, 0x41380000, "i,1,d", 0, v6 },

{ "smul",	F3(0x2, 0x0b, 0), F3(~0x2, ~0x0b, ~0)|ASI(0x1fff),	"1,2,d", 0, v8 },
{ "smul",	F3(0x2, 0x0b, 1), F3(~0x2, ~0x0b, ~1),		"1,i,d", 0, v8 },
{ "smul",	F3(0x2, 0x0b, 1), F3(~0x2, ~0x0b, ~1),		"i,1,d", 0, v8 },
{ "smulcc",	F3(0x2, 0x1b, 0), F3(~0x2, ~0x1b, ~0)|ASI(0x1fff),	"1,2,d", 0, v8 },
{ "smulcc",	F3(0x2, 0x1b, 1), F3(~0x2, ~0x1b, ~1),		"1,i,d", 0, v8 },
{ "smulcc",	F3(0x2, 0x1b, 1), F3(~0x2, ~0x1b, ~1),		"i,1,d", 0, v8 },
{ "umul",	F3(0x2, 0x0a, 0), F3(~0x2, ~0x0a, ~0)|ASI(0x1fff),	"1,2,d", 0, v8 },
{ "umul",	F3(0x2, 0x0a, 1), F3(~0x2, ~0x0a, ~1),		"1,i,d", 0, v8 },
{ "umul",	F3(0x2, 0x0a, 1), F3(~0x2, ~0x0a, ~1),		"i,1,d", 0, v8 },
{ "umulcc",	F3(0x2, 0x1a, 0), F3(~0x2, ~0x1a, ~0)|ASI(0x1fff),	"1,2,d", 0, v8 },
{ "umulcc",	F3(0x2, 0x1a, 1), F3(~0x2, ~0x1a, ~1),		"1,i,d", 0, v8 },
{ "umulcc",	F3(0x2, 0x1a, 1), F3(~0x2, ~0x1a, ~1),		"i,1,d", 0, v8 },

{ "sdiv",	F3(0x2, 0x0f, 0), F3(~0x2, ~0x0f, ~0)|ASI(0x1fff),	"1,2,d", 0, v8 },
{ "sdiv",	F3(0x2, 0x0f, 1), F3(~0x2, ~0x0f, ~1),		"1,i,d", 0, v8 },
{ "sdiv",	F3(0x2, 0x0f, 1), F3(~0x2, ~0x0f, ~1),		"i,1,d", 0, v8 },
{ "sdivcc",	F3(0x2, 0x1f, 0), F3(~0x2, ~0x1f, ~0)|ASI(0x1fff),	"1,2,d", 0, v8 },
{ "sdivcc",	F3(0x2, 0x1f, 1), F3(~0x2, ~0x1f, ~1),		"1,i,d", 0, v8 },
{ "sdivcc",	F3(0x2, 0x1f, 1), F3(~0x2, ~0x1f, ~1),		"i,1,d", 0, v8 },
{ "udiv",	F3(0x2, 0x0e, 0), F3(~0x2, ~0x0e, ~0)|ASI(0x1fff),	"1,2,d", 0, v8 },
{ "udiv",	F3(0x2, 0x0e, 1), F3(~0x2, ~0x0e, ~1),		"1,i,d", 0, v8 },
{ "udiv",	F3(0x2, 0x0e, 1), F3(~0x2, ~0x0e, ~1),		"i,1,d", 0, v8 },
{ "udivcc",	F3(0x2, 0x1e, 0), F3(~0x2, ~0x1e, ~0)|ASI(0x1fff),	"1,2,d", 0, v8 },
{ "udivcc",	F3(0x2, 0x1e, 1), F3(~0x2, ~0x1e, ~1),		"1,i,d", 0, v8 },
{ "udivcc",	F3(0x2, 0x1e, 1), F3(~0x2, ~0x1e, ~1),		"i,1,d", 0, v8 },

{ "call",	0x40000000, 0x80000000, "L", F_DELAYED, v6 },
{ "call",	0x40000000, 0x80000000, "L,#", F_DELAYED, v6 },
{ "call",	0x9fc00000, 0x4038201f, "1", F_DELAYED, v6 }, /* jmpl rs1+%g0, %o7 */
{ "call",	0x9fc00000, 0x4038201f, "1,#", F_DELAYED, v6 },

/* Conditional instructions.

   Because this part of the table was such a mess earlier, I have
   macrofied it so that all the branches and traps are generated from
   a single-line description of each condition value.  */

#define ANNUL	(1<<29)
#define BPRED	(1<<21)	/* v9 */
#define	IMMED	F3I(1)
#define	RS1_G0	RS1(~0)
#define	RS2_G0	RS2(~0)

/* Define branches -- one annulled, one without, etc. */
#define br(opcode, mask, lose, flags) \
 { opcode, (mask)|ANNUL, (lose),       ",al",   (flags), v6 }, \
 { opcode, (mask)      , (lose)|ANNUL, "l",     (flags), v6 }

#define brx(opcode, mask, lose, flags) /* v9 */ \
 { opcode, (mask), (lose)|ANNUL|BPRED, "K",   (flags), v9 }, \
 { opcode, (mask), (lose)|ANNUL|BPRED, ",NK",   (flags), v9 }, \
 { opcode, (mask)|ANNUL, (lose)|BPRED, ",aK", (flags), v9 }, \
 { opcode, (mask)|ANNUL, (lose)|BPRED, ",a,NK", (flags), v9 }, \
 { opcode, (mask)|BPRED, (lose)|ANNUL, ",TK",   (flags), v9 }, \
 { opcode, (mask)|ANNUL|BPRED, (lose), ",a,TK", (flags), v9 }

/* Define four traps: reg+reg, reg + immediate, immediate alone, reg alone. */
#define tr(opcode, mask, lose, flags) \
 { opcode, (mask)|IMMED, (lose)|RS1_G0      , "i", (flags), v6 }, /* %g0 + imm */ \
 { opcode, (mask)|IMMED, (lose)             , "1+i", (flags), v6 }, /* rs1 + imm */ \
 { opcode, (mask)      , (lose)|IMMED       , "1+2", (flags), v6 }, /* rs1 + rs2 */ \
 { opcode, (mask)      , (lose)|IMMED|RS2_G0, "1", (flags), v6 } /* rs1 + %g0 */

#define trx(opcode, mask, lose, flags) /* v9 */ \
 { opcode, (mask)|IMMED, (lose)|(0x3f<<6)|RS1_G0      , "i", (flags), v9 }, /* %g0 + imm */ \
 { opcode, (mask)|IMMED, (lose)|(0x3f<<6)             , "1+i", (flags), v9 }, /* rs1 + imm */ \
 { opcode, (mask)      , (lose)|(0x3f<<6)|IMMED       , "1+2", (flags), v9 }, /* rs1 + rs2 */ \
 { opcode, (mask)      , (lose)|(0x3f<<6)|IMMED|RS2_G0, "1", (flags), v9 } /* rs1 + %g0 */

/* Define both branches and traps based on condition mask */
#define cond(bop, top, mask, flags) \
  br(bop,  F2(0, 2)|(mask), F2(~0, ~2)|((~mask)&COND(~0)), F_DELAYED|(flags)), \
  brx(bop, F2(0, 1)|(mask), F2(~0, ~1)|((~mask)&COND(~0)), F_DELAYED|(flags)), /* v9 */ \
  tr(top,  F3(2, 0x3a, 0)|(mask), F3(~2, ~0x3a, 0)|((~mask)&COND(~0)), (flags))

#define condx(bop, top, mask, flags) /* v9 */ \
  brx(bop, F2(0, 3)|(mask), F2(~0, ~3)|((~(mask))&COND(~0)), (flags)), /* v9 */ \
  trx(top, F3(0x2, 0x3e, 0)|(mask), F3(~0x2, ~0x3e, 0)|((~(mask))&COND(~0)), (flags)) /* v9 */

/* Define all the conditions, all the branches, all the traps.  */

cond ("b",	"t",    CONDA, 0),
cond ("ba",	"ta",   CONDA, F_ALIAS), /* for nothing */
cond ("bcc",	"tcc",  CONDCC, 0),
cond ("bcs",	"tcs",  CONDCS, 0),
cond ("be",	"te",   CONDE, 0),
cond ("bg",	"tg",   CONDG, 0),
cond ("bge",	"tge",  CONDGE, 0),
cond ("bgeu",	"tgeu", CONDGEU, F_ALIAS), /* for cc */
cond ("bgu",	"tgu",  CONDGU, 0),
cond ("bl",	"tl",   CONDL, 0),
cond ("ble",	"tle",  CONDLE, 0),
cond ("bleu",	"tleu", CONDLEU, 0),
cond ("blu",	"tlu",  CONDLU, F_ALIAS), /* for cs */
cond ("bn",	"tn",   CONDN, 0),
cond ("bne",	"tne",  CONDNE, 0),
cond ("bneg",	"tneg", CONDNEG, 0),
cond ("bnz",	"tnz",  CONDNZ, F_ALIAS), /* for ne */
cond ("bpos",	"tpos", CONDPOS, 0),
cond ("bvc",	"tvc",  CONDVC, 0),
cond ("bvs",	"tvs",  CONDVS, 0),
cond ("bz",	"tz",   CONDZ, F_ALIAS), /* for e */

#undef cond
#undef br
#undef tr

/* v9 */ condx ("bx",	"tx",    CONDA, 0),
/* v9 */ condx ("bxa",	"txa",   CONDA, F_ALIAS), /* for nothing */
/* v9 */ condx ("bxcc",	"txcc",  CONDCC, 0),
/* v9 */ condx ("bxcs",	"txcs",  CONDCS, 0),
/* v9 */ condx ("bxe",	"txe",   CONDE, 0),
/* v9 */ condx ("bxg",	"txg",   CONDG, 0),
/* v9 */ condx ("bxge",	"txge",  CONDGE, 0),
/* v9 */ condx ("bxgeu",	"txgeu", CONDGEU, F_ALIAS), /* for cc */
/* v9 */ condx ("bxgu",	"txgu",  CONDGU, 0),
/* v9 */ condx ("bxl",	"txl",   CONDL, 0),
/* v9 */ condx ("bxle",	"txle",  CONDLE, 0),
/* v9 */ condx ("bxleu",	"txleu", CONDLEU, 0),
/* v9 */ condx ("bxlu",	"txlu",  CONDLU, F_ALIAS), /* for cs */
/* v9 */ condx ("bxn",	"txn",   CONDN, 0),
/* v9 */ condx ("bxne",	"txne",  CONDNE, 0),
/* v9 */ condx ("bxneg",	"txneg", CONDNEG, 0),
/* v9 */ condx ("bxnz",	"txnz",  CONDNZ, F_ALIAS), /* for ne */
/* v9 */ condx ("bxpos",	"txpos", CONDPOS, 0),
/* v9 */ condx ("bxvc",	"txvc",  CONDVC, 0),
/* v9 */ condx ("bxvs",	"txvs",  CONDVS, 0),
/* v9 */ condx ("bxz",	"txz",   CONDZ, F_ALIAS), /* for e */

#undef condx /* v9 */
#undef brx /* v9 */
#undef trx /* v9 */

{ "brneg",	0x17400000, 0xc8910000, ",Tk", F_DELAYED, v9 },
{ "brneg",	0x17400000, 0xc8910000, ",Nk", F_DELAYED, v9 },
{ "brneg",	0x17400000, 0xc8910000, ",a,Tk", F_DELAYED, v9 },
{ "brneg",	0x17400000, 0xc8910000, ",a,Nk", F_DELAYED, v9 },
{ "brneg",	0x17400000, 0xc8910000, ",ak", F_DELAYED, v9 },
{ "brneg",	0x17400000, 0xc8910000, "k", F_DELAYED, v9 },
{ "brneg32",	0x07400000, 0xd8910000, ",Tk", F_DELAYED, v9 },
{ "brneg32",	0x07400000, 0xd8910000, ",Nk", F_DELAYED, v9 },
{ "brneg32",	0x07400000, 0xd8910000, ",a,Tk", F_DELAYED, v9 },
{ "brneg32",	0x07400000, 0xd8910000, ",a,Nk", F_DELAYED, v9 },
{ "brneg32",	0x07400000, 0xd8910000, ",ak", F_DELAYED, v9 },
{ "brneg32",	0x07400000, 0xd8910000, "k", F_DELAYED, v9 },
{ "brnz",	0x13400000, 0xcc910000, ",Tk", F_DELAYED, v9 },
{ "brnz",	0x13400000, 0xcc910000, ",Nk", F_DELAYED, v9 },
{ "brnz",	0x13400000, 0xcc910000, ",a,Tk", F_DELAYED, v9 },
{ "brnz",	0x13400000, 0xcc910000, ",a,Nk", F_DELAYED, v9 },
{ "brnz",	0x13400000, 0xcc910000, ",ak", F_DELAYED, v9 },
{ "brnz",	0x13400000, 0xcc910000, "k", F_DELAYED, v9 },
{ "brnz32",	0x03400000, 0xdc910000, ",Tk", F_DELAYED, v9 },
{ "brnz32",	0x03400000, 0xdc910000, ",Nk", F_DELAYED, v9 },
{ "brnz32",	0x03400000, 0xdc910000, ",a,Tk", F_DELAYED, v9 },
{ "brnz32",	0x03400000, 0xdc910000, ",a,Nk", F_DELAYED, v9 },
{ "brnz32",	0x03400000, 0xdc910000, ",ak", F_DELAYED, v9 },
{ "brnz32",	0x03400000, 0xdc910000, "k", F_DELAYED, v9 },
{ "brpos",	0x15400000, 0xca910000, ",Tk", F_DELAYED, v9 },
{ "brpos",	0x15400000, 0xca910000, ",Nk", F_DELAYED, v9 },
{ "brpos",	0x15400000, 0xca910000, ",a,Tk", F_DELAYED, v9 },
{ "brpos",	0x15400000, 0xca910000, ",a,Nk", F_DELAYED, v9 },
{ "brpos",	0x15400000, 0xca910000, ",ak", F_DELAYED, v9 },
{ "brpos",	0x15400000, 0xca910000, "k", F_DELAYED, v9 },
{ "brpos32",	0x05400000, 0xda910000, ",Tk", F_DELAYED, v9 },
{ "brpos32",	0x05400000, 0xda910000, ",Nk", F_DELAYED, v9 },
{ "brpos32",	0x05400000, 0xda910000, ",a,Tk", F_DELAYED, v9 },
{ "brpos32",	0x05400000, 0xda910000, ",a,Nk", F_DELAYED, v9 },
{ "brpos32",	0x05400000, 0xda910000, ",ak", F_DELAYED, v9 },
{ "brpos32",	0x05400000, 0xda910000, "k", F_DELAYED, v9 },
{ "brz",	0x11400000, 0xce910000, ",Tk", F_DELAYED, v9 },
{ "brz",	0x11400000, 0xce910000, ",Nk", F_DELAYED, v9 },
{ "brz",	0x11400000, 0xce910000, ",a,Tk", F_DELAYED, v9 },
{ "brz",	0x11400000, 0xce910000, ",a,Nk", F_DELAYED, v9 },
{ "brz",	0x11400000, 0xce910000, ",ak", F_DELAYED, v9 },
{ "brz",	0x11400000, 0xce910000, "k", F_DELAYED, v9 },
{ "brz32",	0x01400000, 0xde910000, ",Tk", F_DELAYED, v9 },
{ "brz32",	0x01400000, 0xde910000, ",Nk", F_DELAYED, v9 },
{ "brz32",	0x01400000, 0xde910000, ",a,Tk", F_DELAYED, v9 },
{ "brz32",	0x01400000, 0xde910000, ",a,Nk", F_DELAYED, v9 },
{ "brz32",	0x01400000, 0xde910000, ",ak", F_DELAYED, v9 },
{ "brz32",	0x01400000, 0xde910000, "k", F_DELAYED, v9 },

{ "jmp",	0x81c00000, 0x7e382000, "1+2", F_DELAYED, v6 }, /* jmpl rs1+rs2,%g0 */
{ "jmp",	0x81c00000, 0x7e38201f, "1", F_DELAYED, v6 }, /* jmpl rs1+%g0,%g0 */
{ "jmp",	0x81c02000, 0x7e380000, "1+i", F_DELAYED, v6 }, /* jmpl rs1+i,%g0 */
{ "jmp",	0x81c02000, 0x7e380000, "i+1", F_DELAYED, v6 }, /* jmpl i+rs1,%g0 */
{ "jmp",	0x81c02000, 0x7e3fc000, "i", F_DELAYED, v6 }, /* jmpl %g0+i,%g0 */

{ "nop",	F2(0, 4), ~F2(0, 4), "", 0, v6 }, /* sethi 0, %g0 */

{ "set",	0x01000000, 0xc0c00000, "Sh,d", F_ALIAS, v6 },

{ "sethi",	0x01000000, 0xc0c00000, "h,d", 0, v6 },

{ "taddcc",	0x81000000, 0x40f80000, "1,2,d", 0, v6 },
{ "taddcc",	0x81002000, 0x40f80000, "1,i,d", 0, v6 },
{ "taddcctv",	0x81100000, 0x40e00000, "1,2,d", 0, v6 },
{ "taddcctv",	0x81102000, 0x40e00000, "1,i,d", 0, v6 },

{ "tsubcc",	0x81080000, 0x40f00000, "1,2,d", 0, v6 },
{ "tsubcc",	0x81082000, 0x40f00000, "1,i,d", 0, v6 },
{ "tsubcctv",	0x80580000, 0x40a00000, "1,2,d", 0, v6 },
{ "tsubcctv",	0x80582000, 0x40a00000, "1,i,d", 0, v6 },

{ "unimp",	0x00000000, 0xffffffff, "l", 0, v6 },

{ "iflush",	0x81d80000, 0x40202000, "1+2", 0, v6 },
{ "iflush",	0x81d82000, 0x40200000, "1+i", 0, v6 },

{ "xnor",	0x80380000, 0x41c00000, "1,2,d", 0, v6 },
{ "xnor",	0x80382000, 0x41c00000, "1,i,d", 0, v6 },
{ "xnor",	0x80382000, 0x41c00000, "i,1,d", 0, v6 },
{ "xnorcc",	0x80b80000, 0x41400000, "1,2,d", 0, v6 },
{ "xnorcc",	0x80b82000, 0x41400000, "1,i,d", 0, v6 },
{ "xnorcc",	0x80b82000, 0x41400000, "i,1,d", 0, v6 },
{ "xor",	0x80180000, 0x41e00000, "1,2,d", 0, v6 },
{ "xor",	0x80182000, 0x41e00000, "1,i,d", 0, v6 },
{ "xor",	0x80182000, 0x41e00000, "i,1,d", 0, v6 },
{ "xorcc",	0x80980000, 0x41600000, "1,2,d", 0, v6 },
{ "xorcc",	0x80982000, 0x41600000, "1,i,d", 0, v6 },
{ "xorcc",	0x80982000, 0x41600000, "i,1,d", 0, v6 },

{ "not",	0x80380000, 0x41c00000, "1,d", F_ALIAS, v6 }, /* xnor rs1,%0,rd */
{ "not",	0x80380000, 0x41c00000, "r", F_ALIAS, v6 }, /* xnor rd,%0,rd */

{ "btog",	0x80180000, 0x41e02000, "2,r", F_ALIAS, v6 }, /* xor rd,rs2,rd */
{ "btog",	0x80182000, 0x41e00000, "i,r", F_ALIAS, v6 }, /* xor rd,i,rd */

{ "fpop1",	0x81a00000, 0x40580000, "[1+2],d", 0, v6 },
{ "fpop2",	0x81a80000, 0x40500000, "[1+2],d", 0, v6 },

{ "fb",	0x11800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fb",	0x31800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fba",	0x11800000, 0xc0400000, "l", F_DELAYED|F_ALIAS, v6 },
{ "fba",	0x31800000, 0xc0400000, ",al", F_DELAYED|F_ALIAS, v6 },
{ "fbe",	0x13800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbe",	0x33800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbg",	0x0d800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbg",	0x2d800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbge",	0x17800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbge",	0x37800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbl",	0x09800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbl",	0x29800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fble",	0x1b800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fble",	0x3b800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fblg",	0x05800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fblg",	0x25800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbn",	0x01800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbn",	0x21800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbne",	0x03800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbne",	0x23800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbo",	0x1f800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbo",	0x3f800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbu",	0x0f800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbu",	0x2f800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbue",	0x15800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbue",	0x35800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbug",	0x0b800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbug",	0x2b800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbuge",	0x19800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbuge",	0x39800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbul",	0x07800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbul",	0x27800000, 0xc0400000, ",al", F_DELAYED, v6 },
{ "fbule",	0x1d800000, 0xc0400000, "l", F_DELAYED, v6 },
{ "fbule",	0x3d800000, 0xc0400000, ",al", F_DELAYED, v6 },

{ "cb0",	0x13c00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb0",	0x33c00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb01",	0x1bc00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb01",	0x3bc00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb012",	0x1fc00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb012",	0x3fc00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb013",	0x1dc00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb013",	0x3dc00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb02",	0x17c00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb02",	0x37c00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb023",	0x19c00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb023",	0x39c00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb03",	0x15c00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb03",	0x35c00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb1",	0x09c00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb1",	0x29c00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb12",	0x05c00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb12",	0x25c00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb123",	0x03c00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb123",	0x23c00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb13",	0x07c00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb13",	0x27c00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb2",	0x0dc00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb2",	0x2dc00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb23",	0x0bc00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb23",	0x2bc00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cb3",	0x0fc00000, 0xc0000000, "l", F_DELAYED, v6 },
{ "cb3",	0x2fc00000, 0xc0000000, ",al", F_DELAYED, v6 },
{ "cba",	0x11c00000, 0xce000000, "l", F_DELAYED, v6 },
{ "cba",	0x31c00000, 0xce000000, ",al", F_DELAYED, v6 },
{ "cbn",	0x01c00000, 0xde000000, "l", F_DELAYED, v6 },
{ "cbn",	0x21c00000, 0xde000000, ",al", F_DELAYED, v6 },

{ "fdtoi",	0x81a01a40, 0x400025a0, "f,g", 0, v6 },
{ "fstoi",	0x81a01a20, 0x400025c0, "f,g", 0, v6 },

 /* all of these conversions are confused and probably wrong. */
{ "fitod",	0x81a01900, 0x400026e0, "f,g", 0, v6 },
{ "fitos",	0x81a01880, 0x40002660, "f,g", 0, v6 },

{ "fitox",	0x81a01980, 0x40002660, "f,g", 0, v6 },	/* collides in mneumonic with cypress */
 /* fitox collides in opf between cypress and v8, mneumonic between v6 and cypress */
{ "fitox",	F3F(0x2, 0x34, 0x0cc), F3F(~0x2, ~0x34, ~0x0cc)|RS1(~0), "f,g", 0, cypress }, /* collides */
{ "fitoq",	F3F(0x2, 0x34, 0x0cc), F3F(~0x2, ~0x34, ~0x0cc)|RS1(~0), "f,g", 0, v8 }, /* collides in opf with cypress */

{ "fxtoq",	0x81a019e0, 0x40002600, "f,g", 0, v9 },
{ "fxtox",	0x81a019e0, 0x40002600, "f,g", 0, v9 },

{ "fdtoq",	0x81a019c0, 0x40002620, "f,g", 0, v8 },
{ "fdtos",	0x81a018c0, 0x40002720, "f,g", 0, v6 },
{ "fqtod",	0x81a01960, 0x40002680, "f,g", 0, v8 },
{ "fqtos",	0x81a018e0, 0x40002700, "f,g", 0, v8 },
{ "fstod",	0x81a01920, 0x400026c0, "f,g", 0, v6 },
{ "fstoq",	0x81a019a0, 0x40002640, "f,g", 0, v8 },

{ "fxtos",	0x81a018e0, 0x40002700, "f,g", 0, v7 },	/* these collide on the mneumonic */
{ "fxtos",	F3F(0x2, 0x34, 0x0c5), F3F(~0x2, ~0x34, ~0x0c5)|RS1(~0), "f,g", 0, v9 },
{ "fxtos",	F3F(0x2, 0x34, 0x0c7), F3F(~0x2, ~0x34, ~0x0c7)|RS1(~0), "f,g", 0, cypress },

{ "fdtox",	F3F(0x2, 0x34, 0x0ce), F3F(~0x2, ~0x34, ~0x0ce)|RS1(~0), "f,g", 0, cypress }, /* mneumonic collisions */
{ "fdtox",	F3F(0x2, 0x34, 0x0d6), F3F(~0x2, ~0x34, ~0x0d6)|RS1(~0), "f,g", 0, v9 },

{ "fstox",	F3F(0x2, 0x34, 0x0cd), F3F(~0x2, ~0x34, ~0x0cd)|RS1(~0), "f,g", 0, cypress },
{ "fstox",	F3F(0x2, 0x34, 0x0d5), F3F(~0x2, ~0x34, ~0x0d5)|RS1(~0), "f,g", 0, v9 },

{ "fqtoi",	F3F(0x2, 0x34, 0x0d3), F3F(~0x2, ~0x34, ~0x0d3)|RS1(~0), "f,g", 0, v8 },
{ "fxtoi",	F3F(0x2, 0x34, 0x0d3), F3F(~0x2, ~0x34, ~0x0d3)|RS1(~0), "f,g", 0, cypress },

{ "fxtod",	F3F(0x2, 0x34, 0x0cb), F3F(~0x2, ~0x34, ~0x0cb)|RS1(~0), "f,g", 0, cypress }, /* collide in opf & mneumonic */
{ "fxtod",	F3F(0x2, 0x34, 0x0cb), F3F(~0x2, ~0x34, ~0x0cb)|RS1(~0), "f,g", 0, v9 },

{ "fdivd",	0x81a009c0, 0x40003620, "e,f,g", 0, v6 },
{ "fdivq",	F3F(0x2, 0x34, 0x04f), F3F(~0x2, ~0x34, ~0x04f)|RS1(~0),	"e,f,g", 0, v8 },
{ "fdivs",	0x81a009a0, 0x40003640, "e,f,g", 0, v6 },
{ "fdivx",	0x81a009e0, 0x40083600, "e,f,g", 0, cypress },
{ "fmuld",	0x81a00940, 0x400036a0, "e,f,g", 0, v6 },
{ "fmulq",	F3F(0x2, 0x34, 0x04b), F3F(~0x2, ~0x34, ~0x04b)|RS1(~0),	"e,f,g", 0, v8 },
{ "fmuls",	0x81a00920, 0x400036c0, "e,f,g", 0, v6 },
{ "fmulx",	0x81a00960, 0x40003680, "e,f,g", 0, cypress },

{ "fdmulq",	F3F(0x2, 0x34, 0x06e), F3F(~0x2, ~0x34, ~0x06e)|RS1(~0),	"e,f,g", 0, v8 },
{ "fsmuld",	F3F(0x2, 0x34, 0x069), F3F(~0x2, ~0x34, ~0x069)|RS1(~0),	"e,f,g", 0, v8 },

{ "fsqrtd",	0x81a00540, 0x40003aa8, "f,g", 0, v7 },
{ "fsqrtq",	F3F(0x2, 0x34, 0x02b), F3F(~0x2, ~0x34, ~0x02b)|RS1(~0),	"f,g", 0, v8 },
{ "fsqrts",	0x81a00520, 0x40003ac0, "f,g", 0, v7 },
{ "fsqrtx",	0x81a00560, 0x40003a80, "f,g", 0, cypress },

{ "fabsd",	F3F(0x2, 0x34, 0xa), F3F(~0x2, ~0x34, ~0xa), "f,g", 0, v9 },
{ "fabss",	F3F(0x2, 0x34, 0x9), F3F(~0x2, ~0x34, ~0x9), "f,g", 0, v6 },
{ "fmovd",	F3F(0x2, 0x34, 0x2), F3F(~0x2, ~0x34, ~0x2), "f,g", 0, v9 },
{ "fmovs",	F3F(0x2, 0x34, 0x1), F3F(~0x2, ~0x34, ~0x1), "f,g", 0, v6 },
{ "fnegd",	F3F(0x2, 0x34, 0x6), F3F(~0x2, ~0x34, ~0x6), "f,g", 0, v9 },
{ "fnegs",	F3F(0x2, 0x34, 0x5), F3F(~0x2, ~0x34, ~0x5), "f,g", 0, v6 },

{ "fmuladdd",	0xc1a02400, 0, "e,f,j,g", 0, v9 },
{ "fmuladdq",	0xc1a02800, 0, "e,f,j,g", 0, v9 },
{ "fmuladds",	0xc1a02000, 0, "e,f,j,g", 0, v9 },
{ "fmulsubs",	0xc1a03000, 0, "e,f,j,g", 0, v9 },

#ifdef comment /* v9 */
 /* FIXME: the doc says the opcodes here are identical to those for muladd. (v9) */
{ "fmulsubd",	0xc1a0xx00, 0, "e,f,j,g", 0, v9 },
{ "fmulsubq",	0xc1a0xx00, 0, "e,f,j,g", 0, v9 },
#endif /* comment v9 */

{ "cbyte",	F3(0x2, 0x2c, 0), F3(~0x2, ~0x2c, ~0),	"1,2,d", 0, v9 },
{ "cbyte",	F3(0x2, 0x2c, 1), F3(~0x2, ~0x2c, ~1),	"1,i,d", 0, v9 },
{ "cbyte",	F3(0x2, 0x2c, 1), F3(~0x2, ~0x2c, ~1),	"i,1,d", 0, v9 },

{ "popcc",	F3(0x2, 0x3f, 0), F3(~0x2, ~0x3f, ~0)|0xfff,	"1,d", 0, v9 },

{ "faddd",	0x81a00840, 0x400037a0, "e,f,g", 0, v6 },
{ "faddq",	F3F(0x2, 0x34, 0x043), F3F(~0x2, ~0x34, ~0x043)|RS1(~0), "e,f,g", 0, v8 },
{ "fadds",	0x81a00820, 0x400037c0, "e,f,g", 0, v6 },
{ "faddx",	0x81a00860, 0x40003780, "e,f,g", 0, cypress },
{ "fsubd",	0x81a008c0, 0x40003720, "e,f,g", 0, v6 },
{ "fsubq",	F3F(0x2, 0x34, 0x047), F3F(~0x2, ~0x34, ~0x047)|RS1(~0), "e,f,g", 0, v8 },
{ "fsubs",	0x81a008a0, 0x40003740, "e,f,g", 0, v6 },
{ "fsubx",	0x81a008e0, 0x40003700, "e,f,g", 0, cypress },

{ "fcmpd",	0x81a80a40, 0x400035a0, "e,f", 0, v6 },
{ "fcmped",	0x81a80ac0, 0x40003520, "e,f", 0, v6 },
{ "fcmpeq",	F3F(0x2, 0x34, 0x057), F3F(~0x2, ~0x34, ~0x057)|RS1(~0), "e,f,g", 0, v8 },
{ "fcmpes",	0x81a80aa0, 0x40003540, "e,f", 0, v6 },
{ "fcmpex",	0x81a80ae0, 0x40003500, "e,f", 0, cypress },
{ "fcmpq",	F3F(0x2, 0x34, 0x053), F3F(~0x2, ~0x34, ~0x053)|RS1(~0), "e,f,g", 0, v8 },
{ "fcmps",	0x81a80a20, 0x400035c0, "e,f", 0, v6 },
{ "fcmpx",	0x81a80a60, 0x40003580, "e,f", 0, cypress },

{ "cpop1",	0x81b00000, 0x40480000, "[1+2],d", 0, v6 },
{ "cpop2",	0x81b80000, 0x40400000, "[1+2],d", 0, v6 },

};

#define NUMOPCODES ((sizeof sparc_opcodes)/(sizeof sparc_opcodes[0]))

/*
 * Local Variables:
 * fill-column: 131
 * comment-column: 0
 * End:
 */

/* end of sparc-opcode.h */
