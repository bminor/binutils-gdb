/* Table of opcodes for the PA-RISC.
   Copyright (C) 1990, 1991, 1993 Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

This file is part of GAS, the GNU Assembler, and GDB, the GNU disassembler.

GAS/GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS/GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS or GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#if !defined(__STDC__) && !defined(const)
#define const
#endif

/*
 * Structure of an opcode table entry.
 */

/* There are two kinds of delay slot nullification: normal which is
 * controled by the nullification bit, and conditional, which depends
 * on the direction of the branch and its success or failure.
 */
enum delay_type {NONE, NORMAL, CONDITIONAL};
struct pa_opcode
{
    const char *name;
    unsigned long int match;	/* Bits that must be set...  */
    unsigned long int mask;	/* ... in these bits. */
    char *args;
    /* Nonzero if this is a delayed branch instruction.  */
    /* What uses this field?  Nothing in opcodes or gas that I saw.
       If nothing needs it, we could reduce this table by 20% (for
       most machines).  KR */
    char delayed;
};

/*
   All hppa opcodes are 32 bits.

   The match component is a mask saying which bits must match a
   particular opcode in order for an instruction to be an instance
   of that opcode.

   The args component is a string containing one character
   for each operand of the instruction.

   Bit positions in this description follow HP usage of lsb = 31,
   "at" is lsb of field.

   In the args field, the following characters must match exactly:

	'+,() '

   In the args field, the following characters are unused:

	'  "#$%    *+- ./   3      :; =  @'
	' B         L N            [\] _'
	'    e gh   lm   qr        { } '

   Here are all the characters:

	' !"#$%&'()*+-,./0123456789:;<=>?@'
	'ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_'
	'abcdefghijklmnopqrstuvwxyz{|}~'

Kinds of operands:
   x    integer register field at 15.
   b    integer register field at 10.
   t    integer register field at 31.
   y    floating point register field at 31
   5    5 bit immediate at 15.
   s    2 bit space specifier at 17.
   S    3 bit space specifier at 18.
   c    indexed load completer.
   C    short load and store completer.
   Y	Store Bytes Short completer
   <    non-negated compare/subtract conditions.
   a	compare/subtract conditions
   d    non-negated add conditions
   &    logical instruction conditions
   U    unit instruction conditions
   >    shift/extract/deposit conditions.
   ~    bvb,bb conditions
   V    5 bit immediate value at 31
   i    11 bit immediate value at 31
   j    14 bit immediate value at 31
   k    21 bit immediate value at 31
   n	nullification for branch instructions
   w    12 bit branch displacement
   W    17 bit branch displacement (PC relative)
   z    17 bit branch displacement (just a number, not an address)

Also these:

   p    5 bit shift count at 26 (to support the SHD instruction) encoded as
        31-p
   P    5 bit bit position at 26
   T    5 bit field length at 31 (encoded as 32-T)
   A    13 bit immediate at 18 (to support the BREAK instruction)
   ^	like b, but describes a control register
   Z    System Control Completer (to support LPA, LHA, etc.)
   D    26 bit immediate at 31 (to support the DIAG instruction)

   f    3 bit Special Function Unit identifier at 25
   O    20 bit Special Function Unit operation split between 15 bits at 20
        and 5 bits at 31
   o    15 bit Special Function Unit operation at 20
   2    22 bit Special Function Unit operation split between 17 bits at 20
        and 5 bits at 31
   1    15 bit Special Function Unit operation split between 10 bits at 20
        and 5 bits at 31
   0    10 bit Special Function Unit operation split between 5 bits at 20
        and 5 bits at 31
   u    3 bit coprocessor unit identifier at 25
   F    Source Floating Point Operand Format Completer encoded 2 bits at 20
   I    Source Floating Point Operand Format Completer encoded 1 bits at 20
	(for 0xe format FP instructions)
   G    Destination Floating Point Operand Format Completer encoded 2 bits at 18
   M    Floating-Point Compare Conditions (encoded as 5 bits at 31)
   ?    negated compare/subtract conditions.
   !    non-negated add conditions.

   s    2 bit space specifier at 17.
   b    register field at 10.
   r	5 bit immediate value at 31 (for the break instruction)
	(very similar to V above, except the value is unsigned instead of
	low_sign_ext)
   R	5 bit immediate value at 15 (for the ssm, rsm instruction)
	(same as r above, except the value is in a different location)
   Q	5 bit immediate value at 10 (a bit position specified in
	the bb instruction. It's the same as r above, except the
        value is in a different location)
   |	shift/extract/deposit conditions when used in a conditional branch

And these (PJH) for PA-89 F.P. registers and instructions:

   v    a 't' operand type extended to handle L/R register halves.
   E    a 'b' operand type extended to handle L/R register halves.
   X    an 'x' operand type extended to handle L/R register halves.
   J    a 'b' operand type further extended to handle extra 1.1 registers
   K    a 'x' operand type further extended to handle extra 1.1 registers
   4    a variation of the 'b' operand type for 'fmpyadd' and 'fmpysub'
   6    a variation of the 'x' operand type for 'fmpyadd' and 'fmpysub'
   7    a variation of the 't' operand type for 'fmpyadd' and 'fmpysub'
   8    5 bit register field at 20 (used in 'fmpyadd' and 'fmpysub')
   9    5 bit register field at 25 (used in 'fmpyadd' and 'fmpysub')
   H    Floating Point Operand Format at 26 for 'fmpyadd' and 'fmpysub'
        (very similar to 'F')
*/

