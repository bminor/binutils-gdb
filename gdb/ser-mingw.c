/* Serial interface for local (hardwired) serial ports on Windows systems

   Copyright (C) 2006
   Free Software Foundation, Inc.

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

#include "defs.h"
#include "serial.h"
#include "ser-base.h"
#include "ser-tcp.h"

#include <windows.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#include "gdb_assert.h"
#include "gdb_string.h"

void _initialize_ser_windows (void);

struct ser_windows_state
{
  int in_progress;
  OVERLAPPED ov;
  DWORD lastCommMask;
  HANDLE except_event;
};

/* Open up a real live device for serial I/O.  */

static int
ser_windows_open (struct serial *scb, const char *name)
{
  HANDLE h;
  struct ser_windows_state *state;
  COMMTIMEOUTS timeouts;

  /* Only allow COM ports.  */
  if (strncmp (name, "COM", 3) != 0)
    {
      errno = ENOENT;
      return -1;
    }

  h = CreateFile (name, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		  OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
  if (h == INVALID_HANDLE_VALUE)
    {
      errno = ENOENT;
      return -1;
    }

  scb->fd = _open_osfhandle ((long) h, O_RDWR);
  if (scb->fd < 0)
    {
      errno = ENOENT;
      return -1;
    }

  if (!SetCommMask (h, EV_RXCHAR))
    {
      errno = EINVAL;
      return -1;
    }

  timeouts.ReadIntervalTimeout = MAXDWORD;
  timeouts.ReadTotalTimeoutConstant = 0;
  timeouts.ReadTotalTimeoutMultiplier = 0;
  timeouts.WriteTotalTimeoutConstant = 0;
  timeouts.WriteTotalTimeoutMultiplier = 0;
  if (!SetCommTimeouts (h, &timeouts))
    {
      errno = EINVAL;
      return -1;
    }

  state = xmalloc (sizeof (struct ser_windows_state));
  memset (state, 0, sizeof (struct ser_windows_state));
  scb->state = state;

  /* Create a manual reset event to watch the input buffer.  */
  state->ov.hEvent = CreateEvent (0, TRUE, FALSE, 0);

  /* Create a (currently unused) handle to record exceptions.  */
  state->except_event = CreateEvent (0, TRUE, FALSE, 0);

  return 0;
}

/* Wait for the output to drain away, as opposed to flushing (discarding)
   it.  */

static int
ser_windows_drain_output (struct serial *scb)
{
  HANDLE h = (HANDLE) _get_osfhandle (scb->fd);

  return (FlushFileBuffers (h) != 0) ? 0 : -1;
}

static int
ser_windows_flush_output (struct serial *scb)
{
  HANDLE h = (HANDLE) _get_osfhandle (scb->fd);

  return (PurgeComm (h, PURGE_TXCLEAR) != 0) ? 0 : -1;
}

static int
ser_windows_flush_input (struct serial *scb)
{
  HANDLE h = (HANDLE) _get_osfhandle (scb->fd);

  return (PurgeComm (h, PURGE_RXCLEAR) != 0) ? 0 : -1;
}

static int
ser_windows_send_break (struct serial *scb)
{
  HANDLE h = (HANDLE) _get_osfhandle (scb->fd);

  if (SetCommBreak (h) == 0)
    return -1;

  /* Delay for 250 milliseconds.  */
  Sleep (250);

  if (ClearCommBreak (h))
    return -1;

  return 0;
}

static void
ser_windows_raw (struct serial *scb)
{
  HANDLE h = (HANDLE) _get_osfhandle (scb->fd);
  DCB state;

  if (GetCommState (h, &state) == 0)
    return;

  state.fParity = FALSE;
  state.fOutxCtsFlow = FALSE;
  state.fOutxDsrFlow = FALSE;
  state.fDtrControl = DTR_CONTROL_ENABLE;
  state.fDsrSensitivity = FALSE;
  state.fOutX = FALSE;
  state.fInX = FALSE;
  state.fNull = FALSE;
  state.fAbortOnError = FALSE;
  state.ByteSize = 8;
  state.Parity = NOPARITY;

  scb->current_timeout = 0;

  if (SetCommState (h, &state) == 0)
    warning (_("SetCommState failed\n"));
}

static int
ser_windows_setstopbits (struct serial *scb, int num)
{
  HANDLE h = (HANDLE) _get_osfhandle (scb->fd);
  DCB state;

  if (GetCommState (h, &state) == 0)
    return -1;

  switch (num)
    {
    case SERIAL_1_STOPBITS:
      state.StopBits = ONESTOPBIT;
      break;
    case SERIAL_1_AND_A_HALF_STOPBITS:
      state.StopBits = ONE5STOPBITS;
      break;
    case SERIAL_2_STOPBITS:
      state.StopBits = TWOSTOPBITS;
      break;
    default:
      return 1;
    }

  return (SetCommState (h, &state) != 0) ? 0 : -1;
}

static int
ser_windows_setbaudrate (struct serial *scb, int rate)
{
  HANDLE h = (HANDLE) _get_osfhandle (scb->fd);
  DCB state;

  if (GetCommState (h, &state) == 0)
    return -1;

  state.BaudRate = rate;

  return (SetCommState (h, &state) != 0) ? 0 : -1;
}

static void
ser_windows_close (struct serial *scb)
{
  struct ser_windows_state *state;

  /* Stop any pending selects.  */
  CancelIo ((HANDLE) _get_osfhandle (scb->fd));
  state = scb->state;
  CloseHandle (state->ov.hEvent);
  CloseHandle (state->except_event);

  if (scb->fd < 0)
    return;

  close (scb->fd);
  scb->fd = -1;

  xfree (scb->state);
}

static void
ser_windows_wait_handle (struct serial *scb, HANDLE *read, HANDLE *except)
{
  struct ser_windows_state *state;
  COMSTAT status;
  DWORD errors;
  HANDLE h = (HANDLE) _get_osfhandle (scb->fd);

  state = scb->state;

  *except = state->except_event;
  *read = state->ov.hEvent;

  if (state->in_progress)
    return;

  /* Reset the mask - we are only interested in any characters which
     arrive after this point, not characters which might have arrived
     and already been read.  */

  /* This really, really shouldn't be necessary - just the second one.
     But otherwise an internal flag for EV_RXCHAR does not get
     cleared, and we get a duplicated event, if the last batch
     of characters included at least two arriving close together.  */
  if (!SetCommMask (h, 0))
    warning (_("ser_windows_wait_handle: reseting mask failed"));

  if (!SetCommMask (h, EV_RXCHAR))
    warning (_("ser_windows_wait_handle: reseting mask failed (2)"));

  /* There's a potential race condition here; we must check cbInQue
     and not wait if that's nonzero.  */

  ClearCommError (h, &errors, &status);
  if (status.cbInQue > 0)
    {
      SetEvent (state->ov.hEvent);
      return;
    }

  state->in_progress = 1;
  ResetEvent (state->ov.hEvent);
  state->lastCommMask = -2;
  if (WaitCommEvent (h, &state->lastCommMask, &state->ov))
    {
      gdb_assert (state->lastCommMask & EV_RXCHAR);
      SetEvent (state->ov.hEvent);
    }
  else
    gdb_assert (GetLastError () == ERROR_IO_PENDING);
}

static int
ser_windows_read_prim (struct serial *scb, size_t count)
{
  struct ser_windows_state *state;
  OVERLAPPED ov;
  DWORD bytes_read, bytes_read_tmp;
  HANDLE h;
  gdb_byte *p;

  state = scb->state;
  if (state->in_progress)
    {
      WaitForSingleObject (state->ov.hEvent, INFINITE);
      state->in_progress = 0;
      ResetEvent (state->ov.hEvent);
    }

  memset (&ov, 0, sizeof (OVERLAPPED));
  ov.hEvent = CreateEvent (0, FALSE, FALSE, 0);
  h = (HANDLE) _get_osfhandle (scb->fd);

  if (!ReadFile (h, scb->buf, /* count */ 1, &bytes_read, &ov))
    {
      if (GetLastError () != ERROR_IO_PENDING
	  || !GetOverlappedResult (h, &ov, &bytes_read, TRUE))
	bytes_read = -1;
    }

  CloseHandle (ov.hEvent);
  return bytes_read;
}

static int
ser_windows_write_prim (struct serial *scb, const void *buf, size_t len)
{
  struct ser_windows_state *state;
  OVERLAPPED ov;
  DWORD bytes_written;
  HANDLE h;

  memset (&ov, 0, sizeof (OVERLAPPED));
  ov.hEvent = CreateEvent (0, FALSE, FALSE, 0);
  h = (HANDLE) _get_osfhandle (scb->fd);
  if (!WriteFile (h, buf, len, &bytes_written, &ov))
    {
      if (GetLastError () != ERROR_IO_PENDING
	  || !GetOverlappedResult (h, &ov, &bytes_written, TRUE))
	bytes_written = -1;
    }

  CloseHandle (ov.hEvent);
  return bytes_written;
}

struct ser_console_state
{
  HANDLE read_event;
  HANDLE except_event;

  HANDLE start_select;
  HANDLE stop_select;
};

static DWORD WINAPI
console_select_thread (void *arg)
{
  struct serial *scb = arg;
  struct ser_console_state *state, state_copy;
  int event_index, fd;
  HANDLE h;

  /* Copy useful information out of the control block, to make sure
     that we do not race with freeing it.  */
  state_copy = *(struct ser_console_state *) scb->state;
  state = &state_copy;
  fd = scb->fd;

  h = (HANDLE) _get_osfhandle (fd);

  while (1)
    {
      HANDLE wait_events[2];
      INPUT_RECORD record;
      DWORD n_records;

      wait_events[0] = state->start_select;
      wait_events[1] = state->stop_select;

      if (WaitForMultipleObjects (2, wait_events, FALSE, INFINITE) != WAIT_OBJECT_0)
	{
	  CloseHandle (state->stop_select);
	  return 0;
	}

    retry:
      wait_events[0] = state->stop_select;
      wait_events[1] = h;

      event_index = WaitForMultipleObjects (2, wait_events, FALSE, INFINITE);

      if (event_index == WAIT_OBJECT_0
	  || WaitForSingleObject (state->stop_select, 0) == WAIT_OBJECT_0)
	{
	  CloseHandle (state->stop_select);
	  return 0;
	}

      if (event_index != WAIT_OBJECT_0 + 1)
	{
	  /* Wait must have failed; assume an error has occured, e.g.
	     the handle has been closed.  */
	  SetEvent (state->except_event);
	  continue;
	}

      /* We've got a pending event on the console.  See if it's
	 of interest.  */
      if (!PeekConsoleInput (h, &record, 1, &n_records) || n_records != 1)
	{
	  /* Something went wrong.  Maybe the console is gone.  */
	  SetEvent (state->except_event);
	  continue;
	}

      if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown)
	{
	  /* This is really a keypress.  */
	  SetEvent (state->read_event);
	  continue;
	}

      /* Otherwise discard it and wait again.  */
      ReadConsoleInput (h, &record, 1, &n_records);
      goto retry;
    }
}

