/*  This file is part of the program GDB, the GNU debugger.
    
    Copyright (C) 1998 Free Software Foundation, Inc.
    Contributed by Cygnus Solutions.
    
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
#include "hw-main.h"

/* DEVICE

   
   mn103ser - mn103002 serial devices 0, 1 and 2.

   
   DESCRIPTION
   
   Implements the mn103002 serial interfaces as described in the
   mn103002 user guide. 


   PROPERTIES   

   reg = <serial-addr> <serial-size>


   BUGS

   */


/* The serial devices' registers' address block */

struct mn103ser_block {
  unsigned_word base;
  unsigned_word bound;
};



enum serial_register_types {
    SC0CTR,
    SC0ICR,
    SC0TXB,
    SC0RXB,
    SC0STR,
    SC1CTR,
    SC1ICR,
    SC1TXB,
    SC1RXB,
    SC1STR,
    SC2CTR,
    SC2ICR,
    SC2TXB,
    SC2RXB,
    SC2STR,
};


struct mn103ser {
  struct mn103ser_block block;
};

/* output port ID's */

/* for mn103002 */
enum {
  SERIAL0_RECEIVE,
  SERIAL0_SEND,
  SERIAL1_RECEIVE,
  SERIAL1_SEND,
  SERIAL2_RECEIVE,
  SERIAL2_SEND,
};


static const struct hw_port_descriptor mn103ser_ports[] = {

  { "serial-0-receive",  SERIAL0_RECEIVE, 0, output_port, },
  { "serial-0-transmit", SERIAL0_SEND, 0, output_port, },
  { "serial-1-receive",  SERIAL1_RECEIVE, 0, output_port, },
  { "serial-1-transmit", SERIAL1_SEND, 0, output_port, },
  { "serial-2-receive",  SERIAL2_RECEIVE, 0, output_port, },
  { "serial-2-transmit", SERIAL2_SEND, 0, output_port, },

  { NULL, },
};



/* Finish off the partially created hw device.  Attach our local
   callbacks.  Wire up our port names etc */

static hw_io_read_buffer_method mn103ser_io_read_buffer;
static hw_io_write_buffer_method mn103ser_io_write_buffer;

static void
attach_mn103ser_regs (struct hw *me,
		      struct mn103ser *serial)
{
  unsigned_word attach_address;
  int attach_space;
  unsigned attach_size;
  reg_property_spec reg;

  if (hw_find_property (me, "reg") == NULL)
    hw_abort (me, "Missing \"reg\" property");

  if (!hw_find_reg_array_property (me, "reg", 0, &reg))
    hw_abort (me, "\"reg\" property must contain three addr/size entries");
  hw_unit_address_to_attach_address (hw_parent (me),
				     &reg.address,
				     &attach_space,
				     &attach_address,
				     me);
  serial->block.base = attach_address;
  hw_unit_size_to_attach_size (hw_parent (me),
			       &reg.size,
			       &attach_size, me);
  serial->block.bound = attach_address + (attach_size - 1);
  hw_attach_address (hw_parent (me),
		     0,
		     attach_space, attach_address, attach_size,
		     me);
}

static void
mn103ser_finish (struct hw *me)
{
  struct mn103ser *serial;
  int i;

  serial = HW_ZALLOC (me, struct mn103ser);
  set_hw_data (me, serial);
  set_hw_io_read_buffer (me, mn103ser_io_read_buffer);
  set_hw_io_write_buffer (me, mn103ser_io_write_buffer);
  set_hw_ports (me, mn103ser_ports);

  /* Attach ourself to our parent bus */
  attach_mn103ser_regs (me, serial);

  /* Initialize the serial device registers. */
}


/* read and write */

static int
decode_addr (struct hw *me,
	     struct mn103ser *serial,
	     unsigned_word address)
{
  unsigned_word offset;
  offset = address - serial->block.base;

  switch (offset)
    {
    case 0x00: return SC0CTR;
    case 0x04: return SC0ICR;
    case 0x08: return SC0TXB;
    case 0x09: return SC0RXB;
    case 0x0C: return SC0STR;
    case 0x10: return SC1CTR;
    case 0x14: return SC1ICR;
    case 0x18: return SC1TXB;
    case 0x19: return SC1RXB;
    case 0x1C: return SC1STR;
    case 0x20: return SC2CTR;
    case 0x24: return SC2ICR;
    case 0x28: return SC2TXB;
    case 0x29: return SC2RXB;
    case 0x2C: return SC2STR;
    default: 
      {
	hw_abort (me, "bad address");
	return -1;
      }
    }
}

static void
read_control_reg (struct hw *me,
		  struct mn103ser *serial,
		  unsigned_word addr,
		  void *dest,
		  unsigned  nr_bytes)
{
}


