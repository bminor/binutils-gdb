/* Target-vector operations for controlling win32 child processes, for GDB.
   Copyright 1995, 1996, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.
   Contributed by Cygnus Solutions, A Red Hat Company.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

/* by Steve Chamberlain, sac@cygnus.com */

/* We assume we're being built with and will be used for cygwin.  */

#include "defs.h"
#include "frame.h"		/* required by inferior.h */
#include "inferior.h"
#include "target.h"
#include "gdbcore.h"
#include "command.h"
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <windows.h>
#include <imagehlp.h>
#include <sys/cygwin.h>

#include "buildsym.h"
#include "symfile.h"
#include "objfiles.h"
#include "gdb_string.h"
#include "gdbthread.h"
#include "gdbcmd.h"
#include <sys/param.h>
#include <unistd.h>

/* The ui's event loop. */
extern int (*ui_loop_hook) (int signo);

/* If we're not using the old Cygwin header file set, define the
   following which never should have been in the generic Win32 API
   headers in the first place since they were our own invention... */
#ifndef _GNU_H_WINDOWS_H
enum
  {
    FLAG_TRACE_BIT = 0x100,
    CONTEXT_DEBUGGER = (CONTEXT_FULL | CONTEXT_FLOATING_POINT)
  };
#endif
#include <sys/procfs.h>
#include <psapi.h>

/* The string sent by cygwin when it processes a signal.
   FIXME: This should be in a cygwin include file. */
#define CYGWIN_SIGNAL_STRING "cygwin: signal"

#define CHECK(x)	check (x, __FILE__,__LINE__)
#define DEBUG_EXEC(x)	if (debug_exec)		printf x
#define DEBUG_EVENTS(x)	if (debug_events)	printf x
#define DEBUG_MEM(x)	if (debug_memory)	printf x
#define DEBUG_EXCEPT(x)	if (debug_exceptions)	printf x

/* Forward declaration */
extern struct target_ops child_ops;

static void child_stop (void);
static int win32_child_thread_alive (int);
void child_kill_inferior (void);

static int last_sig = 0;	/* Set if a signal was received from the
				   debugged process */
/* Thread information structure used to track information that is
   not available in gdb's thread structure. */
typedef struct thread_info_struct
  {
    struct thread_info_struct *next;
    DWORD id;
    HANDLE h;
    char *name;
    int suspend_count;
    CONTEXT context;
    STACKFRAME sf;
  }
thread_info;

static thread_info thread_head;

/* The process and thread handles for the above context. */

static DEBUG_EVENT current_event;	/* The current debug event from
					   WaitForDebugEvent */
static HANDLE current_process_handle;	/* Currently executing process */
static thread_info *current_thread;	/* Info on currently selected thread */
static DWORD main_thread_id;	/* Thread ID of the main thread */

/* Counts of things. */
static int exception_count = 0;
static int event_count = 0;

/* User options. */
static int new_console = 0;
static int new_group = 1;
static int debug_exec = 0;	/* show execution */
static int debug_events = 0;	/* show events from kernel */
static int debug_memory = 0;	/* show target memory accesses */
static int debug_exceptions = 0;	/* show target exceptions */

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

#define context_offset(x) ((int)&(((CONTEXT *)NULL)->x))
static const int mappings[] =
{
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
  context_offset (FloatSave.ErrorSelector)
};

#undef context_offset

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
  {STATUS_STACK_OVERFLOW, TARGET_SIGNAL_SEGV},
  {EXCEPTION_BREAKPOINT, TARGET_SIGNAL_TRAP},
  {DBG_CONTROL_C, TARGET_SIGNAL_INT},
  {EXCEPTION_SINGLE_STEP, TARGET_SIGNAL_TRAP},
  {-1, -1}};

/* Find a thread record given a thread id.
   If get_context then also retrieve the context for this
   thread. */
static thread_info *
thread_rec (DWORD id, int get_context)
{
  thread_info *th;

  for (th = &thread_head; (th = th->next) != NULL;)
    if (th->id == id)
      {
	if (!th->suspend_count && get_context)
	  {
	    if (get_context > 0 && id != current_event.dwThreadId)
	      th->suspend_count = SuspendThread (th->h) + 1;
	    else if (get_context < 0)
	      th->suspend_count = -1;

	    th->context.ContextFlags = CONTEXT_DEBUGGER;
	    GetThreadContext (th->h, &th->context);
	  }
	return th;
      }

  return NULL;
}

/* Add a thread to the thread list */
static thread_info *
child_add_thread (DWORD id, HANDLE h)
{
  thread_info *th;

  if ((th = thread_rec (id, FALSE)))
    return th;

  th = (thread_info *) xmalloc (sizeof (*th));
  memset (th, 0, sizeof (*th));
  th->id = id;
  th->h = h;
  th->next = thread_head.next;
  thread_head.next = th;
  add_thread (id);
  return th;
}

/* Clear out any old thread list and reintialize it to a
   pristine state. */
static void
child_init_thread_list (void)
{
  thread_info *th = &thread_head;

  DEBUG_EVENTS (("gdb: child_init_thread_list\n"));
  init_thread_list ();
  while (th->next != NULL)
    {
      thread_info *here = th->next;
      th->next = here->next;
      (void) CloseHandle (here->h);
      free (here);
    }
}

/* Delete a thread from the list of threads */
static void
child_delete_thread (DWORD id)
{
  thread_info *th;

  if (info_verbose)
    printf_unfiltered ("[Deleting %s]\n", target_pid_to_str (id));
  delete_thread (id);

  for (th = &thread_head;
       th->next != NULL && th->next->id != id;
       th = th->next)
    continue;

  if (th->next != NULL)
    {
      thread_info *here = th->next;
      th->next = here->next;
      CloseHandle (here->h);
      free (here);
    }
}

static void
check (BOOL ok, const char *file, int line)
{
  if (!ok)
    printf_filtered ("error return %s:%d was %lu\n", file, line, GetLastError ());
}

static void
do_child_fetch_inferior_registers (int r)
{
  char *context_offset = ((char *) &current_thread->context) + mappings[r];
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
  else if (r >= 0)
    supply_register (r, context_offset);
  else
    {
      for (r = 0; r < NUM_REGS; r++)
	do_child_fetch_inferior_registers (r);
    }
}

