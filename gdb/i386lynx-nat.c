/* Native-dependent code for Lynx running on i386's, for GDB.
   Copyright 1988, 1989, 1991, 1992, 1993
   Free Software Foundation, Inc.

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
#include "frame.h"
#include "inferior.h"
#include "gdbcore.h"
#include "target.h"

#include <sys/ptrace.h>
#include "/usr/include/sys/wait.h"

/* these values indicate the offset of the named register in the econtext
   structure */

#define	EAX	10
#define	ECX	9
#define	EDX	8
#define	EBX	7
#define	ESP	16
#define	EBP	5
#define	ESI	4
#define	EDI	3
#define	EIP	13
#define	EFL	15
#define	CS	14
#define	SS	17
#define	DS	2
#define	ES	1

/* Currently these are not being used. So set them to 0 */

#define	FS	0
#define	GS	0

static struct target_ops lynx_core_ops;

/* this table must line up with REGISTER_NAMES in m-i386.h */
static unsigned int regmap[] = 
{
  EAX, ECX, EDX, EBX,
  ESP, EBP, ESI, EDI,
  EIP, EFL, CS, SS,
  DS, ES, FS, GS,
};

/* Return the address in the core dump or inferior of register REGNO.
   BLOCKEND is the address of the econtext structure */

static unsigned int
register_addr (regno, blockend)
     int regno, blockend;
{
  if (regno < 0 || regno >= NUM_REGS)
    error ("Invalid register number %d.", regno);

  return (blockend + regmap[regno] * sizeof (long));
}

/* Fetch one register.  */

static void
fetch_register (regno, offset, bpid)
     int regno, bpid;
     unsigned int offset;
{
  unsigned int regaddr;
  char buf[MAX_REGISTER_RAW_SIZE];
  char mess[128];				/* For messages */
  int i;

  regaddr = register_addr (regno, offset);
  for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
    {
      errno = 0;
      *(int *) &buf[i] = ptrace (PTRACE_PEEKTHREAD, bpid,
				 (PTRACE_ARG3_TYPE) regaddr, 0);
      regaddr += sizeof (int);
      if (errno != 0)
	{
	  sprintf (mess, "reading register %s (#%d)", reg_names[regno], regno);
	  perror_with_name (mess);
	}
    }
  supply_register (regno, buf);
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

static void
store_register (regno, offset, bpid)
     int regno, bpid;
     unsigned int offset;
{
  unsigned int regaddr;
  char mess[128];
  extern char registers[];
  int i;

  regaddr = register_addr (regno, offset);
  for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof(int))
    {
      errno = 0;
      ptrace (PTRACE_POKEUSER, bpid, (PTRACE_ARG3_TYPE) regaddr,
	      *(int *) &registers[REGISTER_BYTE (regno) + i]);
      if (errno != 0)
        {  
	  sprintf (mess, "writing register number %d(%d)", regno, i);
	  perror_with_name (mess);
        }
      regaddr += sizeof(int);
    }
}

/* return an offset for use with register_addr() */

static unsigned int
fetch_offset (pid)
    int pid;
{
  struct st_entry s;
  unsigned int specpage_off, offset = (char *) &s.ecp - (char *) &s;

  errno = 0;
  specpage_off = ptrace (PTRACE_THREADUSER, pid, (PTRACE_ARG3_TYPE) 0, 0);
  if (errno != 0)
    perror_with_name ("ptrace");
  errno = 0;
  offset = ptrace (PTRACE_PEEKTHREAD, pid, (PTRACE_ARG3_TYPE) offset, 0)
      - specpage_off;
  if (errno != 0)
    perror_with_name ("ptrace");
  return offset;
}

/* Fetch all registers, or just one, from the child process.  */

void
fetch_inferior_registers (regno)
     int regno;
{
  unsigned int offset = fetch_offset (inferior_pid);

  if (regno == -1)
    {
      for (regno = 0; regno < NUM_REGS; regno++)
        fetch_register (regno, offset, inferior_pid);
    }
  else
    fetch_register (regno, offset, inferior_pid);
}

/* Store all registers, or just one, to the child process.  */

