/* Manages interpreters for gdb.
   Copyright 2000, 2002 Free Software Foundation, Inc.
   Written by Jim Ingham <jingham@apple.com> of Apple Computer, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA. */

/* This is just a first cut at separating out the "interpreter" functions
   of gdb into self-contained modules.  There are a couple of open areas that
   need to be sorted out:

   1) The interpreter explicitly contains a UI_OUT, and can insert itself
   into the event loop, but it doesn't explicitly contain hooks for readline.
   I did this because it seems to me many interpreters won't want to use
   the readline command interface, and it is probably simpler to just let
   them take over the input in their resume proc.  
*/

#include "defs.h"
#include "gdbcmd.h"
#include "ui-out.h"
#include "event-loop.h"
#include "event-top.h"
#include "interps.h"
#include "completer.h"
#include "gdb_string.h"
#include "gdb-events.h"

struct gdb_interpreter
{
  char *name;			/* This is the name in "-i=" and set interpreter. */
  struct gdb_interpreter *next;	/* Interpreters are stored in a linked list, 
				   this is the next one... */
  void *data;			/* This is a cookie that the instance of the 
				   interpreter can use, for instance to call 
				   itself in hook functions */
  int inited;			/* Has the init_proc been run? */
  struct ui_out *interpreter_out;	/* This is the ui_out used to collect 
					   results for this interpreter.  It can 
					   be a formatter for stdout, as is the 
					   case for the console & mi outputs, or it 
					   might be a result formatter. */
  struct gdb_interpreter_procs procs;
  int quiet_p;
};

/* Functions local to this file. */
static void initialize_interps (void);

static void set_interpreter_cmd (char *args, int from_tty,
				 struct cmd_list_element *c);
static void list_interpreter_cmd (char *args, int from_tty);
static void do_set_interpreter (int not_an_fd);
static char **interpreter_completer (char *text, char *word);

/* The magic initialization routine for this module. */

void _initialize_interpreter (void);

/* Variables local to this file: */

static struct gdb_interpreter *interp_list = NULL;
static struct gdb_interpreter *current_interpreter = NULL;

static int interpreter_initialized = 0;

/* gdb_new_interpreter - This allocates space for a new interpreter,
   fills the fields from the inputs, and returns a pointer to the
   interpreter. */

struct gdb_interpreter *
gdb_new_interpreter (char *name,
		     void *data,
		     struct ui_out *uiout,
		     struct gdb_interpreter_procs *procs)
{
  struct gdb_interpreter *new_interp;

  new_interp =
    (struct gdb_interpreter *) xmalloc (sizeof (struct gdb_interpreter));

  new_interp->name = xstrdup (name);
  new_interp->data = data;
  new_interp->interpreter_out = uiout;
  new_interp->quiet_p = 0;
  new_interp->procs.init_proc = procs->init_proc;
  new_interp->procs.resume_proc = procs->resume_proc;
  new_interp->procs.suspend_proc = procs->suspend_proc;
  new_interp->procs.delete_proc = procs->delete_proc;
  new_interp->procs.exec_proc = procs->exec_proc;
  new_interp->procs.prompt_proc = procs->prompt_proc;
  new_interp->inited = 0;

  return new_interp;
}

/* Add interpreter INTERP to the gdb interpreter list.  If an
   interpreter of the same name is already on the list, then
   the new one is NOT added, and the function returns 0.  Otherwise
   it returns 1. */

int
gdb_add_interpreter (struct gdb_interpreter *interp)
{
  if (!interpreter_initialized)
    initialize_interps ();

  if (gdb_lookup_interpreter (interp->name) != NULL)
    return 0;

  interp->next = interp_list;
  interp_list = interp;

  return 1;
}

/* Looks for the interpreter INTERP in the interpreter list.  If it exists,
   runs the delete_proc, and if this is successful, the INTERP is deleted from
   the interpreter list and the function returns 1.  If the delete_proc fails, the
   function returns -1 and the interpreter is NOT removed from the list.  If the
   interp is not found, 0 is returned.

   This isn't currently used by anything. */

