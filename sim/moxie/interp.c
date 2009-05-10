/* Simulator for the moxie processor
   Copyright (C) 2008, 2009 Free Software Foundation, Inc.
   Contributed by Anthony Green

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
along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <signal.h>
#include <stdlib.h>
#include "sysdep.h"
#include <sys/times.h>
#include <sys/param.h>
#include <netinet/in.h>	/* for byte ordering macros */
#include "bfd.h"
#include "gdb/callback.h"
#include "libiberty.h"
#include "gdb/remote-sim.h"

typedef int word;
typedef unsigned int uword;

host_callback *       callback;

FILE *tracefile;

#define EXTRACT_WORD(addr) (((addr)[0] << 24) \
			    + ((addr)[1] << 16) \
			    + ((addr)[2] << 8) \
			    + ((addr)[3]))

unsigned long
moxie_extract_unsigned_integer (addr, len)
     unsigned char * addr;
     int len;
{
  unsigned long retval;
  unsigned char * p;
  unsigned char * startaddr = (unsigned char *)addr;
  unsigned char * endaddr = startaddr + len;
 
  if (len > (int) sizeof (unsigned long))
    printf ("That operation is not available on integers of more than %d bytes.",
	    sizeof (unsigned long));
 
  /* Start at the most significant end of the integer, and work towards
     the least significant.  */
  retval = 0;

  for (p = endaddr; p > startaddr;)
    retval = (retval << 8) | * -- p;
  
  return retval;
}

void
moxie_store_unsigned_integer (addr, len, val)
     unsigned char * addr;
     int len;
     unsigned long val;
{
  unsigned char * p;
  unsigned char * startaddr = (unsigned char *)addr;
  unsigned char * endaddr = startaddr + len;

  for (p = endaddr; p > startaddr;)
    {
      * -- p = val & 0xff;
      val >>= 8;
    }
}

/* moxie register names.  */
static const char *reg_names[16] = 
  { "$fp", "$sp", "$r0", "$r1", "$r2", "$r3", "$r4", "$r5", 
    "$r6", "$r7", "$r8", "$r9", "$r10", "$r11", "$r12", "$r13" };

/* The machine state.

   This state is maintained in host byte order.  The fetch/store
   register functions must translate between host byte order and the
   target processor byte order.  Keeping this data in target byte
   order simplifies the register read/write functions.  Keeping this
   data in native order improves the performance of the simulator.
   Simulation speed is deemed more important.  */

#define NUM_MOXIE_REGS 17 /* Including PC */
#define NUM_MOXIE_SREGS 256 /* The special registers */
#define PC_REGNO     16

/* The ordering of the moxie_regset structure is matched in the
   gdb/config/moxie/tm-moxie.h file in the REGISTER_NAMES macro.  */
struct moxie_regset
{
  word		  regs[NUM_MOXIE_REGS + 1]; /* primary registers */
  word		  sregs[256];             /* special registers */
  word            cc;                   /* the condition code reg */
  int		  exception;
  unsigned long   msize;
  unsigned char * memory;
  unsigned long long insts;                /* instruction counter */
};

#define CC_GT  1<<0
#define CC_LT  1<<1
#define CC_EQ  1<<2
#define CC_GTU 1<<3
#define CC_LTU 1<<4

union
{
  struct moxie_regset asregs;
  word asints [1];		/* but accessed larger... */
} cpu;

static char *myname;
static SIM_OPEN_KIND sim_kind;
static int issue_messages = 0;

/* Default to a 8 Mbyte (== 2^23) memory space.  */
static int sim_memory_size = 23;

#define	MEM_SIZE_FLOOR	64
void
sim_size (power)
     int power;
{
  sim_memory_size = power;
  cpu.asregs.msize = 1 << sim_memory_size;

  if (cpu.asregs.memory)
    free (cpu.asregs.memory);

  /* Watch out for the '0 count' problem. There's probably a better
     way.. e.g., why do we use 64 here?  */
  if (cpu.asregs.msize < 64)	/* Ensure a boundary.  */
    cpu.asregs.memory = (unsigned char *) calloc (64, (64 + cpu.asregs.msize) / 64);
  else
    cpu.asregs.memory = (unsigned char *) calloc (64, cpu.asregs.msize / 64);

  if (!cpu.asregs.memory)
    {
      if (issue_messages)
	fprintf (stderr,
		 "Not enough VM for simulation of %d bytes of RAM\n",
		 cpu.asregs.msize);

      cpu.asregs.msize = 1;
      cpu.asregs.memory = (unsigned char *) calloc (1, 1);
    }
}

static void
init_pointers ()
{
  if (cpu.asregs.msize != (1 << sim_memory_size))
    sim_size (sim_memory_size);
}


