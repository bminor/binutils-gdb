/* Misc. support for CPU family sparc32.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#define WANT_CPU sparc32
#define WANT_CPU_SPARC32

#include "sim-main.h"

/* Get the value of h-pc.  */

USI
sparc32_h_pc_get (SIM_CPU *current_cpu)
{
  return CPU (h_pc);
}

/* Set a value for h-pc.  */

void
sparc32_h_pc_set (SIM_CPU *current_cpu, USI newval)
{
  CPU (h_pc) = newval;
}

/* Get the value of h-npc.  */

SI
sparc32_h_npc_get (SIM_CPU *current_cpu)
{
  return CPU (h_npc);
}

/* Set a value for h-npc.  */

void
sparc32_h_npc_set (SIM_CPU *current_cpu, SI newval)
{
  CPU (h_npc) = newval;
}

/* Get the value of h-gr.  */

SI
sparc32_h_gr_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_GR (regno);
}

/* Set a value for h-gr.  */

void
sparc32_h_gr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_GR (regno, newval);
}

/* Get the value of h-icc-c.  */

BI
sparc32_h_icc_c_get (SIM_CPU *current_cpu)
{
  return CPU (h_icc_c);
}

/* Set a value for h-icc-c.  */

void
sparc32_h_icc_c_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_icc_c) = newval;
}

/* Get the value of h-icc-n.  */

BI
sparc32_h_icc_n_get (SIM_CPU *current_cpu)
{
  return CPU (h_icc_n);
}

/* Set a value for h-icc-n.  */

void
sparc32_h_icc_n_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_icc_n) = newval;
}

/* Get the value of h-icc-v.  */

BI
sparc32_h_icc_v_get (SIM_CPU *current_cpu)
{
  return CPU (h_icc_v);
}

/* Set a value for h-icc-v.  */

void
sparc32_h_icc_v_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_icc_v) = newval;
}

/* Get the value of h-icc-z.  */

BI
sparc32_h_icc_z_get (SIM_CPU *current_cpu)
{
  return CPU (h_icc_z);
}

/* Set a value for h-icc-z.  */

void
sparc32_h_icc_z_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_icc_z) = newval;
}

/* Get the value of h-xcc-c.  */

BI
sparc32_h_xcc_c_get (SIM_CPU *current_cpu)
{
  return CPU (h_xcc_c);
}

/* Set a value for h-xcc-c.  */

void
sparc32_h_xcc_c_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_xcc_c) = newval;
}

/* Get the value of h-xcc-n.  */

BI
sparc32_h_xcc_n_get (SIM_CPU *current_cpu)
{
  return CPU (h_xcc_n);
}

/* Set a value for h-xcc-n.  */

void
sparc32_h_xcc_n_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_xcc_n) = newval;
}

/* Get the value of h-xcc-v.  */

BI
sparc32_h_xcc_v_get (SIM_CPU *current_cpu)
{
  return CPU (h_xcc_v);
}

/* Set a value for h-xcc-v.  */

void
sparc32_h_xcc_v_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_xcc_v) = newval;
}

/* Get the value of h-xcc-z.  */

BI
sparc32_h_xcc_z_get (SIM_CPU *current_cpu)
{
  return CPU (h_xcc_z);
}

/* Set a value for h-xcc-z.  */

void
sparc32_h_xcc_z_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_xcc_z) = newval;
}

/* Get the value of h-y.  */

SI
sparc32_h_y_get (SIM_CPU *current_cpu)
{
  return GET_H_Y ();
}

/* Set a value for h-y.  */

void
sparc32_h_y_set (SIM_CPU *current_cpu, SI newval)
{
  SET_H_Y (newval);
}

/* Get the value of h-asr.  */

SI
sparc32_h_asr_get (SIM_CPU *current_cpu, UINT regno)
{
  return CPU (h_asr[regno]);
}

/* Set a value for h-asr.  */

void
sparc32_h_asr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  CPU (h_asr[regno]) = newval;
}

/* Get the value of h-annul-p.  */

BI
sparc32_h_annul_p_get (SIM_CPU *current_cpu)
{
  return CPU (h_annul_p);
}

/* Set a value for h-annul-p.  */

