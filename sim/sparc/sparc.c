/* sparc simulator support code, generic to all sparcs
   Copyright (C) 1999 Cygnus Solutions.  */

#include "sim-main.h"
#include "libiberty.h"
#include "bfd.h"
#include "cgen-mem.h"
#include "cgen-ops.h"

#ifdef HAVE_SPARC64
#define WI DI
#else
#define WI SI
#endif

void
sparc_do_unimp (SIM_CPU *current_cpu, IADDR pc, SI imm22)
{
  sim_engine_invalid_insn (current_cpu, pc);
}

void
do_ldstub ()
{
}

void
do_swap ()
{
}

/* The semantic code invokes this for invalid (unrecognized) instructions.  */

void
sim_engine_invalid_insn (SIM_CPU *cpu, IADDR pc)
{
#ifdef HAVE_SPARC32
  if (ARCH32_P (cpu))
    sparc32_invalid_insn (cpu, pc);
#endif
#ifdef HAVE_SPARC64
  if (ARCH64_P (cpu))
    sparc64_invalid_insn (cpu, pc);
#endif
}

/* Process an address exception.  */

void
sparc_core_signal (SIM_DESC sd, SIM_CPU *cpu, sim_cia pc,
		   unsigned int map, int nr_bytes, address_word addr,
		   transfer_type transfer, sim_core_signals sig)
{
#ifdef HAVE_SPARC32
  if (ARCH32_P (cpu))
    sparc32_core_signal (sd, cpu, pc, map, nr_bytes, addr, transfer, sig);
#endif
#ifdef HAVE_SPARC64
  if (ARCH64_P (cpu))
    sparc64_core_signal (sd, cpu, pc, map, nr_bytes, addr, transfer, sig);
#endif
}
