/* Simulator support for m32r.

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
#ifdef HAVE_CPU_M32RBF
  & m32r_mach,
#endif
  0
};

/* Get the value of h-pc.  */

USI
a_m32r_h_pc_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      return m32rbf_h_pc_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-pc.  */

void
a_m32r_h_pc_set (SIM_CPU *current_cpu, USI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      m32rbf_h_pc_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-gr.  */

SI
a_m32r_h_gr_get (SIM_CPU *current_cpu, UINT regno)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      return m32rbf_h_gr_get (current_cpu, regno);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-gr.  */

void
a_m32r_h_gr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      m32rbf_h_gr_set (current_cpu, regno, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-cr.  */

USI
a_m32r_h_cr_get (SIM_CPU *current_cpu, UINT regno)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      return m32rbf_h_cr_get (current_cpu, regno);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-cr.  */

void
a_m32r_h_cr_set (SIM_CPU *current_cpu, UINT regno, USI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      m32rbf_h_cr_set (current_cpu, regno, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-accum.  */

DI
a_m32r_h_accum_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      return m32rbf_h_accum_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-accum.  */

void
a_m32r_h_accum_set (SIM_CPU *current_cpu, DI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      m32rbf_h_accum_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-accums.  */

DI
a_m32r_h_accums_get (SIM_CPU *current_cpu, UINT regno)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
    default :
      abort ();
    }
}

/* Set a value for h-accums.  */

void
a_m32r_h_accums_set (SIM_CPU *current_cpu, UINT regno, DI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
    default :
      abort ();
    }
}

/* Get the value of h-cond.  */

BI
a_m32r_h_cond_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      return m32rbf_h_cond_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-cond.  */

void
a_m32r_h_cond_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      m32rbf_h_cond_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-psw.  */

UQI
a_m32r_h_psw_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      return m32rbf_h_psw_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-psw.  */

void
a_m32r_h_psw_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      m32rbf_h_psw_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-bpsw.  */

UQI
a_m32r_h_bpsw_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      return m32rbf_h_bpsw_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-bpsw.  */

void
a_m32r_h_bpsw_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      m32rbf_h_bpsw_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-bbpsw.  */

UQI
a_m32r_h_bbpsw_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      return m32rbf_h_bbpsw_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-bbpsw.  */

void
a_m32r_h_bbpsw_set (SIM_CPU *current_cpu, UQI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      m32rbf_h_bbpsw_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-lock.  */

BI
a_m32r_h_lock_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      return m32rbf_h_lock_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-lock.  */

void
a_m32r_h_lock_set (SIM_CPU *current_cpu, BI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_M32RBF
    case bfd_mach_m32r : 
      m32rbf_h_lock_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

