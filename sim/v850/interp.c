#include <signal.h>
#include "sysdep.h"
#include "bfd.h"

#include "v850_sim.h"

enum interrupt_type
{
  int_none,
  int_reset,
  int_nmi,
  int_intov1,
  int_intp10,
  int_intp11,
  int_intp12,
  int_intp13,
  int_intcm4,
  num_int_types
};

enum interrupt_cond_type
{
  int_cond_none,
  int_cond_pc,
  int_cond_time
};

struct interrupt_generator
{
  enum interrupt_type type;
  enum interrupt_cond_type cond_type;
  int number;
  int address;
  int time;
  int enabled;
  struct interrupt_generator *next;
};

char *interrupt_names[] = {
  "",
  "reset",
  "nmi",
  "intov1",
  "intp10",
  "intp11",
  "intp12",
  "intp13",
  "intcm4",
  NULL
};

struct interrupt_generator *intgen_list;

/* True if a non-maskable (such as NMI or reset) interrupt generator
   is present.  */

static int have_nm_generator;

#ifndef INLINE
#ifdef __GNUC__
#define INLINE inline
#else
#define INLINE
#endif
#endif

/* These default values correspond to expected usage for the chip.  */

SIM_ADDR rom_size = 0x8000;
SIM_ADDR low_end = 0x200000;
SIM_ADDR high_start = 0xffe000;

SIM_ADDR high_base;

host_callback *v850_callback;

int v850_debug;

/* non-zero if we opened prog_bfd */
static int prog_bfd_was_opened_p;
bfd *prog_bfd;

static SIM_OPEN_KIND sim_kind;
static char *myname;

uint32 OP[4];

static struct hash_entry *lookup_hash PARAMS ((uint32 ins));
static long hash PARAMS ((long));
static void do_format_1_2 PARAMS ((uint32));
static void do_format_3 PARAMS ((uint32));
static void do_format_4 PARAMS ((uint32));
static void do_format_5 PARAMS ((uint32));
static void do_format_6 PARAMS ((uint32));
static void do_format_7 PARAMS ((uint32));
static void do_format_8 PARAMS ((uint32));
static void do_format_9_10 PARAMS ((uint32));
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
  if (   (insn & 0x0600) == 0
      || (insn & 0x0700) == 0x0200
      || (insn & 0x0700) == 0x0600
      || (insn & 0x0780) == 0x0700)
    return (insn & 0x07e0) >> 5;

  if ((insn & 0x0700) == 0x0300
      || (insn & 0x0700) == 0x0400
      || (insn & 0x0700) == 0x0500)
    return (insn & 0x0780) >> 7;

  if ((insn & 0x07c0) == 0x0780)
    return (insn & 0x07c0) >> 6;

  return (insn & 0x07e0) >> 5;
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
	  (*v850_callback->printf_filtered) (v850_callback, "ERROR looking up hash for 0x%x, PC=0x%x\n", ins, PC);
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

uint8 *
map (addr)
     SIM_ADDR addr;
{
  uint8 *p;

  /* Mask down to 24 bits. */
  addr &= 0xffffff;

  if (addr < low_end)
    {
      /* "Mirror" the addresses below 1MB. */
      if (addr < 0x100000)
	addr &= (rom_size - 1);
      else
	addr += (rom_size - 0x100000);
      return (uint8 *) (addr + State.mem);
    }
  else if (addr >= high_start)
    {
      /* If in the peripheral I/O region, mirror 1K region across 4K,
	 and similarly if in the internal RAM region.  */
      if (addr >= 0xfff000)
	addr &= 0xfff3ff;
      else if (addr >= 0xffe000)
	addr &= 0xffe3ff;
      return (uint8 *) (addr - high_start + high_base + State.mem);
    }
  else
    {
      fprintf (stderr, "segmentation fault: access address: %x not below %x or above %x [ep = %x]\n", addr, low_end, high_start, State.regs[30]);
      
      /* Signal a memory error. */
      State.exception = SIGSEGV;
      /* Point to a location not in main memory - renders invalid
	 addresses harmless until we get back to main insn loop. */
      return (uint8 *) &(State.dummy_mem);
    }
}

