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

#ifndef INLINE
#ifdef __GNUC__
#define INLINE inline
#else
#define INLINE
#endif
#endif


/* For compatibility */
SIM_DESC simulator;



/* v850 interrupt model */

enum interrupt_type
{
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

char *interrupt_names[] = {
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

static void
do_interrupt (sd, data)
     SIM_DESC sd;
     void *data;
{
  char **interrupt_name = (char**)data;
  enum interrupt_type inttype;
  inttype = (interrupt_name - STATE_WATCHPOINTS (sd)->interrupt_names);
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

/* These default values correspond to expected usage for the chip.  */

int v850_debug;

uint32 OP[4];

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

#if 0
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
#endif

SIM_DESC
sim_open (kind, cb, abfd, argv)
     SIM_OPEN_KIND kind;
     host_callback *cb;
     struct _bfd *abfd;
     char **argv;
{
  SIM_DESC sd = sim_state_alloc (kind, cb);
#if 0
  struct simops *s;
  struct hash_entry *h;
#endif

  /* for compatibility */
  simulator = sd;

  /* FIXME: should be better way of setting up interrupts */
  STATE_WATCHPOINTS (sd)->pc = &(PC);
  STATE_WATCHPOINTS (sd)->sizeof_pc = sizeof (PC);
  STATE_WATCHPOINTS (sd)->interrupt_handler = do_interrupt;
  STATE_WATCHPOINTS (sd)->interrupt_names = interrupt_names;
  /* start-sanitize-v850e */
  STATE_ARCHITECTURE (sd) = bfd_lookup_arch (bfd_arch_v850, bfd_mach_v850e);
  /* end-sanitize-v850e */
  /* start-sanitize-v850eq */
  STATE_ARCHITECTURE (sd) = bfd_lookup_arch (bfd_arch_v850, bfd_mach_v850eq);
  /* end-sanitize-v850eq */

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    return 0;

  /* Allocate core managed memory */

  /* "Mirror" the ROM addresses below 1MB. */
  sim_do_commandf (sd, "memory region 0,0x100000,0x%lx", V850_ROM_SIZE);
  /* Chunk of ram adjacent to rom */
  sim_do_commandf (sd, "memory region 0x100000,0x%lx", V850_LOW_END-0x100000);
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

#if 0
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
#endif

  return sd;
}


void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  sim_module_uninstall (sd);
}

int
sim_stop (sd)
     SIM_DESC sd;
{
  return 0;
}

#if 0
void
sim_engine_run (sd, next_cpu_nr, siggnal)
     SIM_DESC sd;
     int next_cpu_nr;
     int siggnal;
{
  uint32 inst;
  SIM_ADDR oldpc;

  while (1)
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
      
      if (sim_events_tick (sd))
	{
	  sim_events_process (sd);
	}
    }
}
#endif

#if 0
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
#endif

void
sim_info (sd, verbose)
     SIM_DESC sd;
     int verbose;
{
  profile_print (sd, STATE_VERBOSE_P (sd), NULL, NULL);
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

void
sim_do_command (sd, cmd)
     SIM_DESC sd;
     char *cmd;
{
  char *mm_cmd = "memory-map";
  char *int_cmd = "interrupt";

  if (sim_args_command (sd, cmd) != SIM_RC_OK)
    {
      if (strncmp (cmd, mm_cmd, strlen (mm_cmd) == 0))
	sim_io_eprintf (sd, "`memory-map' command replaced by `sim memory'\n");
      else if (strncmp (cmd, int_cmd, strlen (int_cmd)) == 0)
	sim_io_eprintf (sd, "`interrupt' command replaced by `sim watch'\n");
      else
	sim_io_eprintf (sd, "Unknown command `%s'\n", cmd);
    }
}
