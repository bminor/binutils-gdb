/* Misc. support for CPU family a5f.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright 1996-2007 Free Software Foundation, Inc.

This file is part of the GNU simulators.

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

*/

#define WANT_CPU a5f
#define WANT_CPU_A5F

#include "sim-main.h"
#include "cgen-ops.h"

/* Get the value of h-lbit.  */

BI
a5f_h_lbit_get (SIM_CPU *current_cpu)
{
  return CPU (h_lbit);
}

/* Set a value for h-lbit.  */

void
a5f_h_lbit_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_lbit) = newval;
}

/* Get the value of h-zbit.  */

BI
a5f_h_zbit_get (SIM_CPU *current_cpu)
{
  return CPU (h_zbit);
}

/* Set a value for h-zbit.  */

void
a5f_h_zbit_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_zbit) = newval;
}

/* Get the value of h-nbit.  */

BI
a5f_h_nbit_get (SIM_CPU *current_cpu)
{
  return CPU (h_nbit);
}

/* Set a value for h-nbit.  */

void
a5f_h_nbit_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_nbit) = newval;
}

/* Get the value of h-cbit.  */

BI
a5f_h_cbit_get (SIM_CPU *current_cpu)
{
  return CPU (h_cbit);
}

/* Set a value for h-cbit.  */

void
a5f_h_cbit_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_cbit) = newval;
}

/* Get the value of h-vbit.  */

BI
a5f_h_vbit_get (SIM_CPU *current_cpu)
{
  return CPU (h_vbit);
}

/* Set a value for h-vbit.  */

void
a5f_h_vbit_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_vbit) = newval;
}

/* Get the value of h-ubit.  */

BI
a5f_h_ubit_get (SIM_CPU *current_cpu)
{
  return CPU (h_ubit);
}

/* Set a value for h-ubit.  */

void
a5f_h_ubit_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_ubit) = newval;
}

/* Get the value of h-e1.  */

BI
a5f_h_e1_get (SIM_CPU *current_cpu)
{
  return CPU (h_e1);
}

/* Set a value for h-e1.  */

void
a5f_h_e1_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_e1) = newval;
}

/* Get the value of h-e2.  */

BI
a5f_h_e2_get (SIM_CPU *current_cpu)
{
  return CPU (h_e2);
}

/* Set a value for h-e2.  */

void
a5f_h_e2_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_e2) = newval;
}

/* Get the value of h-s1bit.  */

BI
a5f_h_s1bit_get (SIM_CPU *current_cpu)
{
  return CPU (h_s1bit);
}

/* Set a value for h-s1bit.  */

void
a5f_h_s1bit_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_s1bit) = newval;
}

/* Get the value of h-s2bit.  */

BI
a5f_h_s2bit_get (SIM_CPU *current_cpu)
{
  return CPU (h_s2bit);
}

/* Set a value for h-s2bit.  */

void
a5f_h_s2bit_set (SIM_CPU *current_cpu, BI newval)
{
  CPU (h_s2bit) = newval;
}

/* Get the value of h-Qcondb.  */

BI
a5f_h_Qcondb_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_QCONDB (regno);
}

/* Set a value for h-Qcondb.  */

void
a5f_h_Qcondb_set (SIM_CPU *current_cpu, UINT regno, BI newval)
{
  SET_H_QCONDB (regno, newval);
}

/* Get the value of h-Qcondj.  */

BI
a5f_h_Qcondj_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_QCONDJ (regno);
}

/* Set a value for h-Qcondj.  */

void
a5f_h_Qcondj_set (SIM_CPU *current_cpu, UINT regno, BI newval)
{
  SET_H_QCONDJ (regno, newval);
}

/* Get the value of h-Qcondi.  */

BI
a5f_h_Qcondi_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_QCONDI (regno);
}

/* Set a value for h-Qcondi.  */

void
a5f_h_Qcondi_set (SIM_CPU *current_cpu, UINT regno, BI newval)
{
  SET_H_QCONDI (regno, newval);
}

/* Get the value of h-i2cond.  */

BI
a5f_h_i2cond_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_I2COND (regno);
}

