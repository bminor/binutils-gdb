/* Simulator for Xilinx MicroBlaze processor
   Copyright 2009-2015 Free Software Foundation, Inc.

   This file is part of GDB, the GNU debugger.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bfd.h"
#include "gdb/callback.h"
#include "libiberty.h"
#include "gdb/remote-sim.h"

#include "sim-main.h"
#include "sim-options.h"

#include "microblaze-dis.h"

#define target_big_endian (CURRENT_TARGET_BYTE_ORDER == BIG_ENDIAN)

static unsigned long
microblaze_extract_unsigned_integer (unsigned char *addr, int len)
{
  unsigned long retval;
  unsigned char *p;
  unsigned char *startaddr = (unsigned char *)addr;
  unsigned char *endaddr = startaddr + len;

  if (len > (int) sizeof (unsigned long))
    printf ("That operation is not available on integers of more than "
	    "%zu bytes.", sizeof (unsigned long));

  /* Start at the most significant end of the integer, and work towards
     the least significant.  */
  retval = 0;

  if (!target_big_endian)
    {
      for (p = endaddr; p > startaddr;)
	retval = (retval << 8) | * -- p;
    }
  else
    {
      for (p = startaddr; p < endaddr;)
	retval = (retval << 8) | * p ++;
    }

  return retval;
}

static void
microblaze_store_unsigned_integer (unsigned char *addr, int len,
				   unsigned long val)
{
  unsigned char *p;
  unsigned char *startaddr = (unsigned char *)addr;
  unsigned char *endaddr = startaddr + len;

  if (!target_big_endian)
    {
      for (p = startaddr; p < endaddr;)
	{
	  *p++ = val & 0xff;
	  val >>= 8;
	}
    }
  else
    {
      for (p = endaddr; p > startaddr;)
	{
	  *--p = val & 0xff;
	  val >>= 8;
	}
    }
}

/* TODO: Convert to common tracing framework.  */
static int issue_messages = 0;

static void /* INLINE */
wbat (SIM_CPU *cpu, word x, word v)
{
  if (((uword)x) >= CPU.msize)
    {
      if (issue_messages)
	fprintf (stderr, "byte write to 0x%x outside memory range\n", x);

      CPU.exception = SIGSEGV;
    }
  else
    {
      unsigned char *p = CPU.memory + x;
      p[0] = v;
    }
}

static void /* INLINE */
wlat (SIM_CPU *cpu, word x, word v)
{
  if (((uword)x) >= CPU.msize)
    {
      if (issue_messages)
	fprintf (stderr, "word write to 0x%x outside memory range\n", x);

      CPU.exception = SIGSEGV;
    }
  else
    {
      if ((x & 3) != 0)
	{
	  if (issue_messages)
	    fprintf (stderr, "word write to unaligned memory address: 0x%x\n", x);

	  CPU.exception = SIGBUS;
	}
      else if (!target_big_endian)
	{
	  unsigned char *p = CPU.memory + x;
	  p[3] = v >> 24;
	  p[2] = v >> 16;
	  p[1] = v >> 8;
	  p[0] = v;
	}
      else
	{
	  unsigned char *p = CPU.memory + x;
	  p[0] = v >> 24;
	  p[1] = v >> 16;
	  p[2] = v >> 8;
	  p[3] = v;
	}
    }
}

static void /* INLINE */
what (SIM_CPU *cpu, word x, word v)
{
  if (((uword)x) >= CPU.msize)
    {
      if (issue_messages)
	fprintf (stderr, "short write to 0x%x outside memory range\n", x);

      CPU.exception = SIGSEGV;
    }
  else
    {
      if ((x & 1) != 0)
	{
	  if (issue_messages)
	    fprintf (stderr, "short write to unaligned memory address: 0x%x\n",
		     x);

	  CPU.exception = SIGBUS;
	}
      else if (!target_big_endian)
	{
	  unsigned char *p = CPU.memory + x;
	  p[1] = v >> 8;
	  p[0] = v;
	}
      else
	{
	  unsigned char *p = CPU.memory + x;
	  p[0] = v >> 8;
	  p[1] = v;
	}
    }
}

