/* Low level interface to Windows debugging, for gdbserver.
   Copyright (C) 2006
   Free Software Foundation, Inc.

   Contributed by Leo Zayas.  Based on "win32-nat.c" from GDB.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "server.h"
#include "regcache.h"
#include "gdb/signals.h"

#include <windows.h>
#include <imagehlp.h>
#include <psapi.h>
#include <sys/param.h>
#include <malloc.h>
#include <process.h>

#ifndef USE_WIN32API
#include <sys/cygwin.h>
#endif

#define LOG 0

#define OUTMSG(X) do { printf X; fflush (stdout); } while (0)
#if LOG
#define OUTMSG2(X) do { printf X; fflush (stdout); } while (0)
#else
#define OUTMSG2(X)
#endif

int debug_threads;
int using_threads = 1;

/* Globals.  */
static HANDLE current_process_handle = NULL;
static DWORD current_process_id = 0;
static enum target_signal last_sig = TARGET_SIGNAL_0;

/* The current debug event from WaitForDebugEvent.  */
static DEBUG_EVENT current_event;

static int debug_registers_changed = 0;
static int debug_registers_used = 0;
static unsigned dr[8];

typedef BOOL winapi_DebugActiveProcessStop (DWORD dwProcessId);
typedef BOOL winapi_DebugSetProcessKillOnExit (BOOL KillOnExit);

#define FLAG_TRACE_BIT 0x100
#define CONTEXT_DEBUGGER (CONTEXT_FULL | CONTEXT_FLOATING_POINT)
#define CONTEXT_DEBUGGER_DR CONTEXT_DEBUGGER | CONTEXT_DEBUG_REGISTERS	\
  | CONTEXT_EXTENDED_REGISTERS

/* Thread information structure used to track extra information about
   each thread.  */
typedef struct thread_info_struct
{
  DWORD tid;
  HANDLE h;
  int suspend_count;
  CONTEXT context;
} thread_info;
static DWORD main_thread_id = 0;

/* Get the thread ID from the current selected inferior (the current
   thread).  */
static DWORD
current_inferior_tid (void)
{
  thread_info *th = inferior_target_data (current_inferior);
  return th->tid;
}

/* Find a thread record given a thread id.  If GET_CONTEXT is set then
   also retrieve the context for this thread.  */
static thread_info *
thread_rec (DWORD id, int get_context)
{
  struct thread_info *thread;
  thread_info *th;

  thread = (struct thread_info *) find_inferior_id (&all_threads, id);
  if (thread == NULL)
    return NULL;

  th = inferior_target_data (thread);
  if (!th->suspend_count && get_context)
    {
      if (get_context > 0 && id != current_event.dwThreadId)
	th->suspend_count = SuspendThread (th->h) + 1;
      else if (get_context < 0)
	th->suspend_count = -1;

      th->context.ContextFlags = CONTEXT_DEBUGGER_DR;

      GetThreadContext (th->h, &th->context);

      if (id == current_event.dwThreadId)
	{
	  /* Copy dr values from that thread.  */
	  dr[0] = th->context.Dr0;
	  dr[1] = th->context.Dr1;
	  dr[2] = th->context.Dr2;
	  dr[3] = th->context.Dr3;
	  dr[6] = th->context.Dr6;
	  dr[7] = th->context.Dr7;
	}
    }

  return th;
}

/* Add a thread to the thread list.  */
static thread_info *
child_add_thread (DWORD tid, HANDLE h)
{
  thread_info *th;

  if ((th = thread_rec (tid, FALSE)))
    return th;

  th = (thread_info *) malloc (sizeof (*th));
  memset (th, 0, sizeof (*th));
  th->tid = tid;
  th->h = h;

  add_thread (tid, th, (unsigned int) tid);
  set_inferior_regcache_data ((struct thread_info *)
			      find_inferior_id (&all_threads, tid),
			      new_register_cache ());

  /* Set the debug registers for the new thread if they are used.  */
  if (debug_registers_used)
    {
      /* Only change the value of the debug registers.  */
      th->context.ContextFlags = CONTEXT_DEBUGGER_DR;

      GetThreadContext (th->h, &th->context);

      th->context.Dr0 = dr[0];
      th->context.Dr1 = dr[1];
      th->context.Dr2 = dr[2];
      th->context.Dr3 = dr[3];
      /* th->context.Dr6 = dr[6];
         FIXME: should we set dr6 also ?? */
      th->context.Dr7 = dr[7];
      SetThreadContext (th->h, &th->context);
      th->context.ContextFlags = 0;
    }

  return th;
}

