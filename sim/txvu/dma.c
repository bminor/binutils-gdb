/*  Copyright (C) 1998, Cygnus Solutions

    */

#include "sim-main.h"

#include "device.h"
#include "dma.h"

int
dma_io_read_buffer(device *me,
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

static sim_event_handler do_the_dma_thing_handler;

int
dma_io_write_buffer(device *me,
                    const void *source,
                    int space,
                    address_word addr,
                    unsigned nr_bytes,
                    sim_cpu *processor,
                    sim_cia cia)
{
	printf("%s: Write!\n", me->name);

	/* Do an event before the next instruction! */
	sim_events_schedule (CPU_STATE(processor), 
			     0 /*time*/, 
			     do_the_dma_thing_handler,
			     CPU_STATE(processor) /*data*/);
	return nr_bytes;
}

device dma_device = 
  { 
    "Dma Controller", 
    &dma_io_read_buffer,
    &dma_io_write_buffer 
  };

void 
dma_attach(SIM_DESC sd) 
{
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   DMA_REGISTER_WINDOW_START,
                   DMA_REGISTER_WINDOW_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   &dma_device,
                   NULL /*buffer*/);
}

static void
do_the_dma_thing_handler(SIM_DESC sd, void *data)
{
  printf("Dma Event!!!\n");
}
