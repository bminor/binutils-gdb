/* Startup code for gdbtk.
   Copyright 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.

   Written by Stu Grossman <grossman@cygnus.com> of Cygnus Support.

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
#include "inferior.h"
#include "command.h"
#include "bfd.h"
#include "symfile.h"
#include "objfiles.h"
#include "target.h"
#include "gdbcore.h"
#include "tracepoint.h"
#include "demangle.h"

#ifdef _WIN32
#include <winuser.h>
#endif

#include <sys/stat.h>

#include <tcl.h>
#include <tk.h>
#include <itcl.h> 
#include <tix.h> 
#include "guitcl.h"
#include "gdbtk.h"

#ifdef IDE
/* start-sanitize-ide */
#include "event.h"
#include "idetcl.h"
#include "ilutk.h"
/* end-sanitize-ide */
#endif

#ifdef ANSI_PROTOTYPES
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <setjmp.h>
#include "top.h"
#include <sys/ioctl.h>
#include "gdb_string.h"
#include "dis-asm.h"
#include <stdio.h>
#include "gdbcmd.h"

#include "annotate.h"
#include <sys/time.h>

/* For Cygwin32, we use a timer to periodically check for Windows
   messages.  FIXME: It would be better to not poll, but to instead
   rewrite the target_wait routines to serve as input sources.
   Unfortunately, that will be a lot of work.  */
static sigset_t nullsigmask;
static struct sigaction act1, act2;
static struct itimerval it_on, it_off;

extern int Tktable_Init PARAMS ((Tcl_Interp *interp)); 

static void null_routine PARAMS ((int));
static void gdbtk_init PARAMS ((char *));
void gdbtk_interactive PARAMS ((void));
static void cleanup_init PARAMS ((int));
static void tk_command PARAMS ((char *, int));

int gdbtk_test PARAMS ((char *));

/*
 * gdbtk_fputs is defined in the gdbtk_hooks.c, but we need it here
 * because we delay adding this hook till all the setup is done.  That
 * way errors will go to stdout.
 */

extern void   gdbtk_fputs PARAMS ((const char *, FILE *));

/* Handle for TCL interpreter */
Tcl_Interp *gdbtk_interp = NULL;

static int gdbtk_timer_going = 0;

/* This variable is true when the inferior is running.  See note in
 * gdbtk.h for details.
 */

int running_now;

/* This variable determines where memory used for disassembly is read from.
 * See note in gdbtk.h for details.
 */

int disassemble_from_exec = -1;

/* This variable holds the name of a Tcl file which should be sourced by the
   interpreter when it goes idle at startup. Used with the testsuite. */

static char *gdbtk_source_filename = NULL;

#ifndef _WIN32

/* Supply malloc calls for tcl/tk.  We do not want to do this on
   Windows, because Tcl_Alloc is probably in a DLL which will not call
   the mmalloc routines.  */

char *
Tcl_Alloc (size)
     unsigned int size;
{
  return xmalloc (size);
}

char *
Tcl_Realloc (ptr, size)
     char *ptr;
     unsigned int size;
{
  return xrealloc (ptr, size);
}

void
Tcl_Free(ptr)
     char *ptr;
{
  free (ptr);
}

#endif /* ! _WIN32 */

static void
null_routine(arg)
     int arg;
{
}

#ifdef _WIN32

/* On Windows, if we hold a file open, other programs can't write to
 * it.  In particular, we don't want to hold the executable open,
 * because it will mean that people have to get out of the debugging
 * session in order to remake their program.  So we close it, although
 * this will cost us if and when we need to reopen it.
 */

void
close_bfds ()
{
  struct objfile *o;

  ALL_OBJFILES (o)
    {
      if (o->obfd != NULL)
	bfd_cache_close (o->obfd);
    }

  if (exec_bfd != NULL)
    bfd_cache_close (exec_bfd);
}

#endif /* _WIN32 */


/* TclDebug (const char *fmt, ...) works just like printf() but 
 * sends the output to the GDB TK debug window. 
 * Not for normal use; just a convenient tool for debugging
 */

void
#ifdef ANSI_PROTOTYPES
TclDebug (const char *fmt, ...)
#else
TclDebug (va_alist)
     va_dcl