static void
set_initial_gprs ()
{
  int i;
  long space;
  unsigned long memsize;
  
  init_pointers ();

  /* Set up machine just out of reset.  */
  cpu.asregs.regs[PC_REGNO] = 0;
  
  memsize = cpu.asregs.msize / (1024 * 1024);

  if (issue_messages > 1)
    fprintf (stderr, "Simulated memory of %d Mbytes (0x0 .. 0x%08x)\n",
	     memsize, cpu.asregs.msize - 1);

  /* Clean out the register contents.  */
  for (i = 0; i < NUM_MOXIE_REGS; i++)
    cpu.asregs.regs[i] = 0;
  for (i = 0; i < NUM_MOXIE_SREGS; i++)
    cpu.asregs.sregs[i] = 0;
}

static void
interrupt ()
{
  cpu.asregs.exception = SIGINT;
}

/* Write a 1 byte value to memory.  */

static void INLINE 
wbat (pc, x, v)
     word pc, x, v;
{
  if (((uword)x) >= cpu.asregs.msize)
    {
      if (issue_messages)
	fprintf (stderr, "byte write to 0x%x outside memory range\n", x);
      
      cpu.asregs.exception = SIGSEGV;
    }
  else
    {
      {
	unsigned char * p = cpu.asregs.memory + x;
	*p = v;
      }
    }
}

/* Write a 2 byte value to memory.  */

static void INLINE 
wsat (pc, x, v)
     word pc, x, v;
{
  if (((uword)x) >= cpu.asregs.msize)
    {
      if (issue_messages)
	fprintf (stderr, "short word write to 0x%x outside memory range\n", x);
      
      cpu.asregs.exception = SIGSEGV;
    }
  else
    {
      if ((x & 1) != 0)
	{
	  if (issue_messages)
	    fprintf (stderr, "short word write to unaligned memory address: 0x%x\n", x);
      
	  cpu.asregs.exception = SIGBUS;
	}
      {
	unsigned char * p = cpu.asregs.memory + x;
	p[0] = v >> 8;
	p[1] = v;
      }
    }
}

/* Write a 4 byte value to memory.  */

static void INLINE 
wlat (pc, x, v)
     word pc, x, v;
{
  if (((uword)x) >= cpu.asregs.msize)
    {
      if (issue_messages)
	fprintf (stderr, "word write to 0x%x outside memory range\n", x);
      
      cpu.asregs.exception = SIGSEGV;
    }
  else
    {
      if ((x & 1) != 0)
	{
	  if (issue_messages)
	    fprintf (stderr, "word write to unaligned memory address: 0x%x\n", x);
      
	  cpu.asregs.exception = SIGBUS;
	}
      {
	unsigned char * p = cpu.asregs.memory + x;
	p[0] = v >> 24;
	p[1] = v >> 16;
	p[2] = v >> 8;
	p[3] = v;
      }
    }
}

/* Read 2 bytes from memory.  */

static int INLINE 
rsat (pc, x)
     word pc, x;
{
  if (((uword) x) >= cpu.asregs.msize)
    {
      if (issue_messages)
	fprintf (stderr, "short word read from 0x%x outside memory range\n", x);
      
      cpu.asregs.exception = SIGSEGV;
      return 0;
    }
  else
    {
      if ((x & 1) != 0)
	{
	  if (issue_messages)
	    fprintf (stderr, "short word read from unaligned address: 0x%x\n", x);
      
	  cpu.asregs.exception = SIGBUS;
	  return 0;
	}
      {
	unsigned char * p = cpu.asregs.memory + x;
	return (p[0] << 8) | p[1];
      }
    }
}

/* Read 1 byte from memory.  */

static int INLINE 
rbat (pc, x)
     word pc, x;
{
  if (((uword) x) >= cpu.asregs.msize)
    {
      if (issue_messages)
	fprintf (stderr, "byte read from 0x%x outside memory range\n", x);
      
      cpu.asregs.exception = SIGSEGV;
      return 0;
    }
  else
    {
      unsigned char * p = cpu.asregs.memory + x;
      return *p;
    }
}

/* Read 4 bytes from memory.  */

static int INLINE 
rlat (pc, x)
     word pc, x;
{
  if (((uword) x) >= cpu.asregs.msize)
    {
      if (issue_messages)
	fprintf (stderr, "word read from 0x%x outside memory range\n", x);
      
      cpu.asregs.exception = SIGSEGV;
      return 0;
    }
  else
    {
      if ((x & 3) != 0)
	{
	  if (issue_messages)
	    fprintf (stderr, "word read from unaligned address: 0x%x\n", x);
      
	  cpu.asregs.exception = SIGBUS;
	  return 0;
	}
      {
	unsigned char * p = cpu.asregs.memory + x;
	return (EXTRACT_WORD(p));
      }
    }
}

#define CHECK_FLAG(T,H) if (tflags & T) { hflags |= H; tflags ^= T; }

unsigned int 
convert_target_flags (unsigned int tflags)
{
  unsigned int hflags = 0x0;

  CHECK_FLAG(0x0001, O_WRONLY);
  CHECK_FLAG(0x0002, O_RDWR);
  CHECK_FLAG(0x0008, O_APPEND);
  CHECK_FLAG(0x0200, O_CREAT);
  CHECK_FLAG(0x0400, O_TRUNC);
  CHECK_FLAG(0x0800, O_EXCL);
  CHECK_FLAG(0x2000, O_SYNC);

  if (tflags != 0x0)
    fprintf (stderr, 
	     "Simulator Error: problem converting target open flags for host.  0x%x\n", 
	     tflags);

  return hflags;
}

