/* Host callback routines for GDB.
   Copyright 1995, 1996 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This file provides a standard way for targets to talk to the host OS
   level.  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "ansidecl.h"
#ifdef ANSI_PROTOTYPES
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include "callback.h"
#include "targ-vals.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

extern int system PARAMS ((const char *));

static int os_init PARAMS ((host_callback *));
static int os_shutdown PARAMS ((host_callback *));
static int os_unlink PARAMS ((host_callback *, const char *));
static long os_time PARAMS ((host_callback *, long *));
static int os_system PARAMS ((host_callback *, const char *));
static int os_rename PARAMS ((host_callback *, const char *, const char *));
static int os_write_stdout PARAMS ((host_callback *, const char *, int));
static void os_flush_stdout PARAMS ((host_callback *));
static int os_write_stderr PARAMS ((host_callback *, const char *, int));
static void os_flush_stderr PARAMS ((host_callback *));
static int os_write PARAMS ((host_callback *, int, const char *, int));
static int os_read_stdin PARAMS ((host_callback *, char *, int));
static int os_read PARAMS ((host_callback *, int, char *, int));
static int os_open PARAMS ((host_callback *, const char *, int));
static int os_lseek PARAMS ((host_callback *, int, long, int));
static int os_isatty PARAMS ((host_callback *, int));
static int os_get_errno PARAMS ((host_callback *));
static int os_close PARAMS ((host_callback *, int));
static void os_vprintf_filtered PARAMS ((host_callback *, const char *, va_list));
static void os_evprintf_filtered PARAMS ((host_callback *, const char *, va_list));
static void os_error PARAMS ((host_callback *, const char *, ...));
static int fdmap PARAMS ((host_callback *, int));
static int fdbad PARAMS ((host_callback *, int));
static int wrap PARAMS ((host_callback *, int));

/* Set the callback copy of errno from what we see now.  */

static int 
wrap (p, val)
     host_callback *p;
     int val;
{
  p->last_errno = errno;
  return val;
}

/* Make sure the FD provided is ok.  If not, return non-zero
   and set errno. */

static int 
fdbad (p, fd)
     host_callback *p;
     int fd;
{
  if (fd < 0 || fd > MAX_CALLBACK_FDS || !p->fdopen[fd])
    {
      p->last_errno = EINVAL;
      return -1;
    }
  return 0;
}

static int 
fdmap (p, fd)
     host_callback *p;
     int fd;
{
  return p->fdmap[fd];
}

static int 
os_close (p, fd)
     host_callback *p;
     int fd;
{
  int result;

  result = fdbad (p, fd);
  if (result)
    return result;
  result = wrap (p, close (fdmap (p, fd)));
  if(result == 0 && !p->alwaysopen[fd])
    p->fdopen[fd] = 0;

  return result;
}


/* taken from gdb/util.c - should be in a library */


#if defined(__GO32__) || defined (_WIN32)
int
os_poll_quit (p)
     host_callback *p;
{
#ifndef _MSC_VER
  if (kbhit ())
    {
      int k = getkey ();
      if (k == 1)
	{
	  return 1;
	}
      else if (k == 2)
	{
	  return 1;
	}
      else 
	{
	  p->eprintf (p, "CTRL-A to quit, CTRL-B to quit harder\n");
	}
    }
#else /* !_MSC_VER */
  /* NB - this will not compile! */
  int k = win32pollquit();
  if (k == 1)
    return 1;
  else if (k == 2)
    return 1;
#endif /* !_MSC_VER */
  return 0;
}
#else
#define os_poll_quit 0
#endif /* defined(__GO32__) || defined(_WIN32) */

static int 
os_get_errno (p)
     host_callback *p;
{
  return host_to_target_errno (p->last_errno);
}


static int 
os_isatty (p, fd)
     host_callback *p;
     int fd;
{
  int result;

  result = fdbad (p, fd);
  if (result)
    return result;
  result = wrap (p, isatty (fdmap (p, fd)));

  return result;
}

static int 
os_lseek (p, fd, off, way)
     host_callback *p;
     int fd;
     long off;
     int way;
{
  int result;

  result = fdbad (p, fd);
  if (result)
    return result;
  result = lseek (fdmap (p, fd), off, way);
  return result;
}

static int 
os_open (p, name, flags)
     host_callback *p;
     const char *name;
{
  int i;
  for (i = 0; i < MAX_CALLBACK_FDS; i++)
    {
      if (!p->fdopen[i])
	{
	  int f = open (name, target_to_host_open (flags), 0644);
	  if (f < 0)
	    {
	      p->last_errno = errno;
	      return f;
	    }
	  p->fdopen[i] = 1;
	  p->fdmap[i] = f;
	  return i;
	}
    }
  p->last_errno = EMFILE;
  return -1;
}

static int 
os_read (p, fd, buf, len)
     host_callback *p;
     int fd;
     char *buf;
     int len;
{
  int result;

  result = fdbad (p, fd);
  if (result)
    return result;
  result = wrap (p, read (fdmap (p, fd), buf, len));
  return result;
}

static int 
os_read_stdin (p, buf, len)
     host_callback *p;
     char *buf;
     int len;
{
  return wrap (p, read (0, buf, len));
}

static int 
os_write (p, fd, buf, len)
     host_callback *p;
     int fd;
     const char *buf;
     int len;
{
  int result;

  result = fdbad (p, fd);
  if (result)
    return result;
  result = wrap (p, write (fdmap (p, fd), buf, len));
  return result;
}

