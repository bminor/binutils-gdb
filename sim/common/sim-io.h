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


#ifndef _SIM_IO_H_
#define _SIM_IO_H_


/* See the file include/callbacks.h for a description */

INLINE_SIM_IO\
(int) sim_io_init
(engine *system);

INLINE_SIM_IO\
(int) sim_io_shutdown
(engine *system);

INLINE_SIM_IO\
(int) sim_io_unlink
(engine *system, const char *);

INLINE_SIM_IO\
(long) sim_io_time
(engine *system, long *);

INLINE_SIM_IO\
(int) sim_io_system
(engine *system, const char *);

INLINE_SIM_IO\
(int) sim_io_rename
(engine *system, const char *, const char *);

INLINE_SIM_IO\
(int) sim_io_write_stdout
(engine *system, const char *, int);

INLINE_SIM_IO\
(int) sim_io_flush_stdout
(engine *system);

INLINE_SIM_IO\
(int) sim_io_write_stderr
(engine *system, const char *, int);

INLINE_SIM_IO\
(int) sim_io_flush_stderr
(engine *system);

INLINE_SIM_IO\
(int) sim_io_write
(engine *system, int, const char *, int);

INLINE_SIM_IO\
(int) sim_io_read_stdin
(engine *system, char *, int);

INLINE_SIM_IO\
(int) sim_io_read
(engine *system, int, char *, int);

INLINE_SIM_IO\
(int) sim_io_open
(engine *system, const char *, int);

INLINE_SIM_IO\
(int) sim_io_lseek
(engine *system, int, long, int);

INLINE_SIM_IO\
(int) sim_io_isatty
(engine *system, int);

INLINE_SIM_IO\
(int) sim_io_get_errno
(engine *system);

INLINE_SIM_IO\
(int) sim_io_close
(engine *system, int);

INLINE_SIM_IO\
(void) sim_io_printf
(engine *system,
 const char *fmt,
 ...) __attribute__ ((format (printf, 2, 3)));

INLINE_SIM_IO\
(void) sim_io_vprintf
(engine *system,
 const char *fmt,
 va_list ap);

INLINE_SIM_IO\
(void) sim_io_eprintf
(engine *system,
 const char *fmt,
 ...) __attribute__ ((format (printf, 2, 3)));

INLINE_SIM_IO\
(void) sim_io_evprintf
(engine *system,
 const char *fmt,
 va_list ap);

INLINE_SIM_IO\
(void) sim_io_error
(engine *system,
 const char *fmt,
 ...) __attribute__ ((format (printf, 2, 3)));

#endif