/* Read functions.  */
static int /* INLINE */
rbat (SIM_CPU *cpu, word x)
{
  if (((uword)x) >= CPU.msize)
    {
      if (issue_messages)
	fprintf (stderr, "byte read from 0x%x outside memory range\n", x);

      CPU.exception = SIGSEGV;
      return 0;
    }
  else
    {
      unsigned char *p = CPU.memory + x;
      return p[0];
    }
}

static int /* INLINE */
rlat (SIM_CPU *cpu, word x)
{
  if (((uword) x) >= CPU.msize)
    {
      if (issue_messages)
	fprintf (stderr, "word read from 0x%x outside memory range\n", x);

      CPU.exception = SIGSEGV;
      return 0;
    }
  else
    {
      if ((x & 3) != 0)
	{
	  if (issue_messages)
	    fprintf (stderr, "word read from unaligned address: 0x%x\n", x);

	  CPU.exception = SIGBUS;
	  return 0;
	}
      else if (! target_big_endian)
	{
	  unsigned char *p = CPU.memory + x;
	  return (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
	}
      else
	{
	  unsigned char *p = CPU.memory + x;
	  return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
	}
    }
}

static int /* INLINE */
rhat (SIM_CPU *cpu, word x)
{
  if (((uword)x) >= CPU.msize)
    {
      if (issue_messages)
	fprintf (stderr, "short read from 0x%x outside memory range\n", x);

      CPU.exception = SIGSEGV;
      return 0;
    }
  else
    {
      if ((x & 1) != 0)
	{
	  if (issue_messages)
	    fprintf (stderr, "short read from unaligned address: 0x%x\n", x);

	  CPU.exception = SIGBUS;
	  return 0;
	}
      else if (!target_big_endian)
	{
	  unsigned char *p = CPU.memory + x;
	  return (p[1] << 8) | p[0];
	}
      else
	{
	  unsigned char *p = CPU.memory + x;
	  return (p[0] << 8) | p[1];
	}
    }
}

/* TODO: Delete all sim_size and use common memory functions.  */
/* Default to a 8 Mbyte (== 2^23) memory space.  */
static int sim_memory_size = 1 << 23;

#define	MEM_SIZE_FLOOR	64
static void
sim_size (SIM_CPU *cpu, int size)
{
  sim_memory_size = size;
  CPU.msize = sim_memory_size;

  if (CPU.memory)
    free (CPU.memory);

  CPU.memory = (unsigned char *) calloc (1, CPU.msize);

  if (!CPU.memory)
    {
      if (issue_messages)
	fprintf (stderr,
		 "Not enough VM for simulation of %ld bytes of RAM\n",
		 CPU.msize);

      CPU.msize = 1;
      CPU.memory = (unsigned char *) calloc (1, 1);
    }
}

static void
init_pointers (SIM_CPU *cpu)
{
  if (CPU.msize != (sim_memory_size))
    sim_size (cpu, sim_memory_size);
}

static void
set_initial_gprs (SIM_CPU *cpu)
{
  int i;
  long space;
  unsigned long memsize;

  init_pointers (cpu);

  /* Set up machine just out of reset.  */
  PC = 0;
  MSR = 0;

  memsize = CPU.msize / (1024 * 1024);

  if (issue_messages > 1)
    fprintf (stderr, "Simulated memory of %ld Mbytes (0x0 .. 0x%08lx)\n",
	     memsize, CPU.msize - 1);

  /* Clean out the GPRs */
  for (i = 0; i < 32; i++)
    CPU.regs[i] = 0;
  CPU.insts = 0;
  CPU.cycles = 0;
  CPU.imm_enable = 0;
}

static int tracing = 0;

void
sim_resume (SIM_DESC sd, int step, int siggnal)
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);
  int needfetch;
  word inst;
  enum microblaze_instr op;
  int memops;
  int bonus_cycles;
  int insts;
  int w;
  int cycs;
  word WLhash;
  ubyte carry;
  int imm_unsigned;
  short ra, rb, rd;
  long immword;
  uword oldpc, newpc;
  short delay_slot_enable;
  short branch_taken;
  short num_delay_slot; /* UNUSED except as reqd parameter */
  enum microblaze_instr_type insn_type;

  CPU.exception = step ? SIGTRAP : 0;

  memops = 0;
  bonus_cycles = 0;
  insts = 0;

  do
    {
      /* Fetch the initial instructions that we'll decode. */
      inst = rlat (cpu, PC & 0xFFFFFFFC);

      op = get_insn_microblaze (inst, &imm_unsigned, &insn_type,
				&num_delay_slot);

      if (op == invalid_inst)
	fprintf (stderr, "Unknown instruction 0x%04x", inst);

      if (tracing)
	fprintf (stderr, "%.4x: inst = %.4x ", PC, inst);

      rd = GET_RD;
      rb = GET_RB;
      ra = GET_RA;
      /*      immword = IMM_W; */

      oldpc = PC;
      delay_slot_enable = 0;
      branch_taken = 0;
      if (op == microblaze_brk)
	CPU.exception = SIGTRAP;
      else if (inst == MICROBLAZE_HALT_INST)
	{
	  CPU.exception = SIGQUIT;
	  insts += 1;
	  bonus_cycles++;
	}
      else
	{
	  switch(op)
	    {
#define INSTRUCTION(NAME, OPCODE, TYPE, ACTION)		\
	    case NAME:					\
	      ACTION;					\
	      break;
#include "microblaze.isa"
#undef INSTRUCTION

	    default:
	      CPU.exception = SIGILL;
	      fprintf (stderr, "ERROR: Unknown opcode\n");
	    }
	  /* Make R0 consistent */
	  CPU.regs[0] = 0;

	  /* Check for imm instr */
	  if (op == imm)
	    IMM_ENABLE = 1;
	  else
	    IMM_ENABLE = 0;

	  /* Update cycle counts */
	  insts ++;
	  if (insn_type == memory_store_inst || insn_type == memory_load_inst)
	    memops++;
	  if (insn_type == mult_inst)
	    bonus_cycles++;
	  if (insn_type == barrel_shift_inst)
	    bonus_cycles++;
	  if (insn_type == anyware_inst)
	    bonus_cycles++;
	  if (insn_type == div_inst)
	    bonus_cycles += 33;

	  if ((insn_type == branch_inst || insn_type == return_inst)
	      && branch_taken)
	    {
	      /* Add an extra cycle for taken branches */
	      bonus_cycles++;
	      /* For branch instructions handle the instruction in the delay slot */
	      if (delay_slot_enable)
	        {
	          newpc = PC;
	          PC = oldpc + INST_SIZE;
	          inst = rlat (cpu, PC & 0xFFFFFFFC);
	          op = get_insn_microblaze (inst, &imm_unsigned, &insn_type,
					    &num_delay_slot);
	          if (op == invalid_inst)
		    fprintf (stderr, "Unknown instruction 0x%04x", inst);
	          if (tracing)
		    fprintf (stderr, "%.4x: inst = %.4x ", PC, inst);
	          rd = GET_RD;
	          rb = GET_RB;
	          ra = GET_RA;
	          /*	      immword = IMM_W; */
	          if (op == microblaze_brk)
		    {
		      if (issue_messages)
		        fprintf (stderr, "Breakpoint set in delay slot "
			         "(at address 0x%x) will not be honored\n", PC);
		      /* ignore the breakpoint */
		    }
	          else if (insn_type == branch_inst || insn_type == return_inst)
		    {
		      if (issue_messages)
		        fprintf (stderr, "Cannot have branch or return instructions "
			         "in delay slot (at address 0x%x)\n", PC);
		      CPU.exception = SIGILL;
		    }
	          else
		    {
		      switch(op)
		        {
#define INSTRUCTION(NAME, OPCODE, TYPE, ACTION)		\
		        case NAME:			\
			  ACTION;			\
			  break;
#include "microblaze.isa"
#undef INSTRUCTION

		        default:
		          CPU.exception = SIGILL;
		          fprintf (stderr, "ERROR: Unknown opcode at 0x%x\n", PC);
		        }
		      /* Update cycle counts */
		      insts++;
		      if (insn_type == memory_store_inst
		          || insn_type == memory_load_inst)
		        memops++;
		      if (insn_type == mult_inst)
		        bonus_cycles++;
		      if (insn_type == barrel_shift_inst)
		        bonus_cycles++;
		      if (insn_type == anyware_inst)
		        bonus_cycles++;
		      if (insn_type == div_inst)
		        bonus_cycles += 33;
		    }
	          /* Restore the PC */
	          PC = newpc;
	          /* Make R0 consistent */
	          CPU.regs[0] = 0;
	          /* Check for imm instr */
	          if (op == imm)
		    IMM_ENABLE = 1;
	          else
		    IMM_ENABLE = 0;
	        }
	      else
		/* no delay slot: increment cycle count */
		bonus_cycles++;
	    }
	}

      if (tracing)
	fprintf (stderr, "\n");
    }
  while (!CPU.exception);

  /* Hide away the things we've cached while executing.  */
  /*  CPU.pc = pc; */
  CPU.insts += insts;		/* instructions done ... */
  CPU.cycles += insts;		/* and each takes a cycle */
  CPU.cycles += bonus_cycles;	/* and extra cycles for branches */
  CPU.cycles += memops; 	/* and memop cycle delays */
}


