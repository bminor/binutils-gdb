/* m6811_cpu.c -- 68HC11 CPU Emulation
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
#include "sim-assert.h"
#include "sim-module.h"
#include "sim-options.h"

void cpu_free_frame (sim_cpu* cpu, struct cpu_frame *frame);

enum {
  OPTION_CPU_RESET = OPTION_START,
  OPTION_EMUL_OS,
  OPTION_CPU_CONFIG,
  OPTION_CPU_MODE
};

static DECLARE_OPTION_HANDLER (cpu_option_handler);

static const OPTION cpu_options[] =
{
  { {"cpu-reset", no_argument, NULL, OPTION_CPU_RESET },
      '\0', NULL, "Reset the CPU",
      cpu_option_handler },

  { {"emulos",    no_argument, NULL, OPTION_EMUL_OS },
      '\0', NULL, "Emulate some OS system calls (read, write, ...)",
      cpu_option_handler },

  { {"cpu-config", required_argument, NULL, OPTION_CPU_CONFIG },
      '\0', NULL, "Specify the initial CPU configuration register",
      cpu_option_handler },

  { {NULL, no_argument, NULL, 0}, '\0', NULL, NULL, NULL }
};


static SIM_RC
cpu_option_handler (SIM_DESC sd, sim_cpu *cpu,
                    int opt, char *arg, int is_command)
{
  sim_cpu *cpu;
  int val;
  
  cpu = STATE_CPU (sd, 0);
  switch (opt)
    {
    case OPTION_CPU_RESET:
      sim_board_reset (sd);
      break;

    case OPTION_EMUL_OS:
      cpu->cpu_emul_syscall = 1;
      break;

    case OPTION_CPU_CONFIG:
      if (sscanf(arg, "0x%x", &val) == 1
          || sscanf(arg, "%d", &val) == 1)
        {
          cpu->cpu_config = val;
          cpu->cpu_use_local_config = 1;
        }
      else
        cpu->cpu_use_local_config = 0;
      break;
      
    case OPTION_CPU_MODE:
      break;
    }

  return SIM_RC_OK;
}

/* Tentative to keep track of the cpu frame.  */
struct cpu_frame*
cpu_find_frame (sim_cpu *cpu, uint16 sp)
{
  struct cpu_frame_list *flist;

  flist = cpu->cpu_frames;
  while (flist)
    {
      struct cpu_frame *frame;

      frame = flist->frame;
      while (frame)
	{
	  if (frame->sp_low <= sp && frame->sp_high >= sp)
	    {
	      cpu->cpu_current_frame = flist;
	      return frame;
	    }

	  frame = frame->up;
	}
      flist = flist->next;
    }
  return 0;
}

struct cpu_frame_list*
cpu_create_frame_list (sim_cpu *cpu)
{
  struct cpu_frame_list *flist;

  flist = (struct cpu_frame_list*) malloc (sizeof (struct cpu_frame_list));
  flist->frame = 0;
  flist->next  = cpu->cpu_frames;
  flist->prev  = 0;
  if (flist->next)
    flist->next->prev = flist;
  cpu->cpu_frames = flist;
  cpu->cpu_current_frame = flist;
  return flist;
}

void
cpu_remove_frame_list (sim_cpu *cpu, struct cpu_frame_list *flist)
{
  struct cpu_frame *frame;
  
  if (flist->prev == 0)
    cpu->cpu_frames = flist->next;
  else
    flist->prev->next = flist->next;
  if (flist->next)
    flist->next->prev = flist->prev;

  frame = flist->frame;
  while (frame)
    {
      struct cpu_frame* up = frame->up;
      cpu_free_frame (cpu, frame);
      frame = up;
    }
  free (flist);
}
  
    
struct cpu_frame*
cpu_create_frame (sim_cpu *cpu, uint16 pc, uint16 sp)
{
  struct cpu_frame *frame;

  frame = (struct cpu_frame*) malloc (sizeof(struct cpu_frame));
  frame->up = 0;
  frame->pc = pc;
  frame->sp_low  = sp;
  frame->sp_high = sp;
  return frame;
}

void
cpu_free_frame (sim_cpu *cpu, struct cpu_frame *frame)
{
  free (frame);
}

uint16
cpu_frame_reg (sim_cpu *cpu, uint16 rn)
{
  struct cpu_frame *frame;

  if (cpu->cpu_current_frame == 0)
    return 0;
  
  frame = cpu->cpu_current_frame->frame;
  while (frame)
    {
      if (rn == 0)
	return frame->sp_high;
      frame = frame->up;
      rn--;
    }
  return 0;
}
  
