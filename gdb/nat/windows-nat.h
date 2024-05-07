/* Internal interfaces for the Windows code
   Copyright (C) 1995-2024 Free Software Foundation, Inc.

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

#ifndef NAT_WINDOWS_NAT_H
#define NAT_WINDOWS_NAT_H

#include <windows.h>
#include <psapi.h>
#include <vector>

#include <optional>
#include "target/waitstatus.h"

#define STATUS_WX86_BREAKPOINT 0x4000001F
#define STATUS_WX86_SINGLE_STEP 0x4000001E

namespace windows_nat
{

struct windows_process_info;

/* The reason for explicitly stopping a thread.  Note the enumerators
   are ordered such that when comparing two stopping_kind's numerical
   value, the highest should prevail.  */
enum stopping_kind
  {
    /* Not really stopping the thread.  */
    SK_NOT_STOPPING = 0,

    /* We're stopping the thread for internal reasons, the stop should
       not be reported as an event to the core.  */
    SK_INTERNAL = 1,

    /* We're stopping the thread for external reasons, meaning, the
       core/user asked us to stop the thread, so we must report a stop
       event to the core.  */
    SK_EXTERNAL = 2,
  };

/* Thread information structure used to track extra information about
   each thread.  */
struct windows_thread_info
{
  windows_thread_info (windows_process_info *proc_,
		       DWORD tid_, HANDLE h_, CORE_ADDR tlb)
    : proc (proc_),
      tid (tid_),
      h (h_),
      thread_local_base (tlb)
  {
  }

  DISABLE_COPY_AND_ASSIGN (windows_thread_info);

  /* Ensure that this thread has been suspended.  */
  void suspend ();

  /* Resume the thread if it has been suspended.  */
  void resume ();

  /* Return the thread's name, or nullptr if not known.  The name is
     stored in this thread and is guaranteed to live until at least
     the next call.  */
  const char *thread_name ();

  /* Read LEN bytes of the thread's $_siginfo into READBUF, starting
     at OFFSET.  Store the number of actually-read bytes in
     XFERED_LEN.  Returns true on success, false on failure.  Passing
     READBUF as NULL indicates that the caller is trying to write to
     $_siginfo, which is a failure case.  */
  bool xfer_siginfo (gdb_byte *readbuf,
		     ULONGEST offset, ULONGEST len,
		     ULONGEST *xfered_len);

  /* The process this thread belongs to.  */
  windows_process_info *proc;

  /* The Win32 thread identifier.  */
  DWORD tid;

  /* The handle to the thread.  */
  HANDLE h;

  /* Thread Information Block address.  */
  CORE_ADDR thread_local_base;

#ifdef __CYGWIN__
  /* These two fields are used to handle Cygwin signals.  When a
     thread is signaled, the "sig" thread inside the Cygwin runtime
     reports the fact to us via a special OutputDebugString message.
     In order to make stepping into a signal handler work, we can only
     resume the "sig" thread when we also resume the target signaled
     thread.  When we intercept a Cygwin signal, we set up a cross
     link between the two threads using the two fields below, so we
     can always identify one from the other.  See the "Cygwin signals"
     description in gdb/windows-nat.c for more.  */

  /* If this thread received a signal, then 'cygwin_sig_thread' points
     to the "sig" thread within the Cygwin runtime.  */
  windows_thread_info *cygwin_sig_thread = nullptr;

  /* If this thread is the Cygwin runtime's "sig" thread, then
     'signaled_thread' points at the thread that received a
     signal.  */
  windows_thread_info *signaled_thread = nullptr;
#endif

  /* If the thread had its event postponed with DBG_REPLY_LATER, when
     we later ResumeThread this thread, WaitForDebugEvent will
     re-report the postponed event.  This field holds the continue
     status value to be automatically passed to ContinueDebugEvent
     when we encounter this re-reported event.  0 if the thread has
     not had its event postponed with DBG_REPLY_LATER. */
  DWORD reply_later = 0;

  /* This keeps track of whether SuspendThread was called on this
     thread.  -1 means there was a failure or that the thread was
     explicitly not suspended, 1 means it was called, and 0 means it
     was not.  */
  int suspended = 0;

