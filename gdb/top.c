/* Top level stuff for GDB, the GNU debugger.
   Copyright 1986, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 1998
   Free Software Foundation, Inc.

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
#include "gdbcmd.h"
#include "call-cmds.h"
#include "symtab.h"
#include "inferior.h"
#include "signals.h"
#include "target.h"
#include "breakpoint.h"
#include "gdbtypes.h"
#include "expression.h"
#include "value.h"
#include "language.h"
#include "terminal.h" /* For job_control.  */
#include "annotate.h"
#include "top.h"
#include "event-loop.h"

/* readline include files */
#include <readline/readline.h>
#include <readline/history.h>

/* readline defines this.  */
#undef savestring

#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "gdb_string.h"
#include "gdb_stat.h"
#include <ctype.h>

/* Prototypes for local functions */

static void dont_repeat_command PARAMS ((char *, int));

static void source_cleanup_lines PARAMS ((PTR));

static void user_defined_command PARAMS ((char *, int));

static void init_signals PARAMS ((void));

#ifdef STOP_SIGNAL
static void stop_sig PARAMS ((int));
#endif

static char * line_completion_function PARAMS ((char *, int, char *, int));

static char * readline_line_completion_function PARAMS ((char *, int));

/* NOTE 1999-04-29: this function will be static again, after we make the
   event loop be the default command loop for gdb, and we merge
   event-top.c into this file, top.c */
/* static */ void command_loop_marker PARAMS ((int));

static void while_command PARAMS ((char *, int));

static void if_command PARAMS ((char *, int));

static struct command_line *
build_command_line PARAMS ((enum command_control_type, char *));

static struct command_line *
get_command_line PARAMS ((enum command_control_type, char *));

static void realloc_body_list PARAMS ((struct command_line *, int));

static enum misc_command_type read_next_line PARAMS ((struct command_line **));

static enum command_control_type
recurse_read_control_structure PARAMS ((struct command_line *));

static struct cleanup * setup_user_args PARAMS ((char *));

static char * locate_arg PARAMS ((char *));

static char * insert_args PARAMS ((char *));

static void arg_cleanup PARAMS ((void));

static void init_main PARAMS ((void));

static void init_cmd_lists PARAMS ((void));

static void float_handler PARAMS ((int));

static void init_signals PARAMS ((void));

static void set_verbose PARAMS ((char *, int, struct cmd_list_element *));

static void show_history PARAMS ((char *, int));

static void set_history PARAMS ((char *, int));

static void set_history_size_command PARAMS ((char *, int,
					      struct cmd_list_element *));

static void show_commands PARAMS ((char *, int));

static void echo_command PARAMS ((char *, int));

static void pwd_command PARAMS ((char *, int));

static void show_version PARAMS ((char *, int));

static void document_command PARAMS ((char *, int));

static void define_command PARAMS ((char *, int));

static void validate_comname PARAMS ((char *));

static void help_command PARAMS ((char *, int));

static void show_command PARAMS ((char *, int));

static void info_command PARAMS ((char *, int));

static void complete_command PARAMS ((char *, int));

static void do_nothing PARAMS ((int));

#ifdef SIGHUP
/* NOTE 1999-04-29: This function will be static again, once we modify
   gdb to use the event loop as the default command loop and we merge
   event-top.c into this file, top.c */
/* static */ int quit_cover PARAMS ((PTR));

static void disconnect PARAMS ((int));
#endif

static void source_cleanup PARAMS ((FILE *));

/* If this definition isn't overridden by the header files, assume
   that isatty and fileno exist on this system.  */
#ifndef ISATTY
#define ISATTY(FP)	(isatty (fileno (FP)))
#endif

/* Initialization file name for gdb.  This is overridden in some configs.  */

#ifndef	GDBINIT_FILENAME
#define	GDBINIT_FILENAME	".gdbinit"
#endif
char gdbinit[] = GDBINIT_FILENAME;

int inhibit_gdbinit = 0;

/* If nonzero, and GDB has been configured to be able to use windows,
   attempt to open them upon startup.  */

int use_windows = 1;

/* Version number of GDB, as a string.  */

extern char *version;

/* Canonical host name as a string. */

extern char *host_name;

/* Canonical target name as a string. */

extern char *target_name;

extern char lang_frame_mismatch_warn[];		/* language.c */

/* Flag for whether we want all the "from_tty" gubbish printed.  */

int caution = 1;			/* Default is yes, sigh. */

/* Define all cmd_list_elements.  */

/* Chain containing all defined commands.  */

struct cmd_list_element *cmdlist;

/* Chain containing all defined info subcommands.  */

struct cmd_list_element *infolist;

/* Chain containing all defined enable subcommands. */

struct cmd_list_element *enablelist;

