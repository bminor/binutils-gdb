/* Memory-access and commands for inferior process, for GDB.
   Copyright (C) 1986, 1987, 1988 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

#include "defs.h"
#include "initialize.h"
#include "symtab.h"
#include "param.h"
#include "frame.h"
#include "inferior.h"
#include "environ.h"
#include "value.h"

#include <stdio.h>
#include <signal.h>
#include <sys/param.h>

extern char *sys_siglist[];

#define ERROR_NO_INFERIOR \
   if (inferior_pid == 0) error ("The program is not being run.");

/* String containing arguments to give to the program,
   with a space added at the front.  Just a space means no args.  */

static char *inferior_args;

/* File name for default use for standard in/out in the inferior.  */

char *inferior_io_terminal;

/* Pid of our debugged inferior, or 0 if no inferior now.  */

int inferior_pid;

/* Last signal that the inferior received (why it stopped).  */

int stop_signal;

/* Address at which inferior stopped.  */

CORE_ADDR stop_pc;

/* Stack frame when program stopped.  */

FRAME stop_frame;

/* Number of breakpoint it stopped at, or 0 if none.  */

int stop_breakpoint;

/* Nonzero if stopped due to a step command.  */

int stop_step;

/* Nonzero if stopped due to completion of a stack dummy routine.  */

int stop_stack_dummy;

/* Range to single step within.
   If this is nonzero, respond to a single-step signal
   by continuing to step if the pc is in this range.  */

CORE_ADDR step_range_start; /* Inclusive */
CORE_ADDR step_range_end; /* Exclusive */

/* Stack frame address as of when stepping command was issued.
   This is how we know when we step into a subroutine call,
   and how to set the frame for the breakpoint used to step out.  */

CORE_ADDR step_frame;

/* 1 means step over all subroutine calls.
   -1 means step over calls to undebuggable functions.  */

int step_over_calls;

/* If stepping, nonzero means step count is > 1
   so don't print frame next time inferior stops
   if it stops due to stepping.  */

int step_multi;

/* Environment to use for running inferior,
   in format described in environ.h.  */

struct environ *inferior_environ;

CORE_ADDR read_pc ();
struct command_line *get_breakpoint_commands ();

START_FILE

int
have_inferior_p ()
{
  return inferior_pid != 0;
}

static void 
set_args_command (args)
     char *args;
{
  free (inferior_args);
  if (!args) args = "";
  inferior_args = concat (" ", args, "");
}

void
tty_command (file)
     char *file;
{
  if (file == 0)
    error_no_arg ("terminal name for running target process");

  inferior_io_terminal = savestring (file, strlen (file));
}

static void
run_command (args, from_tty)
     char *args;
     int from_tty;
{
  extern char **environ;
  register int i;
  char *exec_file;
  char *allargs;

  extern int sys_nerr;
  extern char *sys_errlist[];
  extern int errno;

  dont_repeat ();

  if (inferior_pid)
    {
      if (query ("The program being debugged has been started already.\n\
Start it from the beginning? "))
	kill_inferior ();
      else
	error ("Program already started.");
    }

  if (remote_debugging)
    {
      free (allargs);
      if (from_tty)
	{
	  printf ("Starting program: %s%s\n",
		  exec_file, inferior_args);
	  fflush (stdout);
	}
    }
  else
    {
      if (args)
	set_args_command (args);

      exec_file = (char *) get_exec_file (1);
      if (from_tty)
	{
	  printf ("Starting program: %s%s\n",
		  exec_file, inferior_args);
	  fflush (stdout);
	}

      allargs = concat ("exec ", exec_file, inferior_args);
      inferior_pid = create_inferior (allargs, environ_vector (inferior_environ));
    }

  clear_proceed_status ();

  start_inferior ();
}

