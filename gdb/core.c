/* Work with core dump and executable files, for GDB.
   Copyright (C) 1986, 1987, 1989 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include "defs.h"
#include "param.h"
#include "frame.h"  /* required by inferior.h */
#include "inferior.h"
#include "symtab.h"
#include "command.h"
#include "bfd.h"
#include "target.h"
#include "gdbcore.h"

extern int xfer_memory ();
extern void child_attach (), child_create_inferior ();

extern int sys_nerr;
extern char *sys_errlist[];
extern char *sys_siglist[];

extern char registers[];

/* Hook for `exec_file_command' command to call.  */

void (*exec_file_display_hook) () = NULL;

struct section_table *core_sections, *core_sections_end;

/* Binary file diddling handle for the core file.  */

bfd *core_bfd = NULL;

/* Forward decl */
extern struct target_ops core_ops;


/* Discard all vestiges of any previous core file
   and mark data and stack spaces as empty.  */

/* ARGSUSED */
void
core_close (quitting)
     int quitting;
{
  if (core_bfd) {
    free (bfd_get_filename (core_bfd));
    bfd_close (core_bfd);
    core_bfd = NULL;
#ifdef CLEAR_SOLIB
    CLEAR_SOLIB ();
#endif
  }
}

/* This routine opens and sets up the core file bfd */

void
core_open (filename, from_tty)
     char *filename;
     int from_tty;
{
  const char *p;
  int siggy;
  struct cleanup *old_chain;
  char *temp;
  bfd *temp_bfd;
  int ontop;

  target_preopen (from_tty);
  if (!filename)
    {
      error (core_bfd? 
       "No core file specified.  (Use `detach' to stop debugging a core file.)"
     : "No core file specified.");
    }

  filename = tilde_expand (filename);
  if (filename[0] != '/') {
    temp = concat (current_directory, "/", filename);
    free (filename);
    filename = temp;
  }

  old_chain = make_cleanup (free, filename);
  temp_bfd = bfd_openr (filename, NULL);
  if (temp_bfd == NULL)
    {
      perror_with_name (filename);
    }

  if (!bfd_check_format (temp_bfd, bfd_core))
    {
      bfd_close (temp_bfd);
      error ("\"%s\" does not appear to be a core dump", filename);
    }

  /* Looks semi-reasonable.  Toss the old core file and work on the new.  */

  discard_cleanups (old_chain);		/* Don't free filename any more */
  unpush_target (&core_ops);
  core_bfd = temp_bfd;
  old_chain = make_cleanup (core_close, core_bfd);

  validate_files ();

  /* Find the data section */
  if (build_section_table (core_bfd, &core_sections, &core_sections_end))
    error ("Can't find sections in `%s': %s", bfd_get_filename(core_bfd),
	   bfd_errmsg (bfd_error));

  ontop = !push_target (&core_ops);
  make_cleanup (unpush_target, &core_ops);

  p = bfd_core_file_failing_command (core_bfd);
  if (p)
    printf ("Core file invoked as `%s'.\n", p);

  siggy = bfd_core_file_failing_signal (core_bfd);
  if (siggy > 0)
    printf ("Program terminated with signal %d, %s.\n", siggy,
	    siggy < NSIG ? sys_siglist[siggy] : "(undocumented)");

  if (ontop) {
    /* Fetch all registers from core file */
    target_fetch_registers (-1);
    /* Add symbols for any shared libraries that were in use */
#ifdef SOLIB_ADD
    SOLIB_ADD (NULL, from_tty);
#endif
    /* Now, set up the frame cache, and print the top of stack */
    set_current_frame ( create_new_frame (read_register (FP_REGNUM),
					  read_pc ()));
    select_frame (get_current_frame (), 0);
    print_sel_frame (0);	/* Print the top frame and source line */
  } else {
    printf (
"Warning: you won't be able to access this core file until you terminate\n\
your %s; do ``info files''\n", current_target->to_longname);
  }

  discard_cleanups (old_chain);
}

