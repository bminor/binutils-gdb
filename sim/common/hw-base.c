/*  This file is part of the program psim.

    Copyright (C) 1994-1996, 1998, Andrew Cagney <cagney@highland.com.au>

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
#include "hw-base.h"


/* LATER: #include "hwconfig.h" */
struct hw_base_data {
  int finished_p;
  const struct hw_device_descriptor *descriptor;
};
extern const struct hw_device_descriptor dv_core_descriptor[];
extern const struct hw_device_descriptor dv_pal_descriptor[];
const struct hw_device_descriptor *hw_descriptors[] = {
  dv_core_descriptor,
  dv_pal_descriptor,
  NULL,
};

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <ctype.h>


static int
generic_hw_unit_decode (struct hw *bus,
			const char *unit,
			hw_unit *phys)
{
  memset (phys, 0, sizeof (*phys));
  if (unit == NULL)
    return 0;
  else
    {
      int nr_cells = 0;
      const int max_nr_cells = hw_unit_nr_address_cells (bus);
      while (1)
	{
	  char *end = NULL;
	  unsigned long val;
	  val = strtoul (unit, &end, 0);
	  /* parse error? */
	  if (unit == end)
	    return -1;
	  /* two many cells? */
	  if (nr_cells >= max_nr_cells)
	    return -1;
	  /* save it */
	  phys->cells[nr_cells] = val;
	  nr_cells++;
	  unit = end;
	  /* more to follow? */
	  if (isspace (*unit) || *unit == '\0')
	    break;
	  if (*unit != ',')
	    return -1;
	  unit++;
	}
      if (nr_cells < max_nr_cells) {
	/* shift everything to correct position */
	int i;
	for (i = 1; i <= nr_cells; i++)
	  phys->cells[max_nr_cells - i] = phys->cells[nr_cells - i];
	for (i = 0; i < (max_nr_cells - nr_cells); i++)
	  phys->cells[i] = 0;
      }
      phys->nr_cells = max_nr_cells;
      return max_nr_cells;
  }
}

static int
generic_hw_unit_encode (struct hw *bus,
			const hw_unit *phys,
			char *buf,
			int sizeof_buf)
{
  int i;
  int len;
  char *pos = buf;
  /* skip leading zero's */
  for (i = 0; i < phys->nr_cells; i++)
    {
      if (phys->cells[i] != 0)
	break;
    }
  /* don't output anything if empty */
  if (phys->nr_cells == 0)
    {
      strcpy(pos, "");
      len = 0;
    }
  else if (i == phys->nr_cells)
    {
      /* all zero */
      strcpy(pos, "0");
      len = 1;
    }
  else
    {
      for (; i < phys->nr_cells; i++)
	{
	  if (pos != buf) {
	    strcat(pos, ",");
	    pos = strchr(pos, '\0');
	  }
	  if (phys->cells[i] < 10)
	    sprintf (pos, "%ld", (unsigned long)phys->cells[i]);
	  else
	    sprintf (pos, "0x%lx", (unsigned long)phys->cells[i]);
	  pos = strchr(pos, '\0');
	}
      len = pos - buf;
    }
  if (len >= sizeof_buf)
    hw_abort (NULL, "generic_unit_encode - buffer overflow\n");
  return len;
}

static int
generic_hw_unit_address_to_attach_address (struct hw *me,
					   const hw_unit *address,
					   int *attach_space,
					   unsigned_word *attach_address,
					   struct hw *client)
{
  int i;
  for (i = 0; i < address->nr_cells - 2; i++)
    {
      if (address->cells[i] != 0)
	hw_abort (me, "Only 32bit addresses supported");
    }
  if (address->nr_cells >= 2)
    *attach_space = address->cells[address->nr_cells - 2];
  else
    *attach_space = 0;
  *attach_address = address->cells[address->nr_cells - 1];
  return 1;
}

static int
generic_hw_unit_size_to_attach_size (struct hw *me,
				     const hw_unit *size,
				     unsigned *nr_bytes,
				     struct hw *client)
{
  int i;
  for (i = 0; i < size->nr_cells - 1; i++)
    {
      if (size->cells[i] != 0)
	hw_abort (me, "Only 32bit sizes supported");
    }
  *nr_bytes = size->cells[0];
  return *nr_bytes;
}


