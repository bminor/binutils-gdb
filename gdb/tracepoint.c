/* Tracing functionality for remote targets in custom GDB protocol
   Copyright 1997 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "symtab.h"
#include "frame.h"
#include "tracepoint.h"
#include "gdbtypes.h"
#include "expression.h"
#include "gdbcmd.h"
#include "value.h"
#include "target.h"
#include "language.h"

/* readline include files */
#include "readline.h"
#include "history.h"

/* readline defines this.  */
#undef savestring

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

extern int info_verbose;

/* If this definition isn't overridden by the header files, assume
   that isatty and fileno exist on this system.  */
#ifndef ISATTY
#define ISATTY(FP)	(isatty (fileno (FP)))
#endif

/* Chain of all tracepoints defined.  */
struct tracepoint *tracepoint_chain;

/* Number of last tracepoint made.  */
static int tracepoint_count;

/* Number of last traceframe collected.  */
static int traceframe_number;

/* Utility: returns true if "target remote" */
static int
target_is_remote ()
{
  if (current_target.to_shortname &&
      strcmp (current_target.to_shortname, "remote") == 0)
    return 1;
  else
    return 0;
}

/* Utility: generate error from an incoming stub packet.  */
static void 
trace_error (buf)
     char *buf;
{
  if (*buf++ != 'E')
    return;			/* not an error msg */
  switch (*buf) 
    {
    case '1':			/* malformed packet error */
      if (*++buf == '0')	/*   general case: */
	error ("tracepoint.c: badly formed packet.");
      else
	error ("tracepoint.c: badly formed packet at field #%d.", 
	       *buf - '0');
    case '2':
      error ("trace API error '%s'.", buf);
    default:
      error ("Target returns error code '%s'.", buf);
    }
}

/* Obsolete: collect regs from a trace frame */
static void
trace_receive_regs (buf)
     char *buf;
{
  long regno, i;
  char regbuf[MAX_REGISTER_RAW_SIZE], *tmp, *p = buf;

  while (*p)
    {
      regno = strtol (p, &tmp, 16);
      if (p == tmp || *tmp++ != ':')
	error ("tracepoint.c: malformed 'R' packet");
      else p = tmp;

      for (i = 0; i < REGISTER_RAW_SIZE (regno); i++)
	{
	  if (p[0] == 0 || p[1] == 0)
	    warning ("Remote reply is too short: %s", buf);
	  regbuf[i] = fromhex (p[0]) * 16 + fromhex (p[1]);
	  p += 2;
	}

      if (*p++ != ';')
	error ("tracepoint.c: malformed 'R' packet");

      supply_register (regno, regbuf);
    }
}

/* Utility: wait for reply from stub, while accepting "O" packets */
static void
remote_get_noisy_reply (buf)
     char *buf;
{
  do	/* loop on reply from remote stub */
    {
      getpkt (buf, 0);
      if (buf[0] == 0)
	error ("Target does not support this command.");
      else if (buf[0] == 'E')
	trace_error (buf);
      else if (buf[0] == 'R')
	{
	  flush_cached_frames ();
	  registers_changed ();
	  select_frame (get_current_frame (), 0);
	  trace_receive_regs (buf);
	}
      else if (buf[0] == 'O' &&
	       buf[1] != 'K')
	remote_console_output (buf + 1);	/* 'O' message from stub */
      else
	return;			/* here's the actual reply */
    } while (1);
}

/* Set tracepoint count to NUM.  */
static void
set_tracepoint_count (num)
     int num;
{
  tracepoint_count = num;
  set_internalvar (lookup_internalvar ("tpnum"),
		   value_from_longest (builtin_type_int, (LONGEST) num));
}

/* Set traceframe number to NUM.  */
static void
set_traceframe_num (num)
     int num;
{
  traceframe_number = num;
  set_internalvar (lookup_internalvar ("trace_frame"),
		   value_from_longest (builtin_type_int, (LONGEST) num));
}

/* Low level routine to set a tracepoint.
   Returns the tracepoint object so caller can set other things.
   Does not set the tracepoint number!
   Does not print anything.

   ==> This routine should not be called if there is a chance of later
   error(); otherwise it leaves a bogus tracepoint on the chain.  Validate
   your arguments BEFORE calling this routine!  */

static struct tracepoint *
set_raw_tracepoint (sal)
     struct symtab_and_line sal;
{
  register struct tracepoint *t, *tc;
  struct cleanup *old_chain;

  t = (struct tracepoint *) xmalloc (sizeof (struct tracepoint));
  old_chain = make_cleanup (free, t);
  memset (t, 0, sizeof (*t));
  t->address = sal.pc;
  if (sal.symtab == NULL)
    t->source_file = NULL;
  else
    {
      t->source_file = (char *) xmalloc (strlen (sal.symtab->filename) +
                                         strlen (sal.symtab->dirname) + 1);

      strcpy (t->source_file, sal.symtab->dirname);
      strcat (t->source_file, sal.symtab->filename);
    }

  t->language = current_language->la_language;
  t->input_radix = input_radix;
  t->line_number = sal.line;
  t->enabled = enabled;
  t->next = 0;
  t->step_count = 0;
  t->pass_count = 0;

  /* Add this tracepoint to the end of the chain
     so that a list of tracepoints will come out in order
     of increasing numbers.  */

  tc = tracepoint_chain;
  if (tc == 0)
    tracepoint_chain = t;
  else
    {
      while (tc->next)
	tc = tc->next;
      tc->next = t;
    }
  discard_cleanups (old_chain);
  return t;
}

static void
trace_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  char **canonical = (char **)NULL;
  struct symtabs_and_lines sals;
  struct symtab_and_line sal;
  struct tracepoint *t;
  char *addr_start = 0, *addr_end = 0, *cond_start = 0, *cond_end = 0;
  int i;

  if (!arg || !*arg)
    error ("trace command requires an argument");

  if (from_tty && info_verbose)
    printf_filtered ("TRACE %s\n", arg);

  if (arg[0] == '/')
    {
      return;
    }

  addr_start = arg;
  sals = decode_line_1 (&arg, 1, (struct symtab *)NULL, 0, &canonical);
  addr_end   = arg;
  if (! sals.nelts) 
    return;	/* ??? Presumably decode_line_1 has already warned? */

  /* Resolve all line numbers to PC's */
  for (i = 0; i < sals.nelts; i++)
    resolve_sal_pc (&sals.sals[i]);

  /* Now set all the tracepoints.  */
  for (i = 0; i < sals.nelts; i++)
    {
      sal = sals.sals[i];

      t = set_raw_tracepoint (sal);
      set_tracepoint_count (tracepoint_count + 1);
      t->number = tracepoint_count;

      /* If a canonical line spec is needed use that instead of the
         command string.  */
      if (canonical != (char **)NULL && canonical[i] != NULL)
        t->addr_string = canonical[i];
      else if (addr_start)
        t->addr_string = savestring (addr_start, addr_end - addr_start);
      if (cond_start)
        t->cond_string = savestring (cond_start, cond_end - cond_start);

      /* Let the UI know of any additions */
      if (create_tracepoint_hook)
        create_tracepoint_hook (t);
    }

  if (sals.nelts > 1)
    {
      printf_filtered ("Multiple tracepoints were set.\n");
      printf_filtered ("Use the \"delete\" command to delete unwanted tracepoints.\n");
    }
}

