/* Top level for GDB, the GNU debugger.
   Copyright (C) 1986, 1987, 1988, 1989 Free Software Foundation, Inc.

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
#include "defs.h"
#include "command.h"
#include "param.h"

#ifdef USG
#include <sys/types.h>
#include <unistd.h>
#endif

#include <sys/file.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/stat.h>

#ifdef SET_STACK_LIMIT_HUGE
#include <sys/time.h>
#include <sys/resource.h>
#include <ctype.h>

int original_stack_limit;
#endif

/* If this definition isn't overridden by the header files, assume
   that isatty and fileno exist on this system.  */
#ifndef ISATTY
#define ISATTY(FP)	(isatty (fileno (FP)))
#endif

extern void free ();

/* Version number of GDB, as a string.  */

extern char *version;

/*
 * Declare all cmd_list_element's
 */

/* Chain containing all defined commands.  */

struct cmd_list_element *cmdlist;

/* Chain containing all defined info subcommands.  */

struct cmd_list_element *infolist;

/* Chain containing all defined enable subcommands. */

struct cmd_list_element *enablelist;

/* Chain containing all defined disable subcommands. */

struct cmd_list_element *disablelist;

/* Chain containing all defined delete subcommands. */

struct cmd_list_element *deletelist;

/* Chain containing all defined "enable breakpoint" subcommands. */

struct cmd_list_element *enablebreaklist;

/* Chain containing all defined set subcommands */

struct cmd_list_element *setlist;

/* Chain containing all defined \"set history\".  */

struct cmd_list_element *sethistlist;

/* Chain containing all defined \"unset history\".  */

struct cmd_list_element *unsethistlist;

/* stdio stream that command input is being read from.  */

FILE *instream;

/* Current working directory.  */

char *current_directory;

/* The directory name is actually stored here (usually).  */
static char dirbuf[MAXPATHLEN];

/* The number of lines on a page, and the number of spaces
   in a line.  */
int linesize, pagesize;

/* Nonzero if we should refrain from using an X window.  */

int inhibit_windows = 0;

/* Function to call before reading a command, if nonzero.
   The function receives two args: an input stream,
   and a prompt string.  */
   
void (*window_hook) ();

extern int frame_file_full_name;
int xgdb_verbose;

void free_command_lines ();
char *gdb_readline ();
char *command_line_input ();
static void initialize_main ();
static void initialize_cmd_lists ();
void command_loop ();
static void source_command ();
static void print_gdb_version ();
static void float_handler ();
static void cd_command ();

char *getenv ();

/* gdb prints this when reading a command interactively */
static char *prompt;

/* Buffer used for reading command lines, and the size
   allocated for it so far.  */

char *line;
int linesize;


/* Signal to catch ^Z typed while reading a command: SIGTSTP or SIGCONT.  */

#ifndef STOP_SIGNAL
#ifdef SIGTSTP
#define STOP_SIGNAL SIGTSTP
#endif
#endif

/* This is how `error' returns to command level.  */

jmp_buf to_top_level;

void
return_to_top_level ()
{
  quit_flag = 0;
  immediate_quit = 0;
  clear_breakpoint_commands ();
  clear_momentary_breakpoints ();
  disable_current_display ();
  do_cleanups (0);
  longjmp (to_top_level, 1);
}

/* Call FUNC with arg ARG, catching any errors.
   If there is no error, return the value returned by FUNC.
   If there is an error, return zero after printing ERRSTRING
    (which is in addition to the specific error message already printed).  */

int
catch_errors (func, arg, errstring)
     int (*func) ();
     int arg;
     char *errstring;
{
  jmp_buf saved;
  int val;
  struct cleanup *saved_cleanup_chain;

  saved_cleanup_chain = save_cleanups ();

  bcopy (to_top_level, saved, sizeof (jmp_buf));

  if (setjmp (to_top_level) == 0)
    val = (*func) (arg);
  else
    {
      fprintf (stderr, "%s\n", errstring);
      val = 0;
    }

  restore_cleanups (saved_cleanup_chain);

  bcopy (saved, to_top_level, sizeof (jmp_buf));
  return val;
}

/* Handler for SIGHUP.  */

static void
disconnect ()
{
  kill_inferior_fast ();
  signal (SIGHUP, SIG_DFL);
  kill (getpid (), SIGHUP);
}

/* Clean up on error during a "source" command (or execution of a
   user-defined command).
   Close the file opened by the command
   and restore the previous input stream.  */

static void
source_cleanup (stream)
     FILE *stream;
{
  /* Instream may be 0; set to it when executing user-defined command. */
  if (instream)
    fclose (instream);
  instream = stream;
}


