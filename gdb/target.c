/* Select target systems and architectures at runtime for GDB.
   Copyright (C) 1990 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

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
#include <errno.h>
#include <ctype.h>
#include "defs.h"
#include "target.h"
#include "gdbcmd.h"
#include "symtab.h"
#include "inferior.h"
#include "bfd.h"
#include "symfile.h"

extern int memory_insert_breakpoint(), memory_remove_breakpoint();
extern void host_convert_to_virtual(), host_convert_from_virtual();

static void cleanup_target ();

/* Pointer to array of target architecture structures; the size of the
   array; the current index into the array; the allocated size of the 
   array.  */
struct target_ops **target_structs;
unsigned target_struct_size;
unsigned target_struct_index;
unsigned target_struct_allocsize;
#define	DEFAULT_ALLOCSIZE	10

/* The initial current target, so that there is always a semi-valid
   current target.  */

struct target_ops dummy_target = {"None", "None",
    0, 0, 0, 0,		/* open, close, attach, detach */
    0, 0,		/* resume, wait */
    0, 0, 0, 0, 0,	/* registers */
    0, 0, 		/* memory */
    0, 0, 		/* bkpts */
    0, 0, 0, 0, 0, 	/* terminal */
    0, 0, 0,		/* kill, load, add_syms */
    0, 0, 		/* call_function, lookup_symbol */
    0, 0,		/* create_inferior, mourn_inferior */
    dummy_stratum, 0,	/* stratum, next */
    0, 0, 0, 0, 0,	/* all mem, mem, stack, regs, exec */
    OPS_MAGIC,
};

/* The target structure we are currently using to talk to a process
   or file or whatever "inferior" we have.  */

struct target_ops *current_target;

/* The stack of target structures that have been pushed.  */

struct target_ops **current_target_stack;


/* Add a possible target architecture to the list.  */

void
add_target (t)
     struct target_ops *t;
{
  if (t->to_magic != OPS_MAGIC)
    {
      fprintf(stderr, "Magic number of %s target struct wrong\n", 
	t->to_shortname);
      abort();
    }

  if (!target_structs)
    {
      target_struct_allocsize = DEFAULT_ALLOCSIZE;
      target_structs = (struct target_ops **) xmalloc
	(target_struct_allocsize * sizeof (*target_structs));
    }
  if (target_struct_size >= target_struct_allocsize)
    {
      target_struct_allocsize *= 2;
      target_structs = (struct target_ops **) xrealloc (target_structs, 
	target_struct_allocsize * sizeof (*target_structs));
    }
  target_structs[target_struct_size++] = t;
  cleanup_target (t);
}

/* Stub functions */

static void
ignore ()
{
}

/* ARGSUSED */
static int
nomemory (memaddr, myaddr, len, write)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
{
  return 0;		/* No bytes handled */
}

static void
tcomplain ()
{
  error ("You can't do that when your target is `%s'",
	 current_target->to_shortname);
}

static int
noprocess ()
{
  error ("You can't do that without a process to debug");
}

static int
nosymbol (name, addrp)
     char *name;
     CORE_ADDR *addrp;
{
  return 1;		/* Symbol does not exist in target env */
}

static void
default_terminal_info (args, from_tty)
     char *args;
     int from_tty;
{
  printf("No saved terminal information.\n");
}

#if 0
/* With strata, this function is no longer needed.  FIXME.  */
/* This is the default target_create_inferior function.  It looks up
   the stack for some target that cares to create inferiors, then
   calls it -- or complains if not found.  */

static void
upstack_create_inferior (exec, args, env)
     char *exec;
     char *args;
     char **env;
{
  struct target_ops *t;

  for (t = current_target;
       t;
       t = t->to_next)
    {
      if (t->to_create_inferior != upstack_create_inferior)
	{
          t->to_create_inferior (exec, args, env);
	  return;
	}

    }
  tcomplain();
}
#endif

/* This is the default target_create_inferior and target_attach function.
   If the current target is executing, it asks whether to kill it off.
   If this function returns without calling error(), it has killed off
   the target, and the operation should be attempted.  */

static void
kill_or_be_killed (from_tty)
     int from_tty;
{
  struct target_ops *savecur;

  if (target_has_execution)
    {
      printf ("You are already running a program:\n");
      target_files_info ();
      if (query ("Kill it? ")) {
	savecur = current_target;
	target_kill (0, from_tty);
	if (target_has_execution)
	  error ("Killing the program did not help.");
	return;
      } else {
	error ("Program not killed.");
      }
    }
  tcomplain();
}

static void
maybe_kill_then_attach (args, from_tty)
     char *args;
     int from_tty;
{
  kill_or_be_killed (from_tty);
  target_attach (args, from_tty);
}

static void
maybe_kill_then_create_inferior (exec, args, env)
     char *exec;
     char *args;
     char **env;
{
  kill_or_be_killed (0);
  target_create_inferior (exec, args, env);
}

