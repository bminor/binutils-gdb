/* TK interface routines.
   Copyright 1994 Free Software Foundation, Inc.

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
#include "symtab.h"
#include "inferior.h"
#include "command.h"
#include "bfd.h"
#include "symfile.h"
#include "objfiles.h"
#include "target.h"
#include <tcl.h>
#include <tk.h>
#include <varargs.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

/* Non-zero means that we're doing the gdbtk interface. */
int gdbtk = 0;

/* Non-zero means we are reloading breakpoints, etc from the
   Gdbtk kernel, and we should suppress various messages */
static int gdbtk_reloading = 0;

/* Handle for TCL interpreter */
static Tcl_Interp *interp = NULL;

/* Handle for TK main window */
static Tk_Window mainWindow = NULL;

static int x_fd;		/* X network socket */

static void
null_routine(arg)
     int arg;
{
}


/* This routine redirects the output of fputs_unfiltered so that
   the user can see what's going on in his debugger window. */

static char holdbuf[200];
static char *holdbufp = holdbuf;
static int holdfree = sizeof (holdbuf);

static void
flush_holdbuf ()
{
  if (holdbufp == holdbuf)
    return;

  Tcl_VarEval (interp, "gdbtk_tcl_fputs ", "{", holdbuf, "}", NULL);
  holdbufp = holdbuf;
  holdfree = sizeof (holdbuf);
}

static void
gdbtk_flush (stream)
     FILE *stream;
{
  flush_holdbuf ();

  Tcl_VarEval (interp, "gdbtk_tcl_flush", NULL);
}

static void
gdbtk_fputs (ptr)
     const char *ptr;
{
  int len;

  len = strlen (ptr) + 1;

  if (len > holdfree)
    {
      flush_holdbuf ();

      if (len > sizeof (holdbuf))
	{
	  Tcl_VarEval (interp, "gdbtk_tcl_fputs ", "{", ptr, "}", NULL);
	  return;
	}
    }

  strncpy (holdbufp, ptr, len);
  holdbufp += len - 1;
  holdfree -= len - 1;
}

static int
gdbtk_query (args)
     va_list args;
{
  char *query;
  char buf[200];
  long val;

  query = va_arg (args, char *);

  vsprintf(buf, query, args);
  Tcl_VarEval (interp, "gdbtk_tcl_query ", "{", buf, "}", NULL);

  val = atol (interp->result);
  return val;
}

#if 0
static char *
full_filename(symtab)
     struct symtab *symtab;
{
  int pathlen;
  char *filename;

  if (!symtab)
    return NULL;

  if (symtab->fullname)
    return savestring(symtab->fullname, strlen(symtab->fullname));

  if (symtab->filename[0] == '/')
    return savestring(symtab->filename, strlen(symtab->filename));

  if (symtab->dirname)
    pathlen = strlen(symtab->dirname);
  else
    pathlen = 0;
  if (symtab->filename)
    pathlen += strlen(symtab->filename);

  filename = xmalloc(pathlen+1);

  if (symtab->dirname)
    strcpy(filename, symtab->dirname);
  else
    *filename = '\000';
  if (symtab->filename)
    strcat(filename, symtab->filename);

  return filename;
}
#endif

static void
breakpoint_notify(b, action)
     struct breakpoint *b;
     const char *action;
{
  struct symbol *sym;
  char bpnum[50], line[50], pc[50];
  struct symtab_and_line sal;
  char *filename;
  int v;

  if (b->type != bp_breakpoint)
    return;

  sal = find_pc_line (b->address, 0);

  filename = symtab_to_filename (sal.symtab);

  sprintf (bpnum, "%d", b->number);
  sprintf (line, "%d", sal.line);
  sprintf (pc, "0x%x", b->address);
 
  v = Tcl_VarEval (interp,
		   "gdbtk_tcl_breakpoint ",
		   action,
		   " ", bpnum,
		   " ", filename,
		   " ", line,
		   " ", pc,
		   NULL);

  if (v != TCL_OK)
    {
      gdbtk_fputs (interp->result);
      gdbtk_fputs ("\n");
    }
}

static void
gdbtk_create_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify(b, "create");
}

static void
gdbtk_delete_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify(b, "delete");
}

static void
gdbtk_enable_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify(b, "enable");
}

