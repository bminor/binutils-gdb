/* Decode header for sparc32.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#ifndef SPARC32_DECODE_H
#define SPARC32_DECODE_H

extern const IDESC *sparc32_decode (SIM_CPU *, IADDR,
                                  CGEN_INSN_INT, CGEN_INSN_INT,
                                  ARGBUF *);
extern void sparc32_init_idesc_table (SIM_CPU *);

/* Enum declaration for instructions in cpu family sparc32.  */
typedef enum sparc32_insn_type {
  SPARC32_INSN_X_INVALID, SPARC32_INSN_X_AFTER, SPARC32_INSN_X_BEFORE, SPARC32_INSN_X_CTI_CHAIN
 , SPARC32_INSN_X_CHAIN, SPARC32_INSN_X_BEGIN, SPARC32_INSN_RD_ASR, SPARC32_INSN_WR_ASR
 , SPARC32_INSN_WR_ASR_IMM, SPARC32_INSN_RD_PSR, SPARC32_INSN_WR_PSR, SPARC32_INSN_WR_PSR_IMM
 , SPARC32_INSN_RD_WIM, SPARC32_INSN_WR_WIM, SPARC32_INSN_WR_WIM_IMM, SPARC32_INSN_RD_TBR
 , SPARC32_INSN_WR_TBR, SPARC32_INSN_WR_TBR_IMM, SPARC32_INSN_LDSTUB_REG_REG, SPARC32_INSN_LDSTUB_REG_IMM
 , SPARC32_INSN_LDSTUB_REG_REG_ASI, SPARC32_INSN_SWAP_REG_REG, SPARC32_INSN_SWAP_REG_IMM, SPARC32_INSN_SWAP_REG_REG_ASI
 , SPARC32_INSN_LDSB_REG_REG, SPARC32_INSN_LDSB_REG_IMM, SPARC32_INSN_LDSB_REG_REG_ASI, SPARC32_INSN_LDUB_REG_REG
 , SPARC32_INSN_LDUB_REG_IMM, SPARC32_INSN_LDUB_REG_REG_ASI, SPARC32_INSN_LDSH_REG_REG, SPARC32_INSN_LDSH_REG_IMM
 , SPARC32_INSN_LDSH_REG_REG_ASI, SPARC32_INSN_LDUH_REG_REG, SPARC32_INSN_LDUH_REG_IMM, SPARC32_INSN_LDUH_REG_REG_ASI
 , SPARC32_INSN_LDSW_REG_REG, SPARC32_INSN_LDSW_REG_IMM, SPARC32_INSN_LDSW_REG_REG_ASI, SPARC32_INSN_LDUW_REG_REG
 , SPARC32_INSN_LDUW_REG_IMM, SPARC32_INSN_LDUW_REG_REG_ASI, SPARC32_INSN_LDD_REG_REG, SPARC32_INSN_LDD_REG_IMM
 , SPARC32_INSN_LDD_REG_REG_ASI, SPARC32_INSN_STB_REG_REG, SPARC32_INSN_STB_REG_IMM, SPARC32_INSN_STB_REG_REG_ASI
 , SPARC32_INSN_STH_REG_REG, SPARC32_INSN_STH_REG_IMM, SPARC32_INSN_STH_REG_REG_ASI, SPARC32_INSN_ST_REG_REG
 , SPARC32_INSN_ST_REG_IMM, SPARC32_INSN_ST_REG_REG_ASI, SPARC32_INSN_STD_REG_REG, SPARC32_INSN_STD_REG_IMM
 , SPARC32_INSN_STD_REG_REG_ASI, SPARC32_INSN_FP_LD_REG_REG, SPARC32_INSN_FP_LD_REG_IMM, SPARC32_INSN_FP_LD_REG_REG_ASI
 , SPARC32_INSN_SETHI, SPARC32_INSN_ADD, SPARC32_INSN_ADD_IMM, SPARC32_INSN_SUB
 , SPARC32_INSN_SUB_IMM, SPARC32_INSN_ADDCC, SPARC32_INSN_ADDCC_IMM, SPARC32_INSN_SUBCC
 , SPARC32_INSN_SUBCC_IMM, SPARC32_INSN_ADDX, SPARC32_INSN_ADDX_IMM, SPARC32_INSN_SUBX
 , SPARC32_INSN_SUBX_IMM, SPARC32_INSN_ADDXCC, SPARC32_INSN_ADDXCC_IMM, SPARC32_INSN_SUBXCC
 , SPARC32_INSN_SUBXCC_IMM, SPARC32_INSN_AND, SPARC32_INSN_AND_IMM, SPARC32_INSN_ANDCC
 , SPARC32_INSN_ANDCC_IMM, SPARC32_INSN_OR, SPARC32_INSN_OR_IMM, SPARC32_INSN_ORCC
 , SPARC32_INSN_ORCC_IMM, SPARC32_INSN_XOR, SPARC32_INSN_XOR_IMM, SPARC32_INSN_XORCC
 , SPARC32_INSN_XORCC_IMM, SPARC32_INSN_ANDN, SPARC32_INSN_ANDN_IMM, SPARC32_INSN_ANDNCC
 , SPARC32_INSN_ANDNCC_IMM, SPARC32_INSN_ORN, SPARC32_INSN_ORN_IMM, SPARC32_INSN_ORNCC
 , SPARC32_INSN_ORNCC_IMM, SPARC32_INSN_XNOR, SPARC32_INSN_XNOR_IMM, SPARC32_INSN_XNORCC
 , SPARC32_INSN_XNORCC_IMM, SPARC32_INSN_SLL, SPARC32_INSN_SLL_IMM, SPARC32_INSN_SRL
 , SPARC32_INSN_SRL_IMM, SPARC32_INSN_SRA, SPARC32_INSN_SRA_IMM, SPARC32_INSN_SMUL
 , SPARC32_INSN_SMUL_IMM, SPARC32_INSN_SMUL_CC, SPARC32_INSN_SMUL_CC_IMM, SPARC32_INSN_UMUL
 , SPARC32_INSN_UMUL_IMM, SPARC32_INSN_UMUL_CC, SPARC32_INSN_UMUL_CC_IMM, SPARC32_INSN_SDIV
 , SPARC32_INSN_SDIV_IMM, SPARC32_INSN_SDIV_CC, SPARC32_INSN_SDIV_CC_IMM, SPARC32_INSN_UDIV
 , SPARC32_INSN_UDIV_IMM, SPARC32_INSN_UDIV_CC, SPARC32_INSN_UDIV_CC_IMM, SPARC32_INSN_MULSCC
 , SPARC32_INSN_SAVE, SPARC32_INSN_SAVE_IMM, SPARC32_INSN_RESTORE, SPARC32_INSN_RESTORE_IMM
 , SPARC32_INSN_RETT, SPARC32_INSN_RETT_IMM, SPARC32_INSN_UNIMP, SPARC32_INSN_CALL
 , SPARC32_INSN_JMPL, SPARC32_INSN_JMPL_IMM, SPARC32_INSN_BA, SPARC32_INSN_TA
 , SPARC32_INSN_TA_IMM, SPARC32_INSN_BN, SPARC32_INSN_TN, SPARC32_INSN_TN_IMM
 , SPARC32_INSN_BNE, SPARC32_INSN_TNE, SPARC32_INSN_TNE_IMM, SPARC32_INSN_BE
 , SPARC32_INSN_TE, SPARC32_INSN_TE_IMM, SPARC32_INSN_BG, SPARC32_INSN_TG
 , SPARC32_INSN_TG_IMM, SPARC32_INSN_BLE, SPARC32_INSN_TLE, SPARC32_INSN_TLE_IMM
 , SPARC32_INSN_BGE, SPARC32_INSN_TGE, SPARC32_INSN_TGE_IMM, SPARC32_INSN_BL
 , SPARC32_INSN_TL, SPARC32_INSN_TL_IMM, SPARC32_INSN_BGU, SPARC32_INSN_TGU
 , SPARC32_INSN_TGU_IMM, SPARC32_INSN_BLEU, SPARC32_INSN_TLEU, SPARC32_INSN_TLEU_IMM
 , SPARC32_INSN_BCC, SPARC32_INSN_TCC, SPARC32_INSN_TCC_IMM, SPARC32_INSN_BCS
 , SPARC32_INSN_TCS, SPARC32_INSN_TCS_IMM, SPARC32_INSN_BPOS, SPARC32_INSN_TPOS
 , SPARC32_INSN_TPOS_IMM, SPARC32_INSN_BNEG, SPARC32_INSN_TNEG, SPARC32_INSN_TNEG_IMM
 , SPARC32_INSN_BVC, SPARC32_INSN_TVC, SPARC32_INSN_TVC_IMM, SPARC32_INSN_BVS
 , SPARC32_INSN_TVS, SPARC32_INSN_TVS_IMM, SPARC32_INSN_MAX
} SPARC32_INSN_TYPE;

