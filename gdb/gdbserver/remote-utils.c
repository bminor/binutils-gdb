/* Remote utility routines for the remote server for GDB.
   Copyright (C) 1986, 1989, 1993 Free Software Foundation, Inc.

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
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <a.out.h>
#include <sys/file.h>
#include <sgtty.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/time.h>

extern int remote_desc;
extern int remote_debugging;

void remote_open ();
void remote_send ();
void putpkt ();
void getpkt ();

void write_ok ();
void write_enn ();
void convert_ascii_to_int ();
void convert_int_to_ascii ();
void prepare_resume_reply ();

/* Open a connection to a remote debugger.
   NAME is the filename used for communication.  */

void
remote_open (name, from_tty)
     char *name;
     int from_tty;
{
  struct sgttyb sg;

  remote_debugging = 0;

  if (!strchr (name, ':'))
    {
      remote_desc = open (name, O_RDWR);
      if (remote_desc < 0)
	perror_with_name ("Could not open remote device");

      ioctl (remote_desc, TIOCGETP, &sg);
      sg.sg_flags = RAW;
      ioctl (remote_desc, TIOCSETP, &sg);
    }
  else
    {
      char *port_str;
      int port;
      struct sockaddr_in sockaddr;
      int tmp;

      port_str = strchr (name, ':');

      port = atoi (port_str + 1);

      remote_desc = socket (PF_INET, SOCK_STREAM, 0);
      if (remote_desc < 0)
	perror_with_name ("Can't open socket");

      /* Allow rapid reuse of this port. */
      tmp = 1;
      setsockopt (remote_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&tmp,
		  sizeof(tmp));

      /* Enable TCP keep alive process. */
      tmp = 1;
      setsockopt (remote_desc, SOL_SOCKET, SO_KEEPALIVE, (char *)&tmp, sizeof(tmp));

      sockaddr.sin_family = PF_INET;
      sockaddr.sin_port = htons(port);
      sockaddr.sin_addr.s_addr = INADDR_ANY;

      if (bind (remote_desc, &sockaddr, sizeof (sockaddr))
	  || listen (remote_desc, 1))
	perror_with_name ("Can't bind address");

      tmp = sizeof (sockaddr);
      remote_desc = accept (remote_desc, &sockaddr, &tmp);
      if (remote_desc == -1)
	perror_with_name ("Accept failed");

      tmp = 1;
      setsockopt (remote_desc, 6, TCP_NODELAY, (char *)&tmp, sizeof(tmp));
    }

  fprintf (stderr, "Remote debugging using %s\n", name);
  remote_debugging = 1;
}

/* Convert hex digit A to a number.  */

static int
fromhex (a)
     int a;
{
  if (a >= '0' && a <= '9')
    return a - '0';
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else
    error ("Reply contains invalid hex digit");
}

/* Convert number NIB to a hex digit.  */

static int
tohex (nib)
     int nib;
{
  if (nib < 10)
    return '0' + nib;
  else
    return 'a' + nib - 10;
}

/* Send the command in BUF to the remote machine,
   and read the reply into BUF.
   Report an error if we get an error reply.  */

void
remote_send (buf)
     char *buf;
{
  putpkt (buf);
  getpkt (buf);

  if (buf[0] == 'E')
    error ("Remote failure reply: E");
}

/* Send a packet to the remote machine, with error checking.
   The data of the packet is in BUF.  */

void
putpkt (buf)
     char *buf;
{
  int i;
  unsigned char csum = 0;
  char buf2[2000];
  char buf3[1];
  int cnt = strlen (buf);
  char *p;

  /* Copy the packet into buffer BUF2, encapsulating it
     and giving it a checksum.  */

  p = buf2;
  *p++ = '$';

  for (i = 0; i < cnt; i++)
    {
      csum += buf[i];
      *p++ = buf[i];
    }
  *p++ = '#';
  *p++ = tohex ((csum >> 4) & 0xf);
  *p++ = tohex (csum & 0xf);

  /* Send it over and over until we get a positive ack.  */

  do
    {
      write (remote_desc, buf2, p - buf2);
      read (remote_desc, buf3, 1);
    }
  while (buf3[0] != '+');
}

static int
readchar ()
{
  static char buf[BUFSIZ];
  static int bufcnt = 0;
  static char *bufp;

  if (bufcnt-- > 0)
    return *bufp++ & 0x7f;

  bufcnt = read (remote_desc, buf, sizeof (buf));

  if (bufcnt <= 0)
    {
      perror ("readchar");
      fatal ("read error, quitting");
    }

  bufp = buf;
  bufcnt--;
  return *bufp++ & 0x7f;
}

