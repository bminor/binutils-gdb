/* Simulator support for i960.

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
#ifdef HAVE_CPU_I960BASE
  & i960_ka_sa_mach,
#endif
#ifdef HAVE_CPU_I960BASE
  & i960_ca_mach,
#endif
  0
};

/* Get the value of h-pc.  */

USI
a_i960_h_pc_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ka_sa : 
      return i960base_h_pc_get (current_cpu);
#endif
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ca : 
      return i960base_h_pc_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-pc.  */

void
a_i960_h_pc_set (SIM_CPU *current_cpu, USI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ka_sa : 
      i960base_h_pc_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ca : 
      i960base_h_pc_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-gr.  */

SI
a_i960_h_gr_get (SIM_CPU *current_cpu, UINT regno)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ka_sa : 
      return i960base_h_gr_get (current_cpu, regno);
#endif
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ca : 
      return i960base_h_gr_get (current_cpu, regno);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-gr.  */

void
a_i960_h_gr_set (SIM_CPU *current_cpu, UINT regno, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ka_sa : 
      i960base_h_gr_set (current_cpu, regno, newval);
      break;
#endif
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ca : 
      i960base_h_gr_set (current_cpu, regno, newval);
      break;
#endif
    default :
      abort ();
    }
}

/* Get the value of h-cc.  */

SI
a_i960_h_cc_get (SIM_CPU *current_cpu)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ka_sa : 
      return i960base_h_cc_get (current_cpu);
#endif
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ca : 
      return i960base_h_cc_get (current_cpu);
#endif
    default :
      abort ();
    }
}

/* Set a value for h-cc.  */

void
a_i960_h_cc_set (SIM_CPU *current_cpu, SI newval)
{
  switch (STATE_ARCHITECTURE (CPU_STATE (current_cpu))->mach)
    {
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ka_sa : 
      i960base_h_cc_set (current_cpu, newval);
      break;
#endif
#ifdef HAVE_CPU_I960BASE
    case bfd_mach_i960_ca : 
      i960base_h_cc_set (current_cpu, newval);
      break;
#endif
    default :
      abort ();
    }
}