static void
read_intmode_reg (struct hw *me,
		  struct mn103ser *serial,
		  unsigned_word addr,
		  void *dest,
		  unsigned  nr_bytes)
{
}


static void
read_txb (struct hw *me,
	  struct mn103ser *serial,
	  unsigned_word addr,
	  void *dest,
	  unsigned  nr_bytes)
{
}


static void
read_rxb (struct hw *me,
	  struct mn103ser *serial,
	  unsigned_word addr,
	  void *dest,
	  unsigned  nr_bytes)
{
}


static void
read_status_reg (struct hw *me,
		 struct mn103ser *serial,
		 unsigned_word addr,
		 void *dest,
		 unsigned  nr_bytes)
{
}


static unsigned
mn103ser_io_read_buffer (struct hw *me,
			 void *dest,
			 int space,
			 unsigned_word base,
			 unsigned nr_bytes)
{
  struct mn103ser *serial = hw_data (me);
  enum serial_register_types serial_reg;
  HW_TRACE ((me, "read 0x%08lx %d", (long) base, (int) nr_bytes));

  serial_reg = decode_addr (me, serial, base);
  switch (serial_reg)
    {
    /* control registers */
    case SC0CTR:
    case SC1CTR:
    case SC2CTR:
      read_control_reg(me, serial, base, dest, nr_bytes);
      break;

    /* interrupt mode registers */
    case SC0ICR:
    case SC1ICR:
    case SC2ICR:
      read_intmode_reg(me, serial, base, dest, nr_bytes);
      break;

    /* transmission buffers */
    case SC0TXB:
    case SC1TXB:
    case SC2TXB:
      read_txb(me, serial, base, dest, nr_bytes);
      break;

    /* reception buffers */
    case SC0RXB: 
    case SC1RXB:
    case SC2RXB:
      read_rxb(me, serial, base, dest, nr_bytes);
     break;

    /* status registers */
    case SC0STR: 
    case SC1STR: 
    case SC2STR: 
      read_status_reg(me, serial, base, dest, nr_bytes);
      break;

    default:
      hw_abort(me, "invalid address");
    }

  return nr_bytes;
}     


static void
write_control_reg (struct hw *me,
		   struct mn103ser *serial,
		   unsigned_word addr,
		   const void *source,
		   unsigned  nr_bytes)
{
}


static void
write_intmode_reg (struct hw *me,
		   struct mn103ser *serial,
		   unsigned_word addr,
		   const void *source,
		   unsigned  nr_bytes)
{
}


static void
write_txb (struct hw *me,
	   struct mn103ser *serial,
	   unsigned_word addr,
	   const void *source,
	   unsigned  nr_bytes)
{
}


static void
write_rxb (struct hw *me,
	   struct mn103ser *serial,
	   unsigned_word addr,
	   const void *source,
	   unsigned  nr_bytes)
{
}


static void
write_status_reg (struct hw *me,
		  struct mn103ser *serial,
		  unsigned_word addr,
		  const void *source,
		  unsigned  nr_bytes)
{
}


static unsigned
mn103ser_io_write_buffer (struct hw *me,
			  const void *source,
			  int space,
			  unsigned_word base,
			  unsigned nr_bytes)
{
  struct mn103ser *serial = hw_data (me);
  enum serial_register_types serial_reg;
  HW_TRACE ((me, "write 0x%08lx %d", (long) base, (int) nr_bytes));

  serial_reg = decode_addr (me, serial, base);
  switch (serial_reg)
    {
    /* control registers */
    case SC0CTR:
    case SC1CTR:
    case SC2CTR:
      write_control_reg(me, serial, base, source, nr_bytes);
      break;

    /* interrupt mode registers */
    case SC0ICR:
    case SC1ICR:
    case SC2ICR:
      write_intmode_reg(me, serial, base, source, nr_bytes);
      break;

    /* transmission buffers */
    case SC0TXB:
    case SC1TXB:
    case SC2TXB:
      write_txb(me, serial, base, source, nr_bytes);
      break;

    /* reception buffers */
    case SC0RXB: 
    case SC1RXB:
    case SC2RXB:
      write_rxb(me, serial, base, source, nr_bytes);
     break;

    /* status registers */
    case SC0STR: 
    case SC1STR: 
    case SC2STR: 
      write_status_reg(me, serial, base, source, nr_bytes);
      break;

    default:
      hw_abort(me, "invalid address");
    }

  return nr_bytes;
}     


const struct hw_descriptor dv_mn103ser_descriptor[] = {
  { "mn103ser", mn103ser_finish, },
  { NULL },
};
