/* Top level stuff for GDB, the GNU debugger.
   Copyright 1999 Free Software Foundation, Inc.
   Written by Elena Zannoni <ezannoni@cygnus.com> of Cygnus Solutions.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#include "defs.h"
#include "event-loop.h"
#ifdef HAVE_POLL
#include <sys/poll.h>
#endif
#include "inferior.h"

/* readline include files */
#include <readline/readline.h>
#include <readline/history.h>

/* readline defines this.  */
#undef savestring

extern FILE *instream;

static void command_line_handler PARAMS ((char *));
static void gdb_readline2 PARAMS ((void));
static void pop_prompt PARAMS ((void));
static void push_prompt PARAMS ((char *, char *, char *));
static void change_line_handler PARAMS ((void));
static void change_annotation_level PARAMS ((void));
static void command_handler PARAMS ((char *));

/* Signal handlers. */
void handle_sigint PARAMS ((int));
void handle_sigquit PARAMS ((int));
void handle_sighup PARAMS ((int));
void handle_sigfpe PARAMS ((int));
void handle_sigwinch PARAMS ((int));

/* Functions to be invoked by the event loop in response to
   signals. */
void async_request_quit PARAMS ((void));
void async_do_nothing PARAMS ((void));
void async_disconnect PARAMS ((void));
void async_float_handler PARAMS ((void));

/* Functions from top.c. */
extern void command_loop_marker PARAMS ((int));
extern int quit_cover PARAMS ((PTR));
extern void quit_command PARAMS ((char *, int));
extern void execute_command PARAMS ((char *, int));

/* Variables from top.c. */
extern int source_line_number;
extern char *source_file_name;
extern char *source_error;
extern char *source_pre_error;
extern int history_expansion_p;
extern int server_command;

/* If this definition isn't overridden by the header files, assume
   that isatty and fileno exist on this system.  */
#ifndef ISATTY
#define ISATTY(FP)	(isatty (fileno (FP)))
#endif

/* Hook for alternate command interface.  */
void (*async_hook) PARAMS ((void));

/* Readline offers an alternate interface, via callback
   functions. These are all included in the file callback.c in the
   readline distribution.  This file provides (mainly) a function, which
   the event loop uses as callback (i.e. event handler) whenever an event
   is detected on the standard input file descriptor.
   readline_callback_read_char is called (by the GDB event loop) whenever
   there is a new character ready on the input stream. This function
   incrementally builds a buffer internal to readline where it
   accumulates the line read up to the point of invocation.  In the
   special case in which the character read is newline, the function
   invokes a GDB supplied callback routine, which does the processing of
   a full command line.  This latter routine is the asynchronous analog
   of the old command_line_input in gdb. Instead of invoking (and waiting
   for) readline to read the command line and pass it back to
   command_loop for processing, the new command_line_handler function has
   the command line already available as its parameter.  INPUT_HANDLER is
   to be set to the function that readline will invoke when a complete
   line of input is ready.  CALL_READLINE is to be set to the function
   that readline offers as callback to the event_loop. */

void (*input_handler) PARAMS ((char *));
void (*call_readline) PARAMS ((void));

/* Important variables for the event loop. */

/* This is used to determine if GDB is using the readline library or
   its own simplified form of readline. It is used by the asynchronous
   form of the set editing command. 
   ezannoni: as of 1999-04-29 I expect that this
   variable will not be used after gdb is changed to use the event
   loop as default engine, and event-top.c is merged into top.c. */
int async_command_editing_p;

/* This variable contains the new prompt that the user sets with the
   set prompt command. */
char *new_async_prompt;

/* This is the annotation suffix that will be used when the
   annotation_level is 2. */
char *async_annotation_suffix;

/* This is the file descriptor for the input stream that GDB uses to
   read commands from. */
int input_fd;

/* This is the prompt stack. Prompts will be pushed on the stack as
   needed by the different 'kinds' of user inputs GDB is asking
   for. See event-loop.h. */
struct prompts the_prompts;

