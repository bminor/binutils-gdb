/* Main header for sparc.  */

#ifndef SIM_MAIN_H
#define SIM_MAIN_H

#define USING_SIM_BASE_H /* FIXME: quick hack */

/* FIXME: For now always provide sparc32 support.  */
#define HAVE_SPARC32
#undef HAVE_SPARC64 /* FIXME:wip */

/* ??? May eventually want this model (and perhaps runtime) selectable.  */
#define NWINDOWS 8

struct _sim_cpu; /* FIXME: should be in sim-basics.h */
typedef struct _sim_cpu SIM_CPU;

#include "symcat.h"
#include "sim-basics.h"
#include "cgen-types.h"
#include "cpu-opc.h"
#include "arch.h"

/* These must be defined before sim-base.h.  */
typedef USI sim_cia;
typedef int SEMRES;

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

#ifdef WANT_CPU
#include "cpu-sim.h"
#endif
#if defined (WANT_CPU_SPARC32)
#include "regs32.h"
#include "trap32.h"
#elif defined (WANT_CPU_SPARC64)
#include "regs64.h"
#include "trap64.h"
#endif

/* The sim_cpu struct.  */

struct _sim_cpu {
  /* sim/common cpu base.  */
  sim_cpu_base base;

  /* Static parts of cgen.  */
  CGEN_CPU cgen_cpu;

  /* CPU specific parts go here.
     Note that in files that don't need to access these pieces WANT_CPU_FOO
     won't be defined and thus these parts won't appear.  This is ok in the
     sense that things work.  It is a source of bugs though.
     One has to of course be careful to not take the size of this
     struct and no structure members accessed in non-cpu specific files can
     go after here.  Oh for a better language.  */

#if defined (WANT_CPU_SPARC32)

  /* Machine generated registers.  */
  SPARC32_CPU_DATA cpu_data;
#if 0
  /* Working variables for generating profiling information.  */
  SPARC32_CPU_PROFILE cpu_profile;
#endif
  /* Actual values of the window regs.  */
  SI win_regs[NWINDOWS][16];
  /* g0-g7, normal and alternate
     ??? handling the alternate regs still wip, need REAL_GREGS to dtrt.  */
  SI global_regs[2][8];
  /* Working copies of integer regs, swapped in/out on each window change.
     ??? Doing things this way means the .cpu file needn't mark these as
     virtual.  */
  SI current_regs[32];

#elif defined (WANT_CPU_SPARC64)

  /* Machine generated registers.  */
  SPARC64_CPU_DATA cpu_data;
#if 0
  /* Working variables for generating profiling information.  */
  SPARC64_CPU_PROFILE cpu_profile;
#endif
  /* Actual values of the window regs.  */
  DI win_regs[NWINDOWS][16];
  /* g0-g7, normal and alternate */
  DI global_regs[2][8];
  /* Working copies of integer regs, swapped in/out on each window change.
     ??? Doing things this way means .cpu file needn't mark these as
     virtual.  */
  DI current_regs[32];

#endif
};

#define ARCH32_P(cpu) 1 /*FIXME*/
#define ARCH64_P(cpu) (! ARCH32_P (cpu))

/* The sim_state struct.  */

struct sim_state {
  sim_cpu *cpu;
#define STATE_CPU(sd, n) (/*&*/ (sd)->cpu)

  CGEN_STATE cgen_state;

  sim_state_base base;
};

/* Misc.  */

/* Catch address exceptions.  */
extern SIM_CORE_SIGNAL_FN sparc_core_signal;
#define SIM_CORE_SIGNAL(SD,CPU,CIA,MAP,NR_BYTES,ADDR,TRANSFER,ERROR) \
sparc_core_signal ((SD), (CPU), (CIA), (MAP), (NR_BYTES), (ADDR), \
		   (TRANSFER), (ERROR))

/* Default memory size.  */
#define SPARC_DEFAULT_MEM_SIZE 0x800000 /* 8M */

#endif /* SIM_MAIN_H */
