/* Remote debugging interface for Motorola's PPCBUG monitor, an embedded
   monitor for the PowerPC.

   Copyright 1995 Free Software Foundation, Inc.
   Contributed by Cygnus Support.  Written by Michael Meissner
   (cloned from remote-bug.c by K. Richard Pixley.)

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
#include "inferior.h"
#include "wait.h"

#include "gdb_string.h"
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>

#include "terminal.h"
#include "gdbcore.h"
#include "gdbcmd.h"

#include "remote-utils.h"

/* External data declarations */
extern int stop_soon_quietly;	/* for wait_for_inferior */

/* Forward data declarations */
extern struct target_ops ppcbug_ops;	/* Forward declaration */

/* Forward function declarations */
static int ppcbug_clear_breakpoints PARAMS((void));

static int ppcbug_read_memory PARAMS((CORE_ADDR memaddr,
				      char *myaddr,
				      int len));

static int ppcbug_write_memory PARAMS((CORE_ADDR memaddr,
				       char *myaddr,
				       int len));

/* This variable is somewhat arbitrary.  It's here so that it can be
   set from within a running gdb.  */

static int srec_max_retries = 3;

/* Each S-record download to the target consists of an S0 header
   record, some number of S3 data records, and one S7 termination
   record.  I call this download a "frame".  Srec_frame says how many
   bytes will be represented in each frame.  */

#define SREC_SIZE 160
static int srec_frame = SREC_SIZE;

/* This variable determines how many bytes will be represented in each
   S3 s-record.  */

static int srec_bytes = 40;

/* At one point it appeared to me as though the bug monitor could not
   really be expected to receive two sequential characters at 9600
   baud reliably.  Echo-pacing is an attempt to force data across the
   line even in this condition.  Specifically, in echo-pace mode, each
   character is sent one at a time and we look for the echo before
   sending the next.  This is excruciatingly slow.  */

static int srec_echo_pace = 0;

/* How long to wait after an srec for a possible error message.
   Similar to the above, I tried sleeping after sending each S3 record
   in hopes that I might actually see error messages from the bug
   monitor.  This might actually work if we were to use sleep
   intervals smaller than 1 second.  */

static int srec_sleep = 0;

/* Every srec_noise records, flub the checksum.  This is a debugging
   feature.  Set the variable to something other than 1 in order to
   inject *deliberate* checksum errors.  One might do this if one
   wanted to test error handling and recovery.  */

static int srec_noise = 0;

/* Called when SIGALRM signal sent due to alarm() timeout.  */

/* Number of SIGTRAPs we need to simulate.  That is, the next
   NEED_ARTIFICIAL_TRAP calls to bug_wait should just return
   SIGTRAP without actually waiting for anything.  */

static int need_artificial_trap = 0;

/* Data cache header.  */

static DCACHE *ppcbug_dcache;

/*
 * Download a file specified in 'args', to the bug.
 */

static void
ppcbug_load (args, fromtty)
     char *args;
     int fromtty;
{
  bfd *abfd;
  asection *s;
  char buffer[1024];

  sr_check_open ();

  dcache_flush (gr_get_dcache());
  inferior_pid = 0;
  abfd = bfd_openr (args, 0);
  if (!abfd)
    {
      printf_filtered ("Unable to open file %s\n", args);
      return;
    }

  if (bfd_check_format (abfd, bfd_object) == 0)
    {
      printf_filtered ("File is not an object file\n");
      return;
    }

  s = abfd->sections;
  while (s != (asection *) NULL)
    {
      srec_frame = SREC_SIZE;
      if (s->flags & SEC_LOAD)
	{
	  int i;

	  char *buffer = xmalloc (srec_frame);

	  printf_filtered ("%s\t: 0x%4x .. 0x%4x  ", s->name, s->vma, s->vma + s->_raw_size);
	  fflush (stdout);
	  for (i = 0; i < s->_raw_size; i += srec_frame)
	    {
	      if (srec_frame > s->_raw_size - i)
		srec_frame = s->_raw_size - i;

	      bfd_get_section_contents (abfd, s, buffer, i, srec_frame);
	      ppcbug_write_memory (s->vma + i, buffer, srec_frame);
	      printf_filtered ("*");
	      fflush (stdout);
	    }
	  printf_filtered ("\n");
	  free (buffer);
	}
      s = s->next;
    }
  sprintf (buffer, "rs ip %lx", (unsigned long) abfd->start_address);
  sr_write_cr (buffer);
  gr_expect_prompt ();
}

