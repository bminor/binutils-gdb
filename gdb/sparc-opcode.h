/* Table of opcodes for the sparc.
   Copyright (C) 1989 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler, and GDB, the GNU disassembler.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#if !defined(__STDC__) && !defined(const)
#define const
#endif

/*
 * Structure of an opcode table entry.
 */
struct sparc_opcode
{
    const char *name;
    unsigned long int match;	/* Bits that must be set.  */
    unsigned long int lose;	/* Bits that must not be set.  */
    const char *args;
    char flags;
};

#define	F_DELAYED	1	/* Delayed branch */
#define	F_ALIAS		2	/* Alias for a "real" instruction */

/*
   All sparc opcodes are 32 bits, except for the `set' instruction (really
   a macro), which is 64 bits.  It is handled as a special case.

   The match component is a mask saying which bits must match a
   particular opcode in order for an instruction to be an instance
   of that opcode.

   The args component is a string containing one character
   for each operand of the instruction.

Kinds of operands:
   #    Number used by optimizer.  It is ignored.
   1    rs1 register.
   2    rs2 register.
   d    rd register.
   e    frs1 floating point register.
   f    frs2 floating point register.
   g    frsd floating point register.
   b    crs1 coprocessor register
   c    crs2 coprocessor register
   D    crsd coprocessor register
   h    22 high bits.
   i    13 bit Immediate.
   l    22 bit PC relative immediate.
   L    30 bit PC relative immediate.
   a    Annul.  The annul bit is set.
   A    Alternate address space.  Stored as 8 bits.
   C    Coprocessor state register.
   F    floating point state register.
   p    Processor state register.
   q    Floating point queue.
   r    Single register that is both rs1 and rsd.
   Q    Coprocessor queue.
   S    Special case.
   t    Trap base register.
   w    Window invalid mask register.
   y    Y register.

*/

/* The order of the opcodes in this table is significant:
   
   * The assembler requires that all instances of the same mnemonic must be
   consecutive.  If they aren't, the assembler will bomb at runtime.

   * The disassembler should not care about the order of the opcodes.  */

