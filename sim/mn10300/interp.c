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

static struct hash_entry *lookup_hash PARAMS ((uint32 ins));
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


static INLINE long 
hash(insn)
     long insn;
{
/* XXX */
}

static struct hash_entry *
lookup_hash (ins)
     uint32 ins;
{
  struct hash_entry *h;

  h = &hash_table[hash(ins)];

  while ((ins & h->mask) != h->opcode)
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
  struct interrupt_generator *intgen;

  if (step)
    State.exception = SIGTRAP;
  else
    State.exception = 0;

  do
    {
      /* Fetch the current instruction.  */
      inst = RLW (PC);
      oldpc = PC;
      opcode = (inst & 0x07e0) >> 5;

      /* Decode the opcode field. */
      if ((opcode & 0x30) == 0
	  || (opcode & 0x38) == 0x10)
	{
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
