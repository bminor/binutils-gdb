/*  This file is part of the program psim.
    
    Copyright (C) 1994-1996, Andrew Cagney <cagney@highland.com.au>
    
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


#ifndef _HW_PAL_C_
#define _HW_PAL_C_

#ifndef STATIC_INLINE_HW_PAL
#define STATIC_INLINE_HW_PAL STATIC_INLINE
#endif

#include "device_table.h"
#include "cpu.h"


#include <stdio.h>
#include <fcntl.h>

#if 0
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#endif

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

#if !defined(O_NDELAY) || !defined(F_GETFL) || !defined(F_SETFL)
#undef WITH_STDIO
#define WITH_STDIO DO_USE_STDIO
#endif

/* Device:
   
   pal@<address>
   
   
   Description:
   
   Typical hardware dependant hack.  This device allows the firmware
   to gain access to all the things the firmware needs (but the OS
   doesn't).  All registers are little endian (byte 0 is the least
   significant) and must be accessed correctly aligned.
   
   <address> + 0: write - halts simulation with exit code byte[0].
   
   <address> + 4: read - processor nr in byte[0].
   
   <address> + 8: write - send interrupt message to port byte[0] with
   value byte[1].
   
   <address> + 12: read - nr processors in byte[0].
   
   
   Properties:
   
   NONE. */


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
} hw_pal_device;


/* check the console for an available character */
static void
scan_hw_pal(hw_pal_device *hw_pal)
{
  if (WITH_STDIO == DO_USE_STDIO) {
    int c = getchar ();
    if (c == EOF) {
      hw_pal->input.buffer = 0;
      hw_pal->input.status = 0;
    } else {
      hw_pal->input.buffer = c;
      hw_pal->input.status = 1;
    }

  } else {
#if !defined(O_NDELAY) || !defined(F_GETFL) || !defined(F_SETFL)
    error ("O_NDELAY, F_GETFL, or F_SETFL not defined");

#else
    /* check for input */
    int flags;
    int status;
    /* get the old status */
    flags = fcntl(0, F_GETFL, 0);
    if (flags == -1) {
      perror("hw_pal");
      return;
    }
    /* temp, disable blocking IO */
    status = fcntl(0, F_SETFL, flags | O_NDELAY);
    if (status == -1) {
      perror("hw_pal");
      return;
    }
    /* try for input */
    status = read(0, &hw_pal->input.buffer, 1);
    if (status == 1) {
      hw_pal->input.status = 1;
    }
    else {
      hw_pal->input.status = 0;
    }
    /* return to regular vewing */
    flags = fcntl(0, F_SETFL, flags);
    if (flags == -1) {
      perror("hw_pal");
      return;
    }
#endif
  }
}

/* write the character to the hw_pal */
static void
write_hw_pal(hw_pal_device *hw_pal,
	     char val)
{
  if (WITH_STDIO == DO_USE_STDIO) {
    putchar (val);

  } else {
    printf_filtered("%c", val) ;
  }

  hw_pal->output.buffer = val;
  hw_pal->output.status = 1;
}


static unsigned
hw_pal_io_read_buffer_callback(device *me,
			       void *dest,
			       int space,
			       unsigned_word addr,
			       unsigned nr_bytes,
			       cpu *processor,
			       unsigned_word cia)
{
  hw_pal_device *hw_pal = (hw_pal_device*)device_data(me);
  unsigned_1 val;
  switch (addr & hw_pal_address_mask) {
  case hw_pal_cpu_nr_register:
    val = cpu_nr(processor);
    break;
  case hw_pal_nr_cpu_register:
    val = device_find_integer_property(me, "/openprom/options/smp");
    break;
  case hw_pal_read_fifo:
    val = hw_pal->input.buffer;
    break;
  case hw_pal_read_status:
    scan_hw_pal(hw_pal);
    val = hw_pal->input.status;
    break;
  case hw_pal_write_fifo:
    val = hw_pal->output.buffer;
    break;
  case hw_pal_write_status:
    val = hw_pal->output.status;
    break;
  default:
    val = 0;
  }
  memset(dest, 0, nr_bytes);
  *(unsigned_1*)dest = val;
  return nr_bytes;
}


static unsigned
hw_pal_io_write_buffer_callback(device *me,
				const void *source,
				int space,
				unsigned_word addr,
				unsigned nr_bytes,
				cpu *processor,
				unsigned_word cia)
{
  hw_pal_device *hw_pal = (hw_pal_device*)device_data(me);
  unsigned_1 *byte = (unsigned_1*)source;
  
  switch (addr & hw_pal_address_mask) {
  case hw_pal_reset_register:
    cpu_halt(processor, cia, was_exited, byte[0]);
    break;
  case hw_pal_int_register:
    device_interrupt_event(me,
			   byte[0], /*port*/
			   (nr_bytes > 1 ? byte[1] : 0), /* val */
			   processor, cia);
    break;
  case hw_pal_read_fifo:
    hw_pal->input.buffer = byte[0];
    break;
  case hw_pal_read_status:
    hw_pal->input.status = byte[0];
    break;
  case hw_pal_write_fifo:
    write_hw_pal(hw_pal, byte[0]);
    break;
  case hw_pal_write_status:
    hw_pal->output.status = byte[0];
    break;
  }
  return nr_bytes;
}


/* instances of the hw_pal device */
static void *
hw_pal_instance_create_callback(device *me,
				const char *args)
{
  /* make life easier, attach the hw_pal data to the instance */
  return device_data(me);
}

static void
hw_pal_instance_delete_callback(device_instance *instance)
{
  /* nothing to delete, the hw_pal is attached to the device */
  return;
}

static int
hw_pal_instance_read_callback(device_instance *instance,
			      void *buf,
			      unsigned_word len)
{
  char *buf_char = (char *)buf;
  if (WITH_STDIO == DO_USE_STDIO) {
    char *line = fgets (buf_char, len, stdin);
    return ((!line) ? -1 : strlen (buf_char));

  } else {
    return read(0, buf_char, len);
  }
}

static int
hw_pal_instance_write_callback(device_instance *instance,
			       const void *buf,
			       unsigned_word len)
{
  int i;
  const char *chp = buf;
  hw_pal_device *hw_pal = device_instance_data(instance);
  for (i = 0; i < len; i++)
    write_hw_pal(hw_pal, chp[i]);

  if (WITH_STDIO == DO_USE_STDIO) {
    fflush (stdout);
  }
  return i;
}

static device_callbacks const hw_pal_callbacks = {
  { generic_device_init_address, },
  { NULL, }, /* address */
  { hw_pal_io_read_buffer_callback,
      hw_pal_io_write_buffer_callback, },
  { NULL, }, /* DMA */
  { NULL, }, /* interrupt */
  { NULL, }, /* unit */
  { hw_pal_instance_create_callback,
      hw_pal_instance_delete_callback,
      hw_pal_instance_read_callback,
      hw_pal_instance_write_callback, },
};


static void *
hw_pal_create(const char *name,
	      const device_unit *unit_address,
	      const char *args,
	      device *parent)
{
  /* create the descriptor */
  hw_pal_device *hw_pal = ZALLOC(hw_pal_device);
  hw_pal->output.status = 1;
  hw_pal->output.buffer = '\0';
  hw_pal->input.status = 0;
  hw_pal->input.buffer = '\0';
  return hw_pal;
}


const device_descriptor hw_pal_device_descriptor[] = {
  { "pal", hw_pal_create, &hw_pal_callbacks },
  { NULL },
};

#endif /* _HW_PAL_C_ */
