/* Everything about breakpoints, for GDB.
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.

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
#include "param.h"
#include "symtab.h"
#include "frame.h"

#include <stdio.h>

/* This is the sequence of bytes we insert for a breakpoint.  */

static char break_insn[] = BREAKPOINT;

/* States of enablement of breakpoint.
   `temporary' means disable when hit.
   `once' means delete when hit.  */

enum enable { disabled, enabled, temporary, delete};

struct breakpoint
{
  struct breakpoint *next;
  /* Number assigned to distinguish breakpoints.  */
  int number;
  /* Address to break at.  */
  CORE_ADDR address;
  /* Line number of this address.  Redundant.  */
  int line_number;
  /* Symtab of file of this address.  Redundant.  */
  struct symtab *symtab;
  /* Zero means disabled; remember the info but don't break here.  */
  enum enable enable;
  /* Number of stops at this breakpoint that should
     be continued automatically before really stopping.  */
  int ignore_count;
  /* "Real" contents of byte where breakpoint has been inserted.
     Valid only when breakpoints are in the program.  */
  char shadow_contents[sizeof break_insn];
  /* Nonzero if this breakpoint is now inserted.  */
  char inserted;
  /* Nonzero if this is not the first breakpoint in the list
     for the given address.  */
  char duplicate;
  /* Chain of command lines to execute when this breakpoint is hit.  */
  struct command_line *commands;
  /* Stack depth (frame).  If nonzero, break only if fp equals this.  */
  FRAME frame;
  /* Conditional.  Break only if this expression's value is nonzero.  */
  struct expression *cond;
};

#define ALL_BREAKPOINTS(b)  for (b = breakpoint_chain; b; b = b->next)

/* Chain of all breakpoints defined.  */

struct breakpoint *breakpoint_chain;

/* Number of last breakpoint made.  */

static int breakpoint_count;

/* Default address, symtab and line to put a breakpoint at
   for "break" command with no arg.
   if default_breakpoint_valid is zero, the other three are
   not valid, and "break" with no arg is an error.

   This set by print_stack_frame, which calls set_default_breakpoint.  */

int default_breakpoint_valid;
CORE_ADDR default_breakpoint_address;
struct symtab *default_breakpoint_symtab;
int default_breakpoint_line;

/* Remaining commands (not yet executed)
   of last breakpoint hit.  */

struct command_line *breakpoint_commands;

START_FILE

extern char *read_line ();

static void delete_breakpoint ();
void clear_momentary_breakpoints ();
void breakpoint_auto_delete ();

/* condition N EXP -- set break condition of breakpoint N to EXP.  */

static void
condition_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  register struct breakpoint *b;
  register char *p;
  register int bnum;
  register struct expression *expr;

  if (arg == 0)
    error_no_arg ("breakpoint number");

  p = arg;
  while (*p >= '0' && *p <= '9') p++;
  bnum = atoi (arg);

  ALL_BREAKPOINTS (b)
    if (b->number == bnum)
      {
	if (b->cond)
	  free (b->cond);
	if (*p == 0)
	  {
	    b->cond = 0;
	    if (from_tty)
	      printf ("Breakpoint %d now unconditional.\n", bnum);
	  }
	else
	  {
	    if (*p != ' ' && *p != '\t')
	      error ("Arguments must be an integer (breakpoint number) and an expression.");

	    /* Find start of expression */
	    while (*p == ' ' || *p == '\t') p++;

	    arg = p;
	    b->cond = (struct expression *) parse_c_1 (&arg, block_for_pc (b->address), 0);
	    if (*arg)
	      error ("Junk at end of expression");
	  }
	return;
      }

  error ("No breakpoint number %d.", bnum);
}

static void
commands_command (arg)
     char *arg;
{
  register struct breakpoint *b;
  register char *p, *p1;
  register int bnum;
  struct command_line *l;

  if (arg == 0)
    error_no_arg ("breakpoint number");

  /* If we allowed this, we would have problems with when to
     free the storage, if we change the commands currently
     being read from.  */

  if (breakpoint_commands)
    error ("Can't use the \"commands\" command among a breakpoint's commands.");

  p = arg;
  if (! (*p >= '0' && *p <= '9'))
    error ("Argument must be integer (a breakpoint number).");

  while (*p >= '0' && *p <= '9') p++;
  if (*p)
    error ("Unexpected extra arguments following breakpoint number.");

  bnum = atoi (arg);

  ALL_BREAKPOINTS (b)
    if (b->number == bnum)
      {
	if (input_from_terminal_p ())
	  {
	    printf ("Type commands for when breakpoint %d is hit, one per line.\n\
End with a line saying just \"end\".\n", bnum);
	    fflush (stdout);
	  }
	l = read_command_lines ();
	free_command_lines (&b->commands);
	b->commands = l;
	return;
      }
  error ("No breakpoint number %d.", bnum);
}

