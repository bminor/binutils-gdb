/* Remote debugging interface for Densan DVE-R3900 ROM monitor for
   GDB, the GNU debugger.
   Copyright 1997 Free Software Foundation, Inc.

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
#include "gdbcore.h"
#include "target.h"
#include "monitor.h"
#include "serial.h"

static void r3900_open PARAMS ((char *args, int from_tty));

/* Pointers to static functions in monitor.c for fetching and storing
   registers.  We can't use these function in certain cases where the Densan
   monitor acts perversely: for registers that it displays in bit-map
   format, and those that can't be modified at all.  In those cases
   we have to use our own functions to fetch and store their values.  */

static void (*orig_monitor_fetch_registers) PARAMS ((int regno));
static void (*orig_monitor_store_registers) PARAMS ((int regno));


/* This array of registers needs to match the indexes used by GDB. The
   whole reason this exists is because the various ROM monitors use
   different names than GDB does, and don't support all the registers
   either.  */

static char *r3900_regnames[NUM_REGS] =
{
  "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
  "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
  "r16",  "r17",  "r18",  "r19",  "r20",  "r21",  "r22",  "r23",
  "r24",  "r25",  "r26",  "r27",  "r28",  "r29",  "r30",  "r31",

  "S", 		/* PS_REGNUM */
  "l",		/* LO_REGNUM */
  "h",		/* HI_REGNUM */
  "B",		/* BADVADDR_REGNUM */
  "Pcause",	/* CAUSE_REGNUM */
  "p"		/* PC_REGNUM */
};


/* Table of register names produced by monitor's register dump command.  */

static struct reg_entry
{
  char *name;
  int regno;
} reg_table[] =
{
  { "r0_zero", 0 },  { "r1_at",   1 },  { "r2_v0",   2 },  { "r3_v1",   3 },
  { "r4_a0",   4 },  { "r5_a1",   5 },  { "r6_a2",   6 },  { "r7_a3",   7 },
  { "r8_t0",   8 },  { "r9_t1",   9 },  { "r10_t2", 10 },  { "r11_t3", 11 },
  { "r12_t4", 12 },  { "r13_t5", 13 },  { "r14_t6", 14 },  { "r15_t7", 15 },
  { "r16_s0", 16 },  { "r17_s1", 17 },  { "r18_s2", 18 },  { "r19_s3", 19 },
  { "r20_s4", 20 },  { "r21_s5", 21 },  { "r22_s6", 22 },  { "r23_s7", 23 },
  { "r24_t8", 24 },  { "r25_t9", 25 },  { "r26_k0", 26 },  { "r27_k1", 27 },
  { "r28_gp", 28 },  { "r29_sp", 29 },  { "r30_fp", 30 },  { "r31_ra", 31 },
  { "HI",     HI_REGNUM },
  { "LO",     LO_REGNUM },
  { "PC",     PC_REGNUM },
  { "BadV",   BADVADDR_REGNUM },
  { NULL,     0 }
};


/* The monitor prints register values in the form

	regname = xxxx xxxx

   We look up the register name in a table, and remove the embedded space in
   the hex value before passing it to monitor_supply_register.  */

static void
r3900_supply_register (regname, regnamelen, val, vallen)
     char *regname;
     int regnamelen;
     char *val;
     int vallen;
{
  int regno = -1;
  int i;
  char valbuf[10];
  char *p;

  /* Perform some sanity checks on the register name and value.  */
  if (regnamelen < 2 || regnamelen > 7 || vallen != 9)
    return;

  /* Look up the register name.  */
  for (i = 0; reg_table[i].name != NULL; i++)
    {
      int rlen = strlen (reg_table[i].name);
      if (rlen == regnamelen && strncmp (regname, reg_table[i].name, rlen) == 0)
	{
	  regno = reg_table[i].regno;
	  break;
	}
    }
  if (regno == -1)
    return;

  /* Copy the hex value to a buffer and eliminate the embedded space. */
  for (i = 0, p = valbuf; i < vallen; i++)
    if (val[i] != ' ')
      *p++ = val[i];
  *p = '\0';

  monitor_supply_register (regno, valbuf);
}