int
gdb_delete_interpreter (struct gdb_interpreter *interp)
{
  struct gdb_interpreter *cur_ptr, *prev_ptr;

  if (!interpreter_initialized)
    {
      ui_out_message (uiout, 0,
		      "You can't delete an interp before you have added one!");
      return -1;
    }

  if (interp_list == NULL)
    {
      ui_out_message (uiout, 0, "No interpreters to delete.");
      return -1;
    }

  if (interp_list->next == NULL)
    {
      ui_out_message (uiout, 0, "You can't delete gdb's only intepreter.");
      return -1;
    }

  for (cur_ptr = interp_list, prev_ptr = NULL;
       cur_ptr != NULL; prev_ptr = cur_ptr, cur_ptr = cur_ptr->next)
    {
      if (cur_ptr == interp)
	{
	  /* Can't currently delete the console interpreter... */
	  if (strcmp (interp->name, "console") == 0)
	    {
	      ui_out_message (uiout, 0,
			      "You can't delete the console interpreter.");
	      return -1;
	    }

	  /* If the interpreter is the current interpreter, switch
	     back to the console interpreter */

	  if (interp == current_interpreter)
	    {
	      gdb_set_interpreter (gdb_lookup_interpreter ("console"));
	    }

	  /* Don't delete the interpreter if its delete proc fails */

	  if ((interp->procs.delete_proc != NULL)
	      && (!interp->procs.delete_proc (interp->data)))
	    return -1;

	  if (cur_ptr == interp_list)
	    interp_list = cur_ptr->next;
	  else
	    prev_ptr->next = cur_ptr->next;

	  break;
	}
    }

  if (cur_ptr == NULL)
    return 0;
  else
    return 1;
}

/* This sets the current interpreter to be INTERP.  If INTERP has not
   been initialized, then this will also run the init proc.  If the
   init proc is successful, return 1, if it fails, set the old
   interpreter back in place and return 0.  If we can't restore the
   old interpreter, then raise an internal error, since we are in
   pretty bad shape at this point. */

int
gdb_set_interpreter (struct gdb_interpreter *interp)
{
  struct gdb_interpreter *old_interp = current_interpreter;
  int first_time = 0;


  char buffer[64];

  if (current_interpreter != NULL)
    {
      do_all_continuations ();
      ui_out_flush (uiout);
      if (current_interpreter->procs.suspend_proc
	  && !current_interpreter->procs.suspend_proc (current_interpreter->
						       data))
	{
	  error ("Could not suspend interpreter \"%s\"\n",
		 current_interpreter->name);
	}
    }
  else
    {
      first_time = 1;
    }

  current_interpreter = interp;

  /* We use interpreter_p for the "set interpreter" variable, so we need
     to make sure we have a malloc'ed copy for the set command to free. */
  if (interpreter_p != NULL
      && strcmp (current_interpreter->name, interpreter_p) != 0)
    {
      xfree (interpreter_p);

      interpreter_p = xstrdup (current_interpreter->name);
    }

  uiout = interp->interpreter_out;

  /* Run the init proc.  If it fails, try to restore the old interp. */

  if (!interp->inited)
    {
      if (interp->procs.init_proc != NULL)
	{
	  if (!interp->procs.init_proc (interp->data))
	    {
	      if (!gdb_set_interpreter (old_interp))
		internal_error (__FILE__, __LINE__,
				"Failed to initialize new interp \"%s\" %s",
				interp->name,
				"and could not restore old interp!\n");
	      return 0;
	    }
	  else
	    {
	      interp->inited = 1;
	    }
	}
      else
	{
	  interp->inited = 1;
	}
    }

  /* Clear out any installed interpreter hooks/event handlers. */
  clear_interpreter_hooks ();

  if (interp->procs.resume_proc != NULL
      && (!interp->procs.resume_proc (interp->data)))
    {
      if (!gdb_set_interpreter (old_interp))
	internal_error (__FILE__, __LINE__,
			"Failed to initialize new interp \"%s\" %s",
			interp->name, "and could not restore old interp!\n");
      return 0;
    }

  /* Finally, put up the new prompt to show that we are indeed here. 
     Also, display_gdb_prompt for the console does some readline magic
     which is needed for the console interpreter, at least... */

  if (!first_time)
    {
      if (!gdb_interpreter_is_quiet (interp))
	{
	  sprintf (buffer, "Switching to interpreter \"%.24s\".\n",
		   interp->name);
	  ui_out_text (uiout, buffer);
	}
      display_gdb_prompt (NULL);
    }

  return 1;
}

