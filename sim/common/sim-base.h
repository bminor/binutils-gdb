/* Simulator pseudo baseclass.
   Copyright (C) 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of GDB, the GNU debugger.

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
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


/* Simulator state pseudo baseclass.

   Each simulator is required to have the file ``sim-main.h''.  That
   file includes ``sim-basics.h'', defines the base type ``sim_cia''
   (the data type that contains complete current instruction address
   information), include ``sim-base.h'':

     #include "sim-basics.h"
     typedef address_word sim_cia;
     /-* If `sim_cia' is not an integral value (e.g. a struct), define
         CIA_ADDR to return the integral value.  *-/
     /-* #define CIA_ADDR(cia) (...) *-/
     #include "sim-base.h"
   
   finally, two data types `struct _sim_cpu' and `struct sim_state'
   are defined:

     struct _sim_cpu {
        ... simulator specific members ...
        sim_cpu_base base;
     };

     struct sim_state {
       sim_cpu cpu[MAX_NR_PROCESSORS];
     #if (WITH_SMP)
     #define STATE_CPU(sd,n) (&(sd)->cpu[n])
     #else
     #define STATE_CPU(sd,n) (&(sd)->cpu[0])
     #endif
       ... simulator specific members ...
       sim_state_base base;
     };

   Note that `base' appears last.  This makes `base.magic' appear last
   in the entire struct and helps catch miscompilation errors. */


#ifndef SIM_BASE_H
#define SIM_BASE_H

/* Pre-declare certain types. */

/* typedef <target-dependant> sim_cia; */
#ifndef NULL_CIA
#define NULL_CIA ((sim_cia) 0)
#endif
/* Return the current instruction address as a number.
   Some targets treat the current instruction address as a struct
   (e.g. for delay slot handling).  */
#ifndef CIA_ADDR
#define CIA_ADDR(cia) (cia)
#endif
#ifndef INVALID_INSTRUCTION_ADDRESS
#define INVALID_INSTRUCTION_ADDRESS ((address_word)0 - 1)
#endif

typedef struct _sim_cpu sim_cpu;

#include "sim-module.h"

#include "sim-trace.h"
#include "sim-core.h"
#include "sim-events.h"
#include "sim-profile.h"
#ifdef SIM_HAVE_MODEL
#include "sim-model.h"
#endif
#include "sim-io.h"
#include "sim-engine.h"
#include "sim-watch.h"
#include "sim-memopt.h"
#ifdef SIM_HAVE_BREAKPOINTS
#include "sim-break.h"
#endif

/* Global pointer to current state while sim_resume is running.
   On a machine with lots of registers, it might be possible to reserve
   one of them for current_state.  However on a machine with few registers
   current_state can't permanently live in one and indirecting through it
   will be slower [in which case one can have sim_resume set globals from
   current_state for faster access].
   If CURRENT_STATE_REG is defined, it means current_state is living in
   a global register.  */


#ifdef CURRENT_STATE_REG
/* FIXME: wip */
#else
extern struct sim_state *current_state;
#endif


/* The simulator may provide different (and faster) definition.  */
#ifndef CURRENT_STATE
#define CURRENT_STATE current_state
#endif