void
cont_command (proc_count_exp, from_tty)
     char *proc_count_exp;
     int from_tty;
{
  ERROR_NO_INFERIOR;

  clear_proceed_status ();

  /* If have argument, set proceed count of breakpoint we stopped at.  */

  if (stop_breakpoint && proc_count_exp)
    {
      set_ignore_count (stop_breakpoint,
			parse_and_eval_address (proc_count_exp) - 1,
			from_tty);
      if (from_tty)
	printf ("  ");
    }

  if (from_tty)
    printf ("Continuing.\n");

  proceed (-1, -1, 0);
}

/* Step until outside of current statement.  */
static void step_1 ();

static void
step_command (count_string)
{
  step_1 (0, 0, count_string);
}

/* Likewise, but skip over subroutine calls as if single instructions.  */

static void
next_command (count_string)
{
  step_1 (1, 0, count_string);
}

/* Likewise, but step only one instruction.  */

static void
stepi_command (count_string)
{
  step_1 (0, 1, count_string);
}

static void
nexti_command (count_string)
{
  step_1 (1, 1, count_string);
}

static void
step_1 (skip_subroutines, single_inst, count_string)
     int skip_subroutines;
     int single_inst;
     char *count_string;
{
  register int count = 1;

  ERROR_NO_INFERIOR;
  count = count_string ? parse_and_eval_address (count_string) : 1;

  for (; count > 0; count--)
    {
      clear_proceed_status ();

      step_frame = get_current_frame ();

      if (! single_inst)
	{
	  find_pc_line_pc_range (stop_pc, &step_range_start, &step_range_end);
	  if (step_range_end == 0)
	    {
	      terminal_ours ();
	      error ("Current function has no line number information.");
	    }
	}
      else
	{
	  /* Say we are stepping, but stop after one insn whatever it does.
	     Don't step through subroutine calls even to undebuggable functions.  */
	  step_range_start = step_range_end = 1;
	  if (!skip_subroutines)
	    step_over_calls = 0;
	}

      if (skip_subroutines)
	step_over_calls = 1;

      step_multi = (count > 1);
      proceed (-1, -1, 1);
      if (! stop_step)
	break;
    }
}

/* Continue program at specified address.  */

static void
jump_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  register CORE_ADDR addr;
  struct symtab_and_line sal;

  ERROR_NO_INFERIOR;

  if (!arg)
    error_no_arg ("starting address");

  sal = decode_line_spec (arg, 1);

  if (sal.symtab == 0 && sal.pc == 0)
    error ("No source file has been specified.");

  if (sal.pc == 0)
    sal.pc = find_line_pc (sal.symtab, sal.line);

  {
    struct symbol *fn = get_frame_function (get_current_frame ());
    struct symbol *sfn = find_pc_function (sal.pc);
    if (fn != 0 && sfn != fn
	&& ! query ("Line %d is not in `%s'.  Jump anyway? ",
		    sal.line, SYMBOL_NAME (fn)))
      error ("Not confirmed.");
  }

  if (sal.pc == 0)
    error ("No line %d in file \"%s\".", sal.line, sal.symtab->filename);

  addr = sal.pc;

  clear_proceed_status ();

  if (from_tty)
    printf ("Continuing at 0x%x.\n", addr);

  proceed (addr, 0, 0);
}

/* Continue program giving it specified signal.  */

static void
signal_command (signum_exp, from_tty)
     char *signum_exp;
     int from_tty;
{
  register int signum;

  dont_repeat ();		/* Too dangerous.  */
  ERROR_NO_INFERIOR;

  if (!signum_exp)
    error_no_arg ("signal number");

  signum = parse_and_eval_address (signum_exp);

  clear_proceed_status ();

  if (from_tty)
    printf ("Continuing with signal %d.\n", signum);

  proceed (stop_pc, signum, 0);
}

/* Execute a "stack dummy", a piece of code stored in the stack
   by the debugger to be executed in the inferior.

   To call: first, do PUSH_DUMMY_FRAME.
   Then push the contents of the dummy.  It should end with a breakpoint insn.
   Then call here, passing address at which to start the dummy.

   The contents of all registers are saved before the dummy frame is popped
   and copied into the buffer BUFFER.

   The dummy's frame is automatically popped whenever that break is hit.
   If that is the first time the program stops, run_stack_dummy
   returns to its caller with that frame already gone.
   Otherwise, the caller never gets returned to.  */