#if 0
static char *
get_word (p)
     char **p;
{
  char *s = *p;
  char *word;
  char *copy;
  size_t len;

  while (isspace (*s))
    s++;

  word = s;

  len = 0;

  while (*s && !isspace (*s))
    {
      s++;
      len++;

    }
  copy = xmalloc (len + 1);
  memcpy (copy, word, len);
  copy[len] = 0;
  *p = s;
  return copy;
}
#endif

static struct gr_settings ppcbug_settings = {
  NULL,				/* dcache */
  "PPC1-Bug>",			/* prompt */
  &ppcbug_ops,			/* ops */
  ppcbug_clear_breakpoints,	/* clear_all_breakpoints */
  ppcbug_read_memory,		/* readfunc */
  ppcbug_write_memory,		/* writefunc */
  gr_generic_checkin,		/* checkin */
};

static char *cpu_check_strings[] = {
  "=",
  "Invalid Register",
};

static void
ppcbug_open (args, from_tty)
     char *args;
     int from_tty;
{
  if (args == NULL)
      args = "";

  ppcbug_dcache = dcache_init (ppcbug_read_memory, ppcbug_write_memory);
  gr_settings = &ppcbug_settings;
  gr_open(args, from_tty, &ppcbug_settings);
}

/* Tell the remote machine to resume.  */

void
ppcbug_resume (pid, step, sig)
     int pid, step;
     enum target_signal sig;
{
  dcache_flush (ppcbug_dcache);

  if (step)
    {
      sr_write_cr("t");

      /* Force the next ppcbug_wait to return a trap.  Not doing anything
       about I/O from the target means that the user has to type
       "continue" to see any.  FIXME, this should be fixed.  */
      need_artificial_trap = 1;
    }
  else
      sr_write_cr ("g");

  return;
}

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

static char *wait_strings[] = {
  "At Breakpoint",
  "Exception: Data Access Fault (Local Bus Timeout)",
  "\r8???-Bug>",
  "\r197-Bug>",
  NULL,
};

int
ppcbug_wait (pid, status)
     int pid;
     struct target_waitstatus *status;
{
  int old_timeout = sr_get_timeout();
  int old_immediate_quit = immediate_quit;

  status->kind = TARGET_WAITKIND_EXITED;
  status->value.integer = 0;

  /* read off leftovers from resume so that the rest can be passed
     back out as stdout.  */
  if (need_artificial_trap == 0)
    {
      sr_expect("Effective address: ");
      (void) sr_get_hex_word();
      sr_expect ("\r\n");
    }

  sr_set_timeout(-1); /* Don't time out -- user program is running. */
  immediate_quit = 1; /* Helps ability to QUIT */

  switch (gr_multi_scan(wait_strings, need_artificial_trap == 0))
    {
    case 0: /* breakpoint case */
      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = TARGET_SIGNAL_TRAP;
      /* user output from the target can be discarded here. (?) */
      gr_expect_prompt();
      break;

    case 1: /* bus error */
      status->kind = TARGET_WAITKIND_STOPPED;
      status->value.sig = TARGET_SIGNAL_BUS;
      /* user output from the target can be discarded here. (?) */
      gr_expect_prompt();
      break;

    case 2: /* normal case */
    case 3:
      if (need_artificial_trap != 0)
	{
	  /* stepping */
	  status->kind = TARGET_WAITKIND_STOPPED;
	  status->value.sig = TARGET_SIGNAL_TRAP;
	  need_artificial_trap--;
	  break;
	}
      else
	{
	  /* exit case */
	  status->kind = TARGET_WAITKIND_EXITED;
	  status->value.integer = 0;
	  break;
	}

    case -1: /* trouble */
    default:
      fprintf_filtered (stderr,
			"Trouble reading target during wait\n");
      break;
    }

  sr_set_timeout(old_timeout);
  immediate_quit = old_immediate_quit;
  return 0;
}

/* Return the name of register number REGNO
   in the form input and output by ppcbug.

   Returns a pointer to a static buffer containing the answer.  */
