/* Simulator memory option handling.
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.
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

#include "sim-main.h"
#include "sim-assert.h"
#include "sim-options.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

/* "core" command line options. */

enum {
  OPTION_MEMORY_DELETE = OPTION_START,
  OPTION_MEMORY_REGION,
  OPTION_MEMORY_SIZE,
  OPTION_MEMORY_INFO,
  OPTION_MEMORY_ALIAS,
  OPTION_MEMORY_CLEAR,
};

static DECLARE_OPTION_HANDLER (memory_option_handler);

static const OPTION memory_options[] =
{
  { {"memory-delete", optional_argument, NULL, OPTION_MEMORY_DELETE },
      '\0', "ADDRESS", "Delete memory at ADDRESS (all addresses)",
      memory_option_handler },
  { {"delete-memory", required_argument, NULL, OPTION_MEMORY_DELETE },
      '\0', "ADDRESS", NULL,
      memory_option_handler },

  { {"memory-region", required_argument, NULL, OPTION_MEMORY_REGION },
      '\0', "ADDRESS,SIZE[,MODULO]", "Add a memory region",
      memory_option_handler },

  { {"memory-alias", required_argument, NULL, OPTION_MEMORY_ALIAS },
      '\0', "ADDRESS,SIZE{,ADDRESS}", "Add memory shadow",
      memory_option_handler },

  { {"memory-size", required_argument, NULL, OPTION_MEMORY_SIZE },
      '\0', "SIZE", "Add memory at address zero",
      memory_option_handler },

  { {"memory-clear", no_argument, NULL, OPTION_MEMORY_CLEAR },
      '\0', NULL, "Clear all memory regions",
      memory_option_handler },

  { {"memory-info", no_argument, NULL, OPTION_MEMORY_INFO },
      '\0', NULL, "List configurable memory regions",
      memory_option_handler },
  { {"info-memory", no_argument, NULL, OPTION_MEMORY_INFO },
      '\0', NULL, NULL,
      memory_option_handler },

  { {NULL, no_argument, NULL, 0}, '\0', NULL, NULL, NULL }
};


static sim_memopt *
do_memopt_add (sd, addr, nr_bytes, modulo, entry, buffer)
     SIM_DESC sd;
     address_word addr;
     address_word nr_bytes;
     unsigned modulo;
     sim_memopt **entry;
     void *buffer;
{
  sim_core_attach (sd, NULL,
		   attach_raw_memory, access_read_write_exec, 0,
		   addr, nr_bytes, modulo, NULL, buffer);
  while ((*entry) != NULL)
    entry = &(*entry)->next;
  (*entry) = ZALLOC (sim_memopt);
  (*entry)->addr = addr;
  (*entry)->nr_bytes = nr_bytes;
  (*entry)->modulo = modulo;
  (*entry)->buffer = buffer;
  return (*entry);
}

static SIM_RC
do_memopt_delete (sd, addr)
     SIM_DESC sd;
     address_word addr;
{
  sim_memopt **entry = &STATE_MEMOPT (sd);
  sim_memopt *alias;
  while ((*entry) != NULL && (*entry)->addr != addr)
    entry = &(*entry)->next;
  if ((*entry) == NULL)
    {
      sim_io_eprintf (sd, "Memory at 0x%lx not found, not deleted\n",
		      (long) addr);
      return SIM_RC_FAIL;
    }
  /* delete any buffer */
  if ((*entry)->buffer != NULL)
    zfree ((*entry)->buffer);
  /* delete it and its aliases */
  alias = *entry;
  *entry = alias->next;
  while (alias != NULL)
    {
      sim_memopt *dead = alias;
      alias = alias->alias;
      sim_core_detach (sd, NULL, attach_raw_memory, 0, dead->addr);
      zfree (dead);
    }
  return SIM_RC_OK;
}


