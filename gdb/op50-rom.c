/*
 * op50n-rom.c -- configure GDB for an Oki op59n eval board.
 */
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
  "r0_zero", "r1_tO ",  "r2_rp ",  "r3_s15 ", "r4_s14 ", "r5_s13 ", "r6_s12 ",
  "r7_s11 ", "r8_s10 ", "r9_s9 ",  "r10_s8 ", "r11_s7 ", "r12_s6 ", "r13_s5 ",
  "r14_s4 ", "r15_s3 ", "r16_s2 ", "r17_s1 ", "r18_s0 ", "r19_t4 ", "r20_t3 ",
  "r21_t2 ", "r22_t1 ", "r23_a3 ", "r24_a2 ", "r25_a1 ", "r26_a0 ", "r27_dp ",
  "r28_v0 ", "r29_v1 ", "r30_sp ", "r31_t5 ", "",        "p",       "",
  "",        "",       "",       "",       "",       "",       "",
  "",        "",       "",       "",       "",       "",       "",
  "",        "",       "",       "",       "",       "",       "",    "",
  "",        "",       "",       "",       "",       "",       "",
  "",        "",       "",       "",       "",       "",       "",    "",
  "",        "",       "",       "",       "",       "",       "",    "",
  "",        "",       "",       "",       "",       "",       "",    "",
  "",        "",       "",       "",       "",       "",       "",    "",
  "",        "",       "",       "",       "",       "",       "",    "",
  "",        "",       "",       "",       "",       "",       "",    "",
  "",        "",       "",       "",       "",       "",       "",    "",
  "",        "",       "",       "",       "",       "",       "",    ""
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
  1,					/* 1 for ASCII, 0 for binary */
  ".\n",				/* monitor init string */
  "g %x",		        /* execute or usually GO command */
  "",				/* continue command */
  "",				/* single step */
  "b %x\n",				/* set a breakpoint */
  "",				/* clear a breakpoint */
  1,					/* 0 for number, 1 for address */
  {
    "sx %x %x;.\n",			/* set memory */
    "",				/* delimiter  */
    "",					/* the result */
  },
  {
    "sx %x\n",				/* get memory */
    ": ",				/* delimiter */
    " ",				/* the result */
  },
  {
    "x %s %x\n",			/* set a register */
    "=",				/* delimiter between registers */
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
  "srec,binary",			/* load types */
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