int
main (argc, argv, envp)
     int argc;
     char **argv;
     char **envp;
{
  int count;
  int inhibit_gdbinit = 0;
  int quiet = 0;
  int batch = 0;
  register int i;

#if defined (ALIGN_STACK_ON_STARTUP)
  i = (int) &count & 0x3;
  if (i != 0)
    alloca (4 - i);
#endif

  quit_flag = 0;
  linesize = 100;
  line = (char *) xmalloc (linesize);
  instream = stdin;

  getwd (dirbuf);
  current_directory = dirbuf;

#ifdef SET_STACK_LIMIT_HUGE
  {
    struct rlimit rlim;

    /* Set the stack limit huge so that alloca (particularly stringtab
     * in dbxread.c) does not fail. */
    getrlimit (RLIMIT_STACK, &rlim);
    original_stack_limit = rlim.rlim_cur;
    rlim.rlim_cur = rlim.rlim_max;
    setrlimit (RLIMIT_STACK, &rlim);
  }
#endif /* SET_STACK_LIMIT_HUGE */

  /* Look for flag arguments.  */

  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "-q") || !strcmp (argv[i], "-quiet"))
	quiet = 1;
      else if (!strcmp (argv[i], "-nx"))
	inhibit_gdbinit = 1;
      else if (!strcmp (argv[i], "-nw"))
	inhibit_windows = 1;
      else if (!strcmp (argv[i], "-batch"))
	batch = 1, quiet = 1;
      else if (!strcmp (argv[i], "-fullname"))
	frame_file_full_name = 1;
      else if (!strcmp (argv[i], "-xgdb_verbose"))
	xgdb_verbose = 1;
      /* -help: print a summary of command line switches.  */
      else if (!strcmp (argv[i], "-help"))
	{
	  fputs ("\
This is GDB, the GNU debugger.  Use the command\n\
    gdb [options] [executable [core-file]]\n\
to enter the debugger.\n\
\n\
Options available are:\n\
  -help             Print this message.\n\
  -quiet            Do not print version number on startup.\n\
  -fullname         Output information used by emacs-GDB interface.\n\
  -batch            Exit after processing options.\n\
  -nx               Do not read .gdbinit file.\n\
  -tty TTY          Use TTY for input/output by the program being debugged.\n\
  -cd DIR           Change current directory to DIR.\n\
  -directory DIR    Search for source files in DIR.\n\
  -command FILE     Execute GDB commands from FILE.\n\
  -symbols SYMFILE  Read symbols from SYMFILE.\n\
  -exec EXECFILE    Use EXECFILE as the executable.\n\
  -se FILE          Use FILE as symbol file and executable file.\n\
  -core COREFILE    Analyze the core dump COREFILE.\n\
\n\
For more information, type \"help\" from within GDB, or consult the\n\
GDB manual (available as on-line info or a printed manual).\n", stderr);
	  /* Exiting after printing this message seems like
	     the most useful thing to do.  */
	  exit (0);
	}
      else if (argv[i][0] == '-')
	/* Other options take arguments, so don't confuse an
	   argument with an option.  */
	i++;
    }

  /* Run the init function of each source file */

  initialize_cmd_lists ();	/* This needs to be done first */
  initialize_all_files ();
  initialize_main ();		/* But that omits this file!  Do it now */
  initialize_signals ();

  if (!quiet)
    print_gdb_version ();

  /* Process the command line arguments.  */

  count = 0;
  for (i = 1; i < argc; i++)
    {
      register char *arg = argv[i];
      /* Args starting with - say what to do with the following arg
	 as a filename.  */
      if (arg[0] == '-')
	{
	  extern void exec_file_command (), symbol_file_command ();
	  extern void core_file_command (), directory_command ();
	  extern void tty_command ();

	  if (!strcmp (arg, "-q") || !strcmp (arg, "-nx")
	      || !strcmp (arg, "-quiet") || !strcmp (arg, "-batch")
	      || !strcmp (arg, "-fullname") || !strcmp (arg, "-nw")
	      || !strcmp (arg, "-xgdb_verbose")
	      || !strcmp (arg, "-help"))
	    /* Already processed above */
	    continue;

	  if (++i == argc)
	    fprintf (stderr, "No argument follows \"%s\".\n", arg);
	  if (!setjmp (to_top_level))
	    {
	      /* -s foo: get syms from foo.  -e foo: execute foo.
		 -se foo: do both with foo.  -c foo: use foo as core dump.  */
	      if (!strcmp (arg, "-se"))
		{
		  exec_file_command (argv[i], !batch);
		  symbol_file_command (argv[i], !batch);
		}
	      else if (!strcmp (arg, "-s") || !strcmp (arg, "-symbols"))
		symbol_file_command (argv[i], !batch);
	      else if (!strcmp (arg, "-e") || !strcmp (arg, "-exec"))
		exec_file_command (argv[i], !batch);
	      else if (!strcmp (arg, "-c") || !strcmp (arg, "-core"))
		core_file_command (argv[i], !batch);
	      /* -x foo: execute commands from foo.  */
	      else if (!strcmp (arg, "-x") || !strcmp (arg, "-command")
		       || !strcmp (arg, "-commands"))
		source_command (argv[i]);
	      /* -d foo: add directory `foo' to source-file directory
		         search-list */
	      else if (!strcmp (arg, "-d") || !strcmp (arg, "-dir")
		       || !strcmp (arg, "-directory"))
		directory_command (argv[i], 0);
	      /* -cd FOO: specify current directory as FOO.
		 GDB remembers the precise string FOO as the dirname.  */
	      else if (!strcmp (arg, "-cd"))
		{
		  cd_command (argv[i], 0);
		  init_source_path ();
		}
	      /* -t /def/ttyp1: use /dev/ttyp1 for inferior I/O.  */
	      else if (!strcmp (arg, "-t") || !strcmp (arg, "-tty"))
		tty_command (argv[i], 0);

	      else
		error ("Unknown command-line switch: \"%s\"\n", arg);
	    }
	}
      else
	{
	  /* Args not thus accounted for
	     are treated as, first, the symbol/executable file
	     and, second, the core dump file.  */
	  count++;
	  if (!setjmp (to_top_level))
	    switch (count)
	      {
	      case 1:
		exec_file_command (arg, !batch);
		symbol_file_command (arg, !batch);
		break;

	      case 2:
		core_file_command (arg, !batch);
		break;

	      case 3:
		fprintf (stderr, "Excess command line args ignored. (%s%s)\n",
			 arg, (i == argc - 1) ? "" : " ...");
	      }
	}
    }

  {
    struct stat homebuf, cwdbuf;
    char *homedir, *homeinit;

    /* Read init file, if it exists in home directory  */
    homedir = getenv ("HOME");
    if (homedir)
      {
	homeinit = (char *) alloca (strlen (getenv ("HOME")) + 10);
	strcpy (homeinit, getenv ("HOME"));
	strcat (homeinit, "/.gdbinit");
	if (!inhibit_gdbinit && access (homeinit, R_OK) == 0)
	  if (!setjmp (to_top_level))
	    source_command (homeinit);

	/* Do stats; no need to do them elsewhere since we'll only
	   need them if homedir is set.  Make sure that they are
	   zero in case one of them fails (guarantees that they
	   won't match if either exits).  */
	
	bzero (&homebuf, sizeof (struct stat));
	bzero (&cwdbuf, sizeof (struct stat));
	
	stat (homeinit, &homebuf);
	stat ("./.gdbinit", &cwdbuf); /* We'll only need this if
					 homedir was set.  */
      }
    
    /* Read the input file in the current directory, *if* it isn't
       the same file (it should exist, also).  */

    if (!homedir
	|| bcmp ((char *) &homebuf,
		 (char *) &cwdbuf,
		 sizeof (struct stat)))
      if (!inhibit_gdbinit && access (".gdbinit", R_OK) == 0)
	if (!setjmp (to_top_level))
	  source_command (".gdbinit");
  }

  if (batch)
    {
#if 0
      fatal ("Attempt to read commands from stdin in batch mode.");
#endif
      /* We have hit the end of the batch file.  */
      exit (0);
    }

  if (!quiet)
    printf ("Type \"help\" for a list of commands.\n");

  /* The command loop.  */

  while (1)
    {
      if (!setjmp (to_top_level))
	command_loop ();
      clearerr (stdin);		/* Don't get hung if C-d is typed.  */
    }
}

