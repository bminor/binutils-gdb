/* File ada-tasks.c: Ada tasking control for GDB
   Copyright 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004
   Free Software Foundation, Inc.
   Contributed by Ada Core Technologies, Inc.

   This file is part of GDB.

   Authors: Roch-Alexandre Nomine Beguin, Arnaud Charlet <charlet@gnat.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

*/

#include <ctype.h>
#include <gdb_string.h>
#include "defs.h"
#include "command.h"
#include "value.h"
#include "language.h"
#include "inferior.h"
#include "symtab.h"
#include "target.h"
#include "gdbcore.h"
#include "gdbthread.h"
#include "regcache.h"       /* for registers_changed */

#if defined (__fsu__) || defined (HAVE_SPYTHREAD) \
    || (defined(__alpha__) && defined(__osf__) && !defined(__alpha_vxworks))
#include <sys/procfs.h>
#endif

#if defined (__fsu__) || defined (HAVE_SPYTHREAD) \
    || (defined(__alpha__) && defined(__osf__) && !defined(VXWORKS_TARGET))
#include "gregset.h"
#endif

#ifdef I386_GNULINUX_TARGET
#include "gdb_thread_db.h"
#endif

#if defined (HAVE_SPYTHREAD)
#include "irix6-spyThread.h"
#endif

#include "ada-lang.h"
#include "observer.h"

enum task_states
{
  Unactivated,
  Runnable,
  Terminated,
  Activator_Sleep,
  Acceptor_Sleep,
  Entry_Caller_Sleep,
  Async_Select_Sleep,
  Delay_Sleep,
  Master_Completion_Sleep,
  Master_Phase_2_Sleep
};

struct task_control_block
{
  char state;
  CORE_ADDR parent;
  int priority;
  char image [32];
  int image_len;    /* This field is not always present in the ATCB.  */
  CORE_ADDR call;
  CORE_ADDR thread;
  CORE_ADDR lwp;    /* This field is not always present in the ATCB.  */
};

/* The index of certain important fields in the Ada Task Control Block
   record and sub-records.  */

struct tcb_fieldnos
{
  /* Fields in record Ada_Task_Control_Block.  */
  int common;

  /* Fields in record Common_ATCB.  */
  int state;
  int parent;
  int priority;
  int image;
  int image_len;     /* This field may be missing.  */
  int call;
  int ll;

  /* Fields in Task_Primitives.Private_Data.  */
  int ll_thread;
  int ll_lwp;        /* This field may be missing.  */
};

#if defined (linux)
#define TASK_LWP(atcb) 0L
#else
#define TASK_LWP(atcb) extract_unsigned_integer (&(atcb).lwp, sizeof ((atcb).lwp))
#endif

struct task_ptid
{
  int pid;                      /* The Process id */
  long lwp;                     /* The Light Weight Process id */
  long tid;                     /* The Thread id */
};
typedef struct task_ptid task_ptid_t;

struct task_entry
{
  CORE_ADDR task_id;
  struct task_control_block atcb;
  int task_num;
  int known_tasks_index;
  struct task_entry *next_task;
  task_ptid_t task_ptid;
  int stack_per;
};

/* FIXME: move all this conditional compilation in description
   files or in configure.in */

#if defined(__alpha__) && defined(__osf__) && !defined(VXWORKS_TARGET)
#define SPECIAL_THREAD_SUPPORT_ACTIVE() thread_support
#define SAVE_TASK_REGISTERS(task) \
  do { fill_gregset (&gregset_saved, -1); \
       fill_fpregset (&fpregset_saved, -1); \
  } while (0)
#define RESTORE_TASK_REGISTERS(task) \
  do { supply_gregset (&gregset_saved); \
       supply_fpregset (&fpregset_saved); \
  } while (0)

#define THREAD_FETCH_REGISTERS dec_thread_fetch_registers
#define GET_CURRENT_THREAD(PTID) dec_thread_get_current_thread ()
extern unsigned long dec_thread_get_current_thread (void);
extern int dec_thread_get_registers (gdb_gregset_t *, gdb_fpregset_t *);
extern int dec_thread_fetch_registers (void);
#endif

#ifdef __fsu__
#define SPECIAL_THREAD_SUPPORT_ACTIVE() \
   (thread_support && pthread_kern_addr != 0)
#define SAVE_TASK_REGISTERS(task) \
  do { \
    if (pthread_kern_addr != 0) \
      { \
        fill_gregset (&gregset_saved, -1); \
        fill_fpregset (&fpregset_saved, -1); \
      } \
  } while (0)
#define RESTORE_TASK_REGISTERS(task) \
  do { \
    if (pthread_kern_addr != 0) \
      { \
        supply_gregset (&gregset_saved); \
        supply_fpregset (&fpregset_saved); \
      } \
  } while (0)

extern int fsu_thread_fetch_registers (void);
extern unsigned long fsu_thread_get_current_thread (void);
static int fsu_or_linux_thread_fetch_registers (void);
static long fsu_or_linux_thread_get_current_thread (ptid_t);
#define THREAD_FETCH_REGISTERS fsu_or_linux_thread_fetch_registers
#define GET_CURRENT_THREAD(PTID) fsu_or_linux_thread_get_current_thread (PTID)
#define PTHREAD_KERN "pthread_kern"
#endif

#ifdef I386_GNULINUX_TARGET
extern td_thrinfo_t thread_db_pid_to_thread_info (int pid);
extern int thread_db_tid_to_pid (void *tid);
#endif

#if defined(VXWORKS_TARGET)
#define GET_CURRENT_THREAD(PTID) (unsigned long) ptid_get_pid (PTID)
#define THREAD_FETCH_REGISTERS() (-1)

#elif defined (__WIN32__) || defined (__CYGWIN__) || defined (hpux)
#define GET_CURRENT_THREAD(PTID) ptid_get_pid (PTID)
#define THREAD_FETCH_REGISTERS() (-1)
#endif

#if defined (HAVE_SPYTHREAD)
#define GET_CURRENT_THREAD(PTID) (unsigned long) TIDGET (PTID)
#endif