static void
tracepoints_info (tpnum_exp, from_tty)
     char *tpnum_exp;
     int from_tty;
{
  struct tracepoint *t;
  struct action_line *action;
  int found_a_tracepoint = 0;
  char wrap_indent[80];
  struct symbol *sym;
  int tpnum = -1;
#if 0
  char *i1 = "\t", *i2 = "\t  ";
  char *indent, *actionline;;
#endif

  if (tpnum_exp)
    tpnum = parse_and_eval_address (tpnum_exp);

  ALL_TRACEPOINTS (t)
    if (tpnum == -1 || tpnum == t->number)
      {
	extern int addressprint;	/* print machine addresses? */

	if (!found_a_tracepoint++)
	  printf_filtered (" *** [info tracepoints header line] ***\n");

	strcpy (wrap_indent, "                           ");
	if (addressprint)
	  strcat (wrap_indent, "           ");

	printf_filtered ("%-3d %-10s ", t->number, 
			 t->enabled == enabled ? "enabled" : "disabled");
	if (addressprint)
	  { /* FIXME-32x64: need a print_address_numeric with field width */
	    printf_filtered ("%s ", local_hex_string_custom ((unsigned long) t->address, "08l"));
	  }
	if (t->source_file)
	  {
	    sym = find_pc_function (t->address);
	    if (sym)
	      {
		fputs_filtered ("in ", gdb_stdout);
		fputs_filtered (SYMBOL_SOURCE_NAME (sym), gdb_stdout);
		wrap_here (wrap_indent);
		fputs_filtered (" at ", gdb_stdout);
	      }
	    fputs_filtered (t->source_file, gdb_stdout);
	    printf_filtered (":%d", t->line_number);
	  }
	else
	  print_address_symbolic (t->address, gdb_stdout, demangle, " ");

	if (t->pass_count != 0)
	  printf_filtered (" passcount = %d", t->pass_count);
	printf_filtered ("\n");
	if (t->actions)
	  {
	    printf_filtered ("  Actions for tracepoint %d: \n", t->number);
/*	    indent = i1; */
	    for (action = t->actions; action; action = action->next)
	      {
#if 0
		actionline = action->action;
		while (isspace(*actionline))
		  actionline++;

		printf_filtered ("%s%s\n", indent, actionline);
		if (0 == strncasecmp (actionline, "while-stepping", 14))
		  indent = i2;
		else if (0 == strncasecmp (actionline, "end", 3))
		  indent = i1;
#else
		printf_filtered ("\t%s\n", action->action);
#endif
	      }
	  }
      }
  if (!found_a_tracepoint)
    {
      if (tpnum == -1)
        printf_filtered ("No tracepoints.\n");
      else
        printf_filtered ("No tracepoint number %d.\n", tpnum);
    }
}

/* Optimization: the code to parse an enable, disable, or delete TP command
   is virtually identical except for whether it performs an enable, disable,
   or delete.  Therefore I've combined them into one function with an opcode.
   */
enum tracepoint_opcode 
{
  enable, 
  disable,
  delete
};

/* This function implements enable, disable and delete. */
static void
tracepoint_operation (t, from_tty, opcode)
     struct tracepoint *t;
     int from_tty;
     enum tracepoint_opcode opcode;
{
  struct tracepoint *t2;
  struct action_line *action, *next;

  switch (opcode) {
  case enable:
    t->enabled = enabled;
    break;
  case disable:
    t->enabled = disabled;
    break;
  case delete:
    if (tracepoint_chain == t)
      tracepoint_chain = t->next;

    ALL_TRACEPOINTS (t2)
      if (t2->next == t)
	{
	  t2->next = t->next;
	  break;
	}

    /* Let the UI know of any deletions */
    if (delete_tracepoint_hook)
      delete_tracepoint_hook (t);

    if (t->cond_string)
      free (t->cond_string);
    if (t->addr_string)
      free (t->addr_string);
    if (t->source_file)
      free (t->source_file);
    for (action = t->actions; action; action = next)
      {
	next = action->next;
	if (action->action) 
	  free (action->action);
	free (action);
      }
    free (t);
    break;
  }
}

/* Utility: parse a tracepoint number and look it up in the list.  */
struct tracepoint *
get_tracepoint_by_number (arg)
     char **arg;
{
  struct tracepoint *t;
  char *cp;
  value_ptr val;
  int tpnum;

  if (arg == 0)
    error ("Bad tracepoint argument");

  if (*arg == 0 || **arg == 0)	/* empty arg means refer to last tp */
    tpnum = tracepoint_count;
  else if (**arg == '$')	/* handle convenience variable */
    {
      cp = *arg + 1;
      /* find end of convenience variable name */
      while (**arg && **arg != ' ' && **arg != '\t')
	*arg++;
      /* null-terminate if necessary */
      if (**arg != 0)
	*(*arg++) = 0;
      val = value_of_internalvar (lookup_internalvar (cp));
      if (TYPE_CODE( VALUE_TYPE (val)) != TYPE_CODE_INT)
	error ("Convenience variable must have integral type.");
      tpnum = (int) value_as_long (val);
    }
  else		/* handle tracepoint number */
    {
      tpnum = strtol (*arg, arg, 10);
    }
  ALL_TRACEPOINTS (t)
    if (t->number == tpnum)
      {
	return t;
      }
  warning ("No tracepoint number %d.\n", tpnum);
  return NULL;
}

/* Utility: parse a list of tracepoint numbers, and call a func for each. */
static void
map_args_over_tracepoints (args, from_tty, opcode)
     char *args;
     int from_tty;
     enum tracepoint_opcode opcode;
{
  struct tracepoint *t;
  int tpnum;
  char *cp;

  if (args == 0 || *args == 0)	/* do them all */
    ALL_TRACEPOINTS (t)
      tracepoint_operation (t, from_tty, opcode);
  else
    while (*args)
      {
	if (t = get_tracepoint_by_number (&args))
	  tracepoint_operation (t, from_tty, opcode);
	while (*args == ' ' || *args == '\t')
	  args++;
      }
}

static void
enable_trace_command (args, from_tty)
     char *args;
     int from_tty;
{
  dont_repeat ();
  map_args_over_tracepoints (args, from_tty, enable);
}

static void
disable_trace_command (args, from_tty)
     char *args;
     int from_tty;
{
  dont_repeat ();
  map_args_over_tracepoints (args, from_tty, disable);
}

static void
delete_trace_command (args, from_tty)
     char *args;
     int from_tty;
{
  dont_repeat ();
  if (!args || !*args)
    if (!query ("Delete all tracepoints? "))
      return;

  map_args_over_tracepoints (args, from_tty, delete);
}

