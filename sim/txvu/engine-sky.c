/*  Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>
    Copyright (C) 1998, Cygnus Solutions

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    */

#ifndef _ENGINE_C_
#define _ENGINE_C_

#include "sim-inline.c"

#include "sim-main.h"
#include "itable.h"
#include "idecode.h"
#include "semantics.h"
#include "icache.h"
#include "engine.h"
#include "support.h"

#include "sim-assert.h"

#include "hardware.h"

enum {
  /* greater or equal to zero => table */
  function_entry = -1,
  boolean_entry = -2,
};

typedef struct _idecode_table_entry {
  int shift;
  unsigned32 mask;
  unsigned32 value;
  void *function_or_table;
} idecode_table_entry;


INLINE_ENGINE\
(void) engine_run
(SIM_DESC sd,
 int next_cpu_nr,
 int siggnal)
{
  sim_cpu *processor = NULL;
  instruction_address cia;
  int current_cpu = next_cpu_nr;
  instruction_word instruction_0;

  /* Hard coded main loop.  Not pretty, but should work. */
  ASSERT(current_cpu >= 0 && current_cpu < NUMBER_CPUS);

  cia = CPU_CIA (0);			/* Only the 5900 uses this. */
  processor = STATE_CPU (sd, 0);	/* Only the 5900 uses this. */

  switch(current_cpu) 
    {
    case 0:
      goto cpu_0;
    case 1:
      goto cpu_1;
    case 2:
      goto cpu_2;
    case 3:
      goto cpu_3;
    case 4:
      goto cpu_4;
    }
  
  while (1)
    {

cpu_0:

      cia = CPU_CIA (processor);
      instruction_0 = IMEM (cia);

#if defined (ENGINE_ISSUE_PREFIX_HOOK)
      ENGINE_ISSUE_PREFIX_HOOK();
#endif

      cia = idecode_issue(sd, instruction_0, cia);

#if defined (ENGINE_ISSUE_POSTFIX_HOOK)
      ENGINE_ISSUE_POSTFIX_HOOK();
#endif

      /* Update the instruction address */
      CPU_CIA (processor) = cia;

cpu_1:
	pke0_issue();

cpu_2:
	pke1_issue();

cpu_3:
	vu0_issue();

cpu_4:
	vu1_issue();

events:

      /* process any events */
      if (sim_events_tick (sd))
        {
          sim_events_process (sd);
        }
    }
}

#endif /* _ENGINE_C_*/