#if !defined(GET_CURRENT_THREAD)
#define GET_CURRENT_THREAD(PTID) (unsigned long) ptid_get_tid (PTID)
#endif

#if !defined(THREAD_FETCH_REGISTERS)
#define THREAD_FETCH_REGISTERS() (target_fetch_registers (-1), 0)
#endif

#if !defined(SAVE_TASK_REGISTERS)
#define SAVE_TASK_REGISTERS(task)
#define RESTORE_TASK_REGISTERS(task)
#endif

#if !defined(SPECIAL_THREAD_SUPPORT_ACTIVE)
#define SPECIAL_THREAD_SUPPORT_ACTIVE() 0
#endif

#define KNOWN_TASKS_NAME "system__tasking__debug__known_tasks"

#define READ_MEMORY(addr, var) read_memory (addr, (char *) &var, sizeof (var))

/* If defined to 1, means that the thread ptids maintained by core GDB
   follow this format : first field (pid) contains the tid
                       second field (lwp) contains 0
                       third field (tid) contains 0 */
#ifndef THREAD_PTID_CONTAINS_TID_NULL_NULL
#define THREAD_PTID_CONTAINS_TID_NULL_NULL (0)
#endif

/* If defined to 1, means that the thread ptids maintained by core GDB
   follow this format: first field (pid) contains the LWP id
                       second field (lwp) contains 0
                       third field (tid) contains 0 */
#ifndef THREAD_PTID_CONTAINS_LWP_NULL_NULL
#define THREAD_PTID_CONTAINS_LWP_NULL_NULL (0)
#endif

/* If defined to 1, means that the thread ptids maintained by core GDB
   follow this format: first field (pid) contains the PID
                       second field (lwp) contains 0
                       third field (tid) contains the TID */
#ifndef THREAD_PTID_CONTAINS_PID_NULL_TID
#define THREAD_PTID_CONTAINS_PID_NULL_TID (0)
#endif

/* If defined to 1, means that the thread ptids maintained by core GDB
   follow this format: first field (pid) contains the PID
                       second field (lwp) contains the TID
                       third field (tid) contains the 0 */

#ifndef THREAD_PTID_CONTAINS_PID_TID_NULL
#define THREAD_PTID_CONTAINS_PID_TID_NULL (0)
#endif

/* If defined to 1, means that the thread id is not stored in the tid
   field of the task_ptid, but rather in the lwp field.  */
#ifndef ADA_THREAD_ID_IN_LWP
#define ADA_THREAD_ID_IN_LWP (0)
#endif

static int task_ptid_get_pid (task_ptid_t task_ptid);
static long task_ptid_get_lwp (task_ptid_t task_ptid);
static long task_ptid_get_tid (task_ptid_t task_ptid);
static task_ptid_t task_ptid_build (int pid, long lwp, long tid);
static ptid_t task_ptid_get_ptid (task_ptid_t task_ptid);
static long task_ptid_get_thread_id (task_ptid_t task_ptid);

static int task_is_alive (enum task_states state);
static CORE_ADDR get_self_id (ptid_t);
static int get_entry_number (CORE_ADDR);
static void get_tcb_types_info (struct type **atcb_type,
                                struct type **atcb_common_type,
                                struct type **atcb_ll_type,
                                struct tcb_fieldnos *atcb_fieldnos);
static void get_tcb_call_type_info (struct type **atcb_call_type,
                                    int *atcb_call_self_fieldno);
static CORE_ADDR get_known_tasks_addr (void);
static int read_known_tasks_array (void);
static int build_task_list (void);
static void value_as_string (char *dest, struct value *val, int length);
static struct task_control_block read_atcb (CORE_ADDR atcb_addr);
static CORE_ADDR read_caller (const CORE_ADDR call);
static void display_current_task_id (void);
static void task_command_1 (char *tidstr, int from_tty);

/* Ada-tasks observers.  */

static void normal_stop_notification (void);
static void ada_tasks_attach_observers (void);

/* Global visible variables */

int ada__tasks_check_symbol_table = 1;
CORE_ADDR pthread_kern_addr = 0;

/* Local global variables.  */
static struct task_entry *task_list = NULL;

/* When non-zero, this flag indicates that the current task_list
   is obsolete, and should be recomputed before it is accessed.  */
static int stale_task_list_p = 1;

#if defined (__fsu__) || (defined(__alpha__) && defined(__osf__) && !defined(VXWORKS_TARGET))
gdb_gregset_t gregset_saved;
gdb_fpregset_t fpregset_saved;
#endif

/* The maximum number of tasks known to the Ada runtime */
static const int MAX_NUMBER_OF_KNOWN_TASKS = 1000;

/* the current task, as seen by the user. Modified everytime the user
   does a task switch.  */
static int current_task = -1;

unsigned long current_thread;

/* The task where the debugger stopped, giving control back to the user.
   Not affected by task switching. Used to restore the registers before
   continuing the inferior.  */
int current_task_id = -1;

static char *task_states[] = {
  "Unactivated",
  "Runnable",
  "Terminated",
  "Child Activation Wait",
  "Accept Statement",
  "Waiting on entry call",
  "Async Select Wait",
  "Delay Sleep",
  "Child Termination Wait",
  "Wait Child in Term Alt",
  "",
  "",
  "",
  "",
  "Asynchronous Hold"
};

/* Global internal types */

static char *long_task_states[] = {
  "Unactivated",
  "Runnable",
  "Terminated",
  "Waiting for child activation",
  "Blocked in accept statement",
  "Waiting on entry call",
  "Asynchronous Selective Wait",
  "Delay Sleep",
  "Waiting for children termination",
  "Waiting for children in terminate alternative",
  "",
  "",
  "",
  "",
  "Asynchronous Hold"
};

/* Global internal variables */

static int highest_task_num = 0;
static int thread_support = 0;  /* 1 if the thread library in use is
                                   supported. FIXME: Not reinitialized
                                   properly when program reloaded.
                                 */
