/* Serial interface for a pipe to a separate program
   Copyright 1999 Free Software Foundation, Inc.

   Contributed by Cygnus Solutions.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "serial.h"
#include <sys/types.h>
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "signals.h"
#include "gdb_string.h"

extern int (*ui_loop_hook) PARAMS ((int));

static int pipe_open PARAMS ((serial_t scb, const char *name));
static void pipe_raw PARAMS ((serial_t scb));
static int wait_for PARAMS ((serial_t scb, int timeout));
static int pipe_readchar PARAMS ((serial_t scb, int timeout));
static int pipe_setbaudrate PARAMS ((serial_t scb, int rate));
static int pipe_setstopbits PARAMS ((serial_t scb, int num));
static int pipe_write PARAMS ((serial_t scb, const char *str, int len));
/* FIXME: static void pipe_restore PARAMS ((serial_t scb)); */
static void pipe_close PARAMS ((serial_t scb));
static serial_ttystate pipe_get_tty_state PARAMS ((serial_t scb));
static int pipe_set_tty_state PARAMS ((serial_t scb, serial_ttystate state));
static int pipe_return_0 PARAMS ((serial_t));
static int pipe_noflush_set_tty_state PARAMS ((serial_t, serial_ttystate,
					       serial_ttystate));
static void pipe_print_tty_state PARAMS ((serial_t, serial_ttystate));

extern void _initialize_ser_pipe PARAMS ((void));

#undef XMALLOC
#define XMALLOC(T) ((T*) xmalloc (sizeof (T)))


struct pipe_state
  {
    int pid;
  };

/* Open up a raw pipe */

static int
pipe_open (scb, name)
     serial_t scb;
     const char *name;
{
#if !defined(O_NONBLOCK) || !defined(F_GETFL) || !defined(F_SETFL) || !HAVE_SOCKETPAIR
  return -1;
#else
  struct pipe_state *state;
  /* This chunk: */
  /* Copyright (c) 1988, 1993
   *      The Regents of the University of California.  All rights reserved.
   *
   * This code is derived from software written by Ken Arnold and
   * published in UNIX Review, Vol. 6, No. 8.
   */
  int pdes[2];
  int pid;
  if (socketpair (AF_UNIX, SOCK_STREAM, 0, pdes) < 0)
    return -1;

  pid = vfork ();
  
  /* Error. */
  if (pid == -1)
    {
      close (pdes[0]);
      close (pdes[1]);
      return -1;
    }

  /* Child. */
  if (pid == 0)
    {
      /* re-wire pdes[1] to stdin/stdout */
      close (pdes[0]);
      if (pdes[1] != STDOUT_FILENO)
	{
	  dup2 (pdes[1], STDOUT_FILENO);
	  close (pdes[1]);
	}
      dup2 (STDOUT_FILENO, STDIN_FILENO);
#if 0
      /* close any stray FD's - FIXME - how? */
      /* POSIX.2 B.3.2.2 "popen() shall ensure that any streams
         from previous popen() calls that remain open in the 
         parent process are closed in the new child process. */
      for (old = pidlist; old; old = old->next)
	close (fileno (old->fp));	/* don't allow a flush */
#endif
      execl ("/bin/sh", "sh", "-c", name + 1, NULL);
      _exit (127);
    }

  /* Parent. */
  close (pdes[1]);
  /* :end chunk */
  state = XMALLOC (struct pipe_state);
  state->pid = pid;
  scb->fd = pdes[0];
  scb->state = state;

  /* Make it non-blocking */
  {
    int flags = fcntl (scb->fd, F_GETFL, 0);
    if (fcntl (scb->fd, F_SETFL, flags | O_NONBLOCK) < 0)
      {
	perror ("ser-pipe");
	pipe_close (scb);
	return -1;
      }
  }

  /* If we don't do this, GDB simply exits when the remote side dies.  */
  signal (SIGPIPE, SIG_IGN);
  return 0;
#endif
}

static serial_ttystate
pipe_get_tty_state (scb)
     serial_t scb;
{
  /* return garbage */
  return xmalloc (sizeof (int));
}

static int
pipe_set_tty_state (scb, ttystate)
     serial_t scb;
     serial_ttystate ttystate;
{
  return 0;
}

static int
pipe_return_0 (scb)
     serial_t scb;
{
  return 0;
}

static void
pipe_raw (scb)
     serial_t scb;
{
  return;			/* Always in raw mode */
}

/* Wait for input on scb, with timeout seconds.  Returns 0 on success,
   otherwise SERIAL_TIMEOUT or SERIAL_ERROR.

   For termio{s}, we actually just setup VTIME if necessary, and let the
   timeout occur in the read() in pipe_read().
 */

