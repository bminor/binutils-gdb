/* Native-dependent code for NetBSD.

   Copyright (C) 2006-2020 Free Software Foundation, Inc.

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

#include "defs.h"

#include "nbsd-nat.h"
#include "gdbthread.h"
#include "nbsd-tdep.h"
#include "inferior.h"
#include "gdbarch.h"

#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/sysctl.h>

/* Return the name of a file that can be opened to get the symbols for
   the child process identified by PID.  */

char *
nbsd_nat_target::pid_to_exec_file (int pid)
{
  static char buf[PATH_MAX];
  size_t buflen;
  int mib[4] = {CTL_KERN, KERN_PROC_ARGS, pid, KERN_PROC_PATHNAME};
  buflen = sizeof (buf);
  if (sysctl (mib, ARRAY_SIZE (mib), buf, &buflen, NULL, 0))
    return NULL;
  return buf;
}

/* Return the current directory for the process identified by PID.  */

static std::string
nbsd_pid_to_cwd (int pid)
{
  char buf[PATH_MAX];
  size_t buflen;
  int mib[4] = {CTL_KERN, KERN_PROC_ARGS, pid, KERN_PROC_CWD};
  buflen = sizeof (buf);
  if (sysctl (mib, ARRAY_SIZE (mib), buf, &buflen, NULL, 0))
    return "";
  return buf;
}

/* Return the command line for the process identified by PID.  */

static gdb::unique_xmalloc_ptr<char[]>
nbsd_pid_to_cmdline (int pid)
{
  int mib[4] = {CTL_KERN, KERN_PROC_ARGS, pid, KERN_PROC_ARGV};

  size_t size = 0;
  if (sysctl (mib, ARRAY_SIZE (mib), NULL, &size, NULL, 0) == -1 || size == 0)
    return nullptr;

  gdb::unique_xmalloc_ptr<char[]> args (XNEWVAR (char, size));

  if (sysctl (mib, ARRAY_SIZE (mib), args.get (), &size, NULL, 0) == -1
      || size == 0)
    return nullptr;

  /* Arguments are returned as a flattened string with NUL separators.
     Join the arguments with spaces to form a single string.  */
  for (size_t i = 0; i < size - 1; i++)
    if (args[i] == '\0')
      args[i] = ' ';
  args[size - 1] = '\0';

  return args;
}

/* Generic thread (LWP) lister within a specified process.  The callback
   parameters is a C++ function that is called for each detected thread.  */

static bool
nbsd_thread_lister (const pid_t pid,
		    gdb::function_view<bool (const struct kinfo_lwp *)>
		    callback)
{
  int mib[5] = {CTL_KERN, KERN_LWP, pid, sizeof (struct kinfo_lwp), 0};
  size_t size;

  if (sysctl (mib, ARRAY_SIZE (mib), NULL, &size, NULL, 0) == -1 || size == 0)
    perror_with_name (("sysctl"));

  mib[4] = size / sizeof (size_t);

  gdb::unique_xmalloc_ptr<struct kinfo_lwp[]> kl
    ((struct kinfo_lwp *) xcalloc (size, 1));

  if (sysctl (mib, ARRAY_SIZE (mib), kl.get (), &size, NULL, 0) == -1
      || size == 0)
    perror_with_name (("sysctl"));

  for (size_t i = 0; i < size / sizeof (struct kinfo_lwp); i++)
    {
      struct kinfo_lwp *l = &kl[i];

      /* Return true if the specified thread is alive.  */
      auto lwp_alive
	= [] (struct kinfo_lwp *lwp)
	  {
	    switch (lwp->l_stat)
	      {
	      case LSSLEEP:
	      case LSRUN:
	      case LSONPROC:
	      case LSSTOP:
	      case LSSUSPENDED:
		return true;
	      default:
		return false;
	      }
	  };

      /* Ignore embryonic or demised threads.  */
      if (!lwp_alive (l))
	continue;

      if (callback (l))
	return true;
    }

  return false;
}

/* Return true if PTID is still active in the inferior.  */