#ifdef __fsu__
static int
fsu_or_linux_thread_fetch_registers (void)
{
  if (pthread_kern_addr != 0)
    return fsu_thread_fetch_registers ();

  target_fetch_registers (-1);
  return 0L;
}

static long
fsu_or_linux_thread_get_current_thread (ptid_t ptid)
{
  if (pthread_kern_addr != 0)
    return fsu_thread_get_current_thread ();

  return ptid_get_tid (ptid);
}

#endif /* __fsu__ */

static int
add_task_entry (CORE_ADDR p_task_id, int index)
{
  struct task_entry *new_task_entry = NULL;
  struct task_entry *pt;

  highest_task_num++;
  new_task_entry = xmalloc (sizeof (struct task_entry));
  new_task_entry->task_num = highest_task_num;
  new_task_entry->task_id = p_task_id;
  new_task_entry->atcb = read_atcb (p_task_id);
  new_task_entry->known_tasks_index = index;
  new_task_entry->task_ptid =
    task_ptid_build (ptid_get_pid (inferior_ptid), /* ? */
                                   TASK_LWP (new_task_entry->atcb),
                                   new_task_entry->atcb.thread);
  new_task_entry->next_task = NULL;
  pt = task_list;
  if (pt)
    {
      while (pt->next_task)
        pt = pt->next_task;
      pt->next_task = new_task_entry;
      pt->stack_per = 0;
    }
  else
    task_list = new_task_entry;
  return new_task_entry->task_num;
}

static int
get_entry_number (CORE_ADDR p_task_id)
{
  struct task_entry *pt;

  pt = task_list;
  while (pt != NULL)
    {
      if (pt->task_id == p_task_id)
        return pt->task_num;
      pt = pt->next_task;
    }
  return 0;
}

static struct task_entry *
get_thread_entry_vptr (long thread)
{
  struct task_entry *pt;

  pt = task_list;
  while (pt != NULL)
    {
      if (task_ptid_get_thread_id (pt->task_ptid) == thread)
        return pt;
      pt = pt->next_task;
    }
  return 0;
}

static struct task_entry *
get_entry_vptr (int p_task_num)
{
  struct task_entry *pt;

  pt = task_list;
  while (pt)
    {
      if (pt->task_num == p_task_num)
        return pt;
      pt = pt->next_task;
    }
  return NULL;
}

void
init_task_list (void)
{
  struct task_entry *pt, *old_pt;

  target_find_new_threads ();

  pt = task_list;
  while (pt)
    {
      old_pt = pt;
      pt = pt->next_task;
      xfree (old_pt);
    };
  task_list = NULL;
  highest_task_num = 0;
}

int
valid_task_id (int task)
{
  return get_entry_vptr (task) != NULL;
}

/* Return the pid of a given task ptid.  */

static int
task_ptid_get_pid (task_ptid_t task_ptid)
{
  return task_ptid.pid;
}

/* Return the lwp of a given task ptid.  */

static long
task_ptid_get_lwp (task_ptid_t task_ptid)
{
  return task_ptid.lwp;
}

/* Return the tid of a given task ptid.  */

static long
task_ptid_get_tid (task_ptid_t task_ptid)
{
  return task_ptid.tid;
}

/* Build a task ptid from the associated pid, lwp, and tid.  */

static task_ptid_t
task_ptid_build (int pid, long lwp, long tid)
{
  task_ptid_t task_ptid;

  task_ptid.pid = pid;
  task_ptid.lwp = lwp;
  task_ptid.tid = tid;
  return task_ptid;
}

/* Translate a task ptid into a ptid (the ptid maintained by core GDB).

   On most platforms, they are equivalent, and this function can be
   regarded as the identity. However, there are other platforms where
   the task ptid and the ptid are not equivalent. For instance, the task
   LWP value is sometimes stored by GDB-core as a pid! This translation
   therefore becomes necessary before invoking the GDB thread services.  */

static ptid_t
task_ptid_get_ptid (task_ptid_t task_ptid)
{
  ptid_t ptid;

  if (THREAD_PTID_CONTAINS_TID_NULL_NULL)
    ptid = ptid_build (task_ptid_get_tid (task_ptid), 0, 0);
  else if (THREAD_PTID_CONTAINS_LWP_NULL_NULL)
    ptid = ptid_build (task_ptid_get_lwp (task_ptid), 0, 0);
  else if (THREAD_PTID_CONTAINS_PID_NULL_TID)
    ptid = ptid_build (task_ptid_get_pid (task_ptid),
                       0, task_ptid_get_tid (task_ptid));
  else if (THREAD_PTID_CONTAINS_PID_TID_NULL)
    ptid = ptid_build (task_ptid_get_pid (task_ptid),
                       task_ptid_get_tid (task_ptid), 0);
  else
    ptid = ptid_build (task_ptid_get_pid (task_ptid),
                       task_ptid_get_lwp (task_ptid),
                       task_ptid_get_tid (task_ptid));

  return ptid;
}

/* Extract and return the thread_id for the given TASK_PTID.  */

static long
task_ptid_get_thread_id (task_ptid_t task_ptid)
{
  /* On most platforms, the thread_id is stored in task_ptid.tid.
     Unfortunately, some other platforms store it as the task_ptid.lwp...  */

  if (ADA_THREAD_ID_IN_LWP)
    return task_ptid_get_lwp (task_ptid);
  else
    return task_ptid_get_tid (task_ptid);
}

/* Return non-zero iff the task STATE corresponds to a non-terminated
   task state.  */

static int
task_is_alive (enum task_states state)
{
  return (state != Terminated);
}

static CORE_ADDR
get_self_id (ptid_t ptid)
{
#ifdef GNAT_GDB
  struct task_entry *ent;

#if !((defined(sun) && defined(__SVR4)) || defined(VXWORKS_TARGET) || defined(__WIN32__))
  if (thread_support)
#endif
    {
      ent = get_thread_entry_vptr (GET_CURRENT_THREAD (ptid));
      return ent ? ent->task_id : 0;
    }
#endif

  /* FIXME: calling a function in the inferior with a multithreaded application
     is not reliable, so return a null address if there is no safe way to
     get the current task */
  return 0;
}

