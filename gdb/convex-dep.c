/* Low level interface to ptrace, for GDB when running under Unix.
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

#include "defs.h"
#include "param.h"
#include "frame.h"
#include "inferior.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <stdio.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <a.out.h>
#include <sys/file.h>
#include <sys/stat.h>

extern int errno;

/* This function simply calls ptrace with the given arguments.  
   It exists so that all calls to ptrace are isolated in this 
   machine-dependent file. */
int
call_ptrace (request, pid, arg3, arg4)
     int request, pid, arg3, arg4;
{
  return ptrace (request, pid, arg3, arg4);
}

kill_inferior ()
{
  if (remote_debugging)
    return;
  if (inferior_pid == 0)
    return;
  /* Convex ptrace needs an extra 0 arg */
  ptrace (8, inferior_pid, 0, 0, 0);
  wait (0);
  inferior_died ();
}

/* This is used when GDB is exiting.  It gives less chance of error.*/

kill_inferior_fast ()
{
  if (remote_debugging)
    return;
  if (inferior_pid == 0)
    return;
  ptrace (8, inferior_pid, 0, 0);
  wait (0);
}

/* Resume execution of the inferior process.
   If STEP is nonzero, single-step it.
   If SIGNAL is nonzero, give it that signal.  */

void
resume (step, signal)
     int step;
     int signal;
{
  errno = 0;
  if (remote_debugging)
    remote_resume (step, signal);
  else
    {
      extern char registers[];

      /* Blam the trace bits in the stack's saved psws to match the
	 desired step mode.  This is required so that single-stepping a
	 return doesn't restore a psw with a clear trace bit and fly away,
	 and conversely, proceeding through a return in a routine that was
	 stepped into doesn't cause a phantom break by restoring a psw
	 with the trace bit set. */
      scan_stack (PSW_T_BIT, step);

      /* Write the registers back now */
      ptrace (11, inferior_pid, 0, registers, REGISTER_BYTES);
      if (errno != 0) perror_with_name ("writing registers");

      errno = 0;
      ptrace (step ? 9 : 7, inferior_pid, 1, &signal, sizeof signal);
      if (errno)
	perror_with_name ("ptrace");
    }
}

void
fetch_inferior_registers ()
{
  extern char registers[];
  ptrace (10, inferior_pid, 0, registers, REGISTER_BYTES);
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

store_inferior_registers (regno)
     int regno;
{
    /* do this only once, right before resuming inferior.  */
    return;
}

/* NOTE! I tried using PTRACE_READDATA, etc., to read and write memory
   in the NEW_SUN_PTRACE case.
   It ought to be straightforward.  But it appears that writing did
   not write the data that I specified.  I cannot understand where
   it got the data that it actually did write.  */

/* Copy LEN bytes from inferior's memory starting at MEMADDR
   to debugger memory starting at MYADDR. 
   On failure (cannot read from inferior, usually because address is out
   of bounds) returns the value of errno. */

int
read_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  errno = 0;
  ptrace (1, inferior_pid, memaddr, myaddr, len);
  if (errno) 
      bzero (myaddr, len);
  return errno;
}

/* Copy LEN bytes of data from debugger memnory at MYADDR
   to inferior's memory at MEMADDR.
   Returns errno on failure (cannot write the inferior) */

int
write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  errno = 0;
  ptrace (4, inferior_pid, memaddr, myaddr, len);
  return errno;
}

/* Work with core dump and executable files, for GDB. 
   This code would be in core.c if it weren't machine-dependent. */

/* Required by convex's SOFF format 
   It may be desirable to remove the conditionals for SOFF_FORMAT, if 
   Convex is the only machine which will ever make use of them. */
#ifdef SOFF_FORMAT
#include <convex/filehdr.h>
#include <convex/opthdr.h>
#include <convex/scnhdr.h>
#include <convex/core.h>
#endif /* SOFF_FORMAT */

/* Recognize COFF format systems because a.out.h defines AOUTHDR.  */
#ifdef AOUTHDR
#define COFF_FORMAT
#endif

#ifndef N_TXTADDR
#define N_TXTADDR(hdr) 0
#endif /* no N_TXTADDR */

#ifndef N_DATADDR
#define N_DATADDR(hdr) hdr.a_text
#endif /* no N_DATADDR */

/* Make COFF and non-COFF names for things a little more compatible
   to reduce conditionals later.  */

#ifdef COFF_FORMAT
#define a_magic magic
#endif

#ifndef COFF_FORMAT
#define AOUTHDR struct exec
#endif

extern char *sys_siglist[];


/* Hook for `exec_file_command' command to call.  */

extern void (*exec_file_display_hook) ();
   
/* File names of core file and executable file.  */

