#include <signal.h>

#include "sim-main.h"
#include "sim-options.h"
#include "sim-hw.h"

#include "sysdep.h"
#include "bfd.h"
#include "sim-assert.h"


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


host_callback *mn10300_callback;
int mn10300_debug;
struct _state State;


/* simulation target board.  NULL=default configuration */
static char* board = NULL;

static DECLARE_OPTION_HANDLER (mn10300_option_handler);

enum {
  OPTION_BOARD = OPTION_START,
};

static SIM_RC
mn10300_option_handler (sd, cpu, opt, arg, is_command)
     SIM_DESC sd;
     sim_cpu *cpu;
     int opt;
     char *arg;
     int is_command;
{
  int cpu_nr;
  switch (opt)
    {
    case OPTION_BOARD:
      {
	if (arg)
	  {
	    board = zalloc(strlen(arg) + 1);
	    strcpy(board, arg);
	  }
	return SIM_RC_OK;
      }
    }
  
  return SIM_RC_OK;
}

static const OPTION mn10300_options[] = 
{
#define BOARD_AM32 "stdeval1"
  { {"board", required_argument, NULL, OPTION_BOARD},
     '\0', "none" /* rely on compile-time string concatenation for other options */
           "|" BOARD_AM32
    , "Customize simulation for a particular board.", mn10300_option_handler },

  { {NULL, no_argument, NULL, 0}, '\0', NULL, NULL, NULL }
};

/* For compatibility */
SIM_DESC simulator;

/* These default values correspond to expected usage for the chip.  */

