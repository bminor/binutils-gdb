/* Simulator CPU header for arc.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright 1996-2007 Free Software Foundation, Inc.

This file is part of the GNU simulators.

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

*/

#ifndef ARC_CPUALL_H
#define ARC_CPUALL_H

/* Include files for each cpu family.  */

#ifdef WANT_CPU_A5F
#include "eng5.h"
#include "cgen-engine.h"
#include "cpu5.h"
#include "decode5.h"
#endif

#ifdef WANT_CPU_ARC600F
#include "eng6.h"
#include "cgen-engine.h"
#include "cpu6.h"
#include "decode6.h"
#endif

#ifdef WANT_CPU_ARC700F
#include "eng7.h"
#include "cgen-engine.h"
#include "cpu7.h"
#include "decode7.h"
#endif

extern const MACH a5_mach;
extern const MACH arc600_mach;
extern const MACH arc700_mach;

#ifndef WANT_CPU
/* The ARGBUF struct.  */
struct argbuf {
  /* These are the baseclass definitions.  */
  IADDR addr;
  const IDESC *idesc;
  char trace_p;
  char profile_p;
  /* ??? Temporary hack for skip insns.  */
  char skip_count;
  char unused;
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

#endif /* ARC_CPUALL_H */