int
ada_get_current_task (ptid_t ptid)
{
  int result;

  if (current_language->la_language != language_ada)
    return -1;

  result = get_entry_number (get_self_id (ptid));

  /* return -1 if not found */
  return result == 0 ? -1 : result;
}

/* Get from the debugging information the type description of all types
   related to the Ada Task Control Block that will be needed in order to
   read the list of known tasks in the Ada runtime.  Also return the
   associated ATCB_FIELDNOS.

   Error handling:  Any data missing from the debugging info will cause
   an error to be raised, and none of the return values to be set.
   Users of this function can depend on the fact that all or none of the
   return values will be set.  */

static void
get_tcb_types_info (struct type **atcb_type,
                    struct type **atcb_common_type,
                    struct type **atcb_ll_type,
                    struct tcb_fieldnos *atcb_fieldnos)
{
  struct type *type;
  struct type *common_type;
  struct type *ll_type;
  struct tcb_fieldnos fieldnos;

#ifndef ADA_RETAIN_DOTS
  const char *atcb_name = "system__tasking__ada_task_control_block___XVE";
  const char *common_atcb_name = "system__tasking__common_atcb";
  const char *private_data_name = "system__task_primitives__private_data";
#else
  const char *atcb_name = "system.tasking.ada_task_control_block___XVE";
  const char *common_atcb_name = "system.tasking.common_atcb";
  const char *private_data_name = "system.task_primitives.private_data";
#endif

  const struct symbol *atcb_sym =
    lookup_symbol (atcb_name, NULL, VAR_DOMAIN, NULL, NULL);
  const struct symbol *common_atcb_sym =
    lookup_symbol (common_atcb_name, NULL, VAR_DOMAIN, NULL, NULL);
  const struct symbol *private_data_sym =
    lookup_symbol (private_data_name, NULL, VAR_DOMAIN, NULL, NULL);

  if (atcb_sym == NULL || atcb_sym->type == NULL)
    error ("Can not find Ada_Task_Control_Block type. Aborting");
  if (common_atcb_sym == NULL || common_atcb_sym->type == NULL)
    error ("Can not find Common_ATCB type. Aborting");
  if (private_data_sym == NULL || private_data_sym->type == NULL)
    error ("Can not find Private_Data type. Aborting");

  /* Get a static representation of the type record Ada_Task_Control_Block.  */
  type = atcb_sym->type;
  type = ada_template_to_fixed_record_type_1 (type, NULL, 0, NULL, 0);

  /* Get the type for Ada_Task_Control_Block.Common.  */
  common_type = common_atcb_sym->type;

  /* Get the type for Ada_Task_Control_Bloc.Common.Call.LL.  */
  ll_type = private_data_sym->type;

  /* Get the field indices.  */
  fieldnos.common = ada_get_field_index (type, "common", 0);
  fieldnos.state = ada_get_field_index (common_type, "state", 0);
  fieldnos.parent = ada_get_field_index (common_type, "parent", 0);
  fieldnos.priority = ada_get_field_index (common_type, "base_priority", 0);
  fieldnos.image = ada_get_field_index (common_type, "task_image", 0);
  fieldnos.image_len = ada_get_field_index (common_type, "task_image_len", 1);
  fieldnos.call = ada_get_field_index (common_type, "call", 0);
  fieldnos.ll = ada_get_field_index (common_type, "ll", 0);
  fieldnos.ll_thread = ada_get_field_index (ll_type, "thread", 0);
  fieldnos.ll_lwp = ada_get_field_index (ll_type, "lwp", 1);

  /* On certain platforms such as x86-windows, the "lwp" field has been
     named "thread_id".  This field will likely be renamed in the future,
     but we need to support both possibilities to avoid an unnecessary
     dependency on a recent compiler.  We therefore try locating the
     "thread_id" field in place of the "lwp" field if we did not find
     the latter.  */
  if (fieldnos.ll_lwp < 0)
    fieldnos.ll_lwp = ada_get_field_index (ll_type, "thread_id", 1);

  /* Set all the out parameters all at once, now that we are certain
     that there are no potential error() anymore.  */
  *atcb_type = type;
  *atcb_common_type = common_type;
  *atcb_ll_type = ll_type;
  *atcb_fieldnos = fieldnos;
}

/* Get from the debugging information the type description of the
   record type Entry_Call_Record (this is the type of the field
   Call.all in the Common_ATCB record type). Also return the index
   of the field "Self" in Entry_Call_Record.

   Error handling:  Any data missing from the debugging info will cause
   an error to be raised, and none of the return values to be set.
   Users of this function can depend on the fact that all or none of the
   return values will be set.  */

static void
get_tcb_call_type_info (struct type **atcb_call_type,
                        int *atcb_call_self_fieldno)
{
  struct type *call_type;
  int call_self_fieldno;

#ifndef ADA_RETAIN_DOTS
  const char *entry_call_record_name = "system__tasking__entry_call_record";
#else
  const char *entry_call_record_name = "system.tasking.entry_call_record";
#endif

  const struct symbol *entry_call_record_sym =
    lookup_symbol (entry_call_record_name, NULL, VAR_DOMAIN, NULL, NULL);

  if (entry_call_record_sym == NULL || entry_call_record_sym->type == NULL)
    error ("Can not find Entry_Call_Record type. Aborting");

  call_type = entry_call_record_sym->type;
  call_self_fieldno = ada_get_field_index (call_type, "self", 0);

  /* Set all the out parameters all at once, now that we are certain
     that there are no potential error() anymore.  */
  *atcb_call_type = call_type;
  *atcb_call_self_fieldno = call_self_fieldno;
}

/* Return the address of the Known_Tasks array maintained in
   the Ada Runtime.  Return NULL if the array could not be found,
   meaning that the inferior program probably does not use tasking.

   In order to provide a fast response time, this function caches
   the Known_Tasks array address after the lookup during the first
   call. Subsequent calls will simply return this cached address.  */