int
sim_write (SIM_DESC sd, SIM_ADDR addr, const unsigned char *buffer, int size)
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);
  int i;

  init_pointers (cpu);

  memcpy (&CPU.memory[addr], buffer, size);

  return size;
}

int
sim_read (SIM_DESC sd, SIM_ADDR addr, unsigned char *buffer, int size)
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);
  int i;

  init_pointers (cpu);

  memcpy (buffer, &CPU.memory[addr], size);

  return size;
}


int
sim_store_register (SIM_DESC sd, int rn, unsigned char *memory, int length)
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);

  init_pointers (cpu);

  if (rn < NUM_REGS + NUM_SPECIAL && rn >= 0)
    {
      if (length == 4)
	{
	  /* misalignment safe */
	  long ival = microblaze_extract_unsigned_integer (memory, 4);
	  if (rn < NUM_REGS)
	    CPU.regs[rn] = ival;
	  else
	    CPU.spregs[rn-NUM_REGS] = ival;
	  return 4;
	}
      else
	return 0;
    }
  else
    return 0;
}

int
sim_fetch_register (SIM_DESC sd, int rn, unsigned char *memory, int length)
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);
  long ival;

  init_pointers (cpu);

  if (rn < NUM_REGS + NUM_SPECIAL && rn >= 0)
    {
      if (length == 4)
	{
	  if (rn < NUM_REGS)
	    ival = CPU.regs[rn];
	  else
	    ival = CPU.spregs[rn-NUM_REGS];

	  /* misalignment-safe */
	  microblaze_store_unsigned_integer (memory, 4, ival);
	  return 4;
	}
      else
	return 0;
    }
  else
    return 0;
}

