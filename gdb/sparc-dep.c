/* Machine-dependent code which would otherwise be in inflow.c and core.c,
   for GDB, the GNU debugger.
   Copyright (C) 1986, 1987, 1989 Free Software Foundation, Inc.
   This code is for the sparc cpu.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include "defs.h"
#include "param.h"
#include "frame.h"
#include "inferior.h"
#include "obstack.h"

#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <sys/ptrace.h>
#include <machine/reg.h>

#include <a.out.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/core.h>

extern int errno;
extern int attach_flag;

typedef enum
{
  Error, not_branch, bicc, bicca, ba, baa, ticc, ta,
} branch_type;

/* This function simply calls ptrace with the given arguments.  
   It exists so that all calls to ptrace are isolated in this 
   machine-dependent file. */
int
call_ptrace (request, pid, arg3, arg4)
     int request, pid, arg3, arg4;
{
  return ptrace (request, pid, arg3, arg4);
}

void
kill_inferior ()
{
  if (remote_debugging)
    return;
  if (inferior_pid == 0)
    return;
  ptrace (8, inferior_pid, 0, 0);
  wait (0);
  inferior_died ();
}

/* This is used when GDB is exiting.  It gives less chance of error.*/

void
kill_inferior_fast ()
{
  if (remote_debugging)
    return;
  if (inferior_pid == 0)
    return;
  ptrace (8, inferior_pid, 0, 0);
  wait (0);
}

/* Simulate single-step ptrace call for sun4.  Code written by Gary
   Beihl (beihl@mcc.com).  */

/* 
 * Duplicated from breakpoint.c because (at least for now) this is a
 * machine dependent routine.
 */
static char break_insn[] = BREAKPOINT;

/* From infrun.c */
extern int stop_after_trap, stop_after_attach;

static CORE_ADDR next_pc, npc4, target;
static int brknpc4, brktrg;
typedef char binsn_quantum[sizeof break_insn];
static binsn_quantum break_mem[3];

/* Non-zero if we just simulated a single-step ptrace call.  This is
   needed because we cannot remove the breakpoints in the inferior
   process until after the `wait' in `wait_for_inferior'.  Used for
   sun4. */

int one_stepped;

void
single_step (signal)
     int signal;
{
  branch_type br, isannulled();
  CORE_ADDR pc;

  next_pc = read_register (NPC_REGNUM);
  npc4 = next_pc + 4; /* branch not taken */

  if (!one_stepped)
    {
      /* Always set breakpoint for NPC.  */
      read_memory (next_pc, break_mem[0], sizeof break_insn);
      write_memory (next_pc, break_insn, sizeof break_insn);
      /* printf ("set break at %x\n",next_pc); */

      pc = read_register (PC_REGNUM);
      br = isannulled (pc, &target);
      brknpc4 = brktrg = 0;

      if (br == bicca)
	{
	  /* Conditional annulled branch will either end up at
	     npc (if taken) or at npc+4 (if not taken).
	     Trap npc+4.  */
	  brknpc4 = 1;
	  read_memory (npc4, break_mem[1], sizeof break_insn);
	  write_memory (npc4, break_insn, sizeof break_insn);
	}
      else if (br == baa && target != next_pc)
	{
	  /* Unconditional annulled branch will always end up at
	     the target.  */
	  brktrg = 1;
	  read_memory (target, break_mem[2], sizeof break_insn);
	  write_memory (target, break_insn, sizeof break_insn);
	}

      /* Let it go */
      ptrace (7, inferior_pid, 1, signal);
      one_stepped = 1;
      return;
    }
  else
    {
      /* Remove breakpoints */
      write_memory (next_pc, break_mem[0], sizeof break_insn);

      if (brknpc4)
	{
	  write_memory (npc4, break_mem[1], sizeof break_insn);
	}
      if (brktrg)
	{
	  write_memory (target, break_mem[2], sizeof break_insn);
	}
      one_stepped = 0;
    }
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
      /* Sparc doesn't have single step on ptrace */
      if (step)
	single_step (signal);
      else
	ptrace (7, inferior_pid, 1, signal);
      if (errno)
	perror_with_name ("ptrace");
    }
}

#ifdef ATTACH_DETACH

/* Start debugging the process whose number is PID.  */