#define TRACE(str) if (tracing) fprintf(tracefile,"0x%08x, %s, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", opc, str, cpu.asregs.regs[0], cpu.asregs.regs[1], cpu.asregs.regs[2], cpu.asregs.regs[3], cpu.asregs.regs[4], cpu.asregs.regs[5], cpu.asregs.regs[6], cpu.asregs.regs[7], cpu.asregs.regs[8], cpu.asregs.regs[9], cpu.asregs.regs[10], cpu.asregs.regs[11], cpu.asregs.regs[12], cpu.asregs.regs[13], cpu.asregs.regs[14], cpu.asregs.regs[15]);

static int tracing = 0;

void
sim_resume (sd, step, siggnal)
     SIM_DESC sd;
     int step, siggnal;
{
  word pc, opc;
  unsigned long long insts;
  unsigned short inst;
  void (* sigsave)();

  sigsave = signal (SIGINT, interrupt);
  cpu.asregs.exception = step ? SIGTRAP: 0;
  pc = cpu.asregs.regs[PC_REGNO];
  insts = cpu.asregs.insts;
  unsigned char *memory = cpu.asregs.memory;

  /* Run instructions here. */
  do 
    {
      opc = pc;

      /* Fetch the instruction at pc.  */
      inst = (memory[pc] << 8) + memory[pc + 1];

      /* Decode instruction.  */
      if (inst & (1 << 15))
	{
	  if (inst & (1 << 14))
	    {
	      /* This is a Form 3 instruction.  */
	      /* We haven't implemented any yet, so just SIGILL for now.  */
	      TRACE("SIGILL3");
	      cpu.asregs.exception = SIGILL;
	      break;
	    }
	  else
	    {
	      /* This is a Form 2 instruction.  */
	      int opcode = (inst >> 12 & 0x3);
	      switch (opcode)
		{
		case 0x00: /* inc */
		  {
		    int a = (inst >> 8) & 0xf;
		    unsigned av = cpu.asregs.regs[a];
		    unsigned v = (inst & 0xff);
		    TRACE("inc");
		    cpu.asregs.regs[a] = av + v;
		  }
		  break;
		case 0x01: /* dec */
		  {
		    int a = (inst >> 8) & 0xf;
		    unsigned av = cpu.asregs.regs[a];
		    unsigned v = (inst & 0xff);
		    TRACE("dec");
		    cpu.asregs.regs[a] = av - v;
		  }
		  break;
		case 0x02: /* gsr */
		  {
		    int a = (inst >> 8) & 0xf;
		    unsigned v = (inst & 0xff);
		    TRACE("gsr");
		    cpu.asregs.regs[a] = cpu.asregs.sregs[v];
		  }
		  break;
		case 0x03: /* ssr */
		  {
		    int a = (inst >> 8) & 0xf;
		    unsigned v = (inst & 0xff);
		    TRACE("ssr");
		    cpu.asregs.sregs[v] = cpu.asregs.regs[a];
		  }
		  break;
		default:
		  TRACE("SIGILL2");
		  cpu.asregs.exception = SIGILL;
		  break;
		}
	    }
	}
      else
	{
	  /* This is a Form 1 instruction.  */
	  int opcode = inst >> 8;
	  switch (opcode)
	    {
	    case 0x00: /* nop */
	      break;
	    case 0x01: /* ldi.l (immediate) */
	      {
		int reg = (inst >> 4) & 0xf;
		TRACE("ldi.l");
		unsigned int val = EXTRACT_WORD(&(memory[pc + 2]));
		cpu.asregs.regs[reg] = val;
		pc += 4;
	      }
	      break;
	    case 0x02: /* mov (register-to-register) */
	      {
		int dest  = (inst >> 4) & 0xf;
		int src = (inst ) & 0xf;
		TRACE("mov");
		cpu.asregs.regs[dest] = cpu.asregs.regs[src];
	      }
	      break;
 	    case 0x03: /* jsra */
 	      {
 		unsigned int fn = EXTRACT_WORD(&(memory[pc + 2]));
 		unsigned int sp = cpu.asregs.regs[1];
		TRACE("jsra");
 		/* Save a slot for the static chain.  */
		sp -= 4;

 		/* Push the return address.  */
		sp -= 4;
 		wlat (opc, sp, pc + 6);
 		
 		/* Push the current frame pointer.  */
 		sp -= 4;
 		wlat (opc, sp, cpu.asregs.regs[0]);
 
 		/* Uncache the stack pointer and set the pc and $fp.  */
		cpu.asregs.regs[1] = sp;
		cpu.asregs.regs[0] = sp;
 		pc = fn - 2;
 	      }
 	      break;
 	    case 0x04: /* ret */
 	      {
 		unsigned int sp = cpu.asregs.regs[0];

		TRACE("ret");
 
 		/* Pop the frame pointer.  */
 		cpu.asregs.regs[0] = rlat (opc, sp);
 		sp += 4;
 		
 		/* Pop the return address.  */
 		pc = rlat (opc, sp) - 2;
 		sp += 4;

		/* Skip over the static chain slot.  */
		sp += 4;
 
 		/* Uncache the stack pointer.  */
 		cpu.asregs.regs[1] = sp;
  	      }
  	      break;
	    case 0x05: /* add.l */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		unsigned av = cpu.asregs.regs[a];
		unsigned bv = cpu.asregs.regs[b];
		TRACE("add.l");
		cpu.asregs.regs[a] = av + bv;
	      }
	      break;
	    case 0x06: /* push */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		int sp = cpu.asregs.regs[a] - 4;
		TRACE("push");
		wlat (opc, sp, cpu.asregs.regs[b]);
		cpu.asregs.regs[a] = sp;
	      }
	      break;
	    case 0x07: /* pop */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		int sp = cpu.asregs.regs[a];
		TRACE("pop");
		cpu.asregs.regs[b] = rlat (opc, sp);
		cpu.asregs.regs[a] = sp + 4;
	      }
	      break;
	    case 0x08: /* lda.l */
	      {
		int reg = (inst >> 4) & 0xf;
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		TRACE("lda.l");
		cpu.asregs.regs[reg] = rlat (opc, addr);
		pc += 4;
	      }
	      break;
	    case 0x09: /* sta.l */
	      {
		int reg = (inst >> 4) & 0xf;
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		TRACE("sta.l");
		wlat (opc, addr, cpu.asregs.regs[reg]);
		pc += 4;
	      }
	      break;
	    case 0x0a: /* ld.l (register indirect) */
	      {
		int src  = inst & 0xf;
		int dest = (inst >> 4) & 0xf;
		int xv;
		TRACE("ld.l");
		xv = cpu.asregs.regs[src];
		cpu.asregs.regs[dest] = rlat (opc, xv);
	      }
	      break;
	    case 0x0b: /* st.l */
	      {
		int dest = (inst >> 4) & 0xf;
		int val  = inst & 0xf;
		TRACE("st.l");
		wlat (opc, cpu.asregs.regs[dest], cpu.asregs.regs[val]);
	      }
	      break;
	    case 0x0c: /* ldo.l */
	      {
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		TRACE("ldo.l");
		addr += cpu.asregs.regs[b];
		cpu.asregs.regs[a] = rlat(opc, addr);
		pc += 4;
	      }
	      break;
	    case 0x0d: /* sto.l */
	      {
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		TRACE("sto.l");
		addr += cpu.asregs.regs[a];
		wlat(opc, addr, cpu.asregs.regs[b]);
		pc += 4;
	      }
	      break;
	    case 0x0e: /* cmp */
	      {
		int a  = (inst >> 4) & 0xf;
		int b  = inst & 0xf;
		int cc = 0;
		int va = cpu.asregs.regs[a];
		int vb = cpu.asregs.regs[b]; 

		TRACE("cmp");

		if (va == vb)
		  cc = CC_EQ;
		else
		  {
		    cc |= (va < vb ? CC_LT : 0);
		    cc |= (va > vb ? CC_GT : 0);
		    cc |= ((unsigned int) va < (unsigned int) vb ? CC_LTU : 0);
		    cc |= ((unsigned int) va > (unsigned int) vb ? CC_GTU : 0);
		  }

		cpu.asregs.cc = cc;
	      }
	      break;
	    case 0x0f: /* beq */
	      {
		unsigned int tgt = EXTRACT_WORD(&memory[pc+2]);
		TRACE("beq");
		if (cpu.asregs.cc & CC_EQ)
		  {
		    pc = tgt - 2;
		  }
		else
		  pc += 4;
	      }
	      break;
	    case 0x10: /* bne */
	      {
		unsigned int tgt = EXTRACT_WORD(&memory[pc+2]);
		TRACE("bne");
		if (! (cpu.asregs.cc & CC_EQ))
		  {
		    pc = tgt - 2;
		  }
		else
		  pc += 4;
	      }
	      break;
	    case 0x11: /* blt */
	      {
		unsigned int tgt = EXTRACT_WORD(&memory[pc+2]);
		TRACE("blt");
		if (cpu.asregs.cc & CC_LT)
		  {
		    pc = tgt - 2;
		  }
		else
		  pc += 4;
	      }
	      break;
	    case 0x12: /* bgt */
	      {
		unsigned int tgt = EXTRACT_WORD(&memory[pc+2]);
		TRACE("bgt");
		if (cpu.asregs.cc & CC_GT)
		  {
		    pc = tgt - 2;
		  }
		else
		  pc += 4;
	      }
	      break;
	    case 0x13: /* bltu */
	      {
		unsigned int tgt = EXTRACT_WORD(&memory[pc+2]);
		TRACE("bltu");
		if (cpu.asregs.cc & CC_LTU)
		  {
		    pc = tgt - 2;
		  }
		else
		  pc += 4;
	      }
	      break;
	    case 0x14: /* bgtu */
	      {
		unsigned int tgt = EXTRACT_WORD(&memory[pc+2]);
		TRACE("bgtu");
		if (cpu.asregs.cc & CC_GTU)
		  {
		    pc = tgt - 2;
		  }
		else
		  pc += 4;
	      }
	      break;
	    case 0x15: /* bge */
	      {
		unsigned int tgt = EXTRACT_WORD(&memory[pc+2]);
		TRACE("bge");
		if ((cpu.asregs.cc & CC_GT) || (cpu.asregs.cc & CC_EQ))		   
		  {
		    pc = tgt - 2;
		  }
		else
		  pc += 4;
	      }
	      break;
	    case 0x16: /* ble */
	      {
		unsigned int tgt = EXTRACT_WORD(&memory[pc+2]);
		TRACE("ble");
		if ((cpu.asregs.cc & CC_LT) || (cpu.asregs.cc & CC_EQ))
		  {
		    pc = tgt - 2;
		  }
		else
		  pc += 4;
	      }
	      break;
	    case 0x17: /* bgeu */
	      {
		unsigned int tgt = EXTRACT_WORD(&memory[pc+2]);
		TRACE("bgeu");
		if ((cpu.asregs.cc & CC_GTU) || (cpu.asregs.cc & CC_EQ))
		  {
		    pc = tgt - 2;
		  }
		else
		  pc += 4;
	      }
	      break;
	    case 0x18: /* bleu */
	      {
		unsigned int tgt = EXTRACT_WORD(&memory[pc+2]);
		TRACE("bleu");
		if ((cpu.asregs.cc & CC_LTU) || (cpu.asregs.cc & CC_EQ))
		  {
		    pc = tgt - 2;
		  }
		else
		  pc += 4;
	      }
	      break;
	    case 0x19: /* jsr */
	      {
		unsigned int fn = cpu.asregs.regs[(inst >> 4) & 0xf];
		unsigned int sp = cpu.asregs.regs[1];

		TRACE("jsr");

 		/* Save a slot for the static chain.  */
		sp -= 4;

		/* Push the return address.  */
		sp -= 4;
		wlat (opc, sp, pc + 2);
		
		/* Push the current frame pointer.  */
		sp -= 4;
		wlat (opc, sp, cpu.asregs.regs[0]);

		/* Uncache the stack pointer and set the fp & pc.  */
		cpu.asregs.regs[1] = sp;
		cpu.asregs.regs[0] = sp;
		pc = fn - 2;
	      }
	      break;
	    case 0x1a: /* jmpa */
	      {
		unsigned int tgt = EXTRACT_WORD(&memory[pc+2]);
		TRACE("jmpa");
		pc = tgt - 2;
	      }
	      break;
	    case 0x1b: /* ldi.b (immediate) */
	      {
		int reg = (inst >> 4) & 0xf;

		unsigned int val = EXTRACT_WORD(&(memory[pc + 2]));
		TRACE("ldi.b");
		cpu.asregs.regs[reg] = val;
		pc += 4;
	      }
	      break;
	    case 0x1c: /* ld.b (register indirect) */
	      {
		int src  = inst & 0xf;
		int dest = (inst >> 4) & 0xf;
		int xv;
		TRACE("ld.b");
		xv = cpu.asregs.regs[src];
		cpu.asregs.regs[dest] = rbat (opc, xv);
	      }
	      break;
	    case 0x1d: /* lda.b */
	      {
		int reg = (inst >> 4) & 0xf;
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		TRACE("lda.b");
		cpu.asregs.regs[reg] = rbat (opc, addr);
		pc += 4;
	      }
	      break;
	    case 0x1e: /* st.b */
	      {
		int dest = (inst >> 4) & 0xf;
		int val  = inst & 0xf;
		TRACE("st.b");
		wbat (opc, cpu.asregs.regs[dest], cpu.asregs.regs[val]);
	      }
	      break;
	    case 0x1f: /* sta.b */
	      {
		int reg = (inst >> 4) & 0xf;
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		TRACE("sta.b");
		wbat (opc, addr, cpu.asregs.regs[reg]);
		pc += 4;
	      }
	      break;
	    case 0x20: /* ldi.s (immediate) */
	      {
		int reg = (inst >> 4) & 0xf;

		unsigned int val = EXTRACT_WORD(&(memory[pc + 2]));
		TRACE("ldi.s");
		cpu.asregs.regs[reg] = val;
		pc += 4;
	      }
	      break;
	    case 0x21: /* ld.s (register indirect) */
	      {
		int src  = inst & 0xf;
		int dest = (inst >> 4) & 0xf;
		int xv;
		TRACE("ld.s");
		xv = cpu.asregs.regs[src];
		cpu.asregs.regs[dest] = rsat (opc, xv);
	      }
	      break;
	    case 0x22: /* lda.s */
	      {
		int reg = (inst >> 4) & 0xf;
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		TRACE("lda.s");
		cpu.asregs.regs[reg] = rsat (opc, addr);
		pc += 4;
	      }
	      break;
	    case 0x23: /* st.s */
	      {
		int dest = (inst >> 4) & 0xf;
		int val  = inst & 0xf;
		TRACE("st.s");
		wsat (opc, cpu.asregs.regs[dest], cpu.asregs.regs[val]);
	      }
	      break;
	    case 0x24: /* sta.s */
	      {
		int reg = (inst >> 4) & 0xf;
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		TRACE("sta.s");
		wsat (opc, addr, cpu.asregs.regs[reg]);
		pc += 4;
	      }
	      break;
	    case 0x25: /* jmp */
	      {
		int reg = (inst >> 4) & 0xf;
		TRACE("jmp");
		pc = cpu.asregs.regs[reg] - 2;
	      }
	      break;
	    case 0x26: /* and */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		int av, bv;
		TRACE("and");
		av = cpu.asregs.regs[a];
		bv = cpu.asregs.regs[b];
		cpu.asregs.regs[a] = av & bv;
	      }
	      break;
	    case 0x27: /* lshr */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		int av = cpu.asregs.regs[a];
		int bv = cpu.asregs.regs[b];
		TRACE("lshr");
		cpu.asregs.regs[a] = (unsigned) ((unsigned) av >> bv);
	      }
	      break;
	    case 0x28: /* ashl */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		int av = cpu.asregs.regs[a];
		int bv = cpu.asregs.regs[b];
		TRACE("ashl");
		cpu.asregs.regs[a] = av << bv;
	      }
	      break;
	    case 0x29: /* sub.l */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		unsigned av = cpu.asregs.regs[a];
		unsigned bv = cpu.asregs.regs[b];
		TRACE("sub.l");
		cpu.asregs.regs[a] = av - bv;
	      }
	      break;
	    case 0x2a: /* neg */
	      {
		int a  = (inst >> 4) & 0xf;
		int b  = inst & 0xf;
		int bv = cpu.asregs.regs[b];
		TRACE("neg");
		cpu.asregs.regs[a] = - bv;
	      }
	      break;
	    case 0x2b: /* or */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		int av, bv;
		TRACE("or");
		av = cpu.asregs.regs[a];
		bv = cpu.asregs.regs[b];
		cpu.asregs.regs[a] = av | bv;
	      }
	      break;
	    case 0x2c: /* not */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		int bv = cpu.asregs.regs[b];
		TRACE("not");
		cpu.asregs.regs[a] = 0xffffffff ^ bv;
	      }
	      break;
	    case 0x2d: /* ashr */
	      {
		int a  = (inst >> 4) & 0xf;
		int b  = inst & 0xf;
		int av = cpu.asregs.regs[a];
		int bv = cpu.asregs.regs[b];
		TRACE("ashr");
		cpu.asregs.regs[a] = av >> bv;
	      }
	      break;
	    case 0x2e: /* xor */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		int av, bv;
		TRACE("xor");
		av = cpu.asregs.regs[a];
		bv = cpu.asregs.regs[b];
		cpu.asregs.regs[a] = av ^ bv;
	      }
	      break;
	    case 0x2f: /* mul.l */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		unsigned av = cpu.asregs.regs[a];
		unsigned bv = cpu.asregs.regs[b];
		TRACE("mul.l");
		cpu.asregs.regs[a] = av * bv;
	      }
	      break;
	    case 0x30: /* swi */
	      {
		unsigned int inum = EXTRACT_WORD(&memory[pc+2]);
		TRACE("swi");
		switch (inum)
		  {
		  case 0x1: /* SYS_exit */
		    {
		      cpu.asregs.exception = SIGQUIT;
		      break;
		    }
		  case 0x2: /* SYS_open */
		    {
		      char *fname = &memory[cpu.asregs.regs[2]];
		      int mode = (int) convert_target_flags ((unsigned) cpu.asregs.regs[3]);
		      /* Permission bits are at 0x12($fp) */
		      int perm = (int) EXTRACT_WORD(&memory[cpu.asregs.regs[0] + 20]);
		      int fd = open (fname, mode, perm);
#if 0
		      fprintf(stderr, "open(\"%s\", 0x%x, 0x%x) = %d\n", fname, mode, perm, fd);
#endif
		      /* FIXME - set errno */
		      cpu.asregs.regs[2] = fd;
		      break;
		    }
		  case 0x4: /* SYS_read */
		    {
		      int fd = cpu.asregs.regs[2];
		      char *buf = &memory[cpu.asregs.regs[3]];
		      /* String length is at 0x12($fp) */
		      unsigned len = EXTRACT_WORD(&memory[cpu.asregs.regs[0] + 20]);
		      cpu.asregs.regs[2] = read (fd, buf, len);
		      break;
		    }
		  case 0x5: /* SYS_write */
		    {
		      char *str = &memory[cpu.asregs.regs[3]];
		      /* String length is at 0x12($fp) */
		      unsigned count, len = EXTRACT_WORD(&memory[cpu.asregs.regs[0] + 20]);
		      count = write (cpu.asregs.regs[2], str, len);
		      cpu.asregs.regs[2] = count;
		      break;
		    }
		  default:
		    break;
		  }
		pc += 4;
	      }
	      break;
	    case 0x31: /* div.l */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		int av = cpu.asregs.regs[a];
		int bv = cpu.asregs.regs[b];
		TRACE("div.l");
		cpu.asregs.regs[a] = av / bv;
	      }
	      break;
	    case 0x32: /* udiv.l */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		unsigned int av = cpu.asregs.regs[a];
		unsigned int bv = cpu.asregs.regs[b];
		TRACE("udiv.l");
		cpu.asregs.regs[a] = (av / bv);
	      }
	      break;
	    case 0x33: /* mod.l */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		int av = cpu.asregs.regs[a];
		int bv = cpu.asregs.regs[b];
		TRACE("mod.l");
		cpu.asregs.regs[a] = av % bv;
	      }
	      break;
	    case 0x34: /* umod.l */
	      {
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		unsigned int av = cpu.asregs.regs[a];
		unsigned int bv = cpu.asregs.regs[b];
		TRACE("umod.l");
		cpu.asregs.regs[a] = (av % bv);
	      }
	      break;
	    case 0x35: /* brk */
	      TRACE("brk");
	      cpu.asregs.exception = SIGTRAP;
	      pc -= 2; /* Adjust pc */
	      break;
	    case 0x36: /* ldo.b */
	      {
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		TRACE("ldo.b");
		addr += cpu.asregs.regs[b];
		cpu.asregs.regs[a] = rbat(opc, addr);
		pc += 4;
	      }
	      break;
	    case 0x37: /* sto.b */
	      {
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		TRACE("sto.b");
		addr += cpu.asregs.regs[a];
		wbat(opc, addr, cpu.asregs.regs[b]);
		pc += 4;
	      }
	      break;
	    case 0x38: /* ldo.s */
	      {
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		TRACE("ldo.s");
		addr += cpu.asregs.regs[b];
		cpu.asregs.regs[a] = rsat(opc, addr);
		pc += 4;
	      }
	      break;
	    case 0x39: /* sto.s */
	      {
		unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
		int a = (inst >> 4) & 0xf;
		int b = inst & 0xf;
		TRACE("sto.s");
		addr += cpu.asregs.regs[a];
		wsat(opc, addr, cpu.asregs.regs[b]);
		pc += 4;
	      }
	      break;
	    default:
	      opc = opcode;
	      TRACE("SIGILL1");
	      cpu.asregs.exception = SIGILL;
	      break;
	    }
	}

      insts++;
      pc += 2;

    } while (!cpu.asregs.exception);

  /* Hide away the things we've cached while executing.  */
  cpu.asregs.regs[PC_REGNO] = pc;
  cpu.asregs.insts += insts;		/* instructions done ... */

  signal (SIGINT, sigsave);
}

