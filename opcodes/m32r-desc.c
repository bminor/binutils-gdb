/* CPU data for m32r.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1996, 1997, 1998, 1999 Free Software Foundation, Inc.

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
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include "ansidecl.h"
#include "bfd.h"
#include "symcat.h"
#include "m32r-desc.h"
#include "m32r-opc.h"
#include "opintl.h"

/* Attributes.  */

static const CGEN_ATTR_ENTRY bool_attr[] =
{
  { "#f", 0 },
  { "#t", 1 },
  { 0, 0 }
};

static const CGEN_ATTR_ENTRY MACH_attr[] =
{
  { "base", MACH_BASE },
  { "m32r", MACH_M32R },
  { "max", MACH_MAX },
  { 0, 0 }
};

static const CGEN_ATTR_ENTRY ISA_attr[] =
{
  { "m32r", ISA_M32R },
  { "max", ISA_MAX },
  { 0, 0 }
};

const CGEN_ATTR_TABLE m32r_cgen_ifield_attr_table[] =
{
  { "MACH", & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "PCREL-ADDR", &bool_attr[0], &bool_attr[0] },
  { "ABS-ADDR", &bool_attr[0], &bool_attr[0] },
  { "RESERVED", &bool_attr[0], &bool_attr[0] },
  { "SIGN-OPT", &bool_attr[0], &bool_attr[0] },
  { "SIGNED", &bool_attr[0], &bool_attr[0] },
  { "RELOC", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE m32r_cgen_hardware_attr_table[] =
{
  { "MACH", & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "CACHE-ADDR", &bool_attr[0], &bool_attr[0] },
  { "PC", &bool_attr[0], &bool_attr[0] },
  { "PROFILE", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE m32r_cgen_operand_attr_table[] =
{
  { "MACH", & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "PCREL-ADDR", &bool_attr[0], &bool_attr[0] },
  { "ABS-ADDR", &bool_attr[0], &bool_attr[0] },
  { "SIGN-OPT", &bool_attr[0], &bool_attr[0] },
  { "SIGNED", &bool_attr[0], &bool_attr[0] },
  { "NEGATIVE", &bool_attr[0], &bool_attr[0] },
  { "RELAX", &bool_attr[0], &bool_attr[0] },
  { "SEM-ONLY", &bool_attr[0], &bool_attr[0] },
  { "RELOC", &bool_attr[0], &bool_attr[0] },
  { "HASH-PREFIX", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE m32r_cgen_insn_attr_table[] =
{
  { "MACH", & MACH_attr[0] },
  { "ALIAS", &bool_attr[0], &bool_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "UNCOND-CTI", &bool_attr[0], &bool_attr[0] },
  { "COND-CTI", &bool_attr[0], &bool_attr[0] },
  { "SKIP-CTI", &bool_attr[0], &bool_attr[0] },
  { "DELAY-SLOT", &bool_attr[0], &bool_attr[0] },
  { "RELAXABLE", &bool_attr[0], &bool_attr[0] },
  { "RELAX", &bool_attr[0], &bool_attr[0] },
  { "NO-DIS", &bool_attr[0], &bool_attr[0] },
  { "PBB", &bool_attr[0], &bool_attr[0] },
  { "FILL-SLOT", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

/* Instruction set variants.  */

static const CGEN_ISA m32r_cgen_isa_table[] = {
  { "m32r", 32, 32, 16, 32,  },
  { 0 }
};

/* Machine variants.  */

static const CGEN_MACH m32r_cgen_mach_table[] = {
  { "m32r", "m32r", MACH_M32R },
  { 0 }
};

static CGEN_KEYWORD_ENTRY m32r_cgen_opval_gr_names_entries[] =
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

CGEN_KEYWORD m32r_cgen_opval_gr_names =
{
  & m32r_cgen_opval_gr_names_entries[0],
  19
};

static CGEN_KEYWORD_ENTRY m32r_cgen_opval_cr_names_entries[] =
{
  { "psw", 0 },
  { "cbr", 1 },
  { "spi", 2 },
  { "spu", 3 },
  { "bpc", 6 },
  { "bbpsw", 8 },
  { "bbpc", 14 },
  { "cr0", 0 },
  { "cr1", 1 },
  { "cr2", 2 },
  { "cr3", 3 },
  { "cr4", 4 },
  { "cr5", 5 },
  { "cr6", 6 },
  { "cr7", 7 },
  { "cr8", 8 },
  { "cr9", 9 },
  { "cr10", 10 },
  { "cr11", 11 },
  { "cr12", 12 },
  { "cr13", 13 },
  { "cr14", 14 },
  { "cr15", 15 }
};

CGEN_KEYWORD m32r_cgen_opval_cr_names =
{
  & m32r_cgen_opval_cr_names_entries[0],
  23
};



/* The hardware table.  */

#define A(a) (1 << CONCAT2 (CGEN_HW_,a))

const CGEN_HW_ENTRY m32r_cgen_hw_table[] =
{
  { "h-memory", HW_H_MEMORY, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-sint", HW_H_SINT, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-uint", HW_H_UINT, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-addr", HW_H_ADDR, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-iaddr", HW_H_IADDR, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-pc", HW_H_PC, CGEN_ASM_NONE, 0, { 0|A(PROFILE)|A(PC), { (1<<MACH_BASE) } } },
  { "h-hi16", HW_H_HI16, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-slo16", HW_H_SLO16, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-ulo16", HW_H_ULO16, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-gr", HW_H_GR, CGEN_ASM_KEYWORD, (PTR) & m32r_cgen_opval_gr_names, { 0|A(CACHE_ADDR)|A(PROFILE), { (1<<MACH_BASE) } } },
  { "h-cr", HW_H_CR, CGEN_ASM_KEYWORD, (PTR) & m32r_cgen_opval_cr_names, { 0, { (1<<MACH_BASE) } } },
  { "h-accum", HW_H_ACCUM, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-cond", HW_H_COND, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-psw", HW_H_PSW, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-bpsw", HW_H_BPSW, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-bbpsw", HW_H_BBPSW, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { "h-lock", HW_H_LOCK, CGEN_ASM_NONE, 0, { 0, { (1<<MACH_BASE) } } },
  { 0 }
};

#undef A

/* The instruction field table.  */

#define A(a) (1 << CONCAT2 (CGEN_IFLD_,a))

const CGEN_IFLD m32r_cgen_ifld_table[] =
{
  { M32R_F_NIL, "f-nil", 0, 0, 0, 0, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_OP1, "f-op1", 0, 32, 0, 4, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_OP2, "f-op2", 0, 32, 8, 4, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_COND, "f-cond", 0, 32, 4, 4, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_R1, "f-r1", 0, 32, 4, 4, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_R2, "f-r2", 0, 32, 12, 4, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_SIMM8, "f-simm8", 0, 32, 8, 8, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_SIMM16, "f-simm16", 0, 32, 16, 16, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_SHIFT_OP2, "f-shift-op2", 0, 32, 8, 3, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_UIMM4, "f-uimm4", 0, 32, 12, 4, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_UIMM5, "f-uimm5", 0, 32, 11, 5, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_UIMM16, "f-uimm16", 0, 32, 16, 16, { 0, { (1<<MACH_BASE) } }  },
  { M32R_F_UIMM24, "f-uimm24", 0, 32, 8, 24, { 0|A(RELOC)|A(ABS_ADDR), { (1<<MACH_BASE) } }  },
  { M32R_F_HI16, "f-hi16", 0, 32, 16, 16, { 0|A(SIGN_OPT), { (1<<MACH_BASE) } }  },
  { M32R_F_DISP8, "f-disp8", 0, 32, 8, 8, { 0|A(RELOC)|A(PCREL_ADDR), { (1<<MACH_BASE) } }  },
  { M32R_F_DISP16, "f-disp16", 0, 32, 16, 16, { 0|A(RELOC)|A(PCREL_ADDR), { (1<<MACH_BASE) } }  },
  { M32R_F_DISP24, "f-disp24", 0, 32, 8, 24, { 0|A(RELOC)|A(PCREL_ADDR), { (1<<MACH_BASE) } }  },
  { 0 }
};

#undef A

/* The operand table.  */

#define A(a) (1 << CONCAT2 (CGEN_OPERAND_,a))
#define OPERAND(op) CONCAT2 (M32R_OPERAND_,op)

const CGEN_OPERAND m32r_cgen_operand_table[] =
{
/* pc: program counter */
  { "pc", M32R_OPERAND_PC, HW_H_PC, 0, 0,
    { 0|A(SEM_ONLY), { (1<<MACH_BASE) } }  },
/* sr: source register */
  { "sr", M32R_OPERAND_SR, HW_H_GR, 12, 4,
    { 0, { (1<<MACH_BASE) } }  },
/* dr: destination register */
  { "dr", M32R_OPERAND_DR, HW_H_GR, 4, 4,
    { 0, { (1<<MACH_BASE) } }  },
/* src1: source register 1 */
  { "src1", M32R_OPERAND_SRC1, HW_H_GR, 4, 4,
    { 0, { (1<<MACH_BASE) } }  },
/* src2: source register 2 */
  { "src2", M32R_OPERAND_SRC2, HW_H_GR, 12, 4,
    { 0, { (1<<MACH_BASE) } }  },
/* scr: source control register */
  { "scr", M32R_OPERAND_SCR, HW_H_CR, 12, 4,
    { 0, { (1<<MACH_BASE) } }  },
/* dcr: destination control register */
  { "dcr", M32R_OPERAND_DCR, HW_H_CR, 4, 4,
    { 0, { (1<<MACH_BASE) } }  },
/* simm8: 8 bit signed immediate */
  { "simm8", M32R_OPERAND_SIMM8, HW_H_SINT, 8, 8,
    { 0|A(HASH_PREFIX), { (1<<MACH_BASE) } }  },
/* simm16: 16 bit signed immediate */
  { "simm16", M32R_OPERAND_SIMM16, HW_H_SINT, 16, 16,
    { 0|A(HASH_PREFIX), { (1<<MACH_BASE) } }  },
/* uimm4: 4 bit trap number */
  { "uimm4", M32R_OPERAND_UIMM4, HW_H_UINT, 12, 4,
    { 0|A(HASH_PREFIX), { (1<<MACH_BASE) } }  },
/* uimm5: 5 bit shift count */
  { "uimm5", M32R_OPERAND_UIMM5, HW_H_UINT, 11, 5,
    { 0|A(HASH_PREFIX), { (1<<MACH_BASE) } }  },
/* uimm16: 16 bit unsigned immediate */
  { "uimm16", M32R_OPERAND_UIMM16, HW_H_UINT, 16, 16,
    { 0|A(HASH_PREFIX), { (1<<MACH_BASE) } }  },
/* hash: # prefix */
  { "hash", M32R_OPERAND_HASH, HW_H_SINT, 0, 0,
    { 0, { (1<<MACH_BASE) } }  },
/* hi16: high 16 bit immediate, sign optional */
  { "hi16", M32R_OPERAND_HI16, HW_H_HI16, 16, 16,
    { 0|A(SIGN_OPT), { (1<<MACH_BASE) } }  },
/* slo16: 16 bit signed immediate, for low() */
  { "slo16", M32R_OPERAND_SLO16, HW_H_SLO16, 16, 16,
    { 0, { (1<<MACH_BASE) } }  },
/* ulo16: 16 bit unsigned immediate, for low() */
  { "ulo16", M32R_OPERAND_ULO16, HW_H_ULO16, 16, 16,
    { 0, { (1<<MACH_BASE) } }  },
/* uimm24: 24 bit address */
  { "uimm24", M32R_OPERAND_UIMM24, HW_H_ADDR, 8, 24,
    { 0|A(HASH_PREFIX)|A(RELOC)|A(ABS_ADDR), { (1<<MACH_BASE) } }  },
/* disp8: 8 bit displacement */
  { "disp8", M32R_OPERAND_DISP8, HW_H_IADDR, 8, 8,
    { 0|A(RELAX)|A(RELOC)|A(PCREL_ADDR), { (1<<MACH_BASE) } }  },
/* disp16: 16 bit displacement */
  { "disp16", M32R_OPERAND_DISP16, HW_H_IADDR, 16, 16,
    { 0|A(RELOC)|A(PCREL_ADDR), { (1<<MACH_BASE) } }  },
/* disp24: 24 bit displacement */
  { "disp24", M32R_OPERAND_DISP24, HW_H_IADDR, 8, 24,
    { 0|A(RELAX)|A(RELOC)|A(PCREL_ADDR), { (1<<MACH_BASE) } }  },
/* condbit: condition bit */
  { "condbit", M32R_OPERAND_CONDBIT, HW_H_COND, 0, 0,
    { 0|A(SEM_ONLY), { (1<<MACH_BASE) } }  },
/* accum: accumulator */
  { "accum", M32R_OPERAND_ACCUM, HW_H_ACCUM, 0, 0,
    { 0|A(SEM_ONLY), { (1<<MACH_BASE) } }  },
  { 0 }
};

#undef A

#define A(a) (1 << CONCAT2 (CGEN_INSN_,a))
#define OP(field) CGEN_SYNTAX_MAKE_FIELD (OPERAND (field))

/* The instruction table.  */

static const CGEN_IBASE m32r_cgen_insn_table[MAX_INSNS] =
{
  /* Special null first entry.
     A `num' value of zero is thus invalid.
     Also, the special `invalid' insn resides here.  */
  { 0, 0, 0 },
/* add $dr,$sr */
  {
    M32R_INSN_ADD, "add", "add", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* add3 $dr,$sr,$hash$slo16 */
  {
    M32R_INSN_ADD3, "add3", "add3", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* and $dr,$sr */
  {
    M32R_INSN_AND, "and", "and", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* and3 $dr,$sr,$uimm16 */
  {
    M32R_INSN_AND3, "and3", "and3", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* or $dr,$sr */
  {
    M32R_INSN_OR, "or", "or", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* or3 $dr,$sr,$hash$ulo16 */
  {
    M32R_INSN_OR3, "or3", "or3", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* xor $dr,$sr */
  {
    M32R_INSN_XOR, "xor", "xor", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* xor3 $dr,$sr,$uimm16 */
  {
    M32R_INSN_XOR3, "xor3", "xor3", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* addi $dr,$simm8 */
  {
    M32R_INSN_ADDI, "addi", "addi", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* addv $dr,$sr */
  {
    M32R_INSN_ADDV, "addv", "addv", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* addv3 $dr,$sr,$simm16 */
  {
    M32R_INSN_ADDV3, "addv3", "addv3", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* addx $dr,$sr */
  {
    M32R_INSN_ADDX, "addx", "addx", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* bc.s $disp8 */
  {
    M32R_INSN_BC8, "bc8", "bc.s", 16,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bc.l $disp24 */
  {
    M32R_INSN_BC24, "bc24", "bc.l", 32,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* beq $src1,$src2,$disp16 */
  {
    M32R_INSN_BEQ, "beq", "beq", 32,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* beqz $src2,$disp16 */
  {
    M32R_INSN_BEQZ, "beqz", "beqz", 32,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bgez $src2,$disp16 */
  {
    M32R_INSN_BGEZ, "bgez", "bgez", 32,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bgtz $src2,$disp16 */
  {
    M32R_INSN_BGTZ, "bgtz", "bgtz", 32,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* blez $src2,$disp16 */
  {
    M32R_INSN_BLEZ, "blez", "blez", 32,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bltz $src2,$disp16 */
  {
    M32R_INSN_BLTZ, "bltz", "bltz", 32,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bnez $src2,$disp16 */
  {
    M32R_INSN_BNEZ, "bnez", "bnez", 32,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bl.s $disp8 */
  {
    M32R_INSN_BL8, "bl8", "bl.s", 16,
    { 0|A(FILL_SLOT)|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* bl.l $disp24 */
  {
    M32R_INSN_BL24, "bl24", "bl.l", 32,
    { 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* bnc.s $disp8 */
  {
    M32R_INSN_BNC8, "bnc8", "bnc.s", 16,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bnc.l $disp24 */
  {
    M32R_INSN_BNC24, "bnc24", "bnc.l", 32,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bne $src1,$src2,$disp16 */
  {
    M32R_INSN_BNE, "bne", "bne", 32,
    { 0|A(COND_CTI), { (1<<MACH_BASE) } }
  },
/* bra.s $disp8 */
  {
    M32R_INSN_BRA8, "bra8", "bra.s", 16,
    { 0|A(FILL_SLOT)|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* bra.l $disp24 */
  {
    M32R_INSN_BRA24, "bra24", "bra.l", 32,
    { 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* cmp $src1,$src2 */
  {
    M32R_INSN_CMP, "cmp", "cmp", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* cmpi $src2,$simm16 */
  {
    M32R_INSN_CMPI, "cmpi", "cmpi", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* cmpu $src1,$src2 */
  {
    M32R_INSN_CMPU, "cmpu", "cmpu", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* cmpui $src2,$simm16 */
  {
    M32R_INSN_CMPUI, "cmpui", "cmpui", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* div $dr,$sr */
  {
    M32R_INSN_DIV, "div", "div", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* divu $dr,$sr */
  {
    M32R_INSN_DIVU, "divu", "divu", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* rem $dr,$sr */
  {
    M32R_INSN_REM, "rem", "rem", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* remu $dr,$sr */
  {
    M32R_INSN_REMU, "remu", "remu", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* jl $sr */
  {
    M32R_INSN_JL, "jl", "jl", 16,
    { 0|A(FILL_SLOT)|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* jmp $sr */
  {
    M32R_INSN_JMP, "jmp", "jmp", 16,
    { 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* ld $dr,@$sr */
  {
    M32R_INSN_LD, "ld", "ld", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* ld $dr,@($slo16,$sr) */
  {
    M32R_INSN_LD_D, "ld-d", "ld", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* ldb $dr,@$sr */
  {
    M32R_INSN_LDB, "ldb", "ldb", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* ldb $dr,@($slo16,$sr) */
  {
    M32R_INSN_LDB_D, "ldb-d", "ldb", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* ldh $dr,@$sr */
  {
    M32R_INSN_LDH, "ldh", "ldh", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* ldh $dr,@($slo16,$sr) */
  {
    M32R_INSN_LDH_D, "ldh-d", "ldh", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* ldub $dr,@$sr */
  {
    M32R_INSN_LDUB, "ldub", "ldub", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* ldub $dr,@($slo16,$sr) */
  {
    M32R_INSN_LDUB_D, "ldub-d", "ldub", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* lduh $dr,@$sr */
  {
    M32R_INSN_LDUH, "lduh", "lduh", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* lduh $dr,@($slo16,$sr) */
  {
    M32R_INSN_LDUH_D, "lduh-d", "lduh", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* ld $dr,@$sr+ */
  {
    M32R_INSN_LD_PLUS, "ld-plus", "ld", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* ld24 $dr,$uimm24 */
  {
    M32R_INSN_LD24, "ld24", "ld24", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* ldi8 $dr,$simm8 */
  {
    M32R_INSN_LDI8, "ldi8", "ldi8", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* ldi16 $dr,$hash$slo16 */
  {
    M32R_INSN_LDI16, "ldi16", "ldi16", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* lock $dr,@$sr */
  {
    M32R_INSN_LOCK, "lock", "lock", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* machi $src1,$src2 */
  {
    M32R_INSN_MACHI, "machi", "machi", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* maclo $src1,$src2 */
  {
    M32R_INSN_MACLO, "maclo", "maclo", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* macwhi $src1,$src2 */
  {
    M32R_INSN_MACWHI, "macwhi", "macwhi", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* macwlo $src1,$src2 */
  {
    M32R_INSN_MACWLO, "macwlo", "macwlo", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* mul $dr,$sr */
  {
    M32R_INSN_MUL, "mul", "mul", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* mulhi $src1,$src2 */
  {
    M32R_INSN_MULHI, "mulhi", "mulhi", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* mullo $src1,$src2 */
  {
    M32R_INSN_MULLO, "mullo", "mullo", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* mulwhi $src1,$src2 */
  {
    M32R_INSN_MULWHI, "mulwhi", "mulwhi", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* mulwlo $src1,$src2 */
  {
    M32R_INSN_MULWLO, "mulwlo", "mulwlo", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* mv $dr,$sr */
  {
    M32R_INSN_MV, "mv", "mv", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* mvfachi $dr */
  {
    M32R_INSN_MVFACHI, "mvfachi", "mvfachi", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* mvfaclo $dr */
  {
    M32R_INSN_MVFACLO, "mvfaclo", "mvfaclo", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* mvfacmi $dr */
  {
    M32R_INSN_MVFACMI, "mvfacmi", "mvfacmi", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* mvfc $dr,$scr */
  {
    M32R_INSN_MVFC, "mvfc", "mvfc", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* mvtachi $src1 */
  {
    M32R_INSN_MVTACHI, "mvtachi", "mvtachi", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* mvtaclo $src1 */
  {
    M32R_INSN_MVTACLO, "mvtaclo", "mvtaclo", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* mvtc $sr,$dcr */
  {
    M32R_INSN_MVTC, "mvtc", "mvtc", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* neg $dr,$sr */
  {
    M32R_INSN_NEG, "neg", "neg", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* nop */
  {
    M32R_INSN_NOP, "nop", "nop", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* not $dr,$sr */
  {
    M32R_INSN_NOT, "not", "not", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* rac */
  {
    M32R_INSN_RAC, "rac", "rac", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* rach */
  {
    M32R_INSN_RACH, "rach", "rach", 16,
    { 0, { (1<<MACH_M32R) } }
  },
/* rte */
  {
    M32R_INSN_RTE, "rte", "rte", 16,
    { 0|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* seth $dr,$hash$hi16 */
  {
    M32R_INSN_SETH, "seth", "seth", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* sll $dr,$sr */
  {
    M32R_INSN_SLL, "sll", "sll", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* sll3 $dr,$sr,$simm16 */
  {
    M32R_INSN_SLL3, "sll3", "sll3", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* slli $dr,$uimm5 */
  {
    M32R_INSN_SLLI, "slli", "slli", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* sra $dr,$sr */
  {
    M32R_INSN_SRA, "sra", "sra", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* sra3 $dr,$sr,$simm16 */
  {
    M32R_INSN_SRA3, "sra3", "sra3", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* srai $dr,$uimm5 */
  {
    M32R_INSN_SRAI, "srai", "srai", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* srl $dr,$sr */
  {
    M32R_INSN_SRL, "srl", "srl", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* srl3 $dr,$sr,$simm16 */
  {
    M32R_INSN_SRL3, "srl3", "srl3", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* srli $dr,$uimm5 */
  {
    M32R_INSN_SRLI, "srli", "srli", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* st $src1,@$src2 */
  {
    M32R_INSN_ST, "st", "st", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* st $src1,@($slo16,$src2) */
  {
    M32R_INSN_ST_D, "st-d", "st", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* stb $src1,@$src2 */
  {
    M32R_INSN_STB, "stb", "stb", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* stb $src1,@($slo16,$src2) */
  {
    M32R_INSN_STB_D, "stb-d", "stb", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* sth $src1,@$src2 */
  {
    M32R_INSN_STH, "sth", "sth", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* sth $src1,@($slo16,$src2) */
  {
    M32R_INSN_STH_D, "sth-d", "sth", 32,
    { 0, { (1<<MACH_BASE) } }
  },
/* st $src1,@+$src2 */
  {
    M32R_INSN_ST_PLUS, "st-plus", "st", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* st $src1,@-$src2 */
  {
    M32R_INSN_ST_MINUS, "st-minus", "st", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* sub $dr,$sr */
  {
    M32R_INSN_SUB, "sub", "sub", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* subv $dr,$sr */
  {
    M32R_INSN_SUBV, "subv", "subv", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* subx $dr,$sr */
  {
    M32R_INSN_SUBX, "subx", "subx", 16,
    { 0, { (1<<MACH_BASE) } }
  },
/* trap $uimm4 */
  {
    M32R_INSN_TRAP, "trap", "trap", 16,
    { 0|A(FILL_SLOT)|A(UNCOND_CTI), { (1<<MACH_BASE) } }
  },
/* unlock $src1,@$src2 */
  {
    M32R_INSN_UNLOCK, "unlock", "unlock", 16,
    { 0, { (1<<MACH_BASE) } }
  },
};

#undef A
#undef MNEM
#undef OP

/* Initialize anything needed to be done once, before any cpu_open call.  */

static void
init_tables ()
{
}

/* Subroutine of m32r_cgen_cpu_open to look up a mach via its bfd name.  */

static const CGEN_MACH *
lookup_mach_via_bfd_name (table, name)
     const CGEN_MACH *table;
     const char *name;
{
  while (table->name)
    {
      if (strcmp (name, table->bfd_name) == 0)
	return table;
      ++table;
    }
  abort ();
}

/* Subroutine of m32r_cgen_cpu_open to build the hardware table.  */

static void
build_hw_table (cd)
     CGEN_CPU_TABLE *cd;
{
  int i;
  int machs = cd->machs;
  const CGEN_HW_ENTRY *init = & m32r_cgen_hw_table[0];
  /* MAX_HW is only an upper bound on the number of selected entries.
     However each entry is indexed by it's enum so there can be holes in
     the table.  */
  const CGEN_HW_ENTRY **selected =
    (const CGEN_HW_ENTRY **) xmalloc (MAX_HW * sizeof (CGEN_HW_ENTRY *));

  cd->hw_table.init_entries = init;
  cd->hw_table.entry_size = sizeof (CGEN_HW_ENTRY);
  memset (selected, 0, MAX_HW * sizeof (CGEN_HW_ENTRY *));
  /* ??? For now we just use machs to determine which ones we want.  */
  for (i = 0; init[i].name != NULL; ++i)
    if (CGEN_HW_ATTR_VALUE (&init[i], CGEN_HW_MACH)
	& machs)
      selected[init[i].type] = &init[i];
  cd->hw_table.entries = selected;
  cd->hw_table.num_entries = MAX_HW;
}

/* Subroutine of m32r_cgen_cpu_open to build the hardware table.  */

static void
build_ifield_table (cd)
     CGEN_CPU_TABLE *cd;
{
  cd->ifld_table = & m32r_cgen_ifld_table[0];
}

/* Subroutine of m32r_cgen_cpu_open to build the hardware table.  */

static void
build_operand_table (cd)
     CGEN_CPU_TABLE *cd;
{
  int i;
  int machs = cd->machs;
  const CGEN_OPERAND *init = & m32r_cgen_operand_table[0];
  /* MAX_OPERANDS is only an upper bound on the number of selected entries.
     However each entry is indexed by it's enum so there can be holes in
     the table.  */
  const CGEN_OPERAND **selected =
    (const CGEN_OPERAND **) xmalloc (MAX_OPERANDS * sizeof (CGEN_OPERAND *));

  cd->operand_table.init_entries = init;
  cd->operand_table.entry_size = sizeof (CGEN_OPERAND);
  memset (selected, 0, MAX_OPERANDS * sizeof (CGEN_OPERAND *));
  /* ??? For now we just use mach to determine which ones we want.  */
  for (i = 0; init[i].name != NULL; ++i)
    if (CGEN_OPERAND_ATTR_VALUE (&init[i], CGEN_OPERAND_MACH)
	& machs)
      selected[init[i].type] = &init[i];
  cd->operand_table.entries = selected;
  cd->operand_table.num_entries = MAX_OPERANDS;
}

/* Subroutine of m32r_cgen_cpu_open to build the hardware table.
   ??? This could leave out insns not supported by the specified mach/isa,
   but that would cause errors like "foo only supported by bar" to become
   "unknown insn", so for now we include all insns and require the app to
   do the checking later.
   ??? On the other hand, parsing of such insns may require their hardware or
   operand elements to be in the table [which they mightn't be].  */

static void
build_insn_table (cd)
     CGEN_CPU_TABLE *cd;
{
  int i;
  const CGEN_IBASE *ib = & m32r_cgen_insn_table[0];
  CGEN_INSN *insns = (CGEN_INSN *) xmalloc (MAX_INSNS * sizeof (CGEN_INSN));

  memset (insns, 0, MAX_INSNS * sizeof (CGEN_INSN));
  for (i = 0; i < MAX_INSNS; ++i)
    insns[i].base = &ib[i];
  cd->insn_table.init_entries = insns;
  cd->insn_table.entry_size = sizeof (CGEN_IBASE);
  cd->insn_table.num_init_entries = MAX_INSNS;
}

/* Subroutine of m32r_cgen_cpu_open to rebuild the tables.  */

static void
m32r_cgen_rebuild_tables (cd)
     CGEN_CPU_TABLE *cd;
{
  int i,n_isas,n_machs;
  unsigned int isas = cd->isas;
  unsigned int machs = cd->machs;

  cd->int_insn_p = CGEN_INT_INSN_P;

  /* Data derived from the isa spec.  */
#define UNSET (CGEN_SIZE_UNKNOWN + 1)
  cd->default_insn_bitsize = UNSET;
  cd->base_insn_bitsize = UNSET;
  cd->min_insn_bitsize = 65535; /* some ridiculously big number */
  cd->max_insn_bitsize = 0;
  for (i = 0; i < MAX_ISAS; ++i)
    if (((1 << i) & isas) != 0)
      {
	const CGEN_ISA *isa = & m32r_cgen_isa_table[i];

	/* Default insn sizes of all selected isas must be equal or we set
	   the result to 0, meaning "unknown".  */
	if (cd->default_insn_bitsize == UNSET)
	  cd->default_insn_bitsize = isa->default_insn_bitsize;
	else if (isa->default_insn_bitsize == cd->default_insn_bitsize)
	  ; /* this is ok */
	else
	  cd->default_insn_bitsize = CGEN_SIZE_UNKNOWN;

	/* Base insn sizes of all selected isas must be equal or we set
	   the result to 0, meaning "unknown".  */
	if (cd->base_insn_bitsize == UNSET)
	  cd->base_insn_bitsize = isa->base_insn_bitsize;
	else if (isa->base_insn_bitsize == cd->base_insn_bitsize)
	  ; /* this is ok */
	else
	  cd->base_insn_bitsize = CGEN_SIZE_UNKNOWN;

	/* Set min,max insn sizes.  */
	if (isa->min_insn_bitsize < cd->min_insn_bitsize)
	  cd->min_insn_bitsize = isa->min_insn_bitsize;
	if (isa->max_insn_bitsize > cd->max_insn_bitsize)
	  cd->max_insn_bitsize = isa->max_insn_bitsize;

	++n_isas;
      }

  /* Data derived from the mach spec.  */
  for (i = 0; i < MAX_MACHS; ++i)
    if (((1 << i) & machs) != 0)
      {
	const CGEN_MACH *mach = & m32r_cgen_mach_table[i];

	++n_machs;
      }

  /* Determine which hw elements are used by MACH.  */
  build_hw_table (cd);

  /* Build the ifield table.  */
  build_ifield_table (cd);

  /* Determine which operands are used by MACH/ISA.  */
  build_operand_table (cd);

  /* Build the instruction table.  */
  build_insn_table (cd);
}

/* Initialize a cpu table and return a descriptor.
   It's much like opening a file, and must be the first function called.
   The arguments are a set of (type/value) pairs, terminated with
   CGEN_CPU_OPEN_END.

   Currently supported values:
   CGEN_CPU_OPEN_ISAS:    bitmap of values in enum isa_attr
   CGEN_CPU_OPEN_MACHS:   bitmap of values in enum mach_attr
   CGEN_CPU_OPEN_BFDMACH: specify 1 mach using bfd name
   CGEN_CPU_OPEN_ENDIAN:  specify endian choice
   CGEN_CPU_OPEN_END:     terminates arguments

   ??? Simultaneous multiple isas might not make sense, but it's not (yet)
   precluded.

   ??? We only support ISO C stdargs here, not K&R.
   Laziness, plus experiment to see if anything requires K&R - eventually
   K&R will no longer be supported - e.g. GDB is currently trying this.  */

CGEN_CPU_DESC
m32r_cgen_cpu_open (enum cgen_cpu_open_arg arg_type, ...)
{
  CGEN_CPU_TABLE *cd = (CGEN_CPU_TABLE *) xmalloc (sizeof (CGEN_CPU_TABLE));
  static int init_p;
  unsigned int isas = 0;  /* 0 = "unspecified" */
  unsigned int machs = 0; /* 0 = "unspecified" */
  enum cgen_endian endian = CGEN_ENDIAN_UNKNOWN;
  va_list ap;

  if (! init_p)
    {
      init_tables ();
      init_p = 1;
    }

  memset (cd, 0, sizeof (*cd));

  va_start (ap, arg_type);
  while (arg_type != CGEN_CPU_OPEN_END)
    {
      switch (arg_type)
	{
	case CGEN_CPU_OPEN_ISAS :
	  isas = va_arg (ap, unsigned int);
	  break;
	case CGEN_CPU_OPEN_MACHS :
	  machs = va_arg (ap, unsigned int);
	  break;
	case CGEN_CPU_OPEN_BFDMACH :
	  {
	    const char *name = va_arg (ap, const char *);
	    const CGEN_MACH *mach =
	      lookup_mach_via_bfd_name (m32r_cgen_mach_table, name);

	    machs |= mach->num << 1;
	    break;
	  }
	case CGEN_CPU_OPEN_ENDIAN :
	  endian = va_arg (ap, enum cgen_endian);
	  break;
	default :
	  fprintf (stderr, "m32r_cgen_cpu_open: unsupported argument `%d'\n",
		   arg_type);
	  abort (); /* ??? return NULL? */
	}
      arg_type = va_arg (ap, enum cgen_cpu_open_arg);
    }
  va_end (ap);

  /* mach unspecified means "all" */
  if (machs == 0)
    machs = (1 << MAX_MACHS) - 1;
  /* base mach is always selected */
  machs |= 1;
  /* isa unspecified means "all" */
  if (isas == 0)
    isas = (1 << MAX_ISAS) - 1;
  if (endian == CGEN_ENDIAN_UNKNOWN)
    {
      /* ??? If target has only one, could have a default.  */
      fprintf (stderr, "m32r_cgen_cpu_open: no endianness specified\n");
      abort ();
    }

  cd->isas = isas;
  cd->machs = machs;
  cd->endian = endian;
  /* FIXME: for the sparc case we can determine insn-endianness statically.
     The worry here is where both data and insn endian can be independently
     chosen, in which case this function will need another argument.
     Actually, will want to allow for more arguments in the future anyway.  */
  cd->insn_endian = endian;

  /* Table (re)builder.  */
  cd->rebuild_tables = m32r_cgen_rebuild_tables;
  m32r_cgen_rebuild_tables (cd);

  return (CGEN_CPU_DESC) cd;
}

/* Cover fn to m32r_cgen_cpu_open to handle the simple case of 1 isa, 1 mach.
   MACH_NAME is the bfd name of the mach.  */

CGEN_CPU_DESC
m32r_cgen_cpu_open_1 (mach_name, endian)
     const char *mach_name;
     enum cgen_endian endian;
{
  return m32r_cgen_cpu_open (CGEN_CPU_OPEN_BFDMACH, mach_name,
			       CGEN_CPU_OPEN_ENDIAN, endian,
			       CGEN_CPU_OPEN_END);
}

/* Close a cpu table.
   ??? This can live in a machine independent file, but there's currently
   no place to put this file (there's no libcgen).  libopcodes is the wrong
   place as some simulator ports use this but they don't use libopcodes.  */

void
m32r_cgen_cpu_close (cd)
     CGEN_CPU_DESC cd;
{
  if (cd->insn_table.init_entries)
    free ((CGEN_INSN *) cd->insn_table.init_entries);
  if (cd->hw_table.entries)
    free ((CGEN_HW_ENTRY *) cd->hw_table.entries);
  free (cd);
}