/* Set a value for h-i2cond.  */

void
a5f_h_i2cond_set (SIM_CPU *current_cpu, UINT regno, BI newval)
{
  SET_H_I2COND (regno, newval);
}

/* Get the value of h-i3cond.  */

BI
a5f_h_i3cond_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_I3COND (regno);
}

/* Set a value for h-i3cond.  */

void
a5f_h_i3cond_set (SIM_CPU *current_cpu, UINT regno, BI newval)
{
  SET_H_I3COND (regno, newval);
}

/* Get the value of h-cr.  */

SI
a5f_h_cr_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_CR (regno);
}

/* Set a value for h-cr.  */

void
a5f_h_cr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_CR (regno, newval);
}

/* Get the value of h-cr16.  */

SI
a5f_h_cr16_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_CR16 (regno);
}

/* Set a value for h-cr16.  */

void
a5f_h_cr16_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_CR16 (regno, newval);
}

/* Get the value of h-r0.  */

SI
a5f_h_r0_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_R0 (regno);
}

/* Set a value for h-r0.  */

void
a5f_h_r0_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_R0 (regno, newval);
}

/* Get the value of h-gp.  */

SI
a5f_h_gp_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_GP (regno);
}

/* Set a value for h-gp.  */

void
a5f_h_gp_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_GP (regno, newval);
}

/* Get the value of h-sp.  */

SI
a5f_h_sp_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_SP (regno);
}

/* Set a value for h-sp.  */

void
a5f_h_sp_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_SP (regno, newval);
}

/* Get the value of h-pcl.  */

SI
a5f_h_pcl_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_PCL (regno);
}

/* Set a value for h-pcl.  */

void
a5f_h_pcl_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_PCL (regno, newval);
}

/* Get the value of h-noilink.  */

SI
a5f_h_noilink_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_NOILINK (regno);
}

/* Set a value for h-noilink.  */

void
a5f_h_noilink_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_NOILINK (regno, newval);
}

/* Get the value of h-ilinkx.  */

SI
a5f_h_ilinkx_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_ILINKX (regno);
}

/* Set a value for h-ilinkx.  */

void
a5f_h_ilinkx_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_ILINKX (regno, newval);
}

/* Get the value of h-r31.  */

SI
a5f_h_r31_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_R31 (regno);
}

/* Set a value for h-r31.  */

void
a5f_h_r31_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_R31 (regno, newval);
}

/* Get the value of h-auxr.  */

SI
a5f_h_auxr_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_AUXR (regno);
}

/* Set a value for h-auxr.  */

void
a5f_h_auxr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_AUXR (regno, newval);
}

/* Get the value of h-status32.  */

SI
a5f_h_status32_get (SIM_CPU *current_cpu, UINT regno)
{
  return GET_H_STATUS32 (regno);
}

/* Set a value for h-status32.  */

void
a5f_h_status32_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  SET_H_STATUS32 (regno, newval);
}

/* Get the value of h-timer-expire.  */

SI
a5f_h_timer_expire_get (SIM_CPU *current_cpu, UINT regno)
{
  return CPU (h_timer_expire[regno]);
}

/* Set a value for h-timer-expire.  */

void
a5f_h_timer_expire_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  CPU (h_timer_expire[regno]) = newval;
}

/* Get the value of h-prof-offset.  */

SI
a5f_h_prof_offset_get (SIM_CPU *current_cpu, UINT regno)
{
  return CPU (h_prof_offset[regno]);
}

/* Set a value for h-prof-offset.  */

void
a5f_h_prof_offset_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  CPU (h_prof_offset[regno]) = newval;
}

/* Get the value of h-pc.  */

USI
a5f_h_pc_get (SIM_CPU *current_cpu)
{
  return CPU (h_pc);
}

/* Set a value for h-pc.  */

void
a5f_h_pc_set (SIM_CPU *current_cpu, USI newval)
{
  CPU (h_pc) = newval;
}

/* Record trace results for INSN.  */

void
a5f_record_trace_results (SIM_CPU *current_cpu, CGEN_INSN *insn,
			    int *indices, TRACE_RECORD *tr)
{
}
