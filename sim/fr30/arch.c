/* Simulator support for fr30.

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

#include "sim-main.h"
#include "bfd.h"

const MACH *sim_machs[] =
{
#ifdef HAVE_CPU_FR30BF
  & fr30_mach,
#endif
  0
};

/* Get the value of h-pc.  */

USI
a_fr30_h_pc_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_pc_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-pc.  */

void
a_fr30_h_pc_set (SIM_CPU *current_cpu, USI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_pc_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-gr.  */

SI
a_fr30_h_gr_get (SIM_CPU *current_cpu, UINT regno)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_gr_get (current_cpu, regno);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-gr.  */

void
a_fr30_h_gr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_gr_set (current_cpu, regno, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-cr.  */

SI
a_fr30_h_cr_get (SIM_CPU *current_cpu, UINT regno)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_cr_get (current_cpu, regno);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-cr.  */

void
a_fr30_h_cr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_cr_set (current_cpu, regno, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-dr.  */

SI
a_fr30_h_dr_get (SIM_CPU *current_cpu, UINT regno)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_dr_get (current_cpu, regno);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-dr.  */

void
a_fr30_h_dr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_dr_set (current_cpu, regno, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-ps.  */

USI
a_fr30_h_ps_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_ps_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-ps.  */

void
a_fr30_h_ps_set (SIM_CPU *current_cpu, USI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_ps_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-r13.  */

SI
a_fr30_h_r13_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_r13_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-r13.  */

void
a_fr30_h_r13_set (SIM_CPU *current_cpu, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_r13_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-r14.  */

SI
a_fr30_h_r14_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_r14_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-r14.  */

void
a_fr30_h_r14_set (SIM_CPU *current_cpu, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_r14_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-r15.  */

SI
a_fr30_h_r15_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_r15_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-r15.  */

void
a_fr30_h_r15_set (SIM_CPU *current_cpu, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_r15_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-nbit.  */

BI
a_fr30_h_nbit_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_nbit_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-nbit.  */

void
a_fr30_h_nbit_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_nbit_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-zbit.  */

BI
a_fr30_h_zbit_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_zbit_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-zbit.  */

void
a_fr30_h_zbit_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_zbit_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-vbit.  */

BI
a_fr30_h_vbit_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_vbit_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-vbit.  */

void
a_fr30_h_vbit_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_vbit_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-cbit.  */

BI
a_fr30_h_cbit_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_cbit_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-cbit.  */

void
a_fr30_h_cbit_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_cbit_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-ibit.  */

BI
a_fr30_h_ibit_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_ibit_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-ibit.  */

void
a_fr30_h_ibit_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_ibit_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-sbit.  */

BI
a_fr30_h_sbit_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_sbit_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-sbit.  */

void
a_fr30_h_sbit_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_sbit_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-tbit.  */

BI
a_fr30_h_tbit_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_tbit_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-tbit.  */

void
a_fr30_h_tbit_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_tbit_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-d0bit.  */

BI
a_fr30_h_d0bit_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_d0bit_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-d0bit.  */

void
a_fr30_h_d0bit_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_d0bit_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-d1bit.  */

BI
a_fr30_h_d1bit_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_d1bit_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-d1bit.  */

void
a_fr30_h_d1bit_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_d1bit_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-ccr.  */

UQI
a_fr30_h_ccr_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_ccr_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-ccr.  */

void
a_fr30_h_ccr_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_ccr_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-scr.  */

UQI
a_fr30_h_scr_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_scr_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-scr.  */

void
a_fr30_h_scr_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_scr_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-ilm.  */

UQI
a_fr30_h_ilm_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      return fr30bf_h_ilm_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-ilm.  */

void
a_fr30_h_ilm_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_FR30BF
    case bfd_mach_fr30 : 
      fr30bf_h_ilm_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

