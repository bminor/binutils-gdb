/* Everything about breakpoints, for GDB.
   Copyright (C) 1986, 1987, 1989, 1990 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <ctype.h>
#include "defs.h"
#include "param.h"
#include "symtab.h"
#include "frame.h"
#include "breakpoint.h"
#include "expression.h"
#include "gdbcore.h"
#include "gdbcmd.h"
#include "value.h"
#include "ctype.h"
#include "command.h"
#include "inferior.h"
#include "target.h"
#include <string.h>

extern int addressprint;		/* Print machine addresses? */
extern int demangle;			/* Print de-mangled symbol names? */

extern int catch_errors ();
extern void set_next_address ();	/* ...for x/ command */

/* Are we executing breakpoint commands?  */
static int executing_breakpoint_commands;

/* States of enablement of breakpoint.
   `temporary' means disable when hit.
   `delete' means delete when hit.  */

enum enable { disabled, enabled, temporary, delete};

/* Not that the ->silent field is not currently used by any commands
   (though the code is in there if it was to be, and set_raw_breakpoint
   does set it to 0).  I implemented it because I thought it would be
   useful for a hack I had to put in; I'm going to leave it in because
   I can see how there might be times when it would indeed be useful */

/* This is for a breakpoint or a watchpoint.  */

struct breakpoint
{
  struct breakpoint *next;
  /* Number assigned to distinguish breakpoints.  */
  int number;
  /* Address to break at, or NULL if not a breakpoint.  */
  CORE_ADDR address;
  /* Line number of this address.  Redundant.  Only matters if address
     is non-NULL.  */
  int line_number;
  /* Symtab of file of this address.  Redundant.  Only matters if address
     is non-NULL.  */
  struct symtab *symtab;
  /* Zero means disabled; remember the info but don't break here.  */
  enum enable enable;
  /* Non-zero means a silent breakpoint (don't print frame info
     if we stop here). */
  unsigned char silent;
  /* Number of stops at this breakpoint that should
     be continued automatically before really stopping.  */
  int ignore_count;
  /* "Real" contents of byte where breakpoint has been inserted.
     Valid only when breakpoints are in the program.  Under the complete
     control of the target insert_breakpoint and remove_breakpoint routines.
     No other code should assume anything about the value(s) here.  */
  char shadow_contents[BREAKPOINT_MAX];
  /* Nonzero if this breakpoint is now inserted.  Only matters if address
     is non-NULL.  */
  char inserted;
  /* Nonzero if this is not the first breakpoint in the list
     for the given address.  Only matters if address is non-NULL.  */
  char duplicate;
  /* Chain of command lines to execute when this breakpoint is hit.  */
  struct command_line *commands;
  /* Stack depth (address of frame).  If nonzero, break only if fp
     equals this.  */
  FRAME_ADDR frame;
  /* Conditional.  Break only if this expression's value is nonzero.  */
  struct expression *cond;

  /* String we used to set the breakpoint (malloc'd).  Only matters if
     address is non-NULL.  */
  char *addr_string;
  /* String form of the breakpoint condition (malloc'd), or NULL if there
     is no condition.  */
  char *cond_string;

  /* The expression we are watching, or NULL if not a watchpoint.  */
  struct expression *exp;
  /* The largest block within which it is valid, or NULL if it is
     valid anywhere (e.g. consists just of global symbols).  */
  struct block *exp_valid_block;
  /* Value of the watchpoint the last time we checked it.  */
  value val;
};

#define ALL_BREAKPOINTS(b)  for (b = breakpoint_chain; b; b = b->next)

/* Chain of all breakpoints defined.  */

struct breakpoint *breakpoint_chain;

/* Number of last breakpoint made.  */

static int breakpoint_count;

/* Set breakpoint count to NUM.  */
static void
set_breakpoint_count (num)
     int num;
{
  breakpoint_count = num;
  set_internalvar (lookup_internalvar ("bpnum"),
		   value_from_long (builtin_type_int, (LONGEST) num));
}

/* Default address, symtab and line to put a breakpoint at
   for "break" command with no arg.
   if default_breakpoint_valid is zero, the other three are
   not valid, and "break" with no arg is an error.

   This set by print_stack_frame, which calls set_default_breakpoint.  */

int default_breakpoint_valid;
CORE_ADDR default_breakpoint_address;
struct symtab *default_breakpoint_symtab;
int default_breakpoint_line;

static void delete_breakpoint ();
void breakpoint_auto_delete ();

/* Flag indicating extra verbosity for xgdb.  */
extern int xgdb_verbose;

/* *PP is a string denoting a breakpoint.  Get the number of the breakpoint.
   Advance *PP after the string and any trailing whitespace.

   Currently the string can either be a number or "$" followed by the name
   of a convenience variable.  Making it an expression wouldn't work well
   for map_breakpoint_numbers (e.g. "4 + 5 + 6").  */
static int
get_number (pp)
     char **pp;
{
  int retval;
  char *p = *pp;

  if (p == NULL)
    /* Empty line means refer to the last breakpoint.  */
    return breakpoint_count;
  else if (*p == '$')
    {
      /* Make a copy of the name, so we can null-terminate it
	 to pass to lookup_internalvar().  */
      char *varname;
      char *start = ++p;
      value val;

      while (isalnum (*p) || *p == '_')
	p++;
      varname = (char *) alloca (p - start + 1);
      strncpy (varname, start, p - start);
      varname[p - start] = '\0';
      val = value_of_internalvar (lookup_internalvar (varname));
      if (TYPE_CODE (VALUE_TYPE (val)) != TYPE_CODE_INT)
	error (
"Convenience variables used to specify breakpoints must have integer values."
	       );
      retval = (int) value_as_long (val);
    }
  else
    {
      while (*p >= '0' && *p <= '9')
	++p;
      if (p == *pp)
	/* There is no number here.  (e.g. "cond a == b").  */
	error_no_arg ("breakpoint number");
      retval = atoi (*pp);
    }
  if (!(isspace (*p) || *p == '\0'))
    error ("breakpoint number expected");
  while (isspace (*p))
    p++;
  *pp = p;
  return retval;
}

/* condition N EXP -- set break condition of breakpoint N to EXP.  */

static void
condition_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  register struct breakpoint *b;
  char *p;
  register int bnum;

  if (arg == 0)
    error_no_arg ("breakpoint number");

  p = arg;
  bnum = get_number (&p);

  ALL_BREAKPOINTS (b)
    if (b->number == bnum)
      {
	if (b->cond)
	  {
	    free (b->cond);
	    b->cond = 0;
	  }
	if (b->cond_string != NULL)
	  free (b->cond_string);

	if (*p == 0)
	  {
	    b->cond = 0;
	    b->cond_string = NULL;
	    if (from_tty)
	      printf ("Breakpoint %d now unconditional.\n", bnum);
	  }
	else
	  {
	    arg = p;
	    /* I don't know if it matters whether this is the string the user
	       typed in or the decompiled expression.  */
	    b->cond_string = savestring (arg, strlen (arg));
	    b->cond = parse_c_1 (&arg, block_for_pc (b->address), 0);
	    if (*arg)
	      error ("Junk at end of expression");
	  }
	return;
      }

  error ("No breakpoint number %d.", bnum);
}

