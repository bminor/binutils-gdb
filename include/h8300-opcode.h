/* Opcode table for the H8-300
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
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  


By steve@cygnus.com
*/

typedef enum 
  {
Hex0=0,
Hex1,Hex2,Hex3,Hex4,Hex5,Hex6,Hex7,Hex8,Hex9,HexA,HexB,HexC,HexD,HexE,HexF,
    START = 0x20,
    KBIT,				/* K is #1, or #2, yielding 0x0 or 0x8 */
    IMM3,				/* bit number */
    RD8,				/* 8 bit reg as 2nd op  */
    RD16,				/* 16 bit reg as 2nd op */
    RS8,				/* 8 bit reg as 1st op */
    RS16,				/* 16 bit reg 1st op */
    IMM8,				/* constant which fits into 8 bits */
    IMM16,			/* constant which fits into 16 bits */
    CCR,				/* CCR reg */
    ABS8SRC,			/* abs 8 address mode */
    ABS8DST,			/* abs 8 address mode */
    DISP8,			/* pc rel displacement */
    ABS16SRC,			/* abs 16 address mode */
    ABS16DST,			/* abs 16 address mode */
    DISPSRC,			/* @(r:16) address mode src */
    DISPDST,			/* @(r:16) address mode dst*/
    DISPREG,			/* register from DISP address mode */
    RDDEC,			/* @-rn mode */
    RSINC,			/* @rn+ mode */
    RDIND,			/* @R mode dst */
    RSIND,			/* @R mode src */
    MEMIND,			/* @@r8 mode */
    IGNORE,
    B30 = 0x40,			/* bit 3 must be low */
    B31 = 0x80,			/* bit 3 must be high */

  E				/* End of list */
  } op_enum_type;


struct code {



















  op_enum_type nib[9];
} ;

struct h8_opcode {
  int length;
  int noperands;	/* Number of operands this opcode takes */
  int nopcodes;		/* Number of opcodes with same mnemonic */
  char *name;
  struct code args;
  struct code data;
int value;
};



struct h8_opcode h8_opcodes[] 
#ifdef DEFINE_TABLE

#define BITOP(n,imm, name, op00, op01,op10,op11, op20,op21)\
{  2,  2,n, name,	{imm,RD8,E},	{op00, op01, imm, RD8,E}},\
{ 4,   2,n, name,	{imm,RDIND,E},	{op10, op11, RDIND, 0, op00,op01, imm, 0,E}},\
{ 4,   2,n, name,	{imm,ABS8DST,E},	{op20, op21, ABS8DST, IGNORE, op00,op01, imm, 0,E}}

#define EBITOP(n,imm, name, op00, op01,op10,op11, op20,op21)\
   BITOP(n,imm, name, op00+1, op01, op10,op11, op20,op21),\
   BITOP(n, RS8,  name, op00, op01, op10,op11, op20,op21)

#define BRANCH(name, op) \
{   2,  1, 1,name,{DISP8,E}, { 0x4, op, DISP8,IGNORE,E }}

#define TWOOP(name, op1, op2,op3) \
{ 2,  2, 2, name, {IMM8, RD8,E},	{ op1, RD8, IMM8,IGNORE,E}},\
{ 2,  2, 2, name, {RS8, RD8, E},	{ op2, op3, RS8, RD8 ,E}} 

#define UNOP(name, op1, op2) \
{ 2, 1, 1, name, {RS8, E}, { op1, op2, 0, RS8, E}}

#define UNOP3(name, op1, op2, op3) \
{	2, 1, 1, name , {RS8, E}, {op1, op2, op3, RS8, E}}

#define WTWOP(name, op1, op2) \
{2, 2, 1, name, {RS16, RD16, E}, { op1, op2, RS16, RD16, E}}