/* The order of the opcodes in this table is significant:

   * The assembler requires that all instances of the same mnemonic must be
   consecutive.  If they aren't, the assembler will bomb at runtime.

   * The disassembler should not care about the order of the opcodes.  */

static const struct pa_opcode pa_opcodes[] =
{

/* pseudo-instructions */

{ "b",		0xe8000000, 0xffe0e000, "nW", NORMAL}, /* bl foo,r0 */
{ "ldi",	0x34000000, 0xffe0c000, "j,x"},	/* ldo val(r0),r */
{ "comib", 	0x84000000, 0xfc000000, "?n5,b,w", CONDITIONAL}, /* comib{tf}*/
{ "comb",	0x80000000, 0xfc000000, "?nx,b,w", CONDITIONAL}, /* comb{tf} */
{ "addb",	0xa0000000, 0xfc000000, "!nx,b,w", CONDITIONAL}, /* addb{tf} */
{ "addib",	0xa4000000, 0xfc000000, "!n5,b,w", CONDITIONAL}, /* addib{tf}*/
{ "nop",        0x08000240, 0xffffffff, ""},      /* or 0,0,0 */
{ "copy",       0x08000240, 0xffe0ffe0, "x,t"},   /* or r,0,t */
{ "mtsar",      0x01601840, 0xffe0ffff, "x"}, /* mtctl r,cr11 */

/* Loads and Stores for integer registers.  */
{ "ldw",        0x48000000, 0xfc000000, "j(s,b),x"},
{ "ldw",        0x48000000, 0xfc000000, "j(b),x"},
{ "ldh",        0x44000000, 0xfc000000, "j(s,b),x"},
{ "ldh",        0x44000000, 0xfc000000, "j(b),x"},
{ "ldb",        0x40000000, 0xfc000000, "j(s,b),x"},
{ "ldb",        0x40000000, 0xfc000000, "j(b),x"},
{ "stw",        0x68000000, 0xfc000000, "x,j(s,b)"},
{ "stw",        0x68000000, 0xfc000000, "x,j(b)"},
{ "sth",        0x64000000, 0xfc000000, "x,j(s,b)"},
{ "sth",        0x64000000, 0xfc000000, "x,j(b)"},
{ "stb",        0x60000000, 0xfc000000, "x,j(s,b)"},
{ "stb",        0x60000000, 0xfc000000, "x,j(b)"},
{ "ldwm",       0x4c000000, 0xfc000000, "j(s,b),x"},
{ "ldwm",       0x4c000000, 0xfc000000, "j(b),x"},
{ "stwm",       0x6c000000, 0xfc000000, "x,j(s,b)"},
{ "stwm",       0x6c000000, 0xfc000000, "x,j(b)"},
{ "ldwx",       0x0c000080, 0xfc001fc0, "cx(s,b),t"},
{ "ldwx",       0x0c000080, 0xfc001fc0, "cx(b),t"},
{ "ldhx",       0x0c000040, 0xfc001fc0, "cx(s,b),t"},
{ "ldhx",       0x0c000040, 0xfc001fc0, "cx(b),t"},
{ "ldbx",       0x0c000000, 0xfc001fc0, "cx(s,b),t"},
{ "ldbx",       0x0c000000, 0xfc001fc0, "cx(b),t"},
{ "ldwax",      0x0c000180, 0xfc00dfc0, "cx(b),t"},
{ "ldcwx",      0x0c0001c0, 0xfc001fc0, "cx(s,b),t"},
{ "ldcwx",      0x0c0001c0, 0xfc001fc0, "cx(b),t"},
{ "ldws",	0x0c001080, 0xfc001fc0, "C5(s,b),t"},
{ "ldws",	0x0c001080, 0xfc001fc0, "C5(b),t"},
{ "ldhs",	0x0c001040, 0xfc001fc0, "C5(s,b),t"},
{ "ldhs",	0x0c001040, 0xfc001fc0, "C5(b),t"},
{ "ldbs",	0x0c001000, 0xfc001fc0, "C5(s,b),t"},
{ "ldbs",	0x0c001000, 0xfc001fc0, "C5(b),t"},
{ "ldwas",	0x0c001180, 0xfc00dfc0, "C5(b),t"},
{ "ldcws",	0x0c0011c0, 0xfc001fc0, "C5(s,b),t"},
{ "ldcws",	0x0c0011c0, 0xfc001fc0, "C5(b),t"},
{ "stws",	0x0c001280, 0xfc001fc0, "Cx,V(s,b)"},
{ "stws",	0x0c001280, 0xfc001fc0, "Cx,V(b)"},
{ "sths",	0x0c001240, 0xfc001fc0, "Cx,V(s,b)"},
{ "sths",	0x0c001240, 0xfc001fc0, "Cx,V(b)"},
{ "stbs",	0x0c001200, 0xfc001fc0, "Cx,V(s,b)"},
{ "stbs",	0x0c001200, 0xfc001fc0, "Cx,V(b)"},
{ "stwas",	0x0c001380, 0xfc00dfc0, "Cx,V(b)"},
{ "stbys",	0x0c001300, 0xfc001fc0, "Yx,V(s,b)"},
{ "stbys",	0x0c001300, 0xfc001fc0, "Yx,V(b)"},

/* Immediate instructions.  */
{ "ldo",	0x34000000, 0xfc00c000, "j(b),x"},
{ "ldil",	0x20000000, 0xfc000000, "k,b"},
{ "addil",	0x28000000, 0xfc000000, "k,b"},

/* Branching instructions. */
{ "bl",		0xe8000000, 0xfc00e000, "nW,b", NORMAL},
{ "gate",	0xe8002000, 0xfc00e000, "nW,b", NORMAL},
{ "blr",	0xe8004000, 0xfc00e001, "nx,b", NORMAL},
{ "bv",		0xe800c000, 0xfc00e001, "nx(b)", NORMAL},
{ "bv",		0xe800c000, 0xfc00e001, "n(b)", NORMAL},
{ "be",		0xe0000000, 0xfc000000, "nz(S,b)", NORMAL},
{ "ble",	0xe4000000, 0xfc000000, "nz(S,b)", NORMAL},
{ "movb",	0xc8000000, 0xfc000000, "|nx,b,w", CONDITIONAL},
{ "movib",	0xcc000000, 0xfc000000, "|n5,b,w", CONDITIONAL},
{ "combt",	0x80000000, 0xfc000000, "<nx,b,w", CONDITIONAL},
{ "combf",	0x88000000, 0xfc000000, "<nx,b,w", CONDITIONAL},
{ "comibt",	0x84000000, 0xfc000000, "<n5,b,w", CONDITIONAL},
{ "comibf",	0x8c000000, 0xfc000000, "<n5,b,w", CONDITIONAL},
{ "addbt",	0xa0000000, 0xfc000000, "!nx,b,w", CONDITIONAL},
{ "addbf",	0xa8000000, 0xfc000000, "!nx,b,w", CONDITIONAL},
{ "addibt",	0xa4000000, 0xfc000000, "!n5,b,w", CONDITIONAL},
{ "addibf",	0xac000000, 0xfc000000, "!n5,b,w", CONDITIONAL},
{ "bvb",	0xc0000000, 0xffe00000, "~nx,w", CONDITIONAL},
{ "bb",		0xc4000000, 0xfc000000, "~nx,Q,w", CONDITIONAL}, 

/* Computation Instructions */

{ "add",        0x08000600, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "addl",       0x08000a00, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "addo",       0x08000e00, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "addc",       0x08000700, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "addco",      0x08000f00, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "sh1add",     0x08000640, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "sh1addl",    0x08000a40, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "sh1addo",    0x08000e40, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "sh2add",     0x08000680, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "sh2addl",    0x08000a80, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "sh2addo",    0x08000e80, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "sh3add",     0x080006c0, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "sh3addl",    0x08000ac0, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "sh3addo",    0x08000ec0, 0xfc000fe0, "dx,b,t", CONDITIONAL},
{ "sub",        0x08000400, 0xfc000fe0, "ax,b,t", CONDITIONAL},
{ "subo",       0x08000c00, 0xfc000fe0, "ax,b,t", CONDITIONAL},
{ "subb",       0x08000500, 0xfc000fe0, "ax,b,t", CONDITIONAL},
{ "subbo",      0x08000d00, 0xfc000fe0, "ax,b,t", CONDITIONAL},
{ "subt",       0x080004c0, 0xfc000fe0, "ax,b,t", CONDITIONAL},
{ "subto",      0x08000cc0, 0xfc000fe0, "ax,b,t", CONDITIONAL},
{ "ds",         0x08000440, 0xfc000fe0, "ax,b,t", CONDITIONAL},
{ "comclr",     0x08000880, 0xfc000fe0, "ax,b,t", CONDITIONAL},
{ "or",         0x08000240, 0xfc000fe0, "&x,b,t", CONDITIONAL},
{ "xor",        0x08000280, 0xfc000fe0, "&x,b,t", CONDITIONAL},
{ "and",        0x08000200, 0xfc000fe0, "&x,b,t", CONDITIONAL},
{ "andcm",      0x08000000, 0xfc000fe0, "&x,b,t", CONDITIONAL},
{ "uxor",       0x08000380, 0xfc000fe0, "Ux,b,t", CONDITIONAL},
{ "uaddcm",     0x08000980, 0xfc000fe0, "Ux,b,t", CONDITIONAL},
{ "uaddcmt",    0x080009c0, 0xfc000fe0, "Ux,b,t", CONDITIONAL},
{ "dcor",       0x08000b80, 0xfc1f0fe0, "Ub,t",   CONDITIONAL},
{ "idcor",      0x08000bc0, 0xfc1f0fe0, "Ub,t",   CONDITIONAL},
{ "addi",       0xb4000000, 0xfc000800, "di,b,x", CONDITIONAL},
{ "addio",      0xb4000800, 0xfc000800, "di,b,x", CONDITIONAL},
{ "addit",      0xb0000000, 0xfc000800, "di,b,x", CONDITIONAL},
{ "addito",     0xb0000800, 0xfc000800, "di,b,x", CONDITIONAL},
{ "subi",       0x94000000, 0xfc000800, "ai,b,x", CONDITIONAL},
{ "subio",      0x94000800, 0xfc000800, "ai,b,x", CONDITIONAL},
{ "comiclr",    0x90000000, 0xfc000800, "ai,b,x", CONDITIONAL},

/* Extract and Deposit Instructions */

{ "vshd",       0xd0000000, 0xfc001fe0, ">x,b,t", CONDITIONAL},
{ "shd",        0xd0000800, 0xfc001c00, ">x,b,p,t", CONDITIONAL},
{ "vextru",     0xd0001000, 0xfc001fe0, ">b,T,x", CONDITIONAL},
{ "vextrs",     0xd0001400, 0xfc001fe0, ">b,T,x", CONDITIONAL},
{ "extru",      0xd0001800, 0xfc001c00, ">b,P,T,x", CONDITIONAL},
{ "extrs",      0xd0001c00, 0xfc001c00, ">b,P,T,x", CONDITIONAL},
{ "zvdep",      0xd4000000, 0xfc001fe0, ">x,T,b", CONDITIONAL},
{ "vdep",       0xd4000400, 0xfc001fe0, ">x,T,b", CONDITIONAL},
{ "zdep",       0xd4000800, 0xfc001c00, ">x,p,T,b", CONDITIONAL},
{ "dep",        0xd4000c00, 0xfc001c00, ">x,p,T,b", CONDITIONAL},
{ "zvdepi",     0xd4001000, 0xfc001fe0, ">5,T,b", CONDITIONAL},
{ "vdepi",      0xd4001400, 0xfc001fe0, ">5,T,b", CONDITIONAL},
{ "zdepi",      0xd4001800, 0xfc001c00, ">5,p,T,b", CONDITIONAL},
{ "depi",       0xd4001c00, 0xfc001c00, ">5,p,T,b", CONDITIONAL},

/* System Control Instructions */

{ "break",      0x00000000, 0xfc001fe0, "r,A"},
{ "rfi",        0x00000c00, 0xffffffff, ""},
{ "rfir",       0x00000ca0, 0xffffffff, ""},
{ "ssm",        0x00000d60, 0xffe0ffe0, "R,t"},
{ "rsm",        0x00000e60, 0xffe0ffe0, "R,t"},
{ "mtsm",       0x00001860, 0xffe0ffff, "x"},
{ "ldsid",      0x000010a0, 0xfc1f3fe0, "(s,b),t"},
{ "ldsid",      0x000010a0, 0xfc1f3fe0, "(b),t"},
{ "mtsp",       0x00001820, 0xffe01fff, "x,S"},
{ "mtctl",      0x00001840, 0xfc00ffff, "x,^"},
{ "mfsp",       0x000004a0, 0xffff1fe0, "S,t"},
{ "mfctl",      0x000008a0, 0xfc1fffe0, "^,t"},
{ "sync",       0x00000400, 0xffffffff, ""},
{ "prober",     0x04001180, 0xfc003fe0, "(s,b),x,t"},
{ "prober",     0x04001180, 0xfc003fe0, "(b),x,t"},
{ "proberi",    0x04003180, 0xfc003fe0, "(s,b),5,t"},
{ "proberi",    0x04003180, 0xfc003fe0, "(b),5,t"},
{ "probew",     0x040011c0, 0xfc003fe0, "(s,b),x,t"},
{ "probew",     0x040011c0, 0xfc003fe0, "(b),x,t"},
{ "probewi",    0x040031c0, 0xfc003fe0, "(s,b),5,t"},
{ "probewi",    0x040031c0, 0xfc003fe0, "(b),5,t"},
{ "lpa",        0x04001340, 0xfc003fc0, "Zx(s,b),t"},
{ "lpa",        0x04001340, 0xfc003fc0, "Zx(b),t"},
{ "lha",        0x04001300, 0xfc003fc0, "Zx(s,b),t"},
{ "lha",        0x04001300, 0xfc003fc0, "Zx(b),t"},
{ "pdtlb",      0x04001200, 0xfc003fdf, "Zx(s,b)"},
{ "pdtlb",      0x04001200, 0xfc003fdf, "Zx(b)"},
{ "pitlb",      0x04000200, 0xfc003fdf, "Zx(s,b)"},
{ "pitlb",      0x04000200, 0xfc003fdf, "Zx(b)"},
{ "pdtlbe",     0x04001240, 0xfc003fdf, "Zx(s,b)"},
{ "pdtlbe",     0x04001240, 0xfc003fdf, "Zx(b)"},
{ "pitlbe",     0x04000240, 0xfc003fdf, "Zx(s,b)"},
{ "pitlbe",     0x04000240, 0xfc003fdf, "Zx(b)"},
{ "idtlba",     0x04001040, 0xfc003fff, "x,(s,b)"},
{ "idtlba",     0x04001040, 0xfc003fff, "x,(b)"},
{ "iitlba",     0x04000040, 0xfc003fff, "x,(s,b)"},
{ "iitlba",     0x04000040, 0xfc003fff, "x,(b)"},
{ "idtlbp",     0x04001000, 0xfc003fff, "x,(s,b)"},
{ "idtlbp",     0x04001000, 0xfc003fff, "x,(b)"},
{ "iitlbp",     0x04000000, 0xfc003fff, "x,(s,b)"},
{ "iitlbp",     0x04000000, 0xfc003fff, "x,(b)"},
{ "pdc",        0x04001380, 0xfc003fdf, "Zx(s,b)"},
{ "pdc",        0x04001380, 0xfc003fdf, "Zx(b)"},
{ "fdc",        0x04001280, 0xfc003fdf, "Zx(s,b)"},
{ "fdc",        0x04001280, 0xfc003fdf, "Zx(b)"},
{ "fic",        0x04000280, 0xfc003fdf, "Zx(s,b)"},
{ "fic",        0x04000280, 0xfc003fdf, "Zx(b)"},
{ "fdce",       0x040012c0, 0xfc003fdf, "Zx(s,b)"},
{ "fdce",       0x040012c0, 0xfc003fdf, "Zx(b)"},
{ "fice",       0x040002c0, 0xfc003fdf, "Zx(s,b)"},
{ "fice",       0x040002c0, 0xfc003fdf, "Zx(b)"},
{ "diag",       0x14000000, 0xfc000000, "D"},
{ "gfw",	0x04001680, 0xfc003fdf, "Zx(s,b)"},
{ "gfw",	0x04001680, 0xfc003fdf, "Zx(b)"},
{ "gfr",	0x04001a80, 0xfc003fdf, "Zx(s,b)"},
{ "gfr",	0x04001a80, 0xfc003fdf, "Zx(b)"},

/* Floating Point Coprocessor Instructions */
  
{ "fldwx",      0x24000000, 0xfc001f80, "cx(s,b),v"},
{ "fldwx",      0x24000000, 0xfc001f80, "cx(b),v"},
{ "flddx",      0x2c000000, 0xfc001fc0, "cx(s,b),y"},
{ "flddx",      0x2c000000, 0xfc001fc0, "cx(b),y"},
{ "fstwx",      0x24000200, 0xfc001fc0, "cv,x(s,b)"},
{ "fstwx",      0x24000200, 0xfc001fc0, "cv,x(b)"},
{ "fstdx",      0x2c000200, 0xfc001fc0, "cy,x(s,b)"},
{ "fstdx",      0x2c000200, 0xfc001fc0, "cy,x(b)"},
{ "fldws",      0x24001000, 0xfc001f80, "C5(s,b),v"},
{ "fldws",      0x24001000, 0xfc001f80, "C5(b),v"},
{ "fldds",      0x2c001000, 0xfc001fc0, "C5(s,b),y"},
{ "fldds",      0x2c001000, 0xfc001fc0, "C5(b),y"},
{ "fstws",      0x24001200, 0xfc001f80, "Cv,5(s,b)"},
{ "fstws",      0x24001200, 0xfc001f80, "Cy,5(b)"},
{ "fstds",      0x2c001200, 0xfc001fc0, "Cy,5(s,b)"},
{ "fstds",      0x2c001200, 0xfc001fc0, "Cy,5(b)"},
{ "fadd",       0x30000600, 0xfc00e7e0, "FE,X,v"},
{ "fadd",       0x38000600, 0xfc00e720, "IJ,K,v"},
{ "fsub",       0x30002600, 0xfc00e7e0, "FE,X,v"},
{ "fsub",       0x38002600, 0xfc00e720, "IJ,K,v"},
{ "fmpy",       0x30004600, 0xfc00e7e0, "FE,X,v"},
{ "fmpy",       0x38004600, 0xfc00e720, "IJ,K,v"},
{ "fdiv",       0x30006600, 0xfc00e7e0, "FE,X,v"},
{ "fdiv",       0x38006600, 0xfc00e720, "IJ,K,v"},
{ "fsqrt",      0x30008000, 0xfc1fe7e0, "FE,v"},
{ "fsqrt",      0x38008000, 0xfc1fe720, "FJ,v"},
{ "fabs",       0x30006000, 0xfc1fe7e0, "FE,v"},
{ "fabs",       0x38006000, 0xfc1fe720, "FJ,v"},
{ "frem",       0x30008600, 0xfc00e7e0, "FE,X,v"},
{ "frem",       0x38008600, 0xfc00e720, "FJ,K,v"},
{ "frnd",       0x3000a000, 0xfc1fe7e0, "FE,v"},
{ "frnd",       0x3800a000, 0xfc1fe720, "FJ,v"},
{ "fcpy",       0x30004000, 0xfc1fe7e0, "FE,v"},
{ "fcpy",       0x38004000, 0xfc1fe720, "FJ,v"},
{ "fcnvff",     0x30000200, 0xfc1f87e0, "FGE,v"},
{ "fcnvff",     0x38000200, 0xfc1f8720, "FGJ,v"},
{ "fcnvxf",     0x30008200, 0xfc1f87e0, "FGE,v"},
{ "fcnvxf",     0x38008200, 0xfc1f8720, "FGJ,v"},
{ "fcnvfx",     0x30010200, 0xfc1f87e0, "FGE,v"},
{ "fcnvfx",     0x38010200, 0xfc1f8720, "FGJ,v"},
{ "fcnvfxt",    0x30018200, 0xfc1f87e0, "FGE,v"},
{ "fcnvfxt",    0x38018200, 0xfc1f8720, "FGJ,v"},
{ "fcmp",       0x30000400, 0xfc00e7e0, "FME,X"},
{ "fcmp",       0x38000400, 0xfc00e720, "IMJ,K"},
{ "xmpyu",	0x38004700, 0xfc00e720, "FE,X,v"},
{ "fmpyadd",	0x18000000, 0xfc000000, "H4,6,7,9,8"},
{ "fmpysub",	0x98000000, 0xfc000000, "H4,6,7,9,8"},
{ "ftest",      0x30002420, 0xffffffff, ""},


/* Assist Instructions */

{ "spop0",      0x10000000, 0xfc000600, ",f,On", NORMAL},
{ "spop1",      0x10000200, 0xfc000600, ",f,ont", NORMAL},
{ "spop2",      0x10000400, 0xfc000600, ",f,1nb", NORMAL},
{ "spop3",      0x10000600, 0xfc000600, ",f,0nx,b", NORMAL},
{ "copr",       0x30000000, 0xfc000000, ",u,2n", NORMAL},
{ "cldwx",      0x24000000, 0xfc001e00, ",u,Zx(s,b),t"},
{ "cldwx",      0x24000000, 0xfc001e00, ",u,Zx(b),t"},
{ "clddx",      0x2c000000, 0xfc001e00, ",u,Zx(s,b),t"},
{ "clddx",      0x2c000000, 0xfc001e00, ",u,Zx(b),t"},
{ "cstwx",      0x24000200, 0xfc001e00, ",u,Zt,x(s,b)"},
{ "cstwx",      0x24000200, 0xfc001e00, ",u,Zt,x(b)"},
{ "cstdx",      0x2c000200, 0xfc001e00, ",u,Zt,x(s,b)"},
{ "cstdx",      0x2c000200, 0xfc001e00, ",u,Zt,x(b)"},
{ "cldws",      0x24001000, 0xfc001e00, ",u,Z5(s,b),t"},
{ "cldws",      0x24001000, 0xfc001e00, ",u,Z5(b),t"},
{ "cldds",      0x2c001000, 0xfc001e00, ",u,Z5(s,b),t"},
{ "cldds",      0x2c001000, 0xfc001e00, ",u,Z5(b),t"},
{ "cstws",      0x24001200, 0xfc001e00, ",u,Zt,5(s,b)"},
{ "cstws",      0x24001200, 0xfc001e00, ",u,Zt,5(b)"},
{ "cstds",      0x2c001200, 0xfc001e00, ",u,Zt,5(s,b)"},
{ "cstds",      0x2c001200, 0xfc001e00, ",u,Zt,5(b)"},
};

#define NUMOPCODES ((sizeof pa_opcodes)/(sizeof pa_opcodes[0]))

/* SKV 12/18/92. Added some denotations for various operands. */

#define PA_IMM11_AT_31 'i'
#define PA_IMM14_AT_31 'j'
#define PA_IMM21_AT_31 'k'
#define PA_DISP12 'w'
#define PA_DISP17 'W'

#define N_HPPA_OPERAND_FORMATS 5
