/* Remote debugging interface for AMD 29k interfaced via UDI, for GDB.
   Copyright 1990, 1992 Free Software Foundation, Inc.
   Written by Daniel Mann.  Contributed by AMD.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* This is like remote.c but uses the Universal Debug Interface (UDI) to 
   talk to the target hardware (or simulator).  UDI is a TCP/IP based
   protocol; for hardware that doesn't run TCP, an interface adapter 
   daemon talks UDI on one side, and talks to the hardware (typically
   over a serial port) on the other side.

 - Originally written by Daniel Mann at AMD for MiniMON and gdb 3.91.6.
 - David Wood (wood@lab.ultra.nyu.edu) at New York University adapted this
	file to gdb 3.95.  I was unable to get this working on sun3os4
	with termio, only with sgtty.  Because we are only attempting to
	use this module to debug our kernel, which is already loaded when
	gdb is started up, I did not code up the file downloading facilities.  
	As a result this module has only the stubs to download files. 
	You should get tagged at compile time if you need to make any 
	changes/additions.
 - Daniel Mann at AMD took the 3.95 adaptions above and replaced
   	MiniMON interface with UDI-p interface.	  */
 
#include "defs.h"
#include "inferior.h"
#include "wait.h"
#include "value.h"
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "terminal.h"
#include "target.h"
#include "29k-share/udi/udiproc.h"
#include "gdbcmd.h"
#include "bfd.h"

/* access the register store directly, without going through
   the normal handler functions. This avoids an extra data copy.  */

static int kiodebug;
extern int stop_soon_quietly;           /* for wait_for_inferior */
extern struct value *call_function_by_hand();
static void udi_resume PARAMS ((int step, int sig));
static void udi_fetch_registers PARAMS ((int regno));
static void udi_load PARAMS ((char *args, int from_tty));
static void fetch_register PARAMS ((int regno));
static void udi_store_registers PARAMS ((int regno));
static int store_register PARAMS ((int regno));
static int regnum_to_srnum PARAMS ((int regno));
static void udi_close PARAMS ((int quitting));
static CPUSpace udi_memory_space PARAMS ((CORE_ADDR addr));
static int udi_write_inferior_memory PARAMS ((CORE_ADDR memaddr, char *myaddr,
					      int len));
static int udi_read_inferior_memory PARAMS ((CORE_ADDR memaddr, char *myaddr,
					     int len));
static void download PARAMS ((char *load_arg_string, int from_tty));
char   CoffFileName[100] = "";
/*
 * Processor types. 
 */
#define TYPE_UNKNOWN    0
#define TYPE_A29000     1
#define TYPE_A29030     2
#define TYPE_A29050     3
static  char *processor_name[] = { "Unknown", "Am29000", "Am29030", "Am29050" };
static  int processor_type=TYPE_UNKNOWN;
#define FREEZE_MODE     (read_register(CPS_REGNUM) & 0x400)
#define USE_SHADOW_PC	((processor_type == TYPE_A29050) && FREEZE_MODE) 

#define LLOG_FILE "udi.log"
#if defined (LOG_FILE)
FILE *log_file;
#endif

static int timeout = 5;
extern struct target_ops udi_ops;             /* Forward declaration */

/* Special register enumeration.
*/

/******************************************************************* UDI DATA*/
#define	MAXDATA		2*1024		/* max UDI[read/write] byte size */
/* Descriptor for I/O to remote machine.  Initialize it to -1 so that
   udi_open knows that we don't have a file open when the program
   starts.  */

UDISessionId udi_session_id = -1;

CPUOffset IMemStart = 0;
CPUSizeT IMemSize = 0;
CPUOffset DMemStart = 0;
CPUSizeT DMemSize = 0;
CPUOffset RMemStart = 0;
CPUSizeT RMemSize = 0;
UDIUInt32 CPUPRL;
UDIUInt32 CoProcPRL;

UDIMemoryRange address_ranges[2]; /* Text and data */
UDIResource entry = {0, 0};	/* Entry point */
CPUSizeT stack_sizes[2];	/* Regular and memory stacks */

#define	SBUF_MAX	1024	/* maximum size of string handling buffer */
char sbuf[SBUF_MAX];

typedef	struct 	bkpt_entry_str
{
    UDIResource	 Addr;
    UDIUInt32	 PassCount;
    UDIBreakType Type;
    unsigned int BreakId;
} bkpt_entry_t;
#define		BKPT_TABLE_SIZE 40
static bkpt_entry_t	bkpt_table[BKPT_TABLE_SIZE];
extern	char	dfe_errmsg[];		/* error string */

/* Called when SIGALRM signal sent due to alarm() timeout.  */
#ifndef HAVE_TERMIO

volatile int n_alarms;

static void
udi_timer ()
{
#if 0
  if (kiodebug)
    printf ("udi_timer called\n");
#endif
  n_alarms++;
}
#endif	/* HAVE_TERMIO */

/* malloc'd name of the program on the remote system.  */
static char *prog_name = NULL;

/* Number of SIGTRAPs we need to simulate.  That is, the next
   NEED_ARTIFICIAL_TRAP calls to udi_wait should just return
   SIGTRAP without actually waiting for anything.  */

/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */

static void
udi_create_inferior (execfile, args, env)
     char *execfile;
     char *args;
     char **env;
{
  char *args1;

  if (execfile)
    {
      if (prog_name != NULL)
	free (prog_name);
      prog_name = savestring (execfile, strlen (execfile));
    }
  else if (entry.Offset)
    execfile = "";
  else
    error ("No image loaded into target.");

  if (udi_session_id < 0)
    {
      printf("UDI connection not open yet.\n");
      return;
    }

  inferior_pid = 40000;

  if (!entry.Offset)
    download(execfile, 0);

  args1 = alloca (strlen(execfile) + strlen(args) + 2);

  strcpy (args1, execfile);
  strcat (args1, " ");
  strcat (args1, args);

  UDIInitializeProcess (address_ranges,		/* ProcessMemory[] */
			(UDIInt)2,		/* NumberOfRanges */
			entry,			/* EntryPoint */
			stack_sizes,		/* *StackSizes */
			(UDIInt)2,		/* NumberOfStacks */
			args1);			/* ArgString */

  init_wait_for_inferior ();
  clear_proceed_status ();
  proceed(-1,-1,0);
}

