/* Remote debugging interface for Zilog Z8000 simulator
   Copyright 1992,1993 Free Software Foundation, Inc.
   Contributed by Cygnus Support.  Written by Steve Chamberlain
   (sac@cygnus.com).

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
#include "inferior.h"
#include "wait.h"
#include "value.h"
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>
#include "terminal.h"
#include "target.h"
#include "gdbcore.h"
#include "../sim/z8k/sim.h"

/* External data declarations */
extern int stop_soon_quietly;	/* for wait_for_inferior */

/* Forward data declarations */
/*extern struct target_ops sim_ops;	/* Forward declaration */

void sim_store_register ();
void sim_set_oc ();

int inferior_pid;
int
sim_write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  sim_write (memaddr, myaddr, len);
  return 1;
}

static void
store_register (regno)
     int regno;
{
  if (regno == -1)
    {
      for (regno = 0; regno < 16; regno++)
	{
	  store_register (regno);
	}
    }
  else
    {
      sim_store_register (regno, read_register (regno));
    }
}

void
sim_kill (arg, from_tty)
     char *arg;
     int from_tty;
{
}

/*
 * Download a file specified in 'args', to the sim.
 */
static void
sasassim_load (args, fromtty)
     char *args;
     int fromtty;
{
  bfd *abfd;
  asection *s;

  inferior_pid = 0;
  abfd = bfd_openr (args, "coff-z8k");

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
      if (s->flags & SEC_LOAD)
	{
	  int i;
	  int delta = 4096;
	  char *buffer = xmalloc (delta);

	  printf_filtered ("%s\t: 0x%4x .. 0x%4x  ",
			   s->name, s->vma, s->vma + s->_raw_size);
	  for (i = 0; i < s->_raw_size; i += delta)
	    {
	      int sub_delta = delta;

	      if (sub_delta > s->_raw_size - i)
		sub_delta = s->_raw_size - i;

	      bfd_get_section_contents (abfd, s, buffer, i, sub_delta);
	      sim_write_inferior_memory (s->vma + i, buffer, sub_delta);
	      printf_filtered ("*");
	      gdb_flush (gdb_stdout);
	    }
	  printf_filtered ("\n");
	  free (buffer);
	}
      s = s->next;
    }

  sim_set_pc (abfd->start_address);
}

/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */
static void
sim_create_inferior (execfile, args, env)
     char *execfile;
     char *args;
     char **env;
{
  int entry_pt;

  if (args && *args)
    error ("Can't pass arguments to remote sim process.");

  if (execfile == 0 || exec_bfd == 0)
    error ("No exec file specified");

  entry_pt = (int) bfd_get_start_address (exec_bfd);

  sim_kill (NULL, NULL);
  sim_clear_breakpoints ();
  init_wait_for_inferior ();
  insert_breakpoints ();
  proceed (entry_pt, TARGET_SIGNAL_DEFAULT, 0);
}
#if 0
static void
sim_open (name, from_tty)
     char *name;
     int from_tty;
{
  if (name == 0)
    {
      name = "";
    }

  /* Clear any break points */
  sim_clear_breakpoints ();

  push_target (&sim_ops);
  target_fetch_registers (-1);

  printf_filtered ("Connected to the Z8000 Simulator.\n");
}

/* Close out all files and local state before this target loses control. */

static void
sim_close (quitting)
     int quitting;
{
  sim_clear_breakpoints ();
}

/* Terminate the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */
static void
sim_detach (args, from_tty)
     char *args;
     int from_tty;
{
  sim_clear_breakpoints ();

  pop_target ();		/* calls sim_close to do the real work */
  if (from_tty)
    printf_filtered ("Ending remote %s debugging\n", target_shortname);
}
#endif
/* Tell the remote machine to resume.  */

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */
#if 0
/* See remote-sim.c for how this is done now.  */
int
sim_wait (pid, status)
     int pid;
     WAITTYPE *status;
{
  sim_stop_reason (&reason, &sigrc);
  return inferior_pid;
}
#endif
/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that registers contains all the registers from the program being
   debugged.  */

static void
sim_prepare_to_store ()
{
  /* Do nothing, since we can store individual regs */
}

static void
fetch_register (regno)
     int regno;
{
  if (regno == -1)
    {
      for (regno = 0; regno < 16; regno++)
	fetch_register (regno);
    }
  else
    {
      char buf[MAX_REGISTER_RAW_SIZE];

      sim_fetch_register (regno, buf);
      supply_register (regno, buf);
    }
}

/* Write a word WORD into remote address ADDR.
   This goes through the data cache.  */

void
sim_store_word (addr, word)
     CORE_ADDR addr;
     int word;
{
/*	dcache_poke (addr, word);*/
}

int
sim_xfer_inferior_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;	/* ignored */
{
  if (write)
    {
      sim_write (memaddr, myaddr, len);

    }
  else
    {
      sim_read (memaddr, myaddr, len);
    }
  return len;
}

void
sim_files_info ()
{
  char *file = "nothing";

  if (exec_bfd)
    file = bfd_get_filename (exec_bfd);

  printf_filtered ("\tAttached to %s running on the z8k simulator\n", file);
}

/* This routine is run as a hook, just before the main command loop is
   entered.  If gdb is configured for the H8, but has not had its
   target specified yet, this will loop prompting the user to do so.
*/

#if 0
void
sim_before_main_loop ()
{
  push_target (&sim_ops);
}


/* Clear the sims notion of what the break points are */
static void
sim_mourn ()
{
  sim_clear_breakpoints ();
  unpush_target (&sim_ops);
  generic_mourn_inferior ();
}
#endif
static void 
rem_resume (pid, a, b)
     int pid;
     int a;
     enum target_signal siggnal;
{
  sim_resume (a, target_signal_to_host (siggnal));
}


/* Define the target subroutine names */
#if 0
struct target_ops sim_ops =
{
  "sim", "Remote SIM monitor",
  "Use the Z8000 simulator",
  sim_open, sim_close,
  0, sim_detach, rem_resume, sim_wait,	/* attach */
  fetch_register, store_register,
  sim_prepare_to_store,
  sim_xfer_inferior_memory,
  sim_files_info,
  0, 0,				/* Breakpoints */
  0, 0, 0, 0, 0,		/* Terminal handling */
  sim_kill,			/* FIXME, kill */
  sim_load,
  0,				/* lookup_symbol */
  sim_create_inferior,		/* create_inferior */
  sim_mourn,			/* mourn_inferior FIXME */
  0,				/* can_run */
  0,				/* notice_signals */
  process_stratum, 0,		/* next */
  1, 1, 1, 1, 1,		/* all mem, mem, stack, regs, exec */
  0, 0,				/* Section pointers */
  OPS_MAGIC,			/* Always the last thing */
};

/***********************************************************************/

void
_initialize_remote_sim ()
{
  add_target (&sim_ops);
}
#endif