/* 4 => return instead of letting the stack dummy run.  */

static int stack_dummy_testing = 0;

void
run_stack_dummy (addr, buffer)
     CORE_ADDR addr;
     REGISTER_TYPE *buffer;
{
  int saved_pc_changed = pc_changed;
  int saved_stop_signal = stop_signal;
  int saved_stop_pc = stop_pc;
  int saved_stop_frame = stop_frame;
  int saved_stop_breakpoint = stop_breakpoint;
  int saved_stop_step = stop_step;
  int saved_stop_stack_dummy = stop_stack_dummy;
  FRAME saved_selected_frame;
  int saved_selected_level;
  struct command_line *saved_breakpoint_commands
    = get_breakpoint_commands ();

  record_selected_frame (&saved_selected_frame, &saved_selected_level);

  /* Now proceed, having reached the desired place.  */
  clear_proceed_status ();
  if (stack_dummy_testing & 4)
    {
      POP_FRAME;
      return;
    }
  proceed (addr, 0, 0);

  if (!stop_stack_dummy)
    error ("Cannot continue previously requested operation.");

  set_breakpoint_commands (saved_breakpoint_commands);
  select_frame (saved_selected_frame, saved_selected_level);
  stop_signal = saved_stop_signal;
  stop_pc = saved_stop_pc;
  stop_frame = saved_stop_frame;
  stop_breakpoint = saved_stop_breakpoint;
  stop_step = saved_stop_step;
  stop_stack_dummy = saved_stop_stack_dummy;
  pc_changed = saved_pc_changed;

  /* On return, the stack dummy has been popped already.  */

  bcopy (stop_registers, buffer, sizeof stop_registers);
}

/* "finish": Set a temporary breakpoint at the place
   the selected frame will return to, then continue.  */

static void
finish_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  struct symtab_and_line sal;
  register FRAME frame;
  struct frame_info fi;

  register struct symbol *function;

  if (!have_inferior_p ())
    error ("The program is not being run.");
  if (arg)
    error ("The \"finish\" command does not take any arguments.");

  frame = get_prev_frame (selected_frame);
  if (frame == 0)
    error ("\"finish\" not meaningful in the outermost frame.");

  clear_proceed_status ();

  fi = get_frame_info (frame);
  sal = find_pc_line (fi.pc, 0);
  sal.pc = fi.pc;
  set_momentary_breakpoint (sal, frame);

  /* Find the function we will return from.  */

  fi = get_frame_info (fi.next_frame);
  function = find_pc_function (fi.pc);

  if (from_tty)
    {
      printf ("Run till exit from ");
      print_selected_frame ();
    }

  proceed (-1, -1, 0);

  if (stop_breakpoint == -3 && function != 0)
    {
      struct type *value_type;
      register value val;

      value_type = TYPE_TARGET_TYPE (SYMBOL_TYPE (function));
      if (TYPE_CODE (value_type) == TYPE_CODE_VOID)
	return;

      val = value_being_returned (value_type, stop_registers);
      printf ("Value returned is $%d = ", record_latest_value (val));
      value_print (val, stdout, 0);
      putchar ('\n');
    }
}

static void
program_info ()
{
  if (inferior_pid == 0)
    {
      printf ("The program being debugged is not being run.\n");
      return;
    }

  printf ("Program being debugged is in process %d, stopped at 0x%x.\n",
	  inferior_pid, stop_pc);
  if (stop_step)
    printf ("It stopped after being stepped.\n");
  else if (stop_breakpoint)
    printf ("It stopped at breakpoint %d.\n", stop_breakpoint);
  else if (stop_signal)
    printf ("It stopped with signal %d (%s).\n",
	    stop_signal, sys_siglist[stop_signal]);

  printf ("\nType \"info stack\" or \"info reg\" for more information.\n");
}