/* Called from command loop to execute the commands
   associated with the breakpoint we just stopped at.  */

void
do_breakpoint_commands ()
{
  while (breakpoint_commands)
    {
      char *line = breakpoint_commands->line;
      breakpoint_commands = breakpoint_commands->next;
      execute_command (line, 0);
      /* If command was "cont", breakpoint_commands is now 0,
	 of if we stopped at yet another breakpoint which has commands,
	 it is now the commands for the new breakpoint.  */
    }
  clear_momentary_breakpoints ();
}

/* Used when the program is proceeded, to eliminate any remaining
   commands attached to the previous breakpoint we stopped at.  */

void
clear_breakpoint_commands ()
{
  breakpoint_commands = 0;
  breakpoint_auto_delete (0);
}

/* Functions to get and set the current list of pending
   breakpoint commands.  These are used by run_stack_dummy
   to preserve the commands around a function call.  */

struct command_line *
get_breakpoint_commands ()
{
  return breakpoint_commands;
}

void
set_breakpoint_commands (cmds)
     struct command_line *cmds;
{
  breakpoint_commands = cmds;
}

/* insert_breakpoints is used when starting or continuing the program.
   remove_breakpoints is used when the program stops.
   Both return zero if successful,
   or an `errno' value if could not write the inferior.  */

int
insert_breakpoints ()
{
  register struct breakpoint *b;
  int val;

/*   printf ("Inserting breakpoints.\n"); */
  ALL_BREAKPOINTS (b)
    if (b->enable != disabled && ! b->inserted && ! b->duplicate)
      {
	read_memory (b->address, b->shadow_contents, sizeof break_insn);
	val = write_memory (b->address, break_insn, sizeof break_insn);
	if (val)
	  return val;
/*	printf ("Inserted breakpoint at 0x%x, shadow 0x%x, 0x%x.\n",
		b->address, b->shadow_contents[0], b->shadow_contents[1]); */
	b->inserted = 1;
      }
  return 0;
}

int
remove_breakpoints ()
{
  register struct breakpoint *b;
  int val;

/*   printf ("Removing breakpoints.\n"); */
  ALL_BREAKPOINTS (b)
    if (b->inserted)
      {
	val = write_memory (b->address, b->shadow_contents, sizeof break_insn);
	if (val)
	  return val;
	b->inserted = 0;
/*	printf ("Removed breakpoint at 0x%x, shadow 0x%x, 0x%x.\n",
		b->address, b->shadow_contents[0], b->shadow_contents[1]); */
      }

  return 0;
}

/* Clear the "inserted" flag in all breakpoints.
   This is done when the inferior is loaded.  */

int
mark_breakpoints_out ()
{
  register struct breakpoint *b;

  ALL_BREAKPOINTS (b)
    b->inserted = 0;
}

/* breakpoint_here_p (PC) returns 1 if an enabled breakpoint exists at PC.
   When continuing from a location with a breakpoint,
   we actually single step once before calling insert_breakpoints.  */

int
breakpoint_here_p (pc)
     CORE_ADDR pc;
{
  register struct breakpoint *b;

  ALL_BREAKPOINTS (b)
    if (b->enable != disabled && b->address == pc)
      return 1;

  return 0;
}

/* Evaluate the expression EXP and return 1 if value is zero.
   This is used inside a catch_errors to evaluate the breakpoint condition.  */

int
breakpoint_cond_eval (exp)
     struct expression *exp;
{
  return value_zerop (evaluate_expression (exp));
}

/* Return 0 if PC is not the address just after a breakpoint,
   or -1 if breakpoint says do not stop now,
   or -2 if breakpoint says it has deleted itself and don't stop,
   or -3 if hit a breakpoint number -3 (delete when program stops),
   or else the number of the breakpoint,
   with 0x1000000 added for a silent breakpoint.  */