static CORE_ADDR
get_known_tasks_addr (void)
{
  static CORE_ADDR known_tasks_addr = 0;

  if (ada__tasks_check_symbol_table)
    {
      struct symbol *sym;
      struct minimal_symbol *msym;

      thread_support = 0;
#if (defined(__alpha__) && defined(__osf__) & !defined(VXWORKS_TARGET)) \
    || defined (_AIX) || defined (__CYGWIN__)
      thread_support = 1;
#elif defined (__fsu__)
      msym = lookup_minimal_symbol (PTHREAD_KERN, NULL, NULL);
      if (msym != NULL)
        {
          pthread_kern_addr = SYMBOL_VALUE_ADDRESS (msym);
          thread_support = 1;
        }
#elif defined (HAVE_SPYTHREAD)
      thread_support = libspy_enabled;
#endif

#ifdef I386_GNULINUX_TARGET
      /* We support threads via the Linux Threads...  */
      thread_support = 1;
#endif

      msym = lookup_minimal_symbol (KNOWN_TASKS_NAME, NULL, NULL);
      if (msym != NULL)
        known_tasks_addr = SYMBOL_VALUE_ADDRESS (msym);
      else
#ifndef VXWORKS_TARGET
        return 0;
#else
        {
          if (target_lookup_symbol (KNOWN_TASKS_NAME, &known_tasks_addr) != 0)
            return 0;
        }
#endif

      /* FIXME: brobecker 2003-03-05: Here would be a much better place
         to attach the ada-tasks observers, instead of doing this
         unconditionaly in _initialize_tasks. This would avoid an
         unecessary notification when the inferior does not use tasking
         or as long as the user does not use the ada-tasks commands.
         Unfortunately, this is not possible for the moment: the current
         code resets ada__tasks_check_symbol_table back to 1 whenever
         symbols for a new program are being loaded. If we place the
         observers intialization here, we will end up adding new observers
         everytime we do the check for Ada tasking-related symbols
         above. This would currently have benign effects, but is still
         undesirable. The cleanest approach is probably to create a new
         observer to notify us when the user is debugging a new program.
         We would then reset ada__tasks_check_symbol_table back to 1
         during the notification, but also detach all observers.
         BTW: observers are probably not reentrant, so detaching during
         a notification may not be the safest thing to do... Sigh...
         But creating the new observer would be a good idea in any case,
         since this allow us to make ada__tasks_check_symbol_table
         static, which is a good bonus.  */
      ada__tasks_check_symbol_table = 0;
    }

  return known_tasks_addr;
}

/* Read the Known_Tasks array from the inferior memory, and store
   it in task_list.  Return non-zero upon success.  */

static int
read_known_tasks_array (void)
{
  const int target_ptr_byte = TARGET_PTR_BIT / TARGET_CHAR_BIT;
  const int temp_tasks_size = target_ptr_byte * MAX_NUMBER_OF_KNOWN_TASKS;
  const CORE_ADDR known_tasks_addr = get_known_tasks_addr ();
  char *temp_tasks = (char *) alloca (temp_tasks_size);
  CORE_ADDR temp_task;
  int i;

  /* Step 1: Clear the current list, if any.  */
  init_task_list ();

  /* If the application does not use task, then no more needs to be done.
     It is important to have the task list cleared (see above) before we
     return, as we don't want a stale task list to be used...  This can
     happen for instance when debugging a non-multitasking program after
     having debugged a multitasking one.  */
  if (known_tasks_addr == 0)
    return 0;

  /* Step 2: Build a new list by reading the ATCBs from the Known_Tasks
     array in the Ada runtime.  */
  read_memory (known_tasks_addr, temp_tasks, temp_tasks_size);
  for (i = 0; i < MAX_NUMBER_OF_KNOWN_TASKS; i++)
    {
      temp_task = extract_typed_address (temp_tasks + i * target_ptr_byte,
					 builtin_type_void_data_ptr);

      if (temp_task != 0)
        {
          if (get_entry_number (temp_task) == 0)
            add_task_entry (temp_task, i);
        }
    }

  /* Step 3: Unset stale_task_list_p, to avoid re-reading the Known_Tasks
     array unless needed.  Then report a success.  */
  stale_task_list_p = 0;
  return 1;
}

/* Builds the task_list by reading the Known_Tasks array from
   the inferior.  Prints an appropriate message and returns non-zero
   if it failed to build this list.  */

static int
build_task_list (void)
{
  if (!target_has_stack)
    error ("No stack");

  if (stale_task_list_p)
    read_known_tasks_array ();

  if (task_list == NULL)
    {
      printf_filtered ("Your application does not use any Ada task.\n");
      return 0;
    }

  return 1;
}

/* Extract the contents of the value as a string whose length is LENGTH,
   and store the result in DEST.  */

static void
value_as_string (char *dest, struct value *val, int length)
{
  memcpy (dest, VALUE_CONTENTS (val), length);
}

/* Extract the string image from the fat string corresponding to VAL,
   and store it in DEST. The length of the string is stored in LEN.  If
   the string length is greater than MAX_LEN, then truncate the result
   to the first MAX_LEN characters of the fat string.  */