static int
wait_for (scb, timeout)
     serial_t scb;
     int timeout;
{
  int numfds;
  struct timeval tv;
  fd_set readfds, exceptfds;

  FD_ZERO (&readfds);
  FD_ZERO (&exceptfds);

  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  FD_SET (scb->fd, &readfds);
  FD_SET (scb->fd, &exceptfds);

  while (1)
    {
      if (timeout >= 0)
	numfds = select (scb->fd + 1, &readfds, 0, &exceptfds, &tv);
      else
	numfds = select (scb->fd + 1, &readfds, 0, &exceptfds, 0);

      if (numfds <= 0)
	{
	  if (numfds == 0)
	    return SERIAL_TIMEOUT;
	  else if (errno == EINTR)
	    continue;
	  else
	    return SERIAL_ERROR;	/* Got an error from select or poll */
	}

      return 0;
    }
}

/* Read a character with user-specified timeout.  TIMEOUT is number of seconds
   to wait, or -1 to wait forever.  Use timeout of 0 to effect a poll.  Returns
   char if successful.  Returns -2 if timeout expired, EOF if line dropped
   dead, or -3 for any other error (see errno in that case). */

static int
pipe_readchar (scb, timeout)
     serial_t scb;
     int timeout;
{
  int status;
  int delta;

  if (scb->bufcnt-- > 0)
    return *scb->bufp++;

  /* We have to be able to keep the GUI alive here, so we break the original
     timeout into steps of 1 second, running the "keep the GUI alive" hook 
     each time through the loop.

     Also, timeout = 0 means to poll, so we just set the delta to 0, so we
     will only go through the loop once. */

  delta = (timeout == 0 ? 0 : 1);
  while (1)
    {

      /* N.B. The UI may destroy our world (for instance by calling
         remote_stop,) in which case we want to get out of here as
         quickly as possible.  It is not safe to touch scb, since
         someone else might have freed it.  The ui_loop_hook signals that 
         we should exit by returning 1. */

      if (ui_loop_hook)
	{
	  if (ui_loop_hook (0))
	    return SERIAL_TIMEOUT;
	}

      status = wait_for (scb, delta);
      timeout -= delta;

      /* If we got a character or an error back from wait_for, then we can 
         break from the loop before the timeout is completed. */

      if (status != SERIAL_TIMEOUT)
	{
	  break;
	}

      /* If we have exhausted the original timeout, then generate
         a SERIAL_TIMEOUT, and pass it out of the loop. */

      else if (timeout == 0)
	{
	  status == SERIAL_TIMEOUT;
	  break;
	}
    }

  if (status < 0)
    return status;

  while (1)
    {
      scb->bufcnt = read (scb->fd, scb->buf, BUFSIZ);
      if (scb->bufcnt != -1 || errno != EINTR)
	break;
    }

  if (scb->bufcnt <= 0)
    {
      if (scb->bufcnt == 0)
	return SERIAL_TIMEOUT;	/* 0 chars means timeout [may need to
				   distinguish between EOF & timeouts
				   someday] */
      else
	return SERIAL_ERROR;	/* Got an error from read */
    }

  scb->bufcnt--;
  scb->bufp = scb->buf;
  return *scb->bufp++;
}

static int
pipe_noflush_set_tty_state (scb, new_ttystate, old_ttystate)
     serial_t scb;
     serial_ttystate new_ttystate;
     serial_ttystate old_ttystate;
{
  return 0;
}

static void
pipe_print_tty_state (scb, ttystate)
     serial_t scb;
     serial_ttystate ttystate;
{
  /* Nothing to print.  */
  return;
}

static int
pipe_setbaudrate (scb, rate)
     serial_t scb;
     int rate;
{
  return 0;			/* Never fails! */
}

static int
pipe_setstopbits (scb, num)
     serial_t scb;
     int num;
{
  return 0;			/* Never fails! */
}

static int
pipe_write (scb, str, len)
     serial_t scb;
     const char *str;
     int len;
{
  int cc;

  while (len > 0)
    {
      cc = write (scb->fd, str, len);

      if (cc < 0)
	return 1;
      len -= cc;
      str += cc;
    }
  return 0;
}

static void
pipe_close (scb)
     serial_t scb;
{
  struct pipe_state *state = scb->state;
  if (state != NULL)
    {
      int pid = state->pid;
      close (scb->fd);
      scb->fd = -1;
      free (state);
      scb->state = NULL;
      kill (pid, SIGTERM);
      /* Might be useful to check that the child does die. */
    }
}

static struct serial_ops pipe_ops =
{
  "pipe",
  0,
  pipe_open,
  pipe_close,
  pipe_readchar,
  pipe_write,
  pipe_return_0,		/* flush output */
  pipe_return_0,		/* flush input */
  pipe_return_0,		/* send break */
  pipe_raw,
  pipe_get_tty_state,
  pipe_set_tty_state,
  pipe_print_tty_state,
  pipe_noflush_set_tty_state,
  pipe_setbaudrate,
  pipe_setstopbits,
  pipe_return_0,		/* wait for output to drain */
};

void
_initialize_ser_pipe ()
{
  serial_add_interface (&pipe_ops);
}
