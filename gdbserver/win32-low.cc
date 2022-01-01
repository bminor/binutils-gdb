/* Low level interface to Windows debugging, for gdbserver.
   Copyright (C) 2006-2022 Free Software Foundation, Inc.

   Contributed by Leo Zayas.  Based on "win32-nat.c" from GDB.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "server.h"
#include "regcache.h"
#include "gdb/fileio.h"
#include "mem-break.h"
#include "win32-low.h"
#include "gdbthread.h"
#include "dll.h"
#include "hostio.h"
#include <windows.h>
#include <winnt.h>
#include <imagehlp.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <process.h>
#include "gdbsupport/gdb_tilde_expand.h"
#include "gdbsupport/common-inferior.h"
#include "gdbsupport/gdb_wait.h"

using namespace windows_nat;

#ifndef USE_WIN32API
#include <sys/cygwin.h>
#endif

#define OUTMSG(X) do { printf X; fflush (stderr); } while (0)

#define OUTMSG2(X) \
  do						\
    {						\
      if (debug_threads)			\
	{					\
	  printf X;				\
	  fflush (stderr);			\
	}					\
    } while (0)

#ifndef _T
#define _T(x) TEXT (x)
#endif

#ifndef COUNTOF
#define COUNTOF(STR) (sizeof (STR) / sizeof ((STR)[0]))
#endif

int using_threads = 1;

/* Globals.  */
static int attaching = 0;

/* A status that hasn't been reported to the core yet, and so
   win32_wait should return it next, instead of fetching the next
   debug event off the win32 API.  */
static struct target_waitstatus cached_status;

/* Non zero if an interrupt request is to be satisfied by suspending
   all threads.  */
static int soft_interrupt_requested = 0;

/* Non zero if the inferior is stopped in a simulated breakpoint done
   by suspending all the threads.  */
static int faked_breakpoint = 0;

/* True if current_process_handle needs to be closed.  */
static bool open_process_used = false;

const struct target_desc *win32_tdesc;
#ifdef __x86_64__
const struct target_desc *wow64_win32_tdesc;
#endif

#define NUM_REGS (the_low_target.num_regs ())

/* Get the thread ID from the current selected inferior (the current
   thread).  */
static ptid_t
current_thread_ptid (void)
{
  return current_ptid;
}

/* The current debug event from WaitForDebugEvent.  */
static ptid_t
debug_event_ptid (DEBUG_EVENT *event)
{
  return ptid_t (event->dwProcessId, event->dwThreadId, 0);
}

/* Get the thread context of the thread associated with TH.  */

static void
win32_get_thread_context (windows_thread_info *th)
{
#ifdef __x86_64__
  if (wow64_process)
    memset (&th->wow64_context, 0, sizeof (WOW64_CONTEXT));
  else
#endif
    memset (&th->context, 0, sizeof (CONTEXT));
  (*the_low_target.get_thread_context) (th);
}

/* Set the thread context of the thread associated with TH.  */

static void
win32_set_thread_context (windows_thread_info *th)
{
#ifdef __x86_64__
  if (wow64_process)
    Wow64SetThreadContext (th->h, &th->wow64_context);
  else
#endif
    SetThreadContext (th->h, &th->context);
}

/* Set the thread context of the thread associated with TH.  */

static void
win32_prepare_to_resume (windows_thread_info *th)
{
  if (the_low_target.prepare_to_resume != NULL)
    (*the_low_target.prepare_to_resume) (th);
}

/* See win32-low.h.  */

void
win32_require_context (windows_thread_info *th)
{
  DWORD context_flags;
#ifdef __x86_64__
  if (wow64_process)
    context_flags = th->wow64_context.ContextFlags;
  else
#endif
    context_flags = th->context.ContextFlags;
  if (context_flags == 0)
    {
      th->suspend ();
      win32_get_thread_context (th);
    }
}

/* See nat/windows-nat.h.  */

windows_thread_info *
windows_nat::thread_rec (ptid_t ptid, thread_disposition_type disposition)
{
  thread_info *thread = find_thread_ptid (ptid);
  if (thread == NULL)
    return NULL;

  windows_thread_info *th = (windows_thread_info *) thread_target_data (thread);
  if (disposition != DONT_INVALIDATE_CONTEXT)
    win32_require_context (th);
  return th;
}

/* Add a thread to the thread list.  */
static windows_thread_info *
child_add_thread (DWORD pid, DWORD tid, HANDLE h, void *tlb)
{
  windows_thread_info *th;
  ptid_t ptid = ptid_t (pid, tid, 0);

  if ((th = thread_rec (ptid, DONT_INVALIDATE_CONTEXT)))
    return th;

  CORE_ADDR base = (CORE_ADDR) (uintptr_t) tlb;
#ifdef __x86_64__
  /* For WOW64 processes, this is actually the pointer to the 64bit TIB,
     and the 32bit TIB is exactly 2 pages after it.  */
  if (wow64_process)
    base += 2 * 4096; /* page size = 4096 */
#endif
  th = new windows_thread_info (tid, h, base);

  add_thread (ptid, th);

  if (the_low_target.thread_added != NULL)
    (*the_low_target.thread_added) (th);

  return th;
}

/* Delete a thread from the list of threads.  */
static void
delete_thread_info (thread_info *thread)
{
  windows_thread_info *th = (windows_thread_info *) thread_target_data (thread);

  remove_thread (thread);
  delete th;
}

