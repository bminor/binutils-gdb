/* Engine header for Cpu tools GENerated simulators.
   Copyright (C) 1998 Free Software Foundation, Inc.
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

/* This file must be included after eng.h and before ${cpu}.h.  */

/* Semantic functions come in six versions on two axes:
   fast/full-featured, and using one of the simple/scache/compilation engines.
   A full featured simulator is always provided.  --enable-sim-fast includes
   support for fast execution by duplicating the semantic code but leaving
   out all features like tracing and profiling.
   Using the scache is selected with --enable-sim-scache.  */
/* FIXME: --enable-sim-fast not implemented yet.  */
/* FIXME: undecided how to handle WITH_SCACHE_PBB.  */

#ifndef CGEN_ENGINE_H
#define CGEN_ENGINE_H

/* Instruction field support macros.  */

#define EXTRACT_MSB0_INT(val, total, start, length) \
(((INT) (val) << ((sizeof (INT) * 8) - (total) + (start))) \
 >> ((sizeof (INT) * 8) - (length)))
#define EXTRACT_MSB0_UINT(val, total, start, length) \
(((UINT) (val) << ((sizeof (UINT) * 8) - (total) + (start))) \
 >> ((sizeof (UINT) * 8) - (length)))

#define EXTRACT_LSB0_INT(val, total, start, length) \
(((INT) (val) << ((sizeof (INT) * 8) - (start) - (length))) \
 >> ((sizeof (INT) * 8) - (length)))
#define EXTRACT_LSB0_UINT(val, total, start, length) \
(((UINT) (val) << ((sizeof (UINT) * 8) - (start) - (length))) \
 >> ((sizeof (UINT) * 8) - (length)))

#if CGEN_INSN_LSB0_P

#define EXTRACT_INT(val, total, start, length) \
  EXTRACT_LSB0_INT ((val), (total), (start), (length))
#define EXTRACT_UINT(val, total, start, length) \
  EXTRACT_LSB0_UINT ((val), (total), (start), (length))

#else

#define EXTRACT_INT(val, total, start, length) \
  EXTRACT_MSB0_INT ((val), (total), (start), (length))
#define EXTRACT_UINT(val, total, start, length) \
  EXTRACT_MSB0_UINT ((val), (total), (start), (length))

#endif

/* union sem */

/* Types of the machine generated extract and semantic fns.  */
typedef void (EXTRACT_FN) (SIM_CPU *, PCADDR, insn_t, ARGBUF *);
#if HAVE_PARALLEL_INSNS
typedef SEM_PC (SEMANTIC_FN) (SIM_CPU *, SEM_ARG, PAREXEC *);
#else
typedef SEM_PC (SEMANTIC_FN) (SIM_CPU *, SEM_ARG);
#endif

union sem {
#if ! WITH_SEM_SWITCH_FULL
  SEMANTIC_FN *sem_full;
#endif
#if ! WITH_SEM_SWITCH_FAST
  SEMANTIC_FN *sem_fast;
#endif
#if WITH_SEM_SWITCH_FULL || WITH_SEM_SWITCH_FAST
#ifdef __GNUC__
  void *sem_case;
#else
  int sem_case;
#endif
#endif
};

/* Set the appropriate semantic handler in ABUF.  */

#if WITH_SEM_SWITCH_FULL
#ifdef __GNUC__
#define SEM_SET_FULL_CODE(abuf, idesc) \
  do { (abuf)->semantic.sem_case = (idesc)->sem_full_lab; } while (0)
#else 
#define SEM_SET_FULL_CODE(abuf, idesc) \
  do { (abuf)->semantic.sem_case = (idesc)->num; } while (0)
#endif
#else
#define SEM_SET_FULL_CODE(abuf, idesc) \
  do { (abuf)->semantic.sem_full = (idesc)->sem_full; } while (0)
#endif

#if WITH_SEM_SWITCH_FAST
#ifdef __GNUC__
#define SEM_SET_FAST_CODE(abuf, idesc) \
  do { (abuf)->semantic.sem_case = (idesc)->sem_fast_lab; } while (0)