static void
commands_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  register struct breakpoint *b;
  char *p;
  register int bnum;
  struct command_line *l;

  /* If we allowed this, we would have problems with when to
     free the storage, if we change the commands currently
     being read from.  */

  if (executing_breakpoint_commands)
    error ("Can't use the \"commands\" command among a breakpoint's commands.");

  p = arg;
  bnum = get_number (&p);
  if (p && *p)
    error ("Unexpected extra arguments following breakpoint number.");
      
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

extern int memory_breakpoint_size; /* from mem-break.c */

/* Like target_read_memory() but if breakpoints are inserted, return
   the shadow contents instead of the breakpoints themselves.  */
int
read_memory_nobpt (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     unsigned len;
{
  int status;
  struct breakpoint *b;

  if (memory_breakpoint_size < 0)
    /* No breakpoints on this machine.  */
    return target_read_memory (memaddr, myaddr, len);
  
  ALL_BREAKPOINTS (b)
    {
      if (b->address == NULL || !b->inserted)
	continue;
      else if (b->address + memory_breakpoint_size <= memaddr)
	/* The breakpoint is entirely before the chunk of memory
	   we are reading.  */
	continue;
      else if (b->address >= memaddr + len)
	/* The breakpoint is entirely after the chunk of memory we
	   are reading.  */
	continue;
      else
	{
	  /* Copy the breakpoint from the shadow contents, and recurse
	     for the things before and after.  */
	  
	  /* Addresses and length of the part of the breakpoint that
	     we need to copy.  */
	  CORE_ADDR membpt = b->address;
	  unsigned int bptlen = memory_breakpoint_size;
	  /* Offset within shadow_contents.  */
	  int bptoffset = 0;
	  
	  if (membpt < memaddr)
	    {
	      /* Only copy the second part of the breakpoint.  */
	      bptlen -= memaddr - membpt;
	      bptoffset = memaddr - membpt;
	      membpt = memaddr;
	    }

	  if (membpt + bptlen > memaddr + len)
	    {
	      /* Only copy the first part of the breakpoint.  */
	      bptlen -= (membpt + bptlen) - (memaddr + len);
	    }

	  bcopy (b->shadow_contents + bptoffset,
		 myaddr + membpt - memaddr, bptlen);

	  if (membpt > memaddr)
	    {
	      /* Copy the section of memory before the breakpoint.  */
	      status = read_memory_nobpt (memaddr, myaddr, membpt - memaddr);
	      if (status != 0)
		return status;
	    }

	  if (membpt + bptlen < memaddr + len)
	    {
	      /* Copy the section of memory after the breakpoint.  */
	      status = read_memory_nobpt
		(membpt + bptlen,
		 myaddr + membpt + bptlen - memaddr,
		 memaddr + len - (membpt + bptlen));
	      if (status != 0)
		return status;
	    }
	  return 0;
	}
    }
  /* Nothing overlaps.  Just call read_memory_noerr.  */
  return target_read_memory (memaddr, myaddr, len);
}

/* insert_breakpoints is used when starting or continuing the program.
   remove_breakpoints is used when the program stops.
   Both return zero if successful,
   or an `errno' value if could not write the inferior.  */

int
insert_breakpoints ()
{
  register struct breakpoint *b;
  int val = 0;
  int disabled_breaks = 0;

  ALL_BREAKPOINTS (b)
    if (b->address != NULL
	&& b->enable != disabled
	&& ! b->inserted
	&& ! b->duplicate)
      {
	val = target_insert_breakpoint(b->address, b->shadow_contents);
	if (val)
	  {
	    /* Can't set the breakpoint.  */
#if defined (DISABLE_UNSETTABLE_BREAK)
	    if (DISABLE_UNSETTABLE_BREAK (b->address))
	      {
		val = 0;
		b->enable = disabled;
		if (!disabled_breaks)
		  {
		    fprintf (stderr,
			 "Cannot insert breakpoint %d:\n", b->number);
		    printf_filtered ("Disabling shared library breakpoints:\n");
		  }
		disabled_breaks = 1;
		printf_filtered ("%d ", b->number);
	      }
	    else
#endif
	      {
		fprintf (stderr, "Cannot insert breakpoint %d:\n", b->number);
		memory_error (val, b->address);	/* which bombs us out */
	      }
	  }
	else
	  b->inserted = 1;
      }
  if (disabled_breaks)
    printf_filtered ("\n");
  return val;
}

int
remove_breakpoints ()
{
  register struct breakpoint *b;
  int val;

#ifdef BREAKPOINT_DEBUG
  printf ("Removing breakpoints.\n");
#endif /* BREAKPOINT_DEBUG */

  ALL_BREAKPOINTS (b)
    if (b->address != NULL && b->inserted)
      {
	val = target_remove_breakpoint(b->address, b->shadow_contents);
	if (val)
	  return val;
	b->inserted = 0;
#ifdef BREAKPOINT_DEBUG
	printf ("Removed breakpoint at 0x%x, shadow 0x%x, 0x%x.\n",
		b->address, b->shadow_contents[0], b->shadow_contents[1]);
#endif /* BREAKPOINT_DEBUG */
      }

  return 0;
}

/* Clear the "inserted" flag in all breakpoints.
   This is done when the inferior is loaded.  */

void
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

/* bpstat stuff.  External routines' interfaces are documented
   in breakpoint.h.  */
void
bpstat_clear (bsp)
     bpstat *bsp;
{
  bpstat p;
  bpstat q;

  if (bsp == 0)
    return;
  p = *bsp;
  while (p != NULL)
    {
      q = p->next;
      if (p->old_val != NULL)
	value_free (p->old_val);
      free (p);
      p = q;
    }
  *bsp = NULL;
}

bpstat
bpstat_copy (bs)
     bpstat bs;
{
  bpstat p = NULL;
  bpstat tmp;
  bpstat retval;

  if (bs == NULL)
    return bs;

  for (; bs != NULL; bs = bs->next)
    {
      tmp = (bpstat) xmalloc (sizeof (*tmp));
      bcopy (bs, tmp, sizeof (*tmp));
      if (p == NULL)
	/* This is the first thing in the chain.  */
	retval = tmp;
      else
	p->next = tmp;
      p = tmp;
    }
  p->next = NULL;
  return retval;
}

int
bpstat_num (bsp)
     bpstat *bsp;
{
  struct breakpoint *b;

  if ((*bsp) == NULL)
    return 0;			/* No more breakpoint values */
  else
    {
      b = (*bsp)->breakpoint_at;
      *bsp = (*bsp)->next;
      if (b == NULL)
	return -1;		/* breakpoint that's been deleted since */
      else
        return b->number;	/* We have its number */
    }
}

void
bpstat_clear_actions (bs)
     bpstat bs;
{
  for (; bs != NULL; bs = bs->next)
    {
      bs->commands = NULL;
      if (bs->old_val != NULL)
	{
	  value_free (bs->old_val);
	  bs->old_val = NULL;
	}
    }
}

/* Execute all the commands associated with all the breakpoints at this
   location.  Any of these commands could cause the process to proceed
   beyond this point, etc.  We look out for such changes by checking
   the global "breakpoint_proceeded" after each command.  */
void
bpstat_do_actions (bsp)
     bpstat *bsp;
{
  bpstat bs;

top:
  bs = *bsp;

  executing_breakpoint_commands = 1;
  breakpoint_proceeded = 0;
  for (; bs != NULL; bs = bs->next)
    {
      while (bs->commands)
	{
	  char *line = bs->commands->line;
	  bs->commands = bs->commands->next;
	  execute_command (line, 0);
	  /* If the inferior is proceeded by the command, bomb out now.
	     The bpstat chain has been blown away by wait_for_inferior.
	     But since execution has stopped again, there is a new bpstat
	     to look at, so start over.  */
	  if (breakpoint_proceeded)
	    goto top;
	}
    }
  clear_momentary_breakpoints ();

  executing_breakpoint_commands = 0;
}

int
bpstat_print (bs)
     bpstat bs;
{
  /* bs->breakpoint_at can be NULL if it was a momentary breakpoint
     which has since been deleted.  */
  if (bs == NULL || bs->breakpoint_at == NULL)
    return 0;
  
  /* If bpstat_stop_status says don't print, OK, we won't.  An example
     circumstance is when we single-stepped for both a watchpoint and
     for a "stepi" instruction.  The bpstat says that the watchpoint
     explains the stop, but we shouldn't print because the watchpoint's
     value didn't change -- and the real reason we are stopping here
     rather than continuing to step (as the watchpoint would've had us do)
     is because of the "stepi".  */
  if (!bs->print)
    return 0;

  if (bs->breakpoint_at->address != NULL)
    {
      /* I think the user probably only wants to see one breakpoint
	 number, not all of them.  */
      printf_filtered ("\nBreakpoint %d, ", bs->breakpoint_at->number);
      return 0;
    }
      
  if (bs->old_val != NULL)
    {
      printf_filtered ("\nWatchpoint %d, ", bs->breakpoint_at->number);
      print_expression (bs->breakpoint_at->exp, stdout);
      printf_filtered ("\nOld value = ");
      value_print (bs->old_val, stdout, 0, Val_pretty_default);
      printf_filtered ("\nNew value = ");
      value_print (bs->breakpoint_at->val, stdout, 0,
		   Val_pretty_default);
      printf_filtered ("\n");
      value_free (bs->old_val);
      bs->old_val = NULL;
      return 1;
    }

  /* Maybe another breakpoint in the chain caused us to stop.
     (Currently all watchpoints go on the bpstat whether hit or
     not.  That probably could (should) be changed, provided care is taken
     with respect to bpstat_explains_signal).  */
  if (bs->next)
    return bpstat_print (bs->next);

  fprintf_filtered (stderr, "gdb internal error: in bpstat_print\n");
  return 0;
}

/* Evaluate the expression EXP and return 1 if value is zero.
   This is used inside a catch_errors to evaluate the breakpoint condition. 
   The argument is a "struct expression *" that has been cast to int to 
   make it pass through catch_errors.  */

static int
breakpoint_cond_eval (exp)
     int exp;
{
  return value_zerop (evaluate_expression ((struct expression *)exp));
}

/* Allocate a new bpstat and chain it to the current one.  */

static bpstat
bpstat_alloc (b, cbs)
     register struct breakpoint *b;
     bpstat cbs;			/* Current "bs" value */
{
  bpstat bs;

  bs = (bpstat) xmalloc (sizeof (*bs));
  cbs->next = bs;
  bs->breakpoint_at = b;
  /* If the condition is false, etc., don't do the commands.  */
  bs->commands = NULL;
  bs->momentary = b->number == -3;
  bs->old_val = NULL;
  return bs;
}

/* Determine whether we stopped at a breakpoint, etc, or whether we
   don't understand this stop.  Result is a chain of bpstat's such that:

	if we don't understand the stop, the result is a null pointer.

	if we understand why we stopped, the result is not null, and
	the first element of the chain contains summary "stop" and
	"print" flags for the whole chain.

	Each element of the chain refers to a particular breakpoint or
	watchpoint at which we have stopped.  (We may have stopped for
	several reasons.)

	Each element of the chain has valid next, breakpoint_at,
	commands, FIXME??? fields.

 */

	
bpstat
bpstat_stop_status (pc, frame_address)
     CORE_ADDR *pc;
     FRAME_ADDR frame_address;
{
  register struct breakpoint *b;
  int stop = 0;
  int print = 0;
  CORE_ADDR bp_addr;
  /* True if we've hit a breakpoint (as opposed to a watchpoint).  */
  int real_breakpoint = 0;
  /* Root of the chain of bpstat's */
  struct bpstat__struct root_bs[1];
  /* Pointer to the last thing in the chain currently.  */
  bpstat bs = root_bs;

  /* Get the address where the breakpoint would have been.  */
  bp_addr = *pc - DECR_PC_AFTER_BREAK;

  ALL_BREAKPOINTS (b)
    {
      int this_bp_stop;
      int this_bp_print;

      if (b->enable == disabled)
	continue;
      if (b->address != NULL && b->address != bp_addr)
	continue;

      bs = bpstat_alloc (b, bs);	/* Alloc a bpstat to explain stop */

      this_bp_stop = 1;
      this_bp_print = 1;

      if (b->exp != NULL)		/* Watchpoint */
	{
	  int within_current_scope;
	  if (b->exp_valid_block != NULL)
	    within_current_scope =
	      contained_in (get_selected_block (), b->exp_valid_block);
	  else
	    within_current_scope = 1;

	  if (within_current_scope)
	    {
	      /* We use value_{,free_to_}mark because it could be a
		 *long* time before we return to the command level and
		 call free_all_values.  */

	      value mark = value_mark ();
	      value new_val = evaluate_expression (b->exp);
	      if (!value_equal (b->val, new_val))
		{
		  release_value (new_val);
		  value_free_to_mark (mark);
		  bs->old_val = b->val;
		  b->val = new_val;
		  /* We will stop here */
		}
	      else
		{
		  /* Nothing changed, don't do anything.  */
		  value_free_to_mark (mark);
		  continue;
		  /* We won't stop here */
		}
	    }
	  else
	    {
	      /* This seems like the only logical thing to do because
		 if we temporarily ignored the watchpoint, then when
		 we reenter the block in which it is valid it contains
		 garbage (in the case of a function, it may have two
		 garbage values, one before and one after the prologue).
		 So we can't even detect the first assignment to it and
		 watch after that (since the garbage may or may not equal
		 the first value assigned).  */
	      b->enable = disabled;
	      printf_filtered ("\
Watchpoint %d disabled because the program has left the block in\n\
which its expression is valid.\n", b->number);
	      /* We won't stop here */
	      /* FIXME, maybe we should stop here!!! */
	      continue;
	    }
	}
      else
	real_breakpoint = 1;

      if (b->frame && b->frame != frame_address)
	this_bp_stop = 0;
      else
	{
	  int value_zero;

	  if (b->cond)
	    {
	      /* Need to select the frame, with all that implies
		 so that the conditions will have the right context.  */
	      select_frame (get_current_frame (), 0);
	      value_zero
		= catch_errors (breakpoint_cond_eval, (int)(b->cond),
				"Error occurred in testing breakpoint condition.");
	      free_all_values ();
	    }
	  if (b->cond && value_zero)
	    {
	      this_bp_stop = 0;
	    }
	  else if (b->ignore_count > 0)
	    {
	      b->ignore_count--;
	      this_bp_stop = 0;
	    }
	  else
	    {
	      /* We will stop here */
	      if (b->enable == temporary)
		b->enable = disabled;
	      bs->commands = b->commands;
	      if (b->silent)
		this_bp_print = 0;
	      if (bs->commands && !strcmp ("silent", bs->commands->line))
		{
		  bs->commands = bs->commands->next;
		  this_bp_print = 0;
		}
	    }
	}
      if (this_bp_stop)
	stop = 1;
      if (this_bp_print)
	print = 1;
    }

  bs->next = NULL;		/* Terminate the chain */
  bs = root_bs->next;		/* Re-grab the head of the chain */
  if (bs)
    {
      bs->stop = stop;
      bs->print = print;
#if DECR_PC_AFTER_BREAK != 0 || defined (SHIFT_INST_REGS)
      if (real_breakpoint)
	{
	  *pc = bp_addr;
#if defined (SHIFT_INST_REGS)
	  {
	    CORE_ADDR pc = read_register (PC_REGNUM);
	    CORE_ADDR npc = read_register (NPC_REGNUM);
	    if (pc != npc)
	      {
		write_register (NNPC_REGNUM, npc);
		write_register (NPC_REGNUM, pc);
	      }
	  }
#else /* No SHIFT_INST_REGS.  */
	  write_pc (bp_addr);
#endif /* No SHIFT_INST_REGS.  */
	}
#endif /* DECR_PC_AFTER_BREAK != 0.  */
    }
  return bs;
}

int 
bpstat_should_step ()
{
  struct breakpoint *b;
  ALL_BREAKPOINTS (b)
    if (b->enable != disabled && b->exp != NULL)
      return 1;
  return 0;
}

/* Print information on breakpoint number BNUM, or -1 if all.
   If WATCHPOINTS is zero, process only breakpoints; if WATCHPOINTS
   is nonzero, process only watchpoints.  */

static void
breakpoint_1 (bnum, watchpoints)
     int bnum;
     int watchpoints;
{
  register struct breakpoint *b;
  register struct command_line *l;
  register struct symbol *sym;
  CORE_ADDR last_addr = (CORE_ADDR)-1;
  int header_printed = 0;
  
  ALL_BREAKPOINTS (b)
    if (bnum == -1 || bnum == b->number)
      {
	if (b->address == NULL && !watchpoints)
	  {
	    if (bnum == -1)
	      continue;
	    error ("That is a watchpoint, not a breakpoint.");
	  }
	if (b->address != NULL && watchpoints)
	  {
	    if (bnum == -1)
	      continue;
	    error ("That is a breakpoint, not a watchpoint.");
	  }

	if (!header_printed)
	  {
	    if (watchpoints)
	      printf_filtered ("    Enb   Expression\n");
	    else if (addressprint)
	      printf_filtered ("    Enb   Address    Where\n");
	    else
	      printf_filtered ("    Enb   Where\n");
	    header_printed = 1;
	  }

	printf_filtered ("#%-3d %c ", b->number, "nyod"[(int) b->enable]);
	if (b->address == NULL) {
	  printf_filtered (" ");
	  print_expression (b->exp, stdout);
	} else {
	    if (addressprint)
	      printf_filtered (" 0x%08x ", b->address);

	    last_addr = b->address;
	    if (b->symtab)
	      {
		sym = find_pc_function (b->address);
		if (sym)
		  {
		    fputs_filtered (" in ", stdout);
		    fputs_demangled (SYMBOL_NAME (sym), stdout, 1);
		    fputs_filtered (" at ", stdout);
		  }
		fputs_filtered (b->symtab->filename, stdout);
		printf_filtered (":%d", b->line_number);
	      }
	    else
	      print_address_symbolic (b->address, stdout, demangle);
	  }

	printf_filtered ("\n");

	if (b->frame)
	  printf_filtered ("\tstop only in stack frame at 0x%x\n", b->frame);
	if (b->cond)
	  {
	    printf_filtered ("\tstop only if ");
	    print_expression (b->cond, stdout);
	    printf_filtered ("\n");
	  }
	if (b->ignore_count)
	  printf_filtered ("\tignore next %d hits\n", b->ignore_count);
	if ((l = b->commands))
	  while (l)
	    {
	      fputs_filtered ("\t", stdout);
	      fputs_filtered (l->line, stdout);
	      fputs_filtered ("\n", stdout);
	      l = l->next;
	    }
      }

  if (!header_printed)
    {
      char *which = watchpoints ? "watch" : "break";
      if (bnum == -1)
	printf_filtered ("No %spoints.\n", which);
      else
	printf_filtered ("No %spoint numbered %d.\n", which, bnum);
    }

  /* Compare against (CORE_ADDR)-1 in case some compiler decides
     that a comparison of an unsigned with -1 is always false.  */
  if (last_addr != (CORE_ADDR)-1)
    set_next_address (last_addr);
}

static void
breakpoints_info (bnum_exp, from_tty)
     char *bnum_exp;
     int from_tty;
{
  int bnum = -1;

  if (bnum_exp)
    bnum = parse_and_eval_address (bnum_exp);

  breakpoint_1 (bnum, 0);
}

static void
watchpoints_info (bnum_exp, from_tty)
     char *bnum_exp;
     int from_tty;
{
  int bnum = -1;

  if (bnum_exp)
    bnum = parse_and_eval_address (bnum_exp);

  breakpoint_1 (bnum, 1);
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
      printf ("also set at pc 0x%x.\n", pc);
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

  if (address == NULL)		/* Watchpoints are uninteresting */
    return;

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
   Does not print anything.

   ==> This routine should not be called if there is a chance of later
   error(); otherwise it leaves a bogus breakpoint on the chain.  Validate
   your arguments BEFORE calling this routine!  */

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
  b->silent = 0;
  b->ignore_count = 0;
  b->commands = NULL;
  b->frame = NULL;

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
  b->frame = (frame ? FRAME_FP (frame) : 0);
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

/* Tell the user we have just set a breakpoint B.  */
static void
mention (b)
     struct breakpoint *b;
{
  if (b->exp)
    {
      printf_filtered ("Watchpoint %d: ", b->number);
      print_expression (b->exp, stdout);
    }
  else
    {
      printf_filtered ("Breakpoint %d at 0x%x", b->number, b->address);
      if (b->symtab)
	printf_filtered (": file %s, line %d.",
			 b->symtab->filename, b->line_number);
    }
  printf_filtered ("\n");
}

#if 0
/* Nobody calls this currently. */
/* Set a breakpoint from a symtab and line.
   If TEMPFLAG is nonzero, it is a temporary breakpoint.
   ADDR_STRING is a malloc'd string holding the name of where we are
   setting the breakpoint.  This is used later to re-set it after the
   program is relinked and symbols are reloaded.
   Print the same confirmation messages that the breakpoint command prints.  */

void
set_breakpoint (s, line, tempflag, addr_string)
     struct symtab *s;
     int line;
     int tempflag;
     char *addr_string;
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
      set_breakpoint_count (breakpoint_count + 1);
      b->number = breakpoint_count;
      b->cond = 0;
      b->addr_string = addr_string;
      if (tempflag)
	b->enable = temporary;

      mention (b);
    }
}
#endif