extern char *corefile;
extern char *execfile;

/* Descriptors on which core file and executable file are open.
   Note that the execchan is closed when an inferior is created
   and reopened if the inferior dies or is killed.  */

extern int corechan;
extern int execchan;

/* Last modification time of executable file.
   Also used in source.c to compare against mtime of a source file.  */

extern int exec_mtime;

/* Virtual addresses of bounds of the two areas of memory in the core file.  */

extern CORE_ADDR data_start;
extern CORE_ADDR data_end;
extern CORE_ADDR stack_start;
extern CORE_ADDR stack_end;

/* Virtual addresses of bounds of two areas of memory in the exec file.
   Note that the data area in the exec file is used only when there is no core file.  */

extern CORE_ADDR text_start;
extern CORE_ADDR text_end;

extern CORE_ADDR exec_data_start;
extern CORE_ADDR exec_data_end;

/* Address in executable file of start of text area data.  */

extern int text_offset;

/* Address in executable file of start of data area data.  */

extern int exec_data_offset;

/* Address in core file of start of data area data.  */

extern int data_offset;

/* Address in core file of start of stack area data.  */

extern int stack_offset;

#ifdef COFF_FORMAT
/* various coff data structures */

extern FILHDR file_hdr;
extern SCNHDR text_hdr;
extern SCNHDR data_hdr;

#endif /* not COFF_FORMAT */

/* a.out header saved in core file.  */
  
extern AOUTHDR core_aouthdr;

/* a.out header of exec file.  */

extern AOUTHDR exec_aouthdr;

extern void validate_files ();

core_file_command (filename, from_tty)
     char *filename;
     int from_tty;
{
  int val;
  extern char registers[];

  /* Discard all vestiges of any previous core file
     and mark data and stack spaces as empty.  */

  if (corefile)
    free (corefile);
  corefile = 0;

  if (corechan >= 0)
    close (corechan);
  corechan = -1;

  data_start = 0;
  data_end = 0;
  stack_start = STACK_END_ADDR;
  stack_end = STACK_END_ADDR;

  /* Now, if a new core file was specified, open it and digest it.  */

  if (filename)
    {
      if (have_inferior_p ())
	error ("To look at a core file, you must kill the inferior with \"kill\".");
      corechan = open (filename, O_RDONLY, 0);
      if (corechan < 0)
	perror_with_name (filename);

#ifdef SOFF_FORMAT
      {  
	extern char *sys_errlist[], *sys_siglist[];
	extern int sys_nerr;
	int stop_signal;
	struct core_hdr corehdr;
	struct user u;

	if (myread (corechan, &corehdr, sizeof corehdr) < 0)
	  perror_with_name (filename);
	if (corehdr.c_magic != CH_MAGIC) 
	  error ("%s: not a core file.", filename);

	lseek (corechan, corehdr.c_user, 0);
	if (myread (corechan, &u, sizeof u) < 0)
	  perror_with_name (filename);

	lseek (corechan, corehdr.c_syscall_context, 0);
	if (myread (corechan, registers, REGISTER_BYTES) < 0)
	  perror_with_name (filename);

	if (corehdr.c_vecst)
	  {
	    lseek (corechan, corehdr.c_vecst, 0);
	    if (myread (corechan, vecst, sizeof vecst) < 0)
	      perror_with_name (filename);
	  }

	data_offset = corehdr.c_data;
	data_start = exec_data_start;
	data_end = data_start + ctob (u.u_dsize);

	stack_offset = corehdr.c_stack;
	stack_start = stack_end - ctob (u.u_ssize);

	printf ("Program %s", u.u_comm );
	if (u.u_error >= sys_nerr)
	  printf (", last error: %d (undocumented),\n", u.u_error);
	else if (u.u_error != 0)
	  printf (", last error: %s,", sys_errlist[u.u_error]);

	stop_signal = u.u_arg[0];
	printf (" received signal %d, %s\n", stop_signal,
	  stop_signal < NSIG ? sys_siglist[stop_signal] : "(undocumented)");

	core_aouthdr.a_magic = 0;
      }
#ifdef SOFF_FORMAT

      if (filename[0] == '/')
	corefile = savestring (filename, strlen (filename));
      else
	{
	  corefile = concat (current_directory, "/", filename);
	}

      /* Convex-specific code: remove the frame stored by a
	 user-mode trap, if present */

      if (read_register (PC_REGNUM) > STACK_END_ADDR)
	{
	  POP_FRAME;
	}

      set_current_frame ( create_new_frame (read_register (FP_REGNUM),
					    read_pc ()));
      select_frame (get_current_frame (), 0);
      validate_files ();
    }
  else if (from_tty)
    printf ("No core file now.\n");
}