  /* This flag indicates whether we are explicitly stopping this
     thread in response to a target_stop request or for
     backend-internal reasons.  This allows distinguishing between
     threads that are explicitly stopped by the debugger and threads
     that are stopped due to other reasons.

     Typically, when we want to stop a thread, we suspend it, enqueue
     a pending GDB_SIGNAL_0 stop status on the thread, and then set
     this flag to true.  However, if the thread has had its event
     previously postponed with DBG_REPLY_LATER, it means that it
     already has an event to report.  In such case, we simply set the
     'stopping' flag without suspending the thread or enqueueing a
     pending stop.  See stop_one_thread.  */
  stopping_kind stopping = SK_NOT_STOPPING;

/* Info about a potential pending stop.

   Sometimes, Windows will report a stop on a thread that has been
   ostensibly suspended.  We believe what happens here is that two
   threads hit a breakpoint simultaneously, and the Windows kernel
   queues the stop events.  However, this can result in the strange
   effect of trying to single step thread A -- leaving all other
   threads suspended -- and then seeing a stop in thread B.  To handle
   this scenario, we queue all such "pending" stops here, and then
   process them once the step has completed.  See PR gdb/22992.

   TARGET_WAITKIND_IGNORE if the thread does not have a pending
   stop.  */
  target_waitstatus pending_status;

  /* The last Windows event returned by WaitForDebugEvent for this
     thread.  */
  DEBUG_EVENT last_event {};

  /* The last signal reported for this thread, extracted out of
     last_event.  */
  enum gdb_signal last_sig = GDB_SIGNAL_0;

  /* The context of the thread, including any manipulations.  */
  union
  {
    CONTEXT context {};
#ifdef __x86_64__
    WOW64_CONTEXT wow64_context;
#endif
  };

  /* Whether debug registers changed since we last set CONTEXT back to
     the thread.  */
  bool debug_registers_changed = false;

  /* True if this thread is currently stopped at a software
     breakpoint.  This is used to offset the PC when needed.  */
  bool stopped_at_software_breakpoint = false;

  /* True if we've adjusted the PC after hitting a software
     breakpoint, false otherwise.  This lets us avoid multiple
     adjustments if the registers are read multiple times.  */
  bool pc_adjusted = false;

  /* The name of the thread.  */
  gdb::unique_xmalloc_ptr<char> name;
};

enum handle_exception_result
{
  HANDLE_EXCEPTION_UNHANDLED = 0,
  HANDLE_EXCEPTION_HANDLED,
  HANDLE_EXCEPTION_IGNORED
};

/* A single Windows process.  An object of this type (or subclass) is
   created by the client.  Some methods must be provided by the client
   as well.  */

struct windows_process_info
{
  /* The process handle */
  HANDLE handle = 0;
  DWORD process_id = 0;
  DWORD main_thread_id = 0;

#ifdef __x86_64__
  /* The target is a WOW64 process */
  bool wow64_process = false;
  /* Ignore first breakpoint exception of WOW64 process */
  bool ignore_first_breakpoint = false;
#endif

  /* Find a thread record given a thread id.

     This function must be supplied by the embedding application.  */
  virtual windows_thread_info *find_thread (ptid_t ptid) = 0;

  /* Handle OUTPUT_DEBUG_STRING_EVENT from child process.  Updates
     OURSTATUS and returns true if this represents a Cygwin signal,
     otherwise false.

     Cygwin prepends its messages with a "cygwin:".  Interpret this as
     a Cygwin signal.  Otherwise just print the string as a warning.

     This function must be supplied by the embedding application.  */
  virtual bool handle_output_debug_string (const DEBUG_EVENT &current_event,
					   struct target_waitstatus *ourstatus) = 0;

  /* Handle a DLL load event.

     This function assumes that the current event did not occur during
     inferior initialization.

     DLL_NAME is the name of the library.  BASE is the base load
     address.

     This function must be supplied by the embedding application.  */

  virtual void handle_load_dll (const char *dll_name, LPVOID base) = 0;

  /* Handle a DLL unload event.

     This function assumes that this event did not occur during inferior
     initialization.

     This function must be supplied by the embedding application.  */

  virtual void handle_unload_dll (const DEBUG_EVENT &current_event) = 0;

  /* When EXCEPTION_ACCESS_VIOLATION is processed, we give the embedding
     application a chance to change it to be considered "unhandled".
     This function must be supplied by the embedding application.  If it
     returns true, then the exception is "unhandled".  */

  virtual bool handle_access_violation (const EXCEPTION_RECORD *rec) = 0;

  /* Fill in the thread's CONTEXT/WOW64_CONTEXT, if it wasn't filled
     in yet.

     This function must be supplied by the embedding application.  */

  virtual void fill_thread_context (windows_thread_info *th) = 0;

  handle_exception_result handle_exception
      (DEBUG_EVENT &current_event,
       struct target_waitstatus *ourstatus, bool debug_exceptions);

  /* Call to indicate that a DLL was loaded.  */

  void dll_loaded_event (const DEBUG_EVENT &current_event);

  /* Iterate over all DLLs currently mapped by our inferior, and
     add them to our list of solibs.  */

  void add_all_dlls ();

  const char *pid_to_exec_file (int);

private:

  /* Handle MS_VC_EXCEPTION when processing a stop.  MS_VC_EXCEPTION is
     somewhat undocumented but is used to tell the debugger the name of
     a thread.

     Return true if the exception was handled; return false otherwise.  */

