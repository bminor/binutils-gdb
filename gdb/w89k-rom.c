#include "defs.h"
#include "gdbcore.h"
#include "target.h"
#include "monitor.h"

void w89k_open();
void general_open();

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
  0,
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

struct monitor_ops w89k_cmds = {
  "\r",				/* monitor init string */
  "G\r",		        /* execute or usually GO command */
  "G\r",			/* continue command */
  "T\r",			/* single step */
  "BP %x\r",				/* set a breakpoint */
  "BC %x\r",				/* clear a breakpoint */
  "E %x %x\r",				/* set memory to a value */
  "D %x\r",				/* display memory */
  "",				/* prompt memory commands use */
  "R %s %x\r",				/* set a register */
  "",				/* delimiter between registers */
  "R %s\r",				/* read a register */
  "L\r",				/* download command */
  "ROM>",				/* monitor command prompt */
  "",					/* end-of-command delimitor */
  "",				/* optional command terminator */
  &w89k_ops				/* target operations */
};

void
w89k_open(args, from_tty)
     char *args;
     int from_tty;
{
  push_target  (&w89k_ops);
  push_monitor (&w89k_cmds);
  general_open (args, "w89k", from_tty);
}

void
_initialize_w89k ()
{
  add_target (&w89k_ops);
}




