/* Main header for the m32r.  */

#define USING_SIM_BASE_H /* FIXME: quick hack */

struct _sim_cpu; /* FIXME: should be in sim-basics.h */
typedef struct _sim_cpu SIM_CPU;

/* sim-basics.h includes config.h but cgen-types.h must be included before
   sim-basics.h and cgen-types.h needs config.h.  */
#include "config.h"

#include "ansidecl.h"
#include "cgen-types.h"
#include "arch.h"
#include "sim-basics.h"

/* These must be defined before sim-base.h.  */
typedef SI sim_cia;
#define CIA_GET(cpu)     0 /* FIXME:(CPU_CGEN_HW (cpu)->h_pc) */
#define CIA_SET(cpu,val) 0 /* FIXME:(CPU_CGEN_HW (cpu)->h_pc = (val)) */

/* FIXME: Shouldn't be required to define these this early.  */
#define SIM_ENGINE_HALT_HOOK(SD, LAST_CPU, CIA)
#define SIM_ENGINE_RESTART_HOOK(SD, LAST_CPU, CIA)

#include "sim-base.h"
#include "cgen-sim.h"
/*#include "cgen-mem.h"*/
#include "cgen-trace.h"
#include "cpu-sim.h"

#ifdef WANT_CPU_M32R
#include "cpu.h"
#include "decode.h"
#endif
/* start-sanitize-m32rx */
#ifdef WANT_CPU_M32RX
#include "cpux.h"
#include "decodex.h"
#endif
/* end-sanitize-m32rx */
#include "cpuall.h"

/* Misc. profile data.  */
typedef struct {
  /* nop insn slot filler count */
  unsigned int fillnop_count;
} M32R_MISC_PROFILE;

struct _sim_cpu {
  sim_cpu_base base;

  /* Static parts of cgen.  */
  CGEN_CPU cgen_cpu;

  /* CPU specific parts go here.
     Note that in files that don't need to access these pieces WANT_CPU_FOO
     won't be defined and thus these parts won't appear.  This is ok.
     One has to of course be careful to not take the size of this
     struct, etc.  */
#if defined (WANT_CPU_M32R)
  M32R_CPU_DATA cpu_data;
/* start-sanitize-m32rx */
#elif defined (WANT_CPU_M32RX)
  M32RX_CPU_DATA cpu_data;
/* end-sanitize-m32rx */
#endif

  M32R_MISC_PROFILE m32r_misc_profile;
#define CPU_M32R_MISC_PROFILE(cpu) ((cpu)->m32r_misc_profile)
};

struct sim_state {
  sim_cpu *cpu;
#define STATE_CPU(sd, n) (/*&*/ (sd)->cpu)

  CGEN_STATE cgen_state;

  sim_state_base base;
};

/* Default memory size.  */
#define M32R_DEFAULT_MEM_SIZE 0x800000 /* 8M */