int
sim_write (sd, addr, buffer, size)
     SIM_DESC sd;
     SIM_ADDR addr;
     unsigned char * buffer;
     int size;
{
  int i;
  init_pointers ();
  
  memcpy (& cpu.asregs.memory[addr], buffer, size);
  
  return size;
}

int
sim_read (sd, addr, buffer, size)
     SIM_DESC sd;
     SIM_ADDR addr;
     unsigned char * buffer;
     int size;
{
  int i;
  init_pointers ();
  
  memcpy (buffer, & cpu.asregs.memory[addr], size);
  
  return size;
}


int
sim_store_register (sd, rn, memory, length)
     SIM_DESC sd;
     int rn;
     unsigned char * memory;
     int length;
{
  init_pointers ();

  if (rn < NUM_MOXIE_REGS && rn >= 0)
    {
      if (length == 4)
	{
	  long ival;
	  
	  /* misalignment safe */
	  ival = moxie_extract_unsigned_integer (memory, 4);
	  cpu.asints[rn] = ival;
	}

      return 4;
    }
  else
    return 0;
}

int
sim_fetch_register (sd, rn, memory, length)
     SIM_DESC sd;
     int rn;
     unsigned char * memory;
     int length;
{
  init_pointers ();
  
  if (rn < NUM_MOXIE_REGS && rn >= 0)
    {
      if (length == 4)
	{
	  long ival = cpu.asints[rn];

	  /* misalignment-safe */
	  moxie_store_unsigned_integer (memory, 4, ival);
	}
      
      return 4;
    }
  else
    return 0;
}