/* Delete a thread from the list of threads.  */
static void
delete_thread_info (struct inferior_list_entry *thread)
{
  thread_info *th = inferior_target_data ((struct thread_info *) thread);

  remove_thread ((struct thread_info *) thread);
  CloseHandle (th->h);
  free (th);
}

/* Delete a thread from the list of threads.  */
static void
child_delete_thread (DWORD id)
{
  struct inferior_list_entry *thread;

  /* If the last thread is exiting, just return.  */
  if (all_threads.head == all_threads.tail)
    return;

  thread = find_inferior_id (&all_threads, id);
  if (thread == NULL)
    return;

  delete_thread_info (thread);
}

/* Transfer memory from/to the debugged process.  */
static int
child_xfer_memory (CORE_ADDR memaddr, char *our, int len,
		   int write, struct target_ops *target)
{
  SIZE_T done;
  long addr = (long) memaddr;

  if (write)
    {
      WriteProcessMemory (current_process_handle, (LPVOID) addr,
			  (LPCVOID) our, len, &done);
      FlushInstructionCache (current_process_handle, (LPCVOID) addr, len);
    }
  else
    {
      ReadProcessMemory (current_process_handle, (LPCVOID) addr, (LPVOID) our,
			 len, &done);
    }
  return done;
}

/* Generally, what has the program done?  */
enum target_waitkind
{
  /* The program has exited.  The exit status is in value.integer.  */
  TARGET_WAITKIND_EXITED,

  /* The program has stopped with a signal.  Which signal is in
     value.sig.  */
  TARGET_WAITKIND_STOPPED,

  /* The program is letting us know that it dynamically loaded something
     (e.g. it called load(2) on AIX).  */
  TARGET_WAITKIND_LOADED,

  /* The program has exec'ed a new executable file.  The new file's
     pathname is pointed to by value.execd_pathname.  */

  TARGET_WAITKIND_EXECD,

  /* Nothing happened, but we stopped anyway.  This perhaps should be handled
     within target_wait, but I'm not sure target_wait should be resuming the
     inferior.  */
  TARGET_WAITKIND_SPURIOUS,
};

struct target_waitstatus
{
  enum target_waitkind kind;

  /* Forked child pid, execd pathname, exit status or signal number.  */
  union
  {
    int integer;
    enum target_signal sig;
    int related_pid;
    char *execd_pathname;
    int syscall_id;
  }
  value;
};

#define NUM_REGS 41
#define FCS_REGNUM 27
#define FOP_REGNUM 31

#define context_offset(x) ((int)&(((CONTEXT *)NULL)->x))
static const int mappings[] = {
  context_offset (Eax),
  context_offset (Ecx),
  context_offset (Edx),
  context_offset (Ebx),
  context_offset (Esp),
  context_offset (Ebp),
  context_offset (Esi),
  context_offset (Edi),
  context_offset (Eip),
  context_offset (EFlags),
  context_offset (SegCs),
  context_offset (SegSs),
  context_offset (SegDs),
  context_offset (SegEs),
  context_offset (SegFs),
  context_offset (SegGs),
  context_offset (FloatSave.RegisterArea[0 * 10]),
  context_offset (FloatSave.RegisterArea[1 * 10]),
  context_offset (FloatSave.RegisterArea[2 * 10]),
  context_offset (FloatSave.RegisterArea[3 * 10]),
  context_offset (FloatSave.RegisterArea[4 * 10]),
  context_offset (FloatSave.RegisterArea[5 * 10]),
  context_offset (FloatSave.RegisterArea[6 * 10]),
  context_offset (FloatSave.RegisterArea[7 * 10]),
  context_offset (FloatSave.ControlWord),
  context_offset (FloatSave.StatusWord),
  context_offset (FloatSave.TagWord),
  context_offset (FloatSave.ErrorSelector),
  context_offset (FloatSave.ErrorOffset),
  context_offset (FloatSave.DataSelector),
  context_offset (FloatSave.DataOffset),
  context_offset (FloatSave.ErrorSelector),
  /* XMM0-7 */
  context_offset (ExtendedRegisters[10 * 16]),
  context_offset (ExtendedRegisters[11 * 16]),
  context_offset (ExtendedRegisters[12 * 16]),
  context_offset (ExtendedRegisters[13 * 16]),
  context_offset (ExtendedRegisters[14 * 16]),
  context_offset (ExtendedRegisters[15 * 16]),
  context_offset (ExtendedRegisters[16 * 16]),
  context_offset (ExtendedRegisters[17 * 16]),
  /* MXCSR */
  context_offset (ExtendedRegisters[24])
};