/* ignore/passthrough versions of each function */

static void
passthrough_hw_attach_address (struct hw *me,
			       int level,
			       int space,
			       address_word addr,
			       address_word nr_bytes,
			       struct hw *client) /*callback/default*/
{
  if (hw_parent (me) == NULL)
    hw_abort (client, "hw_attach_address: no parent attach method");
  hw_attach_address (hw_parent (me), level,
		     space, addr, nr_bytes,
		     client);
}

static void
passthrough_hw_detach_address (struct hw *me,
			       int level,
			       int space,
			       address_word addr,
			       address_word nr_bytes,
			       struct hw *client) /*callback/default*/
{
  if (hw_parent (me) == NULL)
    hw_abort (client, "hw_attach_address: no parent attach method");
  hw_detach_address (hw_parent (me), level,
		     space, addr, nr_bytes,
		     client);
}

static unsigned
panic_hw_io_read_buffer (struct hw *me,
			 void *dest,
			 int space,
			 unsigned_word addr,
			 unsigned nr_bytes,
			 sim_cpu *processor,
			 sim_cia cia)
{
  hw_abort (me, "no io-read method");
  return 0;
}

static unsigned
panic_hw_io_write_buffer (struct hw *me,
			  const void *source,
			  int space,
			  unsigned_word addr,
			  unsigned nr_bytes,
			  sim_cpu *processor,
			  sim_cia cia)
{
  hw_abort (me, "no io-write method");
  return 0;
}

static unsigned
passthrough_hw_dma_read_buffer (struct hw *me,
				void *dest,
				int space,
				unsigned_word addr,
				unsigned nr_bytes)
{
  if (hw_parent (me) == NULL)
    hw_abort (me, "no parent dma-read method");
  return hw_dma_read_buffer (hw_parent (me), dest,
			     space, addr, nr_bytes);
}

static unsigned
passthrough_hw_dma_write_buffer (struct hw *me,
				 const void *source,
				 int space,
				 unsigned_word addr,
				 unsigned nr_bytes,
				 int violate_read_only_section)
{
  if (hw_parent (me) == NULL)
    hw_abort (me, "no parent dma-write method");
  return hw_dma_write_buffer (hw_parent (me), source,
			      space, addr,
			      nr_bytes,
			      violate_read_only_section);
}

const struct hw_port_descriptor empty_hw_ports[] = {
  { NULL, },
};

static void
panic_hw_port_event (struct hw *me,
		     int my_port,
		     struct hw *source,
		     int source_port,
		     int level,
		     sim_cpu *processor,
		     sim_cia cia)
{
  hw_abort (me, "no port method");
}


static const char *
full_name_of_hw (struct hw *leaf,
		 char *buf,
		 unsigned sizeof_buf)
{
  /* get a buffer */
  char full_name[1024];
  if (buf == (char*)0)
    {
      buf = full_name;
      sizeof_buf = sizeof (full_name);
    }

  /* use head recursion to construct the path */

  if (hw_parent (leaf) == NULL)
    /* root */
    {
      if (sizeof_buf < 1)
	hw_abort (leaf, "buffer overflow");
      *buf = '\0';
    }
  else
    /* sub node */
    {
      char unit[1024];
      full_name_of_hw (hw_parent (leaf), buf, sizeof_buf);
      if (hw_unit_encode (hw_parent (leaf),
			  hw_unit_address (leaf),
			  unit + 1,
			  sizeof (unit) - 1)
	  > 0)
	unit[0] = '@';
      else
	unit[0] = '\0';
      if (strlen (buf) + strlen ("/") + strlen (hw_name (leaf)) + strlen (unit)
	  >= sizeof_buf)
	hw_abort (leaf, "buffer overflow");
      strcat (buf, "/");
      strcat (buf, hw_name (leaf));
      strcat (buf, unit);
    }
  
  /* return it usefully */
  if (buf == full_name)
    buf = (char *) strdup (full_name);
  return buf;
}

