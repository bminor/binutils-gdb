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
#include "serial.h"

static void w89k_open PARAMS ((char *args, int from_tty));

/*
 * this array of registers need to match the indexes used by GDB. The
 * whole reason this exists is cause the various ROM monitors use
 * different strings than GDB does, and doesn't support all the
 * registers either. So, typing "info reg sp" becomes a "r30".
 */
static char *w89k_regnames[NUM_REGS] =
{
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
  "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
  "SAR", "PC", NULL, NULL, NULL, "EIEM", "IIR", "IVA",
  "IOR", "IPSW", NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  "CCR", NULL, NULL, "TR0", "TR1",
};

/*
 * Define the monitor command strings. Since these are passed directly
 * through to a printf style function, we need can include formatting
 * strings. We also need a CR or LF on the end.
 */

static struct target_ops w89k_ops;

static char *w89k_loadtypes[] = {"none", "srec", "default", NULL};
static char *w89k_loadprotos[] = {"none", "xmodem", NULL};

static char *w89k_inits[] = {"\r", NULL};

static struct monitor_ops w89k_cmds =
{
  MO_GETMEM_NEEDS_RANGE,	/* flags */
  w89k_inits,			/* Init strings */
  "g\r",			/* continue command */
  "t\r",			/* single step */
  NULL,				/* Interrupt char */
  "bp %x\r",			/* set a breakpoint */
  "bc %x\r",			/* clear a breakpoint */
  NULL,				/* clear all breakpoints */
  NULL,				/* memory fill cmd */
  {
    "eb %x %x\r",		/* setmem.cmdb (addr, value) */
    "eh %x %x\r",		/* setmem.cmdw (addr, value) */
    "ew %x %x\r",		/* setmem.cmdl (addr, value) */
    NULL,			/* setmem.cmdll (addr, value) */
    NULL,			/* setreg.resp_delim */
    NULL,			/* setreg.term */
    NULL,			/* setreg.term_cmd */
  },
  {
    "db %x %x\r",		/* getmem.cmdb (startaddr, endaddr) */
    "dh %x %x\r",		/* getmem.cmdw (startaddr, endaddr) */
    "dw %x %x\r",		/* getmem.cmdl (startaddr, endaddr) */
    NULL,			/* getmem.cmdll (startaddr, endaddr) */
    "  ",			/* getmem.resp_delim */
    NULL,			/* getmem.term */
    NULL,			/* getmem.term_cmd */
  },
  {
    "r %s %x\r",		/* setreg.cmd (name, value) */
    NULL,			/* setreg.resp_delim */
    NULL,			/* setreg.term */
    NULL,			/* setreg.term_cmd */
  },
  {
    "r %s\r",			/* getreg.cmd (name) */
    "\r",			/* getreg.resp_delim */
    NULL,			/* getreg.term */
    NULL,			/* getreg.term_cmd */
  },
  NULL,				/* dump_registers */
  NULL,				/* register_pattern */
  NULL,				/* supply_register */
  "u\r",			/* download command */
  NULL,				/* load response */
  "ROM>",			/* monitor command prompt */
  NULL,				/* end-of-command delimitor */
  NULL,				/* optional command terminator */
  &w89k_ops,			/* target operations */
  w89k_loadtypes,		/* loadtypes */
  w89k_loadprotos,		/* loadprotos */
  "9600",			/* supported baud rates */
  SERIAL_1_STOPBITS,		/* number of stop bits */
  w89k_regnames,		/* register names */
  MONITOR_OPS_MAGIC		/* magic */
  };

void
w89k_open(args, from_tty)
     char *args;
     int from_tty;
{
  monitor_open (args, &w89k_cmds, from_tty);
}

void
_initialize_w89k ()
{
  init_monitor_ops (&w89k_ops);

  w89k_ops.to_shortname = "w89k";
  w89k_ops.to_longname = "WinBond's debug monitor for the W89k Eval board";
  w89k_ops.to_doc = "Debug on a WinBond W89K eval board.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).";
  w89k_ops.to_open = w89k_open;

  add_target (&w89k_ops);
}