/* Set a breakpoint according to ARG (function, linenum or *address)
   and make it temporary if TEMPFLAG is nonzero. */

static void
break_command_1 (arg, tempflag, from_tty)
     char *arg;
     int tempflag, from_tty;
{
  struct symtabs_and_lines sals;
  struct symtab_and_line sal;
  register struct expression *cond = 0;
  register struct breakpoint *b;

  /* Pointers in arg to the start, and one past the end, of the condition.  */
  char *cond_start = NULL;
  char *cond_end;
  /* Pointers in arg to the start, and one past the end,
     of the address part.  */
  char *addr_start = NULL;
  char *addr_end;
  
  int i;
  CORE_ADDR pc;

  sals.sals = NULL;
  sals.nelts = 0;

  sal.line = sal.pc = sal.end = 0;
  sal.symtab = 0;

  /* If no arg given, or if first arg is 'if ', use the default breakpoint. */

  if (!arg || (arg[0] == 'i' && arg[1] == 'f' 
	       && (arg[2] == ' ' || arg[2] == '\t')))
    {
      if (default_breakpoint_valid)
	{
	  sals.sals = (struct symtab_and_line *) 
	    xmalloc (sizeof (struct symtab_and_line));
	  sal.pc = default_breakpoint_address;
	  sal.line = default_breakpoint_line;
	  sal.symtab = default_breakpoint_symtab;
	  sals.sals[0] = sal;
	  sals.nelts = 1;
	}
      else
	error ("No default breakpoint address now.");
    }
  else
    {
      addr_start = arg;

      /* Force almost all breakpoints to be in terms of the
	 current_source_symtab (which is decode_line_1's default).  This
	 should produce the results we want almost all of the time while
	 leaving default_breakpoint_* alone.  */
      if (default_breakpoint_valid
	  && (!current_source_symtab
	      || (arg && (*arg == '+' || *arg == '-'))))
	sals = decode_line_1 (&arg, 1, default_breakpoint_symtab,
			      default_breakpoint_line);
      else
	sals = decode_line_1 (&arg, 1, (struct symtab *)NULL, 0);

      addr_end = arg;
    }
  
  if (! sals.nelts) 
    return;

  for (i = 0; i < sals.nelts; i++)
    {
      sal = sals.sals[i];
      if (sal.pc == 0 && sal.symtab != 0)
	{
	  pc = find_line_pc (sal.symtab, sal.line);
	  if (pc == 0)
	    error ("No line %d in file \"%s\".",
		   sal.line, sal.symtab->filename);
	}
      else 
	pc = sal.pc;
      
      while (arg && *arg)
	{
	  if (arg[0] == 'i' && arg[1] == 'f'
	      && (arg[2] == ' ' || arg[2] == '\t'))
	    {
	      arg += 2;
	      cond_start = arg;
	      cond = parse_c_1 (&arg, block_for_pc (pc), 0);
	      cond_end = arg;
	    }
	  else
	    error ("Junk at end of arguments.");
	}
      sals.sals[i].pc = pc;
    }

  for (i = 0; i < sals.nelts; i++)
    {
      sal = sals.sals[i];

      if (from_tty)
	describe_other_breakpoints (sal.pc);

      b = set_raw_breakpoint (sal);
      set_breakpoint_count (breakpoint_count + 1);
      b->number = breakpoint_count;
      b->cond = cond;
      
      if (addr_start)
	b->addr_string = savestring (addr_start, addr_end - addr_start);
      if (cond_start)
	b->cond_string = savestring (cond_start, cond_end - cond_start);
				     
      if (tempflag)
	b->enable = temporary;

      mention (b);
    }

  if (sals.nelts > 1)
    {
      printf ("Multiple breakpoints were set.\n");
      printf ("Use the \"delete\" command to delete unwanted breakpoints.\n");
    }
  free (sals.sals);
}