static void
child_fetch_inferior_registers (int r)
{
  current_thread = thread_rec (inferior_pid, TRUE);
  do_child_fetch_inferior_registers (r);
}

static void
do_child_store_inferior_registers (int r)
{
  if (r >= 0)
    read_register_gen (r, ((char *) &current_thread->context) + mappings[r]);
  else
    {
      for (r = 0; r < NUM_REGS; r++)
	do_child_store_inferior_registers (r);
    }
}

/* Store a new register value into the current thread context */
static void
child_store_inferior_registers (int r)
{
  current_thread = thread_rec (inferior_pid, TRUE);
  do_child_store_inferior_registers (r);
}

static int psapi_loaded = 0;
static HMODULE psapi_module_handle = NULL;
static BOOL WINAPI (*psapi_EnumProcessModules) (HANDLE, HMODULE *, DWORD, LPDWORD) = NULL;
static BOOL WINAPI (*psapi_GetModuleInformation) (HANDLE, HMODULE, LPMODULEINFO, DWORD) = NULL;
static DWORD WINAPI (*psapi_GetModuleFileNameExA) (HANDLE, HMODULE, LPSTR, DWORD) = NULL;

int 
psapi_get_dll_name (DWORD BaseAddress, char *dll_name_ret)
{
  DWORD len;
  MODULEINFO mi;
  int i;
  HMODULE dh_buf[1];
  HMODULE *DllHandle = dh_buf;
  DWORD cbNeeded;
  BOOL ok;

  if (!psapi_loaded ||
      psapi_EnumProcessModules == NULL ||
      psapi_GetModuleInformation == NULL ||
      psapi_GetModuleFileNameExA == NULL)
    {
      if (psapi_loaded)
	goto failed;
      psapi_loaded = 1;
      psapi_module_handle = LoadLibrary ("psapi.dll");
      if (!psapi_module_handle)
	{
	  /* printf_unfiltered ("error loading psapi.dll: %u", GetLastError ()); */
	  goto failed;
	}
      psapi_EnumProcessModules = GetProcAddress (psapi_module_handle, "EnumProcessModules");
      psapi_GetModuleInformation = GetProcAddress (psapi_module_handle, "GetModuleInformation");
      psapi_GetModuleFileNameExA = (void *) GetProcAddress (psapi_module_handle,
						    "GetModuleFileNameExA");
      if (psapi_EnumProcessModules == NULL ||
	  psapi_GetModuleInformation == NULL ||
	  psapi_GetModuleFileNameExA == NULL)
	goto failed;
    }

  cbNeeded = 0;
  ok = (*psapi_EnumProcessModules) (current_process_handle,
				    DllHandle,
				    sizeof (HMODULE),
				    &cbNeeded);

  if (!ok || !cbNeeded)
    goto failed;

  DllHandle = (HMODULE *) alloca (cbNeeded);
  if (!DllHandle)
    goto failed;

  ok = (*psapi_EnumProcessModules) (current_process_handle,
				    DllHandle,
				    cbNeeded,
				    &cbNeeded);
  if (!ok)
    goto failed;

  for (i = 0; i < (int) (cbNeeded / sizeof (HMODULE)); i++)
    {
      if (!(*psapi_GetModuleInformation) (current_process_handle,
					  DllHandle[i],
					  &mi,
					  sizeof (mi)))
	error ("Can't get module info");

      len = (*psapi_GetModuleFileNameExA) (current_process_handle,
					   DllHandle[i],
					   dll_name_ret,
					   MAX_PATH);
      if (len == 0)
	error ("Error getting dll name: %u\n", GetLastError ());

      if ((DWORD) (mi.lpBaseOfDll) == BaseAddress)
	return 1;
    }

failed:
  dll_name_ret[0] = '\0';
  return 0;
}

/* Encapsulate the information required in a call to
   symbol_file_add_args */
struct safe_symbol_file_add_args
{
  char *name;
  int from_tty;
  struct section_addr_info *addrs;
  int mainline;
  int flags;
  struct ui_file *err, *out;
  struct objfile *ret;
};

/* Call symbol_file_add with stderr redirected.  We don't care if there
   are errors. */
static int
safe_symbol_file_add_stub (void *argv)
{
#define p ((struct safe_symbol_file_add_args *)argv)
  p->ret = symbol_file_add (p->name, p->from_tty, p->addrs, p->mainline, p->flags);
  return !!p->ret;
#undef p
}

/* Restore gdb's stderr after calling symbol_file_add */
static void
safe_symbol_file_add_cleanup (void *p)
{
#define sp ((struct safe_symbol_file_add_args *)p)
  gdb_flush (gdb_stderr);
  gdb_flush (gdb_stdout);
  ui_file_delete (gdb_stderr);
  ui_file_delete (gdb_stdout);
  gdb_stderr = sp->err;
  gdb_stdout = sp->out;
#undef sp
}

/* symbol_file_add wrapper that prevents errors from being displayed. */
static struct objfile *
safe_symbol_file_add (char *name, int from_tty,
		      struct section_addr_info *addrs,
		      int mainline, int flags)

{
  struct safe_symbol_file_add_args p;
  struct cleanup *cleanup;

  cleanup = make_cleanup (safe_symbol_file_add_cleanup, &p);

  p.err = gdb_stderr;
  p.out = gdb_stdout;
  gdb_flush (gdb_stderr);
  gdb_flush (gdb_stdout);
  gdb_stderr = ui_file_new ();
  gdb_stdout = ui_file_new ();
  p.name = name;
  p.from_tty = from_tty;
  p.addrs = addrs;
  p.mainline = mainline;
  p.flags = flags;
  catch_errors (safe_symbol_file_add_stub, &p, "", RETURN_MASK_ERROR);

  do_cleanups (cleanup);
  return p.ret;
}

/* Maintain a linked list of "so" information. */
struct so_stuff
{
  struct so_stuff *next, **last;
  DWORD load_addr;
  char name[0];
}
solib_start, *solib_end;

/* Remember the maximum DLL length for printing in info dll command. */
int max_dll_name_len;

