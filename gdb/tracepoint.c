/* Tracing functionality for remote targets in custom GDB protocol

   Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2007, 2008, 2009, 2010 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "arch-utils.h"
#include "symtab.h"
#include "frame.h"
#include "gdbtypes.h"
#include "expression.h"
#include "gdbcmd.h"
#include "value.h"
#include "target.h"
#include "language.h"
#include "gdb_string.h"
#include "inferior.h"
#include "breakpoint.h"
#include "tracepoint.h"
#include "linespec.h"
#include "regcache.h"
#include "completer.h"
#include "block.h"
#include "dictionary.h"
#include "observer.h"
#include "user-regs.h"
#include "valprint.h"
#include "gdbcore.h"
#include "objfiles.h"
#include "filenames.h"
#include "gdbthread.h"

#include "ax.h"
#include "ax-gdb.h"

/* readline include files */
#include "readline/readline.h"
#include "readline/history.h"

/* readline defines this.  */
#undef savestring

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

extern int hex2bin (const char *hex, gdb_byte *bin, int count);
extern int bin2hex (const gdb_byte *bin, char *hex, int count);

extern void stop_tracing ();

/* Maximum length of an agent aexpression.
   This accounts for the fact that packets are limited to 400 bytes
   (which includes everything -- including the checksum), and assumes
   the worst case of maximum length for each of the pieces of a
   continuation packet.

   NOTE: expressions get mem2hex'ed otherwise this would be twice as
   large.  (400 - 31)/2 == 184 */
#define MAX_AGENT_EXPR_LEN	184

/* A hook used to notify the UI of tracepoint operations.  */

void (*deprecated_trace_find_hook) (char *arg, int from_tty);
void (*deprecated_trace_start_stop_hook) (int start, int from_tty);

extern void (*deprecated_readline_begin_hook) (char *, ...);
extern char *(*deprecated_readline_hook) (char *);
extern void (*deprecated_readline_end_hook) (void);

/* GDB commands implemented in other modules:
 */  

extern void output_command (char *, int);

/* 
   Tracepoint.c:

   This module defines the following debugger commands:
   trace            : set a tracepoint on a function, line, or address.
   info trace       : list all debugger-defined tracepoints.
   delete trace     : delete one or more tracepoints.
   enable trace     : enable one or more tracepoints.
   disable trace    : disable one or more tracepoints.
   actions          : specify actions to be taken at a tracepoint.
   passcount        : specify a pass count for a tracepoint.
   tstart           : start a trace experiment.
   tstop            : stop a trace experiment.
   tstatus          : query the status of a trace experiment.
   tfind            : find a trace frame in the trace buffer.
   tdump            : print everything collected at the current tracepoint.
   save-tracepoints : write tracepoint setup into a file.

   This module defines the following user-visible debugger variables:
   $trace_frame : sequence number of trace frame currently being debugged.
   $trace_line  : source line of trace frame currently being debugged.
   $trace_file  : source file of trace frame currently being debugged.
   $tracepoint  : tracepoint number of trace frame currently being debugged.
 */


/* ======= Important global variables: ======= */

/* The list of all trace state variables.  We don't retain pointers to
   any of these for any reason - API is by name or number only - so it
   works to have a vector of objects.  */

typedef struct trace_state_variable tsv_s;
DEF_VEC_O(tsv_s);

static VEC(tsv_s) *tvariables;

/* The next integer to assign to a variable.  */

static int next_tsv_number = 1;

/* Number of last traceframe collected.  */
static int traceframe_number;

/* Tracepoint for last traceframe collected.  */
static int tracepoint_number;

/* Symbol for function for last traceframe collected */
static struct symbol *traceframe_fun;

/* Symtab and line for last traceframe collected */
static struct symtab_and_line traceframe_sal;

/* Tracing command lists */
static struct cmd_list_element *tfindlist;

/* List of expressions to collect by default at each tracepoint hit.  */
char *default_collect = "";

static int disconnected_tracing;

/* ======= Important command functions: ======= */
static void trace_actions_command (char *, int);
static void trace_start_command (char *, int);
static void trace_stop_command (char *, int);
static void trace_status_command (char *, int);
static void trace_find_command (char *, int);
static void trace_find_pc_command (char *, int);
static void trace_find_tracepoint_command (char *, int);
static void trace_find_line_command (char *, int);
static void trace_find_range_command (char *, int);
static void trace_find_outside_command (char *, int);
static void trace_dump_command (char *, int);

/* support routines */

struct collection_list;
static void add_aexpr (struct collection_list *, struct agent_expr *);
static char *mem2hex (gdb_byte *, char *, int);
static void add_register (struct collection_list *collection,
			  unsigned int regno);
static struct cleanup *make_cleanup_free_actions (struct breakpoint *t);

extern void send_disconnected_tracing_value (int value);

static void free_uploaded_tps (struct uploaded_tp **utpp);
static void free_uploaded_tsvs (struct uploaded_tsv **utsvp);


extern void _initialize_tracepoint (void);

static struct trace_status trace_status;

char *stop_reason_names[] = {
  "tunknown",
  "tnotrun",
  "tstop",
  "tfull",
  "tdisconnected",
  "tpasscount"
};

struct trace_status *
current_trace_status ()
{
  return &trace_status;
}

/* Set traceframe number to NUM.  */
static void
set_traceframe_num (int num)
{
  traceframe_number = num;
  set_internalvar_integer (lookup_internalvar ("trace_frame"), num);
}

/* Set tracepoint number to NUM.  */
static void
set_tracepoint_num (int num)
{
  tracepoint_number = num;
  set_internalvar_integer (lookup_internalvar ("tracepoint"), num);
}

/* Set externally visible debug variables for querying/printing
   the traceframe context (line, function, file) */

static void
set_traceframe_context (struct frame_info *trace_frame)
{
  CORE_ADDR trace_pc;

  if (trace_frame == NULL)		/* Cease debugging any trace buffers.  */
    {
      traceframe_fun = 0;
      traceframe_sal.pc = traceframe_sal.line = 0;
      traceframe_sal.symtab = NULL;
      clear_internalvar (lookup_internalvar ("trace_func"));
      clear_internalvar (lookup_internalvar ("trace_file"));
      set_internalvar_integer (lookup_internalvar ("trace_line"), -1);
      return;
    }

  /* Save as globals for internal use.  */
  trace_pc = get_frame_pc (trace_frame);
  traceframe_sal = find_pc_line (trace_pc, 0);
  traceframe_fun = find_pc_function (trace_pc);

  /* Save linenumber as "$trace_line", a debugger variable visible to
     users.  */
  set_internalvar_integer (lookup_internalvar ("trace_line"),
			   traceframe_sal.line);

  /* Save func name as "$trace_func", a debugger variable visible to
     users.  */
  if (traceframe_fun == NULL
      || SYMBOL_LINKAGE_NAME (traceframe_fun) == NULL)
    clear_internalvar (lookup_internalvar ("trace_func"));
  else
    set_internalvar_string (lookup_internalvar ("trace_func"),
			    SYMBOL_LINKAGE_NAME (traceframe_fun));

  /* Save file name as "$trace_file", a debugger variable visible to
     users.  */
  if (traceframe_sal.symtab == NULL
      || traceframe_sal.symtab->filename == NULL)
    clear_internalvar (lookup_internalvar ("trace_file"));
  else
    set_internalvar_string (lookup_internalvar ("trace_file"),
			    traceframe_sal.symtab->filename);
}

/* Create a new trace state variable with the given name.  */

struct trace_state_variable *
create_trace_state_variable (const char *name)
{
  struct trace_state_variable tsv;

  memset (&tsv, 0, sizeof (tsv));
  tsv.name = name;
  tsv.number = next_tsv_number++;
  return VEC_safe_push (tsv_s, tvariables, &tsv);
}

/* Look for a trace state variable of the given name.  */

struct trace_state_variable *
find_trace_state_variable (const char *name)
{
  struct trace_state_variable *tsv;
  int ix;

  for (ix = 0; VEC_iterate (tsv_s, tvariables, ix, tsv); ++ix)
    if (strcmp (name, tsv->name) == 0)
      return tsv;

  return NULL;
}

void
delete_trace_state_variable (const char *name)
{
  struct trace_state_variable *tsv;
  int ix;

  for (ix = 0; VEC_iterate (tsv_s, tvariables, ix, tsv); ++ix)
    if (strcmp (name, tsv->name) == 0)
      {
	VEC_unordered_remove (tsv_s, tvariables, ix);
	return;
      }

  warning (_("No trace variable named \"$%s\", not deleting"), name);
}

/* The 'tvariable' command collects a name and optional expression to
   evaluate into an initial value.  */

void
trace_variable_command (char *args, int from_tty)
{
  struct expression *expr;
  struct cleanup *old_chain;
  struct internalvar *intvar = NULL;
  LONGEST initval = 0;
  struct trace_state_variable *tsv;

  if (!args || !*args)
    error_no_arg (_("trace state variable name"));

  /* All the possible valid arguments are expressions.  */
  expr = parse_expression (args);
  old_chain = make_cleanup (free_current_contents, &expr);

  if (expr->nelts == 0)
    error (_("No expression?"));

  /* Only allow two syntaxes; "$name" and "$name=value".  */
  if (expr->elts[0].opcode == OP_INTERNALVAR)
    {
      intvar = expr->elts[1].internalvar;
    }
  else if (expr->elts[0].opcode == BINOP_ASSIGN
	   && expr->elts[1].opcode == OP_INTERNALVAR)
    {
      intvar = expr->elts[2].internalvar;
      initval = value_as_long (evaluate_subexpression_type (expr, 4));
    }
  else
    error (_("Syntax must be $NAME [ = EXPR ]"));

  if (!intvar)
    error (_("No name given"));

  if (strlen (internalvar_name (intvar)) <= 0)
    error (_("Must supply a non-empty variable name"));

  /* If the variable already exists, just change its initial value.  */
  tsv = find_trace_state_variable (internalvar_name (intvar));
  if (tsv)
    {
      tsv->initial_value = initval;
      printf_filtered (_("Trace state variable $%s now has initial value %s.\n"),
		       tsv->name, plongest (tsv->initial_value));
      return;
    }

  /* Create a new variable.  */
  tsv = create_trace_state_variable (internalvar_name (intvar));
  tsv->initial_value = initval;

  printf_filtered (_("Trace state variable $%s created, with initial value %s.\n"),
		   tsv->name, plongest (tsv->initial_value));

  do_cleanups (old_chain);
}

void
delete_trace_variable_command (char *args, int from_tty)
{
  int i, ix;
  char **argv;
  struct cleanup *back_to;
  struct trace_state_variable *tsv;

  if (args == NULL)
    {
      if (query (_("Delete all trace state variables? ")))
	VEC_free (tsv_s, tvariables);
      dont_repeat ();
      return;
    }

  argv = gdb_buildargv (args);
  back_to = make_cleanup_freeargv (argv);

  for (i = 0; argv[i] != NULL; i++)
    {
      if (*argv[i] == '$')
	delete_trace_state_variable (argv[i] + 1);
      else
	warning (_("Name \"%s\" not prefixed with '$', ignoring"), argv[i]);
    }

  do_cleanups (back_to);

  dont_repeat ();
}

/* List all the trace state variables.  */

static void
tvariables_info (char *args, int from_tty)
{
  struct trace_state_variable *tsv;
  int ix;
  char *reply;
  ULONGEST tval;

  if (VEC_length (tsv_s, tvariables) == 0)
    {
      printf_filtered (_("No trace state variables.\n"));
      return;
    }

  /* Try to acquire values from the target.  */
  for (ix = 0; VEC_iterate (tsv_s, tvariables, ix, tsv); ++ix)
    tsv->value_known = target_get_trace_state_variable_value (tsv->number,
							      &(tsv->value));

  printf_filtered (_("Name\t\t  Initial\tCurrent\n"));

  for (ix = 0; VEC_iterate (tsv_s, tvariables, ix, tsv); ++ix)
    {
      printf_filtered ("$%s", tsv->name);
      print_spaces_filtered (17 - strlen (tsv->name), gdb_stdout);
      printf_filtered ("%s ", plongest (tsv->initial_value));
      print_spaces_filtered (11 - strlen (plongest (tsv->initial_value)), gdb_stdout);
      if (tsv->value_known)
	printf_filtered ("  %s", plongest (tsv->value));
      else if (current_trace_status ()->running || traceframe_number >= 0)
	/* The value is/was defined, but we don't have it.  */
	printf_filtered (_("  <unknown>"));
      else
	/* It is not meaningful to ask about the value.  */
	printf_filtered (_("  <undefined>"));
      printf_filtered ("\n");
    }
}

/* ACTIONS functions: */

/* Prototypes for action-parsing utility commands  */
static void read_actions (struct breakpoint *);

/* The three functions:
   collect_pseudocommand, 
   while_stepping_pseudocommand, and 
   end_actions_pseudocommand
   are placeholders for "commands" that are actually ONLY to be used
   within a tracepoint action list.  If the actual function is ever called,
   it means that somebody issued the "command" at the top level,
   which is always an error.  */

void
end_actions_pseudocommand (char *args, int from_tty)
{
  error (_("This command cannot be used at the top level."));
}

void
while_stepping_pseudocommand (char *args, int from_tty)
{
  error (_("This command can only be used in a tracepoint actions list."));
}

static void
collect_pseudocommand (char *args, int from_tty)
{
  error (_("This command can only be used in a tracepoint actions list."));
}

static void
teval_pseudocommand (char *args, int from_tty)
{
  error (_("This command can only be used in a tracepoint actions list."));
}

/* Enter a list of actions for a tracepoint.  */
static void
trace_actions_command (char *args, int from_tty)
{
  struct breakpoint *t;
  char tmpbuf[128];
  char *end_msg = "End with a line saying just \"end\".";

  t = get_tracepoint_by_number (&args, 0, 1);
  if (t)
    {
      sprintf (tmpbuf, "Enter actions for tracepoint %d, one per line.",
	       t->number);

      if (from_tty)
	{
	  if (deprecated_readline_begin_hook)
	    (*deprecated_readline_begin_hook) ("%s  %s\n", tmpbuf, end_msg);
	  else if (input_from_terminal_p ())
	    printf_filtered ("%s\n%s\n", tmpbuf, end_msg);
	}

      free_actions (t);
      t->step_count = 0;	/* read_actions may set this */
      read_actions (t);

      if (deprecated_readline_end_hook)
	(*deprecated_readline_end_hook) ();
      /* tracepoints_changed () */
    }
  /* else just return */
}