/* Chain containing all defined disable subcommands. */

struct cmd_list_element *disablelist;

/* Chain containing all defined toggle subcommands. */

struct cmd_list_element *togglelist;

/* Chain containing all defined stop subcommands. */

struct cmd_list_element *stoplist;

/* Chain containing all defined delete subcommands. */

struct cmd_list_element *deletelist;

/* Chain containing all defined "enable breakpoint" subcommands. */

struct cmd_list_element *enablebreaklist;

/* Chain containing all defined set subcommands */

struct cmd_list_element *setlist;

/* Chain containing all defined unset subcommands */

struct cmd_list_element *unsetlist;

/* Chain containing all defined show subcommands.  */

struct cmd_list_element *showlist;

/* Chain containing all defined \"set history\".  */

struct cmd_list_element *sethistlist;

/* Chain containing all defined \"show history\".  */

struct cmd_list_element *showhistlist;

/* Chain containing all defined \"unset history\".  */

struct cmd_list_element *unsethistlist;

/* Chain containing all defined maintenance subcommands. */

struct cmd_list_element *maintenancelist;

/* Chain containing all defined "maintenance info" subcommands. */

struct cmd_list_element *maintenanceinfolist;

/* Chain containing all defined "maintenance print" subcommands. */

struct cmd_list_element *maintenanceprintlist;

struct cmd_list_element *setprintlist;

struct cmd_list_element *showprintlist;

struct cmd_list_element *setchecklist;

struct cmd_list_element *showchecklist;

/* stdio stream that command input is being read from.  Set to stdin normally.
   Set by source_command to the file we are sourcing.  Set to NULL if we are
   executing a user-defined command or interacting via a GUI.  */

FILE *instream;

/* Current working directory.  */

char *current_directory;

/* The directory name is actually stored here (usually).  */
char gdb_dirbuf[1024];

/* Function to call before reading a command, if nonzero.
   The function receives two args: an input stream,
   and a prompt string.  */

void (*window_hook) PARAMS ((FILE *, char *));

int epoch_interface;
int xgdb_verbose;

/* gdb prints this when reading a command interactively */
static char *gdb_prompt_string;		/* the global prompt string */
extern char *get_prompt PARAMS((void));	/* access function for prompt string */

/* Buffer used for reading command lines, and the size
   allocated for it so far.  */

char *line;
int linesize = 100;

/* Nonzero if the current command is modified by "server ".  This
   affects things like recording into the command history, comamnds
   repeating on RETURN, etc.  This is so a user interface (emacs, GUI,
   whatever) can issue its own commands and also send along commands
   from the user, and have the user not notice that the user interface
   is issuing commands too.  */
int server_command;

/* Baud rate specified for talking to serial target systems.  Default
   is left as -1, so targets can choose their own defaults.  */
/* FIXME: This means that "show remotebaud" and gr_files_info can print -1
   or (unsigned int)-1.  This is a Bad User Interface.  */

int baud_rate = -1;

/* Timeout limit for response from target. */

int remote_timeout = 20;	/* Set default to 20 */

/* Non-zero tells remote* modules to output debugging info.  */

int remote_debug = 0;

/* Level of control structure.  */
static int control_level;

/* Structure for arguments to user defined functions.  */
#define MAXUSERARGS 10
struct user_args
{
  struct user_args *next;
  struct
    {
      char *arg;
      int len;
    } a[MAXUSERARGS];
  int count;
} *user_args;

/* Signal to catch ^Z typed while reading a command: SIGTSTP or SIGCONT.  */

#ifndef STOP_SIGNAL
#ifdef SIGTSTP
#define STOP_SIGNAL SIGTSTP
static void stop_sig PARAMS ((int));
#endif
#endif

/* Some System V have job control but not sigsetmask(). */
#if !defined (HAVE_SIGSETMASK)
#if !defined (USG)
#define HAVE_SIGSETMASK 1
#else
#define HAVE_SIGSETMASK 0
#endif
#endif

#if 0 == (HAVE_SIGSETMASK)
#define sigsetmask(n)
#endif

/* Hooks for alternate command interfaces.  */

/* Called after most modules have been initialized, but before taking users
   command file.  */

void (*init_ui_hook) PARAMS ((char *argv0));

/* This hook is called from within gdb's many mini-event loops which could
   steal control from a real user interface's event loop. It returns
   non-zero if the user is requesting a detach, zero otherwise. */

int (*ui_loop_hook) PARAMS ((int));

/* Called instead of command_loop at top level.  Can be invoked via
   return_to_top_level.  */

void (*command_loop_hook) PARAMS ((void));


/* Called instead of fputs for all output.  */

void (*fputs_unfiltered_hook) PARAMS ((const char *linebuffer, GDB_FILE *stream));

