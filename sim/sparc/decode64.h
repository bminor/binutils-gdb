/* Decode header for sparc64.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#ifndef SPARC64_DECODE_H
#define SPARC64_DECODE_H

extern const IDESC *sparc64_decode (SIM_CPU *, IADDR,
                                  CGEN_INSN_INT, CGEN_INSN_INT,
                                  ARGBUF *);
extern void sparc64_init_idesc_table (SIM_CPU *);

/* Enum declaration for instructions in cpu family sparc64.  */
typedef enum sparc64_insn_type {
  SPARC64_INSN_X_INVALID, SPARC64_INSN_X_AFTER, SPARC64_INSN_X_BEFORE, SPARC64_INSN_X_CTI_CHAIN
 , SPARC64_INSN_X_CHAIN, SPARC64_INSN_X_BEGIN, SPARC64_INSN_BEQZ, SPARC64_INSN_BGEZ
 , SPARC64_INSN_BGTZ, SPARC64_INSN_BLEZ, SPARC64_INSN_BLTZ, SPARC64_INSN_BNEZ
 , SPARC64_INSN_BPCC_BA, SPARC64_INSN_BPCC_BN, SPARC64_INSN_BPCC_BNE, SPARC64_INSN_BPCC_BE
 , SPARC64_INSN_BPCC_BG, SPARC64_INSN_BPCC_BLE, SPARC64_INSN_BPCC_BGE, SPARC64_INSN_BPCC_BL
 , SPARC64_INSN_BPCC_BGU, SPARC64_INSN_BPCC_BLEU, SPARC64_INSN_BPCC_BCC, SPARC64_INSN_BPCC_BCS
 , SPARC64_INSN_BPCC_BPOS, SPARC64_INSN_BPCC_BNEG, SPARC64_INSN_BPCC_BVC, SPARC64_INSN_BPCC_BVS
 , SPARC64_INSN_DONE, SPARC64_INSN_RETRY, SPARC64_INSN_FLUSH, SPARC64_INSN_FLUSH_IMM
 , SPARC64_INSN_FLUSHW, SPARC64_INSN_IMPDEP1, SPARC64_INSN_IMPDEP2, SPARC64_INSN_MEMBAR
 , SPARC64_INSN_MOVA_ICC_ICC, SPARC64_INSN_MOVA_IMM_ICC_ICC, SPARC64_INSN_MOVA_XCC_XCC, SPARC64_INSN_MOVA_IMM_XCC_XCC
 , SPARC64_INSN_MOVN_ICC_ICC, SPARC64_INSN_MOVN_IMM_ICC_ICC, SPARC64_INSN_MOVN_XCC_XCC, SPARC64_INSN_MOVN_IMM_XCC_XCC
 , SPARC64_INSN_MOVNE_ICC_ICC, SPARC64_INSN_MOVNE_IMM_ICC_ICC, SPARC64_INSN_MOVNE_XCC_XCC, SPARC64_INSN_MOVNE_IMM_XCC_XCC
 , SPARC64_INSN_MOVE_ICC_ICC, SPARC64_INSN_MOVE_IMM_ICC_ICC, SPARC64_INSN_MOVE_XCC_XCC, SPARC64_INSN_MOVE_IMM_XCC_XCC
 , SPARC64_INSN_MOVG_ICC_ICC, SPARC64_INSN_MOVG_IMM_ICC_ICC, SPARC64_INSN_MOVG_XCC_XCC, SPARC64_INSN_MOVG_IMM_XCC_XCC
 , SPARC64_INSN_MOVLE_ICC_ICC, SPARC64_INSN_MOVLE_IMM_ICC_ICC, SPARC64_INSN_MOVLE_XCC_XCC, SPARC64_INSN_MOVLE_IMM_XCC_XCC
 , SPARC64_INSN_MOVGE_ICC_ICC, SPARC64_INSN_MOVGE_IMM_ICC_ICC, SPARC64_INSN_MOVGE_XCC_XCC, SPARC64_INSN_MOVGE_IMM_XCC_XCC
 , SPARC64_INSN_MOVL_ICC_ICC, SPARC64_INSN_MOVL_IMM_ICC_ICC, SPARC64_INSN_MOVL_XCC_XCC, SPARC64_INSN_MOVL_IMM_XCC_XCC
 , SPARC64_INSN_MOVGU_ICC_ICC, SPARC64_INSN_MOVGU_IMM_ICC_ICC, SPARC64_INSN_MOVGU_XCC_XCC, SPARC64_INSN_MOVGU_IMM_XCC_XCC
 , SPARC64_INSN_MOVLEU_ICC_ICC, SPARC64_INSN_MOVLEU_IMM_ICC_ICC, SPARC64_INSN_MOVLEU_XCC_XCC, SPARC64_INSN_MOVLEU_IMM_XCC_XCC
 , SPARC64_INSN_MOVCC_ICC_ICC, SPARC64_INSN_MOVCC_IMM_ICC_ICC, SPARC64_INSN_MOVCC_XCC_XCC, SPARC64_INSN_MOVCC_IMM_XCC_XCC
 , SPARC64_INSN_MOVCS_ICC_ICC, SPARC64_INSN_MOVCS_IMM_ICC_ICC, SPARC64_INSN_MOVCS_XCC_XCC, SPARC64_INSN_MOVCS_IMM_XCC_XCC
 , SPARC64_INSN_MOVPOS_ICC_ICC, SPARC64_INSN_MOVPOS_IMM_ICC_ICC, SPARC64_INSN_MOVPOS_XCC_XCC, SPARC64_INSN_MOVPOS_IMM_XCC_XCC
 , SPARC64_INSN_MOVNEG_ICC_ICC, SPARC64_INSN_MOVNEG_IMM_ICC_ICC, SPARC64_INSN_MOVNEG_XCC_XCC, SPARC64_INSN_MOVNEG_IMM_XCC_XCC
 , SPARC64_INSN_MOVVC_ICC_ICC, SPARC64_INSN_MOVVC_IMM_ICC_ICC, SPARC64_INSN_MOVVC_XCC_XCC, SPARC64_INSN_MOVVC_IMM_XCC_XCC
 , SPARC64_INSN_MOVVS_ICC_ICC, SPARC64_INSN_MOVVS_IMM_ICC_ICC, SPARC64_INSN_MOVVS_XCC_XCC, SPARC64_INSN_MOVVS_IMM_XCC_XCC
 , SPARC64_INSN_LDSB_REG_REG, SPARC64_INSN_LDSB_REG_IMM, SPARC64_INSN_LDSB_REG_REG_ASI, SPARC64_INSN_LDUB_REG_REG
 , SPARC64_INSN_LDUB_REG_IMM, SPARC64_INSN_LDUB_REG_REG_ASI, SPARC64_INSN_LDSH_REG_REG, SPARC64_INSN_LDSH_REG_IMM
 , SPARC64_INSN_LDSH_REG_REG_ASI, SPARC64_INSN_LDUH_REG_REG, SPARC64_INSN_LDUH_REG_IMM, SPARC64_INSN_LDUH_REG_REG_ASI
 , SPARC64_INSN_LDSW_REG_REG, SPARC64_INSN_LDSW_REG_IMM, SPARC64_INSN_LDSW_REG_REG_ASI, SPARC64_INSN_LDUW_REG_REG
 , SPARC64_INSN_LDUW_REG_IMM, SPARC64_INSN_LDUW_REG_REG_ASI, SPARC64_INSN_LDX_REG_REG, SPARC64_INSN_LDX_REG_IMM
 , SPARC64_INSN_LDX_REG_REG_ASI, SPARC64_INSN_LDD_REG_REG, SPARC64_INSN_LDD_REG_IMM, SPARC64_INSN_LDD_REG_REG_ASI
 , SPARC64_INSN_STB_REG_REG, SPARC64_INSN_STB_REG_IMM, SPARC64_INSN_STB_REG_REG_ASI, SPARC64_INSN_STH_REG_REG
 , SPARC64_INSN_STH_REG_IMM, SPARC64_INSN_STH_REG_REG_ASI, SPARC64_INSN_ST_REG_REG, SPARC64_INSN_ST_REG_IMM
 , SPARC64_INSN_ST_REG_REG_ASI, SPARC64_INSN_STX_REG_REG, SPARC64_INSN_STX_REG_IMM, SPARC64_INSN_STX_REG_REG_ASI
 , SPARC64_INSN_STD_REG_REG, SPARC64_INSN_STD_REG_IMM, SPARC64_INSN_STD_REG_REG_ASI, SPARC64_INSN_FP_LD_REG_REG
 , SPARC64_INSN_FP_LD_REG_IMM, SPARC64_INSN_FP_LD_REG_REG_ASI, SPARC64_INSN_SETHI, SPARC64_INSN_ADD
 , SPARC64_INSN_ADD_IMM, SPARC64_INSN_SUB, SPARC64_INSN_SUB_IMM, SPARC64_INSN_ADDCC
 , SPARC64_INSN_ADDCC_IMM, SPARC64_INSN_SUBCC, SPARC64_INSN_SUBCC_IMM, SPARC64_INSN_ADDC
 , SPARC64_INSN_ADDC_IMM, SPARC64_INSN_SUBC, SPARC64_INSN_SUBC_IMM, SPARC64_INSN_ADDCCC
 , SPARC64_INSN_ADDCCC_IMM, SPARC64_INSN_SUBCCC, SPARC64_INSN_SUBCCC_IMM, SPARC64_INSN_AND
 , SPARC64_INSN_AND_IMM, SPARC64_INSN_ANDCC, SPARC64_INSN_ANDCC_IMM, SPARC64_INSN_OR
 , SPARC64_INSN_OR_IMM, SPARC64_INSN_ORCC, SPARC64_INSN_ORCC_IMM, SPARC64_INSN_XOR
 , SPARC64_INSN_XOR_IMM, SPARC64_INSN_XORCC, SPARC64_INSN_XORCC_IMM, SPARC64_INSN_ANDN
 , SPARC64_INSN_ANDN_IMM, SPARC64_INSN_ANDNCC, SPARC64_INSN_ANDNCC_IMM, SPARC64_INSN_ORN
 , SPARC64_INSN_ORN_IMM, SPARC64_INSN_ORNCC, SPARC64_INSN_ORNCC_IMM, SPARC64_INSN_XNOR
 , SPARC64_INSN_XNOR_IMM, SPARC64_INSN_XNORCC, SPARC64_INSN_XNORCC_IMM, SPARC64_INSN_SLL
 , SPARC64_INSN_SLL_IMM, SPARC64_INSN_SRL, SPARC64_INSN_SRL_IMM, SPARC64_INSN_SRA
 , SPARC64_INSN_SRA_IMM, SPARC64_INSN_SMUL, SPARC64_INSN_SMUL_IMM, SPARC64_INSN_SMUL_CC
 , SPARC64_INSN_SMUL_CC_IMM, SPARC64_INSN_UMUL, SPARC64_INSN_UMUL_IMM, SPARC64_INSN_UMUL_CC
 , SPARC64_INSN_UMUL_CC_IMM, SPARC64_INSN_MULSCC, SPARC64_INSN_SAVE, SPARC64_INSN_SAVE_IMM
 , SPARC64_INSN_RESTORE, SPARC64_INSN_RESTORE_IMM, SPARC64_INSN_RETT, SPARC64_INSN_RETT_IMM
 , SPARC64_INSN_UNIMP, SPARC64_INSN_CALL, SPARC64_INSN_JMPL, SPARC64_INSN_JMPL_IMM
 , SPARC64_INSN_BA, SPARC64_INSN_TA, SPARC64_INSN_TA_IMM, SPARC64_INSN_BN
 , SPARC64_INSN_TN, SPARC64_INSN_TN_IMM, SPARC64_INSN_BNE, SPARC64_INSN_TNE
 , SPARC64_INSN_TNE_IMM, SPARC64_INSN_BE, SPARC64_INSN_TE, SPARC64_INSN_TE_IMM
 , SPARC64_INSN_BG, SPARC64_INSN_TG, SPARC64_INSN_TG_IMM, SPARC64_INSN_BLE
 , SPARC64_INSN_TLE, SPARC64_INSN_TLE_IMM, SPARC64_INSN_BGE, SPARC64_INSN_TGE
 , SPARC64_INSN_TGE_IMM, SPARC64_INSN_BL, SPARC64_INSN_TL, SPARC64_INSN_TL_IMM
 , SPARC64_INSN_BGU, SPARC64_INSN_TGU, SPARC64_INSN_TGU_IMM, SPARC64_INSN_BLEU
 , SPARC64_INSN_TLEU, SPARC64_INSN_TLEU_IMM, SPARC64_INSN_BCC, SPARC64_INSN_TCC
 , SPARC64_INSN_TCC_IMM, SPARC64_INSN_BCS, SPARC64_INSN_TCS, SPARC64_INSN_TCS_IMM
 , SPARC64_INSN_BPOS, SPARC64_INSN_TPOS, SPARC64_INSN_TPOS_IMM, SPARC64_INSN_BNEG
 , SPARC64_INSN_TNEG, SPARC64_INSN_TNEG_IMM, SPARC64_INSN_BVC, SPARC64_INSN_TVC
 , SPARC64_INSN_TVC_IMM, SPARC64_INSN_BVS, SPARC64_INSN_TVS, SPARC64_INSN_TVS_IMM
 , SPARC64_INSN_LDSTUB_REG_REG, SPARC64_INSN_LDSTUB_REG_IMM, SPARC64_INSN_LDSTUB_REG_REG_ASI, SPARC64_INSN_SWAP_REG_REG
 , SPARC64_INSN_SWAP_REG_IMM, SPARC64_INSN_SWAP_REG_REG_ASI, SPARC64_INSN_MAX
} SPARC64_INSN_TYPE;

