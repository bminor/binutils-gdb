/* Target-vector operations for controlling win32 child processes, for GDB.
   Copyright 1995
   Free Software Foundation, Inc.

   Contributed by Cygnus Support.
   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without eve nthe implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* by Steve Chamberlain, sac@cygnus.com */

#include "defs.h"
#include "frame.h"		/* required by inferior.h */
#include "inferior.h"
#include "target.h"
#include "wait.h"
#include "gdbcore.h"
#include "command.h"
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <windows.h>
#include "buildsym.h"
#include "gdb_string.h"
#include "thread.h"
#include "gdbcmd.h"

#define CHECK(x) check (x, __FILE__,__LINE__)
#define DEBUG(x) if (remote_debug) printf x


/* Forward declaration */
extern struct target_ops child_ops;

/* The most recently read context. Inspect ContextFlags to see what 
   bits are valid. */

static CONTEXT context;

/* The process and thread handles for the above context. */

static HANDLE current_process;
static HANDLE current_thread;
static int current_process_id;
static int current_thread_id;

/* Counts of things. */
static int exception_count = 0;
static int event_count = 0;

/* User options. */
static int new_console = 0;
static int new_group = 0;

/* This vector maps GDB's idea of a register's number into an address
   in the win32 exception context vector. 

   It also contains the bit mask needed to load the register in question.  

   One day we could read a reg, we could inspect the context we
   already have loaded, if it doesn't have the bit set that we need,
   we read that set of registers in using GetThreadContext.  If the
   context already contains what we need, we just unpack it. Then to
   write a register, first we have to ensure that the context contains
   the other regs of the group, and then we copy the info in and set
   out bit. */

struct regmappings
  {
    char *incontext;
    int mask;
  };

static const struct regmappings
  mappings[] =
{
  {(char *) &context.Eax, CONTEXT_INTEGER},
  {(char *) &context.Ecx, CONTEXT_INTEGER},
  {(char *) &context.Edx, CONTEXT_INTEGER},
  {(char *) &context.Ebx, CONTEXT_INTEGER},
  {(char *) &context.Esp, CONTEXT_CONTROL},
  {(char *) &context.Ebp, CONTEXT_CONTROL},
  {(char *) &context.Esi, CONTEXT_INTEGER},
  {(char *) &context.Edi, CONTEXT_INTEGER},
  {(char *) &context.Eip, CONTEXT_CONTROL},
  {(char *) &context.EFlags, CONTEXT_CONTROL},
  {(char *) &context.SegCs, CONTEXT_SEGMENTS},
  {(char *) &context.SegSs, CONTEXT_SEGMENTS},
  {(char *) &context.SegDs, CONTEXT_SEGMENTS},
  {(char *) &context.SegEs, CONTEXT_SEGMENTS},
  {(char *) &context.SegFs, CONTEXT_SEGMENTS},
  {(char *) &context.SegGs, CONTEXT_SEGMENTS},
  {&context.FloatSave.RegisterArea[0 * 10], CONTEXT_FLOATING_POINT},
  {&context.FloatSave.RegisterArea[1 * 10], CONTEXT_FLOATING_POINT},
  {&context.FloatSave.RegisterArea[2 * 10], CONTEXT_FLOATING_POINT},
  {&context.FloatSave.RegisterArea[3 * 10], CONTEXT_FLOATING_POINT},
  {&context.FloatSave.RegisterArea[4 * 10], CONTEXT_FLOATING_POINT},
  {&context.FloatSave.RegisterArea[5 * 10], CONTEXT_FLOATING_POINT},
  {&context.FloatSave.RegisterArea[6 * 10], CONTEXT_FLOATING_POINT},
  {&context.FloatSave.RegisterArea[7 * 10], CONTEXT_FLOATING_POINT},
};


/* This vector maps the target's idea of an exception (extracted
   from the DEBUG_EVENT structure) to GDB's idea. */

struct xlate_exception
  {
    int them;
    enum target_signal us;
  };


static const struct xlate_exception
  xlate[] =
{
  {EXCEPTION_ACCESS_VIOLATION, TARGET_SIGNAL_SEGV},
  {EXCEPTION_BREAKPOINT, TARGET_SIGNAL_TRAP},
  {DBG_CONTROL_C, TARGET_SIGNAL_INT},
  {EXCEPTION_SINGLE_STEP, TARGET_SIGNAL_TRAP},
  {-1, -1}};


static void 
check (BOOL ok, const char *file, int line)
{
  if (!ok)
    printf_filtered ("error return %s:%d was %d\n", file, line, GetLastError ());
}

