/* Misc. support for CPU family sparc64.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#define WANT_CPU sparc64
#define WANT_CPU_SPARC64

#include "sim-main.h"

/* Get the value of h-pc.  */

USI
sparc64_h_pc_get (SIM_CPU *current_cpu)
{
  return CPU (h_pc);
}

/* Set a value for h-pc.  */

void
sparc64_h_pc_set (SIM_CPU *current_cpu, USI newval)
{
  CPU (h_pc) = newval;
}

/* Get the value of h-npc.  */

SI
sparc64_h_npc_get (SIM_CPU *current_cpu)
{
  return CPU (h_npc);
}

/* Set a value for h-npc.  */

void
sparc64_h_npc_set (SIM_CPU *current_cpu, SI newval)
{
  CPU (h_npc) = newval;
}

/* Get the value of h-gr.  */

SI
sparc64_h_gr_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_GR (regno);
}

/* Set a value for h-gr.  */

void
sparc64_h_gr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_GR (regno, newval);
}

/* Get the value of h-icc-c.  */

BI
sparc64_h_icc_c_get (SIM_CPU *current_cpu)
{
  return CPU (h_icc_c);
}

/* Set a value for h-icc-c.  */

void
sparc64_h_icc_c_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_icc_c) = newval;
}

/* Get the value of h-icc-n.  */

BI
sparc64_h_icc_n_get (SIM_CPU *current_cpu)
{
  return CPU (h_icc_n);
}

/* Set a value for h-icc-n.  */

void
sparc64_h_icc_n_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_icc_n) = newval;
}

/* Get the value of h-icc-v.  */

BI
sparc64_h_icc_v_get (SIM_CPU *current_cpu)
{
  return CPU (h_icc_v);
}

/* Set a value for h-icc-v.  */

void
sparc64_h_icc_v_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_icc_v) = newval;
}

/* Get the value of h-icc-z.  */

BI
sparc64_h_icc_z_get (SIM_CPU *current_cpu)
{
  return CPU (h_icc_z);
}

/* Set a value for h-icc-z.  */

void
sparc64_h_icc_z_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_icc_z) = newval;
}

/* Get the value of h-xcc-c.  */

BI
sparc64_h_xcc_c_get (SIM_CPU *current_cpu)
{
  return CPU (h_xcc_c);
}

/* Set a value for h-xcc-c.  */

void
sparc64_h_xcc_c_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_xcc_c) = newval;
}

/* Get the value of h-xcc-n.  */

BI
sparc64_h_xcc_n_get (SIM_CPU *current_cpu)
{
  return CPU (h_xcc_n);
}

/* Set a value for h-xcc-n.  */

void
sparc64_h_xcc_n_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_xcc_n) = newval;
}

/* Get the value of h-xcc-v.  */

BI
sparc64_h_xcc_v_get (SIM_CPU *current_cpu)
{
  return CPU (h_xcc_v);
}

/* Set a value for h-xcc-v.  */

void
sparc64_h_xcc_v_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_xcc_v) = newval;
}

/* Get the value of h-xcc-z.  */

BI
sparc64_h_xcc_z_get (SIM_CPU *current_cpu)
{
  return CPU (h_xcc_z);
}

/* Set a value for h-xcc-z.  */

void
sparc64_h_xcc_z_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_xcc_z) = newval;
}

/* Get the value of h-y.  */

SI
sparc64_h_y_get (SIM_CPU *current_cpu)
{
  return GET_H_Y ();
}

/* Set a value for h-y.  */

void
sparc64_h_y_set (SIM_CPU *current_cpu, SI newval)
{
  SET_H_Y (newval);
}

/* Get the value of h-asr.  */

SI
sparc64_h_asr_get (SIM_CPU *current_cpu, UINT regno)
{
  return CPU (h_asr[regno]);
}

/* Set a value for h-asr.  */

void
sparc64_h_asr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  CPU (h_asr[regno]) = newval;
}

/* Get the value of h-annul-p.  */

BI
sparc64_h_annul_p_get (SIM_CPU *current_cpu)
{
  return CPU (h_annul_p);
}

/* Set a value for h-annul-p.  */

void
sparc64_h_annul_p_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_annul_p) = newval;
}

/* Get the value of h-fr.  */

SF
sparc64_h_fr_get (SIM_CPU *current_cpu, UINT regno)
{
  return CPU (h_fr[regno]);
}

/* Set a value for h-fr.  */

void
sparc64_h_fr_set (SIM_CPU *current_cpu, UINT regno, SF newval)
{
  CPU (h_fr[regno]) = newval;
}

/* Get the value of h-ver.  */

