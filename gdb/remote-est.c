/* Remote debugging interface for EST-300 ICE, for GDB
   Copyright 1994 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

   Written by Steve Chamberlain for Cygnus Support.

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


#include "defs.h"
#include "command.h"
#include "gdbcore.h"
#include "target.h"
#include "wait.h"
#include <varargs.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include "serial.h"
#include "remote-utils.h"


static void expect_char PARAMS ((int));


static void 
write_and_expect (x)
char *x;
{
  sr_write_cr (x);
  sr_expect (x);
}

static void
expect_char (want)
     int want;
{
  int c = sr_readchar ();
  while (c != want)
    c = sr_readchar ();
}


static void
expect_prompt ()
{
  expect_char ('>');
}

static int
get_hex_digit (ch)
     int ch;
{
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  else if (ch >= 'A' && ch <= 'F')
    return ch - 'A' + 10;
  else if (ch >= 'a' && ch <= 'f')
    return ch - 'a' + 10;
  return -1;
}

static int
get_hex (start)
     int *start;
{
  int value = get_hex_digit (*start);
  int try;

  *start = sr_readchar ();
  while ((try = get_hex_digit (*start)) >= 0)
    {
      value <<= 4;
      value += try;
      *start = sr_readchar ();
    }
  return value;
}

/* Tell the remote machine to resume.  */

static void
est_resume (pid, step, sig)
     int pid, step, sig;
{
  write_and_expect (step ? ".SI" : ".GO");
}

/* A reg dump looks like
  D0 = 00000000 D1 = 00000000 D2 = 00000000 D3 = 00000000
  D4 = 00000000 D5 = 00000000 D6 = 00000000 D7 = 00000000
  A0 = 00000000 A1 = 00000000 A2 = 00000000 A3 = 00000000
  A4 = 00000000 A5 = 00000000 A6 = 00000000 A7 = 001104FE
  USP = 00110400 SSP*= 001104FE PC = 00229BBC SR = 2000
  VBR = 00110000 SFC = 0005 DFC = 0005

or

00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00001234 00000000 001104FE 00110400 001104FE 00229BBC 2000 00110000 0005 0005
*/

static int 
target_to_gdb_rn (rn)
     int rn;
{
  if (rn < 16)
    return rn;
  if (rn == 18)
    return PC_REGNUM;
  if (rn == 19)
    return PS_REGNUM;
  return -1;
}


static void est_fetch_register ();
static void
est_fetch_registers ()
{
  int regno;
  unsigned long val;
  int c;
  int target_rn;
  char buf[4];
  write_and_expect (".DR");
  buf[0] = 0;
  buf[1] = 0;
  buf[2] = 0;
  buf[3] = 0;
  for (regno = 0; regno < NUM_REGS; regno++)
    supply_register (regno, buf);

  c = sr_readchar ();
  for (target_rn = 0; target_rn < 23; target_rn++)
    {
      unsigned long val;
      while (!isdigit (c) && !isalpha (c))
	c = sr_readchar ();

      while (isdigit (c) || (c >= 'A' && c <= 'F'))
	{
	  val <<= 4;
	  if (isdigit (c))
	    val = val + c - '0';
	  else
	    val = val + c - 'A' + 10;
	  c = sr_readchar ();
	}

      regno = target_to_gdb_rn (target_rn);
      if (regno >= 0)
	{
	  buf[0] = val >> 24;
	  buf[1] = val >> 16;
	  buf[2] = val >> 8;
	  buf[3] = val >> 0;
	  supply_register (regno, buf);
	}
    }
  expect_prompt();
}

/* Fetch register REGNO, or all registers if REGNO is -1.
   Returns errno value.  */

static
void
est_fetch_register (regno)
     int regno;
{
  est_fetch_registers ();
}

/* Store the remote registers from the contents of the block REGS.  */

static void est_store_register ();
static void
est_store_registers ()
{
  int regno;

  for (regno = 0; regno < 18; regno++)
    est_store_register (regno);
  registers_changed ();
}

/* Store register REGNO, or all if REGNO == 0.
   Return errno value.  */
