/* Top level `main' program for GDB, the GNU debugger.
   Copyright 1986, 1987, 1988, 1989, 1990, 1991, 1992 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
#include "gdbcmd.h"
#include "symtab.h"
#include "inferior.h"
#include "signals.h"
#include "target.h"
#include "breakpoint.h"
#include "gdbtypes.h"
#include "expression.h"
#include "language.h"

#include "getopt.h"

/* readline include files */
#include "readline.h"
#include "history.h"

/* readline defines this.  */
#undef savestring

#ifdef USG
#include <sys/types.h>
#include <unistd.h>
#endif

#include <string.h>
#ifndef	NO_SYS_FILE
#include <sys/file.h>
#endif
#include <setjmp.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <ctype.h>

#ifdef SET_STACK_LIMIT_HUGE
#include <sys/time.h>
#include <sys/resource.h>

int original_stack_limit;
#endif

/* Prototypes for local functions */

static char *
symbol_completion_function PARAMS ((char *, int));

static void
command_loop PARAMS ((void));

static void
command_loop_marker PARAMS ((int));

static void
print_gdb_version PARAMS ((void));

static void
quit_command PARAMS ((char *, int));

static void
initialize_main PARAMS ((void));

static void
initialize_history PARAMS ((void));

static void
initialize_cmd_lists PARAMS ((void));

static void
float_handler PARAMS ((int));

static void
source_command PARAMS ((char *, int));

static void
cd_command PARAMS ((char *, int));

static void
print_gnu_advertisement PARAMS ((void));

static void
init_signals PARAMS ((void));

static void
read_command_file PARAMS ((FILE *));

static void 
set_verbose PARAMS ((char *, int, struct cmd_list_element *));

static void
show_history PARAMS ((char *, int));

static void
set_history PARAMS ((char *, int));

static void
set_history_size_command PARAMS ((char *, int, struct cmd_list_element *));

static void
show_commands PARAMS ((char *, int));

static void
dump_me_command PARAMS ((char *, int));

static void
echo_command PARAMS ((char *, int));

static void
pwd_command PARAMS ((char *, int));

static void
show_version PARAMS ((char *, int));

static void
document_command PARAMS ((char *, int));

static void
define_command PARAMS ((char *, int));

static void
validate_comname PARAMS ((char *));

static void
help_command PARAMS ((char *, int));

static void
show_command PARAMS ((char *, int));

static void
info_command PARAMS ((char *, int));

static void
do_nothing PARAMS ((int));

static void
disconnect PARAMS ((int));

static void
source_cleanup PARAMS ((FILE *));

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

#define	ALL_CLEANUPS	((struct cleanup *)0)

/* Version number of GDB, as a string.  */

extern char *version;

/* Message to be printed before the error message, when an error occurs.  */

extern char *error_pre_print;

/* Message to be printed before the warning message, when a warning occurs.  */

extern char *warning_pre_print;

extern char lang_frame_mismatch_warn[];		/* language.c */

/* Whether GDB's stdin is on a terminal.  */

extern int gdb_has_a_terminal;			/* inflow.c */

/* Flag for whether we want all the "from_tty" gubbish printed.  */

int caution = 1;			/* Default is yes, sigh. */

/*
 * Define all cmd_list_element's
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

/* Chain containing all defined show subcommands.  */
struct cmd_list_element *showlist;

/* Chain containing all defined \"set history\".  */

struct cmd_list_element *sethistlist;

/* Chain containing all defined \"show history\".  */
struct cmd_list_element *showhistlist;

/* Chain containing all defined \"unset history\".  */

struct cmd_list_element *unsethistlist;

/* stdio stream that command input is being read from.  */

FILE *instream;

/* Current working directory.  */

char *current_directory;

/* The directory name is actually stored here (usually).  */
static char dirbuf[1024];

/* Function to call before reading a command, if nonzero.
   The function receives two args: an input stream,
   and a prompt string.  */

void (*window_hook) PARAMS ((FILE *, char *));

extern int frame_file_full_name;
extern int mapped_symbol_files;
extern int readnow_symbol_files;

int epoch_interface;
int xgdb_verbose;

/* gdb prints this when reading a command interactively */
static char *prompt;

/* Buffer used for reading command lines, and the size
   allocated for it so far.  */

char *line;
int linesize = 100;

/* Baud rate specified for talking to serial target systems.  Default
   is left as a zero pointer, so targets can choose their own defaults.  */

char *baud_rate;

/* Signal to catch ^Z typed while reading a command: SIGTSTP or SIGCONT.  */

#ifndef STOP_SIGNAL
#ifdef SIGTSTP
#define STOP_SIGNAL SIGTSTP
static void stop_sig PARAMS ((int));
#endif
#endif

/* Some System V have job control but not sigsetmask(). */
#if !defined (HAVE_SIGSETMASK)
#define HAVE_SIGSETMASK !defined (USG)
#endif

#if 0 == (HAVE_SIGSETMASK)
#define sigsetmask(n)
#endif

/* This is how `error' returns to command level.  */

jmp_buf to_top_level;

NORETURN void
return_to_top_level ()
{
  quit_flag = 0;
  immediate_quit = 0;
  bpstat_clear_actions(stop_bpstat);	/* Clear queued breakpoint commands */
  disable_current_display ();
  do_cleanups (ALL_CLEANUPS);
  (NORETURN void) longjmp (to_top_level, 1);
}

/* Call FUNC with arg ARGS, catching any errors.
   If there is no error, return the value returned by FUNC.
   If there is an error, print ERRSTRING, print the specific error message,
		         then return zero.  */