uint32
load_mem (addr, len)
     SIM_ADDR addr;
     int len;
{
  uint8 *p = map (addr);

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
  uint8 *p = map (addr);

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
  int totsize;

  if (State.mem)
    free (State.mem);

  totsize = rom_size + (low_end - 0x100000) + (0x1000000 - high_start);

  high_base = rom_size + (low_end - 0x100000);

  State.mem = (uint8 *) calloc (1, totsize);
  if (!State.mem)
    {
      (*v850_callback->printf_filtered) (v850_callback, "Allocation of main memory failed.\n");
      exit (1);
    }
}

void
sim_set_memory_map (spec)
     char *spec;
{
  char *reststr, *nreststr;
  SIM_ADDR new_low_end, new_high_start;

  new_low_end = low_end;
  new_high_start = high_start;
  if (! strncmp (spec, "hole=", 5))
    {
      new_low_end = sim_parse_number (spec + 5, &reststr);
      if (new_low_end < 0x100000)
	{
	  (*v850_callback->printf_filtered) (v850_callback,
					     "Low end must be at least 0x100000\n");
	  return;
	}
      if (*reststr == ',')
	{
	  ++reststr;
	  new_high_start = sim_parse_number (reststr, &nreststr);
	  /* FIXME Check high_start also */
	}
      (*v850_callback->printf_filtered) (v850_callback,
					 "Hole goes from 0x%x to 0x%x\n",
					 new_low_end, new_high_start);
    }
  else
    {
      (*v850_callback->printf_filtered) (v850_callback, "Invalid specification for memory map, must be `hole=<m>[,<n>]'\n");
    }

  if (new_low_end != low_end || new_high_start != high_start)
    {
      low_end = new_low_end;
      high_start = new_high_start;
      if (State.mem)
	{
	  (*v850_callback->printf_filtered) (v850_callback, "Reconfiguring memory (old contents will be lost)\n");
	  sim_size (1);
	}
    }
}

/* Parse a number in hex, octal, or decimal form.  */

int
sim_parse_number (str, rest)
     char *str, **rest;
{
  if (str[0] == '0' && str[1] == 'x')
    return strtol (str, rest, 16);
  else if (str[0] == '0')
    return strtol (str, rest, 16);
  else
    return strtol (str, rest, 10);
}

static void
init_system ()
{
  if (!State.mem)
    sim_size(1);
}

int
sim_write (sd, addr, buffer, size)
     SIM_DESC sd;
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

SIM_DESC
sim_open (kind, cb, abfd, argv)
     SIM_OPEN_KIND kind;
     host_callback *cb;
     struct _bfd *abfd;
     char **argv;
{
  struct simops *s;
  struct hash_entry *h;
  char **p;

  sim_kind = kind;
  myname = argv[0];
  v850_callback = cb;

  if (argv != NULL)
    {
      for (p = argv + 1; *p; ++p)
	{
#ifdef DEBUG
	  if (strcmp (*p, "-t") == 0)
	    v850_debug = DEBUG;
	  else
#endif
	    (*v850_callback->printf_filtered) (v850_callback, "ERROR: unsupported option(s): %s\n",*p);
	}
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
	  h->next = (struct hash_entry *) calloc(1,sizeof(struct hash_entry));
	  h = h->next;
	}
      h->ops = s;
      h->mask = s->mask;
      h->opcode = s->opcode;
    }

  /* fudge our descriptor for now */
  return (SIM_DESC) 1;
}


void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  if (prog_bfd != NULL && prog_bfd_was_opened_p)
    bfd_close (prog_bfd);
}

void
sim_set_profile (n)
     int n;
{
  (*v850_callback->printf_filtered) (v850_callback, "sim_set_profile %d\n", n);
}

void
sim_set_profile_size (n)
     int n;
{
  (*v850_callback->printf_filtered) (v850_callback, "sim_set_profile_size %d\n", n);
}

time_t start_time;

static void do_interrupt PARAMS ((enum interrupt_type));

int
sim_stop (sd)
     SIM_DESC sd;
{
  return 0;
}

