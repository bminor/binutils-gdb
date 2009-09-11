/* Remote File-I/O communications

   Copyright (C) 2003, 2005, 2006, 2007, 2008 Free Software Foundation, Inc.

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

/* See the GDB User Guide for details of the GDB remote protocol. */

#include "defs.h"
#include "gdb_string.h"
#include "gdbcmd.h"
#include "remote.h"
#include "gdb/fileio.h"
#include "exceptions.h"
#include "remote-fileio.h"
#include "target-fileio.h"

#include <signal.h>


static int remote_fio_ctrl_c_flag = 0;

#if defined (HAVE_SIGACTION) && defined (SA_RESTART)
static struct sigaction remote_fio_sa;
static struct sigaction remote_fio_osa;
#else
static void (*remote_fio_ofunc)(int);
#endif

static void
remote_fileio_sig_init (void)
{
#if defined (HAVE_SIGACTION) && defined (SA_RESTART)
  remote_fio_sa.sa_handler = SIG_IGN;
  sigemptyset (&remote_fio_sa.sa_mask);
  remote_fio_sa.sa_flags = 0;
  sigaction (SIGINT, &remote_fio_sa, &remote_fio_osa);
#else
  remote_fio_ofunc = signal (SIGINT, SIG_IGN);
#endif
}

static void
remote_fileio_sig_set (void (*sigint_func)(int))
{
#if defined (HAVE_SIGACTION) && defined (SA_RESTART)
  remote_fio_sa.sa_handler = sigint_func;
  sigemptyset (&remote_fio_sa.sa_mask);
  remote_fio_sa.sa_flags = 0;
  sigaction (SIGINT, &remote_fio_sa, NULL);
#else
  signal (SIGINT, sigint_func);
#endif
}

static void
remote_fileio_sig_exit (void)
{
#if defined (HAVE_SIGACTION) && defined (SA_RESTART)
  sigaction (SIGINT, &remote_fio_osa, NULL);
#else
  signal (SIGINT, remote_fio_ofunc);
#endif
}

static void
remote_fileio_ctrl_c_signal_handler (int signo)
{
  remote_fileio_sig_set (SIG_IGN);
  remote_fio_ctrl_c_flag = 1;
  if (!target_fio_no_longjmp)
    deprecated_throw_reason (RETURN_QUIT);
  remote_fileio_sig_set (remote_fileio_ctrl_c_signal_handler);
}

static void
remote_fileio_reply (int retcode, int error)
{
  char buf[32];

  remote_fileio_sig_set (SIG_IGN);
  strcpy (buf, "F");
  if (retcode < 0)
    {
      strcat (buf, "-");
      retcode = -retcode;
    }
  sprintf (buf + strlen (buf), "%x", retcode);
  if (error || remote_fio_ctrl_c_flag)
    {
      if (error && remote_fio_ctrl_c_flag)
        error = FILEIO_EINTR;
      if (error < 0)
        {
	  strcat (buf, "-");
	  error = -error;
	}
      sprintf (buf + strlen (buf), ",%x", error);
      if (remote_fio_ctrl_c_flag)
        strcat (buf, ",C");
    }
  remote_fileio_sig_set (remote_fileio_ctrl_c_signal_handler);
  putpkt (buf);
}


/* Wrapper function for remote_write_bytes() which has the disadvantage to
   write only one packet, regardless of the requested number of bytes to
   transfer.  This wrapper calls remote_write_bytes() as often as needed. */
static int
remote_fileio_write_bytes (CORE_ADDR memaddr, gdb_byte *myaddr, int len)
{
  int ret = 0, written;

  while (len > 0 && (written = remote_write_bytes (memaddr, myaddr, len)) > 0)
    {
      len -= written;
      memaddr += written;
      myaddr += written;
      ret += written;
    }
  return ret;
}


static void
set_ctrl_c_signal_handler(void)
{
  remote_fileio_sig_set (remote_fileio_ctrl_c_signal_handler);
}

/* Close any open descriptors, and reinitialize the file mapping.  */

void
remote_fileio_reset (void)
{
  target_fileio_reset();
}

void
remote_fileio_request (char *buf)
{
  static struct file_io_operations operations =
  {
     remote_read_bytes,
     remote_fileio_write_bytes,
     remote_fileio_reply,
     set_ctrl_c_signal_handler
  };

  remote_fileio_sig_init ();
  target_fileio_request(buf, &operations);
  remote_fileio_sig_exit ();
}

void
initialize_remote_fileio (struct cmd_list_element *remote_set_cmdlist,
			  struct cmd_list_element *remote_show_cmdlist)
{
  initialize_target_fileio(remote_set_cmdlist, remote_show_cmdlist);
}