#if ! WITH_SEM_SWITCH_FULL
#define SEMFULL(fn) extern SEMANTIC_FN CONCAT3 (sparc32,_sem_,fn);
#else
#define SEMFULL(fn)
#endif

#if ! WITH_SEM_SWITCH_FAST
#define SEMFAST(fn) extern SEMANTIC_FN CONCAT3 (sparc32,_semf_,fn);
#else
#define SEMFAST(fn)
#endif

#define SEM(fn) SEMFULL (fn) SEMFAST (fn)

/* The function version of the before/after handlers is always needed,
   so we always want the SEMFULL declaration of them.  */
extern SEMANTIC_FN CONCAT3 (sparc32,_sem_,x_before);
extern SEMANTIC_FN CONCAT3 (sparc32,_sem_,x_after);

SEM (x_invalid)
SEM (x_after)
SEM (x_before)
SEM (x_cti_chain)
SEM (x_chain)
SEM (x_begin)
SEM (rd_asr)
SEM (wr_asr)
SEM (wr_asr_imm)
SEM (rd_psr)
SEM (wr_psr)
SEM (wr_psr_imm)
SEM (rd_wim)
SEM (wr_wim)
SEM (wr_wim_imm)
SEM (rd_tbr)
SEM (wr_tbr)
SEM (wr_tbr_imm)
SEM (ldstub_reg_reg)
SEM (ldstub_reg_imm)
SEM (ldstub_reg_reg_asi)
SEM (swap_reg_reg)
SEM (swap_reg_imm)
SEM (swap_reg_reg_asi)
SEM (ldsb_reg_reg)
SEM (ldsb_reg_imm)
SEM (ldsb_reg_reg_asi)
SEM (ldub_reg_reg)
SEM (ldub_reg_imm)
SEM (ldub_reg_reg_asi)
SEM (ldsh_reg_reg)
SEM (ldsh_reg_imm)
SEM (ldsh_reg_reg_asi)
SEM (lduh_reg_reg)
SEM (lduh_reg_imm)
SEM (lduh_reg_reg_asi)
SEM (ldsw_reg_reg)
SEM (ldsw_reg_imm)
SEM (ldsw_reg_reg_asi)
SEM (lduw_reg_reg)
SEM (lduw_reg_imm)
SEM (lduw_reg_reg_asi)
SEM (ldd_reg_reg)
SEM (ldd_reg_imm)
SEM (ldd_reg_reg_asi)
SEM (stb_reg_reg)
SEM (stb_reg_imm)
SEM (stb_reg_reg_asi)
SEM (sth_reg_reg)
SEM (sth_reg_imm)
SEM (sth_reg_reg_asi)
SEM (st_reg_reg)
SEM (st_reg_imm)
SEM (st_reg_reg_asi)
SEM (std_reg_reg)
SEM (std_reg_imm)
SEM (std_reg_reg_asi)
SEM (fp_ld_reg_reg)
SEM (fp_ld_reg_imm)
SEM (fp_ld_reg_reg_asi)
SEM (sethi)
SEM (add)
SEM (add_imm)
SEM (sub)
SEM (sub_imm)
SEM (addcc)
SEM (addcc_imm)
SEM (subcc)
SEM (subcc_imm)
SEM (addx)
SEM (addx_imm)
SEM (subx)
SEM (subx_imm)
SEM (addxcc)
SEM (addxcc_imm)
SEM (subxcc)
SEM (subxcc_imm)
SEM (and)
SEM (and_imm)
SEM (andcc)
SEM (andcc_imm)
SEM (or)
SEM (or_imm)
SEM (orcc)
SEM (orcc_imm)
SEM (xor)
SEM (xor_imm)
SEM (xorcc)
SEM (xorcc_imm)
SEM (andn)
SEM (andn_imm)
SEM (andncc)
SEM (andncc_imm)
SEM (orn)
SEM (orn_imm)
SEM (orncc)
SEM (orncc_imm)
SEM (xnor)
SEM (xnor_imm)
SEM (xnorcc)
SEM (xnorcc_imm)
SEM (sll)
SEM (sll_imm)
SEM (srl)
SEM (srl_imm)
SEM (sra)
SEM (sra_imm)
SEM (smul)
SEM (smul_imm)
SEM (smul_cc)
SEM (smul_cc_imm)
SEM (umul)
SEM (umul_imm)
SEM (umul_cc)
SEM (umul_cc_imm)
SEM (sdiv)
SEM (sdiv_imm)
SEM (sdiv_cc)
SEM (sdiv_cc_imm)
SEM (udiv)
SEM (udiv_imm)
SEM (udiv_cc)
SEM (udiv_cc_imm)
SEM (mulscc)
SEM (save)
SEM (save_imm)
SEM (restore)
SEM (restore_imm)
SEM (rett)
SEM (rett_imm)
SEM (unimp)
SEM (call)
SEM (jmpl)
SEM (jmpl_imm)
SEM (ba)
SEM (ta)
SEM (ta_imm)
SEM (bn)
SEM (tn)
SEM (tn_imm)
SEM (bne)
SEM (tne)
SEM (tne_imm)
SEM (be)
SEM (te)
SEM (te_imm)
SEM (bg)
SEM (tg)
SEM (tg_imm)
SEM (ble)
SEM (tle)
SEM (tle_imm)
SEM (bge)
SEM (tge)
SEM (tge_imm)
SEM (bl)
SEM (tl)
SEM (tl_imm)
SEM (bgu)
SEM (tgu)
SEM (tgu_imm)
SEM (bleu)
SEM (tleu)
SEM (tleu_imm)
SEM (bcc)
SEM (tcc)
SEM (tcc_imm)
SEM (bcs)
SEM (tcs)
SEM (tcs_imm)
SEM (bpos)
SEM (tpos)
SEM (tpos_imm)
SEM (bneg)
SEM (tneg)
SEM (tneg_imm)
SEM (bvc)
SEM (tvc)
SEM (tvc_imm)
SEM (bvs)
SEM (tvs)
SEM (tvs_imm)

#undef SEMFULL
#undef SEMFAST
#undef SEM

/* Function unit handlers (user written).  */

extern int sparc32_model_sparc32_def_u_exec (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/);

/* Profiling before/after handlers (user written) */

extern void sparc32_model_insn_before (SIM_CPU *, int /*first_p*/);
extern void sparc32_model_insn_after (SIM_CPU *, int /*last_p*/, int /*cycles*/);

#endif /* SPARC32_DECODE_H */