/* Execute the line P as a command.
   Pass FROM_TTY as second argument to the defining function.  */

void
execute_command (p, from_tty)
     char *p;
     int from_tty;
{
  register struct cmd_list_element *c;
  register struct command_line *cmdlines;

  free_all_values ();
  while (*p == ' ' || *p == '\t') p++;
  if (*p)
    {
      c = lookup_cmd (&p, cmdlist, "", 0, 1);
      if (c->function == 0)
	error ("That is not a command, just a help topic.");
      else if (c->class == (int) class_user)
	{
	  struct cleanup *old_chain;
	  
	  if (*p)
	    error ("User-defined commands cannot take arguments.");
	  cmdlines = (struct command_line *) c->function;
	  if (cmdlines == (struct command_line *) 0)
	    /* Null command */
	    return;

	  /* Set the instream to 0, indicating execution of a
	     user-defined function.  */
	  old_chain =  make_cleanup (source_cleanup, instream);
	  instream = (FILE *) 0;
	  while (cmdlines)
	    {
	      execute_command (cmdlines->line, 0);
	      cmdlines = cmdlines->next;
	    }
	  do_cleanups (old_chain);
	}
      else
	/* Pass null arg rather than an empty one.  */
	(*c->function) (*p ? p : 0, from_tty);
    }
}

static void
do_nothing ()
{
}

/* Read commands from `instream' and execute them
   until end of file.  */
void
command_loop ()
{
  struct cleanup *old_chain;
  while (!feof (instream))
    {
      if (window_hook && instream == stdin)
	(*window_hook) (instream, prompt);

      quit_flag = 0;
      if (instream == stdin && ISATTY (stdin))
	reinitialize_more_filter ();
      old_chain = make_cleanup (do_nothing, 0);
      execute_command (command_line_input (instream == stdin ? prompt : 0,
				      instream == stdin),
		       instream == stdin);
      /* Do any commands attached to breakpoint we stopped at.  */
      do_breakpoint_commands ();
      do_cleanups (old_chain);
    }
}

/* Commands call this if they do not want to be repeated by null lines.  */

void
dont_repeat ()
{
  /* If we aren't reading from standard input, we are saving the last
     thing read from stdin in line and don't want to delete it.  Null lines
     won't repeat here in any case.  */
  if (instream == stdin)
    *line = 0;
}

/* Read a line from the stream "instream" without command line editing.

   It prints PROMPT once at the start.

   If RETURN_RESULT is set it allocates
   space for whatever the user types and returns the result.
   If not, it just discards what the user types.  */
char *
gdb_readline (prompt, return_result)
     char *prompt;
     int return_result;
{
  int c;
  char *result;
  int input_index = 0;
  int result_size = 80;

  if (prompt)
    {
      printf (prompt);
      fflush (stdout);
    }
  
  if (return_result)
    result = (char *) xmalloc (result_size);

  while (1)
    {
      c = fgetc (instream);
      if (c == -1 || c == '\n')
	break;
      if (return_result)
	{
	  result[input_index++] = c;
	  while (input_index >= result_size)
	    {
	      result_size *= 2;
	      result = (char *) xrealloc (result, result_size);
	    }
	}
    }
  if (return_result)
    {
      result[input_index++] = '\0';
      return result;
    }
  else
    return (char *) 0;
}

/* Declaration for fancy readline with command line editing.  */
char *readline ();

/* Variables which control command line editing and history
   substitution.  These variables are given default values at the end
   of this file.  */
static int command_editing_p;
static int history_expansion_p;
static int write_history_p;
static int history_size;
static char *history_filename;

/* Variables which are necessary for fancy command line editing.  */
char *gdb_completer_word_break_characters =
  " \t\n!@#$%^&*()-+=|~`}{[]\"';:?/>.<,";

/* Functions that are used as part of the fancy command line editing.  */

/* Generate symbol names one by one for the completer.  If STATE is
   zero, then we need to initialize, otherwise the initialization has
   already taken place.  TEXT is what we expect the symbol to start
   with.  RL_LINE_BUFFER is available to be looked at; it contains the
   entire text of the line.  RL_POINT is the offset in that line of
   the cursor.  You should pretend that the line ends at RL_POINT.  */
