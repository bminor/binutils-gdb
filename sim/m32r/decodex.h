/* Decode header for m32rxf.

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

#ifndef M32RXF_DECODE_H
#define M32RXF_DECODE_H

/* Run-time computed instruction descriptor.  */

struct idesc {
  /* Pointer to parallel handler if serial insn.
     Pointer to writeback handler if parallel insn.  */
  struct idesc *par_idesc;

#if WITH_SEM_SWITCH_FULL
#ifdef __GNUC__
  void *sem_full_lab;
#endif
#else
  SEMANTIC_FN *sem_full;
#endif

#if WITH_SEM_SWITCH_FAST
#ifdef __GNUC__
  void *sem_fast_lab;
#endif
#else
  SEMANTIC_FN *sem_fast;
#endif

  /* Instruction number (index in IDESC table, profile table).
     Also used to switch on in non-gcc semantic switches.  */
  int num;

  /* opcode table data */
  const CGEN_INSN *opcode;

  /* profiling/modelling support */
  const INSN_TIMING *timing;
};

extern const IDESC *m32rxf_decode (SIM_CPU *, IADDR,
                                  CGEN_INSN_INT, CGEN_INSN_INT,
                                  ARGBUF *);

/* Enum declaration for instructions in cpu family m32rxf.  */
typedef enum m32rxf_insn_type {
  M32RXF_INSN_X_INVALID, M32RXF_INSN_X_AFTER, M32RXF_INSN_X_BEFORE, M32RXF_INSN_X_CTI_CHAIN
 , M32RXF_INSN_X_CHAIN, M32RXF_INSN_X_BEGIN, M32RXF_INSN_ADD, M32RXF_INSN_ADD3
 , M32RXF_INSN_AND, M32RXF_INSN_AND3, M32RXF_INSN_OR, M32RXF_INSN_OR3
 , M32RXF_INSN_XOR, M32RXF_INSN_XOR3, M32RXF_INSN_ADDI, M32RXF_INSN_ADDV
 , M32RXF_INSN_ADDV3, M32RXF_INSN_ADDX, M32RXF_INSN_BC8, M32RXF_INSN_BC24
 , M32RXF_INSN_BEQ, M32RXF_INSN_BEQZ, M32RXF_INSN_BGEZ, M32RXF_INSN_BGTZ
 , M32RXF_INSN_BLEZ, M32RXF_INSN_BLTZ, M32RXF_INSN_BNEZ, M32RXF_INSN_BL8
 , M32RXF_INSN_BL24
/* start-sanitize-m32rx */
 , M32RXF_INSN_BCL8
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_BCL24
/* end-sanitize-m32rx */
 , M32RXF_INSN_BNC8, M32RXF_INSN_BNC24, M32RXF_INSN_BNE, M32RXF_INSN_BRA8
 , M32RXF_INSN_BRA24
/* start-sanitize-m32rx */
 , M32RXF_INSN_BNCL8
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_BNCL24
/* end-sanitize-m32rx */
 , M32RXF_INSN_CMP, M32RXF_INSN_CMPI, M32RXF_INSN_CMPU, M32RXF_INSN_CMPUI
/* start-sanitize-m32rx */
 , M32RXF_INSN_CMPEQ
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_CMPZ
/* end-sanitize-m32rx */
 , M32RXF_INSN_DIV, M32RXF_INSN_DIVU, M32RXF_INSN_REM, M32RXF_INSN_REMU
/* start-sanitize-m32rx */
 , M32RXF_INSN_DIVH
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_JC
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_JNC
/* end-sanitize-m32rx */
 , M32RXF_INSN_JL, M32RXF_INSN_JMP, M32RXF_INSN_LD, M32RXF_INSN_LD_D
 , M32RXF_INSN_LDB, M32RXF_INSN_LDB_D, M32RXF_INSN_LDH, M32RXF_INSN_LDH_D
 , M32RXF_INSN_LDUB, M32RXF_INSN_LDUB_D, M32RXF_INSN_LDUH, M32RXF_INSN_LDUH_D
 , M32RXF_INSN_LD_PLUS, M32RXF_INSN_LD24, M32RXF_INSN_LDI8, M32RXF_INSN_LDI16
 , M32RXF_INSN_LOCK
/* start-sanitize-m32rx */
 , M32RXF_INSN_MACHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MACLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MACWHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MACWLO_A
/* end-sanitize-m32rx */
 , M32RXF_INSN_MUL
/* start-sanitize-m32rx */
 , M32RXF_INSN_MULHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MULLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MULWHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MULWLO_A
/* end-sanitize-m32rx */
 , M32RXF_INSN_MV
/* start-sanitize-m32rx */
 , M32RXF_INSN_MVFACHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MVFACLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MVFACMI_A
/* end-sanitize-m32rx */
 , M32RXF_INSN_MVFC
/* start-sanitize-m32rx */
 , M32RXF_INSN_MVTACHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MVTACLO_A
/* end-sanitize-m32rx */
 , M32RXF_INSN_MVTC, M32RXF_INSN_NEG, M32RXF_INSN_NOP, M32RXF_INSN_NOT
/* start-sanitize-m32rx */
 , M32RXF_INSN_RAC_DSI
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_RACH_DSI
/* end-sanitize-m32rx */
 , M32RXF_INSN_RTE, M32RXF_INSN_SETH, M32RXF_INSN_SLL, M32RXF_INSN_SLL3
 , M32RXF_INSN_SLLI, M32RXF_INSN_SRA, M32RXF_INSN_SRA3, M32RXF_INSN_SRAI
 , M32RXF_INSN_SRL, M32RXF_INSN_SRL3, M32RXF_INSN_SRLI, M32RXF_INSN_ST
 , M32RXF_INSN_ST_D, M32RXF_INSN_STB, M32RXF_INSN_STB_D, M32RXF_INSN_STH
 , M32RXF_INSN_STH_D, M32RXF_INSN_ST_PLUS, M32RXF_INSN_ST_MINUS, M32RXF_INSN_SUB
 , M32RXF_INSN_SUBV, M32RXF_INSN_SUBX, M32RXF_INSN_TRAP, M32RXF_INSN_UNLOCK
/* start-sanitize-m32rx */
 , M32RXF_INSN_SATB
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_SATH
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_SAT
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PCMPBZ
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_SADD
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MACWU1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MSBLO
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MULWU1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_MACLH1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_SC
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_SNC
/* end-sanitize-m32rx */
 , M32RXF_INSN_PAR_ADD, M32RXF_INSN_WRITE_ADD, M32RXF_INSN_PAR_AND, M32RXF_INSN_WRITE_AND
 , M32RXF_INSN_PAR_OR, M32RXF_INSN_WRITE_OR, M32RXF_INSN_PAR_XOR, M32RXF_INSN_WRITE_XOR
 , M32RXF_INSN_PAR_ADDI, M32RXF_INSN_WRITE_ADDI, M32RXF_INSN_PAR_ADDV, M32RXF_INSN_WRITE_ADDV
 , M32RXF_INSN_PAR_ADDX, M32RXF_INSN_WRITE_ADDX, M32RXF_INSN_PAR_BC8, M32RXF_INSN_WRITE_BC8
 , M32RXF_INSN_PAR_BL8, M32RXF_INSN_WRITE_BL8
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_BCL8
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_BCL8
/* end-sanitize-m32rx */
 , M32RXF_INSN_PAR_BNC8, M32RXF_INSN_WRITE_BNC8, M32RXF_INSN_PAR_BRA8, M32RXF_INSN_WRITE_BRA8
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_BNCL8
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_BNCL8
/* end-sanitize-m32rx */
 , M32RXF_INSN_PAR_CMP, M32RXF_INSN_WRITE_CMP, M32RXF_INSN_PAR_CMPU, M32RXF_INSN_WRITE_CMPU
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_CMPEQ
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_CMPEQ
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_CMPZ
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_CMPZ
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_JC
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_JC
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_JNC
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_JNC
/* end-sanitize-m32rx */
 , M32RXF_INSN_PAR_JL, M32RXF_INSN_WRITE_JL, M32RXF_INSN_PAR_JMP, M32RXF_INSN_WRITE_JMP
 , M32RXF_INSN_PAR_LD, M32RXF_INSN_WRITE_LD, M32RXF_INSN_PAR_LDB, M32RXF_INSN_WRITE_LDB
 , M32RXF_INSN_PAR_LDH, M32RXF_INSN_WRITE_LDH, M32RXF_INSN_PAR_LDUB, M32RXF_INSN_WRITE_LDUB
 , M32RXF_INSN_PAR_LDUH, M32RXF_INSN_WRITE_LDUH, M32RXF_INSN_PAR_LD_PLUS, M32RXF_INSN_WRITE_LD_PLUS
 , M32RXF_INSN_PAR_LDI8, M32RXF_INSN_WRITE_LDI8, M32RXF_INSN_PAR_LOCK, M32RXF_INSN_WRITE_LOCK
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MACHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MACHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MACLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MACLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MACWHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MACWHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MACWLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MACWLO_A
/* end-sanitize-m32rx */
 , M32RXF_INSN_PAR_MUL, M32RXF_INSN_WRITE_MUL
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MULHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MULHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MULLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MULLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MULWHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MULWHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MULWLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MULWLO_A
/* end-sanitize-m32rx */
 , M32RXF_INSN_PAR_MV, M32RXF_INSN_WRITE_MV
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MVFACHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MVFACHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MVFACLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MVFACLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MVFACMI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MVFACMI_A
/* end-sanitize-m32rx */
 , M32RXF_INSN_PAR_MVFC, M32RXF_INSN_WRITE_MVFC
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MVTACHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MVTACHI_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MVTACLO_A
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MVTACLO_A
/* end-sanitize-m32rx */
 , M32RXF_INSN_PAR_MVTC, M32RXF_INSN_WRITE_MVTC, M32RXF_INSN_PAR_NEG, M32RXF_INSN_WRITE_NEG
 , M32RXF_INSN_PAR_NOP, M32RXF_INSN_WRITE_NOP, M32RXF_INSN_PAR_NOT, M32RXF_INSN_WRITE_NOT
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_RAC_DSI
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_RAC_DSI
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_RACH_DSI
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_RACH_DSI
/* end-sanitize-m32rx */
 , M32RXF_INSN_PAR_RTE, M32RXF_INSN_WRITE_RTE, M32RXF_INSN_PAR_SLL, M32RXF_INSN_WRITE_SLL
 , M32RXF_INSN_PAR_SLLI, M32RXF_INSN_WRITE_SLLI, M32RXF_INSN_PAR_SRA, M32RXF_INSN_WRITE_SRA
 , M32RXF_INSN_PAR_SRAI, M32RXF_INSN_WRITE_SRAI, M32RXF_INSN_PAR_SRL, M32RXF_INSN_WRITE_SRL
 , M32RXF_INSN_PAR_SRLI, M32RXF_INSN_WRITE_SRLI, M32RXF_INSN_PAR_ST, M32RXF_INSN_WRITE_ST
 , M32RXF_INSN_PAR_STB, M32RXF_INSN_WRITE_STB, M32RXF_INSN_PAR_STH, M32RXF_INSN_WRITE_STH
 , M32RXF_INSN_PAR_ST_PLUS, M32RXF_INSN_WRITE_ST_PLUS, M32RXF_INSN_PAR_ST_MINUS, M32RXF_INSN_WRITE_ST_MINUS
 , M32RXF_INSN_PAR_SUB, M32RXF_INSN_WRITE_SUB, M32RXF_INSN_PAR_SUBV, M32RXF_INSN_WRITE_SUBV
 , M32RXF_INSN_PAR_SUBX, M32RXF_INSN_WRITE_SUBX, M32RXF_INSN_PAR_TRAP, M32RXF_INSN_WRITE_TRAP
 , M32RXF_INSN_PAR_UNLOCK, M32RXF_INSN_WRITE_UNLOCK
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_PCMPBZ
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_PCMPBZ
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_SADD
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_SADD
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MACWU1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MACWU1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MSBLO
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MSBLO
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MULWU1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MULWU1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_MACLH1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_MACLH1
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_SC
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_SC
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_PAR_SNC
/* end-sanitize-m32rx */
/* start-sanitize-m32rx */
 , M32RXF_INSN_WRITE_SNC
/* end-sanitize-m32rx */
 , M32RXF_INSN_MAX
} M32RXF_INSN_TYPE;