void
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
watch_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  struct breakpoint *b;
  struct symtab_and_line sal;
  struct expression *exp;
  struct block *exp_valid_block;
  struct value *val;

  sal.pc = NULL;
  sal.symtab = NULL;
  sal.line = 0;
  
  /* Parse arguments.  */
  innermost_block = NULL;
  exp = parse_c_expression (arg);
  exp_valid_block = innermost_block;
  val = evaluate_expression (exp);
  release_value (val);

  /* Now set up the breakpoint.  */
  b = set_raw_breakpoint (sal);
  set_breakpoint_count (breakpoint_count + 1);
  b->number = breakpoint_count;
  b->exp = exp;
  b->exp_valid_block = exp_valid_block;
  b->val = val;
  b->cond = 0;
  b->cond_string = NULL;
  mention (b);
}

/*
 * Helper routine for the until_command routine in infcmd.c.  Here
 * because it uses the mechanisms of breakpoints.
 */
void
until_break_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  struct symtabs_and_lines sals;
  struct symtab_and_line sal;
  FRAME prev_frame = get_prev_frame (selected_frame);

  clear_proceed_status ();

  /* Set a breakpoint where the user wants it and at return from
     this function */
  
  if (default_breakpoint_valid)
    sals = decode_line_1 (&arg, 1, default_breakpoint_symtab,
			  default_breakpoint_line);
  else
    sals = decode_line_1 (&arg, 1, (struct symtab *)NULL, 0);
  
  if (sals.nelts != 1)
    error ("Couldn't get information on specified line.");
  
  sal = sals.sals[0];
  free (sals.sals);		/* malloc'd, so freed */
  
  if (*arg)
    error ("Junk at end of arguments.");
  
  if (sal.pc == 0 && sal.symtab != 0)
    sal.pc = find_line_pc (sal.symtab, sal.line);
  
  if (sal.pc == 0)
    error ("No line %d in file \"%s\".", sal.line, sal.symtab->filename);
  
  set_momentary_breakpoint (sal, selected_frame);
  
  /* Keep within the current frame */
  
  if (prev_frame)
    {
      struct frame_info *fi;
      
      fi = get_frame_info (prev_frame);
      sal = find_pc_line (fi->pc, 0);
      sal.pc = fi->pc;
      set_momentary_breakpoint (sal, prev_frame);
    }
  
  proceed (-1, -1, 0);
}