void
sim_resume (sd, step, siggnal)
     SIM_DESC sd;
     int step, siggnal;
{
  uint32 inst, opcode;
  reg_t oldpc;
  struct interrupt_generator *intgen;
  time_t now;

  if (step)
    State.exception = SIGTRAP;
  else
    State.exception = 0;

  time (&start_time);

  do
    {
      struct hash_entry * h;
      /* Fetch the current instruction.  */
      inst  = RLW (PC);
      oldpc = PC;

      h     = lookup_hash (inst);
      OP[0] = inst & 0x1f;
      OP[1] = (inst >> 11) & 0x1f;
      OP[2] = (inst >> 16) & 0xffff;
      OP[3] = inst;

//      fprintf (stderr, "PC = %x, SP = %x\n", PC, SP );

      if (inst == 0)
	{
	  fprintf (stderr, "NOP encountered!\n");
	  break;
	}
      
      PC += h->ops->func ();

      if (oldpc == PC)
	{
	  fprintf (stderr, "simulator loop at %x\n", PC );
	  break;
	}
      
      /* Check for and handle pending interrupts.  */
      if (intgen_list && (have_nm_generator || !(PSW & PSW_ID)))
	{
	  intgen = NULL;
	  for (intgen = intgen_list; intgen != NULL; intgen = intgen->next)
	    {
	      if (intgen->cond_type == int_cond_pc
		  && oldpc == intgen->address
		  && intgen->enabled)
		{
		  break;
		}
	      else if (intgen->cond_type == int_cond_time
		       && intgen->enabled)
		{
		  time (&now);
		  if (((long) now - (long) start_time) > intgen->time)
		    {
		      intgen->enabled = 0;
		      break;
		    }
		}
	    }
	  if (intgen)
	    do_interrupt (intgen->type);
	}
      else if (State.pending_nmi)
	{
	  State.pending_nmi = 0;
	  do_interrupt (int_nmi);
	}
    }
  while (!State.exception);
}

static void
do_interrupt (inttype)
     enum interrupt_type inttype;
{
  /* Disable further interrupts.  */
  PSW |= PSW_ID;
  /* Indicate that we're doing interrupt not exception processing.  */
  PSW &= ~PSW_EP;
  if (inttype == int_reset)
    {
      PC = 0;
      PSW = 0x20;
      ECR = 0;
      /* (Might be useful to init other regs with random values.) */
    }
  else if (inttype == int_nmi)
    {
      if (PSW & PSW_NP)
	{
	  /* We're already working on an NMI, so this one must wait
	     around until the previous one is done.  The processor
	     ignores subsequent NMIs, so we don't need to count them.  */
	  State.pending_nmi = 1;
	}
      else
	{
	  FEPC = PC;
	  FEPSW = PSW;
	  /* Set the FECC part of the ECR. */
	  ECR &= 0x0000ffff;
	  ECR |= 0x10;
	  PSW |= PSW_NP;
	  PC = 0x10;
	}
    }
  else
    {
      EIPC = PC;
      EIPSW = PSW;
      /* Clear the EICC part of the ECR, will set below. */
      ECR &= 0xffff0000;
      switch (inttype)
	{
	case int_intov1:
	  PC = 0x80;
	  ECR |= 0x80;
	  break;
	case int_intp10:
	  PC = 0x90;
	  ECR |= 0x90;
	  break;
	case int_intp11:
	  PC = 0xa0;
	  ECR |= 0xa0;
	  break;
	case int_intp12:
	  PC = 0xb0;
	  ECR |= 0xb0;
	  break;
	case int_intp13:
	  PC = 0xc0;
	  ECR |= 0xc0;
	  break;
	case int_intcm4:
	  PC = 0xd0;
	  ECR |= 0xd0;
	  break;
	default:
	  /* Should never be possible.  */
	  abort ();
	  break;
	}
    }
}

int
sim_trace (sd)
     SIM_DESC sd;
{
#ifdef DEBUG
  v850_debug = DEBUG;
#endif
  sim_resume (sd, 0, 0);
  return 1;
}

