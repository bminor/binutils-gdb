/* Simulator header for sparc.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#ifndef SPARC_ARCH_H
#define SPARC_ARCH_H

#define TARGET_BIG_ENDIAN 1

/* Cover fns for register access.  */
USI a_sparc_h_pc_get (SIM_CPU *);
void a_sparc_h_pc_set (SIM_CPU *, USI);
SI a_sparc_h_npc_get (SIM_CPU *);
void a_sparc_h_npc_set (SIM_CPU *, SI);
SI a_sparc_h_gr_get (SIM_CPU *, UINT);
void a_sparc_h_gr_set (SIM_CPU *, UINT, SI);
BI a_sparc_h_icc_c_get (SIM_CPU *);
void a_sparc_h_icc_c_set (SIM_CPU *, BI);
BI a_sparc_h_icc_n_get (SIM_CPU *);
void a_sparc_h_icc_n_set (SIM_CPU *, BI);
BI a_sparc_h_icc_v_get (SIM_CPU *);
void a_sparc_h_icc_v_set (SIM_CPU *, BI);
BI a_sparc_h_icc_z_get (SIM_CPU *);
void a_sparc_h_icc_z_set (SIM_CPU *, BI);
BI a_sparc_h_xcc_c_get (SIM_CPU *);
void a_sparc_h_xcc_c_set (SIM_CPU *, BI);
BI a_sparc_h_xcc_n_get (SIM_CPU *);
void a_sparc_h_xcc_n_set (SIM_CPU *, BI);
BI a_sparc_h_xcc_v_get (SIM_CPU *);
void a_sparc_h_xcc_v_set (SIM_CPU *, BI);
BI a_sparc_h_xcc_z_get (SIM_CPU *);
void a_sparc_h_xcc_z_set (SIM_CPU *, BI);
SI a_sparc_h_y_get (SIM_CPU *);
void a_sparc_h_y_set (SIM_CPU *, SI);
SI a_sparc_h_asr_get (SIM_CPU *, UINT);
void a_sparc_h_asr_set (SIM_CPU *, UINT, SI);
BI a_sparc_h_annul_p_get (SIM_CPU *);
void a_sparc_h_annul_p_set (SIM_CPU *, BI);
SF a_sparc_h_fr_get (SIM_CPU *, UINT);
void a_sparc_h_fr_set (SIM_CPU *, UINT, SF);
USI a_sparc_h_psr_get (SIM_CPU *);
void a_sparc_h_psr_set (SIM_CPU *, USI);
BI a_sparc_h_s_get (SIM_CPU *);
void a_sparc_h_s_set (SIM_CPU *, BI);
BI a_sparc_h_ps_get (SIM_CPU *);
void a_sparc_h_ps_set (SIM_CPU *, BI);
UQI a_sparc_h_pil_get (SIM_CPU *);
void a_sparc_h_pil_set (SIM_CPU *, UQI);
BI a_sparc_h_et_get (SIM_CPU *);
void a_sparc_h_et_set (SIM_CPU *, BI);
SI a_sparc_h_tbr_get (SIM_CPU *);
void a_sparc_h_tbr_set (SIM_CPU *, SI);
UQI a_sparc_h_cwp_get (SIM_CPU *);
void a_sparc_h_cwp_set (SIM_CPU *, UQI);
USI a_sparc_h_wim_get (SIM_CPU *);
void a_sparc_h_wim_set (SIM_CPU *, USI);
QI a_sparc_h_ag_get (SIM_CPU *);
void a_sparc_h_ag_set (SIM_CPU *, QI);
BI a_sparc_h_ec_get (SIM_CPU *);
void a_sparc_h_ec_set (SIM_CPU *, BI);
BI a_sparc_h_ef_get (SIM_CPU *);
void a_sparc_h_ef_set (SIM_CPU *, BI);
USI a_sparc_h_fsr_get (SIM_CPU *);
void a_sparc_h_fsr_set (SIM_CPU *, USI);
UDI a_sparc_h_ver_get (SIM_CPU *);
void a_sparc_h_ver_set (SIM_CPU *, UDI);
UDI a_sparc_h_pstate_get (SIM_CPU *);
void a_sparc_h_pstate_set (SIM_CPU *, UDI);
UDI a_sparc_h_tba_get (SIM_CPU *);
void a_sparc_h_tba_set (SIM_CPU *, UDI);
UDI a_sparc_h_tt_get (SIM_CPU *);
void a_sparc_h_tt_set (SIM_CPU *, UDI);
UQI a_sparc_h_asi_get (SIM_CPU *);
void a_sparc_h_asi_set (SIM_CPU *, UQI);
UQI a_sparc_h_tl_get (SIM_CPU *);
void a_sparc_h_tl_set (SIM_CPU *, UQI);
UDI a_sparc_h_tpc_get (SIM_CPU *);
void a_sparc_h_tpc_set (SIM_CPU *, UDI);
UDI a_sparc_h_tnpc_get (SIM_CPU *);
void a_sparc_h_tnpc_set (SIM_CPU *, UDI);
UDI a_sparc_h_tstate_get (SIM_CPU *);
void a_sparc_h_tstate_set (SIM_CPU *, UDI);
UDI a_sparc_h_tick_get (SIM_CPU *);
void a_sparc_h_tick_set (SIM_CPU *, UDI);
UDI a_sparc_h_cansave_get (SIM_CPU *);
void a_sparc_h_cansave_set (SIM_CPU *, UDI);
UDI a_sparc_h_canrestore_get (SIM_CPU *);
void a_sparc_h_canrestore_set (SIM_CPU *, UDI);
UDI a_sparc_h_otherwin_get (SIM_CPU *);
void a_sparc_h_otherwin_set (SIM_CPU *, UDI);
UDI a_sparc_h_cleanwin_get (SIM_CPU *);
void a_sparc_h_cleanwin_set (SIM_CPU *, UDI);
UDI a_sparc_h_wstate_get (SIM_CPU *);
void a_sparc_h_wstate_set (SIM_CPU *, UDI);
UQI a_sparc_h_fcc0_get (SIM_CPU *);
void a_sparc_h_fcc0_set (SIM_CPU *, UQI);
UQI a_sparc_h_fcc1_get (SIM_CPU *);
void a_sparc_h_fcc1_set (SIM_CPU *, UQI);
UQI a_sparc_h_fcc2_get (SIM_CPU *);
void a_sparc_h_fcc2_set (SIM_CPU *, UQI);
UQI a_sparc_h_fcc3_get (SIM_CPU *);
void a_sparc_h_fcc3_set (SIM_CPU *, UQI);
UQI a_sparc_h_fsr_rd_get (SIM_CPU *);
void a_sparc_h_fsr_rd_set (SIM_CPU *, UQI);
UQI a_sparc_h_fsr_tem_get (SIM_CPU *);
void a_sparc_h_fsr_tem_set (SIM_CPU *, UQI);
BI a_sparc_h_fsr_ns_get (SIM_CPU *);
void a_sparc_h_fsr_ns_set (SIM_CPU *, BI);
UQI a_sparc_h_fsr_ver_get (SIM_CPU *);
void a_sparc_h_fsr_ver_set (SIM_CPU *, UQI);
UQI a_sparc_h_fsr_ftt_get (SIM_CPU *);
void a_sparc_h_fsr_ftt_set (SIM_CPU *, UQI);
BI a_sparc_h_fsr_qne_get (SIM_CPU *);
void a_sparc_h_fsr_qne_set (SIM_CPU *, BI);
UQI a_sparc_h_fsr_aexc_get (SIM_CPU *);
void a_sparc_h_fsr_aexc_set (SIM_CPU *, UQI);
UQI a_sparc_h_fsr_cexc_get (SIM_CPU *);
void a_sparc_h_fsr_cexc_set (SIM_CPU *, UQI);
BI a_sparc_h_fpsr_fef_get (SIM_CPU *);
void a_sparc_h_fpsr_fef_set (SIM_CPU *, BI);
BI a_sparc_h_fpsr_du_get (SIM_CPU *);
void a_sparc_h_fpsr_du_set (SIM_CPU *, BI);
BI a_sparc_h_fpsr_dl_get (SIM_CPU *);
void a_sparc_h_fpsr_dl_set (SIM_CPU *, BI);
UQI a_sparc_h_fpsr_get (SIM_CPU *);
void a_sparc_h_fpsr_set (SIM_CPU *, UQI);

/* Enum declaration for model types.  */
typedef enum model_type {
  MODEL_SPARC32_DEF, MODEL_SPARC64_DEF, MODEL_MAX
} MODEL_TYPE;

#define MAX_MODELS ((int) MODEL_MAX)

/* Enum declaration for unit types.  */
typedef enum unit_type {
  UNIT_NONE, UNIT_SPARC32_DEF_U_EXEC, UNIT_SPARC64_DEF_U_EXEC, UNIT_MAX
} UNIT_TYPE;

#define MAX_UNITS (1)

#endif /* SPARC_ARCH_H */