typedef struct {

  /* Simulator's argv[0].  */
  const char *my_name;
#define STATE_MY_NAME(sd) ((sd)->base.my_name)

  /* Who opened the simulator.  */
  SIM_OPEN_KIND open_kind;
#define STATE_OPEN_KIND(sd) ((sd)->base.open_kind)

  /* The host callbacks.  */
  struct host_callback_struct *callback;
#define STATE_CALLBACK(sd) ((sd)->base.callback)

#if 0 /* FIXME: Not ready yet.  */
  /* Stuff defined in sim-config.h.  */
  struct sim_config config;
#define STATE_CONFIG(sd) ((sd)->base.config)
#endif

  /* List of installed module `init' handlers.  */
  struct module_list *modules;
#define STATE_MODULES(sd) ((sd)->base.modules)

  /* Supported options.  */
  struct option_list *options;
#define STATE_OPTIONS(sd) ((sd)->base.options)

  /* Non-zero if -v specified.  */
  int verbose_p;
#define STATE_VERBOSE_P(sd) ((sd)->base.verbose_p)

  /* Non cpu-specific trace data.  See sim-trace.h.  */
  TRACE_DATA trace_data;
#define STATE_TRACE_DATA(sd) (& (sd)->base.trace_data)

  /* If non NULL, the BFD architecture specified on the command line */
  const struct bfd_arch_info *architecture;
#define STATE_ARCHITECTURE(sd) ((sd)->base.architecture)

  /* If non NULL, the bfd target specified on the command line */
  const char *target;
#define STATE_TARGET(sd) ((sd)->base.target)

  /* In standalone simulator, this is the program's arguments passed
     on the command line.  */
  char **prog_argv;
#define STATE_PROG_ARGV(sd) ((sd)->base.prog_argv)

  /* The program's bfd.  */
  struct _bfd *prog_bfd;
#define STATE_PROG_BFD(sd) ((sd)->base.prog_bfd)

  /* The program's text section.  */
  struct sec *text_section;
  /* Starting and ending text section addresses from the bfd.  */
  SIM_ADDR text_start, text_end;
#define STATE_TEXT_SECTION(sd) ((sd)->base.text_section)
#define STATE_TEXT_START(sd) ((sd)->base.text_start)
#define STATE_TEXT_END(sd) ((sd)->base.text_end)

  /* Start address, set when the program is loaded from the bfd.  */
  SIM_ADDR start_addr;
#define STATE_START_ADDR(sd) ((sd)->base.start_addr)

  /* Size of the simulator's cache, if any.
     This is not the target's cache.  It is the cache the simulator uses
     to process instructions.  */
  unsigned int scache_size;
#define STATE_SCACHE_SIZE(sd) ((sd)->base.scache_size)

  /* FIXME: Move to top level sim_state struct (as some struct)?  */
#ifdef SIM_HAVE_FLATMEM
  unsigned int mem_size;
#define STATE_MEM_SIZE(sd) ((sd)->base.mem_size)
  unsigned int mem_base;
#define STATE_MEM_BASE(sd) ((sd)->base.mem_base)
  unsigned char *memory;
#define STATE_MEMORY(sd) ((sd)->base.memory)
#endif

  /* core memory bus */
#define STATE_CORE(sd) (&(sd)->base.core)
  sim_core core;

  /* Record of memory sections added via the memory-options interface.  */
#define STATE_MEMOPT(sd) ((sd)->base.memopt)
  sim_memopt *memopt;

  /* event handler */
#define STATE_EVENTS(sd) (&(sd)->base.events)
  sim_events events;

  /* generic halt/resume engine */
  sim_engine engine;
#define STATE_ENGINE(sd) (&(sd)->base.engine)

  /* generic watchpoint support */
  sim_watchpoints watchpoints;
#define STATE_WATCHPOINTS(sd) (&(sd)->base.watchpoints)

  /* Pointer to list of breakpoints */
  struct sim_breakpoint *breakpoints;
#define STATE_BREAKPOINTS(sd) ((sd)->base.breakpoints)

  /* Marker for those wanting to do sanity checks.
     This should remain the last member of this struct to help catch
     miscompilation errors.  */
  int magic;
#define SIM_MAGIC_NUMBER 0x4242
#define STATE_MAGIC(sd) ((sd)->base.magic)
} sim_state_base;


/* Pseudo baseclass for each cpu.  */

typedef struct {

  /* Backlink to main state struct.  */
  SIM_DESC state;
#define CPU_STATE(cpu) ((cpu)->base.state)

  /* The name of the cpu.  */
  const char *name;
#define CPU_NAME(cpu) ((cpu)->base.name)

  /* Options specific to this cpu.  */
  struct option_list *options;
#define CPU_OPTIONS(cpu) ((cpu)->base.options)

  /* Processor specific core data */
  sim_cpu_core core;
#define CPU_CORE(cpu) (& (cpu)->base.core)

  /* Trace data.  See sim-trace.h.  */
  TRACE_DATA trace_data;
#define CPU_TRACE_DATA(cpu) (& (cpu)->base.trace_data)

  /* Maximum number of debuggable entities.
     This debugging is not intended for normal use.
     It is only enabled when the simulator is configured with --with-debug
     which shouldn't normally be specified.  */
#ifndef MAX_DEBUG_VALUES
#define MAX_DEBUG_VALUES 4
#endif

  /* Boolean array of specified debugging flags.  */
  char debug_flags[MAX_DEBUG_VALUES];
#define CPU_DEBUG_FLAGS(cpu) ((cpu)->base.debug_flags)
  /* Standard values.  */
#define DEBUG_INSN_IDX 0
#define DEBUG_NEXT_IDX 2 /* simulator specific debug bits begin here */

  /* Debugging output goes to this or stderr if NULL.
     We can't store `stderr' here as stderr goes through a callback.  */
  FILE *debug_file;
#define CPU_DEBUG_FILE(cpu) ((cpu)->base.debug_file)

  /* Profile data.  See sim-profile.h.  */
  PROFILE_DATA profile_data;
#define CPU_PROFILE_DATA(cpu) (& (cpu)->base.profile_data)

#ifdef SIM_HAVE_MODEL
  /* Machine tables for this cpu.  See sim-model.h.  */
  const MACH *mach;
#define CPU_MACH(cpu) ((cpu)->base.mach)
  /* The selected model.  */
  const MODEL *model;
#define CPU_MODEL(cpu) ((cpu)->base.model)
#endif

} sim_cpu_base;


/* Functions for allocating/freeing a sim_state.  */
SIM_DESC sim_state_alloc PARAMS ((SIM_OPEN_KIND kind, host_callback *callback));
void sim_state_free PARAMS ((SIM_DESC));

/* Return a pointer to the cpu data for CPU_NAME, or NULL if not found.  */
sim_cpu *sim_cpu_lookup (SIM_DESC sd, const char *cpu_name);


#endif /* SIM_BASE_H */
