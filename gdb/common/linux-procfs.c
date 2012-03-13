/* Linux-specific PROCFS manipulation routines.
   Copyright (C) 2009-2012 Free Software Foundation, Inc.

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

#ifdef GDBSERVER
#include "server.h"
#else
#include "defs.h"
#include "gdb_string.h"
#endif

#include "linux-procfs.h"

/* Return the TGID of LWPID from /proc/pid/status.  Returns -1 if not
   found.  */

int
linux_proc_get_tgid (int lwpid)
{
  FILE *status_file;
  char buf[100];
  int tgid = -1;

  snprintf (buf, sizeof (buf), "/proc/%d/status", (int) lwpid);
  status_file = fopen (buf, "r");
  if (status_file != NULL)
    {
      while (fgets (buf, sizeof (buf), status_file))
	{
	  if (strncmp (buf, "Tgid:", 5) == 0)
	    {
	      tgid = strtoul (buf + strlen ("Tgid:"), NULL, 10);
	      break;
	    }
	}

      fclose (status_file);
    }

  return tgid;
}

/* Detect `T (stopped)' in `/proc/PID/status'.
   Other states including `T (tracing stop)' are reported as false.  */

int
linux_proc_pid_is_stopped (pid_t pid)
{
  FILE *status_file;
  char buf[100];
  int retval = 0;

  snprintf (buf, sizeof (buf), "/proc/%d/status", (int) pid);
  status_file = fopen (buf, "r");
  if (status_file != NULL)
    {
      int have_state = 0;

      while (fgets (buf, sizeof (buf), status_file))
	{
	  if (strncmp (buf, "State:", 6) == 0)
	    {
	      have_state = 1;
	      break;
	    }
	}
      if (have_state && strstr (buf, "T (stopped)") != NULL)
	retval = 1;
      fclose (status_file);
    }
  return retval;
}

/* See linux-procfs.h declaration.  */

int
linux_proc_pid_is_zombie (pid_t pid)
{
  char buffer[100];
  FILE *procfile;
  int retval;
  int have_state;

  xsnprintf (buffer, sizeof (buffer), "/proc/%d/status", (int) pid);
  procfile = fopen (buffer, "r");
  if (procfile == NULL)
    {
      warning (_("unable to open /proc file '%s'"), buffer);
      return 0;
    }

  have_state = 0;
  while (fgets (buffer, sizeof (buffer), procfile) != NULL)
    if (strncmp (buffer, "State:", 6) == 0)
      {
	have_state = 1;
	break;
      }
  retval = (have_state
	    && strcmp (buffer, "State:\tZ (zombie)\n") == 0);
  fclose (procfile);
  return retval;
}