static int 
os_write_stdout (p, buf, len)
     host_callback *p;
     const char *buf;
     int len;
{
  return fwrite(buf, 1, len, stdout);
}

static void
os_flush_stdout (p)
     host_callback *p;
{
  fflush (stdout);
}

static int 
os_write_stderr (p, buf, len)
     host_callback *p;
     const char *buf;
     int len;
{
  return fwrite(buf, 1, len, stderr);
}

static void
os_flush_stderr (p)
     host_callback *p;
{
  fflush (stderr);
}

static int 
os_rename (p, f1, f2)
     host_callback *p;
     const char *f1;
     const char *f2;
{
  return wrap (p, rename (f1, f2));
}


static int
os_system (p, s)
     host_callback *p;
     const char *s;
{
  return wrap (p, system (s));
}

static long 
os_time (p, t)
     host_callback *p;
     long *t;
{
  return wrap (p, time (t));
}


static int 
os_unlink (p, f1)
     host_callback *p;
     const char *f1;
{
  return wrap (p, unlink (f1));
}


static int
os_shutdown (p)
     host_callback *p;
{
  int i;
  for (i = 0; i < MAX_CALLBACK_FDS; i++)
    {
      if (p->fdopen[i] && !p->alwaysopen[i]) {
	close (p->fdmap[i]);
	p->fdopen[i] = 0;
      }
    }
  return 1;
}

static int
os_init(p)
     host_callback *p;
{
  int i;
  os_shutdown (p);
  for (i= 0; i < 3; i++)
    {
      p->fdmap[i] = i;
      p->fdopen[i] = 1;
      p->alwaysopen[i] = 1;
    }
  return 1;
}

/* DEPRECIATED */

/* VARARGS */
static void
#ifdef ANSI_PROTOTYPES
os_printf_filtered (host_callback *p, const char *format, ...)
#else
os_printf_filtered (p, va_alist)
     host_callback *p;
     va_dcl
#endif
{
  va_list args;
#ifdef ANSI_PROTOTYPES
  va_start (args, format);
#else
  char *format;

  va_start (args);
  format = va_arg (args, char *);
#endif

  vfprintf (stdout, format, args);
  va_end (args);
}

/* VARARGS */
static void
#ifdef ANSI_PROTOTYPES
os_vprintf_filtered (host_callback *p, const char *format, va_list args)
#else
os_vprintf_filtered (p, format, args)
     host_callback *p;
     const char *format;
     va_list args;
#endif
{
  vprintf (format, args);
}

/* VARARGS */
static void
#ifdef ANSI_PROTOTYPES
os_evprintf_filtered (host_callback *p, const char *format, va_list args)
#else
os_evprintf_filtered (p, format, args)
     host_callback *p;
     const char *format;
     va_list args;
#endif
{
  vfprintf (stderr, format, args);
}

/* VARARGS */
static void
#ifdef ANSI_PROTOTYPES
os_error (host_callback *p, const char *format, ...)
#else
os_error (p, va_alist)
     host_callback *p;
     va_dcl
#endif
{
  va_list args;
#ifdef ANSI_PROTOTYPES
  va_start (args, format);
#else
  char *format;

  va_start (args);
  format = va_arg (args, char *);
#endif

  vfprintf (stderr, format, args);
  fprintf (stderr, "\n");

  va_end (args);
  exit (1);
}

host_callback default_callback =
{
  os_close,
  os_get_errno,
  os_isatty,
  os_lseek,
  os_open,
  os_read,
  os_read_stdin,
  os_rename,
  os_system,
  os_time,
  os_unlink,
  os_write,
  os_write_stdout,
  os_flush_stdout,
  os_write_stderr,
  os_flush_stderr,

  os_poll_quit,

  os_shutdown,
  os_init,

  os_printf_filtered,  /* depreciated */

  os_vprintf_filtered,
  os_evprintf_filtered,
  os_error,

  0, 		/* last errno */

  { 0, },	/* fdmap */
  { 0, },	/* fdopen */
  { 0, },	/* alwaysopen */

  HOST_CALLBACK_MAGIC,
};

/* FIXME: Need to add hooks so target can tweak as necessary.  */

/* FIXME: struct stat conversion is missing.  */

/* FIXME: sort tables if large.
   Alternatively, an obvious improvement for errno conversion is
   to machine generate a function with a large switch().  */

int
host_to_target_errno (host_val)
     int host_val;
{
  target_defs_map *m;

  for (m = &errno_map[0]; m->host_val; ++m)
    if (m->host_val == host_val)
      return m->target_val;

  /* ??? Which error to return in this case is up for grabs.
     Note that some missing values may have standard alternatives.
     For now return 0 and require caller to deal with it.  */
  return 0;
}

/* Given a set of target bitmasks for the open system call,
   return the host equivalent.
   Mapping open flag values is best done by looping so there's no need
   to machine generate this function.  */

int
target_to_host_open (target_val)
     int target_val;
{
  int host_val = 0;
  target_defs_map *m;

  for (m = &open_map[0]; m->host_val != -1; ++m)
    {
      switch (m->target_val)
	{
	  /* O_RDONLY can be (and usually is) 0 which needs to be treated
	     specially.  */
	case TARGET_O_RDONLY :
	case TARGET_O_WRONLY :
	case TARGET_O_RDWR :
	  if ((target_val & (TARGET_O_RDONLY | TARGET_O_WRONLY | TARGET_O_RDWR))
	      == m->target_val)
	    host_val |= m->host_val;
	  break;
	default :
	  if ((m->target_val & target_val) == m->target_val)
	    host_val |= m->host_val;
	  break;
	}
    }

  return host_val;
}
