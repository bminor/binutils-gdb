#include <signal.h>
#include "sysdep.h"
#include "bfd.h"
#include "remote-sim.h"

#include "d10v_sim.h"

#define IMEM_SIZE 18	/* D10V instruction memory size is 18 bits */
#define DMEM_SIZE 16	/* Data memory */

enum _leftright { LEFT_FIRST, RIGHT_FIRST };

int d10v_debug;
host_callback *d10v_callback;
long ins_type_counters[ (int)INS_MAX ];
long left_nops, right_nops;

uint16 OP[4];

static struct hash_entry *lookup_hash PARAMS ((uint32 ins, int size));

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
hash(insn, format)
     long insn;
     int format;
{
  if (format & LONG_OPCODE)
    return ((insn & 0x3F000000) >> 24);
  else
    return((insn & 0x7E00) >> 9);
}

static struct hash_entry *
lookup_hash (ins, size)
     uint32 ins;
     int size;
{
  struct hash_entry *h;

  if (size)
    h = &hash_table[(ins & 0x3F000000) >> 24];
  else
    h = &hash_table[(ins & 0x7E00) >> 9];

  while ( (ins & h->mask) != h->opcode)
    {
      if (h->next == NULL)
	{
	  (*d10v_callback->printf_filtered) (d10v_callback, "ERROR looking up hash for %x at PC %x\n",ins, PC);
	  exit(1);
	}
      h = h->next;
    }
  return (h);
}

uint32
get_longword (x)
      uint8 *x;
{
  uint8 *a = x;
  return (a[0]<<24) + (a[1]<<16) + (a[2]<<8) + (a[3]);
}

int64
get_longlong (x)
      uint8 *x;
{
  uint8 *a = x;
  return ((int64)a[0]<<56) + ((int64)a[1]<<48) + ((int64)a[2]<<40) + ((int64)a[3]<<32) +
    ((int64)a[4]<< 24) + ((int64)a[5]<<16) + ((int64)a[6]<<8) + (int64)a[7];
}

uint16
get_word (x)
      uint8 *x;
{
  uint8 *a = x;
  return ((uint16)a[0]<<8) + a[1];
}


void
write_word (addr, data)
     uint8 *addr;
     uint16 data;
{
  uint8 *a = addr;
  a[0] = data >> 8;
  a[1] = data & 0xff;
}

void
write_longword (addr, data)
     uint8 *addr;
     uint32 data;
{
  addr[0] = (data >> 24) & 0xff;
  addr[1] = (data >> 16) & 0xff;
  addr[2] = (data >> 8) & 0xff;
  addr[3] = data & 0xff;
}

void
write_longlong (addr, data)
     uint8 *addr;
     int64 data;
{
  uint8 *a = addr;
  a[0] = data >> 56;
  a[1] = (data >> 48) & 0xff;
  a[2] = (data >> 40) & 0xff;
  a[3] = (data >> 32) & 0xff;
  a[4] = (data >> 24) & 0xff;
  a[5] = (data >> 16) & 0xff;
  a[6] = (data >> 8) & 0xff;
  a[7] = data & 0xff;
}

static void
get_operands (struct simops *s, uint32 ins)
{
  int i, shift, bits, flags;
  uint32 mask;
  for (i=0; i < s->numops; i++)
    {
      shift = s->operands[3*i];
      bits = s->operands[3*i+1];
      flags = s->operands[3*i+2];
      mask = 0x7FFFFFFF >> (31 - bits);
      OP[i] = (ins >> shift) & mask;
    }
}