/* Set a breakpoint at the catch clause for NAME.  */
static int
catch_breakpoint (name)
     char *name;
{
}

static int
disable_catch_breakpoint ()
{
}

static int
delete_catch_breakpoint ()
{
}

static int
enable_catch_breakpoint ()
{
}

struct sal_chain
{
  struct sal_chain *next;
  struct symtab_and_line sal;
};

/* For each catch clause identified in ARGS, run FUNCTION
   with that clause as an argument.  */
static struct symtabs_and_lines
map_catch_names (args, function)
     char *args;
     int (*function)();
{
  register char *p = args;
  register char *p1;
  struct symtabs_and_lines sals;
  struct sal_chain *sal_chain = 0;

  if (p == 0)
    error_no_arg ("one or more catch names");

  sals.nelts = 0;
  sals.sals = NULL;

  while (*p)
    {
      p1 = p;
      /* Don't swallow conditional part.  */
      if (p1[0] == 'i' && p1[1] == 'f'
	  && (p1[2] == ' ' || p1[2] == '\t'))
	break;

      if (isalpha (*p1))
	{
	  p1++;
	  while (isalnum (*p1) || *p1 == '_' || *p1 == '$')
	    p1++;
	}

      if (*p1 && *p1 != ' ' && *p1 != '\t')
	error ("Arguments must be catch names.");

      *p1 = 0;
#if 0
      if (function (p))
	{
	  struct sal_chain *next
	    = (struct sal_chain *)alloca (sizeof (struct sal_chain));
	  next->next = sal_chain;
	  next->sal = get_catch_sal (p);
	  sal_chain = next;
	  goto win;
	}
#endif
      printf ("No catch clause for exception %s.\n", p);
    win:
      p = p1;
      while (*p == ' ' || *p == '\t') p++;
    }
}