  bool handle_ms_vc_exception (const DEBUG_EVENT &current_event);

  /* Iterate over all DLLs currently mapped by our inferior, looking for
     a DLL which is loaded at LOAD_ADDR.  If found, add the DLL to our
     list of solibs; otherwise do nothing.  LOAD_ADDR NULL means add all
     DLLs to the list of solibs; this is used when the inferior finishes
     its initialization, and all the DLLs it statically depends on are
     presumed loaded.  */

  void add_dll (LPVOID load_addr);

  /* Try to determine the executable filename.

     EXE_NAME_RET is a pointer to a buffer whose size is EXE_NAME_MAX_LEN.

     Upon success, the filename is stored inside EXE_NAME_RET, and
     this function returns nonzero.

     Otherwise, this function returns zero and the contents of
     EXE_NAME_RET is undefined.  */

  int get_exec_module_filename (char *exe_name_ret, size_t exe_name_max_len);
};

/* A simple wrapper for ContinueDebugEvent that continues the last
   waited-for event.  If DEBUG_EVENTS is true, logging will be
   enabled.  */

extern BOOL continue_last_debug_event (DWORD continue_status,
				       bool debug_events);

/* Return the ptid_t of the thread that the last waited-for event was
   for.  */

extern ptid_t get_last_debug_event_ptid ();

/* A simple wrapper for WaitForDebugEvent that also sets the internal
   'last_wait_event' on success.  */

extern BOOL wait_for_debug_event (DEBUG_EVENT *event, DWORD timeout);

/* Wrappers for CreateProcess.  These exist primarily so that the
   "disable randomization" feature can be implemented in a single
   place.  */

extern BOOL create_process (const char *image, char *command_line,
			    DWORD flags, void *environment,
			    const char *cur_dir,
			    bool no_randomization,
			    STARTUPINFOA *startup_info,
			    PROCESS_INFORMATION *process_info);
#ifdef __CYGWIN__
extern BOOL create_process (const wchar_t *image, wchar_t *command_line,
			    DWORD flags, void *environment,
			    const wchar_t *cur_dir,
			    bool no_randomization,
			    STARTUPINFOW *startup_info,
			    PROCESS_INFORMATION *process_info);
#endif /* __CYGWIN__ */

#define AdjustTokenPrivileges		dyn_AdjustTokenPrivileges
#define DebugActiveProcessStop		dyn_DebugActiveProcessStop
#define DebugBreakProcess		dyn_DebugBreakProcess
#define DebugSetProcessKillOnExit	dyn_DebugSetProcessKillOnExit
#undef EnumProcessModules
#define EnumProcessModules		dyn_EnumProcessModules
#undef EnumProcessModulesEx
#define EnumProcessModulesEx		dyn_EnumProcessModulesEx
#undef GetModuleInformation
#define GetModuleInformation		dyn_GetModuleInformation
#undef GetModuleFileNameExA
#define GetModuleFileNameExA		dyn_GetModuleFileNameExA
#undef GetModuleFileNameExW
#define GetModuleFileNameExW		dyn_GetModuleFileNameExW
#define LookupPrivilegeValueA		dyn_LookupPrivilegeValueA
#define OpenProcessToken		dyn_OpenProcessToken
#define GetConsoleFontSize		dyn_GetConsoleFontSize
#define GetCurrentConsoleFont		dyn_GetCurrentConsoleFont
#define Wow64SuspendThread		dyn_Wow64SuspendThread
#define Wow64GetThreadContext		dyn_Wow64GetThreadContext
#define Wow64SetThreadContext		dyn_Wow64SetThreadContext
#define Wow64GetThreadSelectorEntry	dyn_Wow64GetThreadSelectorEntry
#define GenerateConsoleCtrlEvent	dyn_GenerateConsoleCtrlEvent
#define InitializeProcThreadAttributeList dyn_InitializeProcThreadAttributeList
#define UpdateProcThreadAttribute dyn_UpdateProcThreadAttribute
#define DeleteProcThreadAttributeList dyn_DeleteProcThreadAttributeList

typedef BOOL WINAPI (AdjustTokenPrivileges_ftype) (HANDLE, BOOL,
						   PTOKEN_PRIVILEGES,
						   DWORD, PTOKEN_PRIVILEGES,
						   PDWORD);
extern AdjustTokenPrivileges_ftype *AdjustTokenPrivileges;

typedef BOOL WINAPI (DebugActiveProcessStop_ftype) (DWORD);
extern DebugActiveProcessStop_ftype *DebugActiveProcessStop;

typedef BOOL WINAPI (DebugBreakProcess_ftype) (HANDLE);
extern DebugBreakProcess_ftype *DebugBreakProcess;