static void
register_loaded_dll (const char *name, DWORD load_addr)
{
  struct so_stuff *so;
  so = (struct so_stuff *) xmalloc (sizeof (struct so_stuff) + strlen (name) + 8 + 2);
  so->load_addr = load_addr;
  strcpy (so->name, name);

  solib_end->next = so;
  solib_end = so;
  so->next = NULL;
}

/* Wait for child to do something.  Return pid of child, or -1 in case
   of error; store status through argument pointer OURSTATUS.  */
static int
handle_load_dll (PTR dummy ATTRIBUTE_UNUSED)
{
  LOAD_DLL_DEBUG_INFO *event = &current_event.u.LoadDll;
  DWORD dll_name_ptr;
  DWORD done;
  char dll_buf[MAX_PATH + 1];
  char *dll_name = NULL;
  int len;
  char *p;

  dll_buf[0] = dll_buf[sizeof (dll_buf) - 1] = '\0';

  if (!psapi_get_dll_name ((DWORD) (event->lpBaseOfDll), dll_buf))
    dll_buf[0] = dll_buf[sizeof (dll_buf) - 1] = '\0';

  dll_name = dll_buf;

  /* Attempt to read the name of the dll that was detected.
     This is documented to work only when actively debugging
     a program.  It will not work for attached processes. */
  if (dll_name == NULL || *dll_name == '\0')
    {
      DWORD size = event->fUnicode ? sizeof (WCHAR) : sizeof (char);
      int len = 0;
      char b[2];

      ReadProcessMemory (current_process_handle,
			 (LPCVOID) event->lpImageName,
			 (char *) &dll_name_ptr,
			 sizeof (dll_name_ptr), &done);

      /* See if we could read the address of a string, and that the
         address isn't null. */

      if (done != sizeof (dll_name_ptr) || !dll_name_ptr)
	return 1;

      do
	{
	  ReadProcessMemory (current_process_handle,
			     (LPCVOID) (dll_name_ptr + len * size),
			     &b,
			     size,
			     &done);
	  len++;
	}
      while ((b[0] != 0 || b[size - 1] != 0) && done == size);

      dll_name = alloca (len);

      if (event->fUnicode)
	{
	  WCHAR *unicode_dll_name = (WCHAR *) alloca (len * sizeof (WCHAR));
	  ReadProcessMemory (current_process_handle,
			     (LPCVOID) dll_name_ptr,
			     unicode_dll_name,
			     len * sizeof (WCHAR),
			     &done);

	  WideCharToMultiByte (CP_ACP, 0,
			       unicode_dll_name, len,
			       dll_name, len, 0, 0);
	}
      else
	{
	  ReadProcessMemory (current_process_handle,
			     (LPCVOID) dll_name_ptr,
			     dll_name,
			     len,
			     &done);
	}
    }

  if (!dll_name)
    return 1;

  (void) strlwr (dll_name);

  while ((p = strchr (dll_name, '\\')))
    *p = '/';

  register_loaded_dll (dll_name, (DWORD) event->lpBaseOfDll + 0x1000);
  len = strlen (dll_name);
  if (len > max_dll_name_len)
    max_dll_name_len = len;

  return 1;
}

/* Return name of last loaded DLL. */
char *
child_solib_loaded_library_pathname (int pid ATTRIBUTE_UNUSED)
{
  return !solib_end || !solib_end->name[0] ? NULL : solib_end->name;
}

/* Clear list of loaded DLLs. */
void
child_clear_solibs (void)
{
  struct so_stuff *so, *so1 = solib_start.next;

  while ((so = so1) != NULL)
    {
      so1 = so->next;
      free (so);
    }

  solib_start.next = NULL;
  solib_end = &solib_start;
  max_dll_name_len = sizeof ("DLL Name") - 1;
}

/* Add DLL symbol information. */
void
solib_symbols_add (char *name, CORE_ADDR load_addr)
{
  struct section_addr_info section_addrs;

  /* The symbols in a dll are offset by 0x1000, which is the
     the offset from 0 of the first byte in an image - because
     of the file header and the section alignment. */

  if (!name || !name[0])
    return;

  memset (&section_addrs, 0, sizeof (section_addrs));
  section_addrs.other[0].name = ".text";
  section_addrs.other[0].addr = load_addr;
  safe_symbol_file_add (name, 0, &section_addrs, 0, OBJF_SHARED);

  return;
}

/* Load DLL symbol info. */
void
dll_symbol_command (char *args, int from_tty ATTRIBUTE_UNUSED)
{
  int n;
  dont_repeat ();

  if (args == NULL)
    error ("dll-symbols requires a file name");

  n = strlen (args);
  if (n > 4 && strcasecmp (args + n - 4, ".dll") != 0)
    {
      char *newargs = (char *) alloca (n + 4 + 1);
      strcpy (newargs, args);
      strcat (newargs, ".dll");
      args = newargs;
    }

  safe_symbol_file_add (args, 0, NULL, 0, OBJF_SHARED | OBJF_USERLOADED);
}

/* List currently loaded DLLs. */
void
info_dll_command (char *ignore ATTRIBUTE_UNUSED, int from_tty ATTRIBUTE_UNUSED)
{
  struct so_stuff *so = &solib_start;

  if (!so->next)
    return;

  printf ("%*s  Load Address\n", -max_dll_name_len, "DLL Name");
  while ((so = so->next) != NULL)
    printf_filtered ("%*s  %08lx\n", -max_dll_name_len, so->name, so->load_addr);

  return;
}

/* Handle DEBUG_STRING output from child process.
   Cygwin prepends its messages with a "cygwin:".  Interpret this as
   a Cygwin signal.  Otherwise just print the string as a warning. */
static int
handle_output_debug_string (struct target_waitstatus *ourstatus)
{
  char *s;
  int gotasig = FALSE;

  if (!target_read_string
    ((CORE_ADDR) current_event.u.DebugString.lpDebugStringData, &s, 1024, 0)
      || !s || !*s)
    return gotasig;

  if (strncmp (s, CYGWIN_SIGNAL_STRING, sizeof (CYGWIN_SIGNAL_STRING) - 1) != 0)
    {
      if (strncmp (s, "cYg", 3) != 0)
	warning ("%s", s);
    }
  else
    {
      char *p;
      int sig = strtol (s + sizeof (CYGWIN_SIGNAL_STRING) - 1, &p, 0);
      gotasig = target_signal_from_host (sig);
      ourstatus->value.sig = gotasig;
      if (gotasig)
	ourstatus->kind = TARGET_WAITKIND_STOPPED;
    }

  free (s);
  return gotasig;
}

