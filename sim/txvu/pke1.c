/*  Copyright (C) 1998, Cygnus Solutions

    */

#include "sim-main.h"

#include "device.h"
#include "pke1.h"

void 
pke1_issue() 
{
}

int
pke1_io_read_buffer(device *me,
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
pke1_io_write_buffer(device *me,
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

device pke1_device = 
  {
    "pke1", 
    &pke1_io_read_buffer,
    &pke1_io_write_buffer 
  };

void 
pke1_attach(SIM_DESC sd) 
{
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   PKE1_REGISTER_WINDOW_START,
                   PKE1_REGISTER_WINDOW_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   &pke1_device,
                   NULL /*buffer*/);
}
