/* Miscellaneous simulator utilities.
   Copyright (C) 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef SIM_UTILS_H
#define SIM_UTILS_H

/* Memory management with an allocator that clears memory before use. */

void *zalloc (unsigned long size);

#define ZALLOC(TYPE) (TYPE*)zalloc(sizeof (TYPE))

void zfree(void*);

/* Turn VALUE into a string with commas.  */
char *sim_add_commas (char *, int, unsigned long);

/* Utilities for elapsed time reporting.  */

/* Opaque type, known only inside sim_elapsed_time_foo fns.  */
typedef unsigned long SIM_ELAPSED_TIME;

/* Get reference point for future call to sim_time_elapsed.  */
SIM_ELAPSED_TIME sim_elapsed_time_get (void);

/* Elapsed time in milliseconds since START.  */
unsigned long sim_elapsed_time_since (SIM_ELAPSED_TIME start);

/* Utilities for manipulating the load image.  */

SIM_RC sim_analyze_program (SIM_DESC sd, char *prog_name,
			    struct _bfd *prog_bfd);

/* Load program PROG into the simulator.
   If PROG_BFD is non-NULL, the file has already been opened.
   If VERBOSE_P is non-zero statistics are printed of each loaded section
   and the transfer rate (for consistency with gdb).
   If this fails an error message is printed and NULL is returned.
   If it succeeds the bfd is returned.  */

struct _bfd *sim_load_file (SIM_DESC sd, const char *myname,
			    host_callback *callback, char *prog,
			    struct _bfd *prog_bfd, int verbose_p);

/* These are defined in callback.c as cover functions to the vprintf
   callbacks.  */

void sim_cb_printf (host_callback *, const char *, ...);
void sim_cb_eprintf (host_callback *, const char *, ...);

#endif