static int
handle_exception (struct target_waitstatus *ourstatus)
{
  thread_info *th;
  DWORD code = current_event.u.Exception.ExceptionRecord.ExceptionCode;

  ourstatus->kind = TARGET_WAITKIND_STOPPED;

  /* Record the context of the current thread */
  th = thread_rec (current_event.dwThreadId, -1);

  switch (code)
    {
    case EXCEPTION_ACCESS_VIOLATION:
      DEBUG_EXCEPT (("gdb: Target exception ACCESS_VIOLATION at 0x%08lx\n",
       (DWORD) current_event.u.Exception.ExceptionRecord.ExceptionAddress));
      ourstatus->value.sig = TARGET_SIGNAL_SEGV;
      last_sig = SIGSEGV;
      break;
    case STATUS_FLOAT_UNDERFLOW:
    case STATUS_FLOAT_DIVIDE_BY_ZERO:
    case STATUS_FLOAT_OVERFLOW:
    case STATUS_INTEGER_DIVIDE_BY_ZERO:
      DEBUG_EXCEPT (("gdb: Target exception STACK_OVERFLOW at 0x%08lx\n",
       (DWORD) current_event.u.Exception.ExceptionRecord.ExceptionAddress));
      ourstatus->value.sig = TARGET_SIGNAL_FPE;
      last_sig = SIGFPE;
      break;
    case STATUS_STACK_OVERFLOW:
      DEBUG_EXCEPT (("gdb: Target exception STACK_OVERFLOW at 0x%08lx\n",
       (DWORD) current_event.u.Exception.ExceptionRecord.ExceptionAddress));
      ourstatus->value.sig = TARGET_SIGNAL_SEGV;
      break;
    case EXCEPTION_BREAKPOINT:
      DEBUG_EXCEPT (("gdb: Target exception BREAKPOINT at 0x%08lx\n",
       (DWORD) current_event.u.Exception.ExceptionRecord.ExceptionAddress));
      ourstatus->value.sig = TARGET_SIGNAL_TRAP;
      break;
    case DBG_CONTROL_C:
      DEBUG_EXCEPT (("gdb: Target exception CONTROL_C at 0x%08lx\n",
       (DWORD) current_event.u.Exception.ExceptionRecord.ExceptionAddress));
      ourstatus->value.sig = TARGET_SIGNAL_INT;
      last_sig = SIGINT;	/* FIXME - should check pass state */
      break;
    case EXCEPTION_SINGLE_STEP:
      DEBUG_EXCEPT (("gdb: Target exception SINGLE_STEP at 0x%08lx\n",
       (DWORD) current_event.u.Exception.ExceptionRecord.ExceptionAddress));
      ourstatus->value.sig = TARGET_SIGNAL_TRAP;
      break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
      DEBUG_EXCEPT (("gdb: Target exception SINGLE_ILL at 0x%08lx\n",
       (DWORD) current_event.u.Exception.ExceptionRecord.ExceptionAddress));
      ourstatus->value.sig = TARGET_SIGNAL_ILL;
      last_sig = SIGILL;
      break;
    default:
      printf_unfiltered ("gdb: unknown target exception 0x%08lx at 0x%08lx\n",
		    current_event.u.Exception.ExceptionRecord.ExceptionCode,
	(DWORD) current_event.u.Exception.ExceptionRecord.ExceptionAddress);
      ourstatus->value.sig = TARGET_SIGNAL_UNKNOWN;
      break;
    }
  exception_count++;
  return 1;
}

/* Resume all artificially suspended threads if we are continuing
   execution */
static BOOL
child_continue (DWORD continue_status, int id)
{
  int i;
  thread_info *th;
  BOOL res;

  DEBUG_EVENTS (("ContinueDebugEvent (cpid=%ld, ctid=%ld, DBG_CONTINUE);\n",
		 current_event.dwProcessId, current_event.dwThreadId));
  res = ContinueDebugEvent (current_event.dwProcessId,
			    current_event.dwThreadId,
			    continue_status);
  continue_status = 0;
  if (res)
    for (th = &thread_head; (th = th->next) != NULL;)
      if (((id == -1) || (id == (int) th->id)) && th->suspend_count)
	{
	  for (i = 0; i < th->suspend_count; i++)
	    (void) ResumeThread (th->h);
	  th->suspend_count = 0;
	}

  return res;
}

/* Get the next event from the child.  Return 1 if the event requires
   handling by WFI (or whatever).
 */