static void
read_fat_string_value (char *dest, int *len, struct value *val, int max_len)
{
  struct value *array_val;
  struct value *bounds_val;

  /* The following variables are made static to avoid recomputing them
     each time this function is called.  */
  static int initialize_fieldnos = 1;
  static int array_fieldno;
  static int bounds_fieldno;
  static int upper_bound_fieldno;

  /* Get the index of the fields that we will need to read in order
     to extract the string from the fat string.  */
  if (initialize_fieldnos)
    {
      struct type *type = VALUE_TYPE (val);
      struct type *bounds_type;

      array_fieldno = ada_get_field_index (type, "P_ARRAY", 0);
      bounds_fieldno = ada_get_field_index (type, "P_BOUNDS", 0);

      bounds_type = TYPE_FIELD_TYPE (type, bounds_fieldno);
      if (TYPE_CODE (bounds_type) == TYPE_CODE_PTR)
        bounds_type = TYPE_TARGET_TYPE (bounds_type);
      if (TYPE_CODE (bounds_type) != TYPE_CODE_STRUCT)
        error ("Unknown task name format. Aborting");
      upper_bound_fieldno = ada_get_field_index (bounds_type, "UB0", 0);

      initialize_fieldnos = 0;
    }

  /* Get the size of the task image by checking the value of the bounds.
     The lower bound is always 1, so we only need to read the upper bound.  */
  bounds_val = value_ind (value_field (val, bounds_fieldno));
  *len = value_as_long (value_field (bounds_val, upper_bound_fieldno));

  /* Make sure that we do not read more than max_len characters...  */
  if (*len > max_len)
    *len = max_len;

  /* Extract LEN characters from the fat string.  */
  array_val = value_ind (value_field (val, array_fieldno));
  read_memory (VALUE_ADDRESS (array_val), dest, *len);
}

/* Read the ATCB stored at ATCB_ADDR from the inferior memory.  */

static struct task_control_block
read_atcb (CORE_ADDR atcb_addr)
{
  /* The type description for the ATCB record and subrecords, and
     the associated tcb_fieldnos. For efficiency reasons, these are made
     static so that we can compute them only once the first time and
     reuse them later.  */
  static struct type *atcb_type = NULL;
  static struct type *atcb_common_type = NULL;
  static struct type *atcb_ll_type = NULL;
  static struct tcb_fieldnos fieldno;

  struct task_control_block result;
  struct value *tcb_value;
  struct value *ll_value;

  if (atcb_type == NULL)
    get_tcb_types_info (&atcb_type, &atcb_common_type, &atcb_ll_type, &fieldno);

  tcb_value = value_from_contents_and_address (atcb_type, NULL, atcb_addr);
  tcb_value = value_field (tcb_value, fieldno.common);

  result.state = value_as_long (value_field (tcb_value, fieldno.state));
  result.parent = value_as_address (value_field (tcb_value, fieldno.parent));
  result.priority = value_as_long (value_field (tcb_value, fieldno.priority));

  /* Depending on the GNAT version used, the task image is either a fat
     string, or a thin array of characters.  Older versions of GNAT used
     to use fat strings, and therefore did not need an extra field in
     the ATCB to store the string length. For efficiency reasons, newer
     versions of GNAT replaced the fat string by a static buffer, but this
     also required the addition of a new field named "Image_Len" containing
     the length of the task name. The method used to extract the task name
     is selected depending on the existence of this field.  */
  if (fieldno.image_len == -1)
    {
      read_fat_string_value (result.image, &result.image_len,
                             value_field (tcb_value, fieldno.image),
                             sizeof (result.image));
    }
  else
    {
      value_as_string (result.image, value_field (tcb_value, fieldno.image),
                       sizeof (result.image));
      result.image_len =
        value_as_long (value_field (tcb_value, fieldno.image_len));
    }

  result.call = value_as_address (value_field (tcb_value, fieldno.call));

  ll_value = value_field (tcb_value, fieldno.ll);
  result.thread = value_as_address (value_field (ll_value, fieldno.ll_thread));
  if (fieldno.ll_lwp >= 0)
    result.lwp = value_as_address (value_field (ll_value, fieldno.ll_lwp));
  else
    result.lwp = 0;

  return result;
}

/*  Read the ID of the task with which a task is attempting a rendez-vous
    from the address of its Entry_Call_Record in the Ada TCB.
    If the address of the Entry_Call_Record is null, then return null.  */

static CORE_ADDR
read_caller (const CORE_ADDR call)
{
  /* The type description for the Entry_Call_Record, and the index of
     the field "Self".  For efficiency reasons, these are made static
     so that we can compute them only once the first time and reuse them
     later.  */
  static struct type *atcb_call_type;
  static int self_fieldno = -1;

  struct value *call_value;

  if (call == 0)
    return 0;

  if (atcb_call_type == NULL)
    get_tcb_call_type_info (&atcb_call_type, &self_fieldno);

  call_value = value_from_contents_and_address (atcb_call_type, NULL, call);
  return value_as_address (value_field (call_value, self_fieldno));
}

#if 0
/* FIXME: Now modified and back in breakpoint.c */
/* breakpoint_task_match (PC) returns true if the breakpoint at PC
   is valid for current task.  */

int
breakpoint_task_match (CORE_ADDR pc)
{
  const int this_task = get_current_task ();
  const struct breakpoint *breakpoints = get_breakpoint_chain ();
  const struct breakpoint *b;

  for (b = breakpoints; b; b = b->next)
    {
      if (b->enable_state != bp_disabled
          && b->enable_state != bp_shlib_disabled
          && (b->address == 0 || b->address == pc)
          && (b->task == 0 || b->task == this_task))
        {
          return 1;
        }
    }

  return 0;
}
#endif

/* Print detailed information about specified task */