void
sparc32_h_annul_p_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_annul_p) = newval;
}

/* Get the value of h-fr.  */

SF
sparc32_h_fr_get (SIM_CPU *current_cpu, UINT regno)
{
  return CPU (h_fr[regno]);
}

/* Set a value for h-fr.  */

void
sparc32_h_fr_set (SIM_CPU *current_cpu, UINT regno, SF newval)
{
  CPU (h_fr[regno]) = newval;
}

/* Get the value of h-psr.  */

USI
sparc32_h_psr_get (SIM_CPU *current_cpu)
{
  return GET_H_PSR ();
}

/* Set a value for h-psr.  */

void
sparc32_h_psr_set (SIM_CPU *current_cpu, USI newval)
{
  SET_H_PSR (newval);
}

/* Get the value of h-s.  */

BI
sparc32_h_s_get (SIM_CPU *current_cpu)
{
  return CPU (h_s);
}

/* Set a value for h-s.  */

void
sparc32_h_s_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_s) = newval;
}

/* Get the value of h-ps.  */

BI
sparc32_h_ps_get (SIM_CPU *current_cpu)
{
  return CPU (h_ps);
}

/* Set a value for h-ps.  */

void
sparc32_h_ps_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_ps) = newval;
}

/* Get the value of h-pil.  */

UQI
sparc32_h_pil_get (SIM_CPU *current_cpu)
{
  return CPU (h_pil);
}

/* Set a value for h-pil.  */

void
sparc32_h_pil_set (SIM_CPU *current_cpu, UQI newval)
{
  CPU (h_pil) = newval;
}

/* Get the value of h-et.  */

BI
sparc32_h_et_get (SIM_CPU *current_cpu)
{
  return CPU (h_et);
}

/* Set a value for h-et.  */

void
sparc32_h_et_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_et) = newval;
}

/* Get the value of h-tbr.  */

SI
sparc32_h_tbr_get (SIM_CPU *current_cpu)
{
  return GET_H_TBR ();
}

/* Set a value for h-tbr.  */

void
sparc32_h_tbr_set (SIM_CPU *current_cpu, SI newval)
{
  SET_H_TBR (newval);
}

/* Get the value of h-cwp.  */

UQI
sparc32_h_cwp_get (SIM_CPU *current_cpu)
{
  return GET_H_CWP ();
}

/* Set a value for h-cwp.  */

void
sparc32_h_cwp_set (SIM_CPU *current_cpu, UQI newval)
{
  SET_H_CWP (newval);
}

/* Get the value of h-wim.  */

USI
sparc32_h_wim_get (SIM_CPU *current_cpu)
{
  return GET_H_WIM ();
}

/* Set a value for h-wim.  */

void
sparc32_h_wim_set (SIM_CPU *current_cpu, USI newval)
{
  SET_H_WIM (newval);
}

/* Get the value of h-ag.  */

QI
sparc32_h_ag_get (SIM_CPU *current_cpu)
{
  return CPU (h_ag);
}

/* Set a value for h-ag.  */

void
sparc32_h_ag_set (SIM_CPU *current_cpu, QI newval)
{
  CPU (h_ag) = newval;
}

/* Get the value of h-ec.  */

BI
sparc32_h_ec_get (SIM_CPU *current_cpu)
{
  return CPU (h_ec);
}

/* Set a value for h-ec.  */

void
sparc32_h_ec_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_ec) = newval;
}

/* Get the value of h-ef.  */

BI
sparc32_h_ef_get (SIM_CPU *current_cpu)
{
  return CPU (h_ef);
}

/* Set a value for h-ef.  */

void
sparc32_h_ef_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_ef) = newval;
}

/* Get the value of h-fsr.  */

USI
sparc32_h_fsr_get (SIM_CPU *current_cpu)
{
  return CPU (h_fsr);
}

/* Set a value for h-fsr.  */

void
sparc32_h_fsr_set (SIM_CPU *current_cpu, USI newval)
{
  CPU (h_fsr) = newval;
}

/* Record trace results for INSN.  */

void
sparc32_record_trace_results (SIM_CPU *current_cpu, CGEN_INSN *insn,
			    int *indices, TRACE_RECORD *tr)
{
}