static int
get_child_debug_event (int pid ATTRIBUTE_UNUSED, struct target_waitstatus *ourstatus)
{
  BOOL debug_event;
  DWORD continue_status, event_code;
  thread_info *th = NULL;
  static thread_info dummy_thread_info;
  int retval = 0;

  last_sig = 0;

  if (!(debug_event = WaitForDebugEvent (&current_event, 1000)))
    goto out;

  event_count++;
  continue_status = DBG_CONTINUE;

  event_code = current_event.dwDebugEventCode;
  ourstatus->kind = TARGET_WAITKIND_SPURIOUS;

  switch (event_code)
    {
    case CREATE_THREAD_DEBUG_EVENT:
      DEBUG_EVENTS (("gdb: kernel event for pid=%d tid=%x code=%s)\n",
		     (unsigned) current_event.dwProcessId,
		     (unsigned) current_event.dwThreadId,
		     "CREATE_THREAD_DEBUG_EVENT"));
      /* Record the existence of this thread */
      th = child_add_thread (current_event.dwThreadId,
			     current_event.u.CreateThread.hThread);
      if (info_verbose)
	printf_unfiltered ("[New %s]\n",
			   target_pid_to_str (current_event.dwThreadId));
      retval = current_event.dwThreadId;
      break;

    case EXIT_THREAD_DEBUG_EVENT:
      DEBUG_EVENTS (("gdb: kernel event for pid=%d tid=%d code=%s)\n",
		     (unsigned) current_event.dwProcessId,
		     (unsigned) current_event.dwThreadId,
		     "EXIT_THREAD_DEBUG_EVENT"));
      child_delete_thread (current_event.dwThreadId);
      th = &dummy_thread_info;
      break;

    case CREATE_PROCESS_DEBUG_EVENT:
      DEBUG_EVENTS (("gdb: kernel event for pid=%d tid=%d code=%s)\n",
		     (unsigned) current_event.dwProcessId,
		     (unsigned) current_event.dwThreadId,
		     "CREATE_PROCESS_DEBUG_EVENT"));
      current_process_handle = current_event.u.CreateProcessInfo.hProcess;

      main_thread_id = current_event.dwThreadId;
      /* Add the main thread */
#if 0
      th = child_add_thread (current_event.dwProcessId,
			     current_event.u.CreateProcessInfo.hProcess);
#endif
      th = child_add_thread (main_thread_id,
			     current_event.u.CreateProcessInfo.hThread);
      retval = ourstatus->value.related_pid = current_event.dwThreadId;
      break;

    case EXIT_PROCESS_DEBUG_EVENT:
      DEBUG_EVENTS (("gdb: kernel event for pid=%d tid=%d code=%s)\n",
		     (unsigned) current_event.dwProcessId,
		     (unsigned) current_event.dwThreadId,
		     "EXIT_PROCESS_DEBUG_EVENT"));
      ourstatus->kind = TARGET_WAITKIND_EXITED;
      ourstatus->value.integer = current_event.u.ExitProcess.dwExitCode;
      CloseHandle (current_process_handle);
      retval = main_thread_id;
      break;

    case LOAD_DLL_DEBUG_EVENT:
      DEBUG_EVENTS (("gdb: kernel event for pid=%d tid=%d code=%s)\n",
		     (unsigned) current_event.dwProcessId,
		     (unsigned) current_event.dwThreadId,
		     "LOAD_DLL_DEBUG_EVENT"));
      catch_errors (handle_load_dll, NULL, (char *) "", RETURN_MASK_ALL);
      registers_changed ();	/* mark all regs invalid */
      ourstatus->kind = TARGET_WAITKIND_LOADED;
      ourstatus->value.integer = 0;
      retval = main_thread_id;
      break;

    case UNLOAD_DLL_DEBUG_EVENT:
      DEBUG_EVENTS (("gdb: kernel event for pid=%d tid=%d code=%s)\n",
		     (unsigned) current_event.dwProcessId,
		     (unsigned) current_event.dwThreadId,
		     "UNLOAD_DLL_DEBUG_EVENT"));
      break;			/* FIXME: don't know what to do here */

    case EXCEPTION_DEBUG_EVENT:
      DEBUG_EVENTS (("gdb: kernel event for pid=%d tid=%d code=%s)\n",
		     (unsigned) current_event.dwProcessId,
		     (unsigned) current_event.dwThreadId,
		     "EXCEPTION_DEBUG_EVENT"));
      handle_exception (ourstatus);
      retval = current_event.dwThreadId;
      break;

    case OUTPUT_DEBUG_STRING_EVENT:	/* message from the kernel */
      DEBUG_EVENTS (("gdb: kernel event for pid=%d tid=%d code=%s)\n",
		     (unsigned) current_event.dwProcessId,
		     (unsigned) current_event.dwThreadId,
		     "OUTPUT_DEBUG_STRING_EVENT"));
      if (handle_output_debug_string (ourstatus))
	retval = main_thread_id;
      break;

    default:
      printf_unfiltered ("gdb: kernel event for pid=%ld tid=%ld\n",
			 (DWORD) current_event.dwProcessId,
			 (DWORD) current_event.dwThreadId);
      printf_unfiltered ("                 unknown event code %ld\n",
			 current_event.dwDebugEventCode);
      break;
    }

  if (!retval)
    CHECK (child_continue (continue_status, -1));
  else
    {
      current_thread = th ? : thread_rec (current_event.dwThreadId, TRUE);
      inferior_pid = retval;
    }

out:
  return retval;
}

/* Wait for interesting events to occur in the target process. */
static int
child_wait (int pid, struct target_waitstatus *ourstatus)
{
  /* We loop when we get a non-standard exception rather than return
     with a SPURIOUS because resume can try and step or modify things,
     which needs a current_thread->h.  But some of these exceptions mark
     the birth or death of threads, which mean that the current thread
     isn't necessarily what you think it is. */

  while (1)
    {
      int retval = get_child_debug_event (pid, ourstatus);
      if (retval)
	return retval;
      else
	{
	  int detach = 0;

	  if (ui_loop_hook != NULL)
	    detach = ui_loop_hook (0);

	  if (detach)
	    child_kill_inferior ();
	}
    }
}

static void
do_initial_child_stuff (DWORD pid)
{
  extern int stop_after_trap;

  last_sig = 0;
  event_count = 0;
  exception_count = 0;
  current_event.dwProcessId = pid;
  memset (&current_event, 0, sizeof (current_event));
  push_target (&child_ops);
  child_init_thread_list ();
  child_clear_solibs ();
  clear_proceed_status ();
  init_wait_for_inferior ();

  target_terminal_init ();
  target_terminal_inferior ();

  while (1)
    {
      stop_after_trap = 1;
      wait_for_inferior ();
      if (stop_signal != TARGET_SIGNAL_TRAP)
	resume (0, stop_signal);
      else
	break;
    }
  stop_after_trap = 0;
  return;
}

/* Attach to process PID, then initialize for debugging it.  */

static void
child_attach (char *args, int from_tty)
{
  BOOL ok;
  DWORD pid = strtoul (args, 0, 0);

  if (!args)
    error_no_arg ("process-id to attach");

  ok = DebugActiveProcess (pid);

  if (!ok)
    error ("Can't attach to process.");

  if (from_tty)
    {
      char *exec_file = (char *) get_exec_file (0);

      if (exec_file)
	printf_unfiltered ("Attaching to program `%s', %s\n", exec_file,
			   target_pid_to_str (pid));
      else
	printf_unfiltered ("Attaching to %s\n",
			   target_pid_to_str (pid));

      gdb_flush (gdb_stdout);
    }

  do_initial_child_stuff (pid);
  target_terminal_ours ();
}

