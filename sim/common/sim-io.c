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


#include "sim-main.h"
#include "sim-io.h"
#include "targ-vals.h"

/* See the file include/callbacks.h for a description */


int
sim_io_init(SIM_DESC sd)
{
  return STATE_CALLBACK (sd)->init (STATE_CALLBACK (sd));
}


int
sim_io_shutdown(SIM_DESC sd)
{
  return STATE_CALLBACK (sd)->shutdown (STATE_CALLBACK (sd));
}


int
sim_io_unlink(SIM_DESC sd,
	      const char *f1)
{
  return STATE_CALLBACK (sd)->unlink (STATE_CALLBACK (sd), f1);
}


long
sim_io_time(SIM_DESC sd,
	    long *t)
{
  return STATE_CALLBACK (sd)->time (STATE_CALLBACK (sd), t);
}


int
sim_io_system(SIM_DESC sd, const char *s)
{
  return STATE_CALLBACK (sd)->system (STATE_CALLBACK (sd), s);
}


int
sim_io_rename(SIM_DESC sd,
	      const char *f1,
	      const char *f2)
{
  return STATE_CALLBACK (sd)->rename (STATE_CALLBACK (sd), f1, f2);
}


int
sim_io_write_stdout(SIM_DESC sd,
		    const char *buf,
		    int len)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    return STATE_CALLBACK (sd)->write_stdout (STATE_CALLBACK (sd), buf, len);
    break;
  case DONT_USE_STDIO:
    return STATE_CALLBACK (sd)->write (STATE_CALLBACK (sd), 1, buf, len);
    break;
  default:
    sim_io_error (sd, "sim_io_write_stdout: unaccounted switch\n");
    break;
  }
  return 0;
}


void
sim_io_flush_stdout(SIM_DESC sd)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    STATE_CALLBACK (sd)->flush_stdout (STATE_CALLBACK (sd));
    break;
  case DONT_USE_STDIO:
    break;
  default:
    sim_io_error (sd, "sim_io_flush_stdout: unaccounted switch\n");
    break;
  }
}


int
sim_io_write_stderr(SIM_DESC sd,
		    const char *buf,
		    int len)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    return STATE_CALLBACK (sd)->write_stderr (STATE_CALLBACK (sd), buf, len);
    break;
  case DONT_USE_STDIO:
    return STATE_CALLBACK (sd)->write (STATE_CALLBACK (sd), 2, buf, len);
    break;
  default:
    sim_io_error (sd, "sim_io_write_stderr: unaccounted switch\n");
    break;
  }
  return 0;
}


void
sim_io_flush_stderr(SIM_DESC sd)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    STATE_CALLBACK (sd)->flush_stderr (STATE_CALLBACK (sd));
    break;
  case DONT_USE_STDIO:
    break;
  default:
    sim_io_error (sd, "sim_io_flush_stderr: unaccounted switch\n");
    break;
  }
}


int
sim_io_write(SIM_DESC sd,
	     int fd,
	     const char *buf,
	     int len)
{
  return STATE_CALLBACK (sd)->write (STATE_CALLBACK (sd), fd, buf, len);
}


int
sim_io_read_stdin(SIM_DESC sd,
		  char *buf,
		  int len)
{
  switch (CURRENT_STDIO) {
  case DO_USE_STDIO:
    return STATE_CALLBACK (sd)->read_stdin (STATE_CALLBACK (sd), buf, len);
    break;
  case DONT_USE_STDIO:
    return STATE_CALLBACK (sd)->read (STATE_CALLBACK (sd), 0, buf, len);
    break;
  default:
    sim_io_error (sd, "sim_io_read_stdin: unaccounted switch\n");
    break;
  }
  return 0;
}


int
sim_io_read(SIM_DESC sd, int fd,
	    char *buf,
	    int len)
{
  return STATE_CALLBACK (sd)->read (STATE_CALLBACK (sd), fd, buf, len);
}


int
sim_io_open(SIM_DESC sd,
	    const char *name,
	    int flags)
{
  return STATE_CALLBACK (sd)->open (STATE_CALLBACK (sd), name, flags);
}


int
sim_io_lseek(SIM_DESC sd,
	     int fd,
	     long off,
	     int way)
{
  return STATE_CALLBACK (sd)->lseek (STATE_CALLBACK (sd), fd, off, way);
}


int
sim_io_isatty(SIM_DESC sd,
	      int fd)
{
  return STATE_CALLBACK (sd)->isatty (STATE_CALLBACK (sd), fd);
}


int
sim_io_get_errno(SIM_DESC sd)
{
  return STATE_CALLBACK (sd)->get_errno (STATE_CALLBACK (sd));
}


int
sim_io_close(SIM_DESC sd,
	     int fd)
{
  return STATE_CALLBACK (sd)->close (STATE_CALLBACK (sd), fd);
}


void
sim_io_printf(SIM_DESC sd,
	      const char *fmt,
	      ...)
{
  va_list ap;
  va_start(ap, fmt);
  STATE_CALLBACK (sd)->vprintf_filtered (STATE_CALLBACK (sd), fmt, ap);
  va_end(ap);
}


void
sim_io_vprintf(SIM_DESC sd,
	       const char *fmt,
	       va_list ap)
{
  STATE_CALLBACK (sd)->vprintf_filtered (STATE_CALLBACK (sd), fmt, ap);
}


void
sim_io_eprintf(SIM_DESC sd,
	      const char *fmt,
	      ...)
{
  va_list ap;
  va_start(ap, fmt);
  STATE_CALLBACK (sd)->evprintf_filtered (STATE_CALLBACK (sd), fmt, ap);
  va_end(ap);
}


