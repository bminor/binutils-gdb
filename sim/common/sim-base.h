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

/* This file is meant to be included by sim-basics.h.  */

#ifndef SIM_BASE_H
#define SIM_BASE_H

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

/* Simulator state pseudo baseclass.
   Each simulator is required to have a sim-main.h file that includes
   sim-basics.h and defines struct sim_state to be:

   struct sim_state {
     sim_cpu cpu;
   #define STATE_CPU(sd,n) (&(sd)->cpu)
     ... simulator specific members ...
     sim_state_base base;
   };

   for a single processor or

   struct sim_state {
     sim_cpu cpu[MAX_NR_PROCESSORS]; -- could be also be array of pointers
   #define STATE_CPU(sd,n) (&(sd)->cpu[n])
     ... simulator specific members ...
     sim_state_base base;
   };

   for multiprocessors.
   Note that `base' appears last.  This makes `base.magic' appear last
   in the entire struct and helps catch miscompilation errors.

   sim_cpu is defined to be:

   typedef struct _sim_cpu {
      ... simulator specific members ...
      sim_cpu_base base;
   } sim_cpu;
   */

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
  MODULE_INIT_LIST *init_list;
#define STATE_INIT_LIST(sd) ((sd)->base.init_list)
  /* List of installed module `uninstall' handlers.  */
  MODULE_UNINSTALL_LIST *uninstall_list;
#define STATE_UNINSTALL_LIST(sd) ((sd)->base.uninstall_list)

  /* ??? This might be more appropriate in sim_cpu.  */
  /* Machine tables for this cpu.  See sim-model.h.  */
  const MODEL *model;
#define STATE_MODEL(sd) ((sd)->base.model)

  /* Supported options.  */
  struct option_list *options;
#define STATE_OPTIONS(sd) ((sd)->base.options)

  /* Non-zero if -v specified.  */
  int verbose_p;
#define STATE_VERBOSE_P(sd) ((sd)->base.verbose_p)

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

#if WITH_SCACHE
  /* Size of the simulator's cache, if any.
     This is not the target's cache.  It is the cache the simulator uses
     to process instructions.  */
  unsigned int scache_size;
#define STATE_SCACHE_SIZE(sd) ((sd)->base.scache_size)
#endif

  /* FIXME: Move to top level sim_state struct (as some struct)?  */
#ifdef SIM_HAVE_FLATMEM
  unsigned int mem_size;
#define STATE_MEM_SIZE(sd) ((sd)->base.mem_size)
  unsigned char *memory;
#define STATE_MEMORY(sd) ((sd)->base.memory)
#endif

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
} sim_cpu_base;

/* Functions for allocating/freeing a sim_state.  */
SIM_DESC sim_state_alloc PARAMS ((void));
void sim_state_free PARAMS ((SIM_DESC));

#endif /* SIM_BASE_H */