static void
environment_info (var)
     char *var;
{
  if (var)
    {
      register char *val = get_in_environ (inferior_environ, var);
      if (val)
	printf ("%s = %s\n", var, val);
      else
	printf ("Environment variable \"%s\" not defined.\n", var);
    }
  else
    {
      register char **vector = environ_vector (inferior_environ);
      while (*vector)
	printf ("%s\n", *vector++);
    }
}

static void
set_environment_command (arg)
     char *arg;
{
  register char *p, *val, *var;

  if (arg == 0)
    error_no_arg ("environment variable and value");

  p = (char *) index (arg, '=');
  val = (char *) index (arg, ' ');
  if (p != 0 && val != 0)
    p = arg + min (p - arg, val - arg);
  else if (val != 0 && p == 0)
    p = val;

  if (p == 0)
    error ("Space or \"=\" must separate variable name and its value");
  if (p[1] == 0)
    error_no_arg ("value for the variable");
  if (p == arg)
    error_no_arg ("environment variable to set");

  val = p + 1;
  while (*val == ' ' || *val == '\t') val++;
  while (p != arg && (p[-1] == ' ' || p[-1] == '\t')) p--;

  var = savestring (arg, p - arg);
  set_in_environ (inferior_environ, var, val);
  free (var);
}

static void
unset_environment_command (var)
     char *var;
{
  if (var == 0)
    error_no_arg ("environment variable");

  unset_in_environ (inferior_environ, var);
}

/* Read an integer from debugged memory, given address and number of bytes.  */

read_memory_integer (memaddr, len)
     CORE_ADDR memaddr;
     int len;
{
  char cbuf;
  short sbuf;
  int ibuf;
  long lbuf;

  if (len == sizeof (char))
    {
      read_memory (memaddr, &cbuf, len);
      return cbuf;
    }
  if (len == sizeof (short))
    {
      read_memory (memaddr, &sbuf, len);
      return sbuf;
    }
  if (len == sizeof (int))
    {
      read_memory (memaddr, &ibuf, len);
      return ibuf;
    }
  if (len == sizeof (lbuf))
    {
      read_memory (memaddr, &lbuf, len);
      return lbuf;
    }
  error ("Cannot handle integers of %d bytes.", len);
}

CORE_ADDR
read_pc ()
{
  return (CORE_ADDR) read_register (PC_REGNUM);
}

write_pc (val)
     CORE_ADDR val;
{
  write_register (PC_REGNUM, (long) val);
}

char *reg_names[] = REGISTER_NAMES;

static void
registers_info (addr_exp)
     char *addr_exp;
{
  register int i;
  int regnum;

  if (addr_exp)
    {
      if (*addr_exp >= '0' && *addr_exp <= '9')
	regnum = atoi (addr_exp);
      else
	{
	  register char *p = addr_exp;
	  if (p[0] == '$')
	    p++;
	  for (regnum = 0; regnum < NUM_REGS; regnum++)
	    if (!strcmp (p, reg_names[regnum]))
	      break;
	  if (regnum == NUM_REGS)
	    error ("%s: invalid register name.", addr_exp);
	}
    }
  else
    printf ("Reg\tContents\n\n");

  for (i = 0; i < NUM_REGS; i++)
    {
      unsigned char raw_buffer[MAX_REGISTER_RAW_SIZE];
      unsigned char virtual_buffer[MAX_REGISTER_VIRTUAL_SIZE];
      REGISTER_TYPE val;

      if (addr_exp != 0 && i != regnum)
	continue;

      /* On machines with lots of registers, pause every 16 lines
	 so user can read the output.  */
      if (addr_exp == 0 && i > 0 && i % 16 == 0)
	{
	  printf ("--Type Return to print more--");
	  fflush (stdout);
	  read_line ();
	}

      /* Get the data in raw format, then convert also to virtual format.  */
      read_relative_register_raw_bytes (i, raw_buffer);
      REGISTER_CONVERT_TO_VIRTUAL (i, raw_buffer, virtual_buffer);

      printf ("%s\t", reg_names[i]);

      /* If virtual format is floating, print it that way.  */
      if (TYPE_CODE (REGISTER_VIRTUAL_TYPE (i)) == TYPE_CODE_FLT
	  && ! INVALID_FLOAT (virtual_buffer, REGISTER_VIRTUAL_SIZE (i)))
	val_print (REGISTER_VIRTUAL_TYPE (i), virtual_buffer, 0, stdout, 0);
      /* Else if virtual format is too long for printf,
	 print in hex a byte at a time.  */
      else if (REGISTER_VIRTUAL_SIZE (i) > sizeof (long))
	{
	  register int j;
	  printf ("0x");
	  for (j = 0; j < REGISTER_VIRTUAL_SIZE (i); j++)
	    printf ("%02x", virtual_buffer[j]);
	}
      /* Else print as integer in hex and in decimal.  */
      else
	{
	  long val;

	  bcopy (virtual_buffer, &val, sizeof (long));
	  if (val == 0)
	    printf ("0");
	  else
	    printf ("0x%08x  %d", val, val);
	}

      /* If register has different raw and virtual formats,
	 print the raw format in hex now.  */

      if (REGISTER_CONVERTIBLE (i))
	{
	  register int j;

	  printf ("  (raw 0x");
	  for (j = 0; j < REGISTER_RAW_SIZE (i); j++)
	    printf ("%02x", raw_buffer[j]);
	  printf (")");
	}
      printf ("\n");
    }

  printf ("Contents are relative to selected stack frame.\n");
}