char *
symbol_completion_function (text, state)
     char *text;
     int state;
{
  char **make_symbol_completion_list ();
  static char **list = (char **)NULL;
  static int index;
  char *output;
  extern char *rl_line_buffer;
  extern int rl_point;
  char *tmp_command, *p;
  struct cmd_list_element *c, *result_list;

  if (!state)
    {
      /* Free the storage used by LIST, but not by the strings inside.  This is
	 because rl_complete_internal () frees the strings. */
      if (list)
	free (list);
      list = 0;
      index = 0;

      /* Decide whether to complete on a list of gdb commands or on
	 symbols.  */
      tmp_command = (char *) alloca (rl_point + 1);
      p = tmp_command;
      
      strncpy (tmp_command, rl_line_buffer, rl_point);
      tmp_command[rl_point] = '\0';

      if (rl_point == 0)
	{
	  /* An empty line we want to consider ambiguous; that is,
	     it could be any command.  */
	  c = (struct cmd_list_element *) -1;
	  result_list = 0;
	}
      else
	c = lookup_cmd_1 (&p, cmdlist, &result_list, 1);

      /* Move p up to the next interesting thing.  */
      while (*p == ' ' || *p == '\t')
	p++;

      if (!c)
	/* He's typed something unrecognizable.  Sigh.  */
	list = (char **) 0;
      else if (c == (struct cmd_list_element *) -1)
	{
	  if (p + strlen(text) != tmp_command + rl_point)
	    error ("Unrecognized command.");
	  
	  /* He's typed something ambiguous.  This is easier.  */
	  if (result_list)
	    list = complete_on_cmdlist (*result_list->prefixlist, text);
	  else
	    list = complete_on_cmdlist (cmdlist, text);
	}
      else
	{
	  /* If we've gotten this far, gdb has recognized a full
	     command.  There are several possibilities:

	     1) We need to complete on the command.
	     2) We need to complete on the possibilities coming after
	     the command.
	     2) We need to complete the text of what comes after the
	     command.   */

	  if (!*p && *text)
	    /* Always (might be longer versions of thie command).  */
	    list = complete_on_cmdlist (result_list, text);
	  else if (!*p && !*text)
	    {
	      if (c->prefixlist)
		list = complete_on_cmdlist (*c->prefixlist, "");
	      else
		list = make_symbol_completion_list ("");
	    }
	  else
	    {
	      if (c->prefixlist && !c->allow_unknown)
		{
		  *p = '\0';
		  error ("\"%s\" command requires a subcommand.",
			 tmp_command);
		}
	      else
		list = make_symbol_completion_list (text);
	    }
	}
    }

  /* If the debugged program wasn't compiled with symbols, or if we're
     clearly completing on a command and no command matches, return
     NULL.  */
  if (!list)
    return ((char *)NULL);

  output = list[index];
  if (output)
    index++;

  return (output);
}

#ifdef STOP_SIGNAL
static void
stop_sig ()
{
#if STOP_SIGNAL == SIGTSTP
  signal (SIGTSTP, SIG_DFL);
  sigsetmask (0);
  kill (getpid (), SIGTSTP);
  signal (SIGTSTP, stop_sig);
#else
  signal (STOP_SIGNAL, stop_sig);
#endif
  printf ("%s", prompt);
  fflush (stdout);

  /* Forget about any previous command -- null line now will do nothing.  */
  dont_repeat ();
}
#endif /* STOP_SIGNAL */

#if 0
Writing the history file upon a terminating signal is not useful,
  because the info is rarely relevant and is in the core dump anyway.
  It is an annoyance to have the file cluttering up the place.
/* The list of signals that would terminate us if not caught.
   We catch them, but just so that we can write the history file,
   and so forth. */
int terminating_signals[] = {
  SIGHUP, SIGINT, SIGILL, SIGTRAP, SIGIOT,
  SIGEMT, SIGFPE, SIGKILL, SIGBUS, SIGSEGV, SIGSYS,
  SIGPIPE, SIGALRM, SIGTERM,
#ifdef SIGXCPU
  SIGXCPU,
#endif
#ifdef SIGXFSZ
  SIGXFSZ,
#endif
#ifdef SIGVTALRM
  SIGVTALRM,
#endif
#ifdef SIGPROF
  SIGPROF,
#endif
#ifdef SIGLOST
  SIGLOST,
#endif
#ifdef SIGUSR1
  SIGUSR1, SIGUSR2
#endif
    };

#define TERMSIGS_LENGTH (sizeof (terminating_signals) / sizeof (int))

static void
catch_termination (sig)
     int sig;
{
  /* We are probably here because GDB has a bug.  Write out the history
     so that we might have a better chance of reproducing it.  */
  /* Tell the user what we are doing so he can delete the file if
     it is unwanted.  */
  write_history (history_filename);
  printf ("\n%s written.\n", history_filename);
  signal (sig, SIG_DFL);
  kill (getpid (), sig);
}
#endif

/* Initialize signal handlers. */
initialize_signals ()
{
  extern void request_quit ();
#if 0
  register int i;

  for (i = 0; i < TERMSIGS_LENGTH; i++)
    signal (terminating_signals[i], catch_termination);
#endif

  signal (SIGINT, request_quit);

  /* If we initialize SIGQUIT to SIG_IGN, then the SIG_IGN will get
     passed to the inferior, which we don't want.  It would be
     possible to do a "signal (SIGQUIT, SIG_DFL)" after we fork, but
     on BSD4.3 systems using vfork, that will (apparently) affect the
     GDB process as well as the inferior (the signal handling tables
     being shared between the two, apparently).  Since we establish
     a handler for SIGQUIT, when we call exec it will set the signal
     to SIG_DFL for us.  */
  signal (SIGQUIT, do_nothing);
  if (signal (SIGHUP, do_nothing) != SIG_IGN)
    signal (SIGHUP, disconnect);
  signal (SIGFPE, float_handler);
}

/* Read one line from the command input stream `instream'
   into the local static buffer `linebuffer' (whose current length
   is `linelength').
   The buffer is made bigger as necessary.
   Returns the address of the start of the line.

   *If* the instream == stdin & stdin is a terminal, the line read
   is copied into the file line saver (global var char *line,
   length linesize) so that it can be duplicated.

   This routine either uses fancy command line editing or
   simple input as the user has requested.  */