#else 
#define SEM_SET_FAST_CODE(abuf, idesc) \
  do { (abuf)->semantic.sem_case = (idesc)->num; } while (0)
#endif
#else
#define SEM_SET_FAST_CODE(abuf, idesc) \
  do { (abuf)->semantic.sem_fast = (idesc)->sem_fast; } while (0)
#endif

#define SEM_SET_CODE(abuf, idesc, fast_p) \
do { \
  if (fast_p) \
    SEM_SET_FAST_CODE ((abuf), (idesc)); \
  else \
    SEM_SET_FULL_CODE ((abuf), (idesc)); \
} while (0)

#define IDESC_CTI_P(idesc) \
     ((CGEN_ATTR_BOOLS (CGEN_INSN_ATTRS ((idesc)->opcode)) \
       & (CGEN_ATTR_MASK (CGEN_INSN_COND_CTI) \
	  | CGEN_ATTR_MASK (CGEN_INSN_UNCOND_CTI))) \
      != 0)
#define IDESC_SKIP_P(idesc) \
     ((CGEN_ATTR_BOOLS (CGEN_INSN_ATTRS ((idesc)->opcode)) \
       & CGEN_ATTR_MASK (CGEN_INSN_SKIP_CTI)) \
      != 0)

/* These are used so that we can compile two copies of the semantic code,
   one with full feature support and one without that runs fast(er).  */
#define SEM_FN_NAME(cpu,fn) XCONCAT3 (cpu,_sem_,fn)
#define SEMF_FN_NAME(cpu,fn) XCONCAT3 (cpu,_semf_,fn)

#if WITH_SCACHE

#define CIA_ADDR(cia) (cia)

/* semantics.c support */
#define SEM_ARGBUF(sem_arg) (& (sem_arg) -> argbuf)
#define SEM_INSN(sem_arg) shouldnt_be_used

#if WITH_SCACHE_PBB

/* Return the scache pointer of the current insn.  */
#define SEM_SEM_ARG(vpc, sc) (vpc)
/* Return the virtual pc of the next insn to execute
   (assuming this isn't a cti).  */
#define SEM_NEXT_VPC(sem_arg, pc, len) ((sem_arg) + 1)

/* Update the instruction counter.  */
#define PBB_UPDATE_INSN_COUNT(cpu,sc) \
  (CPU_INSN_COUNT (cpu) += SEM_ARGBUF (sc) -> fields.chain.insn_count)

/* Value for br_addr_ptr indicating branch wasn't taken.  */
#define SEM_BRANCH_UNTAKEN ((SEM_PC *) 0)
/* Value for br_addr_ptr indicating branch was taken to uncacheable
   address (e.g. j reg).  */
#define SEM_BRANCH_UNCACHEABLE ((SEM_PC *) 1)

/* Initialize next-pbb link for SEM_BRANCH_VIA_CACHE.  */
#define SEM_BRANCH_INIT_EXTRACT(abuf) \
do { (abuf)->fields.cti.addr_cache = 0; } while (0)

/* Do not append a `;' to invocations of this.
   npc,npc_ptr are for communication between the cti insn and cti-chain.  */
#define SEM_BRANCH_INIT \
  PCADDR npc = 0; /* assign a value for -Wall */ \
  SEM_PC *npc_ptr = SEM_BRANCH_UNTAKEN;
/* SEM_IN_SWITCH is defined at the top of the mainloop.c files
   generated by genmloop.sh.  It exists so generated semantic code needn't
   care whether it's being put in a switch or in a function.  */
#ifdef SEM_IN_SWITCH
#define SEM_BRANCH_FINI(pcvar) \
do { \
  pbb_br_npc = npc; \
  pbb_br_npc_ptr = npc_ptr; \
} while (0)
#else /* 1 semantic function per instruction */
#define SEM_BRANCH_FINI(pcvar) \
do { \
  CPU_PBB_BR_NPC (current_cpu) = npc; \
  CPU_PBB_BR_NPC_PTR (current_cpu) = npc_ptr; \
} while (0)
#endif

