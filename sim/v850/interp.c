#include <signal.h>
#include "sysdep.h"
#include "bfd.h"
#include "remote-sim.h"
#include "callback.h"

#include "v850_sim.h"

#define MEM_SIZE 18	/* V850 memory size is 18 bits XXX */

uint16 OP[4];

static struct hash_entry *lookup_hash PARAMS ((uint32 ins));

#define MAX_HASH  63
struct hash_entry
{
  struct hash_entry *next;
  long opcode;
  long mask;
  struct simops *ops;
};

struct hash_entry hash_table[MAX_HASH+1];

static long 
hash(insn)
     long insn;
{
  if ((insn & 0x30) == 0
      || (insn & 0x38) == 0x10)
    return (insn & 0x07e0) >> 5;
  if ((insn & 0x3c) == 0x18
      || (insn & 0x3c) == 0x1c
      || (insn & 0x3c) == 0x20
      || (insn & 0x3c) == 0x24
      || (insn & 0x3c) == 0x28
      || (insn & 0x3c) == 0x23)
    return (insn & 0x07c0) >> 6;
  if ((insn & 0x38) == 0x30)
    return (insn & 0x07e0) >> 5;
  /* What about sub-op field? XXX */
  if ((insn & 0x38) == 0x38)
    return (insn & 0x07e0) >> 5;
  if ((insn & 0x3e) == 0x3c)
    return (insn & 0x07c0) >> 6;
  if ((insn & 0x3f) == 0x3e)
    return (insn & 0xc7e0) >> 5;
  /* Not really correct.  XXX */
  return insn & 0xffffffff;
  
}

static struct hash_entry *
lookup_hash (ins)
     uint32 ins;
{
  struct hash_entry *h;

  h = &hash_table[hash(ins)];

  while ( (ins & h->mask) != h->opcode)
    {
      if (h->next == NULL)
	{
	  printf ("ERROR looking up hash for %x\n",ins);
	  exit(1);
	}
      h = h->next;
    }
  return (h);
}

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

static void
do_format_1_2 (insn)
     uint32 insn;
{
  struct hash_entry *h;
  printf("format 1 or 2 0x%x\n", insn);

  h = lookup_hash (insn);
  OP[0] = insn & 0x1f;
  OP[1] = (insn >> 11) & 0x1f;
  (h->ops->func) ();
}

static void
do_format_3 (insn)
     uint32 insn;
{
  struct hash_entry *h;
  printf("format 3 0x%x\n", insn);

  h = lookup_hash (insn);
  OP[0] = (((insn & 0x70) >> 4) | ((insn & 0xf800) >> 8)) << 1;
  (h->ops->func) ();
}

static void
do_format_4 (insn)
     uint32 insn;
{
  printf("format 4 0x%x\n", insn);
}

static void
do_format_5 (insn)
     uint32 insn;
{
  struct hash_entry *h;
  printf("format 5 0x%x\n", insn);

  h = lookup_hash (insn);
  OP[0] = ((insn & 0x3f) | (((insn >> 17) & 0x7fff) << 6)) << 1;
  OP[1] = (insn >> 11) & 0x1f;
  (h->ops->func) ();
}

static void
do_format_6 (insn)
     uint32 insn;
{
  struct hash_entry *h;
  printf("format 6 0x%x\n", insn);

  h = lookup_hash (insn);
  OP[0] = (insn >> 16) & 0xffff;
  OP[1] = insn & 0x1f;
  OP[2] = (insn >> 11) & 0x1f;
  (h->ops->func) ();
}

static void
do_format_7 (insn)
     uint32 insn;
{
  struct hash_entry *h;
  printf("format 7 0x%x\n", insn);

  h = lookup_hash (insn);
  OP[0] = insn & 0x1f;
  OP[1] = (insn >> 11) & 0x1f;
  OP[2] = (insn >> 16) & 0xffff;
  (h->ops->func) ();
}

static void
do_format_8 (insn)
     uint32 insn;
{
  printf("format 8 0x%x\n", insn);
}

static void
do_formats_9_10 (insn)
     uint32 insn;
{
  struct hash_entry *h;
  printf("formats 9 and 10 0x%x\n", insn);

  h = lookup_hash (insn);
  OP[0] = insn & 0x1f;
  OP[1] = (insn >> 11) & 0x1f;
  (h->ops->func) ();
}

void
sim_size (power)
     int power;

{
  if (State.mem)
    {
      free (State.mem);
    }

  State.mem = (uint8 *)calloc(1,1<<MEM_SIZE);
  if (!State.mem)
    {
      fprintf (stderr,"Memory allocation failed.\n");
      exit(1);
    }
  printf ("Allocated %d bytes memory and\n",1<<MEM_SIZE);
}

