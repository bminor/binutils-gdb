/* Profile header for simulators using common framework.
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.
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

#ifndef SIM_PROFILE_H
#define SIM_PROFILE_H

#ifndef WITH_PROFILE
Error, WITH_PROFILE not defined.
#endif

/* Maximum number of profilable entities.  */
#ifndef MAX_PROFILE_VALUES
#define MAX_PROFILE_VALUES 8
#endif

/* Standard profilable entities.  */
#define PROFILE_INSN_IDX 0
#define PROFILE_MEMORY_IDX 1
#define PROFILE_MODEL_IDX 2
#define PROFILE_SCACHE_IDX 3
#define PROFILE_PC_IDX 4
#define PROFILE_CORE_IDX 5
#define PROFILE_NEXT_IDX 6 /* simulator specific profile bits begin here */

/* Masks so WITH_PROFILE can have symbolic values.  */
#define PROFILE_insn 1
#define PROFILE_memory 2
#define PROFILE_model 4
#define PROFILE_scache 8
#define PROFILE_pc 16
#define PROFILE_core 32

/* Preprocessor macros to simplify tests of WITH_PROFILE.  */
#define WITH_PROFILE_INSN_P (WITH_PROFILE & PROFILE_insn)
#define WITH_PROFILE_MEMORY_P (WITH_PROFILE & PROFILE_memory)
#define WITH_PROFILE_MODEL_P (WITH_PROFILE & PROFILE_model)
#define WITH_PROFILE_SCACHE_P (WITH_PROFILE & PROFILE_scache)
#define WITH_PROFILE_PC_P (WITH_PROFILE & PROFILE_pc)
#define WITH_PROFILE_CORE_P (WITH_PROFILE & PROFILE_core)

/* If MAX_INSNS isn't defined, we can't do instruction profiling.
   ??? It is intended that this is a temporary occurence.  Normally
   MAX_INSNS is defined.  */
#ifndef MAX_INSNS
#undef WITH_PROFILE_INSN_P
#define WITH_PROFILE_INSN_P 0
#endif

/* If MAX_MODES isn't defined, we can't do memory profiling.
   ??? It is intended that this is a temporary occurence.  Normally
   MAX_MODES is defined.  */
#ifndef MAX_MODES
#undef WITH_PROFILE_MEMORY_P
#define WITH_PROFILE_MEMORY_P 0
#endif

/* Only build MODEL code when the target simulator has support for it */
#ifndef SIM_HAVE_MODEL
#undef WITH_PROFILE_MODEL_P
#define WITH_PROFILE_MODEL_P 0
#endif

/* Profiling install handler.  */
MODULE_INSTALL_FN profile_install;

/* Output format macros.  */
#ifndef PROFILE_HISTOGRAM_WIDTH
#define PROFILE_HISTOGRAM_WIDTH 40
#endif
#ifndef PROFILE_LABEL_WIDTH
#define PROFILE_LABEL_WIDTH 32
#endif

/* Callbacks for internal profile_info.
   The callbacks may be NULL meaning there isn't one.
   Note that results are indented two spaces to distinguish them from
   section titles.
   If non-NULL, PROFILE_CALLBACK is called to print extra non-cpu related data.
   If non-NULL, PROFILE_CPU_CALLBACK is called to print extra cpu related data.
   */

typedef void (PROFILE_INFO_CALLBACK_FN) (SIM_DESC, int);
struct _sim_cpu; /* forward reference */
typedef void (PROFILE_INFO_CPU_CALLBACK_FN) (struct _sim_cpu *cpu, int verbose);


/* Struct containing most profiling data.
   It doesn't contain all profiling data because for example scache data
   is kept with the rest of scache support.  */