void
store_inferior_registers (regno)
     int regno;
{
    unsigned int offset = fetch_offset (inferior_pid);

    if (regno == -1)
      {
        for (regno = 0; regno < NUM_REGS; regno++)
	  store_register (regno, offset, inferior_pid);
      }
    else
      store_register (regno, offset, inferior_pid);
}

/* Wait for child to do something.  Return pid of child, or -1 in case
   of error; store status through argument pointer STATUS.  */

int
child_wait (status)
     int *status;
{
  int pid;
  int save_errno;
  int thread;

  while (1)
    {
      int sig;

      if (attach_flag)
	set_sigint_trap();	/* Causes SIGINT to be passed on to the
				   attached process. */
      pid = wait (status);
      save_errno = errno;

      if (attach_flag)
	clear_sigint_trap();

      if (pid == -1)
	{
	  if (save_errno == EINTR)
	    continue;
	  fprintf (stderr, "Child process unexpectedly missing: %s.\n",
		   safe_strerror (save_errno));
	  *status = 42;		/* Claim it exited with signal 42 */
	  return -1;
	}

      if (pid != PIDGET (inferior_pid))	/* Some other process?!? */
	continue;

/*      thread = WIFTID (*status);*/
      thread = *status >> 16;

      /* Initial thread value can only be acquired via wait, so we have to
	 resort to this hack.  */

      if (TIDGET (inferior_pid) == 0)
	{
	  inferior_pid = BUILDPID (inferior_pid, thread);
	  add_thread (inferior_pid);
	}

      pid = BUILDPID (pid, thread);

      return pid;
    }
}

/* Convert a Lynx process ID to a string.  Returns the string in a static
   buffer.  */

char *
i386lynx_pid_to_str (pid)
     int pid;
{
  static char buf[40];

  sprintf (buf, "process %d thread %d", PIDGET (pid), TIDGET (pid));

  return buf;
}

/* Extract the register values out of the core file and store
   them where `read_register' will find them.

   CORE_REG_SECT points to the register values themselves, read into memory.
   CORE_REG_SIZE is the size of that area.
   WHICH says which set of registers we are handling (0 = int, 2 = float
         on machines where they are discontiguous).
   REG_ADDR is the offset from u.u_ar0 to the register values relative to
            core_reg_sect.  This is used with old-fashioned core files to
	    locate the registers in a large upage-plus-stack ".reg" section.
	    Original upage address X is at location core_reg_sect+x+reg_addr.
 */

void
fetch_core_registers (core_reg_sect, core_reg_size, which, reg_addr)
     char *core_reg_sect;
     unsigned core_reg_size;
     int which;
     unsigned reg_addr;
{
  struct st_entry s;
  unsigned int regno, addr;

  for (regno = 0; regno < NUM_REGS; regno++)
    {
      addr = register_addr (regno, (char *) &s.ec - (char *) &s);
      supply_register (regno, core_reg_sect + addr);
    }
}

#if 0

/* Discard all vestiges of any previous core file
   and mark data and stack spaces as empty.  */

/* ARGSUSED */
static void
lynx_core_close (quitting)
     int quitting;
{
  if (core_bfd)
    {
      free (bfd_get_filename (core_bfd));
      bfd_close (core_bfd);
      core_bfd = NULL;

      if (core_ops.to_sections)
	{
	  free ((PTR)lynx_core_ops.to_sections);
	  lynx_core_ops.to_sections = NULL;
	  lynx_core_ops.to_sections_end = NULL;
	}
    }
}

/* This routine opens and sets up the core file bfd */

