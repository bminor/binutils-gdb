/* Simulator support for sparc.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#include "sim-main.h"
#include "bfd.h"

const MACH *sim_machs[] =
{
#ifdef HAVE_CPU_SPARC32
  & sparc_v8_mach,
#endif
#ifdef HAVE_CPU_SPARC32
  & sparclite_mach,
#endif
#ifdef HAVE_CPU_SPARC64
  & sparc_v9_mach,
#endif
  0
};

/* Get the value of h-pc.  */

USI
a_sparc_h_pc_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_pc_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_pc_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_pc_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-pc.  */

void
a_sparc_h_pc_set (SIM_CPU *current_cpu, USI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_pc_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_pc_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_pc_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-npc.  */

SI
a_sparc_h_npc_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_npc_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_npc_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_npc_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-npc.  */

void
a_sparc_h_npc_set (SIM_CPU *current_cpu, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_npc_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_npc_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_npc_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-gr.  */

SI
a_sparc_h_gr_get (SIM_CPU *current_cpu, UINT regno)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_gr_get (current_cpu, regno);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_gr_get (current_cpu, regno);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_gr_get (current_cpu, regno);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-gr.  */

void
a_sparc_h_gr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_gr_set (current_cpu, regno, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_gr_set (current_cpu, regno, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_gr_set (current_cpu, regno, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-icc-c.  */

BI
a_sparc_h_icc_c_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_icc_c_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_icc_c_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_icc_c_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-icc-c.  */

void
a_sparc_h_icc_c_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_icc_c_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_icc_c_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_icc_c_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-icc-n.  */

BI
a_sparc_h_icc_n_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_icc_n_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_icc_n_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_icc_n_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-icc-n.  */

void
a_sparc_h_icc_n_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_icc_n_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_icc_n_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_icc_n_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-icc-v.  */

BI
a_sparc_h_icc_v_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_icc_v_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_icc_v_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_icc_v_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-icc-v.  */

void
a_sparc_h_icc_v_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_icc_v_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_icc_v_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_icc_v_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-icc-z.  */

BI
a_sparc_h_icc_z_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_icc_z_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_icc_z_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_icc_z_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-icc-z.  */

void
a_sparc_h_icc_z_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_icc_z_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_icc_z_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_icc_z_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-xcc-c.  */

BI
a_sparc_h_xcc_c_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_xcc_c_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_xcc_c_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_xcc_c_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-xcc-c.  */

void
a_sparc_h_xcc_c_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_xcc_c_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_xcc_c_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_xcc_c_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-xcc-n.  */

BI
a_sparc_h_xcc_n_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_xcc_n_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_xcc_n_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_xcc_n_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-xcc-n.  */

void
a_sparc_h_xcc_n_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_xcc_n_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_xcc_n_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_xcc_n_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-xcc-v.  */

BI
a_sparc_h_xcc_v_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_xcc_v_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_xcc_v_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_xcc_v_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-xcc-v.  */

void
a_sparc_h_xcc_v_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_xcc_v_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_xcc_v_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_xcc_v_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-xcc-z.  */

BI
a_sparc_h_xcc_z_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_xcc_z_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_xcc_z_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_xcc_z_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-xcc-z.  */

void
a_sparc_h_xcc_z_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_xcc_z_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_xcc_z_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_xcc_z_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-y.  */

SI
a_sparc_h_y_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_y_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_y_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_y_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-y.  */

void
a_sparc_h_y_set (SIM_CPU *current_cpu, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_y_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_y_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_y_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-asr.  */

SI
a_sparc_h_asr_get (SIM_CPU *current_cpu, UINT regno)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_asr_get (current_cpu, regno);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_asr_get (current_cpu, regno);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_asr_get (current_cpu, regno);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-asr.  */

void
a_sparc_h_asr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_asr_set (current_cpu, regno, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_asr_set (current_cpu, regno, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_asr_set (current_cpu, regno, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-annul-p.  */

BI
a_sparc_h_annul_p_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_annul_p_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_annul_p_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_annul_p_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-annul-p.  */

void
a_sparc_h_annul_p_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_annul_p_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_annul_p_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_annul_p_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fr.  */

SF
a_sparc_h_fr_get (SIM_CPU *current_cpu, UINT regno)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_fr_get (current_cpu, regno);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_fr_get (current_cpu, regno);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fr_get (current_cpu, regno);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fr.  */

void
a_sparc_h_fr_set (SIM_CPU *current_cpu, UINT regno, SF newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_fr_set (current_cpu, regno, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_fr_set (current_cpu, regno, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fr_set (current_cpu, regno, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-psr.  */

USI
a_sparc_h_psr_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_psr_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_psr_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_psr_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-psr.  */

void
a_sparc_h_psr_set (SIM_CPU *current_cpu, USI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_psr_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_psr_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_psr_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-s.  */

BI
a_sparc_h_s_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_s_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_s_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_s_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-s.  */

void
a_sparc_h_s_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_s_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_s_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_s_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-ps.  */

BI
a_sparc_h_ps_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_ps_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_ps_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_ps_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-ps.  */

void
a_sparc_h_ps_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_ps_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_ps_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_ps_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-pil.  */

UQI
a_sparc_h_pil_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_pil_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_pil_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_pil_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-pil.  */

void
a_sparc_h_pil_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_pil_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_pil_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_pil_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-et.  */

BI
a_sparc_h_et_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_et_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_et_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_et_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-et.  */

void
a_sparc_h_et_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_et_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_et_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_et_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-tbr.  */

SI
a_sparc_h_tbr_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_tbr_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_tbr_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_tbr_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-tbr.  */

void
a_sparc_h_tbr_set (SIM_CPU *current_cpu, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_tbr_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_tbr_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_tbr_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-cwp.  */

UQI
a_sparc_h_cwp_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_cwp_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_cwp_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_cwp_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-cwp.  */

void
a_sparc_h_cwp_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_cwp_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_cwp_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_cwp_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-wim.  */

USI
a_sparc_h_wim_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_wim_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_wim_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_wim_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-wim.  */

void
a_sparc_h_wim_set (SIM_CPU *current_cpu, USI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_wim_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_wim_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_wim_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-ag.  */

QI
a_sparc_h_ag_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_ag_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_ag_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_ag_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-ag.  */

void
a_sparc_h_ag_set (SIM_CPU *current_cpu, QI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_ag_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_ag_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_ag_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-ec.  */

BI
a_sparc_h_ec_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_ec_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_ec_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_ec_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-ec.  */

void
a_sparc_h_ec_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_ec_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_ec_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_ec_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-ef.  */

BI
a_sparc_h_ef_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_ef_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_ef_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_ef_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-ef.  */

void
a_sparc_h_ef_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_ef_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_ef_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_ef_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fsr.  */

USI
a_sparc_h_fsr_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      return sparc32_h_fsr_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      return sparc32_h_fsr_get (current_cpu);
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fsr_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fsr.  */

void
a_sparc_h_fsr_set (SIM_CPU *current_cpu, USI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc : 
      sparc32_h_fsr_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC32
    case bfd_mach_sparc_sparclite : 
      sparc32_h_fsr_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fsr_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-ver.  */

UDI
a_sparc_h_ver_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_ver_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-ver.  */

void
a_sparc_h_ver_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_ver_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-pstate.  */

UDI
a_sparc_h_pstate_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_pstate_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-pstate.  */

void
a_sparc_h_pstate_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_pstate_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-tba.  */

UDI
a_sparc_h_tba_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_tba_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-tba.  */

void
a_sparc_h_tba_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_tba_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-tt.  */

UDI
a_sparc_h_tt_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_tt_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-tt.  */

void
a_sparc_h_tt_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_tt_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-asi.  */

UQI
a_sparc_h_asi_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_asi_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-asi.  */

void
a_sparc_h_asi_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_asi_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-tl.  */

UQI
a_sparc_h_tl_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_tl_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-tl.  */

void
a_sparc_h_tl_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_tl_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-tpc.  */

UDI
a_sparc_h_tpc_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_tpc_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-tpc.  */

void
a_sparc_h_tpc_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_tpc_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-tnpc.  */

UDI
a_sparc_h_tnpc_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_tnpc_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-tnpc.  */

void
a_sparc_h_tnpc_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_tnpc_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-tstate.  */

UDI
a_sparc_h_tstate_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_tstate_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-tstate.  */

void
a_sparc_h_tstate_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_tstate_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-tick.  */

UDI
a_sparc_h_tick_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_tick_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-tick.  */

void
a_sparc_h_tick_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_tick_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-cansave.  */

UDI
a_sparc_h_cansave_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_cansave_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-cansave.  */

void
a_sparc_h_cansave_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_cansave_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-canrestore.  */

UDI
a_sparc_h_canrestore_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_canrestore_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-canrestore.  */

void
a_sparc_h_canrestore_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_canrestore_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-otherwin.  */

UDI
a_sparc_h_otherwin_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_otherwin_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-otherwin.  */

void
a_sparc_h_otherwin_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_otherwin_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-cleanwin.  */

UDI
a_sparc_h_cleanwin_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_cleanwin_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-cleanwin.  */

void
a_sparc_h_cleanwin_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_cleanwin_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-wstate.  */

UDI
a_sparc_h_wstate_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_wstate_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-wstate.  */

void
a_sparc_h_wstate_set (SIM_CPU *current_cpu, UDI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_wstate_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fcc0.  */

UQI
a_sparc_h_fcc0_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fcc0_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fcc0.  */

void
a_sparc_h_fcc0_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fcc0_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fcc1.  */

UQI
a_sparc_h_fcc1_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fcc1_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fcc1.  */

void
a_sparc_h_fcc1_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fcc1_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fcc2.  */

UQI
a_sparc_h_fcc2_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fcc2_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fcc2.  */

void
a_sparc_h_fcc2_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fcc2_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fcc3.  */

UQI
a_sparc_h_fcc3_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fcc3_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fcc3.  */

void
a_sparc_h_fcc3_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fcc3_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fsr-rd.  */

UQI
a_sparc_h_fsr_rd_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fsr_rd_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fsr-rd.  */

void
a_sparc_h_fsr_rd_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fsr_rd_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fsr-tem.  */

UQI
a_sparc_h_fsr_tem_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fsr_tem_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fsr-tem.  */

void
a_sparc_h_fsr_tem_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fsr_tem_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fsr-ns.  */

BI
a_sparc_h_fsr_ns_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fsr_ns_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fsr-ns.  */

void
a_sparc_h_fsr_ns_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fsr_ns_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fsr-ver.  */

UQI
a_sparc_h_fsr_ver_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fsr_ver_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fsr-ver.  */

void
a_sparc_h_fsr_ver_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fsr_ver_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fsr-ftt.  */

UQI
a_sparc_h_fsr_ftt_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fsr_ftt_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fsr-ftt.  */

void
a_sparc_h_fsr_ftt_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fsr_ftt_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fsr-qne.  */

BI
a_sparc_h_fsr_qne_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fsr_qne_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fsr-qne.  */

void
a_sparc_h_fsr_qne_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fsr_qne_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fsr-aexc.  */

UQI
a_sparc_h_fsr_aexc_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fsr_aexc_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fsr-aexc.  */

void
a_sparc_h_fsr_aexc_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fsr_aexc_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fsr-cexc.  */

UQI
a_sparc_h_fsr_cexc_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fsr_cexc_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fsr-cexc.  */

void
a_sparc_h_fsr_cexc_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fsr_cexc_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fpsr-fef.  */

BI
a_sparc_h_fpsr_fef_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fpsr_fef_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fpsr-fef.  */

void
a_sparc_h_fpsr_fef_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fpsr_fef_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fpsr-du.  */

BI
a_sparc_h_fpsr_du_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fpsr_du_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fpsr-du.  */

void
a_sparc_h_fpsr_du_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fpsr_du_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fpsr-dl.  */

BI
a_sparc_h_fpsr_dl_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fpsr_dl_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fpsr-dl.  */

void
a_sparc_h_fpsr_dl_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fpsr_dl_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-fpsr.  */

UQI
a_sparc_h_fpsr_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      return sparc64_h_fpsr_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-fpsr.  */

void
a_sparc_h_fpsr_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_SPARC64
    case bfd_mach_sparc_v9 : 
      sparc64_h_fpsr_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