int
attach (pid)
     int pid;
{
  errno = 0;
  ptrace (PTRACE_ATTACH, pid, 0, 0);
  if (errno)
    perror_with_name ("ptrace");
  attach_flag = 1;
  return pid;
}

/* Stop debugging the process whose number is PID
   and continue it with signal number SIGNAL.
   SIGNAL = 0 means just continue it.  */

void
detach (signal)
     int signal;
{
  errno = 0;
  ptrace (PTRACE_DETACH, inferior_pid, 1, signal);
  if (errno)
    perror_with_name ("ptrace");
  attach_flag = 0;
}
#endif /* ATTACH_DETACH */

void
fetch_inferior_registers ()
{
  struct regs inferior_registers;
  struct fp_status inferior_fp_registers;
  extern char registers[];
  int cwp;
  struct rwindow local_and_ins;

  if (remote_debugging)
    remote_fetch_registers (registers);
  else
    {
      ptrace (PTRACE_GETREGS, inferior_pid, &inferior_registers);
      ptrace (PTRACE_GETFPREGS, inferior_pid, &inferior_fp_registers);

      registers[REGISTER_BYTE (0)] = 0;
      bcopy (&inferior_registers.r_g1, &registers[REGISTER_BYTE (1)], 15 * 4);
      bcopy (&inferior_fp_registers, &registers[REGISTER_BYTE (FP0_REGNUM)],
	     sizeof inferior_fp_registers.fpu_fr);
      *(int *)&registers[REGISTER_BYTE (PS_REGNUM)] = inferior_registers.r_ps; 
      *(int *)&registers[REGISTER_BYTE (PC_REGNUM)] = inferior_registers.r_pc;
      *(int *)&registers[REGISTER_BYTE (NPC_REGNUM)] = inferior_registers.r_npc;
      *(int *)&registers[REGISTER_BYTE (Y_REGNUM)] = inferior_registers.r_y;
/*      *(int *)&registers[REGISTER_BYTE (RP_REGNUM)] =
             inferior_registers.r_o7 + 8;
      bcopy (&inferior_fp_registers.Fpu_fsr,
	     &registers[REGISTER_BYTE (FPS_REGNUM)],
	     sizeof (FPU_FSR_TYPE)); */

      read_inferior_memory (inferior_registers.r_sp,
			    &registers[REGISTER_BYTE (16)],
			    16*4);
    }
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

void
store_inferior_registers (regno)
     int regno;
{
  struct regs inferior_registers;
  struct fp_status inferior_fp_registers;
  extern char registers[];

  if (remote_debugging)
    remote_store_registers (registers);
  else
    {
      int in_regs = 1, in_fpregs = 1, in_fparegs, in_cpregs = 1;
  
      if (regno >= 0)
	if (FP0_REGNUM <= regno && regno <= FP0_REGNUM + 32)
	  in_regs = 0;
	else
	  in_fpregs = 0;

      if (in_regs)
	{
	  bcopy (&registers[REGISTER_BYTE (1)],
		 &inferior_registers.r_g1, 15 * 4);

	  inferior_registers.r_ps =
	    *(int *)&registers[REGISTER_BYTE (PS_REGNUM)];
	  inferior_registers.r_pc =
	    *(int *)&registers[REGISTER_BYTE (PC_REGNUM)];
	  inferior_registers.r_npc =
	    *(int *)&registers[REGISTER_BYTE (NPC_REGNUM)];
	  inferior_registers.r_y =
	    *(int *)&registers[REGISTER_BYTE (Y_REGNUM)];

	  write_inferior_memory (*(int *)&registers[REGISTER_BYTE (SP_REGNUM)],
				 &registers[REGISTER_BYTE (16)],
				 16*4);
	}
      if (in_fpregs)
	{
	  bcopy (&registers[REGISTER_BYTE (FP0_REGNUM)],
		 &inferior_fp_registers,
		 sizeof inferior_fp_registers.fpu_fr);

  /*      bcopy (&registers[REGISTER_BYTE (FPS_REGNUM)],
	         &inferior_fp_registers.Fpu_fsr,
		 sizeof (FPU_FSR_TYPE));
  ****/
	}

      if (in_regs)
	ptrace (PTRACE_SETREGS, inferior_pid, &inferior_registers);
      if (in_fpregs)
	ptrace (PTRACE_SETFPREGS, inferior_pid, &inferior_fp_registers);
    }
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
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr = memaddr & - sizeof (int);
  /* Round ending address up; get number of longwords that makes.  */
  register int count
    = (((memaddr + len) - addr) + sizeof (int) - 1) / sizeof (int);
  /* Allocate buffer of that many longwords.  */
  register int *buffer = (int *) alloca (count * sizeof (int));
  extern int errno;

  /* Read all the longwords */
  for (i = 0; i < count; i++, addr += sizeof (int))
    {
      errno = 0;
      if (remote_debugging)
	buffer[i] = remote_fetch_word (addr);
      else
	buffer[i] = ptrace (1, inferior_pid, addr, 0);
      if (errno)
	return errno;
    }

  /* Copy appropriate bytes out of the buffer.  */
  bcopy ((char *) buffer + (memaddr & (sizeof (int) - 1)), myaddr, len);
  return 0;
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
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr = memaddr & - sizeof (int);
  /* Round ending address up; get number of longwords that makes.  */
  register int count
    = (((memaddr + len) - addr) + sizeof (int) - 1) / sizeof (int);
  /* Allocate buffer of that many longwords.  */
  register int *buffer = (int *) alloca (count * sizeof (int));
  extern int errno;

  /* Fill start and end extra bytes of buffer with existing memory data.  */

  if (remote_debugging)
    buffer[0] = remote_fetch_word (addr);
  else
    buffer[0] = ptrace (1, inferior_pid, addr, 0);

  if (count > 1)
    {
      if (remote_debugging)
	buffer[count - 1]
	  = remote_fetch_word (addr + (count - 1) * sizeof (int));
      else
	buffer[count - 1]
	  = ptrace (1, inferior_pid,
		    addr + (count - 1) * sizeof (int), 0);
    }

  /* Copy data to be written over corresponding part of buffer */

  bcopy (myaddr, (char *) buffer + (memaddr & (sizeof (int) - 1)), len);

  /* Write the entire buffer.  */

  for (i = 0; i < count; i++, addr += sizeof (int))
    {
      errno = 0;
      if (remote_debugging)
	remote_store_word (addr, buffer[i]);
      else
	ptrace (4, inferior_pid, addr, buffer[i]);
      if (errno)
	return errno;
    }

  return 0;
}


/* Machine-dependent code which would otherwise be in core.c */
/* Work with core dump and executable files, for GDB. */

#ifndef N_TXTADDR
#define N_TXTADDR(hdr) 0
#endif /* no N_TXTADDR */

#ifndef N_DATADDR
#define N_DATADDR(hdr) hdr.a_text
#endif /* no N_DATADDR */

/* Non-zero if this is an object (.o) file, rather than an executable.
   Distinguishing between the two is rarely necessary (and seems like
   a hack, but there is no other way to get the text and data
   addresses--N_TXTADDR should probably take care of
   this, but it doesn't).  */
/* This definition will not work
   if someone decides to make ld preserve relocation info.  */
#define IS_OBJECT_FILE(hdr) (hdr.a_trsize != 0)

/* Make COFF and non-COFF names for things a little more compatible
   to reduce conditionals later.  */

#ifdef COFF_FORMAT
#define a_magic magic
#endif

#ifndef COFF_FORMAT
#ifndef AOUTHDR
#define AOUTHDR struct exec
#endif
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

void
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
      filename = tilde_expand (filename);
      make_cleanup (free, filename);
      
      if (have_inferior_p ())
	error ("To look at a core file, you must kill the inferior with \"kill\".");
      corechan = open (filename, O_RDONLY, 0);
      if (corechan < 0)
	perror_with_name (filename);

      {
	struct core corestr;

	val = myread (corechan, &corestr, sizeof corestr);
	if (val < 0)
	  perror_with_name (filename);
	if (corestr.c_magic != CORE_MAGIC)
	  error ("\"%s\" does not appear to be a core dump file (magic 0x%x, expected 0x%x)",
		 filename, corestr.c_magic, (int) CORE_MAGIC);
	else if (sizeof (struct core) != corestr.c_len)
	  error ("\"%s\" has an invalid struct core length (%d, expected %d)",
		 filename, corestr.c_len, (int) sizeof (struct core));

	data_start = exec_data_start;
	data_end = data_start + corestr.c_dsize;
	stack_start = stack_end - corestr.c_ssize;
	data_offset = sizeof corestr;
	stack_offset = sizeof corestr + corestr.c_dsize;

	/* G0 *always* holds 0.  */
	*(int *)&registers[REGISTER_BYTE (0)] = 0;
	/* The globals and output registers.  */

	bcopy (&corestr.c_regs.r_g1, ((int *) registers) + 1, 15 * 4);
	*(int *)&registers[REGISTER_BYTE (PS_REGNUM)] = corestr.c_regs.r_ps;
	*(int *)&registers[REGISTER_BYTE (PC_REGNUM)] = corestr.c_regs.r_pc;
	*(int *)&registers[REGISTER_BYTE (NPC_REGNUM)] = corestr.c_regs.r_npc;
	*(int *)&registers[REGISTER_BYTE (Y_REGNUM)] = corestr.c_regs.r_y;

	/* My best guess at where to get the locals and input
	   registers is exactly where they usually are, right above
	   the stack pointer.  If the core dump was caused by a bus
	   writing off the stack pointer (as is possible) then this
	   won't work, but it's worth the try. */
	{
	  int sp;

	  sp = *(int *)&registers[REGISTER_BYTE (SP_REGNUM)];
	  lseek (corechan, sp - stack_start + stack_offset, L_SET);
	  if (16 * 4 != myread (corechan,
				&registers[REGISTER_BYTE (16)],
				16 * 4))
	    /* fprintf so user can still use gdb */
	    fprintf (stderr, "Couldn't read input and local registers from core file\n");
	}

	bcopy (corestr.c_fpu.fpu_regs,
	       &registers[REGISTER_BYTE (FP0_REGNUM)],
	       sizeof corestr.c_fpu.fpu_regs);
#ifdef FPU
	bcopy (&corestr.c_fpu.fpu_fsr,
	       &registers[REGISTER_BYTE (FPS_REGNUM)],
	       sizeof (FPU_FSR_TYPE));
#endif

	bcopy (&corestr.c_aouthdr, &core_aouthdr, sizeof (struct exec));

	printf ("Core file is from \"%s\".\n", corestr.c_cmdname);
	if (corestr.c_signo > 0)
	  printf ("Program terminated with signal %d, %s.\n",
			corestr.c_signo,
			corestr.c_signo < NSIG
			? sys_siglist[corestr.c_signo]
			: "(undocumented)");
      }
      if (filename[0] == '/')
	corefile = savestring (filename, strlen (filename));
      else
	{
	  corefile = concat (current_directory, "/", filename);
	}

      set_current_frame ( create_new_frame (read_register (FP_REGNUM),
					    read_pc ()));
      select_frame (get_current_frame (), 0);
      validate_files ();
    }
  else if (from_tty)
    printf ("No core file now.\n");
}