static void
udi_mourn()
{
        pop_target ();                /* Pop back to no-child state */
        generic_mourn_inferior ();
}

/******************************************************************** UDI_OPEN
** Open a connection to remote TIP.
   NAME is the socket domain used for communication with the TIP,
   then a space and the socket name or TIP-host name.
   '<udi_udi_config_id>' for example.
 */

/* XXX - need cleanups for udiconnect for various failures!!! */

static char *udi_config_id;
static void
udi_open (name, from_tty)
     char *name;
     int from_tty;
{
  unsigned int prl;
  char *p;
  int cnt;
  UDIMemoryRange KnownMemory[10];
  UDIUInt32 ChipVersions[10];
  UDIInt NumberOfRanges = 10;
  UDIInt NumberOfChips = 10;
  UDIPId PId;
  UDIUInt32 TIPId, TargetId, DFEId, DFE, TIP, DFEIPCId, TIPIPCId;

  target_preopen(from_tty);

  entry.Offset = 0;

  for (cnt = 0; cnt < BKPT_TABLE_SIZE; cnt++)
    bkpt_table[cnt].Type = 0;

  if (udi_config_id)
    free (udi_config_id);

  if (!name)
    error("Usage: target udi config_id, where config_id appears in udi_soc file");

  udi_config_id = strdup (strtok (name, " \t"));

  if (UDIConnect (udi_config_id, &udi_session_id))
    error("UDIConnect() failed: %s\n", dfe_errmsg);

  push_target (&udi_ops);

#ifndef HAVE_TERMIO
#ifndef NO_SIGINTERRUPT
  /* Cause SIGALRM's to make reads fail with EINTR instead of resuming
     the read.  */
  if (siginterrupt (SIGALRM, 1) != 0)
    error ("udi_open: siginterrupt() %s", safe_strerror(errno));
#endif

  /* Set up read timeout timer.  */
  if ((void (*)) signal (SIGALRM, udi_timer) == (void (*)) -1)
    error ("udi_open: signal() %s", safe_strerror(errno));
#endif

#if defined (LOG_FILE)
  log_file = fopen (LOG_FILE, "w");
  if (log_file == NULL)
    error ("udi_open: fopen(%s) %s", LOG_FILE, safe_strerror(errno));
#endif
  /*
  ** Initialize target configuration structure (global)
  */
  if (UDIGetTargetConfig (KnownMemory, &NumberOfRanges,
			  ChipVersions, &NumberOfChips))
    error ("UDIGetTargetConfig() failed");
  if (NumberOfChips > 2)
    fprintf(stderr,"Target has more than one processor\n");
  for (cnt=0; cnt < NumberOfRanges; cnt++)
    {
      switch(KnownMemory[cnt].Space)
	{
	default:
	  fprintf(stderr, "UDIGetTargetConfig() unknown memory space\n");
	  break;
	case UDI29KCP_S:
	  break;
	case UDI29KIROMSpace:
	  RMemStart = KnownMemory[cnt].Offset;
	  RMemSize = KnownMemory[cnt].Size;
	  break;
	case UDI29KIRAMSpace:
	  IMemStart = KnownMemory[cnt].Offset;
	  IMemSize = KnownMemory[cnt].Size;
	  break;
	case UDI29KDRAMSpace:
	  DMemStart = KnownMemory[cnt].Offset;
	  DMemSize = KnownMemory[cnt].Size;
	  break;
	}
    }

  /* Determine the processor revision level */
  prl = (unsigned int)read_register (CFG_REGNUM) >> 24;
  if ((prl&0xe0) == 0)
    {
      fprintf_filtered (stderr,
			"Remote debugging Am29000 rev %c\n",'A'+(prl&0x1f));
      processor_type = TYPE_A29000;
    }
  else if ((prl&0xe0) == 0x40)       /* 29030 = 0x4* */
    {
      fprintf_filtered (stderr,
			"Remote debugging Am2903* rev %c\n",'A'+(prl&0x1f));
      processor_type = TYPE_A29030;
    }
  else if ((prl&0xe0) == 0x20)       /* 29050 = 0x2* */
    {
      fprintf_filtered (stderr,
			"Remote debugging Am29050 rev %c\n",'A'+(prl&0x1f));
      processor_type = TYPE_A29050;
    }
  else
    {
      processor_type = TYPE_UNKNOWN;
      fprintf_filtered (stderr,"WARNING: processor type unknown.\n");
    }
  if (UDICreateProcess (&PId))
     fprintf(stderr, "UDICreateProcess() failed\n");

  /* Print out some stuff, letting the user now what's going on */
  if (UDICapabilities (&TIPId, &TargetId, DFEId, DFE, &TIP, &DFEIPCId,
		       &TIPIPCId, sbuf))
    error ("UDICapabilities() failed");
  if (from_tty)
    {
      printf_filtered ("Remote debugging an %s connected via UDI socket,\n\
 DFE-IPC version %x.%x.%x  TIP-IPC version %x.%x.%x  TIP version %x.%x.%x\n %s\n",
		       processor_name[processor_type],
		       (DFEIPCId>>8)&0xf, (DFEIPCId>>4)&0xf, DFEIPCId&0xf,
		       (TIPIPCId>>8)&0xf, (TIPIPCId>>4)&0xf, TIPIPCId&0xf,
		       (TargetId>>8)&0xf, (TargetId>>4)&0xf, TargetId&0xf,
		       sbuf);
    }
}

/******************************************************************* UDI_CLOSE
   Close the open connection to the TIP process.
   Use this when you want to detach and do something else
   with your gdb.  */