static void
gdbtk_disable_breakpoint(b)
     struct breakpoint *b;
{
  breakpoint_notify(b, "disable");
}

/* This implements the TCL command `gdb_loc', which returns a list consisting
   of the source and line number associated with the current pc. */

static int
gdb_loc (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  char *filename;
  char buf[100];
  struct symtab_and_line sal;
  char *funcname;
  CORE_ADDR pc;

  if (argc == 1)
    {
      struct frame_info *frame;
      struct symbol *func;

      frame = get_frame_info (selected_frame);

      pc = frame ? frame->pc : stop_pc;

      sal = find_pc_line (pc, 0);
    }
  else if (argc == 2)
    {
      struct symtabs_and_lines sals;
      int nelts;

      sals = decode_line_spec (argv[1], 1);

      nelts = sals.nelts;
      sal = sals.sals[0];
      free (sals.sals);

      if (sals.nelts != 1)
	{
	  Tcl_SetResult (interp, "Ambiguous line spec", TCL_STATIC);
	  return TCL_ERROR;
	}

      pc = sal.pc;
    }
  else
    {
      Tcl_SetResult (interp, "wrong # args", TCL_STATIC);
      return TCL_ERROR;
    }

  if (sal.symtab)
    Tcl_AppendElement (interp, sal.symtab->filename);
  else
    Tcl_AppendElement (interp, "");

  find_pc_partial_function (pc, &funcname, NULL, NULL);
  Tcl_AppendElement (interp, funcname);

  filename = symtab_to_filename (sal.symtab);
  Tcl_AppendElement (interp, filename);

  sprintf (buf, "%d", sal.line);
  Tcl_AppendElement (interp, buf); /* line number */

  sprintf (buf, "0x%x", pc);
  Tcl_AppendElement (interp, buf); /* PC */

  return TCL_OK;
}

static int
gdb_cmd_stub (cmd)
     char *cmd;
{
  execute_command (cmd, 1);

  return 1;			/* Indicate success */
}

/* This implements the TCL command `gdb_cmd', which sends it's argument into
   the GDB command scanner.  */

static int
gdb_cmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  int val;
  struct cleanup *old_chain;

  if (argc != 2)
    {
      Tcl_SetResult (interp, "wrong # args", TCL_STATIC);
      return TCL_ERROR;
    }

  old_chain = make_cleanup (null_routine, 0);

  val = catch_errors (gdb_cmd_stub, argv[1], "", RETURN_MASK_ERROR);

  /* In case of an error, we may need to force the GUI into idle mode because
     gdbtk_call_command may have bombed out while in the command routine.  */

  if (val == 0)
    Tcl_VarEval (interp, "gdbtk_tcl_idle", NULL);

  bpstat_do_actions (&stop_bpstat);
  do_cleanups (old_chain);

  /* Drain all buffered command output */

  gdb_flush (gdb_stderr);
  gdb_flush (gdb_stdout);

  /* We could base the return value on val, but that would require most users
     to use catch.  Since GDB errors are already being handled elsewhere, I
     see no reason to pass them up to the caller. */

  return TCL_OK;
}

static int
gdb_listfiles (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  int val;
  struct objfile *objfile;
  struct partial_symtab *psymtab;

  ALL_PSYMTABS (objfile, psymtab)
    Tcl_AppendElement (interp, psymtab->filename);

  return TCL_OK;
}

