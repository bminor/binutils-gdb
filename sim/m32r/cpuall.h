/* Simulator CPU header for m32r.

Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

This file is part of the GNU Simulators.

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
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#ifndef M32R_CPUALL_H
#define M32R_CPUALL_H

extern const IMP_PROPERTIES m32r_imp_properties;

extern const MODEL m32r_models[];

#ifndef WANT_CPU
/* The ARGBUF struct.  */
struct argbuf {
  /* These are the baseclass definitions.  */
  unsigned int length;
  PCADDR addr;
  const struct cgen_insn *opcode;
  /* unsigned long insn; - no longer needed */
  /* cpu specific data follows */
};
#endif

#ifndef WANT_CPU
/* A cached insn.
   This is also used in the non-scache case.  In this situation we assume
   the cache size is 1, and do a few things a little differently.  */

struct scache {
  IADDR next;
  union {
#if ! WITH_SEM_SWITCH_FULL
    SEMANTIC_FN *sem_fn;
#endif
#if ! WITH_SEM_SWITCH_FAST
#if WITH_SCACHE
    SEMANTIC_CACHE_FN *sem_fast_fn;
#else
    SEMANTIC_FN *sem_fast_fn;
#endif
#endif
#if WITH_SEM_SWITCH_FULL || WITH_SEM_SWITCH_FAST
#ifdef __GNUC__
    void *sem_case;
#else
    int sem_case;
#endif
#endif
  } semantic;
  struct argbuf argbuf;
};
#endif

#endif /* M32R_CPUALL_H */