#ifdef ATTACH_DETACH
/*
 * TODO:
 * Should save/restore the tty state since it might be that the
 * program to be debugged was started on this tty and it wants
 * the tty in some state other than what we want.  If it's running
 * on another terminal or without a terminal, then saving and
 * restoring the tty state is a harmless no-op.
 */

/*
 * attach_command --
 * takes a program started up outside of gdb and ``attaches'' to it.
 * This stops it cold in its tracks and allows us to start tracing it.
 * For this to work, we must be able to send the process a
 * signal and we must have the same effective uid as the program.
 */
static void
attach_command (args, from_tty)
     char *args;
     int from_tty;
{
  char *exec_file;
  int pid;
  int remote = 0;

  dont_repeat();

  if (!args)
    error_no_arg ("process-id to attach");

  while (*args == ' ' || *args == '\t') args++;

  if (args[0] == '/')
    remote = 1;
  else
    pid = atoi (args);

  if (inferior_pid)
    {
      if (query ("A program is being debugged already.  Kill it? "))
	kill_inferior ();
      else
	error ("Inferior not killed.");
    }

  exec_file = (char *) get_exec_file (1);

  if (from_tty)
    {
      if (remote)
	printf ("Attaching remote machine\n");
      else
	printf ("Attaching program: %s pid %d\n",
		exec_file, pid);
      fflush (stdout);
    }

  if (remote)
    {
      remote_open (args, from_tty);
      start_remote ();
    }
  else
    attach_program (pid);
}

/*
 * detach_command --
 * takes a program previously attached to and detaches it.
 * The program resumes execution and will no longer stop
 * on signals, etc.  We better not have left any breakpoints
 * in the program or it'll die when it hits one.  For this
 * to work, it may be necessary for the process to have been
 * previously attached.  It *might* work if the program was
 * started via the normal ptrace (PTRACE_TRACEME).
 */

static void
detach_command (args, from_tty)
     char *args;
     int from_tty;
{
  int signal = 0;

  if (!inferior_pid)
    error ("Not currently tracing a program\n");
  if (from_tty)
    {
      char *exec_file = (char *)get_exec_file (0);
      if (exec_file == 0)
	exec_file = "";
      printf ("Detaching program: %s pid %d\n",
	      exec_file, inferior_pid);
      fflush (stdout);
    }
  if (args)
    signal = atoi (args);

  detach (signal);
  inferior_pid = 0;
}
#endif /* ATTACH_DETACH */

