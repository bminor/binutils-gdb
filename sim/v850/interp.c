#include <signal.h>
#include "sysdep.h"
#include "bfd.h"
#include "remote-sim.h"
#include "callback.h"

#include "v850_sim.h"

#define IMEM_SIZE 18	/* V850 instruction memory size is 18 bits */
#define DMEM_SIZE 16	/* Data memory */

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
  /* XXX This isn't right for 32bit opcodes, hell it isn't even
     right for 16bit opcodes!  */
  if (insn & 0x0600)
    return ((insn & 0x3F000000) >> 24);
  else
    return((insn & 0x07E0) >> 5);
}

static struct hash_entry *
lookup_hash (ins)
     uint32 ins;
{
  struct hash_entry *h;

  if (ins & 0x0f00)
    h = &hash_table[(ins & 0x3F000000) >> 24];
  else
    h = &hash_table[(ins & 0x07E0) >> 5];

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

uint32
get_longword_swap (x)
      uint16 x;
{
  uint8 *a = (uint8 *)(x + State.imem);
  return (a[0]<<24) + (a[1]<<16) + (a[2]<<8) + (a[3]);
}

uint16
get_word_swap (x)
      uint16 x;
{
  uint8 *a = (uint8 *)(x + State.imem);
  return (a[0]<<8) + a[1];
}

void
write_word_swap (addr, data)
     uint16 addr, data;
{
  uint8 *a = (uint8 *)(addr + State.imem);
  a[0] = data >> 8;
  a[1] = data & 0xff;
}

static void
do_format_1 (insn)
     uint32 insn;
{
  printf("format 1 0x%x\n", insn >> 16);
}

static void
do_format_2 (insn)
     uint32 insn;
{
  printf("format 2 0x%x\n", insn >> 16);
}

static void
do_format_3 (insn)
     uint32 insn;
{
  printf("format 3 0x%x\n", insn >> 16);
}

static void
do_format_4 (insn)
     uint32 insn;
{
  printf("format 4 0x%x\n", insn >> 16);
}

static void
do_format_5 (insn)
     uint32 insn;
{
  printf("format 5 0x%x\n", insn);
}

static void
do_format_6 (insn)
     uint32 insn;
{
  printf("format 6 0x%x\n", insn);
}

static void
do_format_7 (insn)
     uint32 insn;
{
  printf("format 7 0x%x\n", insn);
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
  printf("formats 9 and 10 0x%x\n", insn);
}

void
sim_size (power)
     int power;

{
  if (State.imem)
    {
      free (State.imem);
      free (State.dmem);
    }

  State.imem = (uint8 *)calloc(1,1<<IMEM_SIZE);
  State.dmem = (uint8 *)calloc(1,1<<DMEM_SIZE);
  if (!State.imem || !State.dmem )
    {
      fprintf (stderr,"Memory allocation failed.\n");
      exit(1);
    }
  printf ("Allocated %d bytes instruction memory and\n",1<<IMEM_SIZE);
  printf ("          %d bytes data memory.\n",1<<DMEM_SIZE);
}

static void
init_system ()
{
  if (!State.imem)
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
      State.imem[i+addr] = buffer[i]; 
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
     opcode = (inst & 0x07e00000) >> 21;
     if ((opcode & 0x30) == 0)
       {
	 do_format_1 (inst >> 16);
	 PC += 2;
       }
     else if ((opcode & 0x38) == 0x10)
       {
	 do_format_2 (inst >> 16);
	 PC += 2;
       }
     else if ((opcode & 0x3C) == 0x18
	      || (opcode & 0x3C) == 0x1C
	      || (opcode & 0x3C) == 0x20
	      || (opcode & 0x3C) == 0x24
	      || (opcode & 0x3C) == 0x28)
       {
	 do_format_4 (inst >> 16);
	 PC += 2;
       }
     else if ((opcode& 0x3C) == 0x23)
       {
	 do_format_3 (inst >> 16);
	 PC += 2;
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
	 PC += 4;
       }
     else if ((opcode & 0x3E) == 0x3C)
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
      buffer[i] = State.imem[addr + i];
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