= 
{
  TWOOP("add.b", 0x8,	0x0,0x8),
  WTWOP("add.w", 0x0, 0x9),
    { 2,  2, 1, "adds", {KBIT,RD16|B30, E},{0x0, 0xb, KBIT, RD16|B30, E}},
  TWOOP("addx", 0x9,0x0,0xe),
  TWOOP("and", 0xe,0x1,0x6),
    {  2,  2, 1, "andc",	{IMM8, CCR, E},	{ 0x0, 0x6, IMM8,IGNORE, E}},
  BITOP(3,IMM3|B30, "band", 0x7, 0x6, 0x7, 0xc,  0x7, 0xe),
  BRANCH("bra",	0x0),
  BRANCH("bt",	0x0),
  BRANCH("brn",	0x1),
  BRANCH("bf",	0x1),
  BRANCH("bhi",	0x2),
  BRANCH("bls",	0x3),
  BRANCH("bcc",	0x4),
  BRANCH("bhs",	0x4),
  BRANCH("bcs",	0x5),
  BRANCH("blo",	0x5),
  BRANCH("bne",	0x6),
  BRANCH("beq",	0x7),
  BRANCH("bvc",	0x8),
  BRANCH("bvs",	0x9),
  BRANCH("bpl",	0xa),
  BRANCH("bmi",	0xb),
  BRANCH("bge",	0xc),
  BRANCH("blt",	0xd),
  BRANCH("bgt",	0xe),
  BRANCH("ble",	0xf),
  EBITOP(6,IMM3|B30,"bclr", 0x6, 0x2, 0x7, 0xd, 0x7, 0xf),
  BITOP(3,IMM3|B31,"biand", 0x7, 0x6, 0x7, 0xc, 0x7, 0xe),
  BITOP(3,IMM3|B31, "bild",	0x7, 0x7,0x7, 0xc, 0x7, 0xe),
  BITOP(3,IMM3|B31, "bior",	0x7, 0x4,0x7, 0xc, 0x7, 0xe),
  BITOP(3,IMM3|B31, "bist",	0x6, 0x7,0x7, 0xd, 0x7, 0xe),
  BITOP(3,IMM3|B31, "bixor",	0x7, 0x5,0x7, 0xc, 0x7, 0xe),
  BITOP(3,IMM3|B30, "bld",	0x7, 0x7,0x7, 0xc, 0x7, 0xe),
  EBITOP(6,IMM3|B30,"bnot", 0x6, 0x1, 0x7, 0xd, 0x7, 0xf),
  BITOP(3,IMM3|B30,"bor",	0x7, 0x4,0x7, 0xc, 0x7, 0xe),
  EBITOP(6,IMM3|B30,"bset",	0x6, 0x0,0x7, 0xd, 0x7, 0xf),
    {  2,  1, 1, "bsr",{DISP8, E},{ 0x5, 0x5, DISP8,IGNORE, E}},
  BITOP(3,IMM3|B30, "bst",	0x6, 0x7,0x7, 0xd, 0x7, 0xf),
  EBITOP(6,IMM3|B30, "btst",	0x6, 0x3,0x7, 0xc, 0x7, 0xe),
  BITOP(3,IMM3|B30, "bxor",	0x7,0x5,0x7, 0xc, 0x7, 0xe),
  TWOOP( "cmp.b",0xa, 0x1, 0xc),
  WTWOP( "cmp.w",0x1,0xd),
  UNOP(   "daa",0x0, 0xf),
  UNOP(   "das",0x1, 0xf),
  UNOP(   "dec",0x1, 0xa),
    {  2,  2, 1, "divxu",{RS8, RD16|B30, E}, { 0x5, 0x1, RS8, RD16|B30, E}},
    {  4,  0, 1, "eepmov",{ E}, {0x7, 0xb, 0x5, 0xc, 0x5, 0x9, 0x8, 0xf,E}},
  UNOP(   "inc",	0x0, 0xa),
    {  2,  1, 3, "jmp",{RSIND|B30, E}, {0x5, 0x9, RSIND|B30, 0x0, E}},
    {  4,  1, 3, "jmp",{ABS16SRC, E}, {0x5, 0xa, 0x0, 0x0, ABS16SRC, IGNORE,IGNORE,IGNORE,E}},
    {  2,  1, 3, "jmp",{MEMIND, E}, {0x5, 0xb, MEMIND,IGNORE, E}},
    {  2,  1, 3, "jsr",{RSIND|B30, E}, {0x5, 0xd, RSIND|B30, 0x0, E}},
    {  4,  1, 3, "jsr",{ABS16SRC, E}, {0x5, 0xe, 0x0, 0x0, ABS16SRC,IGNORE,IGNORE,IGNORE, E}},
    {  2,  1, 3, "jsr",{MEMIND, E}, {0x5, 0xf, MEMIND, IGNORE,E}},
    {  2,  2, 2, "ldc",	{IMM8, CCR, E}, { 0x0, 0x7, IMM8,IGNORE, E}},
    {  2,  2, 2, "ldc",	{RS8, CCR, E},	{ 0x0, 0x3, 0x0, RS8, E}},

    {  2,  2,13,  "mov.b",	{RS8, RD8, E},	{ 0x0, 0xc, RS8, RD8, E}},
    {  2,  2,13,  "mov.b",	{IMM8, RD8, E},	{ 0xf, RD8, IMM8,IGNORE, E}},
    {  2,  2,13,  "mov.b",	{RSIND|B30,RD8, E},	{ 0x6, 0x8, RSIND|B30, RD8, E}},
    {  4,  2,13,  "mov.b",	{DISPSRC,RD8, E},	{ 0x6, 0xe, DISPREG|B30, RD8, DISPSRC, IGNORE, IGNORE, IGNORE, E}} ,
    {  2,  2,13,  "mov.b",	{RSINC|B30, RD8, E},	{ 0x6, 0xc, RSINC|B30, RD8, E}},

    {  4,  2,13,  "mov.b",	{ABS16SRC, RD8, E},	{ 0x6, 0xa, 0x0, RD8,ABS16SRC, IGNORE,IGNORE,IGNORE,E}},
    {  2,  2,13,  "mov.b",	{ABS8SRC, RD8, E},	{ 0x2, RD8, ABS8SRC,IGNORE, E}},
    {  2,  2,13,  "mov.b", 	{RS8, RDIND|B30, E},	{ 0x6, 0x8, RDIND|B31, RS8, E}},
    {  4,  2,13,  "mov.b",	{RS8, DISPDST, E},	{ 0x6, 0xe, DISPREG|B31, RS8,DISPDST, IGNORE, IGNORE, IGNORE, E}},
    {  2,  2,13,  "mov.b",	{RS8, RDDEC|B30, E},	{ 0x6, 0xc, RDDEC|B30, RS8, E}},
/* Put the 16 bit one in first so it matches first */
    {  4,  2,13,  "mov.b",	{RS8, ABS16DST, E},	{ 0x6, 0xa, 0x8, RS8, ABS16DST,IGNORE,IGNORE,IGNORE, E}},
    {  2,  2,13,  "mov.b",	{RS8, ABS8DST, E},	{ 0x3, RS8, ABS8DST,IGNORE, E}},
    {  2,  2,11,  "mov.w",	{RS16|B30, RD16|B30, E},{ 0x0, 0xd, RS16|B30, RD16|B30, E}},
    {  4,  2,11,  "mov.w",	{IMM16, RD16|B30, E},	{ 0x7, 0x9, 0x0, RD16|B30, IMM16,IGNORE,IGNORE,IGNORE, E}},
    {  2,  2,11,  "mov.w",	{RSIND|B30,RD16|B30, E},{ 0x6, 0x9, RSIND|B30, RD16|B30, E}},
    {  4,  2,11,  "mov.w",	{DISPSRC,RD16|B30, E},	{ 0x6, 0xf, DISPREG|B30, RD16|B30, DISPSRC, IGNORE, IGNORE, IGNORE,E}} ,
    {  2,  2,11,  "mov.w",	{RSINC|B30, RD16|B30, E},	{ 0x6, 0xd, RSINC|B30, RD16|B30, E}},
    {  4,  2,11,  "mov.w",	{ABS16SRC, RD16|B30, E},	{ 0x6, 0xb, 0x0, RD16|B30,ABS16SRC,IGNORE,IGNORE,IGNORE, E}},
    {  2,  2,11,  "mov.w", 	{RS16|B30, RDIND|B30, E},{ 0x6, 0x9, RDIND|B31, RS16|B30, E}},
    {  4,  2,11,  "mov.w",	{RS16|B30, DISPDST, E},	{ 0x6, 0xf, DISPREG|B31, RS16|B30,DISPDST, IGNORE,IGNORE,IGNORE,E}},
    {  2,  2,11,  "mov.w",	{RS16|B30, RDDEC|B30, E},{ 0x6, 0xd, RDDEC|B31, RS16|B30, E}},
    {  4,  2,11,  "mov.w",	{RS16|B30, ABS16DST, E},	{ 0x6, 0xb, 0x8, RS16|B30, ABS16DST, IGNORE, IGNORE, IGNORE, E}},


    {  4,  2,1,  "movfpe",	{ABS16SRC, RD8, E},	{ 0x6, 0xa, 0x4, RD8, ABS16SRC,IGNORE,IGNORE,IGNORE, E}},
    {  4,  2,1,  "movtpe",	{RS8, ABS16DST, E},	{ 0x6, 0xa, 0xc, RS8, ABS16DST,IGNORE,IGNORE,IGNORE, E}},
    {  2,  2,1,  "mulxu",	{RS8, RD16|B30, E},	{ 0x5, 0x0, RS8, RD16|B30, E}},
    {  2,  1,1,  "neg",	{RS8, E},		{ 0x1, 0x7, 0x8, RS8, E}},
    {  2,  0,1,  "nop",	{E},		{ 0x0, 0x0, 0x0, 0x0,E}},
    {  2,  1,1,  "not",	{RS8,E},		{ 0x1, 0x7, 0x0, RS8,E}},
  TWOOP("or", 0xc, 0x1, 0x4),
    {  2,  2,1,    "orc",	{IMM8, CCR,E},	{ 0x0, 0x4, IMM8,IGNORE,E}},
    {  2,  1,1,  "pop",	{RS16|B30,E},		{ 0x6, 0xd, 0x7, RS16|B30,E}},
    {  2,  1,1,  "push",	{RS16|B30,E},		{ 0x6, 0xd, 0xf, RS16|B30,E}},
  UNOP3(  "rotl",0x1, 0x2,0x8),
  UNOP3(  "rotr",0x1, 0x3, 0x8),
  UNOP3(  "rotxl",0x1, 0x2, 0x0),
  UNOP3(  "rotxr",0x1, 0x3, 0x0),
    {  2,  0, 1, "rte",	{E},		{ 0x5, 0x6, 0x7, 0x0,E}},
    {  2,  0, 1, "rts",	{E},		{ 0x5, 0x4, 0x7, 0x0,E}},
  UNOP3(   "shal",	0x1, 0x00, 0x08),
  UNOP3(    "shar",	0x1, 0x1, 0x8),
  UNOP3(    "shll",	0x1, 0x0, 0x0),
  UNOP3(    "shlr",	0x1, 0x1, 0x0),
    {  2,  0, 1, "sleep",	{E},		{ 0x0, 0x1, 0x8, 0x0,E}},
    {  2,  2, 1, "stc",	{CCR, RD8,E},	{ 0x0, 0x2, 0x0, RD8,E}},
    {  2,  2, 1, "sub.b",	{RS8,RD8,E},	{ 0x1, 0x8, RS8, RD8,E}},
    {  2,  2, 1, "sub.w",	{RS16|B30, RD16|B30,E}, 	{0x1, 0x9, RS16|B30, RD16|B30,E}},
    {  2,  2, 1, "subs",	{KBIT,RD16|B30,E},	{ 0x1, 0xb, KBIT, RD16|B30,E}},
  TWOOP("subx",0xb, 0x1, 0xe),
  TWOOP("xor", 0xd, 0x1, 0x5), 
    {  2,  2,  1,"xorc",	{IMM8, CCR,E}, { 0x0, 0x5, IMM8,IGNORE,E}},
    {  2,  0,1,  "bad 52",	{E, IMM8},		{ 0x5, 0x2, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 53",	{E, IMM8},		{ 0x5, 0x3, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 57",	{E, IMM8},		{ 0x5, 0x7, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 58",	{E, IMM8},		{ 0x5, 0x8, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 64",	{E, IMM8},		{ 0x6, 0x4, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 65",	{E, IMM8},		{ 0x6, 0x5, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 66",	{E, IMM8},		{ 0x6, 0x6, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 78",	{E, IMM8},		{ 0x7, 0x8, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 7a",	{E, IMM8},		{ 0x7, 0xa, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 5c",	{E, IMM8},		{ 0x5, 0xc, IMM8, IGNORE,E}},

  0
  }



#endif

;