/* worker function */
static void
read_actions (struct breakpoint *t)
{
  char *line;
  char *prompt1 = "> ", *prompt2 = "  > ";
  char *prompt = prompt1;
  enum actionline_type linetype;
  extern FILE *instream;
  struct action_line *next = NULL, *temp;
  struct cleanup *old_chain;

  /* Control-C quits instantly if typed while in this loop
     since it should not wait until the user types a newline.  */
  immediate_quit++;
  /* FIXME: kettenis/20010823: Something is wrong here.  In this file
     STOP_SIGNAL is never defined.  So this code has been left out, at
     least for quite a while now.  Replacing STOP_SIGNAL with SIGTSTP
     leads to compilation failures since the variable job_control
     isn't declared.  Leave this alone for now.  */
#ifdef STOP_SIGNAL
  if (job_control)
    signal (STOP_SIGNAL, handle_stop_sig);
#endif
  old_chain = make_cleanup_free_actions (t);
  while (1)
    {
      /* Make sure that all output has been output.  Some machines may
         let you get away with leaving out some of the gdb_flush, but
         not all.  */
      wrap_here ("");
      gdb_flush (gdb_stdout);
      gdb_flush (gdb_stderr);

      if (deprecated_readline_hook && instream == NULL)
	line = (*deprecated_readline_hook) (prompt);
      else if (instream == stdin && ISATTY (instream))
	{
	  line = gdb_readline_wrapper (prompt);
	  if (line && *line)	/* add it to command history */
	    add_history (line);
	}
      else
	line = gdb_readline (0);

      if (!line)
        {
          line = xstrdup ("end");
          printf_filtered ("end\n");
        }
      
      linetype = validate_actionline (&line, t);
      if (linetype == BADLINE)
	continue;		/* already warned -- collect another line */

      temp = xmalloc (sizeof (struct action_line));
      temp->next = NULL;
      temp->action = line;

      if (next == NULL)		/* first action for this tracepoint? */
	t->actions = next = temp;
      else
	{
	  next->next = temp;
	  next = temp;
	}

      if (linetype == STEPPING)	/* begin "while-stepping" */
	{
	  if (prompt == prompt2)
	    {
	      warning (_("Already processing 'while-stepping'"));
	      continue;
	    }
	  else
	    prompt = prompt2;	/* change prompt for stepping actions */
	}
      else if (linetype == END)
	{
	  if (prompt == prompt2)
	    {
	      prompt = prompt1;	/* end of single-stepping actions */
	    }
	  else
	    {			/* end of actions */
	      if (t->actions->next == NULL)
		{
		  /* An "end" all by itself with no other actions
		     means this tracepoint has no actions.
		     Discard empty list.  */
		  free_actions (t);
		}
	      break;
	    }
	}
    }
#ifdef STOP_SIGNAL
  if (job_control)
    signal (STOP_SIGNAL, SIG_DFL);
#endif
  immediate_quit--;
  discard_cleanups (old_chain);
}

/* worker function */
enum actionline_type
validate_actionline (char **line, struct breakpoint *t)
{
  struct cmd_list_element *c;
  struct expression *exp = NULL;
  struct cleanup *old_chain = NULL;
  char *p;

  /* if EOF is typed, *line is NULL */
  if (*line == NULL)
    return END;

  for (p = *line; isspace ((int) *p);)
    p++;

  /* Symbol lookup etc.  */
  if (*p == '\0')	/* empty line: just prompt for another line.  */
    return BADLINE;

  if (*p == '#')		/* comment line */
    return GENERIC;

  c = lookup_cmd (&p, cmdlist, "", -1, 1);
  if (c == 0)
    {
      warning (_("'%s' is not an action that I know, or is ambiguous."), 
	       p);
      return BADLINE;
    }

  if (cmd_cfunc_eq (c, collect_pseudocommand))
    {
      struct agent_expr *aexpr;
      struct agent_reqs areqs;

      do
	{			/* repeat over a comma-separated list */
	  QUIT;			/* allow user to bail out with ^C */
	  while (isspace ((int) *p))
	    p++;

	  if (*p == '$')	/* look for special pseudo-symbols */
	    {
	      if ((0 == strncasecmp ("reg", p + 1, 3)) ||
		  (0 == strncasecmp ("arg", p + 1, 3)) ||
		  (0 == strncasecmp ("loc", p + 1, 3)))
		{
		  p = strchr (p, ',');
		  continue;
		}
	      /* else fall thru, treat p as an expression and parse it!  */
	    }
	  exp = parse_exp_1 (&p, block_for_pc (t->loc->address), 1);
	  old_chain = make_cleanup (free_current_contents, &exp);

	  if (exp->elts[0].opcode == OP_VAR_VALUE)
	    {
	      if (SYMBOL_CLASS (exp->elts[2].symbol) == LOC_CONST)
		{
		  warning (_("constant %s (value %ld) will not be collected."),
			   SYMBOL_PRINT_NAME (exp->elts[2].symbol),
			   SYMBOL_VALUE (exp->elts[2].symbol));
		  return BADLINE;
		}
	      else if (SYMBOL_CLASS (exp->elts[2].symbol) == LOC_OPTIMIZED_OUT)
		{
		  warning (_("%s is optimized away and cannot be collected."),
			   SYMBOL_PRINT_NAME (exp->elts[2].symbol));
		  return BADLINE;
		}
	    }

	  /* We have something to collect, make sure that the expr to
	     bytecode translator can handle it and that it's not too
	     long.  */
	  aexpr = gen_trace_for_expr (t->loc->address, exp);
	  make_cleanup_free_agent_expr (aexpr);

	  if (aexpr->len > MAX_AGENT_EXPR_LEN)
	    error (_("expression too complicated, try simplifying"));

	  ax_reqs (aexpr, &areqs);
	  (void) make_cleanup (xfree, areqs.reg_mask);

	  if (areqs.flaw != agent_flaw_none)
	    error (_("malformed expression"));

	  if (areqs.min_height < 0)
	    error (_("gdb: Internal error: expression has min height < 0"));

	  if (areqs.max_height > 20)
	    error (_("expression too complicated, try simplifying"));

	  do_cleanups (old_chain);
	}
      while (p && *p++ == ',');
      return GENERIC;
    }
  else if (cmd_cfunc_eq (c, teval_pseudocommand))
    {
      struct agent_expr *aexpr;

      do
	{			/* repeat over a comma-separated list */
	  QUIT;			/* allow user to bail out with ^C */
	  while (isspace ((int) *p))
	    p++;

	  /* Only expressions are allowed for this action.  */
	  exp = parse_exp_1 (&p, block_for_pc (t->loc->address), 1);
	  old_chain = make_cleanup (free_current_contents, &exp);

	  /* We have something to evaluate, make sure that the expr to
	     bytecode translator can handle it and that it's not too
	     long.  */
	  aexpr = gen_eval_for_expr (t->loc->address, exp);
	  make_cleanup_free_agent_expr (aexpr);

	  if (aexpr->len > MAX_AGENT_EXPR_LEN)
	    error (_("expression too complicated, try simplifying"));

	  do_cleanups (old_chain);
	}
      while (p && *p++ == ',');
      return GENERIC;
    }
  else if (cmd_cfunc_eq (c, while_stepping_pseudocommand))
    {
      char *steparg;		/* in case warning is necessary */

      while (isspace ((int) *p))
	p++;
      steparg = p;

      if (*p == '\0' ||
	  (t->step_count = strtol (p, &p, 0)) == 0)
	{
	  warning (_("'%s': bad step-count; command ignored."), *line);
	  return BADLINE;
	}
      return STEPPING;
    }
  else if (cmd_cfunc_eq (c, end_actions_pseudocommand))
    return END;
  else
    {
      warning (_("'%s' is not a supported tracepoint action."), *line);
      return BADLINE;
    }
}

/* worker function */
void
free_actions (struct breakpoint *t)
{
  struct action_line *line, *next;

  for (line = t->actions; line; line = next)
    {
      next = line->next;
      if (line->action)
	xfree (line->action);
      xfree (line);
    }
  t->actions = NULL;
}

static void
do_free_actions_cleanup (void *t)
{
  free_actions (t);
}

static struct cleanup *
make_cleanup_free_actions (struct breakpoint *t)
{
  return make_cleanup (do_free_actions_cleanup, t);
}

enum {
  memrange_absolute = -1
};

struct memrange
{
  int type;		/* memrange_absolute for absolute memory range,
                           else basereg number */
  bfd_signed_vma start;
  bfd_signed_vma end;
};

struct collection_list
  {
    unsigned char regs_mask[32];	/* room for up to 256 regs */
    long listsize;
    long next_memrange;
    struct memrange *list;
    long aexpr_listsize;	/* size of array pointed to by expr_list elt */
    long next_aexpr_elt;
    struct agent_expr **aexpr_list;

  }
tracepoint_list, stepping_list;

/* MEMRANGE functions: */

static int memrange_cmp (const void *, const void *);

/* compare memranges for qsort */
static int
memrange_cmp (const void *va, const void *vb)
{
  const struct memrange *a = va, *b = vb;

  if (a->type < b->type)
    return -1;
  if (a->type > b->type)
    return 1;
  if (a->type == memrange_absolute)
    {
      if ((bfd_vma) a->start < (bfd_vma) b->start)
	return -1;
      if ((bfd_vma) a->start > (bfd_vma) b->start)
	return 1;
    }
  else
    {
      if (a->start < b->start)
	return -1;
      if (a->start > b->start)
	return 1;
    }
  return 0;
}

/* Sort the memrange list using qsort, and merge adjacent memranges.  */
static void
memrange_sortmerge (struct collection_list *memranges)
{
  int a, b;

  qsort (memranges->list, memranges->next_memrange,
	 sizeof (struct memrange), memrange_cmp);
  if (memranges->next_memrange > 0)
    {
      for (a = 0, b = 1; b < memranges->next_memrange; b++)
	{
	  if (memranges->list[a].type == memranges->list[b].type &&
	      memranges->list[b].start - memranges->list[a].end <=
	      MAX_REGISTER_SIZE)
	    {
	      /* memrange b starts before memrange a ends; merge them.  */
	      if (memranges->list[b].end > memranges->list[a].end)
		memranges->list[a].end = memranges->list[b].end;
	      continue;		/* next b, same a */
	    }
	  a++;			/* next a */
	  if (a != b)
	    memcpy (&memranges->list[a], &memranges->list[b],
		    sizeof (struct memrange));
	}
      memranges->next_memrange = a + 1;
    }
}

/* Add a register to a collection list.  */
static void
add_register (struct collection_list *collection, unsigned int regno)
{
  if (info_verbose)
    printf_filtered ("collect register %d\n", regno);
  if (regno >= (8 * sizeof (collection->regs_mask)))
    error (_("Internal: register number %d too large for tracepoint"),
	   regno);
  collection->regs_mask[regno / 8] |= 1 << (regno % 8);
}

/* Add a memrange to a collection list */
static void
add_memrange (struct collection_list *memranges, 
	      int type, bfd_signed_vma base,
	      unsigned long len)
{
  if (info_verbose)
    {
      printf_filtered ("(%d,", type);
      printf_vma (base);
      printf_filtered (",%ld)\n", len);
    }

  /* type: memrange_absolute == memory, other n == basereg */
  memranges->list[memranges->next_memrange].type = type;
  /* base: addr if memory, offset if reg relative.  */
  memranges->list[memranges->next_memrange].start = base;
  /* len: we actually save end (base + len) for convenience */
  memranges->list[memranges->next_memrange].end = base + len;
  memranges->next_memrange++;
  if (memranges->next_memrange >= memranges->listsize)
    {
      memranges->listsize *= 2;
      memranges->list = xrealloc (memranges->list,
				  memranges->listsize);
    }

  if (type != memrange_absolute)		/* Better collect the base register!  */
    add_register (memranges, type);
}

