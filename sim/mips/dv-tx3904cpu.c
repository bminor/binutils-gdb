/*  This file is part of the program GDB, the GU debugger.
    
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
#include "hw-base.h"

/* DEVICE

   
   tx3904cpu - tx3904 cpu virtual device

   
   DESCRIPTION

   
   Implements the external tx3904 functionality.  This includes the
   delivery of of interrupts generated from other devices and the
   handling of device specific registers.


   PROPERTIES
   
   none


   PORTS


   reset (input)

   Currently ignored.


   nmi (input)

   Deliver a non-maskable interrupt to the processor.


   level (input)

   Deliver a maskable interrupt of given level, corresponding to
   IP[5:0], to processor.



   BUGS


   When delivering an interrupt, this code assumes that there is only
   one processor (number 0).

   This code does not attempt to be efficient at handling pending
   interrupts.  It simply schedules the interrupt delivery handler
   every instruction cycle until all pending interrupts go away.  An
   alternative implementation might modify instructions that change
   the PSW and have them check to see if the change makes an interrupt
   delivery possible.

   */



struct tx3904cpu {
  /* Pending interrupts for delivery by event handler */
  int pending_reset, pending_nmi, pending_level;
};



/* input port ID's */ 

enum {
  RESET_PORT,
  NMI_PORT,
  LEVEL_PORT,
};


static const struct hw_port_descriptor tx3904cpu_ports[] = {

  /* interrupt inputs */
  { "reset", RESET_PORT, 0, input_port, },
  { "nmi", NMI_PORT, 0, input_port, },
  { "level", LEVEL_PORT, 0, input_port, },

  { NULL, },
};


/* Finish off the partially created hw device.  Attach our local
   callbacks.  Wire up our port names etc */

static hw_port_event_callback tx3904cpu_port_event;



static void
tx3904cpu_finish (struct hw *me)
{
  struct tx3904cpu *controller;

  controller = HW_ZALLOC (me, struct tx3904cpu);
  set_hw_data (me, controller);
  set_hw_ports (me, tx3904cpu_ports);
  set_hw_port_event (me, tx3904cpu_port_event);

  /* Initialize the pending interrupt flags */
  controller->pending_level = 0;
  controller->pending_reset = 0;
  controller->pending_nmi = 0;
}



/* An event arrives on an interrupt port */

static void
deliver_tx3904cpu_interrupt (struct hw *me,
			    void *data)
{
  struct tx3904cpu *controller = hw_data (me);
  SIM_DESC simulator = hw_system (me);
  sim_cpu *cpu = STATE_CPU (simulator, 0); /* NB: fix CPU 0. */
  address_word cia = CIA_GET (cpu);

#define CPU cpu
#define SD current_state

  if (controller->pending_reset)
    {
      controller->pending_reset = 0;
      HW_TRACE ((me, "reset pc=0x%08lx", (long) CIA_GET (cpu)));
      SignalExceptionNMIReset();
    }
  else if (controller->pending_nmi)
    {
      controller->pending_nmi = 0;
      HW_TRACE ((me, "nmi pc=0x%08lx", (long) CIA_GET (cpu)));
      SignalExceptionNMIReset();
    }
  else if (controller->pending_level)
    {
      HW_TRACE ((me, "interrupt level=%d pc=0x%08lx sr=0x%08lx",
		 controller->pending_level,
		 (long) CIA_GET (cpu), (long) SR));

      /* Don't overwrite the CAUSE field since we have no good place to clear
	 it again.  The specs allow it to be zero by the time the interrupt
	 handler is invoked. */
      /* CAUSE &= ~ (cause_IP_mask << cause_IP_shift);
	 CAUSE |= (controller->pending_level & cause_IP_mask) << cause_IP_shift; */

      /* check for enabled / unmasked interrupts */
      if((SR & status_IEc) &&
	 (controller->pending_level & ((SR >> status_IM_shift) & status_IM_mask)))
	{
	  controller->pending_level = 0;
	  SignalExceptionInterrupt();
	}
      else
	{
	  /* reschedule soon */
	  hw_event_queue_schedule (me, 1, deliver_tx3904cpu_interrupt, NULL);
	}
    }
#undef CPU cpu
#undef SD current_state
}


static void
tx3904cpu_port_event (struct hw *me,
		     int my_port,
		     struct hw *source,
		     int source_port,
		     int level)
{
  struct tx3904cpu *controller = hw_data (me);

  switch (my_port)
    {      
    case RESET_PORT:
      controller->pending_reset = 1;
      HW_TRACE ((me, "port-in reset"));
      break;
      
    case NMI_PORT:
      controller->pending_nmi = 1;
      HW_TRACE ((me, "port-in nmi"));
      break;
      
    case LEVEL_PORT:
      controller->pending_level |= level; /* accumulate bits until they are cleared */
      HW_TRACE ((me, "port-in level=%d", level));
      break;
      
    default:
      hw_abort (me, "bad switch");
      break;
    }

  /* Schedule an event to be delivered immediately after current
     instruction. */
  hw_event_queue_schedule (me, 0, deliver_tx3904cpu_interrupt, NULL);
}


const struct hw_device_descriptor dv_tx3904cpu_descriptor[] = {
  { "tx3904cpu", tx3904cpu_finish, },
  { NULL },
};