char *
command_line_input (prompt, repeat)
     char *prompt;
     int repeat;
{
  static char *linebuffer = 0;
  static int linelength = 0;
  register char *p;
  register char *p1, *rl;
  char *local_prompt = prompt;
  register int c;
  char *nline;

  if (linebuffer == 0)
    {
      linelength = 80;
      linebuffer = (char *) xmalloc (linelength);
    }

  p = linebuffer;

  /* Control-C quits instantly if typed while in this loop
     since it should not wait until the user types a newline.  */
  immediate_quit++;
#ifdef STOP_SIGNAL
  signal (STOP_SIGNAL, stop_sig);
#endif

  while (1)
    {
      /* Don't use fancy stuff if not talking to stdin.  */
      if (command_editing_p && instream == stdin
	  && ISATTY (instream))
	rl = readline (local_prompt);
      else
	rl = gdb_readline (local_prompt, 1);

      if (!rl || rl == (char *) EOF) break;
      if (strlen(rl) + 1 + (p - linebuffer) > linelength)
	{
	  linelength = strlen(rl) + 1 + (p - linebuffer);
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

      if (p == linebuffer || *(p - 1) != '\\')
	break;

      p--;			/* Put on top of '\'.  */
      local_prompt = (char *) 0;
  }

#ifdef STOP_SIGNAL
  signal (SIGTSTP, SIG_DFL);
#endif
  immediate_quit--;

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
	  printf ("%s\n", history_value);

	  /* If there was an error, call this function again.  */
	  if (expanded < 0)
	    {
	      free (history_value);
	      return command_line_input (prompt, repeat);
	    }
	  if (strlen (history_value) > linelength)
	    {
	      linelength = strlen (history_value) + 1;
	      linebuffer = (char *) xrealloc (linebuffer, linelength);
	    }
	  strcpy (linebuffer, history_value);
	  p = linebuffer + strlen(linebuffer);
	  free (history_value);
	}
    }

  /* If we just got an empty line, and that is supposed
     to repeat the previous command, return the value in the
     global buffer.  */
  if (repeat)
    {
      if (p == linebuffer)
	return line;
      p1 = linebuffer;
      while (*p1 == ' ' || *p1 == '\t')
	p1++;
      if (!*p1)
	return line;
    }

  *p = 0;

  /* Add line to history if appropriate.  */
  if (instream == stdin
      && ISATTY (stdin) && *linebuffer)
    add_history (linebuffer);

  /* If line is a comment, clear it out.  */
  /* Note:  comments are added to the command history.
     This is useful when you type a command, and then realize
     you don't want to execute it quite yet.  You can comment out the
     command and then later fetch it from the value history and
     remove the '#'.  */
  p1 = linebuffer;
  while ((c = *p1) == ' ' || c == '\t') p1++;
  if (c == '#')
    *linebuffer = 0;

  /* Save into global buffer if appropriate.  */
  if (repeat)
    {
      if (linelength > linesize)
	{
	  line = xrealloc (line, linelength);
	  linesize = linelength;
	}
      strcpy (line, linebuffer);
      return line;
    }

  return linebuffer;
}

/* Read lines from the input stream
   and accumulate them in a chain of struct command_line's
   which is then returned.  */

struct command_line *
read_command_lines ()
{
  struct command_line *first = 0;
  register struct command_line *next, *tail = 0;
  register char *p, *p1;
  struct cleanup *old_chain = 0;

  while (1)
    {
      dont_repeat ();
      p = command_line_input (0, instream == stdin);
      /* Remove leading and trailing blanks.  */
      while (*p == ' ' || *p == '\t') p++;
      p1 = p + strlen (p);
      while (p1 != p && (p1[-1] == ' ' || p1[-1] == '\t')) p1--;

      /* Is this "end"?  */
      if (p1 - p == 3 && !strncmp (p, "end", 3))
	break;

      /* No => add this line to the chain of command lines.  */
      next = (struct command_line *) xmalloc (sizeof (struct command_line));
      next->line = savestring (p, p1 - p);
      next->next = 0;
      if (tail)
	{
	  tail->next = next;
	}
      else
	{
	  /* We just read the first line.
	     From now on, arrange to throw away the lines we have
	     if we quit or get an error while inside this function.  */
	  first = next;
	  old_chain = make_cleanup (free_command_lines, &first);
	}
      tail = next;
    }

  dont_repeat ();

  /* Now we are about to return the chain to our caller,
     so freeing it becomes his responsibility.  */
  if (first)
    discard_cleanups (old_chain);
  return first;
}

/* Free a chain of struct command_line's.  */

void
free_command_lines (lptr)
      struct command_line **lptr;
{
  register struct command_line *l = *lptr;
  register struct command_line *next;

  while (l)
    {
      next = l->next;
      free (l->line);
      free (l);
      l = next;
    }
}

/* Add an element to the list of info subcommands.  */

void
add_info (name, fun, doc)
     char *name;
     void (*fun) ();
     char *doc;
{
  add_cmd (name, no_class, fun, doc, &infolist);
}

/* Add an alias to the list of info subcommands.  */

void
add_info_alias (name, oldname, abbrev_flag)
     char *name;
     char *oldname;
     int abbrev_flag;
{
  add_alias_cmd (name, oldname, 0, abbrev_flag, &infolist);
}

/* The "info" command is defined as a prefix, with allow_unknown = 0.
   Therefore, its own definition is called only for "info" with no args.  */

static void
info_command ()
{
  printf ("\"info\" must be followed by the name of an info command.\n");
  help_list (infolist, "info ", -1, stdout);
}

/* Add an element to the list of commands.  */

void
add_com (name, class, fun, doc)
     char *name;
     int class;
     void (*fun) ();
     char *doc;
{
  add_cmd (name, class, fun, doc, &cmdlist);
}

/* Add an alias or abbreviation command to the list of commands.  */

void
add_com_alias (name, oldname, class, abbrev_flag)
     char *name;
     char *oldname;
     int class;
     int abbrev_flag;
{
  add_alias_cmd (name, oldname, class, abbrev_flag, &cmdlist);
}

void
error_no_arg (why)
     char *why;
{
  error ("Argument required (%s).", why);
}

static void
help_command (command, from_tty)
     char *command;
     int from_tty; /* Ignored */
{
  help_cmd (command, stdout);
}

static void
validate_comname (comname)
     char *comname;
{
  register char *p;

  if (comname == 0)
    error_no_arg ("name of command to define");

  p = comname;
  while (*p)
    {
      if (!(*p >= 'A' && *p <= 'Z')
	  && !(*p >= 'a' && *p <= 'z')
	  && !(*p >= '0' && *p <= '9')
	  && *p != '-')
	error ("Junk in argument list: \"%s\"", p);
      p++;
    }
}

static void
define_command (comname, from_tty)
     char *comname;
     int from_tty;
{
  register struct command_line *cmds;
  register struct cmd_list_element *c;
  char *tem = comname;

  validate_comname (comname);

  c = lookup_cmd (&tem, cmdlist, "", -1, 1);
  if (c)
    {
      if (c->class == (int) class_user || c->class == (int) class_alias)
	tem = "Redefine command \"%s\"? ";
      else
	tem = "Really redefine built-in command \"%s\"? ";
      if (!query (tem, comname))
	error ("Command \"%s\" not redefined.", comname);
    }

  if (from_tty)
    {
      printf ("Type commands for definition of \"%s\".\n\
End with a line saying just \"end\".\n", comname);
      fflush (stdout);
    }
  comname = savestring (comname, strlen (comname));

  cmds = read_command_lines ();

  if (c && c->class == (int) class_user)
    free_command_lines (&c->function);

  add_com (comname, class_user, cmds,
	   (c && c->class == (int) class_user)
	   ? c->doc : savestring ("User-defined.", 13));
}