void
cpu_call (sim_cpu *cpu, uint16 addr)
{
#if HAVE_FRAME
  uint16 pc = cpu->cpu_insn_pc;
  uint16 sp;
  struct cpu_frame_list *flist;
  struct cpu_frame* frame;
  struct cpu_frame* new_frame;
#endif

  cpu_set_pc (cpu, addr);
#if HAVE_FRAME
  sp = cpu_get_sp (cpu);

  cpu->cpu_need_update_frame = 0;
  flist = cpu->cpu_current_frame;
  if (flist == 0)
    flist = cpu_create_frame_list (cpu);

  frame = flist->frame;
  if (frame && frame->sp_low > sp)
    frame->sp_low = sp;

  new_frame = cpu_create_frame (cpu, pc, sp);
  new_frame->up = frame;
  flist->frame = new_frame;
#endif
}

void
cpu_update_frame (sim_cpu *cpu, int do_create)
{
#if HAVE_FRAME
  struct cpu_frame *frame;

  frame = cpu_find_frame (cpu, cpu_get_sp (cpu));
  if (frame)
    {
      while (frame != cpu->cpu_current_frame->frame)
	{
	  struct cpu_frame* up;
	  
	  up = cpu->cpu_current_frame->frame->up;
	  cpu_free_frame (cpu, cpu->cpu_current_frame->frame);
	  cpu->cpu_current_frame->frame = up;
	}
      return;
    }

  if (do_create)
    {
      cpu_create_frame_list (cpu);
      frame = cpu_create_frame (cpu, cpu_get_pc (cpu), cpu_get_sp (cpu));
      cpu->cpu_current_frame->frame = frame;
    }
#endif
}

void
cpu_return (sim_cpu *cpu)
{
#if HAVE_FRAME
  uint16 sp = cpu_get_sp (cpu);
  struct cpu_frame *frame;
  struct cpu_frame_list *flist;

  cpu->cpu_need_update_frame = 0;
  flist = cpu->cpu_current_frame;
  if (flist && flist->frame && flist->frame->up)
    {
      frame = flist->frame->up;
      if (frame->sp_low <= sp && frame->sp_high >= sp)
	{
	  cpu_free_frame (cpu, flist->frame);
	  flist->frame = frame;
	  return;
	}
    }
  cpu_update_frame (cpu, 1);
#endif
}

void
cpu_print_frame (SIM_DESC sd, sim_cpu *cpu)
{
  struct cpu_frame* frame;
  int level = 0;
  
  if (cpu->cpu_current_frame == 0 || cpu->cpu_current_frame->frame == 0)
    {
      sim_io_printf (sd, "No frame.\n");
      return;
    }
  sim_io_printf (sd, " #   PC   SP-L  SP-H\n");
  frame = cpu->cpu_current_frame->frame;
  while (frame)
    {
      sim_io_printf (sd, "%3d 0x%04x 0x%04x 0x%04x\n",
		     level, frame->pc, frame->sp_low, frame->sp_high);
      frame = frame->up;
      level++;
    }
}

/* Set the stack pointer and re-compute the current frame.  */
void
cpu_set_sp (sim_cpu *cpu, uint16 val)
{
  cpu->cpu_regs.sp = val;
  cpu_update_frame (cpu, 0);
}

int
cpu_initialize (SIM_DESC sd, sim_cpu *cpu)
{
  int result;
  
  sim_add_option_table (sd, 0, cpu_options);

  memset (&cpu->cpu_regs, 0, sizeof(cpu->cpu_regs));

  cpu->cpu_absolute_cycle = 0;
  cpu->cpu_current_cycle  = 0;
  cpu->cpu_emul_syscall   = 1;
  cpu->cpu_running        = 1;
  cpu->cpu_stop_on_interrupt = 0;
  cpu->cpu_frequency = 8 * 1000 * 1000;
  cpu->cpu_frames = 0;
  cpu->cpu_current_frame = 0;
  cpu->cpu_use_elf_start = 0;
  cpu->cpu_elf_start     = 0;
  cpu->cpu_use_local_config = 0;
  cpu->cpu_config        = M6811_NOSEC | M6811_NOCOP | M6811_ROMON |
    M6811_EEON;
  result = interrupts_initialize (cpu);

  cpu->cpu_is_initialized = 1;
  return result;
}


