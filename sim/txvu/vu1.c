/*  Copyright (C) 1998, Cygnus Solutions

    */

#include "sim-main.h"

#include "device.h"
#include "vu1.h"

static char vu1_mem0_buffer[VU1_MEM0_SIZE];
static char vu1_mem1_buffer[VU1_MEM1_SIZE];

void 
vu1_issue() 
{
}

int
vu1_io_read_buffer(device *me,
                      void *dest,
                      int space,
                      address_word addr,
                      unsigned nr_bytes,
                      sim_cpu *processor,
                      sim_cia cia)
{
	printf("%s: Read!\n", me->name);

printf("    vu1_mem0[0] = %d\n", *(int*)&vu1_mem0_buffer[0]);
printf("    vu1_mem1[0] = %d\n", *(int*)&vu1_mem1_buffer[0]);

	return nr_bytes;
}

int
vu1_io_write_buffer(device *me,
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

device vu1_device = 
  { 
    "vu1", 
    &vu1_io_read_buffer,
    &vu1_io_write_buffer 
  };

void 
vu1_attach(SIM_DESC sd) 
{
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   VU1_REGISTER_WINDOW_START,
                   VU1_REGISTER_WINDOW_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   &vu1_device,
                   NULL /*buffer*/);

  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   VU1_MEM0_WINDOW_START,
                   VU1_MEM0_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   0 /*device*/,
                   &vu1_mem0_buffer /*buffer*/);

  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   VU1_MEM1_WINDOW_START,
                   VU1_MEM1_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   0 /*device*/,
                   &vu1_mem1_buffer /*buffer*/);
}