int
breakpoint_stop_status (pc, frame)
     CORE_ADDR pc;
     FRAME frame;
{
  register struct breakpoint *b;
  register int cont = 0;

  /* Get the address where the breakpoint would have been.  */
  pc -= DECR_PC_AFTER_BREAK;

  ALL_BREAKPOINTS (b)
    if (b->enable != disabled && b->address == pc)
      {
	if (b->frame && b->frame != frame)
	  cont = -1;
	else
	  {
	    int value_zero;
	    if (b->cond)
	      {
		value_zero
		  = catch_errors (breakpoint_cond_eval, b->cond,
				  "Error occurred in testing breakpoint condition.");
		free_all_values ();
	      }
	    if (b->cond && value_zero)
	      {
		cont = -1;
	      }
	    else if (b->ignore_count > 0)
	      {
		b->ignore_count--;
		cont = -1;
	      }
	    else
	      {
		if (b->enable == temporary)
		  b->enable = disabled;
		breakpoint_commands = b->commands;
		if (breakpoint_commands
		    && !strcmp ("silent", breakpoint_commands->line))
		  {
		    breakpoint_commands = breakpoint_commands->next;
		    return 0x1000000 + b->number;
		  }
		return b->number;
	      }
	  }
      }

  return cont;
}

static void
breakpoint_1 (bnum)
     int bnum;
{
  register struct breakpoint *b;
  register struct command_line *l;
  register struct symbol *sym;
  CORE_ADDR last_addr = -1;

  ALL_BREAKPOINTS (b)
    if (bnum == -1 || bnum == b->number)
      {
	printf ("#%-3d %c  0x%08x ", b->number,
		"nyod"[(int) b->enable],
		b->address);
	last_addr = b->address;
	if (b->symtab)
	  {
	    sym = find_pc_function (b->address);
	    if (sym)
	      printf (" in %s (%s line %d)", SYMBOL_NAME (sym),
		      b->symtab->filename, b->line_number);
	    else
	      printf ("%s line %d", b->symtab->filename, b->line_number);
	  }
	printf ("\n");

	if (b->ignore_count)
	  printf ("\tignore next %d hits\n", b->ignore_count);
	if (b->frame)
	  printf ("\tstop only in stack frame at 0x%x\n", b->frame);
	if (b->cond)
	  {
	    printf ("\tbreak only if ");
	    print_expression (b->cond, stdout);
	    printf ("\n");
	  }
	if (l = b->commands)
	  while (l)
	    {
	      printf ("\t%s\n", l->line);
	      l = l->next;
	    }
      }

  if (last_addr != -1)
    set_next_address (last_addr);
}

static void
breakpoints_info (bnum_exp)
     char *bnum_exp;
{
  int bnum = -1;

  if (bnum_exp)
    bnum = parse_and_eval_address (bnum_exp);
  else if (breakpoint_chain == 0)
    printf ("No breakpoints.\n");
  else
    printf ("Breakpoints:\n\
Num Enb   Address    Where\n");

  breakpoint_1 (bnum);
}

/* Print a message describing any breakpoints set at PC.  */

static void
describe_other_breakpoints (pc)
     register CORE_ADDR pc;
{
  register int others = 0;
  register struct breakpoint *b;

  ALL_BREAKPOINTS (b)
    if (b->address == pc)
      others++;
  if (others > 0)
    {
      printf ("Note: breakpoint%s ", (others > 1) ? "s" : "");
      ALL_BREAKPOINTS (b)
	if (b->address == pc)
	  {
	    others--;
	    printf ("%d%s%s ",
		    b->number,
		    (b->enable == disabled) ? " (disabled)" : "",
		    (others > 1) ? "," : ((others == 1) ? " and" : ""));
	  }
      printf (" also set at pc 0x%x\n", pc);
    }
}

/* Set the default place to put a breakpoint
   for the `break' command with no arguments.  */

void
set_default_breakpoint (valid, addr, symtab, line)
     int valid;
     CORE_ADDR addr;
     struct symtab *symtab;
     int line;
{
  default_breakpoint_valid = valid;
  default_breakpoint_address = addr;
  default_breakpoint_symtab = symtab;
  default_breakpoint_line = line;
}