static void
est_store_register (regno)
     int regno;
{
  char buf[20];
  if (regno == -1)
    {
      est_store_registers ();
      return;
    }

  if (regno < 8)
    sprintf (buf, ".SR D%d %x", regno, read_register (regno));
  else if (regno < 16)
    sprintf (buf, ".SR A%d %x", regno - 8, read_register (regno));
  else if (regno == PC_REGNUM)
    sprintf (buf, ".SR PC %x", read_register (regno));
  else if (regno == PS_REGNUM)
    sprintf (buf, ".SR SR %x", read_register (regno));
  else
    return;
  write_and_expect (buf);
  expect_prompt ();
}

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */


static
int
stickbyte (where, what)
     char *where;
     unsigned int what;
{
  static CONST char digs[] = "0123456789ABCDEF";
  where[0] = digs[(what >> 4) & 0xf];
  where[1] = digs[(what & 0xf) & 0xf];
  return what;
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.  Returns length moved.   */

static int
est_write_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  int i;
#define maxstride  128
  int stride;

  write_and_expect (".DL");
  expect_char ('+');
  for (i = 0; i < len; i += stride)
    {
      char compose[maxstride * 2 + 50];
      int address = i + memaddr;
      int j;
      int check_sum;
      int where = 0;
      int alen;
      stride = len - i;
      if (stride > maxstride)
	stride = maxstride;

      compose[where++] = 'S';
      check_sum = 0;
      if (address >= 0xffffff)
	{
	  alen = 4;
	}
      else if (address >= 0xffff)
	{
	  alen = 3;
	}
      else
	alen = 2;
      compose[where++] = alen - 1 + '0';	/* insert type */
      check_sum += stickbyte (compose + where, alen + stride + 1);	/* Insert length */
      where += 2;
      while (alen > 0)
	{
	  alen--;
	  check_sum += stickbyte (compose + where, address >> (8 * (alen)));
	  where += 2;
	}

      for (j = 0; j < stride; j++)
	{
	  check_sum += stickbyte (compose + where, myaddr[i + j]);
	  where += 2;
	}

      stickbyte (compose + where, ~check_sum);

      where += 2;
      compose[where++] = 0;

      sr_write_cr (compose);
      while (sr_readchar () != '+')
	  sr_write_cr (compose);
    }

  /* Send the trailer record */
  sr_write_cr ("S70500000000FA");
  expect_prompt ();
  return len;
}



/*

   The dump memory command generates output which looks like:


.dmb 0 100
4E 56 FF FC 4E 71 42 AE FF FC 72 09 B2 AE FF FC     NV..NqB...r.....
6C 02 60 12 2F 2E FF FC 4E B9 00 00 00 2A 58 4F     l.`./...N....*XO
52 AE FF FC 60 E4 4E 5E 4E 75 4E 56 00 00 20 2E     R...`.N^NuNV.. .
00 08 D1 B9 00 00 00 00 4E 5E 4E 75 06 46 40 54     ........N^Nu.F@T
04 45 44 4C 54 45 40 56 42 F4 04 64 24 45 05 05     .EDLTE@VB..d$E..
00 6D 04 46 00 45 4C 05 04 46 04 4C 44 CD 00 65     .m.F.EL..F.LD..e
40 45 44 55 45 45 45 46 04 44 44 40 05 4D 00 44     @EDUEEEF.DD@.M.D

*/

static int
est_read_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  int count;
  int c;
  char buf[20];
  /* Starting address of this pass.  */

  if (((memaddr - 1) + len) < memaddr)
    {
      errno = EIO;
      return 0;
    }

  sprintf (buf, ".dmb %x %x", memaddr, len);
  write_and_expect (buf);
  count = 0;

  c = sr_readchar ();

  while (count < len)
    {
      while (!isdigit (c) && !isalpha (c)) {
	if (c == '!')
	  {
	    expect_prompt();
	    errno =EIO;
	    return 0;

	  }
	c = sr_readchar ();
      }
      myaddr[count++] = get_hex (&c);
      c = sr_readchar ();
      if (c == ' ')
	{
	  c = sr_readchar ();
	  if (c == ' ')
	    while (c != '\r')
	      c = sr_readchar ();
	}
    }

  expect_prompt ();


  return len;
}

static int
est_xfer_inferior_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
     int write;
     struct target_ops *target;	/* ignored */
{
  if (write)
    {
      return est_write_memory (memaddr, myaddr, len);
    }
  else
    {
      return est_read_memory (memaddr, myaddr, len);
    }
}