static void
init_system ()
{
  if (!State.mem)
    sim_size(1);
}

int
sim_write (addr, buffer, size)
     SIM_ADDR addr;
     unsigned char *buffer;
     int size;
{
  int i;
  init_system ();

  /* printf ("sim_write %d bytes to 0x%x\n",size,addr); */
  for (i = 0; i < size; i++)
    {
      State.mem[i+addr] = buffer[i]; 
    }
  return size;
}

void
sim_open (args)
     char *args;
{
  struct simops *s;
  struct hash_entry *h, *prev;
  if (args != NULL)
      printf ("sim_open %s\n",args);

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
  printf ("sim_set_profile %d\n",n);
}

void
sim_set_profile_size (n)
     int n;
{
  printf ("sim_set_profile_size %d\n",n);
}

void
sim_resume (step, siggnal)
     int step, siggnal;
{
  uint32 inst, opcode;
  int i;
  reg_t oldpc;

/*   printf ("sim_resume (%d,%d)  PC=0x%x\n",step,siggnal,PC); */

 if (step)
   State.exception = SIGTRAP;
 else
   State.exception = 0;
 
 do
   {
     inst = RLW (PC);
     oldpc = PC;
     opcode = (inst & 0x07e0) >> 5;
     if ((opcode & 0x30) == 0
	 || (opcode & 0x38) == 0x10)
       {
	 do_format_1_2 (inst & 0xffff);
	 PC += 2;
       }
     else if ((opcode & 0x3C) == 0x18
	      || (opcode & 0x3C) == 0x1C
	      || (opcode & 0x3C) == 0x20
	      || (opcode & 0x3C) == 0x24
	      || (opcode & 0x3C) == 0x28)
       {
	 do_format_4 (inst & 0xffff);
	 PC += 2;
       }
     else if ((opcode & 0x3C) == 0x23)
       {
	 do_format_3 (inst & 0xffff);
	 /* No PC update, it's done in the instruction.  */
       }
     else if ((opcode & 0x38) == 0x30)
       {
	 do_format_6 (inst);
	 PC += 4;
       }
     else if ((opcode & 0x3C) == 0x38)
       {
	 do_format_7 (inst);
	 PC += 4;
       }
     else if ((opcode & 0x3E) == 0x3C)
       {
	 do_format_5 (inst);
	 /* No PC update, it's done in the instruction.  */
       }
     else if ((opcode & 0x3F) == 0x3E)
       {
	 do_format_8 (inst);
	 PC += 4;
       }
     else
       {
	 do_formats_9_10 (inst);
	 PC += 4;
       }
   } 
 while (!State.exception);
}

int
sim_trace ()
{
  printf ("sim_trace\n");
  return 0;
}

void
sim_info (verbose)
     int verbose;
{
  printf ("sim_info\n");
}

void
sim_create_inferior (start_address, argv, env)
     SIM_ADDR start_address;
     char **argv;
     char **env;
{
  printf ("sim_create_inferior:  PC=0x%x\n",start_address);
  PC = start_address;
}


void
sim_kill ()
{
  /* nothing to do */
}

void
sim_set_callbacks(p)
     host_callback *p;
{
  printf ("sim_set_callbacks\n");
  /* callback = p; */
}

void
sim_stop_reason (reason, sigrc)
     enum sim_stop *reason;
     int *sigrc;
{
/*   printf ("sim_stop_reason:  PC=0x%x\n",PC); */

  if (State.exception == SIGQUIT)
    {
      *reason = sim_exited;
      *sigrc = State.exception;
    }
  else
    {
      *reason = sim_stopped;
      *sigrc = State.exception;
    } 
}

void
sim_fetch_register (rn, memory)
     int rn;
     unsigned char *memory;
{
  *(uint32 *)memory = State.regs[rn];
  /* printf ("sim_fetch_register %d 0x%x\n",rn,State.regs[rn]); */
}
 
void
sim_store_register (rn, memory)
     int rn;
     unsigned char *memory;
{
  State.regs[rn]= *(uint32 *)memory;
  /* printf ("store: r%d=0x%x\n",rn,State.regs[rn]); */
}

sim_read (addr, buffer, size)
     SIM_ADDR addr;
     unsigned char *buffer;
     int size;
{
  int i;
  for (i = 0; i < size; i++)
    {
      buffer[i] = State.mem[addr + i];
    }
  return size;
} 

void
sim_do_command (cmd)
     char *cmd;
{ 
  printf("sim_do_command: %s\n",cmd);
}

int
sim_load (prog, from_tty)
     char *prog;
     int from_tty;
{
  /* Return nonzero so GDB will handle it.  */
  return 1;
} 