static void
do_long (ins)
     uint32 ins;
{
  struct hash_entry *h;
#ifdef DEBUG
  if ((d10v_debug & DEBUG_INSTRUCTION) != 0)
    (*d10v_callback->printf_filtered) (d10v_callback, "do_long 0x%x\n", ins);
#endif
  h = lookup_hash (ins, 1);
  get_operands (h->ops, ins);
  State.ins_type = INS_LONG;
  ins_type_counters[ (int)State.ins_type ]++;
  (h->ops->func)();
}
static void
do_2_short (ins1, ins2, leftright)
     uint16 ins1, ins2;
     enum _leftright leftright;
{
  struct hash_entry *h;
#ifdef DEBUG
  if ((d10v_debug & DEBUG_INSTRUCTION) != 0)
    (*d10v_callback->printf_filtered) (d10v_callback, "do_2_short 0x%x (%s) -> 0x%x\n",
				       ins1, (leftright) ? "left" : "right", ins2);
#endif
  /*  printf ("do_2_short %x -> %x\n",ins1,ins2); */
  h = lookup_hash (ins1, 0);
  get_operands (h->ops, ins1);
  State.ins_type = (leftright == LEFT_FIRST) ? INS_LEFT : INS_RIGHT;
  ins_type_counters[ (int)State.ins_type ]++;
  (h->ops->func)();
  h = lookup_hash (ins2, 0);
  get_operands (h->ops, ins2);
  State.ins_type = (leftright == LEFT_FIRST) ? INS_RIGHT : INS_LEFT;
  ins_type_counters[ (int)State.ins_type ]++;
  (h->ops->func)();
}
static void
do_parallel (ins1, ins2)
     uint16 ins1, ins2;
{
  struct hash_entry *h1, *h2;
#ifdef DEBUG
  if ((d10v_debug & DEBUG_INSTRUCTION) != 0)
    (*d10v_callback->printf_filtered) (d10v_callback, "do_parallel 0x%x || 0x%x\n", ins1, ins2);
#endif
  h1 = lookup_hash (ins1, 0);
  h2 = lookup_hash (ins2, 0);

  if (h1->ops->exec_type == PARONLY)
    {
      get_operands (h1->ops, ins1);
      State.ins_type = INS_LEFT;
      ins_type_counters[ (int)State.ins_type ]++;
      (h1->ops->func)();
      if (State.exe)
	{
	  get_operands (h2->ops, ins2);
	  State.ins_type = INS_RIGHT;
	  (h2->ops->func)();
	}
    }
  else if (h2->ops->exec_type == PARONLY)
    {
      get_operands (h2->ops, ins2);
      State.ins_type = INS_RIGHT;
      ins_type_counters[ (int)State.ins_type ]++;
      (h2->ops->func)();
      if (State.exe)
	{
	  get_operands (h1->ops, ins1);
	  State.ins_type = INS_LEFT;
	  (h1->ops->func)();
	}
    }
  else
    {
      get_operands (h1->ops, ins1);
      State.ins_type = INS_LEFT_PARALLEL;
      ins_type_counters[ (int)State.ins_type ]++;
      (h1->ops->func)();
      get_operands (h2->ops, ins2);
      State.ins_type = INS_RIGHT_PARALLEL;
      ins_type_counters[ (int)State.ins_type ]++;
      (h2->ops->func)();
    }
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
      (*d10v_callback->printf_filtered) (d10v_callback, "Memory allocation failed.\n");
      exit(1);
    }

#ifdef DEBUG
  if ((d10v_debug & DEBUG_MEMSIZE) != 0)
    {
      (*d10v_callback->printf_filtered) (d10v_callback, "Allocated %d bytes instruction memory and\n",1<<IMEM_SIZE);
      (*d10v_callback->printf_filtered) (d10v_callback, "          %d bytes data memory.\n",          1<<DMEM_SIZE);
    }