/* Reinitialize the processor after a reset.  */
int
cpu_reset (sim_cpu *cpu)
{
  cpu->cpu_need_update_frame = 0;
  cpu->cpu_current_frame = 0;
  while (cpu->cpu_frames)
    cpu_remove_frame_list (cpu, cpu->cpu_frames);

  /* Initialize the config register.
     It is only initialized at reset time.  */
  memset (cpu->ios, 0, sizeof (cpu->ios));
  cpu->ios[M6811_INIT] = 0x1;

  /* Output compare registers set to 0xFFFF.  */
  cpu->ios[M6811_TOC1_H] = 0xFF;
  cpu->ios[M6811_TOC1_L] = 0xFF;
  cpu->ios[M6811_TOC2_H] = 0xFF;
  cpu->ios[M6811_TOC2_L] = 0xFF;
  cpu->ios[M6811_TOC3_H] = 0xFF;
  cpu->ios[M6811_TOC4_L] = 0xFF;
  cpu->ios[M6811_TOC5_H] = 0xFF;
  cpu->ios[M6811_TOC5_L] = 0xFF;

  /* Setup the processor registers.  */
  memset (&cpu->cpu_regs, 0, sizeof(cpu->cpu_regs));
  cpu->cpu_absolute_cycle = 0;
  cpu->cpu_current_cycle  = 0;
  cpu->cpu_is_initialized = 0;

  /* Reinitialize the CPU operating mode.  */
  cpu->ios[M6811_HPRIO] = cpu->cpu_mode;
  return 0;
}

/* Reinitialize the processor after a reset.  */
int
cpu_restart (sim_cpu *cpu)
{
  uint16 addr;

  /* Get CPU starting address depending on the CPU mode.  */
  if (cpu->cpu_use_elf_start == 0)
    {
      switch ((cpu->ios[M6811_HPRIO]) & (M6811_SMOD | M6811_MDA))
        {
          /* Single Chip  */
        default:
        case 0 :
          addr = memory_read16 (cpu, 0xFFFE);
          break;

          /* Expanded Multiplexed  */
        case M6811_MDA:
          addr = memory_read16 (cpu, 0xFFFE);
          break;

          /* Special Bootstrap  */
        case M6811_SMOD:
          addr = 0;
          break;

          /* Factory Test  */
        case M6811_MDA | M6811_SMOD:
          addr = memory_read16 (cpu, 0xFFFE);
          break;
        }
    }
  else
    {
      addr = cpu->cpu_elf_start;
    }
  
  /* Setup the processor registers.  */
  cpu->cpu_insn_pc  = addr;
  cpu->cpu_regs.pc  = addr;
  cpu->cpu_regs.ccr = M6811_X_BIT | M6811_I_BIT | M6811_S_BIT;
  cpu->cpu_absolute_cycle = 0;
  cpu->cpu_is_initialized = 1;
  cpu->cpu_current_cycle  = 0;

  cpu_call (cpu, addr);
  
  return 0;
}

void
print_io_reg_desc (SIM_DESC sd, io_reg_desc *desc, int val, int mode)
{
  while (desc->mask)
    {
      if (val & desc->mask)
	sim_io_printf (sd, "%s",
		       mode == 0 ? desc->short_name : desc->long_name);
      desc++;
    }
}

void
print_io_byte (SIM_DESC sd, const char *name, io_reg_desc *desc,
	       uint8 val, uint16 addr)
{
  sim_io_printf (sd, "  %-9.9s @ 0x%04x 0x%02x ", name, addr, val);
  if (desc)
    print_io_reg_desc (sd, desc, val, 0);
}

void
cpu_ccr_update_tst8 (sim_cpu *proc, uint8 val)
{
  cpu_set_ccr_V (proc, 0);
  cpu_set_ccr_N (proc, val & 0x80 ? 1 : 0);
  cpu_set_ccr_Z (proc, val == 0 ? 1 : 0);
}


uint16
cpu_fetch_relbranch (sim_cpu *cpu)
{
  uint16 addr = (uint16) cpu_fetch8 (cpu);

  if (addr & 0x0080)
    {
      addr |= 0xFF00;
    }
  addr += cpu->cpu_regs.pc;
  return addr;
}


/* Push all the CPU registers (when an interruption occurs).  */
void
cpu_push_all (sim_cpu *cpu)
{
  cpu_push_uint16 (cpu, cpu->cpu_regs.pc);
  cpu_push_uint16 (cpu, cpu->cpu_regs.iy);
  cpu_push_uint16 (cpu, cpu->cpu_regs.ix);
  cpu_push_uint16 (cpu, cpu->cpu_regs.d);
  cpu_push_uint8 (cpu, cpu->cpu_regs.ccr);
}