static void
trace_pass_command (args, from_tty)
     char *args;
     int from_tty;
{
  struct tracepoint *t1 = (struct tracepoint *) -1, *t2;
  unsigned long count;

  if (args == 0 || *args == 0)
    error ("PASS command requires an argument (count + optional TP num)");

  count = strtoul (args, &args, 10);	/* count comes first, then TP num */

  while (*args && isspace (*args))
    args++;

  if (*args && strncasecmp (args, "all", 3) == 0)
    args += 3;	/* skip special argument "all" */
  else
    t1 = get_tracepoint_by_number (&args);

  if (t1 == NULL)
    return;	/* error, bad tracepoint number */

  ALL_TRACEPOINTS (t2)
    if (t1 == (struct tracepoint *) -1 || t1 == t2)
      {
	t2->pass_count = count;
	if (from_tty)
	  printf_filtered ("Setting tracepoint %d's passcount to %d\n", 
			   t2->number, count);
      }
}

/* ACTIONS ACTIONS ACTIONS */

static void read_actions PARAMS((struct tracepoint *));
static void free_actions PARAMS((struct tracepoint *));
static int  validate_actionline PARAMS((char *, struct tracepoint *));

static void
trace_actions_command (args, from_tty)
     char *args;
     int from_tty;
{
  struct tracepoint *t;
  char *actions;

  if (t = get_tracepoint_by_number (&args))
    {
      if (from_tty)
	printf_filtered ("Enter actions for tracepoint %d, one per line.\n", 
			 t->number);
      free_actions (t);
      read_actions (t);
      /* tracepoints_changed () */
    }
  /* else error, just return; */
}

enum actionline_type
{
  BADLINE  = -1, 
  GENERIC  =  0,
  END      =  1,
  STEPPING =  2,
};

static void
read_actions (t)
     struct tracepoint *t;
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
#ifdef STOP_SIGNAL
  if (job_control)
    signal (STOP_SIGNAL, stop_sig);
#endif
  old_chain = make_cleanup (free_actions, (void *) t);
  while (1)
    {
      /* Make sure that all output has been output.  Some machines may let
	 you get away with leaving out some of the gdb_flush, but not all.  */
      wrap_here ("");
      gdb_flush (gdb_stdout);
      gdb_flush (gdb_stderr);
      if (instream == stdin && ISATTY (instream))
	line = readline (prompt);
      else
	line = gdb_readline (0);

      linetype = validate_actionline (line, t);
      if (linetype == BADLINE)
	continue;	/* already warned -- collect another line */

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
	if (prompt == prompt2)
	  {
	    warning ("Already processing 'while-stepping'");
	    continue;
	  }
	else
	  prompt = prompt2;	/* change prompt for stepping actions */
      else if (linetype == END)
	if (prompt == prompt2)
	  prompt = prompt1;	/* end of single-stepping actions */
	else
	  break;		/* end of actions */
    }
#ifdef STOP_SIGNAL
  if (job_control)
    signal (STOP_SIGNAL, SIG_DFL);
#endif
  immediate_quit = 0;
  discard_cleanups (old_chain);
}

static enum actionline_type
validate_actionline (line, t)
     char *line;
     struct tracepoint *t;
{
  char *p;
  struct expression *exp;
  value_ptr temp, temp2;

  for (p = line; isspace (*p); )
    p++;

  /* symbol lookup etc. */
  if (*p == '\0')	/* empty line: just prompt for another line. */
    return BADLINE;
  else if (0 == strncasecmp (p, "collect", 7))
    {
      p += 7;
      do {			/* repeat over a comma-separated list */
	while (isspace (*p))
	  p++;

	if (*p == '$' &&		/* look for special pseudo-symbols */
	    ((0 == strncasecmp ("reg", p + 1, 3)) ||
	     (0 == strncasecmp ("arg", p + 1, 3)) ||
	     (0 == strncasecmp ("loc", p + 1, 3))))
	  p = (char *) strchr (p, ',');
	else
	  {
	    exp   = parse_exp_1 (&p, block_for_pc (t->address), 1);
	    if (exp->elts[0].opcode != OP_VAR_VALUE &&
	      /*exp->elts[0].opcode != OP_LONG      && */
	      /*exp->elts[0].opcode != UNOP_CAST    && */
		exp->elts[0].opcode != OP_REGISTER)
	      {
		warning ("collect: enter variable name or register.\n");
		return BADLINE;
	      }
	    if (exp->elts[0].opcode == OP_VAR_VALUE)
	      if (SYMBOL_CLASS (exp->elts[2].symbol) == LOC_CONST)
		{
		  warning ("%s is constant (value %d): will not be collected.",
			   SYMBOL_NAME (exp->elts[2].symbol),
			   SYMBOL_VALUE (exp->elts[2].symbol));
		  return BADLINE;
		}
	      else if (SYMBOL_CLASS (exp->elts[2].symbol) == LOC_OPTIMIZED_OUT)
		{
		  warning ("%s is optimized away and cannot be collected.",
			   SYMBOL_NAME (exp->elts[2].symbol));
		  return BADLINE;
		}
	  }
      } while (p && *p++ == ',');
      return GENERIC;
    }
  else if (0 == strncasecmp (p, "while-stepping", 14))
    {
      char *steparg;	/* in case warning is necessary */

      p += 14;
      while (isspace (*p))
	p++;
      steparg = p;

      if (*p)
	{
	  t->step_count = strtol (p, &p, 0);
	  if (t->step_count == 0)
	    {
	      warning ("'%s' evaluates to zero -- command ignored.");
	      return BADLINE;
	    }
	}
      else 
	t->step_count = -1;
      return STEPPING;
    }
  else if (0 == strncasecmp (p, "end", 3))
    return END;
  else
    {
      warning ("'%s' is not a supported tracepoint action.", p);
      return BADLINE;
    }
}

static void 
free_actions (t)
     struct tracepoint *t;
{
  struct action_line *line, *next;

  for (line = t->actions; line; line = next)
    {
      next = line->next;
      free (line);
    }
  t->actions = NULL;
}

struct memrange {
  int type;		/* 0 for absolute memory range, else basereg number */
  bfd_signed_vma start;
  bfd_signed_vma end;
};

struct collection_list {
  unsigned char regs_mask[8];	/* room for up to 256 regs */
  long listsize;
  long next_memrange;
  struct memrange *list;
} tracepoint_list, stepping_list;

static int
memrange_cmp (a, b)
     struct memrange *a, *b;
{
  if (a->type < b->type) return -1;
  if (a->type > b->type) return  1;
  if (a->type == 0)
    {
      if ((bfd_vma) a->start  < (bfd_vma) b->start)  return -1;
      if ((bfd_vma) a->start  > (bfd_vma) b->start)  return  1;
    }
  else
    {
      if (a->start  < b->start)  return -1;
      if (a->start  > b->start)  return  1;
    }
  return 0;
}