/* Fetch the BadVaddr register.  Unlike the other registers, this
   one can't be modified, and the monitor won't even prompt to let
   you modify it.  */

static void
r3900_fetch_badvaddr()
{
  char buf[20];
  int c;

  monitor_printf ("xB\r");
  monitor_expect ("BadV=", NULL, 0);
  monitor_expect_prompt (buf, sizeof(buf));
  monitor_supply_register (BADVADDR_REGNUM, buf);
}

    
/* Certain registers are "bitmapped", in that the monitor can only display
   them or let the user modify them as a series of named bitfields.
   This structure describes a field in a bitmapped register.  */

struct bit_field
{
  char *prefix;		/* string appearing before the value */
  char *suffix;		/* string appearing after the value */
  char *user_name;	/* name used by human when entering field value */
  int  length;		/* number of bits in the field */
  int  start;		/* starting (least significant) bit number of field */
};
        
/* The monitor displays the cache register along with the status register,
   as if they were a single register.  So when we want to fetch the
   status register, parse but otherwise ignore the fields of the
   cache register that the monitor displays.  Register fields that should
   be ignored have a length of zero in the tables below.  */

static struct bit_field status_fields [] =
{
  /* Status register portion */
  { "SR[<CU=",	" ",	"cu",	4, 28 },
  { "RE=",	" ",	"re",	1, 25 },
  { "BEV=",	" ",	"bev",	1, 22 },
  { "TS=",	" ",	"ts",	1, 21 },
  { "Nmi=",	" ",	"nmi",	1, 20 },
  { "INT=",	" ",	"int",  6, 10 },
  { "SW=",	">]",	"sw",   2,  8 },
  { "[<KUO=",	" ",	"kuo",  1,  5 },
  { "IEO=",	" ",	"ieo",  1,  4 },
  { "KUP=",	" ",	"kup",  1,  3 },
  { "IEP=",	" ",	"iep",  1,  2 },
  { "KUC=",	" ",	"kuc",  1,  1 },
  { "IEC=",	">]",	"iec",  1,  0 },

  /* Cache register portion (dummy for parsing only) */
  { "CR[<IalO="," ",	"ialo",	0, 13 },
  { "DalO=",	" ",	"dalo",	0, 12 },
  { "IalP=",	" ",	"ialp",	0, 11 },
  { "DalP=",	" ",	"dalp",	0, 10 },
  { "IalC=",	" ",	"ialc",	0,  9 },
  { "DalC=",	">] ",	"dalc",	0,  8 },

  { NULL,	NULL,	0,  0 }		/* end of table marker */
};


static struct bit_field cache_fields [] =
{
  /* Status register portion (dummy for parsing only) */
  { "SR[<CU=",	" ",	"cu",	0, 28 },
  { "RE=",	" ",	"re",	0, 25 },
  { "BEV=",	" ",	"bev",	0, 22 },
  { "TS=",	" ",	"ts",	0, 21 },
  { "Nmi=",	" ",	"nmi",	0, 20 },
  { "INT=",	" ",	"int",  0, 10 },
  { "SW=",	">]",	"sw",   0,  8 },
  { "[<KUO=",	" ",	"kuo",  0,  5 },
  { "IEO=",	" ",	"ieo",  0,  4 },
  { "KUP=",	" ",	"kup",  0,  3 },
  { "IEP=",	" ",	"iep",  0,  2 },
  { "KUC=",	" ",	"kuc",  0,  1 },
  { "IEC=",	">]",	"iec",  0,  0 },

  /* Cache register portion  */
  { "CR[<IalO="," ",	"ialo",	1, 13 },
  { "DalO=",	" ",	"dalo",	1, 12 },
  { "IalP=",	" ",	"ialp",	1, 11 },
  { "DalP=",	" ",	"dalp",	1, 10 },
  { "IalC=",	" ",	"ialc",	1,  9 },
  { "DalC=",	">] ",	"dalc",	1,  8 },

  { NULL,	NULL,	NULL,	0,  0 }		/* end of table marker */
};