/* Delete a thread from the list of threads.  */
static void
child_delete_thread (DWORD pid, DWORD tid)
{
  /* If the last thread is exiting, just return.  */
  if (all_threads.size () == 1)
    return;

  thread_info *thread = find_thread_ptid (ptid_t (pid, tid));
  if (thread == NULL)
    return;

  delete_thread_info (thread);
}

/* These watchpoint related wrapper functions simply pass on the function call
   if the low target has registered a corresponding function.  */

bool
win32_process_target::supports_z_point_type (char z_type)
{
  return (z_type == Z_PACKET_SW_BP
	  || (the_low_target.supports_z_point_type != NULL
	      && the_low_target.supports_z_point_type (z_type)));
}

int
win32_process_target::insert_point (enum raw_bkpt_type type, CORE_ADDR addr,
				    int size, raw_breakpoint *bp)
{
  if (type == raw_bkpt_type_sw)
    return insert_memory_breakpoint (bp);
  else if (the_low_target.insert_point != NULL)
    return the_low_target.insert_point (type, addr, size, bp);
  else
    /* Unsupported (see target.h).  */
    return 1;
}

int
win32_process_target::remove_point (enum raw_bkpt_type type, CORE_ADDR addr,
				    int size, raw_breakpoint *bp)
{
  if (type == raw_bkpt_type_sw)
    return remove_memory_breakpoint (bp);
  else if (the_low_target.remove_point != NULL)
    return the_low_target.remove_point (type, addr, size, bp);
  else
    /* Unsupported (see target.h).  */
    return 1;
}

bool
win32_process_target::stopped_by_watchpoint ()
{
  if (the_low_target.stopped_by_watchpoint != NULL)
    return the_low_target.stopped_by_watchpoint ();
  else
    return false;
}

CORE_ADDR
win32_process_target::stopped_data_address ()
{
  if (the_low_target.stopped_data_address != NULL)
    return the_low_target.stopped_data_address ();
  else
    return 0;
}


/* Transfer memory from/to the debugged process.  */
static int
child_xfer_memory (CORE_ADDR memaddr, char *our, int len,
		   int write, process_stratum_target *target)
{
  BOOL success;
  SIZE_T done = 0;
  DWORD lasterror = 0;
  uintptr_t addr = (uintptr_t) memaddr;

  if (write)
    {
      success = WriteProcessMemory (current_process_handle, (LPVOID) addr,
				    (LPCVOID) our, len, &done);
      if (!success)
	lasterror = GetLastError ();
      FlushInstructionCache (current_process_handle, (LPCVOID) addr, len);
    }
  else
    {
      success = ReadProcessMemory (current_process_handle, (LPCVOID) addr,
				   (LPVOID) our, len, &done);
      if (!success)
	lasterror = GetLastError ();
    }
  if (!success && lasterror == ERROR_PARTIAL_COPY && done > 0)
    return done;
  else
    return success ? done : -1;
}

/* Clear out any old thread list and reinitialize it to a pristine
   state. */
static void
child_init_thread_list (void)
{
  for_each_thread (delete_thread_info);
}

/* Zero during the child initialization phase, and nonzero otherwise.  */

static int child_initialization_done = 0;

static void
do_initial_child_stuff (HANDLE proch, DWORD pid, int attached)
{
  struct process_info *proc;

  last_sig = GDB_SIGNAL_0;

  current_process_handle = proch;
  current_process_id = pid;
  main_thread_id = 0;

  soft_interrupt_requested = 0;
  faked_breakpoint = 0;
  open_process_used = true;

  memset (&current_event, 0, sizeof (current_event));

#ifdef __x86_64__
  BOOL wow64;
  if (!IsWow64Process (proch, &wow64))
    {
      DWORD err = GetLastError ();
      error ("Check if WOW64 process failed (error %d): %s\n",
	     (int) err, strwinerror (err));
    }
  wow64_process = wow64;

  if (wow64_process
      && (Wow64GetThreadContext == nullptr
	  || Wow64SetThreadContext == nullptr))
    error ("WOW64 debugging is not supported on this system.\n");

  ignore_first_breakpoint = !attached && wow64_process;
#endif

  proc = add_process (pid, attached);
#ifdef __x86_64__
  if (wow64_process)
    proc->tdesc = wow64_win32_tdesc;
  else
#endif
    proc->tdesc = win32_tdesc;
  child_init_thread_list ();
  child_initialization_done = 0;

  if (the_low_target.initial_stuff != NULL)
    (*the_low_target.initial_stuff) ();

  cached_status.set_ignore ();

  /* Flush all currently pending debug events (thread and dll list) up
     to the initial breakpoint.  */
  while (1)
    {
      struct target_waitstatus status;

      the_target->wait (minus_one_ptid, &status, 0);

      /* Note win32_wait doesn't return thread events.  */
      if (status.kind () != TARGET_WAITKIND_LOADED)
	{
	  cached_status = status;
	  break;
	}

      {
	struct thread_resume resume;

	resume.thread = minus_one_ptid;
	resume.kind = resume_continue;
	resume.sig = 0;

	the_target->resume (&resume, 1);
      }
    }

  /* Now that the inferior has been started and all DLLs have been mapped,
     we can iterate over all DLLs and load them in.

     We avoid doing it any earlier because, on certain versions of Windows,
     LOAD_DLL_DEBUG_EVENTs are sometimes not complete.  In particular,
     we have seen on Windows 8.1 that the ntdll.dll load event does not
     include the DLL name, preventing us from creating an associated SO.
     A possible explanation is that ntdll.dll might be mapped before
     the SO info gets created by the Windows system -- ntdll.dll is
     the first DLL to be reported via LOAD_DLL_DEBUG_EVENT and other DLLs
     do not seem to suffer from that problem.

     Rather than try to work around this sort of issue, it is much
     simpler to just ignore DLL load/unload events during the startup
     phase, and then process them all in one batch now.  */
  windows_add_all_dlls ();

  child_initialization_done = 1;
}

