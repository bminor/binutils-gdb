/* CGEN opcode support for m32r.

This file is machine generated.

Copyright (C) 1996, 1997 Free Software Foundation, Inc.

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

static const CGEN_ATTR_ENTRY MACH_attr[] = {
  { "m32r", MACH_M32R },
  { "m32rx", MACH_M32RX },
  { 0, 0 }
};

static const CGEN_ATTR_ENTRY PIPE_attr[] = {
  { "NONE", PIPE_NONE },
  { "O", PIPE_O },
  { "S", PIPE_S },
  { "OS", PIPE_OS },
  { 0, 0 }
};

const CGEN_ATTR_TABLE m32r_cgen_operand_attr_table[] = {
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

const CGEN_ATTR_TABLE m32r_cgen_insn_attr_table[] = {
  { "MACH", & MACH_attr[0] },
  { "PIPE", & PIPE_attr[0] },
  { "ALIAS", NULL },
  { "COND-CTI", NULL },
  { "FILL-SLOT", NULL },
  { "PARALLEL", NULL },
  { "RELAX", NULL },
  { "RELAX-BC", NULL },
  { "RELAX-BCL", NULL },
  { "RELAX-BL", NULL },
  { "RELAX-BNC", NULL },
  { "RELAX-BNCL", NULL },
  { "RELAX-BRA", NULL },
  { "RELAXABLE", NULL },
  { "UNCOND-CTI", NULL },
  { 0, 0 }
};

CGEN_KEYWORD_ENTRY m32r_cgen_opval_mach_entries[] = {
  { "m32r", MACH_M32R },
  { "m32rx", MACH_M32RX }
};

CGEN_KEYWORD m32r_cgen_opval_mach = {
  & m32r_cgen_opval_mach_entries[0],
  2
};

CGEN_KEYWORD_ENTRY m32r_cgen_opval_h_gr_entries[] = {
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

CGEN_KEYWORD m32r_cgen_opval_h_gr = {
  & m32r_cgen_opval_h_gr_entries[0],
  19
};

CGEN_KEYWORD_ENTRY m32r_cgen_opval_h_cr_entries[] = {
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

CGEN_KEYWORD m32r_cgen_opval_h_cr = {
  & m32r_cgen_opval_h_cr_entries[0],
  12
};

CGEN_KEYWORD_ENTRY m32r_cgen_opval_h_accums_entries[] = {
  { "a0", 0 },
  { "a1", 1 }
};

CGEN_KEYWORD m32r_cgen_opval_h_accums = {
  & m32r_cgen_opval_h_accums_entries[0],
  2
};


static CGEN_HW_ENTRY m32r_cgen_hw_entries[] = {
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
  { & m32r_cgen_hw_entries[13], "h-accums", CGEN_ASM_KEYWORD /*FIXME*/, & m32r_cgen_opval_h_accums },
  { & m32r_cgen_hw_entries[14], "h-abort", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[15], "h-cond", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[16], "h-sm", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[17], "h-bsm", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[18], "h-ie", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[19], "h-bie", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
  { & m32r_cgen_hw_entries[20], "h-bcond", CGEN_ASM_KEYWORD /*FIXME*/, 0 },
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
/* acc-s: accumulator register */
  { "acc-s", 12, 2, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
/* acc: accumulator reg (d) */
  { "acc", 8, 1, { 0, 0|(1<<CGEN_OPERAND_UNSIGNED), { 0 } }  },
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
/* abort-parallel-execution: abort parallel execution */
  { "abort-parallel-execution", 0, 0, { 0, 0|(1<<CGEN_OPERAND_FAKE), { 0 } }  },
};

#define OP 1 /* syntax value for mnemonic */

static const CGEN_SYNTAX syntax_table[] = {
/* <op> $dr,$sr */
/*   0 */  { OP, ' ', 130, ',', 129, 0 },
/* <op> $dr,$sr,$slo16 */
/*   1 */  { OP, ' ', 130, ',', 129, ',', 143, 0 },
/* <op> $dr,$sr,$uimm16 */
/*   2 */  { OP, ' ', 130, ',', 129, ',', 139, 0 },
/* <op> $dr,$sr,$ulo16 */
/*   3 */  { OP, ' ', 130, ',', 129, ',', 144, 0 },
/* <op> $dr,$simm8 */
/*   4 */  { OP, ' ', 130, ',', 135, 0 },
/* <op> $dr,$sr,$simm16 */
/*   5 */  { OP, ' ', 130, ',', 129, ',', 136, 0 },
/* <op> $disp8 */
/*   6 */  { OP, ' ', 146, 0 },
/* <op> $disp24 */
/*   7 */  { OP, ' ', 148, 0 },
/* <op> $src1,$src2,$disp16 */
/*   8 */  { OP, ' ', 131, ',', 132, ',', 147, 0 },
/* <op> $src2,$disp16 */
/*   9 */  { OP, ' ', 132, ',', 147, 0 },
/* <op> $src1,$src2 */
/*  10 */  { OP, ' ', 131, ',', 132, 0 },
/* <op> $src2,$simm16 */
/*  11 */  { OP, ' ', 132, ',', 136, 0 },
/* <op> $src2,$uimm16 */
/*  12 */  { OP, ' ', 132, ',', 139, 0 },
/* <op> $src2 */
/*  13 */  { OP, ' ', 132, 0 },
/* <op> $sr */
/*  14 */  { OP, ' ', 129, 0 },
/* <op> $dr,@$sr */
/*  15 */  { OP, ' ', 130, ',', '@', 129, 0 },
/* <op> $dr,@($sr) */
/*  16 */  { OP, ' ', 130, ',', '@', '(', 129, ')', 0 },
/* <op> $dr,@($slo16,$sr) */
/*  17 */  { OP, ' ', 130, ',', '@', '(', 143, ',', 129, ')', 0 },
/* <op> $dr,@($sr,$slo16) */
/*  18 */  { OP, ' ', 130, ',', '@', '(', 129, ',', 143, ')', 0 },
/* <op> $dr,@$sr+ */
/*  19 */  { OP, ' ', 130, ',', '@', 129, '+', 0 },
/* <op> $dr,$uimm24 */
/*  20 */  { OP, ' ', 130, ',', 145, 0 },
/* <op> $dr,$slo16 */
/*  21 */  { OP, ' ', 130, ',', 143, 0 },
/* <op> $src1,$src2,$acc */
/*  22 */  { OP, ' ', 131, ',', 132, ',', 141, 0 },
/* <op> $dr */
/*  23 */  { OP, ' ', 130, 0 },
/* <op> $dr,$accs */
/*  24 */  { OP, ' ', 130, ',', 141, 0 },
/* <op> $dr,$scr */
/*  25 */  { OP, ' ', 130, ',', 133, 0 },
/* <op> $src1 */
/*  26 */  { OP, ' ', 131, 0 },
/* <op> $src1,$accs */
/*  27 */  { OP, ' ', 131, ',', 141, 0 },
/* <op> $sr,$dcr */
/*  28 */  { OP, ' ', 129, ',', 134, 0 },
/* <op> */
/*  29 */  { OP, 0 },
/* <op> $acc-s */
/*  30 */  { OP, ' ', 141, 0 },
/* <op> $dr,$hi16 */
/*  31 */  { OP, ' ', 130, ',', 142, 0 },
/* <op> $dr,$uimm5 */
/*  32 */  { OP, ' ', 130, ',', 138, 0 },
/* <op> $src1,@$src2 */
/*  33 */  { OP, ' ', 131, ',', '@', 132, 0 },
/* <op> $src1,@($src2) */
/*  34 */  { OP, ' ', 131, ',', '@', '(', 132, ')', 0 },
/* <op> $src1,@($slo16,$src2) */
/*  35 */  { OP, ' ', 131, ',', '@', '(', 143, ',', 132, ')', 0 },
/* <op> $src1,@($src2,$slo16) */
/*  36 */  { OP, ' ', 131, ',', '@', '(', 132, ',', 143, ')', 0 },
/* <op> $src1,@+$src2 */
/*  37 */  { OP, ' ', 131, ',', '@', '+', 132, 0 },
/* <op> $src1,@-$src2 */
/*  38 */  { OP, ' ', 131, ',', '@', '-', 132, 0 },
/* <op> $uimm4 */
/*  39 */  { OP, ' ', 137, 0 },
/* <op> $dr,$src2 */
/*  40 */  { OP, ' ', 130, ',', 132, 0 },
};

#undef OP

static const CGEN_FORMAT format_table[] = {
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
/* f-op1.number.f-r1.number.f-op2.number.f-r2.src2. */
/*  13 */  { 16, 16, 0xfff0 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.sr.f-simm16.number. */
/*  14 */  { 32, 32, 0xf0f0ffff },
/* f-op1.number.f-r1.number.f-op2.number.f-r2.sr. */
/*  15 */  { 16, 16, 0xfff0 },
/* f-op1.number.f-r1.dr.f-uimm24.uimm24. */
/*  16 */  { 32, 32, 0xf0000000 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.number.f-simm16.slo16. */
/*  17 */  { 32, 32, 0xf0ff0000 },
/* f-op1.number.f-r1.src1.f-acc.acc.f-op23.number.f-r2.src2. */
/*  18 */  { 16, 16, 0xf070 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.number. */
/*  19 */  { 16, 16, 0xf0ff },
/* f-op1.number.f-r1.dr.f-op2.number.f-acc-s.acc-s.f-op3.number. */
/*  20 */  { 16, 16, 0xf0f3 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.scr. */
/*  21 */  { 16, 16, 0xf0f0 },
/* f-op1.number.f-r1.src1.f-op2.number.f-r2.number. */
/*  22 */  { 16, 16, 0xf0ff },
/* f-op1.number.f-r1.src1.f-op2.number.f-acc-s.acc-s.f-op3.number. */
/*  23 */  { 16, 16, 0xf0f3 },
/* f-op1.number.f-r1.dcr.f-op2.number.f-r2.sr. */
/*  24 */  { 16, 16, 0xf0f0 },
/* f-op1.number.f-r1.number.f-op2.number.f-r2.number. */
/*  25 */  { 16, 16, 0xffff },
/* f-op1.number.f-r1.number.f-op2.number.f-acc-s.acc-s.f-op3.number. */
/*  26 */  { 16, 16, 0xfff3 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.number.f-hi16.hi16. */
/*  27 */  { 32, 32, 0xf0ff0000 },
/* f-op1.number.f-r1.dr.f-shift-op2.number.f-uimm5.uimm5. */
/*  28 */  { 16, 16, 0xf0e0 },
/* f-op1.number.f-r1.src1.f-op2.number.f-r2.src2.f-simm16.slo16. */
/*  29 */  { 32, 32, 0xf0f00000 },
/* f-op1.number.f-r1.number.f-op2.number.f-uimm4.uimm4. */
/*  30 */  { 16, 16, 0xfff0 },
/* f-op1.number.f-r1.dr.f-op2.number.f-r2.src2.f-uimm16.number. */
/*  31 */  { 32, 32, 0xf0f0ffff },
};

#define A(a) (1 << CGEN_CAT3 (CGEN_INSN,_,a))
#define SYN(n) (& syntax_table[n])
#define FMT(n) (& format_table[n])

const CGEN_INSN m32r_cgen_insn_table_entries[CGEN_NUM_INSNS] = {
/* null first entry, end of all hash chains */
  { { 0 }, 0 },
/* add $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "add", "add", SYN (0), FMT (0), 0xa0,
    { 2, 0|A(PARALLEL), { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* add3 $dr,$sr,$slo16 */
  {
    { 1, 1, 1, 1 },
    "add3", "add3", SYN (1), FMT (1), 0x80a00000,
    { 2, 0, { 0 } }
  },
/* and $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "and", "and", SYN (0), FMT (0), 0xc0,
    { 2, 0|A(PARALLEL), { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* and3 $dr,$sr,$uimm16 */
  {
    { 1, 1, 1, 1 },
    "and3", "and3", SYN (2), FMT (2), 0x80c00000,
    { 2, 0, { 0 } }
  },
/* or $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "or", "or", SYN (0), FMT (0), 0xe0,
    { 2, 0|A(PARALLEL), { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* or3 $dr,$sr,$ulo16 */
  {
    { 1, 1, 1, 1 },
    "or3", "or3", SYN (3), FMT (3), 0x80e00000,
    { 2, 0, { 0 } }
  },
/* xor $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "xor", "xor", SYN (0), FMT (0), 0xd0,
    { 2, 0|A(PARALLEL), { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* xor3 $dr,$sr,$uimm16 */
  {
    { 1, 1, 1, 1 },
    "xor3", "xor3", SYN (2), FMT (2), 0x80d00000,
    { 2, 0, { 0 } }
  },
/* addi $dr,$simm8 */
  {
    { 1, 1, 1, 1 },
    "addi", "addi", SYN (4), FMT (4), 0x4000,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* addv $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "addv", "addv", SYN (0), FMT (0), 0x80,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* addv3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    "addv3", "addv3", SYN (5), FMT (5), 0x80800000,
    { 2, 0, { 0 } }
  },
/* addx $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "addx", "addx", SYN (0), FMT (0), 0x90,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* bc $disp8 */
  {
    { 1, 1, 1, 1 },
    "bc8", "bc", SYN (6), FMT (6), 0x7c00,
    { 2, 0|A(RELAX_BC)|A(RELAXABLE)|A(COND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* bc.s $disp8 */
  {
    { 1, 1, 1, 1 },
    "bc8.s", "bc.s", SYN (6), FMT (6), 0x7c00,
    { 2, 0|A(ALIAS)|A(COND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* bc $disp24 */
  {
    { 1, 1, 1, 1 },
    "bc24", "bc", SYN (7), FMT (7), 0xfc000000,
    { 2, 0|A(RELAX_BC)|A(RELAX)|A(COND_CTI), { 0 } }
  },
/* bc.l $disp24 */
  {
    { 1, 1, 1, 1 },
    "bc24.l", "bc.l", SYN (7), FMT (7), 0xfc000000,
    { 2, 0|A(ALIAS)|A(COND_CTI), { 0 } }
  },
/* beq $src1,$src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "beq", "beq", SYN (8), FMT (8), 0xb0000000,
    { 2, 0|A(COND_CTI), { 0 } }
  },
/* beqz $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "beqz", "beqz", SYN (9), FMT (9), 0xb0800000,
    { 2, 0|A(COND_CTI), { 0 } }
  },
/* bgez $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "bgez", "bgez", SYN (9), FMT (9), 0xb0b00000,
    { 2, 0|A(COND_CTI), { 0 } }
  },
/* bgtz $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "bgtz", "bgtz", SYN (9), FMT (9), 0xb0d00000,
    { 2, 0|A(COND_CTI), { 0 } }
  },
/* blez $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "blez", "blez", SYN (9), FMT (9), 0xb0c00000,
    { 2, 0|A(COND_CTI), { 0 } }
  },
/* bltz $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "bltz", "bltz", SYN (9), FMT (9), 0xb0a00000,
    { 2, 0|A(COND_CTI), { 0 } }
  },
/* bnez $src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "bnez", "bnez", SYN (9), FMT (9), 0xb0900000,
    { 2, 0|A(COND_CTI), { 0 } }
  },
/* bl $disp8 */
  {
    { 1, 1, 1, 1 },
    "bl8", "bl", SYN (6), FMT (6), 0x7e00,
    { 2, 0|A(FILL_SLOT)|A(RELAX_BL)|A(RELAXABLE)|A(UNCOND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* bl.s $disp8 */
  {
    { 1, 1, 1, 1 },
    "bl8.s", "bl.s", SYN (6), FMT (6), 0x7e00,
    { 2, 0|A(FILL_SLOT)|A(ALIAS)|A(UNCOND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* bl $disp24 */
  {
    { 1, 1, 1, 1 },
    "bl24", "bl", SYN (7), FMT (7), 0xfe000000,
    { 2, 0|A(RELAX_BL)|A(RELAX)|A(UNCOND_CTI), { 0 } }
  },
/* bl.l $disp24 */
  {
    { 1, 1, 1, 1 },
    "bl24.l", "bl.l", SYN (7), FMT (7), 0xfe000000,
    { 2, 0|A(ALIAS)|A(UNCOND_CTI), { 0 } }
  },
/* bcl $disp8 */
  {
    { 1, 1, 1, 1 },
    "bcl8", "bcl", SYN (6), FMT (6), 0x7800,
    { 2, 0|A(RELAX_BCL)|A(RELAXABLE)|A(COND_CTI), { [CGEN_INSN_PIPE] = PIPE_O, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* bcl.s $disp8 */
  {
    { 1, 1, 1, 1 },
    "bcl8.s", "bcl.s", SYN (6), FMT (6), 0x7800,
    { 2, 0|A(ALIAS)|A(COND_CTI), { [CGEN_INSN_PIPE] = PIPE_O, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* bcl $disp24 */
  {
    { 1, 1, 1, 1 },
    "bcl24", "bcl", SYN (7), FMT (7), 0xf8000000,
    { 2, 0|A(RELAX_BCL)|A(RELAX)|A(COND_CTI), { [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* bcl.l $disp24 */
  {
    { 1, 1, 1, 1 },
    "bcl24.l", "bcl.l", SYN (7), FMT (7), 0xf8000000,
    { 2, 0|A(ALIAS)|A(COND_CTI), { [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* bnc $disp8 */
  {
    { 1, 1, 1, 1 },
    "bnc8", "bnc", SYN (6), FMT (6), 0x7d00,
    { 2, 0|A(RELAX_BNC)|A(RELAXABLE)|A(COND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* bnc.s $disp8 */
  {
    { 1, 1, 1, 1 },
    "bnc8.s", "bnc.s", SYN (6), FMT (6), 0x7d00,
    { 2, 0|A(ALIAS)|A(COND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* bnc $disp24 */
  {
    { 1, 1, 1, 1 },
    "bnc24", "bnc", SYN (7), FMT (7), 0xfd000000,
    { 2, 0|A(RELAX_BNC)|A(RELAX)|A(COND_CTI), { 0 } }
  },
/* bnc.l $disp24 */
  {
    { 1, 1, 1, 1 },
    "bnc24.l", "bnc.l", SYN (7), FMT (7), 0xfd000000,
    { 2, 0|A(ALIAS)|A(COND_CTI), { 0 } }
  },
/* bne $src1,$src2,$disp16 */
  {
    { 1, 1, 1, 1 },
    "bne", "bne", SYN (8), FMT (8), 0xb0100000,
    { 2, 0|A(COND_CTI), { 0 } }
  },
/* bra $disp8 */
  {
    { 1, 1, 1, 1 },
    "bra8", "bra", SYN (6), FMT (6), 0x7f00,
    { 2, 0|A(FILL_SLOT)|A(RELAX_BRA)|A(RELAXABLE)|A(UNCOND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* bra.s $disp8 */
  {
    { 1, 1, 1, 1 },
    "bra8.s", "bra.s", SYN (6), FMT (6), 0x7f00,
    { 2, 0|A(ALIAS)|A(UNCOND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* bra $disp24 */
  {
    { 1, 1, 1, 1 },
    "bra24", "bra", SYN (7), FMT (7), 0xff000000,
    { 2, 0|A(RELAX_BRA)|A(RELAX)|A(UNCOND_CTI), { 0 } }
  },
/* bra.l $disp24 */
  {
    { 1, 1, 1, 1 },
    "bra24.l", "bra.l", SYN (7), FMT (7), 0xff000000,
    { 2, 0|A(ALIAS)|A(UNCOND_CTI), { 0 } }
  },
/* bncl $disp8 */
  {
    { 1, 1, 1, 1 },
    "bncl8", "bncl", SYN (6), FMT (6), 0x7900,
    { 2, 0|A(RELAX_BNCL)|A(RELAXABLE)|A(COND_CTI), { [CGEN_INSN_PIPE] = PIPE_O, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* bncl.s $disp8 */
  {
    { 1, 1, 1, 1 },
    "bncl8.s", "bncl.s", SYN (6), FMT (6), 0x7900,
    { 2, 0|A(ALIAS)|A(COND_CTI), { [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* bncl $disp24 */
  {
    { 1, 1, 1, 1 },
    "bncl24", "bncl", SYN (7), FMT (7), 0xf9000000,
    { 2, 0|A(RELAX_BNC)|A(RELAX)|A(COND_CTI), { [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* bncl.l $disp24 */
  {
    { 1, 1, 1, 1 },
    "bncl24.l", "bncl.l", SYN (7), FMT (7), 0xf9000000,
    { 2, 0|A(ALIAS)|A(COND_CTI), { [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* cmp $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "cmp", "cmp", SYN (10), FMT (10), 0x40,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* cmpi $src2,$simm16 */
  {
    { 1, 1, 1, 1 },
    "cmpi", "cmpi", SYN (11), FMT (11), 0x80400000,
    { 2, 0, { 0 } }
  },
/* cmpu $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "cmpu", "cmpu", SYN (10), FMT (10), 0x50,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* cmpui $src2,$uimm16 */
  {
    { 1, 1, 1, 1 },
    "cmpui", "cmpui", SYN (12), FMT (12), 0x80500000,
    { 2, 0, { [CGEN_INSN_MACH] = (1 << MACH_M32R) | (1 << MACH_M32RX) } }
  },
/* cmpeq $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "cmpeq", "cmpeq", SYN (10), FMT (10), 0x60,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* cmpz $src2 */
  {
    { 1, 1, 1, 1 },
    "cmpz", "cmpz", SYN (13), FMT (13), 0x70,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* div $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "div", "div", SYN (0), FMT (14), 0x90000000,
    { 2, 0, { 0 } }
  },
/* divu $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "divu", "divu", SYN (0), FMT (14), 0x90100000,
    { 2, 0, { 0 } }
  },
/* rem $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "rem", "rem", SYN (0), FMT (14), 0x90200000,
    { 2, 0, { 0 } }
  },
/* remu $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "remu", "remu", SYN (0), FMT (14), 0x90300000,
    { 2, 0, { 0 } }
  },
/* jc $sr */
  {
    { 1, 1, 1, 1 },
    "jc", "jc", SYN (14), FMT (15), 0x1cc0,
    { 2, 0|A(COND_CTI), { [CGEN_INSN_PIPE] = PIPE_O, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* jnc $sr */
  {
    { 1, 1, 1, 1 },
    "jnc", "jnc", SYN (14), FMT (15), 0x1cc0,
    { 2, 0|A(COND_CTI), { [CGEN_INSN_PIPE] = PIPE_O, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* jl $sr */
  {
    { 1, 1, 1, 1 },
    "jl", "jl", SYN (14), FMT (15), 0x1ec0,
    { 2, 0|A(FILL_SLOT)|A(UNCOND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* jmp $sr */
  {
    { 1, 1, 1, 1 },
    "jmp", "jmp", SYN (14), FMT (15), 0x1fc0,
    { 2, 0|A(UNCOND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* ld $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "ld", "ld", SYN (15), FMT (0), 0x20c0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* ld $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    "ld-2", "ld", SYN (16), FMT (0), 0x20c0,
    { 2, 0|A(ALIAS), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* ld $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    "ld-d", "ld", SYN (17), FMT (1), 0xa0c00000,
    { 2, 0, { 0 } }
  },
/* ld $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    "ld-d2", "ld", SYN (18), FMT (1), 0xa0c00000,
    { 2, 0|A(ALIAS), { 0 } }
  },
/* ldb $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "ldb", "ldb", SYN (15), FMT (0), 0x2080,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* ldb $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    "ldb-2", "ldb", SYN (16), FMT (0), 0x2080,
    { 2, 0|A(ALIAS), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* ldb $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    "ldb-d", "ldb", SYN (17), FMT (1), 0xa0800000,
    { 2, 0, { 0 } }
  },
/* ldb $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    "ldb-d2", "ldb", SYN (18), FMT (1), 0xa0800000,
    { 2, 0|A(ALIAS), { 0 } }
  },
/* ldh $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "ldh", "ldh", SYN (15), FMT (0), 0x20a0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* ldh $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    "ldh-2", "ldh", SYN (16), FMT (0), 0x20a0,
    { 2, 0|A(ALIAS), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* ldh $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    "ldh-d", "ldh", SYN (17), FMT (1), 0xa0a00000,
    { 2, 0, { 0 } }
  },
/* ldh $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    "ldh-d2", "ldh", SYN (18), FMT (1), 0xa0a00000,
    { 2, 0|A(ALIAS), { 0 } }
  },
/* ldub $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "ldub", "ldub", SYN (15), FMT (0), 0x2090,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* ldub $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    "ldub-2", "ldub", SYN (16), FMT (0), 0x2090,
    { 2, 0|A(ALIAS), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* ldub $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    "ldub-d", "ldub", SYN (17), FMT (1), 0xa0900000,
    { 2, 0, { 0 } }
  },
/* ldub $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    "ldub-d2", "ldub", SYN (18), FMT (1), 0xa0900000,
    { 2, 0|A(ALIAS), { 0 } }
  },
/* lduh $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "lduh", "lduh", SYN (15), FMT (0), 0x20b0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* lduh $dr,@($sr) */
  {
    { 1, 1, 1, 1 },
    "lduh-2", "lduh", SYN (16), FMT (0), 0x20b0,
    { 2, 0|A(ALIAS), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* lduh $dr,@($slo16,$sr) */
  {
    { 1, 1, 1, 1 },
    "lduh-d", "lduh", SYN (17), FMT (1), 0xa0b00000,
    { 2, 0, { 0 } }
  },
/* lduh $dr,@($sr,$slo16) */
  {
    { 1, 1, 1, 1 },
    "lduh-d2", "lduh", SYN (18), FMT (1), 0xa0b00000,
    { 2, 0|A(ALIAS), { 0 } }
  },
/* ld $dr,@$sr+ */
  {
    { 1, 1, 1, 1 },
    "ld-plus", "ld", SYN (19), FMT (0), 0x20e0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* ld24 $dr,$uimm24 */
  {
    { 1, 1, 1, 1 },
    "ld24", "ld24", SYN (20), FMT (16), 0xe0000000,
    { 2, 0, { 0 } }
  },
/* ldi $dr,$simm8 */
  {
    { 1, 1, 1, 1 },
    "ldi8", "ldi", SYN (4), FMT (4), 0x6000,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* ldi8 $dr,$simm8 */
  {
    { 1, 1, 1, 1 },
    "ldi8a", "ldi8", SYN (4), FMT (4), 0x6000,
    { 2, 0|A(ALIAS), { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* ldi $dr,$slo16 */
  {
    { 1, 1, 1, 1 },
    "ldi16", "ldi", SYN (21), FMT (17), 0x90f00000,
    { 2, 0, { 0 } }
  },
/* ldi16 $dr,$slo16 */
  {
    { 1, 1, 1, 1 },
    "ldi16a", "ldi16", SYN (21), FMT (17), 0x90f00000,
    { 2, 0|A(ALIAS), { 0 } }
  },
/* lock $dr,@$sr */
  {
    { 1, 1, 1, 1 },
    "lock", "lock", SYN (15), FMT (0), 0x20d0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* machi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "machi", "machi", SYN (10), FMT (10), 0x3040,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* machi $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    "machi-a", "machi", SYN (22), FMT (18), 0x3040,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* maclo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "maclo", "maclo", SYN (10), FMT (10), 0x3050,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* maclo $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    "maclo-a", "maclo", SYN (22), FMT (18), 0x3050,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* macwhi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "macwhi", "macwhi", SYN (10), FMT (10), 0x3060,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* macwhi $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    "macwhi-a", "macwhi", SYN (22), FMT (18), 0x3060,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* macwlo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "macwlo", "macwlo", SYN (10), FMT (10), 0x3070,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* macwlo $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    "macwlo-a", "macwlo", SYN (22), FMT (18), 0x3070,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* mul $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "mul", "mul", SYN (0), FMT (0), 0x1060,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* mulhi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "mulhi", "mulhi", SYN (10), FMT (10), 0x3000,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* mulhi $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    "mulhi-a", "mulhi", SYN (22), FMT (18), 0x3000,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* mullo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "mullo", "mullo", SYN (10), FMT (10), 0x3010,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* mullo $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    "mullo-a", "mullo", SYN (22), FMT (18), 0x3010,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* mulwhi $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "mulwhi", "mulwhi", SYN (10), FMT (10), 0x3020,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* mulwhi $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    "mulwhi-a", "mulwhi", SYN (22), FMT (18), 0x3020,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* mulwlo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "mulwlo", "mulwlo", SYN (10), FMT (10), 0x3030,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* mulwlo $src1,$src2,$acc */
  {
    { 1, 1, 1, 1 },
    "mulwlo-a", "mulwlo", SYN (22), FMT (18), 0x3030,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* mv $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "mv", "mv", SYN (0), FMT (0), 0x1080,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* mvfachi $dr */
  {
    { 1, 1, 1, 1 },
    "mvfachi", "mvfachi", SYN (23), FMT (19), 0x50f0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* mvfachi $dr,$accs */
  {
    { 1, 1, 1, 1 },
    "mvfachi-a", "mvfachi", SYN (24), FMT (20), 0x50f0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* mvfaclo $dr */
  {
    { 1, 1, 1, 1 },
    "mvfaclo", "mvfaclo", SYN (23), FMT (19), 0x50f1,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* mvfaclo $dr,$acc-s */
  {
    { 1, 1, 1, 1 },
    "mvfaclo-a", "mvfaclo", SYN (24), FMT (20), 0x50f1,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* mvfacmi $dr */
  {
    { 1, 1, 1, 1 },
    "mvfacmi", "mvfacmi", SYN (23), FMT (19), 0x50f2,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* mvfacmi $dr,$acc-s */
  {
    { 1, 1, 1, 1 },
    "mvfacmi-a", "mvfacmi", SYN (24), FMT (20), 0x50f2,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* mvfc $dr,$scr */
  {
    { 1, 1, 1, 1 },
    "mvfc", "mvfc", SYN (25), FMT (21), 0x1090,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* mvtachi $src1 */
  {
    { 1, 1, 1, 1 },
    "mvtachi", "mvtachi", SYN (26), FMT (22), 0x5070,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* mvtachi $src1,$acc-s */
  {
    { 1, 1, 1, 1 },
    "mvtachi-a", "mvtachi", SYN (27), FMT (23), 0x5070,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* mvtaclo $src1 */
  {
    { 1, 1, 1, 1 },
    "mvtaclo", "mvtaclo", SYN (26), FMT (22), 0x5071,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* mvtaclo $src1,$acc-s */
  {
    { 1, 1, 1, 1 },
    "mvtaclo-a", "mvtaclo", SYN (27), FMT (23), 0x5071,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* mvtc $sr,$dcr */
  {
    { 1, 1, 1, 1 },
    "mvtc", "mvtc", SYN (28), FMT (24), 0x10a0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* neg $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "neg", "neg", SYN (0), FMT (0), 0x30,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* nop */
  {
    { 1, 1, 1, 1 },
    "nop", "nop", SYN (29), FMT (25), 0x7000,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* not $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "not", "not", SYN (0), FMT (0), 0xb0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* rac */
  {
    { 1, 1, 1, 1 },
    "rac", "rac", SYN (29), FMT (25), 0x5090,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* rac $acc-s */
  {
    { 1, 1, 1, 1 },
    "rac-a", "rac", SYN (30), FMT (26), 0x5090,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* rach */
  {
    { 1, 1, 1, 1 },
    "rach", "rach", SYN (29), FMT (25), 0x5080,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S } }
  },
/* rach $acc-s */
  {
    { 1, 1, 1, 1 },
    "rach-a", "rach", SYN (30), FMT (26), 0x5080,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* rte */
  {
    { 1, 1, 1, 1 },
    "rte", "rte", SYN (29), FMT (25), 0x10d6,
    { 2, 0|A(UNCOND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* seth $dr,$hi16 */
  {
    { 1, 1, 1, 1 },
    "seth", "seth", SYN (31), FMT (27), 0xd0c00000,
    { 2, 0, { 0 } }
  },
/* sll $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "sll", "sll", SYN (0), FMT (0), 0x1040,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* sll3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    "sll3", "sll3", SYN (5), FMT (5), 0x90c00000,
    { 2, 0, { 0 } }
  },
/* slli $dr,$uimm5 */
  {
    { 1, 1, 1, 1 },
    "slli", "slli", SYN (32), FMT (28), 0x5040,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* sra $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "sra", "sra", SYN (0), FMT (0), 0x1020,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* sra3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    "sra3", "sra3", SYN (5), FMT (5), 0x90a00000,
    { 2, 0, { 0 } }
  },
/* srai $dr,$uimm5 */
  {
    { 1, 1, 1, 1 },
    "srai", "srai", SYN (32), FMT (28), 0x5020,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* srl $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "srl", "srl", SYN (0), FMT (0), 0x1000,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* srl3 $dr,$sr,$simm16 */
  {
    { 1, 1, 1, 1 },
    "srl3", "srl3", SYN (5), FMT (5), 0x90800000,
    { 2, 0, { 0 } }
  },
/* srli $dr,$uimm5 */
  {
    { 1, 1, 1, 1 },
    "srli", "srli", SYN (32), FMT (28), 0x5000,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* st $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    "st", "st", SYN (33), FMT (10), 0x2040,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* st $src1,@($src2) */
  {
    { 1, 1, 1, 1 },
    "st-2", "st", SYN (34), FMT (10), 0x2040,
    { 2, 0|A(ALIAS), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* st $src1,@($slo16,$src2) */
  {
    { 1, 1, 1, 1 },
    "st-d", "st", SYN (35), FMT (29), 0xa0400000,
    { 2, 0, { 0 } }
  },
/* st $src1,@($src2,$slo16) */
  {
    { 1, 1, 1, 1 },
    "st-d2", "st", SYN (36), FMT (29), 0xa0400000,
    { 2, 0|A(ALIAS), { 0 } }
  },
/* stb $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    "stb", "stb", SYN (33), FMT (10), 0x2000,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* stb $src1,@($src2) */
  {
    { 1, 1, 1, 1 },
    "stb-2", "stb", SYN (34), FMT (10), 0x2000,
    { 2, 0|A(ALIAS), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* stb $src1,@($slo16,$src2) */
  {
    { 1, 1, 1, 1 },
    "stb-d", "stb", SYN (35), FMT (29), 0xa0000000,
    { 2, 0, { 0 } }
  },
/* stb $src1,@($src2,$slo16) */
  {
    { 1, 1, 1, 1 },
    "stb-d2", "stb", SYN (36), FMT (29), 0xa0000000,
    { 2, 0|A(ALIAS), { 0 } }
  },
/* sth $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    "sth", "sth", SYN (33), FMT (10), 0x2020,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* sth $src1,@($src2) */
  {
    { 1, 1, 1, 1 },
    "sth-2", "sth", SYN (34), FMT (10), 0x2020,
    { 2, 0|A(ALIAS), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* sth $src1,@($slo16,$src2) */
  {
    { 1, 1, 1, 1 },
    "sth-d", "sth", SYN (35), FMT (29), 0xa0200000,
    { 2, 0, { 0 } }
  },
/* sth $src1,@($src2,$slo16) */
  {
    { 1, 1, 1, 1 },
    "sth-d2", "sth", SYN (36), FMT (29), 0xa0200000,
    { 2, 0|A(ALIAS), { 0 } }
  },
/* st $src1,@+$src2 */
  {
    { 1, 1, 1, 1 },
    "st-plus", "st", SYN (37), FMT (10), 0x2060,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* st $src1,@-$src2 */
  {
    { 1, 1, 1, 1 },
    "st-minus", "st", SYN (38), FMT (10), 0x2070,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* sub $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "sub", "sub", SYN (0), FMT (0), 0x20,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* subv $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "subv", "subv", SYN (0), FMT (0), 0x0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* subx $dr,$sr */
  {
    { 1, 1, 1, 1 },
    "subx", "subx", SYN (0), FMT (0), 0x10,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS } }
  },
/* trap $uimm4 */
  {
    { 1, 1, 1, 1 },
    "trap", "trap", SYN (39), FMT (30), 0x10f0,
    { 2, 0|A(FILL_SLOT)|A(UNCOND_CTI), { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* unlock $src1,@$src2 */
  {
    { 1, 1, 1, 1 },
    "unlock", "unlock", SYN (33), FMT (10), 0x2050,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O } }
  },
/* push $src1 */
  {
    { 1, 1, 1, 1 },
    "push", "push", SYN (26), FMT (22), 0x207f,
    { 2, 0|A(ALIAS), { 0 } }
  },
/* pop $dr */
  {
    { 1, 1, 1, 1 },
    "pop", "pop", SYN (23), FMT (19), 0x20ef,
    { 2, 0|A(ALIAS), { 0 } }
  },
/* satb $dr,$src2 */
  {
    { 1, 1, 1, 1 },
    "satb", "satb", SYN (40), FMT (31), 0x80000100,
    { 2, 0, { [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* sath $dr,$src2 */
  {
    { 1, 1, 1, 1 },
    "sath", "sath", SYN (40), FMT (31), 0x80000200,
    { 2, 0, { [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* sat $dr,$src2 */
  {
    { 1, 1, 1, 1 },
    "sat", "sat", SYN (40), FMT (31), 0x80000000,
    { 2, 0, { [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* pcmpbz $src2 */
  {
    { 1, 1, 1, 1 },
    "pcmpbz", "pcmpbz", SYN (13), FMT (13), 0x370,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_OS, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* sadd */
  {
    { 1, 1, 1, 1 },
    "sadd", "sadd", SYN (29), FMT (25), 0x50e4,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* macwu1 $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "macwu1", "macwu1", SYN (10), FMT (10), 0x50b0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* msblo $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "msblo", "msblo", SYN (10), FMT (10), 0x50d0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* mulwu1 $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "mulwu1", "mulwu1", SYN (10), FMT (10), 0x50a0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* machl1 $src1,$src2 */
  {
    { 1, 1, 1, 1 },
    "machl1", "machl1", SYN (10), FMT (10), 0x50c0,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_S, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* sc */
  {
    { 1, 1, 1, 1 },
    "sc", "sc", SYN (29), FMT (25), 0x7401,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
/* snc */
  {
    { 1, 1, 1, 1 },
    "snc", "snc", SYN (29), FMT (25), 0x7501,
    { 2, 0, { [CGEN_INSN_PIPE] = PIPE_O, [CGEN_INSN_MACH] = 1 << MACH_M32RX } }
  },
};

#undef A
#undef SYN
#undef FMT

CGEN_INSN_TABLE m32r_cgen_insn_table = {
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
     const char *insn;
{
  return CGEN_ASM_HASH (insn);
}

unsigned int
m32r_cgen_dis_hash_insn (buf, value)
     const char *buf;
     unsigned long value;
{
  return CGEN_DIS_HASH (buf, value);
}

CGEN_OPCODE_DATA m32r_cgen_opcode_data = {
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
     const long *valuep;
     CGEN_FIELDS *fields;
{
  switch (opindex)
    {
    case M32R_OPERAND_SR :
      fields->f_r2 = *valuep;
      break;
    case M32R_OPERAND_DR :
      fields->f_r1 = *valuep;
      break;
    case M32R_OPERAND_SRC1 :
      fields->f_r1 = *valuep;
      break;
    case M32R_OPERAND_SRC2 :
      fields->f_r2 = *valuep;
      break;
    case M32R_OPERAND_SCR :
      fields->f_r2 = *valuep;
      break;
    case M32R_OPERAND_DCR :
      fields->f_r1 = *valuep;
      break;
    case M32R_OPERAND_SIMM8 :
      fields->f_simm8 = *valuep;
      break;
    case M32R_OPERAND_SIMM16 :
      fields->f_simm16 = *valuep;
      break;
    case M32R_OPERAND_UIMM4 :
      fields->f_uimm4 = *valuep;
      break;
    case M32R_OPERAND_UIMM5 :
      fields->f_uimm5 = *valuep;
      break;
    case M32R_OPERAND_UIMM16 :
      fields->f_uimm16 = *valuep;
      break;
    case M32R_OPERAND_ACC_S :
      fields->f_acc_s = *valuep;
      break;
    case M32R_OPERAND_ACC :
      fields->f_acc = *valuep;
      break;
    case M32R_OPERAND_HI16 :
      fields->f_hi16 = *valuep;
      break;
    case M32R_OPERAND_SLO16 :
      fields->f_simm16 = *valuep;
      break;
    case M32R_OPERAND_ULO16 :
      fields->f_uimm16 = *valuep;
      break;
    case M32R_OPERAND_UIMM24 :
      fields->f_uimm24 = *valuep;
      break;
    case M32R_OPERAND_DISP8 :
      fields->f_disp8 = *valuep;
      break;
    case M32R_OPERAND_DISP16 :
      fields->f_disp16 = *valuep;
      break;
    case M32R_OPERAND_DISP24 :
      fields->f_disp24 = *valuep;
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
     const CGEN_FIELDS *fields;
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
    case M32R_OPERAND_ACC_S :
      value = fields->f_acc_s;
      break;
    case M32R_OPERAND_ACC :
      value = fields->f_acc;
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

