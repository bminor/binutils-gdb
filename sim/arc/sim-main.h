/* Main header for the arc.
   Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.  */

#ifndef SIM_MAIN_H
#define SIM_MAIN_H

struct _sim_cpu; /* FIXME: should be in sim-basics.h */
typedef struct _sim_cpu SIM_CPU;

#include "symcat.h"
#include "sim-basics.h"
#include "cgen-types.h"
#include "arc-desc.h"
#include "arc-opc.h"
#include "arch.h"

/* These must be defined before sim-base.h.  */
typedef USI sim_cia;

#define CIA_GET(cpu)     CPU_PC_GET (cpu)
#define CIA_SET(cpu,val) CPU_PC_SET ((cpu), (val))

#define SIM_ENGINE_HALT_HOOK(sd, cpu, cia) \
do { \
  if (cpu) /* null if ctrl-c */ \
    sim_pc_set ((cpu), (cia)); \
} while (0)
#define SIM_ENGINE_RESTART_HOOK(sd, cpu, cia) \
do { \
  sim_pc_set ((cpu), (cia)); \
} while (0)

#include "sim-base.h"
#include "cgen-sim.h"
#include "arc-sim.h"
#include "opcode/cgen.h"

/* The _sim_cpu struct.  */

struct _sim_cpu {
  /* sim/common cpu base.  */
  sim_cpu_base base;

  /* Static parts of cgen.  */
  CGEN_CPU cgen_cpu;

  ARC_MISC_PROFILE arc_misc_profile;
#define CPU_ARC_MISC_PROFILE(cpu) (& (cpu)->arc_misc_profile)

  /* Simulator environment data.  */
  USI endbrk;

  /* CPU specific parts go here.
     Note that in files that don't need to access these pieces WANT_CPU_FOO
     won't be defined and thus these parts won't appear.  This is ok in the
     sense that things work.  It is a source of bugs though.
     One has to of course be careful to not take the size of this
     struct and no structure members accessed in non-cpu specific files can
     go after here.  Oh for a better language.  */
  /* ??? all the CPU registers are considered to be CPU specific, even the
     ones that are present in all members of all CPU families of the
     architecture.  */
#if defined (WANT_CPU_A5F)
  A5F_CPU_DATA cpu_data;
#endif
#if defined (WANT_CPU_ARC600F)
  ARC600F_CPU_DATA cpu_data;
#elif defined (WANT_CPU_ARC700F)
  ARC700F_CPU_DATA cpu_data;
#endif
};

/* The sim_state struct.  */


struct sim_memory {
  USI heap_start;
  USI heap_end;
  USI stack_start;
  USI stack_top;
  USI total_size;
  USI argument_data_start;
  USI num_arguments;
};

struct sim_state {
  sim_cpu *cpu;
#define STATE_CPU(sd, n) (/*&*/ (sd)->cpu)

  CGEN_STATE cgen_state;

  struct sim_memory memory;
  int memory_regions_defined_by_user;

  sim_state_base base;
};

/* Misc.  */

/* Catch address exceptions.  */
extern SIM_CORE_SIGNAL_FN arc_core_signal;
#define SIM_CORE_SIGNAL(SD,CPU,CIA,MAP,NR_BYTES,ADDR,TRANSFER,ERROR) \
arc_core_signal ((SD), (CPU), (CIA), (MAP), (NR_BYTES), (ADDR), \
		  (TRANSFER), (ERROR))

/* Default memory size.  */
#define ARC_DEFAULT_MEM_SIZE 0x2000000 /* 32M */

#endif /* SIM_MAIN_H */