static void
child_fetch_inferior_registers (int r)
{
  if (r < 0)
    {
      for (r = 0; r < NUM_REGS; r++)
	child_fetch_inferior_registers (r);
    }
  else
    {
      supply_register (r, mappings[r].incontext);
    }
}

static void
child_store_inferior_registers (int r)
{
  if (r < 0)
    {
      for (r = 0; r < NUM_REGS; r++)
	child_store_inferior_registers (r);
    }
  else
    {
      read_register_gen (r, mappings[r].incontext);
    }
}


/* Wait for child to do something.  Return pid of child, or -1 in case
   of error; store status through argument pointer OURSTATUS.  */


static void 
handle_load_dll (DEBUG_EVENT * event)
{
  DWORD dll_name_ptr;
  DWORD done;

  ReadProcessMemory (current_process,
		     (DWORD) event->u.LoadDll.lpImageName,
		     (char *) &dll_name_ptr,
		     sizeof (dll_name_ptr), &done);

  /* See if we could read the address of a string, and that the 
     address isn't null. */

  if (done == sizeof (dll_name_ptr) && dll_name_ptr)
    {
      char *dll_name;
      int size = event->u.LoadDll.fUnicode ? sizeof (WCHAR) : sizeof (char);
      int len = 0;
      char b[2];
      do
	{
	  ReadProcessMemory (current_process,
			     dll_name_ptr + len * size,
			     &b,
			     size,
			     &done);
	  len++;
	}
      while ((b[0] != 0 || b[size - 1] != 0) && done == size);


      dll_name = alloca (len);

      if (event->u.LoadDll.fUnicode)
	{
	  WCHAR *unicode_dll_name = (WCHAR *) alloca (len * sizeof (WCHAR));
	  ReadProcessMemory (current_process,
			     dll_name_ptr,
			     unicode_dll_name,
			     len * sizeof (WCHAR),
			     &done);

	  WideCharToMultiByte (CP_ACP, 0,
			       unicode_dll_name, len,
			       dll_name, len, 0, 0);
	}
      else
	{
	  ReadProcessMemory (current_process,
			     dll_name_ptr,
			     dll_name,
			     len,
			     &done);
	}

      /* FIXME!! It would be nice to define one symbol which pointed to the 
         front of the dll if we can't find any symbols. */

      context.ContextFlags = CONTEXT_FULL;
      GetThreadContext (current_thread, &context);

      symbol_file_add (dll_name, 0, (int) event->u.LoadDll.lpBaseOfDll, 0, 0, 0);

      /* We strip off the path of the dll for tidiness. */
      if (strrchr (dll_name, '\\'))
	dll_name = strrchr (dll_name, '\\') + 1;
      printf_unfiltered ("%x:%s\n", event->u.LoadDll.lpBaseOfDll, dll_name);
    }
}


static void
handle_exception (DEBUG_EVENT * event, struct target_waitstatus *ourstatus)
{
  int i;
  int done = 0;
  ourstatus->kind = TARGET_WAITKIND_STOPPED;

  for (i = 0; !done && xlate[i].us > 0; i++)
    {
      if (xlate[i].them == event->u.Exception.ExceptionRecord.ExceptionCode)
	{
	  ourstatus->value.sig = xlate[i].us;
	  done = 1;
	  break;
	}
    }

  if (!done)
    {
      printf_unfiltered ("Want to know about exception code %08x\n",
			 event->u.Exception.ExceptionRecord.ExceptionCode);
      ourstatus->value.sig = TARGET_SIGNAL_UNKNOWN;
    }
  context.ContextFlags = CONTEXT_FULL;
  GetThreadContext (current_thread, &context);
  exception_count++;
}

