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
#include "initialize.h"
#include "param.h"
#include "frame.h"
#include "inferior.h"

#include <stdio.h>
#include <sys/param.h>
#include <sys/dir.h>
#ifndef UMAX_PTRACE
#include <sys/user.h>
#endif
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#ifdef UMAX_PTRACE
#include <a.out.h>
#include <sys/ptrace.h>
#define PTRACE_ATTACH PT_ATTACH
#define PTRACE_DETACH PT_FREEPROC
#endif

#ifdef NEW_SUN_PTRACE
#include <sys/ptrace.h>
#include <machine/reg.h>
#endif

#ifdef HP9K320
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/trap.h>
#endif

#ifdef HAVE_TERMIO
#include <termio.h>
#undef TIOCGETP
#define TIOCGETP TCGETA
#undef TIOCSETN
#define TIOCSETN TCSETA
#undef TIOCSETP
#define TIOCSETP TCSETAF
#define TERMINAL struct termio
#else
#include <sgtty.h>
#define TERMINAL struct sgttyb
#endif

extern int errno;

/* Nonzero if we are debugging an attached outside process
   rather than an inferior.  */

static int attach_flag;

START_FILE

/* Record terminal status separately for debugger and inferior.  */

static TERMINAL sg_inferior;
static TERMINAL sg_ours;
static int tflags_inferior;
static int tflags_ours;

#ifdef TIOCGLTC
static struct tchars tc_inferior;
static struct tchars tc_ours;
static struct ltchars ltc_inferior;
static struct ltchars ltc_ours;
static int lmode_inferior;
static int lmode_ours;
#endif /* TIOCGLTC */

#ifdef TIOCGPGRP
static int pgrp_inferior;
static int pgrp_ours;
#else
static int (*sigint_ours) ();
static int (*sigquit_ours) ();
#endif /* TIOCGPGRP */

/* Copy of inferior_io_terminal when inferior was last started.  */
static char *inferior_thisrun_terminal;

static void terminal_ours_1 ();

/* Nonzero if our terminal settings are in effect.
   Zero if the inferior's settings are in effect.  */
static int terminal_is_ours;

/* Initialize the terminal settings we record for the inferior,
   before we actually run the inferior.  */

void
terminal_init_inferior ()
{
  if (remote_debugging)
    return;

  sg_inferior = sg_ours;
  tflags_inferior = tflags_ours;

#ifdef TIOCGLTC
  tc_inferior = tc_ours;
  ltc_inferior = ltc_ours;
  lmode_inferior = lmode_ours;
#endif /* TIOCGLTC */

#ifdef TIOCGPGRP
  pgrp_inferior = inferior_pid;
#endif /* TIOCGPGRP */

  terminal_is_ours = 1;
}

/* Put the inferior's terminal settings into effect.
   This is preparation for starting or resuming the inferior.  */

void
terminal_inferior ()
{
  if (remote_debugging)
    return;

  if (terminal_is_ours)   /*  && inferior_thisrun_terminal == 0) */
    {
      fcntl (0, F_SETFL, tflags_inferior);
      fcntl (0, F_SETFL, tflags_inferior);
      ioctl (0, TIOCSETN, &sg_inferior);

#ifdef TIOCGLTC
      ioctl (0, TIOCSETC, &tc_inferior);
      ioctl (0, TIOCSLTC, &ltc_inferior);
      ioctl (0, TIOCLSET, &lmode_inferior);
#endif /* TIOCGLTC */

#ifdef TIOCGPGRP
      ioctl (0, TIOCSPGRP, &pgrp_inferior);
#else
      sigint_ours = (signal (SIGINT, SIG_IGN));
      sigquit_ours = (signal (SIGQUIT, SIG_IGN));
#endif /* TIOCGPGRP */
    }
  terminal_is_ours = 0;
}

/* Put some of our terminal settings into effect,
   enough to get proper results from our output,
   but do not change into or out of RAW mode
   so that no input is discarded.

   After doing this, either terminal_ours or terminal_inferior
   should be called to get back to a normal state of affairs.  */

