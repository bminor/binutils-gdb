/* Remote target glue for the Oki op50n based eval board.

   Copyright 1995 Free Software Foundation, Inc.

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
#include "serial.h"

static void op50n_open PARAMS ((char *args, int from_tty));

/*
 * this array of registers need to match the indexes used by GDB. The
 * whole reason this exists is cause the various ROM monitors use
 * different strings than GDB does, and doesn't support all the
 * registers either. So, typing "info reg sp" becomes a "r30".
 */

static char *op50n_regnames[NUM_REGS] =
{
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
  "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
  "cr11", "p", NULL, NULL, NULL, "cr15", "cr19", "cr20",
  "cr21", "cr22", NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, "cr0", "cr8", "cr9", "cr10","cr12",
  "cr13", "cr24", "cr25", "cr26",
};

/*
 * Define the monitor command strings. Since these are passed directly
 * through to a printf style function, we need can include formatting
 * strings. We also need a CR or LF on the end.
 */

static struct target_ops op50n_ops;

static char *op50n_inits[] = {"\003.\n", NULL};

static struct monitor_ops op50n_cmds =
{
  0,				/* flags */
  op50n_inits,			/* Init strings */
  "g\n",			/* continue command */
  "t\n",			/* single step */
  NULL,				/* Interrupt char */
  "b %x\n",			/* set a breakpoint */
  "bx %x\n",			/* clear a breakpoint */
  NULL,				/* clear all breakpoints */
  NULL,				/* memory fill cmd */
  {
    "sx %x %x;.\n",		/* setmem.cmdb (addr, value) */
    NULL,			/* setmem.cmdw (addr, value) */
    NULL,			/* setmem.cmdl (addr, value) */
    NULL,			/* setmem.cmdll (addr, value) */
    NULL,			/* setreg.resp_delim */
    NULL,			/* setreg.term */
    NULL,			/* setreg.term_cmd */
  },
  {
    "sx %x\n",			/* getmem.cmdb (addr, value) */
    NULL,			/* getmem.cmdw (addr, value) */
    NULL,			/* getmem.cmdl (addr, value) */
    NULL,			/* getmem.cmdll (addr, value) */
    ": ",			/* getmem.resp_delim */
    NULL,			/* getmem.term */
    NULL,			/* getmem.term_cmd */
  },
  {
    "x %s %x\n",		/* setreg.cmd (name, value) */
    NULL,			/* setreg.resp_delim */
    NULL,			/* setreg.term */
    NULL,			/* setreg.term_cmd */
  },
  {
    "x %s\n",			/* getreg.cmd (name) */
    "=",			/* getreg.resp_delim */
    NULL,			/* getreg.term */
    NULL,			/* getreg.term_cmd */
  },
  NULL,				/* dump_registers */
  NULL,				/* register_pattern */
  NULL,				/* supply_register */
  NULL,				/* load routine */
  "r 0\n",			/* download command */
  NULL,				/* load response */
  "#",				/* monitor command prompt */
  NULL,				/* end-of-command delimitor */
  NULL,				/* optional command terminator */
  &op50n_ops,			/* target operations */
  SERIAL_1_STOPBITS,		/* number of stop bits */
  op50n_regnames,		/* register names */
  MONITOR_OPS_MAGIC		/* magic */
  };

static void
op50n_open (args, from_tty)
     char *args;
     int from_tty;
{
  monitor_open (args, &op50n_cmds, from_tty);
}

void
_initialize_op50n ()
{
  init_monitor_ops (&op50n_ops);

  op50n_ops.to_shortname = "op50n";
  op50n_ops.to_longname = "Oki's debug monitor for the Op50n Eval board";
  op50n_ops.to_doc = "Debug on a Oki OP50N eval board.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).";
  op50n_ops.to_open = op50n_open;

  add_target (&op50n_ops);
}