static void
document_command (comname, from_tty)
     char *comname;
     int from_tty;
{
  struct command_line *doclines;
  register struct cmd_list_element *c;
  char *tem = comname;

  validate_comname (comname);

  c = lookup_cmd (&tem, cmdlist, "", 0, 1);

  if (c->class != (int) class_user)
    error ("Command \"%s\" is built-in.", comname);

  if (from_tty)
    printf ("Type documentation for \"%s\".\n\
End with a line saying just \"end\".\n", comname);

  doclines = read_command_lines ();

  if (c->doc) free (c->doc);

  {
    register struct command_line *cl1;
    register int len = 0;

    for (cl1 = doclines; cl1; cl1 = cl1->next)
      len += strlen (cl1->line) + 1;

    c->doc = (char *) xmalloc (len + 1);
    *c->doc = 0;

    for (cl1 = doclines; cl1; cl1 = cl1->next)
      {
	strcat (c->doc, cl1->line);
	if (cl1->next)
	  strcat (c->doc, "\n");
      }
  }

  free_command_lines (&doclines);
}

static void
print_gdb_version ()
{
  printf ("GDB %s, Copyright (C) 1989 Free Software Foundation, Inc.\n\
There is ABSOLUTELY NO WARRANTY for GDB; type \"info warranty\" for details.\n\
GDB is free software and you are welcome to distribute copies of it\n\
 under certain conditions; type \"info copying\" to see the conditions.\n",
	  version);
}

static void
version_info ()
{
  immediate_quit++;
  print_gdb_version ();
  immediate_quit--;
}

/* xgdb calls this to reprint the usual GDB prompt.  */

void
print_prompt ()
{
  printf ("%s", prompt);
  fflush (stdout);
}

/* Command to specify a prompt string instead of "(gdb) ".  */

static void
set_prompt_command (text)
     char *text;
{
  char *p, *q;
  register int c;
  char *new;

  if (text == 0)
    error_no_arg ("string to which to set prompt");

  new = (char *) xmalloc (strlen (text) + 2);
  p = text; q = new;
  while (c = *p++)
    {
      if (c == '\\')
	{
	  /* \ at end of argument is used after spaces
	     so they won't be lost.  */
	  if (*p == 0)
	    break;
	  c = parse_escape (&p);
	  if (c == 0)
	    break; /* C loses */
	  else if (c > 0)
	    *q++ = c;
	}
      else
	*q++ = c;
    }
  if (*(p - 1) != '\\')
    *q++ = ' ';
  *q++ = '\0';
  new = (char *) xrealloc (new, q - new);
  free (prompt);
  prompt = new;
}

static void
quit_command ()
{
  if (have_inferior_p ())
    {
      if (query ("The program is running.  Quit anyway? "))
	{
	  /* Prevent any warning message from reopen_exec_file, in case
	     we have a core file that's inconsistent with the exec file.  */
	  exec_file_command (0, 0);
	  kill_inferior ();
	}
      else
	error ("Not confirmed.");
    }
  /* Save the history information if it is appropriate to do so.  */
  if (write_history_p && history_filename)
    write_history (history_filename);
  exit (0);
}

int
input_from_terminal_p ()
{
  return instream == stdin;
}

static void
pwd_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  if (arg) error ("The \"pwd\" command does not take an argument: %s", arg);
  getwd (dirbuf);

  if (strcmp (dirbuf, current_directory))
    printf ("Working directory %s\n (canonically %s).\n",
	    current_directory, dirbuf);
  else
    printf ("Working directory %s.\n", current_directory);
}

static void
cd_command (dir, from_tty)
     char *dir;
     int from_tty;
{
  int len;
  int change;

  if (dir == 0)
    error_no_arg ("new working directory");

  dir = tilde_expand (dir);
  make_cleanup (free, dir);

  len = strlen (dir);
  dir = savestring (dir, len - (len > 1 && dir[len-1] == '/'));
  if (dir[0] == '/')
    current_directory = dir;
  else
    {
      current_directory = concat (current_directory, "/", dir);
      free (dir);
    }

  /* Now simplify any occurrences of `.' and `..' in the pathname.  */

  change = 1;
  while (change)
    {
      char *p;
      change = 0;

      for (p = current_directory; *p;)
	{
	  if (!strncmp (p, "/./", 2)
	      && (p[2] == 0 || p[2] == '/'))
	    strcpy (p, p + 2);
	  else if (!strncmp (p, "/..", 3)
		   && (p[3] == 0 || p[3] == '/')
		   && p != current_directory)
	    {
	      char *q = p;
	      while (q != current_directory && q[-1] != '/') q--;
	      if (q != current_directory)
		{
		  strcpy (q-1, p+3);
		  p = q-1;
		}
	    }
	  else p++;
	}
    }

  if (chdir (dir) < 0)
    perror_with_name (dir);

  if (from_tty)
    pwd_command ((char *) 0, 1);
}

static void
source_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  FILE *stream;
  struct cleanup *cleanups;
  char *file = arg;

  if (file == 0)
    /* Let source without arguments read .gdbinit.  */
    file = ".gdbinit";

  file = tilde_expand (file);
  make_cleanup (free, file);

  stream = fopen (file, "r");
  if (stream == 0)
    perror_with_name (file);

  cleanups = make_cleanup (source_cleanup, instream);

  instream = stream;

  command_loop ();

  do_cleanups (cleanups);
}

static void
echo_command (text)
     char *text;
{
  char *p = text;
  register int c;

  if (text)
    while (c = *p++)
      {
	if (c == '\\')
	  {
	    /* \ at end of argument is used after spaces
	       so they won't be lost.  */
	    if (*p == 0)
	      return;

	    c = parse_escape (&p);
	    if (c >= 0)
	      fputc (c, stdout);
	  }
	else
	  fputc (c, stdout);
      }
}