static void
lynx_core_open (filename, from_tty)
     char *filename;
     int from_tty;
{
  const char *p;
  int siggy;
  struct cleanup *old_chain;
  char *temp;
  bfd *temp_bfd;
  int ontop;
  int scratch_chan;

  target_preopen (from_tty);
  if (!filename)
    {
      error (core_bfd ? 
	     "No core file specified.  (Use `detach' to stop debugging a core file.)"
	     : "No core file specified.");
    }

  filename = tilde_expand (filename);
  if (filename[0] != '/')
    {
      temp = concat (current_directory, "/", filename, NULL);
      free (filename);
      filename = temp;
    }

  old_chain = make_cleanup (free, filename);

  scratch_chan = open (filename, write_files? O_RDWR: O_RDONLY, 0);
  if (scratch_chan < 0)
    perror_with_name (filename);

  temp_bfd = bfd_fdopenr (filename, NULL, scratch_chan);
  if (temp_bfd == NULL)
    perror_with_name (filename);

  if (!bfd_check_format (temp_bfd, bfd_core))
    {
      /* Do it after the err msg */
      make_cleanup (bfd_close, temp_bfd);
      error ("\"%s\" is not a core dump: %s", filename, bfd_errmsg(bfd_error));
    }

  /* Looks semi-reasonable.  Toss the old core file and work on the new.  */

  discard_cleanups (old_chain);		/* Don't free filename any more */
  unpush_target (&core_ops);
  core_bfd = temp_bfd;
  old_chain = make_cleanup (core_close, core_bfd);

  validate_files ();

  /* Find the data section */
  if (build_section_table (core_bfd, &core_ops.to_sections,
			   &core_ops.to_sections_end))
    error ("Can't find sections in `%s': %s", bfd_get_filename(core_bfd),
	   bfd_errmsg (bfd_error));

  ontop = !push_target (&core_ops);
  discard_cleanups (old_chain);

  p = bfd_core_file_failing_command (core_bfd);
  if (p)
    printf_filtered ("Core was generated by `%s'.\n", p);

  siggy = bfd_core_file_failing_signal (core_bfd);
  if (siggy > 0)
    printf_filtered ("Program terminated with signal %d, %s.\n", siggy,
	    safe_strsignal (siggy));

  /* Locate all of the thread register sections.  They have names like .regxx,
     where xx is the thread-id.  */

  bfd_map_over_sections (core_bfd, grok_register_sections, 

  if (ontop)
    {
      /* Fetch all registers from core file */
      target_fetch_registers (-1);

      /* Now, set up the frame cache, and print the top of stack */
      set_current_frame (create_new_frame (read_fp (),
					   read_pc ()));
      select_frame (get_current_frame (), 0);
      print_stack_frame (selected_frame, selected_frame_level, 1);
    }
  else
    {
      warning (
	       "you won't be able to access this core file until you terminate\n\
your %s; do ``info files''", current_target->to_longname);
    }
}

static void
lynx_core_detach (args, from_tty)
     char *args;
     int from_tty;
{
  if (args)
    error ("Too many arguments");
  unpush_target (&core_ops);
  if (from_tty)
    printf_filtered ("No core file now.\n");
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
  char regsecname[20];

  sprintf (regsecname, ".reg%d", TIDGET (inferior_pid));

  reg_sec = bfd_get_section_by_name (core_bfd, regsecname);
  if (!reg_sec)
    goto cant;
  size = bfd_section_size (core_bfd, reg_sec);
  the_regs = alloca (size);
  if (bfd_get_section_contents (core_bfd, reg_sec, the_regs, (file_ptr)0,
				size))
    {
      fetch_core_registers (the_regs, size, 0,
			    (unsigned) bfd_section_vma (abfd,reg_sec));
    }
  else
    {
cant:
      fprintf_filtered (stderr, "Couldn't fetch registers from core file: %s\n",
	       bfd_errmsg (bfd_error));
    }

  registers_fetched();
}

static void
core_files_info (t)
  struct target_ops *t;
{
  print_section_info (t, core_bfd);
}

/* If mourn is being called in all the right places, this could be say
   `gdb internal error' (since generic_mourn calls mark_breakpoints_out).  */

static int
ignore (addr, contents)
     CORE_ADDR addr;
     char *contents;
{
}

static struct target_ops
lynx_core_ops =
{
  "core",
  "Local core dump file",
  "Use a core file as a target.  Specify the filename of the core file.",
  lynx_core_open,
  lynx_core_close,
  find_default_attach,
  lynx_core_detach,
  0,
  0,
  get_core_registers, 
  0,
  0,
  xfer_memory,
  lynx_core_files_info,
  ignore,
  ignore,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  find_default_create_inferior,
  0,				/* mourn_inferior */
  0,				/* can_run */
  0,				/* notice_signals */
  core_stratum,
  0,				/* next */
  0,
  1,
  1,
  1,
  0,
  0,
  0,
  OPS_MAGIC,			/* Always the last thing */
};

void
_initialize_i386lynx_nat()
{
  add_target (&lynx_core_ops);
}
#endif
