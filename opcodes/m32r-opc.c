/* CGEN opcode support for m32r.

Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

This file is part of the GNU Binutils and/or GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/


#include "sysdep.h"
#include <stdio.h>
#include "ansidecl.h"
#include "libiberty.h"
#include "bfd.h"
#include "m32r-opc.h"

/* Attributes.  */

static const CGEN_ATTR_ENTRY MACH_attr[] = 
{
  { "m32r", MACH_M32R },
  { 0, 0 }
};

const CGEN_ATTR_TABLE m32r_cgen_operand_attr_table[] = 
{
  { "ABS-ADDR", NULL },
  { "FAKE", NULL },
  { "NEGATIVE", NULL },
  { "PC", NULL },
  { "PCREL-ADDR", NULL },
  { "RELAX", NULL },
  { "RELOC", NULL },
  { "SIGN-OPT", NULL },
  { "UNSIGNED", NULL },
  { 0, 0 }
};

const CGEN_ATTR_TABLE m32r_cgen_insn_attr_table[] = 
{
  { "ALIAS", NULL },
  { "COND-CTI", NULL },
  { "FILL-SLOT", NULL },
  { "RELAX", NULL },
  { "RELAX-BC", NULL },
  { "RELAX-BL", NULL },
  { "RELAX-BNC", NULL },
  { "RELAX-BRA", NULL },
  { "RELAXABLE", NULL },
  { "UNCOND-CTI", NULL },
  { 0, 0 }
};

CGEN_KEYWORD_ENTRY m32r_cgen_opval_mach_entries[] = 
{
  { "m32r", MACH_M32R }
};

CGEN_KEYWORD m32r_cgen_opval_mach = 
{
  & m32r_cgen_opval_mach_entries[0],
  1
};

CGEN_KEYWORD_ENTRY m32r_cgen_opval_h_gr_entries[] = 
{
  { "fp", 13 },
  { "lr", 14 },
  { "sp", 15 },
  { "r0", 0 },
  { "r1", 1 },
  { "r2", 2 },
  { "r3", 3 },
  { "r4", 4 },
  { "r5", 5 },
  { "r6", 6 },
  { "r7", 7 },
  { "r8", 8 },
  { "r9", 9 },
  { "r10", 10 },
  { "r11", 11 },
  { "r12", 12 },
  { "r13", 13 },
  { "r14", 14 },
  { "r15", 15 }
};

CGEN_KEYWORD m32r_cgen_opval_h_gr = 
{
  & m32r_cgen_opval_h_gr_entries[0],
  19
};

CGEN_KEYWORD_ENTRY m32r_cgen_opval_h_cr_entries[] = 
{
  { "psw", 0 },
  { "cbr", 1 },
  { "spi", 2 },
  { "spu", 3 },
  { "bpc", 6 },
  { "cr0", 0 },
  { "cr1", 1 },
  { "cr2", 2 },
  { "cr3", 3 },
  { "cr4", 4 },
  { "cr5", 5 },
  { "cr6", 6 }
};

CGEN_KEYWORD m32r_cgen_opval_h_cr = 
{
  & m32r_cgen_opval_h_cr_entries[0],
  12
};