void
terminal_ours_for_output ()
{
  if (remote_debugging)
    return;

  terminal_ours_1 (1);
}

/* Put our terminal settings into effect.
   First record the inferior's terminal settings
   so they can be restored properly later.  */

void
terminal_ours ()
{
  if (remote_debugging)
    return;

  terminal_ours_1 (0);
}

static void
terminal_ours_1 (output_only)
     int output_only;
{
#ifdef TIOCGPGRP
  /* Ignore this signal since it will happen when we try to set the pgrp.  */
  int (*osigttou) ();
#endif /* TIOCGPGRP */

  if (!terminal_is_ours)  /*   && inferior_thisrun_terminal == 0)  */
    {
      terminal_is_ours = 1;

#ifdef TIOCGPGRP
      osigttou = signal (SIGTTOU, SIG_IGN);

      ioctl (0, TIOCGPGRP, &pgrp_inferior);
      ioctl (0, TIOCSPGRP, &pgrp_ours);

      signal (SIGTTOU, osigttou);
#else
      signal (SIGINT, sigint_ours);
      signal (SIGQUIT, sigquit_ours);
#endif /* TIOCGPGRP */

      tflags_inferior = fcntl (0, F_GETFL, 0);
      ioctl (0, TIOCGETP, &sg_inferior);

#ifdef TIOCGLTC
      ioctl (0, TIOCGETC, &tc_inferior);
      ioctl (0, TIOCGLTC, &ltc_inferior);
      ioctl (0, TIOCLGET, &lmode_inferior);
#endif /* TIOCGLTC */
    }

#ifdef HAVE_TERMIO
  sg_ours.c_lflag |= ICANON;
  if (output_only && !(sg_inferior.c_lflag & ICANON))
    sg_ours.c_lflag &= ~ICANON;
#else /* not HAVE_TERMIO */
  sg_ours.sg_flags &= ~RAW & ~CBREAK;
  if (output_only)
    sg_ours.sg_flags |= (RAW | CBREAK) & sg_inferior.sg_flags;
#endif /* not HAVE_TERMIO */

  fcntl (0, F_SETFL, tflags_ours);
  fcntl (0, F_SETFL, tflags_ours);
  ioctl (0, TIOCSETN, &sg_ours);

#ifdef TIOCGLTC
  ioctl (0, TIOCSETC, &tc_ours);
  ioctl (0, TIOCSLTC, &ltc_ours);
  ioctl (0, TIOCLSET, &lmode_ours);
#endif /* TIOCGLTC */


#ifdef HAVE_TERMIO
  sg_ours.c_lflag |= ICANON;
#else /* not HAVE_TERMIO */
  sg_ours.sg_flags &= ~RAW & ~CBREAK;
#endif /* not HAVE_TERMIO */
}

static void
term_status_command ()
{
  register int i;

  if (remote_debugging)
    {
      printf ("No terminal status when remote debugging.\n");
      return;
    }

  printf ("Inferior's terminal status (currently saved by GDB):\n");

#ifdef HAVE_TERMIO

  printf ("fcntl flags = 0x%x, c_iflag = 0x%x, c_oflag = 0x%x,\n",
	  tflags_inferior, sg_inferior.c_iflag, sg_inferior.c_oflag);
  printf ("c_cflag = 0x%x, c_lflag = 0x%x, c_line = 0x%x.\n",
	  sg_inferior.c_cflag, sg_inferior.c_lflag, sg_inferior.c_line);
  printf ("c_cc: ");
  for (i = 0; (i < NCC); i += 1)
    printf ("0x%x ", sg_inferior.c_cc[i]);
  printf ("\n");

#else /* not HAVE_TERMIO */

  printf ("fcntl flags = 0x%x, lmode = 0x%x,\nsgttyb.sg_flags = 0x%x, owner pid = %d.\n",
	  tflags_inferior, lmode_inferior,
	  sg_inferior.sg_flags, pgrp_inferior);
  printf ("tchars: ");
  for (i = 0; i < sizeof (struct tchars); i++)
    printf ("0x%x ", ((char *)&tc_inferior)[i]);
  printf ("\n");
  printf ("ltchars: ");
  for (i = 0; i < sizeof (struct ltchars); i++)
    printf ("0x%x ", ((char *)&ltc_inferior)[i]);

#endif /* not HAVE_TERMIO */
}

