/*  dv-m68hc11.c -- CPU 68HC11 as a device.
    Copyright (C) 1999, 2000 Free Software Foundation, Inc.
    Written by Stephane Carrez (stcarrez@worldnet.fr)
    (From a driver model Contributed by Cygnus Solutions.)
    
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

        m68hc11cpu - m68hc11 cpu virtual device

   
   DESCRIPTION

        Implements the external m68hc11 functionality.  This includes the
        delivery of of interrupts generated from other devices and the
        handling of device specific registers.


   PROPERTIES

   reg <base> <size>

        Register base (should be 0x1000 0x03f).

   clock <hz>

        Frequency of the quartz used by the processor.

   mode [single | expanded | bootstrap | test]

        Cpu operating mode (the MODA and MODB external pins).


   PORTS

   reset (input)

        Reset the cpu and generates a cpu-reset event (used to reset
        other devices).

   nmi (input)

        Deliver a non-maskable interrupt to the processor.


   cpu-reset (output)

        Event generated after the CPU performs a reset.


   BUGS

        When delivering an interrupt, this code assumes that there is only
        one processor (number 0).

   */



struct m68hc11cpu {
  /* Pending interrupts for delivery by event handler.  */
  int              pending_reset;
  int              pending_nmi;
  int              pending_level;
  struct hw_event  *event;
  unsigned_word    attach_address;
  int              attach_size;
  int              attach_space;
};



/* input port ID's */ 

enum {
  RESET_PORT,
  NMI_PORT,
  IRQ_PORT,
  CPU_RESET_PORT
};


static const struct hw_port_descriptor m68hc11cpu_ports[] = {

  /* Interrupt inputs.  */
  { "reset",     RESET_PORT,     0, input_port, },
  { "nmi",       NMI_PORT,       0, input_port, },
  { "irq",       IRQ_PORT,       0, input_port, },

  /* Events generated for connection to other devices.  */
  { "cpu-reset", CPU_RESET_PORT, 0, output_port, },

  { NULL, },
};

static hw_io_read_buffer_method m68hc11cpu_io_read_buffer;
static hw_io_write_buffer_method m68hc11cpu_io_write_buffer;
static hw_ioctl_method m68hc11_ioctl;

/* Finish off the partially created hw device.  Attach our local
   callbacks.  Wire up our port names etc.  */

static hw_port_event_method m68hc11cpu_port_event;


static void
dv_m6811_attach_address_callback (struct hw *me,
                                  int level,
                                  int space,
                                  address_word addr,
                                  address_word nr_bytes,
                                  struct hw *client)
{
  HW_TRACE ((me, "attach - level=%d, space=%d, addr=0x%lx, sz=%ld, client=%s",
	     level, space, (unsigned long) addr, (unsigned long) nr_bytes,
             hw_path (client)));

  if (space != io_map)
    {
      sim_core_attach (hw_system (me),
		       NULL, /*cpu*/
		       level,
		       access_read_write_exec,
		       space, addr,
		       nr_bytes,
		       0, /* modulo */
		       client,
		       NULL);
    }
  else
    {
      /*printf("Attach from sub device: %d\n", (long) addr);*/
      sim_core_attach (hw_system (me),
		       NULL, /*cpu*/
		       level,
		       access_io,
		       space, addr,
		       nr_bytes,
		       0, /* modulo */
		       client,
		       NULL);
    }
}

static void
dv_m6811_detach_address_callback (struct hw *me,
                                  int level,
                                  int space,
                                  address_word addr,
                                  address_word nr_bytes,
                                  struct hw *client)
{
  sim_core_detach (hw_system (me), NULL, /*cpu*/
                   level, space, addr);
}

static void
m68hc11_delete (struct hw* me)
{
  struct m68hc11cpu *controller;
  
  controller = hw_data (me);

  hw_detach_address (me, M6811_IO_LEVEL,
		     controller->attach_space,
		     controller->attach_address,
		     controller->attach_size, me);
}