/* Rescan breakpoints at address ADDRESS,
   marking the first one as "first" and any others as "duplicates".
   This is so that the bpt instruction is only inserted once.  */

static void
check_duplicates (address)
     CORE_ADDR address;
{
  register struct breakpoint *b;
  register int count = 0;

  ALL_BREAKPOINTS (b)
    if (b->enable != disabled && b->address == address)
      {
	count++;
	b->duplicate = count > 1;
      }
}

/* Low level routine to set a breakpoint.
   Takes as args the three things that every breakpoint must have.
   Returns the breakpoint object so caller can set other things.
   Does not set the breakpoint number!
   Does not print anything.  */

static struct breakpoint *
set_raw_breakpoint (sal)
     struct symtab_and_line sal;
{
  register struct breakpoint *b, *b1;

  b = (struct breakpoint *) xmalloc (sizeof (struct breakpoint));
  bzero (b, sizeof *b);
  b->address = sal.pc;
  b->symtab = sal.symtab;
  b->line_number = sal.line;
  b->enable = enabled;
  b->next = 0;

  /* Add this breakpoint to the end of the chain
     so that a list of breakpoints will come out in order
     of increasing numbers.  */

  b1 = breakpoint_chain;
  if (b1 == 0)
    breakpoint_chain = b;
  else
    {
      while (b1->next)
	b1 = b1->next;
      b1->next = b;
    }

  check_duplicates (sal.pc);

  return b;
}

/* Set a breakpoint that will evaporate an end of command
   at address specified by SAL.
   Restrict it to frame FRAME if FRAME is nonzero.  */

void
set_momentary_breakpoint (sal, frame)
     struct symtab_and_line sal;
     FRAME frame;
{
  register struct breakpoint *b;
  b = set_raw_breakpoint (sal);
  b->number = -3;
  b->enable = delete;
  b->frame = frame;
}

void
clear_momentary_breakpoints ()
{
  register struct breakpoint *b;
  ALL_BREAKPOINTS (b)
    if (b->number == -3)
      {
	delete_breakpoint (b);
	break;
      }
}

/* Set a breakpoint from a symtab and line.
   If TEMPFLAG is nonzero, it is a temporary breakpoint.
   Print the same confirmation messages that the breakpoint command prints.  */

void
set_breakpoint (s, line, tempflag)
     struct symtab *s;
     int line;
     int tempflag;
{
  register struct breakpoint *b;
  struct symtab_and_line sal;
  
  sal.symtab = s;
  sal.line = line;
  sal.pc = find_line_pc (sal.symtab, sal.line);
  if (sal.pc == 0)
    error ("No line %d in file \"%s\".\n", sal.line, sal.symtab->filename);
  else
    {
      describe_other_breakpoints (sal.pc);

      b = set_raw_breakpoint (sal);
      b->number = ++breakpoint_count;
      b->cond = 0;
      if (tempflag)
	b->enable = temporary;

      printf ("Breakpoint %d at 0x%x", b->number, b->address);
      if (b->symtab)
	printf (": file %s, line %d.", b->symtab->filename, b->line_number);
      printf ("\n");
    }
}

/* Set a breakpoint according to ARG (function, linenum or *address)
   and make it temporary if TEMPFLAG is nonzero.  */

static void
break_command_1 (arg, tempflag, from_tty)
     char *arg;
     int tempflag, from_tty;
{
  struct symtab_and_line sal;
  register struct expression *cond = 0;
  register struct breakpoint *b;

  sal.pc = 0;

  if (arg)
    {
      sal = decode_line_1 (&arg, 1, 0, 0);

      if (sal.pc == 0 && sal.symtab != 0)
	{
	  sal.pc = find_line_pc (sal.symtab, sal.line);
	  if (sal.pc == 0)
	    error ("No line %d in file \"%s\".",
		   sal.line, sal.symtab->filename);
	}

      while (*arg)
	{
	  if (arg[0] == 'i' && arg[1] == 'f'
	      && (arg[2] == ' ' || arg[2] == '\t'))
	    cond = (struct expression *) parse_c_1 ((arg += 2, &arg),
						    block_for_pc (sal.pc), 0);
	  else
	    error ("Junk at end of arguments.");
	}
    }
  else if (default_breakpoint_valid)
    {
      sal.pc = default_breakpoint_address;
      sal.line = default_breakpoint_line;
      sal.symtab = default_breakpoint_symtab;
    }
  else
    error ("No default breakpoint address now.");

  if (from_tty)
    describe_other_breakpoints (sal.pc);

  b = set_raw_breakpoint (sal);
  b->number = ++breakpoint_count;
  b->cond = cond;
  if (tempflag)
    b->enable = temporary;

  printf ("Breakpoint %d at 0x%x", b->number, b->address);
  if (b->symtab)
    printf (": file %s, line %d.", b->symtab->filename, b->line_number);
  printf ("\n");
}