static void
new_tty (ttyname)
     char *ttyname;
{
  register int tty;
  register int fd;

#if 0
  /* I think it is better not to do this.  Then C-z on the GDB terminal
     will still stop the program, while C-z on the data terminal
     will be input.  */

  /* Disconnect the child process from our controlling terminal.  */
  tty = open("/dev/tty", O_RDWR);
  if (tty > 0)
    {
      ioctl(tty, TIOCNOTTY, 0);
      close(tty);
    }
#endif
  /* Now open the specified new terminal.  */

  tty = open(ttyname, O_RDWR);
  if (tty == -1)
    _exit(1);

  dup2(tty, 0);
  dup2(tty, 1);
  dup2(tty, 2);
  close(tty);
}

/* Start an inferior process and returns its pid.
   ALLARGS is a string containing shell command to run the program.
   ENV is the environment vector to pass.  */

#ifndef SHELL_FILE
#define SHELL_FILE "/bin/sh"
#endif

int
create_inferior (allargs, env)
     char *allargs;
     char **env;
{
  int pid;
  char *shell_command;
  extern int sys_nerr;
  extern char *sys_errlist[];
  extern int errno;

  /* If desired, concat something onto the front of ALLARGS.
     SHELL_COMMAND is the result.  */
#ifdef SHELL_COMMAND_CONCAT
  shell_command = (char *) alloca (strlen (SHELL_COMMAND_CONCAT) + strlen (allargs) + 1);
  strcpy (shell_command, SHELL_COMMAND_CONCAT);
  strcat (shell_command, allargs);
#else
  shell_command = allargs;
#endif

  /* exec is said to fail if the executable is open.  */
  close_exec_file ();

  pid = vfork ();
  if (pid < 0)
    perror_with_name ("vfork");

  if (pid == 0)
    {
      char *args[4];

#ifdef TIOCGPGRP
      /* Run inferior in a separate process group.  */
      setpgrp (getpid (), getpid ());
#endif /* TIOCGPGRP */

      inferior_thisrun_terminal = inferior_io_terminal;
      if (inferior_io_terminal != 0)
	new_tty (inferior_io_terminal);

/* Not needed on Sun, at least, and loses there
   because it clobbers the superior.  */
/*???      signal (SIGQUIT, SIG_DFL);
      signal (SIGINT, SIG_DFL);  */

      ptrace (0);

      args[0] = "sh";
      args[1] = "-c";
      args[2] = shell_command;
      args[3] = 0;

      execve (SHELL_FILE, args, env);

      fprintf (stderr, "Cannot exec %s: %s.\n", SHELL_FILE,
	       errno < sys_nerr ? sys_errlist[errno] : "unknown error");
      fflush (stderr);
      _exit (0177);
    }
  return pid;
}

/* Kill the inferior process.  Make us have no inferior.  */

static void
kill_command ()
{
  if (remote_debugging)
    return;
  if (inferior_pid == 0)
    error ("The program is not being run.");
  if (!query ("Kill the inferior process? "))
    error ("Not confirmed.");
  kill_inferior ();
}

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

kill_inferior_fast ()
{
  if (remote_debugging)
    return;
  if (inferior_pid == 0)
    return;
  ptrace (8, inferior_pid, 0, 0);
  wait (0);
}

inferior_died ()
{
  inferior_pid = 0;
  attach_flag = 0;
  mark_breakpoints_out ();
  reopen_exec_file ();
  if (have_core_file_p ())
    set_current_frame (read_register (FP_REGNUM));
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
#ifdef NO_SINGLE_STEP
      if (step)
	{
	  single_step (signal);
	}
      else ptrace (7, inferior_pid, 1, signal);
#else
      ptrace (step ? 9 : 7, inferior_pid, 1, signal);
#endif
      if (errno)
	perror_with_name ("ptrace");
    }
}

#ifdef ATTACH_DETACH