#if ! WITH_SEM_SWITCH_FULL
#define SEMFULL(fn) extern SEMANTIC_FN CONCAT3 (sparc64,_sem_,fn);
#else
#define SEMFULL(fn)
#endif

#if ! WITH_SEM_SWITCH_FAST
#define SEMFAST(fn) extern SEMANTIC_FN CONCAT3 (sparc64,_semf_,fn);
#else
#define SEMFAST(fn)
#endif

#define SEM(fn) SEMFULL (fn) SEMFAST (fn)

/* The function version of the before/after handlers is always needed,
   so we always want the SEMFULL declaration of them.  */
extern SEMANTIC_FN CONCAT3 (sparc64,_sem_,x_before);
extern SEMANTIC_FN CONCAT3 (sparc64,_sem_,x_after);

SEM (x_invalid)
SEM (x_after)
SEM (x_before)
SEM (x_cti_chain)
SEM (x_chain)
SEM (x_begin)
SEM (beqz)
SEM (bgez)
SEM (bgtz)
SEM (blez)
SEM (bltz)
SEM (bnez)
SEM (bpcc_ba)
SEM (bpcc_bn)
SEM (bpcc_bne)
SEM (bpcc_be)
SEM (bpcc_bg)
SEM (bpcc_ble)
SEM (bpcc_bge)
SEM (bpcc_bl)
SEM (bpcc_bgu)
SEM (bpcc_bleu)
SEM (bpcc_bcc)
SEM (bpcc_bcs)
SEM (bpcc_bpos)
SEM (bpcc_bneg)
SEM (bpcc_bvc)
SEM (bpcc_bvs)
SEM (done)
SEM (retry)
SEM (flush)
SEM (flush_imm)
SEM (flushw)
SEM (impdep1)
SEM (impdep2)
SEM (membar)
SEM (mova_icc_icc)
SEM (mova_imm_icc_icc)
SEM (mova_xcc_xcc)
SEM (mova_imm_xcc_xcc)
SEM (movn_icc_icc)
SEM (movn_imm_icc_icc)
SEM (movn_xcc_xcc)
SEM (movn_imm_xcc_xcc)
SEM (movne_icc_icc)
SEM (movne_imm_icc_icc)
SEM (movne_xcc_xcc)
SEM (movne_imm_xcc_xcc)
SEM (move_icc_icc)
SEM (move_imm_icc_icc)
SEM (move_xcc_xcc)
SEM (move_imm_xcc_xcc)
SEM (movg_icc_icc)
SEM (movg_imm_icc_icc)
SEM (movg_xcc_xcc)
SEM (movg_imm_xcc_xcc)
SEM (movle_icc_icc)
SEM (movle_imm_icc_icc)
SEM (movle_xcc_xcc)
SEM (movle_imm_xcc_xcc)
SEM (movge_icc_icc)
SEM (movge_imm_icc_icc)
SEM (movge_xcc_xcc)
SEM (movge_imm_xcc_xcc)
SEM (movl_icc_icc)
SEM (movl_imm_icc_icc)
SEM (movl_xcc_xcc)
SEM (movl_imm_xcc_xcc)
SEM (movgu_icc_icc)
SEM (movgu_imm_icc_icc)
SEM (movgu_xcc_xcc)
SEM (movgu_imm_xcc_xcc)
SEM (movleu_icc_icc)
SEM (movleu_imm_icc_icc)
SEM (movleu_xcc_xcc)
SEM (movleu_imm_xcc_xcc)
SEM (movcc_icc_icc)
SEM (movcc_imm_icc_icc)
SEM (movcc_xcc_xcc)
SEM (movcc_imm_xcc_xcc)
SEM (movcs_icc_icc)
SEM (movcs_imm_icc_icc)
SEM (movcs_xcc_xcc)
SEM (movcs_imm_xcc_xcc)
SEM (movpos_icc_icc)
SEM (movpos_imm_icc_icc)
SEM (movpos_xcc_xcc)
SEM (movpos_imm_xcc_xcc)
SEM (movneg_icc_icc)
SEM (movneg_imm_icc_icc)
SEM (movneg_xcc_xcc)
SEM (movneg_imm_xcc_xcc)
SEM (movvc_icc_icc)
SEM (movvc_imm_icc_icc)
SEM (movvc_xcc_xcc)
SEM (movvc_imm_xcc_xcc)
SEM (movvs_icc_icc)
SEM (movvs_imm_icc_icc)
SEM (movvs_xcc_xcc)
SEM (movvs_imm_xcc_xcc)
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
SEM (ldx_reg_reg)
SEM (ldx_reg_imm)
SEM (ldx_reg_reg_asi)
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
SEM (stx_reg_reg)
SEM (stx_reg_imm)
SEM (stx_reg_reg_asi)
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
SEM (addc)
SEM (addc_imm)
SEM (subc)
SEM (subc_imm)
SEM (addccc)
SEM (addccc_imm)
SEM (subccc)
SEM (subccc_imm)
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
SEM (ldstub_reg_reg)
SEM (ldstub_reg_imm)
SEM (ldstub_reg_reg_asi)
SEM (swap_reg_reg)
SEM (swap_reg_imm)
SEM (swap_reg_reg_asi)

#undef SEMFULL
#undef SEMFAST
#undef SEM

/* Function unit handlers (user written).  */

extern int sparc64_model_sparc64_def_u_exec (SIM_CPU *, const IDESC *, int /*unit_num*/, int /*referenced*/);

/* Profiling before/after handlers (user written) */

extern void sparc64_model_insn_before (SIM_CPU *, int /*first_p*/);
extern void sparc64_model_insn_after (SIM_CPU *, int /*last_p*/, int /*cycles*/);

#endif /* SPARC64_DECODE_H */