/* Add a symbol to a collection list.  */
static void
collect_symbol (struct collection_list *collect, 
		struct symbol *sym,
		struct gdbarch *gdbarch,
		long frame_regno, long frame_offset,
		CORE_ADDR scope)
{
  unsigned long len;
  unsigned int reg;
  bfd_signed_vma offset;

  len = TYPE_LENGTH (check_typedef (SYMBOL_TYPE (sym)));
  switch (SYMBOL_CLASS (sym))
    {
    default:
      printf_filtered ("%s: don't know symbol class %d\n",
		       SYMBOL_PRINT_NAME (sym),
		       SYMBOL_CLASS (sym));
      break;
    case LOC_CONST:
      printf_filtered ("constant %s (value %ld) will not be collected.\n",
		       SYMBOL_PRINT_NAME (sym), SYMBOL_VALUE (sym));
      break;
    case LOC_STATIC:
      offset = SYMBOL_VALUE_ADDRESS (sym);
      if (info_verbose)
	{
	  char tmp[40];

	  sprintf_vma (tmp, offset);
	  printf_filtered ("LOC_STATIC %s: collect %ld bytes at %s.\n",
			   SYMBOL_PRINT_NAME (sym), len,
			   tmp /* address */);
	}
      add_memrange (collect, memrange_absolute, offset, len);
      break;
    case LOC_REGISTER:
      reg = SYMBOL_REGISTER_OPS (sym)->register_number (sym, gdbarch);
      if (info_verbose)
	printf_filtered ("LOC_REG[parm] %s: ", 
			 SYMBOL_PRINT_NAME (sym));
      add_register (collect, reg);
      /* Check for doubles stored in two registers.  */
      /* FIXME: how about larger types stored in 3 or more regs?  */
      if (TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_FLT &&
	  len > register_size (gdbarch, reg))
	add_register (collect, reg + 1);
      break;
    case LOC_REF_ARG:
      printf_filtered ("Sorry, don't know how to do LOC_REF_ARG yet.\n");
      printf_filtered ("       (will not collect %s)\n",
		       SYMBOL_PRINT_NAME (sym));
      break;
    case LOC_ARG:
      reg = frame_regno;
      offset = frame_offset + SYMBOL_VALUE (sym);
      if (info_verbose)
	{
	  printf_filtered ("LOC_LOCAL %s: Collect %ld bytes at offset ",
			   SYMBOL_PRINT_NAME (sym), len);
	  printf_vma (offset);
	  printf_filtered (" from frame ptr reg %d\n", reg);
	}
      add_memrange (collect, reg, offset, len);
      break;
    case LOC_REGPARM_ADDR:
      reg = SYMBOL_VALUE (sym);
      offset = 0;
      if (info_verbose)
	{
	  printf_filtered ("LOC_REGPARM_ADDR %s: Collect %ld bytes at offset ",
			   SYMBOL_PRINT_NAME (sym), len);
	  printf_vma (offset);
	  printf_filtered (" from reg %d\n", reg);
	}
      add_memrange (collect, reg, offset, len);
      break;
    case LOC_LOCAL:
      reg = frame_regno;
      offset = frame_offset + SYMBOL_VALUE (sym);
      if (info_verbose)
	{
	  printf_filtered ("LOC_LOCAL %s: Collect %ld bytes at offset ",
			   SYMBOL_PRINT_NAME (sym), len);
	  printf_vma (offset);
	  printf_filtered (" from frame ptr reg %d\n", reg);
	}
      add_memrange (collect, reg, offset, len);
      break;
    case LOC_UNRESOLVED:
      printf_filtered ("Don't know LOC_UNRESOLVED %s\n", 
		       SYMBOL_PRINT_NAME (sym));
      break;
    case LOC_OPTIMIZED_OUT:
      printf_filtered ("%s has been optimized out of existence.\n",
		       SYMBOL_PRINT_NAME (sym));
      break;

    case LOC_COMPUTED:
      {
	struct agent_expr *aexpr;
	struct cleanup *old_chain1 = NULL;
	struct agent_reqs areqs;

	aexpr = gen_trace_for_var (scope, sym);

	old_chain1 = make_cleanup_free_agent_expr (aexpr);

	ax_reqs (aexpr, &areqs);
	if (areqs.flaw != agent_flaw_none)
	  error (_("malformed expression"));
	
	if (areqs.min_height < 0)
	  error (_("gdb: Internal error: expression has min height < 0"));
	if (areqs.max_height > 20)
	  error (_("expression too complicated, try simplifying"));

	discard_cleanups (old_chain1);
	add_aexpr (collect, aexpr);

	/* take care of the registers */
	if (areqs.reg_mask_len > 0)
	  {
	    int ndx1, ndx2;

	    for (ndx1 = 0; ndx1 < areqs.reg_mask_len; ndx1++)
	      {
		QUIT;	/* allow user to bail out with ^C */
		if (areqs.reg_mask[ndx1] != 0)
		  {
		    /* assume chars have 8 bits */
		    for (ndx2 = 0; ndx2 < 8; ndx2++)
		      if (areqs.reg_mask[ndx1] & (1 << ndx2))
			/* it's used -- record it */
			add_register (collect, 
				      ndx1 * 8 + ndx2);
		  }
	      }
	  }
      }
      break;
    }
}

/* Add all locals (or args) symbols to collection list */
static void
add_local_symbols (struct collection_list *collect,
		   struct gdbarch *gdbarch, CORE_ADDR pc,
		   long frame_regno, long frame_offset, int type)
{
  struct symbol *sym;
  struct block *block;
  struct dict_iterator iter;
  int count = 0;

  block = block_for_pc (pc);
  while (block != 0)
    {
      QUIT;			/* allow user to bail out with ^C */
      ALL_BLOCK_SYMBOLS (block, iter, sym)
	{
	  if (SYMBOL_IS_ARGUMENT (sym)
	      ? type == 'A'	/* collecting Arguments */
	      : type == 'L')	/* collecting Locals */
	    {
	      count++;
	      collect_symbol (collect, sym, gdbarch,
			      frame_regno, frame_offset, pc);
	    }
	}
      if (BLOCK_FUNCTION (block))
	break;
      else
	block = BLOCK_SUPERBLOCK (block);
    }
  if (count == 0)
    warning (_("No %s found in scope."), 
	     type == 'L' ? "locals" : "args");
}

/* worker function */
static void
clear_collection_list (struct collection_list *list)
{
  int ndx;

  list->next_memrange = 0;
  for (ndx = 0; ndx < list->next_aexpr_elt; ndx++)
    {
      free_agent_expr (list->aexpr_list[ndx]);
      list->aexpr_list[ndx] = NULL;
    }
  list->next_aexpr_elt = 0;
  memset (list->regs_mask, 0, sizeof (list->regs_mask));
}

/* reduce a collection list to string form (for gdb protocol) */
static char **
stringify_collection_list (struct collection_list *list, char *string)
{
  char temp_buf[2048];
  char tmp2[40];
  int count;
  int ndx = 0;
  char *(*str_list)[];
  char *end;
  long i;

  count = 1 + list->next_memrange + list->next_aexpr_elt + 1;
  str_list = (char *(*)[]) xmalloc (count * sizeof (char *));

  for (i = sizeof (list->regs_mask) - 1; i > 0; i--)
    if (list->regs_mask[i] != 0)	/* skip leading zeroes in regs_mask */
      break;
  if (list->regs_mask[i] != 0)	/* prepare to send regs_mask to the stub */
    {
      if (info_verbose)
	printf_filtered ("\nCollecting registers (mask): 0x");
      end = temp_buf;
      *end++ = 'R';
      for (; i >= 0; i--)
	{
	  QUIT;			/* allow user to bail out with ^C */
	  if (info_verbose)
	    printf_filtered ("%02X", list->regs_mask[i]);
	  sprintf (end, "%02X", list->regs_mask[i]);
	  end += 2;
	}
      (*str_list)[ndx] = xstrdup (temp_buf);
      ndx++;
    }
  if (info_verbose)
    printf_filtered ("\n");
  if (list->next_memrange > 0 && info_verbose)
    printf_filtered ("Collecting memranges: \n");
  for (i = 0, count = 0, end = temp_buf; i < list->next_memrange; i++)
    {
      QUIT;			/* allow user to bail out with ^C */
      sprintf_vma (tmp2, list->list[i].start);
      if (info_verbose)
	{
	  printf_filtered ("(%d, %s, %ld)\n", 
			   list->list[i].type, 
			   tmp2, 
			   (long) (list->list[i].end - list->list[i].start));
	}
      if (count + 27 > MAX_AGENT_EXPR_LEN)
	{
	  (*str_list)[ndx] = savestring (temp_buf, count);
	  ndx++;
	  count = 0;
	  end = temp_buf;
	}

      {
        bfd_signed_vma length = list->list[i].end - list->list[i].start;

        /* The "%X" conversion specifier expects an unsigned argument,
           so passing -1 (memrange_absolute) to it directly gives you
           "FFFFFFFF" (or more, depending on sizeof (unsigned)).
           Special-case it.  */
        if (list->list[i].type == memrange_absolute)
          sprintf (end, "M-1,%s,%lX", tmp2, (long) length);
        else
          sprintf (end, "M%X,%s,%lX", list->list[i].type, tmp2, (long) length);
      }

      count += strlen (end);
      end = temp_buf + count;
    }

  for (i = 0; i < list->next_aexpr_elt; i++)
    {
      QUIT;			/* allow user to bail out with ^C */
      if ((count + 10 + 2 * list->aexpr_list[i]->len) > MAX_AGENT_EXPR_LEN)
	{
	  (*str_list)[ndx] = savestring (temp_buf, count);
	  ndx++;
	  count = 0;
	  end = temp_buf;
	}
      sprintf (end, "X%08X,", list->aexpr_list[i]->len);
      end += 10;		/* 'X' + 8 hex digits + ',' */
      count += 10;

      end = mem2hex (list->aexpr_list[i]->buf, 
		     end, list->aexpr_list[i]->len);
      count += 2 * list->aexpr_list[i]->len;
    }

  if (count != 0)
    {
      (*str_list)[ndx] = savestring (temp_buf, count);
      ndx++;
      count = 0;
      end = temp_buf;
    }
  (*str_list)[ndx] = NULL;

  if (ndx == 0)
    {
      xfree (str_list);
      return NULL;
    }
  else
    return *str_list;
}

/* Render all actions into gdb protocol.  */
/*static*/ void
encode_actions (struct breakpoint *t, char ***tdp_actions,
		char ***stepping_actions)
{
  static char tdp_buff[2048], step_buff[2048];
  char *action_exp;
  struct expression *exp = NULL;
  struct action_line *action;
  int i;
  struct value *tempval;
  struct collection_list *collect;
  struct cmd_list_element *cmd;
  struct agent_expr *aexpr;
  int frame_reg;
  LONGEST frame_offset;
  char *default_collect_line = NULL;
  struct action_line *default_collect_action = NULL;

  clear_collection_list (&tracepoint_list);
  clear_collection_list (&stepping_list);
  collect = &tracepoint_list;

  *tdp_actions = NULL;
  *stepping_actions = NULL;

  gdbarch_virtual_frame_pointer (t->gdbarch,
				 t->loc->address, &frame_reg, &frame_offset);

  action = t->actions;

  /* If there are default expressions to collect, make up a collect
     action and prepend to the action list to encode.  Note that since
     validation is per-tracepoint (local var "xyz" might be valid for
     one tracepoint and not another, etc), we make up the action on
     the fly, and don't cache it.  */
  if (*default_collect)
    {
      char *line;
      enum actionline_type linetype;

      default_collect_line = xmalloc (12 + strlen (default_collect));
      sprintf (default_collect_line, "collect %s", default_collect);
      line = default_collect_line;
      linetype = validate_actionline (&line, t);
      if (linetype != BADLINE)
	{
	  default_collect_action = xmalloc (sizeof (struct action_line));
	  default_collect_action->next = t->actions;
	  default_collect_action->action = line;
	  action = default_collect_action;
	}
    }

  for (; action; action = action->next)
    {
      QUIT;			/* allow user to bail out with ^C */
      action_exp = action->action;
      while (isspace ((int) *action_exp))
	action_exp++;

      if (*action_exp == '#')	/* comment line */
	return;

      cmd = lookup_cmd (&action_exp, cmdlist, "", -1, 1);
      if (cmd == 0)
	error (_("Bad action list item: %s"), action_exp);

      if (cmd_cfunc_eq (cmd, collect_pseudocommand))
	{
	  do
	    {			/* repeat over a comma-separated list */
	      QUIT;		/* allow user to bail out with ^C */
	      while (isspace ((int) *action_exp))
		action_exp++;

	      if (0 == strncasecmp ("$reg", action_exp, 4))
		{
		  for (i = 0; i < gdbarch_num_regs (t->gdbarch); i++)
		    add_register (collect, i);
		  action_exp = strchr (action_exp, ',');	/* more? */
		}
	      else if (0 == strncasecmp ("$arg", action_exp, 4))
		{
		  add_local_symbols (collect,
				     t->gdbarch,
				     t->loc->address,
				     frame_reg,
				     frame_offset,
				     'A');
		  action_exp = strchr (action_exp, ',');	/* more? */
		}
	      else if (0 == strncasecmp ("$loc", action_exp, 4))
		{
		  add_local_symbols (collect,
				     t->gdbarch,
				     t->loc->address,
				     frame_reg,
				     frame_offset,
				     'L');
		  action_exp = strchr (action_exp, ',');	/* more? */
		}
	      else
		{
		  unsigned long addr, len;
		  struct cleanup *old_chain = NULL;
		  struct cleanup *old_chain1 = NULL;
		  struct agent_reqs areqs;

		  exp = parse_exp_1 (&action_exp, 
				     block_for_pc (t->loc->address), 1);
		  old_chain = make_cleanup (free_current_contents, &exp);

		  switch (exp->elts[0].opcode)
		    {
		    case OP_REGISTER:
		      {
			const char *name = &exp->elts[2].string;

			i = user_reg_map_name_to_regnum (t->gdbarch,
							 name, strlen (name));
			if (i == -1)
			  internal_error (__FILE__, __LINE__,
					  _("Register $%s not available"),
					  name);
			if (info_verbose)
			  printf_filtered ("OP_REGISTER: ");
			add_register (collect, i);
			break;
		      }

		    case UNOP_MEMVAL:
		      /* safe because we know it's a simple expression */
		      tempval = evaluate_expression (exp);
		      addr = value_address (tempval);
		      len = TYPE_LENGTH (check_typedef (exp->elts[1].type));
		      add_memrange (collect, memrange_absolute, addr, len);
		      break;

		    case OP_VAR_VALUE:
		      collect_symbol (collect,
				      exp->elts[2].symbol,
				      t->gdbarch,
				      frame_reg,
				      frame_offset,
				      t->loc->address);
		      break;

		    default:	/* full-fledged expression */
		      aexpr = gen_trace_for_expr (t->loc->address, exp);

		      old_chain1 = make_cleanup_free_agent_expr (aexpr);

		      ax_reqs (aexpr, &areqs);
		      if (areqs.flaw != agent_flaw_none)
			error (_("malformed expression"));

		      if (areqs.min_height < 0)
			error (_("gdb: Internal error: expression has min height < 0"));
		      if (areqs.max_height > 20)
			error (_("expression too complicated, try simplifying"));

		      discard_cleanups (old_chain1);
		      add_aexpr (collect, aexpr);

		      /* take care of the registers */
		      if (areqs.reg_mask_len > 0)
			{
			  int ndx1;
			  int ndx2;

			  for (ndx1 = 0; ndx1 < areqs.reg_mask_len; ndx1++)
			    {
			      QUIT;	/* allow user to bail out with ^C */
			      if (areqs.reg_mask[ndx1] != 0)
				{
				  /* assume chars have 8 bits */
				  for (ndx2 = 0; ndx2 < 8; ndx2++)
				    if (areqs.reg_mask[ndx1] & (1 << ndx2))
				      /* it's used -- record it */
				      add_register (collect, 
						    ndx1 * 8 + ndx2);
				}
			    }
			}
		      break;
		    }		/* switch */
		  do_cleanups (old_chain);
		}		/* do */
	    }
	  while (action_exp && *action_exp++ == ',');
	}			/* if */
      else if (cmd_cfunc_eq (cmd, teval_pseudocommand))
	{
	  do
	    {			/* repeat over a comma-separated list */
	      QUIT;		/* allow user to bail out with ^C */
	      while (isspace ((int) *action_exp))
		action_exp++;

		{
		  unsigned long addr, len;
		  struct cleanup *old_chain = NULL;
		  struct cleanup *old_chain1 = NULL;
		  struct agent_reqs areqs;

		  exp = parse_exp_1 (&action_exp, 
				     block_for_pc (t->loc->address), 1);
		  old_chain = make_cleanup (free_current_contents, &exp);

		  aexpr = gen_eval_for_expr (t->loc->address, exp);
		  old_chain1 = make_cleanup_free_agent_expr (aexpr);

		  ax_reqs (aexpr, &areqs);
		  if (areqs.flaw != agent_flaw_none)
		    error (_("malformed expression"));

		  if (areqs.min_height < 0)
		    error (_("gdb: Internal error: expression has min height < 0"));
		  if (areqs.max_height > 20)
		    error (_("expression too complicated, try simplifying"));

		  discard_cleanups (old_chain1);
		  /* Even though we're not officially collecting, add
		     to the collect list anyway.  */
		  add_aexpr (collect, aexpr);

		  do_cleanups (old_chain);
		}		/* do */
	    }
	  while (action_exp && *action_exp++ == ',');
	}			/* if */
      else if (cmd_cfunc_eq (cmd, while_stepping_pseudocommand))
	{
	  collect = &stepping_list;
	}
      else if (cmd_cfunc_eq (cmd, end_actions_pseudocommand))
	{
	  if (collect == &stepping_list)	/* end stepping actions */
	    collect = &tracepoint_list;
	  else
	    break;		/* end tracepoint actions */
	}
    }				/* for */
  memrange_sortmerge (&tracepoint_list);
  memrange_sortmerge (&stepping_list);

  *tdp_actions = stringify_collection_list (&tracepoint_list, 
					    tdp_buff);
  *stepping_actions = stringify_collection_list (&stepping_list, 
						 step_buff);

  xfree (default_collect_line);
  xfree (default_collect_action);
}