/* Start debugging the process whose number is PID.  */

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

#ifdef NEW_SUN_PTRACE

void
fetch_inferior_registers ()
{
  struct regs inferior_registers;
  struct fp_status inferior_fp_registers;
  extern char registers[];

  if (remote_debugging)
    remote_fetch_registers (registers);
  else
    {
      ptrace (PTRACE_GETREGS, inferior_pid, &inferior_registers);
      ptrace (PTRACE_GETFPREGS, inferior_pid, &inferior_fp_registers);

#if defined(sun2) || defined(sun3)
      bcopy (&inferior_registers, registers, 16 * 4);
      bcopy (&inferior_fp_registers, &registers[REGISTER_BYTE (FP0_REGNUM)],
	     sizeof inferior_fp_registers.fps_regs);
      *(int *)&registers[REGISTER_BYTE (PS_REGNUM)] = inferior_registers.r_ps;
      *(int *)&registers[REGISTER_BYTE (PC_REGNUM)] = inferior_registers.r_pc;
      bcopy (&inferior_fp_registers.fps_control,
	     &registers[REGISTER_BYTE (FPC_REGNUM)],
	     sizeof inferior_fp_registers - sizeof inferior_fp_registers.fps_regs);
#endif
#if defined(sun4)
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
#endif
    }
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

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

#if defined(sun2) || defined(sun3)
      if (in_regs)
	{
	  bcopy (registers, &inferior_registers, 16 * 4);
	  inferior_registers.r_ps = *(int *)&registers[REGISTER_BYTE (PS_REGNUM)];
	  inferior_registers.r_pc = *(int *)&registers[REGISTER_BYTE (PC_REGNUM)];
	}
      if (in_fpregs)
	{
	  bcopy (&registers[REGISTER_BYTE (FP0_REGNUM)], &inferior_fp_registers,
		 sizeof inferior_fp_registers.fps_regs);
	  bcopy (&registers[REGISTER_BYTE (FPC_REGNUM)],
		 &inferior_fp_registers.fps_control,
		 sizeof inferior_fp_registers - sizeof inferior_fp_registers.fps_regs);
	}
      if (in_regs)
	ptrace (PTRACE_SETREGS, inferior_pid, &inferior_registers);
      if (in_fpregs)
	ptrace (PTRACE_SETFPREGS, inferior_pid, &inferior_fp_registers);
#endif
#if defined(sun4)
      if (regno >= 0)
	if (FP0_REGNUM <= regno && regno <= FP0_REGNUM + 32)
	  in_regs = 0;
	else
	  in_fpregs = 0;

      if (in_regs)
	{
	  bcopy (&registers[REGISTER_BYTE (1)], &inferior_registers.r_g1, 15 * 4);
	  inferior_registers.r_ps = *(int *)&registers[REGISTER_BYTE (PS_REGNUM)];
	  inferior_registers.r_pc = *(int *)&registers[REGISTER_BYTE (PC_REGNUM)];
	  inferior_registers.r_npc = *(int *)&registers[REGISTER_BYTE (NPC_REGNUM)];
	  inferior_registers.r_y = *(int *)&registers[REGISTER_BYTE (Y_REGNUM)];
	  write_inferior_memory (*(int *)&registers[REGISTER_BYTE (SP_REGNUM)],
				 &registers[REGISTER_BYTE (16)],
				 16*4);
	}
      if (in_fpregs)
	{
	  bcopy (&registers[REGISTER_BYTE (FP0_REGNUM)], &inferior_fp_registers,
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
#endif
    }
}

#else
#ifdef HP9K320

#define FP_REGISTER_ADDR_DIFF(u, regno)					\
  (((char *) (FP_REGISTER_ADDR (u, regno))) - ((char *) &(u)))

#define INFERIOR_AR0(u)							\
  ((ptrace								\
    (PT_RUAREA, inferior_pid, ((char *) &u.u_ar0 - (char *) &u), 0))	\
   - KERNEL_U_ADDR)

static void
fetch_inferior_register (regno, regaddr)
     register int regno;
     register unsigned int regaddr;
{
#ifndef HPUX_VERSION_5
  if (regno == PS_REGNUM)
    {
      union { int i; short s[2]; } ps_val;
      int regval;

      ps_val.i = (ptrace (PT_RUAREA, inferior_pid, regaddr, 0));
      regval = ps_val.s[0];
      supply_register (regno, &regval);
    }
  else
#endif /* not HPUX_VERSION_5 */
    {
      char buf[MAX_REGISTER_RAW_SIZE];
      register int i;

      for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
	{
	  *(int *) &buf[i] = ptrace (PT_RUAREA, inferior_pid, regaddr, 0);
	  regaddr += sizeof (int);
	}
      supply_register (regno, buf);
    }
  return;
}

static void
store_inferior_register_1 (regno, regaddr, value)
     int regno;
     unsigned int regaddr;
     int value;
{
  errno = 0;
  ptrace (PT_WUAREA, inferior_pid, regaddr, value);
#if 0
  /* HP-UX randomly sets errno to non-zero for regno == 25.
     However, the value is correctly written, so ignore errno. */
  if (errno != 0)
    {
      char string_buf[64];

      sprintf (string_buf, "writing register number %d", regno);
      perror_with_name (string_buf);
    }
#endif
  return;
}

static void
store_inferior_register (regno, regaddr)
     register int regno;
     register unsigned int regaddr;
{
#ifndef HPUX_VERSION_5
  if (regno == PS_REGNUM)
    {
      union { int i; short s[2]; } ps_val;

      ps_val.i = (ptrace (PT_RUAREA, inferior_pid, regaddr, 0));
      ps_val.s[0] = (read_register (regno));
      store_inferior_register_1 (regno, regaddr, ps_val.i);
    }
  else
#endif /* not HPUX_VERSION_5 */
    {
      char buf[MAX_REGISTER_RAW_SIZE];
      register int i;
      extern char registers[];

      for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
	{
	  store_inferior_register_1
	    (regno, regaddr,
	     (*(int *) &registers[(REGISTER_BYTE (regno)) + i]));
	  regaddr += sizeof (int);
	}
    }
  return;
}

void
fetch_inferior_registers ()
{
  struct user u;
  register int regno;
  register unsigned int ar0_offset;

  ar0_offset = (INFERIOR_AR0 (u));
  for (regno = 0; (regno < FP0_REGNUM); regno++)
    fetch_inferior_register (regno, (REGISTER_ADDR (ar0_offset, regno)));
  for (; (regno < NUM_REGS); regno++)
    fetch_inferior_register (regno, (FP_REGISTER_ADDR_DIFF (u, regno)));
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

store_inferior_registers (regno)
     register int regno;
{
  struct user u;
  register unsigned int ar0_offset;

  if (regno >= FP0_REGNUM)
    {
      store_inferior_register (regno, (FP_REGISTER_ADDR_DIFF (u, regno)));
      return;
    }

  ar0_offset = (INFERIOR_AR0 (u));
  if (regno >= 0)
    {
      store_inferior_register (regno, (REGISTER_ADDR (ar0_offset, regno)));
      return;
    }

  for (regno = 0; (regno < FP0_REGNUM); regno++)
    store_inferior_register (regno, (REGISTER_ADDR (ar0_offset, regno)));
  for (; (regno < NUM_REGS); regno++)
    store_inferior_register (regno, (FP_REGISTER_ADDR_DIFF (u, regno)));
  return;
}

#else /* not HP9K320 */

void
fetch_inferior_registers ()
{
  register int regno;
  register unsigned int regaddr;
  char buf[MAX_REGISTER_RAW_SIZE];
  register int i;

#ifdef UMAX_PTRACE
  unsigned int offset = 0;
#else
  struct user u;
  unsigned int offset = (char *) &u.u_ar0 - (char *) &u;
  offset = ptrace (3, inferior_pid, offset, 0) - KERNEL_U_ADDR;
#endif

  for (regno = 0; regno < NUM_REGS; regno++)
    {
      regaddr = register_addr (regno, offset);
      for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (int))
 	{
 	  *(int *) &buf[i] = ptrace (3, inferior_pid, regaddr, 0);
 	  regaddr += sizeof (int);
 	}
      supply_register (regno, buf);
    }
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

store_inferior_registers (regno)
     int regno;
{
  register unsigned int regaddr;
  char buf[80];
  extern char registers[];
  int i;

#ifdef UMAX_PTRACE
  unsigned int offset = 0;
#else
  struct user u;
  unsigned int offset = (char *) &u.u_ar0 - (char *) &u;
  offset = ptrace (3, inferior_pid, offset, 0) - KERNEL_U_ADDR;
#endif

  if (regno >= 0)
    {
      regaddr = register_addr (regno, offset);
      for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof(int))
	{
	  errno = 0;
	  ptrace (6, inferior_pid, regaddr,
		  *(int *) &registers[REGISTER_BYTE (regno) + i]);
	  if (errno != 0)
	    {
	      sprintf (buf, "writing register number %d(%d)", regno, i);
	      perror_with_name (buf);
	    }
	  regaddr += sizeof(int);
	}
    }
  else for (regno = 0; regno < NUM_REGS; regno++)
    {
      regaddr = register_addr (regno, offset);
      for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof(int))
	{
	  errno = 0;
	  ptrace (6, inferior_pid, regaddr,
		  *(int *) &registers[REGISTER_BYTE (regno) + i]);
	  if (errno != 0)
	    {
	      sprintf (buf, "writing register number %d(%d)", regno, i);
	      perror_with_name (buf);
	    }
	  regaddr += sizeof(int);
	}
    }
}