int
catch_errors (func, args, errstring)
     int (*func) PARAMS ((char *));
     char *args;
     char *errstring;
{
  jmp_buf saved;
  int val;
  struct cleanup *saved_cleanup_chain;
  char *saved_error_pre_print;

  saved_cleanup_chain = save_cleanups ();
  saved_error_pre_print = error_pre_print;

  memcpy ((char *)saved, (char *)to_top_level, sizeof (jmp_buf));
  error_pre_print = errstring;

  if (setjmp (to_top_level) == 0)
    val = (*func) (args);
  else
    val = 0;

  restore_cleanups (saved_cleanup_chain);

  error_pre_print = saved_error_pre_print;
  memcpy ((char *)to_top_level, (char *)saved, sizeof (jmp_buf));
  return val;
}

/* Handler for SIGHUP.  */

static void
disconnect (signo)
int signo;
{
  kill_inferior_fast ();
  signal (signo, SIG_DFL);
  kill (getpid (), SIGHUP);
}

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
static void
read_command_file (stream)
     FILE *stream;
{
  struct cleanup *cleanups;

  cleanups = make_cleanup (source_cleanup, instream);
  instream = stream;
  command_loop ();
  do_cleanups (cleanups);
}

int
main (argc, argv)
     int argc;
     char **argv;
{
  int count;
  static int inhibit_gdbinit = 0;
  static int quiet = 0;
  static int batch = 0;

  /* Pointers to various arguments from command line.  */
  char *symarg = NULL;
  char *execarg = NULL;
  char *corearg = NULL;
  char *cdarg = NULL;
  char *ttyarg = NULL;

  /* Pointers to all arguments of +command option.  */
  char **cmdarg;
  /* Allocated size of cmdarg.  */
  int cmdsize;
  /* Number of elements of cmdarg used.  */
  int ncmd;

  /* Indices of all arguments of +directory option.  */
  char **dirarg;
  /* Allocated size.  */
  int dirsize;
  /* Number of elements used.  */
  int ndir;
  
  register int i;

  /* This needs to happen before the first use of malloc.  */
  init_malloc ((PTR) NULL);

#if defined (ALIGN_STACK_ON_STARTUP)
  i = (int) &count & 0x3;
  if (i != 0)
    alloca (4 - i);
#endif

  /* If error() is called from initialization code, just exit */
  if (setjmp (to_top_level)) {
    exit(1);
  }

  cmdsize = 1;
  cmdarg = (char **) xmalloc (cmdsize * sizeof (*cmdarg));
  ncmd = 0;
  dirsize = 1;
  dirarg = (char **) xmalloc (dirsize * sizeof (*dirarg));
  ndir = 0;

  quit_flag = 0;
  line = (char *) xmalloc (linesize);
  line[0] = '\0';		/* Terminate saved (now empty) cmd line */
  instream = stdin;

  getcwd (dirbuf, sizeof (dirbuf));
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

  /* Parse arguments and options.  */
  {
    int c;
    static int print_help;
    /* When var field is 0, use flag field to record the equivalent
       short option (or arbitrary numbers starting at 10 for those
       with no equivalent).  */
    static struct option long_options[] =
      {
	{"readnow", no_argument, &readnow_symbol_files, 1},
	{"r", no_argument, &readnow_symbol_files, 1},
	{"mapped", no_argument, &mapped_symbol_files, 1},
	{"m", no_argument, &mapped_symbol_files, 1},
	{"quiet", no_argument, &quiet, 1},
	{"q", no_argument, &quiet, 1},
	{"nx", no_argument, &inhibit_gdbinit, 1},
	{"n", no_argument, &inhibit_gdbinit, 1},
	{"batch", no_argument, &batch, 1},
	{"epoch", no_argument, &epoch_interface, 1},
	{"fullname", no_argument, &frame_file_full_name, 1},
	{"f", no_argument, &frame_file_full_name, 1},
	{"help", no_argument, &print_help, 1},
	{"se", required_argument, 0, 10},
	{"symbols", required_argument, 0, 's'},
	{"s", required_argument, 0, 's'},
	{"exec", required_argument, 0, 'e'},
	{"e", required_argument, 0, 'e'},
	{"core", required_argument, 0, 'c'},
	{"c", required_argument, 0, 'c'},
	{"command", required_argument, 0, 'x'},
	{"x", required_argument, 0, 'x'},
	{"directory", required_argument, 0, 'd'},
	{"cd", required_argument, 0, 11},
	{"tty", required_argument, 0, 't'},
	{"baud", required_argument, 0, 'b'},
	{"b", required_argument, 0, 'b'},
/* Allow machine descriptions to add more options... */
#ifdef ADDITIONAL_OPTIONS
	ADDITIONAL_OPTIONS
#endif
	{0, no_argument, 0, 0},
      };

    while (1)
      {
	int option_index;

	c = getopt_long_only (argc, argv, "",
			      long_options, &option_index);
	if (c == EOF)
	  break;

	/* Long option that takes an argument.  */
	if (c == 0 && long_options[option_index].flag == 0)
	  c = long_options[option_index].val;

	switch (c)
	  {
	  case 0:
	    /* Long option that just sets a flag.  */
	    break;
	  case 10:
	    symarg = optarg;
	    execarg = optarg;
	    break;
	  case 11:
	    cdarg = optarg;
	    break;
	  case 's':
	    symarg = optarg;
	    break;
	  case 'e':
	    execarg = optarg;
	    break;
	  case 'c':
	    corearg = optarg;
	    break;
	  case 'x':
	    cmdarg[ncmd++] = optarg;
	    if (ncmd >= cmdsize)
	      {
		cmdsize *= 2;
		cmdarg = (char **) xrealloc ((char *)cmdarg,
					     cmdsize * sizeof (*cmdarg));
	      }
	    break;
	  case 'd':
	    dirarg[ndir++] = optarg;
	    if (ndir >= dirsize)
	      {
		dirsize *= 2;
		dirarg = (char **) xrealloc ((char *)dirarg,
					     dirsize * sizeof (*dirarg));
	      }
	    break;
	  case 't':
	    ttyarg = optarg;
	    break;
	  case 'q':
	    quiet = 1;
	    break;
	  case 'b':
	    baud_rate = optarg;
	    break;
#ifdef ADDITIONAL_OPTION_CASES
	  ADDITIONAL_OPTION_CASES
#endif
	  case '?':
	    fprintf (stderr,
		     "Use `%s +help' for a complete list of options.\n",
		     argv[0]);
	    exit (1);
	  }

      }
    if (print_help)
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
  -epoch            Output information used by epoch emacs-GDB interface.\n\
  -batch            Exit after processing options.\n\
  -nx               Do not read .gdbinit file.\n\
  -tty=TTY          Use TTY for input/output by the program being debugged.\n\
  -cd=DIR           Change current directory to DIR.\n\
  -directory=DIR    Search for source files in DIR.\n\
  -command=FILE     Execute GDB commands from FILE.\n\
  -symbols=SYMFILE  Read symbols from SYMFILE.\n\
  -exec=EXECFILE    Use EXECFILE as the executable.\n\
  -se=FILE          Use FILE as symbol file and executable file.\n\
  -core=COREFILE    Analyze the core dump COREFILE.\n\
  -b BAUDRATE       Set serial port baud rate used for remote debugging.\n\
  -mapped           Use mapped symbol files if supported on this system.\n\
  -readnow          Fully read symbol files on first access.\n\
", stderr);
#ifdef ADDITIONAL_OPTION_HELP
	fputs (ADDITIONAL_OPTION_HELP, stderr);
#endif
	fputs ("\n\
For more information, type \"help\" from within GDB, or consult the\n\
GDB manual (available as on-line info or a printed manual).\n", stderr);
	/* Exiting after printing this message seems like
	   the most useful thing to do.  */
	exit (0);
      }
    
    /* OK, that's all the options.  The other arguments are filenames.  */
    count = 0;
    for (; optind < argc; optind++)
      switch (++count)
	{
	case 1:
	  symarg = argv[optind];
	  execarg = argv[optind];
	  break;
	case 2:
	  corearg = argv[optind];
	  break;
	case 3:
	  fprintf (stderr,
		   "Excess command line arguments ignored. (%s%s)\n",
		   argv[optind], (optind == argc - 1) ? "" : " ...");
	  break;
	}
    if (batch)
      quiet = 1;
  }

  /* Run the init function of each source file */

  initialize_cmd_lists ();	/* This needs to be done first */
  initialize_all_files ();
  initialize_main ();		/* But that omits this file!  Do it now */
  init_signals ();

  if (!quiet)
    {
      /* Print all the junk at the top, with trailing "..." if we are about
	 to read a symbol file (possibly slowly).  */
      print_gnu_advertisement ();
      print_gdb_version ();
      if (symarg)
	printf_filtered ("..");
      wrap_here("");
      fflush (stdout);		/* Force to screen during slow operations */
    }

  error_pre_print = "\n\n";
  warning_pre_print = "\n\nwarning: ";

  /* Now perform all the actions indicated by the arguments.  */
  if (cdarg != NULL)
    {
      if (!setjmp (to_top_level))
	{
	  cd_command (cdarg, 0);
	  init_source_path ();
	}
    }
  do_cleanups (ALL_CLEANUPS);

  for (i = 0; i < ndir; i++)
    if (!setjmp (to_top_level))
      directory_command (dirarg[i], 0);
  free (dirarg);
  do_cleanups (ALL_CLEANUPS);

  if (execarg != NULL
      && symarg != NULL
      && strcmp (execarg, symarg) == 0)
    {
      /* The exec file and the symbol-file are the same.  If we can't open
	 it, better only print one error message.  */
      if (!setjmp (to_top_level))
	{
	  exec_file_command (execarg, !batch);
	  symbol_file_command (symarg, 0);
	}
    }
  else
    {
      if (execarg != NULL)
	if (!setjmp (to_top_level))
	  exec_file_command (execarg, !batch);
      if (symarg != NULL)
	if (!setjmp (to_top_level))
	  symbol_file_command (symarg, 0);
    }
  do_cleanups (ALL_CLEANUPS);

  /* After the symbol file has been read, print a newline to get us
     beyond the copyright line...  But errors should still set off
     the error message with a (single) blank line.  */
  if (!quiet)
    printf_filtered ("\n");
  error_pre_print = "\n";
  warning_pre_print = "\nwarning: ";

  /* Set the initial language. */
  {
    struct partial_symtab *pst = find_main_psymtab ();
    enum language lang = language_unknown;  	
    if (pst == NULL) ;
#if 0
    /* A better solution would set the language when reading the psymtab.
       This would win for symbol file formats that encode the langauge,
       such as DWARF.  But, we don't do that yet. FIXME */
    else if (pst->language != language_unknown)
	lang = pst->language;
#endif
    else if (pst->filename != NULL)
      lang = deduce_language_from_filename (pst->filename);
    if (lang == language_unknown) /* Make C the default language */
	lang = language_c;
    set_language (lang);
  }

  if (corearg != NULL)
    if (!setjmp (to_top_level))
      core_file_command (corearg, !batch);
    else if (isdigit (corearg[0]) && !setjmp (to_top_level))
      attach_command (corearg, !batch);
  do_cleanups (ALL_CLEANUPS);

  if (ttyarg != NULL)
    if (!setjmp (to_top_level))
      tty_command (ttyarg, !batch);
  do_cleanups (ALL_CLEANUPS);