void
sim_stop_reason (SIM_DESC sd, enum sim_stop *reason, int *sigrc)
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);

  if (CPU.exception == SIGQUIT)
    {
      *reason = sim_exited;
      *sigrc = RETREG;
    }
  else
    {
      *reason = sim_stopped;
      *sigrc = CPU.exception;
    }
}

void
sim_info (SIM_DESC sd, int verbose)
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);
  host_callback *callback = STATE_CALLBACK (sd);

  callback->printf_filtered (callback, "\n\n# instructions executed  %10d\n",
			     CPU.insts);
  callback->printf_filtered (callback, "# cycles                 %10d\n",
			     (CPU.cycles) ? CPU.cycles+2 : 0);
}

static void
free_state (SIM_DESC sd)
{
  if (STATE_MODULES (sd) != NULL)
    sim_module_uninstall (sd);
  sim_cpu_free_all (sd);
  sim_state_free (sd);
}

SIM_DESC
sim_open (SIM_OPEN_KIND kind, host_callback *cb, struct bfd *abfd, char **argv)
{
  int i;
  SIM_DESC sd = sim_state_alloc (kind, cb);
  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  /* The cpu data is kept in a separately allocated chunk of memory.  */
  if (sim_cpu_alloc_all (sd, 1, /*cgen_cpu_max_extra_bytes ()*/0) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* getopt will print the error message so we just have to exit if this fails.
     FIXME: Hmmm...  in the case of gdb we need getopt to call
     print_filtered.  */
  if (sim_parse_args (sd, argv) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* Check for/establish the a reference program image.  */
  if (sim_analyze_program (sd,
			   (STATE_PROG_ARGV (sd) != NULL
			    ? *STATE_PROG_ARGV (sd)
			    : NULL), abfd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* Configure/verify the target byte order and other runtime
     configuration options.  */
  if (sim_config (sd) != SIM_RC_OK)
    {
      sim_module_uninstall (sd);
      return 0;
    }

  if (sim_post_argv_init (sd) != SIM_RC_OK)
    {
      /* Uninstall the modules to avoid memory leaks,
	 file descriptor leaks, etc.  */
      sim_module_uninstall (sd);
      return 0;
    }

  if (kind == SIM_OPEN_STANDALONE)
    issue_messages = 1;

  /* CPU specific initialization.  */
  for (i = 0; i < MAX_NR_PROCESSORS; ++i)
    {
      SIM_CPU *cpu = STATE_CPU (sd, i);
      int osize = sim_memory_size;

      set_initial_gprs (cpu);

      /* Discard and reacquire memory -- start with a clean slate.  */
      sim_size (cpu, 1);		/* small */
      sim_size (cpu, osize);	/* and back again */
    }

  return sd;
}

void
sim_close (SIM_DESC sd, int quitting)
{
  /* Do nothing.  */
}

SIM_RC
sim_create_inferior (SIM_DESC sd, struct bfd *prog_bfd, char **argv, char **env)
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);

  PC = bfd_get_start_address (prog_bfd);

  return SIM_RC_OK;
}

void
sim_do_command (SIM_DESC sd, const char *cmd)
{
  SIM_CPU *cpu = STATE_CPU (sd, 0);

  /* Nothing there yet; it's all an error.  */

  if (cmd != NULL)
    {
      char ** simargv = buildargv (cmd);

      if (strcmp (simargv[0], "dumpmem") == 0)
	{
	  unsigned char * p;
	  FILE * dumpfile;

	  if (simargv[1] == NULL)
	    fprintf (stderr, "Error: missing argument to dumpmem cmd.\n");

	  fprintf (stderr, "Writing dumpfile %s...",simargv[1]);

	  dumpfile = fopen (simargv[1], "w");
	  p = CPU.memory;
	  fwrite (p, CPU.msize-1, 1, dumpfile);
	  fclose (dumpfile);

	  fprintf (stderr, "done.\n");
	}
      else if (strcmp (simargv[0], "clearstats") == 0)
	{
	  CPU.cycles = 0;
	  CPU.insts = 0;
	}
      else if (strcmp (simargv[0], "verbose") == 0)
	{
	  issue_messages = 2;
	}
      else
	{
	  fprintf (stderr,"Error: \"%s\" is not a valid M.CORE simulator command.\n",
		   cmd);
	}

      freeargv (simargv);
    }
  else
    {
      fprintf (stderr, "M.CORE sim commands: \n");
      fprintf (stderr, "  dumpmem <filename>\n");
      fprintf (stderr, "  clearstats\n");
      fprintf (stderr, "  verbose\n");
    }
}
