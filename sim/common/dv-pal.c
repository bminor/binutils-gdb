/*  This file is part of the program psim.
    
    Copyright (C) 1994-1996,1998, Andrew Cagney <cagney@highland.com.au>
    
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

/* NOTE: pal is naughty and grubs around looking at things outside of
   its immediate domain */
#include "hw-tree.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

/* DEVICE

   
   pal - glue logic device containing assorted junk

   
   DESCRIPTION

   
   Typical hardware dependant hack.  This device allows the firmware
   to gain access to all the things the firmware needs (but the OS
   doesn't).

   The pal contains the following registers.  Except for the interrupt
   level register, each of the below is 8 bytes in size and must be
   accessed using correct alignment.  For 16 and 32 bit accesses the
   bytes not directed to the register are ignored:
   
   |0	reset register (write)
   |4	processor id register (read)
   |8	interrupt port (write)
   |9	interrupt level (write)
   |12	processor count register (read)
   |16	tty input fifo register (read)
   |20	tty input status register (read)
   |24	tty output fifo register (write)
   |28	tty output status register (read)

   Reset register (write) halts the simulator exiting with the
   value written.
   
   Processor id register (read) returns the processor number (0
   .. N-1) of the processor performing the read.
   
   The interrupt registers should be accessed as a pair (using a 16 or
   32 bit store).  The low byte specifies the interrupt port while the
   high byte specifies the level to drive that port at.  By
   convention, the pal's interrupt ports (int0, int1, ...) are wired
   up to the corresponding processor's level sensative external
   interrupt pin.  Eg: A two byte write to address 8 of 0x0102
   (big-endian) will result in processor 2's external interrupt pin to
   be asserted.

   Processor count register (read) returns the total number of
   processors active in the current simulation.

   TTY input fifo register (read), if the TTY input status register
   indicates a character is available by being nonzero, returns the
   next available character from the pal's tty input port.

   Similarly, the TTY output fifo register (write), if the TTY output
   status register indicates the output fifo is not full by being
   nonzero, outputs the character written to the tty's output port.


   PROPERTIES
   

   reg = <address> <size> (required)

   Specify the address (within the parent bus) that this device is to
   live.


   PORTS


   int[0..NR_PROCESSORS] (output)

   Driven as a result of a write to the interrupt-port /
   interrupt-level register pair.


   */


enum {
  hw_pal_reset_register = 0x0,
  hw_pal_cpu_nr_register = 0x4,
  hw_pal_int_register = 0x8,
  hw_pal_nr_cpu_register = 0xa,
  hw_pal_read_fifo = 0x10,
  hw_pal_read_status = 0x14,
  hw_pal_write_fifo = 0x18,
  hw_pal_write_status = 0x1a,
  hw_pal_address_mask = 0x1f,
};


typedef struct _hw_pal_console_buffer {
  char buffer;
  int status;
} hw_pal_console_buffer;

typedef struct _hw_pal_device {
  hw_pal_console_buffer input;
  hw_pal_console_buffer output;
  struct hw *disk;
} hw_pal_device;


/* check the console for an available character */
static void
scan_hw_pal (struct hw *me)
{
#if 0
  hw_pal_struct hw *hw_pal = (hw_pal_struct hw *) hw_data (me);
#endif
  char c;
  int count;
  count = sim_io_read_stdin (hw_system (me), &c, sizeof(c));
#if 0
  switch (count)
    {
    case sim_io_not_ready:
    case sim_io_eof:
      hw_pal->input.buffer = 0;
      hw_pal->input.status = 0;
      break;
    default:
      hw_pal->input.buffer = c;
      hw_pal->input.status = 1;
    }
#endif
}

/* write the character to the hw_pal */
static void
write_hw_pal (struct hw *me,
	      char val)
{
  hw_pal_device *hw_pal = (hw_pal_device *) hw_data (me);
  sim_io_write_stdout (hw_system (me), &val, 1);
  hw_pal->output.buffer = val;
  hw_pal->output.status = 1;
}


static unsigned
hw_pal_io_read_buffer (struct hw *me,
		       void *dest,
		       int space,
		       unsigned_word addr,
		       unsigned nr_bytes,
		       sim_cpu *cpu,
		       sim_cia cia)
{
  hw_pal_device *hw_pal = (hw_pal_device *) hw_data (me);
  unsigned_1 val;
  switch (addr & hw_pal_address_mask)
    {
    case hw_pal_cpu_nr_register:
#ifdef CPU_INDEX
      val = CPU_INDEX (cpu);
#else
      val = 0;
#endif
      HW_TRACE ((me, "read - cpu-nr %d\n", val));
      break;
    case hw_pal_nr_cpu_register:
      val = hw_tree_find_integer_property (me, "/openprom/options/smp");
      HW_TRACE ((me, "read - nr-cpu %d\n", val));
      break;
    case hw_pal_read_fifo:
      val = hw_pal->input.buffer;
      HW_TRACE ((me, "read - input-fifo %d\n", val));
      break;
    case hw_pal_read_status:
      scan_hw_pal (me);
      val = hw_pal->input.status;
      HW_TRACE ((me, "read - input-status %d\n", val));
      break;
    case hw_pal_write_fifo:
      val = hw_pal->output.buffer;
      HW_TRACE ((me, "read - output-fifo %d\n", val));
      break;
    case hw_pal_write_status:
      val = hw_pal->output.status;
      HW_TRACE ((me, "read - output-status %d\n", val));
      break;
    default:
      val = 0;
      HW_TRACE ((me, "read - ???\n"));
    }
  memset (dest, 0, nr_bytes);
  *(unsigned_1*)dest = val;
  return nr_bytes;
}