/* Read a packet from the remote machine, with error checking,
   and store it in BUF.  */

void
getpkt (buf)
     char *buf;
{
  char *bp;
  unsigned char csum, c1, c2;
  int c;

  while (1)
    {
      csum = 0;

      while ((c = readchar ()) != '$');

      bp = buf;
      while (1)
	{
	  c = readchar ();
	  if (c == '#')
	    break;
	  *bp++ = c;
	  csum += c;
	}
      *bp = 0;

      c1 = fromhex (readchar ());
      c2 = fromhex (readchar ());
      if (csum == (c1 << 4) + c2)
	break;

      fprintf (stderr, "Bad checksum, sentsum=0x%x, csum=0x%x, buf=%s\n",
	       (c1 << 4) + c2, csum, buf);
      write (remote_desc, "-", 1);
    }

  write (remote_desc, "+", 1);
}

void
write_ok (buf)
     char *buf;
{
  buf[0] = 'O';
  buf[1] = 'k';
  buf[2] = '\0';
}

void
write_enn (buf)
     char *buf;
{
  buf[0] = 'E';
  buf[1] = 'N';
  buf[2] = 'N';
  buf[3] = '\0';
}

void
convert_int_to_ascii (from, to, n)
     char *from, *to;
     int n;
{
  int nib;
  char ch;
  while (n--)
    {
      ch = *from++;
      nib = ((ch & 0xf0) >> 4) & 0x0f;
      *to++ = tohex (nib);
      nib = ch & 0x0f;
      *to++ = tohex (nib);
    }
  *to++ = 0;
}


void
convert_ascii_to_int (from, to, n)
     char *from, *to;
     int n;
{
  int nib1, nib2;
  while (n--)
    {
      nib1 = fromhex (*from++);
      nib2 = fromhex (*from++);
      *to++ = (((nib1 & 0x0f) << 4) & 0xf0) | (nib2 & 0x0f);
    }
}

static char *
outreg(regno, buf)
     int regno;
     char *buf;
{
  extern char registers[];

  *buf++ = tohex (regno >> 4);
  *buf++ = tohex (regno & 0xf);
  *buf++ = ':';
  convert_int_to_ascii (&registers[REGISTER_BYTE (regno)], buf, 4);
  buf += 8;
  *buf++ = ';';

  return buf;
}

void
prepare_resume_reply (buf, status, signal)
     char *buf, status;
     unsigned char signal;
{
  int nib;
  char ch;

  *buf++ = 'T';

  nib = ((signal & 0xf0) >> 4);
  *buf++ = tohex (nib);
  nib = signal & 0x0f;
  *buf++ = tohex (nib);

  buf = outreg (PC_REGNUM, buf);
  buf = outreg (FP_REGNUM, buf);
  buf = outreg (SP_REGNUM, buf);
#ifdef NPC_REGNUM
  buf = outreg (NPC_REGNUM, buf);
#endif
#ifdef O7_REGNUM
  buf = outreg (O7_REGNUM, buf);
#endif

  *buf++ = 0;
}

void
decode_m_packet (from, mem_addr_ptr, len_ptr)
     char *from;
     unsigned int *mem_addr_ptr, *len_ptr;
{
  int i = 0, j = 0;
  char ch;
  *mem_addr_ptr = *len_ptr = 0;

  while ((ch = from[i++]) != ',')
    {
      *mem_addr_ptr = *mem_addr_ptr << 4;
      *mem_addr_ptr |= fromhex (ch) & 0x0f;
    }

  for (j = 0; j < 4; j++)
    {
      if ((ch = from[i++]) == 0)
	break;
      *len_ptr = *len_ptr << 4;
      *len_ptr |= fromhex (ch) & 0x0f;
    }
}

void
decode_M_packet (from, mem_addr_ptr, len_ptr, to)
     char *from, *to;
     unsigned int *mem_addr_ptr, *len_ptr;
{
  int i = 0, j = 0;
  char ch;
  *mem_addr_ptr = *len_ptr = 0;

  while ((ch = from[i++]) != ',')
    {
      *mem_addr_ptr = *mem_addr_ptr << 4;
      *mem_addr_ptr |= fromhex (ch) & 0x0f;
    }

  while ((ch = from[i++]) != ':')
    {
      *len_ptr = *len_ptr << 4;
      *len_ptr |= fromhex (ch) & 0x0f;
    }

  convert_ascii_to_int (&from[i++], to, *len_ptr);
}
