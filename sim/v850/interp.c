#include <signal.h>
#include "sim-main.h"
#include "sim-options.h"
#include "v850_sim.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif

#include "bfd.h"


/* For compatibility */
SIM_DESC simulator;

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
  SIM_ADDR address;
  unsigned long time;
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

int v850_debug;

uint32 OP[4];

static struct hash_entry *lookup_hash PARAMS ((SIM_DESC sd, uint32 ins));
static long hash PARAMS ((long));
#if 0
static void do_format_1_2 PARAMS ((uint32));
static void do_format_3 PARAMS ((uint32));
static void do_format_4 PARAMS ((uint32));
static void do_format_5 PARAMS ((uint32));
static void do_format_6 PARAMS ((uint32));
static void do_format_7 PARAMS ((uint32));
static void do_format_8 PARAMS ((uint32));
static void do_format_9_10 PARAMS ((uint32));
#endif

#define MAX_HASH  63

struct hash_entry
{
  struct hash_entry *next;
  unsigned long opcode;
  unsigned long mask;
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
lookup_hash (sd, ins)
     SIM_DESC sd;
     uint32 ins;
{
  struct hash_entry *h;

  h = &hash_table[hash(ins)];

  while ((ins & h->mask) != h->opcode)
    {
      if (h->next == NULL)
	{
	  sim_io_error (sd, "ERROR looking up hash for 0x%lx, PC=0x%lx",
			(long) ins, (long) PC);
	}
      h = h->next;
    }
  return (h);
}

SIM_DESC
sim_open (kind, cb, abfd, argv)
     SIM_OPEN_KIND kind;
     host_callback *cb;
     struct _bfd *abfd;
     char **argv;
{
  char *buf;
  SIM_DESC sd = sim_state_alloc (kind, cb);
  struct simops *s;
  struct hash_entry *h;

  /* for compatibility */
  simulator = sd;

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    return 0;

  /* Allocate core managed memory */
  /* "Mirror" the ROM addresses below 1MB. */
  asprintf (&buf, "memory region 0,0x100000,0x%lx", V850_ROM_SIZE);
  sim_do_command (sd, buf);
  free (buf);
  /* Chunk of ram adjacent to rom */
  asprintf (&buf, "memory region 0x100000,0x%lx", V850_LOW_END - 0x100000);
  sim_do_command (sd, buf);
  free (buf);
  /* peripheral I/O region - mirror 1K across 4k (0x1000) */
  sim_do_command (sd, "memory region 0xfff000,0x1000,1024");
  /* similarly if in the internal RAM region */
  sim_do_command (sd, "memory region 0xffe000,0x1000,1024");

  /* getopt will print the error message so we just have to exit if this fails.
     FIXME: Hmmm...  in the case of gdb we need getopt to call
     print_filtered.  */
  if (sim_parse_args (sd, argv) != SIM_RC_OK)
    {
      /* Uninstall the modules to avoid memory leaks,
	 file descriptor leaks, etc.  */
      sim_module_uninstall (sd);
      return 0;
    }

  /* check for/establish the a reference program image */
  if (sim_analyze_program (sd,
			   (STATE_PROG_ARGV (sd) != NULL
			    ? *STATE_PROG_ARGV (sd)
			    : NULL),
			   abfd) != SIM_RC_OK)
    {
      sim_module_uninstall (sd);
      return 0;
    }

  /* establish any remaining configuration options */
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

  return sd;
}


void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  sim_module_uninstall (sd);
}