void
core_detach (args, from_tty)
     char *args;
     int from_tty;
{
  if (args)
    error ("Too many arguments");
  unpush_target (&core_ops);
  if (from_tty)
    printf ("No core file now.\n");
}

/* Backward compatability with old way of specifying core files.  */

void
core_file_command (filename, from_tty)
     char *filename;
     int from_tty;
{
  dont_repeat ();			/* Either way, seems bogus. */
  if (!filename)
    core_detach (filename, from_tty);
  else
    core_open (filename, from_tty);
}


/* Call this to specify the hook for exec_file_command to call back.
   This is called from the x-window display code.  */

void
specify_exec_file_hook (hook)
     void (*hook) ();
{
  exec_file_display_hook = hook;
}

/* The exec file must be closed before running an inferior.
   If it is needed again after the inferior dies, it must
   be reopened.  */

void
close_exec_file ()
{
#ifdef FIXME
  if (exec_bfd)
    bfd_tempclose (exec_bfd);
#endif
}

void
reopen_exec_file ()
{
#ifdef FIXME
  if (exec_bfd)
    bfd_reopen (exec_bfd);
#endif
}

/* If we have both a core file and an exec file,
   print a warning if they don't go together.
   This should really check that the core file came
   from that exec file, but I don't know how to do it.  */

void
validate_files ()
{
  if (exec_bfd && core_bfd)
    {
      if (core_file_matches_executable_p (core_bfd, exec_bfd))
	printf ("Warning: core file does not match specified executable file.\n");
      else if (bfd_get_mtime(exec_bfd) > bfd_get_mtime(core_bfd))
	printf ("Warning: exec file is newer than core file.\n");
    }
}

/* Return the name of the executable file as a string.
   ERR nonzero means get error if there is none specified;
   otherwise return 0 in that case.  */

char *
get_exec_file (err)
     int err;
{
  if (exec_bfd) return bfd_get_filename(exec_bfd);
  if (!err)     return NULL;

  error ("No executable file specified.\n\
Use the \"file\" or \"exec-file\" command.");
  return NULL;
}

static void
core_files_info ()
{
  struct section_table *p;

  printf ("\tCore file `%s'.\n", bfd_get_filename(core_bfd));

  for (p = core_sections; p < core_sections_end; p++)
    printf("\tcore file  from 0x%08x to 0x%08x is %s\n",
	p->addr, p->endaddr,
	bfd_section_name (core_bfd, p->sec_ptr));
}

void
memory_error (status, memaddr)
     int status;
     CORE_ADDR memaddr;
{

  if (status == EIO)
    {
      /* Actually, address between memaddr and memaddr + len
	 was out of bounds. */
      error ("Cannot access memory at address 0x%x.", memaddr);
    }
  else
    {
      if (status >= sys_nerr || status < 0)
	error ("Error accessing memory address 0x%x: unknown error (%d).",
	       memaddr, status);
      else
	error ("Error accessing memory address 0x%x: %s.",
	       memaddr, sys_errlist[status]);
    }
}

/* Same as target_read_memory, but report an error if can't read.  */
void
read_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int status;
  status = target_read_memory (memaddr, myaddr, len);
  if (status != 0)
    memory_error (status, memaddr);
}

/* Same as target_write_memory, but report an error if can't write.  */
void
write_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int status;

  status = target_write_memory (memaddr, myaddr, len);
  if (status != 0)
    memory_error (status, memaddr);
}

/* Read an integer from debugged memory, given address and number of bytes.  */

long
read_memory_integer (memaddr, len)
     CORE_ADDR memaddr;
     int len;
{
  char cbuf;
  short sbuf;
  int ibuf;
  long lbuf;

  if (len == sizeof (char))
    {
      read_memory (memaddr, &cbuf, len);
      return cbuf;
    }
  if (len == sizeof (short))
    {
      read_memory (memaddr, (char *)&sbuf, len);
      SWAP_TARGET_AND_HOST (&sbuf, sizeof (short));
      return sbuf;
    }
  if (len == sizeof (int))
    {
      read_memory (memaddr, (char *)&ibuf, len);
      SWAP_TARGET_AND_HOST (&ibuf, sizeof (int));
      return ibuf;
    }
  if (len == sizeof (lbuf))
    {
      read_memory (memaddr, (char *)&lbuf, len);
      SWAP_TARGET_AND_HOST (&lbuf, sizeof (lbuf));
      return lbuf;
    }
  error ("Cannot handle integers of %d bytes.", len);
  return -1;	/* for lint */
}