/*
 * gdb_lookup_interpreter - Looks up the interpreter for NAME.  If
 * no such interpreter exists, return NULL, otherwise return a pointer
 * to the interpreter. 
 */

struct gdb_interpreter *
gdb_lookup_interpreter (char *name)
{
  struct gdb_interpreter *interp;

  if (name == NULL || strlen (name) == 0)
    return NULL;

  for (interp = interp_list; interp != NULL; interp = interp->next)
    {
      if (strcmp (interp->name, name) == 0)
	return interp;
    }

  return NULL;
}

/* Returns the current interpreter. */

struct gdb_interpreter *
gdb_current_interpreter ()
{
  return current_interpreter;
}

struct ui_out *
gdb_interpreter_ui_out (struct gdb_interpreter *interp)
{
  if (interp != NULL)
    return interp->interpreter_out;

  return current_interpreter->interpreter_out;
}

/* Returns true if the current interp is the passed in name. */
int
gdb_current_interpreter_is_named (char *interp_name)
{
  struct gdb_interpreter *current_interp = gdb_current_interpreter ();

  if (current_interp)
    return (strcmp (current_interp->name, interp_name) == 0);

  return 0;
}

/* This is called in display_gdb_prompt.
   If the current interpreter defines a prompt_proc, then that proc is 
   run.  If the proc returns a non-zero value, display_gdb_prompt will
   return without itself displaying the prompt. */

int
gdb_interpreter_display_prompt (char *new_prompt)
{
  if (current_interpreter->procs.prompt_proc == NULL)
    return 0;
  else
    return current_interpreter->procs.prompt_proc (current_interpreter->data,
						   new_prompt);
}

int
gdb_interpreter_is_quiet (struct gdb_interpreter *interp)
{
  if (interp != NULL)
    return interp->quiet_p;
  else
    return current_interpreter->quiet_p;
}

int
gdb_interpreter_set_quiet (struct gdb_interpreter *interp, int quiet)
{
  int old_val = interp->quiet_p;
  interp->quiet_p = quiet;
  return old_val;
}

/* gdb_interpreter_exec - This executes COMMAND_STR in the current 
   interpreter. */

int
gdb_interpreter_exec (char *command_str)
{
  if (current_interpreter->procs.exec_proc != NULL)
    {
      return current_interpreter->procs.exec_proc (current_interpreter->data,
						   command_str);
    }

  return 0;
}

struct gdb_interpreter_procs *
gdb_interpreter_get_procs (struct gdb_interpreter *interp)
{
  if (interp != NULL)
    return &interp->procs;

  return &current_interpreter->procs;
}

void *
gdb_interpreter_get_data (struct gdb_interpreter *interp)
{
  if (interp != NULL)
    return interp->data;

  return current_interpreter->data;
}

/* A convenience routine that nulls out all the
   common command hooks.  Use it when removing your interpreter in its 
   suspend proc. */

void
clear_interpreter_hooks ()
{
  init_ui_hook = 0;
  print_frame_info_listing_hook = 0;
  /*print_frame_more_info_hook = 0; */
  query_hook = 0;
  warning_hook = 0;
  create_breakpoint_hook = 0;
  delete_breakpoint_hook = 0;
  modify_breakpoint_hook = 0;
  interactive_hook = 0;
  registers_changed_hook = 0;
  readline_begin_hook = 0;
  readline_hook = 0;
  readline_end_hook = 0;
  register_changed_hook = 0;
  memory_changed_hook = 0;
  context_hook = 0;
  target_wait_hook = 0;
  call_command_hook = 0;
  error_hook = 0;
  error_begin_hook = 0;
  command_loop_hook = 0;
  clear_gdb_event_hooks ();
}

/* This is a lazy init routine, called the first time
   the interpreter module is used.  I put it here just in case, but I haven't
   thought of a use for it yet.  I will probably bag it soon, since I don't
   think it will be necessary. */

static void
initialize_interps (void)
{
  interpreter_initialized = 1;
  /* Don't know if anything needs to be done here... */
}

/* set_interpreter_cmd - This implements "set interpreter foo". */

