/* Remote target glue for the WinBond ROM monitor running on the "Cougar"
W89k eval board.

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

void w89k_open();
void monitor_open();

/*
 * this array of registers need to match the indexes used by GDB. The
 * whole reason this exists is cause the various ROM monitors use
 * different strings than GDB does, and doesn't support all the
 * registers either. So, typing "info reg sp" becomes a "r30".
 */
static char *w89k_regnames[] = {
  "r0",   "r1",    "r2",   "r3",   "r4",  "r5",  "r6",  "r7",  "r8",  "r9",
  "r10",  "r11",   "r12",  "r13",  "r14", "r15", "r16", "r17", "r18", "r19",
  "r20",  "r21",   "r22",  "r23",  "r24", "r25", "r26", "r27", "r28", "r29",
  "r30",  "r31",   "sar",  "pc",   "",    "",    "",
  "eiem", "iir",   "iva",  "ior",  "ipsw","",    "",    "",    "",    "",
  "",     "",      "",     "",     "",    "",    "",    "ccr", "",    "",
  "tr0",  "tr1",   "",     "",     "",    "",    "",
  "",     "",      "",     "",     "",    "",    "",    "",
  "",     "",      "",     "",     "",    "",    "",    "",
  "",     "",      "",     "",     "",    "",    "",    "",
  "",     "",      "",     "",     "",    "",    "",    "",
  "",     "",      "",     "",     "",    "",    "",    "",
  "",     "",      "",     "",     "",    "",    "",    "",
  "",     "",      "",     "",     "",    "",    "",    "",
  "",     "",      "",     "",     "",    "",    "",    ""
};

/*
 * Define the monitor command strings. Since these are passed directly
 * through to a printf style function, we need can include formatting
 * strings. We also need a CR or LF on the end.
 */

struct target_ops w89k_ops = {
  "w89k",
  "WinBond's debug monitor for the W89k Eval board",
  "Debug on a WinBond W89K eval board.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).",
  w89k_open,
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
  0,				/* to_stop */
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

struct monitor_ops w89k_cmds = {
  1,					/* 1 for ASCII, 0 for binary */
  "\n",					/* monitor init string */
  "g = %x\n",				/* execute or usually GO command */
  "g\n",				/* continue command */
  "t\n",				/* single step */
  "bp %x\n",				/* set a breakpoint */
  "bc %x\n",				/* clear a breakpoint */
  0,					/* 0 for number, 1 for address */
  {
    "e %x %x\n",			/* set memory */
    "",				        /* delimiter */
    "",					/* the result */
  },
  {
    "db %x %x\n",			/* get memory */
    "",				        /* delimiter */
    "",					/* the result */
  },
  {
    "r %s %x\n",			/* set a register */
    "",				        /* delimiter between registers */
    "",					/* the result */
  },
  {
    "r %s\n",				/* get a register */
    "",				        /* delimiter between registers */
    "",					/* the result */
  },
  "u\n",				/* download command */
  "ROM>",				/* monitor command prompt */
  "",					/* end-of-command delimitor */
  "",					/* optional command terminator */
  &w89k_ops,				/* target operations */
  "none,srec,default",			/* load types */
  "none,xmodem",			/* load protocols */
  "9600",				/* supported baud rates */
  1,					/* number of stop bits */
  w89k_regnames				/* registers names */
};

void
w89k_open(args, from_tty)
     char *args;
     int from_tty;
{
  target_preopen(from_tty);
  push_target  (&w89k_ops);
  push_monitor (&w89k_cmds);
  monitor_open (args, "w89k", from_tty);
}

void
_initialize_w89k ()
{
  add_target (&w89k_ops);

  /* this is the default, since it's the only baud rate supported by the hardware */
  baud_rate = 9600;
}