/* Called when the target says something to the host, which may
   want to appear in a different window. */

void (*target_output_hook) PARAMS ((char *));

/* Called from print_frame_info to list the line we stopped in.  */

void (*print_frame_info_listing_hook) PARAMS ((struct symtab *s, int line,
					       int stopline, int noerror));
/* Replaces most of query.  */

int (*query_hook) PARAMS ((const char *, va_list));

/* Replaces most of warning.  */

void (*warning_hook) PARAMS ((const char *, va_list));

/* Called from gdb_flush to flush output.  */

void (*flush_hook) PARAMS ((GDB_FILE *stream));

/* These three functions support getting lines of text from the user.  They
   are used in sequence.  First readline_begin_hook is called with a text
   string that might be (for example) a message for the user to type in a
   sequence of commands to be executed at a breakpoint.  If this function
   calls back to a GUI, it might take this opportunity to pop up a text
   interaction window with this message.  Next, readline_hook is called
   with a prompt that is emitted prior to collecting the user input.
   It can be called multiple times.  Finally, readline_end_hook is called
   to notify the GUI that we are done with the interaction window and it
   can close it. */

void (*readline_begin_hook) PARAMS ((char *, ...));
char * (*readline_hook) PARAMS ((char *));
void (*readline_end_hook) PARAMS ((void));

/* Called as appropriate to notify the interface of the specified breakpoint
   conditions.  */

void (*create_breakpoint_hook) PARAMS ((struct breakpoint *bpt));
void (*delete_breakpoint_hook) PARAMS ((struct breakpoint *bpt));
void (*modify_breakpoint_hook) PARAMS ((struct breakpoint *bpt));

/* Called during long calculations to allow GUI to repair window damage, and to
   check for stop buttons, etc... */

void (*interactive_hook) PARAMS ((void));

/* Called when the registers have changed, as a hint to a GUI
   to minimize window update. */

void (*registers_changed_hook) PARAMS ((void));

/* Tell the GUI someone changed the register REGNO. -1 means
   that the caller does not know which register changed or
   that several registers have changed (see value_assign).*/
void (*register_changed_hook) PARAMS ((int regno));

/* Tell the GUI someone changed LEN bytes of memory at ADDR */
void (*memory_changed_hook) PARAMS ((CORE_ADDR addr, int len));

/* Called when going to wait for the target.  Usually allows the GUI to run
   while waiting for target events.  */

int (*target_wait_hook) PARAMS ((int pid, struct target_waitstatus *status));

/* Used by UI as a wrapper around command execution.  May do various things
   like enabling/disabling buttons, etc...  */

void (*call_command_hook) PARAMS ((struct cmd_list_element *c, char *cmd,
				   int from_tty));

/* Called when the current thread changes.  Argument is thread id.  */

void (*context_hook) PARAMS ((int id));

/* Takes control from error ().  Typically used to prevent longjmps out of the
   middle of the GUI.  Usually used in conjunction with a catch routine.  */

NORETURN void (*error_hook) PARAMS ((void)) ATTR_NORETURN;


/* Where to go for return_to_top_level (RETURN_ERROR).  */
SIGJMP_BUF error_return;
/* Where to go for return_to_top_level (RETURN_QUIT).  */
SIGJMP_BUF quit_return;

/* Return for reason REASON.  This generally gets back to the command
   loop, but can be caught via catch_errors.  */

NORETURN void
return_to_top_level (reason)
     enum return_reason reason;
{
  quit_flag = 0;
  immediate_quit = 0;

  /* Perhaps it would be cleaner to do this via the cleanup chain (not sure
     I can think of a reason why that is vital, though).  */
  bpstat_clear_actions(stop_bpstat);	/* Clear queued breakpoint commands */

  disable_current_display ();
  do_cleanups (ALL_CLEANUPS);

  if (annotation_level > 1)
    switch (reason)
      {
      case RETURN_QUIT:
	annotate_quit ();
	break;
      case RETURN_ERROR:
	annotate_error ();
	break;
      }

  (NORETURN void) SIGLONGJMP
    (reason == RETURN_ERROR ? error_return : quit_return, 1);
}

/* Call FUNC with arg ARGS, catching any errors.  If there is no
   error, return the value returned by FUNC.  If there is an error,
   print ERRSTRING, print the specific error message, then return
   zero.

   Must not be called with immediate_quit in effect (bad things might
   happen, say we got a signal in the middle of a memcpy to quit_return).
   This is an OK restriction; with very few exceptions immediate_quit can
   be replaced by judicious use of QUIT.

   MASK specifies what to catch; it is normally set to
   RETURN_MASK_ALL, if for no other reason than that the code which
   calls catch_errors might not be set up to deal with a quit which
   isn't caught.  But if the code can deal with it, it generally
   should be RETURN_MASK_ERROR, unless for some reason it is more
   useful to abort only the portion of the operation inside the
   catch_errors.  Note that quit should return to the command line
   fairly quickly, even if some further processing is being done.  */