static struct sparc_opcode sparc_opcodes[] =
{

{ "ldd",        0xc1980000, 0x0060201f, "[1],D", 0 }, /* ldd [rs1+%g0],d */
{ "ldd",        0xc1982000, 0x00601fff, "[1],D", 0 }, /* ldd [rs1+0],d */
{ "ldd",        0xc1982000, 0x00600000, "[1+i],D", 0 },
{ "ldd",        0xc1982000, 0x00600000, "[i+1],D", 0 },
{ "ldd",        0xc1980000, 0x00602000, "[1+2],D", 0 },
{ "ldd",        0xc1180000, 0x00e0201f, "[1],g", 0 }, /* ldd [rs1+%g0],d */
{ "ldd",        0xc1182000, 0x00e01fff, "[1],g", 0 }, /* ldd [rs1+0],d */
{ "ldd",        0xc1182000, 0x00e00000, "[1+i],g", 0 },
{ "ldd",        0xc1182000, 0x00e00000, "[i+1],g", 0 },
{ "ldd",        0xc1180000, 0x00e02000, "[1+2],g", 0 },
{ "ldd",	0xc0180000, 0x01e0201f, "[1],d", 0 }, /* ldd [rs1+%g0],d */
{ "ldd",        0xc0182000, 0x01e01fff, "[1],d", 0 }, /* ldd [rs1+0],d */
{ "ldd",	0xc0182000, 0x01e00000, "[1+i],d", 0 },
{ "ldd",	0xc0182000, 0x01e00000, "[i+1],d", 0 },
{ "ldd",	0xc0180000, 0x01e02000, "[1+2],d", 0 },
{ "ld",         0xc1880000, 0x0070201f, "[1],C", 0 }, /* ld [rs1+%g0],d */
{ "ld",         0xc1882000, 0x00701fff, "[1],C", 0 }, /* ld [rs1+0],d */
{ "ld",         0xc1882000, 0x00700000, "[1+i],C", 0 },
{ "ld",         0xc1882000, 0x00700000, "[i+1],C", 0 },
{ "ld",         0xc1880000, 0x00702000, "[1+2],C", 0 },
{ "ld",         0xc1800000, 0x0078201f, "[1],D", 0 }, /* ld [rs1+%g0],d */
{ "ld",         0xc1802000, 0x00781fff, "[1],D", 0 }, /* ld [rs1+0],d */
{ "ld",         0xc1802000, 0x00780000, "[1+i],D", 0 },
{ "ld",         0xc1802000, 0x00780000, "[i+1],D", 0 },
{ "ld",         0xc1800000, 0x00782000, "[1+2],D", 0 },
{ "ld",         0xc1080000, 0x00f0201f, "[1],F", 0 }, /* ld [rs1+%g0],d */
{ "ld",         0xc1082000, 0x00f01fff, "[1],F", 0 }, /* ld [rs1+0],d */
{ "ld",         0xc1082000, 0x00f00000, "[1+i],F", 0 },
{ "ld",         0xc1082000, 0x00f00000, "[i+1],F", 0 },
{ "ld",         0xc1080000, 0x00f02000, "[1+2],F", 0 },
{ "ld",         0xc1000000, 0x00f8201f, "[1],g", 0 }, /* ld [rs1+%g0],d */
{ "ld",         0xc1002000, 0x00f81fff, "[1],g", 0 }, /* ld [rs1+0],d */
{ "ld",         0xc1002000, 0x00f80000, "[1+i],g", 0 },
{ "ld",         0xc1002000, 0x00f80000, "[i+1],g", 0 },
{ "ld",         0xc1000000, 0x00f82000, "[1+2],g", 0 },
{ "ld",	        0xc0000000, 0x01f8201f, "[1],d", 0 }, /* ld [rs1+%g0],d */
{ "ld",		0xc0002000, 0x01f81fff, "[1],d", 0 }, /* ld [rs1+0],d */
{ "ld",		0xc0002000, 0x01f80000, "[1+i],d", 0 },
{ "ld",		0xc0002000, 0x01f80000, "[i+1],d", 0 },
{ "ld",		0xc0000000, 0x01f82000, "[1+2],d", 0 },
{ "ldstuba",    0xc0d80000, 0x0100201f, "[1]A,d", 0 }, /* ldstuba [rs1+%g0],d */
{ "ldstuba",	0xc0d80000, 0x01002000, "[1+2]A,d", 0 },
{ "ldsha",      0xc0d00000, 0x0128201f, "[1]A,d", 0 }, /* ldsha [rs1+%g0],d */
{ "ldsha",	0xc0d00000, 0x01282000, "[1+2]A,d", 0 },
{ "ldsba",      0xc0c80000, 0x0130201f, "[1]A,d", 0 }, /* ldsba [rs1+%g0],d */
{ "ldsba",	0xc0c80000, 0x01302000, "[1+2]A,d", 0 },
{ "ldda",       0xc0980000, 0x0160201f, "[1]A,d", 0 }, /* ldda [rs1+%g0],d */
{ "ldda",	0xc0980000, 0x01602000, "[1+2]A,d", 0 },
{ "lduha",      0xc0900000, 0x0168201f, "[1]A,d", 0 }, /* lduha [rs1+%g0],d */
{ "lduha",	0xc0900000, 0x01682000, "[1+2]A,d", 0 },
{ "ldstub",     0xc0680000, 0x0190201f, "[1],d", 0 }, /* ldstub [rs1+%g0],d */
{ "ldstub",	0xc0682000, 0x01900000, "[1+i],d", 0 },
{ "ldstub",	0xc0682000, 0x01900000, "[i+1],d", 0 },
{ "ldstub",	0xc0680000, 0x01902000, "[1+2],d", 0 },
{ "lda",        0xc0800000, 0x0178201f, "[1]A,d", 0 }, /* lda [rs1+%g0],d */
{ "lda",	0xc0800000, 0x01782000, "[1+2]A,d", 0 },
{ "ldsh",       0xc0500000, 0x0000000d, "[1],d", 0 }, /* ldsh [rs1+%g0],d */
{ "ldsh",       0xc0502000, 0x01a81fff, "[1],d", 0 }, /* ldsh [rs1+0],d */
{ "ldsh",	0xc0502000, 0x01a80000, "[1+i],d", 0 },
{ "ldsh",	0xc0502000, 0x01a80000, "[i+1],d", 0 },
{ "ldsh",	0xc0500000, 0x01a82000, "[1+2],d", 0 },
{ "ldsb",       0xc0480000, 0x01b0201f, "[1],d", 0 }, /* ldsb [rs1+%g0],d */
{ "ldsb",	0xc0482000, 0x01b01fff, "[1],d", 0 }, /* ldsb [rs1+0],d */
{ "ldsb",	0xc0482000, 0x01b00000, "[1+i],d", 0 },
{ "ldsb",	0xc0482000, 0x01b00000, "[i+1],d", 0 },
{ "ldsb",	0xc0480000, 0x01b02000, "[1+2],d", 0 },
{ "ldub",       0xc0080000, 0x01f0201f, "[1],d", 0 }, /* ldub [rs1+%g0],d */
{ "ldub",       0xc0082000, 0x01f01fff, "[1],d", 0 }, /* ldub [rs1+0],d */
{ "ldub",	0xc0082000, 0x01f00000, "[1+i],d", 0 },
{ "ldub",	0xc0082000, 0x01f00000, "[i+1],d", 0 },
{ "ldub",	0xc0080000, 0x01f02000, "[1+2],d", 0 },
{ "lduba",      0xc0880000, 0x0170201f, "[1]A,d", 0 }, /* lduba [rs1+%g0],d */
{ "lduba",	0xc0880000, 0x01702000, "[1+2]A,d", 0 },
{ "lduh",	0xc0102000, 0x01e80000, "[1+i],d", 0 },
{ "lduh",	0xc0102000, 0x01e80000, "[i+1],d", 0 },
{ "lduh",	0xc0100000, 0x01e8201f, "[1],d", 0 }, /* lduh [rs1+%g0],d */
{ "lduh",	0xc0102000, 0x01e81fff, "[1],d", 0 }, /* lduh [rs1+0],d */
{ "lduh",	0xc0100000, 0x01e82000, "[1+2],d", 0 },

{ "st",	        0xc0200000, 0x01d8201f, "d,[1]", 0 }, /* st d,[rs1+%g0] */
{ "st",	        0xc0202000, 0x01d81fff, "d,[1]", 0 }, /* st d,[rs1+0] */
{ "st",		0xc0202000, 0x01d80000, "d,[1+i]", 0 },
{ "st",		0xc0202000, 0x01d80000, "d,[i+1]", 0 },
{ "st",		0xc0200000, 0x01d82000, "d,[1+2]", 0 },
{ "st",		0xc1200000, 0x00d8201f, "g,[1]", 0 }, /* st d[rs1+%g0] */
{ "st",		0xc1202000, 0x00d81fff, "g,[1]", 0 }, /* st d,[rs1+0] */
{ "st",		0xc1202000, 0x00d80000, "g,[1+i]", 0 },
{ "st",		0xc1202000, 0x00d80000, "g,[i+1]", 0 },
{ "st",		0xc1200000, 0x00d82000, "g,[1+2]", 0 },
{ "st",		0xc1280000, 0x00d0201f, "F,[1]", 0 }, /* st %fsr,[rs1+%g0] */
{ "st",		0xc1282000, 0x00d01fff, "F,[1]", 0 }, /* st %fsr,[rs1+0] */
{ "st",		0xc1282000, 0x00d00000, "F,[1+i]", 0 },
{ "st",		0xc1282000, 0x00d00000, "F,[i+1]", 0 },
{ "st",		0xc1280000, 0x00d02000, "F,[1+2]", 0 },
{ "st",		0xc1a00000, 0x0058201f, "D,[1]", 0 }, /* st d,[rs1+%g0] */
{ "st",		0xc1a02000, 0x00581fff, "D,[1]", 0 }, /* st d,[rs1+0] */
{ "st",		0xc1a02000, 0x00580000, "D,[1+i]", 0 },
{ "st",		0xc1a02000, 0x00580000, "D,[i+1]", 0 },
{ "st",		0xc1a00000, 0x00582000, "D,[1+2]", 0 },
{ "st",		0xc1a80000, 0x0050201f, "C,[1]", 0 }, /* st d,[rs1+%g0] */
{ "st",		0xc1a82000, 0x00501fff, "C,[1]", 0 }, /* st d,[rs1+0] */
{ "st",		0xc1a82000, 0x00500000, "C,[1+i]", 0 },
{ "st",		0xc1a82000, 0x00500000, "C,[i+1]", 0 },
{ "st",		0xc1a80000, 0x00502000, "C,[1+2]", 0 },
{ "sta",        0xc0a00000, 0x0108201f, "d,[1]A", 0 }, /* sta d,[rs1+%g0] */
{ "sta",	0xc0a00000, 0x01082000, "d,[1+2]A", 0 },

{ "stb",        0xc0280000, 0x01d0201f, "d,[1]", 0 }, /* stb d,[rs1+%g0] */
{ "stb",	0xc0282000, 0x01d01fff, "d,[1]", 0 }, /* stb d,[rs1+0] */
{ "stb",	0xc0282000, 0x01d00000, "d,[1+i]", 0 },
{ "stb",	0xc0282000, 0x01d00000, "d,[i+1]", 0 },
{ "stb",	0xc0280000, 0x01d02000, "d,[1+2]", 0 },
{ "stba",       0xc0a80000, 0x01002000, "d,[1+2]A", 0 },
{ "stba",	0xc0a80000, 0x0100201f, "d,[1]A", 0 }, /* stba d,[rs1+%g0] */

{ "std",        0xc0380000, 0x01c0201f, "d,[1]", 0 }, /* std d,[rs1+%g0] */
{ "std",	0xc0382000, 0x01c01fff, "d,[1]", 0 }, /* std d,[rs1+0] */
{ "std",	0xc0382000, 0x01c00000, "d,[1+i]", 0 },
{ "std",	0xc0382000, 0x01c00000, "d,[i+1]", 0 },
{ "std",	0xc0380000, 0x01c02000, "d,[1+2]", 0 },
{ "std",	0xc1380000, 0x00c0201f, "g,[1]", 0 }, /* std d,[rs1+%g0] */
{ "std",        0xc1382000, 0x00c01fff, "g,[1]", 0 }, /* std d,[rs1+0] */
{ "std",	0xc1382000, 0x00c00000, "g,[1+i]", 0 },
{ "std",	0xc1382000, 0x00c00000, "g,[i+1]", 0 },
{ "std",	0xc1380000, 0x00c02000, "g,[1+2]", 0 },
{ "std",        0xc1300000, 0x00c8201f, "q,[1]", 0 }, /* std d,[rs1+%g0] */
{ "std",	0xc1302000, 0x00c81fff, "q,[1]", 0 }, /* std d,[rs1+0] */
{ "std",	0xc1302000, 0x00c80000, "q,[1+i]", 0 },
{ "std",	0xc1302000, 0x00c80000, "q,[i+1]", 0 },
{ "std",	0xc1300000, 0x00c82000, "q,[1+2]", 0 },
{ "std",	0xc1b80000, 0x0040201f, "D,[1]", 0 }, /* std d,[rs1+%g0] */
{ "std",	0xc1b82000, 0x00401fff, "D,[1]", 0 }, /* std d,[rs1+0] */
{ "std",	0xc1b82000, 0x00400000, "D,[1+i]", 0 },
{ "std",	0xc1b82000, 0x00400000, "D,[i+1]", 0 },
{ "std",	0xc1b80000, 0x00402000, "D,[1+2]", 0 },
{ "std",	0xc1b00000, 0x0048201f, "Q,[1]", 0 }, /* std d,[rs1+%g0] */
{ "std",	0xc1b02000, 0x00481fff, "Q,[1]", 0 }, /* std d,[rs1+0] */
{ "std",	0xc1b02000, 0x00480000, "Q,[1+i]", 0 },
{ "std",	0xc1b02000, 0x00480000, "Q,[i+1]", 0 },
{ "std",	0xc1b00000, 0x00482000, "Q,[1+2]", 0 },
{ "stda",       0xc0b80000, 0x01402000, "d,[1+2]A", 0 },
{ "stda",	0xc0b80000, 0x0140201f, "d,[1]A", 0 }, /* stda d,[rs1+%g0] */

{ "sth",        0xc0300000, 0x01c8201f, "d,[1]", 0 }, /* sth d,[rs1+%g0] */
{ "sth",	0xc0302000, 0x01c81fff, "d,[1]", 0 }, /* sth d,[rs1+0] */
{ "sth",	0xc0300000, 0x01c82000, "d,[1+2]", 0 },
{ "sth",	0xc0302000, 0x01c80000, "d,[1+i]", 0 },
{ "sth",	0xc0302000, 0x01c80000, "d,[i+1]", 0 },
{ "stha",       0xc0b00000, 0x0148201f, "d,[1]A", 0 }, /* stha d,[rs1+%g0] */
{ "stha",	0xc0b00000, 0x01482000, "d,[1+2]A", 0 },

{ "swap",       0xc0780000, 0x0180201f, "[1],d", 0 }, /* swap [rs1+%g0],d */
{ "swap",       0xc0782000, 0x01801fff, "[1],d", 0 }, /* swap [rs1+0],d */
{ "swap",       0xc0782000, 0x01800000, "[1+i],d", 0 },
{ "swap",       0xc0782000, 0x01800000, "[i+1],d", 0 },
{ "swap",       0xc0780000, 0x01802000, "[1+2],d", 0 },
{ "swapa",      0xc0f80000, 0x01002000, "[1+2]A,d", 0 },
{ "swapa",      0xc0f80000, 0x0100201f, "[1]A,d", 0 }, /* swapa [rs1+%g0],d */

{ "restore",    0x81e80000, 0x7e17e01f, "", 0 }, /* restore %g0,%g0,%g0 */
{ "restore",    0x81e82000, 0x7e14dfff, "", 0 }, /* restore %g0,0,%g0 */
{ "restore",	0x81e82000, 0x00000000, "1,i,d", 0 },
{ "restore",	0x81e80000, 0x00000000, "1,2,d", 0 },
{ "rett",       0x81c82000, 0x40300000, "1+i", F_DELAYED },
{ "rett",       0x81c82000, 0x40300000, "i+1", F_DELAYED },
{ "rett",	0x81c80000, 0x40302000, "1+2", F_DELAYED },
{ "rett",	0x81c82000, 0x40301fff, "1", F_DELAYED}, /* rett X,0 */
{ "rett",	0x81c80000, 0x4030201f, "1", F_DELAYED}, /* rett X,%g0 */
{ "save",       0x81e02000, 0x40180000, "1,i,d", 0 },
{ "save",	0x81e00000, 0x40180000, "1,2,d", 0 },

{ "ret",	0x81c7e008, 0x00001ff7, "", F_DELAYED }, /* jmpl %i7+8,%g0 */
{ "retl",       0x81c3e008, 0x00001ff7, "", F_DELAYED }, /* jmpl %o7+8,%g0 */

{ "jmpl",       0x81c00000, 0x4038201f, "1,d", F_DELAYED }, /* jmpl rs1+%g0,d */
{ "jmpl",	0x81c02000, 0x4037c000, "i,d", F_DELAYED }, /* jmpl %g0+i,d */
{ "jmpl",	0x81c02000, 0x40380000, "1+i,d", F_DELAYED },
{ "jmpl",	0x81c02000, 0x40380000, "i+1,d", F_DELAYED },
{ "jmpl",	0x81c00000, 0x40382000, "1+2,d", F_DELAYED },
{ "wr",         0x81982000, 0x40600000, "1,i,t", 0 },
{ "wr",         0x81980000, 0x40600000, "1,2,t", 0 },
{ "wr",         0x81902000, 0x40680000, "1,i,w", 0 },
{ "wr",         0x81900000, 0x40680000, "1,2,w", 0 },
{ "wr",         0x81882000, 0x40700000, "1,i,p", 0 },
{ "wr",         0x81880000, 0x40700000, "1,2,p", 0 },
{ "wr",         0x81802000, 0x40780000, "1,i,y", 0 },
{ "wr",         0x81800000, 0x40780000, "1,2,y", 0 },

{ "rd", 	0x81580000, 0x40a00000, "t,d", 0 },
{ "rd", 	0x81500000, 0x40a80000, "w,d", 0 },
{ "rd", 	0x81480000, 0x40b00000, "p,d", 0 },
{ "rd",         0x81400000, 0x40b80000, "y,d", 0 },

{ "sra",	0x81382000, 0x00000000, "1,i,d", 0 },
{ "sra",	0x81380000, 0x00000000, "1,2,d", 0 },
{ "srl",        0x81302000, 0x40c80000, "1,i,d", 0 },
{ "srl",	0x81300000, 0x40c80000, "1,2,d", 0 },
{ "sll",        0x81282000, 0x40d00000, "1,i,d", 0 },
{ "sll",	0x81280000, 0x40d00000, "1,2,d", 0 },

{ "mulscc",     0x81202000, 0x40d80000, "1,i,d", 0 },
{ "mulscc",	0x81200000, 0x40d80000, "1,2,d", 0 },

{ "clr",        0x80100000, 0x4e87e01f, "d", F_ALIAS }, /* or %g0,%g0,d */
{ "clr",        0x80102000, 0x41efdfff, "d", F_ALIAS }, /* or %g0,0,d   */
{ "clr",	0xc0200000, 0x3fd8001f, "[1]", F_ALIAS }, /* st %g0,[rs1+%g0] */
{ "clr",	0xc0202000, 0x3fd81fff, "[1]", F_ALIAS }, /* st %g0,[rs1+0] */
{ "clr",	0xc0202000, 0x3fd80000, "[1+i]", F_ALIAS },
{ "clr",	0xc0202000, 0x3fd80000, "[i+1]", F_ALIAS },
{ "clr",	0xc0200000, 0x3fd80000, "[1+2]", F_ALIAS },

{ "clrb",       0xc0280000, 0x3fd0001f, "[1]", F_ALIAS },/* stb %g0,[rs1+%g0] */
{ "clrb",	0xc0282000, 0x3fd00000, "[1+i]", F_ALIAS },
{ "clrb",	0xc0282000, 0x3fd00000, "[i+1]", F_ALIAS },
{ "clrb",	0xc0280000, 0x3fd00000, "[1+2]", F_ALIAS },

{ "clrh",       0xc0300000, 0x3fc8001f, "[1]", F_ALIAS },/* sth %g0,[rs1+%g0] */
{ "clrh",	0xc0300000, 0x3fc80000, "[1+2]", F_ALIAS },
{ "clrh",	0xc0302000, 0x3fc80000, "[1+i]", F_ALIAS },
{ "clrh",	0xc0302000, 0x3fc80000, "[i+1]", F_ALIAS },

{ "orncc",      0x80b02000, 0x04048000, "1,i,d", 0 },
{ "orncc",	0x80b00000, 0x04048000, "1,2,d", 0 },

{ "tst",        0x80900000, 0x7f6fe000, "2", 0 }, /* orcc %g0, rs2, %g0 */
{ "tst",        0x80900000, 0x7f68201f, "1", 0 }, /* orcc rs1, %g0, %g0 */
{ "tst",        0x80902000, 0x7f681fff, "1", 0 }, /* orcc rs1, 0, %g0 */
  
{ "orcc",       0x80902000, 0x41680000, "1,i,d", 0 },
{ "orcc",	0x80902000, 0x41680000, "i,1,d", 0 },
{ "orcc",	0x80900000, 0x41680000, "1,2,d", 0 },
{ "orn",        0x80302000, 0x41c80000, "1,i,d", 0 },
{ "orn",	0x80300000, 0x41c80000, "1,2,d", 0 },

{ "mov",        0x81800000, 0x4078201f, "1,y", F_ALIAS }, /* wr rs1,%g0,%y */
{ "mov",        0x81802000, 0x40781fff, "1,y", F_ALIAS }, /* wr rs1,0,%y */
{ "mov",        0x81802000, 0x40780000, "i,y", F_ALIAS },
{ "mov",        0x81400000, 0x40b80000, "y,d", F_ALIAS }, /* rd %y,d */
{ "mov",        0x81980000, 0x4060201f, "1,t", F_ALIAS }, /* wr rs1,%g0,%tbr */
{ "mov",        0x81982000, 0x40601fff, "1,t", F_ALIAS }, /* wr rs1,0,%tbr */
{ "mov",        0x81982000, 0x40600000, "i,t", F_ALIAS },
{ "mov",        0x81580000, 0x40a00000, "t,d", F_ALIAS }, /* rd %tbr,d */
{ "mov",        0x81900000, 0x4068201f, "1,w", F_ALIAS }, /* wr rs1,%g0,%wim */
{ "mov",        0x81902000, 0x40681fff, "1,w", F_ALIAS }, /* wr rs1,0,%wim */
{ "mov",        0x81902000, 0x40680000, "i,w", F_ALIAS },
{ "mov",        0x81500000, 0x40a80000, "w,d", F_ALIAS }, /* rd %wim,d */
{ "mov",        0x81880000, 0x4070201f, "1,p", F_ALIAS }, /* wr rs1,%g0,%psr */
{ "mov",        0x81882000, 0x40701fff, "1,p", F_ALIAS }, /* wr rs1,0,%psr */
{ "mov",        0x81882000, 0x40700000, "i,p", F_ALIAS },
{ "mov",        0x81480000, 0x40b00000, "p,d", F_ALIAS }, /* rd %psr,d */

{ "mov",        0x80102000, 0x41efc000, "i,d", 0 }, /* or %g0,i,d   */
{ "mov",        0x80100000, 0x41efe000, "2,d", 0 }, /* or %g0,rs2,d */
{ "mov",        0x80102000, 0x41e81fff, "1,d", 0 }, /* or rs1,0,d */
{ "mov",        0x80100000, 0x41e8201f, "1,d", 0 }, /* or rs1,%g0,d   */

{ "or",	        0x80102000, 0x40800000, "1,i,d", 0 },
{ "or",		0x80102000, 0x40800000, "i,1,d", 0 },
{ "or",		0x80100000, 0x40800000, "1,2,d", 0 },

{ "bset",	0x80102000, 0x40800000, "i,r", F_ALIAS },/* or rd,i,rd */
{ "bset",	0x80100000, 0x40800000, "2,r", F_ALIAS },/* or rd,rs2,rd */

{ "andncc",     0x80a82000, 0x41500000, "1,i,d", 0 },
{ "andncc",	0x80a80000, 0x41500000, "1,2,d", 0 },
{ "andn",       0x80282000, 0x41d00000, "1,i,d", 0 },
{ "andn",	0x80280000, 0x41d00000, "1,2,d", 0 },

{ "bclr",	0x80282000, 0x41d00000, "i,r", F_ALIAS },/* andn rd,i,rd */
{ "bclr",	0x80280000, 0x41d00000, "2,r", F_ALIAS },/* andn rd,rs2,rd */

{ "cmp",        0x80a02000, 0x7d580000, "1,i", 0 },     /* subcc rs1,i,%g0 */
{ "cmp",	0x80a00000, 0x7d580000, "1,2", 0 },     /* subcc rs1,rs2,%g0 */

{ "subcc",      0x80a02000, 0x41580000, "1,i,d", 0 },
{ "subcc",	0x80a00000, 0x41580000, "1,2,d", 0 },
{ "sub",	0x80202000, 0x41d80000, "1,i,d", 0 },
{ "sub",	0x80200000, 0x41d80000, "1,2,d", 0 },
{ "subx",	0x80602000, 0x41980000, "1,i,d", 0 },
{ "subx",	0x80600000, 0x41980000, "1,2,d", 0 },
{ "subxcc",     0x80e02000, 0x41180000, "1,i,d", 0 },
{ "subxcc",	0x80e00000, 0x41180000, "1,2,d", 0 },

{ "andcc",      0x80882000, 0x41700000, "1,i,d", 0 },
{ "andcc",	0x80882000, 0x41700000, "i,1,d", 0 },
{ "andcc",	0x80880000, 0x41700000, "1,2,d", 0 },
{ "and",        0x80082000, 0x41f00000, "1,i,d", 0 },
{ "and",	0x80082000, 0x41f00000, "i,1,d", 0 },
{ "and",	0x80080000, 0x41f00000, "1,2,d", 0 },

{ "inc",	0x80002001, 0x41f81ffe, "r", F_ALIAS },	/* add rs1,1,rsd */
{ "inccc",	0x80802001, 0x41781ffe, "r", F_ALIAS },	/* addcc rd,1,rd */
{ "dec",        0x80202001, 0x41d81ffe, "r", F_ALIAS },	/* sub rd,1,rd */
{ "deccc",	0x80a02001, 0x41581ffe, "r", F_ALIAS },	/* subcc rd,1,rd */

{ "btst",	0x80882000, 0x41700000, "i,1", F_ALIAS },/* andcc rs1,i,%g0 */
{ "btst",	0x80880000, 0x41700000, "1,2", F_ALIAS },/* andcc rs1,rs2,%0 */

{ "neg",	0x80200000, 0x41d80000, "r", F_ALIAS }, /* sub %0,rd,rd */
{ "neg",	0x80200000, 0x41d80000, "2,d", F_ALIAS }, /* sub %0,rs2,rd */

{ "addxcc",     0x80c02000, 0x41380000, "1,i,d", 0 },
{ "addxcc",     0x80c02000, 0x41380000, "i,1,d", 0 },
{ "addxcc",     0x80c00000, 0x41380000, "1,2,d", 0 },
{ "addcc",      0x80802000, 0x41780000, "1,i,d", 0 },
{ "addcc",      0x80802000, 0x41780000, "i,1,d", 0 },
{ "addcc",      0x80800000, 0x41780000, "1,2,d", 0 },
{ "addx",       0x80402000, 0x41b80000, "1,i,d", 0 },
{ "addx",	0x80402000, 0x41b80000, "i,1,d", 0 },
{ "addx",	0x80400000, 0x41b80000, "1,2,d", 0 },
{ "add",        0x80002000, 0x41f80000, "1,i,d", 0 },
{ "add",	0x80002000, 0x41f80000, "i,1,d", 0 },
{ "add",	0x80000000, 0x41f80000, "1,2,d", 0 },

{ "call",       0x9fc00000, 0x4038201f, "1", F_DELAYED }, /* jmpl rs1+%g0, %o7 */
{ "call",	0x9fc00000, 0x4038201f, "1,#", F_DELAYED },
{ "call",	0x40000000, 0x80000000, "L", F_DELAYED },
{ "call",	0x40000000, 0x80000000, "L,#", F_DELAYED },

{ "jmp",        0x81c00000, 0x7e38201f, "1", F_DELAYED }, /* jmpl rs1+%g0,%g0 */
{ "jmp",        0x81c02000, 0x7e3fc000, "i", F_DELAYED }, /* jmpl %g0+i,%g0 */
{ "jmp",        0x81c00000, 0x7e382000, "1+2", F_DELAYED }, /* jmpl rs1+rs2,%g0 */
{ "jmp",        0x81c02000, 0x7e380000, "1+i", F_DELAYED }, /* jmpl rs1+i,%g0 */
{ "jmp",        0x81c02000, 0x7e380000, "i+1", F_DELAYED }, /* jmpl i+rs1,%g0 */

{ "nop",	0x01000000, 0xfeffffff, "", 0 }, /* sethi 0, %g0 */

{ "set",        0x01000000, 0xc0c00000, "Sh,d", F_ALIAS },

{ "sethi",      0x01000000, 0xc0c00000, "h,d", 0 },

{ "taddcctv",   0x81102000, 0x40e00000, "1,i,d", 0 },
{ "taddcctv",   0x81100000, 0x40e00000, "1,2,d", 0 },
{ "taddcc",     0x81002000, 0x40f80000, "1,i,d", 0 },
{ "taddcc",     0x81000000, 0x40f80000, "1,2,d", 0 },

/* Conditional instructions.

   Because this part of the table was such a mess earlier, I have
   macrofied it so that all the branches and traps are generated from
   a single-line description of each condition value.  */

#define ANNUL  0x20000000
#define	IMMED  0x00002000
#define	RS1_G0 0x0007C000
#define	RS2_G0 0x0000001F

/* Define two branches -- one annulled, one without */
#define br(opcode, mask, lose, flags)	\
 { opcode, mask+ANNUL, lose, ",al", flags },	\
 { opcode, mask      , lose, "l",   flags }

/* Define four traps: reg+reg, reg + immediate, immediate alone, reg alone. */
#define tr(opcode, mask, lose, flags) \
 {opcode, mask+IMMED, lose+RS1_G0      , "i", flags },	 /* %g0 + imm */ \
 {opcode, mask+IMMED, lose             , "1+i", flags }, /* rs1 + imm */ \
 {opcode, mask      , lose+IMMED       , "1+2", flags }, /* rs1 + rs2 */ \
 {opcode, mask      , lose+IMMED+RS2_G0, "1", flags }    /* rs1 + %g0 */

/* Define both branches and traps based on condition mask */
#ifdef __STDC__
#define cond(bop, top, mask, flags)	\
  br(#bop,  0x00800000+(mask << 25), 0xC1400000, F_DELAYED|flags), \
  tr(#top,  0x81d00000+(mask << 25), 0x40280000, flags)
#else
#define cond(bop, top, mask, flags)	\
  br("bop", 0x00800000+(mask << 25), 0xC1400000, F_DELAYED|flags), \
  tr("top", 0x81d00000+(mask << 25), 0x40280000, flags)
#endif

/* Define all the conditions, all the branches, all the traps.  */
/* Use no extra spaces or tabs around the first two args, since cpp
   will include them in the printed strings.  */
cond (bvc,tvc,   0xF, 0),
cond (bvs,tvs,   0x7, 0),
cond (bpos,tpos, 0xE, 0),
cond (bneg,tneg, 0x6, 0),
cond (bcc,tcc,   0xD, 0),
cond (bcs,tcs,   0x5, 0),
cond (blu,tlu,   0x5, F_ALIAS), /* for cs */
cond (bgeu,tgeu, 0xD, F_ALIAS), /* for cc */
cond (bgu,tgu,   0xC, 0),
cond (bleu,tleu, 0x4, 0),
cond (bge,tge,   0xB, 0),
cond (bl,tl,     0x3, 0),
cond (bg,tg,     0xA, 0),
cond (ble,tle,   0x2, 0),
cond (be,te,     0x1, 0),
cond (bz,tz,     0x1, F_ALIAS), /* for e */
cond (bne,tne,   0x9, 0),
cond (bnz,tnz,   0x9, F_ALIAS), /* for ne */
cond (b,t,       0x8, 0),
cond (ba,ta,     0x8, F_ALIAS), /* for nothing */
cond (bn,tn,     0x0, 0),

#undef cond
#undef br
#undef tr

{ "tsubcc",     0x81080000, 0x40f00000, "1,2,d", 0 },
{ "tsubcc",     0x81082000, 0x40f00000, "1,i,d", 0 },
{ "tsubcctv",   0x80580000, 0x40a00000, "1,2,d", 0 },
{ "tsubcctv",   0x80582000, 0x40a00000, "1,i,d", 0 },

{ "unimp",      0x00000000, 0xFFFFFFFF, "l", 0 },

{ "iflush",     0x81d80000, 0x40202000, "1+2", 0 },
{ "iflush",     0x81d82000, 0x40200000, "1+i", 0 },

{ "xnorcc",     0x80b80000, 0x41400000, "1,2,d", 0 },
{ "xnorcc",	0x80b82000, 0x41400000, "1,i,d", 0 },
{ "xnorcc",	0x80b82000, 0x41400000, "i,1,d", 0 },
{ "xorcc",      0x80980000, 0x41600000, "1,2,d", 0 },
{ "xorcc",	0x80982000, 0x41600000, "1,i,d", 0 },
{ "xorcc",	0x80982000, 0x41600000, "i,1,d", 0 },
{ "xnor",       0x80380000, 0x41c00000, "1,2,d", 0 },
{ "xnor",	0x80382000, 0x41c00000, "1,i,d", 0 },
{ "xnor",	0x80382000, 0x41c00000, "i,1,d", 0 },
{ "xor",        0x80180000, 0x41e00000, "1,2,d", 0 },
{ "xor",	0x80182000, 0x41e00000, "1,i,d", 0 },
{ "xor",	0x80182000, 0x41e00000, "i,1,d", 0 },

{ "not",        0x80380000, 0x41c00000, "r", F_ALIAS }, /* xnor rd,%0,rd */
{ "not",        0x80380000, 0x41c00000, "1,d", F_ALIAS }, /* xnor rs1,%0,rd */

{ "btog",	0x80180000, 0x41e02000, "2,r", F_ALIAS }, /* xor rd,rs2,rd */
{ "btog",	0x80182000, 0x41e00000, "i,r", F_ALIAS }, /* xor rd,i,rd */

{ "fpop1",      0x81a00000, 0x40580000, "[1+2],d", 0 },
{ "fpop2",      0x81a80000, 0x40500000, "[1+2],d", 0 },

/* Someday somebody should give these the same treatment as the branches
   above.  FIXME someday.  */

{ "fb",         0x31800000, 0xc0400000, ",al", F_DELAYED },
{ "fb",         0x11800000, 0xc0400000, "l", F_DELAYED },
{ "fba",        0x31800000, 0xc0400000, ",al", F_DELAYED|F_ALIAS },
{ "fba",        0x11800000, 0xc0400000, "l", F_DELAYED|F_ALIAS },
{ "fbn",        0x21800000, 0xc0400000, ",al", F_DELAYED },
{ "fbn",        0x01800000, 0xc0400000, "l", F_DELAYED },
{ "fbu",        0x2f800000, 0xc0400000, ",al", F_DELAYED },
{ "fbu",        0x0f800000, 0xc0400000, "l", F_DELAYED },
{ "fbg",        0x2d800000, 0xc0400000, ",al", F_DELAYED },
{ "fbg",        0x0d800000, 0xc0400000, "l", F_DELAYED },
{ "fbug",       0x2b800000, 0xc0400000, ",al", F_DELAYED },
{ "fbug",       0x0b800000, 0xc0400000, "l", F_DELAYED },
{ "fbl",        0x29800000, 0xc0400000, ",al", F_DELAYED },
{ "fbl",        0x09800000, 0xc0400000, "l", F_DELAYED },
{ "fbul",       0x27800000, 0xc0400000, ",al", F_DELAYED },
{ "fbul",       0x07800000, 0xc0400000, "l", F_DELAYED },
{ "fblg",       0x25800000, 0xc0400000, ",al", F_DELAYED },
{ "fblg",       0x05800000, 0xc0400000, "l", F_DELAYED },
{ "fbne",       0x23800000, 0xc0400000, ",al", F_DELAYED },
{ "fbne",       0x03800000, 0xc0400000, "l", F_DELAYED },
{ "fbe",        0x33800000, 0xc0400000, ",al", F_DELAYED },
{ "fbe",        0x13800000, 0xc0400000, "l", F_DELAYED },
{ "fbue",       0x35800000, 0xc0400000, ",al", F_DELAYED },
{ "fbue",       0x15800000, 0xc0400000, "l", F_DELAYED },
{ "fbge",       0x37800000, 0xc0400000, ",al", F_DELAYED },
{ "fbge",       0x17800000, 0xc0400000, "l", F_DELAYED },
{ "fbuge",      0x39800000, 0xc0400000, ",al", F_DELAYED },
{ "fbuge",      0x19800000, 0xc0400000, "l", F_DELAYED },
{ "fble",       0x3b800000, 0xc0400000, ",al", F_DELAYED },
{ "fble",       0x1b800000, 0xc0400000, "l", F_DELAYED },
{ "fbule",      0x3d800000, 0xc0400000, ",al", F_DELAYED },
{ "fbule",      0x1d800000, 0xc0400000, "l", F_DELAYED },
{ "fbo",        0x3f800000, 0xc0400000, ",al", F_DELAYED },
{ "fbo",        0x1f800000, 0xc0400000, "l", F_DELAYED },

{ "cba",        0x31c00000, 0xce000000, ",al", F_DELAYED },
{ "cba",        0x11c00000, 0xce000000, "l", F_DELAYED },
{ "cbn",        0x21c00000, 0xde000000, ",al", F_DELAYED },
{ "cbn",        0x01c00000, 0xde000000, "l", F_DELAYED },
{ "cb3",        0x2fc00000, 0xc0000000, ",al", F_DELAYED },
{ "cb3",        0x0fc00000, 0xc0000000, "l", F_DELAYED },
{ "cb2",        0x2dc00000, 0xc0000000, ",al", F_DELAYED },
{ "cb2",        0x0dc00000, 0xc0000000, "l", F_DELAYED },
{ "cb23",       0x2bc00000, 0xc0000000, ",al", F_DELAYED },
{ "cb23",       0x0bc00000, 0xc0000000, "l", F_DELAYED },
{ "cb1",        0x29c00000, 0xc0000000, ",al", F_DELAYED },
{ "cb1",        0x09c00000, 0xc0000000, "l", F_DELAYED },
{ "cb13",       0x27c00000, 0xc0000000, ",al", F_DELAYED },
{ "cb13",       0x07c00000, 0xc0000000, "l", F_DELAYED },
{ "cb12",       0x25c00000, 0xc0000000, ",al", F_DELAYED },
{ "cb12",       0x05c00000, 0xc0000000, "l", F_DELAYED },
{ "cb123",      0x23c00000, 0xc0000000, ",al", F_DELAYED },
{ "cb123",      0x03c00000, 0xc0000000, "l", F_DELAYED },
{ "cb0",        0x33c00000, 0xc0000000, ",al", F_DELAYED },
{ "cb0",        0x13c00000, 0xc0000000, "l", F_DELAYED },
{ "cb03",       0x35c00000, 0xc0000000, ",al", F_DELAYED },
{ "cb03",       0x15c00000, 0xc0000000, "l", F_DELAYED },
{ "cb02",       0x37c00000, 0xc0000000, ",al", F_DELAYED },
{ "cb02",       0x17c00000, 0xc0000000, "l", F_DELAYED },
{ "cb023",      0x39c00000, 0xc0000000, ",al", F_DELAYED },
{ "cb023",      0x19c00000, 0xc0000000, "l", F_DELAYED },
{ "cb01",       0x3bc00000, 0xc0000000, ",al", F_DELAYED },
{ "cb01",       0x1bc00000, 0xc0000000, "l", F_DELAYED },
{ "cb013",      0x3dc00000, 0xc0000000, ",al", F_DELAYED },
{ "cb013",      0x1dc00000, 0xc0000000, "l", F_DELAYED },
{ "cb012",      0x3fc00000, 0xc0000000, ",al", F_DELAYED },
{ "cb012",      0x1fc00000, 0xc0000000, "l", F_DELAYED },

{ "fstoi",      0x81a01a20, 0x400025c0, "f,g", 0 },
{ "fdtoi",      0x81a01a40, 0x400025a0, "f,g", 0 },
{ "fxtoi",      0x81a01a60, 0x40002580, "f,g", 0 },

{ "fitox",      0x81a01980, 0x40002660, "f,g", 0 },
{ "fitod",      0x81a01900, 0x400026e0, "f,g", 0 },
{ "fitos",      0x81a01880, 0x40002660, "f,g", 0 },

{ "fstod",      0x81a01920, 0x400026c0, "f,g", 0 },
{ "fstox",      0x81a019a0, 0x40002640, "f,g", 0 },
{ "fdtos",      0x81a018c0, 0x40002720, "f,g", 0 },
{ "fdtox",      0x81a019c0, 0x40002620, "f,g", 0 },
{ "fxtos",      0x81a018e0, 0x40002700, "f,g", 0 },
{ "fxtod",      0x81a01960, 0x40002680, "f,g", 0 },

{ "fdivx",      0x81a009e0, 0x40083600, "e,f,g", 0 },
{ "fdivd",      0x81a009c0, 0x40003620, "e,f,g", 0 },
{ "fdivs",      0x81a009a0, 0x40003640, "e,f,g", 0 },

{ "fmuls",      0x81a00920, 0x400036c0, "e,f,g", 0 },
{ "fmuld",      0x81a00940, 0x400036a0, "e,f,g", 0 },
{ "fmulx",      0x81a00960, 0x40003680, "e,f,g", 0 },

{ "fsqrts",     0x81a00520, 0x40003ac0, "f,g", 0 },
{ "fsqrtd",     0x81a00540, 0x40003aa8, "f,g", 0 },
{ "fsqrtx",     0x81a00560, 0x40003a80, "f,g", 0 },

{ "fabss",      0x81a00120, 0x40003ec0, "f,g", 0 },
{ "fnegs",      0x81a000a0, 0x40003f40, "f,g", 0 },
{ "fmovs",      0x81a00020, 0x40003fc0, "f,g", 0 },

{ "fsubx",      0x81a008e0, 0x40003700, "e,f,g", 0 },
{ "fsubd",      0x81a008c0, 0x40003720, "e,f,g", 0 },
{ "fsubs",      0x81a008a0, 0x40003740, "e,f,g", 0 },

{ "faddx",      0x81a00860, 0x40003780, "e,f,g", 0 },
{ "faddd",      0x81a00840, 0x400037a0, "e,f,g", 0 },
{ "fadds",      0x81a00820, 0x400037c0, "e,f,g", 0 },

{ "fcmpex",     0x81a80ae0, 0x40003500, "e,f", 0 },
{ "fcmped",     0x81a80ac0, 0x40003520, "e,f", 0 },
{ "fcmpes",     0x81a80aa0, 0x40003540, "e,f", 0 },
{ "fcmpx",      0x81a80a60, 0x40003580, "e,f", 0 },
{ "fcmpd",      0x81a80a40, 0x400035a0, "e,f", 0 },
{ "fcmps",      0x81a80a20, 0x400035c0, "e,f", 0 },

{ "cpop1",      0x81b00000, 0x40480000, "[1+2],d", 0 },
{ "cpop2",      0x81b80000, 0x40400000, "[1+2],d", 0 },
};

#define NUMOPCODES ((sizeof sparc_opcodes)/(sizeof sparc_opcodes[0]))