static int
child_wait (int pid, struct target_waitstatus *ourstatus)
{
  /* We loop when we get a non-standard exception rather than return
     with a SPURIOUS because resume can try and step or modify things,
     which needs a current_thread.  But some of these exceptions mark
     the birth or death of threads, which mean that the current thread
     isn't necessarily what you think it is. */

  while (1)
    {
      DEBUG_EVENT event;
      BOOL t = WaitForDebugEvent (&event, INFINITE);

      DEBUG (("%d = WaitForDebugEvent() code=%d pid=%d tid=%d)\n",
	      t,
	      event.dwDebugEventCode,
	      event.dwProcessId,
	      event.dwThreadId));

      event_count++;

      current_thread_id = event.dwThreadId;
      current_process_id = event.dwProcessId;

      switch (event.dwDebugEventCode)
	{
	case CREATE_THREAD_DEBUG_EVENT:
	case EXIT_THREAD_DEBUG_EVENT:
	case CREATE_PROCESS_DEBUG_EVENT:
	  break;

	case EXIT_PROCESS_DEBUG_EVENT:
	  ourstatus->kind = TARGET_WAITKIND_EXITED;
	  ourstatus->value.integer = event.u.ExitProcess.dwExitCode;
	  CloseHandle (current_process);
	  CloseHandle (current_thread);
	  return current_process_id;
	  break;

	case LOAD_DLL_DEBUG_EVENT:
	  handle_load_dll (&event);
	  break;
	case EXCEPTION_DEBUG_EVENT:
	  handle_exception (&event, ourstatus);
	  return current_process_id;
	default:
	  printf_unfiltered ("waitfor it %d %d %d %d\n", t,
			     event.dwDebugEventCode,
			     event.dwProcessId,
			     event.dwThreadId);
	  break;
	}
      CHECK (ContinueDebugEvent (current_process_id,
				 current_thread_id,
				 DBG_CONTINUE));
    }
}




/* Attach to process PID, then initialize for debugging it.  */

static void
child_attach (args, from_tty)
     char *args;
     int from_tty;
{
  BOOL ok;

  if (!args)
    error_no_arg ("process-id to attach");

  current_process_id = strtoul (args, 0, 0);

  ok = DebugActiveProcess (current_process_id);

  if (!ok)
    error ("Can't attach to process.");


  exception_count = 0;
  event_count = 0;

  if (from_tty)
    {
      char *exec_file = (char *) get_exec_file (0);

      if (exec_file)
	printf_unfiltered ("Attaching to program `%s', %s\n", exec_file,
			   target_pid_to_str (current_process_id));
      else
	printf_unfiltered ("Attaching to %s\n",
			   target_pid_to_str (current_process_id));

      gdb_flush (gdb_stdout);
    }

  inferior_pid = current_process_id;
  push_target (&child_ops);
}


static void
child_detach (args, from_tty)
     char *args;
     int from_tty;
{
  if (from_tty)
    {
      char *exec_file = get_exec_file (0);
      if (exec_file == 0)
	exec_file = "";
      printf_unfiltered ("Detaching from program: %s %s\n", exec_file,
			 target_pid_to_str (inferior_pid));
      gdb_flush (gdb_stdout);
    }
  inferior_pid = 0;
  unpush_target (&child_ops);
}


/* Print status information about what we're accessing.  */

static void
child_files_info (ignore)
     struct target_ops *ignore;
{
  printf_unfiltered ("\tUsing the running image of %s %s.\n",
      attach_flag ? "attached" : "child", target_pid_to_str (inferior_pid));
}

/* ARGSUSED */
static void
child_open (arg, from_tty)
     char *arg;
     int from_tty;
{
  error ("Use the \"run\" command to start a Unix child process.");
}


/* Start an inferior win32 child process and sets inferior_pid to its pid.
   EXEC_FILE is the file to run.
   ALLARGS is a string containing the arguments to the program.
   ENV is the environment vector to pass.  Errors reported with error().  */


static void
child_create_inferior (exec_file, allargs, env)
     char *exec_file;
     char *allargs;
     char **env;
{
  char *real_path;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  struct target_waitstatus dummy;
  BOOL ret;
  DWORD flags;
  char *args;

  if (!exec_file)
    {
      error ("No executable specified, use `target exec'.\n");
    }

  memset (&si, 0, sizeof (si));
  si.cb = sizeof (si);

  /* A realpath is always the same size, or a bit shorter than a nice path. */
  real_path = alloca (strlen (exec_file) + 1);
  path_to_real_path (exec_file, real_path);

  flags = DEBUG_ONLY_THIS_PROCESS | DEBUG_PROCESS;

  if (new_group)
    flags |= CREATE_NEW_PROCESS_GROUP;

  if (new_console)
    flags |= CREATE_NEW_CONSOLE;

  args = alloca (strlen (exec_file) + strlen (allargs) + 2);

  strcpy (args, exec_file);
  strcat (args, " ");
  strcat (args, allargs);

  ret = CreateProcess (real_path,
		       args,
		       NULL,	/* Security */
		       NULL,	/* thread */
		       TRUE,	/* inherit handles */
		       flags,	/* start flags */
		       env,
		       NULL,	/* current directory */
		       &si,
		       &pi);
  if (!ret)
    error ("Error creating process %s, (error %d)\n", exec_file, GetLastError());

  exception_count = 0;
  event_count = 0;

  inferior_pid = pi.dwProcessId;
  current_process = pi.hProcess;
  current_thread = pi.hThread;
  current_process_id = pi.dwProcessId;
  current_thread_id = pi.dwThreadId;
  push_target (&child_ops);
  init_thread_list ();
  init_wait_for_inferior ();
  clear_proceed_status ();
  target_terminal_init ();
  target_terminal_inferior ();

  /* Ignore the first trap */
  child_wait (inferior_pid, &dummy);

  proceed ((CORE_ADDR) - 1, TARGET_SIGNAL_0, 0);
}