exec_file_command (filename, from_tty)
     char *filename;
     int from_tty;
{
  int val;

  /* Eliminate all traces of old exec file.
     Mark text segment as empty.  */

  if (execfile)
    free (execfile);
  execfile = 0;
  data_start = 0;
  data_end -= exec_data_start;
  text_start = 0;
  text_end = 0;
  exec_data_start = 0;
  exec_data_end = 0;
  if (execchan >= 0)
    close (execchan);
  execchan = -1;

  /* Now open and digest the file the user requested, if any.  */

  if (filename)
    {
      execchan = openp (getenv ("PATH"), 1, filename, O_RDONLY, 0,
			&execfile);
      if (execchan < 0)
	perror_with_name (filename);

#ifdef COFF_FORMAT
      {
	int aout_hdrsize;
	int num_sections;

	if (read_file_hdr (execchan, &file_hdr) < 0)
	  error ("\"%s\": not in executable format.", execfile);

	aout_hdrsize = file_hdr.f_opthdr;
	num_sections = file_hdr.f_nscns;

	if (read_aout_hdr (execchan, &exec_aouthdr, aout_hdrsize) < 0)
	  error ("\"%s\": can't read optional aouthdr", execfile);

	if (read_section_hdr (execchan, _TEXT, &text_hdr, num_sections) < 0)
	  error ("\"%s\": can't read text section header", execfile);

	if (read_section_hdr (execchan, _DATA, &data_hdr, num_sections) < 0)
	  error ("\"%s\": can't read data section header", execfile);

	text_start = exec_aouthdr.text_start;
	text_end = text_start + exec_aouthdr.tsize;
	text_offset = text_hdr.s_scnptr;
	exec_data_start = exec_aouthdr.data_start;
	exec_data_end = exec_data_start + exec_aouthdr.dsize;
	exec_data_offset = data_hdr.s_scnptr;
	data_start = exec_data_start;
	data_end += exec_data_start;
	exec_mtime = file_hdr.f_timdat;
      }
#else /* not COFF_FORMAT */
      {
	struct stat st_exec;

	val = myread (execchan, &exec_aouthdr, sizeof (AOUTHDR));

	if (val < 0)
	  perror_with_name (filename);

	/* Convex-specific SOFF-FORMAT */
#ifdef SOFF_FORMAT
      if (IS_SOFF_MAGIC (exec_aouthdr.a_magic))
	{
	  FILEHDR filhdr;
	  OPTHDR opthdr;
	  SCNHDR scnhdr;
	  int n, gotem;

 	  lseek (execchan, 0L, 0);
 	  if (myread (execchan, &filhdr, sizeof filhdr) < 0) 
 	    perror_with_name (filename);
 	  if (myread (execchan, &opthdr, filhdr.h_opthdr) <= 0)
 	    perror_with_name (filename);

	  lseek (execchan, (long) filhdr.h_scnptr, 0);
	  n = -1;
	  gotem = 0;
	  while (++n < filhdr.h_nscns && gotem < 2)
	    {
	      if (myread (execchan, &scnhdr, SCNHSZ) < 0)
		perror_with_name (filename);
	      if ((scnhdr.s_flags & S_TYPMASK) == S_TEXT)
		{
		  text_start = scnhdr.s_vaddr;
		  text_end = text_start + scnhdr.s_size;
		  text_offset = scnhdr.s_scnptr;
		  ++gotem;
		}
	      else if ((scnhdr.s_flags & S_TYPMASK) == S_DATA)
		{
		  exec_data_start = scnhdr.s_vaddr;
		  exec_data_end = exec_data_start + scnhdr.s_size;
		  exec_data_offset = scnhdr.s_scnptr;
		  data_start = exec_data_start;
		  data_end += exec_data_start;
		  ++gotem;
		}
	    }
	}
      else
#endif SOFF_FORMAT
	{	  
	  text_start = N_TXTADDR (exec_aouthdr);
	  text_end = text_start + exec_aouthdr.a_text;
	  text_offset = N_TXTOFF (exec_aouthdr);
	  exec_data_start = N_DATADDR (exec_aouthdr);
	  exec_data_end = exec_data_start + exec_aouthdr.a_data;
	  exec_data_offset = N_TXTOFF (exec_aouthdr) + exec_aouthdr.a_text;
	  data_start = exec_data_start;
	  data_end += exec_data_start;
	}
	fstat (execchan, &st_exec);
	exec_mtime = st_exec.st_mtime;
      }
#endif /* not COFF_FORMAT */

      validate_files ();
    }
  else if (from_tty)
    printf ("No exec file now.\n");

  /* Tell display code (if any) about the changed file name.  */
  if (exec_file_display_hook)
    (*exec_file_display_hook) (filename);
}