/* Return address of cached branch address value.  */
#define SEM_BRANCH_ADDR_CACHE(sem_arg) \
  (& SEM_ARGBUF (sem_arg)->fields.cti.addr_cache)
#define SEM_BRANCH_VIA_CACHE(cpu, sc, newval, pcvar, cachevarptr) \
do { \
  npc = (newval); \
  npc_ptr = (cachevarptr); \
} while (0)
#define SEM_BRANCH_VIA_ADDR(cpu, sc, newval, pcvar) \
do { \
  npc = (newval); \
  npc_ptr = SEM_BRANCH_UNCACHEABLE; \
} while (0)

#else /* ! WITH_SCACHE_PBB */

#define SEM_SEM_ARG(vpc, sc) (sc)
#define SEM_NEXT_VPC(sem_arg, pc, len) ((pc) + (len))

#define SEM_BRANCH_INIT_EXTRACT(abuf) do { } while (0)

#define SEM_BRANCH_INIT \
  int taken_p = 0;
#ifndef TARGET_SEM_BRANCH_FINI(pcvar, taken_p)
#define TARGET_SEM_BRANCH_FINI(pcvar, taken_p)
#endif
#define SEM_BRANCH_FINI(pcvar) \
  do { TARGET_SEM_BRANCH_FINI (pcvar, taken_p); } while (0)

#define SEM_BRANCH_ADDR_CACHE(sem_arg) shouldnt_be_used
#define SEM_BRANCH_VIA_CACHE(cpu, sc, newval, pcvar, cachevar) \
do { \
  (pcvar) = (newval); \
  taken_p = 1; \
} while (0)
#define SEM_BRANCH_VIA_ADDR(cpu, sc, newval, pcvar) \
do { \
  (pcvar) = (newval); \
  taken_p = 1; \
} while (0)

#endif /* ! WITH_SCACHE_PBB */

#else /* ! WITH_SCACHE */

#define CIA_ADDR(cia) (cia)

/* semantics.c support */
#define SEM_ARGBUF(sem_arg) (sem_arg)
#define SEM_INSN(sem_arg) (SEM_ARGBUF (sem_arg) -> base_insn)

#define SEM_SEM_ARG(vpc, sc) (sc)
#define SEM_NEXT_VPC(sem_arg, pc, len) ((pc) + (len))

#define SEM_BRANCH_INIT \
  int taken_p = 0;
#ifndef TARGET_SEM_BRANCH_FINI
#define TARGET_SEM_BRANCH_FINI(pcvar, taken_p)
#endif
#define SEM_BRANCH_FINI(pcvar) \
  do { TARGET_SEM_BRANCH_FINI (pcvar, taken_p); } while (0)

#define SEM_BRANCH_ADDR_CACHE(sem_arg) shouldnt_be_used
#define SEM_BRANCH_VIA_CACHE(cpu, abuf, newval, pcvar, cachevar) \
do { \
  (pcvar) = (newval); \
  taken_p = 1; \
} while (0)
#define SEM_BRANCH_VIA_ADDR(cpu, abuf, newval, pcvar) \
do { \
  (pcvar) = (newval); \
  taken_p = 1; \
} while (0)

#endif /* ! WITH_SCACHE */

/* Tracing/profiling.  */

/* Return non-zero if a before/after handler is needed.
   When tracing/profiling a selected range there's no need to slow
   down simulation of the other insns (except to get more accurate data!).

   ??? May wish to profile all insns if doing insn tracing, or to
   get more accurate cycle data.

   First test ANY_P so we avoid a potentially expensive HIT_P call
   [if there are lots of address ranges].  */

#define PC_IN_TRACE_RANGE_P(cpu, pc) \
  (TRACE_ANY_P (cpu) \
   && ADDR_RANGE_HIT_P (TRACE_RANGE (CPU_TRACE_DATA (cpu)), (pc)))
#define PC_IN_PROFILE_RANGE_P(cpu, pc) \
  (PROFILE_ANY_P (cpu) \
   && ADDR_RANGE_HIT_P (PROFILE_RANGE (CPU_PROFILE_DATA (cpu)), (pc)))

#endif /* CGEN_ENGINE_H */