int
sim_trace (sd)
     SIM_DESC sd;
{
  if (tracefile == 0)
    tracefile = fopen("trace.csv", "wb");

  tracing = 1;
  
  sim_resume (sd, 0, 0);

  tracing = 0;
  
  return 1;
}

void
sim_stop_reason (sd, reason, sigrc)
     SIM_DESC sd;
     enum sim_stop * reason;
     int * sigrc;
{
  if (cpu.asregs.exception == SIGQUIT)
    {
      * reason = sim_exited;
      * sigrc = cpu.asregs.regs[2];
    }
  else
    {
      * reason = sim_stopped;
      * sigrc = cpu.asregs.exception;
    }
}


int
sim_stop (sd)
     SIM_DESC sd;
{
  cpu.asregs.exception = SIGINT;
  return 1;
}


void
sim_info (sd, verbose)
     SIM_DESC sd;
     int verbose;
{
  callback->printf_filtered (callback, "\n\n# instructions executed  %llu\n",
			     cpu.asregs.insts);
}


SIM_DESC
sim_open (kind, cb, abfd, argv)
     SIM_OPEN_KIND kind;
     host_callback * cb;
     struct bfd * abfd;
     char ** argv;
{
  int osize = sim_memory_size;
  myname = argv[0];
  callback = cb;
  
  if (kind == SIM_OPEN_STANDALONE)
    issue_messages = 1;
  
  /* Discard and reacquire memory -- start with a clean slate.  */
  sim_size (1);		/* small */
  sim_size (osize);	/* and back again */

  set_initial_gprs ();	/* Reset the GPR registers.  */
  
  /* Fudge our descriptor for now.  */
  return (SIM_DESC) 1;
}