SIM_DESC
sim_open (kind, cb, abfd, argv)
     SIM_OPEN_KIND kind;
     host_callback *cb;
     struct bfd *abfd;
     char **argv;
{
  SIM_DESC sd = sim_state_alloc (kind, cb);
  mn10300_callback = cb;

  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  /* for compatibility */
  simulator = sd;

  /* FIXME: should be better way of setting up interrupts.  For
     moment, only support watchpoints causing a breakpoint (gdb
     halt). */
  STATE_WATCHPOINTS (sd)->pc = &(PC);
  STATE_WATCHPOINTS (sd)->sizeof_pc = sizeof (PC);
  STATE_WATCHPOINTS (sd)->interrupt_handler = NULL;
  STATE_WATCHPOINTS (sd)->interrupt_names = NULL;

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    return 0;
  sim_add_option_table (sd, NULL, mn10300_options);

  /* Allocate core managed memory */
  sim_do_command (sd, "memory region 0,0x100000");
  sim_do_command (sd, "memory region 0x40000000,0x200000");

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

  if ( NULL != board
       && (strcmp(board, BOARD_AM32) == 0 ) )
    {
      /* environment */
      STATE_ENVIRONMENT (sd) = OPERATING_ENVIRONMENT;

      sim_do_command (sd, "memory region 0x44000000,0x40000");
      sim_do_command (sd, "memory region 0x48000000,0x400000");

      /* device support for mn1030002 */
      /* interrupt controller */

      sim_hw_parse (sd, "/mn103int@0x34000100/reg 0x34000100 0x7C 0x34000200 0x8 0x34000280 0x8");

      /* DEBUG: NMI input's */
      sim_hw_parse (sd, "/glue@0x30000000/reg 0x30000000 12");
      sim_hw_parse (sd, "/glue@0x30000000 > int0 nmirq /mn103int");
      sim_hw_parse (sd, "/glue@0x30000000 > int1 watchdog /mn103int");
      sim_hw_parse (sd, "/glue@0x30000000 > int2 syserr /mn103int");
      
      /* DEBUG: ACK input */
      sim_hw_parse (sd, "/glue@0x30002000/reg 0x30002000 4");
      sim_hw_parse (sd, "/glue@0x30002000 > int ack /mn103int");
      
      /* DEBUG: LEVEL output */
      sim_hw_parse (sd, "/glue@0x30004000/reg 0x30004000 8");
      sim_hw_parse (sd, "/mn103int > nmi int0 /glue@0x30004000");
      sim_hw_parse (sd, "/mn103int > level int1 /glue@0x30004000");
      
      /* DEBUG: A bunch of interrupt inputs */
      sim_hw_parse (sd, "/glue@0x30006000/reg 0x30006000 32");
      sim_hw_parse (sd, "/glue@0x30006000 > int0 irq-0 /mn103int");
      sim_hw_parse (sd, "/glue@0x30006000 > int1 irq-1 /mn103int");
      sim_hw_parse (sd, "/glue@0x30006000 > int2 irq-2 /mn103int");
      sim_hw_parse (sd, "/glue@0x30006000 > int3 irq-3 /mn103int");
      sim_hw_parse (sd, "/glue@0x30006000 > int4 irq-4 /mn103int");
      sim_hw_parse (sd, "/glue@0x30006000 > int5 irq-5 /mn103int");
      sim_hw_parse (sd, "/glue@0x30006000 > int6 irq-6 /mn103int");
      sim_hw_parse (sd, "/glue@0x30006000 > int7 irq-7 /mn103int");
      
      /* processor interrupt device */
      
      /* the device */
      sim_hw_parse (sd, "/mn103cpu@0x20000000");
      sim_hw_parse (sd, "/mn103cpu@0x20000000/reg 0x20000000 0x42");
      
      /* DEBUG: ACK output wired upto a glue device */
      sim_hw_parse (sd, "/glue@0x20002000");
      sim_hw_parse (sd, "/glue@0x20002000/reg 0x20002000 4");
      sim_hw_parse (sd, "/mn103cpu > ack int0 /glue@0x20002000");
      
      /* DEBUG: RESET/NMI/LEVEL wired up to a glue device */
      sim_hw_parse (sd, "/glue@0x20004000");
      sim_hw_parse (sd, "/glue@0x20004000/reg 0x20004000 12");
      sim_hw_parse (sd, "/glue@0x20004000 > int0 reset /mn103cpu");
      sim_hw_parse (sd, "/glue@0x20004000 > int1 nmi /mn103cpu");
      sim_hw_parse (sd, "/glue@0x20004000 > int2 level /mn103cpu");
      
      /* REAL: The processor wired up to the real interrupt controller */
      sim_hw_parse (sd, "/mn103cpu > ack ack /mn103int");
      sim_hw_parse (sd, "/mn103int > level level /mn103cpu");
      sim_hw_parse (sd, "/mn103int > nmi nmi /mn103cpu");
      
      
      /* PAL */
      
      /* the device */
      sim_hw_parse (sd, "/pal@0x31000000");
      sim_hw_parse (sd, "/pal@0x31000000/reg 0x31000000 64");
      sim_hw_parse (sd, "/pal@0x31000000/poll? true");
      
      /* DEBUG: PAL wired up to a glue device */
      sim_hw_parse (sd, "/glue@0x31002000");
      sim_hw_parse (sd, "/glue@0x31002000/reg 0x31002000 16");
      sim_hw_parse (sd, "/pal@0x31000000 > countdown int0 /glue@0x31002000");
      sim_hw_parse (sd, "/pal@0x31000000 > timer int1 /glue@0x31002000");
      sim_hw_parse (sd, "/pal@0x31000000 > int int2 /glue@0x31002000");
      sim_hw_parse (sd, "/glue@0x31002000 > int0 int3 /glue@0x31002000");
      sim_hw_parse (sd, "/glue@0x31002000 > int1 int3 /glue@0x31002000");
      sim_hw_parse (sd, "/glue@0x31002000 > int2 int3 /glue@0x31002000");
      
      /* REAL: The PAL wired up to the real interrupt controller */
      sim_hw_parse (sd, "/pal@0x31000000 > countdown irq-0 /mn103int");
      sim_hw_parse (sd, "/pal@0x31000000 > timer irq-1 /mn103int");
      sim_hw_parse (sd, "/pal@0x31000000 > int irq-2 /mn103int");
      
      /* 8 and 16 bit timers */
      sim_hw_parse (sd, "/mn103tim@0x34001000/reg 0x34001000 36 0x34001080 100 0x34004000 16");

      /* Hook timer interrupts up to interrupt controller */
      sim_hw_parse (sd, "/mn103tim > timer-0-underflow timer-0-underflow /mn103int");
      sim_hw_parse (sd, "/mn103tim > timer-1-underflow timer-1-underflow /mn103int");
      sim_hw_parse (sd, "/mn103tim > timer-2-underflow timer-2-underflow /mn103int");
      sim_hw_parse (sd, "/mn103tim > timer-3-underflow timer-3-underflow /mn103int");
      sim_hw_parse (sd, "/mn103tim > timer-4-underflow timer-4-underflow /mn103int");
      sim_hw_parse (sd, "/mn103tim > timer-5-underflow timer-5-underflow /mn103int");
      sim_hw_parse (sd, "/mn103tim > timer-6-underflow timer-6-underflow /mn103int");
      sim_hw_parse (sd, "/mn103tim > timer-6-compare-a timer-6-compare-a /mn103int");
      sim_hw_parse (sd, "/mn103tim > timer-6-compare-b timer-6-compare-b /mn103int");
      
      
      /* Serial devices 0,1,2 */
      sim_hw_parse (sd, "/mn103ser@0x34000800/reg 0x34000800 48");
      sim_hw_parse (sd, "/mn103ser@0x34000800/poll? true");
      
      /* Hook serial interrupts up to interrupt controller */
      sim_hw_parse (sd, "/mn103ser > serial-0-receive serial-0-receive /mn103int");
      sim_hw_parse (sd, "/mn103ser > serial-0-transmit serial-0-transmit /mn103int");
      sim_hw_parse (sd, "/mn103ser > serial-1-receive serial-1-receive /mn103int");
      sim_hw_parse (sd, "/mn103ser > serial-1-transmit serial-1-transmit /mn103int");
      sim_hw_parse (sd, "/mn103ser > serial-2-receive serial-2-receive /mn103int");
      sim_hw_parse (sd, "/mn103ser > serial-2-transmit serial-2-transmit /mn103int");
      
      sim_hw_parse (sd, "/mn103iop@0x36008000/reg 0x36008000 8 0x36008020 8 0x36008040 0xc 0x36008060 8 0x36008080 8");

      /* Memory control registers */
      sim_do_command (sd, "memory region 0x32000020,0x30");
      /* Cache control register */
      sim_do_command (sd, "memory region 0x20000070,0x4");
      /* Cache purge regions */
      sim_do_command (sd, "memory region 0x28400000,0x800");
      sim_do_command (sd, "memory region 0x28401000,0x800");
      /* DMA registers */
      sim_do_command (sd, "memory region 0x32000100,0xF");
      sim_do_command (sd, "memory region 0x32000200,0xF");
      sim_do_command (sd, "memory region 0x32000400,0xF");
      sim_do_command (sd, "memory region 0x32000800,0xF");
    }
  else
    {
      if (board != NULL)
        {
	  sim_io_eprintf (sd, "Error: Board `%s' unknown.\n", board);
          return 0;
	}
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


  /* set machine specific configuration */
/*   STATE_CPU (sd, 0)->psw_mask = (PSW_NP | PSW_EP | PSW_ID | PSW_SAT */
/* 			     | PSW_CY | PSW_OV | PSW_S | PSW_Z); */

  return sd;
}


void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  sim_module_uninstall (sd);
}


