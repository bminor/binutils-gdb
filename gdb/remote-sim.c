/* Remote debugging interface for generalized simulator
   Copyright 1992 Free Software Foundation, Inc.
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

/* Forward data declarations */
extern struct target_ops sim_ops;		/* Forward declaration */

int
sim_write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     unsigned char *myaddr;
     int len;
{
  return  sim_write(memaddr, myaddr, len);
}

static void
store_register(regno)
int regno;
{
  if (regno  == -1) 
  {
    for (regno = 0; regno < NUM_REGS; regno++)
     store_register(regno);
  }
  else 
  {
    sim_store_register(regno, read_register(regno));
  }
}


/*
 * Download a file specified in 'args', to the sim. 
 */
static void
sim_load(args,fromtty)
char	*args;
int	fromtty;
{
  bfd	*abfd;
  asection *s;

  inferior_pid = 0;  
  abfd = bfd_openr (args, (char*)gnutarget);

  if (!abfd) 
  {
    printf_filtered("Unable to open file %s\n", args);
    return;
  }

  if (bfd_check_format(abfd, bfd_object) ==0) 
  {
    printf_filtered("File is not an object file\n");
    return ;
  }

  s = abfd->sections;
  while (s != (asection *)NULL) 
  {
    if (s->flags & SEC_LOAD) 
    {
      int i;
      int delta = 4096;
      char *buffer = xmalloc(delta);
      printf_filtered("%s\t: 0x%4x .. 0x%4x  ",
		      s->name, s->vma, s->vma + s->_raw_size);
      for (i = 0; i < s->_raw_size; i+= delta) 
      {
	int sub_delta = delta;
	if (sub_delta > s->_raw_size - i)
	 sub_delta = s->_raw_size - i ;

	bfd_get_section_contents(abfd, s, buffer, i, sub_delta);
	sim_write_inferior_memory(s->vma + i, buffer, sub_delta);
	printf_filtered("*");
	fflush(stdout);
      }
      printf_filtered(  "\n");      
      free(buffer);
    }
    s = s->next;
  }

  sim_store_register(PC_REGNUM, abfd->start_address);
}

/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */
void
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
  init_wait_for_inferior ();
  insert_breakpoints ();
  proceed(entry_pt, -1, 0);
}



static void
sim_open (name, from_tty)
     char *name;
     int from_tty;
{
  if(name == 0) 
  {
    name = "";
  }
  push_target (&sim_ops);
  target_fetch_registers(-1);
  printf_filtered("Connected to the simulator.\n");
}

/* Close out all files and local state before this target loses control. */

static void
sim_close (quitting)
     int quitting;
{
}

/* Terminate the open connection to the remote debugger.
   Use this when you want to detach and do something else
   with your gdb.  */
void
sim_detach (args,from_tty)
     char *args;
     int from_tty;
{
  pop_target();			/* calls sim_close to do the real work */
  if (from_tty)
   printf_filtered ("Ending remote %s debugging\n", target_shortname);

}
 
/* Tell the remote machine to resume.  */


/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

int
sim_wait (status)
     WAITTYPE *status;
{
  WSETSTOP(*status, sim_stop_signal());  
  return 0;
}

static void
fetch_register(regno)
     int regno;
{
  if (regno  == -1) 
    {
      for (regno = 0; regno < NUM_REGS; regno++)
	fetch_register(regno);
    }
  else 
    {
      char buf[MAX_REGISTER_RAW_SIZE];

      sim_fetch_register(regno, buf);
      supply_register(regno, buf);
    }
}


int
sim_xfer_inferior_memory(memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;			/* ignored */
{
  if (write)
  {
    sim_write(memaddr, myaddr, len);
  }
  else 
  {
    sim_read(memaddr, myaddr, len);
  } 
  return len;
}


/* This routine is run as a hook, just before the main command loop is
   entered.  If gdb is configured for the H8, but has not had its
   target specified yet, this will loop prompting the user to do so.
*/

void
sim_before_main_loop ()
{
  push_target (&sim_ops);
}


static void rem_resume(pid, a , b)
{
  sim_resume(a,b);
}

void
pstore()
{
}
/* Define the target subroutine names */

struct target_ops sim_ops = 
{
  "sim", "simulator",
  "Use the simulator",
  sim_open, sim_close, 
  0, sim_detach, rem_resume, sim_wait, /* attach */
  fetch_register, store_register,
  pstore,
  sim_xfer_inferior_memory, 
  0,
  0, 0, /* Breakpoints */
  0, 0, 0, 0, 0,		/* Terminal handling */
  pstore,
  sim_load, 
  0,				/* lookup_symbol */
  sim_create_inferior,		/* create_inferior */ 
  pstore,			/* mourn_inferior FIXME */
  0,				/* can_run */
  0,				/* notice_signals */
  process_stratum, 0,		/* next */
  1, 1, 1, 1, 1,		/* all mem, mem, stack, regs, exec */
  0,0,				/* Section pointers */
  OPS_MAGIC,			/* Always the last thing */
};

/***********************************************************************/

void
_initialize_remote_sim ()
{
  add_target (&sim_ops);
}


