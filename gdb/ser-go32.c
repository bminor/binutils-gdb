/* Remote serial interface for local (hardwired) serial ports for GO32.
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

/* This is unused for now.  We just return a placeholder. */
struct go32_ttystate
{
  int bogus;
};

typedef struct {
  short jmp_op;
  short signature;
  short version;
  short buffer_start;
  short buffer_end;
  short getp;
  short putp;
  short iov;
} ASYNC_STRUCT;

static int go32_open PARAMS ((serial_t scb, const char *name));
static void go32_raw PARAMS ((serial_t scb));
static int wait_for PARAMS ((serial_t scb, int timeout));
static int go32_readchar PARAMS ((serial_t scb, int timeout));
static int rate_to_code PARAMS ((int rate));
static int go32_setbaudrate PARAMS ((serial_t scb, int rate));
static int go32_write PARAMS ((serial_t scb, const char *str, int len));
static void go32_restore PARAMS ((serial_t scb));
static void go32_close PARAMS ((serial_t scb));
static serial_ttystate go32_get_tty_state PARAMS ((serial_t scb));
static int go32_set_tty_state PARAMS ((serial_t scb, serial_ttystate state));
static int strncasecmp PARAMS ((const char *str1, const char *str2, int len));
static char *aptr PARAMS ((short p));
static ASYNC_STRUCT *getivec PARAMS ((int which));
static int dos_async_init PARAMS ((int port));
static void dos_async_tx PARAMS ((const char c));
static int dos_async_ready PARAMS (());
static int dos_async_rx PARAMS (());
static int dosasync_read PARAMS ((int fd, char *buf, int len, int timeout));
static int dosasync_write PARAMS ((int fd, const char *buf, int len));

#define SIGNATURE 0x4154
#define VERSION 1
#define OFFSET 0x104

#define peek(a,b) (*(unsigned short *)(0xe0000000 + (a)*16 + (b)))

static ASYNC_STRUCT *async;
static int iov;
#define com_rb	iov
#define com_tb	iov
#define com_ier	iov+1
#define com_ifr	iov+2
#define com_bfr	iov+3
#define com_mcr	iov+4
#define com_lsr	iov+5
#define com_msr	iov+6

static int
strncasecmp(str1, str2, len)
     const char *str1, *str2;
     register int len;
{
  unsigned char c1, c2;

  for (; len != 0; --len)
    {
      c1 = *str1++;
      c2 = *str2++;

      if (toupper(c1) != toupper(c2))
	return toupper(c1) - toupper(c2);

      if (c1 == '\0')
	return 0;
    }
  return 0;
}

static char *
aptr(p)
     short p;
{
  return (char *)((unsigned)async - OFFSET + p);
}

static ASYNC_STRUCT *
getivec(int which)
{
  ASYNC_STRUCT *a;

  if (peek(0, which*4) != OFFSET)
    return 0;

  a = (ASYNC_STRUCT *)(0xe0000000 + peek(0, which*4+2)*16 + peek(0, which*4));

  if (a->signature != SIGNATURE)
    return 0;

  if (a->version != VERSION)
    return 0;

  return a;
}

static int
dos_async_init(port)
     int port;
{
  int i;

  switch (port)
    {
    case 1:
      async = getivec (12);
      break;
    case 2:
      async = getivec (11);
      break;
    default:
      return 0;
    }

  if (!async)
    {
      error("GDB cannot connect to asynctsr program, check that it is installed\n\
and that serial I/O is not being redirected (perhaps by NFS)\n\n\
example configuration:\n\
C> mode com%d:9600,n,8,1,p\n\
C> asynctsr %d\n\
C> gdb \n", port, port);
    }

  iov = async->iov;
  outportb(com_ier, 0x0f);
  outportb(com_bfr, 0x03);
  outportb(com_mcr, 0x0b);
  async->getp = async->putp = async->buffer_start;
  
  return 1;
}

static void
dos_async_tx(c)
     const char c;
{
  while (~inportb(com_lsr) & 0x20);

  outportb(com_tb, c);
}

static int
dos_async_ready()
{
  return (async->getp != async->putp);
}

static int
dos_async_rx()
{
  char rv;

  while (!dos_async_ready())
    if (kbhit())
      {
	printf("abort!\n");
	return 0;
      }

  rv = *aptr(async->getp++);
  if (async->getp >= async->buffer_end)
    async->getp = async->buffer_start;

  return rv;
}

static int
dosasync_read (fd, buf, len, timeout)
     int fd;
     char *buf;
     int len;
     int timeout;
{
  long now, then;
  int l = len;

  time (&now);
  then = now + timeout;

  while (l--)
    {
      if (timeout)
	{
	  while (!dos_async_ready())
	    {
	      time (&now);
	      if (now >= then)
		return len - l - 1;
	    }
	}
      *buf++ = dos_async_rx();
    }

  return len;
}

static int
dosasync_write(fd, buf, len)
     int fd;
     const char *buf;
     int len;
{
  int l = len;

  while (l--)
    dos_async_tx (*buf++);

  return len;
}

static int
go32_open (scb, name)
     serial_t scb;
     const char *name;
{
  int port;

  if (strncasecmp (name, "com", 3) != 0)
    {
      errno = ENOENT;
      return -1;
    }

  port = name[3] - '0';

  if ((port != 1) && (port != 2))
    {
      errno = ENOENT;
      return -11;
    }

  scb->fd = dos_async_init(port);
  if (!scb->fd)
    return -1;

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

  if (dosasync_read(scb->fd, &buf, 1, timeout))  
    return buf;
  else
    return SERIAL_TIMEOUT;
}

/* go32_{get set}_tty_state() are both dummys to fill out the function
   vector.  Someday, they may do something real... */

static serial_ttystate
go32_get_tty_state(scb)
     serial_t scb;
{
  struct go32_ttystate *state;

  state = (struct go32_ttystate *)xmalloc(sizeof *state);

  return (serial_ttystate)state;
}

static int
go32_set_tty_state(scb, ttystate)
     serial_t scb;
     serial_ttystate ttystate;
{
  struct go32_ttystate *state;

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

static int
go32_write (scb, str, len)
     serial_t scb;
     const char *str;
     int len;
{
  dosasync_write(scb->fd, str, len);

  return 0;
}

static void
go32_close (scb)
     serial_t scb;
{
}

static struct serial_ops go32_ops =
{
  "hardwire",
  0,
  go32_open,
  go32_close,
  go32_readchar,
  go32_write,
  go32_noop, /* flush output */
  go32_noop, /* flush input */
  go32_noop, /* send break -- currently used only for nindy */
  go32_raw,
  go32_get_tty_state,
  go32_set_tty_state,
  go32_print_tty_state,
  go32_noflush_set_tty_state,
  go32_setbaudrate,
};

_initialize_ser_go32 ()
{
  serial_add_interface (&go32_ops);
}