int
catch_errors (func, args, errstring, mask)
     catch_errors_ftype *func;
     PTR args;
     char *errstring;
     return_mask mask;
{
  SIGJMP_BUF saved_error;
  SIGJMP_BUF saved_quit;
  SIGJMP_BUF tmp_jmp;
  int val;
  struct cleanup *saved_cleanup_chain;
  char *saved_error_pre_print;
  char *saved_quit_pre_print;

  saved_cleanup_chain = save_cleanups ();
  saved_error_pre_print = error_pre_print;
  saved_quit_pre_print = quit_pre_print;

  if (mask & RETURN_MASK_ERROR)
    {
      memcpy ((char *)saved_error, (char *)error_return, sizeof (SIGJMP_BUF));
      error_pre_print = errstring;
    }
  if (mask & RETURN_MASK_QUIT)
    {
      memcpy (saved_quit, quit_return, sizeof (SIGJMP_BUF));
      quit_pre_print = errstring;
    }

  if (SIGSETJMP (tmp_jmp) == 0)
    {
      if (mask & RETURN_MASK_ERROR)
	memcpy (error_return, tmp_jmp, sizeof (SIGJMP_BUF));
      if (mask & RETURN_MASK_QUIT)
	memcpy (quit_return, tmp_jmp, sizeof (SIGJMP_BUF));
      val = (*func) (args);
    }
  else
    val = 0;

  restore_cleanups (saved_cleanup_chain);

  if (mask & RETURN_MASK_ERROR)
    {
      memcpy (error_return, saved_error, sizeof (SIGJMP_BUF));
      error_pre_print = saved_error_pre_print;
    }
  if (mask & RETURN_MASK_QUIT)
    {
      memcpy (quit_return, saved_quit, sizeof (SIGJMP_BUF));
      quit_pre_print = saved_quit_pre_print;
    }
  return val;
}

/* Handler for SIGHUP.  */

#ifdef SIGHUP
static void
disconnect (signo)
int signo;
{
  catch_errors (quit_cover, NULL,
		"Could not kill the program being debugged", RETURN_MASK_ALL);
  signal (SIGHUP, SIG_DFL);
  kill (getpid (), SIGHUP);
}

/* Just a little helper function for disconnect().  */

/* NOTE 1999-04-29: This function will be static again, once we modify
   gdb to use the event loop as the default command loop and we merge
   event-top.c into this file, top.c */
/* static */ int
quit_cover (s)
     PTR s;
{
  caution = 0;		/* Throw caution to the wind -- we're exiting.
			   This prevents asking the user dumb questions.  */
  quit_command((char *)0, 0);
  return 0;
}
#endif /* defined SIGHUP */

/* Line number we are currently in in a file which is being sourced.  */
/* NOTE 1999-04-29: This variable will be static again, once we modify
   gdb to use the event loop as the default command loop and we merge
   event-top.c into this file, top.c */
/* static */ int source_line_number;

/* Name of the file we are sourcing.  */
/* NOTE 1999-04-29: This variable will be static again, once we modify
   gdb to use the event loop as the default command loop and we merge
   event-top.c into this file, top.c */
/* static */ char *source_file_name;

/* Buffer containing the error_pre_print used by the source stuff.
   Malloc'd.  */
/* NOTE 1999-04-29: This variable will be static again, once we modify
   gdb to use the event loop as the default command loop and we merge
   event-top.c into this file, top.c */
/* static */ char *source_error;
static int source_error_allocated;

/* Something to glom on to the start of error_pre_print if source_file_name
   is set.  */
/* NOTE 1999-04-29: This variable will be static again, once we modify
   gdb to use the event loop as the default command loop and we merge
   event-top.c into this file, top.c */
/* static */ char *source_pre_error;

/* Clean up on error during a "source" command (or execution of a
   user-defined command).  */

static void
source_cleanup (stream)
     FILE *stream;
{
  /* Restore the previous input stream.  */
  instream = stream;
}

/* Read commands from STREAM.  */
void
read_command_file (stream)
     FILE *stream;
{
  struct cleanup *cleanups;

  cleanups = make_cleanup ((make_cleanup_func) source_cleanup, instream);
  instream = stream;
  command_loop (); 
  do_cleanups (cleanups);
}

extern void init_proc PARAMS ((void));

void (*pre_init_ui_hook) PARAMS ((void));