#endif
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

  /* (*d10v_callback->printf_filtered) (d10v_callback, "sim_write %d bytes to 0x%x\n",size,addr); */
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
  static int init_p = 0;

  if (args != NULL)
    {
#ifdef DEBUG
      if (strcmp (args, "-t") == 0)
	d10v_debug = DEBUG;
      else
#endif
	(*d10v_callback->printf_filtered) (d10v_callback, "ERROR: unsupported option(s): %s\n",args);
    }

  /* put all the opcodes in the hash table */
  if (!init_p++)
    {
      for (s = Simops; s->func; s++)
	{
	  h = &hash_table[hash(s->opcode,s->format)];
      
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
  (*d10v_callback->printf_filtered) (d10v_callback, "sim_set_profile %d\n",n);
}

void
sim_set_profile_size (n)
     int n;
{
  (*d10v_callback->printf_filtered) (d10v_callback, "sim_set_profile_size %d\n",n);
}

void
sim_resume (step, siggnal)
     int step, siggnal;
{
  uint32 inst;
  int i;
  reg_t oldpc;

/*   (*d10v_callback->printf_filtered) (d10v_callback, "sim_resume (%d,%d)  PC=0x%x\n",step,siggnal,PC); */

 if (step)
   State.exception = SIGTRAP;
 else
   State.exception = 0;
 
 do
   {
     inst = RLW (PC << 2); 
     oldpc = PC;
     switch (inst & 0xC0000000)
       {
       case 0xC0000000:
	 /* long instruction */
	 do_long (inst & 0x3FFFFFFF);
	 break;
       case 0x80000000:
	 /* R -> L */
	 do_2_short ( inst & 0x7FFF, (inst & 0x3FFF8000) >> 15, 0);
	 break;
       case 0x40000000:
	 /* L -> R */
	 do_2_short ((inst & 0x3FFF8000) >> 15, inst & 0x7FFF, 1);
	 break;
       case 0:
	 do_parallel ((inst & 0x3FFF8000) >> 15, inst & 0x7FFF);
	 break;
       }
     
     if (State.RP && PC == RPT_E)
       {
	 RPT_C -= 1;
	 if (RPT_C == 0)
	   State.RP = 0;
	 else
	   PC = RPT_S;
       }
     
     /* FIXME */
     if (PC == oldpc)
       PC++;
   } 
 while (!State.exception);
}

int
sim_trace ()
{
#ifdef DEBUG
  d10v_debug = DEBUG;
#endif
  sim_resume (0, 0);
  return 1;
}

void
sim_info (verbose)
     int verbose;
{
  char buf[40];
  int size;
  long total = (ins_type_counters[ (int)INS_LONG ]
		+ ins_type_counters[ (int)INS_LEFT ]
		+ ins_type_counters[ (int)INS_LEFT_PARALLEL ]
		+ ins_type_counters[ (int)INS_RIGHT ]
		+ ins_type_counters[ (int)INS_RIGHT_PARALLEL ]);

  sprintf (buf, "%ld", total);
  size = strlen (buf);

  (*d10v_callback->printf_filtered) (d10v_callback,
				     "executed %*ld instructions in the left  container, %*ld parallel, %*ld nops\n",
				     size, ins_type_counters[ (int)INS_LEFT ] + ins_type_counters[ (int)INS_LEFT_PARALLEL ],
				     size, ins_type_counters[ (int)INS_LEFT_PARALLEL ],
				     size, left_nops);

  (*d10v_callback->printf_filtered) (d10v_callback,
				     "executed %*ld instructions in the right container, %*ld parallel, %*ld nops\n",
				     size, ins_type_counters[ (int)INS_RIGHT ] + ins_type_counters[ (int)INS_RIGHT_PARALLEL ],
				     size, ins_type_counters[ (int)INS_RIGHT_PARALLEL ],
				     size, right_nops);

  (*d10v_callback->printf_filtered) (d10v_callback,
				     "executed %*ld long instructions\n",
				     size, ins_type_counters[ (int)INS_LONG ]);

  (*d10v_callback->printf_filtered) (d10v_callback,
				     "executed %*ld total instructions\n",
				     size, total);
}

void
sim_create_inferior (start_address, argv, env)
     SIM_ADDR start_address;
     char **argv;
     char **env;
{
#ifdef DEBUG
  if (d10v_debug)
    (*d10v_callback->printf_filtered) (d10v_callback, "sim_create_inferior:  PC=0x%x\n", start_address);
#endif
  PC = start_address >> 2;
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
/*  printf ("sim_set_callbacks\n"); */
  d10v_callback = p;
}

void
sim_stop_reason (reason, sigrc)
     enum sim_stop *reason;
     int *sigrc;
{
/*   (*d10v_callback->printf_filtered) (d10v_callback, "sim_stop_reason:  PC=0x%x\n",PC<<2); */

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
  if (rn > 31)
    {
      WRITE_64 (memory, State.a[rn-32]);
      /* (*d10v_callback->printf_filtered) (d10v_callback, "sim_fetch_register %d 0x%llx\n",rn,State.a[rn-32]); */
    }
  else
    {
      WRITE_16 (memory, State.regs[rn]);
      /* (*d10v_callback->printf_filtered) (d10v_callback, "sim_fetch_register %d 0x%x\n",rn,State.regs[rn]); */
    }
}
 
void
sim_store_register (rn, memory)
     int rn;
     unsigned char *memory;
{
  if (rn > 31)
    {
      State.a[rn-32] =  READ_64 (memory) & MASK40;
      /* (*d10v_callback->printf_filtered) (d10v_callback, "store: a%d=0x%llx\n",rn-32,State.a[rn-32]); */
    }
  else
    {
      State.regs[rn]= READ_16 (memory);
      /* (*d10v_callback->printf_filtered) (d10v_callback, "store: r%d=0x%x\n",rn,State.regs[rn]); */
    }
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
  (*d10v_callback->printf_filtered) (d10v_callback, "sim_do_command: %s\n",cmd);
}

int
sim_load (prog, from_tty)
     char *prog;
     int from_tty;
{
  /* Return nonzero so GDB will handle it.  */
  return 1;
} 