UDI
sparc64_h_ver_get (SIM_CPU *current_cpu)
{
  return CPU (h_ver);
}

/* Set a value for h-ver.  */

void
sparc64_h_ver_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_ver) = newval;
}

/* Get the value of h-pstate.  */

UDI
sparc64_h_pstate_get (SIM_CPU *current_cpu)
{
  return CPU (h_pstate);
}

/* Set a value for h-pstate.  */

void
sparc64_h_pstate_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_pstate) = newval;
}

/* Get the value of h-tba.  */

UDI
sparc64_h_tba_get (SIM_CPU *current_cpu)
{
  return CPU (h_tba);
}

/* Set a value for h-tba.  */

void
sparc64_h_tba_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_tba) = newval;
}

/* Get the value of h-tt.  */

UDI
sparc64_h_tt_get (SIM_CPU *current_cpu)
{
  return CPU (h_tt);
}

/* Set a value for h-tt.  */

void
sparc64_h_tt_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_tt) = newval;
}

/* Get the value of h-tpc.  */

UDI
sparc64_h_tpc_get (SIM_CPU *current_cpu)
{
  return CPU (h_tpc);
}

/* Set a value for h-tpc.  */

void
sparc64_h_tpc_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_tpc) = newval;
}

/* Get the value of h-tnpc.  */

UDI
sparc64_h_tnpc_get (SIM_CPU *current_cpu)
{
  return CPU (h_tnpc);
}

/* Set a value for h-tnpc.  */

void
sparc64_h_tnpc_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_tnpc) = newval;
}

/* Get the value of h-tstate.  */

UDI
sparc64_h_tstate_get (SIM_CPU *current_cpu)
{
  return CPU (h_tstate);
}

/* Set a value for h-tstate.  */

void
sparc64_h_tstate_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_tstate) = newval;
}

/* Get the value of h-tl.  */

UQI
sparc64_h_tl_get (SIM_CPU *current_cpu)
{
  return CPU (h_tl);
}

/* Set a value for h-tl.  */

void
sparc64_h_tl_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_tl) = newval;
}

/* Get the value of h-asi.  */

UQI
sparc64_h_asi_get (SIM_CPU *current_cpu)
{
  return CPU (h_asi);
}

/* Set a value for h-asi.  */

void
sparc64_h_asi_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_asi) = newval;
}

/* Get the value of h-tick.  */

UDI
sparc64_h_tick_get (SIM_CPU *current_cpu)
{
  return CPU (h_tick);
}

/* Set a value for h-tick.  */

void
sparc64_h_tick_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_tick) = newval;
}

/* Get the value of h-cansave.  */

UDI
sparc64_h_cansave_get (SIM_CPU *current_cpu)
{
  return CPU (h_cansave);
}

/* Set a value for h-cansave.  */

void
sparc64_h_cansave_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_cansave) = newval;
}

/* Get the value of h-canrestore.  */

UDI
sparc64_h_canrestore_get (SIM_CPU *current_cpu)
{
  return CPU (h_canrestore);
}

/* Set a value for h-canrestore.  */

void
sparc64_h_canrestore_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_canrestore) = newval;
}

/* Get the value of h-otherwin.  */

UDI
sparc64_h_otherwin_get (SIM_CPU *current_cpu)
{
  return CPU (h_otherwin);
}

/* Set a value for h-otherwin.  */

void
sparc64_h_otherwin_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_otherwin) = newval;
}

/* Get the value of h-cleanwin.  */

UDI
sparc64_h_cleanwin_get (SIM_CPU *current_cpu)
{
  return CPU (h_cleanwin);
}

/* Set a value for h-cleanwin.  */

void
sparc64_h_cleanwin_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_cleanwin) = newval;
}

/* Get the value of h-wstate.  */

UDI
sparc64_h_wstate_get (SIM_CPU *current_cpu)
{
  return CPU (h_wstate);
}

/* Set a value for h-wstate.  */

void
sparc64_h_wstate_set (SIM_CPU *current_cpu, UDI newval)
{
  CPU (h_wstate) = newval;
}

/* Get the value of h-fcc0.  */

UQI
sparc64_h_fcc0_get (SIM_CPU *current_cpu)
{
  return CPU (h_fcc0);
}

/* Set a value for h-fcc0.  */

void
sparc64_h_fcc0_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_fcc0) = newval;
}

/* Get the value of h-fcc1.  */

UQI
sparc64_h_fcc1_get (SIM_CPU *current_cpu)
{
  return CPU (h_fcc1);
}

/* Set a value for h-fcc1.  */

void
sparc64_h_fcc1_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_fcc1) = newval;
}

