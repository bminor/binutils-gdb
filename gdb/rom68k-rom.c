/* Remote target glue for the rom68k ROM monitor. This was running on a
Motorola IDP board.

   Copyright 1988, 1991, 1992, 1993, 1994 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
#include "gdbcore.h"
#include "target.h"
#include "monitor.h"

extern int baud_rate;

void rom68k_open();
void monitor_open();

/*
 * this array of registers need to match the indexes used by GDB. The
 * whole reason this exists is cause the various ROM monitors use
 * different strings than GDB does, and doesn't support all the
 * registers either. So, typing "info reg sp" becomes a "r30".
 */
static char *rom68k_regnames[] = {
  "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "a0", "a1",
  "a2", "a3", "a4", "a5", "a6", "usp", "ssp","pc",   "",   "",
  "",   "",   "",   "",   "",   "",   "",   "",   "",   "",
  "",   "",   "",   "",   "",   "",    "",
  "",   "",   "",   "",   "",   "",    "",    "",  "",  "",
  "",   "",   "",   "",   "",   "",    "",    "",  "",  "",
  "",   "",   "",   "",   "",   "",    "",
  "",   "",   "",   "",   "",   "",    "",    "",
  "",   "",   "",   "",   "",   "",    "",    "",
  "",   "",   "",   "",   "",   "",    "",    "",
  "",   "",   "",   "",   "",   "",    "",    "",
  "",   "",   "",   "",   "",   "",    "",    "",
  "",   "",   "",   "",   "",   "",    "",    "",
  "",   "",   "",   "",   "",   "",    "",    "",
  "",   "",   "",   "",   "",   "",    "",    ""
};

/*
 * Define the monitor command strings. Since these are passed directly
 * through to a printf style function, we need can include formatting
 * strings. We also need a CR or LF on the end.
 */

struct target_ops rom68k_ops = {
  "rom68k",
  "WinBond's debug monitor for the Rom68k Eval board",
  "Debug on a Motorola IDP eval board running the ROM68K monitor.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).",
  rom68k_open,
  monitor_close, 
  monitor_attach,
  monitor_detach,
  monitor_resume,
  monitor_wait,
  monitor_fetch_register,
  monitor_store_register,
  monitor_prepare_to_store,
  monitor_xfer_inferior_memory,
  monitor_files_info,
  monitor_insert_breakpoint,
  monitor_remove_breakpoint,	/* Breakpoints */
  0,
  0,
  0,
  0,
  0,				/* Terminal handling */
  monitor_kill,
  monitor_load,			/* load */
  0,				/* lookup_symbol */
  monitor_create_inferior,
  monitor_mourn_inferior,
  0,				/* can_run */
  0, 				/* notice_signals */
  process_stratum,
  0,				/* next */
  1,
  1,
  1,
  1,
  1,				/* all mem, mem, stack, regs, exec */
  0,
  0,				/* Section pointers */
  OPS_MAGIC,			/* Always the last thing */
};

struct monitor_ops rom68k_cmds = {
  1,					/* 1 for ASCII, 0 for binary */
  "\n",					/* monitor init string */
  "go \n",				/* execute or usually GO command */
  "go \n",				/* continue command */
  "st \n",				/* single step */
  "db %x\n",				/* set a breakpoint */
  "cb %x\r",				/* clear a breakpoint */
  0,					/* 0 for number, 1 for address */
  {
    "pm %x %x\r",			/* set memory */
    "=",				/* delimiter */
    "",					/* the result */
  },
  {
    "dm %x 1\r",			/* get memory */
    "",				        /* delimiter */
    "",					/* the result */
  },
  {
    "pr %s %x\r",			/* set a register */
    "",				        /* delimiter between registers */
    "",					/* the result */
  },
  {
    "pr %s\n",				/* get a register */
    ":",				/* delimiter between registers */
    "",					/* the result */
  },
  "dc\n",				/* download command */
  "ROM68K :->",				/* monitor command prompt */
  "=",					/* end-of-command delimitor */
  ".\n",				/* optional command terminator */
  &rom68k_ops,				/* target operations */
  "srec,xmodem-ascii,xmodem-srec,default",/* load types */
  rom68k_regnames			/* registers names */
};

void
rom68k_open(args, from_tty)
     char *args;
     int from_tty;
{
  target_preopen(from_tty);
  push_target  (&rom68k_ops);
  push_monitor (&rom68k_cmds);
  monitor_open (args, "rom68k", from_tty);
}

void
_initialize_rom68k ()
{
  add_target (&rom68k_ops);

  /* this is the default, since it's the only baud rate supported by the hardware */
  baud_rate = 9600;
}