static void
add_aexpr (struct collection_list *collect, struct agent_expr *aexpr)
{
  if (collect->next_aexpr_elt >= collect->aexpr_listsize)
    {
      collect->aexpr_list =
	xrealloc (collect->aexpr_list,
		2 * collect->aexpr_listsize * sizeof (struct agent_expr *));
      collect->aexpr_listsize *= 2;
    }
  collect->aexpr_list[collect->next_aexpr_elt] = aexpr;
  collect->next_aexpr_elt++;
}

/* tstart command:

   Tell target to clear any previous trace experiment.
   Walk the list of tracepoints, and send them (and their actions)
   to the target.  If no errors, 
   Tell target to start a new trace experiment.  */

static void
trace_start_command (char *args, int from_tty)
{
  char buf[2048];
  VEC(breakpoint_p) *tp_vec = NULL;
  int ix;
  struct breakpoint *t;
  struct trace_state_variable *tsv;
  int any_downloaded = 0;

  dont_repeat ();	/* Like "run", dangerous to repeat accidentally.  */

  target_trace_init ();
  
  tp_vec = all_tracepoints ();
  for (ix = 0; VEC_iterate (breakpoint_p, tp_vec, ix, t); ix++)
    {
      t->number_on_target = 0;
      target_download_tracepoint (t);
      t->number_on_target = t->number;
      any_downloaded = 1;
    }
  VEC_free (breakpoint_p, tp_vec);
  
  /* No point in tracing without any tracepoints... */
  if (!any_downloaded)
    error ("No tracepoints downloaded, not starting trace");
  
  /* Send down all the trace state variables too.  */
  for (ix = 0; VEC_iterate (tsv_s, tvariables, ix, tsv); ++ix)
    {
      target_download_trace_state_variable (tsv);
    }
  
  /* Tell target to treat text-like sections as transparent.  */
  target_trace_set_readonly_regions ();

  /* Now insert traps and begin collecting data.  */
  target_trace_start ();

  /* Reset our local state.  */
  set_traceframe_num (-1);
  set_tracepoint_num (-1);
  set_traceframe_context (NULL);
  current_trace_status()->running = 1;
}

/* tstop command */
static void
trace_stop_command (char *args, int from_tty)
{
  stop_tracing ();
}

void
stop_tracing ()
{
  target_trace_stop ();
  /* should change in response to reply? */
  current_trace_status ()->running = 0;
}

/* tstatus command */
static void
trace_status_command (char *args, int from_tty)
{
  struct trace_status *ts = current_trace_status ();
  int status;
  
  status = target_get_trace_status (ts);

  if (status == -1)
    {
      if (ts->from_file)
	printf_filtered (_("Using a trace file.\n"));
      else
	{
	  printf_filtered (_("Trace can not be run on this target.\n"));
	  return;
	}
    }

  if (!ts->running_known)
    {
      printf_filtered (_("Run/stop status is unknown.\n"));
    }
  else if (ts->running)
    {
      printf_filtered (_("Trace is running on the target.\n"));
      if (disconnected_tracing)
	printf_filtered (_("Trace will continue if GDB disconnects.\n"));
      else
	printf_filtered (_("Trace will stop if GDB disconnects.\n"));
    }
  else
    {
      switch (ts->stop_reason)
	{
	case trace_never_run:
	  printf_filtered (_("No trace has been run on the target.\n"));
	  break;
	case tstop_command:
	  printf_filtered (_("Trace stopped by a tstop command.\n"));
	  break;
	case trace_buffer_full:
	  printf_filtered (_("Trace stopped because the buffer was full.\n"));
	  break;
	case trace_disconnected:
	  printf_filtered (_("Trace stopped because of disconnection.\n"));
	  break;
	case tracepoint_passcount:
	  /* FIXME account for number on target */
	  printf_filtered (_("Trace stopped by tracepoint %d.\n"),
			   ts->stopping_tracepoint);
	  break;
	case trace_stop_reason_unknown:
	  printf_filtered (_("Trace stopped for an unknown reason.\n"));
	  break;
	default:
	  printf_filtered (_("Trace stopped for some other reason (%d).\n"),
			   ts->stop_reason);
	  break;
	}
    }

  if (ts->traceframe_count >= 0)
    {
      printf_filtered (_("Collected %d trace frames.\n"),
		       ts->traceframe_count);
    }

  if (ts->buffer_free)
    {
      printf_filtered (_("Trace buffer has %llu bytes free.\n"),
		       ts->buffer_free);
    }

  /* Now report on what we're doing with tfind.  */
  if (traceframe_number >= 0)
    printf_filtered (_("Looking at trace frame %d, tracepoint %d.\n"),
		     traceframe_number, tracepoint_number);
  else
    printf_filtered (_("Not looking at any trace frame.\n"));
}

void
disconnect_or_stop_tracing (int from_tty)
{
  /* It can happen that the target that was tracing went away on its
     own, and we didn't notice.  Get a status update, and if the
     current target doesn't even do tracing, then assume it's not
     running anymore.  */
  if (target_get_trace_status (current_trace_status ()) < 0)
    current_trace_status ()->running = 0;

  if (current_trace_status ()->running && from_tty)
    {
      int cont = query (_("Trace is running.  Continue tracing after detach? "));
      /* Note that we send the query result without affecting the
	 user's setting of disconnected_tracing, so that the answer is
	 a one-time-only.  */
      send_disconnected_tracing_value (cont);

      /* Also ensure that we do the equivalent of a tstop command if
	 tracing is not to continue after the detach.  */
      if (!cont)
	stop_tracing ();
    }
}

/* Worker function for the various flavors of the tfind command.  */
static void
finish_tfind_command (enum trace_find_type type, int num,
		      ULONGEST addr1, ULONGEST addr2,
		      int from_tty)
{
  int target_frameno = -1, target_tracept = -1;
  struct frame_id old_frame_id;
  char *reply;
  struct breakpoint *tp;

  old_frame_id = get_frame_id (get_current_frame ());

  target_frameno = target_trace_find (type, num, addr1, addr2,
				      &target_tracept);
  
  if (type == tfind_number
      && num == -1
      && target_frameno == -1)
    {
      /* We told the target to get out of tfind mode, and it did.  */
    }
  else if (target_frameno == -1)
    {
      /* A request for a non-existant trace frame has failed.
	 Our response will be different, depending on FROM_TTY:

	 If FROM_TTY is true, meaning that this command was 
	 typed interactively by the user, then give an error
	 and DO NOT change the state of traceframe_number etc.

	 However if FROM_TTY is false, meaning that we're either
	 in a script, a loop, or a user-defined command, then 
	 DON'T give an error, but DO change the state of
	 traceframe_number etc. to invalid.

	 The rationalle is that if you typed the command, you
	 might just have committed a typo or something, and you'd
	 like to NOT lose your current debugging state.  However
	 if you're in a user-defined command or especially in a
	 loop, then you need a way to detect that the command
	 failed WITHOUT aborting.  This allows you to write
	 scripts that search thru the trace buffer until the end,
	 and then continue on to do something else.  */
  
      if (from_tty)
	error (_("Target failed to find requested trace frame."));
      else
	{
	  if (info_verbose)
	    printf_filtered ("End of trace buffer.\n");
#if 0 /* dubious now? */
	  /* The following will not recurse, since it's
	     special-cased.  */
	  trace_find_command ("-1", from_tty);
#endif
	}
    }
  
  tp = get_tracepoint_by_number_on_target (target_tracept);

  reinit_frame_cache ();
  registers_changed ();
  set_traceframe_num (target_frameno);
  set_tracepoint_num (tp ? tp->number : target_tracept);
  if (target_frameno == -1)
    set_traceframe_context (NULL);
  else
    set_traceframe_context (get_current_frame ());

  /* If we're in nonstop mode and getting out of looking at trace
     frames, there won't be any current frame to go back to and
     display.  */
  if (from_tty
      && (has_stack_frames () || traceframe_number >= 0))
    {
      enum print_what print_what;

      /* NOTE: in immitation of the step command, try to determine
         whether we have made a transition from one function to
         another.  If so, we'll print the "stack frame" (ie. the new
         function and it's arguments) -- otherwise we'll just show the
         new source line.  */

      if (frame_id_eq (old_frame_id,
		       get_frame_id (get_current_frame ())))
	print_what = SRC_LINE;
      else
	print_what = SRC_AND_LOC;

      print_stack_frame (get_selected_frame (NULL), 1, print_what);
      do_displays ();
    }
}

/* trace_find_command takes a trace frame number n, 
   sends "QTFrame:<n>" to the target, 
   and accepts a reply that may contain several optional pieces
   of information: a frame number, a tracepoint number, and an
   indication of whether this is a trap frame or a stepping frame.

   The minimal response is just "OK" (which indicates that the 
   target does not give us a frame number or a tracepoint number).
   Instead of that, the target may send us a string containing
   any combination of:
   F<hexnum>    (gives the selected frame number)
   T<hexnum>    (gives the selected tracepoint number)
 */

/* tfind command */
static void
trace_find_command (char *args, int from_tty)
{ /* this should only be called with a numeric argument */
  int frameno = -1;

  if (current_trace_status ()->running && !current_trace_status ()->from_file)
    error ("May not look at trace frames while trace is running.");
  
  if (args == 0 || *args == 0)
    { /* TFIND with no args means find NEXT trace frame.  */
      if (traceframe_number == -1)
	frameno = 0;	/* "next" is first one */
        else
	frameno = traceframe_number + 1;
    }
  else if (0 == strcmp (args, "-"))
    {
      if (traceframe_number == -1)
	error (_("not debugging trace buffer"));
      else if (from_tty && traceframe_number == 0)
	error (_("already at start of trace buffer"));
      
      frameno = traceframe_number - 1;
      }
  /* A hack to work around eval's need for fp to have been collected.  */
  else if (0 == strcmp (args, "-1"))
    frameno = -1;
  else
    frameno = parse_and_eval_long (args);

  if (frameno < -1)
    error (_("invalid input (%d is less than zero)"), frameno);

  finish_tfind_command (tfind_number, frameno, 0, 0, from_tty);
}

/* tfind end */
static void
trace_find_end_command (char *args, int from_tty)
{
  trace_find_command ("-1", from_tty);
}

/* tfind none */
static void
trace_find_none_command (char *args, int from_tty)
{
  trace_find_command ("-1", from_tty);
}

/* tfind start */
static void
trace_find_start_command (char *args, int from_tty)
{
  trace_find_command ("0", from_tty);
}

/* tfind pc command */
static void
trace_find_pc_command (char *args, int from_tty)
{
  CORE_ADDR pc;
  char tmp[40];

  if (current_trace_status ()->running && !current_trace_status ()->from_file)
    error ("May not look at trace frames while trace is running.");

  if (args == 0 || *args == 0)
    pc = regcache_read_pc (get_current_regcache ());
  else
    pc = parse_and_eval_address (args);

  finish_tfind_command (tfind_pc, 0, pc, 0, from_tty);
}

/* tfind tracepoint command */
static void
trace_find_tracepoint_command (char *args, int from_tty)
{
  int tdp;
  struct breakpoint *tp;

  if (current_trace_status ()->running && !current_trace_status ()->from_file)
    error ("May not look at trace frames while trace is running.");

  if (args == 0 || *args == 0)
    {
      if (tracepoint_number == -1)
	error (_("No current tracepoint -- please supply an argument."));
      else
	tdp = tracepoint_number;	/* default is current TDP */
    }
  else
    tdp = parse_and_eval_long (args);

  /* If we have the tracepoint on hand, use the number that the
     target knows about (which may be different if we disconnected
     and reconnected).  */
  tp = get_tracepoint (tdp);
  if (tp)
    tdp = tp->number_on_target;

  finish_tfind_command (tfind_tp, tdp, 0, 0, from_tty);
}

/* TFIND LINE command:

   This command will take a sourceline for argument, just like BREAK
   or TRACE (ie. anything that "decode_line_1" can handle).

   With no argument, this command will find the next trace frame 
   corresponding to a source line OTHER THAN THE CURRENT ONE.  */