/* signal handling variables */
/* Each of these is a pointer to a function that the event loop will
   invoke if the corresponding signal has received. The real signal
   handlers mark these functions as ready to be executed and the event
   loop, in a later iteration, calls them. See the function
   invoke_async_signal_handler. */
async_signal_handler *sigint_token;
#ifdef SIGHUP
async_signal_handler *sighup_token;
#endif
async_signal_handler *sigquit_token;
async_signal_handler *sigfpe_token;
#if defined(SIGWINCH) && defined(SIGWINCH_HANDLER)
async_signal_handler *sigwinch_token;
#endif

/* Structure to save a partially entered command.  This is used when
   the user types '\' at the end of a command line. This is necessary
   because each line of input is handled by a different call to
   command_line_handler, and normally there is no state retained
   between different calls. */
int more_to_come = 0;

struct readline_input_state
  {
    char *linebuffer;
    char *linebuffer_ptr;
  }
readline_input_state;


/* Initialize all the necessary variables, start the event loop,
   register readline, and stdin. */
void
setup_event_loop ()
{
  int length = strlen (PREFIX (0)) + strlen (PROMPT (0)) + strlen (SUFFIX (0)) + 1;
  char *a_prompt = (char *) xmalloc (length);

  /* Set things up for readline to be invoked via the alternate
     interface, i.e. via a callback function (rl_callback_read_char). */
  call_readline = rl_callback_read_char;

  /* When readline has read an end-of-line character, it passes the
     complete line to gdb for processing. command_line_handler is the
     function that does this. */
  input_handler = command_line_handler;

  /* Tell readline what the prompt to display is and what function it
     will need to call after a whole line is read. */
  strcpy (a_prompt, PREFIX (0));
  strcat (a_prompt, PROMPT (0));
  strcat (a_prompt, SUFFIX (0));
  rl_callback_handler_install (a_prompt, input_handler);

  /* Tell readline to use the same input stream that gdb uses. */
  rl_instream = instream;
  /* Get a file descriptor for the input stream, so that we can
     register it with the event loop. */
  input_fd = fileno (instream);

  /* Now we need to create the event sources for the input file descriptor. */
  /* At this point in time, this is the only event source that we
     register with the even loop. Another source is going to be the
     target program (inferior), but that must be registered only when
     it actually exists (I.e. after we say 'run' or after we connect
     to a remote target. */
#ifdef HAVE_POLL
  create_file_handler (input_fd, POLLIN,
		       (file_handler_func *) call_readline, 0);
#else
  create_file_handler (input_fd, GDB_READABLE,
		       (file_handler_func *) call_readline, 0);
#endif

  /* Loop until there is something to do. This is the entry point to
     the event loop engine. gdb_do_one_event will process one event
     for each invocation.  It always returns 1, unless there are no
     more event sources registered. In this case it returns 0.  */
  while (gdb_do_one_event () != 0)
    ;

  /* We are done with the event loop. There are no more event sources
     to listen to.  So we exit GDB. */
  return;
}

/* Change the function to be invoked every time there is a character
   ready on stdin. This is used when the user sets the editing off,
   therefore bypassing readline, and letting gdb handle the input
   itself, via gdb_readline2. Also it is used in the opposite case in
   which the user sets editing on again, by restoring readline
   handling of the input. */
static void
change_line_handler ()
{
  if (async_command_editing_p)
    {
      /* Turn on editing by using readline. */
      call_readline = rl_callback_read_char;
    }
  else
    {
      /* Turn off editing by using gdb_readline2. */
      rl_callback_handler_remove ();
      call_readline = gdb_readline2;
    }

  /* To tell the event loop to change the handler associated with the
     input file descriptor, we need to create a new event source,
     corresponding to the same fd, but with a new event handler
     function. */
  delete_file_handler (input_fd);
#ifdef HAVE_POLL
  create_file_handler (input_fd, POLLIN,
		       (file_handler_func *) call_readline, 0);
#else
  create_file_handler (input_fd, GDB_READABLE,
		       (file_handler_func *) call_readline, 0);
#endif
}