static CGEN_HW_ENTRY m32r_cgen_hw_entries[] = 
{
  { & m32r_cgen_hw_entries[1], "h-pc", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[2], "h-memory", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[3], "h-sint", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[4], "h-uint", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[5], "h-addr", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[6], "h-iaddr", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[7], "h-hi16", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[8], "h-slo16", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[9], "h-ulo16", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[10], "h-gr", CGEN_ASM_KEYWORD /*FIXME*/, & m32r_cgen_opval_h_gr },
  { & m32r_cgen_hw_entries[11], "h-cr", CGEN_ASM_KEYWORD /*FIXME*/, & m32r_cgen_opval_h_cr },
  { & m32r_cgen_hw_entries[12], "h-accum", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[13], "h-cond", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[14], "h-sm", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[15], "h-bsm", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[16], "h-ie", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[17], "h-bie", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[18], "h-bcond", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { NULL, "h-bpc", CGEN_ASM_KEYWORD /*FIXME*/, 0 }
};


const CGEN_OPERAND m32r_cgen_operand_table[CGEN_NUM_OPERANDS] =
{
/* pc: program counter */
  { "pc", 0, 0, { 0, 0|(1<<CGEN_OPERAND_FAKE)|(1<<CGEN_OPERAND_PC), { 0 } }  },
/* sr: source register */
  { "sr", 12, 4, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* dr: destination register */
  { "dr", 4, 4, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* src1: source register 1 */
  { "src1", 4, 4, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* src2: source register 2 */
  { "src2", 12, 4, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* scr: source control register */
  { "scr", 12, 4, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* dcr: destination control register */
  { "dcr", 4, 4, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* simm8: 8 bit signed immediate */
  { "simm8", 8, 8, { 0, 0, { 0 } }  },
/* simm16: 16 bit signed immediate */
  { "simm16", 16, 16, { 0, 0, { 0 } }  },
/* uimm4: 4 bit trap number */
  { "uimm4", 12, 4, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* uimm5: 5 bit shift count */
  { "uimm5", 11, 5, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* uimm16: 16 bit unsigned immediate */
  { "uimm16", 16, 16, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* hi16: high 16 bit immediate, sign optional */
  { "hi16", 16, 16, { 0, 0|(1<<CGEN_OPERAND_SIGN_OPT)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* slo16: 16 bit signed immediate, for low() */
  { "slo16", 16, 16, { 0, 0, { 0 } }  },
/* ulo16: 16 bit unsigned immediate, for low() */
  { "ulo16", 16, 16, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* uimm24: 24 bit address */
  { "uimm24", 8, 24, { 0, 0|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_ABS_ADDR)|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* disp8: 8 bit displacement */
  { "disp8", 8, 8, { 0, 0|(1<<CGEN_OPERAND_RELAX)|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), { 0 } }  },
/* disp16: 16 bit displacement */
  { "disp16", 16, 16, { 0, 0|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), { 0 } }  },
/* disp24: 24 bit displacement */
  { "disp24", 8, 24, { 0, 0|(1<<CGEN_OPERAND_RELAX)|(1<<CGEN_OPERAND_RELOC)|(1<<CGEN_OPERAND_PCREL_ADDR), { 0 } }  },
/* condbit: condition bit */
  { "condbit", 0, 0, { 0, 0|(1<<CGEN_OPERAND_FAKE), { 0 } }  },
/* accum: accumulator */
  { "accum", 0, 0, { 0, 0|(1<<CGEN_OPERAND_FAKE), { 0 } }  },
};

#define OP 1 /* syntax value for mnemonic */

static const CGEN_SYNTAX syntax_table[] =
{
/* <op> $dr,$sr */
/*   0 */  { OP, ' ', 130, ',', 129, 0 },
/* <op> $dr,$sr,$slo16 */
/*   1 */  { OP, ' ', 130, ',', 129, ',', 141, 0 },
/* <op> $dr,$sr,$uimm16 */
/*   2 */  { OP, ' ', 130, ',', 129, ',', 139, 0 },
/* <op> $dr,$sr,$ulo16 */
/*   3 */  { OP, ' ', 130, ',', 129, ',', 142, 0 },
/* <op> $dr,$simm8 */
/*   4 */  { OP, ' ', 130, ',', 135, 0 },
/* <op> $dr,$sr,$simm16 */
/*   5 */  { OP, ' ', 130, ',', 129, ',', 136, 0 },
/* <op> $disp8 */
/*   6 */  { OP, ' ', 144, 0 },
/* <op> $disp24 */
/*   7 */  { OP, ' ', 146, 0 },
/* <op> $src1,$src2,$disp16 */
/*   8 */  { OP, ' ', 131, ',', 132, ',', 145, 0 },
/* <op> $src2,$disp16 */
/*   9 */  { OP, ' ', 132, ',', 145, 0 },
/* <op> $src1,$src2 */
/*  10 */  { OP, ' ', 131, ',', 132, 0 },
/* <op> $src2,$simm16 */
/*  11 */  { OP, ' ', 132, ',', 136, 0 },
/* <op> $src2,$uimm16 */
/*  12 */  { OP, ' ', 132, ',', 139, 0 },
/* <op> $sr */
/*  13 */  { OP, ' ', 129, 0 },
/* <op> $dr,@$sr */
/*  14 */  { OP, ' ', 130, ',', '@', 129, 0 },
/* <op> $dr,@($sr) */
/*  15 */  { OP, ' ', 130, ',', '@', '(', 129, ')', 0 },
/* <op> $dr,@($slo16,$sr) */
/*  16 */  { OP, ' ', 130, ',', '@', '(', 141, ',', 129, ')', 0 },
/* <op> $dr,@($sr,$slo16) */
/*  17 */  { OP, ' ', 130, ',', '@', '(', 129, ',', 141, ')', 0 },
/* <op> $dr,@$sr+ */
/*  18 */  { OP, ' ', 130, ',', '@', 129, '+', 0 },
/* <op> $dr,$uimm24 */
/*  19 */  { OP, ' ', 130, ',', 143, 0 },
/* <op> $dr,$slo16 */
/*  20 */  { OP, ' ', 130, ',', 141, 0 },
/* <op> $dr */
/*  21 */  { OP, ' ', 130, 0 },
/* <op> $dr,$scr */
/*  22 */  { OP, ' ', 130, ',', 133, 0 },
/* <op> $src1 */
/*  23 */  { OP, ' ', 131, 0 },
/* <op> $sr,$dcr */
/*  24 */  { OP, ' ', 129, ',', 134, 0 },
/* <op> */
/*  25 */  { OP, 0 },
/* <op> $dr,$hi16 */
/*  26 */  { OP, ' ', 130, ',', 140, 0 },
/* <op> $dr,$uimm5 */
/*  27 */  { OP, ' ', 130, ',', 138, 0 },
/* <op> $src1,@$src2 */
/*  28 */  { OP, ' ', 131, ',', '@', 132, 0 },
/* <op> $src1,@($src2) */
/*  29 */  { OP, ' ', 131, ',', '@', '(', 132, ')', 0 },
/* <op> $src1,@($slo16,$src2) */
/*  30 */  { OP, ' ', 131, ',', '@', '(', 141, ',', 132, ')', 0 },
/* <op> $src1,@($src2,$slo16) */
/*  31 */  { OP, ' ', 131, ',', '@', '(', 132, ',', 141, ')', 0 },
/* <op> $src1,@+$src2 */
/*  32 */  { OP, ' ', 131, ',', '@', '+', 132, 0 },
/* <op> $src1,@-$src2 */
/*  33 */  { OP, ' ', 131, ',', '@', '-', 132, 0 },
/* <op> $uimm4 */
/*  34 */  { OP, ' ', 137, 0 },
};

#undef OP

static const CGEN_FORMAT format_table[] = 
{
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.sr. */
/*   0 */  { 16, 16, 0xf0f0 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.sr.f-simm16.slo16. */
/*   1 */  { 32, 32, 0xf0f00000 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.sr.f-uimm16.uimm16. */
/*   2 */  { 32, 32, 0xf0f00000 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.sr.f-uimm16.ulo16. */
/*   3 */  { 32, 32, 0xf0f00000 },
/* f-op1.number.f-r1.dr.f-simm8.simm8. */
/*   4 */  { 16, 16, 0xf000 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.sr.f-simm16.simm16. */
/*   5 */  { 32, 32, 0xf0f00000 },
/* f-op1.number.f-r1.number.f-disp8.disp8. */
/*   6 */  { 16, 16, 0xff00 },
/* f-op1.number.f-r1.number.f-disp24.disp24. */
/*   7 */  { 32, 32, 0xff000000 },
/* f-op1.number.f-r1.src1.f-op2.number.f-r2.src2.f-disp16.disp16. */
/*   8 */  { 32, 32, 0xf0f00000 },
/* f-op1.number.f-r1.number.f-op2.number.f-r2.src2.f-disp16.disp16. */
/*   9 */  { 32, 32, 0xfff00000 },
/* f-op1.number.f-r1.src1.f-op2.number.f-r2.src2. */
/*  10 */  { 16, 16, 0xf0f0 },
/* f-op1.number.f-r1.number.f-op2.number.f-r2.src2.f-simm16.simm16. */
/*  11 */  { 32, 32, 0xfff00000 },
/* f-op1.number.f-r1.number.f-op2.number.f-r2.src2.f-uimm16.uimm16. */
/*  12 */  { 32, 32, 0xfff00000 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.sr.f-simm16.number. */
/*  13 */  { 32, 32, 0xf0f0ffff },
/* f-op1.number.f-r1.number.f-op2.number.f-r2.sr. */
/*  14 */  { 16, 16, 0xfff0 },
/* f-op1.number.f-r1.dr.f-uimm24.uimm24. */
/*  15 */  { 32, 32, 0xf0000000 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.number.f-simm16.slo16. */
/*  16 */  { 32, 32, 0xf0ff0000 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.number. */
/*  17 */  { 16, 16, 0xf0ff },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.scr. */
/*  18 */  { 16, 16, 0xf0f0 },
/* f-op1.number.f-r1.src1.f-op2.number.f-r2.number. */
/*  19 */  { 16, 16, 0xf0ff },
/* f-op1.number.f-r1.dcr.f-op2.number.f-r2.sr. */
/*  20 */  { 16, 16, 0xf0f0 },
/* f-op1.number.f-r1.number.f-op2.number.f-r2.number. */
/*  21 */  { 16, 16, 0xffff },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.number.f-hi16.hi16. */
/*  22 */  { 32, 32, 0xf0ff0000 },
/* f-op1.number.f-r1.dr.f-shift-op2.number.f-uimm5.uimm5. */
/*  23 */  { 16, 16, 0xf0e0 },
/* f-op1.number.f-r1.src1.f-op2.number.f-r2.src2.f-simm16.slo16. */
/*  24 */  { 32, 32, 0xf0f00000 },
/* f-op1.number.f-r1.number.f-op2.number.f-uimm4.uimm4. */
/*  25 */  { 16, 16, 0xfff0 },
};

#define A(a) (1 << CGEN_CAT3 (CGEN_INSN,_,a))
#define SYN(n) (& syntax_table[n])
#define FMT(n) (& format_table[n])

const CGEN_INSN m32r_cgen_insn_table_entries[CGEN_NUM_INSNS] =
{
  /* null first entry, end of all hash chains */
  { { 0 }, 0 },
/* add $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "add", "add", SYN (0), FMT (0), 0xa0,
    { 0, 0, { 0 } }
  },
/* add3 $dr,$sr,$slo16 */
  {
    { 1, 1, 1, 1 },
    "add3", "add3", SYN (1), FMT (1), 0x80a00000,
    { 0, 0, { 0 } }
  },
/* and $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "and", "and", SYN (0), FMT (0), 0xc0,
    { 0, 0, { 0 } }
  },
/* and3 $dr,$sr,$uimm16 */
  {
    { 1, 1, 1, 1 },
    "and3", "and3", SYN (2), FMT (2), 0x80c00000,
    { 0, 0, { 0 } }
  },
/* or $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "or", "or", SYN (0), FMT (0), 0xe0,
    { 0, 0, { 0 } }
  },
/* or3 $dr,$sr,$ulo16 */
  {
    { 1, 1, 1, 1 },
    "or3", "or3", SYN (3), FMT (3), 0x80e00000,
    { 0, 0, { 0 } }
  },
/* xor $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "xor", "xor", SYN (0), FMT (0), 0xd0,
    { 0, 0, { 0 } }
  },
/* xor3 $dr,$sr,$uimm16 */
  {
    { 1, 1, 1, 1 },
    "xor3", "xor3", SYN (2), FMT (2), 0x80d00000,
    { 0, 0, { 0 } }
  },
/* addi $dr,$simm8 */
  {
    { 1, 1, 1, 1 },
    "addi", "addi", SYN (4), FMT (4), 0x4000,
    { 0, 0, { 0 } }
  },
/* addv $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "addv", "addv", SYN (0), FMT (0), 0x80,
    { 0, 0, { 0 } }
  },
/* addv3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    "addv3", "addv3", SYN (5), FMT (5), 0x80800000,
    { 0, 0, { 0 } }
  },
/* addx $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "addx", "addx", SYN (0), FMT (0), 0x90,
    { 0, 0, { 0 } }
  },
/* bc $disp8 */
  {
    { 1, 1, 1, 1 },
    "bc8", "bc", SYN (6), FMT (6), 0x7c00,
    { 0, 0|A(RELAX_BC)|A(RELAXABLE)|A(COND_CTI), { 0 } }
  },
/* bc.s $disp8 */
  {
    { 1, 1, 1, 1 },
    "bc8.s", "bc.s", SYN (6), FMT (6), 0x7c00,
    { 0, 0|A(ALIAS)|A(COND_CTI), { 0 } }
  },
/* bc $disp24 */
  {
    { 1, 1, 1, 1 },
    "bc24", "bc", SYN (7), FMT (7), 0xfc000000,
    { 0, 0|A(RELAX_BC)|A(RELAX)|A(COND_CTI), { 0 } }
  },
/* bc.l $disp24 */
  {
    { 1, 1, 1, 1 },
    "bc24.l", "bc.l", SYN (7), FMT (7), 0xfc000000,
    { 0, 0|A(ALIAS)|A(COND_CTI), { 0 } }
  },
/* beq $src1,$src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "beq", "beq", SYN (8), FMT (8), 0xb0000000,
    { 0, 0|A(COND_CTI), { 0 } }
  },
/* beqz $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "beqz", "beqz", SYN (9), FMT (9), 0xb0800000,
    { 0, 0|A(COND_CTI), { 0 } }
  },
/* bgez $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "bgez", "bgez", SYN (9), FMT (9), 0xb0b00000,
    { 0, 0|A(COND_CTI), { 0 } }
  },
/* bgtz $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "bgtz", "bgtz", SYN (9), FMT (9), 0xb0d00000,
    { 0, 0|A(COND_CTI), { 0 } }
  },
/* blez $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "blez", "blez", SYN (9), FMT (9), 0xb0c00000,
    { 0, 0|A(COND_CTI), { 0 } }
  },
/* bltz $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "bltz", "bltz", SYN (9), FMT (9), 0xb0a00000,
    { 0, 0|A(COND_CTI), { 0 } }
  },
/* bnez $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "bnez", "bnez", SYN (9), FMT (9), 0xb0900000,
    { 0, 0|A(COND_CTI), { 0 } }
  },
/* bl $disp8 */
  {
    { 1, 1, 1, 1 },
    "bl8", "bl", SYN (6), FMT (6), 0x7e00,
    { 0, 0|A(FILL_SLOT)|A(RELAX_BL)|A(RELAXABLE)|A(UNCOND_CTI), { 0 } }
  },
/* bl.s $disp8 */
  {
    { 1, 1, 1, 1 },
    "bl8.s", "bl.s", SYN (6), FMT (6), 0x7e00,
    { 0, 0|A(FILL_SLOT)|A(ALIAS)|A(UNCOND_CTI), { 0 } }
  },
/* bl $disp24 */
  {
    { 1, 1, 1, 1 },
    "bl24", "bl", SYN (7), FMT (7), 0xfe000000,
    { 0, 0|A(RELAX_BL)|A(RELAX)|A(UNCOND_CTI), { 0 } }
  },
/* bl.l $disp24 */
  {
    { 1, 1, 1, 1 },
    "bl24.l", "bl.l", SYN (7), FMT (7), 0xfe000000,
    { 0, 0|A(ALIAS)|A(UNCOND_CTI), { 0 } }
  },
/* bnc $disp8 */
  {
    { 1, 1, 1, 1 },
    "bnc8", "bnc", SYN (6), FMT (6), 0x7d00,
    { 0, 0|A(RELAX_BNC)|A(RELAXABLE)|A(COND_CTI), { 0 } }
  },
/* bnc.s $disp8 */
  {
    { 1, 1, 1, 1 },
    "bnc8.s", "bnc.s", SYN (6), FMT (6), 0x7d00,
    { 0, 0|A(ALIAS)|A(COND_CTI), { 0 } }
  },
/* bnc $disp24 */
  {
    { 1, 1, 1, 1 },
    "bnc24", "bnc", SYN (7), FMT (7), 0xfd000000,
    { 0, 0|A(RELAX_BNC)|A(RELAX)|A(COND_CTI), { 0 } }
  },
/* bnc.l $disp24 */
  {
    { 1, 1, 1, 1 },
    "bnc24.l", "bnc.l", SYN (7), FMT (7), 0xfd000000,
    { 0, 0|A(ALIAS)|A(COND_CTI), { 0 } }
  },
/* bne $src1,$src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "bne", "bne", SYN (8), FMT (8), 0xb0100000,
    { 0, 0|A(COND_CTI), { 0 } }
  },
/* bra $disp8 */
  {
    { 1, 1, 1, 1 },
    "bra8", "bra", SYN (6), FMT (6), 0x7f00,
    { 0, 0|A(RELAX_BRA)|A(RELAXABLE)|A(UNCOND_CTI), { 0 } }
  },
/* bra.s $disp8 */
  {
    { 1, 1, 1, 1 },
    "bra8.s", "bra.s", SYN (6), FMT (6), 0x7f00,
    { 0, 0|A(ALIAS)|A(UNCOND_CTI), { 0 } }
  },
/* bra $disp24 */
  {
    { 1, 1, 1, 1 },
    "bra24", "bra", SYN (7), FMT (7), 0xff000000,
    { 0, 0|A(RELAX_BRA)|A(RELAX)|A(UNCOND_CTI), { 0 } }
  },
/* bra.l $disp24 */
  {
    { 1, 1, 1, 1 },
    "bra24.l", "bra.l", SYN (7), FMT (7), 0xff000000,
    { 0, 0|A(ALIAS)|A(UNCOND_CTI), { 0 } }
  },
/* cmp $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "cmp", "cmp", SYN (10), FMT (10), 0x40,
    { 0, 0, { 0 } }
  },
/* cmpi $src2,$simm16 */
  {
    { 1, 1, 1, 1 },
    "cmpi", "cmpi", SYN (11), FMT (11), 0x80400000,
    { 0, 0, { 0 } }
  },
/* cmpu $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "cmpu", "cmpu", SYN (10), FMT (10), 0x50,
    { 0, 0, { 0 } }
  },
/* cmpui $src2,$uimm16 */
  {
    { 1, 1, 1, 1 },
    "cmpui", "cmpui", SYN (12), FMT (12), 0x80500000,
    { 0, 0, { 0 } }
  },
/* div $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "div", "div", SYN (0), FMT (13), 0x90000000,
    { 0, 0, { 0 } }
  },
/* divu $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "divu", "divu", SYN (0), FMT (13), 0x90100000,
    { 0, 0, { 0 } }
  },
/* rem $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "rem", "rem", SYN (0), FMT (13), 0x90200000,
    { 0, 0, { 0 } }
  },
/* remu $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "remu", "remu", SYN (0), FMT (13), 0x90300000,
    { 0, 0, { 0 } }
  },
/* jl $sr */
  {
    { 1, 1, 1, 1 },
    "jl", "jl", SYN (13), FMT (14), 0x1ec0,
    { 0, 0|A(FILL_SLOT)|A(UNCOND_CTI), { 0 } }
  },
/* jmp $sr */
  {
    { 1, 1, 1, 1 },
    "jmp", "jmp", SYN (13), FMT (14), 0x1fc0,
    { 0, 0|A(UNCOND_CTI), { 0 } }
  },
/* ld $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "ld", "ld", SYN (14), FMT (0), 0x20c0,
    { 0, 0, { 0 } }
  },
/* ld $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    "ld-2", "ld", SYN (15), FMT (0), 0x20c0,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* ld $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    "ld-d", "ld", SYN (16), FMT (1), 0xa0c00000,
    { 0, 0, { 0 } }
  },
/* ld $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    "ld-d2", "ld", SYN (17), FMT (1), 0xa0c00000,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* ldb $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "ldb", "ldb", SYN (14), FMT (0), 0x2080,
    { 0, 0, { 0 } }
  },
/* ldb $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    "ldb-2", "ldb", SYN (15), FMT (0), 0x2080,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* ldb $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    "ldb-d", "ldb", SYN (16), FMT (1), 0xa0800000,
    { 0, 0, { 0 } }
  },
/* ldb $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    "ldb-d2", "ldb", SYN (17), FMT (1), 0xa0800000,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* ldh $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "ldh", "ldh", SYN (14), FMT (0), 0x20a0,
    { 0, 0, { 0 } }
  },
/* ldh $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    "ldh-2", "ldh", SYN (15), FMT (0), 0x20a0,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* ldh $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    "ldh-d", "ldh", SYN (16), FMT (1), 0xa0a00000,
    { 0, 0, { 0 } }
  },
/* ldh $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    "ldh-d2", "ldh", SYN (17), FMT (1), 0xa0a00000,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* ldub $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "ldub", "ldub", SYN (14), FMT (0), 0x2090,
    { 0, 0, { 0 } }
  },
/* ldub $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    "ldub-2", "ldub", SYN (15), FMT (0), 0x2090,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* ldub $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    "ldub-d", "ldub", SYN (16), FMT (1), 0xa0900000,
    { 0, 0, { 0 } }
  },
/* ldub $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    "ldub-d2", "ldub", SYN (17), FMT (1), 0xa0900000,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* lduh $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "lduh", "lduh", SYN (14), FMT (0), 0x20b0,
    { 0, 0, { 0 } }
  },
/* lduh $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    "lduh-2", "lduh", SYN (15), FMT (0), 0x20b0,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* lduh $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    "lduh-d", "lduh", SYN (16), FMT (1), 0xa0b00000,
    { 0, 0, { 0 } }
  },
/* lduh $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    "lduh-d2", "lduh", SYN (17), FMT (1), 0xa0b00000,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* ld $dr,@$sr+ */
  {
    { 1, 1, 1, 1 },
    "ld-plus", "ld", SYN (18), FMT (0), 0x20e0,
    { 0, 0, { 0 } }
  },
/* ld24 $dr,$uimm24 */
  {
    { 1, 1, 1, 1 },
    "ld24", "ld24", SYN (19), FMT (15), 0xe0000000,
    { 0, 0, { 0 } }
  },
/* ldi $dr,$simm8 */
  {
    { 1, 1, 1, 1 },
    "ldi8", "ldi", SYN (4), FMT (4), 0x6000,
    { 0, 0, { 0 } }
  },
/* ldi8 $dr,$simm8 */
  {
    { 1, 1, 1, 1 },
    "ldi8a", "ldi8", SYN (4), FMT (4), 0x6000,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* ldi $dr,$slo16 */
  {
    { 1, 1, 1, 1 },
    "ldi16", "ldi", SYN (20), FMT (16), 0x90f00000,
    { 0, 0, { 0 } }
  },
/* ldi16 $dr,$slo16 */
  {
    { 1, 1, 1, 1 },
    "ldi16a", "ldi16", SYN (20), FMT (16), 0x90f00000,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* lock $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "lock", "lock", SYN (14), FMT (0), 0x20d0,
    { 0, 0, { 0 } }
  },
/* machi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "machi", "machi", SYN (10), FMT (10), 0x3040,
    { 0, 0, { 0 } }
  },
/* maclo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "maclo", "maclo", SYN (10), FMT (10), 0x3050,
    { 0, 0, { 0 } }
  },
/* macwhi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "macwhi", "macwhi", SYN (10), FMT (10), 0x3060,
    { 0, 0, { 0 } }
  },
/* macwlo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "macwlo", "macwlo", SYN (10), FMT (10), 0x3070,
    { 0, 0, { 0 } }
  },
/* mul $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "mul", "mul", SYN (0), FMT (0), 0x1060,
    { 0, 0, { 0 } }
  },
/* mulhi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "mulhi", "mulhi", SYN (10), FMT (10), 0x3000,
    { 0, 0, { 0 } }
  },
/* mullo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "mullo", "mullo", SYN (10), FMT (10), 0x3010,
    { 0, 0, { 0 } }
  },
/* mulwhi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "mulwhi", "mulwhi", SYN (10), FMT (10), 0x3020,
    { 0, 0, { 0 } }
  },
/* mulwlo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "mulwlo", "mulwlo", SYN (10), FMT (10), 0x3030,
    { 0, 0, { 0 } }
  },
/* mv $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "mv", "mv", SYN (0), FMT (0), 0x1080,
    { 0, 0, { 0 } }
  },
/* mvfachi $dr */
  {
    { 1, 1, 1, 1 },
    "mvfachi", "mvfachi", SYN (21), FMT (17), 0x50f0,
    { 0, 0, { 0 } }
  },
/* mvfaclo $dr */
  {
    { 1, 1, 1, 1 },
    "mvfaclo", "mvfaclo", SYN (21), FMT (17), 0x50f1,
    { 0, 0, { 0 } }
  },
/* mvfacmi $dr */
  {
    { 1, 1, 1, 1 },
    "mvfacmi", "mvfacmi", SYN (21), FMT (17), 0x50f2,
    { 0, 0, { 0 } }
  },
/* mvfc $dr,$scr */
  {
    { 1, 1, 1, 1 },
    "mvfc", "mvfc", SYN (22), FMT (18), 0x1090,
    { 0, 0, { 0 } }
  },
/* mvtachi $src1 */
  {
    { 1, 1, 1, 1 },
    "mvtachi", "mvtachi", SYN (23), FMT (19), 0x5070,
    { 0, 0, { 0 } }
  },
/* mvtaclo $src1 */
  {
    { 1, 1, 1, 1 },
    "mvtaclo", "mvtaclo", SYN (23), FMT (19), 0x5071,
    { 0, 0, { 0 } }
  },
/* mvtc $sr,$dcr */
  {
    { 1, 1, 1, 1 },
    "mvtc", "mvtc", SYN (24), FMT (20), 0x10a0,
    { 0, 0, { 0 } }
  },
/* neg $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "neg", "neg", SYN (0), FMT (0), 0x30,
    { 0, 0, { 0 } }
  },
/* nop */
  {
    { 1, 1, 1, 1 },
    "nop", "nop", SYN (25), FMT (21), 0x7000,
    { 0, 0, { 0 } }
  },
/* not $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "not", "not", SYN (0), FMT (0), 0xb0,
    { 0, 0, { 0 } }
  },
/* rac */
  {
    { 1, 1, 1, 1 },
    "rac", "rac", SYN (25), FMT (21), 0x5090,
    { 0, 0, { 0 } }
  },
/* rach */
  {
    { 1, 1, 1, 1 },
    "rach", "rach", SYN (25), FMT (21), 0x5080,
    { 0, 0, { 0 } }
  },
/* rte */
  {
    { 1, 1, 1, 1 },
    "rte", "rte", SYN (25), FMT (21), 0x10d6,
    { 0, 0|A(UNCOND_CTI), { 0 } }
  },
/* seth $dr,$hi16 */
  {
    { 1, 1, 1, 1 },
    "seth", "seth", SYN (26), FMT (22), 0xd0c00000,
    { 0, 0, { 0 } }
  },
/* sll $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "sll", "sll", SYN (0), FMT (0), 0x1040,
    { 0, 0, { 0 } }
  },
/* sll3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    "sll3", "sll3", SYN (5), FMT (5), 0x90c00000,
    { 0, 0, { 0 } }
  },
/* slli $dr,$uimm5 */
  {
    { 1, 1, 1, 1 },
    "slli", "slli", SYN (27), FMT (23), 0x5040,
    { 0, 0, { 0 } }
  },
/* sra $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "sra", "sra", SYN (0), FMT (0), 0x1020,
    { 0, 0, { 0 } }
  },
/* sra3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    "sra3", "sra3", SYN (5), FMT (5), 0x90a00000,
    { 0, 0, { 0 } }
  },
/* srai $dr,$uimm5 */
  {
    { 1, 1, 1, 1 },
    "srai", "srai", SYN (27), FMT (23), 0x5020,
    { 0, 0, { 0 } }
  },
/* srl $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "srl", "srl", SYN (0), FMT (0), 0x1000,
    { 0, 0, { 0 } }
  },
/* srl3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    "srl3", "srl3", SYN (5), FMT (5), 0x90800000,
    { 0, 0, { 0 } }
  },
/* srli $dr,$uimm5 */
  {
    { 1, 1, 1, 1 },
    "srli", "srli", SYN (27), FMT (23), 0x5000,
    { 0, 0, { 0 } }
  },
/* st $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    "st", "st", SYN (28), FMT (10), 0x2040,
    { 0, 0, { 0 } }
  },
/* st $src1,@($src2) */
  {
    { 1, 1, 1, 1 },
    "st-2", "st", SYN (29), FMT (10), 0x2040,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* st $src1,@($slo16,$src2) */
  {
    { 1, 1, 1, 1 },
    "st-d", "st", SYN (30), FMT (24), 0xa0400000,
    { 0, 0, { 0 } }
  },
/* st $src1,@($src2,$slo16) */
  {
    { 1, 1, 1, 1 },
    "st-d2", "st", SYN (31), FMT (24), 0xa0400000,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* stb $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    "stb", "stb", SYN (28), FMT (10), 0x2000,
    { 0, 0, { 0 } }
  },
/* stb $src1,@($src2) */
  {
    { 1, 1, 1, 1 },
    "stb-2", "stb", SYN (29), FMT (10), 0x2000,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* stb $src1,@($slo16,$src2) */
  {
    { 1, 1, 1, 1 },
    "stb-d", "stb", SYN (30), FMT (24), 0xa0000000,
    { 0, 0, { 0 } }
  },
/* stb $src1,@($src2,$slo16) */
  {
    { 1, 1, 1, 1 },
    "stb-d2", "stb", SYN (31), FMT (24), 0xa0000000,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* sth $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    "sth", "sth", SYN (28), FMT (10), 0x2020,
    { 0, 0, { 0 } }
  },
/* sth $src1,@($src2) */
  {
    { 1, 1, 1, 1 },
    "sth-2", "sth", SYN (29), FMT (10), 0x2020,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* sth $src1,@($slo16,$src2) */
  {
    { 1, 1, 1, 1 },
    "sth-d", "sth", SYN (30), FMT (24), 0xa0200000,
    { 0, 0, { 0 } }
  },
/* sth $src1,@($src2,$slo16) */
  {
    { 1, 1, 1, 1 },
    "sth-d2", "sth", SYN (31), FMT (24), 0xa0200000,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* st $src1,@+$src2 */
  {
    { 1, 1, 1, 1 },
    "st-plus", "st", SYN (32), FMT (10), 0x2060,
    { 0, 0, { 0 } }
  },
/* st $src1,@-$src2 */
  {
    { 1, 1, 1, 1 },
    "st-minus", "st", SYN (33), FMT (10), 0x2070,
    { 0, 0, { 0 } }
  },
/* sub $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "sub", "sub", SYN (0), FMT (0), 0x20,
    { 0, 0, { 0 } }
  },
/* subv $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "subv", "subv", SYN (0), FMT (0), 0x0,
    { 0, 0, { 0 } }
  },
/* subx $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "subx", "subx", SYN (0), FMT (0), 0x10,
    { 0, 0, { 0 } }
  },
/* trap $uimm4 */
  {
    { 1, 1, 1, 1 },
    "trap", "trap", SYN (34), FMT (25), 0x10f0,
    { 0, 0|A(FILL_SLOT)|A(UNCOND_CTI), { 0 } }
  },
/* unlock $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    "unlock", "unlock", SYN (28), FMT (10), 0x2050,
    { 0, 0, { 0 } }
  },
/* push $src1 */
  {
    { 1, 1, 1, 1 },
    "push", "push", SYN (23), FMT (19), 0x207f,
    { 0, 0|A(ALIAS), { 0 } }
  },
/* pop $dr */
  {
    { 1, 1, 1, 1 },
    "pop", "pop", SYN (21), FMT (17), 0x20ef,
    { 0, 0|A(ALIAS), { 0 } }
  },
};

#undef A
#undef SYN
#undef FMT

CGEN_INSN_TABLE m32r_cgen_insn_table =
{
  & m32r_cgen_insn_table_entries[0],
  sizeof (CGEN_INSN),
  CGEN_NUM_INSNS,
  NULL,
  m32r_cgen_asm_hash_insn, CGEN_ASM_HASH_SIZE,
  m32r_cgen_dis_hash_insn, CGEN_DIS_HASH_SIZE
};

/* The hash functions are recorded here to help keep assembler code out of
   the disassembler and vice versa.  */

unsigned int
m32r_cgen_asm_hash_insn (insn)
     const char * insn;
{
  return CGEN_ASM_HASH (insn);
}

unsigned int
m32r_cgen_dis_hash_insn (buf, value)
     const char * buf;
     unsigned long value;
{
  return CGEN_DIS_HASH (buf, value);
}

CGEN_OPCODE_DATA m32r_cgen_opcode_data = 
{
  & m32r_cgen_hw_entries[0],
  & m32r_cgen_insn_table,
};

void
m32r_cgen_init_tables (mach)
    int mach;
{
}

/* Main entry point for stuffing values in cgen_fields.  */

CGEN_INLINE void
m32r_cgen_set_operand (opindex, valuep, fields)
     int opindex;
     const long * valuep;
     CGEN_FIELDS * fields;
{
  switch (opindex)
    {
    case M32R_OPERAND_SR :
      fields->f_r2 = * valuep;
      break;
    case M32R_OPERAND_DR :
      fields->f_r1 = * valuep;
      break;
    case M32R_OPERAND_SRC1 :
      fields->f_r1 = * valuep;
      break;
    case M32R_OPERAND_SRC2 :
      fields->f_r2 = * valuep;
      break;
    case M32R_OPERAND_SCR :
      fields->f_r2 = * valuep;
      break;
    case M32R_OPERAND_DCR :
      fields->f_r1 = * valuep;
      break;
    case M32R_OPERAND_SIMM8 :
      fields->f_simm8 = * valuep;
      break;
    case M32R_OPERAND_SIMM16 :
      fields->f_simm16 = * valuep;
      break;
    case M32R_OPERAND_UIMM4 :
      fields->f_uimm4 = * valuep;
      break;
    case M32R_OPERAND_UIMM5 :
      fields->f_uimm5 = * valuep;
      break;
    case M32R_OPERAND_UIMM16 :
      fields->f_uimm16 = * valuep;
      break;
    case M32R_OPERAND_HI16 :
      fields->f_hi16 = * valuep;
      break;
    case M32R_OPERAND_SLO16 :
      fields->f_simm16 = * valuep;
      break;
    case M32R_OPERAND_ULO16 :
      fields->f_uimm16 = * valuep;
      break;
    case M32R_OPERAND_UIMM24 :
      fields->f_uimm24 = * valuep;
      break;
    case M32R_OPERAND_DISP8 :
      fields->f_disp8 = * valuep;
      break;
    case M32R_OPERAND_DISP16 :
      fields->f_disp16 = * valuep;
      break;
    case M32R_OPERAND_DISP24 :
      fields->f_disp24 = * valuep;
      break;

    default :
      fprintf (stderr, "Unrecognized field %d while setting operand.\n",
		       opindex);
      abort ();
  }
}

/* Main entry point for getting values from cgen_fields.  */

CGEN_INLINE long
m32r_cgen_get_operand (opindex, fields)
     int opindex;
     const CGEN_FIELDS * fields;
{
  long value;

  switch (opindex)
    {
    case M32R_OPERAND_SR :
      value = fields->f_r2;
      break;
    case M32R_OPERAND_DR :
      value = fields->f_r1;
      break;
    case M32R_OPERAND_SRC1 :
      value = fields->f_r1;
      break;
    case M32R_OPERAND_SRC2 :
      value = fields->f_r2;
      break;
    case M32R_OPERAND_SCR :
      value = fields->f_r2;
      break;
    case M32R_OPERAND_DCR :
      value = fields->f_r1;
      break;
    case M32R_OPERAND_SIMM8 :
      value = fields->f_simm8;
      break;
    case M32R_OPERAND_SIMM16 :
      value = fields->f_simm16;
      break;
    case M32R_OPERAND_UIMM4 :
      value = fields->f_uimm4;
      break;
    case M32R_OPERAND_UIMM5 :
      value = fields->f_uimm5;
      break;
    case M32R_OPERAND_UIMM16 :
      value = fields->f_uimm16;
      break;
    case M32R_OPERAND_HI16 :
      value = fields->f_hi16;
      break;
    case M32R_OPERAND_SLO16 :
      value = fields->f_simm16;
      break;
    case M32R_OPERAND_ULO16 :
      value = fields->f_uimm16;
      break;
    case M32R_OPERAND_UIMM24 :
      value = fields->f_uimm24;
      break;
    case M32R_OPERAND_DISP8 :
      value = fields->f_disp8;
      break;
    case M32R_OPERAND_DISP16 :
      value = fields->f_disp16;
      break;
    case M32R_OPERAND_DISP24 :
      value = fields->f_disp24;
      break;

    default :
      fprintf (stderr, "Unrecognized field %d while getting operand.\n",
		       opindex);
      abort ();
  }

  return value;
}

