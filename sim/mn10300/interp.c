#include <signal.h>
#include "sysdep.h"
#include "bfd.h"

#include "mn10300_sim.h"

#ifndef INLINE
#ifdef __GNUC__
#define INLINE inline
#else
#define INLINE
#endif
#endif

host_callback *mn10300_callback;
int mn10300_debug;

uint32 OP[4];

static struct hash_entry *lookup_hash PARAMS ((uint32 ins, int));
static long hash PARAMS ((long));
static void init_system PARAMS ((void));
#define MAX_HASH  63

struct hash_entry
{
  struct hash_entry *next;
  long opcode;
  long mask;
  struct simops *ops;
};

struct hash_entry hash_table[MAX_HASH+1];


/* This probably doesn't do a very good job at bucket filling, but
   it's simple... */
static INLINE long 
hash(insn)
     long insn;
{
  /* These are one byte insns.  */
  if ((insn & 0xffffff00) == 0)
    {
      if ((insn & 0xf0) == 0x00
	  || (insn & 0xf0) == 0x40)
	return (insn & 0xf3) & 0x3f;

      if ((insn & 0xf0) == 0x10
	  || (insn & 0xf0) == 0x30
	  || (insn & 0xf0) == 0x50)
	return (insn & 0xfc) & 0x3f;

      if ((insn & 0xf0) == 0x60
	  || (insn & 0xf0) == 0x70
	  || (insn & 0xf0) == 0x80
	  || (insn & 0xf0) == 0x90
	  || (insn & 0xf0) == 0xa0
	  || (insn & 0xf0) == 0xb0
	  || (insn & 0xf0) == 0xe0)
	return (insn & 0xf0) & 0x3f;

      return (insn & 0xff) & 0x3f;
    }

  /* These are two byte insns */
  if ((insn & 0xffff0000) == 0)
    {
      if ((insn & 0xf000) == 0x2000
	  || (insn & 0xf000) == 0x5000)
	return ((insn & 0xfc00) >> 8) & 0x3f;

      if ((insn & 0xf000) == 0x4000)
	return ((insn & 0xf300) >> 8) & 0x3f;

      if ((insn & 0xf000) == 0x8000
	  || (insn & 0xf000) == 0x9000
	  || (insn & 0xf000) == 0xa000
	  || (insn & 0xf000) == 0xb000)
	return ((insn & 0xf000) >> 8) & 0x3f;

      return ((insn & 0xff00) >> 8) & 0x3f;
    }

  /* These are three byte insns.  */
  if ((insn & 0xff000000) == 0)
    {
      if ((insn & 0xf00000) == 0x000000)
	return ((insn & 0xf30000) >> 16) & 0x3f;

      if ((insn & 0xf00000) == 0x200000
	  || (insn & 0xf00000) == 0x300000)
	return ((insn & 0xfc0000) >> 16) & 0x3f;

        return ((insn & 0xff0000) >> 16) & 0x3f;
    }

  /* These are four byte or larger insns.  */
  return ((insn & 0xff000000) >> 24) & 0x3f;
}

static struct hash_entry *
lookup_hash (ins, length)
     uint32 ins;
     int length;
{
  struct hash_entry *h;

  h = &hash_table[hash(ins)];

  while ((ins & h->mask) != h->opcode
	 || (length != h->ops->length))
    {
      if (h->next == NULL)
	{
	  (*mn10300_callback->printf_filtered) (mn10300_callback, "ERROR looking up hash for 0x%x, PC=0x%x\n", ins, PC);
	  exit(1);
	}
      h = h->next;
    }
  return (h);
}

/* FIXME These would more efficient to use than load_mem/store_mem,
   but need to be changed to use the memory map.  */

uint8
get_byte (x)
     uint8 *x;
{
  return *x;
}

uint16
get_half (x)
     uint8 *x;
{
  uint8 *a = x;
  return (a[1] << 8) + (a[0]);
}

uint32
get_word (x)
      uint8 *x;
{
  uint8 *a = x;
  return (a[3]<<24) + (a[2]<<16) + (a[1]<<8) + (a[0]);
}

void
put_byte (addr, data)
     uint8 *addr;
     uint8 data;
{
  uint8 *a = addr;
  a[0] = data;
}

void
put_half (addr, data)
     uint8 *addr;
     uint16 data;
{
  uint8 *a = addr;
  a[0] = data & 0xff;
  a[1] = (data >> 8) & 0xff;
}

void
put_word (addr, data)
     uint8 *addr;
     uint32 data;
{
  uint8 *a = addr;
  a[0] = data & 0xff;
  a[1] = (data >> 8) & 0xff;
  a[2] = (data >> 16) & 0xff;
  a[3] = (data >> 24) & 0xff;
}


uint32
load_mem_big (addr, len)
     SIM_ADDR addr;
     int len;
{
  uint8 *p = addr + State.mem;

  switch (len)
    {
    case 1:
      return p[0];
    case 2:
      return p[0] << 8 | p[1];
    case 3:
      return p[0] << 16 | p[1] << 8 | p[2];
    case 4:
      return p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
    default:
      abort ();
    }
}

