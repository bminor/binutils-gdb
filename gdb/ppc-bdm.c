/* Remote target communications for the Macraigor Systems BDM Wiggler
   talking to a Motorola PPC 505.
   Copyright 1996, 1997 Free Software Foundation, Inc.

This file is part of GDB.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "gdbcore.h"
#include "gdb_string.h"
#include <fcntl.h>
#include "frame.h"
#include "inferior.h"
#include "bfd.h"
#include "symfile.h"
#include "target.h"
#include "wait.h"
#include "gdbcmd.h"
#include "objfiles.h"
#include "gdb-stabs.h"
#include <sys/types.h>
#include <signal.h>
#include "serial.h"
#include "ocd.h"

static void bdm_ppc_open PARAMS ((char *name, int from_tty));

static int bdm_ppc_wait PARAMS ((int pid,
				 struct target_waitstatus *target_status));

static void bdm_ppc_fetch_registers PARAMS ((int regno));

static void bdm_ppc_store_registers PARAMS ((int regno));

extern struct target_ops bdm_ppc_ops;	/* Forward decl */

/* Open a connection to a remote debugger.
   NAME is the filename used for communication.  */

static void
bdm_ppc_open (name, from_tty)
     char *name;
     int from_tty;
{
  ocd_open (name, from_tty, OCD_TARGET_MOTO_PPC, &bdm_ppc_ops);
}

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.
   Returns "pid" (though it's not clear what, if anything, that
   means in the case of this target).  */

static int
bdm_ppc_wait (pid, target_status)
     int pid;
     struct target_waitstatus *target_status;
{
  int stop_reason;

  target_status->kind = TARGET_WAITKIND_STOPPED;

  stop_reason = ocd_wait ();

  if (stop_reason)
    {
      target_status->value.sig = TARGET_SIGNAL_INT;
      return inferior_pid;
    }

  target_status->value.sig = TARGET_SIGNAL_TRAP; /* XXX for now */

  {
    unsigned long ecr, der;

    ecr = ocd_read_bdm_register (148); /* Read the exception cause register */
    der = ocd_read_bdm_register (149); /* Read the debug enables register */
    fprintf_unfiltered (gdb_stdout, "ecr = 0x%x, der = 0x%x\n", ecr, der);
  }

  return inferior_pid;
}

static int bdm_regmap[] = {BDM_REGMAP};

/* Read the remote registers into regs.

   The Wiggler uses the following codes to access the registers:

   0 -> 1023		SPR 0 -> 1023
	0 - SPR 0 - MQ
	1 - SPR 1 - XER
	8 - SPR 8 - LR
	9 - SPR 9 - CTR (known as cnt in GDB)
	26 - SPR 26 - SRR0 - pc
   1024 -> 2047		DCR 0 -> DCR 1023 (IBM PPC 4xx only)
   2048 -> 2079		R0 -> R31
   2080 -> 2143		FP0 -> FP31 (64 bit regs)
   2144			CR (known as cnd in GDB)
   2145			FPCSR
   2146			MSR (known as ps in GDB)
 */

static void
bdm_ppc_fetch_registers (regno)
     int regno;
{
  int i;
  unsigned char *regs;
  int first_regno, last_regno;
  int first_bdm_regno, last_bdm_regno;
  int reglen;

  if (regno == -1)
    {
      first_regno = 0;
      last_regno = NUM_REGS - 1;

      first_bdm_regno = 0;
      last_bdm_regno = BDM_NUM_REGS - 1;
    }
  else
    {
      first_regno = regno;
      last_regno = regno;

      first_bdm_regno = bdm_regmap [regno];
      last_bdm_regno = bdm_regmap [regno];
    }

  if (first_bdm_regno == -1)
    {
      supply_register (first_regno, NULL);
      return;			/* Unsupported register */
    }

  regs = ocd_read_bdm_registers (first_bdm_regno, last_bdm_regno, &reglen);

  for (i = first_regno; i <= last_regno; i++)
    {
      int bdm_regno, regoffset;

      bdm_regno = bdm_regmap [i];
      if (bdm_regno != -1)
	{
	  regoffset = bdm_regno - first_bdm_regno;

	  if (regoffset >= reglen / 4)
	    continue;

	  supply_register (i, regs + 4 * regoffset);
	}
      else
	supply_register (i, NULL); /* Unsupported register */
    }
}

/* Store register REGNO, or all registers if REGNO == -1, from the contents
   of REGISTERS.  FIXME: ignores errors.  */

static void
bdm_ppc_store_registers (regno)
     int regno;
{
  int i;
  int first_regno, last_regno;
  int first_bdm_regno, last_bdm_regno;

  if (regno == -1)
    {
      first_regno = 0;
      last_regno = NUM_REGS - 1;

      first_bdm_regno = 0;
      last_bdm_regno = BDM_NUM_REGS - 1;
    }
  else
    {
      first_regno = regno;
      last_regno = regno;

      first_bdm_regno = bdm_regmap [regno];
      last_bdm_regno = bdm_regmap [regno];
    }

  if (first_bdm_regno == -1)
    return;			/* Unsupported register */

  for (i = first_regno; i <= last_regno; i++)
    {
      int bdm_regno;

      bdm_regno = bdm_regmap [i];

      ocd_write_bdm_registers (bdm_regno, registers + REGISTER_BYTE (i), 4);
    }
}

/* Define the target subroutine names */

struct target_ops bdm_ppc_ops = {
  "ocd",			/* to_shortname */
  "",				/* to_longname */
  "",				/* to_doc */
  bdm_ppc_open,			/* to_open */
  ocd_close,		/* to_close */
  NULL,				/* to_attach */
  ocd_detach,		/* to_detach */
  ocd_resume,		/* to_resume */
  bdm_ppc_wait,			/* to_wait */
  bdm_ppc_fetch_registers,	/* to_fetch_registers */
  bdm_ppc_store_registers,	/* to_store_registers */
  ocd_prepare_to_store,	/* to_prepare_to_store */
  ocd_xfer_memory,		/* to_xfer_memory */
  ocd_files_info,		/* to_files_info */
  memory_insert_breakpoint,	/* to_insert_breakpoint */
  memory_remove_breakpoint,	/* to_remove_breakpoint */
  NULL,				/* to_terminal_init */
  NULL,				/* to_terminal_inferior */
  NULL,				/* to_terminal_ours_for_output */
  NULL,				/* to_terminal_ours */
  NULL,				/* to_terminal_info */
  ocd_kill,			/* to_kill */
  ocd_load,			/* to_load */
  NULL,				/* to_lookup_symbol */
  ocd_create_inferior,	/* to_create_inferior */
  ocd_mourn,		/* to_mourn_inferior */
  0,				/* to_can_run */
  0,				/* to_notice_signals */
  ocd_thread_alive,		/* to_thread_alive */
  ocd_stop,			/* to_stop */
  process_stratum,		/* to_stratum */
  NULL,				/* to_next */
  1,				/* to_has_all_memory */
  1,				/* to_has_memory */
  1,				/* to_has_stack */
  1,				/* to_has_registers */
  1,				/* to_has_execution */
  NULL,				/* sections */
  NULL,				/* sections_end */
  OPS_MAGIC			/* to_magic */
};

void
_initialize_bdm_ppc ()
{
  add_target (&bdm_ppc_ops);
}