static void
trace_find_line_command (char *args, int from_tty)
{
  static CORE_ADDR start_pc, end_pc;
  struct symtabs_and_lines sals;
  struct symtab_and_line sal;
  struct cleanup *old_chain;
  char   startpc_str[40], endpc_str[40];

  if (current_trace_status ()->running && !current_trace_status ()->from_file)
    error ("May not look at trace frames while trace is running.");

  if (args == 0 || *args == 0)
    {
      sal = find_pc_line (get_frame_pc (get_current_frame ()), 0);
      sals.nelts = 1;
      sals.sals = (struct symtab_and_line *)
	xmalloc (sizeof (struct symtab_and_line));
      sals.sals[0] = sal;
    }
  else
      {
      sals = decode_line_spec (args, 1);
      sal = sals.sals[0];
    }
  
  old_chain = make_cleanup (xfree, sals.sals);
  if (sal.symtab == 0)
    {
      printf_filtered ("TFIND: No line number information available");
      if (sal.pc != 0)
	{
	  /* This is useful for "info line *0x7f34".  If we can't
	     tell the user about a source line, at least let them
	     have the symbolic address.  */
	  printf_filtered (" for address ");
	  wrap_here ("  ");
	  print_address (get_current_arch (), sal.pc, gdb_stdout);
	  printf_filtered (";\n -- will attempt to find by PC. \n");
  	}
        else
  	{
	  printf_filtered (".\n");
	  return;		/* No line, no PC; what can we do?  */
  	}
    }
  else if (sal.line > 0
	   && find_line_pc_range (sal, &start_pc, &end_pc))
    {
      if (start_pc == end_pc)
  	{
	  printf_filtered ("Line %d of \"%s\"",
			   sal.line, sal.symtab->filename);
	  wrap_here ("  ");
	  printf_filtered (" is at address ");
	  print_address (get_current_arch (), start_pc, gdb_stdout);
	  wrap_here ("  ");
	  printf_filtered (" but contains no code.\n");
	  sal = find_pc_line (start_pc, 0);
	  if (sal.line > 0
	      && find_line_pc_range (sal, &start_pc, &end_pc)
	      && start_pc != end_pc)
	    printf_filtered ("Attempting to find line %d instead.\n",
			     sal.line);
  	  else
	    error (_("Cannot find a good line."));
  	}
      }
    else
    /* Is there any case in which we get here, and have an address
       which the user would want to see?  If we have debugging
       symbols and no line numbers?  */
    error (_("Line number %d is out of range for \"%s\"."),
	   sal.line, sal.symtab->filename);

  /* Find within range of stated line.  */
  if (args && *args)
    finish_tfind_command (tfind_range, 0, start_pc, end_pc - 1, from_tty);
  else
    finish_tfind_command (tfind_outside, 0, start_pc, end_pc - 1, from_tty);
  do_cleanups (old_chain);
}

/* tfind range command */
static void
trace_find_range_command (char *args, int from_tty)
{
  static CORE_ADDR start, stop;
  char start_str[40], stop_str[40];
  char *tmp;

  if (current_trace_status ()->running && !current_trace_status ()->from_file)
    error ("May not look at trace frames while trace is running.");

  if (args == 0 || *args == 0)
    { /* XXX FIXME: what should default behavior be?  */
      printf_filtered ("Usage: tfind range <startaddr>,<endaddr>\n");
      return;
    }

  if (0 != (tmp = strchr (args, ',')))
    {
      *tmp++ = '\0';	/* terminate start address */
      while (isspace ((int) *tmp))
	tmp++;
      start = parse_and_eval_address (args);
      stop = parse_and_eval_address (tmp);
    }
  else
    {			/* no explicit end address? */
      start = parse_and_eval_address (args);
      stop = start + 1;	/* ??? */
    }

  finish_tfind_command (tfind_range, 0, start, stop, from_tty);
}

/* tfind outside command */
static void
trace_find_outside_command (char *args, int from_tty)
{
  CORE_ADDR start, stop;
  char start_str[40], stop_str[40];
  char *tmp;

  if (current_trace_status ()->running && !current_trace_status ()->from_file)
    error ("May not look at trace frames while trace is running.");

  if (args == 0 || *args == 0)
    { /* XXX FIXME: what should default behavior be? */
      printf_filtered ("Usage: tfind outside <startaddr>,<endaddr>\n");
      return;
    }

  if (0 != (tmp = strchr (args, ',')))
    {
      *tmp++ = '\0';	/* terminate start address */
      while (isspace ((int) *tmp))
	tmp++;
      start = parse_and_eval_address (args);
      stop = parse_and_eval_address (tmp);
    }
  else
    {			/* no explicit end address? */
      start = parse_and_eval_address (args);
      stop = start + 1;	/* ??? */
    }

  finish_tfind_command (tfind_outside, 0, start, stop, from_tty);
}

/* info scope command: list the locals for a scope.  */
static void
scope_info (char *args, int from_tty)
{
  struct symtabs_and_lines sals;
  struct symbol *sym;
  struct minimal_symbol *msym;
  struct block *block;
  char **canonical, *symname, *save_args = args;
  struct dict_iterator iter;
  int j, count = 0;
  struct gdbarch *gdbarch;
  int regno;

  if (args == 0 || *args == 0)
    error (_("requires an argument (function, line or *addr) to define a scope"));

  sals = decode_line_1 (&args, 1, NULL, 0, &canonical, NULL);
  if (sals.nelts == 0)
    return;		/* presumably decode_line_1 has already warned */

  /* Resolve line numbers to PC */
  resolve_sal_pc (&sals.sals[0]);
  block = block_for_pc (sals.sals[0].pc);

  while (block != 0)
    {
      QUIT;			/* allow user to bail out with ^C */
      ALL_BLOCK_SYMBOLS (block, iter, sym)
	{
	  QUIT;			/* allow user to bail out with ^C */
	  if (count == 0)
	    printf_filtered ("Scope for %s:\n", save_args);
	  count++;

	  symname = SYMBOL_PRINT_NAME (sym);
	  if (symname == NULL || *symname == '\0')
	    continue;		/* probably botched, certainly useless */

	  gdbarch = get_objfile_arch (SYMBOL_SYMTAB (sym)->objfile);

	  printf_filtered ("Symbol %s is ", symname);
	  switch (SYMBOL_CLASS (sym))
	    {
	    default:
	    case LOC_UNDEF:	/* messed up symbol? */
	      printf_filtered ("a bogus symbol, class %d.\n",
			       SYMBOL_CLASS (sym));
	      count--;		/* don't count this one */
	      continue;
	    case LOC_CONST:
	      printf_filtered ("a constant with value %ld (0x%lx)",
			       SYMBOL_VALUE (sym), SYMBOL_VALUE (sym));
	      break;
	    case LOC_CONST_BYTES:
	      printf_filtered ("constant bytes: ");
	      if (SYMBOL_TYPE (sym))
		for (j = 0; j < TYPE_LENGTH (SYMBOL_TYPE (sym)); j++)
		  fprintf_filtered (gdb_stdout, " %02x",
				    (unsigned) SYMBOL_VALUE_BYTES (sym)[j]);
	      break;
	    case LOC_STATIC:
	      printf_filtered ("in static storage at address ");
	      printf_filtered ("%s", paddress (gdbarch,
					       SYMBOL_VALUE_ADDRESS (sym)));
	      break;
	    case LOC_REGISTER:
	      /* GDBARCH is the architecture associated with the objfile
		 the symbol is defined in; the target architecture may be
		 different, and may provide additional registers.  However,
		 we do not know the target architecture at this point.
		 We assume the objfile architecture will contain all the
		 standard registers that occur in debug info in that
		 objfile.  */
	      regno = SYMBOL_REGISTER_OPS (sym)->register_number (sym, gdbarch);

	      if (SYMBOL_IS_ARGUMENT (sym))
		printf_filtered ("an argument in register $%s",
				 gdbarch_register_name (gdbarch, regno));
	      else
		printf_filtered ("a local variable in register $%s",
				 gdbarch_register_name (gdbarch, regno));
	      break;
	    case LOC_ARG:
	      printf_filtered ("an argument at stack/frame offset %ld",
			       SYMBOL_VALUE (sym));
	      break;
	    case LOC_LOCAL:
	      printf_filtered ("a local variable at frame offset %ld",
			       SYMBOL_VALUE (sym));
	      break;
	    case LOC_REF_ARG:
	      printf_filtered ("a reference argument at offset %ld",
			       SYMBOL_VALUE (sym));
	      break;
	    case LOC_REGPARM_ADDR:
	      /* Note comment at LOC_REGISTER.  */
	      regno = SYMBOL_REGISTER_OPS (sym)->register_number (sym, gdbarch);
	      printf_filtered ("the address of an argument, in register $%s",
			       gdbarch_register_name (gdbarch, regno));
	      break;
	    case LOC_TYPEDEF:
	      printf_filtered ("a typedef.\n");
	      continue;
	    case LOC_LABEL:
	      printf_filtered ("a label at address ");
	      printf_filtered ("%s", paddress (gdbarch,
					       SYMBOL_VALUE_ADDRESS (sym)));
	      break;
	    case LOC_BLOCK:
	      printf_filtered ("a function at address ");
	      printf_filtered ("%s",
		paddress (gdbarch, BLOCK_START (SYMBOL_BLOCK_VALUE (sym))));
	      break;
	    case LOC_UNRESOLVED:
	      msym = lookup_minimal_symbol (SYMBOL_LINKAGE_NAME (sym),
					    NULL, NULL);
	      if (msym == NULL)
		printf_filtered ("Unresolved Static");
	      else
		{
		  printf_filtered ("static storage at address ");
		  printf_filtered ("%s",
		    paddress (gdbarch, SYMBOL_VALUE_ADDRESS (msym)));
		}
	      break;
	    case LOC_OPTIMIZED_OUT:
	      printf_filtered ("optimized out.\n");
	      continue;
	    case LOC_COMPUTED:
	      SYMBOL_COMPUTED_OPS (sym)->describe_location (sym, gdb_stdout);
	      break;
	    }
	  if (SYMBOL_TYPE (sym))
	    printf_filtered (", length %d.\n",
			     TYPE_LENGTH (check_typedef (SYMBOL_TYPE (sym))));
	}
      if (BLOCK_FUNCTION (block))
	break;
      else
	block = BLOCK_SUPERBLOCK (block);
    }
  if (count <= 0)
    printf_filtered ("Scope for %s contains no locals or arguments.\n",
		     save_args);
}

/* worker function (cleanup) */
static void
replace_comma (void *data)
{
  char *comma = data;
  *comma = ',';
}

/* tdump command */
static void
trace_dump_command (char *args, int from_tty)
{
  struct regcache *regcache;
  struct gdbarch *gdbarch;
  struct breakpoint *t;
  struct action_line *action;
  char *action_exp, *next_comma;
  struct cleanup *old_cleanups;
  int stepping_actions = 0;
  int stepping_frame = 0;

  if (tracepoint_number == -1)
    {
      warning (_("No current trace frame."));
      return;
    }

  t = get_tracepoint (tracepoint_number);

  if (t == NULL)
    error (_("No known tracepoint matches 'current' tracepoint #%d."),
	   tracepoint_number);

  old_cleanups = make_cleanup (null_cleanup, NULL);

  printf_filtered ("Data collected at tracepoint %d, trace frame %d:\n",
		   tracepoint_number, traceframe_number);

  /* The current frame is a trap frame if the frame PC is equal
     to the tracepoint PC.  If not, then the current frame was
     collected during single-stepping.  */

  regcache = get_current_regcache ();
  gdbarch = get_regcache_arch (regcache);

  stepping_frame = (t->loc->address != (regcache_read_pc (regcache)));

  for (action = t->actions; action; action = action->next)
    {
      struct cmd_list_element *cmd;

      QUIT;			/* allow user to bail out with ^C */
      action_exp = action->action;
      while (isspace ((int) *action_exp))
	action_exp++;

      /* The collection actions to be done while stepping are
         bracketed by the commands "while-stepping" and "end".  */

      if (*action_exp == '#')	/* comment line */
	continue;

      cmd = lookup_cmd (&action_exp, cmdlist, "", -1, 1);
      if (cmd == 0)
	error (_("Bad action list item: %s"), action_exp);

      if (cmd_cfunc_eq (cmd, while_stepping_pseudocommand))
	stepping_actions = 1;
      else if (cmd_cfunc_eq (cmd, end_actions_pseudocommand))
	stepping_actions = 0;
      else if (cmd_cfunc_eq (cmd, collect_pseudocommand))
	{
	  /* Display the collected data.
	     For the trap frame, display only what was collected at
	     the trap.  Likewise for stepping frames, display only
	     what was collected while stepping.  This means that the
	     two boolean variables, STEPPING_FRAME and
	     STEPPING_ACTIONS should be equal.  */
	  if (stepping_frame == stepping_actions)
	    {
	      do
		{		/* repeat over a comma-separated list */
		  QUIT;		/* allow user to bail out with ^C */
		  if (*action_exp == ',')
		    action_exp++;
		  while (isspace ((int) *action_exp))
		    action_exp++;

		  next_comma = strchr (action_exp, ',');

		  if (0 == strncasecmp (action_exp, "$reg", 4))
		    registers_info (NULL, from_tty);
		  else if (0 == strncasecmp (action_exp, "$loc", 4))
		    locals_info (NULL, from_tty);
		  else if (0 == strncasecmp (action_exp, "$arg", 4))
		    args_info (NULL, from_tty);
		  else
		    {		/* variable */
		      if (next_comma)
			{
			  make_cleanup (replace_comma, next_comma);
			  *next_comma = '\0';
			}
		      printf_filtered ("%s = ", action_exp);
		      output_command (action_exp, from_tty);
		      printf_filtered ("\n");
		    }
		  if (next_comma)
		    *next_comma = ',';
		  action_exp = next_comma;
		}
	      while (action_exp && *action_exp == ',');
	    }
	}
    }
  discard_cleanups (old_cleanups);
}

extern int trace_regblock_size;

