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

/* start-sanitize-ide */
#ifdef IDE
#include "event.h"
#include "idetcl.h"
#include "ilutk.h"
#endif
/* end-sanitize-ide */

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

#ifdef __CYGWIN32__
#include <sys/cygwin.h> /* for cygwin32_attach_handle_to_fd */
#endif

/* For Cygwin, we use a timer to periodically check for Windows
   messages.  FIXME: It would be better to not poll, but to instead
   rewrite the target_wait routines to serve as input sources.
   Unfortunately, that will be a lot of work.  */
static sigset_t nullsigmask;
static struct sigaction act1, act2;
static struct itimerval it_on, it_off;

 /*
  * These two variables control the interaction with an external editor.
  * If enable_external_editor is set at startup, BEFORE Gdbtk_Init is run
  * then the Tcl variable of the same name will be set, and a command will
  * called external_editor_command will be invoked to call out to the
  * external editor.  We give a dummy version here to warn if it is not set.
  */
int enable_external_editor = 0;
char * external_editor_command = "tk_dialog .warn-external \\\n\
\"No command is specified.\nUse --tclcommand <tcl/file> or --external-editor <cmd> to specify a new command\" 0 Ok";

extern int Tktable_Init PARAMS ((Tcl_Interp *interp)); 

static void gdbtk_init PARAMS ((char *));
void gdbtk_interactive PARAMS ((void));
static void cleanup_init PARAMS ((int));
static void tk_command PARAMS ((char *, int));

void gdbtk_add_hooks PARAMS ((void));
int gdbtk_test PARAMS ((char *));

/*
 * gdbtk_fputs is defined in the gdbtk_hooks.c, but we need it here
 * because we delay adding this hook till all the setup is done.  That
 * way errors will go to stdout.
 */

extern void   gdbtk_fputs PARAMS ((const char *, GDB_FILE *));

/* Handle for TCL interpreter */
Tcl_Interp *gdbtk_interp = NULL;

static int gdbtk_timer_going = 0;

/* linked variable used to tell tcl what the current thread is */
int gdb_context = 0;

/* This variable is true when the inferior is running.  See note in
 * gdbtk.h for details.
 */
int running_now;

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

/* gdbtk_init installs this function as a final cleanup.  */