static char *
get_reg_name (regno)
     int regno;
{
  static char *rn[] = {
    "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
    "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
    "r16",  "r17",  "r18",  "r19",  "r20",  "r21",  "r22",  "r23",
    "r24",  "r25",  "r26",  "r27",  "r28",  "r29",  "r30",  "r31",

    "fr0",  "fr1",  "fr2",  "fr3",  "fr4",  "fr5",  "fr6",  "fr7",
    "fr8",  "fr9",  "fr10", "fr11", "fr12", "fr13", "fr14", "fr15",
    "fr16", "fr17", "fr18", "fr19", "fr20", "fr21", "fr22", "fr23",
    "fr24", "fr25", "fr26", "fr27", "fr28", "fr29", "fr30", "fr31",

/*   pc      ps      cnd     lr      cnt     xer     mq */
    "ip",   "msr",  "cr",   "spr8", "spr9", "spr1", "spr0"
  };

  return rn[regno];
}

#if 0 /* not currently used */
/* Read from remote while the input matches STRING.  Return zero on
   success, -1 on failure.  */

static int
ppcbug_scan (s)
     char *s;
{
  int c;

  while (*s)
    {
      c = sr_readchar();
      if (c != *s++)
	{
	  fflush(stdout);
	  printf("\nNext character is '%c' - %d and s is \"%s\".\n", c, c, --s);
	  return(-1);
	}
    }

  return(0);
}
#endif /* never */

static int
ppcbug_srec_write_cr (s)
     char *s;
{
  char *p = s;

  if (srec_echo_pace)
    for (p = s; *p; ++p)
      {
	if (sr_get_debug() > 0)
	  printf ("%c", *p);

	do
	  SERIAL_WRITE(sr_get_desc(), p, 1);
	while (sr_pollchar() != *p);
      }
  else
    {  
      sr_write_cr (s);
/*       return(ppcbug_scan (s) || ppcbug_scan ("\n")); */
    }

  return(0);
}

/* Store register REGNO, or all if REGNO == -1. */

static void
ppcbug_fetch_register(regno)
     int regno;
{
  sr_check_open();

  if (regno == -1)
    {
      int i;

      for (i = 0; i < NUM_REGS; ++i)
	ppcbug_fetch_register(i);
    }
  else if (regno < 32 || regno > 63)
    {
      char buffer[MAX_REGISTER_RAW_SIZE];

      sr_write ("rs ", 3);
      sr_write_cr (get_reg_name(regno));
      sr_expect ("=");
      store_unsigned_integer (buffer, REGISTER_RAW_SIZE (regno),
			      sr_get_hex_word());
      gr_expect_prompt ();
      supply_register (regno, buffer);
    }
  else
    {
      /* Float register so we need to parse a strange data format. */
      long p;
      unsigned char fpreg_buf[10];

      sr_write("rs ", 3);
      sr_write(get_reg_name(regno), strlen(get_reg_name(regno)));
      sr_write_cr(";d");
      sr_expect("rs");
      sr_expect(get_reg_name(regno));
      sr_expect(";d");
      sr_expect("=");

      /* sign */
      p = sr_get_hex_digit(1);
      fpreg_buf[0] = p << 7;

      /* exponent */
      sr_expect("_");
      p = sr_get_hex_digit(1);
      fpreg_buf[0] += (p << 4);
      fpreg_buf[0] += sr_get_hex_digit(1);

      fpreg_buf[1] = sr_get_hex_digit(1) << 4;

      /* fraction */
      sr_expect("_");
      fpreg_buf[1] += sr_get_hex_digit(1);

      fpreg_buf[2] = (sr_get_hex_digit(1) << 4) + sr_get_hex_digit(1);
      fpreg_buf[3] = (sr_get_hex_digit(1) << 4) + sr_get_hex_digit(1);
      fpreg_buf[4] = (sr_get_hex_digit(1) << 4) + sr_get_hex_digit(1);
      fpreg_buf[5] = (sr_get_hex_digit(1) << 4) + sr_get_hex_digit(1);
      fpreg_buf[6] = (sr_get_hex_digit(1) << 4) + sr_get_hex_digit(1);
      fpreg_buf[7] = (sr_get_hex_digit(1) << 4) + sr_get_hex_digit(1);
      fpreg_buf[8] = 0;
      fpreg_buf[9] = 0;

      gr_expect_prompt();
      supply_register(regno, fpreg_buf);
    }

  return;
}

/* Store register REGNO, or all if REGNO == -1. */