static void
trace_save_command (char *args, int from_tty)
{
  char **argv;
  char *filename = NULL, *pathname;
  int target_does_save = 0;
  struct cleanup *cleanup;
  struct trace_status *ts = current_trace_status ();
  int err, status;
  FILE *fp;
  struct uploaded_tp *uploaded_tps = NULL, *utp;
  struct uploaded_tsv *uploaded_tsvs = NULL, *utsv;
  int a;
  LONGEST gotten = 0;
  ULONGEST offset = 0;
#define MAX_TRACE_UPLOAD 2000
  gdb_byte buf[MAX_TRACE_UPLOAD];
  int written;

  if (args == NULL)
    error_no_arg (_("file in which to save trace data"));

  argv = gdb_buildargv (args);
  make_cleanup_freeargv (argv);

  for (; *argv; ++argv)
    {
      if (strcmp (*argv, "-r") == 0)
	target_does_save = 1;
      else if (**argv == '-')
	error (_("unknown option `%s'"), *argv);
      else
	filename = *argv;
    }

  if (!filename)
    error_no_arg (_("file in which to save trace data"));

  /* If the target is to save the data to a file on its own, then just
     send the command and be done with it.  */
  if (target_does_save)
    {
      err = target_save_trace_data (filename);
      if (err < 0)
	error (_("Target failed to save trace data to '%s'."),
	       filename);
      return;
    }

  /* Get the trace status first before opening the file, so if the
     target is losing, we can get out without touching files.  */
  status = target_get_trace_status (ts);

  pathname = tilde_expand (args);
  cleanup = make_cleanup (xfree, pathname);

  fp = fopen (pathname, "w");
  if (!fp)
    error (_("Unable to open file '%s' for saving trace data (%s)"),
	   args, safe_strerror (errno));
  make_cleanup_fclose (fp);

  /* Write a file header, with a high-bit-set char to indicate a
     binary file, plus a hint as what this file is, and a version
     number in case of future needs.  */
  written = fwrite ("\x7fTRACE0\n", 8, 1, fp);
  if (written < 8)
    perror_with_name (pathname);

  /* Write descriptive info.  */

  /* Write out the size of a register block.  */
  fprintf (fp, "R %x\n", trace_regblock_size);

  /* Write out status of the tracing run (aka "tstatus" info).  */
  fprintf (fp, "status %c;%s:%x;tframes:%x;tfree:%llx\n",
	   (ts->running ? '1' : '0'),
	   stop_reason_names[ts->stop_reason], ts->stopping_tracepoint,
	   ts->traceframe_count, ts->buffer_free);

  /* Note that we want to upload tracepoints and save those, rather
     than simply writing out the local ones, because the user may have
     changed tracepoints in GDB in preparation for a future tracing
     run, or maybe just mass-deleted all types of breakpoints as part
     of cleaning up.  So as not to contaminate the session, leave the
     data in its uploaded form, don't make into real tracepoints.  */

  /* Get trace state variables first, they may be checked when parsing
     uploaded commands.  */

  target_upload_trace_state_variables (&uploaded_tsvs);

  for (utsv = uploaded_tsvs; utsv; utsv = utsv->next)
    {
      char *buf = "";

      if (utsv->name)
	{
	  buf = (char *) xmalloc (strlen (utsv->name) * 2 + 1);
	  bin2hex ((gdb_byte *) (utsv->name), buf, 0);
	}

      fprintf (fp, "tsv %x:%s:%x:%s\n",
	       utsv->number, phex_nz (utsv->initial_value, 8),
	       utsv->builtin, buf);

      if (utsv->name)
	xfree (buf);
    }

  free_uploaded_tsvs (&uploaded_tsvs);

  target_upload_tracepoints (&uploaded_tps);

  for (utp = uploaded_tps; utp; utp = utp->next)
    {
      fprintf (fp, "tp T%x:%s:%c:%x:%x",
	       utp->number, phex_nz (utp->addr, sizeof (utp->addr)),
	       (utp->enabled ? 'E' : 'D'), utp->step, utp->pass);
      if (utp->type == bp_fast_tracepoint)
	fprintf (fp, ":F%x", utp->orig_size);
      if (utp->cond)
	fprintf (fp, ":X%x,%s", (unsigned int) strlen (utp->cond) / 2,
		 utp->cond);
      fprintf (fp, "\n");
      for (a = 0; a < utp->numactions; ++a)
	fprintf (fp, "tp A%x:%s:%s\n",
		 utp->number, phex_nz (utp->addr, sizeof (utp->addr)),
		 utp->actions[a]);
      for (a = 0; a < utp->num_step_actions; ++a)
	fprintf (fp, "tp S%x:%s:%s\n",
		 utp->number, phex_nz (utp->addr, sizeof (utp->addr)),
		 utp->step_actions[a]);
    }

  free_uploaded_tps (&uploaded_tps);

  /* Mark the end of the definition section.  */
  fprintf (fp, "\n");

  /* Get and write the trace data proper.  We ask for big blocks, in
     the hopes of efficiency, but will take less if the target has
     packet size limitations or some such.  */
  while (1)
    {
      gotten = target_get_raw_trace_data (buf, offset, MAX_TRACE_UPLOAD);
      if (gotten < 0)
	error (_("Failure to get requested trace buffer data"));
      /* No more data is forthcoming, we're done.  */
      if (gotten == 0)
	break;
      written = fwrite (buf, gotten, 1, fp);
      if (written < gotten)
	perror_with_name (pathname);
      offset += gotten;
    }

  /* Mark the end of trace data.  */
  written = fwrite (&gotten, 4, 1, fp);
  if (written < 4)
    perror_with_name (pathname);

  do_cleanups (cleanup);
  if (from_tty)
    printf_filtered (_("Trace data saved to file '%s'.\n"), args);
}

/* Tell the target what to do with an ongoing tracing run if GDB
   disconnects for some reason.  */

void
send_disconnected_tracing_value (int value)
{
  target_set_disconnected_tracing (value);
}

static void
set_disconnected_tracing (char *args, int from_tty,
			  struct cmd_list_element *c)
{
  send_disconnected_tracing_value (disconnected_tracing);
}

/* Convert the memory pointed to by mem into hex, placing result in buf.
 * Return a pointer to the last char put in buf (null)
 * "stolen" from sparc-stub.c
 */

static const char hexchars[] = "0123456789abcdef";

static char *
mem2hex (gdb_byte *mem, char *buf, int count)
{
  gdb_byte ch;

  while (count-- > 0)
    {
      ch = *mem++;

      *buf++ = hexchars[ch >> 4];
      *buf++ = hexchars[ch & 0xf];
    }

  *buf = 0;

  return buf;
}

int
get_traceframe_number (void)
{
  return traceframe_number;
}


/* Given a number and address, return an uploaded tracepoint with that
   number, creating if necessary.  */

struct uploaded_tp *
get_uploaded_tp (int num, ULONGEST addr, struct uploaded_tp **utpp)
{
  struct uploaded_tp *utp;

  for (utp = *utpp; utp; utp = utp->next)
    if (utp->number == num && utp->addr == addr)
      return utp;
  utp = (struct uploaded_tp *) xmalloc (sizeof (struct uploaded_tp));
  memset (utp, 0, sizeof (struct uploaded_tp));
  utp->number = num;
  utp->addr = addr;
  utp->next = *utpp;
  *utpp = utp;
  return utp;
}

static void
free_uploaded_tps (struct uploaded_tp **utpp)
{
  struct uploaded_tp *next_one;

  while (*utpp)
    {
      next_one = (*utpp)->next;
      xfree (*utpp);
      *utpp = next_one;
    }
}

/* Given a number and address, return an uploaded tracepoint with that
   number, creating if necessary.  */

struct uploaded_tsv *
get_uploaded_tsv (int num, struct uploaded_tsv **utsvp)
{
  struct uploaded_tsv *utsv;

  for (utsv = *utsvp; utsv; utsv = utsv->next)
    if (utsv->number == num)
      return utsv;
  utsv = (struct uploaded_tsv *) xmalloc (sizeof (struct uploaded_tsv));
  memset (utsv, 0, sizeof (struct uploaded_tsv));
  utsv->number = num;
  utsv->next = *utsvp;
  *utsvp = utsv;
  return utsv;
}

static void
free_uploaded_tsvs (struct uploaded_tsv **utsvp)
{
  struct uploaded_tsv *next_one;

  while (*utsvp)
    {
      next_one = (*utsvp)->next;
      xfree (*utsvp);
      *utsvp = next_one;
    }
}

/* Look for an existing tracepoint that seems similar enough to the
   uploaded one.  Enablement isn't compared, because the user can
   toggle that freely, and may have done so in anticipation of the
   next trace run.  */

struct breakpoint *
find_matching_tracepoint (struct uploaded_tp *utp)
{
  VEC(breakpoint_p) *tp_vec = all_tracepoints ();
  int ix;
  struct breakpoint *t;
  struct bp_location *loc;

  for (ix = 0; VEC_iterate (breakpoint_p, tp_vec, ix, t); ix++)
    {
      if (t->type == utp->type
	  && t->step_count == utp->step
	  && t->pass_count == utp->pass
	  /* FIXME also test conditionals and actions */
	  )
	{
	  /* Scan the locations for an address match.  */
	  for (loc = t->loc; loc; loc = loc->next)
	    {
	      if (loc->address == utp->addr)
		return t;
	    }
	}
    }
  return NULL;
}

/* Given a list of tracepoints uploaded from a target, attempt to
   match them up with existing tracepoints, and create new ones if not
   found.  */

void
merge_uploaded_tracepoints (struct uploaded_tp **uploaded_tps)
{
  struct uploaded_tp *utp;
  struct breakpoint *t;

  /* Look for GDB tracepoints that match up with our uploaded versions.  */
  for (utp = *uploaded_tps; utp; utp = utp->next)
    {
      t = find_matching_tracepoint (utp);
      if (t)
	printf_filtered (_("Assuming tracepoint %d is same as target's tracepoint %d at %s.\n"),
			 t->number, utp->number, paddress (get_current_arch (), utp->addr));
      else
	{
	  t = create_tracepoint_from_upload (utp);
	  if (t)
	    printf_filtered (_("Created tracepoint %d for target's tracepoint %d at %s.\n"),
			     t->number, utp->number, paddress (get_current_arch (), utp->addr));
	  else
	    printf_filtered (_("Failed to create tracepoint for target's tracepoint %d at %s, skipping it.\n"),
			     utp->number, paddress (get_current_arch (), utp->addr));
	}
      /* Whether found or created, record the number used by the
	 target, to help with mapping target tracepoints back to their
	 counterparts here.  */
      if (t)
	t->number_on_target = utp->number;
    }

  free_uploaded_tps (uploaded_tps);
}

/* Trace state variables don't have much to identify them beyond their
   name, so just use that to detect matches.  */

struct trace_state_variable *
find_matching_tsv (struct uploaded_tsv *utsv)
{
  if (!utsv->name)
    return NULL;

  return find_trace_state_variable (utsv->name);
}

struct trace_state_variable *
create_tsv_from_upload (struct uploaded_tsv *utsv)
{
  const char *namebase;
  char buf[20];
  int try_num = 0;
  struct trace_state_variable *tsv;

  if (utsv->name)
    {
      namebase = utsv->name;
      sprintf (buf, "%s", namebase);
    }
  else
    {
      namebase = "__tsv";
      sprintf (buf, "%s_%d", namebase, try_num++);
    }

  /* Fish for a name that is not in use.  */
  /* (should check against all internal vars?) */
  while (find_trace_state_variable (buf))
    sprintf (buf, "%s_%d", namebase, try_num++);

  /* We have an available name, create the variable.  */
  tsv = create_trace_state_variable (xstrdup (buf));
  tsv->initial_value = utsv->initial_value;
  tsv->builtin = utsv->builtin;

  return tsv;
}

/* Given a list of uploaded trace state variables, try to match them
   up with existing variables, or create additional ones.  */

void
merge_uploaded_trace_state_variables (struct uploaded_tsv **uploaded_tsvs)
{
  int ix;
  struct uploaded_tsv *utsv;
  struct trace_state_variable *tsv;
  int highest;

  /* Most likely some numbers will have to be reassigned as part of
     the merge, so clear them all in anticipation.  */
  for (ix = 0; VEC_iterate (tsv_s, tvariables, ix, tsv); ++ix)
    tsv->number = 0;

  for (utsv = *uploaded_tsvs; utsv; utsv = utsv->next)
    {
      tsv = find_matching_tsv (utsv);
      if (tsv)
	printf_filtered (_("Assuming trace state variable $%s is same as target's variable %d.\n"),
			 tsv->name, utsv->number);
      else
	{
	  tsv = create_tsv_from_upload (utsv);
	  printf_filtered (_("Created trace state variable $%s for target's variable %d.\n"),
			   tsv->name, utsv->number);
	}
      /* Give precedence to numberings that come from the target.  */
      if (tsv)
	tsv->number = utsv->number;
    }

  /* Renumber everything that didn't get a target-assigned number.  */
  highest = 0;
  for (ix = 0; VEC_iterate (tsv_s, tvariables, ix, tsv); ++ix)
    if (tsv->number > highest)
      highest = tsv->number;

  ++highest;
  for (ix = 0; VEC_iterate (tsv_s, tvariables, ix, tsv); ++ix)
    if (tsv->number == 0)
      tsv->number = highest++;

  free_uploaded_tsvs (uploaded_tsvs);
}

/* target tfile command */

struct target_ops tfile_ops;

/* Fill in tfile_ops with its defined operations and properties.  */

#define TRACE_HEADER_SIZE 8

char *trace_filename;
int trace_fd = -1;
off_t trace_frames_offset;
off_t cur_offset;
int cur_data_size;
int trace_regblock_size;

static void tfile_interp_line (char *line,
			       struct uploaded_tp **utpp,
			       struct uploaded_tsv **utsvp);