/* Handle special instructions.  */
void
cpu_special (sim_cpu *cpu, enum M6811_Special special)
{
  switch (special)
    {
    case M6811_RTI:
      {
        uint8 ccr;

        ccr = cpu_pop_uint8 (cpu);
        cpu_set_ccr (cpu, ccr);
        cpu_set_d (cpu, cpu_pop_uint16 (cpu));
        cpu_set_x (cpu, cpu_pop_uint16 (cpu));
        cpu_set_y (cpu, cpu_pop_uint16 (cpu));
        cpu_set_pc (cpu, cpu_pop_uint16 (cpu));
	cpu_return (cpu);
        break;
      }
      
    case M6811_WAI:
      /* In the ELF-start mode, we are in a special mode where
	 the WAI corresponds to an exit.  */
      if (cpu->cpu_use_elf_start)
        {
          cpu_set_pc (cpu, cpu->cpu_insn_pc);
          sim_engine_halt (CPU_STATE (cpu), cpu,
                           NULL, NULL_CIA, sim_exited,
                           cpu_get_d (cpu));
          return;
        }
      /* SCz: not correct... */
      cpu_push_all (cpu);
      break;
      
    case M6811_SWI:
      interrupts_raise (&cpu->cpu_interrupts, M6811_INT_SWI);
      interrupts_process (&cpu->cpu_interrupts);
      break;
      
    case M6811_EMUL_SYSCALL:
    case M6811_ILLEGAL:
      if (cpu->cpu_emul_syscall)
        {
          uint8 op = memory_read8 (cpu,
                                   cpu_get_pc (cpu) - 1);
          if (op == 0x41)
            {
	      cpu_set_pc (cpu, cpu->cpu_insn_pc);
	      sim_engine_halt (CPU_STATE (cpu), cpu,
			       NULL, NULL_CIA, sim_exited,
			       cpu_get_d (cpu));
	      return;
            }
          else
            {
              emul_os (op, cpu);
            }
          return;
        }
      
      interrupts_raise (&cpu->cpu_interrupts, M6811_INT_ILLEGAL);
      interrupts_process (&cpu->cpu_interrupts);
      break;

    case M6811_TEST:
      {
        SIM_DESC sd;

        sd = CPU_STATE (cpu);

        /* Breakpoint instruction if we are under gdb.  */
        if (STATE_OPEN_KIND (sd) == SIM_OPEN_DEBUG)
          {
            cpu->cpu_regs.pc --;
            sim_engine_halt (CPU_STATE (cpu), cpu,
                             0, cpu_get_pc (cpu), sim_stopped,
                             SIM_SIGTRAP);
          }
        /* else this is a nop but not in test factory mode.  */
        break;
      }
    }
}


void
cpu_single_step (sim_cpu *cpu)
{
  cpu->cpu_current_cycle = 0;
  cpu->cpu_insn_pc = cpu_get_pc (cpu);

  /* Handle the pending interrupts.  If an interrupt is handled,
     treat this as an single step.  */
  if (interrupts_process (&cpu->cpu_interrupts))
    {
      cpu->cpu_absolute_cycle += cpu->cpu_current_cycle;
      return;
    }
  
  /*  printf("PC = 0x%04x\n", cpu_get_pc (cpu));*/
  cpu_interp (cpu);
  cpu->cpu_absolute_cycle += cpu->cpu_current_cycle;
}

/* VARARGS */
void
sim_memory_error (sim_cpu *cpu, SIM_SIGNAL excep,
		  uint16 addr, const char *message, ...)
{
  char buf[1024];
  va_list args;

  va_start (args, message);
  vsprintf (buf, message, args);
  va_end (args);

  printf("%s\n", buf);
  cpu_memory_exception (cpu, excep, addr, buf);
}


void
cpu_memory_exception (sim_cpu *cpu, SIM_SIGNAL excep,
                      uint16 addr, const char *message)
{
  if (cpu->cpu_running == 0)
    return;

  cpu_set_pc (cpu, cpu->cpu_insn_pc);
  sim_engine_halt (CPU_STATE (cpu), cpu, NULL,
                   cpu_get_pc (cpu), sim_stopped, excep);
  
#if 0
  cpu->mem_exception = excep;
  cpu->fault_addr    = addr;
  cpu->fault_msg     = strdup (message);

  if (cpu->cpu_use_handler)
    {
      longjmp (&cpu->cpu_exception_handler, 1);
    }
  (* cpu->callback->printf_filtered)
    (cpu->callback, "Fault at 0x%04x: %s\n", addr, message);
#endif
}

void
cpu_info (SIM_DESC sd, sim_cpu *cpu)
{
  sim_io_printf (sd, "CPU info:\n");
  sim_io_printf (sd, "  Absolute cycle: %llu\n",
                 cpu->cpu_absolute_cycle);
  sim_io_printf (sd, "  Syscall emulation: %s\n",
                 cpu->cpu_emul_syscall ? "yes, via 0xcd <n>" : "no");
  sim_io_printf (sd, "  Memory errors detection: %s\n",
                 cpu->cpu_check_memory ? "yes" : "no");
  sim_io_printf (sd, "  Stop on interrupt: %s\n",
                 cpu->cpu_stop_on_interrupt ? "yes" : "no");
}