static int
fd_is_pipe (int fd)
{
  if (PeekNamedPipe ((HANDLE) _get_osfhandle (fd), NULL, 0, NULL, NULL, NULL))
    return 1;
  else
    return 0;
}

static DWORD WINAPI
pipe_select_thread (void *arg)
{
  struct serial *scb = arg;
  struct ser_console_state *state, state_copy;
  int event_index, fd;
  HANDLE h;

  /* Copy useful information out of the control block, to make sure
     that we do not race with freeing it.  */
  state_copy = *(struct ser_console_state *) scb->state;
  state = &state_copy;
  fd = scb->fd;

  h = (HANDLE) _get_osfhandle (fd);

  while (1)
    {
      HANDLE wait_events[2];
      DWORD n_avail;

      wait_events[0] = state->start_select;
      wait_events[1] = state->stop_select;

      if (WaitForMultipleObjects (2, wait_events, FALSE, INFINITE) != WAIT_OBJECT_0)
	{
	  CloseHandle (state->stop_select);
	  return 0;
	}

    retry:
      if (!PeekNamedPipe (h, NULL, 0, NULL, &n_avail, NULL))
	{
	  SetEvent (state->except_event);
	  continue;
	}

      if (n_avail > 0)
	{
	  SetEvent (state->read_event);
	  continue;
	}

      if (WaitForSingleObject (state->stop_select, 0) == WAIT_OBJECT_0)
	{
	  CloseHandle (state->stop_select);
	  return 0;
	}

      Sleep (10);
      goto retry;
    }
}