void
gdb_init (argv0)
     char *argv0;
{
  if (pre_init_ui_hook)
    pre_init_ui_hook ();

  /* Run the init function of each source file */

  getcwd (gdb_dirbuf, sizeof (gdb_dirbuf));
  current_directory = gdb_dirbuf;

  init_cmd_lists ();	/* This needs to be done first */
  initialize_targets (); /* Setup target_terminal macros for utils.c */
  initialize_utils ();	/* Make errors and warnings possible */
  initialize_all_files ();
  init_main ();		/* But that omits this file!  Do it now */

  /* The signal handling mechanism is different depending whether or
     not the async version is run. NOTE: in the future we plan to make
     the event loop be the default engine of gdb, and this difference
     will disappear. */
  if (async_p)
    async_init_signals ();
  else
    init_signals (); 

  /* We need a default language for parsing expressions, so simple things like
     "set width 0" won't fail if no language is explicitly set in a config file
     or implicitly set by reading an executable during startup. */
  set_language (language_c);
  expected_language = current_language;	/* don't warn about the change.  */

  /* All the interpreters should have had a look at things by now.
     Initialize the selected interpreter. */
  if (init_ui_hook)
    init_ui_hook (argv0);
}

/* Allocate, initialize a new command line structure for one of the
   control commands (if/while).  */

static struct command_line *
build_command_line (type, args)
     enum command_control_type type;
     char *args;
{
  struct command_line *cmd;

  if (args == NULL)
    error ("if/while commands require arguments.\n");

  cmd = (struct command_line *)xmalloc (sizeof (struct command_line));
  cmd->next = NULL;
  cmd->control_type = type;

  cmd->body_count = 1;
  cmd->body_list
    = (struct command_line **)xmalloc (sizeof (struct command_line *)
				       * cmd->body_count);
  memset (cmd->body_list, 0, sizeof (struct command_line *) * cmd->body_count);
  cmd->line = savestring (args, strlen (args));
  return cmd;
}

/* Build and return a new command structure for the control commands
   such as "if" and "while".  */

static struct command_line *
get_command_line (type, arg)
     enum command_control_type type;
     char *arg;
{
  struct command_line *cmd;
  struct cleanup *old_chain = NULL;

  /* Allocate and build a new command line structure.  */
  cmd = build_command_line (type, arg);

  old_chain = make_cleanup ((make_cleanup_func) free_command_lines, &cmd);

  /* Read in the body of this command.  */
  if (recurse_read_control_structure (cmd) == invalid_control)
    {
      warning ("error reading in control structure\n");
      do_cleanups (old_chain);
      return NULL;
    }

  discard_cleanups (old_chain);
  return cmd;
}

/* Recursively print a command (including full control structures).  */
void
print_command_line (cmd, depth, stream)
     struct command_line *cmd;
     unsigned int depth;
     GDB_FILE *stream;
{
  unsigned int i;

  if (depth)
    {
      for (i = 0; i < depth; i++)
	fputs_filtered ("  ", stream);
    }

  /* A simple command, print it and return.  */
  if (cmd->control_type == simple_control)
    {
      fputs_filtered (cmd->line, stream);
      fputs_filtered ("\n", stream);
      return;
    }

  /* loop_continue to jump to the start of a while loop, print it
     and return. */
  if (cmd->control_type == continue_control)
    {
      fputs_filtered ("loop_continue\n", stream);
      return;
    }

  /* loop_break to break out of a while loop, print it and return.  */
  if (cmd->control_type == break_control)
    {
      fputs_filtered ("loop_break\n", stream);
      return;
    }

  /* A while command.  Recursively print its subcommands before returning.  */
  if (cmd->control_type == while_control)
    {
      struct command_line *list;
      fputs_filtered ("while ", stream);
      fputs_filtered (cmd->line, stream);
      fputs_filtered ("\n", stream);
      list = *cmd->body_list;
      while (list)
	{
	  print_command_line (list, depth + 1, stream);
	  list = list->next;
	}
    }

  /* An if command.  Recursively print both arms before returning.  */
  if (cmd->control_type == if_control)
    {
      fputs_filtered ("if ", stream);
      fputs_filtered (cmd->line, stream);
      fputs_filtered ("\n", stream);
      /* The true arm. */
      print_command_line (cmd->body_list[0], depth + 1, stream);

      /* Show the false arm if it exists.  */
      if (cmd->body_count == 2)
	  {
	    if (depth)
	      {
		for (i = 0; i < depth; i++)
		  fputs_filtered ("  ", stream);
	      }
	    fputs_filtered ("else\n", stream);
	    print_command_line (cmd->body_list[1], depth + 1, stream);
	  }
      if (depth)
	{
	  for (i = 0; i < depth; i++)
	    fputs_filtered ("  ", stream);
	}
      fputs_filtered ("end\n", stream);
    }
}

/* Execute the command in CMD.  */