/* Clean up a target struct so it no longer has any zero pointers in it.
   We default entries, at least to stubs that print error messages.  */

static void
cleanup_target (t)
     struct target_ops *t;
{

  /* Check magic number.  If wrong, it probably means someone changed
     the struct definition, but not all the places that initialize one.  */
  if (t->to_magic != OPS_MAGIC)
    {
      fprintf(stderr, "Magic number of %s target struct wrong\n", 
	t->to_shortname);
      abort();
    }

#define de_fault(field, value) \
  if (!t->field)	t->field = value

  /*        FIELD			DEFAULT VALUE        */

  de_fault (to_open, 			tcomplain);
  de_fault (to_close, 			(void (*)())ignore);
  de_fault (to_attach, 			maybe_kill_then_attach);
  de_fault (to_detach, 			(void (*)())ignore);
  de_fault (to_resume, 			(void (*)())noprocess);
  de_fault (to_wait, 			noprocess);
  de_fault (to_fetch_registers, 	noprocess);
  de_fault (to_store_registers,		noprocess);
  de_fault (to_prepare_to_store,	(void (*)())noprocess);
  de_fault (to_convert_to_virtual,	host_convert_to_virtual);
  de_fault (to_convert_from_virtual,	host_convert_from_virtual);
  de_fault (to_xfer_memory,		nomemory);
  de_fault (to_files_info,		ignore);
  de_fault (to_insert_breakpoint,	memory_insert_breakpoint);
  de_fault (to_remove_breakpoint,	memory_remove_breakpoint);
  de_fault (to_terminal_init,		ignore);
  de_fault (to_terminal_inferior,	ignore);
  de_fault (to_terminal_ours_for_output,ignore);
  de_fault (to_terminal_ours,		ignore);
  de_fault (to_terminal_info,		default_terminal_info);
  de_fault (to_kill,			(void (*)())noprocess);
  de_fault (to_load,			tcomplain);
  de_fault (to_add_syms,		tcomplain);
  de_fault (to_call_function,		(struct value *(*)())noprocess);
  de_fault (to_lookup_symbol,		nosymbol);
  de_fault (to_create_inferior,		maybe_kill_then_create_inferior);
  de_fault (to_mourn_inferior,		(void (*)())noprocess);
  de_fault (to_next,			0);
  de_fault (to_has_all_memory,		0);
  de_fault (to_has_memory,		0);
  de_fault (to_has_stack,		0);
  de_fault (to_has_registers,		0);
  de_fault (to_has_execution,		0);

#undef de_fault
}

/* Push a new target type into the stack of the existing target accessors,
   possibly superseding some of the existing accessors.

   Result is zero if the pushed target ended up on top of the stack,
   nonzero if at least one target is on top of it.

   Rather than allow an empty stack, we always have the dummy target at
   the bottom stratum, so we can call the function vectors without
   checking them.  */

int
push_target (t)
     struct target_ops *t;
{
  struct target_ops *st, *prev;

  for (prev = 0, st = current_target;
       st;
       prev = st, st = st->to_next) {
    if ((int)(t->to_stratum) >= (int)(st->to_stratum))
      break;
  }

  while (t->to_stratum == st->to_stratum) {
    /* There's already something on this stratum.  Close it off.  */
    (st->to_close) (0);
    if (prev)
      prev->to_next = st->to_next;	/* Unchain old target_ops */
    else
      current_target = st->to_next;	/* Unchain first on list */
    st = st->to_next;
  }

  /* We have removed all targets in our stratum, now add ourself.  */
  t->to_next = st;
  if (prev)
    prev->to_next = t;
  else
    current_target = t;

  cleanup_target (current_target);
  return prev != 0;
}

/* Remove a target_ops vector from the stack, wherever it may be. 
   Return how many times it was removed (0 or 1 unless bug).  */

int
unpush_target (t)
     struct target_ops *t;
{
  struct target_ops *u, *v;
  int result = 0;

  for (u = current_target, v = 0;
       u;
       v = u, u = u->to_next)
    if (u == t)
      {
	if (v == 0)
	  pop_target();			/* unchain top copy */
	else {
	  (t->to_close)(0);		/* Let it clean up */
	  v->to_next = t->to_next;	/* unchain middle copy */
	}
	result++;
      }
  return result;
}

void
pop_target ()
{
  (current_target->to_close)(0);	/* Let it clean up */
  current_target = current_target->to_next;
  if (!current_target)		/* At bottom, push dummy.  */
    push_target (&dummy_target);
}

/* Print things about the whole set of targets and about the
   current target stack.  */
static void
targets_info ()
{
  int i;

  printf("Possible targets:\n\n");
  for (i = 0; i < target_struct_size; i++)
    printf ("%-15s %s\n",
      target_structs[i]->to_shortname, 
      target_structs[i]->to_longname);
}

