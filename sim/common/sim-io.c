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

#include "engine.h"

/* See the file include/callbacks.h for a description */


INLINE_SIM_IO(int)
sim_io_init(engine *system)
{
  return system->callback->init (system->callback);
}


INLINE_SIM_IO(int)
sim_io_shutdown(engine *system)
{
  return system->callback->shutdown (system->callback);
}


INLINE_SIM_IO(int)
sim_io_unlink(engine *system,
	      const char *f1)
{
  return system->callback->unlink (system->callback, f1);
}


INLINE_SIM_IO(long)
sim_io_time(engine *system,
	    long *t)
{
  return system->callback->time (system->callback, t);
}


INLINE_SIM_IO(int)
sim_io_system(engine *system, const char *s)
{
  return system->callback->system (system->callback, s);
}


INLINE_SIM_IO(int)
sim_io_rename(engine *system,
	      const char *f1,
	      const char *f2)
{
  return system->callback->rename (system->callback, f1, f2);
}


INLINE_SIM_IO(int)
sim_io_write_stdout(engine *system,
		    const char *buf,
		    int len)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    return system->callback->write_stdout (system->callback, buf, len);
    break;
  case DONT_USE_STDIO:
    return system->callback->write (system->callback, 1, buf, len);
    break;
  default:
    sim_io_error (system, "sim_io_write_stdout: unaccounted switch\n");
    break;
  }
  return 0;
}


INLINE_SIM_IO(int)
sim_io_flush_stdout(engine *system)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    return system->callback->flush_stdout (system->callback);
    break;
  case DONT_USE_STDIO:
    break;
  default:
    sim_io_error (system, "sim_io_flush_stdout: unaccounted switch\n");
    break;
  }
}


INLINE_SIM_IO(int)
sim_io_write_stderr(engine *system,
		    const char *buf,
		    int len)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    return system->callback->write_stderr (system->callback, buf, len);
    break;
  case DONT_USE_STDIO:
    return system->callback->write (system->callback, 2, buf, len);
    break;
  default:
    sim_io_error (system, "sim_io_write_stderr: unaccounted switch\n");
    break;
  }
  return 0;
}


INLINE_SIM_IO(int)
sim_io_flush_stderr(engine *system)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    return system->callback->flush_stderr (system->callback);
    break;
  case DONT_USE_STDIO:
    break;
  default:
    sim_io_error (system, "sim_io_flush_stderr: unaccounted switch\n");
    break;
  }
}


INLINE_SIM_IO(int)
sim_io_write(engine *system,
	     int fd,
	     const char *buf,
	     int len)
{
  return system->callback->write (system->callback, fd, buf, len);
}


INLINE_SIM_IO(int)
sim_io_read_stdin(engine *system,
		  char *buf,
		  int len)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    return system->callback->read_stdin (system->callback, buf, len);
    break;
  case DONT_USE_STDIO:
    return system->callback->read (system->callback, 0, buf, len);
    break;
  default:
    sim_io_error (system, "sim_io_read_stdin: unaccounted switch\n");
    break;
  }
  return 0;
}


INLINE_SIM_IO(int)
sim_io_read(engine *system, int fd,
	    char *buf,
	    int len)
{
  return system->callback->read(system->callback, fd, buf, len);
}


INLINE_SIM_IO(int)
sim_io_open(engine *system,
	    const char *name,
	    int flags)
{
  return system->callback->open (system->callback, name, flags);
}


INLINE_SIM_IO(int)
sim_io_lseek(engine *system,
	     int fd,
	     long off,
	     int way)
{
  return system->callback->lseek (system->callback, fd, off, way);
}


INLINE_SIM_IO(int)
sim_io_isatty(engine *system,
	      int fd)
{
  return system->callback->isatty (system->callback, fd);
}


INLINE_SIM_IO(int)
sim_io_get_errno(engine *system)
{
  return system->callback->get_errno (system->callback);
}


INLINE_SIM_IO(int)
sim_io_close(engine *system,
	     int fd)
{
  return system->callback->close (system->callback, fd);
}


INLINE_SIM_IO(void)
sim_io_printf(engine *system,
	      const char *fmt,
	      ...)
{
  va_list ap;
  va_start(ap, fmt);
  system->callback->vprintf_filtered (system->callback, fmt, ap);
  va_end(ap);
}


INLINE_SIM_IO(void)
sim_io_vprintf(engine *system,
	       const char *fmt,
	       va_list ap)
{
  system->callback->vprintf_filtered (system->callback, fmt, ap);
}


INLINE_SIM_IO(void)
sim_io_eprintf(engine *system,
	      const char *fmt,
	      ...)
{
  va_list ap;
  va_start(ap, fmt);
  system->callback->evprintf_filtered (system->callback, fmt, ap);
  va_end(ap);
}


INLINE_SIM_IO(void)
sim_io_evprintf(engine *system,
		const char *fmt,
		va_list ap)
{
  system->callback->evprintf_filtered (system->callback, fmt, ap);
}


INLINE_SIM_IO(void)
sim_io_error(engine *system,
	     const char *fmt,
	     ...)
{
  char buf[1000];
  va_list ap;
  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);

  if (strlen(buf) >= sizeof(buf))
    abort();

  system->callback->error (system->callback, "%s", buf);
}



#endif