/* This shares a lot of code with `print_frame_label_vars' from stack.c.  */

static struct symtabs_and_lines
get_catch_sals (this_level_only)
     int this_level_only;
{
  extern struct blockvector *blockvector_for_pc ();
  register struct blockvector *bl;
  register struct block *block = get_frame_block (selected_frame);
  int index, have_default = 0;
  struct frame_info *fi = get_frame_info (selected_frame);
  CORE_ADDR pc = fi->pc;
  struct symtabs_and_lines sals;
  struct sal_chain *sal_chain = 0;
  char *blocks_searched;

  sals.nelts = 0;
  sals.sals = NULL;

  if (block == 0)
    error ("No symbol table info available.\n");

  bl = blockvector_for_pc (BLOCK_END (block) - 4, &index);
  blocks_searched = (char *) alloca (BLOCKVECTOR_NBLOCKS (bl) * sizeof (char));
  bzero (blocks_searched, BLOCKVECTOR_NBLOCKS (bl) * sizeof (char));

  while (block != 0)
    {
      CORE_ADDR end = BLOCK_END (block) - 4;
      int last_index;

      if (bl != blockvector_for_pc (end, &index))
	error ("blockvector blotch");
      if (BLOCKVECTOR_BLOCK (bl, index) != block)
	error ("blockvector botch");
      last_index = BLOCKVECTOR_NBLOCKS (bl);
      index += 1;

      /* Don't print out blocks that have gone by.  */
      while (index < last_index
	     && BLOCK_END (BLOCKVECTOR_BLOCK (bl, index)) < pc)
	index++;

      while (index < last_index
	     && BLOCK_END (BLOCKVECTOR_BLOCK (bl, index)) < end)
	{
	  if (blocks_searched[index] == 0)
	    {
	      struct block *b = BLOCKVECTOR_BLOCK (bl, index);
	      int nsyms;
	      register int i;
	      register struct symbol *sym;

	      nsyms = BLOCK_NSYMS (b);

	      for (i = 0; i < nsyms; i++)
		{
		  sym = BLOCK_SYM (b, i);
		  if (! strcmp (SYMBOL_NAME (sym), "default"))
		    {
		      if (have_default)
			continue;
		      have_default = 1;
		    }
		  if (SYMBOL_CLASS (sym) == LOC_LABEL)
		    {
		      struct sal_chain *next = (struct sal_chain *)
			alloca (sizeof (struct sal_chain));
		      next->next = sal_chain;
		      next->sal = find_pc_line (SYMBOL_VALUE_ADDRESS (sym), 0);
		      sal_chain = next;
		    }
		}
	      blocks_searched[index] = 1;
	    }
	  index++;
	}
      if (have_default)
	break;
      if (sal_chain && this_level_only)
	break;

      /* After handling the function's top-level block, stop.
	 Don't continue to its superblock, the block of
	 per-file symbols.  */
      if (BLOCK_FUNCTION (block))
	break;
      block = BLOCK_SUPERBLOCK (block);
    }

  if (sal_chain)
    {
      struct sal_chain *tmp_chain;

      /* Count the number of entries.  */
      for (index = 0, tmp_chain = sal_chain; tmp_chain;
	   tmp_chain = tmp_chain->next)
	index++;

      sals.nelts = index;
      sals.sals = (struct symtab_and_line *)
	xmalloc (index * sizeof (struct symtab_and_line));
      for (index = 0; sal_chain; sal_chain = sal_chain->next, index++)
	sals.sals[index] = sal_chain->sal;
    }

  return sals;
}

/* Commands to deal with catching exceptions.  */

void
catch_command_1 (arg, tempflag, from_tty)
     char *arg;
     int tempflag;
     int from_tty;
{
  /* First, translate ARG into something we can deal with in terms
     of breakpoints.  */

  struct symtabs_and_lines sals;
  struct symtab_and_line sal;
  register struct expression *cond = 0;
  register struct breakpoint *b;
  char *save_arg;
  int i;
  CORE_ADDR pc;

  sal.line = sal.pc = sal.end = 0;
  sal.symtab = 0;

  /* If no arg given, or if first arg is 'if ', all active catch clauses
     are breakpointed. */

  if (!arg || (arg[0] == 'i' && arg[1] == 'f' 
	       && (arg[2] == ' ' || arg[2] == '\t')))
    {
      /* Grab all active catch clauses.  */
      sals = get_catch_sals (0);
    }
  else
    {
      /* Grab selected catch clauses.  */
      error ("catch NAME not implemeneted");
      sals = map_catch_names (arg, catch_breakpoint);
    }

  if (! sals.nelts) 
    return;

  save_arg = arg;
  for (i = 0; i < sals.nelts; i++)
    {
      sal = sals.sals[i];
      if (sal.pc == 0 && sal.symtab != 0)
	{
	  pc = find_line_pc (sal.symtab, sal.line);
	  if (pc == 0)
	    error ("No line %d in file \"%s\".",
		   sal.line, sal.symtab->filename);
	}
      else 
	pc = sal.pc;
      
      while (arg && *arg)
	{
	  if (arg[0] == 'i' && arg[1] == 'f'
	      && (arg[2] == ' ' || arg[2] == '\t'))
	    cond = (struct expression *) parse_c_1 ((arg += 2, &arg),
						    block_for_pc (pc), 0);
	  else
	    error ("Junk at end of arguments.");
	}
      arg = save_arg;
      sals.sals[i].pc = pc;
    }

  for (i = 0; i < sals.nelts; i++)
    {
      sal = sals.sals[i];

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

  if (sals.nelts > 1)
    {
      printf ("Multiple breakpoints were set.\n");
      printf ("Use the \"delete\" command to delete unwanted breakpoints.\n");
    }
  free (sals.sals);
}

/* Disable breakpoints on all catch clauses described in ARGS.  */
static void
disable_catch (args)
     char *args;
{
  /* Map the disable command to catch clauses described in ARGS.  */
}

/* Enable breakpoints on all catch clauses described in ARGS.  */
static void
enable_catch (args)
     char *args;
{
  /* Map the disable command to catch clauses described in ARGS.  */
}

/* Delete breakpoints on all catch clauses in the active scope.  */
static void
delete_catch (args)
     char *args;
{
  /* Map the delete command to catch clauses described in ARGS.  */
}

static void
catch_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  catch_command_1 (arg, 0, from_tty);
}

