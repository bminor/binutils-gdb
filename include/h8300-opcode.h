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
{   2,  1, 1,name,{DISP8,E}, { Hex4, op, DISP8,IGNORE,E }}

#define TWOOP(name, op1, op2,op3) \
{ 2,  2, 2, name, {IMM8, RD8,E},	{ op1, RD8, IMM8,IGNORE,E}},\
{ 2,  2, 2, name, {RS8, RD8, E},	{ op2, op3, RS8, RD8 ,E}} 

#define UNOP(name, op1, op2) \
{ 2, 1, 1, name, {RS8, E}, { op1, op2, 0, RS8, E}}

#define UNOP3(name, op1, op2, op3) \
{2, 1, 1, name , {RS8, E}, {op1, op2, op3, RS8, E}}

#define WTWOP(name, op1, op2) \
{2, 2, 1, name, {RS16, RD16, E}, { op1, op2, RS16, RD16, E}}

= 
{
  TWOOP("add.b", Hex8,	Hex0,Hex8),
  WTWOP("add.w", Hex0, Hex9),
    { 2,  2, 1, "adds", {KBIT,RD16|B30, E},{Hex0, HexB, KBIT, RD16|B30, E}},
  TWOOP("addx", Hex9,Hex0,HexE),
  TWOOP("and", HexE,Hex1,Hex6),
    {  2,  2, 1, "andc",	{IMM8, CCR, E},	{ Hex0, Hex6, IMM8,IGNORE, E}},
  BITOP(3,IMM3|B30, "band", Hex7, Hex6, Hex7, HexC,  Hex7, HexE),
  BRANCH("bra",	Hex0),
  BRANCH("bt",	Hex0),
  BRANCH("brn",	Hex1),
  BRANCH("bf",	Hex1),
  BRANCH("bhi",	Hex2),
  BRANCH("bls",	Hex3),
  BRANCH("bcc",	Hex4),
  BRANCH("bhs",	Hex4),
  BRANCH("bcs",	Hex5),
  BRANCH("blo",	Hex5),
  BRANCH("bne",	Hex6),
  BRANCH("beq",	Hex7),
  BRANCH("bvc",	Hex8),
  BRANCH("bvs",	Hex9),
  BRANCH("bpl",	HexA),
  BRANCH("bmi",	HexB),
  BRANCH("bge",	HexC),
  BRANCH("blt",	HexD),
  BRANCH("bgt",	HexE),
  BRANCH("ble",	HexF),
  EBITOP(6,IMM3|B30,"bclr", Hex6, Hex2, Hex7, HexD, Hex7, HexF),
  BITOP(3,IMM3|B31,"biand", Hex7, Hex6, Hex7, HexC, Hex7, HexE),
  BITOP(3,IMM3|B31, "bild",	Hex7, Hex7,Hex7, HexC, Hex7, HexE),
  BITOP(3,IMM3|B31, "bior",	Hex7, Hex4,Hex7, HexC, Hex7, HexE),
  BITOP(3,IMM3|B31, "bist",	Hex6, Hex7,Hex7, HexD, Hex7, HexE),
  BITOP(3,IMM3|B31, "bixor",	Hex7, Hex5,Hex7, HexC, Hex7, HexE),
  BITOP(3,IMM3|B30, "bld",	Hex7, Hex7,Hex7, HexC, Hex7, HexE),
  EBITOP(6,IMM3|B30,"bnot", Hex6, Hex1, Hex7, HexD, Hex7, HexF),
  BITOP(3,IMM3|B30,"bor",	Hex7, Hex4,Hex7, HexC, Hex7, HexE),
  EBITOP(6,IMM3|B30,"bset",	Hex6, Hex0,Hex7, HexD, Hex7, HexF),
    {  2,  1, 1, "bsr",{DISP8, E},{ Hex5, Hex5, DISP8,IGNORE, E}},
  BITOP(3,IMM3|B30, "bst",	Hex6, Hex7,Hex7, HexD, Hex7, HexF),
  EBITOP(6,IMM3|B30, "btst",	Hex6, Hex3,Hex7, HexC, Hex7, HexE),
  BITOP(3,IMM3|B30, "bxor",	Hex7,Hex5,Hex7, HexC, Hex7, HexE),
  TWOOP( "cmp.b",HexA, Hex1, HexC),
  WTWOP( "cmp.w",Hex1,HexD),
  UNOP(   "daa",Hex0, HexF),
  UNOP(   "das",Hex1, HexF),
  UNOP(   "dec",Hex1, HexA),
    {  2,  2, 1, "divxu",{RS8, RD16|B30, E}, { Hex5, Hex1, RS8, RD16|B30, E}},
    {  4,  0, 1, "eepmov",{ E}, {Hex7, HexB, Hex5, HexC, Hex5, Hex9, Hex8, HexF,E}},
  UNOP(   "inc",	Hex0, HexA),
    {  2,  1, 3, "jmp",{RSIND|B30, E}, {Hex5, Hex9, RSIND|B30, Hex0, E}},
    {  4,  1, 3, "jmp",{ABS16SRC, E}, {Hex5, HexA, Hex0, Hex0, ABS16SRC, IGNORE,IGNORE,IGNORE,E}},
    {  2,  1, 3, "jmp",{MEMIND, E}, {Hex5, HexB, MEMIND,IGNORE, E}},
    {  2,  1, 3, "jsr",{RSIND|B30, E}, {Hex5, HexD, RSIND|B30, Hex0, E}},
    {  4,  1, 3, "jsr",{ABS16SRC, E}, {Hex5, HexE, Hex0, Hex0, ABS16SRC,IGNORE,IGNORE,IGNORE, E}},
    {  2,  1, 3, "jsr",{MEMIND, E}, {Hex5, HexF, MEMIND, IGNORE,E}},
    {  2,  2, 2, "ldc",	{IMM8, CCR, E}, { Hex0, Hex7, IMM8,IGNORE, E}},
    {  2,  2, 2, "ldc",	{RS8, CCR, E},	{ Hex0, Hex3, Hex0, RS8, E}},

    {  2,  2,13,  "mov.b",	{RS8, RD8, E},	{ Hex0, HexC, RS8, RD8, E}},
    {  2,  2,13,  "mov.b",	{IMM8, RD8, E},	{ HexF, RD8, IMM8,IGNORE, E}},
    {  2,  2,13,  "mov.b",	{RSIND|B30,RD8, E},	{ Hex6, Hex8, RSIND|B30, RD8, E}},
    {  4,  2,13,  "mov.b",	{DISPSRC,RD8, E},	{ Hex6, HexE, DISPREG|B30, RD8, DISPSRC, IGNORE, IGNORE, IGNORE, E}} ,
    {  2,  2,13,  "mov.b",	{RSINC|B30, RD8, E},	{ Hex6, HexC, RSINC|B30, RD8, E}},

    {  4,  2,13,  "mov.b",	{ABS16SRC, RD8, E},	{ Hex6, HexA, Hex0, RD8,ABS16SRC, IGNORE,IGNORE,IGNORE,E}},
    {  2,  2,13,  "mov.b",	{ABS8SRC, RD8, E},	{ Hex2, RD8, ABS8SRC,IGNORE, E}},
    {  2,  2,13,  "mov.b", 	{RS8, RDIND|B30, E},	{ Hex6, Hex8, RDIND|B31, RS8, E}},
    {  4,  2,13,  "mov.b",	{RS8, DISPDST, E},	{ Hex6, HexE, DISPREG|B31, RS8,DISPDST, IGNORE, IGNORE, IGNORE, E}},
    {  2,  2,13,  "mov.b",	{RS8, RDDEC|B30, E},	{ Hex6, HexC, RDDEC|B30, RS8, E}},
/* Put the 16 bit one in first so it matches first */
    {  4,  2,13,  "mov.b",	{RS8, ABS16DST, E},	{ Hex6, HexA, Hex8, RS8, ABS16DST,IGNORE,IGNORE,IGNORE, E}},
    {  2,  2,13,  "mov.b",	{RS8, ABS8DST, E},	{ Hex3, RS8, ABS8DST,IGNORE, E}},
    {  2,  2,11,  "mov.w",	{RS16|B30, RD16|B30, E},{ Hex0, HexD, RS16|B30, RD16|B30, E}},
    {  4,  2,11,  "mov.w",	{IMM16, RD16|B30, E},	{ Hex7, Hex9, Hex0, RD16|B30, IMM16,IGNORE,IGNORE,IGNORE, E}},
    {  2,  2,11,  "mov.w",	{RSIND|B30,RD16|B30, E},{ Hex6, Hex9, RSIND|B30, RD16|B30, E}},
    {  4,  2,11,  "mov.w",	{DISPSRC,RD16|B30, E},	{ Hex6, HexF, DISPREG|B30, RD16|B30, DISPSRC, IGNORE, IGNORE, IGNORE,E}} ,
    {  2,  2,11,  "mov.w",	{RSINC|B30, RD16|B30, E},	{ Hex6, HexD, RSINC|B30, RD16|B30, E}},
    {  4,  2,11,  "mov.w",	{ABS16SRC, RD16|B30, E},	{ Hex6, HexB, Hex0, RD16|B30,ABS16SRC,IGNORE,IGNORE,IGNORE, E}},
    {  2,  2,11,  "mov.w", 	{RS16|B30, RDIND|B30, E},{ Hex6, Hex9, RDIND|B31, RS16|B30, E}},
    {  4,  2,11,  "mov.w",	{RS16|B30, DISPDST, E},	{ Hex6, HexF, DISPREG|B31, RS16|B30,DISPDST, IGNORE,IGNORE,IGNORE,E}},
    {  2,  2,11,  "mov.w",	{RS16|B30, RDDEC|B30, E},{ Hex6, HexD, RDDEC|B31, RS16|B30, E}},
    {  4,  2,11,  "mov.w",	{RS16|B30, ABS16DST, E},	{ Hex6, HexB, Hex8, RS16|B30, ABS16DST, IGNORE, IGNORE, IGNORE, E}},


    {  4,  2,1,  "movfpe",	{ABS16SRC, RD8, E},	{ Hex6, HexA, Hex4, RD8, ABS16SRC,IGNORE,IGNORE,IGNORE, E}},
    {  4,  2,1,  "movtpe",	{RS8, ABS16DST, E},	{ Hex6, HexA, HexC, RS8, ABS16DST,IGNORE,IGNORE,IGNORE, E}},
    {  2,  2,1,  "mulxu",	{RS8, RD16|B30, E},	{ Hex5, Hex0, RS8, RD16|B30, E}},
    {  2,  1,1,  "neg",	{RS8, E},		{ Hex1, Hex7, Hex8, RS8, E}},
    {  2,  0,1,  "nop",	{E},		{ Hex0, Hex0, Hex0, Hex0,E}},
    {  2,  1,1,  "not",	{RS8,E},		{ Hex1, Hex7, Hex0, RS8,E}},
  TWOOP("or", HexC, Hex1, Hex4),
    {  2,  2,1,    "orc",	{IMM8, CCR,E},	{ Hex0, Hex4, IMM8,IGNORE,E}},
    {  2,  1,1,  "pop",	{RS16|B30,E},		{ Hex6, HexD, Hex7, RS16|B30,E}},
    {  2,  1,1,  "push",	{RS16|B30,E},		{ Hex6, HexD, HexF, RS16|B30,E}},
  UNOP3(  "rotl",Hex1, Hex2,Hex8),
  UNOP3(  "rotr",Hex1, Hex3, Hex8),
  UNOP3(  "rotxl",Hex1, Hex2, Hex0),
  UNOP3(  "rotxr",Hex1, Hex3, Hex0),
    {  2,  0, 1, "rte",	{E},		{ Hex5, Hex6, Hex7, Hex0,E}},
    {  2,  0, 1, "rts",	{E},		{ Hex5, Hex4, Hex7, Hex0,E}},
  UNOP3(   "shal",	Hex1, Hex0, Hex8),
  UNOP3(    "shar",	Hex1, Hex1, Hex8),
  UNOP3(    "shll",	Hex1, Hex0, Hex0),
  UNOP3(    "shlr",	Hex1, Hex1, Hex0),
    {  2,  0, 1, "sleep",	{E},		{ Hex0, Hex1, Hex8, Hex0,E}},
    {  2,  2, 1, "stc",	{CCR, RD8,E},	{ Hex0, Hex2, Hex0, RD8,E}},
    {  2,  2, 1, "sub.b",	{RS8,RD8,E},	{ Hex1, Hex8, RS8, RD8,E}},
    {  2,  2, 1, "sub.w",	{RS16|B30, RD16|B30,E}, 	{Hex1, Hex9, RS16|B30, RD16|B30,E}},
    {  2,  2, 1, "subs",	{KBIT,RD16|B30,E},	{ Hex1, HexB, KBIT, RD16|B30,E}},
  TWOOP("subx",HexB, Hex1, HexE),
  TWOOP("xor", HexD, Hex1, Hex5), 
    {  2,  2,  1,"xorc",	{IMM8, CCR,E}, { Hex0, Hex5, IMM8,IGNORE,E}},
    {  2,  0,1,  "bad 52",	{E, IMM8},		{ Hex5, Hex2, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 53",	{E, IMM8},		{ Hex5, Hex3, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 57",	{E, IMM8},		{ Hex5, Hex7, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 58",	{E, IMM8},		{ Hex5, Hex8, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 64",	{E, IMM8},		{ Hex6, Hex4, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 65",	{E, IMM8},		{ Hex6, Hex5, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 66",	{E, IMM8},		{ Hex6, Hex6, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 78",	{E, IMM8},		{ Hex7, Hex8, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 7a",	{E, IMM8},		{ Hex7, HexA, IMM8, IGNORE,E}},
    {  2,  0,1,  "bad 5c",	{E, IMM8},		{ Hex5, HexC, IMM8, IGNORE,E}},

  0
  }



#endif

;
