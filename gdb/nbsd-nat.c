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
#include "inferior.h"

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
