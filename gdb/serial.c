/* Generic serial interface routines
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

/* Open up a device or a network socket, depending upon the syntax of NAME. */

static struct serial_ops *serial_ops_list = NULL;

static struct serial_ops *
serial_interface_lookup (name)
     char *name;
{
  struct serial_ops *ops;

  for (ops = serial_ops_list; ops; ops = ops->next)
    if (strcmp (name, ops->name) == 0)
      return ops;

  return NULL;
}

void
serial_add_interface(optable)
     struct serial_ops *optable;
{
  optable->next = serial_ops_list;
  serial_ops_list = optable;
}

serial_t
serial_open(name)
     const char *name;
{
  serial_t scb;
  struct serial_ops *ops;

  ops = serial_interface_lookup ("hardwire");

  if (!ops)
    return NULL;

  scb = (serial_t)xmalloc (sizeof (struct _serial_t));

  scb->ops = ops;

  scb->bufcnt = 0;
  scb->bufp = scb->buf;

  if (scb->ops->open(scb, name))
    {
      free (scb);
      return NULL;
    }

  return scb;
}

void
serial_close(scb)
     serial_t scb;
{
  scb->ops->close(scb);

  free(scb);
}

#if 0
/* Connect the user directly to the remote system.  This command acts just like
   the 'cu' or 'tip' command.  Use <CR>~. or <CR>~^D to break out.  */

static void
cleanup_tty(ttystate)
     struct ttystate ttystate;
{
  printf("\r\n[Exiting connect mode]\r\n");
  serial_restore(0, &ttystate);
}

static void
connect_command (args, fromtty)
     char	*args;
     int	fromtty;
{
  fd_set readfds;
  int numfds;
  int c;
  char cur_esc = 0;
  static struct ttystate ttystate;

  dont_repeat();

  if (desc < 0)
    error("target not open.");
  
  if (args)
    fprintf("This command takes no args.  They have been ignored.\n");
	
  printf("[Entering connect mode.  Use ~. or ~^D to escape]\n");

  serial_raw(0, &ttystate);

  make_cleanup(cleanup_tty, &ttystate);

  FD_ZERO(&readfds);

  while (1)
    {
      do
	{
	  FD_SET(0, &readfds);
	  FD_SET(desc, &readfds);
	  numfds = select(sizeof(readfds)*8, &readfds, 0, 0, 0);
	}
      while (numfds == 0);

      if (numfds < 0)
	perror_with_name("select");

      if (FD_ISSET(0, &readfds))
	{			/* tty input, send to stdebug */
	  char cx;

	  c = serial_readchar(-1);
	  if (c < 0)
	    perror_with_name("connect");

	  cx = c;
	  serial_write(&cx, 1);
	  switch (cur_esc)
	    {
	    case 0:
	      if (c == '\r')
		cur_esc = c;
	      break;
	    case '\r':
	      if (c == '~')
		cur_esc = c;
	      else
		cur_esc = 0;
	      break;
	    case '~':
	      if (c == '.' || c == '\004')
		return;
	      else
		cur_esc = 0;
	    }
	}

      if (FD_ISSET(desc, &readfds))
	{
	  while (1)
	    {
	      c = serial_readchar(-1);
	      if (c < 0)
		break;
	      putchar(c);
	    }
	  fflush(stdout);
	}
    }
}
#endif

#if 0
void
_initialize_serial ()
{
  add_com ("connect", class_obscure, connect_command,
	   "Connect the terminal directly up to the command monitor.\n\
Use <CR>~. or <CR>~^D to break out.");
}
#endif
