#include "defs.h"
#include "gdbcore.h"
#include "target.h"
#include "monitor.h"

void op50n_open();
void general_open();

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

struct monitor_ops op50n_cmds = {
  "\r",				/* monitor init string */
  "g\r",		        /* execute or usually GO command */
  "g\r",			/* continue command */
  "t\r",			/* single step */
  "b %x\r",				/* set a breakpoint */
  "b %x\r",				/* clear a breakpoint */
  "s %x %x\r",				/* set memory to a value */
  "s %x\r",				/* display memory */
  "",				/* prompt memory commands use */
  "x %s %x\r",				/* set a register */
  "",				/* delimiter between registers */
  "x %s\r",				/* read a register */
  "r\r",				/* download command */
  "#",				/* monitor command prompt */
  "",					/* end-of-command delimitor */
  ".",				/* optional command terminator */
  &op50n_ops				/* target operations */
};

void
op50n_open(args, from_tty)
     char *args;
     int from_tty;
{
  push_target(&op50n_ops);
  push_monitor (&op50n_cmds);
  general_open (args, "op50n", from_tty);
}

void
_initialize_op50n ()
{
  add_target (&op50n_ops);
}