/* Move memory to or from the targets.  Iterate until all of it has
   been moved, if necessary.  The top target gets priority; anything
   it doesn't want, is offered to the next one down, etc.  Note the
   business with curlen:  if an early target says "no, but I have a
   boundary overlapping this xfer" then we shorten what we offer to
   the subsequent targets so the early guy will get a chance at the
   tail before the subsequent ones do. 

   Result is 0 or errno value.  */

int
target_read_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  return target_xfer_memory (memaddr, myaddr, len, 0);
}

int
target_write_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  return target_xfer_memory (memaddr, myaddr, len, 1);
}
 
int
target_xfer_memory (memaddr, myaddr, len, write)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
{
  int curlen;
  int res;
  struct target_ops *t;
  
  /* The quick case is that the top target does it all.  */
  res = current_target->to_xfer_memory(memaddr, myaddr, len, write);
  if (res == len)
    return 0;

  if (res > 0)
    goto bump;
  /* If res <= 0 then we call it again in the loop.  Ah well.  */

  for (; len > 0;)
    {
      curlen = len;		/* Want to do it all */
      for (t = current_target;
	   t;
	   t = t->to_has_all_memory? 0: t->to_next)
	{
	  res = t->to_xfer_memory(memaddr, myaddr, curlen, write);
	  if (res > 0) break;	/* Handled all or part of xfer */
	  if (res == 0) continue;	/* Handled none */
	  curlen = -res;	/* Could handle once we get past res bytes */
	}
      if (res <= 0)
	{
	  /* If this address is for nonexistent memory,
	     read zeros if reading, or do nothing if writing.  Return error. */
	  if (!write)
	    bzero (myaddr, len);
	  return EIO;
	}
bump:
      memaddr += res;
      myaddr  += res;
      len     -= res;
    }
  return 0;			/* We managed to cover it all somehow. */
}


static void
target_info (args, from_tty)
     char *args;
     int from_tty;
{
  struct target_ops *t;
  int has_all_mem = 0;
  
  if (symfile != 0)
    printf ("Symbols from \"%s\".\n", symfile);

#ifdef FILES_INFO_HOOK
  if (FILES_INFO_HOOK ())
    return;
#endif

  for (t = current_target;
       t;
       t = t->to_next)
    {
      if ((int)(t->to_stratum) <= (int)dummy_stratum)
	continue;
      if (has_all_mem)
	printf("\tWhile running this, gdb does not access memory from...\n");
      printf("%s:\n", t->to_longname);
      (t->to_files_info)();
      has_all_mem = t->to_has_all_memory;
    }
}

/* The target command selects a target and calls its open routine.
   The open routine takes the rest of the parameters from the command,
   and (if successful) pushes a new target onto the stack.  */

static void
target_command (args, from_tty)
     char *args;
     int from_tty;
{
  int i;
  char *rest;

  dont_repeat();

  if (!args)
    error (
     "Argument required (target name).  `info targets' lists possible targets");

  if (target_has_execution)
    {   
      if (query ("A program is being debugged already.  Kill it? "))
        target_kill ((char *)0, from_tty);
      else
        error ("Program not killed.");
    }
        
  /* Skip to first space, or end of args */
  for (rest = args; *rest && !isspace(*rest); rest++) ;
  if (*rest == '\0')
    rest = 0;		/* Only one word in args */
  else
    {
      *rest = '\0';	/* Terminate first word, scan for next */
      for (rest++; isspace (*rest); rest++) ;
      if (*rest == '\0')	/* Only one word w/trailing blanks */
	rest = 0;
    }

  /* Search target list for a match */

  for (i = 0; i < target_struct_size; i++)
    {
      if (!strcmp (args, target_structs[i]->to_shortname))
	goto gotit;
    }
  error ("No such target.  `info targets' will list all targets");

gotit:
  (*target_structs[i]->to_open) (rest, from_tty);
}

static char targ_desc[] = 
    "Names of targets and files being debugged.\n\
Shows the entire stack of targets currently in use (including the exec-file,\n\
core-file, and process, if any), as well as the symbol file name.";

void
_initialize_targets ()
{
  current_target = &dummy_target;
  cleanup_target (current_target);

  add_info ("targets", targets_info,
    "Names of all possible targets.\n\
A target is typically a protocol for talking to debugging facilities;\n\
for example, `child' for Unix child processes, or `vxworks' for a\n\
TCP/IP link to a VxWorks system.");

  add_info ("target", target_info, targ_desc);
  add_info ("files", target_info, targ_desc);

  add_com ("target", class_run, target_command,
"Connect to a target machine or process.\n\
The first argument is the type or protocol of the target machine.  Remaining\n\
arguments are interpreted by the target protocol, but typically include\n\
things like device names or host names to connect with, process numbers,\n\
baud rates, etc.  You can list all possible targets with the `info targets'\n\
command.");
}