#define MAX_DEBUG_BREAKPOINTS 100

extern int memory_breakpoint_size;
static CORE_ADDR breakaddr[MAX_DEBUG_BREAKPOINTS] =
{0};

int 
est_clear_all_breakpoints ()
{
  int i;
  for (i = 0; i < MAX_DEBUG_BREAKPOINTS; i++)
    {
      breakaddr[i] = 0;
    }

  if (sr_is_open ())
    {
      write_and_expect (".RB");
      expect_prompt ();
    }
  return 0;
}

static int
est_insert_breakpoint (addr, shadow)
     CORE_ADDR addr;
     unsigned char *shadow;
{
  int i;

  for (i = 0; i <= MAX_DEBUG_BREAKPOINTS; i++)
    if (breakaddr[i] == 0)
      {
	char buf[20];
	breakaddr[i] = addr;
	sprintf (buf, ".SB %x", addr);
	write_and_expect (buf);
	expect_prompt ();
	return 0;
      }
  error ("Too many breakpoints ( > %d) for the est\n", MAX_DEBUG_BREAKPOINTS);
  return 1;
}

static int
est_remove_breakpoint (addr, shadow)
     CORE_ADDR addr;
     unsigned char *shadow;
{
  int i;

  for (i = 0; i < MAX_DEBUG_BREAKPOINTS; i++)
    if (breakaddr[i] == addr)
      {
	char buf[20];
	breakaddr[i] = 0;
	sprintf (buf, ".RB %x", addr);
	write_and_expect (buf);
	expect_prompt ();
	return 0;
      }

  error ("Can't find breakpoint associated with 0x%x\n", addr);
  return 1;
}


/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

static int
est_wait (pid, status)
     int pid;
     struct target_waitstatus *status;
{
  int c = sr_readchar ();
  while (c != '!')
    c = sr_readchar ();
  /* What sort of stop */
  c = sr_readchar ();
  status->kind = TARGET_WAITKIND_STOPPED;
  switch (c)
    {
    case 'E':
      status->value.sig = TARGET_SIGNAL_BUS;
      break;
      /* Address error */
    case 'A':
      status->value.sig = TARGET_SIGNAL_BUS;
      break;
      /* Break */
    case 'B':
      status->value.sig = TARGET_SIGNAL_TRAP;
      break;
    }
  expect_prompt ();
  return 0;
}

void 
est_checkin ()
{
  write_and_expect (".in");
  gr_expect_prompt ();
}

extern struct gr_settings est_settings;

static void
est_open (args, from_tty)
     char *args;
     int from_tty;
{
  gr_open (args, from_tty, &est_settings);
}

/* Define the target subroutine names */

struct target_ops est_ops =
{
  "est",
  "Remote EST-300 target",
  "Use a remote EST-300 ICE connected by a serial line,\n\
or a network connection.\n\
Arguments are the name of the device for the serial line,\n\
the speed to connect at in bits per second.\n\
eg\n\
target est /dev/ttya 9600\n\
target est foobar",
  est_open,
  gr_close,
  0,
  gr_detach,
  est_resume,
  est_wait,
  est_fetch_register,
  est_store_register,
  gr_prepare_to_store,
  est_xfer_inferior_memory,
  gr_files_info,
  est_insert_breakpoint,
  est_remove_breakpoint,	/* Breakpoints */
  0,
  0,
  0,
  0,
  0,				/* Terminal handling */
  gr_kill,
  gr_load_image,		/* load */
  0,				/* lookup_symbol */
  gr_create_inferior,
  gr_mourn,
  0,				/* can_run */
  0,				/* notice_signals */
  0,				/* to_stop */
  process_stratum,
  0,				/* next */
  1,
  1,
  1,
  1,
  1,				/* all mem, mem, stack, regs, exec */
  0,
  0,				/* Section pointers */
  OPS_MAGIC,			/* Always the last thing */
};

static struct gr_settings est_settings =
{
  NULL,				/* dcache */
  ">",				/* prompt */
  &est_ops,			/* ops */
  est_clear_all_breakpoints,
  est_read_memory,		/* readfunc */
  est_write_memory,		/* writefunc */
  est_checkin,			/* checkin */
};

void
_initialize_remote_est ()
{
  add_target (&est_ops);
}