void
sim_io_evprintf(SIM_DESC sd,
		const char *fmt,
		va_list ap)
{
  STATE_CALLBACK (sd)->evprintf_filtered (STATE_CALLBACK (sd), fmt, ap);
}


void
sim_io_error(SIM_DESC sd,
	     const char *fmt,
	     ...)
{
  if (sd == NULL || STATE_CALLBACK (sd) == NULL) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf (stderr, fmt, ap);
    va_end(ap);
    fprintf (stderr, "\n");
    abort ();
  }
  else {
    va_list ap;
    va_start(ap, fmt);
    STATE_CALLBACK (sd)->evprintf_filtered (STATE_CALLBACK (sd), fmt, ap);
    va_end(ap);
    STATE_CALLBACK (sd)->error (STATE_CALLBACK (sd), "");
  }
}


void
sim_io_poll_quit(SIM_DESC sd)
{
  if (STATE_CALLBACK (sd)->poll_quit != NULL)
    if (STATE_CALLBACK (sd)->poll_quit (STATE_CALLBACK (sd)))
      sim_stop (sd);
}


static char *
sim_io_getstring(SIM_DESC sd,
		 sim_cpu *cpu,
		 address_word cia,
		 address_word target_string)
{
  int len = 0;
  char *buf;

  while (sim_core_read_1 (cpu, cia, sim_core_read_map, target_string+len) != 0)
    len++;

  buf = NZALLOC (char, len+1);
  buf[len] = '\0';
  sim_core_read_buffer (sd, cpu, sim_core_read_map, buf, target_string, len);
  return buf;
}

void
sim_io_syscalls(SIM_DESC sd,
		int syscall,
		address_word cia,
		address_word parm1,
		address_word parm2,
		address_word parm3,
		address_word parm4,
		address_word *syscall_return,
		address_word *errno_return)
{
  sim_cpu *cpu = STATE_CPU (sd, 0);
  struct host_callback_struct *callback = STATE_CALLBACK (sd);
  int len, len2, len3;
  int ret;
  char *buf;
  int fd;

  *errno_return = 0;
  *syscall_return = 0;

  switch (syscall)
    {
    case 1:			/* exit (status) */
      sim_engine_halt (sd, STATE_CPU (sd, 0), NULL, cia, sim_exited, parm1);
      break;

    case 2:			/* open (filename, flags, mode) */
      buf = sim_io_getstring (sd, cpu, cia, parm1);
      ret = *syscall_return = callback->open (callback, buf, parm2);
      if (ret < 0)
	*errno_return = callback->get_errno (callback);

      zfree (buf);
      break;

    case 3:			/* close (filedes) */
      ret = *syscall_return = callback->close (callback, parm1);
      if (ret < 0)
	*errno_return = callback->get_errno (callback);
      break;

    case 4:			/* read (filedes, buffer, len) */
      fd = parm1;
      len = parm3;
      buf = NZALLOC (char, len);

      if (fd == 0)
	len2 = sim_io_read_stdin (sd, buf, len);
      else
	len2 = sim_io_read (sd, fd, buf, len);

      if (len2 > 0)
	{
	  len3 = sim_core_write_buffer (sd, cpu, sim_core_write_map, buf, parm2,
					len);

	  if (len3 < len2)
	    sim_engine_abort (sd, cpu, cia,
			      "Could only write back %d bytes for read system call, wanted to write %d\n",
			      len3, len2);

	  *syscall_return = len2;
	}
      else
	*errno_return = callback->get_errno (callback);

      zfree (buf);
      break;

    case 5:			/* write (filedes, buffer, len) */
      fd = parm1;
      len = parm3;
      buf = NZALLOC (char, len);

      len = sim_core_read_buffer (sd, cpu, sim_core_read_map, buf, parm2, len);
      if (fd == 1)
	{
	  len2 = sim_io_write_stdout (sd, buf, len);
	  sim_io_flush_stdout (sd);
	}
      else if (fd == 2)
	{
	  len2 = sim_io_write_stderr (sd, buf, len);
	  sim_io_flush_stderr (sd);
	}
      else
	len2 = sim_io_write (sd, fd, buf, len);

      if (len2 > 0)
	*syscall_return = len2;
      else
	*errno_return = callback->get_errno (callback);

      zfree (buf);
      break;
      
    case 6:			/* lseek (filedes, offset, whence) */
      *errno_return = TARGET_ENOSYS;
      break;

    case 7:			/* unlink (filename) */
      buf = sim_io_getstring (sd, cpu, cia, parm1);
      ret = *syscall_return = callback->unlink (callback, buf);
      if (ret < 0)
	*errno_return = callback->get_errno (callback);

      zfree (buf);
      break;

    case 8:			/* getpid () */
    case 9:			/* kill (signal, pid) */
    case 10:			/* fstat (filedes, packet) */
    case 11:			/* reserved for sbrk */
    case 12:			/* argvlen () */
    case 13:			/* argv () */
    case 14:			/* chdir (dir) */
    case 15:			/* stat (filename, packet) */
    case 16:			/* chmod (filename, mode) */
    case 17:			/* utime (filename, packet) */
    case 18:			/* time (time_t *) */
      *errno_return = TARGET_ENOSYS;
      break;

    default:
      sim_engine_abort (sd, cpu, cia, "Unknown monitor call %d", syscall);
      break;
    }

  if (*errno_return)
    *syscall_return = -1;

  return;
}