/* Resume all artificially suspended threads if we are continuing
   execution.  */
static void
continue_one_thread (thread_info *thread, int thread_id)
{
  windows_thread_info *th = (windows_thread_info *) thread_target_data (thread);

  if (thread_id == -1 || thread_id == th->tid)
    {
      win32_prepare_to_resume (th);

      if (th->suspended)
	{
	  DWORD *context_flags;
#ifdef __x86_64__
	  if (wow64_process)
	    context_flags = &th->wow64_context.ContextFlags;
	  else
#endif
	    context_flags = &th->context.ContextFlags;
	  if (*context_flags)
	    {
	      win32_set_thread_context (th);
	      *context_flags = 0;
	    }

	  th->resume ();
	}
    }
}

static BOOL
child_continue (DWORD continue_status, int thread_id)
{
  desired_stop_thread_id = thread_id;
  if (matching_pending_stop (debug_threads))
    return TRUE;

  /* The inferior will only continue after the ContinueDebugEvent
     call.  */
  for_each_thread ([&] (thread_info *thread)
    {
      continue_one_thread (thread, thread_id);
    });
  faked_breakpoint = 0;

  return continue_last_debug_event (continue_status, debug_threads);
}

/* Fetch register(s) from the current thread context.  */
static void
child_fetch_inferior_registers (struct regcache *regcache, int r)
{
  int regno;
  windows_thread_info *th = thread_rec (current_thread_ptid (),
					INVALIDATE_CONTEXT);
  if (r == -1 || r > NUM_REGS)
    child_fetch_inferior_registers (regcache, NUM_REGS);
  else
    for (regno = 0; regno < r; regno++)
      (*the_low_target.fetch_inferior_register) (regcache, th, regno);
}

/* Store a new register value into the current thread context.  We don't
   change the program's context until later, when we resume it.  */
static void
child_store_inferior_registers (struct regcache *regcache, int r)
{
  int regno;
  windows_thread_info *th = thread_rec (current_thread_ptid (),
					INVALIDATE_CONTEXT);
  if (r == -1 || r == 0 || r > NUM_REGS)
    child_store_inferior_registers (regcache, NUM_REGS);
  else
    for (regno = 0; regno < r; regno++)
      (*the_low_target.store_inferior_register) (regcache, th, regno);
}

/* Map the Windows error number in ERROR to a locale-dependent error
   message string and return a pointer to it.  Typically, the values
   for ERROR come from GetLastError.

   The string pointed to shall not be modified by the application,
   but may be overwritten by a subsequent call to strwinerror

   The strwinerror function does not change the current setting
   of GetLastError.  */

char *
strwinerror (DWORD error)
{
  static char buf[1024];
  TCHAR *msgbuf;
  DWORD lasterr = GetLastError ();
  DWORD chars = FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM
			       | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			       NULL,
			       error,
			       0, /* Default language */
			       (LPTSTR) &msgbuf,
			       0,
			       NULL);
  if (chars != 0)
    {
      /* If there is an \r\n appended, zap it.  */
      if (chars >= 2
	  && msgbuf[chars - 2] == '\r'
	  && msgbuf[chars - 1] == '\n')
	{
	  chars -= 2;
	  msgbuf[chars] = 0;
	}

      if (chars > ((COUNTOF (buf)) - 1))
	{
	  chars = COUNTOF (buf) - 1;
	  msgbuf [chars] = 0;
	}

#ifdef UNICODE
      wcstombs (buf, msgbuf, chars + 1);
#else
      strncpy (buf, msgbuf, chars + 1);
#endif
      LocalFree (msgbuf);
    }
  else
    sprintf (buf, "unknown win32 error (%u)", (unsigned) error);

  SetLastError (lasterr);
  return buf;
}

static BOOL
create_process (const char *program, char *args,
		DWORD flags, PROCESS_INFORMATION *pi)
{
  const std::string &inferior_cwd = get_inferior_cwd ();
  BOOL ret;
  size_t argslen, proglen;

  proglen = strlen (program) + 1;
  argslen = strlen (args) + proglen;

  STARTUPINFOA si = { sizeof (STARTUPINFOA) };
  char *program_and_args = (char *) alloca (argslen + 1);

  strcpy (program_and_args, program);
  strcat (program_and_args, " ");
  strcat (program_and_args, args);
  ret = CreateProcessA (program,           /* image name */
			program_and_args,  /* command line */
			NULL,              /* security */
			NULL,              /* thread */
			TRUE,              /* inherit handles */
			flags,             /* start flags */
			NULL,              /* environment */
			/* current directory */
			(inferior_cwd.empty ()
			 ? NULL
			 : gdb_tilde_expand (inferior_cwd.c_str ()).c_str()),
			&si,               /* start info */
			pi);               /* proc info */

  return ret;
}