void
sim_info (sd, verbose)
     SIM_DESC sd;
     int verbose;
{
  (*v850_callback->printf_filtered) (v850_callback, "sim_info\n");
}

SIM_RC
sim_create_inferior (sd, argv, env)
     SIM_DESC sd;
     char **argv;
     char **env;
{
  return SIM_RC_OK;
}

void
sim_set_callbacks (p)
     host_callback *p;
{
  v850_callback = p;
}

/* All the code for exiting, signals, etc needs to be revamped.

   This is enough to get c-torture limping though.  */

void
sim_stop_reason (sd, reason, sigrc)
     SIM_DESC sd;
     enum sim_stop *reason;
     int *sigrc;
{
  if (State.exception == SIG_V850_EXIT)
    {
      *reason = sim_exited;
      *sigrc = State.regs[7];
    }
  else
    {
      *reason = sim_stopped;
      *sigrc = State.exception;
    }
}

void
sim_fetch_register (sd, rn, memory)
     SIM_DESC sd;
     int rn;
     unsigned char *memory;
{
  put_word (memory, State.regs[rn]);
}
 
void
sim_store_register (sd, rn, memory)
     SIM_DESC sd;
     int rn;
     unsigned char *memory;
{
  State.regs[rn] = get_word (memory);
}

int
sim_read (sd, addr, buffer, size)
     SIM_DESC sd;
     SIM_ADDR addr;
     unsigned char *buffer;
     int size;
{
  int i;
  for (i = 0; i < size; i++)
    buffer[i] = load_mem (addr + i, 1);

  return size;
} 

int current_intgen_number = 1;

void
sim_set_interrupt (spec)
     char *spec;
{
  int i, num;
  char **argv;
  struct interrupt_generator *intgen, *tmpgen;
  extern char **buildargv ();

  argv = buildargv (spec);

  if (*argv && ! strcmp (*argv, "add"))
    {
      /* Create a new interrupt generator object.  */
      intgen = (struct interrupt_generator *)
	malloc (sizeof(struct interrupt_generator));
      intgen->type = int_none;
      intgen->cond_type = int_cond_none;
      intgen->address = 0;
      intgen->time = 0;
      intgen->enabled = 0;
      ++argv;
      /* Match on interrupt type name.  */
      for (i = 0; i < num_int_types; ++i)
	{
	  if (*argv && ! strcmp (*argv, interrupt_names[i]))
	    {
	      intgen->type = i;
	      break;
	    }
	}
      if (intgen->type == int_none)
	{
	  (*v850_callback->printf_filtered) (v850_callback, "Interrupt type unknown; known types are\n");
	  for (i = 0; i < num_int_types; ++i)
	    {
	      (*v850_callback->printf_filtered) (v850_callback, " %s", interrupt_names[i]);
	    }
	  (*v850_callback->printf_filtered) (v850_callback, "\n");
	  free (intgen);
	  return;
	}
      ++argv;
      intgen->address = 0;
      intgen->time = 0;
      if (*argv && ! strcmp (*argv, "pc"))
	{
	  intgen->cond_type = int_cond_pc;
	  ++argv;
	  intgen->address = sim_parse_number (*argv, NULL);
	}
      else if (*argv && ! strcmp (*argv, "time"))
	{
	  intgen->cond_type = int_cond_time;
	  ++argv;
	  intgen->time = sim_parse_number (*argv, NULL);
	}
      else
	{
	  (*v850_callback->printf_filtered) (v850_callback, "Condition type must be `pc' or `time'.\n");
	  free (intgen);
	  return;
	}
      /* We now have a valid interrupt generator.  Number it and add
	 to the list of generators.  */
      intgen->number = current_intgen_number++;
      intgen->enabled = 1;
      intgen->next = intgen_list;
      intgen_list = intgen;
      (*v850_callback->printf_filtered) (v850_callback, "Interrupt generator %d (NMI) at pc=0x%x, time=%d.\n", intgen_list->number, intgen_list->address, intgen_list->time);
    }
  else if (*argv && !strcmp (*argv, "remove"))
    {
      ++argv;
      num = sim_parse_number (*argv, NULL);
      tmpgen = NULL;
      if (intgen_list)
	{
	  if (intgen_list->number == num)
	    {
	      tmpgen = intgen_list;
	      intgen_list = intgen_list->next;
	    }
	  else
	    {
	      for (intgen = intgen_list; intgen != NULL; intgen = intgen->next)
		{
		  if (intgen->next != NULL && intgen->next->number == num)
		    {
		      tmpgen = intgen->next;
		      intgen->next = intgen->next->next;
		      break;
		    }
		}
	    }
	  if (tmpgen)
	    free (tmpgen);
	  else
	    (*v850_callback->printf_filtered) (v850_callback,
					       "No interrupt generator numbered %d, ignoring.\n", num);
	}
    }
  else if (*argv && !strcmp (*argv, "info"))
    {
      if (intgen_list)
	{
	  for (intgen = intgen_list; intgen != NULL; intgen = intgen->next)
	    (*v850_callback->printf_filtered) (v850_callback,
					       "Interrupt generator %d (%s) at pc=0x%x/time=%d%s.\n",
					       intgen->number,
					       interrupt_names[intgen->type],
					       intgen->address,
					       intgen->time,
					       (intgen->enabled ? "" : " (disabled)"));
	}
      else
	{
	  (*v850_callback->printf_filtered) (v850_callback, "No interrupt generators defined.\n"); 
	}

    }
  else
    {
      (*v850_callback->printf_filtered) (v850_callback,
					 "Invalid interrupt command, must be one of `add', `remove', or `info'.\n");
    }
  /* Cache the presence of a non-maskable generator.  */
  have_nm_generator = 0;
  for (intgen = intgen_list; intgen != NULL; intgen = intgen->next)
    {
      if (intgen->type == int_nmi || intgen->type == int_reset)
	{
	  have_nm_generator = 1;
	  break;
	}
    }
}