static unsigned
hw_pal_io_write_buffer (struct hw *me,
			const void *source,
			int space,
			unsigned_word addr,
			unsigned nr_bytes,
			sim_cpu *cpu,
			sim_cia cia)
{
  hw_pal_device *hw_pal = (hw_pal_device*) hw_data (me);
  unsigned_1 *byte = (unsigned_1*) source;
  
  switch (addr & hw_pal_address_mask)
    {
    case hw_pal_reset_register:
      sim_engine_halt (NULL, cpu, NULL, cia, sim_exited, byte[0]);
      break;
    case hw_pal_int_register:
      hw_port_event (me,
		     byte[0], /*port*/
		     (nr_bytes > 1 ? byte[1] : 0), /* val */
		     cpu, cia);
      break;
    case hw_pal_read_fifo:
      hw_pal->input.buffer = byte[0];
      HW_TRACE ((me, "write - input-fifo %d\n", byte[0]));
      break;
    case hw_pal_read_status:
      hw_pal->input.status = byte[0];
      HW_TRACE ((me, "write - input-status %d\n", byte[0]));
      break;
    case hw_pal_write_fifo:
      write_hw_pal (me, byte[0]);
      HW_TRACE ((me, "write - output-fifo %d\n", byte[0]));
      break;
    case hw_pal_write_status:
      hw_pal->output.status = byte[0];
      HW_TRACE ((me, "write - output-status %d\n", byte[0]));
      break;
    }
  return nr_bytes;
}


/* instances of the hw_pal struct hw */

#if NOT_YET
static void
hw_pal_instance_delete_callback(hw_instance *instance)
{
  /* nothing to delete, the hw_pal is attached to the struct hw */
  return;
}
#endif

#if NOT_YET
static int
hw_pal_instance_read_callback (hw_instance *instance,
			      void *buf,
			      unsigned_word len)
{
  DITRACE (pal, ("read - %s (%ld)", (const char*) buf, (long int) len));
  return sim_io_read_stdin (buf, len);
}
#endif

#if NOT_YET
static int
hw_pal_instance_write_callback (hw_instance *instance,
				const void *buf,
				unsigned_word len)
{
  int i;
  const char *chp = buf;
  hw_pal_device *hw_pal = hw_instance_data (instance);
  DITRACE (pal, ("write - %s (%ld)", (const char*) buf, (long int) len));
  for (i = 0; i < len; i++)
    write_hw_pal (hw_pal, chp[i]);
  sim_io_flush_stdoutput ();
  return i;
}
#endif

#if NOT_YET
static const hw_instance_callbacks hw_pal_instance_callbacks = {
  hw_pal_instance_delete_callback,
  hw_pal_instance_read_callback,
  hw_pal_instance_write_callback,
};
#endif

#if 0
static hw_instance *
hw_pal_create_instance (struct hw *me,
			const char *path,
			const char *args)
{
  return hw_create_instance_from (me, NULL,
				      hw_data (me),
				      path, args,
				      &hw_pal_instance_callbacks);
}
#endif

static const struct hw_port_descriptor hw_pal_ports[] = {
  { "int", 0, MAX_NR_PROCESSORS },
  { NULL }
};


static void
hw_pal_attach_address (struct hw *me,
		       int level,
		       int space,
		       address_word addr,
		       address_word nr_bytes,
		       struct hw *client)
{
  hw_pal_device *pal = (hw_pal_device*) hw_data (me);
  pal->disk = client;
}


#if 0
static hw_callbacks const hw_pal_callbacks = {
  { generic_hw_init_address, },
  { hw_pal_attach_address, }, /* address */
  { hw_pal_io_read_buffer_callback,
      hw_pal_io_write_buffer_callback, },
  { NULL, }, /* DMA */
  { NULL, NULL, hw_pal_interrupt_ports }, /* interrupt */
  { generic_hw_unit_decode,
    generic_hw_unit_encode,
    generic_hw_address_to_attach_address,
    generic_hw_size_to_attach_size },
  hw_pal_create_instance,
};
#endif


static void
hw_pal_finish (struct hw *hw)
{
  /* create the descriptor */
  hw_pal_device *hw_pal = HW_ZALLOC (hw, hw_pal_device);
  hw_pal->output.status = 1;
  hw_pal->output.buffer = '\0';
  hw_pal->input.status = 0;
  hw_pal->input.buffer = '\0';
  set_hw_data (hw, hw_pal);
  set_hw_attach_address (hw, hw_pal_attach_address);
  set_hw_io_read_buffer (hw, hw_pal_io_read_buffer);
  set_hw_io_write_buffer (hw, hw_pal_io_write_buffer);
  set_hw_ports (hw, hw_pal_ports);
}


const struct hw_device_descriptor dv_pal_descriptor[] = {
  { "pal", hw_pal_finish, },
  { NULL },
};