/* Start a new process.
   PROGRAM is the program name.
   PROGRAM_ARGS is the vector containing the inferior's args.
   Returns the new PID on success, -1 on failure.  Registers the new
   process with the process list.  */
int
win32_process_target::create_inferior (const char *program,
				       const std::vector<char *> &program_args)
{
  client_state &cs = get_client_state ();
#ifndef USE_WIN32API
  char real_path[PATH_MAX];
  char *orig_path, *new_path, *path_ptr;
#endif
  BOOL ret;
  DWORD flags;
  PROCESS_INFORMATION pi;
  DWORD err;
  std::string str_program_args = construct_inferior_arguments (program_args);
  char *args = (char *) str_program_args.c_str ();

  /* win32_wait needs to know we're not attaching.  */
  attaching = 0;

  if (!program)
    error ("No executable specified, specify executable to debug.\n");

  flags = DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS;

#ifndef USE_WIN32API
  orig_path = NULL;
  path_ptr = getenv ("PATH");
  if (path_ptr)
    {
      int size = cygwin_conv_path_list (CCP_POSIX_TO_WIN_A, path_ptr, NULL, 0);
      orig_path = (char *) alloca (strlen (path_ptr) + 1);
      new_path = (char *) alloca (size);
      strcpy (orig_path, path_ptr);
      cygwin_conv_path_list (CCP_POSIX_TO_WIN_A, path_ptr, new_path, size);
      setenv ("PATH", new_path, 1);
     }
  cygwin_conv_path (CCP_POSIX_TO_WIN_A, program, real_path, PATH_MAX);
  program = real_path;
#endif

  OUTMSG2 (("Command line is \"%s %s\"\n", program, args));

#ifdef CREATE_NEW_PROCESS_GROUP
  flags |= CREATE_NEW_PROCESS_GROUP;
#endif

  ret = create_process (program, args, flags, &pi);
  err = GetLastError ();
  if (!ret && err == ERROR_FILE_NOT_FOUND)
    {
      char *exename = (char *) alloca (strlen (program) + 5);
      strcat (strcpy (exename, program), ".exe");
      ret = create_process (exename, args, flags, &pi);
      err = GetLastError ();
    }

#ifndef USE_WIN32API
  if (orig_path)
    setenv ("PATH", orig_path, 1);
#endif

  if (!ret)
    {
      error ("Error creating process \"%s %s\", (error %d): %s\n",
	     program, args, (int) err, strwinerror (err));
    }
  else
    {
      OUTMSG2 (("Process created: %s %s\n", program, (char *) args));
    }

  CloseHandle (pi.hThread);

  do_initial_child_stuff (pi.hProcess, pi.dwProcessId, 0);

  /* Wait till we are at 1st instruction in program, return new pid
     (assuming success).  */
  cs.last_ptid = wait (ptid_t (current_process_id), &cs.last_status, 0);

  /* Necessary for handle_v_kill.  */
  signal_pid = current_process_id;

  return current_process_id;
}

/* Attach to a running process.
   PID is the process ID to attach to, specified by the user
   or a higher layer.  */
int
win32_process_target::attach (unsigned long pid)
{
  HANDLE h;
  DWORD err;

  h = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pid);
  if (h != NULL)
    {
      if (DebugActiveProcess (pid))
	{
	  DebugSetProcessKillOnExit (FALSE);

	  /* win32_wait needs to know we're attaching.  */
	  attaching = 1;
	  do_initial_child_stuff (h, pid, 1);
	  return 0;
	}

      CloseHandle (h);
    }

  err = GetLastError ();
  error ("Attach to process failed (error %d): %s\n",
	 (int) err, strwinerror (err));
}

/* See nat/windows-nat.h.  */

int
windows_nat::handle_output_debug_string (struct target_waitstatus *ourstatus)
{
#define READ_BUFFER_LEN 1024
  CORE_ADDR addr;
  char s[READ_BUFFER_LEN + 1] = { 0 };
  DWORD nbytes = current_event.u.DebugString.nDebugStringLength;

  if (nbytes == 0)
    return 0;

  if (nbytes > READ_BUFFER_LEN)
    nbytes = READ_BUFFER_LEN;

  addr = (CORE_ADDR) (size_t) current_event.u.DebugString.lpDebugStringData;

  if (current_event.u.DebugString.fUnicode)
    {
      /* The event tells us how many bytes, not chars, even
	 in Unicode.  */
      WCHAR buffer[(READ_BUFFER_LEN + 1) / sizeof (WCHAR)] = { 0 };
      if (read_inferior_memory (addr, (unsigned char *) buffer, nbytes) != 0)
	return 0;
      wcstombs (s, buffer, (nbytes + 1) / sizeof (WCHAR));
    }
  else
    {
      if (read_inferior_memory (addr, (unsigned char *) s, nbytes) != 0)
	return 0;
    }

  if (!startswith (s, "cYg"))
    {
      if (!server_waiting)
	{
	  OUTMSG2(("%s", s));
	  return 0;
	}

      monitor_output (s);
    }
#undef READ_BUFFER_LEN

  return 0;
}