static void
child_detach (char *args ATTRIBUTE_UNUSED, int from_tty)
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
child_files_info (struct target_ops *ignore ATTRIBUTE_UNUSED)
{
  printf_unfiltered ("\tUsing the running image of %s %s.\n",
      attach_flag ? "attached" : "child", target_pid_to_str (inferior_pid));
}

/* ARGSUSED */
static void
child_open (char *arg ATTRIBUTE_UNUSED, int from_tty ATTRIBUTE_UNUSED)
{
  error ("Use the \"run\" command to start a Unix child process.");
}

/* Start an inferior win32 child process and sets inferior_pid to its pid.
   EXEC_FILE is the file to run.
   ALLARGS is a string containing the arguments to the program.
   ENV is the environment vector to pass.  Errors reported with error().  */

static void
child_create_inferior (char *exec_file, char *allargs, char **env)
{
  char real_path[MAXPATHLEN];
  char *winenv;
  char *temp;
  int envlen;
  int i;
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  BOOL ret;
  DWORD flags;
  char *args;

  if (!exec_file)
    error ("No executable specified, use `target exec'.\n");

  memset (&si, 0, sizeof (si));
  si.cb = sizeof (si);

  cygwin_conv_to_win32_path (exec_file, real_path);

  flags = DEBUG_ONLY_THIS_PROCESS;

  if (new_group)
    flags |= CREATE_NEW_PROCESS_GROUP;

  if (new_console)
    flags |= CREATE_NEW_CONSOLE;

  args = alloca (strlen (real_path) + strlen (allargs) + 2);

  strcpy (args, real_path);

  strcat (args, " ");
  strcat (args, allargs);

  /* Prepare the environment vars for CreateProcess.  */
  {
    /* This code use to assume all env vars were file names and would
       translate them all to win32 style.  That obviously doesn't work in the
       general case.  The current rule is that we only translate PATH.
       We need to handle PATH because we're about to call CreateProcess and
       it uses PATH to find DLL's.  Fortunately PATH has a well-defined value
       in both posix and win32 environments.  cygwin.dll will change it back
       to posix style if necessary.  */

    static const char *conv_path_names[] =
    {
      "PATH=",
      0
    };

    /* CreateProcess takes the environment list as a null terminated set of
       strings (i.e. two nulls terminate the list).  */

    /* Get total size for env strings.  */
    for (envlen = 0, i = 0; env[i] && *env[i]; i++)
      {
	int j, len;

	for (j = 0; conv_path_names[j]; j++)
	  {
	    len = strlen (conv_path_names[j]);
	    if (strncmp (conv_path_names[j], env[i], len) == 0)
	      {
		if (cygwin_posix_path_list_p (env[i] + len))
		  envlen += len
		    + cygwin_posix_to_win32_path_list_buf_size (env[i] + len);
		else
		  envlen += strlen (env[i]) + 1;
		break;
	      }
	  }
	if (conv_path_names[j] == NULL)
	  envlen += strlen (env[i]) + 1;
      }

    winenv = alloca (envlen + 1);

    /* Copy env strings into new buffer.  */
    for (temp = winenv, i = 0; env[i] && *env[i]; i++)
      {
	int j, len;

	for (j = 0; conv_path_names[j]; j++)
	  {
	    len = strlen (conv_path_names[j]);
	    if (strncmp (conv_path_names[j], env[i], len) == 0)
	      {
		if (cygwin_posix_path_list_p (env[i] + len))
		  {
		    memcpy (temp, env[i], len);
		    cygwin_posix_to_win32_path_list (env[i] + len, temp + len);
		  }
		else
		  strcpy (temp, env[i]);
		break;
	      }
	  }
	if (conv_path_names[j] == NULL)
	  strcpy (temp, env[i]);

	temp += strlen (temp) + 1;
      }

    /* Final nil string to terminate new env.  */
    *temp = 0;
  }

  ret = CreateProcess (0,
		       args,	/* command line */
		       NULL,	/* Security */
		       NULL,	/* thread */
		       TRUE,	/* inherit handles */
		       flags,	/* start flags */
		       winenv,
		       NULL,	/* current directory */
		       &si,
		       &pi);
  if (!ret)
    error ("Error creating process %s, (error %d)\n", exec_file, GetLastError ());

  do_initial_child_stuff (pi.dwProcessId);

  /* child_continue (DBG_CONTINUE, -1); */
  proceed ((CORE_ADDR) - 1, TARGET_SIGNAL_0, 0);
}

static void
child_mourn_inferior (void)
{
  (void) child_continue (DBG_CONTINUE, -1);
  unpush_target (&child_ops);
  generic_mourn_inferior ();
}

/* Send a SIGINT to the process group.  This acts just like the user typed a
   ^C on the controlling terminal. */

static void
child_stop (void)
{
  DEBUG_EVENTS (("gdb: GenerateConsoleCtrlEvent (CTRLC_EVENT, 0)\n"));
  CHECK (GenerateConsoleCtrlEvent (CTRL_C_EVENT, current_event.dwProcessId));
  registers_changed ();		/* refresh register state */
}

int
child_xfer_memory (CORE_ADDR memaddr, char *our, int len,
		   int write, struct target_ops *target ATTRIBUTE_UNUSED)
{
  DWORD done;
  if (write)
    {
      DEBUG_MEM (("gdb: write target memory, %d bytes at 0x%08lx\n",
		  len, (DWORD) memaddr));
      WriteProcessMemory (current_process_handle, (LPVOID) memaddr, our,
			  len, &done);
      FlushInstructionCache (current_process_handle, (LPCVOID) memaddr, len);
    }
  else
    {
      DEBUG_MEM (("gdb: read target memory, %d bytes at 0x%08lx\n",
		  len, (DWORD) memaddr));
      ReadProcessMemory (current_process_handle, (LPCVOID) memaddr, our, len,
			 &done);
    }
  return done;
}