static void
ppcbug_store_register (regno)
     int regno;
{
  char buffer[1024];
  sr_check_open();

  if (regno == -1)
    {
      int i;

      for (i = 0; i < NUM_REGS; ++i)
	ppcbug_store_register(i);
    }
  else
    {
      char *regname;

      regname = get_reg_name(regno);

      if (regno < 32 || regno > 63)
	sprintf(buffer, "rs %s %08lx",
		regname,
		(unsigned long)read_register(regno));
      else
	{
	  unsigned char *fpreg_buf =
	    (unsigned char *)&registers[REGISTER_BYTE(regno)];
	  
	  sprintf(buffer, "rs %s %1x_%02x%1x_%1x%02x%02x%02x%02x%02x%02x;d",
		  regname,
		  /* sign */
		  (fpreg_buf[0] >> 7) & 0xf,
		  /* exponent */
		  fpreg_buf[0] & 0x7f,
		  (fpreg_buf[1] >> 8) & 0xf,
		  /* fraction */
		  fpreg_buf[1] & 0xf,
		  fpreg_buf[2],
		  fpreg_buf[3],
		  fpreg_buf[4],
		  fpreg_buf[5],
		  fpreg_buf[6],
		  fpreg_buf[7]);
	}

      sr_write_cr(buffer);
      gr_expect_prompt();
    }

  return;
}

