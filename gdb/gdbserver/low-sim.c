/* Low level interface to simulators, for the remote server for GDB.
   Copyright (C) 1995, 1996 Free Software Foundation, Inc.

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
#include "bfd.h"
#include "server.h"
#include "callback.h"   /* GDB simulator callback interface */
#include "remote-sim.h" /* GDB simulator interface */

extern host_callback default_callback;	/* in callback.c */

char registers[REGISTER_BYTES];

int target_byte_order;	/* used by simulator */

/* This version of "load" should be usable for any simulator that
   does not support loading itself.  */

static void
generic_load (loadfile_bfd)
    bfd *loadfile_bfd;
{
  asection *s;

  for (s = loadfile_bfd->sections; s; s = s->next) 
    {
      if (s->flags & SEC_LOAD) 
	{
	  bfd_size_type size;

	  size = bfd_get_section_size_before_reloc (s);
	  if (size > 0)
	    {
	      char *buffer;
	      bfd_vma vma;

	      buffer = xmalloc (size);
	      vma = bfd_get_section_vma (loadfile_bfd, s);

	      /* Is this really necessary?  I guess it gives the user something
		 to look at during a long download.  */
	      fprintf (stderr, "Loading section %s, size 0x%lx vma 0x%lx\n",
		       bfd_get_section_name (loadfile_bfd, s),
		       (unsigned long) size,
		       (unsigned long) vma); /* chops high 32 bits.  FIXME!! */

	      bfd_get_section_contents (loadfile_bfd, s, buffer, 0, size);

	      write_inferior_memory (vma, buffer, size);
	      free (buffer);
	    }
	}
    }

  fprintf (stderr, "Start address 0x%lx\n",
	   (unsigned long)loadfile_bfd->start_address);

  /* We were doing this in remote-mips.c, I suspect it is right
     for other targets too.  */
  /* write_pc (loadfile_bfd->start_address); */	/* FIXME!! */
}

int
create_inferior (program, allargs)
     char *program;
     char **allargs;
{
  bfd *abfd;
  int pid = 0;

  abfd = bfd_openr (program, 0);
  if (!abfd) 
    {
      fprintf (stderr, "gdbserver: can't open %s: %s\n", 
	       program, bfd_errmsg (bfd_get_error ()));
      exit (1);
    }

  if (!bfd_check_format (abfd, bfd_object))
    {
      fprintf (stderr, "gdbserver: unknown load format for %s: %s\n",
	       program, bfd_errmsg (bfd_get_error ()));
      exit (1);
    }

  /* This must be set before sim_open is called, because gdb assumes that
     the simulator endianness is known immediately after the sim_open call.  */
  target_byte_order = bfd_big_endian (abfd) ? 4321 : 1234;

  sim_set_callbacks (&default_callback);
  default_callback.init (&default_callback);

  /* Should concatenate args here.  FIXME!! */
  sim_open (allargs[0]);

  /* Load program.  */
  if (sim_load (allargs[0], 0) != 0)
    generic_load (abfd);

  return pid;
}

/* Kill the inferior process.  Make us have no inferior.  */

void
kill_inferior ()
{
  sim_close (0);
  default_callback.shutdown (&default_callback);
}

/* Fetch one register.  */

static void
fetch_register (regno)
     int regno;
{
  sim_fetch_register (regno, &registers[REGISTER_BYTE (regno)]);
}

/* Fetch all registers, or just one, from the child process.  */

void
fetch_inferior_registers (regno)
     int regno;
{
  if (regno == -1 || regno == 0)
    for (regno = 0; regno < NUM_REGS/*-NUM_FREGS*/; regno++)
      fetch_register (regno);
  else
    fetch_register (regno);
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

void
store_inferior_registers (regno)
     int regno;
{
  if (regno  == -1) 
    {
      for (regno = 0; regno < NUM_REGS; regno++)
	store_inferior_registers (regno);
    }
  else
    sim_store_register (regno, &registers[REGISTER_BYTE (regno)]);
}

/* Return nonzero if the given thread is still alive.  */
int
mythread_alive (pid)
     int pid;
{
  return 1;
}

/* Wait for process, returns status */

unsigned char
mywait (status)
     char *status;
{
  int sigrc;
  enum sim_stop reason;

  sim_stop_reason (&reason, &sigrc);
  switch (reason)
    {
    case sim_exited:
      fprintf (stderr, "\nChild exited with retcode = %x \n", sigrc);
      *status = 'W';
      return sigrc;

#if 0
    case sim_stopped:
      fprintf (stderr, "\nChild terminated with signal = %x \n", sigrc);
      *status = 'X';
      return sigrc;
#endif

    default:   /* should this be sim_signalled or sim_stopped?  FIXME!! */
      fprintf (stderr, "\nChild received signal = %x \n", sigrc);
      fetch_inferior_registers (0);
      *status = 'T';
      return (unsigned char) sigrc;
    }
}

/* Resume execution of the inferior process.
   If STEP is nonzero, single-step it.
   If SIGNAL is nonzero, give it that signal.  */

void
myresume (step, signo)
     int step;
     int signo;
{
  /* Should be using target_signal_to_host() or signal numbers in target.h
     to convert GDB signal number to target signal number.  */
  sim_resume (step, signo);
}

/* Copy LEN bytes from inferior's memory starting at MEMADDR
   to debugger memory starting at MYADDR.  */

void
read_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  sim_read (memaddr, myaddr, len);
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.
   On failure (cannot write the inferior)
   returns the value of errno.  */

int
write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  sim_write (memaddr, myaddr, len);  /* should check for error.  FIXME!! */
  return 0;
}

#if 0
void
initialize ()
{
  inferior_pid = 0;
}

int
have_inferior_p ()
{
  return inferior_pid != 0;
}
#endif
