/* Copyright (C) 1998, Cygnus Solutions */

#include "sky-pke.h"
#include <stdlib.h>


/* Imported functions */

void device_error (device *me, char* message);  /* device.c */


/* Internal function declarations */

static int pke_io_read_buffer(device*, void*, int, address_word,
			       unsigned, sim_cpu*, sim_cia);
static int pke_io_write_buffer(device*, const void*, int, address_word,
			       unsigned, sim_cpu*, sim_cia);
static void pke_issue(struct pke_device*);


/* Static data */

struct pke_device pke0_device = 
{ 
  { "pke0", &pke_io_read_buffer, &pke_io_write_buffer }, /* device */
  0, 0,        /* ID, flags */
  PKE0_REGISTER_WINDOW_START, PKE0_FIFO_START,  /* memory-mapping addresses */
  {},          /* regs */
  NULL, 0, 0, NULL,  /* FIFO */
  0            /* pc */
};


struct pke_device pke1_device = 
{ 
  { "pke1", &pke_io_read_buffer, &pke_io_write_buffer }, /* device */
  1, 0,        /* ID, flags */
  PKE1_REGISTER_WINDOW_START, PKE1_FIFO_START,  /* memory-mapping addresses */
  {},          /* regs */
  NULL, 0, 0, NULL, /* FIFO */
  0            /* pc */
};



/* External functions */


/* Attach PKE0 addresses to main memory */

void
pke0_attach(SIM_DESC sd) 
{
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   pke0_device.register_memory_addr,
                   PKE_REGISTER_WINDOW_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   (device*) &pke0_device,
                   NULL /*buffer*/);

  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   pke0_device.fifo_memory_addr,
                   sizeof(quadword) /*nr_bytes*/,
                   0 /*modulo*/,
                   (device*) &pke1_device,
                   NULL /*buffer*/);
}


/* Attach PKE1 addresses to main memory */

void 
pke1_attach(SIM_DESC sd) 
{
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   pke1_device.register_memory_addr,
                   PKE_REGISTER_WINDOW_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   (device*) &pke1_device,
                   NULL /*buffer*/);

  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   pke1_device.fifo_memory_addr,
                   sizeof(quadword) /*nr_bytes*/,
                   0 /*modulo*/,
                   (device*) &pke1_device,
                   NULL /*buffer*/);
}


/* Issue a PKE0 instruction if possible */

void 
pke0_issue() 
{
  pke_issue(& pke0_device);
}


/* Issue a PKE1 instruction if possible */

void 
pke1_issue() 
{
  pke_issue(& pke0_device);
}



/* Internal functions */


/* Handle a PKE read; return no. of bytes read */

int
pke_io_read_buffer(device *me_,
		   void *dest,
		   int space,
		   address_word addr,
		   unsigned nr_bytes,
		   sim_cpu *processor,
		   sim_cia cia)
{
  /* downcast to gather embedding pke_device struct */
  struct pke_device* me = (struct pke_device*) me_;

  /* enforce that an access does not span more than one quadword */
  address_word low = ADDR_TRUNC_QW(addr);
  address_word high = ADDR_TRUNC_QW(addr + nr_bytes - 1);
  if(low != high)
    return 0;

  /* classify address & handle */
  if(addr >= me->register_memory_addr &&
     addr < me->register_memory_addr + PKE_REGISTER_WINDOW_SIZE)
    {
      /* register bank */
      int reg_num = ADDR_TRUNC_QW(addr - me->register_memory_addr) >> 4;
      int readable = 1;

      /* ensure readibility of register: all okay except PKE1-only ones read on PKE0 */
      switch(reg_num)
	{
	case PKE_REG_BASE:
	case PKE_REG_OFST:
	case PKE_REG_TOPS:
	case PKE_REG_TOP:
	case PKE_REG_DBF:
	  if(me->pke_number == 0) /* PKE0 cannot access these registers */
	    readable = 0;
	}

      /* perform read & return */
      if(readable) 
	{
	  /* find byte-offset inside register bank */
	  int reg_byte = ADDR_OFFSET_QW(addr);
 	  void* src = ((unsigned_1*) (& me->regs[reg_num])) + reg_byte;
	  /* copy the bits */
	  memcpy(dest, src, nr_bytes);
	  /* okay */
	  return nr_bytes;
	}
      else
	{
	  /* error */
	  return 0;
	} 

      /* NOTREACHED */
    }
  else if(addr >= me->fifo_memory_addr &&
	  addr < me->fifo_memory_addr + sizeof(quadword))
    {
      /* FIFO */

      /* XXX: FIFO is not readable. */
      return 0;
    }

  /* NOTREACHED */
}


/* Handle a PKE read; return no. of bytes written */

int
pke_io_write_buffer(device *me_,
		    const void *src,
		    int space,
		    address_word addr,
		    unsigned nr_bytes,
		    sim_cpu *processor,
		    sim_cia cia)
{ 
  /* downcast to gather embedding pke_device struct */
  struct pke_device* me = (struct pke_device*) me_;

  /* enforce that an access does not span more than one quadword */
  address_word low = ADDR_TRUNC_QW(addr);
  address_word high = ADDR_TRUNC_QW(addr + nr_bytes - 1);
  if(low != high)
    return 0;

  /* classify address & handle */
  if(addr >= me->register_memory_addr &&
     addr < me->register_memory_addr + PKE_REGISTER_WINDOW_SIZE)
    {
      /* register bank */
      int reg_num = ADDR_TRUNC_QW(addr - me->register_memory_addr) >> 4;
      int writeable = 1;

      /* ensure readibility of register: all okay except PKE1-only ones read on PKE0 */
      switch(reg_num)
	{
	case PKE_REG_BASE:
	case PKE_REG_OFST:
	case PKE_REG_TOPS:
	case PKE_REG_TOP:
	case PKE_REG_DBF:
	  if(me->pke_number == 0) /* PKE0 cannot access these registers */
	    writeable = 0;
	}

      /* perform write & return */
      if(writeable)
	{
	  /* find byte-offset inside register bank */
	  int reg_byte = ADDR_OFFSET_QW(addr);
 	  void* dest = ((unsigned_1*) (& me->regs[reg_num])) + reg_byte;
	  /* copy the bits */
	  memcpy(dest, src, nr_bytes);
	  return nr_bytes;
	}
      else
	{
	  /* error */
	  return 0;
	} 

      /* NOTREACHED */
    }
  else if(addr >= me->fifo_memory_addr &&
	  addr < me->fifo_memory_addr + sizeof(quadword))
    {
      /* FIFO */

      /* assert transfer size == 128 bits */
      if(nr_bytes != sizeof(quadword))
	return 0;

      /* ensure FIFO has enough elements */
      if(me->fifo_num_elements == me->fifo_buffer_size)
	{
	  /* time to grow */
	  int new_fifo_buffer_size = me->fifo_buffer_size + 20;
	  void* ptr = realloc((void*) me->fifo, new_fifo_buffer_size*sizeof(quadword));

	  if(ptr == NULL)
	    {
	      /* oops, cannot enlarge FIFO any more */
	      device_error(me_, "Cannot enlarge FIFO buffer\n");
	      return 0;
	    }

	  me->fifo_buffer_size = new_fifo_buffer_size;
	}

      /* add new quadword at end of FIFO */
      memcpy(& me->fifo[++me->fifo_num_elements], src, nr_bytes);
      
      /* okay */
      return nr_bytes;
    }

  /* NOTREACHED */
}



/* Issue & swallow one PKE opcode if possible */

void
pke_issue(struct pke_device* me)
{


}