int
ppcbug_xfer_memory (memaddr, myaddr, len, should_write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int should_write;
     struct target_ops *target;	/* ignored */
{
  return dcache_xfer_memory (ppcbug_dcache, memaddr, myaddr, len, should_write);
}

static void
start_load()
{
  char *command;

  command = (srec_echo_pace ? "lo 0 ;x" : "lo 0");

  sr_write_cr (command);
  sr_expect (command);
  sr_expect ("\r\n");
  ppcbug_srec_write_cr ("S0030000FC");
  return;
}

/* This is an extremely vulnerable and fragile function.  I've made
   considerable attempts to make this deterministic, but I've
   certainly forgotten something.  The trouble is that S-records are
   only a partial file format, not a protocol.  Worse, apparently the
   m88k bug monitor does not run in real time while receiving
   S-records.  Hence, we must pay excruciating attention to when and
   where error messages are returned, and what has actually been sent.

   Each call represents a chunk of memory to be sent to the target.
   We break that chunk into an S0 header record, some number of S3
   data records each containing srec_bytes, and an S7 termination
   record.  */

static char *srecord_strings[] = {
  "S-RECORD",
  "-Bug>",
  NULL,
};

static int
ppcbug_write_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int done;
  int checksum;
  int x;
  int retries;
  char *buffer = alloca ((srec_bytes + 8) << 1);

  retries = 0;

  do
    {
      done = 0;

      if (retries > srec_max_retries)
	return(-1);

      if (retries > 0)
	{
	  if (sr_get_debug() > 0)
	    printf("\n<retrying...>\n");

	  /* This gr_expect_prompt call is extremely important.  Without
	     it, we will tend to resend our packet so fast that it
	     will arrive before the bug monitor is ready to receive
	     it.  This would lead to a very ugly resend loop.  */

	  gr_expect_prompt();
	}

      start_load();

      while (done < len)
	{
	  int thisgo;
	  int idx;
	  char *buf = buffer;
	  CORE_ADDR address;

	  checksum = 0;
	  thisgo = len - done;
	  if (thisgo > srec_bytes)
	    thisgo = srec_bytes;

	  address = memaddr + done;
	  sprintf (buf, "S3%02lx%08lx", (long)(thisgo + 4 + 1), (unsigned long)address);
	  buf += 12;

	  checksum += (thisgo + 4 + 1
		       + (address & 0xff)
		       + ((address >>  8) & 0xff)
		       + ((address >> 16) & 0xff)
		       + ((address >> 24) & 0xff));

	  for (idx = 0; idx < thisgo; idx++)
	    {
	      sprintf (buf, "%02lx", (unsigned long)(myaddr[idx + done]));
	      checksum += myaddr[idx + done];
	      buf += 2;
	    }

	  if (srec_noise > 0)
	    {
	      /* FIXME-NOW: insert a deliberate error every now and then.
		 This is intended for testing/debugging the error handling
		 stuff.  */
	      static int counter = 0;
	      if (++counter > srec_noise)
		{
		  counter = 0;
		  ++checksum;
		}
	    }

	  sprintf(buf, "%02lx", (unsigned long)(~checksum & 0xff));
	  ppcbug_srec_write_cr (buffer);

	  if (srec_sleep != 0)
	    sleep(srec_sleep);

	  /* This pollchar is probably redundant to the gr_multi_scan
	     below.  Trouble is, we can't be sure when or where an
	     error message will appear.  Apparently, when running at
	     full speed from a typical sun4, error messages tend to
	     appear to arrive only *after* the s7 record.   */

	  if ((x = sr_pollchar()) != 0)
	    {
	      if (sr_get_debug() > 0)
		printf("\n<retrying...>\n");

	      ++retries;

	      /* flush any remaining input and verify that we are back
		 at the prompt level. */
	      gr_expect_prompt();
	      /* start all over again. */
	      start_load();
	      done = 0;
	      continue;
	    }

	  done += thisgo;
	}

      ppcbug_srec_write_cr("S7060000000000F9");
      ++retries;

      /* Having finished the load, we need to figure out whether we
	 had any errors.  */
    } while (gr_multi_scan(srecord_strings, 0) == 0);;

  return(0);
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.  Returns errno value.
 * sb/sh instructions don't work on unaligned addresses, when TU=1.
 */

/* Read LEN bytes from inferior memory at MEMADDR.  Put the result
   at debugger address MYADDR.  Returns errno value.  */
static int
ppcbug_read_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  char request[100];
  char *buffer;
  char *p;
  char type;
  char size;
  unsigned char c;
  unsigned int inaddr;
  unsigned int checksum;

  sprintf(request, "du 0 %lx:&%ld", (unsigned long)memaddr, (long)len);
  sr_write_cr(request);

  p = buffer = alloca(len);

  /* scan up through the header */
  sr_expect("S0030000FC");

  while (p < buffer + len)
    {
      /* scan off any white space. */
      while (sr_readchar() != 'S') ;;

      /* what kind of s-rec? */
      type = sr_readchar();

      /* scan record size */
      sr_get_hex_byte(&size);
      checksum = size;
      --size;
      inaddr = 0;

      switch (type)
	{
	case '7':
	case '8':
	case '9':
	  goto done;

	case '3':
	  sr_get_hex_byte(&c);
	  inaddr = (inaddr << 8) + c;
	  checksum += c;
	  --size;
	  /* intentional fall through */
	case '2':
	  sr_get_hex_byte(&c);
	  inaddr = (inaddr << 8) + c;
	  checksum += c;
	  --size;
	  /* intentional fall through */
	case '1':
	  sr_get_hex_byte(&c);
	  inaddr = (inaddr << 8) + c;
	  checksum += c;
	  --size;
	  sr_get_hex_byte(&c);
	  inaddr = (inaddr << 8) + c;
	  checksum += c;
	  --size;
	  break;

	default:
	  /* bonk */
	  error("reading s-records.");
	}

      if (inaddr < memaddr
	  || (memaddr + len) < (inaddr + size))
	error("srec out of memory range.");

      if (p != buffer + inaddr - memaddr)
	error("srec out of sequence.");

      for (; size; --size, ++p)
	{
	  sr_get_hex_byte(p);
	  checksum += *p;
	}

      sr_get_hex_byte(&c);
      if (c != (~checksum & 0xff))
	error("bad s-rec checksum");
    }

 done:
  gr_expect_prompt();
  if (p != buffer + len)
    return(1);

  memcpy(myaddr, buffer, len);
  return(0);
}

#define MAX_BREAKS	16
static int num_brkpts = 0;
static int
ppcbug_insert_breakpoint (addr, save)
     CORE_ADDR addr;
     char *save;		/* Throw away, let ppcbug save instructions */
{
  sr_check_open ();

  if (num_brkpts < MAX_BREAKS)
    {
      char buffer[100];

      num_brkpts++;
      sprintf (buffer, "br %lx", (unsigned long)addr);
      sr_write_cr (buffer);
      gr_expect_prompt ();
      return(0);
    }
  else
    {
      fprintf_filtered (stderr,
		      "Too many break points, break point not installed\n");
      return(1);
    }

}
static int
ppcbug_remove_breakpoint (addr, save)
     CORE_ADDR addr;
     char *save;		/* Throw away, let ppcbug save instructions */
{
  if (num_brkpts > 0)
    {
      char buffer[100];

      num_brkpts--;
      sprintf (buffer, "nobr %lx", (unsigned long)addr);
      sr_write_cr (buffer);
      gr_expect_prompt ();

    }
  return (0);
}

