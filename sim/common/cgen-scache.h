/* Simulator cache definitions for CGEN simulators (and maybe others).
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

#ifndef SCACHE_H
#define SCACHE_H

/* A cached insn.  */
typedef struct scache {
  IADDR next;
  union {
#ifdef USE_SEM_SWITCH
#ifdef __GNUC__
    void *sem_case;
#else
    int sem_case;
#endif
#endif
    SEMANTIC_CACHE_FN *sem_fn;
  } semantic;
  ARGBUF argbuf;
} SCACHE;

/* Scache data for each cpu.  */

typedef struct cpu_scache {
  /* Simulator cache size.  */
  int size;
#define CPU_SCACHE_SIZE(cpu) ((cpu)->cgen_cpu.scache.size)
  /* Cache.  */
  SCACHE *cache;
#define CPU_SCACHE_CACHE(cpu) ((cpu)->cgen_cpu.scache.cache)
#if 0 /* FIXME: wip */
  /* Free list.  */
  SCACHE *free;
#define CPU_SCACHE_FREE(cpu) ((cpu)->cgen_cpu.scache.free)
  /* Hash table.  */
  SCACHE **hash_table;
#define CPU_SCACHE_HASH_TABLE(cpu) ((cpu)->cgen_cpu.scache.hash_table)
#endif

#if WITH_PROFILE_SCACHE_P
  /* Cache hits, misses.  */
  unsigned long hits, misses;
#define CPU_SCACHE_HITS(cpu) ((cpu)->cgen_cpu.scache.hits)
#define CPU_SCACHE_MISSES(cpu) ((cpu)->cgen_cpu.scache.misses)
#endif
} CPU_SCACHE;

/* Default number of cached blocks.  */
#ifdef CONFIG_SIM_CACHE_SIZE
#define SCACHE_DEFAULT_CACHE_SIZE CONFIG_SIM_CACHE_SIZE
#else
#define SCACHE_DEFAULT_CACHE_SIZE 1024
#endif

/* Hash a PC value.  */
/* FIXME: cpu specific */
#define SCACHE_HASH_PC(state, pc) \
(((pc) >> 1) & (STATE_SCACHE_SIZE (sd) - 1))

/* Non-zero if cache is in use.  */
#define USING_SCACHE_P(sd) (STATE_SCACHE_SIZE (sd) > 0)

/* Install the simulator cache into the simulator.  */
MODULE_INSTALL_FN scache_install;

/* Flush all cpu's caches.  */
void scache_flush (SIM_DESC);

/* Profiling support.  */

/* Print summary scache usage information.  */
void scache_print_profile (SIM_DESC sd, int verbose);

#if WITH_PROFILE_SCACHE_P
#define PROFILE_COUNT_SCACHE_HIT(cpu) \
do { \
  if (CPU_PROFILE_FLAGS (cpu) [PROFILE_SCACHE_IDX]) \
    ++ CPU_SCACHE_HITS (cpu); \
} while (0)
#define PROFILE_COUNT_SCACHE_MISS(cpu) \
do { \
  if (CPU_PROFILE_FLAGS (cpu) [PROFILE_SCACHE_IDX]) \
    ++ CPU_SCACHE_MISSES (cpu); \
} while (0)
#else
#define PROFILE_COUNT_SCACHE_HIT(cpu)
#define PROFILE_COUNT_SCACHE_MISS(cpu)
#endif

#endif /* SCACHE_H */