void
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
      filename = tilde_expand (filename);
      make_cleanup (free, filename);
      
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

	if (read_section_hdr (execchan, _TEXT, &text_hdr, num_sections,
			      aout_hdrsize) < 0)
	  error ("\"%s\": can't read text section header", execfile);

	if (read_section_hdr (execchan, _DATA, &data_hdr, num_sections,
			      aout_hdrsize) < 0)
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

	text_start =
	  IS_OBJECT_FILE (exec_aouthdr) ? 0 : N_TXTADDR (exec_aouthdr);
        exec_data_start = IS_OBJECT_FILE (exec_aouthdr)
	  ? exec_aouthdr.a_text : N_DATADDR (exec_aouthdr);
	text_offset = N_TXTOFF (exec_aouthdr);
	exec_data_offset = N_TXTOFF (exec_aouthdr) + exec_aouthdr.a_text;

	text_end = text_start + exec_aouthdr.a_text;
        exec_data_end = exec_data_start + exec_aouthdr.a_data;
	data_start = exec_data_start;
	data_end += exec_data_start;

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

/*
 * Find the pc saved in frame FRAME.  
 */
CORE_ADDR
frame_saved_pc (frame)
     FRAME frame;
{
  CORE_ADDR prev_pc;

  /* If it's at the bottom, the return value's stored in i7/rp */
  if (get_current_frame () == frame)
    prev_pc = GET_RWINDOW_REG (read_register (SP_REGNUM), rw_in[7]);
  else
    /* Wouldn't this always work?  This would allow this routine to
       be completely a macro.  */
    prev_pc = GET_RWINDOW_REG (frame->bottom, rw_in[7]);
  
  return PC_ADJUST (prev_pc);
}