static void
win32_clear_inferiors (void)
{
  if (open_process_used)
    {
      CloseHandle (current_process_handle);
      open_process_used = false;
    }

  for_each_thread (delete_thread_info);
  siginfo_er.ExceptionCode = 0;
  clear_inferiors ();
}

/* Implementation of target_ops::kill.  */

int
win32_process_target::kill (process_info *process)
{
  TerminateProcess (current_process_handle, 0);
  for (;;)
    {
      if (!child_continue (DBG_CONTINUE, -1))
	break;
      if (!wait_for_debug_event (&current_event, INFINITE))
	break;
      if (current_event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
	break;
      else if (current_event.dwDebugEventCode == OUTPUT_DEBUG_STRING_EVENT)
	handle_output_debug_string (nullptr);
    }

  win32_clear_inferiors ();

  remove_process (process);
  return 0;
}

/* Implementation of target_ops::detach.  */

int
win32_process_target::detach (process_info *process)
{
  struct thread_resume resume;
  resume.thread = minus_one_ptid;
  resume.kind = resume_continue;
  resume.sig = 0;
  this->resume (&resume, 1);

  if (!DebugActiveProcessStop (current_process_id))
    return -1;

  DebugSetProcessKillOnExit (FALSE);
  remove_process (process);

  win32_clear_inferiors ();
  return 0;
}

void
win32_process_target::mourn (struct process_info *process)
{
  remove_process (process);
}

/* Implementation of target_ops::join.  */

void
win32_process_target::join (int pid)
{
  HANDLE h = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pid);
  if (h != NULL)
    {
      WaitForSingleObject (h, INFINITE);
      CloseHandle (h);
    }
}

/* Return true iff the thread with thread ID TID is alive.  */
bool
win32_process_target::thread_alive (ptid_t ptid)
{
  /* Our thread list is reliable; don't bother to poll target
     threads.  */
  return find_thread_ptid (ptid) != NULL;
}

/* Resume the inferior process.  RESUME_INFO describes how we want
   to resume.  */
void
win32_process_target::resume (thread_resume *resume_info, size_t n)
{
  DWORD tid;
  enum gdb_signal sig;
  int step;
  windows_thread_info *th;
  DWORD continue_status = DBG_CONTINUE;
  ptid_t ptid;

  /* This handles the very limited set of resume packets that GDB can
     currently produce.  */

  if (n == 1 && resume_info[0].thread == minus_one_ptid)
    tid = -1;
  else if (n > 1)
    tid = -1;
  else
    /* Yes, we're ignoring resume_info[0].thread.  It'd be tricky to make
       the Windows resume code do the right thing for thread switching.  */
    tid = current_event.dwThreadId;

  if (resume_info[0].thread != minus_one_ptid)
    {
      sig = gdb_signal_from_host (resume_info[0].sig);
      step = resume_info[0].kind == resume_step;
    }
  else
    {
      sig = GDB_SIGNAL_0;
      step = 0;
    }

  if (sig != GDB_SIGNAL_0)
    {
      if (current_event.dwDebugEventCode != EXCEPTION_DEBUG_EVENT)
	{
	  OUTMSG (("Cannot continue with signal %s here.\n",
		   gdb_signal_to_string (sig)));
	}
      else if (sig == last_sig)
	continue_status = DBG_EXCEPTION_NOT_HANDLED;
      else
	OUTMSG (("Can only continue with received signal %s.\n",
		 gdb_signal_to_string (last_sig)));
    }

  last_sig = GDB_SIGNAL_0;

  /* Get context for the currently selected thread.  */
  ptid = debug_event_ptid (&current_event);
  th = thread_rec (ptid, DONT_INVALIDATE_CONTEXT);
  if (th)
    {
      win32_prepare_to_resume (th);

      DWORD *context_flags;
#ifdef __x86_64__
      if (wow64_process)
	context_flags = &th->wow64_context.ContextFlags;
      else
#endif
	context_flags = &th->context.ContextFlags;
      if (*context_flags)
	{
	  /* Move register values from the inferior into the thread
	     context structure.  */
	  regcache_invalidate ();

	  if (step)
	    {
	      if (the_low_target.single_step != NULL)
		(*the_low_target.single_step) (th);
	      else
		error ("Single stepping is not supported "
		       "in this configuration.\n");
	    }

	  win32_set_thread_context (th);
	  *context_flags = 0;
	}
    }

  /* Allow continuing with the same signal that interrupted us.
     Otherwise complain.  */

  child_continue (continue_status, tid);
}

/* See nat/windows-nat.h.  */

void
windows_nat::handle_load_dll (const char *name, LPVOID base)
{
  CORE_ADDR load_addr = (CORE_ADDR) (uintptr_t) base;

  char buf[MAX_PATH + 1];
  char buf2[MAX_PATH + 1];

  WIN32_FIND_DATAA w32_fd;
  HANDLE h = FindFirstFileA (name, &w32_fd);

  /* The symbols in a dll are offset by 0x1000, which is the
     offset from 0 of the first byte in an image - because
     of the file header and the section alignment. */
  load_addr += 0x1000;

  if (h == INVALID_HANDLE_VALUE)
    strcpy (buf, name);
  else
    {
      FindClose (h);
      strcpy (buf, name);
      {
	char cwd[MAX_PATH + 1];
	char *p;
	if (GetCurrentDirectoryA (MAX_PATH + 1, cwd))
	  {
	    p = strrchr (buf, '\\');
	    if (p)
	      p[1] = '\0';
	    SetCurrentDirectoryA (buf);
	    GetFullPathNameA (w32_fd.cFileName, MAX_PATH, buf, &p);
	    SetCurrentDirectoryA (cwd);
	  }
      }
    }

  if (strcasecmp (buf, "ntdll.dll") == 0)
    {
      GetSystemDirectoryA (buf, sizeof (buf));
      strcat (buf, "\\ntdll.dll");
    }

#ifdef __CYGWIN__
  cygwin_conv_path (CCP_WIN_A_TO_POSIX, buf, buf2, sizeof (buf2));
#else
  strcpy (buf2, buf);
#endif

  loaded_dll (buf2, load_addr);
}