static void
child_mourn_inferior ()
{
  unpush_target (&child_ops);
  generic_mourn_inferior ();
}


/* Send a SIGINT to the process group.  This acts just like the user typed a
   ^C on the controlling terminal. */

void
child_stop ()
{
  CHECK (GenerateConsoleCtrlEvent (CTRL_C_EVENT, 0));
}

int
child_xfer_memory (CORE_ADDR memaddr, char *our, int len,
		   int write, struct target_ops *target)
{
  DWORD done;
  if (write)
    {
      WriteProcessMemory (current_process, memaddr, our, len, &done);
      FlushInstructionCache (current_process, memaddr, len);
    }
  else
    {
      ReadProcessMemory (current_process, memaddr, our, len, &done);
    }
  return done;
}

void
child_kill_inferior (void)
{
  CHECK (TerminateProcess (current_process, 0));
  CHECK (CloseHandle (current_process));
  CHECK (CloseHandle (current_thread));
}

void
child_resume (int pid, int step, enum target_signal signal)
{
  DEBUG (("child_resume (%d, %d, %d);\n", pid, step, signal));

  if (step)
    {
      /* Single step by setting t bit */
      child_fetch_inferior_registers (PS_REGNUM);
      context.EFlags |= FLAG_TRACE_BIT;
    }

  if (context.ContextFlags)
    {
      CHECK (SetThreadContext (current_thread, &context));
      context.ContextFlags = 0;
    }

  if (signal)
    {
      fprintf_unfiltered (gdb_stderr, "Can't send signals to the child.\n");
    }

  CHECK (ContinueDebugEvent (current_process_id,
			     current_thread_id,
			     DBG_CONTINUE));
}

static void
child_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

static int
child_can_run ()
{
  return 1;
}

static void
child_close ()
{

}
struct target_ops child_ops =
{
  "child",			/* to_shortname */
  "Win32 child process",	/* to_longname */
  "Win32 child process (started by the \"run\" command).",	/* to_doc */
  child_open,			/* to_open */
  child_close,			/* to_close */
  child_attach,			/* to_attach */
  child_detach,			/* to_detach */
  child_resume,			/* to_resume */
  child_wait,			/* to_wait */
  child_fetch_inferior_registers,/* to_fetch_registers */
  child_store_inferior_registers,/* to_store_registers */
  child_prepare_to_store,	/* to_child_prepare_to_store */
  child_xfer_memory,		/* to_xfer_memory */
  child_files_info,		/* to_files_info */
  memory_insert_breakpoint,	/* to_insert_breakpoint */
  memory_remove_breakpoint,	/* to_remove_breakpoint */
  terminal_init_inferior,	/* to_terminal_init */
  terminal_inferior,		/* to_terminal_inferior */
  terminal_ours_for_output,	/* to_terminal_ours_for_output */
  terminal_ours,		/* to_terminal_ours */
  child_terminal_info,		/* to_terminal_info */
  child_kill_inferior,		/* to_kill */
  0,				/* to_load */
  0,				/* to_lookup_symbol */
  child_create_inferior,	/* to_create_inferior */
  child_mourn_inferior,		/* to_mourn_inferior */
  child_can_run,		/* to_can_run */
  0,				/* to_notice_signals */
  0,				/* to_thread_alive */
  child_stop,			/* to_stop */
  process_stratum,		/* to_stratum */
  0,				/* to_next */
  1,				/* to_has_all_memory */
  1,				/* to_has_memory */
  1,				/* to_has_stack */
  1,				/* to_has_registers */
  1,				/* to_has_execution */
  0,				/* to_sections */
  0,				/* to_sections_end */
  OPS_MAGIC			/* to_magic */
};

void
_initialize_inftarg ()
{
  add_show_from_set
    (add_set_cmd ("new-console", class_support, var_boolean,
		  (char *) &new_console,
		  "Set creation of new console when creating child process.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("new-group", class_support, var_boolean,
		  (char *) &new_group,
		  "Set creation of new group when creating child process.",
		  &setlist),
     &showlist);

  add_target (&child_ops);
}