/* Read or write the core file.

   Args are address within core file, address within gdb address-space,
   length, and a flag indicating whether to read or write.

   Result is a length:

	0:    We cannot handle this address and length.
	> 0:  We have handled N bytes starting at this address.
	      (If N == length, we did it all.)  We might be able
	      to handle more bytes beyond this length, but no
	      promises.
	< 0:  We cannot handle this address, but if somebody
	      else handles (-N) bytes, we can start from there.

   The actual work is done by xfer_memory in exec.c, which we share
   in common with exec_xfer_memory().  */

static int
core_xfer_memory (memaddr, myaddr, len, write)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
{
  int res;
  res = xfer_memory (memaddr, myaddr, len, write,
		      core_bfd, core_sections, core_sections_end);
#ifdef SOLIB_XFER_MEMORY
  if (res == 0)
    res = SOLIB_XFER_MEMORY (memaddr, myaddr, len, write);
#endif
  return res;
}

/* Get the registers out of a core file.  This is the machine-
   independent part.  Fetch_core_registers is the machine-dependent
   part, typically implemented in the xm-file for each architecture.  */

/* We just get all the registers, so we don't use regno.  */
/* ARGSUSED */
static void
get_core_registers (regno)
     int regno;
{
  sec_ptr reg_sec;
  unsigned size;
  char *the_regs;

  reg_sec = bfd_get_section_by_name (core_bfd, ".reg");
  size = bfd_section_size (core_bfd, reg_sec);
  the_regs = alloca (size);
  if (bfd_get_section_contents (core_bfd, reg_sec, the_regs,
				(unsigned)0, size))
    {
      fetch_core_registers (the_regs, size, 0);
    }
  else
    {
      fprintf (stderr, "Couldn't fetch registers from core file: %s\n",
	       bfd_errmsg (bfd_error));
    }

  /* Now do it again for the float registers, if they exist.  */
  reg_sec = bfd_get_section_by_name (core_bfd, ".reg2");
  if (reg_sec) {
    size = bfd_section_size (core_bfd, reg_sec);
    the_regs = alloca (size);
    if (bfd_get_section_contents (core_bfd, reg_sec, the_regs,
				  (unsigned)0, size))
      {
	fetch_core_registers (the_regs, size, 2);
      }
    else
      {
	fprintf (stderr, "Couldn't fetch register set 2 from core file: %s\n",
		 bfd_errmsg (bfd_error));
      }
  }
  registers_fetched();
}

struct target_ops core_ops = {
	"core", "Local core dump file",
	"Use a core file as a target.  Specify the filename of the core file.",
	core_open, core_close,
	child_attach, core_detach, 0, 0, /* resume, wait */
	get_core_registers, 
	0, 0, 0, 0, /* store_regs, prepare_to_store, conv_to, conv_from */
	core_xfer_memory, core_files_info,
	0, 0, /* core_insert_breakpoint, core_remove_breakpoint, */
	0, 0, 0, 0, 0, /* terminal stuff */
	0, 0, 0, 0, 0, /* kill, load, add_syms, call fn, lookup sym */
	child_create_inferior, 0, /* mourn_inferior */
	core_stratum, 0, /* next */
	0, 1, 1, 1, 0,	/* all mem, mem, stack, regs, exec */
	OPS_MAGIC,		/* Always the last thing */
};

void
_initialize_core()
{

  add_com ("core-file", class_files, core_file_command,
	   "Use FILE as core dump for examining memory and registers.\n\
No arg means have no core file.  This command has been superseded by the\n\
`target core' and `detach' commands.");
  add_target (&core_ops);
}
