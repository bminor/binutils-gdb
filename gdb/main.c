/* Top level for GDB, the GNU debugger.
   Copyright (C) 1986, 1987, 1988 Free Software Foundation, Inc.

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

#include <sys/file.h>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/param.h>
#include "defs.h"
#include "command.h"
#include "param.h"

#ifdef SET_STACK_LIMIT_HUGE
#include <sys/time.h>
#include <sys/resource.h>
#endif

/* Version number of GDB, as a string.  */

extern char *version;

/* Chain containing all defined commands.  */

struct cmd_list_element *cmdlist;

/* Chain containing all defined info subcommands.  */

struct cmd_list_element *infolist;

/* stdio stream that command input is being read from.  */

FILE *instream;

/* Current working directory.  */

char *current_directory;

/* The directory name is actually stored here (usually).  */
static char dirbuf[MAXPATHLEN];

/* Nonzero if we should refrain from using an X window.  */

int inhibit_windows = 0;

/* Hook for window manager argument parsing.  */

int *win_argc;
char **win_argv;
char *win_prgm;

/* Function to call before reading a command, if nonzero.
   The function receives two args: an input stream,
   and a prompt string.  */
   
void (*window_hook) ();

extern int frame_file_full_name;

void free_command_lines ();
char *read_line ();
static void initialize_main ();
void command_loop ();
static void source_command ();
void print_gdb_version ();

/* gdb prints this when reading a command interactively */
static char *prompt;

/* Buffer used for reading command lines, and the size
   allocated for it so far.  */

char *line;
int linesize;

/* This is how `error' returns to command level.  */

jmp_buf to_top_level;

return_to_top_level ()
{
  quit_flag = 0;
  immediate_quit = 0;
  clear_breakpoint_commands ();
  clear_momentary_breakpoints ();
  delete_current_display ();
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

  bcopy (to_top_level, saved, sizeof (jmp_buf));

  if (setjmp (to_top_level) == 0)
    val = (*func) (arg);
  else
    {
      fprintf (stderr, "%s\n", errstring);
      val = 0;
    }

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

main (argc, argv, envp)
     int argc;
     char **argv;
     char **envp;
{
  extern void request_quit ();
  int count;
  int inhibit_gdbinit = 0;
  int quiet = 0;
  int batch = 0;
  register int i;

  quit_flag = 0;
  linesize = 100;
  line = (char *) xmalloc (linesize);
  instream = stdin;

  getwd (dirbuf);
  current_directory = dirbuf;

  win_argc = &argc;
  win_argv = argv;
  win_prgm = argv[0];

#ifdef SET_STACK_LIMIT_HUGE
  {
    struct rlimit rlim;

    /* Set the stack limit huge so that alloca (particularly stringtab
     * in dbxread.c) does not fail. */
    getrlimit (RLIMIT_STACK, &rlim);
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
      else if (argv[i][0] == '-')
	i++;
    }

  /* Run the init function of each source file */

  initialize_all_files ();
  initialize_main ();		/* But that omits this file!  Do it now */

  signal (SIGINT, request_quit);
  signal (SIGQUIT, SIG_IGN);
  if (signal (SIGHUP, SIG_IGN) != SIG_IGN)
    signal (SIGHUP, disconnect);

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
	      || !strcmp (arg, "-fullname"))
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
		  int len = strlen (argv[i]);
		  current_directory = argv[i];
		  if (len > 1 && current_directory[len - 1] == '/')
		    current_directory = savestring (current_directory, len-1);
		  chdir (current_directory);
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

  /* Read init file, if it exists in home directory  */
  if (getenv ("HOME"))
    {
      char *s;
      s = (char *) xmalloc (strlen (getenv ("HOME")) + 10);
      strcpy (s, getenv ("HOME"));
      strcat (s, "/.gdbinit");
      if (!inhibit_gdbinit && access (s, R_OK) == 0)
	if (!setjmp (to_top_level))
	  source_command (s);
    }

  /* Read init file, if it exists in current directory.  */
  if (!inhibit_gdbinit && access (".gdbinit", R_OK) == 0)
    if (!setjmp (to_top_level))
      source_command (".gdbinit");

  if (batch)
    fatal ("Attempt to read commands from stdin in batch mode.");

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
      c = lookup_cmd (&p, cmdlist, "", 0);
      if (c->function == 0)
	error ("That is not a command, just a help topic.");
      else if (c->class == (int) class_user)
	{
	  if (*p)
	    error ("User-defined commands cannot take arguments.");
	  cmdlines = (struct command_line *) c->function;
	  if (cmdlines == (struct command_line *) 0)
	    /* Null command */
	    return;
	  while (cmdlines)
	    {
	      execute_command (cmdlines->line, 0);
	      cmdlines = cmdlines->next;
	    }
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
      if (instream == stdin)
	printf ("%s", prompt);
      fflush (stdout);

      if (window_hook && instream == stdin)
	(*window_hook) (instream, prompt);

      quit_flag = 0;
      old_chain = make_cleanup (do_nothing, 0);
      execute_command (read_line (instream == stdin), instream == stdin);
      /* Do any commands attached to breakpoint we stopped at.  */
      do_breakpoint_commands ();
      do_cleanups (old_chain);
    }
}