static void
ser_console_wait_handle (struct serial *scb, HANDLE *read, HANDLE *except)
{
  struct ser_console_state *state = scb->state;

  if (state == NULL)
    {
      DWORD threadId;
      int is_tty;

      is_tty = isatty (scb->fd);
      if (!is_tty && !fd_is_pipe (scb->fd))
	{
	  *read = NULL;
	  *except = NULL;
	  return;
	}

      state = xmalloc (sizeof (struct ser_console_state));
      memset (state, 0, sizeof (struct ser_console_state));
      scb->state = state;

      /* Create auto reset events to wake and terminate the select thread.  */
      state->start_select = CreateEvent (0, FALSE, FALSE, 0);
      state->stop_select = CreateEvent (0, FALSE, FALSE, 0);

      /* Create our own events to report read and exceptions separately.
	 The exception event is currently never used.  */
      state->read_event = CreateEvent (0, FALSE, FALSE, 0);
      state->except_event = CreateEvent (0, FALSE, FALSE, 0);

      /* And finally start the select thread.  */
      if (is_tty)
	CreateThread (NULL, 0, console_select_thread, scb, 0, &threadId);
      else
	CreateThread (NULL, 0, pipe_select_thread, scb, 0, &threadId);
    }

  ResetEvent (state->read_event);
  ResetEvent (state->except_event);

  SetEvent (state->start_select);

  *read = state->read_event;
  *except = state->except_event;
}

