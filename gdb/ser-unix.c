/* Serial interface for local (hardwired) serial ports on Un*x like systems
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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>

#if !defined (HAVE_TERMIOS) && !defined (HAVE_TERMIO) && !defined (HAVE_SGTTY)
#define HAVE_SGTTY
#endif

#ifdef HAVE_TERMIOS
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#endif
#ifdef HAVE_TERMIO
#include <termio.h>
#include <poll.h>
#endif
#ifdef HAVE_SGTTY
#include <sgtty.h>
#endif

static int hardwire_open PARAMS ((serial_t scb, const char *name));
static void hardwire_raw PARAMS ((serial_t scb));
static int wait_for PARAMS ((serial_t scb, int timeout));
static int hardwire_readchar PARAMS ((serial_t scb, int timeout));
static int rate_to_code PARAMS ((int rate));
static int hardwire_setbaudrate PARAMS ((serial_t scb, int rate));
static int hardwire_write PARAMS ((serial_t scb, const char *str, int len));
static void hardwire_restore PARAMS ((serial_t scb));
static void hardwire_close PARAMS ((serial_t scb));

/* Open up a real live device for serial I/O */

static int
hardwire_open(scb, name)
     serial_t scb;
     const char *name;
{
  scb->fd = open (name, O_RDWR);
  if (scb->fd < 0)
    return -1;

  return 0;
}

static void
hardwire_raw(scb)
     serial_t scb;
{
#ifdef HAVE_TERMIOS
  struct termios termios;

  if (tcgetattr(scb->fd, &termios))
    {
      fprintf(stderr, "tcgetattr failed: %s\n", safe_strerror(errno));
    }

  termios.c_iflag = 0;
  termios.c_oflag = 0;
  termios.c_lflag = 0;
  termios.c_cflag &= ~(CSIZE|PARENB);
  termios.c_cflag |= CS8;
  termios.c_cc[VMIN] = 0;
  termios.c_cc[VTIME] = 0;

  if (tcsetattr(scb->fd, TCSANOW, &termios))
    {
      fprintf(stderr, "tcsetattr failed: %s\n", safe_strerror(errno));
    }
#endif

#ifdef HAVE_TERMIO
  struct termio termio;

  if (ioctl (scb->fd, TCGETA, &termio))
    {
      fprintf(stderr, "TCGETA failed: %s\n", safe_strerror(errno));
    }

  termio.c_iflag = 0;
  termio.c_oflag = 0;
  termio.c_lflag = 0;
  termio.c_cflag &= ~(CSIZE|PARENB);
  termio.c_cflag |= CS8;
  termio.c_cc[VMIN] = 0;
  termio.c_cc[VTIME] = 0;

  if (ioctl (scb->fd, TCSETA, &termio))
    {
      fprintf(stderr, "TCSETA failed: %s\n", safe_strerror(errno));
    }
#endif

#ifdef HAVE_SGTTY
  struct sgttyb sgttyb;

  if (ioctl (scb->fd, TIOCGETP, &sgttyb))
    fprintf(stderr, "TIOCGETP failed: %s\n", safe_strerror(errno));

  sgttyb.sg_flags |= RAW | ANYP;
  sgttyb.sg_flags &= ~(CBREAK | ECHO);

  if (ioctl (scb->fd, TIOCSETP, &sgttyb))
    fprintf(stderr, "TIOCSETP failed: %s\n", safe_strerror(errno));
#endif
}

/* Wait for input on scb, with timeout seconds */

static int
wait_for(scb, timeout)
     serial_t scb;
     int timeout;
{
  int numfds;

#ifdef HAVE_SGTTY
  struct timeval tv;
  fd_set readfds;

  FD_ZERO (&readfds);

  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  FD_SET(scb->fd, &readfds);

  if (timeout >= 0)
    numfds = select(scb->fd+1, &readfds, 0, 0, &tv);
  else
    numfds = select(scb->fd+1, &readfds, 0, 0, 0);

#endif	/* HAVE_SGTTY */

#if defined HAVE_TERMIO || defined HAVE_TERMIOS
  struct pollfd pollfd;

  pollfd.fd = scb->fd;
  pollfd.events = POLLIN | POLLPRI;

  if (timeout > 0)
    timeout *= 1000;

  numfds = poll(&pollfd, 1, timeout);

#endif	/* defined HAVE_TERMIO || defined HAVE_TERMIOS */

  if (numfds <= 0)
    if (numfds == 0)
      return SERIAL_TIMEOUT;
    else
      return SERIAL_ERROR;	/* Got an error from select or poll */
}

