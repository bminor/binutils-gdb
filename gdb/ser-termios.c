/* Remote serial interface for OS's with termios, for GDB.
   Copyright 1992 Free Software Foundation, Inc.

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
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>

/* File descriptor used in termios routines to access serial line.  */
static int desc;

/* Saved state about the terminal.  */
static struct termios otermios;
static int oflags;

const char *
serial_default_name ()
{
  return "/dev/ttya";
}

void
serial_raw ()
{
  /* Now happens inside of serial_open */
}

void
serial_normal ()
{
  fcntl(desc, oflags, 0);

  if (tcsetattr(desc, TCSANOW, &otermios))
    {
      printf("tcgetattr failed: errno=%d\n", errno);
    }
}

int
serial_open (name)
      const char *name;
{
  struct termios termios;

  desc = open (name, O_RDWR);
  if (desc < 0)
   perror("Open failed: ");

  oflags = fcntl(desc, F_GETFL, 0);

  fcntl(desc, F_SETFL, oflags|FNDELAY);

  if (tcgetattr(desc, &termios)) {
    printf("tcgetattr failed: errno=%d\n", errno);
  }

  otermios = termios;

  termios.c_iflag = 0;
  termios.c_oflag = 0;
  termios.c_lflag = 0;
  termios.c_cc[VMIN] = 0;
  termios.c_cc[VTIME] = 0;

  if (tcsetattr(desc, TCSANOW, &termios)) {
    printf("tcgetattr failed: errno=%d\n", errno);
  }

  return 1;
}

int
serial_timedreadchar (timeout, ok) 
      int timeout;
      int *ok;
{
  unsigned char buf;
  fd_set readfds;
  int val;
  struct timeval tv;

  FD_ZERO(&readfds);
  FD_SET(desc, &readfds);

  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  val = select(desc+1, &readfds, 0, 0, &tv);

  if (val > 0 && FD_ISSET(desc, &readfds))
    {
      val = read (desc, &buf, 1);

      if (val == 1)
	{
	  *ok = 1;
	  return buf;
	}
    }

  *ok = 0;

  return 0;
}

/* Translate baud rates from integers to damn B_codes.  Unix should
   have outgrown this crap years ago, but even POSIX wouldn't buck it.  */

#ifndef B19200
#define B19200 EXTA
#endif
#ifndef B38400
#define B38400 EXTB
#endif

static struct {int rate, damn_b;} baudtab[] = {
	{9600, B9600},

	{19200, B19200},
	{38400, B38400},
#if 0
	{300, B300},
	{1200, B1200},
	{2400, B2400},
	{4800, B4800},
#endif
	{-1, -1},
};

static int 
damn_b (rate)
      int rate;
{
  int i;
  for (i = 0; baudtab[i].rate != -1; i++)
  {
    if (rate == baudtab[i].rate)  
    {
      return i;
    }
  }
  return -1;
}

int 
serial_setbaudrate (rate)
     int rate;
{
  struct termios termios;

  if (tcgetattr(desc, &termios)) {
    printf("tcgetattr failed: errno=%d\n", errno);
  }

  cfsetospeed(&termios, baudtab[damn_b(rate)].damn_b);
  cfsetispeed(&termios, baudtab[damn_b(rate)].damn_b);

  if (tcsetattr(desc, TCSANOW, &termios)) {
    printf("tcgetattr failed: errno=%d\n", errno);
  }

  return 1;
}

int
serial_nextbaudrate (rate)
      int rate;
{
  int lookup;
  lookup = damn_b(rate);
  if (lookup == -1)
   return baudtab[0].rate;
  lookup++;
  if (baudtab[lookup].rate == -1)
   return baudtab[0].rate;
  return baudtab[lookup].rate;
}

int
serial_write (str, len)
      const char *str;
      int len;
{
  return (write (desc, str, len));
}


int
serial_close ()
{
  return (close(desc));
}
