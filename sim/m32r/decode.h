/* Decode header for m32rbf.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1996, 1997, 1998, 1999 Free Software Foundation, Inc.

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

#ifndef M32RBF_DECODE_H
#define M32RBF_DECODE_H

extern const IDESC *m32rbf_decode (SIM_CPU *, IADDR,
                                  CGEN_INSN_INT, CGEN_INSN_INT,
                                  ARGBUF *);
extern void m32rbf_init_idesc_table (SIM_CPU *);

/* Enum declaration for instructions in cpu family m32rbf.  */
typedef enum m32rbf_insn_type {
  M32RBF_INSN_X_INVALID, M32RBF_INSN_X_AFTER, M32RBF_INSN_X_BEFORE, M32RBF_INSN_X_CTI_CHAIN
 , M32RBF_INSN_X_CHAIN, M32RBF_INSN_X_BEGIN, M32RBF_INSN_ADD, M32RBF_INSN_ADD3
 , M32RBF_INSN_AND, M32RBF_INSN_AND3, M32RBF_INSN_OR, M32RBF_INSN_OR3
 , M32RBF_INSN_XOR, M32RBF_INSN_XOR3, M32RBF_INSN_ADDI, M32RBF_INSN_ADDV
 , M32RBF_INSN_ADDV3, M32RBF_INSN_ADDX, M32RBF_INSN_BC8, M32RBF_INSN_BC24
 , M32RBF_INSN_BEQ, M32RBF_INSN_BEQZ, M32RBF_INSN_BGEZ, M32RBF_INSN_BGTZ
 , M32RBF_INSN_BLEZ, M32RBF_INSN_BLTZ, M32RBF_INSN_BNEZ, M32RBF_INSN_BL8
 , M32RBF_INSN_BL24, M32RBF_INSN_BNC8, M32RBF_INSN_BNC24, M32RBF_INSN_BNE
 , M32RBF_INSN_BRA8, M32RBF_INSN_BRA24, M32RBF_INSN_CMP, M32RBF_INSN_CMPI
 , M32RBF_INSN_CMPU, M32RBF_INSN_CMPUI, M32RBF_INSN_DIV, M32RBF_INSN_DIVU
 , M32RBF_INSN_REM, M32RBF_INSN_REMU, M32RBF_INSN_JL, M32RBF_INSN_JMP
 , M32RBF_INSN_LD, M32RBF_INSN_LD_D, M32RBF_INSN_LDB, M32RBF_INSN_LDB_D
 , M32RBF_INSN_LDH, M32RBF_INSN_LDH_D, M32RBF_INSN_LDUB, M32RBF_INSN_LDUB_D
 , M32RBF_INSN_LDUH, M32RBF_INSN_LDUH_D, M32RBF_INSN_LD_PLUS, M32RBF_INSN_LD24
 , M32RBF_INSN_LDI8, M32RBF_INSN_LDI16, M32RBF_INSN_LOCK, M32RBF_INSN_MACHI
 , M32RBF_INSN_MACLO, M32RBF_INSN_MACWHI, M32RBF_INSN_MACWLO, M32RBF_INSN_MUL
 , M32RBF_INSN_MULHI, M32RBF_INSN_MULLO, M32RBF_INSN_MULWHI, M32RBF_INSN_MULWLO
 , M32RBF_INSN_MV, M32RBF_INSN_MVFACHI, M32RBF_INSN_MVFACLO, M32RBF_INSN_MVFACMI
 , M32RBF_INSN_MVFC, M32RBF_INSN_MVTACHI, M32RBF_INSN_MVTACLO, M32RBF_INSN_MVTC
 , M32RBF_INSN_NEG, M32RBF_INSN_NOP, M32RBF_INSN_NOT, M32RBF_INSN_RAC
 , M32RBF_INSN_RACH, M32RBF_INSN_RTE, M32RBF_INSN_SETH, M32RBF_INSN_SLL
 , M32RBF_INSN_SLL3, M32RBF_INSN_SLLI, M32RBF_INSN_SRA, M32RBF_INSN_SRA3
 , M32RBF_INSN_SRAI, M32RBF_INSN_SRL, M32RBF_INSN_SRL3, M32RBF_INSN_SRLI
 , M32RBF_INSN_ST, M32RBF_INSN_ST_D, M32RBF_INSN_STB, M32RBF_INSN_STB_D
 , M32RBF_INSN_STH, M32RBF_INSN_STH_D, M32RBF_INSN_ST_PLUS, M32RBF_INSN_ST_MINUS
 , M32RBF_INSN_SUB, M32RBF_INSN_SUBV, M32RBF_INSN_SUBX, M32RBF_INSN_TRAP
 , M32RBF_INSN_UNLOCK, M32RBF_INSN_MAX
} M32RBF_INSN_TYPE;

