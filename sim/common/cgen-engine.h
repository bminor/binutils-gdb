/* Simulator header for the cgen engine.
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

/* This file must be included after eng.h has been included
   as it specifies the configuration of the engine.  */

#ifndef CGEN_ENGINE_H
#define CGEN_ENGINE_H

/* Execution support.  */

#if WITH_SCACHE

/* instruction address
   ??? This was intended to be a struct of two elements in the WITH_SCACHE_PBB
   case.  The first element is the PCADDR, the second element is the SCACHE *.
   Haven't found the time yet to make this work, but it is a nicer approach
   than the current br_cache stuff.  */
typedef PCADDR IADDR;
/* current instruction address */
typedef PCADDR CIA;
/* argument to semantic functions */
typedef SCACHE *SEM_ARG;
/* semantic code's version of pc */
#if WITH_SCACHE_PBB
typedef SCACHE *SEM_PC;
#else
typedef PCADDR SEM_PC;
#endif

#else /* ! WITH_SCACHE */

/* instruction address */
typedef PCADDR IADDR;
/* current instruction address */
typedef PCADDR CIA;
/* argument to semantic functions */
typedef ARGBUF *SEM_ARG;
/* semantic code's version of pc */
typedef PCADDR SEM_PC;

#endif /* ! WITH_SCACHE */

/* Semantic functions come in six versions on two axes:
   fast/full-featured, and using one of the simple/scache/compilation engines.
   A full featured simulator is always provided.  --enable-sim-fast includes
   support for fast execution by duplicating the semantic code but leaving
   out all features like tracing and profiling.
   Using the scache is selected with --enable-sim-scache.  */
/* FIXME: --enable-sim-fast not implemented yet.  */
/* FIXME: undecided how to handle WITH_SCACHE_PBB.  */

/* Types of the machine generated extract and semantic fns.  */
typedef void (EXTRACT_FN) (SIM_CPU *, PCADDR, insn_t, ARGBUF *);
#ifdef HAVE_PARALLEL_EXEC
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

/* Engine support.  */

/* These are used so that we can compile two copies of the semantic code,
   one with full feature support and one without that runs fast(er).  */
/* FIXME: Eventually delete extraction if not using scache.  */
#define EX_FN_NAME(cpu,fn) XCONCAT3 (cpu,_ex_,fn)
#define SEM_FN_NAME(cpu,fn) XCONCAT3 (cpu,_sem_,fn)
#define SEMF_FN_NAME(cpu,fn) XCONCAT3 (cpu,_semf_,fn)

#if WITH_SCACHE

#define CIA_ADDR(cia) (cia)

/* semantics.c support */
#define SEM_ARGBUF(sem_arg) (& (sem_arg) -> argbuf)
#define SEM_INSN(sem_arg) shouldnt_be_used
#define SEM_NEXT_VPC(sc, len) ((sc) + 1)

#if WITH_SCACHE_PBB

/* Update the instruction counter.  */
#define PBB_UPDATE_INSN_COUNT(cpu,sc) \
  (CPU_INSN_COUNT (cpu) += SEM_ARGBUF (sc) -> fields.chain.insn_count)

/* Value for br_addr_ptr indicating branch wasn't taken.  */
#define SEM_BRANCH_UNTAKEN ((SEM_PC *) 0)
/* Value for br_addr_ptr indicating branch was taken to uncacheable
   address (e.g. j reg).  */
#define SEM_BRANCH_UNCACHEABLE ((SEM_PC *) 1)

/* ??? Only necessary if SEM_BRANCH_VIA_CACHE will be used,
   but for simplicity it's done this way.  */
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
/* Do not append a `;' to invocations of this.
   ??? Unnecessary here, but for consistency with ..._INIT.  */
#define SEM_BRANCH_FINI \
{ \
  pbb_br_npc = npc; \
  pbb_br_npc_ptr = npc_ptr; \
}
#else /* 1 semantic function per instruction */
/* Do not append a `;' to invocations of this.
   ??? Unnecessary here, but for consistency with ..._INIT.  */
#define SEM_BRANCH_FINI \
{ \
  CPU_PBB_BR_NPC (current_cpu) = npc; \
  CPU_PBB_BR_NPC_PTR (current_cpu) = npc_ptr; \
}
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

#define SEM_BRANCH_INIT
#define SEM_BRANCH_FINI

#define SEM_BRANCH_ADDR_CACHE(sem_arg) shouldnt_be_used
#define SEM_BRANCH_VIA_CACHE(cpu, sc, newval, pcvar, cachevar) \
do { \
  (pcvar) = (newval); \
} while (0)
#define SEM_BRANCH_VIA_ADDR(cpu, sc, newval, pcvar) \
do { \
  (pcvar) = (newval); \
} while (0)

#endif /* ! WITH_SCACHE_PBB */

/* Return address a branch insn will branch to.
   This is only used during tracing.  */
#define SEM_NEW_PC_ADDR(new_pc) (new_pc)

#else /* ! WITH_SCACHE */

#define CIA_ADDR(cia) (cia)

/* semantics.c support */
#define SEM_ARGBUF(sem_arg) (sem_arg)
#define SEM_INSN(sem_arg) (SEM_ARGBUF (sem_arg) -> insn)
/* FIXME:wip */
#define SEM_NEXT_VPC(abuf, len) ((abuf) -> addr + (abuf) -> length)

#define SEM_BRANCH_INIT
#define SEM_BRANCH_FINI

#define SEM_BRANCH_ADDR_CACHE(sem_arg) shouldnt_be_used
#define SEM_BRANCH_VIA_CACHE(cpu, abuf, newval, pcvar, cachevar) \
do { \
  (pcvar) = (newval); \
} while (0)
#define SEM_BRANCH_VIA_ADDR(cpu, abuf, newval, pcvar) \
do { \
  (pcvar) = (newval); \
} while (0)

#define SEM_NEW_PC_ADDR(new_pc) (new_pc)

#endif /* ! WITH_SCACHE */

#endif /* CGEN_ENGINE_H */
