/* Decode header for m32rx.

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

#ifndef M32RX_DECODE_H
#define M32RX_DECODE_H

#define EX(fn) XCONCAT3 (m32rx,_ex_,fn)
#define SEM(fn) XCONCAT3 (m32rx,_sem_,fn)
#define SEMC(fn) XCONCAT3 (m32rx,_semc_,fn)

extern EXTRACT_FN EX (illegal);
extern SEMANTIC_FN SEM (illegal);
extern SEMANTIC_CACHE_FN SEMC (illegal);

extern EXTRACT_FN EX (fmt_0_add);
extern EXTRACT_FN EX (fmt_1_add3);
extern EXTRACT_FN EX (fmt_2_and3);
extern EXTRACT_FN EX (fmt_3_or3);
extern EXTRACT_FN EX (fmt_4_addi);
extern EXTRACT_FN EX (fmt_5_addv3);
extern EXTRACT_FN EX (fmt_6_addx);
extern EXTRACT_FN EX (fmt_7_bc8);
extern EXTRACT_FN EX (fmt_8_bc24);
extern EXTRACT_FN EX (fmt_9_beq);
extern EXTRACT_FN EX (fmt_10_beqz);
extern EXTRACT_FN EX (fmt_11_bl8);
extern EXTRACT_FN EX (fmt_12_bl24);
extern EXTRACT_FN EX (fmt_13_bcl8);
extern EXTRACT_FN EX (fmt_14_bcl24);
extern EXTRACT_FN EX (fmt_15_bra8);
extern EXTRACT_FN EX (fmt_16_bra24);
extern EXTRACT_FN EX (fmt_17_cmp);
extern EXTRACT_FN EX (fmt_18_cmpi);
extern EXTRACT_FN EX (fmt_19_cmpui);
extern EXTRACT_FN EX (fmt_20_cmpz);
extern EXTRACT_FN EX (fmt_21_div);
extern EXTRACT_FN EX (fmt_22_jc);
extern EXTRACT_FN EX (fmt_23_jl);
extern EXTRACT_FN EX (fmt_24_jmp);
extern EXTRACT_FN EX (fmt_25_ld);
extern EXTRACT_FN EX (fmt_26_ld_d);
extern EXTRACT_FN EX (fmt_27_ldb);
extern EXTRACT_FN EX (fmt_28_ldb_d);
extern EXTRACT_FN EX (fmt_29_ldh);
extern EXTRACT_FN EX (fmt_30_ldh_d);
extern EXTRACT_FN EX (fmt_31_ld24);
extern EXTRACT_FN EX (fmt_32_ldi8);
extern EXTRACT_FN EX (fmt_33_ldi16);
extern EXTRACT_FN EX (fmt_34_machi_a);
extern EXTRACT_FN EX (fmt_35_mulhi_a);
extern EXTRACT_FN EX (fmt_36_mv);
extern EXTRACT_FN EX (fmt_37_mvfachi_a);
extern EXTRACT_FN EX (fmt_38_mvfc);
extern EXTRACT_FN EX (fmt_39_mvtachi_a);
extern EXTRACT_FN EX (fmt_40_mvtc);
extern EXTRACT_FN EX (fmt_41_nop);
extern EXTRACT_FN EX (fmt_42_rac_d);
extern EXTRACT_FN EX (fmt_43_rac_ds);
extern EXTRACT_FN EX (fmt_44_rac_dsi);
extern EXTRACT_FN EX (fmt_45_rte);
extern EXTRACT_FN EX (fmt_46_seth);
extern EXTRACT_FN EX (fmt_47_slli);
extern EXTRACT_FN EX (fmt_48_st_d);
extern EXTRACT_FN EX (fmt_49_trap);
extern EXTRACT_FN EX (fmt_50_satb);
extern EXTRACT_FN EX (fmt_51_sat);
extern EXTRACT_FN EX (fmt_52_sadd);
extern EXTRACT_FN EX (fmt_53_macwu1);
extern EXTRACT_FN EX (fmt_54_msblo);
extern EXTRACT_FN EX (fmt_55_sc);

extern SEMANTIC_FN SEM (add);
extern SEMANTIC_CACHE_FN SEMC (add);
extern SEMANTIC_FN SEM (add3);
extern SEMANTIC_CACHE_FN SEMC (add3);
extern SEMANTIC_FN SEM (and);
extern SEMANTIC_CACHE_FN SEMC (and);
extern SEMANTIC_FN SEM (and3);
extern SEMANTIC_CACHE_FN SEMC (and3);
extern SEMANTIC_FN SEM (or);
extern SEMANTIC_CACHE_FN SEMC (or);
extern SEMANTIC_FN SEM (or3);
extern SEMANTIC_CACHE_FN SEMC (or3);
extern SEMANTIC_FN SEM (xor);
extern SEMANTIC_CACHE_FN SEMC (xor);
extern SEMANTIC_FN SEM (xor3);
extern SEMANTIC_CACHE_FN SEMC (xor3);
extern SEMANTIC_FN SEM (addi);
extern SEMANTIC_CACHE_FN SEMC (addi);
extern SEMANTIC_FN SEM (addv);
extern SEMANTIC_CACHE_FN SEMC (addv);
extern SEMANTIC_FN SEM (addv3);
extern SEMANTIC_CACHE_FN SEMC (addv3);
extern SEMANTIC_FN SEM (addx);
extern SEMANTIC_CACHE_FN SEMC (addx);
extern SEMANTIC_FN SEM (bc8);
extern SEMANTIC_CACHE_FN SEMC (bc8);
extern SEMANTIC_FN SEM (bc24);
extern SEMANTIC_CACHE_FN SEMC (bc24);
extern SEMANTIC_FN SEM (beq);
extern SEMANTIC_CACHE_FN SEMC (beq);
extern SEMANTIC_FN SEM (beqz);
extern SEMANTIC_CACHE_FN SEMC (beqz);
extern SEMANTIC_FN SEM (bgez);
extern SEMANTIC_CACHE_FN SEMC (bgez);
extern SEMANTIC_FN SEM (bgtz);
extern SEMANTIC_CACHE_FN SEMC (bgtz);
extern SEMANTIC_FN SEM (blez);
extern SEMANTIC_CACHE_FN SEMC (blez);
extern SEMANTIC_FN SEM (bltz);
extern SEMANTIC_CACHE_FN SEMC (bltz);
extern SEMANTIC_FN SEM (bnez);
extern SEMANTIC_CACHE_FN SEMC (bnez);
extern SEMANTIC_FN SEM (bl8);
extern SEMANTIC_CACHE_FN SEMC (bl8);
extern SEMANTIC_FN SEM (bl24);
extern SEMANTIC_CACHE_FN SEMC (bl24);
extern SEMANTIC_FN SEM (bcl8);
extern SEMANTIC_CACHE_FN SEMC (bcl8);
extern SEMANTIC_FN SEM (bcl24);
extern SEMANTIC_CACHE_FN SEMC (bcl24);
extern SEMANTIC_FN SEM (bnc8);
extern SEMANTIC_CACHE_FN SEMC (bnc8);
extern SEMANTIC_FN SEM (bnc24);
extern SEMANTIC_CACHE_FN SEMC (bnc24);
extern SEMANTIC_FN SEM (bne);
extern SEMANTIC_CACHE_FN SEMC (bne);
extern SEMANTIC_FN SEM (bra8);
extern SEMANTIC_CACHE_FN SEMC (bra8);
extern SEMANTIC_FN SEM (bra24);
extern SEMANTIC_CACHE_FN SEMC (bra24);
extern SEMANTIC_FN SEM (bncl8);
extern SEMANTIC_CACHE_FN SEMC (bncl8);
extern SEMANTIC_FN SEM (bncl24);
extern SEMANTIC_CACHE_FN SEMC (bncl24);
extern SEMANTIC_FN SEM (cmp);
extern SEMANTIC_CACHE_FN SEMC (cmp);
extern SEMANTIC_FN SEM (cmpi);
extern SEMANTIC_CACHE_FN SEMC (cmpi);
extern SEMANTIC_FN SEM (cmpu);
extern SEMANTIC_CACHE_FN SEMC (cmpu);
extern SEMANTIC_FN SEM (cmpui);
extern SEMANTIC_CACHE_FN SEMC (cmpui);
extern SEMANTIC_FN SEM (cmpeq);
extern SEMANTIC_CACHE_FN SEMC (cmpeq);
extern SEMANTIC_FN SEM (cmpz);
extern SEMANTIC_CACHE_FN SEMC (cmpz);
extern SEMANTIC_FN SEM (div);
extern SEMANTIC_CACHE_FN SEMC (div);
extern SEMANTIC_FN SEM (divu);
extern SEMANTIC_CACHE_FN SEMC (divu);
extern SEMANTIC_FN SEM (rem);
extern SEMANTIC_CACHE_FN SEMC (rem);
extern SEMANTIC_FN SEM (remu);
extern SEMANTIC_CACHE_FN SEMC (remu);
extern SEMANTIC_FN SEM (divh);
extern SEMANTIC_CACHE_FN SEMC (divh);
extern SEMANTIC_FN SEM (jc);
extern SEMANTIC_CACHE_FN SEMC (jc);
extern SEMANTIC_FN SEM (jnc);
extern SEMANTIC_CACHE_FN SEMC (jnc);
extern SEMANTIC_FN SEM (jl);
extern SEMANTIC_CACHE_FN SEMC (jl);
extern SEMANTIC_FN SEM (jmp);
extern SEMANTIC_CACHE_FN SEMC (jmp);
extern SEMANTIC_FN SEM (ld);
extern SEMANTIC_CACHE_FN SEMC (ld);
extern SEMANTIC_FN SEM (ld_d);
extern SEMANTIC_CACHE_FN SEMC (ld_d);
extern SEMANTIC_FN SEM (ldb);
extern SEMANTIC_CACHE_FN SEMC (ldb);
extern SEMANTIC_FN SEM (ldb_d);
extern SEMANTIC_CACHE_FN SEMC (ldb_d);
extern SEMANTIC_FN SEM (ldh);
extern SEMANTIC_CACHE_FN SEMC (ldh);
extern SEMANTIC_FN SEM (ldh_d);
extern SEMANTIC_CACHE_FN SEMC (ldh_d);
extern SEMANTIC_FN SEM (ldub);
extern SEMANTIC_CACHE_FN SEMC (ldub);
extern SEMANTIC_FN SEM (ldub_d);
extern SEMANTIC_CACHE_FN SEMC (ldub_d);
extern SEMANTIC_FN SEM (lduh);
extern SEMANTIC_CACHE_FN SEMC (lduh);
extern SEMANTIC_FN SEM (lduh_d);
extern SEMANTIC_CACHE_FN SEMC (lduh_d);
extern SEMANTIC_FN SEM (ld_plus);
extern SEMANTIC_CACHE_FN SEMC (ld_plus);
extern SEMANTIC_FN SEM (ld24);
extern SEMANTIC_CACHE_FN SEMC (ld24);
extern SEMANTIC_FN SEM (ldi8);
extern SEMANTIC_CACHE_FN SEMC (ldi8);
extern SEMANTIC_FN SEM (ldi16);
extern SEMANTIC_CACHE_FN SEMC (ldi16);
extern SEMANTIC_FN SEM (lock);
extern SEMANTIC_CACHE_FN SEMC (lock);
extern SEMANTIC_FN SEM (machi_a);
extern SEMANTIC_CACHE_FN SEMC (machi_a);
extern SEMANTIC_FN SEM (maclo_a);
extern SEMANTIC_CACHE_FN SEMC (maclo_a);
extern SEMANTIC_FN SEM (mul);
extern SEMANTIC_CACHE_FN SEMC (mul);
extern SEMANTIC_FN SEM (mulhi_a);
extern SEMANTIC_CACHE_FN SEMC (mulhi_a);
extern SEMANTIC_FN SEM (mullo_a);
extern SEMANTIC_CACHE_FN SEMC (mullo_a);
extern SEMANTIC_FN SEM (mv);
extern SEMANTIC_CACHE_FN SEMC (mv);
extern SEMANTIC_FN SEM (mvfachi_a);
extern SEMANTIC_CACHE_FN SEMC (mvfachi_a);
extern SEMANTIC_FN SEM (mvfaclo_a);
extern SEMANTIC_CACHE_FN SEMC (mvfaclo_a);
extern SEMANTIC_FN SEM (mvfacmi_a);
extern SEMANTIC_CACHE_FN SEMC (mvfacmi_a);
extern SEMANTIC_FN SEM (mvfc);
extern SEMANTIC_CACHE_FN SEMC (mvfc);
extern SEMANTIC_FN SEM (mvtachi_a);
extern SEMANTIC_CACHE_FN SEMC (mvtachi_a);
extern SEMANTIC_FN SEM (mvtaclo_a);
extern SEMANTIC_CACHE_FN SEMC (mvtaclo_a);
extern SEMANTIC_FN SEM (mvtc);
extern SEMANTIC_CACHE_FN SEMC (mvtc);
extern SEMANTIC_FN SEM (neg);
extern SEMANTIC_CACHE_FN SEMC (neg);
extern SEMANTIC_FN SEM (nop);
extern SEMANTIC_CACHE_FN SEMC (nop);
extern SEMANTIC_FN SEM (not);
extern SEMANTIC_CACHE_FN SEMC (not);
extern SEMANTIC_FN SEM (rac_d);
extern SEMANTIC_CACHE_FN SEMC (rac_d);
extern SEMANTIC_FN SEM (rac_ds);
extern SEMANTIC_CACHE_FN SEMC (rac_ds);
extern SEMANTIC_FN SEM (rac_dsi);
extern SEMANTIC_CACHE_FN SEMC (rac_dsi);
extern SEMANTIC_FN SEM (rach_d);
extern SEMANTIC_CACHE_FN SEMC (rach_d);
extern SEMANTIC_FN SEM (rach_ds);
extern SEMANTIC_CACHE_FN SEMC (rach_ds);
extern SEMANTIC_FN SEM (rach_dsi);
extern SEMANTIC_CACHE_FN SEMC (rach_dsi);
extern SEMANTIC_FN SEM (rte);
extern SEMANTIC_CACHE_FN SEMC (rte);
extern SEMANTIC_FN SEM (seth);
extern SEMANTIC_CACHE_FN SEMC (seth);
extern SEMANTIC_FN SEM (sll);
extern SEMANTIC_CACHE_FN SEMC (sll);
extern SEMANTIC_FN SEM (sll3);
extern SEMANTIC_CACHE_FN SEMC (sll3);
extern SEMANTIC_FN SEM (slli);
extern SEMANTIC_CACHE_FN SEMC (slli);
extern SEMANTIC_FN SEM (sra);
extern SEMANTIC_CACHE_FN SEMC (sra);
extern SEMANTIC_FN SEM (sra3);
extern SEMANTIC_CACHE_FN SEMC (sra3);
extern SEMANTIC_FN SEM (srai);
extern SEMANTIC_CACHE_FN SEMC (srai);
extern SEMANTIC_FN SEM (srl);
extern SEMANTIC_CACHE_FN SEMC (srl);
extern SEMANTIC_FN SEM (srl3);
extern SEMANTIC_CACHE_FN SEMC (srl3);
extern SEMANTIC_FN SEM (srli);
extern SEMANTIC_CACHE_FN SEMC (srli);
extern SEMANTIC_FN SEM (st);
extern SEMANTIC_CACHE_FN SEMC (st);
extern SEMANTIC_FN SEM (st_d);
extern SEMANTIC_CACHE_FN SEMC (st_d);
extern SEMANTIC_FN SEM (stb);
extern SEMANTIC_CACHE_FN SEMC (stb);
extern SEMANTIC_FN SEM (stb_d);
extern SEMANTIC_CACHE_FN SEMC (stb_d);
extern SEMANTIC_FN SEM (sth);
extern SEMANTIC_CACHE_FN SEMC (sth);
extern SEMANTIC_FN SEM (sth_d);
extern SEMANTIC_CACHE_FN SEMC (sth_d);
extern SEMANTIC_FN SEM (st_plus);
extern SEMANTIC_CACHE_FN SEMC (st_plus);
extern SEMANTIC_FN SEM (st_minus);
extern SEMANTIC_CACHE_FN SEMC (st_minus);
extern SEMANTIC_FN SEM (sub);
extern SEMANTIC_CACHE_FN SEMC (sub);
extern SEMANTIC_FN SEM (subv);
extern SEMANTIC_CACHE_FN SEMC (subv);
extern SEMANTIC_FN SEM (subx);
extern SEMANTIC_CACHE_FN SEMC (subx);
extern SEMANTIC_FN SEM (trap);
extern SEMANTIC_CACHE_FN SEMC (trap);
extern SEMANTIC_FN SEM (unlock);
extern SEMANTIC_CACHE_FN SEMC (unlock);
extern SEMANTIC_FN SEM (satb);
extern SEMANTIC_CACHE_FN SEMC (satb);
extern SEMANTIC_FN SEM (sath);
extern SEMANTIC_CACHE_FN SEMC (sath);
extern SEMANTIC_FN SEM (sat);
extern SEMANTIC_CACHE_FN SEMC (sat);
extern SEMANTIC_FN SEM (pcmpbz);
extern SEMANTIC_CACHE_FN SEMC (pcmpbz);
extern SEMANTIC_FN SEM (sadd);
extern SEMANTIC_CACHE_FN SEMC (sadd);
extern SEMANTIC_FN SEM (macwu1);
extern SEMANTIC_CACHE_FN SEMC (macwu1);
extern SEMANTIC_FN SEM (msblo);
extern SEMANTIC_CACHE_FN SEMC (msblo);
extern SEMANTIC_FN SEM (mulwu1);
extern SEMANTIC_CACHE_FN SEMC (mulwu1);
extern SEMANTIC_FN SEM (maclh1);
extern SEMANTIC_CACHE_FN SEMC (maclh1);
extern SEMANTIC_FN SEM (sc);
extern SEMANTIC_CACHE_FN SEMC (sc);
extern SEMANTIC_FN SEM (snc);
extern SEMANTIC_CACHE_FN SEMC (snc);

#undef EX
#undef SEM
#undef SEMC

#endif /* M32RX_DECODE_H */
