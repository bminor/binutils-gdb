/* interrupts.c -- 68HC11 Interrupts Emulation
   Copyright 1999, 2000 Free Software Foundation, Inc.
   Written by Stephane Carrez (stcarrez@worldnet.fr)

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "sim-main.h"

struct interrupt_def idefs[] = {
  /* Serial interrupts.  */
  { M6811_INT_SCI,      M6811_SCSR,   M6811_TDRE,  M6811_SCCR2,  M6811_TIE },
  { M6811_INT_SCI,      M6811_SCSR,   M6811_TC,    M6811_SCCR2,  M6811_TCIE },
  { M6811_INT_SCI,      M6811_SCSR,   M6811_RDRF,  M6811_SCCR2,  M6811_RIE },
  { M6811_INT_SCI,      M6811_SCSR,   M6811_IDLE,  M6811_SCCR2,  M6811_ILIE },

  /* SPI interrupts.  */
  { M6811_INT_SPI,      M6811_SPSR,   M6811_SPIF,  M6811_SPCR,   M6811_SPIE },

  /* Realtime interrupts.  */
  { M6811_INT_TCTN,     M6811_TFLG2,  M6811_TOF,   M6811_TMSK2,  M6811_TOI },
  { M6811_INT_RT,       M6811_TFLG2,  M6811_RTIF,  M6811_TMSK2,  M6811_RTII },

  /* Output compare interrupts.  */
  { M6811_INT_OUTCMP1,  M6811_TFLG1,  M6811_OC1F,  M6811_TMSK1,  M6811_OC1I },
  { M6811_INT_OUTCMP2,  M6811_TFLG1,  M6811_OC2F,  M6811_TMSK1,  M6811_OC2I },
  { M6811_INT_OUTCMP3,  M6811_TFLG1,  M6811_OC3F,  M6811_TMSK1,  M6811_OC3I },
  { M6811_INT_OUTCMP4,  M6811_TFLG1,  M6811_OC4F,  M6811_TMSK1,  M6811_OC4I },
  { M6811_INT_OUTCMP5,  M6811_TFLG1,  M6811_OC5F,  M6811_TMSK1,  M6811_OC5I },

  /* Input compare interrupts.  */
  { M6811_INT_INCMP1,   M6811_TFLG1,  M6811_IC1F,  M6811_TMSK1,  M6811_IC1I },
  { M6811_INT_INCMP2,   M6811_TFLG1,  M6811_IC2F,  M6811_TMSK1,  M6811_IC2I },
  { M6811_INT_INCMP3,   M6811_TFLG1,  M6811_IC3F,  M6811_TMSK1,  M6811_IC3I },
#if 0
  { M6811_INT_COPRESET, M6811_CONFIG, M6811_NOCOP, 0,            0 },
  { M6811_INT_COPFAIL,  M6811_CONFIG, M6811_NOCOP, 0,            0 }
#endif
};

#define TableSize(X) (sizeof X / sizeof(X[0]))
#define CYCLES_MAX ((((signed64) 1) << 62) - 1)

/* Initialize the interrupts of the processor.  */
int
interrupts_initialize (struct _sim_cpu *proc)
{
  struct interrupts *interrupts = &proc->cpu_interrupts;
  int i;
  
  interrupts->cpu          = proc;
  interrupts->pending_mask = 0;
  interrupts->vectors_addr = 0xffc0;
  interrupts->nb_interrupts_raised = 0;
  interrupts->min_mask_cycles = CYCLES_MAX;
  interrupts->max_mask_cycles = 0;
  interrupts->start_mask_cycle = -1;
  interrupts->xirq_start_mask_cycle = -1;
  interrupts->xirq_max_mask_cycles = 0;
  interrupts->xirq_min_mask_cycles = CYCLES_MAX;
  
  for (i = 0; i < M6811_INT_NUMBER; i++)
    {
      interrupts->interrupt_order[i] = i;
    }
  return 0;
}