static void
break_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  break_command_1 (arg, 0, from_tty);
}

static void
tbreak_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  break_command_1 (arg, 1, from_tty);
}

static void
clear_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  register struct breakpoint *b, *b1;
  struct symtab_and_line sal;
  register struct breakpoint *found;

  if (arg)
    sal = decode_line_spec (arg, 1);
  else
    {
      sal.line = default_breakpoint_line;
      sal.symtab = default_breakpoint_symtab;
      sal.pc = 0;
      if (sal.symtab == 0)
	error ("No source file specified.");
    }

  /* If exact pc given, clear bpts at that pc.
     But if sal.pc is zero, clear all bpts on specified line.  */

  found = (struct breakpoint *) 0;
  while (breakpoint_chain
	 && (sal.pc ? breakpoint_chain->address == sal.pc
	     : (breakpoint_chain->symtab == sal.symtab
		&& breakpoint_chain->line_number == sal.line)))
    {
      b1 = breakpoint_chain;
      breakpoint_chain = b1->next;
      b1->next = found;
      found = b1;
    }

  ALL_BREAKPOINTS (b)
    while (b->next
	   && (sal.pc ? b->next->address == sal.pc
	       : (b->next->symtab == sal.symtab
		  && b->next->line_number == sal.line)))
      {
	b1 = b->next;
	b->next = b1->next;
	b1->next = found;
	found = b1;
      }

  if (found == 0)
    error ("No breakpoint at %s.", arg);

  if (found->next) from_tty = 1; /* Alwats report if deleted more than one */
  if (from_tty) printf ("Deleted breakpoint%s ", found->next ? "s" : "");
  while (found)
    {
      if (from_tty) printf ("%d ", found->number);
      b1 = found->next;
      delete_breakpoint (found);
      found = b1;
    }
  if (from_tty) putchar ('\n');
}

/* Delete breakpoint number BNUM if it is a `delete' breakpoint.
   This is called after breakpoint BNUM has been hit.
   Also delete any breakpoint numbered -3 unless there are breakpoint
   commands to be executed.  */

void
breakpoint_auto_delete (bnum)
     int bnum;
{
  register struct breakpoint *b;
  if (bnum != 0)
    ALL_BREAKPOINTS (b)
      if (b->number == bnum)
	{
	  if (b->enable == delete)
	    delete_breakpoint (b);
	  break;
	}
  if (breakpoint_commands == 0)
    clear_momentary_breakpoints ();
}

static void
delete_breakpoint (bpt)
     struct breakpoint *bpt;
{
  register struct breakpoint *b;

  if (bpt->inserted)
    write_memory (bpt->address, bpt->shadow_contents, sizeof break_insn);

  if (breakpoint_chain == bpt)
    breakpoint_chain = bpt->next;

  ALL_BREAKPOINTS (b)
    if (b->next == bpt)
      {
	b->next = bpt->next;
	break;
      }

  check_duplicates (bpt->address);

  free_command_lines (&bpt->commands);
  if (bpt->cond)
    free (bpt->cond);
  free (bpt);
}

void map_breakpoint_numbers ();

static void
delete_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  register struct breakpoint *b, *b1;

  if (arg == 0)
    {
      if (!from_tty || query ("Delete all breakpoints? "))
	{
	  /* No arg; clear all breakpoints.  */
	  while (breakpoint_chain)
	    delete_breakpoint (breakpoint_chain);
	}
    }
  else
    map_breakpoint_numbers (arg, delete_breakpoint);
}

/* Delete all breakpoints.
   Done when new symtabs are loaded, since the break condition expressions
   may become invalid, and the breakpoints are probably wrong anyway.  */