#undef context_offset

/* Clear out any old thread list and reintialize it to a pristine
   state. */
static void
child_init_thread_list (void)
{
  for_each_inferior (&all_threads, delete_thread_info);
}

static void
do_initial_child_stuff (DWORD pid)
{
  int i;

  last_sig = TARGET_SIGNAL_0;

  debug_registers_changed = 0;
  debug_registers_used = 0;
  for (i = 0; i < sizeof (dr) / sizeof (dr[0]); i++)
    dr[i] = 0;
  memset (&current_event, 0, sizeof (current_event));

  child_init_thread_list ();
}

/* Resume all artificially suspended threads if we are continuing
   execution.  */
static int
continue_one_thread (struct inferior_list_entry *this_thread, void *id_ptr)
{
  struct thread_info *thread = (struct thread_info *) this_thread;
  int thread_id = * (int *) id_ptr;
  thread_info *th = inferior_target_data (thread);
  int i;

  if ((thread_id == -1 || thread_id == th->tid)
      && th->suspend_count)
    {
      for (i = 0; i < th->suspend_count; i++)
	(void) ResumeThread (th->h);
      th->suspend_count = 0;
      if (debug_registers_changed)
	{
	  /* Only change the value of the debug registers.  */
	  th->context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	  th->context.Dr0 = dr[0];
	  th->context.Dr1 = dr[1];
	  th->context.Dr2 = dr[2];
	  th->context.Dr3 = dr[3];
	  /* th->context.Dr6 = dr[6];
	     FIXME: should we set dr6 also ?? */
	  th->context.Dr7 = dr[7];
	  SetThreadContext (th->h, &th->context);
	  th->context.ContextFlags = 0;
	}
    }

  return 0;
}

static BOOL
child_continue (DWORD continue_status, int thread_id)
{
  BOOL res;

  res = ContinueDebugEvent (current_event.dwProcessId,
			    current_event.dwThreadId, continue_status);
  continue_status = 0;
  if (res)
    find_inferior (&all_threads, continue_one_thread, &thread_id);

  debug_registers_changed = 0;
  return res;
}

/* Fetch register(s) from gdbserver regcache data.  */
static void
do_child_fetch_inferior_registers (thread_info *th, int r)
{
  char *context_offset = ((char *) &th->context) + mappings[r];
  long l;
  if (r == FCS_REGNUM)
    {
      l = *((long *) context_offset) & 0xffff;
      supply_register (r, (char *) &l);
    }
  else if (r == FOP_REGNUM)
    {
      l = (*((long *) context_offset) >> 16) & ((1 << 11) - 1);
      supply_register (r, (char *) &l);
    }
  else
    supply_register (r, context_offset);
}

/* Fetch register(s) from the current thread context.  */
static void
child_fetch_inferior_registers (int r)
{
  int regno;
  thread_info *th = thread_rec (current_inferior_tid (), TRUE);
  if (r == -1 || r == 0 || r > NUM_REGS)
    child_fetch_inferior_registers (NUM_REGS);
  else
    for (regno = 0; regno < r; regno++)
      do_child_fetch_inferior_registers (th, regno);
}

/* Get register from gdbserver regcache data.  */
static void
do_child_store_inferior_registers (thread_info *th, int r)
{
  collect_register (r, ((char *) &th->context) + mappings[r]);
}