struct hw *
hw_create (SIM_DESC sd,
	   struct hw *parent,
	   const char *family,
	   const char *name,
	   const char *unit,
	   const char *args)
{
  struct hw *hw = ZALLOC (struct hw);

  /* our identity */
  hw->family_of_hw = family;
  hw->name_of_hw = name;
  hw->args_of_hw = args;

  /* a hook into the system */
  if (sd != NULL)
    hw->system_of_hw = sd;
  else if (parent != NULL)
    hw->system_of_hw = hw_system (parent);
  else
    hw_abort (parent, "No system found");

  /* in a tree */
  if (parent != NULL)
    {
      struct hw **sibling = &parent->child_of_hw;
      while ((*sibling) != NULL)
	sibling = &(*sibling)->sibling_of_hw;
      *sibling = hw;
      hw->parent_of_hw = parent;
    }

  /* top of tree */
  if (parent != NULL)
    {
      struct hw *root = parent;
      while (root->parent_of_hw != NULL)
	root = root->parent_of_hw;
      hw->root_of_hw = root;
    }
  
  /* a unique identifier for the device on the parents bus */
  if (parent != NULL)
    {
      hw_unit_decode (parent, unit, &hw->unit_address_of_hw);
    }

  /* Determine our path */
  if (parent != NULL)
    hw->path_of_hw = full_name_of_hw (hw, NULL, 0);
  else
    hw->path_of_hw = "/";

  /* our callbacks */
  set_hw_io_read_buffer (hw, panic_hw_io_read_buffer);
  set_hw_io_write_buffer (hw, panic_hw_io_write_buffer);
  set_hw_dma_read_buffer (hw, passthrough_hw_dma_read_buffer);
  set_hw_dma_write_buffer (hw, passthrough_hw_dma_write_buffer);
  set_hw_unit_decode (hw, generic_hw_unit_decode);
  set_hw_unit_encode (hw, generic_hw_unit_encode);
  set_hw_unit_address_to_attach_address (hw, generic_hw_unit_address_to_attach_address);
  set_hw_unit_size_to_attach_size (hw, generic_hw_unit_size_to_attach_size);
  set_hw_attach_address (hw, passthrough_hw_attach_address);
  set_hw_detach_address (hw, passthrough_hw_detach_address);

  /* locate a descriptor */
  {
    const struct hw_device_descriptor **table;
    for (table = hw_descriptors;
	 *table != NULL;
	 table++)
      {
	const struct hw_device_descriptor *entry;
	for (entry = *table;
	     entry->family != NULL;
	     entry++)
	  {
	    if (strcmp (family, entry->family) == 0)
	      {
		hw->base_of_hw = ZALLOC (struct hw_base_data);
		hw->base_of_hw->descriptor = entry;
		hw->base_of_hw->finished_p = 0;
	      }
	  }
      }
    if (hw->base_of_hw == NULL)
      {
	hw_abort (parent, "Unknown device `%s'", family);
      }
  }

  /* Attach dummy ports */
  set_hw_ports (hw, empty_hw_ports);
  set_hw_port_event (hw, panic_hw_port_event);
  
  return hw;
}


int
hw_finished_p (struct hw *me)
{
  return (me->base_of_hw->finished_p);
}

void
hw_finish (struct hw *me)
{
  if (hw_finished_p (me))
    hw_abort (me, "Attempt to finish finished device");

  /* Fill in the (hopefully) defined address/size cells values */
  if (hw_find_property (me, "#address-cells") != NULL)
    me->nr_address_cells_of_hw_unit =
      hw_find_integer_property (me, "#address-cells");
  else
    me->nr_address_cells_of_hw_unit = 2;
  if (hw_find_property (me, "#size-cells") != NULL)
    me->nr_size_cells_of_hw_unit =
      hw_find_integer_property (me, "#size-cells");
  else
    me->nr_size_cells_of_hw_unit = 1;

  /* Allow the real device to override any methods */
  me->base_of_hw->descriptor->to_finish (me);
  me->base_of_hw->finished_p = 1;
}