SIM_RC
sim_create_inferior (sd, prog_bfd, argv, env)
     SIM_DESC sd;
     struct bfd *prog_bfd;
     char **argv;
     char **env;
{
  memset (&State, 0, sizeof (State));
  if (prog_bfd != NULL) {
    PC = bfd_get_start_address (prog_bfd);
  } else {
    PC = 0;
  }
  CIA_SET (STATE_CPU (sd, 0), (unsigned64) PC);

  return SIM_RC_OK;
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

int
sim_fetch_register (sd, rn, memory, length)
     SIM_DESC sd;
     int rn;
     unsigned char *memory;
     int length;
{
  put_word (memory, State.regs[rn]);
  return -1;
}
 
int
sim_store_register (sd, rn, memory, length)
     SIM_DESC sd;
     int rn;
     unsigned char *memory;
     int length;
{
  State.regs[rn] = get_word (memory);
  return -1;
}


void
mn10300_core_signal (SIM_DESC sd,
                 sim_cpu *cpu,
                 sim_cia cia,
                 unsigned map,
                 int nr_bytes,
                 address_word addr,
                 transfer_type transfer,
                 sim_core_signals sig)
{
  const char *copy = (transfer == read_transfer ? "read" : "write");
  address_word ip = CIA_ADDR (cia);

  switch (sig)
    {
    case sim_core_unmapped_signal:
      sim_io_eprintf (sd, "mn10300-core: %d byte %s to unmapped address 0x%lx at 0x%lx\n",
                      nr_bytes, copy, 
                      (unsigned long) addr, (unsigned long) ip);
      program_interrupt(sd, cpu, cia, SIM_SIGSEGV);
      break;

    case sim_core_unaligned_signal:
      sim_io_eprintf (sd, "mn10300-core: %d byte %s to unaligned address 0x%lx at 0x%lx\n",
                      nr_bytes, copy, 
                      (unsigned long) addr, (unsigned long) ip);
      program_interrupt(sd, cpu, cia, SIM_SIGBUS);
      break;

    default:
      sim_engine_abort (sd, cpu, cia,
                        "mn10300_core_signal - internal error - bad switch");
    }
}


void
program_interrupt (SIM_DESC sd,
		   sim_cpu *cpu,
		   sim_cia cia,
		   SIM_SIGNAL sig)
{
  int status;
  struct hw *device;
  static int in_interrupt = 0;

#ifdef SIM_CPU_EXCEPTION_TRIGGER
  SIM_CPU_EXCEPTION_TRIGGER(sd,cpu,cia);
#endif

  /* avoid infinite recursion */
  if (in_interrupt)
    {
      (*mn10300_callback->printf_filtered) (mn10300_callback, 
					    "ERROR: recursion in program_interrupt during software exception dispatch.");
    }
  else
    {
      in_interrupt = 1;
      /* copy NMI handler code from dv-mn103cpu.c */
      store_word (SP - 4, CIA_GET (cpu));
      store_half (SP - 8, PSW);

      /* Set the SYSEF flag in NMICR by backdoor method.  See
	 dv-mn103int.c:write_icr().  This is necessary because
         software exceptions are not modelled by actually talking to
         the interrupt controller, so it cannot set its own SYSEF
         flag. */
     if ((NULL != board) && (strcmp(board, BOARD_AM32) == 0))
       store_byte (0x34000103, 0x04);
    }

  PSW &= ~PSW_IE;
  SP = SP - 8;
  CIA_SET (cpu, 0x40000008);

  in_interrupt = 0;
  sim_engine_halt(sd, cpu, NULL, cia, sim_stopped, sig);
}


void
mn10300_cpu_exception_trigger(SIM_DESC sd, sim_cpu* cpu, address_word cia)
{
  ASSERT(cpu != NULL);

  if(State.exc_suspended > 0)
    sim_io_eprintf(sd, "Warning, nested exception triggered (%d)\n", State.exc_suspended); 

  CIA_SET (cpu, cia);
  memcpy(State.exc_trigger_regs, State.regs, sizeof(State.exc_trigger_regs));
  State.exc_suspended = 0;
}

void
mn10300_cpu_exception_suspend(SIM_DESC sd, sim_cpu* cpu, int exception)
{
  ASSERT(cpu != NULL);

  if(State.exc_suspended > 0)
    sim_io_eprintf(sd, "Warning, nested exception signal (%d then %d)\n", 
		   State.exc_suspended, exception); 

  memcpy(State.exc_suspend_regs, State.regs, sizeof(State.exc_suspend_regs));
  memcpy(State.regs, State.exc_trigger_regs, sizeof(State.regs));
  CIA_SET (cpu, PC); /* copy PC back from new State.regs */
  State.exc_suspended = exception;
}

void
mn10300_cpu_exception_resume(SIM_DESC sd, sim_cpu* cpu, int exception)
{
  ASSERT(cpu != NULL);

  if(exception == 0 && State.exc_suspended > 0)
    {
      if(State.exc_suspended != SIGTRAP) /* warn not for breakpoints */
         sim_io_eprintf(sd, "Warning, resuming but ignoring pending exception signal (%d)\n",
  		       State.exc_suspended); 
    }
  else if(exception != 0 && State.exc_suspended > 0)
    {
      if(exception != State.exc_suspended) 
	sim_io_eprintf(sd, "Warning, resuming with mismatched exception signal (%d vs %d)\n",
		       State.exc_suspended, exception); 
      
      memcpy(State.regs, State.exc_suspend_regs, sizeof(State.regs)); 
      CIA_SET (cpu, PC); /* copy PC back from new State.regs */
    }
  else if(exception != 0 && State.exc_suspended == 0)
    {
      sim_io_eprintf(sd, "Warning, ignoring spontanous exception signal (%d)\n", exception); 
    }
  State.exc_suspended = 0; 
}
