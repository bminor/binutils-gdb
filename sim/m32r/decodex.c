/* Simulator instruction decoder for m32r.

This file is machine generated with CGEN.

Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

This file is part of the GNU Simulators.

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

#define WANT_CPU
#define WANT_CPU_M32RX

#include "sim-main.h"
#include "sim-xcat.h"
#include "cpu-sim.h"
#include "cpu-opc.h"

/* FIXME: wip, may eventually only want one form so this would then go
   away.  However, in the mean time, having both keeps a stable version
   around while the cache version is being developed.
   It may still be useful to allow two versions to exist though.  */
#if WITH_SCACHE
#define EX(fn) XCONCAT3 (m32rx,_ex_,fn)
#else
#define EX(fn) 0
#endif

#if WITH_SEM_SWITCH_FULL
#define FULL(fn) 0
#else
#define FULL(fn) XCONCAT3 (m32rx,_sem_,fn)
#endif

#if WITH_SEM_SWITCH_FAST
#define FAST(fn) 0
#else
#if WITH_SCACHE
#define FAST(fn) XCONCAT3 (m32rx,_semc_,fn)
#else
#define FAST(fn) 0
#endif
#endif

/*#define DECODE M32RX_DECODE*/

/* The decode_illegal case is currently non-static and the generator doesn't
   prepend m32rx_, so simplify things by handling it here.  */
#define decode_illegal m32rx_decode_illegal