static void
ser_console_close (struct serial *scb)
{
  struct ser_console_state *state = scb->state;

  if (scb->state)
    {
      SetEvent (state->stop_select);

      CloseHandle (state->read_event);
      CloseHandle (state->except_event);

      xfree (scb->state);
    }
}

struct ser_console_ttystate
{
  int is_a_tty;
};

static serial_ttystate
ser_console_get_tty_state (struct serial *scb)
{
  if (isatty (scb->fd))
    {
      struct ser_console_ttystate *state;
      state = (struct ser_console_ttystate *) xmalloc (sizeof *state);
      state->is_a_tty = 1;
      return state;
    }
  else
    return NULL;
}

struct net_windows_state
{
  HANDLE read_event;
  HANDLE except_event;

  HANDLE start_select;
  HANDLE stop_select;
  HANDLE sock_event;
};

static DWORD WINAPI
net_windows_select_thread (void *arg)
{
  struct serial *scb = arg;
  struct net_windows_state *state, state_copy;
  int event_index, fd;

  /* Copy useful information out of the control block, to make sure
     that we do not race with freeing it.  */
  state_copy = *(struct net_windows_state *) scb->state;
  state = &state_copy;
  fd = scb->fd;

  while (1)
    {
      HANDLE wait_events[2];
      WSANETWORKEVENTS events;

      wait_events[0] = state->start_select;
      wait_events[1] = state->stop_select;

      if (WaitForMultipleObjects (2, wait_events, FALSE, INFINITE) != WAIT_OBJECT_0)
	{
	  CloseHandle (state->stop_select);
	  return 0;
	}

      wait_events[0] = state->stop_select;
      wait_events[1] = state->sock_event;

      event_index = WaitForMultipleObjects (2, wait_events, FALSE, INFINITE);

      if (event_index == WAIT_OBJECT_0
	  || WaitForSingleObject (state->stop_select, 0) == WAIT_OBJECT_0)
	{
	  CloseHandle (state->stop_select);
	  return 0;
	}

      if (event_index != WAIT_OBJECT_0 + 1)
	{
	  /* Some error has occured.  Assume that this is an error
	     condition.  */
	  SetEvent (state->except_event);
	  continue;
	}

      /* Enumerate the internal network events, and reset the object that
	 signalled us to catch the next event.  */
      WSAEnumNetworkEvents (fd, state->sock_event, &events);

      if (events.lNetworkEvents & FD_READ)
	SetEvent (state->read_event);

      if (events.lNetworkEvents & FD_CLOSE)
	SetEvent (state->except_event);
    }
}

static void
net_windows_wait_handle (struct serial *scb, HANDLE *read, HANDLE *except)
{
  struct net_windows_state *state = scb->state;

  ResetEvent (state->read_event);
  ResetEvent (state->except_event);

  SetEvent (state->start_select);

  *read = state->read_event;
  *except = state->except_event;
}