#endif
{
  va_list args;
  char buf[512], *v[2], *merge;

#ifdef ANSI_PROTOTYPES
  va_start (args, fmt);
#else
  char *fmt;
  va_start (args);
  fmt = va_arg (args, char *);
#endif

  v[0] = "debug";
  v[1] = buf;

  vsprintf (buf, fmt, args);
  va_end (args);

  merge = Tcl_Merge (2, v);
  Tcl_Eval (gdbtk_interp, merge);
  Tcl_Free (merge);
}

	   
/*
 * The rest of this file contains the start-up, and event handling code for gdbtk.
 */
	   
/*
 * This cleanup function is added to the cleanup list that surrounds the Tk
 * main in gdbtk_init.  It deletes the Tcl interpreter.
 */
 
static void
cleanup_init (ignored)
     int ignored;
{
  if (gdbtk_interp != NULL)
    Tcl_DeleteInterp (gdbtk_interp);
  gdbtk_interp = NULL;
}

/* Come here during long calculations to check for GUI events.  Usually invoked
   via the QUIT macro.  */

void
gdbtk_interactive ()
{
  /* Tk_DoOneEvent (TK_DONT_WAIT|TK_IDLE_EVENTS); */
}


void
gdbtk_start_timer ()
{
  static int first = 1;
  /*TclDebug ("Starting timer....");*/  
  if (first)
    {
      /* first time called, set up all the structs */
      first = 0;
      sigemptyset (&nullsigmask);

      act1.sa_handler = x_event;
      act1.sa_mask = nullsigmask;
      act1.sa_flags = 0;

      act2.sa_handler = SIG_IGN;
      act2.sa_mask = nullsigmask;
      act2.sa_flags = 0;

      it_on.it_interval.tv_sec = 0;
      it_on.it_interval.tv_usec = 250000; /* .25 sec */
      it_on.it_value.tv_sec = 0;
      it_on.it_value.tv_usec = 250000;

      it_off.it_interval.tv_sec = 0;
      it_off.it_interval.tv_usec = 0;
      it_off.it_value.tv_sec = 0;
      it_off.it_value.tv_usec = 0;
    }
  
  if (!gdbtk_timer_going)
    {
      sigaction (SIGALRM, &act1, NULL);
      setitimer (ITIMER_REAL, &it_on, NULL);
      gdbtk_timer_going = 1;
    }
}

void
gdbtk_stop_timer ()
{
  if (gdbtk_timer_going)
    {
      gdbtk_timer_going = 0;
      /*TclDebug ("Stopping timer.");*/
      setitimer (ITIMER_REAL, &it_off, NULL);
      sigaction (SIGALRM, &act2, NULL);
    }
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
  running_now = 0;
  if (cmdblk->class == class_run || cmdblk->class == class_trace)
    {

/* HACK! HACK! This is to get the gui to update the tstart/tstop
   button only incase of tstart/tstop commands issued from the console
   We don't want to update the src window, so we need to have specific
   procedures to do tstart and tstop
   Unfortunately this will not display errors from tstart or tstop in the 
   console window itself, but as dialogs.*/

      if (!strcmp(cmdblk->name, "tstart") && !No_Update)
        {
              Tcl_Eval (gdbtk_interp, "gdbtk_tcl_tstart"); 
              (*cmdblk->function.cfunc)(arg, from_tty);
        }
      else if (!strcmp(cmdblk->name, "tstop") && !No_Update) 
             {
              Tcl_Eval (gdbtk_interp, "gdbtk_tcl_tstop"); 
              (*cmdblk->function.cfunc)(arg, from_tty);
             }
/* end of hack */
           else 
             {
                 running_now = 1;
                 if (!No_Update)
                   Tcl_Eval (gdbtk_interp, "gdbtk_tcl_busy");
                 (*cmdblk->function.cfunc)(arg, from_tty);
                 running_now = 0;
                 if (!No_Update)
                   Tcl_Eval (gdbtk_interp, "gdbtk_tcl_idle");
             }
    }
  else
    (*cmdblk->function.cfunc)(arg, from_tty);
}

/* gdbtk_init installs this function as a final cleanup.  */