static DECODE decode_add = { M32R_INSN_ADD, & m32r_cgen_insn_table_entries[M32R_INSN_ADD], EX (fmt_0_add), FULL (add), FAST (add) };
static DECODE decode_add3 = { M32R_INSN_ADD3, & m32r_cgen_insn_table_entries[M32R_INSN_ADD3], EX (fmt_1_add3), FULL (add3), FAST (add3) };
static DECODE decode_and = { M32R_INSN_AND, & m32r_cgen_insn_table_entries[M32R_INSN_AND], EX (fmt_0_add), FULL (and), FAST (and) };
static DECODE decode_and3 = { M32R_INSN_AND3, & m32r_cgen_insn_table_entries[M32R_INSN_AND3], EX (fmt_2_and3), FULL (and3), FAST (and3) };
static DECODE decode_or = { M32R_INSN_OR, & m32r_cgen_insn_table_entries[M32R_INSN_OR], EX (fmt_0_add), FULL (or), FAST (or) };
static DECODE decode_or3 = { M32R_INSN_OR3, & m32r_cgen_insn_table_entries[M32R_INSN_OR3], EX (fmt_3_or3), FULL (or3), FAST (or3) };
static DECODE decode_xor = { M32R_INSN_XOR, & m32r_cgen_insn_table_entries[M32R_INSN_XOR], EX (fmt_0_add), FULL (xor), FAST (xor) };
static DECODE decode_xor3 = { M32R_INSN_XOR3, & m32r_cgen_insn_table_entries[M32R_INSN_XOR3], EX (fmt_2_and3), FULL (xor3), FAST (xor3) };
static DECODE decode_addi = { M32R_INSN_ADDI, & m32r_cgen_insn_table_entries[M32R_INSN_ADDI], EX (fmt_4_addi), FULL (addi), FAST (addi) };
static DECODE decode_addv = { M32R_INSN_ADDV, & m32r_cgen_insn_table_entries[M32R_INSN_ADDV], EX (fmt_0_add), FULL (addv), FAST (addv) };
static DECODE decode_addv3 = { M32R_INSN_ADDV3, & m32r_cgen_insn_table_entries[M32R_INSN_ADDV3], EX (fmt_5_addv3), FULL (addv3), FAST (addv3) };
static DECODE decode_addx = { M32R_INSN_ADDX, & m32r_cgen_insn_table_entries[M32R_INSN_ADDX], EX (fmt_6_addx), FULL (addx), FAST (addx) };
static DECODE decode_bc8 = { M32R_INSN_BC8, & m32r_cgen_insn_table_entries[M32R_INSN_BC8], EX (fmt_7_bc8), FULL (bc8), FAST (bc8) };
static DECODE decode_bc24 = { M32R_INSN_BC24, & m32r_cgen_insn_table_entries[M32R_INSN_BC24], EX (fmt_8_bc24), FULL (bc24), FAST (bc24) };
static DECODE decode_beq = { M32R_INSN_BEQ, & m32r_cgen_insn_table_entries[M32R_INSN_BEQ], EX (fmt_9_beq), FULL (beq), FAST (beq) };
static DECODE decode_beqz = { M32R_INSN_BEQZ, & m32r_cgen_insn_table_entries[M32R_INSN_BEQZ], EX (fmt_10_beqz), FULL (beqz), FAST (beqz) };
static DECODE decode_bgez = { M32R_INSN_BGEZ, & m32r_cgen_insn_table_entries[M32R_INSN_BGEZ], EX (fmt_10_beqz), FULL (bgez), FAST (bgez) };
static DECODE decode_bgtz = { M32R_INSN_BGTZ, & m32r_cgen_insn_table_entries[M32R_INSN_BGTZ], EX (fmt_10_beqz), FULL (bgtz), FAST (bgtz) };
static DECODE decode_blez = { M32R_INSN_BLEZ, & m32r_cgen_insn_table_entries[M32R_INSN_BLEZ], EX (fmt_10_beqz), FULL (blez), FAST (blez) };
static DECODE decode_bltz = { M32R_INSN_BLTZ, & m32r_cgen_insn_table_entries[M32R_INSN_BLTZ], EX (fmt_10_beqz), FULL (bltz), FAST (bltz) };
static DECODE decode_bnez = { M32R_INSN_BNEZ, & m32r_cgen_insn_table_entries[M32R_INSN_BNEZ], EX (fmt_10_beqz), FULL (bnez), FAST (bnez) };
static DECODE decode_bl8 = { M32R_INSN_BL8, & m32r_cgen_insn_table_entries[M32R_INSN_BL8], EX (fmt_11_bl8), FULL (bl8), FAST (bl8) };
static DECODE decode_bl24 = { M32R_INSN_BL24, & m32r_cgen_insn_table_entries[M32R_INSN_BL24], EX (fmt_12_bl24), FULL (bl24), FAST (bl24) };
static DECODE decode_bcl8 = { M32R_INSN_BCL8, & m32r_cgen_insn_table_entries[M32R_INSN_BCL8], EX (fmt_13_bcl8), FULL (bcl8), FAST (bcl8) };
static DECODE decode_bcl24 = { M32R_INSN_BCL24, & m32r_cgen_insn_table_entries[M32R_INSN_BCL24], EX (fmt_14_bcl24), FULL (bcl24), FAST (bcl24) };
static DECODE decode_bnc8 = { M32R_INSN_BNC8, & m32r_cgen_insn_table_entries[M32R_INSN_BNC8], EX (fmt_7_bc8), FULL (bnc8), FAST (bnc8) };
static DECODE decode_bnc24 = { M32R_INSN_BNC24, & m32r_cgen_insn_table_entries[M32R_INSN_BNC24], EX (fmt_8_bc24), FULL (bnc24), FAST (bnc24) };
static DECODE decode_bne = { M32R_INSN_BNE, & m32r_cgen_insn_table_entries[M32R_INSN_BNE], EX (fmt_9_beq), FULL (bne), FAST (bne) };
static DECODE decode_bra8 = { M32R_INSN_BRA8, & m32r_cgen_insn_table_entries[M32R_INSN_BRA8], EX (fmt_15_bra8), FULL (bra8), FAST (bra8) };
static DECODE decode_bra24 = { M32R_INSN_BRA24, & m32r_cgen_insn_table_entries[M32R_INSN_BRA24], EX (fmt_16_bra24), FULL (bra24), FAST (bra24) };
static DECODE decode_bncl8 = { M32R_INSN_BNCL8, & m32r_cgen_insn_table_entries[M32R_INSN_BNCL8], EX (fmt_13_bcl8), FULL (bncl8), FAST (bncl8) };
static DECODE decode_bncl24 = { M32R_INSN_BNCL24, & m32r_cgen_insn_table_entries[M32R_INSN_BNCL24], EX (fmt_14_bcl24), FULL (bncl24), FAST (bncl24) };
static DECODE decode_cmp = { M32R_INSN_CMP, & m32r_cgen_insn_table_entries[M32R_INSN_CMP], EX (fmt_17_cmp), FULL (cmp), FAST (cmp) };
static DECODE decode_cmpi = { M32R_INSN_CMPI, & m32r_cgen_insn_table_entries[M32R_INSN_CMPI], EX (fmt_18_cmpi), FULL (cmpi), FAST (cmpi) };
static DECODE decode_cmpu = { M32R_INSN_CMPU, & m32r_cgen_insn_table_entries[M32R_INSN_CMPU], EX (fmt_17_cmp), FULL (cmpu), FAST (cmpu) };
static DECODE decode_cmpui = { M32R_INSN_CMPUI, & m32r_cgen_insn_table_entries[M32R_INSN_CMPUI], EX (fmt_19_cmpui), FULL (cmpui), FAST (cmpui) };
static DECODE decode_cmpeq = { M32R_INSN_CMPEQ, & m32r_cgen_insn_table_entries[M32R_INSN_CMPEQ], EX (fmt_17_cmp), FULL (cmpeq), FAST (cmpeq) };
static DECODE decode_cmpz = { M32R_INSN_CMPZ, & m32r_cgen_insn_table_entries[M32R_INSN_CMPZ], EX (fmt_20_cmpz), FULL (cmpz), FAST (cmpz) };
static DECODE decode_div = { M32R_INSN_DIV, & m32r_cgen_insn_table_entries[M32R_INSN_DIV], EX (fmt_21_div), FULL (div), FAST (div) };
static DECODE decode_divu = { M32R_INSN_DIVU, & m32r_cgen_insn_table_entries[M32R_INSN_DIVU], EX (fmt_21_div), FULL (divu), FAST (divu) };
static DECODE decode_rem = { M32R_INSN_REM, & m32r_cgen_insn_table_entries[M32R_INSN_REM], EX (fmt_21_div), FULL (rem), FAST (rem) };
static DECODE decode_remu = { M32R_INSN_REMU, & m32r_cgen_insn_table_entries[M32R_INSN_REMU], EX (fmt_21_div), FULL (remu), FAST (remu) };
static DECODE decode_jc = { M32R_INSN_JC, & m32r_cgen_insn_table_entries[M32R_INSN_JC], EX (fmt_22_jc), FULL (jc), FAST (jc) };
static DECODE decode_jnc = { M32R_INSN_JNC, & m32r_cgen_insn_table_entries[M32R_INSN_JNC], EX (fmt_22_jc), FULL (jnc), FAST (jnc) };
static DECODE decode_jl = { M32R_INSN_JL, & m32r_cgen_insn_table_entries[M32R_INSN_JL], EX (fmt_23_jl), FULL (jl), FAST (jl) };
static DECODE decode_jmp = { M32R_INSN_JMP, & m32r_cgen_insn_table_entries[M32R_INSN_JMP], EX (fmt_24_jmp), FULL (jmp), FAST (jmp) };
static DECODE decode_ld = { M32R_INSN_LD, & m32r_cgen_insn_table_entries[M32R_INSN_LD], EX (fmt_25_ld), FULL (ld), FAST (ld) };
static DECODE decode_ld_d = { M32R_INSN_LD_D, & m32r_cgen_insn_table_entries[M32R_INSN_LD_D], EX (fmt_26_ld_d), FULL (ld_d), FAST (ld_d) };
static DECODE decode_ldb = { M32R_INSN_LDB, & m32r_cgen_insn_table_entries[M32R_INSN_LDB], EX (fmt_27_ldb), FULL (ldb), FAST (ldb) };
static DECODE decode_ldb_d = { M32R_INSN_LDB_D, & m32r_cgen_insn_table_entries[M32R_INSN_LDB_D], EX (fmt_28_ldb_d), FULL (ldb_d), FAST (ldb_d) };
static DECODE decode_ldh = { M32R_INSN_LDH, & m32r_cgen_insn_table_entries[M32R_INSN_LDH], EX (fmt_29_ldh), FULL (ldh), FAST (ldh) };
static DECODE decode_ldh_d = { M32R_INSN_LDH_D, & m32r_cgen_insn_table_entries[M32R_INSN_LDH_D], EX (fmt_30_ldh_d), FULL (ldh_d), FAST (ldh_d) };
static DECODE decode_ldub = { M32R_INSN_LDUB, & m32r_cgen_insn_table_entries[M32R_INSN_LDUB], EX (fmt_27_ldb), FULL (ldub), FAST (ldub) };
static DECODE decode_ldub_d = { M32R_INSN_LDUB_D, & m32r_cgen_insn_table_entries[M32R_INSN_LDUB_D], EX (fmt_28_ldb_d), FULL (ldub_d), FAST (ldub_d) };
static DECODE decode_lduh = { M32R_INSN_LDUH, & m32r_cgen_insn_table_entries[M32R_INSN_LDUH], EX (fmt_29_ldh), FULL (lduh), FAST (lduh) };
static DECODE decode_lduh_d = { M32R_INSN_LDUH_D, & m32r_cgen_insn_table_entries[M32R_INSN_LDUH_D], EX (fmt_30_ldh_d), FULL (lduh_d), FAST (lduh_d) };
static DECODE decode_ld_plus = { M32R_INSN_LD_PLUS, & m32r_cgen_insn_table_entries[M32R_INSN_LD_PLUS], EX (fmt_25_ld), FULL (ld_plus), FAST (ld_plus) };
static DECODE decode_ld24 = { M32R_INSN_LD24, & m32r_cgen_insn_table_entries[M32R_INSN_LD24], EX (fmt_31_ld24), FULL (ld24), FAST (ld24) };
static DECODE decode_ldi8 = { M32R_INSN_LDI8, & m32r_cgen_insn_table_entries[M32R_INSN_LDI8], EX (fmt_32_ldi8), FULL (ldi8), FAST (ldi8) };
static DECODE decode_ldi16 = { M32R_INSN_LDI16, & m32r_cgen_insn_table_entries[M32R_INSN_LDI16], EX (fmt_33_ldi16), FULL (ldi16), FAST (ldi16) };
static DECODE decode_lock = { M32R_INSN_LOCK, & m32r_cgen_insn_table_entries[M32R_INSN_LOCK], EX (fmt_0_add), FULL (lock), FAST (lock) };
static DECODE decode_machi_a = { M32R_INSN_MACHI_A, & m32r_cgen_insn_table_entries[M32R_INSN_MACHI_A], EX (fmt_34_machi_a), FULL (machi_a), FAST (machi_a) };
static DECODE decode_maclo_a = { M32R_INSN_MACLO_A, & m32r_cgen_insn_table_entries[M32R_INSN_MACLO_A], EX (fmt_34_machi_a), FULL (maclo_a), FAST (maclo_a) };
static DECODE decode_mul = { M32R_INSN_MUL, & m32r_cgen_insn_table_entries[M32R_INSN_MUL], EX (fmt_0_add), FULL (mul), FAST (mul) };
static DECODE decode_mulhi_a = { M32R_INSN_MULHI_A, & m32r_cgen_insn_table_entries[M32R_INSN_MULHI_A], EX (fmt_35_mulhi_a), FULL (mulhi_a), FAST (mulhi_a) };
static DECODE decode_mullo_a = { M32R_INSN_MULLO_A, & m32r_cgen_insn_table_entries[M32R_INSN_MULLO_A], EX (fmt_35_mulhi_a), FULL (mullo_a), FAST (mullo_a) };
static DECODE decode_mv = { M32R_INSN_MV, & m32r_cgen_insn_table_entries[M32R_INSN_MV], EX (fmt_36_mv), FULL (mv), FAST (mv) };
static DECODE decode_mvfachi_a = { M32R_INSN_MVFACHI_A, & m32r_cgen_insn_table_entries[M32R_INSN_MVFACHI_A], EX (fmt_37_mvfachi_a), FULL (mvfachi_a), FAST (mvfachi_a) };
static DECODE decode_mvfaclo_a = { M32R_INSN_MVFACLO_A, & m32r_cgen_insn_table_entries[M32R_INSN_MVFACLO_A], EX (fmt_37_mvfachi_a), FULL (mvfaclo_a), FAST (mvfaclo_a) };
static DECODE decode_mvfacmi_a = { M32R_INSN_MVFACMI_A, & m32r_cgen_insn_table_entries[M32R_INSN_MVFACMI_A], EX (fmt_37_mvfachi_a), FULL (mvfacmi_a), FAST (mvfacmi_a) };
static DECODE decode_mvfc = { M32R_INSN_MVFC, & m32r_cgen_insn_table_entries[M32R_INSN_MVFC], EX (fmt_38_mvfc), FULL (mvfc), FAST (mvfc) };
static DECODE decode_mvtachi_a = { M32R_INSN_MVTACHI_A, & m32r_cgen_insn_table_entries[M32R_INSN_MVTACHI_A], EX (fmt_39_mvtachi_a), FULL (mvtachi_a), FAST (mvtachi_a) };
static DECODE decode_mvtaclo_a = { M32R_INSN_MVTACLO_A, & m32r_cgen_insn_table_entries[M32R_INSN_MVTACLO_A], EX (fmt_39_mvtachi_a), FULL (mvtaclo_a), FAST (mvtaclo_a) };
static DECODE decode_mvtc = { M32R_INSN_MVTC, & m32r_cgen_insn_table_entries[M32R_INSN_MVTC], EX (fmt_40_mvtc), FULL (mvtc), FAST (mvtc) };
static DECODE decode_neg = { M32R_INSN_NEG, & m32r_cgen_insn_table_entries[M32R_INSN_NEG], EX (fmt_36_mv), FULL (neg), FAST (neg) };
static DECODE decode_nop = { M32R_INSN_NOP, & m32r_cgen_insn_table_entries[M32R_INSN_NOP], EX (fmt_41_nop), FULL (nop), FAST (nop) };
static DECODE decode_not = { M32R_INSN_NOT, & m32r_cgen_insn_table_entries[M32R_INSN_NOT], EX (fmt_36_mv), FULL (not), FAST (not) };
static DECODE decode_rac_a = { M32R_INSN_RAC_A, & m32r_cgen_insn_table_entries[M32R_INSN_RAC_A], EX (fmt_42_rac_a), FULL (rac_a), FAST (rac_a) };
static DECODE decode_rach_a = { M32R_INSN_RACH_A, & m32r_cgen_insn_table_entries[M32R_INSN_RACH_A], EX (fmt_42_rac_a), FULL (rach_a), FAST (rach_a) };
static DECODE decode_rte = { M32R_INSN_RTE, & m32r_cgen_insn_table_entries[M32R_INSN_RTE], EX (fmt_43_rte), FULL (rte), FAST (rte) };
static DECODE decode_seth = { M32R_INSN_SETH, & m32r_cgen_insn_table_entries[M32R_INSN_SETH], EX (fmt_44_seth), FULL (seth), FAST (seth) };
static DECODE decode_sll = { M32R_INSN_SLL, & m32r_cgen_insn_table_entries[M32R_INSN_SLL], EX (fmt_0_add), FULL (sll), FAST (sll) };
static DECODE decode_sll3 = { M32R_INSN_SLL3, & m32r_cgen_insn_table_entries[M32R_INSN_SLL3], EX (fmt_5_addv3), FULL (sll3), FAST (sll3) };
static DECODE decode_slli = { M32R_INSN_SLLI, & m32r_cgen_insn_table_entries[M32R_INSN_SLLI], EX (fmt_45_slli), FULL (slli), FAST (slli) };
static DECODE decode_sra = { M32R_INSN_SRA, & m32r_cgen_insn_table_entries[M32R_INSN_SRA], EX (fmt_0_add), FULL (sra), FAST (sra) };
static DECODE decode_sra3 = { M32R_INSN_SRA3, & m32r_cgen_insn_table_entries[M32R_INSN_SRA3], EX (fmt_5_addv3), FULL (sra3), FAST (sra3) };
static DECODE decode_srai = { M32R_INSN_SRAI, & m32r_cgen_insn_table_entries[M32R_INSN_SRAI], EX (fmt_45_slli), FULL (srai), FAST (srai) };
static DECODE decode_srl = { M32R_INSN_SRL, & m32r_cgen_insn_table_entries[M32R_INSN_SRL], EX (fmt_0_add), FULL (srl), FAST (srl) };
static DECODE decode_srl3 = { M32R_INSN_SRL3, & m32r_cgen_insn_table_entries[M32R_INSN_SRL3], EX (fmt_5_addv3), FULL (srl3), FAST (srl3) };
static DECODE decode_srli = { M32R_INSN_SRLI, & m32r_cgen_insn_table_entries[M32R_INSN_SRLI], EX (fmt_45_slli), FULL (srli), FAST (srli) };
static DECODE decode_st = { M32R_INSN_ST, & m32r_cgen_insn_table_entries[M32R_INSN_ST], EX (fmt_17_cmp), FULL (st), FAST (st) };
static DECODE decode_st_d = { M32R_INSN_ST_D, & m32r_cgen_insn_table_entries[M32R_INSN_ST_D], EX (fmt_46_st_d), FULL (st_d), FAST (st_d) };
static DECODE decode_stb = { M32R_INSN_STB, & m32r_cgen_insn_table_entries[M32R_INSN_STB], EX (fmt_17_cmp), FULL (stb), FAST (stb) };
static DECODE decode_stb_d = { M32R_INSN_STB_D, & m32r_cgen_insn_table_entries[M32R_INSN_STB_D], EX (fmt_46_st_d), FULL (stb_d), FAST (stb_d) };
static DECODE decode_sth = { M32R_INSN_STH, & m32r_cgen_insn_table_entries[M32R_INSN_STH], EX (fmt_17_cmp), FULL (sth), FAST (sth) };
static DECODE decode_sth_d = { M32R_INSN_STH_D, & m32r_cgen_insn_table_entries[M32R_INSN_STH_D], EX (fmt_46_st_d), FULL (sth_d), FAST (sth_d) };
static DECODE decode_st_plus = { M32R_INSN_ST_PLUS, & m32r_cgen_insn_table_entries[M32R_INSN_ST_PLUS], EX (fmt_17_cmp), FULL (st_plus), FAST (st_plus) };
static DECODE decode_st_minus = { M32R_INSN_ST_MINUS, & m32r_cgen_insn_table_entries[M32R_INSN_ST_MINUS], EX (fmt_17_cmp), FULL (st_minus), FAST (st_minus) };
static DECODE decode_sub = { M32R_INSN_SUB, & m32r_cgen_insn_table_entries[M32R_INSN_SUB], EX (fmt_0_add), FULL (sub), FAST (sub) };
static DECODE decode_subv = { M32R_INSN_SUBV, & m32r_cgen_insn_table_entries[M32R_INSN_SUBV], EX (fmt_0_add), FULL (subv), FAST (subv) };
static DECODE decode_subx = { M32R_INSN_SUBX, & m32r_cgen_insn_table_entries[M32R_INSN_SUBX], EX (fmt_6_addx), FULL (subx), FAST (subx) };
static DECODE decode_trap = { M32R_INSN_TRAP, & m32r_cgen_insn_table_entries[M32R_INSN_TRAP], EX (fmt_47_trap), FULL (trap), FAST (trap) };
static DECODE decode_unlock = { M32R_INSN_UNLOCK, & m32r_cgen_insn_table_entries[M32R_INSN_UNLOCK], EX (fmt_17_cmp), FULL (unlock), FAST (unlock) };
static DECODE decode_satb = { M32R_INSN_SATB, & m32r_cgen_insn_table_entries[M32R_INSN_SATB], EX (fmt_48_satb), FULL (satb), FAST (satb) };
static DECODE decode_sath = { M32R_INSN_SATH, & m32r_cgen_insn_table_entries[M32R_INSN_SATH], EX (fmt_48_satb), FULL (sath), FAST (sath) };
static DECODE decode_sat = { M32R_INSN_SAT, & m32r_cgen_insn_table_entries[M32R_INSN_SAT], EX (fmt_49_sat), FULL (sat), FAST (sat) };
static DECODE decode_pcmpbz = { M32R_INSN_PCMPBZ, & m32r_cgen_insn_table_entries[M32R_INSN_PCMPBZ], EX (fmt_20_cmpz), FULL (pcmpbz), FAST (pcmpbz) };
static DECODE decode_sadd = { M32R_INSN_SADD, & m32r_cgen_insn_table_entries[M32R_INSN_SADD], EX (fmt_50_sadd), FULL (sadd), FAST (sadd) };
static DECODE decode_macwu1 = { M32R_INSN_MACWU1, & m32r_cgen_insn_table_entries[M32R_INSN_MACWU1], EX (fmt_51_macwu1), FULL (macwu1), FAST (macwu1) };
static DECODE decode_msblo = { M32R_INSN_MSBLO, & m32r_cgen_insn_table_entries[M32R_INSN_MSBLO], EX (fmt_52_msblo), FULL (msblo), FAST (msblo) };
static DECODE decode_mulwu1 = { M32R_INSN_MULWU1, & m32r_cgen_insn_table_entries[M32R_INSN_MULWU1], EX (fmt_17_cmp), FULL (mulwu1), FAST (mulwu1) };
static DECODE decode_machl1 = { M32R_INSN_MACHL1, & m32r_cgen_insn_table_entries[M32R_INSN_MACHL1], EX (fmt_51_macwu1), FULL (machl1), FAST (machl1) };
static DECODE decode_sc = { M32R_INSN_SC, & m32r_cgen_insn_table_entries[M32R_INSN_SC], EX (fmt_53_sc), FULL (sc), FAST (sc) };
static DECODE decode_snc = { M32R_INSN_SNC, & m32r_cgen_insn_table_entries[M32R_INSN_SNC], EX (fmt_53_sc), FULL (snc), FAST (snc) };
DECODE m32rx_decode_illegal = {
  M32R_INSN_ILLEGAL, & m32r_cgen_insn_table_entries[0],
  EX (illegal), FULL (illegal),
  FAST (illegal)
};

