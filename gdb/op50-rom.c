/* Remote target glue for the Oki op50n based eval board.

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

void op50n_open();
void monitor_open();

/*
 * this array of registers need to match the indexes used by GDB. The
 * whole reason this exists is cause the various ROM monitors use
 * different strings than GDB does, and doesn't support all the
 * registers either. So, typing "info reg sp" becomes a "r30".
 */
static char *op50n_regnames[] = {
  "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",
  "r7",  "r8",  "r9",  "r10", "r11", "r12", "r13",
  "r14", "r15", "r16", "r17", "r18", "r19", "r20",
  "r21", "r22", "r23", "r24", "r25", "r26", "r27",
  "r28", "r29", "r30", "r31", "",        "p",       "",
  "",    "",    "",    "",    "",       "",       "",
  "",    "",    "",    "",    "",       "",       "",
  "",    "",    "",    "",    "",       "",       "",    "",
  "",    "",    "",    "",    "",       "",       "",
  "",    "",    "",    "",    "",       "",       "",    "",
  "",    "",    "",    "",    "",       "",       "",    "",
  "",    "",    "",    "",    "",       "",       "",    "",
  "",    "",    "",    "",    "",       "",       "",    "",
  "",    "",    "",    "",    "",       "",       "",    "",
  "",    "",    "",    "",    "",       "",       "",    "",
  "",    "",    "",    "",    "",       "",       "",    "",
  "",    "",    "",    "",    "",       "",       "",    ""
};

/*
 * Define the monitor command strings. Since these are passed directly
 * through to a printf style function, we need can include formatting
 * strings. We also need a CR or LF on the end.
 */

struct target_ops op50n_ops = {
  "op50n",
  "Oki's debug monitor for the Op50n Eval board",

  "Debug on a Oki OP50N eval board.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).",
  op50n_open,
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

struct monitor_ops op50n_cmds = {
  1,					/* 1 for ASCII, 0 for binary */
  "\003.\n",				/* monitor init string */
  "g %x\n",				/* execute or usually GO command */
  "g\n",				/* continue command */
  "t\n",				/* single step */
  "b %x\n",				/* set a breakpoint */
  "bx %x\n",				/* clear a breakpoint */
  0,					/* 0 for number, 1 for address */
  {
    "sx %x %x;.\n",			/* set memory */
    "",					/* delimiter  */
    "",					/* the result */
  },
  {
    "sx %x\n",				/* get memory */
    ": ",				/* delimiter */
    " ",				/* the result */
  },
  {
    "x %s %x\n",			/* set a register */
    "",				        /* delimiter between registers */
    "",					/* the result */
  },
  {
    "x %s\n",				/* get a register */
    "=",				/* delimiter between registers */
    "",					/* the result */
  },
  "r 0\n",				/* download command */
  "#",					/* monitor command prompt */
  " ",					/* end-of-command delimitor */
  ".\n",				/* optional command terminator */
  &op50n_ops,				/* target operations */
  "none,srec,default",			/* load types */
  "none",				/* load types */
  op50n_regnames
};

void
op50n_open(args, from_tty)
     char *args;
     int from_tty;
{
  target_preopen(from_tty);
  push_target(&op50n_ops);
  push_monitor (&op50n_cmds);
  monitor_open (args, "op50n", from_tty);
}

void
_initialize_op50n ()
{
  add_target (&op50n_ops);

  /* this is the default, since it's that's how the board comes up after
     power cycle. It can then be changed using set remotebaud
   */
  baud_rate = 9600;
}