static void
info_task (char *arg, int from_tty)
{
#ifdef GNAT_GDB
  struct task_entry *pt, *pt2;
  CORE_ADDR caller;
  int num;

  if (current_language->la_language != language_ada)
    {
      printf_filtered ("The current language does not support tasks.\n");
      return;
    }

  target_find_new_threads ();

  pt = get_entry_vptr (atoi (arg));
  if (pt == NULL)
    {
      printf_filtered ("Task %s not found.\n", arg);
      return;
    }

  /* print the Ada task id */
  printf_filtered ("Ada Task: %s\n", paddr_nz (pt->task_id));

  /* print the name of the task */
  if (pt->atcb.image_len != 0)
    printf_filtered ("Name: %.*s\n", pt->atcb.image_len, pt->atcb.image);
  else
    printf_filtered ("<no name>\n");

  /* print the thread id */

  if (task_ptid_get_tid (pt->task_ptid) < 65536)
    printf_filtered
      ("Thread: %ld\n", (long int) task_ptid_get_tid (pt->task_ptid));
  else
    printf_filtered
      ("Thread: %#lx\n", (long int) task_ptid_get_tid (pt->task_ptid));

  if (task_ptid_get_lwp (pt->task_ptid) != 0)
    {
      if ((long) task_ptid_get_lwp (pt->task_ptid) < 65536)
        printf_filtered
          ("LWP: %ld\n", (long int) task_ptid_get_lwp (pt->task_ptid));
      else
        printf_filtered
          ("LWP: %#lx\n", (long int) task_ptid_get_lwp (pt->task_ptid));
    }

  /* print the parent gdb task id */
  num = get_entry_number (pt->atcb.parent);
  if (num != 0)
    {
      printf_filtered ("Parent: %d", num);
      pt2 = get_entry_vptr (num);

      /* print the name of the task */
      if (pt2->atcb.image_len != 0)
	printf_filtered (" (%.*s)\n", pt2->atcb.image_len, pt2->atcb.image);
      else
	printf_filtered ("\n");
    }
  else
    printf_filtered ("No parent\n");

  /* print the base priority of the task */
  printf_filtered ("Base Priority: %d\n", pt->atcb.priority);

  /* print the current state of the task */

  /* check if this task is accepting a rendezvous */
  caller = read_caller (pt->atcb.call);
  if (caller != 0)
    {
      num = get_entry_number (caller);
      printf_filtered ("Accepting rendezvous with %d", num);

      if (num != 0)
	{
	  pt2 = get_entry_vptr (num);

	  /* print the name of the task */
	  if (pt2->atcb.image_len != 0) {
	    printf_filtered (" (%.*s)\n", pt2->atcb.image_len, pt2->atcb.image);
	  }
	  else
	    printf_filtered ("\n");
	}
      else
        printf_filtered ("\n");
    }
  else
    printf_filtered ("State: %s\n", long_task_states[pt->atcb.state]);
#endif
}

/* Print information about currently known tasks */

static void
info_tasks (char *arg, int from_tty)
{
#ifdef GNAT_GDB
  struct task_entry *pt;
  CORE_ADDR caller;
  long thread_id = 0L;
  int size;
  char car;
  ptid_t current_ptid;

#if defined(__alpha__) && defined(__osf__) && !defined(VXWORKS_TARGET)
  pthreadTeb_t thr;
  gdb_gregset_t regs;
#endif

  current_ptid = inferior_ptid;

#if !((defined(sun) && defined(__SVR4)) || defined(VXWORKS_TARGET) || defined(__WIN32__) || defined (hpux))
  if (thread_support)
#endif
    thread_id = GET_CURRENT_THREAD (inferior_ptid);

  /* print the header */

#if defined(__alpha__) && defined(__osf__) && !defined(VXWORKS_TARGET)
  printf_filtered
    ("  ID       TID P-ID Pri Stack  %% State                  Name\n");
#else
  printf_filtered ("  ID       TID P-ID Pri State                  Name\n");
#endif

  /* Now that we have a list of task id's, we can print them */
  pt = task_list;
  while (pt)
    {
      /* print a star if this task is the current one */
      if (thread_id)
#if defined (__WIN32__) || defined (SGI) || defined (hpux)
        printf_filtered
          (task_ptid_get_lwp (pt->task_ptid) == thread_id ? "*" : " ");
#else
        printf_filtered
          (task_ptid_get_thread_id (pt->task_ptid) == thread_id ? "*" : " ");
#endif

      /* print the gdb task id */
      printf_filtered ("%3d", pt->task_num);

      /* print the Ada task id */
      printf_filtered (" %9lx", (long) pt->task_id);

      /* print the parent gdb task id */
      printf_filtered (" %4d", get_entry_number (pt->atcb.parent));

      /* print the base priority of the task */
      printf_filtered (" %3d", pt->atcb.priority);

#if defined(__alpha__) && defined(__osf__) && !defined(VXWORKS_TARGET)
      if (pt->task_num == 1 || pt->atcb.state == Terminated)
        {
          printf_filtered ("  Unknown");
          goto next;
        }

      READ_MEMORY (pt->atcb.thread, thr);
      switch_to_thread (task_ptid_get_ptid (pt->task_ptid));
      /* ??? Brobecker 2003-03-13: Not sure what the next line is used for.
         And even if useful, it should probably be replaced by call to
         task_ptid_get_thread_id.  */
      current_thread = task_ptid_get_tid (pt->task_ptid);
      regs.regs[SP_REGNUM] = 0;
      if (dec_thread_get_registers (&regs, NULL) == 0)
        {
          pt->stack_per = (100 * ((long) thr.__stack_base -
                                  regs.regs[SP_REGNUM])) / thr.__stack_size;
          /* if the thread is terminated but still there, the
             stack_base/size values are erroneous. Try to patch it */
          if (pt->stack_per < 0 || pt->stack_per > 100)
            pt->stack_per = 0;
        }
      else
        {
          /* Set stack_per to an invalid value to signal that we did not
             manage to compute its value.  */
          pt->stack_per = -1;
        }

      /* print information about stack space used in the thread */
      if (thr.__stack_size < 1024 * 1024)
        {
          size = thr.__stack_size / 1024;
          car = 'K';
        }
      else if (thr.__stack_size < 1024 * 1024 * 1024)
        {
          size = thr.__stack_size / 1024 / 1024;
          car = 'M';
        }
      else                      /* Who knows... */
        {
          size = thr.__stack_size / 1024 / 1024 / 1024;
          car = 'G';
        }

      /* print the stack usage in percent, if available.  */
      if (pt->stack_per != -1)
        printf_filtered (" %4d%c %2d", size, car, pt->stack_per);
      else
        {
          /* This error is not serious enough that we should raise
             an internal error, but print '???' to make it unambiguous
             that we failed to compute this value.  */
          printf_filtered (" ???");
        }

    next:
#endif

      /* print the current state of the task */

      /* check if this task is accepting a rendezvous */
      caller = read_caller (pt->atcb.call);
      if (caller != 0)
        printf_filtered (" Accepting RV with %-4d",
                         get_entry_number (caller));
      else
        {
#if defined (__WIN32__) || defined (SGI) || defined (hpux)
          if (pt->atcb.state == Runnable
              && (thread_id
                  && task_ptid_get_lwp (pt->task_ptid) == thread_id))
#else
          if (pt->atcb.state == Runnable
              && (thread_id
                  && task_ptid_get_thread_id (pt->task_ptid) == thread_id))
#endif
            /* Replace "Runnable" by "Running" if this is the current task */
            printf_filtered (" %-22s", "Running");
          else
            printf_filtered (" %-22s", task_states[pt->atcb.state]);
        }

      /* finally, print the name of the task */
      if (pt->atcb.image_len != 0) {
        printf_filtered (" %.*s\n", pt->atcb.image_len, pt->atcb.image);
      }
      else
	printf_filtered (" <no name>\n");

      pt = pt->next_task;
    }
  if (!ptid_equal (inferior_ptid, current_ptid))
    switch_to_thread (current_ptid);
#endif
}

