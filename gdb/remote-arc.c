/* Remote target communications for serial-line targets in custom GDB protocol
   Copyright 1988, 1991, 1992, 1993, 1994 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "gdb_string.h"
#include <fcntl.h>
#include "frame.h"
#include "inferior.h"
#include "bfd.h"
#include "symfile.h"
#include "target.h"
#include "wait.h"
#include "terminal.h"
#include "gdbcmd.h"
#include "objfiles.h"
#include "gdb-stabs.h"

#include "dcache.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <signal.h>
#include "serial.h"

/* Prototypes for local functions */

static int
arc_write_bytes PARAMS ((CORE_ADDR memaddr, unsigned char *myaddr, int len));

static int
arc_read_bytes PARAMS ((CORE_ADDR memaddr, unsigned char *myaddr, int len));

static void
arc_files_info PARAMS ((struct target_ops *ignore));

static int
arc_xfer_memory PARAMS ((CORE_ADDR memaddr, char *myaddr, int len,
			    int should_write, struct target_ops *target));

static void
arc_xfer_cntlreg PARAMS ((int rw, unsigned char *data));

static void 
arc_prepare_to_store PARAMS ((void));

static void
arc_fetch_registers PARAMS ((int regno));

static void
arc_resume PARAMS ((int pid, int step, enum target_signal siggnal));

static int
arc_start_remote PARAMS ((char *dummy));

static void
arc_open PARAMS ((char *name, int from_tty));

static void
arc_close PARAMS ((int quitting));

static void
arc_store_registers PARAMS ((int regno));

static void
getpkt PARAMS ((char *buf, int len));

static int
putpkt PARAMS ((char *buf, int len));

static int arc_wait PARAMS ((int pid, struct target_waitstatus *status));

static void
arc_detach PARAMS ((char *args, int from_tty));

static void
arc_interrupt PARAMS ((int signo));

static void
arc_interrupt_twice PARAMS ((int signo));

static void
interrupt_query PARAMS ((void));

extern struct target_ops arc_ops;	/* Forward decl */

static int aux_reg_map[3][31] = AUX_REG_MAP;

/* This was 5 seconds, which is a long time to sit and wait.
   Unless this is going though some terminal server or multiplexer or
   other form of hairy serial connection, I would think 2 seconds would
   be plenty.  */
static int remote_timeout = 2;

#if 0
int icache;
#endif

/* Descriptor for I/O to remote machine.  Initialize it to NULL so that
   arc_open knows that we don't have a file open when the program
   starts.  */
static serial_t arc_desc = NULL;

#define AUDIO_PROCESSOR   0
#define GRAPHIC_PROCESSOR 1
#define HOST_PROCESSOR    2
static unsigned char cntl_reg_halt_bit[3] = { 0x08, 0x10, 0x20 };
static unsigned char cntl_reg_step_bit[3] = { 0x01, 0x02, 0x04 };

static int curr_processor = HOST_PROCESSOR;
static unsigned char cntl_reg = 0;
static unsigned int status_reg = 0;

#define	PBUFSIZ	32
#define MAXBUFBYTES 32


/* Clean up connection to a remote debugger.  */

/* ARGSUSED */
static void
arc_close (quitting)
     int quitting;
{
  if (arc_desc)
    SERIAL_CLOSE (arc_desc);
  arc_desc = NULL;
}

static int
arc_start_remote (dummy)
     char *dummy;
{
  immediate_quit = 1;		/* Allow user to interrupt it */
  arc_xfer_cntlreg (1, &cntl_reg);
  immediate_quit = 0;

  start_remote ();		/* Initialize gdb process mechanisms */
  return 1;
}

/* Open a connection to a remote debugger.
   NAME is the filename used for communication.  */

static DCACHE *remote_dcache;