static SIM_RC
memory_option_handler (sd, opt, arg, is_command)
     SIM_DESC sd;
     int opt;
     char *arg;
     int is_command;
{
  switch (opt)
    {

    case OPTION_MEMORY_DELETE:
      if (arg == NULL)
	{
	  while (STATE_MEMOPT (sd) != NULL)
	    do_memopt_delete (sd, STATE_MEMOPT (sd)->addr);
	  return SIM_RC_OK;
	}
      else
	{
	  address_word addr = strtoul (arg, NULL, 0);
	  return do_memopt_delete (sd, addr);
	}
    
    case OPTION_MEMORY_REGION:
      {
	char *chp = arg;
	address_word addr = 0;
	address_word nr_bytes = 0;
	unsigned modulo = 0;
	/* parse the arguments */
	addr = strtoul (chp, &chp, 0);
	if (*chp != ',')
	  {
	    sim_io_eprintf (sd, "Missing size for memory-region\n");
	    return SIM_RC_FAIL;
	  }
	chp++;
	nr_bytes = strtoul (chp, &chp, 0);
	if (*chp == ',')
	  modulo = strtoul (chp + 1, NULL, 0);
	/* try to attach/insert it */
	do_memopt_add (sd, addr, nr_bytes, modulo, &STATE_MEMOPT (sd), NULL);
	return SIM_RC_OK;
      }

    case OPTION_MEMORY_ALIAS:
      {
	char *chp = arg;
	address_word addr = 0;
	address_word nr_bytes = 0;
	sim_memopt *entry;
	/* parse the arguments */
	addr = strtoul (chp, &chp, 0);
	if (*chp != ',')
	  {
	    sim_io_eprintf (sd, "Missing size for memory-region\n");
	    return SIM_RC_FAIL;
	  }
	chp++;
	nr_bytes = strtoul (chp, &chp, 0);
	/* try to attach/insert the main record */
	entry = do_memopt_add (sd, addr, nr_bytes, 0/*modulo*/,
			       &STATE_MEMOPT (sd), zalloc (nr_bytes));
	/* now attach all the aliases */
	while (*chp == ',')
	  {
	    address_word alias;
	    chp++;
	    alias = strtoul (chp, &chp, 0);
	    do_memopt_add (sd, alias, nr_bytes, 0/*modulo*/,
			   &entry->alias, entry->buffer);
	  }
	return SIM_RC_OK;
      }

    case OPTION_MEMORY_SIZE:
      {
	address_word nr_bytes = strtoul (arg, NULL, 0);
	/* try to attach/insert it */
	do_memopt_add (sd, 0/*addr*/, nr_bytes, 0/*modulo*/,
		       &STATE_MEMOPT (sd), NULL);
	return SIM_RC_OK;
      }

    case OPTION_MEMORY_CLEAR:
      {
	sim_memopt *entry;
	for (entry = STATE_MEMOPT (sd); entry != NULL; entry = entry->next)
	  {
	    sim_memopt *alias;
	    for (alias = entry; alias != NULL; alias = alias->next)
	      {
		unsigned8 zero = 0;
		address_word nr_bytes;
		if (alias->modulo != 0)
		  nr_bytes = alias->modulo;
		else
		  nr_bytes = alias->nr_bytes;
		sim_core_write_buffer (sd, NULL, sim_core_write_map,
				       &zero,
				       alias->addr + nr_bytes,
				       sizeof (zero));
		
	      }
	  }
	return SIM_RC_OK;
	break;
      }

    case OPTION_MEMORY_INFO:
      {
	sim_memopt *entry;
	sim_io_printf (sd, "Memory maps:\n");
	for (entry = STATE_MEMOPT (sd); entry != NULL; entry = entry->next)
	  {
	    sim_memopt *alias;
	    sim_io_printf (sd, " memory");
	    if (entry->alias == NULL)
	      {
		sim_io_printf (sd, " region 0x%08lx,0x%lx",
			       (long) entry->addr,
			       (long) entry->nr_bytes);
		if (entry->modulo != 0)
		  sim_io_printf (sd, ",0x%lx", (long) entry->modulo);
	      }
	    else
	      {
		sim_io_printf (sd, " alias 0x%08lx,0x%lx",
			       (long) entry->addr,
			       (long) entry->nr_bytes);
		for (alias = entry->alias;
		     alias != NULL;
		     alias = alias->next)
		  sim_io_printf (sd, ",0x%08lx", alias->addr);
	      }
	    sim_io_printf (sd, "\n");
	  }
	return SIM_RC_OK;
	break;
      }

    default:
      sim_io_eprintf (sd, "Unknown watch option %d\n", opt);
      return SIM_RC_FAIL;

    }

  return SIM_RC_FAIL;
}


/* "memory" module install handler.

   This is called via sim_module_install to install the "memory" subsystem
   into the simulator.  */

static MODULE_INIT_FN sim_memory_init;
static MODULE_UNINSTALL_FN sim_memory_uninstall;

SIM_RC
sim_memopt_install (SIM_DESC sd)
{
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);
  sim_add_option_table (sd, memory_options);
  sim_module_add_uninstall_fn (sd, sim_memory_uninstall);
  sim_module_add_init_fn (sd, sim_memory_init);
  return SIM_RC_OK;
}


/* Uninstall the "memory" subsystem from the simulator.  */

static void
sim_memory_uninstall (SIM_DESC sd)
{
  /* FIXME: free buffers, etc. */
}


static SIM_RC
sim_memory_init (SIM_DESC sd)
{
  /* FIXME: anything needed? */
  return SIM_RC_OK;
}