/* Displays the prompt. The prompt that is displayed is the current
   top of the prompt stack, if the argument NEW_PROMPT is
   0. Otherwise, it displays whatever NEW_PROMPT is. This is used
   after each gdb command has completed, and in the following cases:
   1. when the user enters a command line which is ended by '\' 
   indicating that the command will continue on the next line. 
   In that case the prompt that is displayed is the empty string.
   2. When the user is entering 'commands' for a breakpoint, or 
   actions for a tracepoint. In this case the prompt will be '>' 
   3. Other???? 
   FIXME: 2. & 3. not implemented yet for async. */
void
display_gdb_prompt (new_prompt)
     char *new_prompt;
{
  int prompt_length = 0;

  if (!new_prompt)
    {
      /* Just use the top of the prompt stack. */
      prompt_length = strlen (PREFIX (0)) +
	strlen (SUFFIX (0)) +
	strlen (PROMPT (0)) + 1;

      new_prompt = (char *) alloca (prompt_length);

      /* Prefix needs to have new line at end. */
      strcpy (new_prompt, PREFIX (0));
      strcat (new_prompt, PROMPT (0));
      /* Suffix needs to have a new line at end and \032 \032 at
         beginning. */
      strcat (new_prompt, SUFFIX (0));
    }

  if (async_command_editing_p)
    {
      rl_callback_handler_remove ();
      rl_callback_handler_install (new_prompt, input_handler);
    }
  else if (new_prompt)
    {
      /* Don't use a _filtered function here.  It causes the assumed
         character position to be off, since the newline we read from
         the user is not accounted for.  */
      fputs_unfiltered (new_prompt, gdb_stdout);

#ifdef MPW
      /* Move to a new line so the entered line doesn't have a prompt
         on the front of it. */
      fputs_unfiltered ("\n", gdb_stdout);
#endif /* MPW */
      gdb_flush (gdb_stdout);
    }
}

/* Used when the user requests a different annotation level, with
   'set annotate'. It pushes a new prompt (with prefix and suffix) on top
   of the prompt stack, if the annotation level desired is 2, otherwise
   it pops the top of the prompt stack when we want the annotation level
   to be the normal ones (1 or 2). */
static void
change_annotation_level ()
{
  char *prefix, *suffix;

  if (!PREFIX (0) || !PROMPT (0) || !SUFFIX (0))
    {
      /* The prompt stack has not been initialized to "", we are
         using gdb w/o the --async switch */
      warning ("Command has same effect as set annotate");
      return;
    }

  if (annotation_level > 1)
    {
      if (!strcmp (PREFIX (0), "") && !strcmp (SUFFIX (0), ""))
	{
	  /* Push a new prompt if the previous annotation_level was not >1. */
	  prefix = (char *) alloca (strlen (async_annotation_suffix) + 10);
	  strcpy (prefix, "\n\032\032pre-");
	  strcat (prefix, async_annotation_suffix);
	  strcat (prefix, "\n");

	  suffix = (char *) alloca (strlen (async_annotation_suffix) + 6);
	  strcpy (suffix, "\n\032\032");
	  strcat (suffix, async_annotation_suffix);
	  strcat (suffix, "\n");

	  push_prompt (prefix, (char *) 0, suffix);
	}
    }
  else
    {
      if (strcmp (PREFIX (0), "") && strcmp (SUFFIX (0), ""))
	{
	  /* Pop the top of the stack, we are going back to annotation < 1. */
	  pop_prompt ();
	}
    }
}

/* Pushes a new prompt on the prompt stack. Each prompt has three
   parts: prefix, prompt, suffix. Usually prefix and suffix are empty
   strings, except when the annotation level is 2. Memory is allocated
   within savestring for the new prompt. */