/* The order must match that of `labels' in sem-switch.c.  */

DECODE *m32rx_decode_vars[] = {
  & m32rx_decode_illegal,
  & decode_add,
  & decode_add3,
  & decode_and,
  & decode_and3,
  & decode_or,
  & decode_or3,
  & decode_xor,
  & decode_xor3,
  & decode_addi,
  & decode_addv,
  & decode_addv3,
  & decode_addx,
  & decode_bc8,
  & decode_bc24,
  & decode_beq,
  & decode_beqz,
  & decode_bgez,
  & decode_bgtz,
  & decode_blez,
  & decode_bltz,
  & decode_bnez,
  & decode_bl8,
  & decode_bl24,
  & decode_bcl8,
  & decode_bcl24,
  & decode_bnc8,
  & decode_bnc24,
  & decode_bne,
  & decode_bra8,
  & decode_bra24,
  & decode_bncl8,
  & decode_bncl24,
  & decode_cmp,
  & decode_cmpi,
  & decode_cmpu,
  & decode_cmpui,
  & decode_cmpeq,
  & decode_cmpz,
  & decode_div,
  & decode_divu,
  & decode_rem,
  & decode_remu,
  & decode_jc,
  & decode_jnc,
  & decode_jl,
  & decode_jmp,
  & decode_ld,
  & decode_ld_d,
  & decode_ldb,
  & decode_ldb_d,
  & decode_ldh,
  & decode_ldh_d,
  & decode_ldub,
  & decode_ldub_d,
  & decode_lduh,
  & decode_lduh_d,
  & decode_ld_plus,
  & decode_ld24,
  & decode_ldi8,
  & decode_ldi16,
  & decode_lock,
  & decode_machi_a,
  & decode_maclo_a,
  & decode_mul,
  & decode_mulhi_a,
  & decode_mullo_a,
  & decode_mv,
  & decode_mvfachi_a,
  & decode_mvfaclo_a,
  & decode_mvfacmi_a,
  & decode_mvfc,
  & decode_mvtachi_a,
  & decode_mvtaclo_a,
  & decode_mvtc,
  & decode_neg,
  & decode_nop,
  & decode_not,
  & decode_rac_a,
  & decode_rach_a,
  & decode_rte,
  & decode_seth,
  & decode_sll,
  & decode_sll3,
  & decode_slli,
  & decode_sra,
  & decode_sra3,
  & decode_srai,
  & decode_srl,
  & decode_srl3,
  & decode_srli,
  & decode_st,
  & decode_st_d,
  & decode_stb,
  & decode_stb_d,
  & decode_sth,
  & decode_sth_d,
  & decode_st_plus,
  & decode_st_minus,
  & decode_sub,
  & decode_subv,
  & decode_subx,
  & decode_trap,
  & decode_unlock,
  & decode_satb,
  & decode_sath,
  & decode_sat,
  & decode_pcmpbz,
  & decode_sadd,
  & decode_macwu1,
  & decode_msblo,
  & decode_mulwu1,
  & decode_machl1,
  & decode_sc,
  & decode_snc,
  0
};