/* See nat/windows-nat.h.  */

void
windows_nat::handle_unload_dll ()
{
  CORE_ADDR load_addr =
	  (CORE_ADDR) (uintptr_t) current_event.u.UnloadDll.lpBaseOfDll;

  /* The symbols in a dll are offset by 0x1000, which is the
     offset from 0 of the first byte in an image - because
     of the file header and the section alignment. */
  load_addr += 0x1000;
  unloaded_dll (NULL, load_addr);
}

static void
suspend_one_thread (thread_info *thread)
{
  windows_thread_info *th = (windows_thread_info *) thread_target_data (thread);

  th->suspend ();
}

static void
fake_breakpoint_event (void)
{
  OUTMSG2(("fake_breakpoint_event\n"));

  faked_breakpoint = 1;

  memset (&current_event, 0, sizeof (current_event));
  current_event.dwThreadId = main_thread_id;
  current_event.dwDebugEventCode = EXCEPTION_DEBUG_EVENT;
  current_event.u.Exception.ExceptionRecord.ExceptionCode
    = EXCEPTION_BREAKPOINT;

  for_each_thread (suspend_one_thread);
}

/* See nat/windows-nat.h.  */

bool
windows_nat::handle_ms_vc_exception (const EXCEPTION_RECORD *rec)
{
  return false;
}

/* See nat/windows-nat.h.  */

bool
windows_nat::handle_access_violation (const EXCEPTION_RECORD *rec)
{
  return false;
}

/* A helper function that will, if needed, set
   'stopped_at_software_breakpoint' on the thread and adjust the
   PC.  */

static void
maybe_adjust_pc ()
{
  struct regcache *regcache = get_thread_regcache (current_thread, 1);
  child_fetch_inferior_registers (regcache, -1);

  windows_thread_info *th = thread_rec (current_thread_ptid (),
					DONT_INVALIDATE_CONTEXT);
  th->stopped_at_software_breakpoint = false;

  if (current_event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT
      && ((current_event.u.Exception.ExceptionRecord.ExceptionCode
	   == EXCEPTION_BREAKPOINT)
	  || (current_event.u.Exception.ExceptionRecord.ExceptionCode
	      == STATUS_WX86_BREAKPOINT))
      && child_initialization_done)
    {
      th->stopped_at_software_breakpoint = true;
      CORE_ADDR pc = regcache_read_pc (regcache);
      CORE_ADDR sw_breakpoint_pc = pc - the_low_target.decr_pc_after_break;
      regcache_write_pc (regcache, sw_breakpoint_pc);
    }
}

/* Get the next event from the child.  */

static int
get_child_debug_event (DWORD *continue_status,
		       struct target_waitstatus *ourstatus)
{
  ptid_t ptid;

  last_sig = GDB_SIGNAL_0;
  ourstatus->set_spurious ();
  *continue_status = DBG_CONTINUE;

  /* Check if GDB sent us an interrupt request.  */
  check_remote_input_interrupt_request ();

  if (soft_interrupt_requested)
    {
      soft_interrupt_requested = 0;
      fake_breakpoint_event ();
      goto gotevent;
    }

  attaching = 0;
  {
    gdb::optional<pending_stop> stop = fetch_pending_stop (debug_threads);
    if (stop.has_value ())
      {
	*ourstatus = stop->status;
	current_event = stop->event;
	ptid = debug_event_ptid (&current_event);
	switch_to_thread (find_thread_ptid (ptid));
	return 1;
      }

    /* Keep the wait time low enough for comfortable remote
       interruption, but high enough so gdbserver doesn't become a
       bottleneck.  */
    if (!wait_for_debug_event (&current_event, 250))
      {
	DWORD e  = GetLastError();

	if (e == ERROR_PIPE_NOT_CONNECTED)
	  {
	    /* This will happen if the loader fails to succesfully
	       load the application, e.g., if the main executable
	       tries to pull in a non-existing export from a
	       DLL.  */
	    ourstatus->set_exited (1);
	    return 1;
	  }

	return 0;
      }
  }

 gotevent:

  switch (current_event.dwDebugEventCode)
    {
    case CREATE_THREAD_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event CREATE_THREAD_DEBUG_EVENT "
		"for pid=%u tid=%x)\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));

      /* Record the existence of this thread.  */
      child_add_thread (current_event.dwProcessId,
			current_event.dwThreadId,
			current_event.u.CreateThread.hThread,
			current_event.u.CreateThread.lpThreadLocalBase);
      break;

    case EXIT_THREAD_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event EXIT_THREAD_DEBUG_EVENT "
		"for pid=%u tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      child_delete_thread (current_event.dwProcessId,
			   current_event.dwThreadId);