static void
push_prompt (prefix, prompt, suffix)
     char *prefix;
     char *prompt;
     char *suffix;
{
  the_prompts.top++;
  PREFIX (0) = savestring (prefix, strlen (prefix));

  if (prompt)
    PROMPT (0) = savestring (prompt, strlen (prompt));
  else
    PROMPT (0) = savestring (PROMPT (-1), strlen (PROMPT (-1)));

  SUFFIX (0) = savestring (suffix, strlen (suffix));
}

/* Pops the top of the prompt stack, and frees the memory allocated for it. */
static void
pop_prompt ()
{
  if (strcmp (PROMPT (0), PROMPT (-1)))
    {
      free (PROMPT (-1));
      PROMPT (-1) = savestring (PROMPT (0), strlen (PROMPT (0)));
    }

  free (PREFIX (0));
  free (PROMPT (0));
  free (SUFFIX (0));
  the_prompts.top--;
}

/* Handles a gdb command. This function is called by
   command_line_handler, which has processed one or more input lines
   into COMMAND. */
/* NOTE: 1999-04-30 This is the asynchronous version of the command_loop
   function.  The command_loop function will be obsolete when we
   switch to use the event loop at every execution of gdb. */
static void
command_handler (command)
     char *command;
{
  struct cleanup *old_chain;
  int stdin_is_tty = ISATTY (stdin);
  long time_at_cmd_start;
#ifdef HAVE_SBRK
  long space_at_cmd_start = 0;
#endif
  extern int display_time;
  extern int display_space;

#if defined(TUI)
  extern int insert_mode;
#endif

  quit_flag = 0;
  if (instream == stdin && stdin_is_tty)
    reinitialize_more_filter ();
  old_chain = make_cleanup ((make_cleanup_func) command_loop_marker, 0);

#if defined(TUI)
  insert_mode = 0;
#endif
  /* If readline returned a NULL command, it means that the 
     connection with the terminal is gone. This happens at the
     end of a testsuite run, after Expect has hung up 
     but GDB is still alive. In such a case, we just quit gdb
     killing the inferior program too. */
  if (command == 0)
    quit_command ((char *) 0, stdin == instream);

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

/* Handle a complete line of input. This is called by the callback
   mechanism within the readline library.  Deal with incomplete commands
   as well, by saving the partial input in a global buffer.  */

/* NOTE: 1999-04-30 This is the asynchronous version of the
   command_line_input function. command_line_input will become
   obsolete once we use the event loop as the default mechanism in
   GDB. */
static void
command_line_handler (rl)
     char *rl;
{
  static char *linebuffer = 0;
  static unsigned linelength = 0;
  register char *p;
  char *p1;
  int change_prompt = 0;
  extern char *line;
  extern int linesize;
  char *nline;
  char got_eof = 0;


  int repeat = (instream == stdin);

  if (annotation_level > 1 && instream == stdin)
    {
      printf_unfiltered ("\n\032\032post-");
      printf_unfiltered (async_annotation_suffix);
      printf_unfiltered ("\n");
    }

  if (linebuffer == 0)
    {
      linelength = 80;
      linebuffer = (char *) xmalloc (linelength);
    }

  p = linebuffer;

  if (more_to_come)
    {
      strcpy (linebuffer, readline_input_state.linebuffer);
      p = readline_input_state.linebuffer_ptr;
      free (readline_input_state.linebuffer);
      more_to_come = 0;
      change_prompt = 1;
    }

#ifdef STOP_SIGNAL
  if (job_control)
    signal (STOP_SIGNAL, stop_sig);
#endif

  /* Make sure that all output has been output.  Some machines may let
     you get away with leaving out some of the gdb_flush, but not all.  */
  wrap_here ("");
  gdb_flush (gdb_stdout);
  gdb_flush (gdb_stderr);

  if (source_file_name != NULL)
    {
      ++source_line_number;
      sprintf (source_error,
	       "%s%s:%d: Error in sourced command file:\n",
	       source_pre_error,
	       source_file_name,
	       source_line_number);
      error_pre_print = source_error;
    }

  /* If we are in this case, then command_handler will call quit 
     and exit from gdb. */
  if (!rl || rl == (char *) EOF)
    {
      got_eof = 1;
      command_handler (0);
    }
  if (strlen (rl) + 1 + (p - linebuffer) > linelength)
    {
      linelength = strlen (rl) + 1 + (p - linebuffer);
      nline = (char *) xrealloc (linebuffer, linelength);
      p += nline - linebuffer;
      linebuffer = nline;
    }
  p1 = rl;
  /* Copy line.  Don't copy null at end.  (Leaves line alone
     if this was just a newline)  */
  while (*p1)
    *p++ = *p1++;

  free (rl);			/* Allocated in readline.  */

  if (p == linebuffer || *(p - 1) == '\\')
    {
      /* We come here also if the line entered is empty (just a 'return') */
      p--;			/* Put on top of '\'.  */

      if (*p == '\\')
	{
	  readline_input_state.linebuffer = savestring (linebuffer,
							strlen (linebuffer));
	  readline_input_state.linebuffer_ptr = p;

	  /* We will not invoke a execute_command if there is more
	     input expected to complete the command. So, we need to
	     print an empty prompt here. */
	  display_gdb_prompt ("");
	  more_to_come = 1;
	}
    }

#ifdef STOP_SIGNAL
  if (job_control)
    signal (STOP_SIGNAL, SIG_DFL);
#endif

#define SERVER_COMMAND_LENGTH 7
  server_command =
    (p - linebuffer > SERVER_COMMAND_LENGTH)
    && STREQN (linebuffer, "server ", SERVER_COMMAND_LENGTH);
  if (server_command)
    {
      /* Note that we don't set `line'.  Between this and the check in
         dont_repeat, this insures that repeating will still do the
         right thing.  */
      *p = '\0';
      command_handler (linebuffer + SERVER_COMMAND_LENGTH);
      display_gdb_prompt (0);
      return;
    }

  /* Do history expansion if that is wished.  */
  if (history_expansion_p && instream == stdin
      && ISATTY (instream))
    {
      char *history_value;
      int expanded;

      *p = '\0';		/* Insert null now.  */
      expanded = history_expand (linebuffer, &history_value);
      if (expanded)
	{
	  /* Print the changes.  */
	  printf_unfiltered ("%s\n", history_value);

	  /* If there was an error, call this function again.  */
	  if (expanded < 0)
	    {
	      free (history_value);
	      return;
	    }
	  if (strlen (history_value) > linelength)
	    {
	      linelength = strlen (history_value) + 1;
	      linebuffer = (char *) xrealloc (linebuffer, linelength);
	    }
	  strcpy (linebuffer, history_value);
	  p = linebuffer + strlen (linebuffer);
	  free (history_value);
	}
    }

  /* If we just got an empty line, and that is supposed
     to repeat the previous command, return the value in the
     global buffer.  */
  if (repeat && p == linebuffer && *p != '\\')
    {
      command_handler (line);
      display_gdb_prompt (0);
      return;
    }

  for (p1 = linebuffer; *p1 == ' ' || *p1 == '\t'; p1++);
  if (repeat && !*p1)
    {
      command_handler (line);
      display_gdb_prompt (0);
      return;
    }

  *p = 0;

  /* Add line to history if appropriate.  */
  if (instream == stdin
      && ISATTY (stdin) && *linebuffer)
    add_history (linebuffer);

  /* Note: lines consisting solely of comments are added to the command
     history.  This is useful when you type a command, and then
     realize you don't want to execute it quite yet.  You can comment
     out the command and then later fetch it from the value history
     and remove the '#'.  The kill ring is probably better, but some
     people are in the habit of commenting things out.  */
  if (*p1 == '#')
    *p1 = '\0';			/* Found a comment. */

  /* Save into global buffer if appropriate.  */
  if (repeat)
    {
      if (linelength > linesize)
	{
	  line = xrealloc (line, linelength);
	  linesize = linelength;
	}
      strcpy (line, linebuffer);
      if (!more_to_come)
	{
	  command_handler (line);
	  display_gdb_prompt (0);
	}
      return;
    }

  command_handler (linebuffer);
  display_gdb_prompt (0);
  return;
}

/* Does reading of input from terminal w/o the editing features
   provided by the readline library. */

/* NOTE: 1999-04-30 Asynchronous version of gdb_readline. gdb_readline
   will become obsolete when the event loop is made the default
   execution for gdb. */
static void
gdb_readline2 ()
{
  int c;
  char *result;
  int input_index = 0;
  int result_size = 80;

  result = (char *) xmalloc (result_size);

  /* We still need the while loop here, even though it would seem
     obvious to invoke gdb_readline2 at every character entered.  If
     not using the readline library, the terminal is in cooked mode,
     which sends the characters all at once. Poll will notice that the
     input fd has changed state only after enter is pressed. At this
     point we still need to fetch all the chars entered. */

  while (1)
    {
      /* Read from stdin if we are executing a user defined command.
         This is the right thing for prompt_for_continue, at least.  */
      c = fgetc (instream ? instream : stdin);

      if (c == EOF)
	{
	  if (input_index > 0)
	    /* The last line does not end with a newline.  Return it, and
	       if we are called again fgetc will still return EOF and
	       we'll return NULL then.  */
	    break;
	  free (result);
	  command_line_handler (0);
	}

      if (c == '\n')
#ifndef CRLF_SOURCE_FILES
	break;
#else
	{
	  if (input_index > 0 && result[input_index - 1] == '\r')
	    input_index--;
	  break;
	}
#endif

      result[input_index++] = c;
      while (input_index >= result_size)
	{
	  result_size *= 2;
	  result = (char *) xrealloc (result, result_size);
	}
    }

  result[input_index++] = '\0';
  command_line_handler (result);
}


/* Initialization of signal handlers and tokens.  There is a function
   handle_sig* for each of the signals GDB cares about. Specifically:
   SIGINT, SIGFPE, SIGQUIT, SIGTSTP, SIGHUP, SIGWINCH.  These
   functions are the actual signal handlers associated to the signals
   via calls to signal().  The only job for these functions is to
   enqueue the appropriate event/procedure with the event loop.  Such
   procedures are the old signal handlers. The event loop will take
   care of invoking the queued procedures to perform the usual tasks
   associated with the reception of the signal. */
/* NOTE: 1999-04-30 This is the asynchronous version of init_signals.
   init_signals will become obsolete as we move to have to event loop
   as the default for gdb. */
void
async_init_signals ()
{
  signal (SIGINT, handle_sigint);
  sigint_token =
    create_async_signal_handler ((async_handler_func *) async_request_quit, NULL);

  /* If SIGTRAP was set to SIG_IGN, then the SIG_IGN will get passed
     to the inferior and breakpoints will be ignored.  */
#ifdef SIGTRAP
  signal (SIGTRAP, SIG_DFL);
#endif

  /* If we initialize SIGQUIT to SIG_IGN, then the SIG_IGN will get
     passed to the inferior, which we don't want.  It would be
     possible to do a "signal (SIGQUIT, SIG_DFL)" after we fork, but
     on BSD4.3 systems using vfork, that can affect the
     GDB process as well as the inferior (the signal handling tables
     might be in memory, shared between the two).  Since we establish
     a handler for SIGQUIT, when we call exec it will set the signal
     to SIG_DFL for us.  */
  signal (SIGQUIT, handle_sigquit);
  sigquit_token =
    create_async_signal_handler ((async_handler_func *) async_do_nothing, NULL);
#ifdef SIGHUP
  if (signal (SIGHUP, handle_sighup) != SIG_IGN)
    sighup_token =
      create_async_signal_handler ((async_handler_func *) async_disconnect, NULL);
  else
    sighup_token =
      create_async_signal_handler ((async_handler_func *) async_do_nothing, NULL);
#endif
  signal (SIGFPE, handle_sigfpe);
  sigfpe_token =
    create_async_signal_handler ((async_handler_func *) async_float_handler, NULL);

#if defined(SIGWINCH) && defined(SIGWINCH_HANDLER)
  signal (SIGWINCH, handle_sigwinch);
  sigwinch_token =
    create_async_signal_handler ((async_handler_func *) SIGWINCH_HANDLER, NULL);
#endif
}

/* Tell the event loop what to do if SIGINT is received. 
   See event-signal.c. */
void 
handle_sigint (sig)
     int sig;
{
  signal (sig, handle_sigint);

  /* If immediate_quit is set, we go ahead and process the SIGINT right
     away, even if we usually would defer this to the event loop. The
     assumption here is that it is safe to process ^C immediately if
     immediate_quit is set. If we didn't, SIGINT would be really
     processed only the next time through the event loop.  To get to
     that point, though, the command that we want to interrupt needs to
     finish first, which is unacceptable. */
  if (immediate_quit)
    async_request_quit ();
  else
    /* If immediate quit is not set, we process SIGINT the next time
       through the loop, which is fine. */
    mark_async_signal_handler (sigint_token);
}

/* Do the quit. All the checks have been done by the caller. */
void 
async_request_quit ()
{
  quit_flag = 1;
#ifdef REQUEST_QUIT
  REQUEST_QUIT;
#else
  quit ();
#endif
}

/* Tell the event loop what to do if SIGQUIT is received. 
   See event-signal.c. */
void 
handle_sigquit (sig)
     int sig;
{
  mark_async_signal_handler (sigquit_token);
  signal (sig, handle_sigquit);
}

/* Called by the event loop in response to a SIGQUIT. */
void 
async_do_nothing ()
{
  /* Empty function body. */
}

#ifdef SIGHUP
/* Tell the event loop what to do if SIGHUP is received. 
   See event-signal.c. */
void 
handle_sighup (sig)
     int sig;
{
  mark_async_signal_handler (sighup_token);
  signal (sig, handle_sighup);
}

/* Called by the event loop to process a SIGHUP. */
void 
async_disconnect ()
{
  catch_errors (quit_cover, NULL,
		"Could not kill the program being debugged",
		RETURN_MASK_ALL);
  signal (SIGHUP, SIG_DFL);	/*FIXME: ??????????? */
  kill (getpid (), SIGHUP);
}
#endif

/* Tell the event loop what to do if SIGFPE is received. 
   See event-signal.c. */
void 
handle_sigfpe (sig)
     int sig;
{
  mark_async_signal_handler (sigfpe_token);
  signal (sig, handle_sigfpe);
}

/* Event loop will call this functin to process a SIGFPE. */
void 
async_float_handler ()
{
  /* This message is based on ANSI C, section 4.7. Note that integer
     divide by zero causes this, so "float" is a misnomer. */
  error ("Erroneous arithmetic operation.");
}

/* Tell the event loop what to do if SIGWINCH is received. 
   See event-signal.c. */
#if defined(SIGWINCH) && defined(SIGWINCH_HANDLER)
void 
handle_sigwinch (sig)
     int sig;
{
  mark_async_signal_handler (sigwinch_token);
  signal (sig, handle_sigwinch);
}
#endif


/* Called by do_setshow_command.  */
/* ARGSUSED */
void
set_async_editing_command (args, from_tty, c)
     char *args;
     int from_tty;
     struct cmd_list_element *c;
{
  change_line_handler ();
}

/* Called by do_setshow_command.  */
/* ARGSUSED */
void
set_async_annotation_level (args, from_tty, c)
     char *args;
     int from_tty;
     struct cmd_list_element *c;
{
  change_annotation_level ();
}

/* Called by do_setshow_command.  */
/* ARGSUSED */
void
set_async_prompt (args, from_tty, c)
     char *args;
     int from_tty;
     struct cmd_list_element *c;
{
  PROMPT (0) = savestring (new_async_prompt, strlen (new_async_prompt));
}