/* Store a new register value into the current thread context.  We don't
   change the program's context until later, when we resume it.  */
static void
child_store_inferior_registers (int r)
{
  int regno;
  thread_info *th = thread_rec (current_inferior_tid (), TRUE);
  if (r == -1 || r == 0 || r > NUM_REGS)
    child_store_inferior_registers (NUM_REGS);
  else
    for (regno = 0; regno < r; regno++)
      do_child_store_inferior_registers (th, regno);
}

/* Start a new process.
   PROGRAM is a path to the program to execute.
   ARGS is a standard NULL-terminated array of arguments,
   to be passed to the inferior as ``argv''.
   Returns the new PID on success, -1 on failure.  Registers the new
   process with the process list.  */
static int
win32_create_inferior (char *program, char **program_args)
{
#ifndef USE_WIN32API
  char real_path[MAXPATHLEN];
  char *orig_path, *new_path, *path_ptr;
#endif
  char *winenv = NULL;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  BOOL ret;
  DWORD flags;
  char *args;
  int argslen;
  int argc;

  if (!program)
    error ("No executable specified, specify executable to debug.\n");

  memset (&si, 0, sizeof (si));
  si.cb = sizeof (si);

  flags = DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS;

#ifndef USE_WIN32API
  orig_path = NULL;
  path_ptr = getenv ("PATH");
  if (path_ptr)
    {
      orig_path = alloca (strlen (path_ptr) + 1);
      new_path = alloca (cygwin_posix_to_win32_path_list_buf_size (path_ptr));
      strcpy (orig_path, path_ptr);
      cygwin_posix_to_win32_path_list (path_ptr, new_path);
      setenv ("PATH", new_path, 1);
    }
  cygwin_conv_to_win32_path (program, real_path);
  program = real_path;
#endif

  argslen = strlen (program) + 1;
  for (argc = 1; program_args[argc]; argc++)
    argslen += strlen (program_args[argc]) + 1;
  args = alloca (argslen);
  strcpy (args, program);
  for (argc = 1; program_args[argc]; argc++)
    {
      /* FIXME: Can we do better about quoting?  How does Cygwin
         handle this?  */
      strcat (args, " ");
      strcat (args, program_args[argc]);
    }
  OUTMSG2 (("Command line is %s\n", args));

  flags |= CREATE_NEW_PROCESS_GROUP;

  ret = CreateProcess (0, args,	/* command line */
		       NULL,	/* Security */
		       NULL,	/* thread */
		       TRUE,	/* inherit handles */
		       flags,	/* start flags */
		       winenv, NULL,	/* current directory */
		       &si, &pi);

#ifndef USE_WIN32API
  if (orig_path)
    setenv ("PATH", orig_path, 1);
#endif

  if (!ret)
    {
      error ("Error creating process %s, (error %d): %s\n", args,
	     (int) GetLastError (), strerror (GetLastError ()));
    }
  else
    {
      OUTMSG2 (("Process created: %s\n", (char *) args));
    }

  CloseHandle (pi.hThread);

  current_process_handle = pi.hProcess;
  current_process_id = pi.dwProcessId;

  do_initial_child_stuff (current_process_id);

  return current_process_id;
}

/* Attach to a running process.
   PID is the process ID to attach to, specified by the user
   or a higher layer.  */
static int
win32_attach (unsigned long pid)
{
  int res = 0;
  HMODULE kernel32 = LoadLibrary ("KERNEL32.DLL");
  winapi_DebugActiveProcessStop *DebugActiveProcessStop = NULL;
  winapi_DebugSetProcessKillOnExit *DebugSetProcessKillOnExit = NULL;

  DebugActiveProcessStop =
    (winapi_DebugActiveProcessStop *) GetProcAddress (kernel32,
						      "DebugActiveProcessStop");
  DebugSetProcessKillOnExit =
    (winapi_DebugSetProcessKillOnExit *) GetProcAddress (kernel32,
							 "DebugSetProcessKillOnExit");

  res = DebugActiveProcess (pid) ? 1 : 0;

  if (!res)
    error ("Attach to process failed.");

  if (DebugSetProcessKillOnExit != NULL)
    DebugSetProcessKillOnExit (FALSE);

  current_process_id = pid;
  current_process_handle = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pid);

  if (current_process_handle == NULL)
    {
      res = 0;
      if (DebugActiveProcessStop != NULL)
	DebugActiveProcessStop (current_process_id);
    }

  if (res)
    do_initial_child_stuff (pid);

  FreeLibrary (kernel32);

  return res;
}