/* The decoder needs a slightly different computed goto switch control.  */
#ifdef __GNUC__
#define DECODE_SWITCH(N, X) goto *labels_##N[X];
#else
#define DECODE_SWITCH(N, X) switch (X)
#endif

/* Given an instruction, return a pointer to its DECODE entry.  */

DECODE *
m32rx_decode (current_cpu, pc, insn)
     SIM_CPU *current_cpu;
     PCADDR pc;
     insn_t insn;
{
  {
#ifdef __GNUC__
    static void *labels_0[256] = {
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && case_0_7, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && case_0_28, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && case_0_87, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && case_0_95, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && case_0_112, && case_0_113, && case_0_114, && case_0_115, 
      && case_0_116, && case_0_117, && case_0_118, && case_0_119, 
      && case_0_120, && case_0_121, && case_0_122, && case_0_123, 
      && case_0_124, && case_0_125, && case_0_126, && case_0_127, 
      && case_0_128, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && default_0, && default_0, && default_0, && default_0, 
      && case_0_240, && case_0_241, && case_0_242, && case_0_243, 
      && case_0_244, && case_0_245, && case_0_246, && case_0_247, 
      && case_0_248, && case_0_249, && case_0_250, && case_0_251, 
      && case_0_252, && case_0_253, && case_0_254, && case_0_255, 
    };
#endif
    static DECODE *insns[256] = {
      &decode_subv, &decode_subx, &decode_sub, &decode_neg, 
      &decode_cmp, &decode_cmpu, &decode_cmpeq, 0, 
      &decode_addv, &decode_addx, &decode_add, &decode_not, 
      &decode_and, &decode_xor, &decode_or, &decode_illegal, 
      &decode_srl, &decode_illegal, &decode_sra, &decode_illegal, 
      &decode_sll, &decode_illegal, &decode_mul, &decode_illegal, 
      &decode_mv, &decode_mvfc, &decode_mvtc, &decode_illegal, 
      0, &decode_rte, &decode_illegal, &decode_trap, 
      &decode_stb, &decode_illegal, &decode_sth, &decode_illegal, 
      &decode_st, &decode_unlock, &decode_st_plus, &decode_st_minus, 
      &decode_ldb, &decode_ldub, &decode_ldh, &decode_lduh, 
      &decode_ld, &decode_lock, &decode_ld_plus, &decode_illegal, 
      &decode_mulhi_a, &decode_mullo_a, &decode_illegal, &decode_illegal, 
      &decode_machi_a, &decode_maclo_a, &decode_illegal, &decode_illegal, 
      &decode_mulhi_a, &decode_mullo_a, &decode_illegal, &decode_illegal, 
      &decode_machi_a, &decode_maclo_a, &decode_illegal, &decode_illegal, 
      &decode_addi, &decode_addi, &decode_addi, &decode_addi, 
      &decode_addi, &decode_addi, &decode_addi, &decode_addi, 
      &decode_addi, &decode_addi, &decode_addi, &decode_addi, 
      &decode_addi, &decode_addi, &decode_addi, &decode_addi, 
      &decode_srli, &decode_srli, &decode_srai, &decode_srai, 
      &decode_slli, &decode_slli, &decode_illegal, 0, 
      &decode_rach_a, &decode_rac_a, &decode_mulwu1, &decode_macwu1, 
      &decode_machl1, &decode_msblo, &decode_sadd, 0, 
      &decode_ldi8, &decode_ldi8, &decode_ldi8, &decode_ldi8, 
      &decode_ldi8, &decode_ldi8, &decode_ldi8, &decode_ldi8, 
      &decode_ldi8, &decode_ldi8, &decode_ldi8, &decode_ldi8, 
      &decode_ldi8, &decode_ldi8, &decode_ldi8, &decode_ldi8, 
      0, 0, 0, 0, 
      0, 0, 0, 0, 
      0, 0, 0, 0, 
      0, 0, 0, 0, 
      0, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_cmpi, &decode_cmpui, &decode_illegal, &decode_illegal, 
      &decode_addv3, &decode_illegal, &decode_add3, &decode_illegal, 
      &decode_and3, &decode_xor3, &decode_or3, &decode_illegal, 
      &decode_div, &decode_divu, &decode_rem, &decode_remu, 
      &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_srl3, &decode_illegal, &decode_sra3, &decode_illegal, 
      &decode_sll3, &decode_illegal, &decode_illegal, &decode_ldi16, 
      &decode_stb_d, &decode_illegal, &decode_sth_d, &decode_illegal, 
      &decode_st_d, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_ldb_d, &decode_ldub_d, &decode_ldh_d, &decode_lduh_d, 
      &decode_ld_d, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_beq, &decode_bne, &decode_illegal, &decode_illegal, 
      &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_beqz, &decode_bnez, &decode_bltz, &decode_bgez, 
      &decode_blez, &decode_bgtz, &decode_illegal, &decode_illegal, 
      &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_seth, &decode_illegal, &decode_illegal, &decode_illegal, 
      &decode_ld24, &decode_ld24, &decode_ld24, &decode_ld24, 
      &decode_ld24, &decode_ld24, &decode_ld24, &decode_ld24, 
      &decode_ld24, &decode_ld24, &decode_ld24, &decode_ld24, 
      &decode_ld24, &decode_ld24, &decode_ld24, &decode_ld24, 
      0, 0, 0, 0, 
      0, 0, 0, 0, 
      0, 0, 0, 0, 
      0, 0, 0, 0, 
    };
    unsigned int val;
    val = (((insn >> 8) & (15 << 4)) | ((insn >> 4) & (15 << 0)));
    DECODE_SWITCH (0, val)
      {
      CASE (0, 7) :
        {
          static DECODE *insns[16] = {
            &decode_cmpz, &decode_illegal, &decode_illegal, &decode_pcmpbz, 
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
          };
          unsigned int val = (((insn >> 8) & (15 << 0)));
          return insns[val];
        }
      CASE (0, 28) :
        {
          static DECODE *insns[16] = {
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_jc, &decode_jnc, &decode_jl, &decode_jmp, 
          };
          unsigned int val = (((insn >> 8) & (15 << 0)));
          return insns[val];
        }
      CASE (0, 87) :
        {
          static DECODE *insns[4] = {
            &decode_mvtachi_a, &decode_mvtaclo_a, &decode_illegal, &decode_illegal, 
          };
          unsigned int val = (((insn >> 0) & (3 << 0)));
          return insns[val];
        }
      CASE (0, 95) :
        {
          static DECODE *insns[4] = {
            &decode_mvfachi_a, &decode_mvfaclo_a, &decode_mvfacmi_a, &decode_illegal, 
          };
          unsigned int val = (((insn >> 0) & (3 << 0)));
          return insns[val];
        }
      CASE (0, 112) :
        {
          static DECODE *insns[16] = {
            &decode_nop, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_sc, &decode_snc, &decode_illegal, &decode_illegal, 
            &decode_bcl8, &decode_bncl8, &decode_illegal, &decode_illegal, 
            &decode_bc8, &decode_bnc8, &decode_bl8, &decode_bra8, 
          };
          unsigned int val = (((insn >> 8) & (15 << 0)));
          return insns[val];
        }
      CASE (0, 113) : /* fall through */
      CASE (0, 114) : /* fall through */
      CASE (0, 115) : /* fall through */
      CASE (0, 116) : /* fall through */
      CASE (0, 117) : /* fall through */
      CASE (0, 118) : /* fall through */
      CASE (0, 119) : /* fall through */
      CASE (0, 120) : /* fall through */
      CASE (0, 121) : /* fall through */
      CASE (0, 122) : /* fall through */
      CASE (0, 123) : /* fall through */
      CASE (0, 124) : /* fall through */
      CASE (0, 125) : /* fall through */
      CASE (0, 126) : /* fall through */
      CASE (0, 127) :
        {
          static DECODE *insns[16] = {
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_bcl8, &decode_bncl8, &decode_illegal, &decode_illegal, 
            &decode_bc8, &decode_bnc8, &decode_bl8, &decode_bra8, 
          };
          unsigned int val = (((insn >> 8) & (15 << 0)));
          return insns[val];
        }
      CASE (0, 128) :
        {
#ifdef __GNUC__
          static void *labels_0_128[16] = {
            && case_0_128_0, && default_0_128, && default_0_128, && default_0_128, 
            && default_0_128, && default_0_128, && default_0_128, && default_0_128, 
            && default_0_128, && default_0_128, && default_0_128, && default_0_128, 
            && default_0_128, && default_0_128, && default_0_128, && default_0_128, 
          };
#endif
          static DECODE *insns[16] = {
            0, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
          };
          unsigned int val;
          /* Must fetch more bits.  */
          insn = GETIMEMUHI (current_cpu, CPU (h_pc) + 2);
          val = (((insn >> 12) & (15 << 0)));
          DECODE_SWITCH (0_128, val)
            {
            CASE (0_128, 0) :
              {
                static DECODE *insns[16] = {
                  &decode_sat, &decode_satb, &decode_sath, &decode_illegal, 
                  &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
                  &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
                  &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
                };
                unsigned int val = (((insn >> 8) & (15 << 0)));
                return insns[val];
              }
            DEFAULT (0_128) : return insns[val];
            }
          ENDSWITCH (0_128)
        }
      CASE (0, 240) : /* fall through */
      CASE (0, 241) : /* fall through */
      CASE (0, 242) : /* fall through */
      CASE (0, 243) : /* fall through */
      CASE (0, 244) : /* fall through */
      CASE (0, 245) : /* fall through */
      CASE (0, 246) : /* fall through */
      CASE (0, 247) : /* fall through */
      CASE (0, 248) : /* fall through */
      CASE (0, 249) : /* fall through */
      CASE (0, 250) : /* fall through */
      CASE (0, 251) : /* fall through */
      CASE (0, 252) : /* fall through */
      CASE (0, 253) : /* fall through */
      CASE (0, 254) : /* fall through */
      CASE (0, 255) :
        {
          static DECODE *insns[16] = {
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_illegal, &decode_illegal, &decode_illegal, &decode_illegal, 
            &decode_bcl24, &decode_bncl24, &decode_illegal, &decode_illegal, 
            &decode_bc24, &decode_bnc24, &decode_bl24, &decode_bra24, 
          };
          unsigned int val = (((insn >> 8) & (15 << 0)));
          return insns[val];
        }
      DEFAULT (0) : return insns[val];
      }
    ENDSWITCH (0)
  }
}