enum command_control_type
execute_control_command (cmd)
     struct command_line *cmd;
{
  struct expression *expr;
  struct command_line *current;
  struct cleanup *old_chain = 0;
  value_ptr val;
  value_ptr val_mark;
  int loop;
  enum command_control_type ret;
  char *new_line;

  switch (cmd->control_type)
    {
    case simple_control:
      /* A simple command, execute it and return.  */
      new_line = insert_args (cmd->line);
      if (!new_line)
	return invalid_control;
      old_chain = make_cleanup ((make_cleanup_func) free_current_contents, 
                                &new_line);
      execute_command (new_line, 0);
      ret = cmd->control_type;
      break;

    case continue_control:
    case break_control:
      /* Return for "continue", and "break" so we can either
	 continue the loop at the top, or break out.  */
      ret = cmd->control_type;
      break;

    case while_control:
      {
	/* Parse the loop control expression for the while statement.  */
	new_line = insert_args (cmd->line);
	if (!new_line)
	  return invalid_control;
	old_chain = make_cleanup ((make_cleanup_func) free_current_contents, 
                                  &new_line);
	expr = parse_expression (new_line);
	make_cleanup ((make_cleanup_func) free_current_contents, &expr);
	
	ret = simple_control;
	loop = 1;

	/* Keep iterating so long as the expression is true.  */
	while (loop == 1)
	  {
	    int cond_result;

	    QUIT;

	    /* Evaluate the expression.  */
	    val_mark = value_mark ();
	    val = evaluate_expression (expr);
	    cond_result = value_true (val);
	    value_free_to_mark (val_mark);

	    /* If the value is false, then break out of the loop.  */
	    if (!cond_result)
	      break;

	    /* Execute the body of the while statement.  */
	    current = *cmd->body_list;
	    while (current)
	      {
		ret = execute_control_command (current);

		/* If we got an error, or a "break" command, then stop
		   looping.  */
		if (ret == invalid_control || ret == break_control)
		  {
		    loop = 0;
		    break;
		  }

		/* If we got a "continue" command, then restart the loop
		   at this point.  */
		if (ret == continue_control)
		  break;
		
		/* Get the next statement.  */
		current = current->next; 
	      }
	  }

	/* Reset RET so that we don't recurse the break all the way down.  */
	if (ret == break_control)
	  ret = simple_control;

	break;
      }

    case if_control:
      {
	new_line = insert_args (cmd->line);
	if (!new_line)
	  return invalid_control;
	old_chain = make_cleanup ((make_cleanup_func) free_current_contents, 
                                  &new_line);
	/* Parse the conditional for the if statement.  */
	expr = parse_expression (new_line);
	make_cleanup ((make_cleanup_func) free_current_contents, &expr);

	current = NULL;
	ret = simple_control;

	/* Evaluate the conditional.  */
	val_mark = value_mark ();
	val = evaluate_expression (expr);

	/* Choose which arm to take commands from based on the value of the
	   conditional expression.  */
	if (value_true (val))
	  current = *cmd->body_list;
	else if (cmd->body_count == 2)
	  current = *(cmd->body_list + 1);
	value_free_to_mark (val_mark);

	/* Execute commands in the given arm.  */
	while (current)
	  {
	    ret = execute_control_command (current);

	    /* If we got an error, get out.  */
	    if (ret != simple_control)
	      break;

	    /* Get the next statement in the body.  */
	    current = current->next;
	  }

	break;
      }

    default:
      warning ("Invalid control type in command structure.");
      return invalid_control;
    }

  if (old_chain)
    do_cleanups (old_chain);

  return ret;
}

/* "while" command support.  Executes a body of statements while the
   loop condition is nonzero.  */

static void
while_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  struct command_line *command = NULL;

  control_level = 1;
  command = get_command_line (while_control, arg);

  if (command == NULL)
    return;

  execute_control_command (command);
  free_command_lines (&command);
}

/* "if" command support.  Execute either the true or false arm depending
   on the value of the if conditional.  */

static void
if_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  struct command_line *command = NULL;

  control_level = 1;
  command = get_command_line (if_control, arg);

  if (command == NULL)
    return;

  execute_control_command (command);
  free_command_lines (&command);
}

/* Cleanup */
static void
arg_cleanup ()
{
  struct user_args *oargs = user_args;
  if (!user_args)
    fatal ("Internal error, arg_cleanup called with no user args.\n");

  user_args = user_args->next;
  free (oargs);
}

/* Bind the incomming arguments for a user defined command to
   $arg0, $arg1 ... $argMAXUSERARGS.  */