bool
nbsd_nat_target::thread_alive (ptid_t ptid)
{
  pid_t pid = ptid.pid ();
  int lwp = ptid.lwp ();

  auto fn
    = [&lwp] (const struct kinfo_lwp *kl)
      {
        return kl->l_lid == lwp;
      };

  return nbsd_thread_lister (pid, fn);
}

/* Return the name assigned to a thread by an application.  Returns
   the string in a static buffer.  */

const char *
nbsd_nat_target::thread_name (struct thread_info *thr)
{
  ptid_t ptid = thr->ptid;
  pid_t pid = ptid.pid ();
  int lwp = ptid.lwp ();

  static char buf[KI_LNAMELEN] = {};

  auto fn
    = [&lwp] (const struct kinfo_lwp *kl)
      {
	if (kl->l_lid == lwp)
	  {
	    xsnprintf (buf, sizeof buf, "%s", kl->l_name);
	    return true;
	  }
	return false;
      };

  if (nbsd_thread_lister (pid, fn))
    return buf;
  else
    return NULL;
}

/* Implement the "post_attach" target_ops method.  */

static void
nbsd_add_threads (nbsd_nat_target *target, pid_t pid)
{
  auto fn
    = [&target, &pid] (const struct kinfo_lwp *kl)
      {
	ptid_t ptid = ptid_t (pid, kl->l_lid, 0);
	if (!in_thread_list (target, ptid))
	  {
	    if (inferior_ptid.lwp () == 0)
	      thread_change_ptid (target, inferior_ptid, ptid);
	    else
	      add_thread (target, ptid);
	  }
	return false;
      };

  nbsd_thread_lister (pid, fn);
}

/* Implement the "post_attach" target_ops method.  */

void
nbsd_nat_target::post_attach (int pid)
{
  nbsd_add_threads (this, pid);
}

/* Implement the "update_thread_list" target_ops method.  */

void
nbsd_nat_target::update_thread_list ()
{
  prune_threads ();

  nbsd_add_threads (this, inferior_ptid.pid ());
}

/* Convert PTID to a string.  */

std::string
nbsd_nat_target::pid_to_str (ptid_t ptid)
{
  int lwp = ptid.lwp ();

  if (lwp != 0)
    {
      pid_t pid = ptid.pid ();

      return string_printf ("LWP %d of process %d", lwp, pid);
    }

  return normal_pid_to_str (ptid);
}

/* Retrieve all the memory regions in the specified process.  */

static gdb::unique_xmalloc_ptr<struct kinfo_vmentry[]>
nbsd_kinfo_get_vmmap (pid_t pid, size_t *size)
{
  int mib[5] = {CTL_VM, VM_PROC, VM_PROC_MAP, pid,
		sizeof (struct kinfo_vmentry)};

  size_t length = 0;
  if (sysctl (mib, ARRAY_SIZE (mib), NULL, &length, NULL, 0))
    {
      *size = 0;
      return NULL;
    }

  /* Prereserve more space.  The length argument is volatile and can change
     between the sysctl(3) calls as this function can be called against a
     running process.  */
  length = length * 5 / 3;

  gdb::unique_xmalloc_ptr<struct kinfo_vmentry[]> kiv
    (XNEWVAR (kinfo_vmentry, length));

  if (sysctl (mib, ARRAY_SIZE (mib), kiv.get (), &length, NULL, 0))
    {
      *size = 0;
      return NULL;
    }

  *size = length / sizeof (struct kinfo_vmentry);
  return kiv;
}

/* Iterate over all the memory regions in the current inferior,
   calling FUNC for each memory region.  OBFD is passed as the last
   argument to FUNC.  */