static void
attach_m68hc11_regs (struct hw *me,
		     struct m68hc11cpu *controller)
{
  SIM_DESC sd;
  sim_cpu *cpu;
  reg_property_spec reg;
  const char *cpu_mode;
  
  if (hw_find_property (me, "reg") == NULL)
    hw_abort (me, "Missing \"reg\" property");

  if (!hw_find_reg_array_property (me, "reg", 0, &reg))
    hw_abort (me, "\"reg\" property must contain one addr/size entry");

  hw_unit_address_to_attach_address (hw_parent (me),
				     &reg.address,
				     &controller->attach_space,
				     &controller->attach_address,
				     me);
  hw_unit_size_to_attach_size (hw_parent (me),
			       &reg.size,
			       &controller->attach_size, me);

  hw_attach_address (hw_parent (me), M6811_IO_LEVEL,
		     controller->attach_space,
                     controller->attach_address,
                     controller->attach_size,
		     me);
  set_hw_delete (me, m68hc11_delete);

  /* Get cpu frequency.  */
  sd = hw_system (me);
  cpu = STATE_CPU (sd, 0);
  if (hw_find_property (me, "clock") != NULL)
    {
      cpu->cpu_frequency = hw_find_integer_property (me, "clock");
    }
  else
    {
      cpu->cpu_frequency = 8*1000*1000;
    }

  cpu_mode = "expanded";
  if (hw_find_property (me, "mode") != NULL)
    cpu_mode = hw_find_string_property (me, "mode");

  if (strcmp (cpu_mode, "test") == 0)
    cpu->cpu_mode = M6811_MDA | M6811_SMOD;
  else if (strcmp (cpu_mode, "bootstrap") == 0)
    cpu->cpu_mode = M6811_SMOD;
  else if (strcmp (cpu_mode, "single") == 0)
    cpu->cpu_mode = 0;
  else
    cpu->cpu_mode = M6811_MDA;
}

static void
m68hc11cpu_finish (struct hw *me)
{
  struct m68hc11cpu *controller;

  controller = HW_ZALLOC (me, struct m68hc11cpu);
  set_hw_data (me, controller);
  set_hw_io_read_buffer (me, m68hc11cpu_io_read_buffer);
  set_hw_io_write_buffer (me, m68hc11cpu_io_write_buffer);
  set_hw_ports (me, m68hc11cpu_ports);
  set_hw_port_event (me, m68hc11cpu_port_event);
  set_hw_attach_address (me, dv_m6811_attach_address_callback);
  set_hw_detach_address (me, dv_m6811_detach_address_callback);
#ifdef set_hw_ioctl
  set_hw_ioctl (me, m68hc11_ioctl);
#else
  me->to_ioctl = m68hc11_ioctl;
#endif

  /* Initialize the pending interrupt flags.  */
  controller->pending_level = 0;
  controller->pending_reset = 0;
  controller->pending_nmi = 0;
  controller->event = NULL;

  attach_m68hc11_regs (me, controller);
}



/* An event arrives on an interrupt port.  */

static void
deliver_m68hc11cpu_interrupt (struct hw *me, void *data)
{
}


static void
m68hc11cpu_port_event (struct hw *me,
                       int my_port,
                       struct hw *source,
                       int source_port,
                       int level)
{
  struct m68hc11cpu *controller = hw_data (me);
  SIM_DESC sd;
  sim_cpu* cpu;
  
  sd  = hw_system (me);
  cpu = STATE_CPU (sd, 0);
  switch (my_port)
    {
    case RESET_PORT:
      HW_TRACE ((me, "port-in reset"));

      /* The reset is made in 3 steps:
         - First, cleanup the current sim_cpu struct.
         - Reset the devices.
         - Restart the cpu for the reset (get the CPU mode from the
           CONFIG register that gets initialized by EEPROM device).  */
      cpu_reset (cpu);
      hw_port_event (me, CPU_RESET_PORT, 1);
      cpu_restart (cpu);
      break;
      
    case NMI_PORT:
      controller->pending_nmi = 1;
      HW_TRACE ((me, "port-in nmi"));
      break;
      
    case IRQ_PORT:
      /* level == 0 means that the interrupt was cleared.  */
      if(level == 0)
	controller->pending_level = -1; /* signal end of interrupt */
      else
	controller->pending_level = level;
      HW_TRACE ((me, "port-in level=%d", level));
      break;
      
    default:
      hw_abort (me, "bad switch");
      break;
    }

  /* Schedule an event to be delivered immediately after current
     instruction.  */
  if(controller->event != NULL)
    hw_event_queue_deschedule(me, controller->event);
  controller->event =
    hw_event_queue_schedule (me, 0, deliver_m68hc11cpu_interrupt, NULL);
}