#endif /* not HP9K320 */
#endif /* not NEW_SUN_PTRACE */

/* NOTE! I tried using PTRACE_READDATA, etc., to read and write memory
   in the NEW_SUN_PTRACE case.
   It ought to be straightforward.  But it appears that writing did
   not write the data that I specified.  I cannot understand where
   it got the data that it actually did write.  */

/* Copy LEN bytes from inferior's memory starting at MEMADDR
   to debugger memory starting at MYADDR.  */

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

  /* Read all the longwords */
  for (i = 0; i < count; i++, addr += sizeof (int))
    {
      if (remote_debugging)
	buffer[i] = remote_fetch_word (addr);
      else
	buffer[i] = ptrace (1, inferior_pid, addr, 0);
    }

  /* Copy appropriate bytes out of the buffer.  */
  bcopy ((char *) buffer + (memaddr & (sizeof (int) - 1)), myaddr, len);
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

static void
try_writing_regs_command ()
{
  register int i;
  register int value;
  extern int errno;

  if (inferior_pid == 0)
    error ("There is no inferior process now.");

  for (i = 0; ; i += 2)
    {
      QUIT;
      errno = 0;
      value = ptrace (3, inferior_pid, i, 0);
      ptrace (6, inferior_pid, i, value);
      if (errno == 0)
	{
	  printf (" Succeeded with address 0x%x; value 0x%x (%d).\n",
		  i, value, value);
	}
      else if ((i & 0377) == 0)
	printf (" Failed at 0x%x.\n", i);
    }
}

static
initialize ()
{
  add_com ("term-status", class_obscure, term_status_command,
	   "Print info on inferior's saved terminal status.");

  add_com ("try-writing-regs", class_obscure, try_writing_regs_command,
	   "Try writing all locations in inferior's system block.\n\
Report which ones can be written.");

  add_com ("kill", class_run, kill_command,
	   "Kill execution of program being debugged.");

  inferior_pid = 0;

  ioctl (0, TIOCGETP, &sg_ours);
  fcntl (0, F_GETFL, tflags_ours);

#ifdef TIOCGLTC
  ioctl (0, TIOCGETC, &tc_ours);
  ioctl (0, TIOCGLTC, &ltc_ours);
  ioctl (0, TIOCLGET, &lmode_ours);
#endif /* TIOCGLTC */

#ifdef TIOCGPGRP
  ioctl (0, TIOCGPGRP, &pgrp_ours);
#endif /* TIOCGPGRP */

  terminal_is_ours = 1;
}

END_FILE