static struct cleanup *
setup_user_args (p)
     char *p;
{
  struct user_args *args;
  struct cleanup *old_chain;
  unsigned int arg_count = 0;

  args = (struct user_args *)xmalloc (sizeof (struct user_args));
  memset (args, 0, sizeof (struct user_args));

  args->next = user_args;
  user_args = args;

  old_chain = make_cleanup ((make_cleanup_func) arg_cleanup, 0);

  if (p == NULL)
    return old_chain;

  while (*p)
    {
      char *start_arg;
      int squote = 0;
      int dquote = 0;
      int bsquote = 0;

      if (arg_count >= MAXUSERARGS)
	{
	  error ("user defined function may only have %d arguments.\n",
		 MAXUSERARGS);
	  return old_chain;
	}

      /* Strip whitespace.  */
      while (*p == ' ' || *p == '\t')
	p++;

      /* P now points to an argument.  */
      start_arg = p;
      user_args->a[arg_count].arg = p;

      /* Get to the end of this argument.  */
      while (*p)
	{
	  if (((*p == ' ' || *p == '\t')) && !squote && !dquote && !bsquote)
	    break;
	  else
	    {
	      if (bsquote)
		bsquote = 0;
	      else if (*p == '\\')
		bsquote = 1;
	      else if (squote)
		{
		  if (*p == '\'')
		    squote = 0;
		}
	      else if (dquote)
		{
		  if (*p == '"')
		    dquote = 0;
		}
	      else
		{
		  if (*p == '\'')
		    squote = 1;
		  else if (*p == '"')
		    dquote = 1;
		}
	      p++;
	    }
	}

      user_args->a[arg_count].len = p - start_arg;
      arg_count++;
      user_args->count++;
    }
  return old_chain;
}

/* Given character string P, return a point to the first argument ($arg),
   or NULL if P contains no arguments.  */

static char *
locate_arg (p)
     char *p;
{
  while ((p = strchr (p, '$')))
    {
      if (strncmp (p, "$arg", 4) == 0 && isdigit (p[4]))
	return p;
      p++;
    }
  return NULL;
}

/* Insert the user defined arguments stored in user_arg into the $arg
   arguments found in line, with the updated copy being placed into nline.  */

static char *
insert_args (line)
     char *line;
{
  char *p, *save_line, *new_line;
  unsigned len, i;

  /* First we need to know how much memory to allocate for the new line.  */
  save_line = line;
  len = 0;
  while ((p = locate_arg (line)))
    {
      len += p - line;
      i = p[4] - '0';
      
      if (i >= user_args->count)
	{
	  error ("Missing argument %d in user function.\n", i);
	  return NULL;
	}
      len += user_args->a[i].len;
      line = p + 5;
    }

  /* Don't forget the tail.  */
  len += strlen (line);

  /* Allocate space for the new line and fill it in.  */
  new_line = (char *)xmalloc (len + 1);
  if (new_line == NULL)
    return NULL;

  /* Restore pointer to beginning of old line.  */
  line = save_line;

  /* Save pointer to beginning of new line.  */
  save_line = new_line;

  while ((p = locate_arg (line)))
    {
      int i, len;

      memcpy (new_line, line, p - line);
      new_line += p - line;
      i = p[4] - '0';

      len = user_args->a[i].len;
      if (len)
	{
	  memcpy (new_line, user_args->a[i].arg, len);
	  new_line += len;
	}
      line = p + 5;
    }
  /* Don't forget the tail.  */
  strcpy (new_line, line);

  /* Return a pointer to the beginning of the new line.  */
  return save_line;
}

void
execute_user_command (c, args)
     struct cmd_list_element *c;
     char *args;
{
  register struct command_line *cmdlines;
  struct cleanup *old_chain;
  enum command_control_type ret;

  old_chain = setup_user_args (args);

  cmdlines = c->user_commands;
  if (cmdlines == 0)
    /* Null command */
    return;

  /* Set the instream to 0, indicating execution of a
     user-defined function.  */
  old_chain = make_cleanup ((make_cleanup_func) source_cleanup, instream);
  instream = (FILE *) 0;
  while (cmdlines)
    {
      ret = execute_control_command (cmdlines);
      if (ret != simple_control && ret != break_control)
	{
	  warning ("Error in control structure.\n");
	  break;
	}
      cmdlines = cmdlines->next;
    }
  do_cleanups (old_chain);
}

/* Execute the line P as a command.
   Pass FROM_TTY as second argument to the defining function.  */

