/*  This file is part of the program psim.

    Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#ifndef _SIM_IO_C_
#define _SIM_IO_C_

#include "sim-state.h"

/* See the file include/callbacks.h for a description */


INLINE_SIM_IO(int)
sim_io_init(SIM_DESC sd)
{
  return sd->callback->init (sd->callback);
}


INLINE_SIM_IO(int)
sim_io_shutdown(SIM_DESC sd)
{
  return sd->callback->shutdown (sd->callback);
}


INLINE_SIM_IO(int)
sim_io_unlink(SIM_DESC sd,
	      const char *f1)
{
  return sd->callback->unlink (sd->callback, f1);
}


INLINE_SIM_IO(long)
sim_io_time(SIM_DESC sd,
	    long *t)
{
  return sd->callback->time (sd->callback, t);
}


INLINE_SIM_IO(int)
sim_io_system(SIM_DESC sd, const char *s)
{
  return sd->callback->system (sd->callback, s);
}


INLINE_SIM_IO(int)
sim_io_rename(SIM_DESC sd,
	      const char *f1,
	      const char *f2)
{
  return sd->callback->rename (sd->callback, f1, f2);
}


INLINE_SIM_IO(int)
sim_io_write_stdout(SIM_DESC sd,
		    const char *buf,
		    int len)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    return sd->callback->write_stdout (sd->callback, buf, len);
    break;
  case DONT_USE_STDIO:
    return sd->callback->write (sd->callback, 1, buf, len);
    break;
  default:
    sim_io_error (sd, "sim_io_write_stdout: unaccounted switch\n");
    break;
  }
  return 0;
}


INLINE_SIM_IO(void)
sim_io_flush_stdout(SIM_DESC sd)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    sd->callback->flush_stdout (sd->callback);
    break;
  case DONT_USE_STDIO:
    break;
  default:
    sim_io_error (sd, "sim_io_flush_stdout: unaccounted switch\n");
    break;
  }
}


INLINE_SIM_IO(int)
sim_io_write_stderr(SIM_DESC sd,
		    const char *buf,
		    int len)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    return sd->callback->write_stderr (sd->callback, buf, len);
    break;
  case DONT_USE_STDIO:
    return sd->callback->write (sd->callback, 2, buf, len);
    break;
  default:
    sim_io_error (sd, "sim_io_write_stderr: unaccounted switch\n");
    break;
  }
  return 0;
}


INLINE_SIM_IO(void)
sim_io_flush_stderr(SIM_DESC sd)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    sd->callback->flush_stderr (sd->callback);
    break;
  case DONT_USE_STDIO:
    break;
  default:
    sim_io_error (sd, "sim_io_flush_stderr: unaccounted switch\n");
    break;
  }
}


INLINE_SIM_IO(int)
sim_io_write(SIM_DESC sd,
	     int fd,
	     const char *buf,
	     int len)
{
  return sd->callback->write (sd->callback, fd, buf, len);
}


INLINE_SIM_IO(int)
sim_io_read_stdin(SIM_DESC sd,
		  char *buf,
		  int len)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    return sd->callback->read_stdin (sd->callback, buf, len);
    break;
  case DONT_USE_STDIO:
    return sd->callback->read (sd->callback, 0, buf, len);
    break;
  default:
    sim_io_error (sd, "sim_io_read_stdin: unaccounted switch\n");
    break;
  }
  return 0;
}


INLINE_SIM_IO(int)
sim_io_read(SIM_DESC sd, int fd,
	    char *buf,
	    int len)
{
  return sd->callback->read (sd->callback, fd, buf, len);
}


INLINE_SIM_IO(int)
sim_io_open(SIM_DESC sd,
	    const char *name,
	    int flags)
{
  return sd->callback->open (sd->callback, name, flags);
}


INLINE_SIM_IO(int)
sim_io_lseek(SIM_DESC sd,
	     int fd,
	     long off,
	     int way)
{
  return sd->callback->lseek (sd->callback, fd, off, way);
}


INLINE_SIM_IO(int)
sim_io_isatty(SIM_DESC sd,
	      int fd)
{
  return sd->callback->isatty (sd->callback, fd);
}


INLINE_SIM_IO(int)
sim_io_get_errno(SIM_DESC sd)
{
  return sd->callback->get_errno (sd->callback);
}


INLINE_SIM_IO(int)
sim_io_close(SIM_DESC sd,
	     int fd)
{
  return sd->callback->close (sd->callback, fd);
}


INLINE_SIM_IO(void)
sim_io_printf(SIM_DESC sd,
	      const char *fmt,
	      ...)
{
  va_list ap;
  va_start(ap, fmt);
  sd->callback->vprintf_filtered (sd->callback, fmt, ap);
  va_end(ap);
}


INLINE_SIM_IO(void)
sim_io_vprintf(SIM_DESC sd,
	       const char *fmt,
	       va_list ap)
{
  sd->callback->vprintf_filtered (sd->callback, fmt, ap);
}


INLINE_SIM_IO(void)
sim_io_eprintf(SIM_DESC sd,
	      const char *fmt,
	      ...)
{
  va_list ap;
  va_start(ap, fmt);
  sd->callback->evprintf_filtered (sd->callback, fmt, ap);
  va_end(ap);
}


INLINE_SIM_IO(void)
sim_io_evprintf(SIM_DESC sd,
		const char *fmt,
		va_list ap)
{
  sd->callback->evprintf_filtered (sd->callback, fmt, ap);
}


INLINE_SIM_IO(void)
sim_io_error(SIM_DESC sd,
	     const char *fmt,
	     ...)
{
  if (sd == NULL || sd->callback == NULL) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf (stderr, fmt, ap);
    va_end(ap);
    abort ();
  }
  else {
    va_list ap;
    va_start(ap, fmt);
    sd->callback->evprintf_filtered (sd->callback, fmt, ap);
    va_end(ap);
    sd->callback->error (sd->callback, "");
  }
}

#endif