void
sim_do_command (sd, cmd)
     SIM_DESC sd;
     char *cmd;
{
  char *mm_cmd = "memory-map";
  char *int_cmd = "interrupt";

  if (! strncmp (cmd, mm_cmd, strlen (mm_cmd))
      && strchr (" 	", cmd[strlen(mm_cmd)]))
    sim_set_memory_map (cmd + strlen(mm_cmd) + 1);

  else if (! strncmp (cmd, int_cmd, strlen (int_cmd))
      && strchr (" 	", cmd[strlen(int_cmd)]))
    sim_set_interrupt (cmd + strlen(int_cmd) + 1);

  else if (! strcmp (cmd, "help"))
    {
      (*v850_callback->printf_filtered) (v850_callback, "V850 simulator commands:\n\n");
      (*v850_callback->printf_filtered) (v850_callback, "interrupt add <inttype> { pc | time } <value> -- Set up an interrupt generator\n");
      (*v850_callback->printf_filtered) (v850_callback, "interrupt remove <n> -- Remove an existing interrupt generator\n");
      (*v850_callback->printf_filtered) (v850_callback, "interrupt info -- List all the interrupt generators\n");
      (*v850_callback->printf_filtered) (v850_callback, "memory-map hole=<m>,<n> -- Set the memory map to have a hole between <m> and <n>\n");
      (*v850_callback->printf_filtered) (v850_callback, "\n");
    }
  else
    (*v850_callback->printf_filtered) (v850_callback, "\"%s\" is not a valid V850 simulator command.\n",
				       cmd);
}

SIM_RC
sim_load (sd, prog, abfd, from_tty)
     SIM_DESC sd;
     char *prog;
     bfd *abfd;
     int from_tty;
{
  extern bfd *sim_load_file (); /* ??? Don't know where this should live.  */

  if (prog_bfd != NULL && prog_bfd_was_opened_p)
    bfd_close (prog_bfd);
  prog_bfd = sim_load_file (sd, myname, v850_callback, prog, abfd,
			    sim_kind == SIM_OPEN_DEBUG);
  if (prog_bfd == NULL)
    return SIM_RC_FAIL;
  PC = bfd_get_start_address (prog_bfd);
  prog_bfd_was_opened_p = abfd == NULL;
  return SIM_RC_OK;
} 