static
initialize ()
{
  add_com ("tty", class_run, tty_command,
	   "Set terminal for future runs of program being debugged.");

  add_com ("set-args", class_run, set_args_command,
	   "Specify arguments to give program being debugged when it is started.\n\
Follow this command with any number of args, to be passed to the program.");

  add_info ("environment", environment_info,
	    "The environment to give the program, or one variable's value.\n\
With an argument VAR, prints the value of environment variable VAR to\n\
give the program being debugged.  With no arguments, prints the entire\n\
environment to be given to the program.");

  add_com ("unset-environment", class_run, unset_environment_command,
	   "Cancel environment variable VAR for the program.\n\
This does not affect the program until the next \"run\" command.");
  add_com ("set-environment", class_run, set_environment_command,
	   "Set environment variable value to give the program.\n\
Arguments are VAR VALUE where VAR is variable name and VALUE is value.\n\
VALUES of environment variables are uninterpreted strings.\n\
This does not affect the program until the next \"run\" command.");
 
#ifdef ATTACH_DETACH
 add_com ("attach", class_run, attach_command,
 	   "Attach to a process that was started up outside of GDB.\n\
To do this, you must have permission to send the process a signal.\n\
And it must have the same effective uid as the debugger.\n\n\
Before using \"attach\", you must use the \"exec-file\" command\n\
to specify the program running in the process,\n\
and the \"symbol-file\" command to load its symbol table.");
  add_com ("detach", class_run, detach_command,
	   "Detach the process previously attached.\n\
The process is no longer traced and continues its execution.");
#endif /* ATTACH_DETACH */

  add_com ("signal", class_run, signal_command,
	   "Continue program giving it signal number SIGNUMBER.");

  add_com ("stepi", class_run, stepi_command,
	   "Step one instruction exactly.\n\
Argument N means do this N times (or till program stops for another reason).");
  add_com_alias ("si", "stepi", class_alias, 0);

  add_com ("nexti", class_run, nexti_command,
	   "Step one instruction, but proceed through subroutine calls.\n\
Argument N means do this N times (or till program stops for another reason).");
  add_com_alias ("ni", "nexti", class_alias, 0);

  add_com ("finish", class_run, finish_command,
	   "Execute until selected stack frame returns.\n\
Upon return, the value returned is printed and put in the value history.");

  add_com ("next", class_run, next_command,
	   "Step program, proceeding through subroutine calls.\n\
Like the \"step\" command as long as subroutine calls do not happen;\n\
when they do, the call is treated as one instruction.\n\
Argument N means do this N times (or till program stops for another reason).");
  add_com_alias ("n", "next", class_run, 1);

  add_com ("step", class_run, step_command,
	   "Step program until it reaches a different source line.\n\
Argument N means do this N times (or till program stops for another reason).");
  add_com_alias ("s", "step", class_run, 1);

  add_com ("jump", class_run, jump_command,
	   "Continue program being debugged at specified line or address.\n\
Give as argument either LINENUM or *ADDR, where ADDR is an expression\n\
for an address to start at.");

  add_com ("cont", class_run, cont_command,
	   "Continue program being debugged, after signal or breakpoint.\n\
If proceeding from breakpoint, a number N may be used as an argument:\n\
then the same breakpoint won't break until the Nth time it is reached.");
  add_com_alias ("c", "cont", class_run, 1);

  add_com ("run", class_run, run_command,
	   "Start debugged program.  You may specify arguments to give it.\n\
Args may include \"*\", or \"[...]\"; they are expanded using \"sh\".\n\
Input and output redirection with \">\", \"<\", or \">>\" are also allowed.\n\n\
With no arguments, uses arguments last specified (with \"run\" or \"set-args\".\n\
To cancel previous arguments and run with no arguments,\n\
use \"set-args\" without arguments.");
  add_com_alias ("r", "run", class_run, 1);

  add_info ("registers", registers_info,
	    "List of registers and their contents, for selected stack frame.\n\
Register name as argument means describe only that register.");

  add_info ("program", program_info,
	    "Execution status of the program.");

  inferior_args = savestring (" ", 1); /* By default, no args.  */
  inferior_environ = make_environ ();
  init_environ (inferior_environ);
}

END_FILE