io_reg_desc config_desc[] = {
  { M6811_NOSEC, "NOSEC ", "Security Mode Disable" },
  { M6811_NOCOP, "NOCOP ", "COP System Disable" },
  { M6811_ROMON, "ROMON ", "Enable On-chip Rom" },
  { M6811_EEON,  "EEON  ", "Enable On-chip EEprom" },
  { 0,  0, 0 }
};

io_reg_desc hprio_desc[] = {
  { M6811_RBOOT, "RBOOT ", "Read Bootstrap ROM" },
  { M6811_SMOD,  "SMOD  ", "Special Mode" },
  { M6811_MDA,   "MDA   ", "Mode Select A" },
  { M6811_IRV,   "IRV   ", "Internal Read Visibility" },
  { 0,  0, 0 }
};

io_reg_desc option_desc[] = {
  { M6811_ADPU,  "ADPU  ", "A/D Powerup" },
  { M6811_CSEL,  "CSEL  ", "A/D/EE Charge pump clock source select" },
  { M6811_IRQE,  "IRQE  ", "IRQ Edge/Level sensitive" },
  { M6811_DLY,   "DLY   ", "Stop exit turn on delay" },
  { M6811_CME,   "CME   ", "Clock Monitor Enable" },
  { M6811_CR1,   "CR1   ", "COP timer rate select (CR1)" },
  { M6811_CR0,   "CR0   ", "COP timer rate select (CR0)" },
  { 0,  0, 0 }
};

static void
m68hc11_info (struct hw *me)
{
  SIM_DESC sd;
  uint16 base = 0;
  sim_cpu *cpu;
  struct m68hc11sio *controller;
  uint8 val;
  
  sd = hw_system (me);
  cpu = STATE_CPU (sd, 0);
  controller = hw_data (me);

  base = cpu_get_io_base (cpu);
  sim_io_printf (sd, "M68HC11:\n");

  val = cpu->ios[M6811_HPRIO];
  print_io_byte (sd, "HPRIO ", hprio_desc, val, base + M6811_HPRIO);
  sim_io_printf (sd, "\n");

  val = cpu->ios[M6811_CONFIG];
  print_io_byte (sd, "CONFIG", config_desc, val, base + M6811_CONFIG);
  sim_io_printf (sd, "\n");

  val = cpu->ios[M6811_OPTION];
  print_io_byte (sd, "OPTION", option_desc, val, base + M6811_OPTION);
  sim_io_printf (sd, "\n");

  val = cpu->ios[M6811_INIT];
  print_io_byte (sd, "INIT  ", 0, val, base + M6811_INIT);
  sim_io_printf (sd, "Ram = 0x%04x IO = 0x%04x\n",
		 (((uint16) (val & 0xF0)) << 8),
		 (((uint16) (val & 0x0F)) << 12));


  cpu_info (sd, cpu);
  interrupts_info (sd, &cpu->cpu_interrupts);
}

static int
m68hc11_ioctl (struct hw *me,
	       hw_ioctl_request request,
	       va_list ap)
{
  m68hc11_info (me);
  return 0;
}

/* generic read/write */

