/*  Copyright (C) 1998, Cygnus Solutions

    */

#include "sim-main.h"

#include "sky-device.h"
#include "sky-vu.h"
#include "sky-vu0.h"

VectorUnitState vu0_state;

/* these are aligned versions of zalloc() pointers - do not zfree()! */
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
  if (addr < VU0_REGISTER_WINDOW_START)
    return 0;

  addr -= VU0_REGISTER_WINDOW_START;

  /* Adjust nr_bytes if too big */
  if ((addr + nr_bytes) > VU_REG_END)
    nr_bytes -= addr + nr_bytes - VU_REG_END;

  return read_vu_registers (&vu0_state, addr, nr_bytes, dest);
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
  if (addr < VU0_REGISTER_WINDOW_START)
    return 0;

  addr -= VU0_REGISTER_WINDOW_START;

  /* Adjust nr_bytes if too big */
  if ((addr + nr_bytes) > VU_REG_END)
    nr_bytes -= addr + nr_bytes - VU_REG_END;

  return write_vu_registers (&vu0_state, addr, nr_bytes, source);
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
                   VU_REG_END /*nr_bytes*/,
                   0 /*modulo*/,
                   &vu0_device,
                   NULL /*buffer*/);

  vu0_mem0_buffer = zalloc(VU0_MEM0_SIZE);
  vu0_mem0_buffer = (void*) ALIGN_16((unsigned)vu0_mem0_buffer);
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
  vu0_mem1_buffer = (void*) ALIGN_16((unsigned)vu0_mem1_buffer);
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