#if ! WITH_SEM_SWITCH_FULL
#define SEMFULL(fn) extern SEMANTIC_FN CONCAT3 (m32rbf,_sem_,fn);
#else
#define SEMFULL(fn)
#endif

#if ! WITH_SEM_SWITCH_FAST
#define SEMFAST(fn) extern SEMANTIC_FN CONCAT3 (m32rbf,_semf_,fn);
#else
#define SEMFAST(fn)
#endif

#define SEM(fn) SEMFULL (fn) SEMFAST (fn)

/* The function version of the before/after handlers is always needed,
   so we always want the SEMFULL declaration of them.  */
extern SEMANTIC_FN CONCAT3 (m32rbf,_sem_,x_before);
extern SEMANTIC_FN CONCAT3 (m32rbf,_sem_,x_after);

SEM (x_invalid)
SEM (x_after)
SEM (x_before)
SEM (x_cti_chain)
SEM (x_chain)
SEM (x_begin)
SEM (add)
SEM (add3)
SEM (and)
SEM (and3)
SEM (or)
SEM (or3)
SEM (xor)
SEM (xor3)
SEM (addi)
SEM (addv)
SEM (addv3)
SEM (addx)
SEM (bc8)
SEM (bc24)
SEM (beq)
SEM (beqz)
SEM (bgez)
SEM (bgtz)
SEM (blez)
SEM (bltz)
SEM (bnez)
SEM (bl8)
SEM (bl24)
SEM (bnc8)
SEM (bnc24)
SEM (bne)
SEM (bra8)
SEM (bra24)
SEM (cmp)
SEM (cmpi)
SEM (cmpu)
SEM (cmpui)
SEM (div)
SEM (divu)
SEM (rem)
SEM (remu)
SEM (jl)
SEM (jmp)
SEM (ld)
SEM (ld_d)
SEM (ldb)
SEM (ldb_d)
SEM (ldh)
SEM (ldh_d)
SEM (ldub)
SEM (ldub_d)
SEM (lduh)
SEM (lduh_d)
SEM (ld_plus)
SEM (ld24)
SEM (ldi8)
SEM (ldi16)
SEM (lock)
SEM (machi)
SEM (maclo)
SEM (macwhi)
SEM (macwlo)
SEM (mul)
SEM (mulhi)
SEM (mullo)
SEM (mulwhi)
SEM (mulwlo)
SEM (mv)
SEM (mvfachi)
SEM (mvfaclo)
SEM (mvfacmi)
SEM (mvfc)
SEM (mvtachi)
SEM (mvtaclo)
SEM (mvtc)
SEM (neg)
SEM (nop)
SEM (not)
SEM (rac)
SEM (rach)
SEM (rte)
SEM (seth)
SEM (sll)
SEM (sll3)
SEM (slli)
SEM (sra)
SEM (sra3)
SEM (srai)
SEM (srl)
SEM (srl3)
SEM (srli)
SEM (st)
SEM (st_d)
SEM (stb)
SEM (stb_d)
SEM (sth)
SEM (sth_d)
SEM (st_plus)
SEM (st_minus)
SEM (sub)
SEM (subv)
SEM (subx)
SEM (trap)
SEM (unlock)

#undef SEMFULL
#undef SEMFAST
#undef SEM

/* Function unit handlers (user written).  */

extern int m32rbf_model_m32r_d_u_store (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*src1*/, INT /*src2*/);
extern int m32rbf_model_m32r_d_u_load (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*sr*/, INT /*dr*/);
extern int m32rbf_model_m32r_d_u_cti (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*sr*/);
extern int m32rbf_model_m32r_d_u_mac (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*src1*/, INT /*src2*/);
extern int m32rbf_model_m32r_d_u_cmp (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*src1*/, INT /*src2*/);
extern int m32rbf_model_m32r_d_u_exec (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*sr*/, INT /*dr*/, INT /*dr*/);
extern int m32rbf_model_test_u_exec (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/);

/* Profiling before/after handlers (user written) */

extern void m32rbf_model_insn_before (SIM_CPU *, int /*first_p*/);
extern void m32rbf_model_insn_after (SIM_CPU *, int /*last_p*/, int /*cycles*/);

#endif /* M32RBF_DECODE_H */