static void
udi_close (quitting)	/*FIXME: how is quitting used */
     int quitting;
{
  if (udi_session_id < 0)
    return;

  /* We should never get here if there isn't something valid in
     udi_session_id. */

  if (UDIDisconnect (udi_session_id, UDITerminateSession))
    error ("UDIDisconnect() failed in udi_close");

  /* Do not try to close udi_session_id again, later in the program.  */
  udi_session_id = -1;
  inferior_pid = 0;

#if defined (LOG_FILE)
  if (ferror (log_file))
    printf ("Error writing log file.\n");
  if (fclose (log_file) != 0)
    printf ("Error closing log file.\n");
#endif

  printf_filtered ("  Ending remote debugging\n");
} 

/**************************************************************** UDI_ATACH */
/* Attach to a program that is already loaded and running 
 * Upon exiting the process's execution is stopped.
 */
static void
udi_attach (args, from_tty)
     char *args;
     int from_tty;
{
  UDIResource	From;
  UDIInt32	PC_adds;
  UDICount	Count = 1;
  UDISizeT	Size = 4;
  UDICount	CountDone;
  UDIBool	HostEndian = 0;
  UDIError	err;

  if (udi_session_id < 0)
      error ("UDI connection not opened yet, use the 'target udi' command.\n");
	
  if (from_tty)
      printf ("Attaching to remote program %s...\n", prog_name);

  UDIStop();
  From.Space = 11;
  From.Offset = UDI29KSpecialRegs;
  if (err = UDIRead(From, &PC_adds, Count, Size, &CountDone, HostEndian))
    error ("UDIRead failed in udi_attach");
  printf ("Remote process is now halted, pc1 = 0x%x.\n", PC_adds);
}
/************************************************************* UDI_DETACH */
/* Terminate the open connection to the TIP process.
   Use this when you want to detach and do something else
   with your gdb.  Leave remote process running (with no breakpoints set). */
static void
udi_detach (args,from_tty)
     char *args;
     int from_tty;
{

  remove_breakpoints();		/* Just in case there were any left in */

  if (UDIDisconnect (udi_session_id, UDIContinueSession))
    error ("UDIDisconnect() failed in udi_detach");

  pop_target();         	/* calls udi_close to do the real work */

  if (from_tty)
    printf ("Ending remote debugging\n");
}


/****************************************************************** UDI_RESUME
** Tell the remote machine to resume.  */

static void
udi_resume (step, sig)
     int step, sig;
{
  UDIError tip_error;
  UDIUInt32 Steps = 1;
  UDIStepType StepType = UDIStepNatural;
  UDIRange Range;

  if (step) 			/* step 1 instruction */
    {
      tip_error = UDIStep (Steps, StepType, Range);
      if (!tip_error)
	return;

      fprintf (stderr,  "UDIStep() error = %d\n", tip_error);
      error ("failed in udi_resume");
    }

  if (UDIExecute())
    error ("UDIExecute() failed in udi_resume");
}

/******************************************************************** UDI_WAIT
** Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

static int
udi_wait (status)
     WAITTYPE *status;
{
  UDIInt32	MaxTime;
  UDIPId	PId;
  UDIInt32	StopReason;
  UDISizeT	CountDone;
  int 		old_timeout = timeout;
  int 		old_immediate_quit = immediate_quit;
  int		i;

  WSETEXIT ((*status), 0);

/* wait for message to arrive. It should be:
  If the target stops executing, udi_wait() should return.
*/
  timeout = 0;			/* Wait indefinetly for a message */
  immediate_quit = 1;   	/* Helps ability to QUIT */

  while(1)
    {
      i = 0;
      MaxTime = UDIWaitForever;
      UDIWait(MaxTime, &PId, &StopReason);
      QUIT;			/* Let user quit if they want */

      switch (StopReason & UDIGrossState)
	{
	case UDIStdoutReady:
	  if (UDIGetStdout (sbuf, (UDISizeT)SBUF_MAX, &CountDone))
	    error ("UDIGetStdin() failed in udi_wait");
	  fwrite (sbuf, 1, CountDone, stdout);
	  fflush(stdout);
	  continue;
	case UDIStderrReady:
	  UDIGetStderr (sbuf, (UDISizeT)SBUF_MAX, &CountDone);
	  fwrite (sbuf, 1, CountDone, stderr);
	  fflush(stderr);
	  continue;
	case UDIStdinNeeded:
	  printf("DEBUG: stdin requested ... continue\n");
	  /*	UDIPutStdin(sbuf, (UDISizeT)i, &CountDone); */
	  continue;
	case UDIRunning:
	  /* In spite of the fact that we told UDIWait to wait forever, it will
	     return spuriously sometimes.  */
	case UDIStdinModeX:
	  continue;
	default:
	  break;
	}
      break;
    }

  switch (StopReason & UDIGrossState)
    {
    case UDITrapped:
      printf("Am290*0 received vector number %d\n", StopReason >> 24);
	  
      switch (StopReason >> 8)
	{
	case 0:			/* Illegal opcode */
	  printf("	(break point)\n");
	  WSETSTOP ((*status), SIGTRAP);
	  break;
	case 1:			/* Unaligned Access */
	  WSETSTOP ((*status), SIGBUS);
	  break;
	case 3:
	case 4:
	  WSETSTOP ((*status), SIGFPE);
	  break;
	case 5:			/* Protection Violation */
	  WSETSTOP ((*status), SIGILL);
	  break;
	case 6:
	case 7:
	case 8:			/* User Instruction Mapping Miss */
	case 9:			/* User Data Mapping Miss */
	case 10:		/* Supervisor Instruction Mapping Miss */
	case 11:		/* Supervisor Data Mapping Miss */
	  WSETSTOP ((*status), SIGSEGV);
	  break;
	case 12:
	case 13:
	  WSETSTOP ((*status), SIGILL);
	  break;
	case 14:		/* Timer */
	  WSETSTOP ((*status), SIGALRM);
	  break;
	case 15:		/* Trace */
	  WSETSTOP ((*status), SIGTRAP);
	  break;
	case 16:		/* INTR0 */
	case 17:		/* INTR1 */
	case 18:		/* INTR2 */
	case 19:		/* INTR3/Internal */
	case 20:		/* TRAP0 */
	case 21:		/* TRAP1 */
	  WSETSTOP ((*status), SIGINT);
	  break;
	case 22:		/* Floating-Point Exception */
	  WSETSTOP ((*status), SIGILL);
	  break;
	case 77:		/* assert 77 */
	  WSETSTOP ((*status), SIGTRAP);
	  break;
	default:
	  WSETEXIT ((*status), 0);
	}
      break;
    case UDINotExecuting:
      WSETSTOP ((*status), SIGTERM);
      break;
    case UDIStopped:
      WSETSTOP ((*status), SIGTSTP);
      break;
    case UDIWarned:
      WSETSTOP ((*status), SIGLOST);
      break;
    case UDIStepped:
    case UDIBreak:
      WSETSTOP ((*status), SIGTRAP);
      break;
    case UDIWaiting:
      WSETSTOP ((*status), SIGSTOP);
      break;
    case UDIHalted:
      WSETSTOP ((*status), SIGKILL);
      break;
    case UDIExited:
    default:
      WSETEXIT ((*status), 0);
    }

  timeout = old_timeout;	/* Restore original timeout value */
  immediate_quit = old_immediate_quit;
  return 0;
}