static void
tfile_open (char *filename, int from_tty)
{
  char *temp;
  struct cleanup *old_chain;
  int flags;
  int scratch_chan;
  char header[TRACE_HEADER_SIZE];
  char linebuf[1000]; /* should be max remote packet size or so */
  char byte;
  int bytes, i, gotten;
  struct trace_status *ts;
  struct uploaded_tp *uploaded_tps = NULL;
  struct uploaded_tsv *uploaded_tsvs = NULL;

  target_preopen (from_tty);
  if (!filename)
    error (_("No trace file specified."));

  filename = tilde_expand (filename);
  if (!IS_ABSOLUTE_PATH(filename))
    {
      temp = concat (current_directory, "/", filename, (char *)NULL);
      xfree (filename);
      filename = temp;
    }

  old_chain = make_cleanup (xfree, filename);

  flags = O_BINARY | O_LARGEFILE;
  flags |= O_RDONLY;
  scratch_chan = open (filename, flags, 0);
  if (scratch_chan < 0)
    perror_with_name (filename);

  /* Looks semi-reasonable.  Toss the old trace file and work on the new.  */

  discard_cleanups (old_chain);	/* Don't free filename any more */
  unpush_target (&tfile_ops);

  push_target (&tfile_ops);
  discard_cleanups (old_chain);

  trace_filename = xstrdup (filename);
  trace_fd = scratch_chan;

  bytes = 0;
  /* Read the file header and test for validity.  */
  gotten = read (trace_fd, &header, TRACE_HEADER_SIZE);
  if (gotten < 0)
    perror_with_name (trace_filename);
  else if (gotten < TRACE_HEADER_SIZE)
    error (_("Premature end of file while reading trace file"));

  bytes += TRACE_HEADER_SIZE;
  if (!(header[0] == 0x7f
	&& (strncmp (header + 1, "TRACE0\n", 7) == 0)))
    error (_("File is not a valid trace file."));

  trace_regblock_size = 0;
  ts = current_trace_status ();
  /* We know we're working with a file.  */
  ts->from_file = 1;
  /* Set defaults in case there is no status line.  */
  ts->running_known = 0;
  ts->stop_reason = trace_stop_reason_unknown;
  ts->traceframe_count = -1;
  ts->buffer_free = 0;

  /* Read through a section of newline-terminated lines that
     define things like tracepoints.  */
  i = 0;
  while (1)
    {
      gotten = read (trace_fd, &byte, 1);
      if (gotten < 0)
	perror_with_name (trace_filename);
      else if (gotten < 1)
	error (_("Premature end of file while reading trace file"));

      ++bytes;
      if (byte == '\n')
	{
	  /* Empty line marks end of the definition section.  */
	  if (i == 0)
	    break;
	  linebuf[i] = '\0';
	  i = 0;
	  tfile_interp_line (linebuf, &uploaded_tps, &uploaded_tsvs);
	}
      else
	linebuf[i++] = byte;
      if (i >= 1000)
	error (_("Excessively long lines in trace file"));
    }

  /* Add the file's tracepoints and variables into the current mix.  */

  merge_uploaded_tracepoints (&uploaded_tps);

  merge_uploaded_trace_state_variables (&uploaded_tsvs);

  /* Record the starting offset of the binary trace data.  */
  trace_frames_offset = bytes;

  /* If we don't have a blocksize, we can't interpret the
     traceframes.  */
  if (trace_regblock_size == 0)
    error (_("No register block size recorded in trace file"));
  if (ts->traceframe_count <= 0)
    {
      warning ("No traceframes present in this file.");
      return;
    }

#define TFILE_PID (1)
  inferior_appeared (current_inferior (), TFILE_PID);
  inferior_ptid = pid_to_ptid (TFILE_PID);
  add_thread_silent (inferior_ptid);

  post_create_inferior (&tfile_ops, from_tty);

#if 0
  /* FIXME this will get defined in MI patch submission */
  tfind_1 (tfind_number, 0, 0, 0, 0);
#endif
}

/* Interpret the given line from the definitions part of the trace
   file.  */

static void
tfile_interp_line (char *line,
		   struct uploaded_tp **utpp, struct uploaded_tsv **utsvp)
{
  char *p = line;

  if (strncmp (p, "R ", strlen ("R ")) == 0)
    {
      p += strlen ("R ");
      trace_regblock_size = strtol (p, &p, 16);
    }
  else if (strncmp (p, "status ", strlen ("status ")) == 0)
    {
      p += strlen ("status ");
      parse_trace_status (p, current_trace_status ());
    }
  else if (strncmp (p, "tp ", strlen ("tp ")) == 0)
    {
      p += strlen ("tp ");
      parse_tracepoint_definition (p, utpp);
    }
  else if (strncmp (p, "tsv ", strlen ("tsv ")) == 0)
    {
      p += strlen ("tsv ");
      parse_tsv_definition (p, utsvp);
    }
  else
    warning ("Ignoring trace file definition \"%s\"", line);
}

/* Parse the part of trace status syntax that is shared between
   the remote protocol and the trace file reader.  */

extern char *unpack_varlen_hex (char *buff, ULONGEST *result);

void
parse_trace_status (char *line, struct trace_status *ts)
{
  char *p = line, *p1, *p_temp;
  ULONGEST val;

  ts->running_known = 1;
  ts->running = (*p++ == '1');
  ts->stop_reason = trace_stop_reason_unknown;
  while (*p++)
    {
      p1 = strchr (p, ':');
      if (p1 == NULL)
	error (_("Malformed trace status, at %s\n\
Status line: '%s'\n"), p, line);
      if (strncmp (p, stop_reason_names[trace_buffer_full], p1 - p) == 0)
	{
	  p = unpack_varlen_hex (++p1, &val);
	  ts->stop_reason = trace_buffer_full;
	}
      else if (strncmp (p, stop_reason_names[trace_never_run], p1 - p) == 0)
	{
	  p = unpack_varlen_hex (++p1, &val);
	  ts->stop_reason = trace_never_run;
	}
      else if (strncmp (p, stop_reason_names[tracepoint_passcount], p1 - p) == 0)
	{
	  p = unpack_varlen_hex (++p1, &val);
	  ts->stop_reason = tracepoint_passcount;
	  ts->stopping_tracepoint = val;
	}
      else if (strncmp (p, stop_reason_names[tstop_command], p1 - p) == 0)
	{
	  p = unpack_varlen_hex (++p1, &val);
	  ts->stop_reason = tstop_command;
	}
      if (strncmp (p, "tframes", p1 - p) == 0)
	{
	  p = unpack_varlen_hex (++p1, &val);
	  ts->traceframe_count = val;
	}
      if (strncmp (p, "tfree", p1 - p) == 0)
	{
	  p = unpack_varlen_hex (++p1, &val);
	  ts->buffer_free = val;
	}
      else
	{
	  /* Silently skip unknown optional info.  */
	  p_temp = strchr (p1 + 1, ';');
	  if (p_temp)
	    p = p_temp;
	  else
	    /* Must be at the end.  */
	    break;
	}
    }
}

/* Given a line of text defining a tracepoint or tracepoint action, parse
   it into an "uploaded tracepoint".  */

void
parse_tracepoint_definition (char *line, struct uploaded_tp **utpp)
{
  char *p;
  char piece;
  ULONGEST num, addr, step, pass, orig_size, xlen;
  int enabled, i;
  enum bptype type;
  char *cond;
  struct uploaded_tp *utp = NULL;

  p = line;
  /* Both tracepoint and action definitions start with the same number
     and address sequence.  */
  piece = *p++;
  p = unpack_varlen_hex (p, &num);
  p++;  /* skip a colon */
  p = unpack_varlen_hex (p, &addr);
  p++;  /* skip a colon */
  if (piece == 'T')
    {
      enabled = (*p++ == 'E');
      p++;  /* skip a colon */
      p = unpack_varlen_hex (p, &step);
      p++;  /* skip a colon */
      p = unpack_varlen_hex (p, &pass);
      type = bp_tracepoint;
      cond = NULL;
      /* Thumb through optional fields.  */
      while (*p == ':')
	{
	  p++;  /* skip a colon */
	  if (*p == 'F')
	    {
	      type = bp_fast_tracepoint;
	      p++;
	      p = unpack_varlen_hex (p, &orig_size);
	    }
	  else if (*p == 'X')
	    {
	      p++;
	      p = unpack_varlen_hex (p, &xlen);
	      p++;  /* skip a comma */
	      cond = (char *) xmalloc (2 * xlen + 1);
	      strncpy (cond, p, 2 * xlen);
	      cond[2 * xlen] = '\0';
	      p += 2 * xlen;
	    }
	  else
	    warning ("Unrecognized char '%c' in tracepoint definition, skipping rest", *p);
	}
      utp = get_uploaded_tp (num, addr, utpp);
      utp->type = type;
      utp->enabled = enabled;
      utp->step = step;
      utp->pass = pass;
      utp->cond = cond;
    }
  else if (piece == 'A')
    {
      utp = get_uploaded_tp (num, addr, utpp);
      utp->actions[utp->numactions++] = xstrdup (p);
    }
  else if (piece == 'S')
    {
      utp = get_uploaded_tp (num, addr, utpp);
      utp->step_actions[utp->num_step_actions++] = xstrdup (p);
    }
  else
    {
      error ("Invalid tracepoint piece");
    }
}

/* Convert a textual description of a trace state variable into an
   uploaded object.  */

void
parse_tsv_definition (char *line, struct uploaded_tsv **utsvp)
{
  char *p, *buf;
  ULONGEST num, initval, builtin;
  int end;
  struct uploaded_tsv *utsv = NULL;

  buf = alloca (strlen (line));

  p = line;
  p = unpack_varlen_hex (p, &num);
  p++; /* skip a colon */
  p = unpack_varlen_hex (p, &initval);
  p++; /* skip a colon */
  p = unpack_varlen_hex (p, &builtin);
  p++; /* skip a colon */
  end = hex2bin (p, (gdb_byte *) buf, strlen (p) / 2);
  buf[end] = '\0';

  utsv = get_uploaded_tsv (num, utsvp);
  utsv->initial_value = initval;
  utsv->builtin = builtin;
  utsv->name = xstrdup (buf);
}

/* Close the trace file and generally clean up.  */

static void
tfile_close (int quitting)
{
  int pid;

  if (trace_fd < 0)
    return;

  pid = ptid_get_pid (inferior_ptid);
  inferior_ptid = null_ptid;	/* Avoid confusion from thread stuff */
  exit_inferior_silent (pid);

  close (trace_fd);
  trace_fd = -1;
  if (trace_filename)
    xfree (trace_filename);
}

static void
tfile_files_info (struct target_ops *t)
{
  /* (it would be useful to mention the name of the file) */
  printf_filtered ("Looking at a trace file.\n");
}

/* The trace status for a file is that tracing can never be run.  */

static int
tfile_get_trace_status (struct trace_status *ts)
{
  /* Other bits of trace status were collected as part of opening the
     trace files, so nothing to do here.  */

  return -1;
}

/* Given the position of a traceframe in the file, figure out what
   address the frame was collected at.  This would normally be the
   value of a collected PC register, but if not available, we
   improvise.  */

static ULONGEST
tfile_get_traceframe_address (off_t tframe_offset)
{
  ULONGEST addr = 0;
  short tpnum;
  struct breakpoint *tp;
  off_t saved_offset = cur_offset;
  int gotten;

  /* FIXME dig pc out of collected registers */

  /* Fall back to using tracepoint address.  */
  lseek (trace_fd, tframe_offset, SEEK_SET);
  gotten = read (trace_fd, &tpnum, 2);
  if (gotten < 0)
    perror_with_name (trace_filename);
  else if (gotten < 2)
    error (_("Premature end of file while reading trace file"));

  tp = get_tracepoint_by_number_on_target (tpnum);
  if (tp && tp->loc)
    addr = tp->loc->address;

  /* Restore our seek position.  */
  cur_offset = saved_offset;
  lseek (trace_fd, cur_offset, SEEK_SET);
  return addr;
}

/* Given a type of search and some parameters, scan the collection of
   traceframes in the file looking for a match.  When found, return
   both the traceframe and tracepoint number, otherwise -1 for
   each.  */

static int
tfile_trace_find (enum trace_find_type type, int num,
		  ULONGEST addr1, ULONGEST addr2, int *tpp)
{
  short tpnum;
  int tfnum = 0, found = 0, gotten;
  int data_size;
  struct breakpoint *tp;
  off_t offset, tframe_offset;
  ULONGEST tfaddr;

  lseek (trace_fd, trace_frames_offset, SEEK_SET);
  offset = trace_frames_offset;
  while (1)
    {
      tframe_offset = offset;
      gotten = read (trace_fd, &tpnum, 2);
      if (gotten < 0)
	perror_with_name (trace_filename);
      else if (gotten < 2)
	error (_("Premature end of file while reading trace file"));
      offset += 2;
      if (tpnum == 0)
	break;
      gotten = read (trace_fd, &data_size, 4);	
      if (gotten < 0)
	perror_with_name (trace_filename);
      else if (gotten < 4)
	error (_("Premature end of file while reading trace file"));
      offset += 4;
      switch (type)
	{
	case tfind_number:
	  if (tfnum == num)
	    found = 1;
	  break;
	case tfind_pc:
	  tfaddr = tfile_get_traceframe_address (tframe_offset);
	  if (tfaddr == addr1)
	    found = 1;
	  break;
	case tfind_tp:
	  tp = get_tracepoint (num);
	  if (tp && tpnum == tp->number_on_target)
	    found = 1;
	  break;
	case tfind_range:
	  tfaddr = tfile_get_traceframe_address (tframe_offset);
	  if (addr1 <= tfaddr && tfaddr <= addr2)
	    found = 1;
	  break;
	case tfind_outside:
	  tfaddr = tfile_get_traceframe_address (tframe_offset);
	  if (!(addr1 <= tfaddr && tfaddr <= addr2))
	    found = 1;
	  break;
	default:
	  internal_error (__FILE__, __LINE__, _("unknown tfind type"));
	}
      if (found)
	{
	  printf_filtered ("Found traceframe %d.\n", tfnum);
	  if (tpp)
	    *tpp = tpnum;
	  cur_offset = offset;
	  cur_data_size = data_size;
	  return tfnum;
	}
      /* Skip past the traceframe's data.  */
      lseek (trace_fd, data_size, SEEK_CUR);
      offset += data_size;
      /* Update our own count of traceframes.  */
      ++tfnum;
    }
  /* Did not find what we were looking for.  */
  if (tpp)
    *tpp = -1;
  return -1;
}

/* Look for a block of saved registers in the traceframe, and get the
   requested register from it.  */

static void
tfile_fetch_registers (struct target_ops *ops,
		       struct regcache *regcache, int regno)
{
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  char block_type;
  int i, pos, offset, regn, regsize, gotten;
  unsigned short mlen;
  char *regs;

  /* An uninitialized reg size says we're not going to be
     successful at getting register blocks.  */
  if (!trace_regblock_size)
    return;

  regs = alloca (trace_regblock_size);

