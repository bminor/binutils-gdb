/*  This file is part of the program psim.

    Copyright (C) 1994-1998, Andrew Cagney <cagney@highland.com.au>

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


#ifndef HW_ROOT
#define HW_ROOT

/* A root device from which dv-* devices can be built */

#include "hw-device.h"

#include "hw-properties.h"
/* #include "hw-instances.h" */
/* #include "hw-handles.h" */
#include "hw-ports.h"

typedef void (hw_finish_callback)
     (struct hw *me);

struct hw_device_descriptor {
  const char *family;
  hw_finish_callback *to_finish;
};

/* Create a primative device */

struct hw *hw_create
(SIM_DESC sd,
 struct hw *parent,
 const char *family,
 const char *name,
 const char *unit,
 const char *args);


/* Complete the creation of that device (finish overrides methods
   using the set_hw_* operations below) */

void hw_finish
(struct hw *me);

int hw_finished_p
(struct hw *me);


/* Delete the entire device */

void hw_delete
(struct hw *me);


/* Override device methods */

#define set_hw_data(hw, value) \
((hw)->data_of_hw = (value))

#define set_hw_reset(hw, method) \
((hw)->to_reset = method)

#define set_hw_io_read_buffer(hw, method) \
((hw)->to_io_read_buffer = (method))
#define set_hw_io_write_buffer(hw, method) \
((hw)->to_io_write_buffer = (method))

#define set_hw_dma_read_buffer(me, method) \
((me)->to_dma_read_buffer = (method))
#define set_hw_dma_write_buffer(me, method) \
((me)->to_dma_write_buffer = (method))

#define set_hw_attach_address(hw, method) \
((hw)->to_attach_address = (method))
#define set_hw_detach_address(hw, method) \
((hw)->to_detach_address = (method))

#define set_hw_unit_decode(hw, method) \
((hw)->to_unit_decode = (method))
#define set_hw_unit_encode(hw, method) \
((hw)->to_unit_encode = (method))

#define set_hw_unit_address_to_attach_address(hw, method) \
((hw)->to_unit_address_to_attach_address = (method))
#define set_hw_unit_size_to_attach_size(hw, method) \
((hw)->to_unit_size_to_attach_size = (method))

typedef void (hw_delete_callback)
     (struct hw *me);

#define set_hw_delete(hw, method) \
((hw)->base_of_hw->to_delete = (method))


struct hw_port_descriptor {
  const char *name;
  int number; 
  int nr_ports;
  port_direction direction;
};

typedef void (hw_port_event_callback)
     (struct hw *me,
      int my_port,
      struct hw *source,
      int source_port,
      int level,
      sim_cpu *processor,
      sim_cia cia);

extern void set_hw_ports (struct hw *hw, const struct hw_port_descriptor ports[]);
extern void set_hw_port_event (struct hw *hw, hw_port_event_callback *to_port_event);

#endif