/********************************************************** UDI_FETCH_REGISTERS
 * Read a remote register 'regno'. 
 * If regno==-1 then read all the registers.
 */
static void 
udi_fetch_registers (regno)
int	regno;
{
  UDIResource	From;
  UDIUInt32	*To;
  UDICount	Count;
  UDISizeT	Size = 4;
  UDICount	CountDone;
  UDIBool	HostEndian = 0;
  UDIError	err;
  int		i;

  if (regno >= 0)  {
    fetch_register(regno);
    return;
  }

/* Gr1/rsp */

  From.Space = UDI29KGlobalRegs;
  From.Offset = 1;
  To = (UDIUInt32 *)&registers[4 * GR1_REGNUM];
  Count = 1;
  if (err = UDIRead(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIRead() failed in udi_fetch_registers");

  register_valid[GR1_REGNUM] = 1;

#if defined(GR64_REGNUM)	/* Read gr64-127 */

/* Global Registers gr64-gr95 */ 

  From.Space = UDI29KGlobalRegs;
  From.Offset = 64;
  To = (UDIUInt32 *)&registers[4 * GR64_REGNUM];
  Count = 32;
  if (err = UDIRead(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIRead() failed in udi_fetch_registers");

  for (i = GR64_REGNUM; i < GR64_REGNUM + 32; i++)
    register_valid[i] = 1;

#endif	/*  GR64_REGNUM */

/* Global Registers gr96-gr127 */ 

  From.Space = UDI29KGlobalRegs;
  From.Offset = 96;
  To = (UDIUInt32 *)&registers[4 * GR96_REGNUM];
  Count = 32;
  if (err = UDIRead(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIRead() failed in udi_fetch_registers");

  for (i = GR96_REGNUM; i < GR96_REGNUM + 32; i++)
    register_valid[i] = 1;

/* Local Registers */

  From.Space = UDI29KLocalRegs;
  From.Offset = 0;
  To = (UDIUInt32 *)&registers[4 * LR0_REGNUM];
  Count = 128;
  if (err = UDIRead(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIRead() failed in udi_fetch_registers");

  for (i = LR0_REGNUM; i < LR0_REGNUM + 128; i++)
    register_valid[i] = 1;

/* Protected Special Registers */

  From.Space = UDI29KSpecialRegs;
  From.Offset = 0;
  To = (UDIUInt32 *)&registers[4 * SR_REGNUM(0)];
  Count = 15;
  if (err = UDIRead(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIRead() failed in udi_fetch_registers");

  for (i = SR_REGNUM(0); i < SR_REGNUM(0) + 15; i++)
    register_valid[i] = 1;

  if (USE_SHADOW_PC) {	/* Let regno_to_srnum() handle the register number */
    fetch_register(NPC_REGNUM);
    fetch_register(PC_REGNUM);
    fetch_register(PC2_REGNUM);

/* Unprotected Special Registers sr128-sr135 */

    From.Space = UDI29KSpecialRegs;
    From.Offset = 128;
    To = (UDIUInt32 *)&registers[4 * SR_REGNUM(128)];
    Count = 135-128 + 1;
    if (err = UDIRead(From, To, Count, Size, &CountDone, HostEndian))
      error("UDIRead() failed in udi_fetch_registers");

    for (i = SR_REGNUM(128); i < SR_REGNUM(128) + 135-128+1; i++)
      register_valid[i] = 1;
  }

  if (kiodebug)
    {
      printf("Fetching all registers\n");
      printf("Fetching PC0 = 0x%x, PC1 = 0x%x, PC2 = 0x%x\n",
	     read_register(NPC_REGNUM), read_register(PC_REGNUM),
	     read_register(PC2_REGNUM));
    }

  /* There doesn't seem to be any way to get these.  */
  {
    int val = -1;
    supply_register (FPE_REGNUM, (char *) &val);
    supply_register (INTE_REGNUM, (char *) &val);
    supply_register (FPS_REGNUM, (char *) &val);
    supply_register (EXO_REGNUM, (char *) &val);
  }
}


/********************************************************* UDI_STORE_REGISTERS
** Store register regno into the target.  
 * If regno==-1 then store all the registers.
 */

static void
udi_store_registers (regno)
int regno;
{
  UDIUInt32	*From;
  UDIResource	To;
  UDICount	Count;
  UDISizeT	Size = 4;
  UDICount	CountDone;
  UDIBool	HostEndian = 0;
  
  if (regno >= 0)
    {
      store_register(regno);
      return;
    }

  if (kiodebug)
    {
      printf("Storing all registers\n");
      printf("PC0 = 0x%x, PC1 = 0x%x, PC2 = 0x%x\n", read_register(NPC_REGNUM),
	     read_register(PC_REGNUM), read_register(PC2_REGNUM));
    }

/* Gr1/rsp */

  From = (UDIUInt32 *)&registers[4 * GR1_REGNUM];
  To.Space = UDI29KGlobalRegs;
  To.Offset = 1;
  Count = 1;
  if(UDIWrite(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIWrite() failed in udi_store_regisetrs");

#if defined(GR64_REGNUM)

/* Global registers gr64-gr95 */

  From = (UDIUInt32 *)&registers[4 * GR64_REGNUM];
  To.Space = UDI29KGlobalRegs;
  To.Offset = 64;
  Count = 32;
  if(UDIWrite(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIWrite() failed in udi_store_regisetrs");

#endif	/* GR64_REGNUM */

/* Global registers gr96-gr127 */

  From = (UDIUInt32 *)&registers[4 * GR96_REGNUM];
  To.Space = UDI29KGlobalRegs;
  To.Offset = 96;
  Count = 32;
  if(UDIWrite(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIWrite() failed in udi_store_regisetrs");

/* Local Registers */

  From = (UDIUInt32 *)&registers[4 * LR0_REGNUM];
  To.Space = UDI29KLocalRegs;
  To.Offset = 0;
  Count = 128;
  if(UDIWrite(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIWrite() failed in udi_store_regisetrs");


/* Protected Special Registers */ /* VAB through TMR */

  From = (UDIUInt32 *)&registers[4 * SR_REGNUM(0)];
  To.Space = UDI29KSpecialRegs;
  To.Offset = 0;
  Count = 10;
  if(UDIWrite(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIWrite() failed in udi_store_regisetrs");

/* PC0, PC1, PC2 possibly as shadow registers */

  From = (UDIUInt32 *)&registers[4 * SR_REGNUM(10)];
  To.Space = UDI29KSpecialRegs;
  Count = 3;
  if (USE_SHADOW_PC) 
    To.Offset = 20;				/* SPC0 */
  else 
    To.Offset = 10;				/* PC0 */
  if(UDIWrite(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIWrite() failed in udi_store_regisetrs");

  /* LRU and MMU */

  From = (UDIUInt32 *)&registers[4 * SR_REGNUM(13)];
  To.Space = UDI29KSpecialRegs;
  To.Offset = 13;
  Count = 2;
  if(UDIWrite(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIWrite() failed in udi_store_regisetrs");

/* Unprotected Special Registers */ 

  From = (UDIUInt32 *)&registers[4 * SR_REGNUM(128)];
  To.Space = UDI29KSpecialRegs;
  To.Offset = 128;
  Count = 135-128 +1;
  if(UDIWrite(From, To, Count, Size, &CountDone, HostEndian))
    error("UDIWrite() failed in udi_store_regisetrs");

  registers_changed ();
}

/****************************************************** UDI_PREPARE_TO_STORE */
/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

static void
udi_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

/********************************************************** TRANSLATE_ADDR */
static CORE_ADDR
translate_addr(addr)
CORE_ADDR addr;
{
#if defined(ULTRA3) && defined(KERNEL_DEBUGGING)
        /* Check for a virtual address in the kernel */
        /* Assume physical address of ublock is in  paddr_u register */
	/* FIXME: doesn't work for user virtual addresses */
        if (addr >= UVADDR) {
                /* PADDR_U register holds the physical address of the ublock */
                CORE_ADDR i = (CORE_ADDR)read_register(PADDR_U_REGNUM);
                return(i + addr - (CORE_ADDR)UVADDR);
        } else {
        	return(addr);
        }
#else
        return(addr);
#endif
}
/************************************************* UDI_XFER_INFERIOR_MEMORY */
/* FIXME!  Merge these two.  */
static int
udi_xfer_inferior_memory (memaddr, myaddr, len, write)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
{

  memaddr = translate_addr(memaddr);

  if (write)
    return udi_write_inferior_memory (memaddr, myaddr, len);
  else
    return udi_read_inferior_memory (memaddr, myaddr, len);
}

/********************************************************** UDI_FILES_INFO */
static void
udi_files_info ()
{
  printf ("\tAttached to UDI socket to %s and running program %s.\n",
          udi_config_id, prog_name);
}

/**************************************************** UDI_INSERT_BREAKPOINT */
static int
udi_insert_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  int cnt;
  UDIError err;

  for (cnt = 0; cnt < BKPT_TABLE_SIZE; cnt++)
    if (bkpt_table[cnt].Type == 0) /* Find first free slot */
      break;

  if(cnt >= BKPT_TABLE_SIZE)
    error("Too many breakpoints set");

  bkpt_table[cnt].Addr.Offset = addr;
  bkpt_table[cnt].Addr.Space  = UDI29KIRAMSpace;
  bkpt_table[cnt].PassCount = 1;
  bkpt_table[cnt].Type = UDIBreakFlagExecute;
  
  err = UDISetBreakpoint(bkpt_table[cnt].Addr,
			 bkpt_table[cnt].PassCount,
			 bkpt_table[cnt].Type,
			 &bkpt_table[cnt].BreakId);

  if (err == 0) return 0;		/* Success */

  bkpt_table[cnt].Type = 0;
  error("UDISetBreakpoint returned error code %d\n", err);
}

/**************************************************** UDI_REMOVE_BREAKPOINT */
static int
udi_remove_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  int cnt;
  UDIError err;

  for (cnt = 0; cnt < BKPT_TABLE_SIZE; cnt++)
    if (bkpt_table[cnt].Addr.Offset == addr) /* Find matching breakpoint */
      break;

  if(cnt >= BKPT_TABLE_SIZE)
    error("Can't find breakpoint in table");

  bkpt_table[cnt].Type = 0;

  err = UDIClearBreakpoint(bkpt_table[cnt].BreakId);
  if (err == 0) return 0;	/* Success */

  error("UDIClearBreakpoint returned error code %d\n", err);
}

static void
udi_kill(arg,from_tty)
     char *arg;
     int from_tty;
{

#if 0
/*
UDIStop does not really work as advertised.  It causes the TIP to close it's
connection, which usually results in GDB dying with a SIGPIPE.  For now, we
just invoke udi_close, which seems to get things right.
*/
  UDIStop();

  udi_session_id = -1;
  inferior_pid = 0;

  if (from_tty)
    printf("Target has been stopped.");
#else
  udi_close(0);
#endif
  pop_target();
}

/* 
   Load a program into the target.  Args are: `program {options}'.  The options
   are used to control loading of the program, and are NOT passed onto the
   loaded code as arguments.  (You need to use the `run' command to do that.)

   The options are:
 		-ms %d	Set mem stack size to %d
		-rs %d	Set regular stack size to %d
		-i	send init info (default)
		-noi	don't send init info
		-[tT]  	Load Text section
		-[dD]	Load Data section
		-[bB]	Load BSS section
		-[lL]	Load Lit section
  */

static void
download(load_arg_string, from_tty)
     char *load_arg_string;
     int from_tty;
{
#define DEFAULT_MEM_STACK_SIZE 		0x6000
#define DEFAULT_REG_STACK_SIZE 		0x2000

  char *token;
  char *filename;
  asection *section;
  bfd *pbfd;
  UDIError err;
  int load_text = 1, load_data = 1, load_bss = 1, load_lit = 1;

  address_ranges[0].Space = UDI29KIRAMSpace;
  address_ranges[0].Offset = 0xffffffff;
  address_ranges[0].Size = 0;

  address_ranges[1].Space = UDI29KDRAMSpace;
  address_ranges[1].Offset = 0xffffffff;
  address_ranges[1].Size = 0;

  stack_sizes[0] = DEFAULT_REG_STACK_SIZE;
  stack_sizes[1] = DEFAULT_MEM_STACK_SIZE;

  dont_repeat ();

  filename = strtok(load_arg_string, " \t");
  if (!filename)
    error ("Must specify at least a file name with the load command");

  filename = tilde_expand (filename);
  make_cleanup (free, filename);

  while (token = strtok (NULL, " \t"))
    {
      if (token[0] == '-')
	{
	  token++;

	  if (STREQ (token, "ms"))
	    stack_sizes[1] = atol (strtok (NULL, " \t"));
	  else if (STREQ (token, "rs"))
	    stack_sizes[0] = atol (strtok (NULL, " \t"));
	  else
	    {
	      load_text = load_data = load_bss = load_lit = 0;

	      while (*token)
		{
		  switch (*token++)
		    {
		    case 't':
		    case 'T':
		      load_text = 1;
		      break;
		    case 'd':
		    case 'D':
		      load_data = 1;
		      break;
		    case 'b':
		    case 'B':
		      load_bss = 1;
		      break;
		    case 'l':
		    case 'L':
		      load_lit = 1;
		      break;
		    default:
		      error ("Unknown UDI load option -%s", token-1);
		    }
		}
	    }
	}
    }

  pbfd = bfd_openr (filename, 0);

  if (!pbfd) 
    perror_with_name (filename);
  
  make_cleanup (bfd_close, pbfd);

  QUIT;
  immediate_quit++;

  if (!bfd_check_format (pbfd, bfd_object)) 
    error ("It doesn't seem to be an object file");
  
  for (section = pbfd->sections; section; section = section->next) 
    {
      if (bfd_get_section_flags (pbfd, section) & SEC_ALLOC)
	{
	  UDIResource To;
	  UDICount Count;
	  unsigned long section_size, section_end;
	  const char *section_name;

	  section_name = bfd_get_section_name (pbfd, section);
	  if (STREQ (section_name, ".text") && !load_text)
	    continue;
	  else if (STREQ (section_name, ".data") && !load_data)
	    continue;
	  else if (STREQ (section_name, ".bss") && !load_bss)
	    continue;
	  else if (STREQ (section_name, ".lit") && !load_lit)
	    continue;

	  To.Offset = bfd_get_section_vma (pbfd, section);
	  section_size = bfd_section_size (pbfd, section);
	  section_end = To.Offset + section_size;

	  printf("[Loading section %s at %x (%d bytes)]\n",
		 section_name,
		 To.Offset,
		 section_size);

	  if (bfd_get_section_flags (pbfd, section) & SEC_CODE)
	    {
	      To.Space = UDI29KIRAMSpace;

	      address_ranges[0].Offset = min (address_ranges[0].Offset,
					      To.Offset);
	      address_ranges[0].Size = max (address_ranges[0].Size,
					    section_end
					    - address_ranges[0].Offset);
	    }
	  else
	    {
	      To.Space = UDI29KDRAMSpace;

	      address_ranges[1].Offset = min (address_ranges[1].Offset,
					      To.Offset);
	      address_ranges[1].Size = max (address_ranges[1].Size,
					    section_end
					    - address_ranges[1].Offset);
	    }

	  if (bfd_get_section_flags (pbfd, section) & SEC_LOAD) /* Text, data or lit */
	    {
	      file_ptr fptr;

	      fptr = 0;

	      while (section_size > 0)
		{
		  char buffer[1024];

		  Count = min (section_size, 1024);

		  bfd_get_section_contents (pbfd, section, buffer, fptr,
					    Count);

		  err = UDIWrite ((UDIHostMemPtr)buffer, /* From */
				  To,			/* To */
				  Count,		/* Count */
				  (UDISizeT)1,		/* Size */
				  &Count,		/* CountDone */
				  (UDIBool)0);		/* HostEndian */
		  if (err)
		    error ("UDIWrite failed, error = %d", err);

		  To.Offset += Count;
		  fptr += Count;
		  section_size -= Count;
		}
	    }
	  else			/* BSS */
	    {
	      UDIResource From;
	      unsigned long zero = 0;

	      /* Write a zero byte at the vma */
	      err = UDIWrite ((UDIHostMemPtr)&zero,	/* From */
			      To,			/* To */
			      (UDICount)1,		/* Count */
			      (UDISizeT)4,		/* Size */
			      &Count,			/* CountDone */
			      (UDIBool)0);		/* HostEndian */
	      if (err)
		error ("UDIWrite failed, error = %d", err);

	      From = To;
	      To.Offset+=4;

	      /* Now, duplicate it for the length of the BSS */
	      err = UDICopy (From,			/* From */
			     To,			/* To */
			     (UDICount)(section_size/4 - 1), /* Count */
			     (UDISizeT)4,		/* Size */
			     &Count,			/* CountDone */
			     (UDIBool)1);		/* Direction */
	      if (err)
		{
		  char message[100];
		  int xerr;

		  xerr = UDIGetErrorMsg(err, 100, message, &Count);
		  if (!xerr)
		    fprintf (stderr, "Error is %s\n", message);
		  else
		    fprintf (stderr, "xerr is %d\n", xerr);
		  error ("UDICopy failed, error = %d", err);
		}
	    }

	}
    }

  entry.Space = UDI29KIRAMSpace;
  entry.Offset = bfd_get_start_address (pbfd);
  
  immediate_quit--;
}

/* User interface to download an image into the remote target.  See download()
 * for details on args.
 */

static void
udi_load(args, from_tty)
     char *args;
     int from_tty;
{
  download (args, from_tty);

  symbol_file_add (strtok (args, " \t"), from_tty, 0, 0, 0, 0);
}

/*************************************************** UDI_WRITE_INFERIOR_MEMORY
** Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.  Returns number of bytes written.  */
static int
udi_write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int		nwritten = 0;
  UDIUInt32	*From;
  UDIResource	To;
  UDICount	Count;
  UDISizeT	Size = 1;
  UDICount	CountDone = 0;
  UDIBool	HostEndian = 0;
  
  To.Space = udi_memory_space(memaddr);
  From = (UDIUInt32*)myaddr;

  while (nwritten < len)
  {	Count = len - nwritten;
	if (Count > MAXDATA) Count = MAXDATA;
  	To.Offset = memaddr + nwritten;
        if(UDIWrite(From, To, Count, Size, &CountDone, HostEndian))
    	{  error("UDIWrite() failed in udi_write_inferrior_memory");
	   break;	
	}
	else
  	{  nwritten += CountDone;
	   From += CountDone;
	}
  }
  return(nwritten);
}

/**************************************************** UDI_READ_INFERIOR_MEMORY
** Read LEN bytes from inferior memory at MEMADDR.  Put the result
   at debugger address MYADDR.  Returns number of bytes read.  */
static int
udi_read_inferior_memory(memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int		nread = 0;
  UDIResource	From;
  UDIUInt32	*To;
  UDICount	Count;
  UDISizeT	Size = 1;
  UDICount	CountDone = 0;
  UDIBool	HostEndian = 0;
  UDIError	err;
  
  From.Space = udi_memory_space(memaddr);	
  To = (UDIUInt32*)myaddr;

  while (nread < len)
  {	Count = len - nread;
	if (Count > MAXDATA) Count = MAXDATA;
  	From.Offset = memaddr + nread;
        if(err = UDIRead(From, To, Count, Size, &CountDone, HostEndian))
    	{  error("UDIRead() failed in udi_read_inferrior_memory");
	   break;	
	}
	else
  	{  nread += CountDone;
	   To += CountDone;
	}
  }
  return(nread);
}

/********************************************************************* WARNING
*/
udi_warning(num)
int	num;
{
    error ("ERROR while loading program into remote TIP: $d\n", num);
}


/*****************************************************************************/ 
/* Fetch a single register indicatated by 'regno'. 
 * Returns 0/-1 on success/failure.  
 */
static void
fetch_register (regno)
     int regno;
{
  UDIResource	From;
  UDIUInt32	To;
  UDICount	Count = 1;
  UDISizeT	Size = 4;
  UDICount	CountDone;
  UDIBool	HostEndian = 0;
  UDIError	err;
  int  		result;

  if (regno == GR1_REGNUM)
    {
      From.Space = UDI29KGlobalRegs;
      From.Offset = 1;
    }
  else if (regno >= GR96_REGNUM && regno < GR96_REGNUM + 32)
    {
      From.Space = UDI29KGlobalRegs;
      From.Offset = (regno - GR96_REGNUM) + 96;;
    }

#if defined(GR64_REGNUM)

  else if (regno >= GR64_REGNUM && regno < GR64_REGNUM + 32 )
    {
      From.Space = UDI29KGlobalRegs;
      From.Offset = (regno - GR64_REGNUM) + 64;
    }

#endif	/* GR64_REGNUM */

  else if (regno >= LR0_REGNUM && regno < LR0_REGNUM + 128)
    {
      From.Space = UDI29KLocalRegs;
      From.Offset = (regno - LR0_REGNUM);
    }
  else if (regno>=FPE_REGNUM && regno<=EXO_REGNUM)  
    {
      int val = -1;
      supply_register(160 + (regno - FPE_REGNUM),(char *) &val);
      return;		/* Pretend Success */
    }
  else 
    {
      From.Space = UDI29KSpecialRegs;
      From.Offset = regnum_to_srnum(regno); 
    }

  if (err = UDIRead(From, &To, Count, Size, &CountDone, HostEndian))
    error("UDIRead() failed in udi_fetch_registers");

  supply_register(regno, (char *) &To);

  if (kiodebug)
    printf("Fetching register %s = 0x%x\n", reg_names[regno], To);
}
/*****************************************************************************/ 
/* Store a single register indicated by 'regno'. 
 * Returns 0/-1 on success/failure.  
 */
static int
store_register (regno)
     int regno;
{
  int  		result;
  UDIUInt32	From;
  UDIResource	To;
  UDICount	Count = 1;
  UDISizeT	Size = 4;
  UDICount	CountDone;
  UDIBool	HostEndian = 0;

  From =  read_register (regno);	/* get data value */

  if (kiodebug)
    printf("Storing register %s = 0x%x\n", reg_names[regno], From);

  if (regno == GR1_REGNUM)
  { To.Space = UDI29KGlobalRegs;
    To.Offset = 1;
    result = UDIWrite(&From, To, Count, Size, &CountDone, HostEndian);
    /* Setting GR1 changes the numbers of all the locals, so invalidate the 
     * register cache.  Do this *after* calling read_register, because we want 
     * read_register to return the value that write_register has just stuffed 
     * into the registers array, not the value of the register fetched from 
     * the inferior.  
     */
    registers_changed ();
  }
#if defined(GR64_REGNUM)
  else if (regno >= GR64_REGNUM && regno < GR64_REGNUM + 32 )
  { To.Space = UDI29KGlobalRegs;
    To.Offset = (regno - GR64_REGNUM) + 64;
    result = UDIWrite(&From, To, Count, Size, &CountDone, HostEndian);
  }
#endif	/* GR64_REGNUM */
  else if (regno >= GR96_REGNUM && regno < GR96_REGNUM + 32)
  { To.Space = UDI29KGlobalRegs;
    To.Offset = (regno - GR96_REGNUM) + 96;
    result = UDIWrite(&From, To, Count, Size, &CountDone, HostEndian);
  }
  else if (regno >= LR0_REGNUM && regno < LR0_REGNUM + 128)
  { To.Space = UDI29KLocalRegs;
    To.Offset = (regno - LR0_REGNUM);
    result = UDIWrite(&From, To, Count, Size, &CountDone, HostEndian);
  }
  else if (regno>=FPE_REGNUM && regno<=EXO_REGNUM)  
  { 
    return 0;		/* Pretend Success */
  }
  else 	/* An unprotected or protected special register */
  { To.Space = UDI29KSpecialRegs;
    To.Offset = regnum_to_srnum(regno); 
    result = UDIWrite(&From, To, Count, Size, &CountDone, HostEndian);
  }

  if(result)
  { result = -1;
    error("UDIWrite() failed in store_registers");
  }
  return result;
}
/********************************************************** REGNUM_TO_SRNUM */
/* 
 * Convert a gdb special register number to a 29000 special register number.
 */
static int
regnum_to_srnum(regno)
int	regno;
{
	switch(regno) {
		case VAB_REGNUM: return(0); 
		case OPS_REGNUM: return(1); 
		case CPS_REGNUM: return(2); 
		case CFG_REGNUM: return(3); 
		case CHA_REGNUM: return(4); 
		case CHD_REGNUM: return(5); 
		case CHC_REGNUM: return(6); 
		case RBP_REGNUM: return(7); 
		case TMC_REGNUM: return(8); 
		case TMR_REGNUM: return(9); 
		case NPC_REGNUM: return(USE_SHADOW_PC ? (20) : (10));
		case PC_REGNUM:  return(USE_SHADOW_PC ? (21) : (11));
		case PC2_REGNUM: return(USE_SHADOW_PC ? (22) : (12));
		case MMU_REGNUM: return(13); 
		case LRU_REGNUM: return(14); 
		case IPC_REGNUM: return(128); 
		case IPA_REGNUM: return(129); 
		case IPB_REGNUM: return(130); 
		case Q_REGNUM:   return(131); 
		case ALU_REGNUM: return(132); 
		case BP_REGNUM:  return(133); 
		case FC_REGNUM:  return(134); 
		case CR_REGNUM:  return(135); 
		case FPE_REGNUM: return(160); 
		case INTE_REGNUM: return(161); 
		case FPS_REGNUM: return(162); 
		case EXO_REGNUM:return(164); 
		default:
			return(255);	/* Failure ? */
	}
}
/****************************************************************************/
/*
 * Determine the Target memory space qualifier based on the addr. 
 * FIXME: Can't distinguis I_ROM/D_ROM.  
 * FIXME: Doesn't know anything about I_CACHE/D_CACHE.
 */
static CPUSpace
udi_memory_space(addr)
CORE_ADDR	addr;
{
	UDIUInt32 tstart = IMemStart;
	UDIUInt32 tend   = tstart + IMemSize;  
	UDIUInt32 dstart = DMemStart;
	UDIUInt32 dend   = tstart + DMemSize;  
	UDIUInt32 rstart = RMemStart;
	UDIUInt32 rend   = tstart + RMemSize;  

	if (((UDIUInt32)addr >= tstart) && ((UDIUInt32)addr < tend)) { 
		return UDI29KIRAMSpace;
	} else if (((UDIUInt32)addr >= dstart) && ((UDIUInt32)addr < dend)) { 
		return UDI29KDRAMSpace;
	} else if (((UDIUInt32)addr >= rstart) && ((UDIUInt32)addr < rend)) {
		/* FIXME: how do we determine between D_ROM and I_ROM */
		return UDI29KIROMSpace;
	} else	/* FIXME: what do me do now? */
		return UDI29KDRAMSpace;	/* Hmmm! */
}
/*********************************************************************** STUBS
*/

void  convert16() {;}
void  convert32() {;}
FILE* EchoFile = 0;		/* used for debugging */
int   QuietMode = 0;		/* used for debugging */

/****************************************************************************/
/* 
 *  Define the target subroutine names 
 */
static struct target_ops udi_ops = {
        "udi",
	"Remote UDI connected TIP",
	"Remote debug an AMD 29k using UDI socket connection to TIP process",
        udi_open,
	udi_close,
        udi_attach,
	udi_detach,
	udi_resume,
	udi_wait,
        udi_fetch_registers,
	udi_store_registers,
        udi_prepare_to_store,
        udi_xfer_inferior_memory,
        udi_files_info,
        udi_insert_breakpoint,
	udi_remove_breakpoint,
        0,			/* termial_init */
	0,			/* terminal_inferior */
	0,			/* terminal_ours_for_output */
	0,			/* terminal_ours */
	0,			/* terminal_info */
        udi_kill,             	/* FIXME, kill */
        udi_load,
        0,                      /* lookup_symbol */
        udi_create_inferior,
        udi_mourn,		/* mourn_inferior FIXME */
	0,			/* can_run */
	0,			/* notice_signals */
        process_stratum,
	0,			/* next */
        1,			/* has_all_memory */
	1,			/* has_memory */
	1,			/* has_stack */
	1,			/* has_registers */
	1,			/* has_execution */
	0,			/* sections */
	0,			/* sections_end */
	OPS_MAGIC,		/* Always the last thing */
};

void _initialize_remote_udi()
{
  add_target (&udi_ops);
  add_show_from_set (
		     add_set_cmd ("remotedebug", no_class, var_boolean,
				  (char *)&kiodebug,
				  "Set debugging of UDI I/O.\n\
When enabled, debugging info is displayed.",
				  &setlist),
		     &showlist);
}

#ifdef NO_HIF_SUPPORT
service_HIF(msg)
union msg_t	*msg;
{
	return(0);	/* Emulate a failure */
}
#endif