static struct bit_field cause_fields[] = 
{
  { "<BD=",	" ",	"bd",   1, 31 },
  { "CE=",	" ",	"ce",   2, 28 },
  { "IP=",	" ",	"ip",   6, 10 },
  { "SW=", 	" ",	"sw",   2,  8 },
  { "EC=", 	">]" ,	"ec",   5,  2 },

  { NULL,	NULL,	NULL,	0,  0 }		/* end of table marker */
};


/* Read a series of bit fields from the monitor, and return their
   combined binary value.  */

static unsigned long
r3900_fetch_fields (bf)
     struct bit_field *bf;
{
  char buf[20];
  int c;
  unsigned long val = 0;
  unsigned long bits;

  for ( ; bf->prefix != NULL; bf++)
    {
      monitor_expect (bf->prefix, NULL, 0);		/* get prefix */
      monitor_expect (bf->suffix, buf, sizeof (buf));	/* hex value, suffix */
      if (bf->length != 0)
        {
	  bits = strtoul (buf, NULL, 16);	/* get field value */
	  bits &= ((1 << bf->length) - 1);	/* mask out useless bits */
	  val |= bits << bf->start;		/* insert into register */
	}
	  
    }

  return val;
}

static void
r3900_fetch_bitmapped_register (regno, bf)
     int regno;
     struct bit_field *bf;
{
  char buf[20];
  int c;
  unsigned long val;
  unsigned long bits;
  unsigned char regbuf[MAX_REGISTER_RAW_SIZE];

  monitor_printf ("x%s\r", r3900_regnames[regno]);
  val = r3900_fetch_fields (bf);
  monitor_printf (".\r");
  monitor_expect_prompt (NULL, 0);

  /* supply register stores in target byte order, so swap here */

  store_unsigned_integer (regbuf, REGISTER_RAW_SIZE (regno), val);
  supply_register (regno, regbuf);

}

/* Fetch all registers (if regno is -1), or one register from the
   monitor.  For most registers, we can use the generic monitor_
   monitor_fetch_registers function.  But others are displayed in
   very unusual fashion and must be handled specially.  */

static void
r3900_fetch_registers (regno)
     int regno;
{
  switch (regno)
    {
    case BADVADDR_REGNUM:
      r3900_fetch_badvaddr ();
      return;
    case PS_REGNUM:
      r3900_fetch_bitmapped_register (PS_REGNUM, status_fields);
      return;
    case CAUSE_REGNUM:
      r3900_fetch_bitmapped_register (CAUSE_REGNUM, cause_fields);
      return;
    default:
      orig_monitor_fetch_registers (regno);
    }
}


/* Write the new value of the bitmapped register to the monitor.  */

static void
r3900_store_bitmapped_register (regno, bf)
     int regno;
     struct bit_field *bf;
{
  unsigned long oldval, newval;

  /* Fetch the current value of the register.  */
  monitor_printf ("x%s\r", r3900_regnames[regno]);
  oldval = r3900_fetch_fields (bf);
  newval = read_register (regno);

  /* To save time, write just the fields that have changed.  */
  for ( ; bf->prefix != NULL; bf++)
    {
      if (bf->length != 0)
        {
	  unsigned long oldbits, newbits, mask;

	  mask = (1 << bf->length) - 1;
	  oldbits = (oldval >> bf->start) & mask;
	  newbits = (newval >> bf->start) & mask;
	  if (oldbits != newbits)
	    monitor_printf ("%s %x ", bf->user_name, newbits);
	}
    }

  monitor_printf (".\r");
  monitor_expect_prompt (NULL, 0);
}

static void
r3900_store_registers (regno)
     int regno;
{
  switch (regno)
    {
    case PS_REGNUM:
      r3900_store_bitmapped_register (PS_REGNUM, status_fields);
      return;
    case CAUSE_REGNUM:
      r3900_store_bitmapped_register (CAUSE_REGNUM, cause_fields);
      return;
    default:
      orig_monitor_store_registers (regno);
    }
}

