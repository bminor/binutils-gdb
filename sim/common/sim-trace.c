/* Simulator tracing/debugging support.
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

#include "sim-main.h"
#include "sim-io.h"

void
trace_printf VPARAMS ((sim_cpu *cpu, const char *fmt, ...))
{
#ifndef __STDC__
  sim_cpu *cpu;
  const char *fmt;
#endif
  va_list ap;

  VA_START (ap, fmt);
#ifndef __STDC__
  cpu = va_arg (ap, sim_cpu *);
  fmt = va_arg (ap, const char *);
#endif

  if (CPU_TRACE_FILE (cpu) == NULL)
    (* STATE_CALLBACK (CPU_STATE (cpu))->evprintf_filtered)
      (STATE_CALLBACK (CPU_STATE (cpu)), fmt, ap);
  else
    vfprintf (CPU_TRACE_FILE (cpu), fmt, ap);

  va_end (ap);
}

void
debug_printf VPARAMS ((sim_cpu *cpu, const char *fmt, ...))
{
#ifndef __STDC__
  sim_cpu *cpu;
  const char *fmt;
#endif
  va_list ap;

  VA_START (ap, fmt);
#ifndef __STDC__
  cpu = va_arg (ap, sim_cpu *);
  fmt = va_arg (ap, const char *);
#endif

  if (CPU_DEBUG_FILE (cpu) == NULL)
    (* STATE_CALLBACK (CPU_STATE (cpu))->evprintf_filtered)
      (STATE_CALLBACK (CPU_STATE (cpu)), fmt, ap);
  else
    vfprintf (CPU_DEBUG_FILE (cpu), fmt, ap);

  va_end (ap);
}