void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  /* nothing to do */
}

SIM_RC
sim_load (sd, prog, abfd, from_tty)
     SIM_DESC sd;
     char * prog;
     bfd * abfd;
     int from_tty;
{

  /* Do the right thing for ELF executables; this turns out to be
     just about the right thing for any object format that:
       - we crack using BFD routines
       - follows the traditional UNIX text/data/bss layout
       - calls the bss section ".bss".   */

  extern bfd * sim_load_file (); /* ??? Don't know where this should live.  */
  bfd * prog_bfd;

  {
    bfd * handle;
    handle = bfd_openr (prog, 0);	/* could be "moxie" */
    
    if (!handle)
      {
	printf("``%s'' could not be opened.\n", prog);
	return SIM_RC_FAIL;
      }
    
    /* Makes sure that we have an object file, also cleans gets the 
       section headers in place.  */
    if (!bfd_check_format (handle, bfd_object))
      {
	/* wasn't an object file */
	bfd_close (handle);
	printf ("``%s'' is not appropriate object file.\n", prog);
	return SIM_RC_FAIL;
      }

    /* Clean up after ourselves.  */
    bfd_close (handle);
  }

  /* from sh -- dac */
  prog_bfd = sim_load_file (sd, myname, callback, prog, abfd,
                            sim_kind == SIM_OPEN_DEBUG,
                            0, sim_write);
  if (prog_bfd == NULL)
    return SIM_RC_FAIL;
  
  if (abfd == NULL)
    bfd_close (prog_bfd);

  return SIM_RC_OK;
}