static void do_interrupt PARAMS ((SIM_DESC sd, enum interrupt_type));

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
  SIM_ELAPSED_TIME start_time;
  uint32 inst;
  SIM_ADDR oldpc;
  struct interrupt_generator *intgen;

  if (step)
    State.exception = SIGTRAP;
  else
    State.exception = 0;


  start_time = sim_elapsed_time_get ();

  do
    {
      struct hash_entry * h;
      /* Fetch the current instruction.  */
      inst  = RLW (PC);
      oldpc = PC;

      h     = lookup_hash (sd, inst);
      OP[0] = inst & 0x1f;
      OP[1] = (inst >> 11) & 0x1f;
      OP[2] = (inst >> 16) & 0xffff;
      OP[3] = inst;

      /* fprintf (stderr, "PC = %x, SP = %x\n", PC, SP ); */

      if (inst == 0)
	{
	  fprintf (stderr, "NOP encountered!\n");
	  break;
	}
      
      PC += h->ops->func ();

      if (oldpc == PC)
	{
	  sim_io_eprintf (sd, "simulator loop at %lx\n", (long) PC );
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
		  SIM_ELAPSED_TIME delta;
		  delta = sim_elapsed_time_since (start_time);
		  if (delta > intgen->time)
		    {
		      intgen->enabled = 0;
		      break;
		    }
		}
	    }
	  if (intgen)
	    do_interrupt (sd, intgen->type);
	}
      else if (State.pending_nmi)
	{
	  State.pending_nmi = 0;
	  do_interrupt (sd, int_nmi);
	}
    }
  while (!State.exception);
}

static void
do_interrupt (sd, inttype)
     SIM_DESC sd;
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
  sim_io_printf (sd, "sim_info\n");
}

SIM_RC
sim_create_inferior (sd, prog_bfd, argv, env)
     SIM_DESC sd;
     struct _bfd *prog_bfd;
     char **argv;
     char **env;
{
  memset (&State, 0, sizeof (State));
  if (prog_bfd != NULL)
    PC = bfd_get_start_address (prog_bfd);
  return SIM_RC_OK;
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
  *(unsigned32*)memory = H2T_4 (State.regs[rn]);
}
 
void
sim_store_register (sd, rn, memory)
     SIM_DESC sd;
     int rn;
     unsigned char *memory;
{
  State.regs[rn] = T2H_4 (*(unsigned32*)memory);
}

static int
sim_parse_number (str, rest)
     char *str, **rest;
{
  if (str[0] == '0' && str[1] == 'x')
    return strtoul (str, rest, 16);
  else if (str[0] == '0')
    return strtoul (str, rest, 16);
  else
    return strtoul (str, rest, 10);
}

int current_intgen_number = 1;

static void
sim_set_interrupt (sd, spec)
     SIM_DESC sd;
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
	  sim_io_printf (sd, "Interrupt type unknown; known types are\n");
	  for (i = 0; i < num_int_types; ++i)
	    {
	      sim_io_printf (sd, " %s", interrupt_names[i]);
	    }
	  sim_io_printf (sd, "\n");
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
	  sim_io_printf (sd, "Condition type must be `pc' or `time'.\n");
	  free (intgen);
	  return;
	}
      /* We now have a valid interrupt generator.  Number it and add
	 to the list of generators.  */
      intgen->number = current_intgen_number++;
      intgen->enabled = 1;
      intgen->next = intgen_list;
      intgen_list = intgen;
      sim_io_printf (sd, "Interrupt generator %d (NMI) at pc=0x%x, time=%ld.\n", intgen_list->number, intgen_list->address, intgen_list->time);
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
	    sim_io_printf (sd, "No interrupt generator numbered %d, ignoring.\n", num);
	}
    }
  else if (*argv && !strcmp (*argv, "info"))
    {
      if (intgen_list)
	{
	  for (intgen = intgen_list; intgen != NULL; intgen = intgen->next)
	    sim_io_printf (sd, "Interrupt generator %d (%s) at pc=0x%lx/time=%ld%s.\n",
			   intgen->number,
			   interrupt_names[intgen->type],
			   (long) intgen->address,
			   intgen->time,
			   (intgen->enabled ? "" : " (disabled)"));
	}
      else
	{
	  sim_io_printf (sd, "No interrupt generators defined.\n"); 
	}

    }
  else
    {
      sim_io_printf (sd, "Invalid interrupt command, must be one of `add', `remove', or `info'.\n");
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

  if (strncmp (cmd, mm_cmd, strlen (mm_cmd) == 0))
    sim_io_eprintf (sd, "`memory-map' command replaced by `sim memory'\n");

  else if (! strncmp (cmd, int_cmd, strlen (int_cmd))
      && strchr (" 	", cmd[strlen(int_cmd)]))
    sim_set_interrupt (sd, cmd + strlen(int_cmd) + 1);

  else if (sim_args_command (sd, cmd) != SIM_RC_OK)
    sim_io_eprintf (sd, "Unknown command `%s'\n", cmd);
}