static void
arc_open (name, from_tty)
     char *name;
     int from_tty;
{
  if (name == 0)
    error (
"To open a arc debug connection, you need to specify what parallel\n\
device is attached to the remote system.");

  target_preopen (from_tty);

  unpush_target (&arc_ops);

  remote_dcache = dcache_init (arc_read_bytes, arc_write_bytes);

  arc_desc = SERIAL_OPEN (name);
  if (!arc_desc)
    perror_with_name (name);

  /* If there is something sitting in the buffer we might take it as a
     response to a command, which would be bad.  */
  SERIAL_FLUSH_INPUT (arc_desc);

  if (from_tty)
    {
      puts_filtered ("Remote debugging using ");
      puts_filtered (name);
      puts_filtered ("\n");
    }
  push_target (&arc_ops);	/* Switch to using remote target now */

  /* Without this, some commands which require an active target (such as kill)
     won't work.  This variable serves (at least) double duty as both the pid
     of the target process (if it has such), and as a flag indicating that a
     target is active.  These functions should be split out into seperate
     variables, especially since GDB will someday have a notion of debugging
     several processes.  */

  inferior_pid = 42000;

  /* Start the remote connection; if error (0), discard this target.
     In particular, if the user quits, be sure to discard it
     (we'd be in an inconsistent state otherwise).  */
  if (!catch_errors (arc_start_remote, (char *)0, 
	"Couldn't establish connection to remote target\n", RETURN_MASK_ALL))
    pop_target();
}

/* arc_detach()
   takes a program previously attached to and detaches it.
   We better not have left any breakpoints
   in the program or it'll die when it hits one.
   Close the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */

static void
arc_detach (args, from_tty)
     char *args;
     int from_tty;
{
  if (args)
    error ("Argument given to \"detach\" when remotely debugging.");
  
  pop_target ();
  if (from_tty)
    puts_filtered ("Ending remote debugging.\n");
}

/* arc_set_addrreg
   set addr reg from debug system. */