SIM_RC
sim_create_inferior (sd, prog_bfd, argv, env)
     SIM_DESC sd;
     struct bfd * prog_bfd;
     char ** argv;
     char ** env;
{
  char ** avp;
  int l, argc, i, tp;

  /* Set the initial register set.  */
  l = issue_messages;
  issue_messages = 0;
  set_initial_gprs ();
  issue_messages = l;
  
  cpu.asregs.regs[PC_REGNO] = bfd_get_start_address (prog_bfd);

  /* Copy args into target memory.  */
  avp = argv;
  for (argc = 0; *avp; avp++)
    argc++;

  /* Target memory looks like this:
     0x00000000 zero word
     0x00000004 argc word
     0x00000008 start of argv
     .
     0x0000???? end of argv
     0x0000???? zero word 
     0x0000???? start of data pointed to by argv  */

  wlat (0, 0, 0);
  wlat (0, 4, argc);

  /* tp is the offset of our first argv data.  */
  tp = 4 + 4 + argc * 4 + 4;

  for (i = 0; i < argc; i++)
    {
      /* Set the argv value.  */
      wlat (0, 4 + 4 + i * 4, tp);

      /* Store the string.  */
      strcpy (&cpu.asregs.memory[tp], argv[i]);
      tp += strlen (argv[i]) + 1;
    }

  wlat (0, 4 + 4 + i * 4, 0);

  return SIM_RC_OK;
}

void
sim_kill (sd)
     SIM_DESC sd;
{
  if (tracefile)
    fclose(tracefile);
}

void
sim_do_command (sd, cmd)
     SIM_DESC sd;
     char * cmd;
{
  /* Nothing there yet; it's all an error.  */
  
  if (cmd != NULL)
    {
      char ** simargv = buildargv (cmd);
      if (strcmp (simargv[0], "verbose") == 0)
	{
	  issue_messages = 2;
	}
      else
	{
	  fprintf (stderr,"Error: \"%s\" is not a valid moxie simulator command.\n",
		   cmd);
	}
    }
  else
    {
      fprintf (stderr, "moxie sim commands: \n");
      fprintf (stderr, "  verbose\n");
    }
}

void
sim_set_callbacks (ptr)
     host_callback * ptr;
{
  callback = ptr; 
}