/*
 * Since an individual frame in the frame cache is defined by two
 * arguments (a frame pointer and a stack pointer), we need two
 * arguments to get info for an arbitrary stack frame.  This routine
 * takes two arguments and makes the cached frames look as if these
 * two arguments defined a frame on the cache.  This allows the rest
 * of info frame to extract the important arguments without
 * difficulty. 
 */
FRAME
setup_arbitrary_frame (frame, stack)
     FRAME_ADDR frame, stack;
{
  struct frame_info *fci;
  FRAME fid = create_new_frame (frame, 0);

  if (!fid)
    fatal ("internal: create_new_frame returned invalid frame id");
  
  fid->bottom = stack;

  return fid;
}

/* This code was written by Gary Beihl (beihl@mcc.com).
   It was modified by Michael Tiemann (tiemann@corto.inria.fr).  */

struct command_line *get_breakpoint_commands ();

/*
 * This routine appears to be passed a size by which to increase the
 * stack.  It then executes a save instruction in the inferior to
 * increase the stack by this amount.  Only the register window system
 * should be affected by this; the program counter & etc. will not be.
 *
 * This instructions used for this purpose are:
 *
 * 	sethi %hi(0x0),g1                    *
 * 	add g1,0x1ee0,g1                     *
 * 	save sp,g1,sp                        
 * 	sethi %hi(0x0),g1                    *
 * 	add g1,0x1ee0,g1                     *
 * 	t g0,0x1,o0
 * 	sethi %hi(0x0),g0                    (nop)
 *
 *  I presume that these set g1 to be the negative of the size, do a
 * save (putting the stack pointer at sp - size) and restore the
 * original contents of g1.  A * indicates that the actual value of
 * the instruction is modified below.
 */