/* Update the mask of pending interrupts.  This operation must be called
   when the state of some 68HC11 IO registers changes.  It looks the
   different registers that indicate a pending interrupt (timer, SCI, SPI,
   ...) and records the interrupt if it's there and enabled.  */
void
interrupts_update_pending (struct interrupts *interrupts)
{
  int i;
  uint8 *ioregs;

  ioregs = &interrupts->cpu->ios[0];
  
  for (i = 0; i < TableSize(idefs); i++)
    {
      struct interrupt_def *idef = &idefs[i];
      uint8 data;
      
      /* Look if the interrupt is enabled.  */
      if (idef->enable_paddr)
	{
	  data = ioregs[idef->enable_paddr];
	  if (!(data & idef->enabled_mask))
            {
              /* Disable it.  */
              interrupts->pending_mask &= ~(1 << idef->int_number);
              continue;
            }
	}

      /* Interrupt is enabled, see if it's there.  */
      data = ioregs[idef->int_paddr];
      if (!(data & idef->int_mask))
        {
          /* Disable it.  */
          interrupts->pending_mask &= ~(1 << idef->int_number);
          continue;
        }

      /* Ok, raise it.  */
      interrupts->pending_mask |= (1 << idef->int_number);
    }
}


/* Finds the current active and non-masked interrupt.
   Returns the interrupt number (index in the vector table) or -1
   if no interrupt can be serviced.  */
int
interrupts_get_current (struct interrupts *interrupts)
{
  int i;
  
  if (interrupts->pending_mask == 0)
    return -1;

  /* SWI and illegal instructions are simulated by an interrupt.
     They are not maskable.  */
  if (interrupts->pending_mask & (1 << M6811_INT_SWI))
    {
      interrupts->pending_mask &= ~(1 << M6811_INT_SWI);
      return M6811_INT_SWI;
    }
  if (interrupts->pending_mask & (1 << M6811_INT_ILLEGAL))
    {
      interrupts->pending_mask &= ~(1 << M6811_INT_ILLEGAL);
      return M6811_INT_ILLEGAL;
    }
  
  /* If there is a non maskable interrupt, go for it (unless we are masked
     by the X-bit.  */
  if (interrupts->pending_mask & (1 << M6811_INT_XIRQ))
    {
      if (cpu_get_ccr_X (interrupts->cpu) == 0)
	{
	  interrupts->pending_mask &= ~(1 << M6811_INT_XIRQ);
	  return M6811_INT_XIRQ;
	}
      return -1;
    }

  /* Interrupts are masked, do nothing.  */
  if (cpu_get_ccr_I (interrupts->cpu) == 1)
    {
      return -1;
    }

  /* Returns the first interrupt number which is pending.
     The interrupt priority is specified by the table `interrupt_order'.
     For these interrupts, the pending mask is cleared when the program
     performs some actions on the corresponding device.  If the device
     is not reset, the interrupt remains and will be re-raised when
     we return from the interrupt (see 68HC11 pink book).  */
  for (i = 0; i < M6811_INT_NUMBER; i++)
    {
      enum M6811_INT int_number = interrupts->interrupt_order[i];

      if (interrupts->pending_mask & (1 << int_number))
	{
	  return int_number;
	}
    }
  return -1;
}


/* Process the current interrupt if there is one.  This operation must
   be called after each instruction to handle the interrupts.  If interrupts
   are masked, it does nothing.  */
