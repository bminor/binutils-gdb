/* Simulator CPU header for sparc.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1999 Cygnus Solutions, Inc.

This file is part of the Cygnus Simulators.


*/

#ifndef SPARC_CPUALL_H
#define SPARC_CPUALL_H

/* Include files for each cpu family.  */

#ifdef WANT_CPU_SPARC32
#include "eng32.h"
#include "cgen-engine.h"
#include "cpu32.h"
#include "decode32.h"
#endif

extern const MACH sparc_v8_mach;
extern const MACH sparclite_mach;

#ifndef WANT_CPU
/* The ARGBUF struct.  */
struct argbuf {
  /* These are the baseclass definitions.  */
  IADDR addr;
  const IDESC *idesc;
  char trace_p;
  char profile_p;
  /* cpu specific data follows */
};
#endif

#ifndef WANT_CPU
/* A cached insn.

   ??? SCACHE used to contain more than just argbuf.  We could delete the
   type entirely and always just use ARGBUF, but for future concerns and as
   a level of abstraction it is left in.  */

struct scache {
  struct argbuf argbuf;
};
#endif

#endif /* SPARC_CPUALL_H */
