/*  Copyright (C) 1998, Cygnus Solutions

    */

#include "sim-main.h"

#include "sky-device.h"
#include "sky-vu0.h"

static char* vu0_mem0_buffer = 0;
static char* vu0_mem1_buffer = 0;


void 
vu0_issue(void) 
{
}

static int
vu0_io_read_buffer(device *me,
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

static int
vu0_io_write_buffer(device *me,
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

device vu0_device = 
  { 
    "vu0", 
    &vu0_io_read_buffer,
    &vu0_io_write_buffer 
  };

void 
vu0_attach(SIM_DESC sd) 
{
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   VU0_REGISTER_WINDOW_START,
                   VU0_REGISTER_WINDOW_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   &vu0_device,
                   NULL /*buffer*/);

  vu0_mem0_buffer = zalloc(VU0_MEM0_SIZE);
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   VU0_MEM0_WINDOW_START,
                   VU0_MEM0_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   0 /*device*/,
                   vu0_mem0_buffer /*buffer*/);

  vu0_mem1_buffer = zalloc(VU0_MEM1_SIZE);
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   VU0_MEM1_WINDOW_START,
                   VU0_MEM1_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   0 /*device*/,
                   vu0_mem1_buffer /*buffer*/);
}