typedef struct {
  /* Boolean array of specified profiling flags.  */
  char profile_flags[MAX_PROFILE_VALUES];
#define PROFILE_FLAGS(p) ((p)->profile_flags)

  /* The total insn count is tracked separately.
     It is always computed, regardless of insn profiling.  */
  unsigned long total_insn_count;
#define PROFILE_TOTAL_INSN_COUNT(p) ((p)->total_insn_count)

  /* Execution time in milliseconds.  */
  unsigned long exec_time;
#define PROFILE_EXEC_TIME(p) ((p)->exec_time)

#if WITH_PROFILE_INSN_P
  unsigned int insn_count[MAX_INSNS];
#define PROFILE_INSN_COUNT(p) ((p)->insn_count)
#endif

#if WITH_PROFILE_MEMORY_P
  unsigned int read_count[MAX_MODES];
#define PROFILE_READ_COUNT(p) ((p)->read_count)
  unsigned int write_count[MAX_MODES];
#define PROFILE_WRITE_COUNT(p) ((p)->write_count)
#endif

#if WITH_PROFILE_CORE_P
  /* Count read/write/exec accesses separatly. */
  unsigned int core_count[nr_sim_core_maps];
#define PROFILE_CORE_COUNT(p) ((p)->core_count)
#endif

#if WITH_PROFILE_MODEL_P
  /* Total cycle count (less stalls).  */
  unsigned long cycle_count;
#define PROFILE_MODEL_CYCLE_COUNT(p) ((p)->cycle_count)
  /* Stalls due to branches.  */
  unsigned long cti_stall_count;
#define PROFILE_MODEL_CTI_STALL_COUNT(p) ((p)->cti_stall_count)
  unsigned long load_stall_count;
#define PROFILE_MODEL_LOAD_STALL_COUNT(p) ((p)->load_stall_count)
  /* Taken and not-taken branches (and other cti's).  */
#define PROFILE_TOTAL_CYCLE_COUNT(p) \
(PROFILE_MODEL_CYCLE_COUNT(p) \
 + PROFILE_MODEL_CTI_STALL_COUNT(p) \
 + PROFILE_MODEL_LOAD_STALL_COUNT(p))

  unsigned long taken_count, untaken_count;
#define PROFILE_MODEL_TAKEN_COUNT(p) ((p)->taken_count)
#define PROFILE_MODEL_UNTAKEN_COUNT(p) ((p)->untaken_count)
#endif

#if WITH_PROFILE_PC_P
  /* PC profiling attempts to determine function usage by sampling the PC
     every so many instructions.  */
  unsigned int profile_pc_freq;
#define PROFILE_PC_FREQ(p) ((p)->profile_pc_freq)
  unsigned int profile_pc_nr_buckets;
#define PROFILE_PC_NR_BUCKETS(p) ((p)->profile_pc_nr_buckets)
  address_word profile_pc_start;
#define PROFILE_PC_START(p) ((p)->profile_pc_start)
  address_word profile_pc_end;
#define PROFILE_PC_END(p) ((p)->profile_pc_end)
  unsigned profile_pc_shift;
#define PROFILE_PC_SHIFT(p) ((p)->profile_pc_shift)
#define PROFILE_PC_BUCKET_SIZE(p) (PROFILE_PC_SHIFT (p) ? (1 << PROFILE_PC_SHIFT (p)) : 0)
  unsigned *profile_pc_count;
#define PROFILE_PC_COUNT(p) ((p)->profile_pc_count)
  sim_event *profile_pc_event;
#define PROFILE_PC_EVENT(p) ((p)->profile_pc_event)
#endif

  /* Profile output goes to this or stderr if NULL.
     We can't store `stderr' here as stderr goes through a callback.  */
  FILE *profile_file;
#define PROFILE_FILE(p) ((p)->profile_file)

  /* When reporting a profile summary, hook to include per-processor
     target specific profile information */
  PROFILE_INFO_CPU_CALLBACK_FN *info_cpu_callback;
#define PROFILE_INFO_CPU_CALLBACK(p) ((p)->info_cpu_callback)

  /* When reporting a profile summary, hook to include common target
     specific profile information */
  PROFILE_INFO_CALLBACK_FN *info_callback;
#define STATE_PROFILE_INFO_CALLBACK(sd) \
(CPU_PROFILE_DATA (STATE_CPU (sd, 0))->info_callback)

} PROFILE_DATA;

/* Usage macros.  */

#define CPU_PROFILE_FLAGS(cpu) PROFILE_FLAGS (CPU_PROFILE_DATA (cpu))

#if WITH_PROFILE_INSN_P
#define PROFILE_COUNT_INSN(cpu, pc, insn_num) \
do { \
  if (CPU_PROFILE_FLAGS (cpu) [PROFILE_INSN_IDX]) \
    ++ PROFILE_INSN_COUNT (CPU_PROFILE_DATA (cpu)) [insn_num]; \
} while (0)
#else
#define PROFILE_COUNT_INSN(cpu, pc, insn_num)
#endif /* ! insn */

#if WITH_PROFILE_MEMORY_P
#define PROFILE_COUNT_READ(cpu, addr, mode_num) \
do { \
  if (CPU_PROFILE_FLAGS (cpu) [PROFILE_MEMORY_IDX]) \
    ++ PROFILE_READ_COUNT (CPU_PROFILE_DATA (cpu)) [mode_num]; \
} while (0)
#define PROFILE_COUNT_WRITE(cpu, addr, mode_num) \
do { \
  if (CPU_PROFILE_FLAGS (cpu) [PROFILE_MEMORY_IDX]) \
    ++ PROFILE_WRITE_COUNT (CPU_PROFILE_DATA (cpu)) [mode_num]; \
} while (0)
#else
#define PROFILE_COUNT_READ(cpu, addr, mode_num)
#define PROFILE_COUNT_WRITE(cpu, addr, mode_num)
#endif /* ! memory */

#if WITH_PROFILE_CORE_P
#define PROFILE_COUNT_CORE(cpu, addr, size, map) \
do { \
  if (CPU_PROFILE_FLAGS (cpu) [PROFILE_CORE_IDX]) \
    PROFILE_CORE_COUNT (CPU_PROFILE_DATA (cpu)) [map] += 1; \
} while (0)
#else
#define PROFILE_COUNT_CORE(cpu, addr, size, map)
#endif /* ! core */

#if WITH_PROFILE_MODEL_P
/* Model profiling is a bit more complicated so we just provide a macro
   to say whether to do it or not.  */
#define PROFILE_MODEL_P(cpu) \
(CPU_PROFILE_FLAGS (cpu) [PROFILE_MODEL_IDX])
#else
#define PROFILE_MODEL_P(cpu) 0
#endif /* ! model */

#endif /* SIM_PROFILE_H */