/* Clear the ppcbugs notion of what the break points are */
static int
ppcbug_clear_breakpoints ()
{

  if (sr_is_open())
    {
      sr_write_cr ("nobr");
      sr_expect("nobr");
      gr_expect_prompt ();
    }
  num_brkpts = 0;
  return(0);
}

struct target_ops ppcbug_ops =
{
  "ppcbug", "Remote PPCBUG monitor",
  "Use the PowerStack board running the PPCBUG monitor connected by a serial line.",

  ppcbug_open,			/* to_open */
  gr_close,			/* to_close */
  0,				/* to_attach */
  gr_detach,			/* to_detach */
  ppcbug_resume,		/* to_resume */
  ppcbug_wait,			/* to_wait */
  ppcbug_fetch_register,	/* to_fetch_registers */
  ppcbug_store_register,	/* to_store_registers */
  gr_prepare_to_store,		/* to_prepare_to_store */
  ppcbug_xfer_memory,		/* to_xfer_memory */
#if 0
  0,				/* to_search */
#endif
  gr_files_info,		/* to_files_info */
  ppcbug_insert_breakpoint,	/* to_insert_breakpoint */
  ppcbug_remove_breakpoint,	/* to_remove_breakpoint */
  0,				/* to_terminal_init */
  0,				/* to_terminal_inferior */
  0,				/* to_terminal_ours_for_output */
  0,				/* to_terminal_ours */
  0,				/* to_terminal_info */
  gr_kill,			/* to_kill, FIXME, kill */
  ppcbug_load,			/* to_load */
  0,				/* to_lookup_symbol */
  gr_create_inferior,		/* to_create_inferior */
  gr_mourn,			/* to_mourn_inferior FIXME */
  0,				/* to_can_run */
  0,				/* to_notice_signals */
  0,				/* to_thread_alive */
  0,				/* to_stop */
  process_stratum,		/* to_stratum */
  0,				/* DONT_USE */
  1,				/* to_has_all_memory */
  1,				/* to_has_memory */
  1,				/* to_has_stack */
  1,				/* to_has_registers */
  1,				/* to_has_execution */
  0,				/* to_sections */
  0,				/* to_sections_end */
  OPS_MAGIC,			/* Always the last thing */
};

void
_initialize_remote_ppcbug ()
{
  add_target (&ppcbug_ops);

  add_show_from_set
    (add_set_cmd ("srec-bytes", class_support, var_uinteger,
		  (char *) &srec_bytes,
		  "\
Set the number of bytes represented in each S-record.\n\
This affects the communication protocol with the remote target.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("srec-max-retries", class_support, var_uinteger,
		  (char *) &srec_max_retries,
		  "\
Set the number of retries for shipping S-records.\n\
This affects the communication protocol with the remote target.",
		  &setlist),
     &showlist);

#if 0
  /* This needs to set SREC_SIZE, not srec_frame which gets changed at the
     end of a download.  But do we need the option at all?  */
  add_show_from_set
    (add_set_cmd ("srec-frame", class_support, var_uinteger,
		  (char *) &srec_frame,
		  "\
Set the number of bytes in an S-record frame.\n\
This affects the communication protocol with the remote target.",
		  &setlist),
     &showlist);
#endif /* 0 */

  add_show_from_set
    (add_set_cmd ("srec-noise", class_support, var_zinteger,
		  (char *) &srec_noise,
		  "\
Set number of S-record to send before deliberately flubbing a checksum.\n\
Zero means flub none at all.  This affects the communication protocol\n\
with the remote target.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("srec-sleep", class_support, var_zinteger,
		  (char *) &srec_sleep,
		  "\
Set number of seconds to sleep after an S-record for a possible error message to arrive.\n\
This affects the communication protocol with the remote target.",
		  &setlist),
     &showlist);

  add_show_from_set
    (add_set_cmd ("srec-echo-pace", class_support, var_boolean,
		  (char *) &srec_echo_pace,
		  "\
Set echo-verification.\n\
When on, use verification by echo when downloading S-records.  This is\n\
much slower, but generally more reliable.", 
		  &setlist),
     &showlist);
}