  lseek (trace_fd, cur_offset, SEEK_SET);
  pos = 0;
  while (pos < cur_data_size)
    {
      gotten = read (trace_fd, &block_type, 1);
      if (gotten < 0)
	perror_with_name (trace_filename);
      else if (gotten < 1)
	error (_("Premature end of file while reading trace file"));

      ++pos;
      switch (block_type)
	{
	case 'R':
	  gotten = read (trace_fd, regs, trace_regblock_size);
	  if (gotten < 0)
	    perror_with_name (trace_filename);
	  else if (gotten < trace_regblock_size)
	    error (_("Premature end of file while reading trace file"));

	  /* Assume the block is laid out in GDB register number order,
	     each register with the size that it has in GDB.  */
	  offset = 0;
	  for (regn = 0; regn < gdbarch_num_regs (gdbarch); regn++)
	    {
	      regsize = register_size (gdbarch, regn);
	      /* Make sure we stay within block bounds.  */
	      if (offset + regsize >= trace_regblock_size)
		break;
	      if (!regcache_valid_p (regcache, regn))
		{
		  if (regno == regn)
		    {
		      regcache_raw_supply (regcache, regno, regs + offset);
		      break;
		    }
		  else if (regno == -1)
		    {
		      regcache_raw_supply (regcache, regn, regs + offset);
		    }
		}
	      offset += regsize;
	    }
	  return;
	case 'M':
	  lseek (trace_fd, 8, SEEK_CUR);
	  gotten = read (trace_fd, &mlen, 2);
	  if (gotten < 0)
	    perror_with_name (trace_filename);
	  else if (gotten < 2)
	    error (_("Premature end of file while reading trace file"));
	  lseek (trace_fd, mlen, SEEK_CUR);
	  pos += (8 + 2 + mlen);
	  break;
	case 'V':
	  lseek (trace_fd, 4 + 8, SEEK_CUR);
	  pos += (4 + 8);
	  break;
	default:
	  error ("Unknown block type '%c' (0x%x) in trace frame",
		 block_type, block_type);
	  break;
	}
    }
}

static LONGEST
tfile_xfer_partial (struct target_ops *ops, enum target_object object,
		    const char *annex, gdb_byte *readbuf,
		    const gdb_byte *writebuf, ULONGEST offset, LONGEST len)
{
  char block_type;
  int pos, gotten;
  ULONGEST maddr;
  unsigned short mlen;

  /* We're only doing regular memory for now.  */
  if (object != TARGET_OBJECT_MEMORY)
    return -1;

  if (readbuf == NULL)
    error ("tfile_xfer_partial: trace file is read-only");

  lseek (trace_fd, cur_offset, SEEK_SET);
  pos = 0;
  while (pos < cur_data_size)
    {
      gotten = read (trace_fd, &block_type, 1);
      if (gotten < 0)
	perror_with_name (trace_filename);
      else if (gotten < 1)
	error (_("Premature end of file while reading trace file"));
      ++pos;
      switch (block_type)
	{
	case 'R':
	  lseek (trace_fd, trace_regblock_size, SEEK_CUR);
	  pos += trace_regblock_size;
	  break;
	case 'M':
	  gotten = read (trace_fd, &maddr, 8);
	  if (gotten < 0)
	    perror_with_name (trace_filename);
	  else if (gotten < 8)
	    error (_("Premature end of file while reading trace file"));

	  gotten = read (trace_fd, &mlen, 2);
	  if (gotten < 0)
	    perror_with_name (trace_filename);
	  else if (gotten < 2)
	    error (_("Premature end of file while reading trace file"));
	  if (maddr <= offset && (offset + len) <= (maddr + mlen))
	    {
	      gotten = read (trace_fd, readbuf, mlen);
	      if (gotten < 0)
		perror_with_name (trace_filename);
	      else if (gotten < mlen)
		error (_("Premature end of file qwhile reading trace file"));

	      return mlen;
	    }
	  lseek (trace_fd, mlen, SEEK_CUR);
	  pos += (8 + 2 + mlen);
	  break;
	case 'V':
	  lseek (trace_fd, 4 + 8, SEEK_CUR);
	  pos += (4 + 8);
	  break;
	default:
	  error ("Unknown block type '%c' (0x%x) in traceframe",
		 block_type, block_type);
	  break;
	}
    }
  /* Indicate failure to find the requested memory block.  */
  return -1;
}

/* Iterate through the blocks of a trace frame, looking for a 'V'
   block with a matching tsv number.  */

static int
tfile_get_trace_state_variable_value (int tsvnum, LONGEST *val)
{
  char block_type;
  int pos, vnum, gotten;
  unsigned short mlen;

  lseek (trace_fd, cur_offset, SEEK_SET);
  pos = 0;
  while (pos < cur_data_size)
    {
      gotten = read (trace_fd, &block_type, 1);
      if (gotten < 0)
	perror_with_name (trace_filename);
      else if (gotten < 1)
	error (_("Premature end of file while reading trace file"));
      ++pos;
      switch (block_type)
	{
	case 'R':
	  lseek (trace_fd, trace_regblock_size, SEEK_CUR);
	  pos += trace_regblock_size;
	  break;
	case 'M':
	  lseek (trace_fd, 8, SEEK_CUR);
	  gotten = read (trace_fd, &mlen, 2);
	  if (gotten < 0)
	    perror_with_name (trace_filename);
	  else if (gotten < 2)
	    error (_("Premature end of file while reading trace file"));
	  lseek (trace_fd, mlen, SEEK_CUR);
	  pos += (8 + 2 + mlen);
	  break;
	case 'V':
	  gotten = read (trace_fd, &vnum, 4);
	  if (gotten < 0)
	    perror_with_name (trace_filename);
	  else if (gotten < 4)
	    error (_("Premature end of file while reading trace file"));
	  if (tsvnum == vnum)
	    {
	      gotten = read (trace_fd, val, 8);
	      if (gotten < 0)
		perror_with_name (trace_filename);
	      else if (gotten < 8)
		error (_("Premature end of file while reading trace file"));
	      return 1;
	    }
	  lseek (trace_fd, 8, SEEK_CUR);
	  pos += (4 + 8);
	  break;
	default:
	  error ("Unknown block type '%c' (0x%x) in traceframe",
		 block_type, block_type);
	  break;
	}
    }
  /* Didn't find anything.  */
  return 0;
}

static int
tfile_has_memory (struct target_ops *ops)
{
  return 1;
}

static int
tfile_has_stack (struct target_ops *ops)
{
  return 1;
}

static int
tfile_has_registers (struct target_ops *ops)
{
  return 1;
}

static void
init_tfile_ops (void)
{
  tfile_ops.to_shortname = "tfile";
  tfile_ops.to_longname = "Local trace dump file";
  tfile_ops.to_doc =
    "Use a trace file as a target.  Specify the filename of the trace file.";
  tfile_ops.to_open = tfile_open;
  tfile_ops.to_close = tfile_close;
  tfile_ops.to_fetch_registers = tfile_fetch_registers;
  tfile_ops.to_xfer_partial = tfile_xfer_partial;
  tfile_ops.to_files_info = tfile_files_info;
  tfile_ops.to_get_trace_status = tfile_get_trace_status;
  tfile_ops.to_trace_find = tfile_trace_find;
  tfile_ops.to_get_trace_state_variable_value = tfile_get_trace_state_variable_value;
  /* core_stratum might seem more logical, but GDB doesn't like having
     more than one core_stratum vector.  */
  tfile_ops.to_stratum = process_stratum;
  tfile_ops.to_has_memory = tfile_has_memory;
  tfile_ops.to_has_stack = tfile_has_stack;
  tfile_ops.to_has_registers = tfile_has_registers;
  tfile_ops.to_magic = OPS_MAGIC;
}

/* module initialization */
void
_initialize_tracepoint (void)
{
  struct cmd_list_element *c;

  traceframe_number = -1;
  tracepoint_number = -1;

  if (tracepoint_list.list == NULL)
    {
      tracepoint_list.listsize = 128;
      tracepoint_list.list = xmalloc
	(tracepoint_list.listsize * sizeof (struct memrange));
    }
  if (tracepoint_list.aexpr_list == NULL)
    {
      tracepoint_list.aexpr_listsize = 128;
      tracepoint_list.aexpr_list = xmalloc
	(tracepoint_list.aexpr_listsize * sizeof (struct agent_expr *));
    }

  if (stepping_list.list == NULL)
    {
      stepping_list.listsize = 128;
      stepping_list.list = xmalloc
	(stepping_list.listsize * sizeof (struct memrange));
    }

  if (stepping_list.aexpr_list == NULL)
    {
      stepping_list.aexpr_listsize = 128;
      stepping_list.aexpr_list = xmalloc
	(stepping_list.aexpr_listsize * sizeof (struct agent_expr *));
    }

  add_info ("scope", scope_info,
	    _("List the variables local to a scope"));

  add_cmd ("tracepoints", class_trace, NULL,
	   _("Tracing of program execution without stopping the program."),
	   &cmdlist);

  add_com ("tdump", class_trace, trace_dump_command,
	   _("Print everything collected at the current tracepoint."));

  add_com ("tsave", class_trace, trace_save_command, _("\
Save the trace data to a file.\n\
Use the '-r' option to direct the target to save directly to the file,\n\
using its own filesystem."));

  c = add_com ("tvariable", class_trace, trace_variable_command,_("\
Define a trace state variable.\n\
Argument is a $-prefixed name, optionally followed\n\
by '=' and an expression that sets the initial value\n\
at the start of tracing."));
  set_cmd_completer (c, expression_completer);

  add_cmd ("tvariable", class_trace, delete_trace_variable_command, _("\
Delete one or more trace state variables.\n\
Arguments are the names of the variables to delete.\n\
If no arguments are supplied, delete all variables."), &deletelist);
  /* FIXME add a trace variable completer */

  add_info ("tvariables", tvariables_info, _("\
Status of trace state variables and their values.\n\
"));

  add_prefix_cmd ("tfind", class_trace, trace_find_command, _("\
Select a trace frame;\n\
No argument means forward by one frame; '-' means backward by one frame."),
		  &tfindlist, "tfind ", 1, &cmdlist);

  add_cmd ("outside", class_trace, trace_find_outside_command, _("\
Select a trace frame whose PC is outside the given range.\n\
Usage: tfind outside addr1, addr2"),
	   &tfindlist);

  add_cmd ("range", class_trace, trace_find_range_command, _("\
Select a trace frame whose PC is in the given range.\n\
Usage: tfind range addr1,addr2"),
	   &tfindlist);

  add_cmd ("line", class_trace, trace_find_line_command, _("\
Select a trace frame by source line.\n\
Argument can be a line number (with optional source file), \n\
a function name, or '*' followed by an address.\n\
Default argument is 'the next source line that was traced'."),
	   &tfindlist);

  add_cmd ("tracepoint", class_trace, trace_find_tracepoint_command, _("\
Select a trace frame by tracepoint number.\n\
Default is the tracepoint for the current trace frame."),
	   &tfindlist);

  add_cmd ("pc", class_trace, trace_find_pc_command, _("\
Select a trace frame by PC.\n\
Default is the current PC, or the PC of the current trace frame."),
	   &tfindlist);

  add_cmd ("end", class_trace, trace_find_end_command, _("\
Synonym for 'none'.\n\
De-select any trace frame and resume 'live' debugging."),
	   &tfindlist);

  add_cmd ("none", class_trace, trace_find_none_command,
	   _("De-select any trace frame and resume 'live' debugging."),
	   &tfindlist);

  add_cmd ("start", class_trace, trace_find_start_command,
	   _("Select the first trace frame in the trace buffer."),
	   &tfindlist);

  add_com ("tstatus", class_trace, trace_status_command,
	   _("Display the status of the current trace data collection."));

  add_com ("tstop", class_trace, trace_stop_command,
	   _("Stop trace data collection."));

  add_com ("tstart", class_trace, trace_start_command,
	   _("Start trace data collection."));

  add_com ("end", class_trace, end_actions_pseudocommand, _("\
Ends a list of commands or actions.\n\
Several GDB commands allow you to enter a list of commands or actions.\n\
Entering \"end\" on a line by itself is the normal way to terminate\n\
such a list.\n\n\
Note: the \"end\" command cannot be used at the gdb prompt."));

  add_com ("while-stepping", class_trace, while_stepping_pseudocommand, _("\
Specify single-stepping behavior at a tracepoint.\n\
Argument is number of instructions to trace in single-step mode\n\
following the tracepoint.  This command is normally followed by\n\
one or more \"collect\" commands, to specify what to collect\n\
while single-stepping.\n\n\
Note: this command can only be used in a tracepoint \"actions\" list."));

  add_com_alias ("ws", "while-stepping", class_alias, 0);
  add_com_alias ("stepping", "while-stepping", class_alias, 0);

  add_com ("collect", class_trace, collect_pseudocommand, _("\
Specify one or more data items to be collected at a tracepoint.\n\
Accepts a comma-separated list of (one or more) expressions.  GDB will\n\
collect all data (variables, registers) referenced by that expression.\n\
Also accepts the following special arguments:\n\
    $regs   -- all registers.\n\
    $args   -- all function arguments.\n\
    $locals -- all variables local to the block/function scope.\n\
Note: this command can only be used in a tracepoint \"actions\" list."));

  add_com ("teval", class_trace, teval_pseudocommand, _("\
Specify one or more expressions to be evaluated at a tracepoint.\n\
Accepts a comma-separated list of (one or more) expressions.\n\
The result of each evaluation will be discarded.\n\
Note: this command can only be used in a tracepoint \"actions\" list."));

  add_com ("actions", class_trace, trace_actions_command, _("\
Specify the actions to be taken at a tracepoint.\n\
Tracepoint actions may include collecting of specified data, \n\
single-stepping, or enabling/disabling other tracepoints, \n\
depending on target's capabilities."));

  default_collect = xstrdup ("");
  add_setshow_string_cmd ("default-collect", class_trace,
			  &default_collect, _("\
Set the list of expressions to collect by default"), _("\
Show the list of expressions to collect by default"), NULL,
			  NULL, NULL,
			  &setlist, &showlist);

  add_setshow_boolean_cmd ("disconnected-tracing", no_class,
			   &disconnected_tracing, _("\
Set whether tracing continues after GDB disconnects."), _("\
Show whether tracing continues after GDB disconnects."), _("\
Use this to continue a tracing run even if GDB disconnects\n\
or detaches from the target.  You can reconnect later and look at\n\
trace data collected in the meantime."),
			   set_disconnected_tracing,
			   NULL,
			   &setlist,
			   &showlist);

  init_tfile_ops ();

  add_target (&tfile_ops);
}