static void
arc_set_addrreg (addr)
     unsigned int addr;
{
  unsigned char buf[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

  buf[0] = 0x0;
  buf[4] = addr & 0xff;
  buf[3] = (addr >> 8) & 0xff;
  buf[2] = (addr >> 16) & 0xff;
  buf[1] = (addr >> 24) & 0xff;

  putpkt (buf, 5);
}

/* arc_xfer_datareg
   read or write data reg from debug system. */

static void
arc_xfer_datareg (rw, aux, incr, data)
     int rw;
     int aux;
     int incr;
     unsigned int *data;
{
  unsigned char buf1[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
  unsigned char buf2[5];
  unsigned int tmp;

  /* read or write data */
  buf1[0] = (aux ? 0x81 : 0x41);
  if (incr)
    buf1[0] += 0x20;
  if (rw)    /* read */
    {
      buf1[0] += 0x10;
      putpkt (buf1, 1);
      getpkt (buf2, 4);
      *data = *(int *)buf2;
    }
  else
    {
      tmp = *data;
      buf1[4] = tmp & 0xff;
      buf1[3] = (tmp >> 8) & 0xff;
      buf1[2] = (tmp >> 16) & 0xff;
      buf1[1] = (tmp >> 24) & 0xff;
      putpkt (buf1, 5);
    }
}

/* arc_xfer_cntlreg
   read or write control reg from debug system. */

static void
arc_xfer_cntlreg (rw, data)
     int rw;
     unsigned char *data;
{
  unsigned char buf[3] = {0x0, 0x0, 0x0};

  if (rw)
    {
      buf[0] = 0x12;
      putpkt (buf, 1);
      getpkt (data, 1);
    }
  else
    {
      buf[0] = 0x02;
      buf[1] = *data;
      putpkt (buf, 2);
    }
}

/* arc_xfer_reg
   read or write a reg to arc processors. */

static void
arc_xfer_reg (processor, rw, aux, regnum, data)
     int processor;
     int rw;
     int aux;
     int regnum;
     unsigned int *data;
{
  unsigned int tmp;

  if (processor == HOST_PROCESSOR)
    {
      /* write addr (regnum) */
      arc_set_addrreg (regnum);
      arc_xfer_datareg (rw, aux, 0, data);
    }
  else
    {
      /* write addr register (aux r14) */
      arc_set_addrreg (0xe);
      tmp = aux ? (regnum | 0x80000000) : regnum;
      arc_xfer_datareg (0, 1, 0, tmp);

      /* read/write from data reg (aux reg 15/16) */
      arc_set_addrreg (processor == AUDIO_PROCESSOR ? 0x10 : 0xf);
      arc_xfer_datareg (rw, 1, 0, data);
    }
}


/* Tell the remote machine to resume.  */

static enum target_signal last_sent_signal = TARGET_SIGNAL_0;
int last_sent_step;

static void
arc_resume (pid, step, siggnal)
     int pid, step;
     enum target_signal siggnal;
{
  unsigned int tmp;

  dcache_flush (remote_dcache);

  last_sent_signal = siggnal;
  last_sent_step = step;

  /* don't know how to handle signal in ARC ***
  if (siggnal != TARGET_SIGNAL_0)
    {
      buf[0] = step ? 'S' : 'C';
      buf[1] = tohex (((int)siggnal >> 4) & 0xf);
      buf[2] = tohex ((int)siggnal & 0xf);
      buf[3] = '\0';
    }
  */

  if (step)
    {
      /* write the step bit in control reg of debug system */
      unsigned char tmp_char = cntl_reg | cntl_reg_step_bit[curr_processor];
      arc_xfer_cntlreg (0, &tmp_char);
    }
  else
    {
      /* clear the halt bit in the status register */
      tmp = status_reg | 0x02000000;
      arc_xfer_reg (curr_processor, 0, 1, 0, &tmp);
    }
}


static void (*ofunc)();

/* Send to target to halt it. */
static void
arc_interrupt (signo)
     int signo;
{
  unsigned char buf[3] = {0x02, 0x0, 0x0};
  /* If this doesn't work, try more severe steps.  */
  signal (signo, arc_interrupt_twice);
  
  if (remote_debug)
    printf_unfiltered ("arc_interrupt called\n");

  if (cntl_reg & cntl_reg_halt_bit[curr_processor]) 
    return;

  buf[1] = cntl_reg | cntl_reg_halt_bit[curr_processor];
  putpkt (buf, 2);
  return;  
}

/* The user typed ^C twice.  */
static void
arc_interrupt_twice (signo)
     int signo;
{
  signal (signo, ofunc);
  
  interrupt_query ();

  signal (signo, arc_interrupt);
}

/* Ask the user what to do when an interrupt is received.  */

static void
interrupt_query ()
{
  target_terminal_ours ();

  if (query ("Interrupted while waiting for the program.\n\
Give up (and stop debugging it)? "))
    {
      target_mourn_inferior ();
      return_to_top_level (RETURN_QUIT);
    }

  target_terminal_inferior ();
}

/* If nonzero, ignore the next kill.  */
int kill_kludge;

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.
   Returns "pid" (though it's not clear what, if anything, that
   means in the case of this target).  */

static int
arc_wait (pid, status)
     int pid;
     struct target_waitstatus *status;
{
  unsigned char buf[PBUFSIZ];
  int thread_num = -1;
  unsigned char cmd;
  int proc;

  status->kind = TARGET_WAITKIND_EXITED;
  status->value.integer = 0;

  while (1)
    {
      unsigned char *p;

      ofunc = (void (*)()) signal (SIGINT, arc_interrupt);
      arc_xfer_cntlreg (1, &cntl_reg);
      signal (SIGINT, ofunc);
      if (cntl_reg & cntl_reg_halt_bit[curr_processor])
	break;
      status->kind = TARGET_WAITKIND_STOPPED;
    }

  if ((curr_processor != HOST_PROCESSOR) && 
      !(cntl_reg & cntl_reg_halt_bit[HOST_PROCESSOR]))
    {
      cmd = cntl_reg | cntl_reg_halt_bit[HOST_PROCESSOR];
      arc_xfer_cntlreg (0, &cmd);
      while (1)
	{
	  unsigned char *p;
	  
	  ofunc = (void (*)()) signal (SIGINT, arc_interrupt);
	  arc_xfer_cntlreg (1, &cntl_reg);
	  signal (SIGINT, ofunc);
	  if (cntl_reg & cntl_reg_halt_bit[HOST_PROCESSOR])
	    break;
	}
    }

  for (proc = AUDIO_PROCESSOR ; proc <= GRAPHIC_PROCESSOR; proc++)
    {
      if ((cntl_reg & cntl_reg_halt_bit[proc]))
	continue;
      cmd = cntl_reg | cntl_reg_halt_bit[proc];
      arc_xfer_cntlreg (0, &cmd);
    }

  arc_xfer_reg (curr_processor, 1, 1, 0, &status_reg);
  return inferior_pid;
}

/* Number of bytes of registers this implements.  */
static int register_bytes_found;

/* Read the remote registers into the block REGS.  */
/* Currently we just read all the registers, so we don't use regno.  */

static void
arc_fetch_registers (regno)
     int regno;
{
  int i;
  char regs[REGISTER_BYTES];

  /* Unimplemented registers read as all bits zero.  */
  memset (regs, 0, REGISTER_BYTES);

  /* get core register */
  arc_set_addrreg (0);
  for (i = 0; i < AUX_BEG_REGNUM; i++)
    {
      if (curr_processor == HOST_PROCESSOR)
	arc_xfer_datareg (1, 0, 1, &regs[REGISTER_BYTE(i)]);
      else
	arc_xfer_reg (curr_processor, 1, 0, regno, &regs[REGISTER_BYTE(i)]);
    }

  /* get aux register */
  for (i = AUX_BEG_REGNUM; i < AUX_END_REGNUM; i++)
    {
      int auxregnum = aux_reg_map[curr_processor][i-AUX_BEG_REGNUM+1];
      if (auxregnum == -1)
	continue;
      arc_xfer_reg (curr_processor, 1, 1, auxregnum, &regs[REGISTER_BYTE(i)]);
    }
  /* copy from status register to pc */
  for (i = 1; i <= 3; i++)
    registers[REGISTER_BYTE (PC_REGNUM)+i] = 
      registers[REGISTER_BYTE (STA_REGNUM)+i];

/*
  if (i != register_bytes_found)
    {
      register_bytes_found = i;
      if (!REGISTER_BYTES_OK (i))
	warning ("Remote reply is too short: %s", buf);
    }
*/

  for (i = 0; i < NUM_REGS; i++)
    supply_register (i, &regs[REGISTER_BYTE(i)]);
}

/* Prepare to store registers.  Since we may send them all,
   we have to read out the ones we don't want to change first.  */

static void 
arc_prepare_to_store ()
{
  /* Make sure the entire registers array is valid.  */
  read_register_bytes (0, (char *)NULL, REGISTER_BYTES);
}

/* Store register REGNO, or all registers if REGNO == -1, from the contents
   of REGISTERS.  FIXME: ignores errors.  */

static void
arc_store_registers (regno)
     int regno;
{
  int i;
  char *regp;

  /* Try storing a single register.  */
  if (regno >= 0)
    {
      int isaux = (regno >= AUX_BEG_REGNUM ? 1 : 0);

      regp = &registers[REGISTER_BYTE (regno)];
      arc_xfer_reg (curr_processor, 0, isaux, regno, regp);
    }

  /* store core regs */
  arc_set_addrreg (0);
  for (i = 0; i < AUX_BEG_REGNUM; i++)
    {
      regp = &registers[REGISTER_BYTE (i)];
      if (curr_processor == HOST_PROCESSOR)
	arc_xfer_datareg (0, 0, 1, regp);
      else
	arc_xfer_reg (curr_processor, 0, 0, regno, regp);
    }

  /* store aux regs */
  /* copy pc back to status register */
  for (i = 1; i <= 3; i++)
    registers[REGISTER_BYTE (STA_REGNUM)+i] = 
      registers[REGISTER_BYTE (PC_REGNUM)+i];
  for (i = AUX_BEG_REGNUM; i <= AUX_END_REGNUM; i++)
    {
      int auxregnum = aux_reg_map[curr_processor][i-AUX_BEG_REGNUM+1];
      if (auxregnum == -1)
	continue;
      regp = &registers[REGISTER_BYTE (i)];
      arc_xfer_reg (curr_processor, 0, 1, auxregnum, regp);
    }

}

#if 0
/* Use of the data cache is disabled because it loses for looking at
   and changing hardware I/O ports and the like.  Accepting `voltile'
   would perhaps be one way to fix it, but a better way which would
   win for more cases would be to use the executable file for the text
   segment, like the `icache' code below but done cleanly (in some
   target-independent place, perhaps in target_xfer_memory, perhaps
   based on assigning each target a speed or perhaps by some simpler
   mechanism).  */

/* Read a word from remote address ADDR and return it.
   This goes through the data cache.  */

static int
arc_fetch_word (addr)
     CORE_ADDR addr;
{
#if 0
  if (icache)
    {
      extern CORE_ADDR text_start, text_end;

      if (addr >= text_start && addr < text_end)
	{
	  int buffer;
	  xfer_core_file (addr, &buffer, sizeof (int));
	  return buffer;
	}
    }
#endif
  return dcache_fetch (remote_dcache, addr);
}

/* Write a word WORD into remote address ADDR.
   This goes through the data cache.  */

static void
arc_store_word (addr, word)
     CORE_ADDR addr;
     int word;
{
  dcache_poke (remote_dcache, addr, word);
}
#endif /* 0 */


/* Write memory data directly to the remote machine.
   This does not inform the data cache; the data cache uses this.
   MEMADDR is the address in the remote memory space.
   MYADDR is the address of the buffer in our space.
   LEN is the number of bytes.

   Returns number of bytes transferred, or 0 for error.  */

static int
arc_write_bytes (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  char buf1[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
  char buf2[6] = {0x21, 0x0, 0x0, 0x0, 0x0, 0x0};
  int i;
  char *p;

  /* We send target system values byte by byte, in increasing byte addresses*/

  buf1[4] = memaddr & 0xff;
  buf1[3] = (memaddr >> 8) & 0xff;
  buf1[2] = (memaddr >> 16) & 0xff;
  buf1[1] = (memaddr >> 24) & 0xff;
  putpkt (buf1, 5);

  for (i = 0; i < len; )
    {
      buf2[1] = myaddr[i++];
      buf2[2] = myaddr[i++];
      buf2[3] = myaddr[i++];
      buf2[4] = myaddr[i++];
      putpkt (buf2, 5);
    }

  return len;
}

/* Read memory data directly from the remote machine.
   This does not use the data cache; the data cache uses this.
   MEMADDR is the address in the remote memory space.
   MYADDR is the address of the buffer in our space.
   LEN is the number of bytes.

   Returns number of bytes transferred, or 0 for error.  */

static int
arc_read_bytes (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  unsigned char buf1[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
  unsigned char buf2[3] = {0x31, 0x0, 0x0};
  unsigned char buf3[5];
  int i;
  char *p;

  /* We send target system values byte by byte, in increasing byte addresses*/

  buf1[4] = memaddr & 0xff;
  buf1[3] = (memaddr >> 8) & 0xff;
  buf1[2] = (memaddr >> 16) & 0xff;
  buf1[1] = (memaddr >> 24) & 0xff;
  putpkt (buf1, 5);

  for (i = 0; i < len; )
    {
      putpkt (buf2, 2);
      getpkt (buf3, 4);
      myaddr[i++] = buf3[1];
      myaddr[i++] = buf3[2];
      myaddr[i++] = buf3[3];
      myaddr[i++] = buf3[4];
    }

  return i;
}


/* Read or write LEN bytes from inferior memory at MEMADDR, transferring
   to or from debugger address MYADDR.  Write to inferior if SHOULD_WRITE is
   nonzero.  Returns length of data written or read; 0 for error.  */

/* ARGSUSED */
static int
arc_xfer_memory(memaddr, myaddr, len, should_write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int should_write;
     struct target_ops *target;			/* ignored */
{
  int xfersize;
  int bytes_xferred;
  int total_xferred = 0;

  while (len > 0)
    {
      if (len > MAXBUFBYTES)
	xfersize = MAXBUFBYTES;
      else
	xfersize = len;

      if (should_write)
        bytes_xferred = arc_write_bytes (memaddr,
					    (unsigned char *)myaddr, xfersize);
      else
	bytes_xferred = arc_read_bytes (memaddr,
					   (unsigned char *)myaddr, xfersize);

      /* If we get an error, we are done xferring.  */
      if (bytes_xferred == 0)
	break;

      memaddr += bytes_xferred;
      myaddr  += bytes_xferred;
      len     -= bytes_xferred;
      total_xferred += bytes_xferred;
    }
  return total_xferred;
}


static void
arc_files_info (ignore)
     struct target_ops *ignore;
{
  puts_filtered ("Debugging a target over a serial line.\n");
}


/* Read a single character from the remote end, masking it down to 7 bits. */
static int
readchar ()
{
  int ch;

  ch = SERIAL_READCHAR (arc_desc, 0);

  switch (ch)
    {
    case SERIAL_EOF:
      error ("Remote connection closed");
    case SERIAL_ERROR:
      perror_with_name ("Remote communication error");
    case SERIAL_TIMEOUT:
      return ch;
    default:
      return ch & 0x7f;
    }
}

/* Send a packet to the remote machine, with error checking.
   The data of the packet is in BUF.  */

static int
putpkt (buf, len)
     char *buf;
     int len;
{
  int i = 0;
  unsigned char csum = 0;

  while (i < len)
    {
      if (remote_debug)
	{
	  printf_unfiltered ("Sending packet: %s...", buf);
	  gdb_flush(gdb_stdout);
	}
      if (SERIAL_WRITE (arc_desc, buf, i))
	perror_with_name ("putpkt: write failed");
      i++;

#if 0
      /* This is wrong.  If doing a long backtrace, the user should be
	 able to get out next time we call QUIT, without anything as violent
	 as interrupt_query.  If we want to provide a way out of here
	 without getting to the next QUIT, it should be based on hitting
	 ^C twice as in arc_wait.  */
      if (quit_flag)
	{
	  quit_flag = 0;
	  interrupt_query ();
	}
#endif
    }
}

/* Read a packet from the remote machine, with error checking,
   and store it in BUF.  BUF is expected to be of size PBUFSIZ.
   If FOREVER, wait forever rather than timing out; this is used
   while the target is executing user code.  */

static void
getpkt (buf, len)
     char *buf;
     int len;
{
  int c;
  int i;

  for (i = 0; i < len; i++)
    {
      c = readchar ();
      if (c == SERIAL_ERROR)
	{
	  if (remote_debug)
	    puts_filtered ("Read error.\n");
	}
    }
  buf[i] = '\0';

  if (remote_debug)
    fprintf_unfiltered (gdb_stderr, "Packet received: %s\n", buf);
  if (i != len)
    printf_unfiltered ("Ignoring packet error, continuing...\n");
}

static void
arc_kill ()
{
  /* For some mysterious reason, wait_for_inferior calls kill instead of
     mourn after it gets TARGET_WAITKIND_SIGNALLED.  Work around it.  */
  if (kill_kludge)
    {
      kill_kludge = 0;
      target_mourn_inferior ();
      return;
    }
  target_mourn_inferior ();
}

static void
arc_mourn ()
{
  unpush_target (&arc_ops);
  generic_mourn_inferior ();
}



static unsigned char big_break_insn[] = BIG_BREAKPOINT;
static unsigned char little_break_insn[] = LITTLE_BREAKPOINT;
#define BREAKPOINT_LEN (sizeof little_break_insn)

/* Insert a breakpoint on targets that don't have any better breakpoint
   support.  We read the contents of the target location and stash it,
   then overwrite it with a breakpoint instruction.  ADDR is the target
   location in the target machine.  CONTENTS_CACHE is a pointer to 
   memory allocated for saving the target contents.  It is guaranteed
   by the caller to be long enough to save sizeof BREAKPOINT bytes (this
   is accomplished via BREAKPOINT_MAX).  */

static int
arc_insert_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  int val;

  val = target_read_memory (addr, contents_cache, BREAKPOINT_LEN);
  if (val == 0)
    {
      if (TARGET_BYTE_ORDER == BIG_ENDIAN)
	val = target_write_memory (addr, (char *) big_break_insn,
				   BREAKPOINT_LEN);
      else
	val = target_write_memory (addr, (char *) little_break_insn,
				   BREAKPOINT_LEN);
    }
  return val;
}

static int
arc_remove_breakpoint (addr, contents_cache)
     CORE_ADDR addr;
     char *contents_cache;
{
  return target_write_memory (addr, contents_cache, BREAKPOINT_LEN);
}

/* switch_command
   support 'switch' command to switch among the three processors of ARC. */

static void
switch_command (args, fromtty)
     char *args;
     int fromtty;
{
  struct target_waitstatus status;
  int proc;

  if (strncmp (args, "audio", 3) == 0)
    proc = 0;
  else if (strncmp (args, "graphic", 3) == 0)
    proc = 1;
  else if (strncmp (args, "host", 4) == 0)
    proc = 2;

  curr_processor = proc;
  
  switch (proc)
    {
    case 0:
      tm_print_insn = arc_get_disassembler (bfd_mach_arc_audio,
					    TARGET_BYTE_ORDER == BIG_ENDIAN);
      break;
    case 1:
      tm_print_insn = arc_get_disassembler (bfd_mach_arc_graphics,
					    TARGET_BYTE_ORDER == BIG_ENDIAN);
      break;
    case 2:
      tm_print_insn = arc_get_disassembler (bfd_mach_arc_host,
					    TARGET_BYTE_ORDER == BIG_ENDIAN);
      break;
    }

  return;
}


/* Define the target subroutine names */

struct target_ops arc_ops = {
  "arc",			/* to_shortname */
  "Remote target in arc-specific protocol",	/* to_longname */
  "Use a remote computer via a parallel line, using a arc-specific protocol.\n\
Specify the device it is connected to.",  /* to_doc */
  arc_open,			/* to_open */
  arc_close,			/* to_close */
  NULL,				/* to_attach */
  arc_detach,		/* to_detach */
  arc_resume,		/* to_resume */
  arc_wait,			/* to_wait */
  arc_fetch_registers,	/* to_fetch_registers */
  arc_store_registers,	/* to_store_registers */
  arc_prepare_to_store,	/* to_prepare_to_store */
  arc_xfer_memory,		/* to_xfer_memory */
  arc_files_info,		/* to_files_info */

  arc_insert_breakpoint,	/* to_insert_breakpoint */
  arc_remove_breakpoint,	/* to_remove_breakpoint */

  NULL,				/* to_terminal_init */
  NULL,				/* to_terminal_inferior */
  NULL,				/* to_terminal_ours_for_output */
  NULL,				/* to_terminal_ours */
  NULL,				/* to_terminal_info */
  arc_kill,			/* to_kill */
  generic_load,			/* to_load */
  NULL,				/* to_lookup_symbol */
  NULL,				/* to_create_inferior */
  arc_mourn,			/* to_mourn_inferior */
  0,				/* to_can_run */
  0,				/* to_notice_signals */
  0,				/* to_thread_alive */
  0,				/* to_stop */
  process_stratum,		/* to_stratum */
  NULL,				/* to_next */
  1,				/* to_has_all_memory */
  1,				/* to_has_memory */
  1,				/* to_has_stack */
  1,				/* to_has_registers */
  1,				/* to_has_execution */
  NULL,				/* sections */
  NULL,				/* sections_end */
  OPS_MAGIC			/* to_magic */
};

void
_initialize_remote_arc ()
{
  add_target (&arc_ops);
}