static void
dump_me_command ()
{
  if (query ("Should GDB dump core? "))
    {
      signal (SIGQUIT, SIG_DFL);
      kill (getpid (), SIGQUIT);
    }
}

int
parse_binary_operation (caller, arg)
     char *caller, *arg;
{
  int length;

  if (!arg || !*arg)
    return 1;

  length = strlen (arg);

  while (arg[length - 1] == ' ' || arg[length - 1] == '\t')
    length--;

  if (!strncmp (arg, "on", length)
      || !strncmp (arg, "1", length)
      || !strncmp (arg, "yes", length))
    return 1;
  else
    if (!strncmp (arg, "off", length)
	|| !strncmp (arg, "0", length)
	|| !strncmp (arg, "no", length))
      return 0;
    else
      error ("\"%s\" not given a binary valued argument.", caller);
}

/* Functions to manipulate command line editing control variables.  */

static void
set_editing (arg, from_tty)
     char *arg;
     int from_tty;
{
  command_editing_p = parse_binary_operation ("set command-editing", arg);
}

/* Number of commands to print in each call to editing_info.  */
#define Hist_print 10
static void
editing_info (arg, from_tty)
     char *arg;
     int from_tty;
{
  /* Index for history commands.  Relative to history_base.  */
  int offset;

  /* Number of the history entry which we are planning to display next.
     Relative to history_base.  */
  static int num = 0;

  /* The first command in the history which doesn't exist (i.e. one more
     than the number of the last command).  Relative to history_base.  */
  int hist_len;

  struct _hist_entry {
    char *line;
    char *data;
  } *history_get();
  extern int history_base;

  printf_filtered ("Interactive command editing is %s.\n",
	  command_editing_p ? "on" : "off");

  printf_filtered ("History expansion of command input is %s.\n",
	  history_expansion_p ? "on" : "off");
  printf_filtered ("Writing of a history record upon exit is %s.\n",
	  write_history_p ? "enabled" : "disabled");
  printf_filtered ("The size of the history list (number of stored commands) is %d.\n",
	  history_size);
  printf_filtered ("The name of the history record is \"%s\".\n\n",
	  history_filename ? history_filename : "");

  /* Print out some of the commands from the command history.  */
  /* First determine the length of the history list.  */
  hist_len = history_size;
  for (offset = 0; offset < history_size; offset++)
    {
      if (!history_get (history_base + offset))
	{
	  hist_len = offset;
	  break;
	}
    }

  if (arg)
    {
      if (arg[0] == '+' && arg[1] == '\0')
	/* "info editing +" should print from the stored position.  */
	;
      else
	/* "info editing <exp>" should print around command number <exp>.  */
	num = (parse_and_eval_address (arg) - history_base) - Hist_print / 2;
    }
  /* "info editing" means print the last Hist_print commands.  */
  else
    {
      num = hist_len - Hist_print;
    }

  if (num < 0)
    num = 0;

  /* If there are at least Hist_print commands, we want to display the last
     Hist_print rather than, say, the last 6.  */
  if (hist_len - num < Hist_print)
    {
      num = hist_len - Hist_print;
      if (num < 0)
	num = 0;
    }

  if (num == hist_len - Hist_print)
    printf_filtered ("The list of the last %d commands is:\n\n", Hist_print);
  else
    printf_filtered ("Some of the stored commands are:\n\n");

  for (offset = num; offset < num + Hist_print && offset < hist_len; offset++)
    {
      printf_filtered ("%5d  %s\n", history_base + offset,
	      (history_get (history_base + offset))->line);
    }

  /* The next command we want to display is the next one that we haven't
     displayed yet.  */
  num += Hist_print;
  
  /* If the user repeats this command with return, it should do what
     "info editing +" does.  This is unnecessary if arg is null,
     because "info editing +" is not useful after "info editing".  */
  if (from_tty && arg)
    {
      arg[0] = '+';
      arg[1] = '\0';
    }
}

static void
set_history_expansion (arg, from_tty)
     char *arg;
     int from_tty;
{
  history_expansion_p = parse_binary_operation ("set history expansion", arg);
}

static void
set_history_write (arg, from_tty)
     char *arg;
     int from_tty;
{
  write_history_p = parse_binary_operation ("set history write", arg);
}

static void
set_history (arg, from_tty)
     char *arg;
     int from_tty;
{
  printf ("\"set history\" must be followed by the name of a history subcommand.\n");
  help_list (sethistlist, "set history ", -1, stdout);
}

static void
set_history_size (arg, from_tty)
     char *arg;
     int from_tty;
{
  if (!*arg)
    error_no_arg ("set history size");

  history_size = atoi (arg);
}

static void
set_history_filename (arg, from_tty)
     char *arg;
     int from_tty;
{
  int i;

  if (!arg)
    error_no_arg ("history file name");
  
  arg = tilde_expand (arg);
  make_cleanup (free, arg);

  i = strlen (arg) - 1;
  
  free (history_filename);
  
  while (i > 0 && (arg[i] == ' ' || arg[i] == '\t'))
    i--;

  if (!*arg)
    history_filename = (char *) 0;
  else
    history_filename = savestring (arg, i + 1);
  history_filename[i] = '\0';
}

int info_verbose;

static void
set_verbose_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  info_verbose = parse_binary_operation ("set verbose", arg);
}

static void
verbose_info (arg, from_tty)
     char *arg;
     int from_tty;
{
  if (arg)
    error ("\"info verbose\" does not take any arguments.\n");
  
  printf ("Verbose printing of information is %s.\n",
	  info_verbose ? "on" : "off");
}

static void
float_handler ()
{
  error ("Invalid floating value encountered or computed.");
}


static void
initialize_cmd_lists ()
{
  cmdlist = (struct cmd_list_element *) 0;
  infolist = (struct cmd_list_element *) 0;
  enablelist = (struct cmd_list_element *) 0;
  disablelist = (struct cmd_list_element *) 0;
  deletelist = (struct cmd_list_element *) 0;
  enablebreaklist = (struct cmd_list_element *) 0;
  setlist = (struct cmd_list_element *) 0;
  sethistlist = (struct cmd_list_element *) 0;
  unsethistlist = (struct cmd_list_element *) 0;
}