void
execute_command (p, from_tty)
     char *p;
     int from_tty;
{
  register struct cmd_list_element *c;
  register enum language flang;
  static int warned = 0;
  /* FIXME: These should really be in an appropriate header file */
  extern void serial_log_command PARAMS ((const char *));

  free_all_values ();

  /* Force cleanup of any alloca areas if using C alloca instead of
     a builtin alloca.  */
  alloca (0);

  /* This can happen when command_line_input hits end of file.  */
  if (p == NULL)
      return;

  serial_log_command (p);

  while (*p == ' ' || *p == '\t') p++;
  if (*p)
    {
      char *arg;

      c = lookup_cmd (&p, cmdlist, "", 0, 1);
      /* Pass null arg rather than an empty one.  */
      arg = *p ? p : 0;

      /* Clear off trailing whitespace, except for set and complete command.  */
      if (arg && c->type != set_cmd && c->function.cfunc != complete_command)
	{
	  p = arg + strlen (arg) - 1;
	  while (p >= arg && (*p == ' ' || *p == '\t'))
	    p--;
	  *(p + 1) = '\0';
	}

      /* If this command has been hooked, run the hook first. */
      if (c->hook)
	execute_user_command (c->hook, (char *)0);

      if (c->class == class_user)
	execute_user_command (c, arg);
      else if (c->type == set_cmd || c->type == show_cmd)
	do_setshow_command (arg, from_tty & caution, c);
      else if (c->function.cfunc == NO_FUNCTION)
	error ("That is not a command, just a help topic.");
      else if (call_command_hook)
	call_command_hook (c, arg, from_tty & caution);
      else
	(*c->function.cfunc) (arg, from_tty & caution);
   }

  /* Tell the user if the language has changed (except first time).  */
  if (current_language != expected_language)
  {
    if (language_mode == language_mode_auto) {
      language_info (1);	/* Print what changed.  */
    }
    warned = 0;
  }

  /* Warn the user if the working language does not match the
     language of the current frame.  Only warn the user if we are
     actually running the program, i.e. there is a stack. */
  /* FIXME:  This should be cacheing the frame and only running when
     the frame changes.  */

  if (target_has_stack)
    {
      flang = get_frame_language ();
      if (!warned
	  && flang != language_unknown
	  && flang != current_language->la_language)
	{
	  printf_filtered ("%s\n", lang_frame_mismatch_warn);
	  warned = 1;
	}
    }
}

/* ARGSUSED */
/* NOTE 1999-04-29: This function will be static again, once we modify
   gdb to use the event loop as the default command loop and we merge
   event-top.c into this file, top.c */
/* static */ void
command_loop_marker (foo)
     int foo;
{
}

/* Read commands from `instream' and execute them
   until end of file or error reading instream.  */

void
command_loop ()
{
  struct cleanup *old_chain;
  char *command;
  int stdin_is_tty = ISATTY (stdin);
  long time_at_cmd_start;
#ifdef HAVE_SBRK
  long space_at_cmd_start = 0;
#endif
  extern int display_time;
  extern int display_space;

  while (instream && !feof (instream))
    {
#if defined(TUI)
      extern int insert_mode;
#endif
      if (window_hook && instream == stdin)
	(*window_hook) (instream, get_prompt ());

      quit_flag = 0;
      if (instream == stdin && stdin_is_tty)
	reinitialize_more_filter ();
      old_chain = make_cleanup ((make_cleanup_func) command_loop_marker, 0);

#if defined(TUI)
      /* A bit of paranoia: I want to make sure the "insert_mode" global
       * is clear except when it is being used for command-line editing
       * (see tuiIO.c, utils.c); otherwise normal output will
       * get messed up in the TUI. So clear it before/after
       * the command-line-input call. - RT
       */
      insert_mode = 0;
#endif
      /* Get a command-line. This calls the readline package. */
      command = command_line_input (instream == stdin ? 
				    get_prompt () : (char *) NULL,
				    instream == stdin, "prompt");
#if defined(TUI)
      insert_mode = 0;
#endif
      if (command == 0)
	return;

      time_at_cmd_start = get_run_time ();

      if (display_space)
	{
#ifdef HAVE_SBRK
	  extern char **environ;
	  char *lim = (char *) sbrk (0);

	  space_at_cmd_start = (long) (lim - (char *) &environ);
#endif
	}

      execute_command (command, instream == stdin);
      /* Do any commands attached to breakpoint we stopped at.  */
      bpstat_do_actions (&stop_bpstat);
      do_cleanups (old_chain);

      if (display_time)
	{
	  long cmd_time = get_run_time () - time_at_cmd_start;

	  printf_unfiltered ("Command execution time: %ld.%06ld\n",
			     cmd_time / 1000000, cmd_time % 1000000);
	}

      if (display_space)
	{
#ifdef HAVE_SBRK
	  extern char **environ;
	  char *lim = (char *) sbrk (0);
	  long space_now = lim - (char *) &environ;
	  long space_diff = space_now - space_at_cmd_start;

	  printf_unfiltered ("Space used: %ld (%c%ld for this command)\n",
			     space_now,
			     (space_diff >= 0 ? '+' : '-'),
			     space_diff);
#endif
	}
    }
}