#ifdef SIGTSTP
static void
stop_sig ()
{
  signal (SIGTSTP, SIG_DFL);
  sigsetmask (0);
  kill (getpid (), SIGTSTP);
  signal (SIGTSTP, stop_sig);
  printf ("%s", prompt);
  fflush (stdout);

  /* Forget about any previous command -- null line now will do nothing.  */
  *line = 0;
}
#endif /* SIGTSTP */

/* Commands call this if they do not want to be repeated by null lines.  */

void
dont_repeat ()
{
  *line = 0;
}

/* Read one line from the command input stream `instream'
   into the buffer `line' (whose current length is `linesize').
   The buffer is made bigger as necessary.
   Returns the address of the start of the line.  */

char *
read_line (repeat)
     int repeat;
{
  register char *p = line;
  register char *p1;
  register int c;
  char *nline;

  /* Control-C quits instantly if typed while in this loop
     since it should not wait until the user types a newline.  */
  immediate_quit++;
#ifdef SIGTSTP
  signal (SIGTSTP, stop_sig);
#endif

  while (1)
    {
      c = fgetc (instream);
      if (c == -1 || c == '\n')
	break;
      /* Ignore backslash-newline; keep adding to the same line.  */
      else if (c == '\\')
	{
	  int c1 = fgetc (instream);
	  if (c1 == '\n')
	    continue;
	  else
	    ungetc (c1, instream);
	}

      if (p - line == linesize - 1)
	{
	  linesize *= 2;
	  nline = (char *) xrealloc (line, linesize);
	  p += nline - line;
	  line = nline;
	}
      *p++ = c;
    }

#ifdef SIGTSTP
  signal (SIGTSTP, SIG_DFL);
#endif
  immediate_quit--;

  /* If we just got an empty line, and that is supposed
     to repeat the previous command, leave the last input unchanged.  */
  if (p == line && repeat)
    return line;

  /* If line is a comment, clear it out.  */
  p1 = line;
  while ((c = *p1) == ' ' || c == '\t') p1++;
  if (c == '#')
    p = line;

  *p = 0;

  return line;
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
      p = read_line (1);
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
  add_cmd (name, 0, fun, doc, &infolist);
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
  help_cmd (0, infolist, "info ", -1, stdout);
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
  help_cmd (command, cmdlist, "", -2, stdout);
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

  c = lookup_cmd (&tem, cmdlist, "", -1);
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
    printf ("Type commands for definition of \"%s\".\n\
End with a line saying just \"end\".\n", comname);

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

  c = lookup_cmd (&tem, cmdlist, "", 0);

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
copying_info ()
{
  immediate_quit++;
  printf ("		    GDB GENERAL PUBLIC LICENSE\n\
		    (Clarified 11 Feb 1988)\n\
\n\
 Copyright (C) 1988 Richard M. Stallman\n\
 Everyone is permitted to copy and distribute verbatim copies\n\
 of this license, but changing it is not allowed.\n\
 You can also use this wording to make the terms for other programs.\n\
\n\
  The license agreements of most software companies keep you at the\n\
mercy of those companies.  By contrast, our general public license is\n\
intended to give everyone the right to share GDB.  To make sure that\n\
you get the rights we want you to have, we need to make restrictions\n\
that forbid anyone to deny you these rights or to ask you to surrender\n\
the rights.  Hence this license agreement.\n\
\n\
  Specifically, we want to make sure that you have the right to give\n\
away copies of GDB, that you receive source code or else can get it\n\
if you want it, that you can change GDB or use pieces of it in new\n\
free programs, and that you know you can do these things.\n\
--Type Return to print more--");
  fflush (stdout);
  read_line ();

  printf ("\
  To make sure that everyone has such rights, we have to forbid you to\n\
deprive anyone else of these rights.  For example, if you distribute\n\
copies of GDB, you must give the recipients all the rights that you\n\
have.  You must make sure that they, too, receive or can get the\n\
source code.  And you must tell them their rights.\n\
\n\
  Also, for our own protection, we must make certain that everyone\n\
finds out that there is no warranty for GDB.  If GDB is modified by\n\
someone else and passed on, we want its recipients to know that what\n\
they have is not what we distributed, so that any problems introduced\n\
by others will not reflect on our reputation.\n\
\n\
  Therefore we (Richard Stallman and the Free Software Foundation,\n\
Inc.) make the following terms which say what you must do to be\n\
allowed to distribute or change GDB.\n\
--Type Return to print more--");
  fflush (stdout);
  read_line ();

  printf ("\
			COPYING POLICIES\n\
\n\
  1. You may copy and distribute verbatim copies of GDB source code as\n\
you receive it, in any medium, provided that you conspicuously and\n\
appropriately publish on each copy a valid copyright notice \"Copyright\n\
\(C) 1988 Free Software Foundation, Inc.\" (or with whatever year is\n\
appropriate); keep intact the notices on all files that refer\n\
to this License Agreement and to the absence of any warranty; and give\n\
any other recipients of the GDB program a copy of this License\n\
Agreement along with the program.  You may charge a distribution fee\n\
for the physical act of transferring a copy.\n\
\n\
  2. You may modify your copy or copies of GDB or any portion of it,\n\
and copy and distribute such modifications under the terms of\n\
Paragraph 1 above, provided that you also do the following:\n\
\n\
    a) cause the modified files to carry prominent notices stating\n\
    that you changed the files and the date of any change; and\n\
--Type Return to print more--");
  fflush (stdout);
  read_line ();

  printf ("\
    b) cause the whole of any work that you distribute or publish,\n\
    that in whole or in part contains or is a derivative of GDB\n\
    or any part thereof, to be licensed to all third parties on terms\n\
    identical to those contained in this License Agreement (except that\n\
    you may choose to grant more extensive warranty protection to some\n\
    or all third parties, at your option).\n\
\n");
  printf ("\
    c) if the modified program serves as a debugger, cause it\n\
    when started running in the simplest and usual way, to print\n\
    an announcement including a valid copyright notice\n\
    \"Copyright (C) 1988 Free Software Foundation, Inc.\" (or with\n\
    the year that is appropriate), saying that there is no warranty\n\
    (or else, saying that you provide a warranty) and that users may\n\
    redistribute the program under these conditions, and telling the user\n\
    how to view a copy of this License Agreement.\n\
\n\
    d) You may charge a distribution fee for the physical act of\n\
    transferring a copy, and you may at your option offer warranty\n\
    protection in exchange for a fee.\n\
\n\
Mere aggregation of another unrelated program with this program (or its\n\
derivative) on a volume of a storage or distribution medium does not bring\n\
the other program under the scope of these terms.\n\
--Type Return to print more--");
  fflush (stdout);
  read_line ();

  printf ("\
  3. You may copy and distribute GDB (or a portion or derivative of it,\n\
under Paragraph 2) in object code or executable form under the terms of\n\
Paragraphs 1 and 2 above provided that you also do one of the following:\n\
\n\
    a) accompany it with the complete corresponding machine-readable\n\
    source code, which must be distributed under the terms of\n\
    Paragraphs 1 and 2 above; or,\n\
\n\
    b) accompany it with a written offer, valid for at least three\n\
    years, to give any third party free (except for a nominal\n\
    shipping charge) a complete machine-readable copy of the\n\
    corresponding source code, to be distributed under the terms of\n\
    Paragraphs 1 and 2 above; or,\n\n");

  printf ("\
    c) accompany it with the information you received as to where the\n\
    corresponding source code may be obtained.  (This alternative is\n\
    allowed only for noncommercial distribution and only if you\n\
    received the program in object code or executable form alone.)\n\
\n\
For an executable file, complete source code means all the source code for\n\
all modules it contains; but, as a special exception, it need not include\n\
source code for modules which are standard libraries that accompany the\n\
operating system on which the executable file runs.\n\
--Type Return to print more--");
  fflush (stdout);
  read_line ();

  printf ("\
  4. You may not copy, sublicense, distribute or transfer GDB\n\
except as expressly provided under this License Agreement.  Any attempt\n\
otherwise to copy, sublicense, distribute or transfer GDB is void and\n\
your rights to use the program under this License agreement shall be\n\
automatically terminated.  However, parties who have received computer\n\
software programs from you with this License Agreement will not have\n\
their licenses terminated so long as such parties remain in full compliance.\n\
\n\
  5. If you wish to incorporate parts of GDB into other free\n\
programs whose distribution conditions are different, write to the Free\n\
Software Foundation at 675 Mass Ave, Cambridge, MA 02139.  We have not yet\n\
worked out a simple rule that can be stated here, but we will often permit\n\
this.  We will be guided by the two goals of preserving the free status of\n\
all derivatives of our free software and of promoting the sharing and reuse\n\
of software.\n\
\n\
In other words, go ahead and share GDB, but don't try to stop\n\
anyone else from sharing it farther.  Help stamp out software hoarding!\n\
");
  immediate_quit--;
}

static void
warranty_info ()
{
  immediate_quit++;
  printf ("			 NO WARRANTY\n\
\n\
  BECAUSE GDB IS LICENSED FREE OF CHARGE, WE PROVIDE ABSOLUTELY NO\n\
WARRANTY, TO THE EXTENT PERMITTED BY APPLICABLE STATE LAW.  EXCEPT\n\
WHEN OTHERWISE STATED IN WRITING, FREE SOFTWARE FOUNDATION, INC,\n\
RICHARD M. STALLMAN AND/OR OTHER PARTIES PROVIDE GDB \"AS IS\" WITHOUT\n\
WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT\n\
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n\
A PARTICULAR PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND\n\
PERFORMANCE OF GDB IS WITH YOU.  SHOULD GDB PROVE DEFECTIVE, YOU\n\
ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n\n");

  printf ("\
 IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW WILL RICHARD M.\n\
STALLMAN, THE FREE SOFTWARE FOUNDATION, INC., AND/OR ANY OTHER PARTY\n\
WHO MAY MODIFY AND REDISTRIBUTE GDB, BE LIABLE TO\n\
YOU FOR DAMAGES, INCLUDING ANY LOST PROFITS, LOST MONIES, OR OTHER\n\
SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR\n\
INABILITY TO USE (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA\n\
BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY THIRD PARTIES OR A\n\
FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS) GDB, EVEN\n\
IF YOU HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR FOR\n\
ANY CLAIM BY ANY OTHER PARTY.\n");
  immediate_quit--;
}

static void
print_gdb_version ()
{
  printf ("GDB %s, Copyright (C) 1988 Free Software Foundation, Inc.\n\
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

/* Clean up on error during a "source" command.
   Close the file opened by the command
   and restore the previous input stream.  */

static void
source_cleanup (stream)
     FILE *stream;
{
  fclose (instream);
  instream = stream;
}

static void
source_command (file)
     char *file;
{
  FILE *stream;
  struct cleanup *cleanups;

  if (file == 0)
    error_no_arg ("file to read commands from");

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

static void
initialize_main ()
{
  prompt = savestring ("(gdb) ", 6);

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

  add_com ("set-prompt", class_support, set_prompt_command,
	   "Change gdb's prompt from the default of \"(gdb)\"");
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

  add_com ("dump-me", class_obscure, dump_me_command,
	   "Get fatal error; make debugger dump its core.");

  add_prefix_cmd ("info", class_info, info_command,
		  "Generic command for printing status.",
		  &infolist, "info ", 0, &cmdlist);
  add_com_alias ("i", "info", class_info, 1);

  add_info ("copying", copying_info, "Conditions for redistributing copies of GDB.");
  add_info ("warranty", warranty_info, "Various kinds of warranty you do not have.");
  add_info ("version", version_info, "Report what version of GDB this is.");
}