static void
set_interpreter_cmd (char *args, int from_tty, struct cmd_list_element *c)
{
  struct gdb_interpreter *interp_ptr;

  dont_repeat ();

  if (cmd_type (c) != set_cmd)
    return;

  interp_ptr = gdb_lookup_interpreter (interpreter_p);
  if (interp_ptr != NULL)
    {
      if (!gdb_set_interpreter (interp_ptr))
	error ("\nCould not switch to interpreter \"%s\", %s%s\".\n",
	       interp_ptr->name, "reverting to interpreter \"",
	       current_interpreter->name);
    }
  else
    {
      char *bad_name = interpreter_p;
      interpreter_p = xstrdup (current_interpreter->name);
      error ("Could not find interpreter \"%s\".", bad_name);
    }
}

void
interpreter_exec_cmd (char *args, int from_tty)
{
  struct gdb_interpreter *old_interp, *interp_to_use;
  char **prules = NULL;
  char **trule = NULL;
  unsigned int nrules;
  unsigned int i;
  int old_quiet, use_quiet;

  prules = buildargv (args);
  if (prules == NULL)
    {
      error ("unable to parse arguments");
    }

  nrules = 0;
  if (prules != NULL)
    {
      for (trule = prules; *trule != NULL; trule++)
	{
	  nrules++;
	}
    }

  if (nrules < 2)
    error ("usage: interpreter-exec <interpreter> [ <command> ... ]");

  old_interp = gdb_current_interpreter ();

  interp_to_use = gdb_lookup_interpreter (prules[0]);
  if (interp_to_use == NULL)
    error ("Could not find interpreter \"%s\".", prules[0]);

  /* Temporarily set interpreters quiet */
  old_quiet = gdb_interpreter_set_quiet (old_interp, 1);
  use_quiet = gdb_interpreter_set_quiet (interp_to_use, 1);

  if (!gdb_set_interpreter (interp_to_use))
    error ("Could not switch to interpreter \"%s\".", prules[0]);

  for (i = 1; i < nrules; i++)
    {
      if (!gdb_interpreter_exec (prules[i]))
	{
	  gdb_set_interpreter (old_interp);
	  gdb_interpreter_set_quiet (interp_to_use, old_quiet);
	  error ("error in command: \"%s\".", prules[i]);
	  break;
	}
    }

  gdb_set_interpreter (old_interp);
  gdb_interpreter_set_quiet (interp_to_use, use_quiet);
  gdb_interpreter_set_quiet (old_interp, old_quiet);
}

/* List the possible interpreters which could complete the given text. */

static char **
interpreter_completer (char *text, char *word)
{
  int alloced, textlen;
  int num_matches;
  char **matches;
  struct gdb_interpreter *interp;

  /* We expect only a very limited number of interpreters, so just
     allocate room for all of them. */
  for (interp = interp_list; interp != NULL; interp = interp->next)
    ++alloced;
  matches = (char **) xmalloc (alloced * sizeof (char *));

  num_matches = 0;
  textlen = strlen (text);
  for (interp = interp_list; interp != NULL; interp = interp->next)
    {
      if (strncmp (interp->name, text, textlen) == 0)
	{
	  matches[num_matches] =
	    (char *) xmalloc (strlen (word) + strlen (interp->name) + 1);
	  if (word == text)
	    strcpy (matches[num_matches], interp->name);
	  else if (word > text)
	    {
	      /* Return some portion of interp->name */
	      strcpy (matches[num_matches], interp->name + (word - text));
	    }
	  else
	    {
	      /* Return some of text plus interp->name */
	      strncpy (matches[num_matches], word, text - word);
	      matches[num_matches][text - word] = '\0';
	      strcat (matches[num_matches], interp->name);
	    }
	  ++num_matches;
	}
    }

  if (num_matches == 0)
    {
      xfree (matches);
      matches = NULL;
    }
  else if (num_matches < alloced)
    {
      matches = (char **) xrealloc ((char *) matches, ((num_matches + 1)
						       * sizeof (char *)));
      matches[num_matches] = NULL;
    }

  return matches;
}

/* This just adds the "set interpreter" and "info interpreters" commands. */

void
_initialize_interpreter (void)
{
  struct cmd_list_element *c;

  c = add_cmd ("interpreter-exec", class_support,
	       interpreter_exec_cmd,
	       "Execute a command in an interpreter.  It takes two arguments:\n\
The first argument is the name of the interpreter to use.\n\
The second argument is the command to execute.\n", &cmdlist);
  set_cmd_completer (c, interpreter_completer);
}