#ifdef ADDITIONAL_OPTION_HANDLER
  ADDITIONAL_OPTION_HANDLER;
#endif

  /* Error messages should no longer be distinguished with extra output. */
  error_pre_print = 0;
  warning_pre_print = "warning: ";

  {
    struct stat homebuf, cwdbuf;
    char *homedir, *homeinit;

    /* Read init file, if it exists in home directory  */
    homedir = getenv ("HOME");
    if (homedir)
      {
	homeinit = (char *) alloca (strlen (getenv ("HOME")) +
				    strlen (gdbinit) + 10);
	strcpy (homeinit, getenv ("HOME"));
	strcat (homeinit, "/");
	strcat (homeinit, gdbinit);
	if (!inhibit_gdbinit && access (homeinit, R_OK) == 0)
	  if (!setjmp (to_top_level))
	    source_command (homeinit, 0);
	do_cleanups (ALL_CLEANUPS);

	/* Do stats; no need to do them elsewhere since we'll only
	   need them if homedir is set.  Make sure that they are
	   zero in case one of them fails (this guarantees that they
	   won't match if either exists).  */
	
	memset (&homebuf, 0, sizeof (struct stat));
	memset (&cwdbuf, 0, sizeof (struct stat));
	
	stat (homeinit, &homebuf);
	stat (gdbinit, &cwdbuf); /* We'll only need this if
					 homedir was set.  */
      }
    
    /* Read the input file in the current directory, *if* it isn't
       the same file (it should exist, also).  */

    if (!homedir
	|| bcmp ((char *) &homebuf,
		 (char *) &cwdbuf,
		 sizeof (struct stat)))
      if (!inhibit_gdbinit && access (gdbinit, R_OK) == 0)
	if (!setjmp (to_top_level))
	  source_command (gdbinit, 0);
	do_cleanups (ALL_CLEANUPS);
  }

  for (i = 0; i < ncmd; i++)
    if (!setjmp (to_top_level))
      {
	if (cmdarg[i][0] == '-' && cmdarg[i][1] == '\0')
	  read_command_file (stdin);
	else
	  source_command (cmdarg[i], !batch);
	do_cleanups (ALL_CLEANUPS);
      }
  free (cmdarg);

  /* Read in the old history after all the command files have been read. */
  initialize_history();

  if (batch)
    {
      /* We have hit the end of the batch file.  */
      exit (0);
    }

  /* Do any host- or target-specific hacks.  This is used for i960 targets
     to force the user to set a nindy target and spec its parameters.  */

