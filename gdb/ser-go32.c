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

#define disable() asm("cli")
#define enable() asm("sti")

struct go32_ttystate
  {
    int bogus;
  };
typedef struct
  {
    short jmp_op;
    short signature;
    short version;
    short buffer_start;
    short buffer_end;
    short getp;
    short putp;
    short iov;
    short count;
    short overflow;
    short buffer_size;
    short ovflushes;
  }
ASYNC_STRUCT;

#define AOFF_JMP_OP 		0
#define AOFF_SIGNATURE 		2
#define AOFF_VERSION 		4
#define AOFF_BUFFER_START 	6
#define AOFF_BUFFER_END 	8
#define AOFF_GETP 		10
#define AOFF_PUTP 		12
#define AOFF_IOV 		14
#define AOFF_COUNT 		16
#define AOFF_OVERFLOW 		18
#define AOFF_BUFFER_SIZE 	20
#define AOFF_OVFLUSHES 		22


static ASYNC_STRUCT a;		/* Copy in our mem of the struct */
static long aindex;		/* index into dos space of struct */

static int go32_open PARAMS ((serial_t scb, const char *name));
static void go32_raw PARAMS ((serial_t scb));
static int go32_readchar PARAMS ((serial_t scb, int timeout));
static int go32_setbaudrate PARAMS ((serial_t scb, int rate));
static int go32_write PARAMS ((serial_t scb, const char *str, int len));
static void go32_close PARAMS ((serial_t scb));
static serial_ttystate go32_get_tty_state PARAMS ((serial_t scb));
static int go32_set_tty_state PARAMS ((serial_t scb, serial_ttystate state));
static unsigned char aptr PARAMS ((short p));
static unsigned long getivec PARAMS ((int which));
static int dos_async_init PARAMS ((int port));
static void dos_async_tx PARAMS ((const char c));
static int dos_async_rx PARAMS (());
static int dosasync_read PARAMS ((int fd, char *buf, int len, int timeout));
static int dosasync_write PARAMS ((int fd, const char *buf, int len));

#define SIGNATURE 0x4154
#define VERSION 1
#define OFFSET 0x104

unsigned char bb;
unsigned short sb;
unsigned long sl;

#define SET_BYTE(x,y)   { char _buf = y;dosmemput(&_buf,1, x);}
#define SET_WORD(x,y)   { short _buf = y;dosmemput(&_buf,2, x);}
#define GET_BYTE(x)     ( dosmemget((x),1,&bb), bb)


#define GET_LONG(x)     ( dosmemget((x),4,&sl), sl)

static
unsigned short 
GET_WORD (x)
{
  unsigned short sb;
  dosmemget ((x), 2, &sb);
  return sb;
}

static int iov[2];

#define com_rb(n)	iov[n]
#define com_tb(n)	iov[n]
#define com_ier(n)	iov[n]+1
#define com_ifr(n)	iov[n]+2
#define com_bfr(n)	iov[n]+3
#define com_mcr(n)	iov[n]+4
#define com_lsr(n)	iov[n]+5
#define com_msr(n)	iov[n]+6

static unsigned char
aptr (p)
     short p;
{
  return GET_BYTE (aindex - OFFSET + p);
}

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

static int
dos_async_init (port)
     int port;
{
  switch (port)
    {
    case 1:
      aindex = getivec (12);
      break;
    case 2:
      aindex = getivec (11);
      break;
    default:
      return 0;
    }

  if (!aindex)
    {
      error ("GDB cannot connect to asynctsr program, check that it is installed\n\
and that serial I/O is not being redirected (perhaps by NFS)\n\n\
example configuration:\n\
C> mode com%d:9600,n,8,1,p\n\
C> asynctsr %d\n\
C> gdb \n", port, port);
    }

  iov[0] = GET_WORD (aindex + AOFF_IOV);
  outportb (com_ier (0), 0x0f);
  outportb (com_bfr (0), 0x03);
  outportb (com_mcr (0), 0x0b);
  return 1;
}

static void
dos_async_tx (c)
     const char c;
{
  while (~inportb (com_lsr (0)) & 0x20)
    ;
  outportb (com_tb (0), c);
}

static int
dos_async_ready ()
{
  int ret;

  disable ();
#if RDY_CNT
  ret = GET_WORD (aindex + AOFF_COUNT);
#else
  ret = GET_WORD (aindex + AOFF_GETP) != GET_WORD (aindex + AOFF_PUTP);
#endif
  enable ();
  return ret;


}

static int
dos_async_rx ()
{
  char rv;
  short idx;

  while (!dos_async_ready ())
    {
      if (kbhit ())
	{
	  printf_unfiltered ("abort!\n");
	  return 0;
	}
    }
  disable ();
  idx = GET_WORD (aindex + AOFF_GETP);
  idx++;
  SET_WORD (aindex + AOFF_GETP, idx);
  rv = aptr (idx - 1);
  SET_WORD (aindex + AOFF_COUNT, GET_WORD (aindex + AOFF_COUNT) - 1);
  if (GET_WORD (aindex + AOFF_GETP) > GET_WORD (aindex + AOFF_BUFFER_END))
    SET_WORD (aindex + AOFF_GETP, GET_WORD (aindex + AOFF_BUFFER_START));
  enable ();
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
  int i;

  time (&now);
  then = now + timeout;

  for (i = 0; i < len; i++)
    {
      if (timeout)
	{
	  while (!dos_async_ready ())
	    {
	      time (&now);
	      if (now >= then && timeout > 0)
		return i;
	    }
	}
      *buf++ = dos_async_rx ();
    }
  return len;
}

static int
dosasync_write (fd, buf, len)
     int fd;
     const char *buf;
     int len;
{
  int l;

  for (l = 0; l < len; l++)
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

  scb->fd = dos_async_init (port);
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

  /* Shortcut for polling */
  if (timeout == 0)
    {
      if (dos_async_ready ())
	{
	  return dos_async_rx ();
	}
      return SERIAL_TIMEOUT;
    }

  if (dosasync_read (scb->fd, &buf, 1, timeout))
    return buf;
  else
    return SERIAL_TIMEOUT;
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

static int
go32_write (scb, str, len)
     serial_t scb;
     const char *str;
     int len;
{
  dosasync_write (scb->fd, str, len);

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