typedef BOOL WINAPI (DebugSetProcessKillOnExit_ftype) (BOOL);
extern DebugSetProcessKillOnExit_ftype *DebugSetProcessKillOnExit;

typedef BOOL WINAPI (EnumProcessModules_ftype) (HANDLE, HMODULE *, DWORD,
						LPDWORD);
extern EnumProcessModules_ftype *EnumProcessModules;

#ifdef __x86_64__
typedef BOOL WINAPI (EnumProcessModulesEx_ftype) (HANDLE, HMODULE *, DWORD,
						  LPDWORD, DWORD);
extern EnumProcessModulesEx_ftype *EnumProcessModulesEx;
#endif

typedef BOOL WINAPI (GetModuleInformation_ftype) (HANDLE, HMODULE,
						  LPMODULEINFO, DWORD);
extern GetModuleInformation_ftype *GetModuleInformation;

typedef DWORD WINAPI (GetModuleFileNameExA_ftype) (HANDLE, HMODULE, LPSTR,
						  DWORD);
extern GetModuleFileNameExA_ftype *GetModuleFileNameExA;

typedef DWORD WINAPI (GetModuleFileNameExW_ftype) (HANDLE, HMODULE,
						   LPWSTR, DWORD);
extern GetModuleFileNameExW_ftype *GetModuleFileNameExW;

typedef BOOL WINAPI (LookupPrivilegeValueA_ftype) (LPCSTR, LPCSTR, PLUID);
extern LookupPrivilegeValueA_ftype *LookupPrivilegeValueA;

typedef BOOL WINAPI (OpenProcessToken_ftype) (HANDLE, DWORD, PHANDLE);
extern OpenProcessToken_ftype *OpenProcessToken;

typedef BOOL WINAPI (GetCurrentConsoleFont_ftype) (HANDLE, BOOL,
						   CONSOLE_FONT_INFO *);
extern GetCurrentConsoleFont_ftype *GetCurrentConsoleFont;

typedef COORD WINAPI (GetConsoleFontSize_ftype) (HANDLE, DWORD);
extern GetConsoleFontSize_ftype *GetConsoleFontSize;

#ifdef __x86_64__
typedef DWORD WINAPI (Wow64SuspendThread_ftype) (HANDLE);
extern Wow64SuspendThread_ftype *Wow64SuspendThread;

typedef BOOL WINAPI (Wow64GetThreadContext_ftype) (HANDLE, PWOW64_CONTEXT);
extern Wow64GetThreadContext_ftype *Wow64GetThreadContext;

typedef BOOL WINAPI (Wow64SetThreadContext_ftype) (HANDLE,
						   const WOW64_CONTEXT *);
extern Wow64SetThreadContext_ftype *Wow64SetThreadContext;

typedef BOOL WINAPI (Wow64GetThreadSelectorEntry_ftype) (HANDLE, DWORD,
							 PLDT_ENTRY);
extern Wow64GetThreadSelectorEntry_ftype *Wow64GetThreadSelectorEntry;
#endif

typedef BOOL WINAPI (GenerateConsoleCtrlEvent_ftype) (DWORD, DWORD);
extern GenerateConsoleCtrlEvent_ftype *GenerateConsoleCtrlEvent;

/* We use a local typedef for this type to avoid depending on
   Windows 8.  */
typedef void *gdb_lpproc_thread_attribute_list;

typedef BOOL WINAPI (InitializeProcThreadAttributeList_ftype)
     (gdb_lpproc_thread_attribute_list lpAttributeList,
      DWORD dwAttributeCount, DWORD dwFlags, PSIZE_T lpSize);
extern InitializeProcThreadAttributeList_ftype *InitializeProcThreadAttributeList;

typedef BOOL WINAPI (UpdateProcThreadAttribute_ftype)
     (gdb_lpproc_thread_attribute_list lpAttributeList,
      DWORD dwFlags, DWORD_PTR Attribute, PVOID lpValue, SIZE_T cbSize,
      PVOID lpPreviousValue, PSIZE_T lpReturnSize);
extern UpdateProcThreadAttribute_ftype *UpdateProcThreadAttribute;

typedef void WINAPI (DeleteProcThreadAttributeList_ftype)
     (gdb_lpproc_thread_attribute_list lpAttributeList);
extern DeleteProcThreadAttributeList_ftype *DeleteProcThreadAttributeList;

/* Return true if it's possible to disable randomization on this
   host.  */

extern bool disable_randomization_available ();

/* This is available starting with Windows 10.  */
#ifndef DBG_REPLY_LATER
# define DBG_REPLY_LATER 0x40010001L
#endif

/* Return true if it's possible to use DBG_REPLY_LATER with
   ContinueDebugEvent on this host.  */
extern bool dbg_reply_later_available ();

/* Load any functions which may not be available in ancient versions
   of Windows.  */

extern bool initialize_loadable ();

}

#endif