static int
gdb_stop (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char *argv[];
{
  extern pid_t inferior_process_group;

  /* XXX - This is WRONG for remote targets.  Probably need a target vector
     entry to do this right.  */

  kill (-inferior_process_group, SIGINT);
}


static void
tk_command (cmd, from_tty)
     char *cmd;
     int from_tty;
{
  Tcl_VarEval (interp, cmd, NULL);

  gdbtk_fputs (interp->result);
  gdbtk_fputs ("\n");
}

static void
cleanup_init (ignored)
     int ignored;
{
  if (mainWindow != NULL)
    Tk_DestroyWindow (mainWindow);
  mainWindow = NULL;

  if (interp != NULL)
    Tcl_DeleteInterp (interp);
  interp = NULL;
}

/* Come here during long calculations to check for GUI events.  Usually invoked
   via the QUIT macro.  */

static void
gdbtk_interactive ()
{
  /* Tk_DoOneEvent (TK_DONT_WAIT|TK_IDLE_EVENTS); */
}

/* Come here when there is activity on the X file descriptor. */

static void
x_event (signo)
     int signo;
{
  /* Process pending events */

  while (Tk_DoOneEvent (TK_DONT_WAIT|TK_ALL_EVENTS) != 0);
}

static int
gdbtk_wait (pid, ourstatus)
     int pid;
     struct target_waitstatus *ourstatus;
{
  signal (SIGIO, x_event);

  pid = target_wait (pid, ourstatus);

  signal (SIGIO, SIG_IGN);

  return pid;
}

/* This is called from execute_command, and provides a wrapper around
   various command routines in a place where both protocol messages and
   user input both flow through.  Mostly this is used for indicating whether
   the target process is running or not.
*/

static void
gdbtk_call_command (cmdblk, arg, from_tty)
     struct cmd_list_element *cmdblk;
     char *arg;
     int from_tty;
{
  if (cmdblk->class == class_run)
    {
      Tcl_VarEval (interp, "gdbtk_tcl_busy", NULL);
      (*cmdblk->function.cfunc)(arg, from_tty);
      Tcl_VarEval (interp, "gdbtk_tcl_idle", NULL);
    }
  else
    (*cmdblk->function.cfunc)(arg, from_tty);
}

static void
gdbtk_init ()
{
  struct cleanup *old_chain;
  char *gdbtk_filename;
  int i;

  old_chain = make_cleanup (cleanup_init, 0);

  /* First init tcl and tk. */

  interp = Tcl_CreateInterp ();

  if (!interp)
    error ("Tcl_CreateInterp failed");

  mainWindow = Tk_CreateMainWindow (interp, NULL, "gdb", "Gdb");

  if (!mainWindow)
    return;			/* DISPLAY probably not set */

  if (Tcl_Init(interp) != TCL_OK)
    error ("Tcl_Init failed: %s", interp->result);

  if (Tk_Init(interp) != TCL_OK)
    error ("Tk_Init failed: %s", interp->result);

  Tcl_CreateCommand (interp, "gdb_cmd", gdb_cmd, NULL, NULL);
  Tcl_CreateCommand (interp, "gdb_loc", gdb_loc, NULL, NULL);
  Tcl_CreateCommand (interp, "gdb_listfiles", gdb_listfiles, NULL, NULL);
  Tcl_CreateCommand (interp, "gdb_stop", gdb_stop, NULL, NULL);

  gdbtk_filename = getenv ("GDBTK_FILENAME");
  if (!gdbtk_filename)
    if (access ("gdbtk.tcl", R_OK) == 0)
      gdbtk_filename = "gdbtk.tcl";
    else
      gdbtk_filename = GDBTK_FILENAME;

  if (Tcl_EvalFile (interp, gdbtk_filename) != TCL_OK)
    error ("Failure reading %s: %s", gdbtk_filename, interp->result);

  /* Get the file descriptor for the X server */

  x_fd = ConnectionNumber (Tk_Display (mainWindow));

  /* Setup for I/O interrupts */

  signal (SIGIO, SIG_IGN);

  i = fcntl (x_fd, F_GETFL, 0);
  fcntl (x_fd, F_SETFL, i|FASYNC);
  fcntl (x_fd, F_SETOWN, getpid()); 

  command_loop_hook = Tk_MainLoop;
  fputs_unfiltered_hook = gdbtk_fputs;
  print_frame_info_listing_hook = null_routine;
  query_hook = gdbtk_query;
  flush_hook = gdbtk_flush;
  create_breakpoint_hook = gdbtk_create_breakpoint;
  delete_breakpoint_hook = gdbtk_delete_breakpoint;
  enable_breakpoint_hook = gdbtk_enable_breakpoint;
  disable_breakpoint_hook = gdbtk_disable_breakpoint;
  interactive_hook = gdbtk_interactive;
  target_wait_hook = gdbtk_wait;
  call_command_hook = gdbtk_call_command;

  discard_cleanups (old_chain);

  add_com ("tk", class_obscure, tk_command,
	   "Send a command directly into tk.");
}

/* Come here during initialze_all_files () */

void
_initialize_gdbtk ()
{
  if (use_windows)
    {
      /* Tell the rest of the world that Gdbtk is now set up. */

      init_ui_hook = gdbtk_init;
    }
}