int
nbsd_nat_target::find_memory_regions (find_memory_region_ftype func,
				      void *data)
{
  pid_t pid = inferior_ptid.pid ();

  size_t nitems;
  gdb::unique_xmalloc_ptr<struct kinfo_vmentry[]> vmentl
    = nbsd_kinfo_get_vmmap (pid, &nitems);
  if (vmentl == NULL)
    perror_with_name (_("Couldn't fetch VM map entries."));

  for (size_t i = 0; i < nitems; i++)
    {
      struct kinfo_vmentry *kve = &vmentl[i];

      /* Skip unreadable segments and those where MAP_NOCORE has been set.  */
      if (!(kve->kve_protection & KVME_PROT_READ)
	  || kve->kve_flags & KVME_FLAG_NOCOREDUMP)
	continue;

      /* Skip segments with an invalid type.  */
      switch (kve->kve_type)
	{
	case KVME_TYPE_VNODE:
	case KVME_TYPE_ANON:
	case KVME_TYPE_SUBMAP:
	case KVME_TYPE_OBJECT:
	  break;
	default:
	  continue;
	}

      size_t size = kve->kve_end - kve->kve_start;
      if (info_verbose)
	{
	  fprintf_filtered (gdb_stdout,
			    "Save segment, %ld bytes at %s (%c%c%c)\n",
			    (long) size,
			    paddress (target_gdbarch (), kve->kve_start),
			    kve->kve_protection & KVME_PROT_READ ? 'r' : '-',
			    kve->kve_protection & KVME_PROT_WRITE ? 'w' : '-',
			    kve->kve_protection & KVME_PROT_EXEC ? 'x' : '-');
	}

      /* Invoke the callback function to create the corefile segment.
	 Pass MODIFIED as true, we do not know the real modification state.  */
      func (kve->kve_start, size, kve->kve_protection & KVME_PROT_READ,
	    kve->kve_protection & KVME_PROT_WRITE,
	    kve->kve_protection & KVME_PROT_EXEC, 1, data);
    }
  return 0;
}

/* Implement the "info_proc" target_ops method.  */

bool
nbsd_nat_target::info_proc (const char *args, enum info_proc_what what)
{
  pid_t pid;
  bool do_cmdline = false;
  bool do_cwd = false;
  bool do_exe = false;
  bool do_mappings = false;

  switch (what)
    {
    case IP_MAPPINGS:
      do_mappings = true;
      break;
    case IP_CMDLINE:
      do_cmdline = true;
      break;
    case IP_EXE:
      do_exe = true;
      break;
    case IP_CWD:
      do_cwd = true;
      break;
    default:
      error (_("Not supported on this target."));
    }

  gdb_argv built_argv (args);
  if (built_argv.count () == 0)
    {
      pid = inferior_ptid.pid ();
      if (pid == 0)
        error (_("No current process: you must name one."));
    }
  else if (built_argv.count () == 1 && isdigit (built_argv[0][0]))
    pid = strtol (built_argv[0], NULL, 10);
  else
    error (_("Invalid arguments."));

  printf_filtered (_("process %d\n"), pid);

  if (do_cmdline)
    {
      gdb::unique_xmalloc_ptr<char[]> cmdline = nbsd_pid_to_cmdline (pid);
      if (cmdline != nullptr)
	printf_filtered ("cmdline = '%s'\n", cmdline.get ());
      else
	warning (_("unable to fetch command line"));
    }
  if (do_cwd)
    {
      std::string cwd = nbsd_pid_to_cwd (pid);
      if (cwd != "")
	printf_filtered ("cwd = '%s'\n", cwd.c_str ());
      else
	warning (_("unable to fetch current working directory"));
    }
  if (do_exe)
    {
      const char *exe = pid_to_exec_file (pid);
      if (exe != nullptr)
	printf_filtered ("exe = '%s'\n", exe);
      else
	warning (_("unable to fetch executable path name"));
    }
  if (do_mappings)
    {
      size_t nvment;
      gdb::unique_xmalloc_ptr<struct kinfo_vmentry[]> vmentl
	= nbsd_kinfo_get_vmmap (pid, &nvment);

      if (vmentl != nullptr)
	{
	  int addr_bit = TARGET_CHAR_BIT * sizeof (void *);
	  nbsd_info_proc_mappings_header (addr_bit);

	  struct kinfo_vmentry *kve = vmentl.get ();
	  for (int i = 0; i < nvment; i++, kve++)
	    nbsd_info_proc_mappings_entry (addr_bit, kve->kve_start,
					   kve->kve_end, kve->kve_offset,
					   kve->kve_flags, kve->kve_protection,
					   kve->kve_path);
	}
      else
	warning (_("unable to fetch virtual memory map"));
    }

  return true;
}