static void
info_tasks_command (char *arg, int from_tty)
{
  const int task_list_built = build_task_list ();

  if (!task_list_built)
    return;

  if (arg == NULL || *arg == '\000')
    info_tasks (arg, from_tty);
  else
    info_task (arg, from_tty);
}

/* Switch to task indicated by NEW_TASK.  Return 0 iff successful.  */

static int
switch_to_task (struct task_entry *new_task)
{
#ifdef GNAT_GDB
  /* Raise an error if task-switching is currently not allowed.  */
  if (!THREAD_SWITCH_ALLOWED ())
    error ("Task switching is currently not allowed.");

  if (!task_is_alive (new_task->atcb.state))
    error ("Can not switch to task %d: Task is no longer running",
           new_task->task_num);

  current_task = new_task->task_num;
  current_thread = task_ptid_get_thread_id (new_task->task_ptid);

  if (current_task_id == -1)
    {
      SAVE_TASK_REGISTERS (new_task);
      current_task_id = ada_get_current_task (inferior_ptid);
    }

  if (SPECIAL_THREAD_SUPPORT_ACTIVE ())
    {
      /* FIXME: Integrate with switch_to_thread */
      int ret_code;
      flush_cached_frames ();
      registers_changed ();
      if (current_task == current_task_id)
        {
          RESTORE_TASK_REGISTERS (new_task);
          ret_code = 0;
        }
      else
        ret_code = THREAD_FETCH_REGISTERS ();
      if (ret_code == 0)
        stop_pc = read_pc ();
      select_frame (get_current_frame ());
      return ret_code;
    }
  else if (task_ptid_get_pid (new_task->task_ptid) != 0)        /* ?? */
    {
      switch_to_thread (task_ptid_get_ptid (new_task->task_ptid));
      return 0;
    }
#endif
  return -1;
}

/* Print a message telling the user id of the current task.
   Print an error message if the application does not appear to
   be using any Ada task.  */

static void
display_current_task_id (void) 
{
  const int current_task = ada_get_current_task (inferior_ptid);

  if (current_task == -1)
    printf_filtered ("[Current task is unknown]\n");
  else
    printf_filtered ("[Current task is %d]\n", current_task);
}

/* Parse and evaluate TIDSTR into a task id, and try to switch to
   that task.  Print an error message if the task switch failed.  */

static void
task_command_1 (char *tidstr, int from_tty)
{
  const int num = value_as_long (parse_and_eval (tidstr));
  struct task_entry *e = get_entry_vptr (num);

  if (e == NULL)
    error ("Task ID %d not known.  Use the \"info tasks\" command to\n"
           "see the IDs of currently known tasks", num);

  if (switch_to_task (e) == 0)
    {
      ada_find_printable_frame (get_selected_frame ());
      printf_filtered ("[Switching to task %d]\n", num);
      print_stack_frame (get_selected_frame (),
                         frame_relative_level (get_selected_frame ()), 1);
    }
  else
    printf_filtered ("Unable to switch to task %d\n", num);
}

/* Switch to task indicated in TIDSTR.  Simply print the current task
   if TIDSTR is empty or NULL.  */

static void
task_command (char *tidstr, int from_tty)
{
  const int task_list_built = build_task_list ();

  if (!task_list_built)
    return;

  if (tidstr == NULL || tidstr[0] == '\0')
    display_current_task_id ();
  else
    task_command_1 (tidstr, from_tty);
}

#if defined (__fsu__) || (defined(__alpha__) && defined(__osf__) && !defined(VXWORKS_TARGET))
/* Restore saved registers if there was a task switch.  */
void
ada_reset_thread_registers (void)
{
  if (current_task_id != -1 && SPECIAL_THREAD_SUPPORT_ACTIVE ())
    {
      supply_gregset (&gregset_saved);
      supply_fpregset (&fpregset_saved);
      reinit_frame_cache ();
      stop_pc = read_pc ();
    }
  current_task_id = -1;
}
#else
void
ada_reset_thread_registers (void)
{
}
#endif

/* The 'normal_stop' observer notification callback.  */

static void
normal_stop_notification (void)
{
  /* The inferior has been resumed, and just stopped. This means that
     our task_list needs to be recomputed before it can be used again.  */
  stale_task_list_p = 1;
}

/* Attach all the observers needed by the ada-tasks module.  */

static void
ada_tasks_attach_observers (void)
{
  observer_attach_normal_stop (&normal_stop_notification);
}

void
_initialize_tasks (void)
{
#ifdef GNAT_GDB
  extern struct cmd_list_element *cmdlist;

  ada_tasks_attach_observers ();

  add_info ("tasks", info_tasks_command,
            "Without argument: list all known Ada tasks, with status information.\n"
            "info tasks n: print detailed information of task n.");

  add_cmd ("task", class_run, task_command,
           "Without argument: print the current task ID.\n"
           "task n: Use this command to switch to task n.",
           &cmdlist);
#endif
}