static int
net_windows_open (struct serial *scb, const char *name)
{
  struct net_windows_state *state;
  int ret;
  DWORD threadId;

  ret = net_open (scb, name);
  if (ret != 0)
    return ret;

  state = xmalloc (sizeof (struct net_windows_state));
  memset (state, 0, sizeof (struct net_windows_state));
  scb->state = state;

  /* Create auto reset events to wake and terminate the select thread.  */
  state->start_select = CreateEvent (0, FALSE, FALSE, 0);
  state->stop_select = CreateEvent (0, FALSE, FALSE, 0);

  /* Associate an event with the socket.  */
  state->sock_event = CreateEvent (0, TRUE, FALSE, 0);
  WSAEventSelect (scb->fd, state->sock_event, FD_READ | FD_CLOSE);

  /* Create our own events to report read and close separately.  */
  state->read_event = CreateEvent (0, FALSE, FALSE, 0);
  state->except_event = CreateEvent (0, FALSE, FALSE, 0);

  /* And finally start the select thread.  */
  CreateThread (NULL, 0, net_windows_select_thread, scb, 0, &threadId);

  return 0;
}


static void
net_windows_close (struct serial *scb)
{
  struct net_windows_state *state = scb->state;

  SetEvent (state->stop_select);

  CloseHandle (state->read_event);
  CloseHandle (state->except_event);
  CloseHandle (state->start_select);
  CloseHandle (state->sock_event);

  xfree (scb->state);

  net_close (scb);
}

void
_initialize_ser_windows (void)
{
  WSADATA wsa_data;
  struct serial_ops *ops;

  /* First register the serial port driver.  */

  ops = XMALLOC (struct serial_ops);
  memset (ops, 0, sizeof (struct serial_ops));
  ops->name = "hardwire";
  ops->next = 0;
  ops->open = ser_windows_open;
  ops->close = ser_windows_close;

  ops->flush_output = ser_windows_flush_output;
  ops->flush_input = ser_windows_flush_input;
  ops->send_break = ser_windows_send_break;

  /* These are only used for stdin; we do not need them for serial
     ports, so supply the standard dummies.  */
  ops->get_tty_state = ser_base_get_tty_state;
  ops->set_tty_state = ser_base_set_tty_state;
  ops->print_tty_state = ser_base_print_tty_state;
  ops->noflush_set_tty_state = ser_base_noflush_set_tty_state;

  ops->go_raw = ser_windows_raw;
  ops->setbaudrate = ser_windows_setbaudrate;
  ops->setstopbits = ser_windows_setstopbits;
  ops->drain_output = ser_windows_drain_output;
  ops->readchar = ser_base_readchar;
  ops->write = ser_base_write;
  ops->async = ser_base_async;
  ops->read_prim = ser_windows_read_prim;
  ops->write_prim = ser_windows_write_prim;
  ops->wait_handle = ser_windows_wait_handle;

  serial_add_interface (ops);

  /* Next create the dummy serial driver used for terminals.  We only
     provide the TTY-related methods.  */

  ops = XMALLOC (struct serial_ops);
  memset (ops, 0, sizeof (struct serial_ops));

  ops->name = "terminal";
  ops->next = 0;

  ops->close = ser_console_close;
  ops->get_tty_state = ser_console_get_tty_state;
  ops->set_tty_state = ser_base_set_tty_state;
  ops->print_tty_state = ser_base_print_tty_state;
  ops->noflush_set_tty_state = ser_base_noflush_set_tty_state;
  ops->drain_output = ser_base_drain_output;
  ops->wait_handle = ser_console_wait_handle;

  serial_add_interface (ops);

  /* If WinSock works, register the TCP/UDP socket driver.  */

  if (WSAStartup (MAKEWORD (1, 0), &wsa_data) != 0)
    /* WinSock is unavailable.  */
    return;

  ops = XMALLOC (struct serial_ops);
  memset (ops, 0, sizeof (struct serial_ops));
  ops->name = "tcp";
  ops->next = 0;
  ops->open = net_windows_open;
  ops->close = net_windows_close;
  ops->readchar = ser_base_readchar;
  ops->write = ser_base_write;
  ops->flush_output = ser_base_flush_output;
  ops->flush_input = ser_base_flush_input;
  ops->send_break = ser_base_send_break;
  ops->go_raw = ser_base_raw;
  ops->get_tty_state = ser_base_get_tty_state;
  ops->set_tty_state = ser_base_set_tty_state;
  ops->print_tty_state = ser_base_print_tty_state;
  ops->noflush_set_tty_state = ser_base_noflush_set_tty_state;
  ops->setbaudrate = ser_base_setbaudrate;
  ops->setstopbits = ser_base_setstopbits;
  ops->drain_output = ser_base_drain_output;
  ops->async = ser_base_async;
  ops->read_prim = net_read_prim;
  ops->write_prim = net_write_prim;
  ops->wait_handle = net_windows_wait_handle;
  serial_add_interface (ops);
}