uint32
load_mem (addr, len)
     SIM_ADDR addr;
     int len;
{
  uint8 *p = addr + State.mem;

  switch (len)
    {
    case 1:
      return p[0];
    case 2:
      return p[1] << 8 | p[0];
    case 3:
      return p[2] << 16 | p[1] << 8 | p[0];
    case 4:
      return p[3] << 24 | p[2] << 16 | p[1] << 8 | p[0];
    default:
      abort ();
    }
}

void
store_mem (addr, len, data)
     SIM_ADDR addr;
     int len;
     uint32 data;
{
  uint8 *p = addr + State.mem;

  switch (len)
    {
    case 1:
      p[0] = data;
      return;
    case 2:
      p[0] = data;
      p[1] = data >> 8;
      return;
    case 4:
      p[0] = data;
      p[1] = data >> 8;
      p[2] = data >> 16;
      p[3] = data >> 24;
      return;
    default:
      abort ();
    }
}

void
sim_size (power)
     int power;

{
  if (State.mem)
    free (State.mem);

  State.mem = (uint8 *) calloc (1,  1 << power);
  if (!State.mem)
    {
      (*mn10300_callback->printf_filtered) (mn10300_callback, "Allocation of main memory failed.\n");
      exit (1);
    }
}

static void
init_system ()
{
  if (!State.mem)
    sim_size(18);
}

int
sim_write (addr, buffer, size)
     SIM_ADDR addr;
     unsigned char *buffer;
     int size;
{
  int i;

  init_system ();

  for (i = 0; i < size; i++)
    store_mem (addr + i, 1, buffer[i]);

  return size;
}

void
sim_open (args)
     char *args;
{
  struct simops *s;
  struct hash_entry *h;
  if (args != NULL)
    {
#ifdef DEBUG
      if (strcmp (args, "-t") == 0)
	mn10300_debug = DEBUG;
      else
#endif
	(*mn10300_callback->printf_filtered) (mn10300_callback, "ERROR: unsupported option(s): %s\n",args);
    }

  /* put all the opcodes in the hash table */
  for (s = Simops; s->func; s++)
    {
      h = &hash_table[hash(s->opcode)];
      
      /* go to the last entry in the chain */
      while (h->next)
	  h = h->next;

      if (h->ops)
	{
	  h->next = calloc(1,sizeof(struct hash_entry));
	  h = h->next;
	}
      h->ops = s;
      h->mask = s->mask;
      h->opcode = s->opcode;
    }
}


void
sim_close (quitting)
     int quitting;
{
  /* nothing to do */
}

void
sim_set_profile (n)
     int n;
{
  (*mn10300_callback->printf_filtered) (mn10300_callback, "sim_set_profile %d\n", n);
}

void
sim_set_profile_size (n)
     int n;
{
  (*mn10300_callback->printf_filtered) (mn10300_callback, "sim_set_profile_size %d\n", n);
}

