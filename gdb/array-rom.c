/* Remote target code for the Array Tech LSI33k based RAID disk controller board.

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

extern int baud_rate;

void array_open();
void monitor_open();

/*
 * this array of registers need to match the indexes used by GDB. The
 * whole reason this exists is cause the various ROM monitors use
 * different strings than GDB does, and doesn't support all the
 * registers either. So, typing "info reg sp" becomes a "r30".
 */
static char *array_regnames[] = REGISTER_NAMES;
extern char *tmp_mips_processor_type;
extern int mips_set_processor_type();

/*
 * Define the monitor command strings. Since these are passed directly
 * through to a printf style function, we need can include formatting
 * strings. We also need a CR or LF on the end.
 */

struct target_ops array_ops = {
  "array",
  "Debug using the standard GDB remote protocol for the Array Tech target.",
  "Debug using the standard GDB remote protocol for the Array Tech target.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).",
  array_open,
  monitor_close, 
  monitor_attach,
  monitor_detach,
  monitor_resume,
  monitor_wait,
  monitor_fetch_registers,
  monitor_store_registers,
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

struct monitor_ops array_cmds = {
  0,					/* 1 for ASCII, 0 for binary */
  "$?#b8+\n",				/* monitor init string */
  "go %x\n",				/* execute or usually GO command */
  "c\n",				/* continue command */
  "s\n",				/* single step */
  "brk 0x%x\n",				/* set a breakpoint */
  "unbrk %x\n",				/* clear a breakpoint */
  0,					/* 0 for number, 1 for address */
  {
    "M%8x,%4x:%8x",			/* set memory */
    "",				        /* delimiter */
    "",					/* the result */
  },
  {
    "m%8x,%4x",				/* get memory */
    "",				        /* delimiter */
    "",					/* the result */
  },
  {
    "G%8x",			        /* set registers */
    "",				        /* delimiter between registers */
    "",					/* the result */
  },
  {
    "g",				/* get registers */
    "",				        /* delimiter between registers */
    "",					/* the result */
  },
  "sload -a tty(0)\r\n",		/* download command */
  ">> ",				/* monitor command prompt */
  "",					/* end-of-command delimitor */
  "",					/* optional command terminator */
  &array_ops,				/* target operations */
  "none,srec,default",			/* load types */
  "none",				/* load protocols */
  "4800",				/* supported baud rates */
  2,					/* number of stop bits */
  array_regnames			/* registers names */
};

/*
 * array_open -- open the Array Tech LSI33k based RAID disk controller.
 */
void
array_open(args, from_tty)
     char *args;
     int from_tty;
{
  tmp_mips_processor_type = "lsi33k";	/* change the default from r3051 */
  mips_set_processor_type_command ("lsi33k", 0);

  baud_rate = 4800;			/* this is the only supported baud rate */

  target_preopen(from_tty);
  push_target  (&array_ops);
  push_monitor (&array_cmds);
  monitor_open (args, "array", from_tty);
}

/*
 * _initialize_array -- do any special init stuff for the target.
 */
void
_initialize_array ()
{
  add_target (&array_ops);
}