void
clear_breakpoints ()
{
  delete_command (0, 0);
}

/* Set ignore-count of breakpoint number BPTNUM to COUNT.
   If from_tty is nonzero, it prints a message to that effect,
   which ends with a period (no newline).  */

void
set_ignore_count (bptnum, count, from_tty)
     int bptnum, count, from_tty;
{
  register struct breakpoint *b;

  if (count < 0)
    count = 0;

  ALL_BREAKPOINTS (b)
    if (b->number == bptnum)
      {
	b->ignore_count = count;
	if (!from_tty)
	  return;
	else if (count == 0)
	  printf ("Will stop next time breakpoint %d is reached.", bptnum);
	else if (count == 1)
	  printf ("Will ignore next crossing of breakpoint %d.", bptnum);
	else
	  printf ("Will ignore next %d crossings of breakpoint %d.",
		  count, bptnum);
	return;
      }

  error ("No breakpoint number %d.", bptnum);
}

/* Command to set ignore-count of breakpoint N to COUNT.  */

static void
ignore_command (args, from_tty)
     char *args;
     int from_tty;
{
  register char *p = args;
  register int num;

  if (p == 0)
    error_no_arg ("a breakpoint number");
  
  while (*p >= '0' && *p <= '9') p++;
  if (*p && *p != ' ' && *p != '\t')
    error ("First argument must be a breakpoint number.");

  num = atoi (args);

  if (*p == 0)
    error ("Second argument (specified ignore-count) is missing.");

  set_ignore_count (num, parse_and_eval_address (p), from_tty);
  printf ("\n");
}

/* Call FUNCTION on each of the breakpoints
   whose numbers are given in ARGS.  */

static void
map_breakpoint_numbers (args, function)
     char *args;
     void (*function) ();
{
  register char *p = args;
  register char *p1;
  register int num;
  register struct breakpoint *b;

  if (p == 0)
    error_no_arg ("one or more breakpoint numbers");

  while (*p)
    {
      p1 = p;
      while (*p1 >= '0' && *p1 <= '9') p1++;
      if (*p1 && *p1 != ' ' && *p1 != '\t')
	error ("Arguments must be breakpoint numbers.");

      num = atoi (p);

      ALL_BREAKPOINTS (b)
	if (b->number == num)
	  {
	    function (b);
	    goto win;
	  }
      printf ("No breakpoint number %d.\n", num);
    win:
      p = p1;
      while (*p == ' ' || *p == '\t') p++;
    }
}

static void
enable_breakpoint (bpt)
     struct breakpoint *bpt;
{
  bpt->enable = enabled;

  check_duplicates (bpt->address);
}

static void
enable_command (args)
     char *args;
{
  map_breakpoint_numbers (args, enable_breakpoint);
}

static void
disable_breakpoint (bpt)
     struct breakpoint *bpt;
{
  bpt->enable = disabled;

  check_duplicates (bpt->address);
}

static void
disable_command (args)
     char *args;
{
  register struct breakpoint *bpt;
  if (args == 0)
    ALL_BREAKPOINTS (bpt)
      disable_breakpoint (bpt);
  else
    map_breakpoint_numbers (args, disable_breakpoint);
}

static void
enable_once_breakpoint (bpt)
     struct breakpoint *bpt;
{
  bpt->enable = temporary;

  check_duplicates (bpt->address);
}

static void
enable_once_command (args)
     char *args;
{
  map_breakpoint_numbers (args, enable_once_breakpoint);
}

static void
enable_delete_breakpoint (bpt)
     struct breakpoint *bpt;
{
  bpt->enable = delete;

  check_duplicates (bpt->address);
}

static void
enable_delete_command (args)
     char *args;
{
  map_breakpoint_numbers (args, enable_delete_breakpoint);
}


/* Chain containing all defined enable commands.  */

struct cmd_list_element *enablelist;

extern struct cmd_list_element *cmdlist;