      switch_to_thread (get_first_thread ());
      return 1;

    case CREATE_PROCESS_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event CREATE_PROCESS_DEBUG_EVENT "
		"for pid=%u tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      CloseHandle (current_event.u.CreateProcessInfo.hFile);

      if (open_process_used)
	{
	  CloseHandle (current_process_handle);
	  open_process_used = false;
	}

      current_process_handle = current_event.u.CreateProcessInfo.hProcess;
      main_thread_id = current_event.dwThreadId;

      /* Add the main thread.  */
      child_add_thread (current_event.dwProcessId,
			main_thread_id,
			current_event.u.CreateProcessInfo.hThread,
			current_event.u.CreateProcessInfo.lpThreadLocalBase);
      break;

    case EXIT_PROCESS_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event EXIT_PROCESS_DEBUG_EVENT "
		"for pid=%u tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      {
	DWORD exit_status = current_event.u.ExitProcess.dwExitCode;
	/* If the exit status looks like a fatal exception, but we
	   don't recognize the exception's code, make the original
	   exit status value available, to avoid losing information.  */
	int exit_signal
	  = WIFSIGNALED (exit_status) ? WTERMSIG (exit_status) : -1;
	if (exit_signal == -1)
	  ourstatus->set_exited (exit_status);
	else
	  ourstatus->set_signalled (gdb_signal_from_host (exit_signal));
      }
      child_continue (DBG_CONTINUE, desired_stop_thread_id);
      break;

    case LOAD_DLL_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event LOAD_DLL_DEBUG_EVENT "
		"for pid=%u tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      CloseHandle (current_event.u.LoadDll.hFile);
      if (! child_initialization_done)
	break;
      dll_loaded_event ();

      ourstatus->set_loaded ();
      break;

    case UNLOAD_DLL_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event UNLOAD_DLL_DEBUG_EVENT "
		"for pid=%u tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      if (! child_initialization_done)
	break;
      handle_unload_dll ();
      ourstatus->set_loaded ();
      break;

    case EXCEPTION_DEBUG_EVENT:
      OUTMSG2 (("gdbserver: kernel event EXCEPTION_DEBUG_EVENT "
		"for pid=%u tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      if (handle_exception (ourstatus, debug_threads)
	  == HANDLE_EXCEPTION_UNHANDLED)
	*continue_status = DBG_EXCEPTION_NOT_HANDLED;
      break;

    case OUTPUT_DEBUG_STRING_EVENT:
      /* A message from the kernel (or Cygwin).  */
      OUTMSG2 (("gdbserver: kernel event OUTPUT_DEBUG_STRING_EVENT "
		"for pid=%u tid=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId));
      handle_output_debug_string (nullptr);
      break;

    default:
      OUTMSG2 (("gdbserver: kernel event unknown "
		"for pid=%u tid=%x code=%x\n",
		(unsigned) current_event.dwProcessId,
		(unsigned) current_event.dwThreadId,
		(unsigned) current_event.dwDebugEventCode));
      break;
    }

  ptid = debug_event_ptid (&current_event);

  if (desired_stop_thread_id != -1 && desired_stop_thread_id != ptid.lwp ())
    {
      /* Pending stop.  See the comment by the definition of
	 "pending_stops" for details on why this is needed.  */
      OUTMSG2 (("get_windows_debug_event - "
		"unexpected stop in 0x%lx (expecting 0x%x)\n",
		ptid.lwp (), desired_stop_thread_id));
      maybe_adjust_pc ();
      pending_stops.push_back ({(DWORD) ptid.lwp (), *ourstatus, current_event});
      ourstatus->set_spurious ();
    }
  else
    switch_to_thread (find_thread_ptid (ptid));

  return 1;
}

/* Wait for the inferior process to change state.
   STATUS will be filled in with a response code to send to GDB.
   Returns the signal which caused the process to stop. */
ptid_t
win32_process_target::wait (ptid_t ptid, target_waitstatus *ourstatus,
			    target_wait_flags options)
{
  if (cached_status.kind () != TARGET_WAITKIND_IGNORE)
    {
      /* The core always does a wait after creating the inferior, and
	 do_initial_child_stuff already ran the inferior to the
	 initial breakpoint (or an exit, if creating the process
	 fails).  Report it now.  */
      *ourstatus = cached_status;
      cached_status.set_ignore ();
      return debug_event_ptid (&current_event);
    }

  while (1)
    {
      DWORD continue_status;
      if (!get_child_debug_event (&continue_status, ourstatus))
	continue;

      switch (ourstatus->kind ())
	{
	case TARGET_WAITKIND_EXITED:
	  OUTMSG2 (("Child exited with retcode = %x\n",
		    ourstatus->exit_status ()));
	  win32_clear_inferiors ();
	  return ptid_t (current_event.dwProcessId);
	case TARGET_WAITKIND_STOPPED:
	case TARGET_WAITKIND_SIGNALLED:
	case TARGET_WAITKIND_LOADED:
	  {
	    OUTMSG2 (("Child Stopped with signal = %d \n",
		      ourstatus->sig ()));
	    maybe_adjust_pc ();
	    return debug_event_ptid (&current_event);
	  }
	default:
	  OUTMSG (("Ignoring unknown internal event, %d\n",
		  ourstatus->kind ()));
	  /* fall-through */
	case TARGET_WAITKIND_SPURIOUS:
	  /* do nothing, just continue */
	  child_continue (continue_status, desired_stop_thread_id);
	  break;
	}
    }
}

/* Fetch registers from the inferior process.
   If REGNO is -1, fetch all registers; otherwise, fetch at least REGNO.  */
void
win32_process_target::fetch_registers (regcache *regcache, int regno)
{
  child_fetch_inferior_registers (regcache, regno);
}

/* Store registers to the inferior process.
   If REGNO is -1, store all registers; otherwise, store at least REGNO.  */
void
win32_process_target::store_registers (regcache *regcache, int regno)
{
  child_store_inferior_registers (regcache, regno);
}

/* Read memory from the inferior process.  This should generally be
   called through read_inferior_memory, which handles breakpoint shadowing.
   Read LEN bytes at MEMADDR into a buffer at MYADDR.  */
int
win32_process_target::read_memory (CORE_ADDR memaddr, unsigned char *myaddr,
				   int len)
{
  return child_xfer_memory (memaddr, (char *) myaddr, len, 0, 0) != len;
}

/* Write memory to the inferior process.  This should generally be
   called through write_inferior_memory, which handles breakpoint shadowing.
   Write LEN bytes from the buffer at MYADDR to MEMADDR.
   Returns 0 on success and errno on failure.  */
int
win32_process_target::write_memory (CORE_ADDR memaddr,
				    const unsigned char *myaddr, int len)
{
  return child_xfer_memory (memaddr, (char *) myaddr, len, 1, 0) != len;
}

/* Send an interrupt request to the inferior process. */
void
win32_process_target::request_interrupt ()
{
  if (GenerateConsoleCtrlEvent (CTRL_BREAK_EVENT, current_process_id))
    return;

  /* GenerateConsoleCtrlEvent can fail if process id being debugged is
     not a process group id.
     Fallback to XP/Vista 'DebugBreakProcess', which generates a
     breakpoint exception in the interior process.  */

  if (DebugBreakProcess (current_process_handle))
    return;

  /* Last resort, suspend all threads manually.  */
  soft_interrupt_requested = 1;
}

bool
win32_process_target::supports_hardware_single_step ()
{
  return true;
}

bool
win32_process_target::supports_qxfer_siginfo ()
{
  return true;
}

/* Write Windows signal info.  */

int
win32_process_target::qxfer_siginfo (const char *annex,
				     unsigned char *readbuf,
				     unsigned const char *writebuf,
				     CORE_ADDR offset, int len)
{
  if (siginfo_er.ExceptionCode == 0)
    return -1;

  if (readbuf == nullptr)
    return -1;

  char *buf = (char *) &siginfo_er;
  size_t bufsize = sizeof (siginfo_er);

#ifdef __x86_64__
  EXCEPTION_RECORD32 er32;
  if (wow64_process)
    {
      buf = (char *) &er32;
      bufsize = sizeof (er32);

      er32.ExceptionCode = siginfo_er.ExceptionCode;
      er32.ExceptionFlags = siginfo_er.ExceptionFlags;
      er32.ExceptionRecord = (uintptr_t) siginfo_er.ExceptionRecord;
      er32.ExceptionAddress = (uintptr_t) siginfo_er.ExceptionAddress;
      er32.NumberParameters = siginfo_er.NumberParameters;
      int i;
      for (i = 0; i < EXCEPTION_MAXIMUM_PARAMETERS; i++)
	er32.ExceptionInformation[i] = siginfo_er.ExceptionInformation[i];
    }
#endif

  if (offset > bufsize)
    return -1;

  if (offset + len > bufsize)
    len = bufsize - offset;

  memcpy (readbuf, buf + offset, len);

  return len;
}

bool
win32_process_target::supports_get_tib_address ()
{
  return true;
}

/* Write Windows OS Thread Information Block address.  */

int
win32_process_target::get_tib_address (ptid_t ptid, CORE_ADDR *addr)
{
  windows_thread_info *th;
  th = thread_rec (ptid, DONT_INVALIDATE_CONTEXT);
  if (th == NULL)
    return 0;
  if (addr != NULL)
    *addr = th->thread_local_base;
  return 1;
}

/* Implementation of the target_ops method "sw_breakpoint_from_kind".  */

const gdb_byte *
win32_process_target::sw_breakpoint_from_kind (int kind, int *size)
{
  *size = the_low_target.breakpoint_len;
  return the_low_target.breakpoint;
}

bool
win32_process_target::stopped_by_sw_breakpoint ()
{
  windows_thread_info *th = thread_rec (current_thread_ptid (),
					DONT_INVALIDATE_CONTEXT);
  return th == nullptr ? false : th->stopped_at_software_breakpoint;
}

bool
win32_process_target::supports_stopped_by_sw_breakpoint ()
{
  return true;
}

CORE_ADDR
win32_process_target::read_pc (struct regcache *regcache)
{
  return (*the_low_target.get_pc) (regcache);
}

void
win32_process_target::write_pc (struct regcache *regcache, CORE_ADDR pc)
{
  return (*the_low_target.set_pc) (regcache, pc);
}

/* The win32 target ops object.  */

static win32_process_target the_win32_target;

/* Initialize the Win32 backend.  */
void
initialize_low (void)
{
  set_target_ops (&the_win32_target);
  the_low_target.arch_setup ();

  initialize_loadable ();
}