static unsigned
m68hc11cpu_io_read_buffer (struct hw *me,
			   void *dest,
			   int space,
			   unsigned_word base,
			   unsigned nr_bytes)
{
  SIM_DESC sd;
  struct m68hc11cpu *controller = hw_data (me);
  sim_cpu *cpu;
  unsigned byte = 0;
  int result;
  
  HW_TRACE ((me, "read 0x%08lx %d", (long) base, (int) nr_bytes));

  sd  = hw_system (me); 
  cpu = STATE_CPU (sd, 0);

  /* Handle reads for the sub-devices.  */
  base -= controller->attach_address;
  result = sim_core_read_buffer (sd, cpu,
				 io_map, dest, base, nr_bytes);
  if (result > 0)
    return result;
  
  while (nr_bytes)
    {
      if (base >= 0x3F)
	break;

      memcpy (dest, &cpu->ios[base], 1);
      dest++;
      base++;
      byte++;
      nr_bytes--;
    }
  return byte;
}     


static void
m68hc11cpu_io_write (struct hw *me, sim_cpu *cpu,
                     unsigned_word addr, uint8 val)
{
  switch (addr)
    {
    case M6811_PORTA:
      break;

    case M6811_PIOC:
      break;

    case M6811_PORTC:
      break;

    case M6811_PORTB:
      break;

    case M6811_PORTCL:
      break;

    case M6811_DDRC:
      break;

    case M6811_PORTD:
      break;

    case M6811_DDRD:
      break;

    case M6811_TMSK2:
      
      break;
      
      /* Change the RAM and I/O mapping.  */
    case M6811_INIT:
      {
	uint8 old_bank = cpu->ios[M6811_INIT];
	
	cpu->ios[M6811_INIT] = val;

	/* Update IO mapping.  Detach from the old address
	   and attach to the new one.  */
	if ((old_bank & 0xF0) != (val & 0xF0))
	  {
            struct m68hc11cpu *controller = hw_data (me);

            hw_detach_address (hw_parent (me), M6811_IO_LEVEL,
                               controller->attach_space,
                               controller->attach_address,
                               controller->attach_size,
                               me);
            controller->attach_address = (val & 0x0F0) << 12;
            hw_attach_address (hw_parent (me), M6811_IO_LEVEL,
                               controller->attach_space,
                               controller->attach_address,
                               controller->attach_size,
                               me);
	  }
	if ((old_bank & 0x0F) != (val & 0x0F))
	  {
	    ;
	  }
	return;
      }

    /* Writing the config is similar to programing the eeprom.
       The config register value is the last byte of the EEPROM.
       This last byte is not mapped in memory (that's why we have
       to add '1' to 'end_addr').  */
    case M6811_CONFIG:
      {
        return;
      }
      

      /* COP reset.  */
    case M6811_COPRST:
      if (val == 0xAA && cpu->ios[addr] == 0x55)
	{
          val = 0;
          /* COP reset here.  */
	}
      break;
      
    default:
      break;

    }
  cpu->ios[addr] = val;
}

static unsigned
m68hc11cpu_io_write_buffer (struct hw *me,
			    const void *source,
			    int space,
			    unsigned_word base,
			    unsigned nr_bytes)
{
  SIM_DESC sd;
  struct m68hc11cpu *controller = hw_data (me);
  unsigned byte;
  sim_cpu *cpu;
  int result;

  HW_TRACE ((me, "write 0x%08lx %d", (long) base, (int) nr_bytes));

  sd = hw_system (me); 
  cpu = STATE_CPU (sd, 0);  
  base -= controller->attach_address;
  result = sim_core_write_buffer (sd, cpu,
				  io_map, source, base, nr_bytes);
  if (result > 0)
    return result;

  byte = 0;
  while (nr_bytes)
    {
      uint8 val;
      if (base >= 0x3F)
	break;

      val = *((uint8*) source);
      m68hc11cpu_io_write (me, cpu, base, val);
      source++;
      base++;
      byte++;
      nr_bytes--;
    }
  return byte;
}

const struct hw_descriptor dv_m68hc11_descriptor[] = {
  { "m68hc11", m68hc11cpu_finish, },
  { NULL },
};