static void
clear_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  register struct breakpoint *b, *b1;
  struct symtabs_and_lines sals;
  struct symtab_and_line sal;
  register struct breakpoint *found;
  int i;

  if (arg)
    {
      sals = decode_line_spec (arg, 1);
    }
  else
    {
      sals.sals = (struct symtab_and_line *) xmalloc (sizeof (struct symtab_and_line));
      sal.line = default_breakpoint_line;
      sal.symtab = default_breakpoint_symtab;
      sal.pc = 0;
      if (sal.symtab == 0)
	error ("No source file specified.");

      sals.sals[0] = sal;
      sals.nelts = 1;
    }

  for (i = 0; i < sals.nelts; i++)
    {
      /* If exact pc given, clear bpts at that pc.
	 But if sal.pc is zero, clear all bpts on specified line.  */
      sal = sals.sals[i];
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
	       && b->next->address != NULL
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
	{
	  if (arg)
	    error ("No breakpoint at %s.", arg);
	  else
	    error ("No breakpoint at this line.");
	}

      if (found->next) from_tty = 1; /* Always report if deleted more than one */
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
  free (sals.sals);
}

/* Delete breakpoint in BS if they are `delete' breakpoints.
   This is called after any breakpoint is hit, or after errors.  */

void
breakpoint_auto_delete (bs)
     bpstat bs;
{
  for (; bs; bs = bs->next)
    if (bs->breakpoint_at && bs->breakpoint_at->enable == delete)
      delete_breakpoint (bs->breakpoint_at);
}

/* Delete a breakpoint and clean up all traces of it in the data structures. */

static void
delete_breakpoint (bpt)
     struct breakpoint *bpt;
{
  register struct breakpoint *b;
  register bpstat bs;

  if (bpt->inserted)
      target_remove_breakpoint(bpt->address, bpt->shadow_contents);

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
  if (bpt->cond_string != NULL)
    free (bpt->cond_string);
  if (bpt->addr_string != NULL)
    free (bpt->addr_string);

  if (xgdb_verbose && bpt->number >=0)
    printf ("breakpoint #%d deleted\n", bpt->number);

  /* Be sure no bpstat's are pointing at it after it's been freed.  */
  /* FIXME, how can we find all bpstat's?  We just check stop_bpstat for now. */
  for (bs = stop_bpstat; bs; bs = bs->next)
    if (bs->breakpoint_at == bpt)
      bs->breakpoint_at = NULL;
  free (bpt);
}

static void map_breakpoint_numbers ();

static void
delete_command (arg, from_tty)
     char *arg;
     int from_tty;
{

  if (arg == 0)
    {
      /* Ask user only if there are some breakpoints to delete.  */
      if (!from_tty
	  || (breakpoint_chain && query ("Delete all breakpoints? ", 0, 0)))
	{
	  /* No arg; clear all breakpoints.  */
	  while (breakpoint_chain)
	    delete_breakpoint (breakpoint_chain);
	}
    }
  else
    map_breakpoint_numbers (arg, delete_breakpoint);
}

static void
breakpoint_re_set_one (bint)
     int bint;
{
  struct breakpoint *b = (struct breakpoint *)bint;  /* get past catch_errs */
  int i;
  struct symtabs_and_lines sals;
  struct symtab_and_line sal;
  char *s;

  if (b->address != NULL && b->addr_string != NULL)
    {
      s = b->addr_string;
      sals = decode_line_1 (&s, 1, (struct symtab *)NULL, 0);
      for (i = 0; i < sals.nelts; i++)
	{
	  sal = sals.sals[i];
	  
	  b->symtab = sal.symtab;
	  b->line_number = sal.line;
	  if (sal.pc == 0 && sal.symtab != 0)
	    {
	      sal.pc = find_line_pc (sal.symtab, sal.line);
	      if (sal.pc == 0)
		error ("No line %d in file \"%s\".",
		       sal.line, sal.symtab->filename);
	    }
	  b->address = sal.pc;

	  if (b->cond_string != NULL)
	    {
	      s = b->cond_string;
	      b->cond = parse_c_1 (&s, block_for_pc (sal.pc), 0);
	    }
	  
	  check_duplicates (b->address);

	  mention (b);
	}
      free (sals.sals);
    }
  else
    {
      /* Anything without a string can't be re-set. */
      delete_breakpoint (b);
    }
}

