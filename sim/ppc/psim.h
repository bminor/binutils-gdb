/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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


#ifndef _PSIM_H_
#define _PSIM_H_

#ifndef INLINE_PSIM
#define INLINE_PSIM
#endif


#include "basics.h"

/* the system object */

typedef struct _psim psim;


/* when the `system' stops, find out why.  FIXME - at this point this
   is really a bit puzzling.  After all, how can there be a status
   when there several processors involved */

typedef struct _psim_status {
  int cpu_nr;
  stop_reason reason;
  int signal;
  unsigned_word program_counter;
} psim_status;


/* create a new simulator */

extern psim *psim_create
(const char *file_name,
 int nr_processors);


/* Given the created simulator load either its low or high memory */

extern void psim_load
(psim *system);

extern void psim_stack
(psim *system,
 char **argv,
 char **envp);


/* Run/stop the system */

extern void psim_step
(psim *system);

extern void psim_run
(psim *system);

extern void psim_run_until_stop
(psim *system,
 volatile int *stop);

extern void psim_restart
(psim *system,
 int cpu_nr);

extern void psim_halt
(psim *system,
 int cpu_nr,
 unsigned_word cia,
 stop_reason reason,
 int signal);

extern psim_status psim_get_status
(psim *system);


/* reveal the internals of the simulation, giving access to the cpu's */

extern cpu *psim_cpu
(psim *system,
 int cpu_nr);


/* manipulate the state (registers or memory) of a processor within
   the system.  In the case of memory, the read/write is performed
   using the specified processors address translation tables */

extern void psim_read_register
(psim *system,
 int which_processor,
 void *host_ordered_buf,
 const char reg[],
 transfer_mode mode);

extern void psim_write_register
(psim *system,
 int which_processor,
 const void *host_ordered_buf,
 const char reg[],
 transfer_mode mode);

extern unsigned psim_read_memory
(psim *system,
 int which_processor,
 void *buf,
 unsigned_word vaddr,
 unsigned len,
 transfer_mode mode);

extern unsigned psim_write_memory
(psim *system,
 int which_processor,
 const void *buf,
 unsigned_word vaddr,
 unsigned len,
 transfer_mode mode,
 int violate_read_only_section);

extern void psim_print_info
(psim *system,
 int verbose);

#endif /* _PSIM_H_ */