static void
gdbtk_cleanup (dummy)
     PTR dummy;
{
  Tcl_Eval (gdbtk_interp, "gdbtk_cleanup");
#ifdef IDE
  struct ide_event_handle *h = (struct ide_event_handle *) dummy;
  ide_interface_deregister_all (h);
#endif
  Tcl_Finalize ();
}

/* Initialize gdbtk.  This involves creating a Tcl interpreter,
 * defining all the Tcl commands that the GUI will use, pointing
 * all the gdb "hooks" to the correct functions,
 * and setting the Tcl auto loading environment so that we can find all
 * the Tcl based library files.
 */

static void
gdbtk_init ( argv0 )
     char *argv0;
{
  struct cleanup *old_chain;
  char *lib, *gdbtk_lib, *gdbtk_lib_tmp, *gdbtk_file;
  int i, found_main;
  Tcl_Obj *auto_path_elem, *auto_path_name;
#ifndef WINNT
  struct sigaction action;
  static sigset_t nullsigmask = {0};
#endif
#ifdef IDE
  /* start-sanitize-ide */
  struct ide_event_handle *h;
  const char *errmsg;
  char *libexecdir;
  /* end-sanitize-ide */
#endif 

  /* If there is no DISPLAY environment variable, Tk_Init below will fail,
     causing gdb to abort.  If instead we simply return here, gdb will
     gracefully degrade to using the command line interface. */

#ifndef WINNT
  if (getenv ("DISPLAY") == NULL)
    return;
#endif

  old_chain = make_cleanup (cleanup_init, 0);

  /* First init tcl and tk. */
  Tcl_FindExecutable (argv0); 
  gdbtk_interp = Tcl_CreateInterp ();

#ifdef TCL_MEM_DEBUG
  Tcl_InitMemory (gdbtk_interp);
#endif

  if (!gdbtk_interp)
    error ("Tcl_CreateInterp failed");

  if (Tcl_Init(gdbtk_interp) != TCL_OK)
    error ("Tcl_Init failed: %s", gdbtk_interp->result);

#ifndef IDE
  /* For the IDE we register the cleanup later, after we've
     initialized events.  */
  make_final_cleanup (gdbtk_cleanup,  NULL);
#endif

  /* Initialize the Paths variable.  */
  if (ide_initialize_paths (gdbtk_interp, "gdbtcl") != TCL_OK)
    error ("ide_initialize_paths failed: %s", gdbtk_interp->result);

#ifdef IDE
  /* start-sanitize-ide */
  /* Find the directory where we expect to find idemanager.  We ignore
     errors since it doesn't really matter if this fails.  */
  libexecdir = Tcl_GetVar2 (gdbtk_interp, "Paths", "libexecdir", TCL_GLOBAL_ONLY);

  IluTk_Init ();

  h = ide_event_init_from_environment (&errmsg, libexecdir);
  make_final_cleanup (gdbtk_cleanup, h);
  if (h == NULL)
    {
      Tcl_AppendResult (gdbtk_interp, "can't initialize event system: ", errmsg,
			(char *) NULL);
      fprintf(stderr, "WARNING: ide_event_init_client failed: %s\n", gdbtk_interp->result);

      Tcl_SetVar (gdbtk_interp, "IDE_ENABLED", "0", 0);
    }
  else 
    {
      if (ide_create_tclevent_command (gdbtk_interp, h) != TCL_OK)
	error ("ide_create_tclevent_command failed: %s", gdbtk_interp->result);

      if (ide_create_edit_command (gdbtk_interp, h) != TCL_OK)
	error ("ide_create_edit_command failed: %s", gdbtk_interp->result);
      
      if (ide_create_property_command (gdbtk_interp, h) != TCL_OK)
	error ("ide_create_property_command failed: %s", gdbtk_interp->result);

      if (ide_create_build_command (gdbtk_interp, h) != TCL_OK)
	error ("ide_create_build_command failed: %s", gdbtk_interp->result);

      if (ide_create_window_register_command (gdbtk_interp, h, "gdb-restore")
	  != TCL_OK)
	error ("ide_create_window_register_command failed: %s",
	       gdbtk_interp->result);

      if (ide_create_window_command (gdbtk_interp, h) != TCL_OK)
	error ("ide_create_window_command failed: %s", gdbtk_interp->result);

      if (ide_create_exit_command (gdbtk_interp, h) != TCL_OK)
	error ("ide_create_exit_command failed: %s", gdbtk_interp->result);

      if (ide_create_help_command (gdbtk_interp) != TCL_OK)
	error ("ide_create_help_command failed: %s", gdbtk_interp->result);

      /*
	if (ide_initialize (gdbtk_interp, "gdb") != TCL_OK)
	error ("ide_initialize failed: %s", gdbtk_interp->result);
      */

      Tcl_SetVar (gdbtk_interp, "IDE_ENABLED", "1", 0);
    }
  /* end-sanitize-ide */
#else
  Tcl_SetVar (gdbtk_interp, "IDE_ENABLED", "0", 0);
#endif /* IDE */

  /* We don't want to open the X connection until we've done all the
     IDE initialization.  Otherwise, goofy looking unfinished windows
     pop up when ILU drops into the TCL event loop.  */

  if (Tk_Init(gdbtk_interp) != TCL_OK)
    error ("Tk_Init failed: %s", gdbtk_interp->result);

  if (Itcl_Init(gdbtk_interp) == TCL_ERROR) 
    error ("Itcl_Init failed: %s", gdbtk_interp->result);
  Tcl_StaticPackage(gdbtk_interp, "Tktable", Tktable_Init,
		    (Tcl_PackageInitProc *) NULL);  

  if (Tix_Init(gdbtk_interp) != TCL_OK)
    error ("Tix_Init failed: %s", gdbtk_interp->result);
  Tcl_StaticPackage(gdbtk_interp, "Tktable", Tktable_Init,
		    (Tcl_PackageInitProc *) NULL);  

  if (Tktable_Init(gdbtk_interp) != TCL_OK)
    error ("Tktable_Init failed: %s", gdbtk_interp->result);
  
  Tcl_StaticPackage(gdbtk_interp, "Tktable", Tktable_Init,
		    (Tcl_PackageInitProc *) NULL);  
  /*
   * These are the commands to do some Windows Specific stuff...
   */
  
#ifdef __CYGWIN32__
  if (ide_create_messagebox_command (gdbtk_interp) != TCL_OK)
    error ("messagebox command initialization failed");
  /* On Windows, create a sizebox widget command */
  if (ide_create_sizebox_command (gdbtk_interp) != TCL_OK)
    error ("sizebox creation failed");
  if (ide_create_winprint_command (gdbtk_interp) != TCL_OK)
    error ("windows print code initialization failed");
  /* start-sanitize-ide */
  /* An interface to ShellExecute.  */
  if (ide_create_shell_execute_command (gdbtk_interp) != TCL_OK)
    error ("shell execute command initialization failed");
  /* end-sanitize-ide */
  if (ide_create_win_grab_command (gdbtk_interp) != TCL_OK)
    error ("grab support command initialization failed");
  /* Path conversion functions.  */
  if (ide_create_cygwin_path_command (gdbtk_interp) != TCL_OK)
    error ("cygwin path command initialization failed");
#endif

  /*
   * This adds all the Gdbtk commands.
   */
  
  if (Gdbtk_Init(gdbtk_interp) != TCL_OK)
    {
       error("Gdbtk_Init failed: %s", gdbtk_interp->result);
    }

  Tcl_StaticPackage(gdbtk_interp, "Gdbtk", Gdbtk_Init, NULL);
  
  /* This adds all the hooks that call up from the bowels of gdb
   *  back into Tcl-land...
   */

  gdbtk_add_hooks();
  
  /* Add a back door to Tk from the gdb console... */

  add_com ("tk", class_obscure, tk_command,
	   "Send a command directly into tk.");

  Tcl_LinkVar (gdbtk_interp, "disassemble-from-exec", (char *) &disassemble_from_exec,
	       TCL_LINK_INT);

  /* find the gdb tcl library and source main.tcl */

  gdbtk_lib = getenv ("GDBTK_LIBRARY");
  if (!gdbtk_lib)
    if (access ("gdbtcl/main.tcl", R_OK) == 0)
      gdbtk_lib = "gdbtcl";
    else
      gdbtk_lib = GDBTK_LIBRARY;

  gdbtk_lib_tmp = xstrdup (gdbtk_lib);

  found_main = 0;
  /* see if GDBTK_LIBRARY is a path list */
  lib = strtok (gdbtk_lib_tmp, GDBTK_PATH_SEP);

  auto_path_name = Tcl_NewStringObj ("auto_path", -1);

  do
    {
      auto_path_elem = Tcl_NewStringObj (lib, -1);
      if (Tcl_ObjSetVar2 (gdbtk_interp, auto_path_name, NULL, auto_path_elem,
			  TCL_GLOBAL_ONLY | TCL_APPEND_VALUE | TCL_LIST_ELEMENT ) == NULL)
	{
	  fputs_unfiltered (Tcl_GetVar (gdbtk_interp, "errorInfo", 0), gdb_stderr);
	  error ("");
	}
      if (!found_main)
	{
	  gdbtk_file = concat (lib, "/main.tcl", (char *) NULL);
	  if (access (gdbtk_file, R_OK) == 0)
	    {
	      found_main++;
	      Tcl_SetVar (gdbtk_interp, "GDBTK_LIBRARY", lib, 0);
	    }
	}
     } 
  while ((lib = strtok (NULL, ":")) != NULL);

  free (gdbtk_lib_tmp);
  Tcl_DecrRefCount(auto_path_name);

  if (!found_main)
    {
      /* Try finding it with the auto path.  */

      static const char script[] ="\
proc gdbtk_find_main {} {\n\
  global auto_path GDBTK_LIBRARY\n\
  foreach dir $auto_path {\n\
    set f [file join $dir main.tcl]\n\
    if {[file exists $f]} then {\n\
      set GDBTK_LIBRARY $dir\n\
      return $f\n\
    }\n\
  }\n\
  return ""\n\
}\n\
gdbtk_find_main";

      if (Tcl_GlobalEval (gdbtk_interp, (char *) script) != TCL_OK)
	{
	  fputs_unfiltered (Tcl_GetVar (gdbtk_interp, "errorInfo", 0), gdb_stderr);
	  error ("");
	}

      if (gdbtk_interp->result[0] != '\0')
	{
	  gdbtk_file = xstrdup (gdbtk_interp->result);
	  found_main++;
	}
    }

  if (!found_main)
    {
      fputs_unfiltered_hook = NULL; /* Force errors to stdout/stderr */
      if (getenv("GDBTK_LIBRARY"))
	{
	  fprintf_unfiltered (stderr, "Unable to find main.tcl in %s\n",getenv("GDBTK_LIBRARY"));
	  fprintf_unfiltered (stderr, 
			      "Please set GDBTK_LIBRARY to a path that includes the GDB tcl files.\n");
	}
      else
	{
	  fprintf_unfiltered (stderr, "Unable to find main.tcl in %s\n", GDBTK_LIBRARY);
	  fprintf_unfiltered (stderr, "You might want to set GDBTK_LIBRARY\n");	  
	}
      error("");
    }

/* Defer setup of fputs_unfiltered_hook to near the end so that error messages
   prior to this point go to stdout/stderr.  */

  fputs_unfiltered_hook = gdbtk_fputs;

/* start-sanitize-tclpro */
#ifdef TCLPRO_DEBUGGER
  {
    Tcl_DString source_cmd;

    Tcl_DStringInit (&source_cmd);
    Tcl_DStringAppend (&source_cmd,
		      "if {[info exists env(DEBUG_STUB)]} {source $env(DEBUG_STUB); " -1);
    Tcl_DStringAppend (&source_cmd, "debugger_init; debugger_eval {source {", -1);
    Tcl_DStringAppend (&source_cmd, gdbtk_file, -1);
    Tcl_DStringAppend (&source_cmd, "}}} else {source {", -1);
    Tcl_DStringAppend (&source_cmd, gdbtk_file, -1);
    Tcl_DStringAppend (&source_cmd, "}}", -1);
    if (Tcl_GlobalEval (gdbtk_interp, Tcl_DStringValue (&source_cmd)) != TCL_OK)
#else
/* end-sanitize-tclpro */
      if (Tcl_EvalFile (gdbtk_interp, gdbtk_file) != TCL_OK)
/* start-sanitize-tclpro */
#endif
/* end-sanitize-tclpro */
	{
      char *msg;

      /* Force errorInfo to be set up propertly.  */
      Tcl_AddErrorInfo (gdbtk_interp, "");

      msg = Tcl_GetVar (gdbtk_interp, "errorInfo", TCL_GLOBAL_ONLY);

      fputs_unfiltered_hook = NULL; /* Force errors to stdout/stderr */

#ifdef _WIN32
      MessageBox (NULL, msg, NULL, MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
      fputs_unfiltered (msg, gdb_stderr);
#endif

      error ("");
    }
/* start-sanitize-tclpro */
#ifdef TCLPRO_DEBUGGER
      Tcl_DStringFree(&source_cmd);
    }
#endif
/* end-sanitize-tclpro */
  
#ifdef IDE
  /* start-sanitize-ide */
  /* Don't do this until we have initialized.  Otherwise, we may get a
     run command before we are ready for one.  */
  if (ide_run_server_init (gdbtk_interp, h) != TCL_OK)
    error ("ide_run_server_init failed: %s", gdbtk_interp->result);
  /* end-sanitize-ide */
#endif

  free (gdbtk_file);

  /* Now source in the filename provided by the --tclcommand option.
     This is mostly used for the gdbtk testsuite... */
  
  if (gdbtk_source_filename != NULL)
    {
      char *s = "after idle source ";
      char *script = concat (s, gdbtk_source_filename, (char *) NULL);
      Tcl_Eval (gdbtk_interp, script);
      free (gdbtk_source_filename);
      free (script);
    }
   

  discard_cleanups (old_chain);
}

/* gdbtk_test is used in main.c to validate the -tclcommand option to
   gdb, which sources in a file of tcl code after idle during the
   startup procedure. */
  
int
gdbtk_test (filename)
     char *filename;
{
  if (access (filename, R_OK) != 0)
    return 0;
  else
    gdbtk_source_filename = xstrdup (filename);
  return 1;
}
 
/* Come here during initialize_all_files () */

void
_initialize_gdbtk ()
{
  if (use_windows)
    {
      /* Tell the rest of the world that Gdbtk is now set up. */

      init_ui_hook = gdbtk_init;
#ifdef __CYGWIN32__
      (void) FreeConsole ();
#endif
    }
#ifdef __CYGWIN32__
  else
    {
      DWORD ft = GetFileType (GetStdHandle (STD_INPUT_HANDLE));
      void cygwin32_attach_handle_to_fd (char *, int, HANDLE, int, int);

      switch (ft)
	{
	  case FILE_TYPE_DISK:
	  case FILE_TYPE_CHAR:
	  case FILE_TYPE_PIPE:
	    break;
	  default:
	    AllocConsole();
	    cygwin32_attach_handle_to_fd ("/dev/conin", 0,
					  GetStdHandle (STD_INPUT_HANDLE),
					  1, GENERIC_READ);
	    cygwin32_attach_handle_to_fd ("/dev/conout", 1,
					  GetStdHandle (STD_OUTPUT_HANDLE),
					  0, GENERIC_WRITE);
	    cygwin32_attach_handle_to_fd ("/dev/conout", 2,
					  GetStdHandle (STD_ERROR_HANDLE),
					  0, GENERIC_WRITE);
	    break;
	}
    }
#endif
}

static void
tk_command (cmd, from_tty)
     char *cmd;
     int from_tty;
{
  int retval;
  char *result;
  struct cleanup *old_chain;

  /* Catch case of no argument, since this will make the tcl interpreter dump core. */
  if (cmd == NULL)
    error_no_arg ("tcl command to interpret");

  retval = Tcl_Eval (gdbtk_interp, cmd);

  result = strdup (gdbtk_interp->result);

  old_chain = make_cleanup (free, result);

  if (retval != TCL_OK)
    error (result);

  printf_unfiltered ("%s\n", result);

  do_cleanups (old_chain);
}