/* Re-set all breakpoints after symbols have been re-loaded.  */
void
breakpoint_re_set ()
{
  struct breakpoint *b;
  
  ALL_BREAKPOINTS (b)
    {
      b->symtab = 0;		/* Be sure we don't point to old dead symtab */
      (void) catch_errors (breakpoint_re_set_one, (int) b, 
			   "Error in re-setting breakpoint");
    }

  /* Blank line to finish off all those mention() messages we just printed.  */
  printf_filtered ("\n");
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

/* Clear the ignore counts of all breakpoints.  */
void
breakpoint_clear_ignore_counts ()
{
  struct breakpoint *b;

  ALL_BREAKPOINTS (b)
    b->ignore_count = 0;
}

/* Command to set ignore-count of breakpoint N to COUNT.  */

static void
ignore_command (args, from_tty)
     char *args;
     int from_tty;
{
  char *p = args;
  register int num;

  if (p == 0)
    error_no_arg ("a breakpoint number");
  
  num = get_number (&p);

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
  char *p1;
  register int num;
  register struct breakpoint *b;

  if (p == 0)
    error_no_arg ("one or more breakpoint numbers");

  while (*p)
    {
      p1 = p;
      
      num = get_number (&p1);

      ALL_BREAKPOINTS (b)
	if (b->number == num)
	  {
	    function (b);
	    goto win;
	  }
      printf ("No breakpoint number %d.\n", num);
    win:
      p = p1;
    }
}

static void
enable_breakpoint (bpt)
     struct breakpoint *bpt;
{
  bpt->enable = enabled;

  if (xgdb_verbose && bpt->number >= 0)
    printf ("breakpoint #%d enabled\n", bpt->number);

  check_duplicates (bpt->address);
  if (bpt->val != NULL)
    {
      if (bpt->exp_valid_block != NULL
       && !contained_in (get_selected_block (), bpt->exp_valid_block))
	{
	  printf_filtered ("\
Cannot enable watchpoint %d because the block in which its expression\n\
is valid is not currently in scope.\n", bpt->number);
	  return;
	}

      value_free (bpt->val);

      bpt->val = evaluate_expression (bpt->exp);
      release_value (bpt->val);
    }
}

static void
enable_command (args, from_tty)
     char *args;
     int from_tty;
{
  struct breakpoint *bpt;
  if (args == 0)
    ALL_BREAKPOINTS (bpt)
      enable_breakpoint (bpt);
  else
    map_breakpoint_numbers (args, enable_breakpoint);
}

static void
disable_breakpoint (bpt)
     struct breakpoint *bpt;
{
  bpt->enable = disabled;

  if (xgdb_verbose && bpt->number >= 0)
    printf ("breakpoint #%d disabled\n", bpt->number);

  check_duplicates (bpt->address);
}

static void
disable_command (args, from_tty)
     char *args;
     int from_tty;
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
enable_once_command (args, from_tty)
     char *args;
     int from_tty;
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
enable_delete_command (args, from_tty)
     char *args;
     int from_tty;
{
  map_breakpoint_numbers (args, enable_delete_breakpoint);
}

/*
 * Use default_breakpoint_'s, or nothing if they aren't valid.
 */
struct symtabs_and_lines
decode_line_spec_1 (string, funfirstline)
     char *string;
     int funfirstline;
{
  struct symtabs_and_lines sals;
  if (string == 0)
    error ("Empty line specification.");
  if (default_breakpoint_valid)
    sals = decode_line_1 (&string, funfirstline,
			  default_breakpoint_symtab, default_breakpoint_line);
  else
    sals = decode_line_1 (&string, funfirstline, (struct symtab *)NULL, 0);
  if (*string)
    error ("Junk at end of line specification: %s", string);
  return sals;
}


/* Chain containing all defined enable commands.  */

extern struct cmd_list_element 
  *enablelist, *disablelist,
  *deletelist, *enablebreaklist;

extern struct cmd_list_element *cmdlist;

void
_initialize_breakpoint ()
{
  breakpoint_chain = 0;
  /* Don't bother to call set_breakpoint_count.  $bpnum isn't useful
     before a breakpoint is set.  */
  breakpoint_count = 0;

  add_com ("ignore", class_breakpoint, ignore_command,
	   "Set ignore-count of breakpoint number N to COUNT.");

  add_com ("commands", class_breakpoint, commands_command,
	   "Set commands to be executed when a breakpoint is hit.\n\
Give breakpoint number as argument after \"commands\".\n\
With no argument, the targeted breakpoint is the last one set.\n\
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
		  "Enable some breakpoints.\n\
Give breakpoint numbers (separated by spaces) as arguments.\n\
With no subcommand, breakpoints are enabled until you command otherwise.\n\
This is used to cancel the effect of the \"disable\" command.\n\
With a subcommand you can enable temporarily.",
		  &enablelist, "enable ", 1, &cmdlist);

  add_abbrev_prefix_cmd ("breakpoints", class_breakpoint, enable_command,
		  "Enable some breakpoints.\n\
Give breakpoint numbers (separated by spaces) as arguments.\n\
This is used to cancel the effect of the \"disable\" command.\n\
May be abbreviated to simply \"enable\".\n",
		  &enablebreaklist, "enable breakpoints ", 1, &enablelist);

  add_cmd ("once", no_class, enable_once_command,
	   "Enable breakpoints for one hit.  Give breakpoint numbers.\n\
If a breakpoint is hit while enabled in this fashion, it becomes disabled.\n\
See the \"tbreak\" command which sets a breakpoint and enables it once.",
	   &enablebreaklist);

  add_cmd ("delete", no_class, enable_delete_command,
	   "Enable breakpoints and delete when hit.  Give breakpoint numbers.\n\
If a breakpoint is hit while enabled in this fashion, it is deleted.",
	   &enablebreaklist);

  add_cmd ("delete", no_class, enable_delete_command,
	   "Enable breakpoints and delete when hit.  Give breakpoint numbers.\n\
If a breakpoint is hit while enabled in this fashion, it is deleted.",
	   &enablelist);

  add_cmd ("once", no_class, enable_once_command,
	   "Enable breakpoints for one hit.  Give breakpoint numbers.\n\
If a breakpoint is hit while enabled in this fashion, it becomes disabled.\n\
See the \"tbreak\" command which sets a breakpoint and enables it once.",
	   &enablelist);

  add_prefix_cmd ("disable", class_breakpoint, disable_command,
	   "Disable some breakpoints.\n\
Arguments are breakpoint numbers with spaces in between.\n\
To disable all breakpoints, give no argument.\n\
A disabled breakpoint is not forgotten, but has no effect until reenabled.",
		  &disablelist, "disable ", 1, &cmdlist);
  add_com_alias ("dis", "disable", class_breakpoint, 1);
  add_com_alias ("disa", "disable", class_breakpoint, 1);

  add_cmd ("breakpoints", class_alias, disable_command,
	   "Disable some breakpoints.\n\
Arguments are breakpoint numbers with spaces in between.\n\
To disable all breakpoints, give no argument.\n\
A disabled breakpoint is not forgotten, but has no effect until reenabled.\n\
This command may be abbreviated \"disable\".",
	   &disablelist);

  add_prefix_cmd ("delete", class_breakpoint, delete_command,
	   "Delete some breakpoints or auto-display expressions.\n\
Arguments are breakpoint numbers with spaces in between.\n\
To delete all breakpoints, give no argument.\n\
\n\
Also a prefix command for deletion of other GDB objects.\n\
The \"unset\" command is also an alias for \"delete\".",
		  &deletelist, "delete ", 1, &cmdlist);
  add_com_alias ("d", "delete", class_breakpoint, 1);

  add_cmd ("breakpoints", class_alias, delete_command,
	   "Delete some breakpoints or auto-display expressions.\n\
Arguments are breakpoint numbers with spaces in between.\n\
To delete all breakpoints, give no argument.\n\
This command may be abbreviated \"delete\".",
	   &deletelist);

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
are set to the address of the last breakpoint listed.\n\n\
Convenience variable \"$bpnum\" contains the number of the last\n\
breakpoint set.");

  add_com ("catch", class_breakpoint, catch_command,
         "Set breakpoints to catch exceptions that are raised.\n\
Argument may be a single exception to catch, multiple exceptions\n\
to catch, or the default exception \"default\".  If no arguments\n\
are given, breakpoints are set at all exception handlers catch clauses\n\
within the current scope.\n\
\n\
A condition specified for the catch applies to all breakpoints set\n\
with this command\n\
\n\
Do \"help breakpoints\" for info on other commands dealing with breakpoints.");

  add_com ("watch", class_breakpoint, watch_command,
	   "Set a watchpoint for an expression.\n\
A watchpoint stops execution of your program whenever the value of\n\
an expression changes.");

  add_info ("watchpoints", watchpoints_info,
	    "Status of all watchpoints, or watchpoint number NUMBER.\n\
Second column is \"y\" for enabled watchpoints, \"n\" for disabled.");
}
