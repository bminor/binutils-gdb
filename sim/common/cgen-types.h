/* Types for Cpu tools GENerated simulators.
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

#ifndef CGEN_TYPES_H
#define CGEN_TYPES_H

#ifdef __GNUC__
#define HAVE_LONGLONG
#undef DI_FN_SUPPORT
#define SIM_INLINE extern inline
#else
#undef HAVE_LONGLONG
#define DI_FN_SUPPORT
#define SIM_INLINE
#endif

#ifndef CGEN_CAT3
#if defined(__STDC__) || defined(ALMOST_STDC)
#define CGEN_XCAT3(a,b,c) a ## b ## c
#define CGEN_CAT3(a,b,c) CGEN_XCAT3 (a, b, c)
#else
#define CGEN_CAT3(a,b,c) a/**/b/**/c
#endif
#endif

extern const char *mode_names[];
#define MODE_NAME(m) (mode_names[m])

#ifdef __STDC__
typedef /*FIXME*/ signed char BI;
typedef /*FIXME*/ signed char QI;
#else
typedef /*FIXME*/ char BI;
typedef /*FIXME*/ char QI;
#endif
typedef short HI;
typedef int SI;
typedef unsigned char UBI;
typedef unsigned char UQI;
typedef unsigned short UHI;
typedef unsigned int USI;

#ifdef HAVE_LONGLONG
typedef long long DI;
typedef unsigned long long UDI;
#define GETLODI(di) ((SI) (di))
#define GETHIDI(di) ((SI) ((di) >> 32))
#define SETLODI(di, val) ((di) = (((di) & 0xffffffff00000000LL) | (val)))
#define SETHIDI(di, val) ((di) = (((di) & 0xffffffffLL) | (((DI) (val)) << 32)))
#define SETDI(di, hi, lo) ((di) = MAKEDI (hi, lo))
#define MAKEDI(hi, lo) ((((DI) (hi)) << 32) | ((DI) (lo)))
#else
typedef struct { SI hi,lo; } DI;
typedef DI UDI;
#define GETLODI(di) ((di).lo)
#define GETHIDI(di) ((di).hi)
#define SETLODI(di, val) ((di).lo = (val))
#define SETHIDI(di, val) ((di).hi = (val))
#define SETDI(di, hi, lo) ((di) = MAKEDI (hi, lo))
extern DI make_struct_di (SI, SI);
#define MAKEDI(hi, lo) (make_struct_di ((hi), (lo)))
#endif

/* FIXME: Need to provide libraries if these aren't appropriate for target,
   or user's needs.  */
typedef float SF;
typedef double DF;
typedef double XF; /* FIXME: configure, provide library */
typedef double TF; /* FIXME: configure, provide library */

struct argbuf;
struct cgen_insn;
struct scache;

/* This is used to record extracted raw data from an instruction, among other
   things.  It must be a host data type, and not a target one so USI is
   inappropriate.  */
typedef unsigned int UINT;

typedef unsigned long PCADDR;
typedef unsigned long ADDR;
typedef /*FIXME*/ unsigned long insn_t;

/* Forward declaration of STATE.
   Must be defined before profile.h and other module headers.  */
typedef struct sim_state *STATE;

/* Execution support.

   Semantic functions come in two versions.
   One that uses the cache, and one that doesn't.
   The one that doesn't may eventually be thrown away or replaced with
   something else.  */

/* ??? The cache stuff is still wip, but it at least works.  */

#ifdef SCACHE_P

/* iaddr: instruction address */
typedef PCADDR IADDR;
/* cia: current instruction address */
typedef PCADDR CIA;
#define CIA_ADDR(cia) (cia)
typedef struct scache *SEM_ARG;
#define EX_FN_NAME(fn) CGEN_CAT3 (exc,_,fn)
#define SEM_FN_NAME(fn) CGEN_CAT3 (semc,_,fn)

/* extract.c support */
/* scache_unset is a cache entry that is never used.
   It's raison d'etre is so BRANCH_VIA_CACHE doesn't have to test for
   newval.cache == NULL.  */
extern struct scache scache_unset;
#define RECORD_IADDR(fld, val) \
do { (fld) = (val); } while (0)

/* semantics.c support */
#define SEM_ARGBUF(sem_arg) (&(sem_arg)->argbuf)
#define SEM_NEXT_PC(sc) ((sc)->next)
#define SEM_BRANCH_VIA_CACHE(sc, newval) (newval)
#define SEM_BRANCH_VIA_ADDR(sc, newval) (newval)
/* Return address a branch insn will branch to.
   This is only used during tracing.  */
#define SEM_NEW_PC_ADDR(new_pc) (new_pc)

#else /* ! SCACHE_P */

typedef PCADDR IADDR;
typedef PCADDR CIA;
#define CIA_ADDR(cia) (cia)
typedef struct argbuf *SEM_ARG;
#define EX_FN_NAME(fn) CGEN_CAT3 (ex,_,fn)
#define SEM_FN_NAME(fn) CGEN_CAT3 (sem,_,fn)

/* extract.c support */
#define RECORD_IADDR(fld, val) \
do { (fld) = (val); } while (0)

/* semantics.c support */
#define SEM_ARGBUF(sem_arg) (sem_arg)
#define SEM_NEXT_PC(abuf) (abuf->addr + abuf->length)
#define SEM_BRANCH_VIA_CACHE(abuf, newval) (newval)
#define SEM_BRANCH_VIA_ADDR(abuf, newval) (newval)
#define SEM_NEW_PC_ADDR(new_pc) (new_pc)

#endif /* ! SCACHE_P */

#define EXTRACT_SIGNED(val, total, start, length) \
(((((val) >> ((total) - ((start) + (length)))) & ((1 << (length)) - 1)) \
  ^ (1 << ((length) - 1))) \
 - (1 << ((length) - 1)))

#define EXTRACT_UNSIGNED(val, total, start, length) \
(((val) >> ((total) - ((start) + (length)))) & ((1 << (length)) - 1))

/* Compute number of longs required to hold N bits.  */
#define HOST_LONGS_FOR_BITS(n) \
  (((n) + sizeof (long) * 8 - 1) / sizeof (long) * 8)

#endif /* CGEN_TYPES_H */
