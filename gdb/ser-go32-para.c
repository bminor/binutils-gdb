/* Remote parallel interface for local parallel ports for GO32.
   Copyright 1992, 1993 Free Software Foundation, Inc.

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
#include "serial.h"
#include <sys/dos.h>

#if 0
#define disable() asm("cli")
#define enable() asm("sti")
#endif

/* this is a duumy to fill the ops structure */
struct go32_ttystate
  {
    int bogus;
  };

static int go32_open PARAMS ((serial_t scb, const char *name));
static void go32_raw PARAMS ((serial_t scb));
static int go32_readchar PARAMS ((serial_t scb, int timeout));
static int go32_setbaudrate PARAMS ((serial_t scb, int rate));
static int go32_write PARAMS ((serial_t scb, const char *str, int len));
static void go32_close PARAMS ((serial_t scb));
static serial_ttystate go32_get_tty_state PARAMS ((serial_t scb));
static int go32_set_tty_state PARAMS ((serial_t scb, serial_ttystate state));
static unsigned long getivec PARAMS ((int which));
static int dos_read PARAMS ((int fd, char *buf, int len));
static int dos_write PARAMS ((int fd, const char *buf, int len));

#if 0
static int iov[2];
#define com_rb(n)	iov[n]
#define com_tb(n)	iov[n]
#define com_ier(n)	iov[n]+1
#define com_ifr(n)	iov[n]+2
#define com_bfr(n)	iov[n]+3
#define com_mcr(n)	iov[n]+4
#define com_lsr(n)	iov[n]+5
#define com_msr(n)	iov[n]+6

static unsigned long
getivec (int which)
{
  long tryaindex;

  if (GET_WORD (which * 4) != OFFSET)
    return 0;

  /* Find out where in memory this lives */
  tryaindex = GET_WORD (which * 4 + 2) * 16 + GET_WORD (which * 4);

  if (GET_WORD (tryaindex + 2) != SIGNATURE)
    return 0;
  if (GET_WORD (tryaindex + 4) != VERSION)
    return 0;
  return tryaindex;
}
#endif /* 0 */

static int biosprn (cmd, byte, port)
     int cmd;
     int byte;
     int port;
{
  union REGS regs;
  regs.h.ah = cmd;
  regs.h.al = byte;
  regs.x.dx = port;
  int86 (0x17, &regs, &regs);
  return regs.h.ah;
}

static int
dos_read (fd, buf, len)
     int fd;
     char *buf;
     int len;
{
  int i;

  for (i = 0; i < len; i++)
    {
      *buf++ = biosprn (2, 0, fd);
    }
  return len;
}

static int
dos_write (fd, buf, len)
     int fd;
     const char *buf;
     int len;
{
  int l;

  for (l = 0; l < len; l++)
    biosprn (0, *buf++, fd);

  return len;
}

static int
go32_open (scb, name)
     serial_t scb;
     const char *name;
{
  int port, ret;

  if (strncasecmp (name, "lpt", 3) != 0)
    {
      errno = ENOENT;
      return -1;
    }

  port = name[3] - '0';

  if (port != 1 && port != 2 && port != 3)
    {
      errno = ENOENT;
      return -11;
    }

  ret = biosprn (1, 0, port);
  if (!ret)
    return -1;

  scb->fd = port;
  return 0;
}

static int
go32_noop (scb)
     serial_t scb;
{
  return 0;
}

static void
go32_raw (scb)
     serial_t scb;
{
  /* Always in raw mode */
}

static int
go32_readchar (scb, timeout)
     serial_t scb;
     int timeout;
{
  char buf;

  if (dos_read (scb->fd, &buf, 1))
    return buf;
  else
    return SERIAL_TIMEOUT;
}

static int
go32_write (scb, str, len)
     serial_t scb;
     const char *str;
     int len;
{
  dos_write (scb->fd, str, len);
  return 0;
}

/* go32_{get set}_tty_state() are both dummys to fill out the function
   vector.  Someday, they may do something real... */

static serial_ttystate
go32_get_tty_state (scb)
     serial_t scb;
{
  struct go32_ttystate *state;

  state = (struct go32_ttystate *) xmalloc (sizeof *state);

  return (serial_ttystate) state;
}

static int
go32_set_tty_state (scb, ttystate)
     serial_t scb;
     serial_ttystate ttystate;
{
  return 0;
}

static int
go32_noflush_set_tty_state (scb, new_ttystate, old_ttystate)
     serial_t scb;
     serial_ttystate new_ttystate;
     serial_ttystate old_ttystate;
{
  return 0;
}

static void
go32_print_tty_state (scb, ttystate)
     serial_t scb;
     serial_ttystate ttystate;
{
  /* Nothing to print.  */
  return;
}

static int
go32_setbaudrate (scb, rate)
     serial_t scb;
     int rate;
{
  return 0;
}

static void
go32_close (scb)
     serial_t scb;
{
}

static struct serial_ops go32_ops =
{
  "parallel",
  0,
  go32_open,
  go32_close,
  go32_readchar,
  go32_write,
  go32_noop,			/* flush output */
  go32_noop,			/* flush input */
  go32_noop,			/* send break -- currently used only for nindy */
  go32_raw,
  go32_get_tty_state,
  go32_set_tty_state,
  go32_print_tty_state,
  go32_noflush_set_tty_state,
  go32_setbaudrate,
};

void
_initialize_ser_go32 ()
{
  serial_add_interface (&go32_ops);
}