/* Read a character with user-specified timeout.  TIMEOUT is number of seconds
   to wait, or -1 to wait forever.  Use timeout of 0 to effect a poll.  Returns
   char if successful.  Returns -2 if timeout expired, EOF if line dropped
   dead, or -3 for any other error (see errno in that case). */

static int
hardwire_readchar(scb, timeout)
     serial_t scb;
     int timeout;
{
  int status;

  if (scb->bufcnt-- > 0)
    return *scb->bufp++;

  status = wait_for(scb, timeout);

  if (status < 0)
    return status;

  scb->bufcnt = read(scb->fd, scb->buf, BUFSIZ);

  if (scb->bufcnt <= 0)
    if (scb->bufcnt == 0)
      return SERIAL_EOF;	/* 0 chars means end of file */
    else
      return SERIAL_ERROR;	/* Got an error from read */

  scb->bufcnt--;
  scb->bufp = scb->buf;
  return *scb->bufp++;
}

#ifndef B19200
#define B19200 EXTA
#endif

#ifndef B38400
#define B38400 EXTB
#endif

/* Translate baud rates from integers to damn B_codes.  Unix should
   have outgrown this crap years ago, but even POSIX wouldn't buck it.  */

static struct
{
  int rate;
  int code;
}
baudtab[] =
{
  {50, B50},
  {75, B75},
  {110, B110},
  {134, B134},
  {150, B150},
  {200, B200},
  {300, B300},
  {600, B600},
  {1200, B1200},
  {1800, B1800},
  {2400, B2400},
  {4800, B4800},
  {9600, B9600},
  {19200, B19200},
  {38400, B38400},
  {-1, -1},
};

static int 
rate_to_code(rate)
     int rate;
{
  int i;

  for (i = 0; baudtab[i].rate != -1; i++)
    if (rate == baudtab[i].rate)  
      return baudtab[i].code;

  return -1;
}

static int
hardwire_setbaudrate(scb, rate)
     serial_t scb;
     int rate;
{
#ifdef HAVE_TERMIOS
  struct termios termios;

  if (tcgetattr (scb->fd, &termios))
    return -1;

  cfsetospeed (&termios, rate_to_code (rate));
  cfsetispeed (&termios, rate_to_code (rate));

  if (tcsetattr (scb->fd, TCSANOW, &termios))
    return -1;
#endif

#ifdef HAVE_TERMIO
  struct termio termio;

  if (ioctl (scb->fd, TCGETA, &termio))
    return -1;

#ifndef CIBAUD
#define CIBAUD CBAUD
#endif

  termio.c_cflag &= ~(CBAUD | CIBAUD);
  termio.c_cflag |= rate_to_code (rate);

  if (ioctl (scb->fd, TCSETA, &termio))
    return -1;
#endif

#ifdef HAVE_SGTTY
  struct sgttyb sgttyb;

  if (ioctl (scb->fd, TIOCGETP, &sgttyb))
    return -1;

  sgttyb.sg_ispeed = rate_to_code (rate);
  sgttyb.sg_ospeed = rate_to_code (rate);

  if (ioctl (scb->fd, TIOCSETP, &sgttyb))
    return -1;
#endif
  return 0;
}

static int
hardwire_write(scb, str, len)
     serial_t scb;
     const char *str;
     int len;
{
  int cc;

  while (len > 0)
    {
      cc = write(scb->fd, str, len);

      if (cc < 0)
	return 1;
      len -= cc;
      str += cc;
    }
  return 0;
}

static void
hardwire_restore(scb)
     serial_t scb;
{
}

static void
hardwire_close(scb)
     serial_t scb;
{
  if (scb->fd < 0)
    return;

  close(scb->fd);
  scb->fd = -1;
}

static struct serial_ops hardwire_ops =
{
  "hardwire",
  0,
  hardwire_open,
  hardwire_close,
  hardwire_readchar,
  hardwire_write,
  hardwire_raw,
  hardwire_restore,
  hardwire_setbaudrate
};

void
_initialize_ser_hardwire ()
{
  serial_add_interface (&hardwire_ops);
}
