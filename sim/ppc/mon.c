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


#ifndef _MON_C_
#define _MON_C_

#ifndef STATIC_INLINE_MON
#define STATIC_INLINE_MON STATIC_INLINE
#endif

#include <string.h>

#include "basics.h"
#include "cpu.h"
#include "mon.h"

struct _cpu_mon {
  unsigned issue_count[nr_itable_entries];
  unsigned read_count;
  unsigned write_count;
};

struct _mon {
  int nr_cpus;
  cpu_mon cpu_monitor[MAX_NR_PROCESSORS];
};


INLINE_MON mon *
mon_create(void)
{
  mon *monitor = ZALLOC(mon);
  return monitor;
}


INLINE_MON cpu_mon *
mon_cpu(mon *monitor,
	int cpu_nr)
{
  if (cpu_nr < 0 || cpu_nr >= MAX_NR_PROCESSORS)
    error("mon_cpu() - invalid cpu number\n");
  return &monitor->cpu_monitor[cpu_nr];
}


INLINE_MON void
mon_init(mon *monitor,
	 int nr_cpus)
{
  bzero(monitor, sizeof(*monitor));
  monitor->nr_cpus = nr_cpus;
}


INLINE_MON void
mon_issue(itable_index index,
	  cpu *processor, 
	  unsigned_word cia)
{
  cpu_mon *monitor = cpu_monitor(processor);
  ASSERT(index <= nr_itable_entries);
  monitor->issue_count[index] += 1;
}


INLINE_MON void
mon_read(unsigned_word ea,
	 unsigned_word ra,
	 unsigned nr_bytes,
	 cpu *processor,
	 unsigned_word cia)
{
  cpu_mon *monitor = cpu_monitor(processor);
  monitor->read_count += 1;
}


INLINE_MON void
mon_write(unsigned_word ea,
	  unsigned_word ra,
	  unsigned nr_bytes,
	  cpu *processor,
	  unsigned_word cia)
{
  cpu_mon *monitor = cpu_monitor(processor);
  monitor->write_count += 1;
}


STATIC_INLINE_MON unsigned
mon_get_number_of_insns(cpu_mon *monitor)
{
  itable_index index;
  unsigned total_insns = 0;
  for (index = 0; index < nr_itable_entries; index++)
    total_insns += monitor->issue_count[index];
  return total_insns;
}

STATIC_INLINE_MON char *
mon_add_commas(char *buf,
	       int sizeof_buf,
	       long value)
{
  int comma = 3;
  char *endbuf = buf + sizeof_buf - 1;

  *--endbuf = '\0';
  do {
    if (comma-- == 0)
      {
	*--endbuf = ',';
	comma = 2;
      }

    *--endbuf = (value % 10) + '0';
  } while ((value /= 10) != 0);

  ASSERT(endbuf >= buf);
  return endbuf;
}


INLINE_MON void
mon_print_info(mon *monitor,
	       int verbose)
{
  char buffer[20];
  int cpu_nr;
  int len_cpu;
  int len_num = 0;
  int len;

  for (cpu_nr = 0; cpu_nr < monitor->nr_cpus; cpu_nr++) {
    len = strlen (mon_add_commas(buffer,
				 sizeof(buffer),
				 mon_get_number_of_insns(&monitor->cpu_monitor[cpu_nr])));
    if (len_num < len)
      len_num = len;
  }
  
  sprintf (buffer, "%d", (int)monitor->nr_cpus + 1);
  len_cpu = strlen (buffer);

  for (cpu_nr = 0; cpu_nr < monitor->nr_cpus; cpu_nr++) {

    if (verbose > 1) {
      itable_index index;
      for (index = 0; index < nr_itable_entries; index++) {
	if (monitor->cpu_monitor[cpu_nr].issue_count[index])
	  printf_filtered("CPU #%*d executed %*s %s instruction%s.\n",
			  len_cpu, cpu_nr+1,
			  len_num, mon_add_commas(buffer,
						  sizeof(buffer),
						  monitor->cpu_monitor[cpu_nr].issue_count[index]),
			  itable[index].name,
			  (monitor->cpu_monitor[cpu_nr].issue_count[index] == 1) ? "" : "s");
      }
    }
    
    printf_filtered("CPU #%d executed %s instructions in total.\n",
		    cpu_nr+1,
		    mon_add_commas(buffer,
				   sizeof(buffer),
				   mon_get_number_of_insns(&monitor->cpu_monitor[cpu_nr])));

  }
}

#endif /* _MON_C_ */