void
child_kill_inferior (void)
{
  CHECK (TerminateProcess (current_process_handle, 0));

  for (;;)
    {
      if (!child_continue (DBG_CONTINUE, -1))
	break;
      if (!WaitForDebugEvent (&current_event, INFINITE))
	break;
      if (current_event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
	break;
    }

  CHECK (CloseHandle (current_process_handle));

  /* this may fail in an attached process so don't check. */
  (void) CloseHandle (current_thread->h);
  target_mourn_inferior ();	/* or just child_mourn_inferior? */
}

void
child_resume (int pid, int step, enum target_signal sig)
{
  thread_info *th;
  DWORD continue_status = last_sig > 0 && last_sig < NSIG ?
  DBG_EXCEPTION_NOT_HANDLED : DBG_CONTINUE;

  last_sig = 0;

  DEBUG_EXEC (("gdb: child_resume (pid=%d, step=%d, sig=%d);\n",
	       pid, step, sig));

  /* Get context for currently selected thread */
  th = thread_rec (current_event.dwThreadId, FALSE);
  if (th)
    {
      if (step)
	{
	  /* Single step by setting t bit */
	  child_fetch_inferior_registers (PS_REGNUM);
	  th->context.EFlags |= FLAG_TRACE_BIT;
	}

      if (th->context.ContextFlags)
	{
	  CHECK (SetThreadContext (th->h, &th->context));
	  th->context.ContextFlags = 0;
	}
    }

  /* Allow continuing with the same signal that interrupted us.
     Otherwise complain. */

  child_continue (continue_status, pid);
}

static void
child_prepare_to_store (void)
{
  /* Do nothing, since we can store individual regs */
}

static int
child_can_run (void)
{
  return 1;
}

static void
child_close (int x ATTRIBUTE_UNUSED)
{
  DEBUG_EVENTS (("gdb: child_close, inferior_pid=%d\n", inferior_pid));
}

struct target_ops child_ops;

static void
init_child_ops (void)
{
  child_ops.to_shortname = "child";
  child_ops.to_longname = "Win32 child process";
  child_ops.to_doc = "Win32 child process (started by the \"run\" command).";
  child_ops.to_open = child_open;
  child_ops.to_close = child_close;
  child_ops.to_attach = child_attach;
  child_ops.to_detach = child_detach;
  child_ops.to_resume = child_resume;
  child_ops.to_wait = child_wait;
  child_ops.to_fetch_registers = child_fetch_inferior_registers;
  child_ops.to_store_registers = child_store_inferior_registers;
  child_ops.to_prepare_to_store = child_prepare_to_store;
  child_ops.to_xfer_memory = child_xfer_memory;
  child_ops.to_files_info = child_files_info;
  child_ops.to_insert_breakpoint = memory_insert_breakpoint;
  child_ops.to_remove_breakpoint = memory_remove_breakpoint;
  child_ops.to_terminal_init = terminal_init_inferior;
  child_ops.to_terminal_inferior = terminal_inferior;
  child_ops.to_terminal_ours_for_output = terminal_ours_for_output;
  child_ops.to_terminal_ours = terminal_ours;
  child_ops.to_terminal_info = child_terminal_info;
  child_ops.to_kill = child_kill_inferior;
  child_ops.to_load = 0;
  child_ops.to_lookup_symbol = 0;
  child_ops.to_create_inferior = child_create_inferior;
  child_ops.to_mourn_inferior = child_mourn_inferior;
  child_ops.to_can_run = child_can_run;
  child_ops.to_notice_signals = 0;
  child_ops.to_thread_alive = win32_child_thread_alive;
  child_ops.to_pid_to_str = cygwin_pid_to_str;
  child_ops.to_stop = child_stop;
  child_ops.to_stratum = process_stratum;
  child_ops.DONT_USE = 0;
  child_ops.to_has_all_memory = 1;
  child_ops.to_has_memory = 1;
  child_ops.to_has_stack = 1;
  child_ops.to_has_registers = 1;
  child_ops.to_has_execution = 1;
  child_ops.to_sections = 0;
  child_ops.to_sections_end = 0;
  child_ops.to_magic = OPS_MAGIC;
}

void
_initialize_inftarg (void)
{
  init_child_ops ();

  add_com ("dll-symbols", class_files, dll_symbol_command,
	   "Load dll library symbols from FILE.");

  auto_solib_add = 1;
  add_com_alias ("sharedlibrary", "dll-symbols", class_alias, 1);

  add_show_from_set (add_set_cmd ("new-console", class_support, var_boolean,
				  (char *) &new_console,
		 "Set creation of new console when creating child process.",
				  &setlist),
		     &showlist);

  add_show_from_set (add_set_cmd ("new-group", class_support, var_boolean,
				  (char *) &new_group,
		   "Set creation of new group when creating child process.",
				  &setlist),
		     &showlist);

  add_show_from_set (add_set_cmd ("debugexec", class_support, var_boolean,
				  (char *) &debug_exec,
		       "Set whether to display execution in child process.",
				  &setlist),
		     &showlist);

  add_show_from_set (add_set_cmd ("debugevents", class_support, var_boolean,
				  (char *) &debug_events,
		   "Set whether to display kernel events in child process.",
				  &setlist),
		     &showlist);

  add_show_from_set (add_set_cmd ("debugmemory", class_support, var_boolean,
				  (char *) &debug_memory,
		 "Set whether to display memory accesses in child process.",
				  &setlist),
		     &showlist);

  add_show_from_set (add_set_cmd ("debugexceptions", class_support, var_boolean,
				  (char *) &debug_exceptions,
	       "Set whether to display kernel exceptions in child process.",
				  &setlist),
		     &showlist);

  add_info ("dll", info_dll_command, "Status of loaded DLLs.");
  add_info_alias ("sharedlibrary", "dll", 1);

  add_target (&child_ops);
}

/* Determine if the thread referenced by "pid" is alive
   by "polling" it.  If WaitForSingleObject returns WAIT_OBJECT_0
   it means that the pid has died.  Otherwise it is assumed to be alive. */
static int
win32_child_thread_alive (int pid)
{
  return WaitForSingleObject (thread_rec (pid, FALSE)->h, 0) == WAIT_OBJECT_0 ?
    FALSE : TRUE;
}

/* Convert pid to printable format. */
char *
cygwin_pid_to_str (int pid)
{
  static char buf[80];
  if ((DWORD) pid == current_event.dwProcessId)
    sprintf (buf, "process %d", pid);
  else
    sprintf (buf, "thread %ld.0x%x", current_event.dwProcessId, pid);
  return buf;
}

static int
core_dll_symbols_add (char *dll_name, DWORD base_addr)
{
  struct objfile *objfile;
  char *objfile_basename;
  const char *dll_basename;

  if (!(dll_basename = strrchr (dll_name, '/')))
    dll_basename = dll_name;
  else
    dll_basename++;

  ALL_OBJFILES (objfile)
  {
    objfile_basename = strrchr (objfile->name, '/');

    if (objfile_basename &&
	strcmp (dll_basename, objfile_basename + 1) == 0)
      {
	printf_unfiltered ("%08lx:%s (symbols previously loaded)\n",
			   base_addr, dll_name);
	goto out;
      }
  }

  register_loaded_dll (dll_name, base_addr + 0x1000);
  solib_symbols_add (dll_name, (CORE_ADDR) base_addr + 0x1000);

out:
  return 1;
}

typedef struct
{
  struct target_ops *target;
  bfd_vma addr;
}
map_code_section_args;

static void
map_single_dll_code_section (bfd * abfd, asection * sect, PTR obj)
{
  int old;
  int update_coreops;
  struct section_table *new_target_sect_ptr;

  map_code_section_args *args = (map_code_section_args *) obj;
  struct target_ops *target = args->target;
  if (sect->flags & SEC_CODE)
    {
      update_coreops = core_ops.to_sections == target->to_sections;

      if (target->to_sections)
	{
	  old = target->to_sections_end - target->to_sections;
	  target->to_sections = (struct section_table *)
	    xrealloc ((char *) target->to_sections,
		      (sizeof (struct section_table)) * (1 + old));
	}
      else
	{
	  old = 0;
	  target->to_sections = (struct section_table *)
	    xmalloc ((sizeof (struct section_table)));
	}
      target->to_sections_end = target->to_sections + (1 + old);

      /* Update the to_sections field in the core_ops structure
         if needed.  */
      if (update_coreops)
	{
	  core_ops.to_sections = target->to_sections;
	  core_ops.to_sections_end = target->to_sections_end;
	}
      new_target_sect_ptr = target->to_sections + old;
      new_target_sect_ptr->addr = args->addr + bfd_section_vma (abfd, sect);
      new_target_sect_ptr->endaddr = args->addr + bfd_section_vma (abfd, sect) +
	bfd_section_size (abfd, sect);;
      new_target_sect_ptr->the_bfd_section = sect;
      new_target_sect_ptr->bfd = abfd;
    }
}

static int
dll_code_sections_add (const char *dll_name, int base_addr, struct target_ops *target)
{
  bfd *dll_bfd;
  map_code_section_args map_args;
  asection *lowest_sect;
  char *name;
  if (dll_name == NULL || target == NULL)
    return 0;
  name = strdup (dll_name);
  dll_bfd = bfd_openr (name, "pei-i386");
  if (dll_bfd == NULL)
    return 0;

  if (bfd_check_format (dll_bfd, bfd_object))
    {
      lowest_sect = bfd_get_section_by_name (dll_bfd, ".text");
      if (lowest_sect == NULL)
	return 0;
      map_args.target = target;
      map_args.addr = base_addr - bfd_section_vma (dll_bfd, lowest_sect);

      bfd_map_over_sections (dll_bfd, &map_single_dll_code_section, (PTR) (&map_args));
    }

  return 1;
}

static void
core_section_load_dll_symbols (bfd * abfd, asection * sect, PTR obj)
{
  struct target_ops *target = (struct target_ops *) obj;

  DWORD base_addr;

  int dll_name_size;
  char *dll_name = NULL;
  char *buf = NULL;
  struct win32_pstatus *pstatus;
  char *p;

  if (strncmp (sect->name, ".module", 7))
    return;

  buf = (char *) xmalloc (sect->_raw_size + 1);
  if (!buf)
    {
      printf_unfiltered ("memory allocation failed for %s\n", sect->name);
      goto out;
    }
  if (!bfd_get_section_contents (abfd, sect, buf, 0, sect->_raw_size))
    goto out;

  pstatus = (struct win32_pstatus *) buf;

  memmove (&base_addr, &(pstatus->data.module_info.base_address), sizeof (base_addr));
  dll_name_size = pstatus->data.module_info.module_name_size;
  if (offsetof (struct win32_pstatus, data.module_info.module_name) + dll_name_size > sect->_raw_size)
      goto out;

  dll_name = (char *) xmalloc (dll_name_size + 1);
  if (!dll_name)
    {
      printf_unfiltered ("memory allocation failed for %s\n", sect->name);
      goto out;
    }
  strncpy (dll_name, pstatus->data.module_info.module_name, dll_name_size);

  while ((p = strchr (dll_name, '\\')))
    *p = '/';

  if (!core_dll_symbols_add (dll_name, (DWORD) base_addr))
    printf_unfiltered ("%s: Failed to load dll symbols.\n", dll_name);

  if (!dll_code_sections_add (dll_name, (DWORD) base_addr + 0x1000, target))
    printf_unfiltered ("%s: Failed to map dll code sections.\n", dll_name);

out:
  if (buf)
    free (buf);
  if (dll_name)
    free (dll_name);
  return;
}

void
child_solib_add (char *filename ATTRIBUTE_UNUSED, int from_tty ATTRIBUTE_UNUSED, struct target_ops *target)
{
  if (core_bfd)
    {
      child_clear_solibs ();
      bfd_map_over_sections (core_bfd, &core_section_load_dll_symbols, target);
    }
  else
    {
      if (solib_end && solib_end->name)
	solib_symbols_add (solib_end->name, solib_end->load_addr);
    }
}

static void
fetch_elf_core_registers (char *core_reg_sect,
			  unsigned core_reg_size,
			  int which,
			  CORE_ADDR reg_addr)
{
  int r;
  if (core_reg_size < sizeof (CONTEXT))
    {
      error ("Core file register section too small (%u bytes).", core_reg_size);
      return;
    }
  for (r = 0; r < NUM_REGS; r++)
    supply_register (r, core_reg_sect + mappings[r]);
}

static struct core_fns win32_elf_core_fns =
{
  bfd_target_elf_flavour,
  default_check_format,
  default_core_sniffer,
  fetch_elf_core_registers,
  NULL
};

void
_initialize_core_win32 ()
{
  add_core_fns (&win32_elf_core_fns);
}
