/*  Copyright (C) 1998, Cygnus Solutions

    */

#include "sim-main.h"

#include "device.h"
#include "pke0.h"

int
pke0_io_read_buffer(device *me,
                    void *dest,
                    int space,
                    address_word addr,
                    unsigned nr_bytes,
                    sim_cpu *processor,
                    sim_cia cia)
{
	printf("%s: Read!\n", me->name);
	return nr_bytes;
}

int
pke0_io_write_buffer(device *me,
                     const void *source,
                     int space,
                     address_word addr,
                     unsigned nr_bytes,
                     sim_cpu *processor,
                     sim_cia cia)
{ 
	printf("%s: Write!\n", me->name);
	return nr_bytes;
}

device pke0_device = 
  { 
    "pke0", 
    &pke0_io_read_buffer,
    &pke0_io_write_buffer 
  };

void 
pke0_attach(SIM_DESC sd) 
{
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   PKE0_REGISTER_WINDOW_START,
                   PKE0_REGISTER_WINDOW_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   &pke0_device,
                   NULL /*buffer*/);
}
