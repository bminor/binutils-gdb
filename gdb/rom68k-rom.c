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
#include "serial.h"

void rom68k_open();

/*
 * this array of registers need to match the indexes used by GDB. The
 * whole reason this exists is cause the various ROM monitors use
 * different strings than GDB does, and doesn't support all the
 * registers either. So, typing "info reg sp" becomes a "r30".
 */
static char *rom68k_regnames[NUM_REGS] = {
  "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7",
  "A0", "A1", "A2", "A3", "A4", "A5", "A6", "ISP",
  "SR", "PC" };

/*
 * Define the monitor command strings. Since these are passed directly
 * through to a printf style function, we need can include formatting
 * strings. We also need a CR or LF on the end.
 */

static struct target_ops rom68k_ops;

static char *rom68k_loadtypes[] = {"none", "srec", "default", NULL};
static char *rom68k_loadprotos[] = {"none", NULL};

static struct monitor_ops rom68k_cmds =
{
  1,				/* 1 for ASCII, 0 for binary */
  ".\r\r",			/* monitor init string */
  "go \r",			/* execute or usually GO command */
  "go \r",			/* continue command */
  "st \r",			/* single step */
  "db %x\r",			/* set a breakpoint */
  "cb %x\r",			/* clear a breakpoint */
  0,				/* 0 for number, 1 for address */
  {
    "pm %x %x\r",		/* setmem.cmd (addr, value) */
    NULL,			/* setreg.resp_delim */
    NULL,			/* setreg.term */
    NULL,			/* setreg.term_cmd */
  },
  {
    "dm %x 1\r",		/* getmem.cmd (addr) */
    "  ",			/* getmem.resp_delim */
    NULL,			/* getmem.term */
    NULL,			/* getmem.term_cmd */
  },
  {
    "pr %s %x\r",		/* setreg.cmd (name, value) */
    NULL,			/* setreg.resp_delim */
    NULL,			/* setreg.term */
    NULL			/* setreg.term_cmd */
  },
  {
    "pr %s\r",			/* getreg.cmd (name) */
    ":  ",			/* getreg.resp_delim */
    "= ",			/* getreg.term */
    ".\r"			/* getreg.term_cmd */
  },
  "dc\r",			/* download command */
  "ROM68K :->",			/* monitor command prompt */
  "=",				/* end-of-command delimitor */
  ".\r",			/* optional command terminator */
  &rom68k_ops,			/* target operations */
  rom68k_loadtypes,		/* loadtypes */
  rom68k_loadprotos,		/* loadprotos */
  "9600",			/* supported baud rates */
  SERIAL_1_STOPBITS,		/* number of stop bits */
  rom68k_regnames		/* registers names */
  };

void
rom68k_open(args, from_tty)
     char *args;
     int from_tty;
{
  monitor_open (args, &rom68k_cmds, from_tty);
}

void
_initialize_rom68k ()
{
  init_monitor_ops (&rom68k_ops);

  rom68k_ops.to_shortname = "rom68k";
  rom68k_ops.to_longname = "Rom68k debug monitor for the IDP Eval board";
  rom68k_ops.to_doc = "Debug on a Motorola IDP eval board running the ROM68K monitor.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).";
  rom68k_ops.to_open = rom68k_open;

  add_target (&rom68k_ops);

  /* this is the default, since it's the only baud rate supported by the hardware */
  baud_rate = 9600;
}