static void
initialize_main ()
{
  char *tmpenv;
  /* Command line editing externals.  */
  extern int (*rl_completion_entry_function)();
  extern char *rl_completer_word_break_characters;
  
  /* Set default verbose mode on.  */
  info_verbose = 1;

  prompt = savestring ("(gdb) ", 6);

  /* Set the important stuff up for command editing.  */
  command_editing_p = 1;
  history_expansion_p = 0;
  write_history_p = 0;
  
  if (tmpenv = getenv ("HISTSIZE"))
    history_size = atoi (tmpenv);
  else
    history_size = 256;

  stifle_history (history_size);

  if (tmpenv = getenv ("GDBHISTFILE"))
    history_filename = savestring (tmpenv, strlen(tmpenv));
  else
    /* We include the current directory so that if the user changes
       directories the file written will be the same as the one
       that was read.  */
    history_filename = concat (current_directory, "/.gdb_history", "");

  read_history (history_filename);

  /* Setup important stuff for command line editing.  */
  rl_completion_entry_function = (int (*)()) symbol_completion_function;
  rl_completer_word_break_characters = gdb_completer_word_break_characters;

  /* Define the classes of commands.
     They will appear in the help list in the reverse of this order.  */

  add_cmd ("obscure", class_obscure, 0, "Obscure features.", &cmdlist);
  add_cmd ("alias", class_alias, 0, "Aliases of other commands.", &cmdlist);
  add_cmd ("user", class_user, 0, "User-defined commands.\n\
The commands in this class are those defined by the user.\n\
Use the \"define\" command to define a command.", &cmdlist);
  add_cmd ("support", class_support, 0, "Support facilities.", &cmdlist);
  add_cmd ("status", class_info, 0, "Status inquiries.", &cmdlist);
  add_cmd ("files", class_files, 0, "Specifying and examining files.", &cmdlist);
  add_cmd ("breakpoints", class_breakpoint, 0, "Making program stop at certain points.", &cmdlist);
  add_cmd ("data", class_vars, 0, "Examining data.", &cmdlist);
  add_cmd ("stack", class_stack, 0, "Examining the stack.\n\
The stack is made up of stack frames.  Gdb assigns numbers to stack frames\n\
counting from zero for the innermost (currently executing) frame.\n\n\
At any time gdb identifies one frame as the \"selected\" frame.\n\
Variable lookups are done with respect to the selected frame.\n\
When the program being debugged stops, gdb selects the innermost frame.\n\
The commands below can be used to select other frames by number or address.",
	   &cmdlist);
  add_cmd ("running", class_run, 0, "Running the program.", &cmdlist);

  add_com ("pwd", class_files, pwd_command,
	   "Print working directory.  This is used for your program as well.");
  add_com ("cd", class_files, cd_command,
	   "Set working directory to DIR for debugger and program being debugged.\n\
The change does not take effect for the program being debugged\n\
until the next time it is started.");

  add_cmd ("prompt", class_support, set_prompt_command,
	   "Change gdb's prompt from the default of \"(gdb)\"",
	   &setlist);
  add_com ("echo", class_support, echo_command,
	   "Print a constant string.  Give string as argument.\n\
C escape sequences may be used in the argument.\n\
No newline is added at the end of the argument;\n\
use \"\\n\" if you want a newline to be printed.\n\
Since leading and trailing whitespace are ignored in command arguments,\n\
if you want to print some you must use \"\\\" before leading whitespace\n\
to be printed or after trailing whitespace.");
  add_com ("document", class_support, document_command,
	   "Document a user-defined command.\n\
Give command name as argument.  Give documentation on following lines.\n\
End with a line of just \"end\".");
  add_com ("define", class_support, define_command,
	   "Define a new command name.  Command name is argument.\n\
Definition appears on following lines, one command per line.\n\
End with a line of just \"end\".\n\
Use the \"document\" command to give documentation for the new command.\n\
Commands defined in this way do not take arguments.");

  add_com ("source", class_support, source_command,
	   "Read commands from a file named FILE.\n\
Note that the file \".gdbinit\" is read automatically in this way\n\
when gdb is started.");
  add_com ("quit", class_support, quit_command, "Exit gdb.");
  add_com ("help", class_support, help_command, "Print list of commands.");
  add_com_alias ("q", "quit", class_support, 1);
  add_com_alias ("h", "help", class_support, 1);

  add_cmd ("verbose", class_support, set_verbose_command,
	   "Change the number of informational messages gdb prints.",
	   &setlist);
  add_info ("verbose", verbose_info,
	    "Status of gdb's verbose printing option.\n");

  add_com ("dump-me", class_obscure, dump_me_command,
	   "Get fatal error; make debugger dump its core.");

  add_cmd ("editing", class_support, set_editing,
	   "Enable or disable command line editing.\n\
Use \"on\" to enable to enable the editing, and \"off\" to disable it.\n\
Without an argument, command line editing is enabled.", &setlist);

  add_prefix_cmd ("history", class_support, set_history,
		  "Generic command for setting command history parameters.",
		  &sethistlist, "set history ", 0, &setlist);

  add_cmd ("expansion", no_class, set_history_expansion,
	   "Enable or disable history expansion on command input.\n\
Without an argument, history expansion is enabled.", &sethistlist);

  add_cmd ("write", no_class, set_history_write,
	   "Enable or disable saving of the history record on exit.\n\
Use \"on\" to enable to enable the saving, and \"off\" to disable it.\n\
Without an argument, saving is enabled.", &sethistlist);

  add_cmd ("size", no_class, set_history_size,
	   "Set the size of the command history, \n\
ie. the number of previous commands to keep a record of.", &sethistlist);

  add_cmd ("filename", no_class, set_history_filename,
	   "Set the filename in which to record the command history\n\
 (the list of previous commands of which a record is kept).", &sethistlist);

  add_prefix_cmd ("info", class_info, info_command,
		  "Generic command for printing status.",
		  &infolist, "info ", 0, &cmdlist);
  add_com_alias ("i", "info", class_info, 1);

  add_info ("editing", editing_info, "Status of command editor.");

  add_info ("version", version_info, "Report what version of GDB this is.");
}