#ifdef BEFORE_MAIN_LOOP_HOOK
  BEFORE_MAIN_LOOP_HOOK;
#endif

  /* The command loop.  */

  while (1)
    {
      if (!setjmp (to_top_level))
	{
	  do_cleanups (ALL_CLEANUPS);		/* Do complete cleanup */
	  command_loop ();
          quit_command ((char *)0, instream == stdin);
	}
    }
  /* No exit -- exit is through quit_command.  */
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
  register enum language flang;
  static const struct language_defn *saved_language = 0;
  static int warned = 0;

  free_all_values ();

  /* This can happen when command_line_input hits end of file.  */
  if (p == NULL)
      return;
  
  while (*p == ' ' || *p == '\t') p++;
  if (*p)
    {
      char *arg;
      
      c = lookup_cmd (&p, cmdlist, "", 0, 1);
      /* Pass null arg rather than an empty one.  */
      arg = *p ? p : 0;
      if (c->class == class_user)
	{
	  struct cleanup *old_chain;
	  
	  if (*p)
	    error ("User-defined commands cannot take arguments.");
	  cmdlines = c->user_commands;
	  if (cmdlines == 0)
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
      else if (c->type == set_cmd || c->type == show_cmd)
	do_setshow_command (arg, from_tty & caution, c);
      else if (c->function.cfunc == NO_FUNCTION)
	error ("That is not a command, just a help topic.");
      else
	(*c->function.cfunc) (arg, from_tty & caution);
   }

  /* Tell the user if the language has changed (except first time).  */
  if (current_language != saved_language)
  {
    if (language_mode == language_mode_auto) {
      if (saved_language)
	language_info (1);	/* Print what changed.  */
    }
    saved_language = current_language;
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
static void
command_loop_marker (foo)
     int foo;
{
}

/* Read commands from `instream' and execute them
   until end of file or error reading instream.  */
static void
command_loop ()
{
  struct cleanup *old_chain;
  char *command;
  int stdin_is_tty = ISATTY (stdin);

  while (!feof (instream))
    {
      if (window_hook && instream == stdin)
	(*window_hook) (instream, prompt);

      quit_flag = 0;
      if (instream == stdin && stdin_is_tty)
	reinitialize_more_filter ();
      old_chain = make_cleanup (command_loop_marker, 0);
      command = command_line_input (instream == stdin ? prompt : (char *) NULL,
				      instream == stdin);
      if (command == 0)
	return;
      execute_command (command, instream == stdin);
      /* Do any commands attached to breakpoint we stopped at.  */
      bpstat_do_actions (&stop_bpstat);
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

   It prints PRROMPT once at the start.
   Action is compatible with "readline", e.g. space for the result is 
   malloc'd and should be freed by the caller.

   A NULL return means end of file.  */
char *
gdb_readline (prrompt)
     char *prrompt;
{
  int c;
  char *result;
  int input_index = 0;
  int result_size = 80;

  if (prrompt)
    {
      printf (prrompt);
      fflush (stdout);
    }
  
  result = (char *) xmalloc (result_size);

  while (1)
    {
      /* Read from stdin if we are executing a user defined command.
	 This is the right thing for prompt_for_continue, at least.  */
      c = fgetc (instream ? instream : stdin);

      if (c == EOF)
	{
	  free (result);
	  return NULL;
	}

      if (c == '\n')
	break;

      result[input_index++] = c;
      while (input_index >= result_size)
	{
	  result_size *= 2;
	  result = (char *) xrealloc (result, result_size);
	}
    }

  result[input_index++] = '\0';
  return result;
}

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

/* This can be used for functions which don't want to complete on symbols
   but don't want to complete on anything else either.  */
/* ARGSUSED */
char **
noop_completer (text)
     char *text;
{
  return NULL;
}

/* Generate symbol names one by one for the completer.  If STATE is
   zero, then we need to initialize, otherwise the initialization has
   already taken place.  TEXT is what we expect the symbol to start
   with.  RL_LINE_BUFFER is available to be looked at; it contains the
   entire text of the line.  RL_POINT is the offset in that line of
   the cursor.  You should pretend that the line ends at RL_POINT.
   The result is NULL if there are no more completions, else a char
   string which is a possible completion.  */

static char *
symbol_completion_function (text, state)
     char *text;
     int state;
{
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
	  /* If we didn't recognize everything up to the thing that
	     needs completing, and we don't know what command it is
	     yet, we are in trouble.  Part of the trouble might be
	     that the list of delimiters used by readline includes
	     '-', which we use in commands.  Check for this.  */
	  if (p + strlen(text) != tmp_command + rl_point) {
	    if (tmp_command[rl_point - strlen(text) - 1] == '-')
	      text = p;
	    else {
	      /* This really should not produce an error.  Better would
		 be to pretend to hit RETURN here; this would produce a
		 response like "Ambiguous command: foo, foobar, etc",
		 and leave the line available for re-entry with ^P.  Instead,
		 this error blows away the user's typed input without
		 any way to get it back.  */
	      error ("  Unrecognized command.");
	    }
	  }
	  
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
		list = (*c->completer) ("");
	    }
	  else
	    {
	      if (c->prefixlist && !c->allow_unknown)
		{
#if 0
		  /* Something like "info adsfkdj".  But error() is not
		     the proper response; just return no completions
		     instead.  */
		  *p = '\0';
		  error ("\"%s\" command requires a subcommand.",
			 tmp_command);
#else
		  list = NULL;
#endif
		}
	      else
		list = (*c->completer) (text);
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
stop_sig (signo)
int signo;
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

/* Initialize signal handlers. */
static void
do_nothing (signo)
int signo;
{
}

static void
init_signals ()
{
  signal (SIGINT, request_quit);

  /* If we initialize SIGQUIT to SIG_IGN, then the SIG_IGN will get
     passed to the inferior, which we don't want.  It would be
     possible to do a "signal (SIGQUIT, SIG_DFL)" after we fork, but
     on BSD4.3 systems using vfork, that can affect the
     GDB process as well as the inferior (the signal handling tables
     might be in memory, shared between the two).  Since we establish
     a handler for SIGQUIT, when we call exec it will set the signal
     to SIG_DFL for us.  */
  signal (SIGQUIT, do_nothing);
  if (signal (SIGHUP, do_nothing) != SIG_IGN)
    signal (SIGHUP, disconnect);
  signal (SIGFPE, float_handler);

#if defined(SIGWINCH) && defined(SIGWINCH_HANDLER)
  signal (SIGWINCH, SIGWINCH_HANDLER);
#endif
}

/* Read one line from the command input stream `instream'
   into the local static buffer `linebuffer' (whose current length
   is `linelength').
   The buffer is made bigger as necessary.
   Returns the address of the start of the line.

   NULL is returned for end of file.

   *If* the instream == stdin & stdin is a terminal, the line read
   is copied into the file line saver (global var char *line,
   length linesize) so that it can be duplicated.

   This routine either uses fancy command line editing or
   simple input as the user has requested.  */

char *
command_line_input (prrompt, repeat)
     char *prrompt;
     int repeat;
{
  static char *linebuffer = 0;
  static unsigned linelength = 0;
  register char *p;
  char *p1;
  char *rl;
  char *local_prompt = prrompt;
  register int c;
  char *nline;
  char got_eof = 0;

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
      /* Reports are that some Sys V's don't flush stdout/err on reads
	 from stdin, when stdin/out are sockets rather than ttys.  So we
	 have to do it ourselves, to make emacs-gdb and xxgdb work.
	 On other machines, doing this once per input should be a cheap nop.  */
      fflush (stdout);
      fflush (stderr);

      /* Don't use fancy stuff if not talking to stdin.  */
      if (command_editing_p && instream == stdin
	  && ISATTY (instream))
	rl = readline (local_prompt);
      else
	rl = gdb_readline (local_prompt);

      if (!rl || rl == (char *) EOF)
	{
	  got_eof = 1;
	  break;
	}
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

  if (got_eof)
    return NULL;

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
	      return command_line_input (prrompt, repeat);
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

  /* Note: lines consisting soley of comments are added to the command
     history.  This is useful when you type a command, and then
     realize you don't want to execute it quite yet.  You can comment
     out the command and then later fetch it from the value history
     and remove the '#'.  The kill ring is probably better, but some
     people are in the habit of commenting things out.  */
  p1 = linebuffer;
  while ((c = *p1++) != '\0')
    {
      if (c == '"')
	while ((c = *p1++) != '"')
	  {
	    /* Make sure an escaped '"' doesn't make us think the string
	       is ended.  */
	    if (c == '\\')
	      parse_escape (&p1);
	    if (c == '\0')
	      break;
	  }
      else if (c == '\'')
	while ((c = *p1++) != '\'')
	  {
	    /* Make sure an escaped '\'' doesn't make us think the string
	       is ended.  */
	    if (c == '\\')
	      parse_escape (&p1);
	    if (c == '\0')
	      break;
	  }
      else if (c == '#')
	{
	  /* Found a comment.  */
	  p1[-1] = '\0';
	  break;
	}
    }

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
      p = command_line_input ((char *) NULL, instream == stdin);
      if (p == NULL)
	/* Treat end of file like "end".  */
	break;
      
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
     void (*fun) PARAMS ((char *, int));
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

/* ARGSUSED */
static void
info_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  printf ("\"info\" must be followed by the name of an info command.\n");
  help_list (infolist, "info ", -1, stdout);
}

/* The "show" command with no arguments shows all the settings.  */

/* ARGSUSED */
static void
show_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  cmd_show_list (showlist, from_tty, "");
}

/* Add an element to the list of commands.  */

void
add_com (name, class, fun, doc)
     char *name;
     enum command_class class;
     void (*fun) PARAMS ((char *, int));
     char *doc;
{
  add_cmd (name, class, fun, doc, &cmdlist);
}

/* Add an alias or abbreviation command to the list of commands.  */

void
add_com_alias (name, oldname, class, abbrev_flag)
     char *name;
     char *oldname;
     enum command_class class;
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

/* ARGSUSED */
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
      if (!isalnum(*p) && *p != '-')
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
  register struct cmd_list_element *c, *newc;
  char *tem = comname;

  validate_comname (comname);

  /* Look it up, and verify that we got an exact match.  */
  c = lookup_cmd (&tem, cmdlist, "", -1, 1);
  if (c && 0 != strcmp (comname, c->name))
    c = 0;
    
  if (c)
    {
      if (c->class == class_user || c->class == class_alias)
	tem = "Redefine command \"%s\"? ";
      else
	tem = "Really redefine built-in command \"%s\"? ";
      if (!query (tem, c->name))
	error ("Command \"%s\" not redefined.", c->name);
    }

  comname = savestring (comname, strlen (comname));

  /* If the rest of the commands will be case insensetive, this one 
     should behave in the same manner. */
  for (tem = comname; *tem; tem++)
    if (isupper(*tem)) *tem = tolower(*tem);

  if (from_tty)
    {
      printf ("Type commands for definition of \"%s\".\n\
End with a line saying just \"end\".\n", comname);
      fflush (stdout);
    }

  cmds = read_command_lines ();

  if (c && c->class == class_user)
    free_command_lines (&c->user_commands);

  newc = add_cmd (comname, class_user, not_just_help_class_command,
	   (c && c->class == class_user)
	   ? c->doc : savestring ("User-defined.", 13), &cmdlist);
  newc->user_commands = cmds;
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

  if (c->class != class_user)
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
print_gnu_advertisement()
{
    printf ("\
GDB is free software and you are welcome to distribute copies of it\n\
 under certain conditions; type \"show copying\" to see the conditions.\n\
There is absolutely no warranty for GDB; type \"show warranty\" for details.\n\
");
}

static void
print_gdb_version ()
{
  printf_filtered ("\
GDB %s, Copyright 1992 Free Software Foundation, Inc.",
	  version);
}

/* ARGSUSED */
static void
show_version (args, from_tty)
     char *args;
     int from_tty;
{
  immediate_quit++;
  print_gnu_advertisement ();
  print_gdb_version ();
  printf_filtered ("\n");
  immediate_quit--;
}

/* xgdb calls this to reprint the usual GDB prompt.  */

void
print_prompt ()
{
  printf ("%s", prompt);
  fflush (stdout);
}

static void
quit_command (args, from_tty)
     char *args;
     int from_tty;
{
  if (inferior_pid != 0 && target_has_execution)
    {
      if (query ("The program is running.  Quit anyway? "))
	{
	  target_kill ();
	}
      else
	error ("Not confirmed.");
    }
  /* Save the history information if it is appropriate to do so.  */
  if (write_history_p && history_filename)
    write_history (history_filename);
  exit (0);
}

/* Returns whether GDB is running on a terminal and whether the user
   desires that questions be asked of them on that terminal.  */

int
input_from_terminal_p ()
{
  return gdb_has_a_terminal && (instream == stdin) & caution;
}

/* ARGSUSED */
static void
pwd_command (args, from_tty)
     char *args;
     int from_tty;
{
  if (args) error ("The \"pwd\" command does not take an argument: %s", args);
  getcwd (dirbuf, sizeof (dirbuf));

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

  /* If the new directory is absolute, repeat is a no-op; if relative,
     repeat might be useful but is more likely to be a mistake.  */
  dont_repeat ();

  if (dir == 0)
    error_no_arg ("new working directory");

  dir = tilde_expand (dir);
  make_cleanup (free, dir);

  if (chdir (dir) < 0)
    perror_with_name (dir);

  len = strlen (dir);
  dir = savestring (dir, len - (len > 1 && dir[len-1] == '/'));
  if (dir[0] == '/')
    current_directory = dir;
  else
    {
      current_directory = concat (current_directory, "/", dir, NULL);
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

  forget_cached_source_info ();

  if (from_tty)
    pwd_command ((char *) 0, 1);
}

/* ARGSUSED */
static void
source_command (args, from_tty)
     char *args;
     int from_tty;
{
  FILE *stream;
  struct cleanup *cleanups;
  char *file = args;

  if (file == 0)
    /* Let source without arguments read .gdbinit.  */
    file = gdbinit;

  file = tilde_expand (file);
  make_cleanup (free, file);

  stream = fopen (file, "r");
  if (stream == 0)
    perror_with_name (file);

  cleanups = make_cleanup (fclose, stream);

  read_command_file (stream);

  do_cleanups (cleanups);
}

/* ARGSUSED */
static void
echo_command (text, from_tty)
     char *text;
     int from_tty;
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
	      printf_filtered ("%c", c);
	  }
	else
	  printf_filtered ("%c", c);
      }

  /* Force this output to appear now.  */
  wrap_here ("");
  fflush (stdout);
}

/* ARGSUSED */
static void
dump_me_command (args, from_tty)
     char *args;
     int from_tty;
{
  if (query ("Should GDB dump core? "))
    {
      signal (SIGQUIT, SIG_DFL);
      kill (getpid (), SIGQUIT);
    }
}

/* Functions to manipulate command line editing control variables.  */

/* Number of commands to print in each call to show_commands.  */
#define Hist_print 10
static void
show_commands (args, from_tty)
     char *args;
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

  extern struct _hist_entry *history_get PARAMS ((int));
  extern int history_base;

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

  if (args)
    {
      if (args[0] == '+' && args[1] == '\0')
	/* "info editing +" should print from the stored position.  */
	;
      else
	/* "info editing <exp>" should print around command number <exp>.  */
	num = (parse_and_eval_address (args) - history_base) - Hist_print / 2;
    }
  /* "show commands" means print the last Hist_print commands.  */
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

  for (offset = num; offset < num + Hist_print && offset < hist_len; offset++)
    {
      printf_filtered ("%5d  %s\n", history_base + offset,
	      (history_get (history_base + offset))->line);
    }

  /* The next command we want to display is the next one that we haven't
     displayed yet.  */
  num += Hist_print;
  
  /* If the user repeats this command with return, it should do what
     "show commands +" does.  This is unnecessary if arg is null,
     because "show commands +" is not useful after "show commands".  */
  if (from_tty && args)
    {
      args[0] = '+';
      args[1] = '\0';
    }
}

/* Called by do_setshow_command.  */
/* ARGSUSED */
static void
set_history_size_command (args, from_tty, c)
     char *args;
     int from_tty;
     struct cmd_list_element *c;
{
  if (history_size == UINT_MAX)
    unstifle_history ();
  else if (history_size >= 0)
    stifle_history (history_size);
  else
    {
      history_size = UINT_MAX;
      error ("History size must be non-negative");
    }
}

/* ARGSUSED */
static void
set_history (args, from_tty)
     char *args;
     int from_tty;
{
  printf ("\"set history\" must be followed by the name of a history subcommand.\n");
  help_list (sethistlist, "set history ", -1, stdout);
}

/* ARGSUSED */
static void
show_history (args, from_tty)
     char *args;
     int from_tty;
{
  cmd_show_list (showhistlist, from_tty, "");
}

int info_verbose = 0;		/* Default verbose msgs off */

/* Called by do_setshow_command.  An elaborate joke.  */
/* ARGSUSED */
static void 
set_verbose (args, from_tty, c)
     char *args;
     int from_tty;
     struct cmd_list_element *c;
{
  char *cmdname = "verbose";
  struct cmd_list_element *showcmd;
  
  showcmd = lookup_cmd_1 (&cmdname, showlist, NULL, 1);

  if (info_verbose)
    {
      c->doc = "Set verbose printing of informational messages.";
      showcmd->doc = "Show verbose printing of informational messages.";
    }
  else
    {
      c->doc = "Set verbosity.";
      showcmd->doc = "Show verbosity.";
    }
}

static void
float_handler (signo)
int signo;
{
  /* This message is based on ANSI C, section 4.7.  Note that integer
     divide by zero causes this, so "float" is a misnomer.  */
  error ("Erroneous arithmetic operation.");
}

/* Return whether we are running a batch file or from terminal.  */
int
batch_mode ()
{
  return !(instream == stdin && ISATTY (stdin));
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
  showlist = NULL;
  sethistlist = (struct cmd_list_element *) 0;
  showhistlist = NULL;
  unsethistlist = (struct cmd_list_element *) 0;
}

/* Init the history buffer.  Note that we are called after the init file(s)
 * have been read so that the user can change the history file via his
 * .gdbinit file (for instance).  The GDBHISTFILE environment variable
 * overrides all of this.
 */

static void
initialize_history()
{
  char *tmpenv;

  tmpenv = getenv ("HISTSIZE");
  if (tmpenv)
    history_size = atoi (tmpenv);
  else if (!history_size)
    history_size = 256;

  stifle_history (history_size);

  tmpenv = getenv ("GDBHISTFILE");
  if (tmpenv)
    history_filename = savestring (tmpenv, strlen(tmpenv));
  else if (!history_filename) {
    /* We include the current directory so that if the user changes
       directories the file written will be the same as the one
       that was read.  */
    history_filename = concat (current_directory, "/.gdb_history", NULL);
  }
  read_history (history_filename);
}

static void
initialize_main ()
{
  struct cmd_list_element *c;
  
#ifdef DEFAULT_PROMPT
  prompt = savestring (DEFAULT_PROMPT, strlen(DEFAULT_PROMPT));
#else
  prompt = savestring ("(gdb) ", 6);
#endif

  /* Set the important stuff up for command editing.  */
  command_editing_p = 1;
  history_expansion_p = 0;
  write_history_p = 0;
  
  /* Setup important stuff for command line editing.  */
  rl_completion_entry_function = (int (*)()) symbol_completion_function;
  rl_completer_word_break_characters = gdb_completer_word_break_characters;
  rl_readline_name = "gdb";

  /* Define the classes of commands.
     They will appear in the help list in the reverse of this order.  */

  add_cmd ("obscure", class_obscure, NO_FUNCTION, "Obscure features.", &cmdlist);
  add_cmd ("aliases", class_alias, NO_FUNCTION, "Aliases of other commands.", &cmdlist);
  add_cmd ("user-defined", class_user, NO_FUNCTION, "User-defined commands.\n\
The commands in this class are those defined by the user.\n\
Use the \"define\" command to define a command.", &cmdlist);
  add_cmd ("support", class_support, NO_FUNCTION, "Support facilities.", &cmdlist);
  add_cmd ("status", class_info, NO_FUNCTION, "Status inquiries.", &cmdlist);
  add_cmd ("files", class_files, NO_FUNCTION, "Specifying and examining files.", &cmdlist);
  add_cmd ("breakpoints", class_breakpoint, NO_FUNCTION, "Making program stop at certain points.", &cmdlist);
  add_cmd ("data", class_vars, NO_FUNCTION, "Examining data.", &cmdlist);
  add_cmd ("stack", class_stack, NO_FUNCTION, "Examining the stack.\n\
The stack is made up of stack frames.  Gdb assigns numbers to stack frames\n\
counting from zero for the innermost (currently executing) frame.\n\n\
At any time gdb identifies one frame as the \"selected\" frame.\n\
Variable lookups are done with respect to the selected frame.\n\
When the program being debugged stops, gdb selects the innermost frame.\n\
The commands below can be used to select other frames by number or address.",
	   &cmdlist);
  add_cmd ("running", class_run, NO_FUNCTION, "Running the program.", &cmdlist);

  add_com ("pwd", class_files, pwd_command,
	   "Print working directory.  This is used for your program as well.");
  add_com ("cd", class_files, cd_command,
	   "Set working directory to DIR for debugger and program being debugged.\n\
The change does not take effect for the program being debugged\n\
until the next time it is started.");

  add_show_from_set
    (add_set_cmd ("prompt", class_support, var_string, (char *)&prompt,
	   "Set gdb's prompt",
	   &setlist),
     &showlist);
  
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

#ifdef __STDC__
  add_com ("source", class_support, source_command,
	   "Read commands from a file named FILE.\n\
Note that the file \"" GDBINIT_FILENAME "\" is read automatically in this way\n\
when gdb is started.");
#else
  /* Punt file name, we can't help it easily.  */
  add_com ("source", class_support, source_command,
	   "Read commands from a file named FILE.\n\
Note that the file \".gdbinit\" is read automatically in this way\n\
when gdb is started.");
#endif

  add_com ("quit", class_support, quit_command, "Exit gdb.");
  add_com ("help", class_support, help_command, "Print list of commands.");
  add_com_alias ("q", "quit", class_support, 1);
  add_com_alias ("h", "help", class_support, 1);


  c = add_set_cmd ("verbose", class_support, var_boolean, (char *)&info_verbose,
		   "Set ",
		   &setlist),
  add_show_from_set (c, &showlist);
  c->function.sfunc = set_verbose;
  set_verbose (NULL, 0, c);
  
  add_com ("dump-me", class_obscure, dump_me_command,
	   "Get fatal error; make debugger dump its core.");

  add_show_from_set
    (add_set_cmd ("editing", class_support, var_boolean, (char *)&command_editing_p,
	   "Set editing of command lines as they are typed.\n\
Use \"on\" to enable to enable the editing, and \"off\" to disable it.\n\
Without an argument, command line editing is enabled.  To edit, use\n\
EMACS-like or VI-like commands like control-P or ESC.", &setlist),
     &showlist);

  add_prefix_cmd ("history", class_support, set_history,
		  "Generic command for setting command history parameters.",
		  &sethistlist, "set history ", 0, &setlist);
  add_prefix_cmd ("history", class_support, show_history,
		  "Generic command for showing command history parameters.",
		  &showhistlist, "show history ", 0, &showlist);

  add_show_from_set
    (add_set_cmd ("expansion", no_class, var_boolean, (char *)&history_expansion_p,
	   "Set history expansion on command input.\n\
Without an argument, history expansion is enabled.", &sethistlist),
     &showhistlist);

  add_show_from_set
    (add_set_cmd ("save", no_class, var_boolean, (char *)&write_history_p,
	   "Set saving of the history record on exit.\n\
Use \"on\" to enable to enable the saving, and \"off\" to disable it.\n\
Without an argument, saving is enabled.", &sethistlist),
     &showhistlist);

  c = add_set_cmd ("size", no_class, var_uinteger, (char *)&history_size,
		   "Set the size of the command history, \n\
ie. the number of previous commands to keep a record of.", &sethistlist);
  add_show_from_set (c, &showhistlist);
  c->function.sfunc = set_history_size_command;

  add_show_from_set
    (add_set_cmd ("filename", no_class, var_filename, (char *)&history_filename,
	   "Set the filename in which to record the command history\n\
 (the list of previous commands of which a record is kept).", &sethistlist),
     &showhistlist);

  add_show_from_set
    (add_set_cmd ("confirm", class_support, var_boolean,
		  (char *)&caution,
		  "Set whether to confirm potentially dangerous operations.",
		  &setlist),
     &showlist);

  add_prefix_cmd ("info", class_info, info_command,
        "Generic command for showing things about the program being debugged.",
		  &infolist, "info ", 0, &cmdlist);
  add_com_alias ("i", "info", class_info, 1);

  add_prefix_cmd ("show", class_info, show_command,
		  "Generic command for showing things about the debugger.",
		  &showlist, "show ", 0, &cmdlist);
  /* Another way to get at the same thing.  */
  add_info ("set", show_command, "Show all GDB settings.");

  add_cmd ("commands", no_class, show_commands,
	   "Show the the history of commands you typed.\n\
You can supply a command number to start with, or a `+' to start after\n\
the previous command number shown.",
	   &showlist);

  add_cmd ("version", no_class, show_version,
	   "Show what version of GDB this is.", &showlist);
}