int
interrupts_process (struct interrupts *interrupts)
{
  int id;
  uint8 ccr;

  /* See if interrupts are enabled/disabled and keep track of the
     number of cycles the interrupts are masked.  Such information is
     then reported by the info command.  */
  ccr = cpu_get_ccr (interrupts->cpu);
  if (ccr & M6811_I_BIT)
    {
      if (interrupts->start_mask_cycle < 0)
        interrupts->start_mask_cycle = cpu_current_cycle (interrupts->cpu);
    }
  else if (interrupts->start_mask_cycle >= 0
           && (ccr & M6811_I_BIT) == 0)
    {
      signed64 t = cpu_current_cycle (interrupts->cpu);

      t -= interrupts->start_mask_cycle;
      if (t < interrupts->min_mask_cycles)
        interrupts->min_mask_cycles = t;
      if (t > interrupts->max_mask_cycles)
        interrupts->max_mask_cycles = t;
      interrupts->start_mask_cycle = -1;
    }
  if (ccr & M6811_X_BIT)
    {
      if (interrupts->xirq_start_mask_cycle < 0)
        interrupts->xirq_start_mask_cycle
	  = cpu_current_cycle (interrupts->cpu);
    }
  else if (interrupts->xirq_start_mask_cycle >= 0
           && (ccr & M6811_X_BIT) == 0)
    {
      signed64 t = cpu_current_cycle (interrupts->cpu);

      t -= interrupts->xirq_start_mask_cycle;
      if (t < interrupts->xirq_min_mask_cycles)
        interrupts->xirq_min_mask_cycles = t;
      if (t > interrupts->xirq_max_mask_cycles)
        interrupts->xirq_max_mask_cycles = t;
      interrupts->xirq_start_mask_cycle = -1;
    }

  id = interrupts_get_current (interrupts);
  if (id >= 0)
    {
      uint16 addr;
      
      cpu_push_all (interrupts->cpu);
      addr = memory_read16 (interrupts->cpu,
                            interrupts->vectors_addr + id * 2);
      cpu_call (interrupts->cpu, addr);

      /* Now, protect from nested interrupts.  */
      if (id == M6811_INT_XIRQ)
	{
	  cpu_set_ccr_X (interrupts->cpu, 1);
	}
      else
	{
	  cpu_set_ccr_I (interrupts->cpu, 1);
	}

      interrupts->nb_interrupts_raised++;
      cpu_add_cycles (interrupts->cpu, 14);
      return 1;
    }
  return 0;
}

void
interrupts_raise (struct interrupts *interrupts, enum M6811_INT number)
{
  interrupts->pending_mask |= (1 << number);
  interrupts->nb_interrupts_raised ++;
}



void
interrupts_info (SIM_DESC sd, struct interrupts *interrupts)
{
  signed64 t;
  
  if (interrupts->start_mask_cycle >= 0)
    {
      t = cpu_current_cycle (interrupts->cpu);

      t -= interrupts->start_mask_cycle;
      if (t > interrupts->max_mask_cycles)
        interrupts->max_mask_cycles = t;
    }
  if (interrupts->xirq_start_mask_cycle >= 0)
    {
      t = cpu_current_cycle (interrupts->cpu);

      t -= interrupts->xirq_start_mask_cycle;
      if (t > interrupts->xirq_max_mask_cycles)
        interrupts->xirq_max_mask_cycles = t;
    }

  sim_io_printf (sd, "Interrupts Info:\n");
  sim_io_printf (sd, "  Interrupts raised: %lu\n",
                 interrupts->nb_interrupts_raised);

  t = interrupts->min_mask_cycles == CYCLES_MAX ?
    interrupts->max_mask_cycles :
    interrupts->min_mask_cycles;
  sim_io_printf (sd, "  Shortest interrupts masked sequence: %s\n",
                 cycle_to_string (interrupts->cpu, t));

  t = interrupts->max_mask_cycles;
  sim_io_printf (sd, "  Longest interrupts masked sequence: %s\n",
                 cycle_to_string (interrupts->cpu, t));

  t = interrupts->xirq_min_mask_cycles == CYCLES_MAX ?
    interrupts->xirq_max_mask_cycles :
    interrupts->xirq_min_mask_cycles;
  sim_io_printf (sd, "  XIRQ Min interrupts masked sequence: %s\n",
                 cycle_to_string (interrupts->cpu, t));

  t = interrupts->xirq_max_mask_cycles;
  sim_io_printf (sd, "  XIRQ Max interrupts masked sequence: %s\n",
                 cycle_to_string (interrupts->cpu, t));
}