#if ! WITH_SEM_SWITCH_FULL
#define SEMFULL(fn) extern SEMANTIC_FN CONCAT3 (m32rxf,_sem_,fn);
#else
#define SEMFULL(fn)
#endif

#if ! WITH_SEM_SWITCH_FAST
#define SEMFAST(fn) extern SEMANTIC_FN CONCAT3 (m32rxf,_semf_,fn);
#else
#define SEMFAST(fn)
#endif

#define SEM(fn) SEMFULL (fn) SEMFAST (fn)

/* The function version of the before/after handlers is always needed,
   so we always want the SEMFULL declaration of them.  */
extern SEMANTIC_FN CONCAT3 (m32rxf,_sem_,x_before);
extern SEMANTIC_FN CONCAT3 (m32rxf,_sem_,x_after);

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
SEM (bcl8)
SEM (bcl24)
SEM (bnc8)
SEM (bnc24)
SEM (bne)
SEM (bra8)
SEM (bra24)
SEM (bncl8)
SEM (bncl24)
SEM (cmp)
SEM (cmpi)
SEM (cmpu)
SEM (cmpui)
SEM (cmpeq)
SEM (cmpz)
SEM (div)
SEM (divu)
SEM (rem)
SEM (remu)
SEM (divh)
SEM (jc)
SEM (jnc)
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
SEM (machi_a)
SEM (maclo_a)
SEM (macwhi_a)
SEM (macwlo_a)
SEM (mul)
SEM (mulhi_a)
SEM (mullo_a)
SEM (mulwhi_a)
SEM (mulwlo_a)
SEM (mv)
SEM (mvfachi_a)
SEM (mvfaclo_a)
SEM (mvfacmi_a)
SEM (mvfc)
SEM (mvtachi_a)
SEM (mvtaclo_a)
SEM (mvtc)
SEM (neg)
SEM (nop)
SEM (not)
SEM (rac_dsi)
SEM (rach_dsi)
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
SEM (satb)
SEM (sath)
SEM (sat)
SEM (pcmpbz)
SEM (sadd)
SEM (macwu1)
SEM (msblo)
SEM (mulwu1)
SEM (maclh1)
SEM (sc)
SEM (snc)

#undef SEMFULL
#undef SEMFAST
#undef SEM

/* Function unit handlers (user written).  */

extern int m32rxf_model_m32rx_u_store (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*src1*/, INT /*src2*/);
extern int m32rxf_model_m32rx_u_load (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*sr*/, INT /*dr*/);
extern int m32rxf_model_m32rx_u_cti (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*sr*/);
extern int m32rxf_model_m32rx_u_mac (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*src1*/, INT /*src2*/);
extern int m32rxf_model_m32rx_u_cmp (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*src1*/, INT /*src2*/);
extern int m32rxf_model_m32rx_u_exec (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/, INT /*sr*/, INT /*sr2*/, INT /*dr*/);

/* Profiling before/after handlers (user written) */

extern void m32rxf_model_insn_before (SIM_CPU *, int /*first_p*/);
extern void m32rxf_model_insn_after (SIM_CPU *, int /*last_p*/, int /*cycles*/);

#endif /* M32RXF_DECODE_H */