static
initialize ()
{
  breakpoint_chain = 0;
  breakpoint_count = 0;
  enablelist = 0;

  add_com ("ignore", class_breakpoint, ignore_command,
	   "Set ignore-count of breakpoint number N to COUNT.");

  add_com ("commands", class_breakpoint, commands_command,
	   "Set commands to be executed when a breakpoint is hit.\n\
Give breakpoint number as argument after \"commands\".\n\
The commands themselves follow starting on the next line.\n\
Type a line containing \"end\" to indicate the end of them.\n\
Give \"silent\" as the first line to make the breakpoint silent;\n\
then no output is printed when it is hit, except what the commands print.");

  add_com ("condition", class_breakpoint, condition_command,
	   "Specify breakpoint number N to break only if COND is true.\n\
N is an integer; COND is a C expression to be evaluated whenever\n\
breakpoint N is reached.  Actually break only when COND is nonzero.");

  add_com ("tbreak", class_breakpoint, tbreak_command,
	   "Set a temporary breakpoint.  Args like \"break\" command.\n\
Like \"break\" except the breakpoint is only enabled temporarily,\n\
so it will be disabled when hit.  Equivalent to \"break\" followed\n\
by using \"enable once\" on the breakpoint number.");

  add_prefix_cmd ("enable", class_breakpoint, enable_command,
		  "Enable some breakpoints.  Give breakpoint numbers as arguments.\n\
With no subcommand, breakpoints are enabled until you command otherwise.\n\
This is used to cancel the effect of the \"disable\" command.\n\
With a subcommand you can enable temporarily.",
		  &enablelist, "enable ", 1, &cmdlist);

  add_cmd ("delete", 0, enable_delete_command,
	   "Enable breakpoints and delete when hit.  Give breakpoint numbers.\n\
If a breakpoint is hit while enabled in this fashion, it is deleted.",
	   &enablelist);

  add_cmd ("once", 0, enable_once_command,
	   "Enable breakpoints for one hit.  Give breakpoint numbers.\n\
If a breakpoint is hit while enabled in this fashion, it becomes disabled.\n\
See the \"tbreak\" command which sets a breakpoint and enables it once.",
	   &enablelist);

  add_com ("disable", class_breakpoint, disable_command,
	   "Disable some breakpoints.  Give breakpoint numbers as arguments.\n\
With no arguments, disable all breakpoints.\n\
A disabled breakpoint is not forgotten,\n\
but it has no effect until enabled again.");
  add_com_alias ("dis", "disable", class_breakpoint, 1);

  add_com ("delete", class_breakpoint, delete_command,
	   "Delete breakpoints, specifying breakpoint numbers; or all breakpoints.\n\
Arguments are breakpoint numbers with spaces in between.\n\
To delete all breakpoints, give no argument.");
  add_com_alias ("d", "delete", class_breakpoint, 1);

  add_com ("clear", class_breakpoint, clear_command,
	   "Clear breakpoint at specified line or function.\n\
Argument may be line number, function name, or \"*\" and an address.\n\
If line number is specified, all breakpoints in that line are cleared.\n\
If function is specified, breakpoints at beginning of function are cleared.\n\
If an address is specified, breakpoints at that address are cleared.\n\n\
With no argument, clears all breakpoints in the line that the selected frame\n\
is executing in.\n\
\n\
See also the \"delete\" command which clears breakpoints by number.");

  add_com ("break", class_breakpoint, break_command,
	   "Set breakpoint at specified line or function.\n\
Argument may be line number, function name, or \"*\" and an address.\n\
If line number is specified, break at start of code for that line.\n\
If function is specified, break at start of code for that function.\n\
If an address is specified, break at that exact address.\n\
With no arg, uses current execution address of selected stack frame.\n\
This is useful for breaking on return to a stack frame.\n\
\n\
Multiple breakpoints at one place are permitted, and useful if conditional.\n\
\n\
Do \"help breakpoints\" for info on other commands dealing with breakpoints.");
  add_com_alias ("b", "break", class_run, 1);
  add_com_alias ("br", "break", class_run, 1);
  add_com_alias ("bre", "break", class_run, 1);
  add_com_alias ("brea", "break", class_run, 1);

  add_info ("breakpoints", breakpoints_info,
	    "Status of all breakpoints, or breakpoint number NUMBER.\n\
Second column is \"y\" for enabled breakpoint, \"n\" for disabled,\n\
\"o\" for enabled once (disable when hit), \"d\" for enable but delete when hit.\n\
Then come the address and the file/line number.\n\n\
Convenience variable \"$_\" and default examine address for \"x\"\n\
are set to the address of the last breakpoint listed.");
}

END_FILE