static int save_insn_opcodes[] = {
  0x03000000, 0x82007ee0, 0x9de38001, 0x03000000,
  0x82007ee0, 0x91d02001, 0x01000000 };

/* Neither do_save_insn or do_restore_insn save stack configuration
   (since the stack is in an indeterminate state through the call to
   each of them); that responsibility of the routine which calls them.  */

void
do_save_insn (size)
     int size;
{
  int g1 = read_register (1);
  CORE_ADDR sp = read_register (SP_REGNUM);
  CORE_ADDR pc = read_register (PC_REGNUM);
  CORE_ADDR npc = read_register (NPC_REGNUM);
  CORE_ADDR fake_pc = sp - sizeof (save_insn_opcodes);
  struct inferior_status inf_status;

  save_inferior_status (&inf_status, 0); /* Don't restore stack info */
  /*
   * See above.
   */
  save_insn_opcodes[0] = 0x03000000 | ((-size >> 10) & 0x3fffff);
  save_insn_opcodes[1] = 0x82006000 | (-size & 0x3ff);
  save_insn_opcodes[3] = 0x03000000 | ((g1 >> 10) & 0x3fffff);
  save_insn_opcodes[4] = 0x82006000 | (g1 & 0x3ff);
  write_memory (fake_pc, save_insn_opcodes, sizeof (save_insn_opcodes));

  clear_proceed_status ();
  stop_after_trap = 1;
  proceed (fake_pc, 0, 0);

  write_register (PC_REGNUM, pc);
  write_register (NPC_REGNUM, npc);
  restore_inferior_status (&inf_status);
}

/*
 * This routine takes a program counter value.  It restores the
 * register window system to the frame above the current one, and sets
 * the pc and npc to the correct values.
 */

/*    The following insns translate to:
 
 	restore
 	t g0,0x1,o0
 	sethi %hi(0x0), g0	*/

static int restore_insn_opcodes[] = { 0x81e80000, 0x91d02001, 0x01000000 };