void
sim_resume (step, siggnal)
     int step, siggnal;
{
  uint32 inst, opcode;
  reg_t oldpc;
  struct hash_entry *h;

  if (step)
    State.exception = SIGTRAP;
  else
    State.exception = 0;

  do
    {
      unsigned long insn, extension;

      /* Fetch the current instruction.  */
      inst = load_mem_big (PC, 1);
      oldpc = PC;

      /* These are one byte insns.  */
      if ((inst & 0xf3) == 0x00
	  || (inst & 0xf0) == 0x10
	  || (inst & 0xfc) == 0x3c
	  || (inst & 0xf3) == 0x41
	  || (inst & 0xf3) == 0x40
	  || (inst & 0xfc) == 0x50
	  || (inst & 0xfc) == 0x54
	  || (inst & 0xf0) == 0x60
	  || (inst & 0xf0) == 0x70
	  || ((inst & 0xf0) == 0x80
	      && (inst & 0x0c) >> 2 != (inst & 0x03))
	  || ((inst & 0xf0) == 0x90
	      && (inst & 0x0c) >> 2 != (inst & 0x03))
	  || ((inst & 0xf0) == 0xa0
	      && (inst & 0x0c) >> 2 != (inst & 0x03))
	  || ((inst & 0xf0) == 0xb0
	      && (inst & 0x0c) >> 2 != (inst & 0x03))
	  || (inst & 0xff) == 0xcb
	  || (inst & 0xfc) == 0xd0
	  || (inst & 0xfc) == 0xd4
	  || (inst & 0xfc) == 0xd8
	  || (inst & 0xf0) == 0xe0)
	{
	  insn = inst;
	  h = lookup_hash (insn, 1);
	  extension = 0;
	  (h->ops->func)(insn, extension);
	  PC += 1;
	}

      /* These are two byte insns.  */
      else if ((inst & 0xf0) == 0x80
	       || (inst & 0xf0) == 0x90
	       || (inst & 0xf0) == 0xa0
	       || (inst & 0xf0) == 0xb0
	       || (inst & 0xfc) == 0x20
	       || (inst & 0xfc) == 0x28
	       || (inst & 0xf3) == 0x43
	       || (inst & 0xf3) == 0x42
	       || (inst & 0xfc) == 0x58
	       || (inst & 0xfc) == 0x5c
	       || ((inst & 0xf0) == 0xc0
		   && (inst & 0xff) != 0xcb
		   && (inst & 0xff) != 0xcc
		   && (inst & 0xff) != 0xcd)
	       || (inst & 0xff) == 0xf0
	       || (inst & 0xff) == 0xf1
	       || (inst & 0xff) == 0xf2
	       || (inst & 0xff) == 0xf3
	       || (inst & 0xff) == 0xf4
	       || (inst & 0xff) == 0xf5
	       || (inst & 0xff) == 0xf6)
	{
	  insn = load_mem_big (PC, 2);
	  h = lookup_hash (insn, 2);
	  extension = 0;
	  (h->ops->func)(insn, extension);
	  PC += 2;
	}

      /* These are three byte insns.  */
      else if ((inst & 0xff) == 0xf8
	       || (inst & 0xff) == 0xcc 
	       || (inst & 0xff) == 0xf9
	       || (inst & 0xf3) == 0x01
	       || (inst & 0xf3) == 0x02
	       || (inst & 0xf3) == 0x03
	       || (inst & 0xfc) == 0x24
	       || (inst & 0xfc) == 0x2c
	       || (inst & 0xfc) == 0x30
	       || (inst & 0xfc) == 0x34
	       || (inst & 0xfc) == 0x38
	       || (inst & 0xff) == 0xde
	       || (inst & 0xff) == 0xdf
	       || (inst & 0xff) == 0xcc)
	{
	  insn = load_mem_big (PC, 3);
	  h = lookup_hash (insn, 3);
	  extension = 0;
	  (h->ops->func)(insn, extension);
	  PC += 3;
	}

      /* These are four byte insns.  */
      else if ((inst & 0xff) == 0xfa
	       || (inst & 0xff) == 0xfb)
	{
	  insn = load_mem_big (PC, 4);
	  h = lookup_hash (insn, 4);
	  extension = 0;
	  (h->ops->func)();
	  PC += 4;
	}

      /* These are five byte insns.  */
      else if ((inst & 0xff) == 0xcd
	       || (inst & 0xff) == 0xdc)
	{
	  insn = load_mem_big (PC, 4);
	  h = lookup_hash (insn, 5);
	  extension = load_mem_big (PC + 4, 1);
	  (h->ops->func)(insn, extension);
	  PC += 5;
	}

      /* These are six byte insns.  */
      else if ((inst & 0xff) == 0xfd
	       || (inst & 0xff) == 0xfc)
	{
	  insn = load_mem_big (PC, 4);
	  h = lookup_hash (insn, 6);
	  extension = load_mem_big (PC + 4, 2);
	  (h->ops->func)(insn, extension);
	  PC += 6;
	}

      /* Else its a seven byte insns (in theory).  */
      else
	{
	  insn = load_mem_big (PC, 4);
	  h = lookup_hash (insn, 7);
	  extension = load_mem_big (PC + 4, 3);
	  (h->ops->func)(insn, extension);
	  PC += 7;
	}
    }
  while (!State.exception);
}

int
sim_trace ()
{
#ifdef DEBUG
  mn10300_debug = DEBUG;
#endif
  sim_resume (0, 0);
  return 1;
}

void
sim_info (verbose)
     int verbose;
{
  (*mn10300_callback->printf_filtered) (mn10300_callback, "sim_info\n");
}

void
sim_create_inferior (start_address, argv, env)
     SIM_ADDR start_address;
     char **argv;
     char **env;
{
  PC = start_address;
}

void
sim_kill ()
{
  /* nothing to do */
}

void
sim_set_callbacks (p)
     host_callback *p;
{
  mn10300_callback = p;
}

/* All the code for exiting, signals, etc needs to be revamped.

   This is enough to get c-torture limping though.  */

void
sim_stop_reason (reason, sigrc)
     enum sim_stop *reason;
     int *sigrc;
{
  *reason = sim_stopped;
  if (State.exception == SIGQUIT)
    *sigrc = 0;
  else
    *sigrc = State.exception;
}

void
sim_fetch_register (rn, memory)
     int rn;
     unsigned char *memory;
{
  put_word (memory, State.regs[rn]);
}
 
void
sim_store_register (rn, memory)
     int rn;
     unsigned char *memory;
{
  State.regs[rn] = get_word (memory);
}

int
sim_read (addr, buffer, size)
     SIM_ADDR addr;
     unsigned char *buffer;
     int size;
{
  int i;
  for (i = 0; i < size; i++)
    buffer[i] = load_mem (addr + i, 1);

  return size;
} 

void
sim_do_command (cmd)
     char *cmd;
{
  (*mn10300_callback->printf_filtered) (mn10300_callback, "\"%s\" is not a valid mn10300 simulator command.\n", cmd);
}

int
sim_load (prog, from_tty)
     char *prog;
     int from_tty;
{
  /* Return nonzero so GDB will handle it.  */
  return 1;
} 