static void
r3900_load (monops, filename, from_tty)
     struct monitor_ops *monops;
     char *filename;
     int from_tty;
{
  extern int inferior_pid;

  generic_load (filename, from_tty);

  /* Finally, make the PC point at the start address */
  if (exec_bfd)
    write_pc (bfd_get_start_address (exec_bfd));

  inferior_pid = 0;             /* No process now */
}


static struct target_ops r3900_ops;

/* Commands to send to the monitor when first connecting:
    * The bare carriage return forces a prompt from the monitor
      (monitor doesn't prompt after a reset).
    * The "Xtr" command causes subsequent "t" (trace) commands to display
      the general registers only.
    * The "Xxr" command does the same thing for the "x" (examine
      registers) command.
    * The "bx" command clears all breakpoints.
*/

static char *r3900_inits[] = {"\r", "Xtr\r", "Xxr\r", "bx\r", NULL};

static struct monitor_ops r3900_cmds;

static void
r3900_open (args, from_tty)
     char *args;
     int from_tty;
{
  monitor_open (args, &r3900_cmds, from_tty);
}

void
_initialize_r3900_rom ()
{
  r3900_cmds.flags = MO_HANDLE_NL |
		     MO_NO_ECHO_ON_OPEN |
		     MO_ADDR_BITS_REMOVE |
		     MO_CLR_BREAK_USES_ADDR;

  r3900_cmds.init = r3900_inits;
  r3900_cmds.cont = "g\r";
  r3900_cmds.step = "t\r";
  r3900_cmds.set_break = "b %Lx\r";		/* COREADDR */
  r3900_cmds.clr_break = "b %Lx,0\r";		/* COREADDR */
  r3900_cmds.fill = "fx %Lx s %x %x\r";		/* COREADDR, len, val */

  r3900_cmds.setmem.cmdb = "sx %Lx %x\r";	/* COREADDR, val */
  r3900_cmds.setmem.cmdw = "sh %Lx %x\r";	/* COREADDR, val */
  r3900_cmds.setmem.cmdl = "sw %Lx %x\r";	/* COREADDR, val */

  r3900_cmds.getmem.cmdb = "dx %Lx s %x\r";	/* COREADDR, len */
  r3900_cmds.getmem.resp_delim = " : ";

  r3900_cmds.setreg.cmd = "x%s %x\r";		/* regname, val */

  r3900_cmds.getreg.cmd = "x%s\r";		/* regname */
  r3900_cmds.getreg.resp_delim = "=";
  r3900_cmds.getreg.term = " ";
  r3900_cmds.getreg.term_cmd = ".\r";

  r3900_cmds.dump_registers = "x\r";
  r3900_cmds.register_pattern =
	"\\([a-zA-Z0-9_]+\\) *=\\([0-9a-f]+ [0-9a-f]+\\b\\)";
  r3900_cmds.supply_register = r3900_supply_register;
  /* S-record download, via "keyboard port".  */
  r3900_cmds.load = "r0\r";
#if 0 /* FIXME - figure out how to get fast load to work */
  r3900_cmds.load_routine = r3900_load;
#endif
  r3900_cmds.prompt = "#";
  r3900_cmds.line_term = "\r";
  r3900_cmds.target = &r3900_ops;
  r3900_cmds.stopbits = SERIAL_1_STOPBITS;
  r3900_cmds.regnames = r3900_regnames;
  r3900_cmds.magic = MONITOR_OPS_MAGIC;

  init_monitor_ops (&r3900_ops);

  r3900_ops.to_shortname = "r3900";
  r3900_ops.to_longname = "R3900 monitor";
  r3900_ops.to_doc = "Debug using the DVE R3900 monitor.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).";
  r3900_ops.to_open = r3900_open;

  /* Override the functions to fetch and store registers.  But save the
     addresses of the default functions, because we will use those functions
     for "normal" registers.  */

  orig_monitor_fetch_registers = r3900_ops.to_fetch_registers;
  orig_monitor_store_registers = r3900_ops.to_store_registers;
  r3900_ops.to_fetch_registers = r3900_fetch_registers;
  r3900_ops.to_store_registers = r3900_store_registers;

  add_target (&r3900_ops);
}