/* Kill all inferiors.  */
static void
win32_kill (void)
{
  TerminateProcess (current_process_handle, 0);
  for (;;)
    {
      if (!child_continue (DBG_CONTINUE, -1))
	break;
      if (!WaitForDebugEvent (&current_event, INFINITE))
	break;
      if (current_event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
	break;
    }
}

/* Detach from all inferiors.  */
static void
win32_detach (void)
{
  HMODULE kernel32 = LoadLibrary ("KERNEL32.DLL");
  winapi_DebugActiveProcessStop *DebugActiveProcessStop = NULL;
  winapi_DebugSetProcessKillOnExit *DebugSetProcessKillOnExit = NULL;

  DebugActiveProcessStop =
    (winapi_DebugActiveProcessStop *) GetProcAddress (kernel32,
						      "DebugActiveProcessStop");
  DebugSetProcessKillOnExit =
    (winapi_DebugSetProcessKillOnExit *) GetProcAddress (kernel32,
							 "DebugSetProcessKillOnExit");

  if (DebugSetProcessKillOnExit != NULL)
    DebugSetProcessKillOnExit (FALSE);

  if (DebugActiveProcessStop != NULL)
    DebugActiveProcessStop (current_process_id);
  else
    win32_kill ();

  FreeLibrary (kernel32);
}

/* Return 1 iff the thread with thread ID TID is alive.  */
static int
win32_thread_alive (unsigned long tid)
{
  int res;

  /* Our thread list is reliable; don't bother to poll target
     threads.  */
  if (find_inferior_id (&all_threads, tid) != NULL)
    res = 1;
  else
    res = 0;
  return res;
}

/* Resume the inferior process.  RESUME_INFO describes how we want
   to resume.  */
static void
win32_resume (struct thread_resume *resume_info)
{
  DWORD tid;
  enum target_signal sig;
  int step;
  thread_info *th;
  DWORD continue_status = DBG_CONTINUE;

  /* This handles the very limited set of resume packets that GDB can
     currently produce.  */

  if (resume_info[0].thread == -1)
    tid = -1;
  else if (resume_info[1].thread == -1 && !resume_info[1].leave_stopped)
    tid = -1;
  else
    /* Yes, we're ignoring resume_info[0].thread.  It'd be tricky to make
       the Windows resume code do the right thing for thread switching.  */
    tid = current_event.dwThreadId;

  if (resume_info[0].thread != -1)
    {
      sig = resume_info[0].sig;
      step = resume_info[0].step;
    }
  else
    {
      sig = 0;
      step = 0;
    }

  if (sig != TARGET_SIGNAL_0)
    {
      if (current_event.dwDebugEventCode != EXCEPTION_DEBUG_EVENT)
	{
	  OUTMSG (("Cannot continue with signal %d here.\n", sig));
	}
      else if (sig == last_sig)
	continue_status = DBG_EXCEPTION_NOT_HANDLED;
      else
	OUTMSG (("Can only continue with recieved signal %d.\n", last_sig));
    }

  last_sig = TARGET_SIGNAL_0;

  /* Get context for the currently selected thread.  */
  th = thread_rec (current_event.dwThreadId, FALSE);
  if (th)
    {
      if (th->context.ContextFlags)
	{
	  if (debug_registers_changed)
	    {
	      th->context.Dr0 = dr[0];
	      th->context.Dr1 = dr[1];
	      th->context.Dr2 = dr[2];
	      th->context.Dr3 = dr[3];
	      /* th->context.Dr6 = dr[6];
	         FIXME: should we set dr6 also ?? */
	      th->context.Dr7 = dr[7];
	    }

	  /* Move register values from the inferior into the thread
	     context structure.  */
	  regcache_invalidate ();

	  if (step)
	    th->context.EFlags |= FLAG_TRACE_BIT;

	  SetThreadContext (th->h, &th->context);
	  th->context.ContextFlags = 0;
	}
    }

  /* Allow continuing with the same signal that interrupted us.
     Otherwise complain.  */

  child_continue (continue_status, tid);
}

static int
handle_exception (struct target_waitstatus *ourstatus)
{
  thread_info *th;
  DWORD code = current_event.u.Exception.ExceptionRecord.ExceptionCode;

  ourstatus->kind = TARGET_WAITKIND_STOPPED;

  /* Record the context of the current thread.  */
  th = thread_rec (current_event.dwThreadId, -1);

  switch (code)
    {
    case EXCEPTION_ACCESS_VIOLATION:
      OUTMSG2 (("EXCEPTION_ACCESS_VIOLATION"));
      ourstatus->value.sig = TARGET_SIGNAL_SEGV;
      break;
    case STATUS_STACK_OVERFLOW:
      OUTMSG2 (("STATUS_STACK_OVERFLOW"));
      ourstatus->value.sig = TARGET_SIGNAL_SEGV;
      break;
    case STATUS_FLOAT_DENORMAL_OPERAND:
      OUTMSG2 (("STATUS_FLOAT_DENORMAL_OPERAND"));
      ourstatus->value.sig = TARGET_SIGNAL_FPE;
      break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      OUTMSG2 (("EXCEPTION_ARRAY_BOUNDS_EXCEEDED"));
      ourstatus->value.sig = TARGET_SIGNAL_FPE;
      break;
    case STATUS_FLOAT_INEXACT_RESULT:
      OUTMSG2 (("STATUS_FLOAT_INEXACT_RESULT"));
      ourstatus->value.sig = TARGET_SIGNAL_FPE;
      break;
    case STATUS_FLOAT_INVALID_OPERATION:
      OUTMSG2 (("STATUS_FLOAT_INVALID_OPERATION"));
      ourstatus->value.sig = TARGET_SIGNAL_FPE;
      break;
    case STATUS_FLOAT_OVERFLOW:
      OUTMSG2 (("STATUS_FLOAT_OVERFLOW"));
      ourstatus->value.sig = TARGET_SIGNAL_FPE;
      break;
    case STATUS_FLOAT_STACK_CHECK:
      OUTMSG2 (("STATUS_FLOAT_STACK_CHECK"));
      ourstatus->value.sig = TARGET_SIGNAL_FPE;
      break;
    case STATUS_FLOAT_UNDERFLOW:
      OUTMSG2 (("STATUS_FLOAT_UNDERFLOW"));
      ourstatus->value.sig = TARGET_SIGNAL_FPE;
      break;
    case STATUS_FLOAT_DIVIDE_BY_ZERO:
      OUTMSG2 (("STATUS_FLOAT_DIVIDE_BY_ZERO"));
      ourstatus->value.sig = TARGET_SIGNAL_FPE;
      break;
    case STATUS_INTEGER_DIVIDE_BY_ZERO:
      OUTMSG2 (("STATUS_INTEGER_DIVIDE_BY_ZERO"));
      ourstatus->value.sig = TARGET_SIGNAL_FPE;
      break;
    case STATUS_INTEGER_OVERFLOW:
      OUTMSG2 (("STATUS_INTEGER_OVERFLOW"));
      ourstatus->value.sig = TARGET_SIGNAL_FPE;
      break;
    case EXCEPTION_BREAKPOINT:
      OUTMSG2 (("EXCEPTION_BREAKPOINT"));
      ourstatus->value.sig = TARGET_SIGNAL_TRAP;
      break;
    case DBG_CONTROL_C:
      OUTMSG2 (("DBG_CONTROL_C"));
      ourstatus->value.sig = TARGET_SIGNAL_INT;
      break;
    case DBG_CONTROL_BREAK:
      OUTMSG2 (("DBG_CONTROL_BREAK"));
      ourstatus->value.sig = TARGET_SIGNAL_INT;
      break;
    case EXCEPTION_SINGLE_STEP:
      OUTMSG2 (("EXCEPTION_SINGLE_STEP"));
      ourstatus->value.sig = TARGET_SIGNAL_TRAP;
      break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
      OUTMSG2 (("EXCEPTION_ILLEGAL_INSTRUCTION"));
      ourstatus->value.sig = TARGET_SIGNAL_ILL;
      break;
    case EXCEPTION_PRIV_INSTRUCTION:
      OUTMSG2 (("EXCEPTION_PRIV_INSTRUCTION"));
      ourstatus->value.sig = TARGET_SIGNAL_ILL;
      break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      OUTMSG2 (("EXCEPTION_NONCONTINUABLE_EXCEPTION"));
      ourstatus->value.sig = TARGET_SIGNAL_ILL;
      break;
    default:
      if (current_event.u.Exception.dwFirstChance)
	return 0;
      OUTMSG2 (("gdbserver: unknown target exception 0x%08lx at 0x%08lx",
		current_event.u.Exception.ExceptionRecord.ExceptionCode,
		(DWORD) current_event.u.Exception.ExceptionRecord.
		ExceptionAddress));
      ourstatus->value.sig = TARGET_SIGNAL_UNKNOWN;
      break;
    }
  OUTMSG2 (("\n"));
  last_sig = ourstatus->value.sig;
  return 1;
}

/* Get the next event from the child.  Return 1 if the event requires
   handling.  */
static int
get_child_debug_event (struct target_waitstatus *ourstatus)
{
  BOOL debug_event;
  DWORD continue_status, event_code;
  thread_info *th = NULL;
  static thread_info dummy_thread_info;
  int retval = 0;

in:

  last_sig = TARGET_SIGNAL_0;
  ourstatus->kind = TARGET_WAITKIND_SPURIOUS;

  if (!(debug_event = WaitForDebugEvent (&current_event, 1000)))
    goto out;

  current_inferior =
    (struct thread_info *) find_inferior_id (&all_threads,
					     current_event.dwThreadId);

  continue_status = DBG_CONTINUE;
  event_code = current_event.dwDebugEventCode;

  switch (event_code)
    {
    case CREATE_THREAD_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event CREATE_THREAD_DEBUG_EVENT "
		"for pid=%d tid=%x)\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));

      /* Record the existence of this thread.  */
      th = child_add_thread (current_event.dwThreadId,
			     current_event.u.CreateThread.hThread);

      retval = current_event.dwThreadId;
      break;

    case EXIT_THREAD_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event EXIT_THREAD_DEBUG_EVENT "
		"for pid=%d tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      child_delete_thread (current_event.dwThreadId);
      th = &dummy_thread_info;
      break;

    case CREATE_PROCESS_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event CREATE_PROCESS_DEBUG_EVENT "
		"for pid=%d tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      CloseHandle (current_event.u.CreateProcessInfo.hFile);

      current_process_handle = current_event.u.CreateProcessInfo.hProcess;
      main_thread_id = current_event.dwThreadId;

      ourstatus->kind = TARGET_WAITKIND_EXECD;
      ourstatus->value.execd_pathname = "Main executable";

      /* Add the main thread.  */
      th =
	child_add_thread (main_thread_id,
			  current_event.u.CreateProcessInfo.hThread);

      retval = ourstatus->value.related_pid = current_event.dwThreadId;
      break;

    case EXIT_PROCESS_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event EXIT_PROCESS_DEBUG_EVENT "
		"for pid=%d tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      ourstatus->kind = TARGET_WAITKIND_EXITED;
      ourstatus->value.integer = current_event.u.ExitProcess.dwExitCode;
      CloseHandle (current_process_handle);
      retval = main_thread_id;
      break;

    case LOAD_DLL_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event LOAD_DLL_DEBUG_EVENT "
		"for pid=%d tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      CloseHandle (current_event.u.LoadDll.hFile);

      ourstatus->kind = TARGET_WAITKIND_LOADED;
      ourstatus->value.integer = 0;
      retval = main_thread_id;
      break;

    case UNLOAD_DLL_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event UNLOAD_DLL_DEBUG_EVENT "
		"for pid=%d tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      break;

    case EXCEPTION_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event EXCEPTION_DEBUG_EVENT "
		"for pid=%d tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      retval = handle_exception (ourstatus);
      break;

    case OUTPUT_DEBUG_STRING_EVENT:
      /* A message from the kernel (or Cygwin).  */
      OUTMSG2 (("gdbserver: kernel event OUTPUT_DEBUG_STRING_EVENT "
		"for pid=%d tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      break;

    default:
      OUTMSG2 (("gdbserver: kernel event unknown "
		"for pid=%d tid=%x code=%ld\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId,
		current_event.dwDebugEventCode));
      break;
    }

  current_inferior =
    (struct thread_info *) find_inferior_id (&all_threads,
					     current_event.dwThreadId);

  if (!retval || (event_code != EXCEPTION_DEBUG_EVENT && event_code != EXIT_PROCESS_DEBUG_EVENT))
    {
      child_continue (continue_status, -1);
      goto in;
    }

  if (th == NULL)
    thread_rec (current_event.dwThreadId, TRUE);

out:
  return retval;
}

/* Wait for the inferior process to change state.
   STATUS will be filled in with a response code to send to GDB.
   Returns the signal which caused the process to stop. */
static unsigned char
win32_wait (char *status)
{
  struct target_waitstatus our_status;

  *status = 'T';

  while (1)
    {
      get_child_debug_event (&our_status);

      if (our_status.kind == TARGET_WAITKIND_EXITED)
	{
	  OUTMSG2 (("Child exited with retcode = %x\n",
		    our_status.value.integer));

	  *status = 'W';

	  child_fetch_inferior_registers (-1);

	  return our_status.value.integer;
	}
      else if (our_status.kind == TARGET_WAITKIND_STOPPED)
	{
	  OUTMSG2 (("Child Stopped with signal = %x \n",
		    WSTOPSIG (our_status.value.sig)));

	  *status = 'T';

	  child_fetch_inferior_registers (-1);

	  return our_status.value.sig;
	}
      else
	OUTMSG (("Ignoring unknown internal event, %d\n", our_status.kind));

      {
	struct thread_resume resume;
	resume.thread = -1;
	resume.step = 0;
	resume.sig = 0;
	resume.leave_stopped = 0;
	win32_resume (&resume);
      }
    }
}

/* Fetch registers from the inferior process.
   If REGNO is -1, fetch all registers; otherwise, fetch at least REGNO.  */
static void
win32_fetch_inferior_registers (int regno)
{
  child_fetch_inferior_registers (regno);
}

/* Store registers to the inferior process.
   If REGNO is -1, store all registers; otherwise, store at least REGNO.  */
static void
win32_store_inferior_registers (int regno)
{
  child_store_inferior_registers (regno);
}

/* Read memory from the inferior process.  This should generally be
   called through read_inferior_memory, which handles breakpoint shadowing.
   Read LEN bytes at MEMADDR into a buffer at MYADDR.  */
static int
win32_read_inferior_memory (CORE_ADDR memaddr, unsigned char *myaddr, int len)
{
  return child_xfer_memory (memaddr, myaddr, len, 0, 0) != len;
}

/* Write memory to the inferior process.  This should generally be
   called through write_inferior_memory, which handles breakpoint shadowing.
   Write LEN bytes from the buffer at MYADDR to MEMADDR.
   Returns 0 on success and errno on failure.  */
static int
win32_write_inferior_memory (CORE_ADDR memaddr, const unsigned char *myaddr,
			     int len)
{
  return child_xfer_memory (memaddr, (char *) myaddr, len, 1, 0) != len;
}

static struct target_ops win32_target_ops = {
  win32_create_inferior,
  win32_attach,
  win32_kill,
  win32_detach,
  win32_thread_alive,
  win32_resume,
  win32_wait,
  win32_fetch_inferior_registers,
  win32_store_inferior_registers,
  win32_read_inferior_memory,
  win32_write_inferior_memory,
  0,
  0
};

/* Initialize the Win32 backend.  */
void
initialize_low (void)
{
  set_target_ops (&win32_target_ops);

  init_registers ();
}
