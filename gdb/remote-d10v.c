/* Remote target communications for d10v connected via a serial line.
   Copyright 1988, 1991, 1992, 1993, 1994, 1995, 1996, 1997 Free
   Software Foundation, Inc.

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
/*#include "terminal.h"*/
#include "gdbcmd.h"
#include "objfiles.h"
#include "gdb-stabs.h"
#include "gdbthread.h"

#include "dcache.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <signal.h>
#include "serial.h"

/* Prototypes for local functions */

static int remote_d10v_xfer_memory PARAMS ((CORE_ADDR memaddr, char *myaddr,
					    int len, int should_write,
					    struct target_ops *target));

static void remote_d10v_open PARAMS ((char *name, int from_tty));

static void remote_d10v_mourn PARAMS ((void));

static void initialize_remote_d10v_ops PARAMS ((int copy_remote));

/* Define the target subroutine names */

struct target_ops remote_d10v_ops;
extern struct target_ops remote_ops;
extern struct target_ops extended_remote_ops;
struct target_ops *inherited_ops = &extended_remote_ops;

void
init_remote_d10v_ops (copy_remote)
     int copy_remote;
{
  printf_filtered ("Opening d10v ...\n");
  if (copy_remote)
    memcpy (&remote_d10v_ops, inherited_ops, sizeof remote_d10v_ops);
  remote_d10v_ops.to_shortname = "d10v";
  remote_d10v_ops.to_longname = "Remote d10v serial target in gdb-specific protocol";
  remote_d10v_ops.to_doc = "Use a remote d10v via a serial line, using a gdb-specific protocol.\n\
Specify the serial device it is connected to (e.g. /dev/ttya).";
  remote_d10v_ops.to_open = remote_d10v_open;
  remote_d10v_ops.to_xfer_memory = remote_d10v_xfer_memory;
  remote_d10v_ops.to_mourn_inferior = remote_d10v_mourn;
  remote_d10v_ops.to_magic = OPS_MAGIC;
}


/* Open a connection to a remote debugger.
   NAME is the filename used for communication.  */

static void
remote_d10v_open (name, from_tty)
     char *name;
     int from_tty;
{
  init_remote_d10v_ops (1);
  open_remote_target (name, from_tty, &remote_d10v_ops,
		      inherited_ops == &extended_remote_ops);
}


/* Worker function for remote_mourn.  */
static void
remote_d10v_mourn ()
{
  if (inherited_ops == &remote_ops)
    {
      unpush_target (&remote_d10v_ops);
      generic_mourn_inferior ();
    }
  /* see remote.c:extended_remote_mourn() for why an extended remote
     target doesn't mourn */
}


/* Read or write LEN bytes from inferior memory at MEMADDR, transferring
   to or from debugger address MYADDR.  Write to inferior if SHOULD_WRITE is
   nonzero.  Returns length of data written or read; 0 for error.  */

/* ARGSUSED */
static int
remote_d10v_xfer_memory(memaddr, myaddr, nr_bytes, write_p, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int nr_bytes;
     int write_p;
     struct target_ops *target;			/* ignored */
{
  CORE_ADDR phys;
  CORE_ADDR seg;
  CORE_ADDR off;
  char *from = "unknown";
  char *to = "unknown";
  unsigned short imap0 = read_register (IMAP0_REGNUM);
  unsigned short imap1 = read_register (IMAP1_REGNUM);
  unsigned short dmap = read_register (DMAP_REGNUM);

  /* GDB interprets addresses as:

       0x00xxxxxx: Logical data address segment        (DMAP translated memory)
       0x01xxxxxx: Logical instruction address segment (IMAP translated memory)
       0x10xxxxxx: Physical data memory segment        (On-chip data memory)
       0x11xxxxxx: Physical instruction memory segment (On-chip insn memory)
       0x12xxxxxx: Phisical unified memory segment     (Unified memory)

     The remote d10v board interprets addresses as:

       0x00xxxxxx: Phisical unified memory segment     (Unified memory)
       0x01xxxxxx: Physical instruction memory segment (On-chip insn memory)
       0x02xxxxxx: Physical data memory segment        (On-chip data memory)

     Translate according to current IMAP/dmap registers */

  enum {
    targ_unified = 0x00000000,
    targ_insn = 0x01000000,
    targ_data = 0x02000000,
  };

  seg = (memaddr >> 24);
  off = (memaddr & 0xffffffL);

  switch (seg) 
      {
      case 0x00: /* in logical data address segment */
	{
	  from = "logical-data";
	  if (off <= 0x7fffL)
	    {
	      /* On chip data */
	      phys = targ_data + off;
	      if (off + nr_bytes > 0x7fffL)
		/* don't cross VM boundary */
		nr_bytes = 0x7fffL - off + 1;
	      to = "chip-data";
	    }
	  else if (off <= 0xbfffL)
	    {
	      short map = dmap;
	      if (map & 0x1000)
		{
		  /* Instruction memory */
		  phys = targ_insn | ((map & 0xf) << 14) | (off & 0x3fff);
		  to = "chip-insn";
		}
	      else
		{
		  /* Unified memory */
		  phys = targ_unified | ((map & 0x3ff) << 14) | (off & 0x3fff);
		  to = "unified";
		}
	      if (off + nr_bytes > 0xbfffL)
		/* don't cross VM boundary */
		nr_bytes = (0xbfffL - off + 1);
	    }	    
	  else
	    {
	      /* Logical address out side of data segments, not supported */
	      return (0);
	    }
	  break;
	}

      case 0x01: /* in logical instruction address segment */
	{
	  short map;
	  from = "logical-insn";
	  if (off <= 0x1ffffL)
	    {
	      map = imap0;
	    }
	  else if (off <= 0x3ffffL)
	    {
	      map = imap1;
	    }
	  else
	    {
	      /* Logical address outside of IMAP[01] segment, not
		 supported */
	      return (0);
	    }
	  if ((off & 0x1ffff) + nr_bytes > 0x1ffffL)
	    {
	      /* don't cross VM boundary */
	      nr_bytes = 0x1ffffL - (off & 0x1ffffL) + 1;
	    }
	  if (map & 0x1000)
	    /* Instruction memory */
	    {
	      phys = targ_insn | off;
	      to = "chip-insn";
	    }
	  else
	    {
	      phys = ((map & 0x7fL) << 17) + (off & 0x1ffffL);
	      if (phys > 0xffffffL)
		/* Address outside of unified address segment */
		return (0);
	      phys |= targ_unified;
	      to = "unified";
	    }
	  break;
	}

      case 0x10: /* Physical data memory segment */
	from = "phys-data";
	phys = targ_data | off;
	to = "chip-data";
	break;

      case 0x11: /* Physical instruction memory */
	from = "phys-insn";
	phys = targ_insn | off;
	to = "chip-insn";
	break;

      case 0x12: /* Physical unified memory */
	from = "phys-unified";
	phys = targ_unified | off;
	to = "unified";
	break;

      default:
	return (0);
      }


  printf_unfiltered ("%s-xfer: 0x%08lx -> 0x%08lx imap0=%04x imap1=%04x, dmap=%04x, %s->%s, %d bytes\n",
		     (write_p ? "wr" : "rd"),
		     (long) memaddr,
		     (long) phys,
		     (int) imap0, (int) imap1, (int) dmap,
		     from, to,
		     (int) nr_bytes);

  return inherited_ops->to_xfer_memory (phys, myaddr, nr_bytes, write_p, target);
}


void
_initialize_remote_d10v ()
{
  init_remote_d10v_ops (0);
  add_target (&remote_d10v_ops);
}