/* Get the value of h-fcc2.  */

UQI
sparc64_h_fcc2_get (SIM_CPU *current_cpu)
{
  return CPU (h_fcc2);
}

/* Set a value for h-fcc2.  */

void
sparc64_h_fcc2_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_fcc2) = newval;
}

/* Get the value of h-fcc3.  */

UQI
sparc64_h_fcc3_get (SIM_CPU *current_cpu)
{
  return CPU (h_fcc3);
}

/* Set a value for h-fcc3.  */

void
sparc64_h_fcc3_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_fcc3) = newval;
}

/* Get the value of h-fsr-rd.  */

UQI
sparc64_h_fsr_rd_get (SIM_CPU *current_cpu)
{
  return CPU (h_fsr_rd);
}

/* Set a value for h-fsr-rd.  */

void
sparc64_h_fsr_rd_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_fsr_rd) = newval;
}

/* Get the value of h-fsr-tem.  */

UQI
sparc64_h_fsr_tem_get (SIM_CPU *current_cpu)
{
  return CPU (h_fsr_tem);
}

/* Set a value for h-fsr-tem.  */

void
sparc64_h_fsr_tem_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_fsr_tem) = newval;
}

/* Get the value of h-fsr-ns.  */

BI
sparc64_h_fsr_ns_get (SIM_CPU *current_cpu)
{
  return CPU (h_fsr_ns);
}

/* Set a value for h-fsr-ns.  */

void
sparc64_h_fsr_ns_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_fsr_ns) = newval;
}

/* Get the value of h-fsr-ver.  */

UQI
sparc64_h_fsr_ver_get (SIM_CPU *current_cpu)
{
  return CPU (h_fsr_ver);
}

/* Set a value for h-fsr-ver.  */

void
sparc64_h_fsr_ver_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_fsr_ver) = newval;
}

/* Get the value of h-fsr-ftt.  */

UQI
sparc64_h_fsr_ftt_get (SIM_CPU *current_cpu)
{
  return CPU (h_fsr_ftt);
}

/* Set a value for h-fsr-ftt.  */

void
sparc64_h_fsr_ftt_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_fsr_ftt) = newval;
}

/* Get the value of h-fsr-qne.  */

BI
sparc64_h_fsr_qne_get (SIM_CPU *current_cpu)
{
  return CPU (h_fsr_qne);
}

/* Set a value for h-fsr-qne.  */

void
sparc64_h_fsr_qne_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_fsr_qne) = newval;
}

/* Get the value of h-fsr-aexc.  */

UQI
sparc64_h_fsr_aexc_get (SIM_CPU *current_cpu)
{
  return CPU (h_fsr_aexc);
}

/* Set a value for h-fsr-aexc.  */

void
sparc64_h_fsr_aexc_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_fsr_aexc) = newval;
}

/* Get the value of h-fsr-cexc.  */

UQI
sparc64_h_fsr_cexc_get (SIM_CPU *current_cpu)
{
  return CPU (h_fsr_cexc);
}

/* Set a value for h-fsr-cexc.  */

void
sparc64_h_fsr_cexc_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_fsr_cexc) = newval;
}

/* Get the value of h-fpsr-fef.  */

BI
sparc64_h_fpsr_fef_get (SIM_CPU *current_cpu)
{
  return CPU (h_fpsr_fef);
}

/* Set a value for h-fpsr-fef.  */

void
sparc64_h_fpsr_fef_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_fpsr_fef) = newval;
}

/* Get the value of h-fpsr-du.  */

BI
sparc64_h_fpsr_du_get (SIM_CPU *current_cpu)
{
  return CPU (h_fpsr_du);
}

/* Set a value for h-fpsr-du.  */

void
sparc64_h_fpsr_du_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_fpsr_du) = newval;
}

/* Get the value of h-fpsr-dl.  */

BI
sparc64_h_fpsr_dl_get (SIM_CPU *current_cpu)
{
  return CPU (h_fpsr_dl);
}

/* Set a value for h-fpsr-dl.  */

void
sparc64_h_fpsr_dl_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_fpsr_dl) = newval;
}

/* Get the value of h-fpsr.  */

UQI
sparc64_h_fpsr_get (SIM_CPU *current_cpu)
{
  return GET_H_FPSR ();
}

/* Set a value for h-fpsr.  */

void
sparc64_h_fpsr_set (SIM_CPU *current_cpu, UQI newval)
{
  SET_H_FPSR (newval);
}

/* Record trace results for INSN.  */

void
sparc64_record_trace_results (SIM_CPU *current_cpu, CGEN_INSN *insn,
			    int *indices, TRACE_RECORD *tr)
{
}