void
do_restore_insn (pc)
     CORE_ADDR pc;
{
  CORE_ADDR sp = read_register (SP_REGNUM);
  CORE_ADDR npc = pc + 4;
  CORE_ADDR fake_pc = sp - sizeof (restore_insn_opcodes);
  struct inferior_status inf_status;

  save_inferior_status (&inf_status, 0); /* Don't restore stack info */

  if (!pc)
    abort();

  write_memory (fake_pc, restore_insn_opcodes, sizeof (restore_insn_opcodes));

  clear_proceed_status ();
  stop_after_trap = 1;
  proceed (fake_pc, 0, 0);

  write_register (PC_REGNUM, pc);
  write_register (NPC_REGNUM, npc);
  restore_inferior_status (&inf_status);
}

/*
 * This routine should be more specific in it's actions; making sure
 * that it uses the same register in the initial prologue section.
 */
CORE_ADDR 
skip_prologue (pc)
     CORE_ADDR pc;
{
  union
    {
      unsigned long int code;
      struct
	{
	  unsigned int op:2;
	  unsigned int rd:5;
	  unsigned int op2:3;
	  unsigned int imm22:22;
	} sethi;
      struct
	{
	  unsigned int op:2;
	  unsigned int rd:5;
	  unsigned int op3:6;
	  unsigned int rs1:5;
	  unsigned int i:1;
	  unsigned int simm13:13;
	} add;
      int i;
    } x;
  int dest = -1;

  x.i = read_memory_integer (pc, 4);

  /* Recognize the `sethi' insn and record its destination.  */
  if (x.sethi.op == 0 && x.sethi.op2 == 4)
    {
      dest = x.sethi.rd;
      pc += 4;
      x.i = read_memory_integer (pc, 4);
    }

  /* Recognize an add immediate value to register to either %g1 or
     the destination register recorded above.  Actually, this might
     well recognize several different arithmetic operations.  */
  if (x.add.op == 2 && x.add.i && (x.add.rd == 1 || x.add.rd == dest))
    {
      pc += 4;
      x.i = read_memory_integer (pc, 4);
    }

  /* This recognizes any SAVE insn.  But why do the XOR and then
     the compare?  That's identical to comparing against 60 (as long
     as there isn't any sign extension).  */
  if (x.add.op == 2 && (x.add.op3 ^ 32) == 28)
    {
      pc += 4;
      x.i = read_memory_integer (pc, 4);
    }

  /* Now we need to recognize stores into the frame from the input
     registers.  This recognizes all non alternate stores of input
     register, into a location offset from the frame pointer.  */
  while (x.add.op == 3
	 && (x.add.op3 & 0x3c) == 4 /* Store, non-alternate.  */
	 && (x.add.rd & 0x18) == 0x18 /* Input register.  */
	 && x.add.i		/* Immediate mode.  */
	 && x.add.rs1 == 30	/* Off of frame pointer.  */
	 /* Into reserved stack space.  */
	 && x.add.simm13 >= 0x44
	 && x.add.simm13 < 0x5b)
    {
      pc += 4;
      x.i = read_memory_integer (pc, 4);
    }
  return pc;
}

/* Check instruction at ADDR to see if it is an annulled branch.
   All other instructions will go to NPC or will trap.
   Set *TARGET if we find a canidate branch; set to zero if not. */
   
branch_type
isannulled (addr,  target)
     CORE_ADDR addr, *target;
{
  branch_type val = not_branch;
  long int offset;		/* Must be signed for sign-extend.  */
  union
    {
      unsigned long int code;
      struct
	{
	  unsigned int op:2;
	  unsigned int a:1;
	  unsigned int cond:4;
	  unsigned int op2:3;
	  unsigned int disp22:22;
	} b;
    } insn;

  *target = 0;
  insn.code = read_memory_integer (addr, 4);

  if (insn.b.op == 0
      && (insn.b.op2 == 2 || insn.b.op2 == 6 || insn.b.op2 == 7))
    {
      if (insn.b.cond == 8)
	val = insn.b.a ? baa : ba;
      else
	val = insn.b.a ? bicca : bicc;
      offset = 4 * ((int) (insn.b.disp22 << 10) >> 10);
      *target = addr + offset;
    }

  return val;
}