static void
memrange_sortmerge (memranges)
     struct collection_list *memranges;
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
	      MAX_REGISTER_VIRTUAL_SIZE)
	    {
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

void
add_register (collection, regno)
     struct collection_list *collection;
     unsigned long regno;
{
  if (info_verbose)
    printf_filtered ("collect register %d\n", regno);
  if (regno > (8 * sizeof (collection->regs_mask)))
    error ("Internal: register number %d too large for tracepoint",
	   regno);
  collection->regs_mask [regno / 8] |= 1 << (regno  % 8);
}

static void
add_memrange (memranges, type, base, len)
     struct collection_list *memranges;
     int type;
     bfd_signed_vma base;
     unsigned long len;
{
  if (info_verbose)
    printf_filtered ("(%d,0x%x,%d)\n", type, base, len);
  /* type: 0 == memory, n == basereg */
  memranges->list[memranges->next_memrange].type  = type;
  /* base: addr if memory, offset if reg relative. */
  memranges->list[memranges->next_memrange].start = base;
  /* len: we actually save end (base + len) for convenience */
  memranges->list[memranges->next_memrange].end   = base + len;
  memranges->next_memrange++;
  if (memranges->next_memrange >= memranges->listsize)
    {
      memranges->listsize *= 2;
      memranges->list = xrealloc (memranges->list, 
				  memranges->listsize);
    }

  if (type != 0)	/* better collect the base register! */
    add_register (memranges, type);
}

static void
collect_symbol (collect, sym)
     struct collection_list *collect;
     struct symbol *sym;
{
  unsigned long  len;
  unsigned long  reg;
  bfd_signed_vma offset;

  len  = TYPE_LENGTH (check_typedef (SYMBOL_TYPE (sym)));
  switch (SYMBOL_CLASS (sym)) {
  default:
    printf_filtered ("%s: don't know symbol class %d\n",
		     SYMBOL_NAME (sym), SYMBOL_CLASS (sym));
    break;
  case LOC_CONST:
    printf_filtered ("%s is constant, value is %d: will not be collected.\n",
		     SYMBOL_NAME (sym), SYMBOL_VALUE (sym));
    break;
  case LOC_STATIC:
    offset = SYMBOL_VALUE_ADDRESS (sym); 
    if (info_verbose)
      printf_filtered ("LOC_STATIC %s: collect %d bytes "
		       "at 0x%08x\n",
		       SYMBOL_NAME (sym), len, offset);
    add_memrange (collect, 0, offset, len);	/* 0 == memory */
    break;
  case LOC_REGISTER:
  case LOC_REGPARM:
    reg = SYMBOL_VALUE (sym); 
    if (info_verbose)
      printf_filtered ("LOC_REG[parm] %s: ", SYMBOL_NAME (sym));
    add_register (collect, reg);
    break;
  case LOC_ARG:
  case LOC_REF_ARG:
    printf_filtered ("Sorry, don't know how to do LOC_ARGs yet.\n");
    printf_filtered ("       (will not collect %s)\n", 
		     SYMBOL_NAME (sym));
    break;
  case LOC_REGPARM_ADDR:
    reg = SYMBOL_VALUE (sym);
    offset = 0;
    if (info_verbose)
      {
	printf_filtered ("LOC_REGPARM_ADDR %s: Collect %d bytes at offset %d from reg %d\n", 
			 SYMBOL_NAME (sym), len, offset, reg);
      }
    add_memrange (collect, reg, offset, len);
    break;
  case LOC_LOCAL:
  case LOC_LOCAL_ARG:
    offset = SYMBOL_VALUE (sym);
    reg = FP_REGNUM;
    if (info_verbose)
      {
	printf_filtered ("LOC_LOCAL %s: Collect %d bytes at offset %d from frame ptr reg %d\n", 
			 SYMBOL_NAME (sym), len, offset, reg);
      }
    add_memrange (collect, reg, offset, len);
    break;
  case LOC_BASEREG:
  case LOC_BASEREG_ARG:
    reg = SYMBOL_BASEREG (sym);
    offset  = SYMBOL_VALUE (sym);
    if (info_verbose)
      {
	printf_filtered ("LOC_BASEREG %s: collect %d bytes at offset %d from basereg %d\n", 
			 SYMBOL_NAME (sym), len, offset, reg);
      }
    add_memrange (collect, reg, offset, len);
    break;
  case LOC_UNRESOLVED:
    printf_filtered ("Don't know LOC_UNRESOLVED %s\n", SYMBOL_NAME (sym));
    break;
  case LOC_OPTIMIZED_OUT:
    printf_filtered ("%s has been optimized out of existance.\n",
		     SYMBOL_NAME (sym));
    break;
  }
}

static void
add_local_symbols (collect, pc, type)
     struct collection_list *collect;
     CORE_ADDR pc;
     char type;
{
  struct symbol *sym;
  struct block  *block;
  int i, nsyms, count = 0;

  block = block_for_pc (pc);
  while (block != 0)
    {
      nsyms = BLOCK_NSYMS (block);
      for (i = 0; i < nsyms; i++)
	{
	  sym = BLOCK_SYM (block, i);
	  switch (SYMBOL_CLASS (sym)) {
	  case LOC_LOCAL:
	  case LOC_STATIC:
	  case LOC_REGISTER:
	  case LOC_BASEREG:
	    if (type == 'L')	/* collecting Locals */
	      {
		count++;
		collect_symbol (collect, sym);
	      }
	    break;
	  case LOC_ARG:
	  case LOC_LOCAL_ARG:
	  case LOC_REF_ARG:
	  case LOC_REGPARM:
	  case LOC_REGPARM_ADDR:
	  case LOC_BASEREG_ARG:
	    if (type == 'A')	/* collecting Arguments */
	      {
		count++;
		collect_symbol (collect, sym);
	      }
	  }
	}
      if (BLOCK_FUNCTION (block))
	break;
      else
	block = BLOCK_SUPERBLOCK (block);
    }
  if (count == 0)
    warning ("No %s found in scope.", type == 'L' ? "locals" : "args");
}

static void
clear_collection_list (list)
     struct collection_list *list;
{
  list->next_memrange = 0;
  memset (list->regs_mask, 0, sizeof (list->regs_mask));
}

static char *
stringify_collection_list (list, string)
     struct collection_list *list;
     char *string;
{
  char *end = string;
  long  i;

  for (i = sizeof (list->regs_mask) - 1; i > 0; i--)
    if (list->regs_mask[i] != 0)	/* skip leading zeroes in regs_mask */
      break;
  if (list->regs_mask[i] != 0)	/* prepare to send regs_mask to the stub */
    {
      if (info_verbose)
	printf_filtered ("\nCollecting registers (mask): 0x");
      *end++='R';
      for (; i >= 0; i--)
	{
	  if (info_verbose)
	    printf_filtered ("%02X", list->regs_mask[i]);
	  sprintf (end,  "%02X", list->regs_mask[i]);
	  end += 2;
	}
    }
  if (info_verbose)
    printf_filtered ("\n");
  if (list->next_memrange > 0 && info_verbose)
    printf_filtered ("Collecting memranges: \n");
  for (i = 0; i < list->next_memrange; i++)
    {
      if (info_verbose)
	printf_filtered ("(%d, 0x%x, %d)\n", 
			 list->list[i].type, 
			 list->list[i].start, 
			 list->list[i].end - list->list[i].start);
      sprintf (end, "M%X,%X,%X", 
	       list->list[i].type, 
	       list->list[i].start, 
	       list->list[i].end - list->list[i].start);
      end += strlen (end);
    }
  if (end == string)
    return NULL;
  else
    return string;
}

static void
encode_actions (t, tdp_actions, step_count, stepping_actions)
     struct tracepoint  *t;
     char              **tdp_actions;
     unsigned long      *step_count;
     char              **stepping_actions;
{
  struct expression  *exp;
  static char        tdp_buff[2048], step_buff[2048];
  struct action_line *action;
  char               *action_exp;
  bfd_signed_vma      offset;
  long                i;
  struct collection_list *collect;

  clear_collection_list (&tracepoint_list);
  clear_collection_list (&stepping_list);
  collect = &tracepoint_list;

  *tdp_actions = NULL;
  *stepping_actions = NULL;

  for (action = t->actions; action; action = action->next)
    {
      action_exp = action->action;
      while (isspace (*action_exp))
	action_exp++;

      if (0 == strncasecmp (action_exp, "collect", 7))
	{
	  action_exp = action_exp + 7;
	  do {	/* repeat over a comma-separated list */
	    while (isspace (*action_exp))
	      action_exp++;

	    if (0 == strncasecmp ("$reg", action_exp, 4))
	      {
		for (i = 0; i < NUM_REGS; i++)
		  add_register (collect, i);
		action_exp = (char *) strchr (action_exp, ','); /* more? */
	      }
	    else if (0 == strncasecmp ("$arg", action_exp, 4))
	      {
		add_local_symbols (collect, t->address, 'A');
		action_exp = (char *) strchr (action_exp, ','); /* more? */
	      }
	    else if (0 == strncasecmp ("$loc", action_exp, 4))
	      {
		add_local_symbols (collect, t->address, 'L');
		action_exp = (char *) strchr (action_exp, ','); /* more? */
	      }
	    else
	      {
		unsigned long addr, len;

		exp = parse_exp_1 (&action_exp, block_for_pc (t->address), 1);
		switch (exp->elts[0].opcode) {
		case OP_REGISTER:
		  i = exp->elts[1].longconst; 
		  if (info_verbose)
		    printf_filtered ("OP_REGISTER: ");
		  add_register (collect, i);
		  break;
		case OP_VAR_VALUE:
		  collect_symbol (collect, exp->elts[2].symbol);
		  break;
#if 0
		case OP_LONG:
		  addr = exp->elts[2].longconst;
		  if (*action_exp == ':')
		    {
		      exp = parse_exp_1 (&action_exp, 
					 block_for_pc (t->address), 
					 1);
		      if (exp->elts[0].opcode == OP_LONG)
			len = exp->elts[2].longconst;
		      else
			error ("length field requires a literal long const");
		    }
		  else 
		    len = 4;

		  add_memrange (collect, 0, addr, len);
		  break;
#endif
		}
	      }
	  } while (action_exp && *action_exp++ == ',');
	}
      else if (0 == strncasecmp (action_exp, "while-stepping", 14))
	{
	  collect = &stepping_list;
	}
      else if (0 == strncasecmp (action_exp, "end", 3))
	{
	  if (collect == &stepping_list)	/* end stepping actions */
	    collect = &tracepoint_list;
	  else
	    break;			/* end tracepoint actions */
	}
    }
  memrange_sortmerge (&tracepoint_list); 
  memrange_sortmerge (&stepping_list); 

  *tdp_actions      = stringify_collection_list (&tracepoint_list, &tdp_buff);
  *stepping_actions = stringify_collection_list (&stepping_list,   &step_buff);
}

static char target_buf[2048];

static void
trace_start_command (args, from_tty)
     char *args;
     int from_tty;
{ /* STUB_COMM MOSTLY_IMPLEMENTED */
  struct tracepoint *t;
  char buf[2048];
  char *tdp_actions;
  char *stepping_actions;
  unsigned long step_count;

  dont_repeat ();	/* like "run", dangerous to repeat accidentally */
  
  if (target_is_remote ())
    {
      putpkt ("QTinit");
      remote_get_noisy_reply (target_buf);
      if (strcmp (target_buf, "OK"))
	error ("Target does not support this command.");

      ALL_TRACEPOINTS (t)
	{
	  int ss_count;		/* if actions include singlestepping */
	  int disable_mask;	/* ??? */
	  int enable_mask;	/* ??? */

	  sprintf (buf, "QTDP:%x:%x:%c:%x:%x", t->number, t->address, 
		   t->enabled == enabled ? 'E' : 'D', 
		   t->step_count, t->pass_count);
	  if (t->actions)
	    {
	      encode_actions (t, &tdp_actions, &step_count, &stepping_actions);
	      /* do_single_steps (t); */
	      if (tdp_actions)
		{
		  if (strlen (buf) + strlen (tdp_actions) >= sizeof (buf))
		    error ("Actions for tracepoint %d too complex; "
			   "please simplify.", t->number);
		  strcat (buf, tdp_actions);
		}
	      if (stepping_actions)
		{
		  strcat (buf, "S");
		  if (strlen (buf) + strlen (stepping_actions) >= sizeof (buf))
		    error ("Actions for tracepoint %d too complex; "
			   "please simplify.", t->number);
		  strcat (buf, stepping_actions);
		}
	    }
	  putpkt (buf);
	  remote_get_noisy_reply (target_buf);
	  if (strcmp (target_buf, "OK"))
	    error ("Target does not support tracepoints.");
	}
      putpkt ("QTStart");
      remote_get_noisy_reply (target_buf);
      if (strcmp (target_buf, "OK"))
	error ("Bogus reply from target: %s", target_buf);
    }
  else
    printf_filtered ("Trace can only be run on remote targets.\n");
}

static void
trace_stop_command (args, from_tty)
     char *args;
     int from_tty;
{ /* STUB_COMM IS_IMPLEMENTED */
  if (target_is_remote ())
    {
      putpkt ("QTStop");
      remote_get_noisy_reply (target_buf);
      if (strcmp (target_buf, "OK"))
	error ("Bogus reply from target: %s", target_buf);
    }
  else
    error ("Trace can only be run on remote targets.");
}

static void
trace_status_command (args, from_tty)
     char *args;
     int from_tty;
{ /* STUB_COMM IS_IMPLEMENTED */
  if (target_is_remote ())
    {
      putpkt ("qTStatus");
      remote_get_noisy_reply (target_buf);
      if (strcmp (target_buf, "OK"))
	error ("Bogus reply from target: %s", target_buf);
    }
  else
    error ("Trace can only be run on remote targets.");
}

static void
trace_buff_command (args, from_tty)
     char *args;
     int from_tty;
{ /* STUB_COMM NOT_IMPLEMENTED */
  if (args == 0 || *args == 0)
    printf_filtered ("TBUFFER command requires argument (on or off)\n");
  else if (strcasecmp (args, "on") == 0)
    printf_filtered ("tbuffer overflow on.\n");
  else if (strcasecmp (args, "off") == 0)
    printf_filtered ("tbuffer overflow off.\n");
  else
    printf_filtered ("TBUFFER: unknown argument (use on or off)\n");
}

static void
trace_limit_command (args, from_tty)
     char *args;
     int from_tty;
{ /* STUB_COMM NOT_IMPLEMENTED */
  printf_filtered ("Limit it to what?\n");
}

static void
trace_find_command (args, from_tty)
     char *args;
     int from_tty;
{ /* STUB_COMM PART_IMPLEMENTED */
  /* this should only be called with a numeric argument */
  int frameno, target_frameno;
  char buf[40], *tmp;
  
  if (target_is_remote ())
    {
      if (args == 0 || *args == 0)
	frameno = traceframe_number + 1;
      else if (*args != '-' && !isdigit(*args))
	error ("tfind requires a literal (for now): %s rejected.", args);
      else
	frameno = strtol (args, 0, 0);  /* for now, literals only */

      sprintf (buf, "QTFrame:%x", frameno);
      putpkt (buf);
      remote_get_noisy_reply (target_buf);

      if (target_buf[0] != 'F')
	error ("Bogus reply from target: %s", target_buf);
      target_frameno = strtol (&target_buf[1], &tmp, 16);
      if (tmp == &target_buf[1])
	error ("Bogus reply from target: %s", target_buf);
      if (target_frameno != frameno)
	warning ("Target replied with different framenumber, %s != %x",
		 target_buf, frameno);

      set_traceframe_num (target_frameno);
      flush_cached_frames ();
      registers_changed ();
      select_frame (get_current_frame (), 0);
    }
  else
    error ("Trace can only be run on remote targets.");
}

static void
trace_find_pc_command (args, from_tty)
     char *args;
     int from_tty;
{ /* STUB_COMM PART_IMPLEMENTED */
  CORE_ADDR pc;
  int target_frameno;
  char buf[40], *tmp;

  if (target_is_remote ())
    {
      if (args == 0 || *args == 0)
	{	/* TFIND PC <no args> is the same as TFIND <no args> */
	  trace_find_command (args, from_tty);
	  return;
	}
      if (!isdigit(*args))
	error ("tfind pc requires a literal argument (for now): %s rejected.",
	       args);

      pc = strtol (args, 0, 0);  /* for now, literals only */
      sprintf (buf, "QTFrame:pc:%x", pc);
      putpkt (buf);
      remote_get_noisy_reply (target_buf);

      if (target_buf[0] != 'F')
	error ("Bogus reply from target: %s", target_buf);
      target_frameno = strtol (&target_buf[1], &tmp, 16);
      if (tmp == &target_buf[1])
	error ("Bogus reply from target: %s", target_buf);

      set_traceframe_num (target_frameno);
      flush_cached_frames ();
      registers_changed ();
      select_frame (get_current_frame (), 0);
    }
  else
    error ("Trace can only be run on remote targets.");
}

static void
trace_find_tdp_command (args, from_tty)
     char *args;
     int from_tty;
{ /* STUB_COMM PART_IMPLEMENTED */
  int target_frameno, tdp;
  char buf[40], *tmp;

  if (target_is_remote ())
    {
      if (args == 0 || *args == 0)
	{ /* TFIND TDP <no args> is the same as TFIND <no args> */
	  trace_find_command (args, from_tty);
	  return;
	}
      if (!isdigit(*args))
	error ("tfind tdp command requires a literal argument (for now): %s",
	       args);

      tdp = strtol (args, 0, 0);  /* for now, literals only */
      sprintf (buf, "QTFrame:tdp:%x", tdp);
      putpkt (buf);
      remote_get_noisy_reply (target_buf);

      if (target_buf[0] != 'F')
	error ("Bogus reply from target: %s", target_buf);
      target_frameno = strtol (&target_buf[1], &tmp, 16);
      if (tmp == &target_buf[1])
	error ("Bogus reply from target: %s", target_buf);

      set_traceframe_num (target_frameno);
      flush_cached_frames ();
      registers_changed ();
      select_frame (get_current_frame (), 0);
    }
  else
    error ("Trace can only be run on remote targets.");
}

static void
trace_find_range_command (args, from_tty)
     char *args;
     int from_tty;
{ /* STUB_COMM PART_IMPLEMENTED */
  static CORE_ADDR start, stop;
  int target_frameno;
  char buf[50], *tmp;

  if (target_is_remote ())
    {
      if (args == 0 || *args == 0 || !isdigit(*args))
	{ /* XXX FIXME: what should default behavior be? */
	  printf_filtered ("Usage: tfind range <address> <address>\n");
	  return;
	}

      start = strtol (args, &args, 0); /* for now, literals only */
      while (args && *args && isspace (*args))
	args++;

      if (args == 0 || *args == 0 || !isdigit(*args))
	{
	  printf_filtered ("Usage: tfind range <address> <address>\n");
	  return;
	}

      stop = strtol (args, &args, 0); /* for now, literals only */

      sprintf (buf, "QTFrame:range:%x:%x", start, stop);
      putpkt (buf);
      remote_get_noisy_reply (target_buf);

      if (target_buf[0] != 'F')
	error ("Bogus reply from target: %s", target_buf);
      target_frameno = strtol (&target_buf[1], &tmp, 16);
      if (tmp == &target_buf[1])
	error ("Bogus reply from target: %s", target_buf);

      set_traceframe_num (target_frameno);
      flush_cached_frames ();
      registers_changed ();
      select_frame (get_current_frame (), 0);
    }
  else
      error ("Trace can only be run on remote targets.");
}

static void
trace_find_outside_command (args, from_tty)
     char *args;
     int from_tty;
{ /* STUB_COMM PART_IMPLEMENTED */
  CORE_ADDR start, stop;
  int target_frameno;
  char buf[50], *tmp;

  if (target_is_remote ())
    {
      if (args == 0 || *args == 0 || !isdigit(*args))
	{ /* XXX FIXME: what should default behavior be? */
	  printf_filtered ("Usage: tfind outside <address> <address>\n");
	  return;
	}

      start = strtol (args, &args, 0);
      while (args && *args && isspace (*args))
	args++;

      if (args == 0 || *args == 0 || !isdigit(*args))
	{
	  printf_filtered ("Usage: tfind outside <address> <address>\n");
	  return;
	}

      stop = strtol (args, &args, 0);

      sprintf (buf, "QTFrame:outside:%x:%x", start, stop);
      putpkt (buf);
      remote_get_noisy_reply (target_buf);

      if (target_buf[0] != 'F')
	error ("Bogus reply from target: %s", target_buf);
      target_frameno = strtol (&target_buf[1], &tmp, 16);
      if (tmp == &target_buf[1])
	error ("Bogus reply from target: %s", target_buf);

      set_traceframe_num (target_frameno);
      flush_cached_frames ();
      registers_changed ();
      select_frame (get_current_frame (), 0);
    }
  else
      error ("Trace can only be run on remote targets.");
}

static void
trace_display_command (args, from_tty)
     char *args;
     int from_tty;
{ /* STUB_COMM NOT_IMPLEMENTED */
  if (!target_is_remote ())
    {
      printf_filtered ("Trace can only be run on remote targets.\n");
      return;
    }

  if (args && *args)
    printf_filtered ("Displaying trace as %s.\n", args);
  else
    printf_filtered ("Displaying trace.\n");
}

static void
tracepoint_save_command (args, from_tty)
     char *args;
     int from_tty;
{
  struct tracepoint  *tp;
  struct action_line *line;
  FILE *fp;
  char *i1 = "    ", *i2 = "      ";
  char *indent, *actionline;

  if (args == 0 || *args == 0)
    error ("Argument required (file name in which to save tracepoints");

  if (tracepoint_chain == 0)
    {
      warning ("save-tracepoints: no tracepoints to save.\n");
      return;
    }

  if (!(fp = fopen (args, "w")))
    error ("Unable to open file '%s' for saving tracepoints");

  ALL_TRACEPOINTS (tp)
    {
      if (tp->addr_string)
	fprintf (fp, "trace %s\n", tp->addr_string);
      else
	fprintf (fp, "trace *0x%x\n", tp->address);

      if (tp->pass_count)
	fprintf (fp, "  passcount %d\n", tp->pass_count);

      if (tp->actions)
	{
	  fprintf (fp, "  actions\n");
	  indent = i1;
	  for (line = tp->actions; line; line = line->next)
	    {
	      actionline = line->action;
	      while (isspace(*actionline))
		actionline++;

	      fprintf (fp, "%s%s\n", indent, actionline);
	      if (0 == strncasecmp (actionline, "while-stepping", 14))
		indent = i2;
	      else if (0 == strncasecmp (actionline, "end", 3))
		indent = i1;
	    }
	}
    }
  fclose (fp);
  if (from_tty)
    printf_filtered ("Tracepoints saved to file '%s'.\n", args);
  return;
}

static void
scope_info (args, from_tty)
     char *args;
     int from_tty;
{
  struct symtab_and_line sal;
  struct symtabs_and_lines sals;
  struct symbol *sym;
  struct block *block;
  char **canonical, *save_args = args, *symname;
  int i, nsyms, count = 0;
    enum address_class aclass;

  if (args == 0 || *args == 0)
    error ("requires an argument (function, line or *addr) to define a scope");

  sals = decode_line_1 (&args, 1, NULL, 0, &canonical);
  if (sals.nelts == 0)
    return;		/* presumably decode_line_1 has already warned */

  printf_filtered ("Block for %s", save_args);
  /* Resolve all line numbers to PC's */
  for (i = 0; i < sals.nelts; i++)
    resolve_sal_pc (&sals.sals[i]);

  block = block_for_pc (sals.sals[0].pc);
  while (block != 0)
    {
      nsyms = BLOCK_NSYMS (block);
      for (i = 0; i < nsyms; i++)
	{
	  count++;
	  sym = BLOCK_SYM (block, i);
	  switch (SYMBOL_CLASS (sym)) {
	  default:
	  case LOC_UNDEF:		/* messed up symbol? */
	    symname = SYMBOL_NAME (sym);
	    if (symname && *symname)	/* guard against messed up name */
	      printf_filtered ("Bogus symbol %s, class %d\n", 
			       symname, SYMBOL_CLASS (sym));
	    else 
	      printf_filtered ("Completely bogus symbol, class %d.\n",
			       SYMBOL_CLASS (sym));
	    count--;			/* don't count this one */
	    continue;
	  case LOC_CONST:	 printf_filtered ("\nConstant       ");	break;
	  case LOC_STATIC:	 printf_filtered ("\nStatic         ");	break;
	  case LOC_REGISTER:	 printf_filtered ("\nRegister       ");	break;
	  case LOC_ARG:		 printf_filtered ("\nArg            ");	break;
	  case LOC_REF_ARG:	 printf_filtered ("\nReference Arg  ");	break;
	  case LOC_REGPARM:	 printf_filtered ("\nRegister Arg   ");	break;
	  case LOC_REGPARM_ADDR:
	    printf_filtered ("\nIndirect Register Arg ");		break;
	  case LOC_LOCAL:	 printf_filtered ("\nStack Local    ");	break;
	  case LOC_TYPEDEF:	 printf_filtered ("\nLocal Typedef  ");	break;
	  case LOC_LABEL:	 printf_filtered ("\nLocal Label    ");	break;
	  case LOC_BLOCK:	 printf_filtered ("\nLocal Function ");	break;
	  case LOC_CONST_BYTES:	 printf_filtered ("\nLoc. Byte Seq. ");	break;
	  case LOC_LOCAL_ARG:	 printf_filtered ("\nStack Arg      ");	break;
	  case LOC_BASEREG:	 printf_filtered ("\nBasereg Local  ");	break;
	  case LOC_BASEREG_ARG:	 printf_filtered ("\nBasereg Arg    ");	break;
	  case LOC_UNRESOLVED:
	    printf_filtered ("\nUnresolved Static ");			break;
	  case LOC_OPTIMIZED_OUT: printf_filtered ("\nOptimized-Out ");	break;
	  }
	  type_print (SYMBOL_TYPE (sym), SYMBOL_NAME (sym), gdb_stdout, -1);
	}
      if (BLOCK_FUNCTION (block))
	break;
      else
	block = BLOCK_SUPERBLOCK (block);
    }
  if (count <= 0)
    printf_filtered (" contains no locals or arguments.");
  printf_filtered ("\n");
}

static struct cmd_list_element *tfindlist;
static struct cmd_list_element *tracelist;

void
_initialize_tracepoint ()
{
  tracepoint_chain  = 0;
  tracepoint_count  = 0;
  traceframe_number = 0;

  set_internalvar (lookup_internalvar ("tpnum"), 
		   value_from_longest (builtin_type_int, (LONGEST) 0));
  set_internalvar (lookup_internalvar ("trace_frame"), 
		   value_from_longest (builtin_type_int, (LONGEST) 0));

  if (tracepoint_list.list == NULL)
    {
      tracepoint_list.listsize = 128;
      tracepoint_list.list = xmalloc 
	(tracepoint_list.listsize * sizeof (struct memrange));
    }
  if (stepping_list.list == NULL)
    {
      stepping_list.listsize = 128;
      stepping_list.list = xmalloc 
	(stepping_list.listsize * sizeof (struct memrange));
    }

  add_info ("scope", scope_info, 
	    "List the variables local to a scope");

#if 1
  add_cmd ("tracepoints", class_trace, NO_FUNCTION, 
	   "Tracing program execution without stopping the program.", 
	   &cmdlist);

  add_info ("tracepoints", tracepoints_info,
	    "Display tracepoints, or tracepoint number NUMBER.\n"
	    "Convenience variable \"$tpnum\" contains the number of the\n"
	    "last tracepoint set.");

  add_info_alias ("tp", "tracepoints", 1);

  add_com ("save-tracepoints", class_trace, tracepoint_save_command, 
	   "Save current tracepoint definitions as a script.\n"
	   "Use the SOURCE command in another debug session to restore them.");

  add_com ("tlimit",  class_trace, trace_limit_command,
	   "Not sure what this should do yet....");

  add_com ("tbuffer",  class_trace, trace_buff_command,
	   "See also 'set trace buffer overflow'.");

  add_prefix_cmd ("tfind",  class_trace,
		  trace_find_command,
		  "Select a trace frame (default by frame number).",
		  &tfindlist, "tfind ", 1, &cmdlist);

  add_cmd ("outside", class_trace, trace_find_outside_command,
	   "Select a trace frame by falling outside of a PC range", 
	   &tfindlist);

  add_cmd ("range", class_trace, trace_find_range_command,
	   "Select a trace frame by PC range", &tfindlist);

  add_cmd ("tdp", class_trace, trace_find_tdp_command,
	   "Select a trace frame by TDP", &tfindlist);

  add_cmd ("pc", class_trace, trace_find_pc_command,
	   "Select a trace frame by PC", &tfindlist);

  add_com ("tdisplay",  class_trace, trace_display_command,
	   "Display the results of a trace");

  add_com ("tstatus",  class_trace, trace_status_command,
	   "Inquire about trace data collection status.");

  add_com ("tstop",  class_trace, trace_stop_command,
	   "Stop trace data collection.");

  add_com ("tstart", class_trace, trace_start_command,
	   "Start trace data collection.");

  add_com ("passcount", class_trace, trace_pass_command, 
	   "Set the passcount for a tracepoint.\n"
	   "The trace will end when the tracepoint has been passed "
	   "'count' times.\n"
	   "Usage: passcount COUNT TPNUM, where TPNUM may also be \"all\",\n"
	   "or if omitted refers to the last tracepoint defined.");

  add_com ("actions", class_trace, trace_actions_command,
	   "Specify the actions to be taken at a tracepoint.\n"
	   "Actions can include collection of data, enabling or \n"
	   "disabling other tracepoints, or ending the trace.");

  add_cmd ("tracepoints", class_trace, delete_trace_command, 
	   "Delete specified tracepoints; or with no argument, delete all.",
	   &deletelist);

  add_cmd ("tracepoints", class_trace, disable_trace_command, 
	   "Disable specified tracepoints; or with no argument, disable all.",
	   &disablelist);

  add_cmd ("tracepoints", class_trace, enable_trace_command, 
	   "Enable specified tracepoints; or with no argument, enable all.", 
	   &enablelist);

  add_com ("trace", class_trace, trace_command,
	   "Set a tracepoint at a specified line, function, or address.\n"
	   "Argument may be a line number, function name, or "
	   "'*' plus an address.\n"
	   "For a line number or function, trace at the start of its code.\n"
	   "If an address is specified, trace at that exact address.");

  add_com_alias ("tp",   "trace", class_alias, 0);
  add_com_alias ("tr",   "trace", class_alias, 1);
  add_com_alias ("tra",  "trace", class_alias, 1);
  add_com_alias ("trac", "trace", class_alias, 1);


#else /* command set based on TRACE as a prefix (incomplete) */
  add_cmd ("tracepoints", class_trace, NO_FUNCTION, 
	   "Tracing program execution without stopping the program.", 
	   &cmdlist);

  add_prefix_cmd ("trace", class_trace, trace_command, 
		  "prefix for tracing commands", 
		  &tracelist, "trace ", 1, &cmdlist);

  add_cmd ("limit", class_trace, trace_limit_command, 
	   "Not sure what the hell this does....", &tracelist);

  add_cmd ("buffer", class_trace, trace_buff_command, 
	   "See also 'set trace buffer overflow'.", &tracelist);

  add_prefix_cmd ("find", class_trace, trace_find_command, 
		  "Select a trace frame (default by frame number).", 
		  &tfindlist, "trace find ", 1, &tracelist);

  add_cmd ("outside", class_trace, trace_find_outside_command, 
	   "Select a tracepoint by falling outside of a PC range.", 
	   &tfindlist);

  add_cmd ("range", class_trace, trace_find_range_command, 
	   "Select a tracepoint by PC range.", 
	   &tfindlist);

  add_cmd ("pc", class_trace, trace_find_pc_command, 
	   "Select a tracepoint by PC.", 
	   &tfindlist);

  add_cmd ("display", class_trace, trace_display_command, 
	   "Display the results of a trace.", &tracelist);

  add_cmd ("delete", class_trace, delete_trace_command, 
	   "Delete some tracepoints; no argument means all.", &tracelist);

  add_cmd ("disable", class_trace, disable_trace_command, 
	   "Disable some tracepoints; no argument means all.", &tracelist);

  add_cmd ("enable", class_trace, enable_trace_command, 
	   "Enable some tracepoints; no argument means all.", &tracelist);

  add_cmd ("tracepoints", class_trace, delete_trace_command, 
	   "Delete some tracepoints; no argument means all.",
	   &deletelist);

  add_cmd ("tracepoints", class_trace, disable_trace_command, 
	   "Disable some tracepoints; no argument means all.",
	   &disablelist);

  add_cmd ("tracepoints", class_trace, enable_trace_command, 
	   "Enable some tracepoints; no argument means all.", 
	   &enablelist);

  add_cmd ("at", class_trace, trace_command, 
	   "Set a tracepoint at a specified line or function.\n"
	   "Argument may be a line number, function name, or "
	   "'*' and an address.\n"
	   "For a line number or function, trace from the start of "
	   "its code.\n"
	   "If an address is specified, trace at that exact address.\n"
	   "With no arg, uses current execution address of "
	   "selected stack frame.\n"
	   "This is useful for breaking on return to a stack frame.", 
	   &tracelist);

  add_cmd ("status", class_trace, trace_status_command, 
	   "Inquire about trace data collection status.", &tracelist);

  add_cmd ("stop", class_trace, trace_stop_command, 
	   "Stop trace data collection.", &tracelist);
  
  add_cmd ("start", class_trace, trace_start_command, 
	   "Start trace data collection.", &tracelist);
  
  add_cmd ("info", class_info, tracepoints_info,
	   "Status of tracepoints, or tracepoint number NUMBER.\n"
	   "The \"Address\" and \"What\" columns indicate the\n"
	   "address and file/line number respectively.\n\n"
	   "Convenience variable \"$tpnum\" contains the number of the\n"
	   "last tracepoint set.", 
	   &tracelist);

  add_info ("tracepoints", tracepoints_info,
	    "Status of tracepoints, or tracepoint number NUMBER.\n"
	    "The \"Address\" and \"What\" columns indicate the\n"
	    "address and file/line number respectively.\n\n"
	    "Convenience variable \"$tpnum\" contains the number of the\n"
	    "last tracepoint set.");

  add_info_alias ("tp", "tracepoints", 1);

#endif
}