static void
gdbtk_cleanup (dummy)
     PTR dummy;
{
  Tcl_Eval (gdbtk_interp, "gdbtk_cleanup");
#ifdef IDE
  {
    struct ide_event_handle *h = (struct ide_event_handle *) dummy;
    ide_interface_deregister_all (h);
  }
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
  int found_main;
  char s[5];
  Tcl_Obj *auto_path_elem, *auto_path_name;

  /* start-sanitize-ide */
#ifdef IDE
  struct ide_event_handle *h;
  const char *errmsg;
  char *libexecdir;
#endif 
  /* end-sanitize-ide */

  /* If there is no DISPLAY environment variable, Tk_Init below will fail,
     causing gdb to abort.  If instead we simply return here, gdb will
     gracefully degrade to using the command line interface. */

#ifndef _WIN32
  if (getenv ("DISPLAY") == NULL)
    return;
#endif

  old_chain = make_cleanup ((make_cleanup_func) cleanup_init, 0);

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

  /* Set up some globals used by gdb to pass info to gdbtk
     for start up options and the like */
  sprintf (s, "%d", inhibit_gdbinit);
  Tcl_SetVar2 (gdbtk_interp, "GDBStartup", "inhibit_prefs", s, TCL_GLOBAL_ONLY);

  /* start-sanitize-ide */
#ifndef IDE
  /* end-sanitize-ide */
  /* For the IDE we register the cleanup later, after we've
     initialized events.  */
  make_final_cleanup (gdbtk_cleanup,  NULL);
  /* start-sanitize-ide */
#endif /* IDE */
  /* end-sanitize-ide */

  /* Initialize the Paths variable.  */
  if (ide_initialize_paths (gdbtk_interp, "") != TCL_OK)
    error ("ide_initialize_paths failed: %s", gdbtk_interp->result);

  /* start-sanitize-ide */
#ifdef IDE
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
#else
  /* end-sanitize-ide */
  Tcl_SetVar (gdbtk_interp, "IDE_ENABLED", "0", 0);
  /* start-sanitize-ide */
#endif /* IDE */
  /* end-sanitize-ide */

  /* We don't want to open the X connection until we've done all the
     IDE initialization.  Otherwise, goofy looking unfinished windows
     pop up when ILU drops into the TCL event loop.  */

  if (Tk_Init(gdbtk_interp) != TCL_OK)
    error ("Tk_Init failed: %s", gdbtk_interp->result);

  if (Itcl_Init(gdbtk_interp) == TCL_ERROR) 
    error ("Itcl_Init failed: %s", gdbtk_interp->result);
  Tcl_StaticPackage(gdbtk_interp, "Itcl", Itcl_Init,
		    (Tcl_PackageInitProc *) NULL);  

  if (Itk_Init(gdbtk_interp) == TCL_ERROR) 
    error ("Itk_Init failed: %s", gdbtk_interp->result);
  Tcl_StaticPackage(gdbtk_interp, "Itk", Itk_Init,
		    (Tcl_PackageInitProc *) NULL);  

  if (Tix_Init(gdbtk_interp) != TCL_OK)
    error ("Tix_Init failed: %s", gdbtk_interp->result);
  Tcl_StaticPackage(gdbtk_interp, "Tix", Tix_Init,
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
#else
  /* for now, this testing function is Unix only */
  if (cyg_create_warp_pointer_command (gdbtk_interp) != TCL_OK)
    error ("warp_pointer command initialization failed");
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
  
  /*
   * Set the variables for external editor:
   */
  
  Tcl_SetVar (gdbtk_interp, "enable_external_editor", enable_external_editor ? "1" : "0", 0);
  Tcl_SetVar (gdbtk_interp, "external_editor_command", external_editor_command, 0);

  /* find the gdb tcl library and source main.tcl */

  {
#ifdef NO_TCLPRO_DEBUGGER
    static const char script[] ="\
proc gdbtk_find_main {} {\n\
    global Paths GDBTK_LIBRARY\n\
    rename gdbtk_find_main {}\n\
    tcl_findLibrary gdb 1.0 {} main.tcl GDBTK_LIBRARY GDBTK_LIBRARY gdbtcl2 gdbtcl {}\n\
    set Paths(appdir) $GDBTK_LIBRARY\n\
}\n\
gdbtk_find_main";
#else
    static const char script[] ="\
proc gdbtk_find_main {} {\n\
    global Paths GDBTK_LIBRARY env\n\
    rename gdbtk_find_main {}\n\
    if {[info exists env(DEBUG_STUB)]} {\n\
        source $env(DEBUG_STUB)\n\
        debugger_init\n\
        set debug_startup 1\n\
    } else {\n\
        set debug_startup 0\n\
    }\n\
    tcl_findLibrary gdb 1.0 {} main.tcl GDBTK_LIBRARY GDBTK_LIBRARY gdbtcl2 gdbtcl {} $debug_startup\n\
    set Paths(appdir) $GDBTK_LIBRARY\n\
}\n\
gdbtk_find_main";
#endif /* NO_TCLPRO_DEBUGGER */
    
    /* fputs_unfiltered_hook = NULL; */ /* Force errors to stdout/stderr */
    
    /*
     * Set the variables for external editor, do this before eval'ing main.tcl
     * since the value is used there...
     */
    
    Tcl_SetVar (gdbtk_interp, "enable_external_editor",
		enable_external_editor ? "1" : "0", 0);
    Tcl_SetVar (gdbtk_interp, "external_editor_command",
		external_editor_command, 0);
    
  fputs_unfiltered_hook = gdbtk_fputs;

  if (Tcl_GlobalEval (gdbtk_interp, (char *) script) != TCL_OK)
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
  }

  
  /* start-sanitize-ide */
#ifdef IDE
  /* Don't do this until we have initialized.  Otherwise, we may get a
     run command before we are ready for one.  */
  if (ide_run_server_init (gdbtk_interp, h) != TCL_OK)
    error ("ide_run_server_init failed: %s", gdbtk_interp->result);
#endif
  /* end-sanitize-ide */

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

/* Local variables: */
/* change-log-default-name: "ChangeLog-gdbtk" */
/* End: */

